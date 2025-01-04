#ifndef __HTTP_CLIENT_SESSION_H
#define __HTTP_CLIENT_SESSION_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif// defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <asio/io_context.hpp>

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
#include <string_view>
#include <atomic>

#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <sys/stat.h>

#ifndef _MSC_VER
#include <sys/types.h>
#include <sys/fcntl.h>
#include <unistd.h>
#endif

#ifndef WIN32
#include <sys/wait.h>
#endif

#ifdef WIN32
#define stat _stat
#endif

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

#include <zlib.h>
#include "sendqueue.h"
#include "http_header.h"
#include "http_socket.h"
#include "http2_frame.h"
#include "cookie.h"
#include "clientdatacache.h"

namespace http
{

class client_session : public std::enable_shared_from_this<client_session>
{
  public:
    client_session();
    ~client_session();
    bool send_data(const std::string &msg);
    bool send_data(const unsigned char *, unsigned int);

    bool isopensocket();
    std::shared_ptr<client_session> get_ptr();
    std::string getremoteip();
    unsigned int getremoteport();

    std::string getlocalip();
    unsigned int getlocalport();

    asio::awaitable<void> http2_send_enddata(unsigned int s_stream_id);
    asio::awaitable<void> co_send_zero_data(unsigned int stream_id);
    asio::awaitable<void> co_send_setting();

    bool send_switch101();

    void send_ping();
    asio::awaitable<void> http2_send_ping();

    void send_recv_setting();
    void send_window_update(unsigned int, unsigned int streamid = 0);
    void recv_window_update(unsigned int, unsigned int streamid = 0);
    void stop();
    asio::awaitable<void> co_send_goway();

    asio::awaitable<void> co_send_writer(const std::string &msg);
    asio::awaitable<void> co_send_writer(const unsigned char *, unsigned int);
    asio::awaitable<bool> read_some(unsigned int &readnum, std::string &log_item);

    asio::awaitable<void> http2_send_writer(std::string_view msg);
    asio::awaitable<void> http2_send_data_loop_co();

    asio::awaitable<void> http2_send_queue_add_co(const std::string &msg);
    asio::awaitable<void> http2_send_queue_add_co(const unsigned char *buffer, unsigned int buffersize);
    void http2_send_queue_add(const std::string &msg);
    void http2_send_queue_add(const unsigned char *buffer, unsigned int buffersize);
    asio::awaitable<void> http2_send_queue_co();

  public:
    unsigned char *_cache_data  = nullptr;
    std::atomic_uint time_limit = 0;

    bool isssl   = false;
    bool isgoway = false;
    bool isclose = false;
    bool iserror = false;

    unsigned char httpv = 0;

    std::unique_ptr<asio::ip::tcp::socket> socket;
    std::unique_ptr<asio::ssl::stream<asio::ip::tcp::socket>> sslsocket;

    std::atomic<unsigned long long> window_update_num     = 0;
    std::atomic<unsigned long long> old_window_update_num = 0;
    std::atomic<long long> new_send_balance_num           = 0;
    std::atomic<unsigned long long> has_send_update_num   = 0;

    std::mutex http2_loop_send_mutex;
    std::atomic_bool http_loop_in = false;
    std::queue<std::string> http2_send_queue;
};
}// namespace http

#endif
