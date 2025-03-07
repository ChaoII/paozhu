/*
 *  @author 黄自权 huangziquan
 *  @date 2023-01-06
 *  @dest controller method pick file
 */
#include <cstdio>
#include <string>
#include <sstream>
#include <algorithm>
#include <sstream>
#include <map>
#include <vector>
#include <filesystem>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#define stat _stat
#endif

#include "autopickcontrolmethod.hpp"
#include "md5.h"

namespace fs = std::filesystem;
bool stringcasecmp(std::string_view str1, std::string_view str2)
{
    if (str1.size() != str2.size())
    {
        return false;
    }
    for (unsigned int i = 0; i < str1.size(); i++)
    {
        if (str1[i] != str2[i])
        {
            if (str1[i] < 91 && str1[i] > 64)
            {
                if ((str1[i] + 32) == str2[i])
                {
                    continue;
                }
            }
            else if (str2[i] < 91 && str2[i] > 64)
            {
                if (str1[i] == (str2[i] + 32))
                {
                    continue;
                }
            }
            return false;
        }
    }
    return true;
}
std::string get_filename(const std::string &filename)
{
    std::string filename_name;
    int j = filename.size() - 1;
    if (j == -1)
        return "";
    for (; j >= 0; j--)
    {
        if (filename[j] == '.')
        {
            if (filename_name.size() == 2 && filename_name[0] == 'z' && filename_name[1] == 'g')
            {
                continue;
            }
            filename_name.clear();
            continue;
        }
        if (filename[j] == '/')
        {
            j--;
            break;
        }
        filename_name.push_back(filename[j]);
    }
    std::reverse(filename_name.begin(), filename_name.end());
    return filename_name;
}

void make_all_directory(const std::string &path, const std::string &pathname)
{
    std::string path_temp = path;

    unsigned int path_size = pathname.size();

    for (unsigned int j = 0; j < path_size; j++)
    {
        if (pathname[j] == '/' && path_temp.size() > 0)
        {
            fs::path paths = path_temp;
            if (!fs::exists(paths))
            {
                fs::create_directories(paths);
                fs::permissions(paths,
                                fs::perms::owner_all | fs::perms::group_all | fs::perms::others_read,
                                fs::perm_options::add);
            }
        }
        path_temp.push_back(pathname[j]);
    }
}

