/*
 * Inspired by the Node.JS cluster.
 */

#pragma once
#include <uvw/process.h>
#include <uvw/pipe.h>
#include <cassert>

namespace cluster {
    struct Worker {
        std::shared_ptr<uvw::ProcessHandle> process; // The worker process handle.
        std::shared_ptr<uvw::PipeHandle> pipe[4]; // Worker pipe: stdin, stdout, stderr, ipc

        uint32_t id; // unique id for this worker.

        bool is_connected; // is the worker connected to the master?
        bool is_dead; // is the worker dead/exited?

        // Send message to child if from primary, otherwise send to primary.
        bool send(const std::string &message) {
            if (!pipe[3]->writable() || !pipe[3]->readable()) {
                return false;
            }

            if (!is_connected || is_dead) {
                return false;
            }

            pipe[3]->write(const_cast<char *>(message.c_str()), message.size());
            return true;
        }
    };
}