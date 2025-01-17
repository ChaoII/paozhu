#ifndef ORM_CMS_SITEINFO_H
#define ORM_CMS_SITEINFO_H
#include "mysqlorm.hpp"
#include "cms/include/siteinfobase.h"

/* 如果此文件存在不会自动覆盖，没有则会自动生成。
 *If this file exists, it will not be overwritten automatically. If not, it will be generated automatically. */

namespace orm
{
namespace cms
{
class Siteinfo : public mysql_orm<Siteinfo, siteinfobase>
{
  public:
    Siteinfo(std::string dbtag);
    Siteinfo();
};
}// namespace cms
};// namespace orm
#endif
