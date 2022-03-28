/*
 * Inspired by the Node.JS cluster.
 */

#pragma once
#include <uvw/process.h>
#include <uvw/pipe.h>

#include "eventemitter.h"

namespace cluster {
    class Cluster;

    class Worker : public EventEmitter {
    public:
        Worker(std::shared_ptr<uvw::Loop> &loop, int id);
        ~Worker();

        void create_process(const std::string &program_path, bool silent = true);

        bool send(const std::string &message);
        bool send_internal(const std::string &message);

        void process(std::shared_ptr<uvw::ProcessHandle> &process) { m_process = process; }
        [[nodiscard]] std::shared_ptr<uvw::ProcessHandle> process() const { return m_process; }

        [[nodiscard]] std::shared_ptr<uvw::PipeHandle> stdin_pipe() const { return m_pipe[0]; }
        [[nodiscard]] std::shared_ptr<uvw::PipeHandle> stdout_pipe() const { return m_pipe[1]; }
        [[nodiscard]] std::shared_ptr<uvw::PipeHandle> stderr_pipe() const { return m_pipe[2]; }
        [[nodiscard]] std::shared_ptr<uvw::PipeHandle> ipc_pipe() const { return m_pipe[3]; }

        [[nodiscard]] uint32_t id() const { return m_id; }

        [[nodiscard]] bool is_connected() const { return m_is_connected; }
        [[nodiscard]] bool is_dead() const { return m_is_dead; }

    private:
        std::shared_ptr<uvw::Loop> m_loop;

        std::shared_ptr<uvw::ProcessHandle> m_process; // The worker process handle.
        std::shared_ptr<uvw::PipeHandle> m_pipe[4]; // Worker pipe: stdin, stdout, stderr, ipc

        int m_id; // unique id for this worker.

        bool m_is_connected; // is the worker connected to the master?
        bool m_is_dead; // is the worker dead/exited?
    };
}