/*
 * Inspired by the Node.JS cluster.
 */

#include <uvw/util.h>

#include "cluster.h"

namespace cluster {
    bool Cluster::is_primary() {
        if (!uvw::Utilities::OS::env("CHILD").empty()) {
            return false;
        }

        return true;
    }

    bool Cluster::is_worker() {
        return is_primary() ? false : true; // !is_primary() make CLion grey out all code after call is_worker().
    }
}