/*
 * Inspired by the Node.JS cluster.
 */

#pragma once
#include <string>
#include <thread>
#include <uvw/process.h>

#include "eventemitter.h"
#include "worker.h"

namespace cluster {
    class Cluster : public EventEmitter {
    public:
        explicit Cluster(std::shared_ptr<uvw::Loop> &loop);
        ~Cluster();

        void setup_primary(bool silent);

        bool is_primary();
        bool is_worker();

        // Fork new worker.
        Worker *fork(char *program_name);

        // Send message to parent.
        bool send(const std::string &message);

    private:
        std::shared_ptr<uvw::Loop> m_loop;
        std::vector<Worker *> m_workers;
        Worker *m_worker; // This for worker.

        bool m_silent;
    };
}