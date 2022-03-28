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
    class Child : public EventEmitter {
    public:
        explicit Child(std::shared_ptr<uvw::Loop> &loop);
        ~Child() = default;

        void disconnect();
        void destroy(int signum = SIGTERM);

        [[nodiscard]] Worker *worker() const { return m_worker; }

    private:
        Worker *m_worker;
    };
}