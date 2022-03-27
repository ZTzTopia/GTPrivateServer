/*
 * Inspired by the Node.JS cluster.
 */

#pragma once
#include <uvw/process.h>
#include <uvw/pipe.h>
#include <cassert>

namespace cluster {
    struct Worker {
        std::shared_ptr<uvw::ProcessHandle> process;
        std::shared_ptr<uvw::PipeHandle> pipe;
        uint32_t id;
        bool is_connected;
        bool is_dead;

        bool send(const std::string &message) {
            assert(pipe->writable());
            assert(pipe->readable());

            /*if (!is_connected || is_dead) {
                return false;
            }*/

            pipe->write(const_cast<char *>(message.c_str()), message.size());
            return true;
        }
    };
}