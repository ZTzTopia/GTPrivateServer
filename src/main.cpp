#include <iostream>
#include <csignal>
#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <uvw/async.h>
#include <uvw/loop.h>
#include <uvw/signal.h>
#include <uvw/thread.h>

#include "config.h"
#include "cluster/cluster.h"
#include "database/database.h"
#include "enetwrapper/enetserver.h"
#include "http/http.h"
#include "include/backward-cpp/backward.hpp"
#include "items/itemsdb.h"
#include "server/server.h"

static std::atomic<bool> running{ true };
static std::function<void()> shutdown_callback;

/*int main(int argc, char *argv[]) {
    uv_loop_t *loop = uv_default_loop();

    if (cluster::get_cluster(loop)->is_primary()) {
        std::cout << "I am the primary." << std::endl;
        std::cout << "Worker ProcessID: " << cluster::get_cluster(loop)->fork(argv[0]) << std::endl;

        cluster::get_cluster(loop)->on("data", [](std::string &data) {
            std::cout << "Received data from worker: " << data<< std::endl;
        });
    }
    else if (cluster::get_cluster(loop)->is_worker()) {
        uv_sleep(1000);
        std::cout << "I am a worker." << std::endl;
    }

    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);
    return 0;
}*/

int main(int argc, char *argv[]) {
#ifndef _WIN32
    std::cout << "Other platforms not tested." << std::endl;
    return EXIT_FAILURE;
#endif

    auto loop = uvw::Loop::getDefault();

    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>("server.log", 1024 * 1024, 8));
    auto logger = std::make_shared<spdlog::logger>("server", sinks.begin(), sinks.end());
    logger->set_pattern("[%Y-%m-%dT%TZ] [%n] [%^%l%$] [thread %t] %v");
    logger->set_level(config::debug ? spdlog::level::debug : spdlog::level::info);
    logger->flush_on(spdlog::level::debug);
    spdlog::set_default_logger(logger);

    spdlog::info("Growtopia private server starting..");

    auto signal_handle = loop->resource<uvw::SignalHandle>();

    /*signal_handle->on<uvw::SignalEvent>([](int signum) {
        spdlog::info("Received signal {}. Exiting.", signum);

        if (signum != SIGINT) {
            backward::TraceResolver tr;
            backward::StackTrace st;
            st.load_here(32);
            tr.load_stacktrace(st);
            for (size_t i = 0; i < st.size(); ++i) {
                backward::ResolvedTrace trace = tr.resolve(st[i]);
                spdlog::error("#{} {} {} [{}]", i, trace.object_filename, trace.object_function, trace.addr);
            }
        }
    });*/

    signal_handle->start(SIGINT);

    // TODO: Make this more good.
    if (!database::get_database()->connect()) {
        spdlog::error("Failed to connect to database.");

        delete database::get_database();
        return EXIT_FAILURE;
    }

    auto *http = new http::HTTP{ *loop };
    http->listen("0.0.0.0", 80);

    if (!items::get_items_db()->initialize()) {
        spdlog::error("Failed to load items database.");

        delete database::get_database();
        delete items::get_items_db();
        return EXIT_FAILURE;
    }

    if (!enetwrapper::ENetServer::one_time_init()) {
        spdlog::error("Failed to initialize ENet.");

        delete database::get_database();
        delete items::get_items_db();
        return EXIT_FAILURE;
    }

    auto *server = new server::Server{};
    if (!server->initialize(config::server::start_port, config::server::max_peer)) {
        spdlog::error("Failed to create server.");

        delete server;
        delete database::get_database();
        delete items::get_items_db();
        return EXIT_FAILURE;
    }

    auto on_update_stopped = std::make_shared<bool>();
    auto on_update = [](const std::shared_ptr<void> &data) {
        while (!*std::static_pointer_cast<bool>(data)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    };

    auto on_update_thread = loop->resource<uvw::Thread>(on_update, on_update_stopped);
    on_update_thread->run();

    auto exit_thread = [](const std::shared_ptr<void> &data) {
        std::string command;
        while (std::cin >> command) {
            if (command == "exit" || command == "quit" || command == "stop") {
                shutdown_callback();
                break;
            }
        }
    };

    auto exit_thread_thread = loop->resource<uvw::Thread>(exit_thread);
    exit_thread_thread->run();

    auto async_handle = loop->resource<uvw::AsyncHandle>();
    async_handle->on<uvw::AsyncEvent>([loop](const auto &, auto &handle) {
        handle.close();
        loop->stop();
    });

    shutdown_callback = [http, signal_handle, on_update_thread, exit_thread_thread, async_handle] {
        signal_handle->close();

        delete http;
        delete database::get_database();

        on_update_thread->join();
        exit_thread_thread->join();

        async_handle->send();
    };

    loop->run();
    loop->close();
    return 1;
}

