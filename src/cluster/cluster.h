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

        [[nodiscard]] bool is_primary() const;
        [[nodiscard]] bool is_worker() const;

        // Fork new worker.
        int fork(char *program_name);

        // Send message to parent.
        bool send(const std::string &message);

    private:
        std::shared_ptr<uvw::Loop> m_loop;
        std::vector<Worker *> m_workers;
        Worker *m_worker; // This for worker.
    };
}