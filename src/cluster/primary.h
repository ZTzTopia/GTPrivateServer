/*
 * Inspired by the Node.JS cluster.
 */

#pragma once
#include <string>
#include <thread>
#include <uvw/loop.h>

#include "eventemitter.h"
#include "worker.h"

namespace cluster {
    class Primary : public EventEmitter {
    public:
        explicit Primary(std::shared_ptr<uvw::Loop> &loop, bool silent = true);
        ~Primary() = default;

        Worker *fork(const std::string &program_path);

        bool broadcast(const std::string &message);

        void disconnect();
        void destroy(int signum = SIGTERM);

        [[nodiscard]] std::unordered_map<uint8_t, Worker *> workers() const { return m_workers; }

    private:
        std::shared_ptr<uvw::Loop> m_loop;

        // Workers.
        std::unordered_map<uint8_t, Worker *> m_workers;

        // Options.
        bool m_silent;
    };
}