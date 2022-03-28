/*
 * Inspired by the Node.JS cluster.
 */

#include <functional>

#include "child.h"

namespace cluster {
    Child::Child(std::shared_ptr<uvw::Loop> &loop) {
        int id = -1;
        if (!uvw::Utilities::OS::env("CHILD_UNIQUE_ID").empty()) {
            id = std::stoi(uvw::Utilities::OS::env("CHILD_UNIQUE_ID"));
        }

        m_worker = new Worker{ loop, id };

        m_worker->on("message", [this](const std::string &message) {
            emit("message", message);
        });

        m_worker->ipc_pipe()->open(3);
        m_worker->ipc_pipe()->read();

        m_worker->send_internal("online");
    }
    
    void Child::disconnect() {
        if (m_worker->is_connected()) {
            m_worker->ipc_pipe()->close();
        }
    }

    void Child::destroy(int signum) {
        if (m_worker->is_connected()) {
            m_worker->once("disconnect", [this, signum]() {
                m_worker->process()->kill(signum);
            });

            m_worker->ipc_pipe()->close();
            return;
        }

        m_worker->process()->kill(signum);
    }
}