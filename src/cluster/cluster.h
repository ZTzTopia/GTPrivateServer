/*
 * Inspired by the Node.JS cluster.
 */

#pragma once

namespace cluster {
    class Cluster {
    public:
        Cluster() = default;
        ~Cluster() = default;

        static bool is_primary();
        static bool is_worker();
    };
}