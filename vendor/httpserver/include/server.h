#pragma once
#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <asio/io_context.hpp>
// #include <asio/co_spawn.hpp>
// #include <asio/detached.hpp>
// #include <asio/io_context.hpp>
// #include <asio/ip/tcp.hpp>
// #include <asio/signal_set.hpp>
// #include <asio/write.hpp>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <memory>
#include <set>

#include <cstdlib>
#include <fstream>
#include <algorithm>
// #include <sys/types.h>
// #include <sys/stat.h>

// #ifndef _MSC_VER
// #include <sys/fcntl.h>
// #include <unistd.h>
// #endif

// #ifndef _WIN32
// #include <sys/wait.h>
// #endif

// #ifdef WIN32
// #define stat _stat
// #endif

#include <array>
#include <iostream>
#include <ctime>
#include <map>
#include <map>
#include <thread>
#include <mutex>
#include <filesystem>
#include <future>
#include <functional>
#include <stdexcept>

#include "zlib.h"
#include "terminal_color.h"
#include "http_socket.h"
#include "client_session.h"
#include "http2_parse.h"
#include "threadpool.h"
#include "httppeer.h"
#include "http2_flow.h"
// #include "http2_define.h"
// #include "http_domain.h"
// namespace this_coro = asio::this_coro;
namespace fs = std::filesystem;

namespace http
{

class httpserver
{
  public:
    httpserver() {}
    asio::awaitable<void> clientpeerfun(std::shared_ptr<client_session>, bool isssl);
    asio::awaitable<void>
    sslhandshake(std::shared_ptr<client_session>);

    void http2pool(int threadid);
    asio::awaitable<void> http2loop(std::shared_ptr<httppeer>);
    asio::awaitable<void> http2task(std::shared_ptr<http2parse>);
    asio::awaitable<void>
    http2_send_content(std::shared_ptr<httppeer> peer, const unsigned char *buffer, unsigned int begin_end);
    bool http2_send_body(std::shared_ptr<httppeer> peer, const unsigned char *buffer, unsigned int begin_end);
    asio::awaitable<void> http2_co_send_file(std::shared_ptr<httppeer> peer);
    bool http2_send_file(std::shared_ptr<httppeer>);
    asio::awaitable<void> http2_send_file_range(std::shared_ptr<httppeer> peer);

    void http2send_filedata(struct http2sendblock_t &http2_ff_send);
    void add_error_lists(const std::string &);
    asio::awaitable<void> http1loop(unsigned int sig, std::shared_ptr<httppeer>, std::shared_ptr<client_session>);
    void websocket_loop(int myid);
    
    void listeners();
    void listener();

    asio::awaitable<void> http1_send_status_content(std::shared_ptr<httppeer> peer, unsigned int status_code, const std::string &bodycontent);
    asio::awaitable<bool> http1_static_file_authority(std::shared_ptr<httppeer> peer);

    asio::awaitable<void> http2_send_status_content(std::shared_ptr<httppeer> peer, unsigned int status_code, const std::string &bodycontent);
    asio::awaitable<bool> http2_static_file_authority(std::shared_ptr<httppeer> peer);

    asio::awaitable<size_t> co_user_task(std::shared_ptr<httppeer> peer, asio::use_awaitable_t<> h = {});
    asio::awaitable<size_t> co_user_fastcgi_task(std::shared_ptr<httppeer> peer, asio::use_awaitable_t<> h = {});
    asio::awaitable<size_t> co_http2_wait_window_update(std::shared_ptr<httppeer> peer, asio::use_awaitable_t<> h = {});
    //asio::awaitable<size_t> co_http_link_clear_wait(std::list<asio::detail::awaitable_handler<asio::any_io_executor, size_t>> &, asio::use_awaitable_t<> h = {});
    void add_runsocketthread();
    int checkhttp2(std::shared_ptr<client_session> peer_session);
    asio::awaitable<void> http1_send_bad_request(unsigned int, std::shared_ptr<client_session>);
    asio::awaitable<void> http1_send_bad_server(std::shared_ptr<httppeer>, std::shared_ptr<client_session>);
    /*bool http1_send_body(unsigned int streamid, std::shared_ptr<httppeer> peer, std::shared_ptr<client_session>
     * peer_session, const unsigned char *buffer, unsigned int begin_end);*/
    // bool http1_send_file(unsigned int streamid,
    //                      std::shared_ptr<httppeer> peer,
    //                      std::shared_ptr<client_session> peer_session,
    //                      const std::string &filename);
    asio::awaitable<void> http1_send_file(unsigned int streamid,
                                          std::shared_ptr<httppeer> peer,
                                          std::shared_ptr<client_session> peer_session,
                                          const std::string &filename);

    asio::awaitable<void> http1_send_file_range(unsigned int streamid,
                               std::shared_ptr<httppeer> peer,
                               std::shared_ptr<client_session> peer_session,
                               const std::string &filename);
    void run(const std::string &);

    void add_nullptrlog(const std::string &logstrb);
    void httpwatch();
    // void http1_linkwatch();
    // void http2_linkwatch();
    void save_traffic_arrays();
    void stop();
    ~httpserver()
    {
        std::printf("~httpserver\n");
        isstop = true;
        io_context.stop();

        for (unsigned int i = 0; i < runthreads.size(); ++i)
        {
            if (runthreads[i].joinable())
            {
                runthreads[i].join();
            }
        }
    }

  public:
    // httpheader begin
    unsigned char runhands_num = 4;
    asio::io_context io_context{1};
    std::vector<std::thread> runthreads;
    std::vector<std::thread> websocketthreads;
    std::list<std::weak_ptr<httppeer>> websockettasks;
    std::list<std::pair<std::size_t, std::shared_ptr<httppeer>>> clientlooptasks;
    
    // std::list<std::weak_ptr<httppeer>> httpclient1_wach_links;
    // std::list<std::weak_ptr<http2parse>> httpclient2_wach_links;
    std::string traffic_arrays;
    std::queue<httpsocket_t> tasks;

    bool isstop =false;
    bool istraffic =false;
    std::atomic_uint total_count = 0;

    std::atomic_uint total_http2_count = 0;
    std::atomic_uint total_http1_count = 0;
    // httpheader end
    // log
    //std::set<std::shared_ptr<client_session>> client_sessions;
    std::mutex socket_session_lists_mutex;
    std::list<std::weak_ptr<client_session>> socket_session_lists;
    std::list<std::shared_ptr<client_session>> socket_session_wait_clear;
    // std::list<asio::detail::awaitable_handler<asio::any_io_executor, size_t>> socket_session_handler_call;
    
    std::mutex wait_clear_mutex;
    std::list<std::string> access_loglist;
    std::list<std::string> error_loglist;
    std::mutex log_mutex;

    ThreadPool clientrunpool{16};

    std::mutex http2_task_mutex;
    std::condition_variable http2condition;
    std::list<struct http2sendblock_t> http2send_tasks;

    std::mutex websocket_task_mutex;
    std::condition_variable websocketcondition;

    const unsigned char magicstr[24] = {0x50, 0x52, 0x49, 0x20, 0x2A, 0x20, 0x48, 0x54, 0x54, 0x50, 0x2F, 0x32, 0x2E, 0x30, 0x0D, 0x0A, 0x0D, 0x0A, 0x53, 0x4D, 0x0D, 0x0A, 0x0D, 0x0A};
};
httpserver &get_server_app();
void add_server_timetask(std::shared_ptr<httppeer>);
}// namespace http
#endif
