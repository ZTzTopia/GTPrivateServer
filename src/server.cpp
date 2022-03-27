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
#include <uvw/util.h>

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

int start_server(std::shared_ptr<uvw::Loop> loop) {
    spdlog::info("Growtopia private server starting..");

    auto signal_handle = loop->resource<uvw::SignalHandle>();

    signal_handle->on<uvw::SignalEvent>([](const uvw::SignalEvent &signal_event, uvw::SignalHandle &signal) {
        spdlog::info("Received signal {}. Exiting.", signal_event.signum);

        if (signal_event.signum != SIGINT) {
            backward::TraceResolver tr;
            backward::StackTrace st;
            st.load_here(32);
            tr.load_stacktrace(st);
            for (size_t i = 0; i < st.size(); ++i) {
                backward::ResolvedTrace trace = tr.resolve(st[i]);
                spdlog::error("#{} {} {} [{}]", i, trace.object_filename, trace.object_function, trace.addr);
            }
        }

        signal.stop();
        signal.close();
    });

    signal_handle->start(SIGINT);
    signal_handle->start(SIGHUP);

    auto *http = new http::HTTP{ *loop };
    http->listen("0.0.0.0", 80);

    // TODO: Make this more good.
    if (!database::get_database()->connect()) {
        spdlog::error("Failed to connect to database.");

        delete database::get_database();
        return EXIT_FAILURE;
    }

    if (!items::get_items_db()->serialize()) {
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

    shutdown_callback = [http, signal_handle, server, on_update_thread, exit_thread_thread, async_handle] {
        delete http;

        signal_handle->close();

        delete server;
        delete database::get_database();

        on_update_thread->join();
        exit_thread_thread->join();

        async_handle->send();
    };

    loop->run();
    loop->close();
    return EXIT_SUCCESS;
}