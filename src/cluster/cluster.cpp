#include <functional>
#include <cassert>
#include <iostream>
#include <uvw/pipe.h>

#include "cluster.h"

namespace cluster {
#ifdef _MSC_VER
    const std::string sockname{ R"(\\.\pipe\cluster.sock)" };
#else
    const std::string sockname = std::string{ "/temp/cluster.sock" };
#endif

    Cluster::Cluster(std::shared_ptr<uvw::Loop> loop)
        : m_loop(loop)
    {
        if (is_worker()) {
            auto pipe_handle = m_loop->resource<uvw::PipeHandle>();
            pipe_handle->init();
            // pipe_handle->open(0);

            pipe_handle->on<uvw::ErrorEvent>([](const auto &, auto &) {
                std::cout << "pipe error" << std::endl;
            });

            pipe_handle->once<uvw::WriteEvent>([](const uvw::WriteEvent &, uvw::PipeHandle &handle) {
                handle.close();
            });

            pipe_handle->once<uvw::ConnectEvent>([](const uvw::ConnectEvent &, uvw::PipeHandle &handle) {
                std::string msg{ "online" };
                handle.write((char *)msg.c_str(), msg.length());
            });
            pipe_handle->connect(sockname);
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

        auto process_handle = m_loop->resource<uvw::ProcessHandle>();
        process_handle->on<uvw::ExitEvent>([this, process_handle](const uvw::ExitEvent &exit_event, uvw::ProcessHandle &handle) {
            emit("exit", process_handle, exit_event.status, exit_event.signal);
        });

        auto pipe_handle = m_loop->resource<uvw::PipeHandle>();
        pipe_handle->init();
        pipe_handle->on<uvw::ErrorEvent>([](const auto &, auto &) {
            std::cout << "pipe error" << std::endl;
        });

        // This not working :(
        // process_handle->stdio(*pipe_handle, uvw::Flags<uvw::ProcessHandle::StdIO>::from<uvw::ProcessHandle::StdIO::CREATE_PIPE, uvw::ProcessHandle::StdIO::READABLE_PIPE, uvw::ProcessHandle::StdIO::WRITABLE_PIPE>());

        process_handle->stdio(uvw::StdIN, uvw::ProcessHandle::StdIO::IGNORE_STREAM);
        process_handle->stdio(uvw::StdOUT, uvw::ProcessHandle::StdIO::IGNORE_STREAM);
        process_handle->stdio(uvw::StdOUT, uvw::ProcessHandle::StdIO::INHERIT_FD);
        process_handle->stdio(uvw::StdERR, uvw::ProcessHandle::StdIO::IGNORE_STREAM);
        process_handle->stdio(uvw::StdERR, uvw::ProcessHandle::StdIO::INHERIT_FD);

        char *args[3];
        args[0] = program_name;
        args[1] = const_cast<char *>("CHILD");
        args[2] = nullptr;

        char *env[2];
        env[0] = const_cast<char *>("CHILD=TRUE");
        env[1] = nullptr;

        process_handle->spawn(args[0], args, env);

        pipe_handle->once<uvw::ListenEvent>([this, process_handle](const uvw::ListenEvent &, uvw::PipeHandle &handle) {
            std::shared_ptr<uvw::PipeHandle> socket = handle.loop().resource<uvw::PipeHandle>();

            socket->on<uvw::EndEvent>([this, process_handle](const uvw::EndEvent &, uvw::PipeHandle &sock) {
                emit("disconnect", process_handle);
                sock.close();
            });

            socket->on<uvw::DataEvent>([this, process_handle](const uvw::DataEvent &data_event, uvw::PipeHandle &socket) {
                std::string data(data_event.data.get(), data_event.length);
                if (data == "online") {
                    emit("online", process_handle);
                    socket.close();
                    return;
                }

                emit("message", process_handle, data);
                socket.close();
            });

            handle.accept(*socket);
            socket->read();
        });
        pipe_handle->bind(sockname);
        pipe_handle->listen();

        emit("fork", process_handle);

        m_workers.emplace_back(process_handle);
        return process_handle->pid();
    }
}