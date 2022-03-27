/*
 * Inspired by the Node.JS cluster.
 */

#include <functional>
#include <uvw/pipe.h>

#include "cluster.h"
#include "../utils/random.h"

namespace cluster {
    Cluster::Cluster(std::shared_ptr<uvw::Loop> &loop)
        : m_loop(loop)
    {
        if (is_worker()) {
            m_worker = new Worker{};
            m_worker->pipe = m_loop->resource<uvw::PipeHandle>(true);
            m_worker->pipe->on<uvw::ErrorEvent>([this](const uvw::ErrorEvent &error_event, auto &) {
                printf("[%d] Worker pipe error\n", m_worker->id);
                m_worker->is_dead = true;
                emit("error", std::string{ error_event.what() });
            });
            m_worker->pipe->on<uvw::CloseEvent>([this](const uvw::CloseEvent &, uvw::PipeHandle &socket) {
                m_worker->is_connected = false;
                emit("disconnect");
                socket.close();
            });
            m_worker->pipe->on<uvw::EndEvent>([this](const uvw::EndEvent &, uvw::PipeHandle &socket) {
                m_worker->is_connected = false;
                emit("disconnect");
                socket.close();
            });
            m_worker->pipe->on<uvw::DataEvent>([this](const uvw::DataEvent &data_event, uvw::PipeHandle &socket) {
                std::string data(data_event.data.get(), data_event.length);
                emit("message", data);
            });
            m_worker->pipe->init();
            m_worker->pipe->open(0);
            m_worker->pipe->read();
            m_worker->is_connected = true;
            m_worker->is_dead = false;
            m_worker->send("online"); // Send the online message to the parent.
        }
    }

    Cluster::~Cluster() {
        if (is_primary()) {
            for (auto &worker : m_workers) {
                worker->process->close();
                delete worker;
            }

            m_workers.clear();
        }
        else {
            m_worker->process->close();
            delete m_worker;
        }
    }

    bool Cluster::is_primary() const {
        size_t buffer_size = 5;
        char buffer[5];

        if (uv_os_getenv("CHILD", buffer, &buffer_size) != UV_ENOENT) {
            return false;
        }

        return true;
    }

    bool Cluster::is_worker() const {
        return !is_primary();
    }

    int Cluster::fork(char *program_name) {
        if (is_worker()) {
            return 0;
        }

        auto *worker = new Worker{};
        auto process_handle = m_loop->resource<uvw::ProcessHandle>();
        process_handle->on<uvw::ErrorEvent>([this, worker](const uvw::ErrorEvent &error_event, auto &) {
            worker->is_dead = true;
            emit("error", worker, std::string{ error_event.what() });
        });
        process_handle->on<uvw::ExitEvent>([this, worker](const uvw::ExitEvent &exit_event, uvw::ProcessHandle &handle) {
            worker->is_dead = true;
            emit("exit", worker, exit_event.status, exit_event.signal);
        });

        auto worker_pipe = m_loop->resource<uvw::PipeHandle>(true);
        worker_pipe->on<uvw::ErrorEvent>([this, worker](const uvw::ErrorEvent &error_event, uvw::PipeHandle &pipe) {
            worker->is_dead = true;
            emit("error", worker, std::string{ error_event.what() });
            pipe.close();
        });
        worker_pipe->on<uvw::CloseEvent>([this, worker](const uvw::CloseEvent &, uvw::PipeHandle &pipe) {
            m_worker->is_connected = false;
            emit("disconnect", worker);
            pipe.close();
        });
        worker_pipe->on<uvw::EndEvent>([this, worker](const uvw::EndEvent &, uvw::PipeHandle &pipe) {
            worker->is_connected = false;
            emit("disconnect", worker);
            pipe.close();
        });
        worker_pipe->on<uvw::DataEvent>([this, worker, worker_pipe](const uvw::DataEvent &data_event, uvw::PipeHandle &pipe) {
            std::string data(data_event.data.get(), data_event.length);
            if (data == "online") {
                worker->is_connected = true;
                emit("online", worker);
                return;
            }

            emit("message", worker, data);
        });
        worker_pipe->init();

        // The error because uvw dev insert the poStreamStdio to the end of poStdio. Why not use the same variable?
        process_handle->disableStdIOInheritance();
        process_handle->stdio(*worker_pipe, uvw::Flags<uvw::ProcessHandle::StdIO>::from<uvw::ProcessHandle::StdIO::CREATE_PIPE, uvw::ProcessHandle::StdIO::READABLE_PIPE, uvw::ProcessHandle::StdIO::WRITABLE_PIPE>());

        char *args[3];
        args[0] = program_name;
        args[1] = const_cast<char *>("CHILD");
        args[2] = nullptr;

        char *env[2];
        env[0] = const_cast<char *>("CHILD=TRUE");
        env[1] = nullptr;

        process_handle->spawn(args[0], args, env);
        worker_pipe->read();
        emit("fork", process_handle);

        worker->process = process_handle;
        worker->pipe = worker_pipe;
        worker->id = utils::random::get_generator_static().uniform(static_cast<uint32_t>(std::numeric_limits<int32_t>::max()), std::numeric_limits<uint32_t>::max());
        worker->is_connected = false;
        worker->is_dead = false;
        m_workers.emplace_back(worker);

        return process_handle->pid();
    }

    bool Cluster::send(const std::string &message) {
        if (is_primary()) {
            return false;
        }

        return m_worker->send(message);
    }
}