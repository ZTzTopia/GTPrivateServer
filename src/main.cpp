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

int main(int argc, char *argv[]) {
#if (!defined(_WIN32) || defined(__MINGW32__)) && !defined(__linux__)
    std::cout << "Other platforms not tested." << std::endl;
    return EXIT_FAILURE;
#endif

    auto loop = uvw::Loop::getDefault();

    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>("server.log", 1024 * 1024, 8)); // Using the port??
    auto logger = std::make_shared<spdlog::logger>("server", sinks.begin(), sinks.end());
    logger->set_pattern("[%Y-%m-%dT%TZ] [%n] [%^%l%$] [thread %t] %v");
    logger->set_level(config::debug ? spdlog::level::debug : spdlog::level::info);
    logger->flush_on(spdlog::level::debug);
    spdlog::set_default_logger(logger);

    if (!config::dev) {
        cluster::Cluster cluster{loop};
        if (cluster.is_primary()) {
            static uint8_t process_count = 0;
            static const uint8_t cpu_count = uvw::Utilities::cpuInfo().size();

            cluster.fork(argv[0]);

            cluster.on("error", [](const std::string &error) {
                spdlog::error("Primary error: {}", error);
            });

            cluster.on("online", [&cluster, argv](const std::shared_ptr<uvw::ProcessHandle> &process_handle) {
                if (process_count >= cpu_count) {
                    return;
                }

                process_count++;
                cluster.fork(argv[0]);
            });

            cluster.on("exit", [&cluster, argv](const std::shared_ptr<uvw::ProcessHandle> &process_handle, int64_t exit_status, int term_signal) {
                cluster.fork(argv[0]);
            });

            loop->run();
            return EXIT_SUCCESS;
        }

        cluster.on("error", [](const std::string &error) {
            spdlog::error("Worker error: {}", error);
        });
    }

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

    // TODO: Make this more good.
    if (!database::get_database()->connect()) {
        spdlog::error("Failed to connect to database.");

        delete database::get_database();
        return EXIT_FAILURE;
    }

    auto *http = new http::HTTP{ *loop };
    http->listen("0.0.0.0", 80);

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