int main(int argc, char *argv[])
{

    std::string current_run_path    = "";
    std::string current_method_file = "";
    fs::path current_path;
    if (argc > 0)
    {
        current_run_path.append(argv[1]);
    }
    else
    {
        current_path = fs::current_path();
        current_run_path.append(current_path.string());
    }

    if (current_run_path.back() != '/')
    {
        current_run_path.push_back('/');
    }

    http::pickcontrol lp;
    std::vector<std::string> header_lists;
    std::vector<struct http::reg_autoitem> reg_method_lists;
    std::vector<struct http::reg_autoitem> domain_method_lists;

    std::vector<struct http::reg_autoitem> co_reg_method_lists;
    std::vector<struct http::reg_autoitem> co_domain_method_lists;

    std::string src_path = current_run_path + "controller/src";

    fs::path vsrcpath = src_path;

    if (!fs::exists(vsrcpath))
    {
        std::cout << " controller directory not in current path " << std::endl;
        return 0;
    }

    fs::path cache_path = current_run_path + "cache";
    if (!fs::exists(cache_path))
    {
        fs::create_directories(cache_path);
        fs::permissions(cache_path,
                        fs::perms::owner_all | fs::perms::group_all | fs::perms::others_read,
                        fs::perm_options::add);
    }
    // md5 hash => filetime

    auto plist = lp.listpath(src_path);

    std::string cache_file_path = current_run_path + "cache/method_notes.data";

    std::map<std::string, struct http::file_regitem> reginfo_list = lp.loadcacheinfo(cache_file_path);

    struct stat sessfileinfo;
    std::string md5hash;
    std::string hash_value;
    std::string mttime_value;
    std::string domain_value;
    struct http::file_regitem cahce_info;
    bool is_must_again = false;
    for (unsigned int i = 0; i < plist.size(); i++)
    {
        std::string filename;// = src_path;// get_filename(plist[i]);

        bool is_haspath     = false;
        bool is_domain_path = false;
        bool is_file_domain = false;
        filename            = plist[i];
        domain_value.clear();
        for (unsigned int jj = 0; jj < filename.size(); jj++)
        {
            if (filename[jj] == '/')
            {
                is_haspath = true;
                break;
            }
            if (filename[jj] == '.')
            {
                is_domain_path = true;
            }
            domain_value.push_back(filename[jj]);
        }

        if (is_haspath)
        {
            if (domain_value.size() > 0 && is_domain_path)
            {
                is_file_domain = true;
            }
        }

        filename = src_path;
        if (filename.back() != '/')
        {
            filename.push_back('/');
        }
        filename.append(plist[i]);
        filename.append(".cpp");
        auto method_item = lp.pickfile(filename);

        if (method_item.size() > 0)
        {
            md5hash.clear();
            mttime_value.clear();
            for (unsigned int j = 0; j < method_item.size(); j++)
            {
                md5hash.append(method_item[j].pre);
                md5hash.append(method_item[j].func);
                md5hash.append(method_item[j].urlpath);
            }
            hash_value = http::md5(md5hash);

            filename.clear();
            filename.append(current_run_path);
            filename.append("controller/include/");
            // create custom directory
            make_all_directory(filename, plist[i]);

            filename.append(plist[i]);

            std::string fileinh = filename + ".h";
            filename            = plist[i];

            memset(&sessfileinfo, 0, sizeof(sessfileinfo));
            if (stat(fileinh.c_str(), &sessfileinfo) == 0)
            {
                if (sessfileinfo.st_mode & S_IFREG)
                {
                    mttime_value = std::to_string(sessfileinfo.st_mtime);
                }
            }
            cahce_info = reginfo_list[filename];

            if (cahce_info.filehash.size() > 0 && cahce_info.filehash == hash_value && mttime_value.size() > 0 &&
                mttime_value == cahce_info.filetime)
            {
            }
            else
            {
                lp.createhfile(fileinh, method_item);

                memset(&sessfileinfo, 0, sizeof(sessfileinfo));
                if (stat(fileinh.c_str(), &sessfileinfo) == 0)
                {
                    if (sessfileinfo.st_mode & S_IFREG)
                    {
                        mttime_value = std::to_string(sessfileinfo.st_mtime);
                    }
                }

                cahce_info.filename    = filename;
                cahce_info.filetime    = mttime_value;
                cahce_info.filehash    = hash_value;
                reginfo_list[filename] = cahce_info;
                is_must_again          = true;
            }

            for (unsigned int j = 0; j < method_item.size(); j++)
            {
                if (is_file_domain)
                {
                    if (method_item[j].is_co)
                    {
                        method_item[j].domain = domain_value;
                        co_domain_method_lists.emplace_back(method_item[j]);
                    }
                    else
                    {
                        method_item[j].domain = domain_value;
                        domain_method_lists.emplace_back(method_item[j]);
                    }
                }
                else
                {
                    if (method_item[j].is_co)
                    {
                        co_reg_method_lists.emplace_back(method_item[j]);
                    }
                    else
                    {
                        reg_method_lists.emplace_back(method_item[j]);
                    }
                }
            }
            filename.append(".h\"\r\n");
            header_lists.emplace_back(filename);
        }
    }

    lp.savecacheinfo(cache_file_path, reginfo_list);

    current_method_file = current_run_path + "common/autocontrolmethod.hpp";

    std::unique_ptr<std::FILE, int (*)(std::FILE *)> fp(std::fopen(current_method_file.c_str(), "wb"), std::fclose);

    if (!fp.get())
    {
        return 0;
    }

    std::string automethod_content;

    automethod_content = R"(
#ifndef __HTTP_AUTO_REG_CONTROL_HTTPMETHOD_HPP
#define __HTTP_AUTO_REG_CONTROL_HTTPMETHOD_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "httppeer.h" 

)";
    //all .h files
    for (unsigned int j = 0; j < header_lists.size(); j++)
    {
        automethod_content.append("#include \"");
        automethod_content.append(header_lists[j]);
    }
    // co and nomral use method
    std::map<std::string, std::vector<std::string>> _http_regurlpath_table;
    std::map<std::string, std::map<std::string, std::vector<std::string>>> _domain_regurlpath_table;

    automethod_content.append(R"( 
namespace http
{ 
    )");

    //nomral
    ///////////////////////////////////
    automethod_content.append(R"( 
    void _initauto_control_httpmethodregto(std::map<std::string, regmethold_t> &methodcallback)
    {
        struct regmethold_t temp;

)");

    for (unsigned int j = 0; j < reg_method_lists.size(); j++)
    {
        if (stringcasecmp(reg_method_lists[j].pre, "null") ||
            stringcasecmp(reg_method_lists[j].pre, "nullptr"))
        {
            automethod_content.append("\t\ttemp.pre = nullptr;\r\n");
        }
        else
        {
            automethod_content.append("\t\ttemp.pre = ");
            automethod_content.append(reg_method_lists[j].pre);
            automethod_content.append(";\r\n");
        }
        automethod_content.append("\t\ttemp.regfun = ");
        automethod_content.append(reg_method_lists[j].func);
        automethod_content.append(";\r\n");

        // test restfull {}
        bool ismatch = false;
        for (unsigned int n = 0; n < reg_method_lists[j].urlpath.size(); n++)
        {
            if (reg_method_lists[j].urlpath[n] == ':')
            {
                ismatch = true;
                break;
            }
        }

        if (ismatch)
        {
            std::vector<std::string> paths;
            unsigned int n = 0;
            if (reg_method_lists[j].urlpath.size() > 0 && reg_method_lists[j].urlpath[0] == '/')
            {
                n += 1;
            }
            ismatch = true;
            std::string temp_str;
            std::string temp_new_urlpath;
            for (; n < reg_method_lists[j].urlpath.size(); n++)
            {
                if (reg_method_lists[j].urlpath[n] == ':')
                {
                    ismatch = false;
                }

                if (reg_method_lists[j].urlpath[n] == '/')
                {
                    paths.push_back(temp_str);
                    if (ismatch)
                    {
                        if (temp_new_urlpath.size() > 0)
                        {
                            temp_new_urlpath.push_back('/');
                            temp_new_urlpath.append(temp_str);
                        }
                        else
                        {
                            temp_new_urlpath.append(temp_str);
                        }
                    }
                    temp_str.clear();
                    continue;
                }
                if (reg_method_lists[j].urlpath[n] == '"' || reg_method_lists[j].urlpath[n] == 0x5C ||
                    reg_method_lists[j].urlpath[n] == 0x27)
                {
                    continue;
                }
                temp_str.push_back(reg_method_lists[j].urlpath[n]);
            }
            if (temp_str.size() > 0)
            {
                paths.push_back(temp_str);
                if (ismatch)
                {
                    if (temp_new_urlpath.size() > 0)
                    {
                        temp_new_urlpath.push_back('/');
                        temp_new_urlpath.append(temp_str);
                    }
                    else
                    {
                        temp_new_urlpath.append(temp_str);
                    }
                }
                temp_str.clear();
            }
            reg_method_lists[j].urlpath              = temp_new_urlpath;
            _http_regurlpath_table[temp_new_urlpath] = paths;
        }

        automethod_content.append("\t\tmethodcallback.emplace(\"");
        automethod_content.append(reg_method_lists[j].urlpath);
        automethod_content.append("\",temp);\r\n");
    }

    automethod_content.append(R"(

    }
    )");

    //co handler
    ///////////////////////////////////
    automethod_content.append(R"(
    void _initauto_co_control_httpmethodregto(std::map<std::string, regmethold_co_t> &methodcallback)
    {
        struct regmethold_co_t temp;
)");

    for (unsigned int j = 0; j < co_reg_method_lists.size(); j++)
    {
        if (stringcasecmp(co_reg_method_lists[j].pre, "null") ||
            stringcasecmp(co_reg_method_lists[j].pre, "nullptr"))
        {
            automethod_content.append("\t\ttemp.pre = nullptr;\r\n");
        }
        else
        {
            automethod_content.append("\t\ttemp.pre = ");
            automethod_content.append(co_reg_method_lists[j].pre);
            automethod_content.append(";\r\n");
        }
        automethod_content.append("\t\ttemp.regfun = ");
        automethod_content.append(co_reg_method_lists[j].func);
        automethod_content.append(";\r\n");

        // test restfull {}
        bool ismatch = false;
        for (unsigned int n = 0; n < co_reg_method_lists[j].urlpath.size(); n++)
        {
            if (co_reg_method_lists[j].urlpath[n] == ':')
            {
                ismatch = true;
                break;
            }
        }

        if (ismatch)
        {
            std::vector<std::string> paths;
            unsigned int n = 0;
            if (co_reg_method_lists[j].urlpath.size() > 0 && co_reg_method_lists[j].urlpath[0] == '/')
            {
                n += 1;
            }
            ismatch = true;
            std::string temp_str;
            std::string temp_new_urlpath;
            for (; n < co_reg_method_lists[j].urlpath.size(); n++)
            {
                if (co_reg_method_lists[j].urlpath[n] == ':')
                {
                    ismatch = false;
                }

                if (co_reg_method_lists[j].urlpath[n] == '/')
                {
                    paths.push_back(temp_str);
                    if (ismatch)
                    {
                        if (temp_new_urlpath.size() > 0)
                        {
                            temp_new_urlpath.push_back('/');
                            temp_new_urlpath.append(temp_str);
                        }
                        else
                        {
                            temp_new_urlpath.append(temp_str);
                        }
                    }
                    temp_str.clear();
                    continue;
                }
                if (co_reg_method_lists[j].urlpath[n] == '"' || co_reg_method_lists[j].urlpath[n] == 0x5C ||
                    co_reg_method_lists[j].urlpath[n] == 0x27)
                {
                    continue;
                }
                temp_str.push_back(co_reg_method_lists[j].urlpath[n]);
            }
            if (temp_str.size() > 0)
            {
                paths.push_back(temp_str);
                if (ismatch)
                {
                    if (temp_new_urlpath.size() > 0)
                    {
                        temp_new_urlpath.push_back('/');
                        temp_new_urlpath.append(temp_str);
                    }
                    else
                    {
                        temp_new_urlpath.append(temp_str);
                    }
                }
                temp_str.clear();
            }
            co_reg_method_lists[j].urlpath           = temp_new_urlpath;
            _http_regurlpath_table[temp_new_urlpath] = paths;
        }

        automethod_content.append("\t\tmethodcallback.emplace(\"");
        automethod_content.append(co_reg_method_lists[j].urlpath);
        automethod_content.append("\",temp);\r\n");
    }

    automethod_content.append(R"(

    }
    )");

    // nomral domain handler
    automethod_content.append(R"(
    void _initauto_domain_httpmethodregto(std::map<std::string, std::map<std::string, regmethold_t>> &domain_methodcallback)
    {
        struct regmethold_t temp;
        std::map<std::string, regmethold_t> methodcallback;
        std::map<std::string, std::map<std::string, regmethold_t>>::iterator domain_iterator;
    )");

    for (unsigned int j = 0; j < domain_method_lists.size(); j++)
    {
        if (stringcasecmp(domain_method_lists[j].pre, "null") ||
            stringcasecmp(domain_method_lists[j].pre, "nullptr"))
        {
            automethod_content.append("\ttemp.pre = nullptr;\r\n");
        }
        else
        {
            automethod_content.append("\ttemp.pre = ");
            automethod_content.append(domain_method_lists[j].pre);
            automethod_content.append(";\r\n");
        }
        automethod_content.append("\t\ttemp.regfun = ");
        automethod_content.append(domain_method_lists[j].func);
        automethod_content.append(";\r\n");

        // test restfull {}
        bool ismatch = false;
        for (unsigned int n = 0; n < domain_method_lists[j].urlpath.size(); n++)
        {
            if (domain_method_lists[j].urlpath[n] == ':')
            {
                ismatch = true;
                break;
            }
        }

        if (ismatch)
        {
            std::vector<std::string> paths;
            unsigned int n = 0;
            if (domain_method_lists[j].urlpath.size() > 0 && domain_method_lists[j].urlpath[0] == '/')
            {
                n += 1;
            }
            ismatch = true;
            std::string temp_str;
            std::string temp_new_urlpath;
            for (; n < domain_method_lists[j].urlpath.size(); n++)
            {
                if (domain_method_lists[j].urlpath[n] == ':')
                {
                    ismatch = false;
                }

                if (domain_method_lists[j].urlpath[n] == '/')
                {
                    paths.push_back(temp_str);
                    if (ismatch)
                    {
                        if (temp_new_urlpath.size() > 0)
                        {
                            temp_new_urlpath.push_back('/');
                            temp_new_urlpath.append(temp_str);
                        }
                        else
                        {
                            temp_new_urlpath.append(temp_str);
                        }
                    }
                    temp_str.clear();
                    continue;
                }
                if (domain_method_lists[j].urlpath[n] == '"' || domain_method_lists[j].urlpath[n] == 0x5C ||
                    domain_method_lists[j].urlpath[n] == 0x27)
                {
                    continue;
                }
                temp_str.push_back(domain_method_lists[j].urlpath[n]);
            }
            if (temp_str.size() > 0)
            {
                paths.push_back(temp_str);
                if (ismatch)
                {
                    if (temp_new_urlpath.size() > 0)
                    {
                        temp_new_urlpath.push_back('/');
                        temp_new_urlpath.append(temp_str);
                    }
                    else
                    {
                        temp_new_urlpath.append(temp_str);
                    }
                }
                temp_str.clear();
            }
            domain_method_lists[j].urlpath = temp_new_urlpath;
            //_http_regurlpath_table[temp_new_urlpath] = paths;
            //domain table
            auto domain_map_iter = _domain_regurlpath_table.find(domain_method_lists[j].domain);
            if (domain_map_iter != _domain_regurlpath_table.end())
            {
                domain_map_iter->second[temp_new_urlpath] = paths;
            }
            else
            {
                std::map<std::string, std::vector<std::string>> temp_domain_map_a;
                temp_domain_map_a[temp_new_urlpath] = paths;
                _domain_regurlpath_table.emplace(domain_method_lists[j].domain, temp_domain_map_a);
            }
        }
        //path safe filter
        automethod_content.append(R"(
        domain_iterator=domain_methodcallback.find(")");
        automethod_content.append(domain_method_lists[j].domain);
        automethod_content.append(R"("); 
        if(domain_iterator!=domain_methodcallback.end())
        {
            domain_iterator->second.emplace(")");
        automethod_content.append(domain_method_lists[j].urlpath);
        automethod_content.append("\",temp);\r\n");
        automethod_content.append(R"( 
        }
        else
        {
            methodcallback.clear();)");
        automethod_content.append(R"( 
            methodcallback.emplace(")");
        automethod_content.append(domain_method_lists[j].urlpath);
        automethod_content.append(R"(",temp);
            domain_methodcallback.emplace(")");
        automethod_content.append(domain_method_lists[j].domain);
        automethod_content.append(R"(",methodcallback); 
        } 
        )");
        // automethod_content.append("\t\tmethodcallback.emplace(\"");
        // automethod_content.append(domain_method_lists[j].urlpath);
        // automethod_content.append("\",temp);\r\n");
    }

    automethod_content.append(R"(

    }
    )");

    //co domain handler
    automethod_content.append(R"(
    void _initauto_co_domain_httpmethodregto(std::map<std::string, std::map<std::string, regmethold_co_t>> &domain_methodcallback)
    {
        struct regmethold_co_t temp;
        std::map<std::string, regmethold_co_t> methodcallback;
        std::map<std::string, std::map<std::string, regmethold_co_t>>::iterator domain_iterator;
    )");

    for (unsigned int j = 0; j < co_domain_method_lists.size(); j++)
    {
        if (stringcasecmp(co_domain_method_lists[j].pre, "null") ||
            stringcasecmp(co_domain_method_lists[j].pre, "nullptr"))
        {
            automethod_content.append("\ttemp.pre = nullptr;\r\n");
        }
        else
        {
            automethod_content.append("\ttemp.pre = ");
            automethod_content.append(co_domain_method_lists[j].pre);
            automethod_content.append(";\r\n");
        }
        automethod_content.append("\t\ttemp.regfun = ");
        automethod_content.append(co_domain_method_lists[j].func);
        automethod_content.append(";\r\n");

        // test restfull {}
        bool ismatch = false;
        for (unsigned int n = 0; n < co_domain_method_lists[j].urlpath.size(); n++)
        {
            if (co_domain_method_lists[j].urlpath[n] == ':')
            {
                ismatch = true;
                break;
            }
        }

        if (ismatch)
        {
            std::vector<std::string> paths;
            unsigned int n = 0;
            if (co_domain_method_lists[j].urlpath.size() > 0 && co_domain_method_lists[j].urlpath[0] == '/')
            {
                n += 1;
            }
            ismatch = true;
            std::string temp_str;
            std::string temp_new_urlpath;
            for (; n < co_domain_method_lists[j].urlpath.size(); n++)
            {
                if (co_domain_method_lists[j].urlpath[n] == ':')
                {
                    ismatch = false;
                }

                if (co_domain_method_lists[j].urlpath[n] == '/')
                {
                    paths.push_back(temp_str);
                    if (ismatch)
                    {
                        if (temp_new_urlpath.size() > 0)
                        {
                            temp_new_urlpath.push_back('/');
                            temp_new_urlpath.append(temp_str);
                        }
                        else
                        {
                            temp_new_urlpath.append(temp_str);
                        }
                    }
                    temp_str.clear();
                    continue;
                }
                if (co_domain_method_lists[j].urlpath[n] == '"' || co_domain_method_lists[j].urlpath[n] == 0x5C ||
                    co_domain_method_lists[j].urlpath[n] == 0x27)
                {
                    continue;
                }
                temp_str.push_back(co_domain_method_lists[j].urlpath[n]);
            }
            if (temp_str.size() > 0)
            {
                paths.push_back(temp_str);
                if (ismatch)
                {
                    if (temp_new_urlpath.size() > 0)
                    {
                        temp_new_urlpath.push_back('/');
                        temp_new_urlpath.append(temp_str);
                    }
                    else
                    {
                        temp_new_urlpath.append(temp_str);
                    }
                }
                temp_str.clear();
            }
            co_domain_method_lists[j].urlpath = temp_new_urlpath;
            //_http_regurlpath_table[temp_new_urlpath] = paths;
            //domain table
            auto domain_map_iter = _domain_regurlpath_table.find(co_domain_method_lists[j].domain);
            if (domain_map_iter != _domain_regurlpath_table.end())
            {
                domain_map_iter->second[temp_new_urlpath] = paths;
            }
            else
            {
                std::map<std::string, std::vector<std::string>> temp_domain_map_a;
                temp_domain_map_a[temp_new_urlpath] = paths;
                _domain_regurlpath_table.emplace(co_domain_method_lists[j].domain, temp_domain_map_a);
            }
        }
        //path safe filter
        automethod_content.append(R"(
        domain_iterator=domain_methodcallback.find(")");
        automethod_content.append(co_domain_method_lists[j].domain);
        automethod_content.append(R"("); 
        if(domain_iterator!=domain_methodcallback.end())
        {
            domain_iterator->second.emplace(")");
        automethod_content.append(co_domain_method_lists[j].urlpath);
        automethod_content.append("\",temp);\r\n");
        automethod_content.append(R"( 
        }
        else
        {
            methodcallback.clear();)");
        automethod_content.append(R"( 
            methodcallback.emplace(")");
        automethod_content.append(co_domain_method_lists[j].urlpath);
        automethod_content.append(R"(",temp);
            domain_methodcallback.emplace(")");
        automethod_content.append(co_domain_method_lists[j].domain);
        automethod_content.append(R"(",methodcallback); 
        } 
        )");
        // automethod_content.append("\t\tmethodcallback.emplace(\"");
        // automethod_content.append(domain_method_lists[j].urlpath);
        // automethod_content.append("\",temp);\r\n");
    }

    automethod_content.append(R"(

    }
    )");
    //reg func handler

    automethod_content.append(R"(    
}    
#endif

    )");

    fwrite(&automethod_content[0], 1, automethod_content.size(), fp.get());
    if (is_must_again)
    {
        std::cout << "\033[1m\033[31m You must cmake and make again because the .h "
                     "header file has changed. \033[0m"
                  << std::endl;
    }

    automethod_content = R"(
#ifndef __HTTP_AUTO_REG_CONTROL_HTTPRESTFUL_HPP
#define __HTTP_AUTO_REG_CONTROL_HTTPRESTFUL_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "httppeer.h" 

)";
    automethod_content.append(R"(

namespace http
{
  void _initauto_control_httprestful_paths(std::map<std::string, std::vector<std::string>>  &restfulmethod)
  {
    
)");

    if (_http_regurlpath_table.size() > 0)
    {
        for (auto [keyname, paths] : _http_regurlpath_table)
        {
            automethod_content.append("\n\t\trestfulmethod[\"");
            automethod_content.append(keyname);
            automethod_content.append("\"]={");

            for (unsigned int j = 0; j < paths.size(); j++)
            {
                if (j > 0)
                {
                    automethod_content.append(",{\"");
                }
                else
                {
                    automethod_content.append("{\"");
                }

                if (paths[j].size() > 0 && paths[j][0] == ':')
                {
                    hash_value.clear();
                    for (unsigned int jj = 0; jj < paths[j].size(); jj++)
                    {
                        if (paths[j][jj] == ':')
                        {
                            continue;
                        }
                        //                         else if (paths[j][jj] == '}')
                        //                         {
                        //                             continue;
                        //                         }
                        hash_value.push_back(paths[j][jj]);
                    }
                    automethod_content.append(hash_value);
                }

                automethod_content.append("\"}");
            }

            automethod_content.append("};");
        }
    }
    else
    {
        automethod_content.append(R"(
        if(restfulmethod.size())
        {}
        )");
    }

    automethod_content.append(R"(

  }
    )");

    automethod_content.append(R"(
    void _initauto_domain_httprestful_paths(std::map<std::string,std::map<std::string, std::vector<std::string>>>  &restfulmethod)
    {
        std::map<std::string, std::vector<std::string>> temp_path;
        std::map<std::string,std::map<std::string, std::vector<std::string>>>::iterator domain_iterator;  
)");

    if (_domain_regurlpath_table.size() > 0)
    {
        for (auto [_domain_name, _regurlpath_table] : _domain_regurlpath_table)
        {
            for (auto [keyname, paths] : _regurlpath_table)
            {

                automethod_content.append(R"(     
                domain_iterator=restfulmethod.find(")");
                automethod_content.append(_domain_name);
                automethod_content.append(R"(");  
                if(domain_iterator!=restfulmethod.end())
                {
                )");
                automethod_content.append("\t domain_iterator->second[\"");
                automethod_content.append(keyname);
                automethod_content.append("\"]={");

                for (unsigned int j = 0; j < paths.size(); j++)
                {
                    if (j > 0)
                    {
                        automethod_content.append(",{\"");
                    }
                    else
                    {
                        automethod_content.append("{\"");
                    }

                    if (paths[j].size() > 0 && paths[j][0] == ':')
                    {
                        hash_value.clear();
                        for (unsigned int jj = 0; jj < paths[j].size(); jj++)
                        {
                            if (paths[j][jj] == ':')
                            {
                                continue;
                            }
                            hash_value.push_back(paths[j][jj]);
                        }
                        automethod_content.append(hash_value);
                    }

                    automethod_content.append("\"}");
                }

                automethod_content.append("};");

                //empty iter
                automethod_content.append(R"(     
                }
                else
                {
                    temp_path.clear(); 
                    temp_path[")");
                automethod_content.append(keyname);
                automethod_content.append("\"]={");

                for (unsigned int j = 0; j < paths.size(); j++)
                {
                    if (j > 0)
                    {
                        automethod_content.append(",{\"");
                    }
                    else
                    {
                        automethod_content.append("{\"");
                    }

                    if (paths[j].size() > 0 && paths[j][0] == ':')
                    {
                        hash_value.clear();
                        for (unsigned int jj = 0; jj < paths[j].size(); jj++)
                        {
                            if (paths[j][jj] == ':')
                            {
                                continue;
                            }
                            hash_value.push_back(paths[j][jj]);
                        }
                        automethod_content.append(hash_value);
                    }

                    automethod_content.append("\"}");
                }

                automethod_content.append("};");
                automethod_content.append(R"(
                    restfulmethod.emplace(")");
                automethod_content.append(_domain_name);
                automethod_content.append(R"(",temp_path); 
                }
                )");
            }
        }
    }
    else
    {
        automethod_content.append(R"(
        domain_iterator=restfulmethod.begin();
        temp_path.clear();
        )");
    }

    automethod_content.append(R"(

    }
    )");

    automethod_content.append(R"(
}

#endif

    )");

    current_method_file = current_run_path + "common/autorestfulpaths.hpp";

    std::unique_ptr<std::FILE, int (*)(std::FILE *)> fprest(std::fopen(current_method_file.c_str(), "wb"),
                                                            std::fclose);

    fwrite(&automethod_content[0], 1, automethod_content.size(), fprest.get());
    return 0;
}