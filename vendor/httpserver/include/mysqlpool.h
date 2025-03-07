#ifndef HTTP_MYSQLPOOL_H
#define HTTP_MYSQLPOOL_H

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <mutex>

#include <list>
#include "mysql.h"

namespace http
{
using MYSQL_CONN_PTR = std::unique_ptr<MYSQL, decltype(&mysql_close)>;

struct mysql_connect_link_info
{
    std::string host;
    std::string username;
    std::string password;
    std::string db;
    std::string unix_socket;
    unsigned int port;
};
class mysqllinkpool
{
  public:
    mysqllinkpool() = delete;
    mysqllinkpool(struct mysql_connect_link_info, struct mysql_connect_link_info);
    mysqllinkpool(struct mysql_connect_link_info, struct mysql_connect_link_info, unsigned int num, unsigned int editnum);
    MYSQL_CONN_PTR get_select_connect();
    MYSQL_CONN_PTR get_edit_connect();

    MYSQL_CONN_PTR add_select_connect();
    MYSQL_CONN_PTR add_edit_connect();

    void addpool_select_connect();
    void addpool_edit_connect();

    void back_select_connect(MYSQL_CONN_PTR);
    void back_edit_connect(MYSQL_CONN_PTR);
    unsigned int clearpool();

  public:
    std::list<MYSQL_CONN_PTR> mysql_select_pool_list;
    std::list<MYSQL_CONN_PTR> mysql_edit_pool_list;
    unsigned int select_current_num  = 0;
    unsigned int select_max_pool_num = 350;
    unsigned int edit_current_num    = 0;
    unsigned int edit_max_pool_num   = 350;

    struct mysql_connect_link_info select_link;
    struct mysql_connect_link_info edit_link;

    std::mutex lock_select_list;
    std::mutex lock_edit_list;
};
std::map<std::size_t, std::shared_ptr<mysqllinkpool>> &get_mysqlpool();
}// namespace http
#endif