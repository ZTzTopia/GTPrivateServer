#pragma once
#include <string>
#include <thread>
#include <uvw/process.h>

#include "eventemitter.h"

namespace cluster {
    class Cluster : public EventEmitter {
    public:
        explicit Cluster(std::shared_ptr<uvw::Loop> loop);
        ~Cluster() = default;

        [[nodiscard]] bool is_primary() const;
        [[nodiscard]] bool is_worker() const;

        // Fork new worker.
        int fork(char *program_name);

    private:
        std::shared_ptr<uvw::Loop> m_loop;
        std::vector<std::shared_ptr<uvw::ProcessHandle>> m_workers;
    };
}