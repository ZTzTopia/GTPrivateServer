//#include <functional>
//#include <uv.h>
//#include <cassert>
//#include <iostream>
//
//#include "cluster.h"
//
//namespace cluster {
//    void close_process_handle(uv_process_t *req, int64_t exit_status, int term_signal) {
//        uv_close((uv_handle_t *)req, nullptr);
//    }
//
//    void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
//        buf->base = static_cast<char *>(malloc(suggested_size));
//        buf->len = suggested_size;
//    }
//
//    void on_new_connection_primary(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
//        if (get_cluster(g_loop)->is_worker()) {
//            return;
//        }
//
//        if (nread < 0) {
//            if (nread != UV_EOF) {
//                // Error
//            }
//
//            uv_close((uv_handle_t *)stream, nullptr);
//            return;
//        }
//
//        get_cluster(g_loop)->emit("data", std::string(buf->base, nread));
//    }
//
//    void on_new_connection_worker(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
//        if (get_cluster(g_loop)->is_primary()) {
//            return;
//        }
//
//        std::cout << "on_new_connection_worker" << nread << std::endl;
//        if (nread < 0) {
//            if (nread != UV_EOF) {
//                // Error
//            }
//
//            uv_close((uv_handle_t *)stream, nullptr);
//            return;
//        }
//        std::cout << "on_new_connection_worker" << nread << std::endl;
//
//        get_cluster(g_loop)->emit("data", std::string(buf->base, nread));
//    }
//
//    uv_pipe_t parent_pipe{};
//    Cluster::Cluster(uv_loop_t *loop)
//        : m_loop(loop)
//    {
//        if (is_worker()) {
//            uv_pipe_init(g_loop, &parent_pipe, 1);
//            uv_pipe_open(&parent_pipe, 0);
//            uv_read_start((uv_stream_t *)&parent_pipe, alloc_buffer, on_new_connection_worker);
//
//            uv_buf_t buf = uv_buf_init((char*)"hello", 5);
//            assert(!uv_write((uv_write_t*)malloc(sizeof(uv_write_t)), (uv_stream_t *)&parent_pipe, &buf, 1, nullptr));
//        }
//    }
//
//    bool Cluster::is_primary() const {
//        size_t buffer_size = 5;
//        char buffer[5];
//
//        if (uv_os_getenv("CHILD", buffer, &buffer_size) != UV_ENOENT) {
//            return false;
//        }
//
//        return true;
//    }
//
//    bool Cluster::is_worker() const {
//        return !is_primary();
//    }
//
//    int Cluster::fork(char *program_name) {
//        if (is_worker()) {
//            return 0;
//        }
//
//        char *args[3];
//        args[0] = program_name;
//        args[1] = const_cast<char *>("CHILD");
//        args[2] = nullptr;
//
//        char *env[2];
//        env[0] = const_cast<char *>("CHILD=TRUE");
//        env[1] = nullptr;
//
//        uv_cpu_info_t *info;
//    int cpu_count;
//    uv_cpu_info(&info, &cpu_count);
//    uv_free_cpu_info(info, cpu_count);
//cpu_count=1;
//    workers = (ChildWorker*)calloc(cpu_count, sizeof(struct ChildWorker));
//    while (cpu_count--) {
//        struct ChildWorker *worker = &workers[cpu_count];
//        uv_pipe_init(m_loop, &worker->pipe, 1);
//
//        uv_stdio_container_t child_stdio[3];
//        child_stdio[0].flags = static_cast<uv_stdio_flags>(UV_CREATE_PIPE | UV_READABLE_PIPE | UV_WRITABLE_PIPE);
//        child_stdio[0].data.stream = (uv_stream_t*) &worker->pipe;
//        child_stdio[1].flags = UV_INHERIT_FD;
//        child_stdio[1].data.fd = 1;
//        child_stdio[2].flags = UV_INHERIT_FD;
//        child_stdio[2].data.fd = 2;
//
//        worker->options.stdio = child_stdio;
//        worker->options.stdio_count = 3;
//
//        worker->options.exit_cb = close_process_handle;
//        worker->options.file = args[0];
//        worker->options.args = args;
//        worker->options.env = env;
//
//        uv_spawn(m_loop, &worker->req, &worker->options);
//        uv_read_start((uv_stream_t *)&worker->pipe, alloc_buffer, on_new_connection_primary);
//
//    return worker->req.pid;
//    }
//    }
//}