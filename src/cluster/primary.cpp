/*
 * Inspired by the Node.JS cluster.
 */

#include <spdlog/fmt/fmt.h>

#include "primary.h"

namespace cluster {
    Primary::Primary(std::shared_ptr<uvw::Loop> &loop, bool silent)
        : m_loop(loop)
        , m_silent(silent) {}

    Worker *Primary::fork(const std::string &program_path) {
        static int id = 0;
        auto worker = new Worker{ m_loop, ++id };

        worker->on("message", [this, worker](const std::string &message) {
            emit("message", worker, message);
        });

        worker->on("message_internal", [this, worker](const std::string &message) {
            if (message == "online") {
                emit("online", worker);
            }
        });

        worker->once("exit", [this, worker](const uvw::ExitEvent &exit_event) {
            emit("exit", worker, exit_event.status, exit_event.signal);
        });

        worker->once("disconnect", [this, worker]() {
            emit("disconnect", worker);
        });

        worker->create_process(program_path, m_silent);
        worker->ipc_pipe()->read();

        m_workers[worker->id()] = worker;
        return worker;
    }

    bool Primary::broadcast(const std::string &message) {
        bool sucessful = true;
        for (auto &worker : m_workers) {
            if (!worker.second->send(message)) {
                sucessful = false;
            }
        }

        return sucessful;
    }

    void Primary::disconnect() {
        for (auto &worker : m_workers) {
            if (worker.second->is_connected()) {
                worker.second->ipc_pipe()->close();
            }
        }
    }

    void Primary::destroy(int signum) {
        for (auto &worker : m_workers) {
            if (worker.second->is_connected()) {
                worker.second->once("disconnect", [worker, signum]() {
                    worker.second->process()->kill(signum);
                });

                worker.second->ipc_pipe()->close();
                continue;
            }

            worker.second->process()->kill(signum);
        }
    }
}