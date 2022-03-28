/*
 * Inspired by the Node.JS cluster.
 */

#include <functional>

#include "cluster.h"
#include "../utils/random.h"

namespace cluster {
    Cluster::Cluster(std::shared_ptr<uvw::Loop> &loop)
        : m_loop(loop)
        , m_worker(nullptr)
        , m_silent(true)
    {
        if (is_worker()) {
            m_worker = new Worker{};
            m_worker->pipe[3] = m_loop->resource<uvw::PipeHandle>(true);

            m_worker->pipe[3]->on<uvw::ErrorEvent>([this](const uvw::ErrorEvent &error_event, uvw::PipeHandle &pipe) {
                m_worker->is_dead = true;
                emit("error", std::string{ error_event.what() });
                pipe.close();
            });

            m_worker->pipe[3]->on<uvw::CloseEvent>([this](const uvw::CloseEvent &, uvw::PipeHandle &pipe) {
                m_worker->is_connected = false;
                emit("disconnect");
                pipe.close();
            });

            m_worker->pipe[3]->on<uvw::EndEvent>([this](const uvw::EndEvent &, uvw::PipeHandle &pipe) {
                m_worker->is_connected = false;
                emit("disconnect");
                pipe.stop();
            });

            m_worker->pipe[3]->on<uvw::DataEvent>([this](const uvw::DataEvent &data_event, uvw::PipeHandle &socket) {
                std::string data(data_event.data.get(), data_event.length);
                emit("message", data);
            });

            m_worker->pipe[3]->init();
            m_worker->pipe[3]->open(3);
            m_worker->pipe[3]->read();

            m_worker->is_connected = true;
            m_worker->is_dead = false;

            // Send the online message to the primary.
            m_worker->send("online");
        }
    }

    Cluster::~Cluster() {
        if (is_primary()) {
            for (auto &worker : m_workers) {
                for (auto &pipe : worker->pipe) {
                    if (pipe) {
                        pipe->close();
                    }
                }

                if (worker->process) {
                    worker->process->close();
                }

                delete worker;
            }

            m_workers.clear();
        }
        else {
            for (auto &pipe : m_worker->pipe) {
                if (pipe) {
                    pipe->close();
                }
            }

            if (m_worker->process) {
                m_worker->process->close();
            }

            delete m_worker;
        }
    }

    void Cluster::setup_primary(bool silent) {
        if (is_worker()) {
            return;
        }

        m_silent = silent;
    }

    bool Cluster::is_primary() {
        size_t buffer_size = 5;
        char buffer[5];

        if (uv_os_getenv("CHILD", buffer, &buffer_size) != UV_ENOENT) {
            return false;
        }

        return true;
    }

    bool Cluster::is_worker() {
        return is_primary() ? false : true; // !is_primary() make CLion grey out all code after call is_worker().
    }

    Worker *Cluster::fork(char *program_name) {
        if (is_worker()) {
            return nullptr;
        }

        auto *worker = new Worker{};
        auto process = m_loop->resource<uvw::ProcessHandle>();

        process->on<uvw::ErrorEvent>([this, worker](const uvw::ErrorEvent &error_event, auto &) {
            worker->is_dead = true;
            emit("error", worker, std::string{ error_event.what() });
        });

        process->on<uvw::ExitEvent>([this, worker](const uvw::ExitEvent &exit_event, uvw::ProcessHandle &process) {
            worker->is_dead = true;
            emit("exit", worker, exit_event.status, exit_event.signal);
            process.close();
        });

        auto ipc = m_loop->resource<uvw::PipeHandle>(true);

        ipc->on<uvw::ErrorEvent>([this, worker](const uvw::ErrorEvent &error_event, uvw::PipeHandle &pipe) {
            worker->is_dead = true;
            emit("error", worker, std::string{ error_event.what() });
            pipe.close();
        });

        ipc->on<uvw::CloseEvent>([this, worker](const uvw::CloseEvent &, uvw::PipeHandle &pipe) {
            m_worker->is_connected = false;
            emit("disconnect", worker);
            pipe.close();
        });

        ipc->on<uvw::EndEvent>([this, worker](const uvw::EndEvent &, uvw::PipeHandle &pipe) {
            worker->is_connected = false;
            emit("disconnect", worker);
            pipe.stop();
        });

        ipc->on<uvw::DataEvent>([this, worker](const uvw::DataEvent &data_event, uvw::PipeHandle &pipe) {
            std::string data(data_event.data.get(), data_event.length);
            if (data == "online") {
                worker->is_connected = true;
                emit("online", worker);
                return;
            }

            emit("message", worker, data);
        });

        ipc->init();

        process->disableStdIOInheritance();
        process->stdio(*ipc, uvw::Flags<uvw::ProcessHandle::StdIO>::from<uvw::ProcessHandle::StdIO::CREATE_PIPE, uvw::ProcessHandle::StdIO::READABLE_PIPE, uvw::ProcessHandle::StdIO::WRITABLE_PIPE>());

        uvw::Flags<uvw::details::UVStdIOFlags> inherit = m_silent ? uvw::ProcessHandle::StdIO::IGNORE_STREAM : uvw::ProcessHandle::StdIO::INHERIT_FD;
        process->stdio(uvw::StdIN, uvw::ProcessHandle::StdIO::IGNORE_STREAM);
        process->stdio(uvw::StdOUT, inherit);
        process->stdio(uvw::StdERR, inherit);

        char *args[3];
        args[0] = program_name;
        args[1] = const_cast<char *>("CHILD");
        args[2] = nullptr;

        char *env[2];
        env[0] = const_cast<char *>("CHILD=TRUE");
        env[1] = nullptr;

        process->spawn(args[0], args, env);
        emit("fork", process);

        ipc->read();

        worker->process = process;
        worker->pipe[3] = ipc;
        worker->id = utils::random::get_generator_static().uniform(static_cast<uint32_t>(std::numeric_limits<int32_t>::max()), std::numeric_limits<uint32_t>::max());
        worker->is_connected = false;
        worker->is_dead = false;
        m_workers.emplace_back(worker);

        return worker;
    }

    bool Cluster::send(const std::string &message) {
        if (is_primary()) {
            return false;
        }

        return m_worker->send(message);
    }
}