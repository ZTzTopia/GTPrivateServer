/*
 * Inspired by the Node.JS cluster.
 */

#include <fmt/format.h>

#include "worker.h"

namespace cluster {
    Worker::Worker(const std::shared_ptr<uvw::Loop> &loop, int id)
        : m_loop(loop)
        , m_id(id)
        , m_is_connected(true)
        , m_is_dead(false)
    {
        m_pipe[3] = loop->resource<uvw::PipeHandle>(true);
        m_pipe[3]->once<uvw::ErrorEvent>([this](const uvw::ErrorEvent &error_event, uvw::PipeHandle &pipe) {
            emit("error", this, std::string{ error_event.what() });
            m_is_dead = true;
            pipe.close();
        });
        
        m_pipe[3]->once<uvw::CloseEvent>([this](const uvw::CloseEvent &, uvw::PipeHandle &pipe) {
            emit("disconnect");
            m_is_connected = false;
            pipe.close();
        });

        m_pipe[3]->once<uvw::EndEvent>([this](const uvw::EndEvent &, uvw::PipeHandle &pipe) {
            emit("disconnect");
            m_is_connected = false;
            pipe.stop();
        });

        m_pipe[3]->on<uvw::DataEvent>([this](const uvw::DataEvent &data_event, uvw::PipeHandle &socket) {
            static std::size_t verify_hash = std::hash<std::string>{}(__DATE__ __TIME__);

            std::size_t verify_hash_send;
            std::memcpy(&verify_hash_send, data_event.data.get(), sizeof(std::size_t));

            if (verify_hash_send == verify_hash) {
                uint16_t len;
                std::memcpy(&len, data_event.data.get() + sizeof(std::size_t), 2);
                std::string message = std::string(reinterpret_cast<char *>(data_event.data.get() + sizeof(std::size_t) + 2), len);
                emit("message_internal", message);
                return;
            }

            std::string data(data_event.data.get(), data_event.length);
            emit("message", data);
        });

        m_pipe[3]->init();
    }

    Worker::~Worker() {
        for (auto &pipe : m_pipe) {
            if (pipe) {
                pipe->close();
            }
        }

        if (m_process) {
            m_process->close();
        }
    }

    void Worker::create_process(const std::string &program_path, bool silent) {
        m_process = m_loop->resource<uvw::ProcessHandle>();

        m_process->once<uvw::ExitEvent>([this](const uvw::ExitEvent &error_event, uvw::ProcessHandle &handle) {
            emit("exit", error_event.status, error_event.signal);
            handle.close();
        });

        m_process->disableStdIOInheritance();
        m_process->stdio(*m_pipe[3], uvw::Flags<uvw::ProcessHandle::StdIO>::from<uvw::ProcessHandle::StdIO::CREATE_PIPE, uvw::ProcessHandle::StdIO::READABLE_PIPE, uvw::ProcessHandle::StdIO::WRITABLE_PIPE>());
        uvw::Flags<uvw::details::UVStdIOFlags> inherit = silent ? uvw::ProcessHandle::StdIO::IGNORE_STREAM : uvw::ProcessHandle::StdIO::INHERIT_FD;
        m_process->stdio(uvw::StdIN, uvw::ProcessHandle::StdIO::IGNORE_STREAM);
        m_process->stdio(uvw::StdOUT, inherit);
        m_process->stdio(uvw::StdERR, inherit);

        char *args[3];
        args[0] = const_cast<char *>(program_path.c_str());
        args[1] = const_cast<char *>("CHILD");
        args[2] = nullptr;

        char *env[3];
        env[0] = const_cast<char *>("CHILD=TRUE");
        std::string unique_id{ fmt::format("CHILD_UNIQUE_ID={}", m_id) };
        env[1] = const_cast<char *>(unique_id.c_str());
        env[2] = nullptr;

        m_process->spawn(args[0], args, env);
    }

    bool Worker::send(const std::string &message) {
        if (!m_pipe[3]->writable() || !m_pipe[3]->readable()) {
            return false;
        }

        if (!m_is_connected || m_is_dead) {
            return false;
        }

        m_pipe[3]->write(const_cast<char *>(message.c_str()), static_cast<unsigned int>(message.size()));
        return true;
    }

    bool Worker::send_internal(const std::string &message) {
        if (!m_pipe[3]->writable() || !m_pipe[3]->readable()) {
            return false;
        }

        if (!m_is_connected || m_is_dead) {
            return false;
        }

        auto mem_need = static_cast<uint8_t>(sizeof(std::size_t) + 2 + message.length() + 1);
        char *data = new char[mem_need];
        data[mem_need] = '\0';

        // We use this to verify whether the message is internal, so we don't use a fixed string.
        static std::size_t verify_hash = std::hash<std::string>{}(__DATE__ __TIME__);
        std::memcpy(data, &verify_hash, sizeof(std::size_t));

        auto len = static_cast<uint16_t>(message.length());
        std::memcpy(data + sizeof(std::size_t), &len, 2);
        std::memcpy(data + sizeof(std::size_t) + 2, message.c_str(), message.length());

        m_pipe[3]->write(data, mem_need - 1);
        return true;
    }
}