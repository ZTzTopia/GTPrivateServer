#pragma once
#include <string>
#include <thread>
#include <uv.h>

#include "eventemitter.h"

namespace cluster {
    class Cluster : public EventEmitter {
    public:
        explicit Cluster(uv_loop_t *loop);
        ~Cluster() = default;

        [[nodiscard]] bool is_primary() const;
        [[nodiscard]] bool is_worker() const;

        // Fork new worker.
        int fork(char *program_name);

    private:
        uv_loop_t *m_loop;

        struct ChildWorker {
            uv_process_t req;
            uv_process_options_t options;
            uv_pipe_t pipe;
        } *workers;

        std::vector<ChildWorker *> m_workers;
    };

    static uv_loop_t *g_loop{ nullptr };
    inline Cluster *get_cluster(uv_loop_t *loop) {
        g_loop = loop;
        static auto *database = new Cluster{ loop };
        return database;
    }
}