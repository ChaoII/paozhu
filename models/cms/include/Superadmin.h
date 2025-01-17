#ifndef ORM_CMS_SUPERADMIN_H
#define ORM_CMS_SUPERADMIN_H
#include "mysqlorm.hpp"
#include "cms/include/superadminbase.h"

/* 如果此文件存在不会自动覆盖，没有则会自动生成。
 *If this file exists, it will not be overwritten automatically. If not, it will be generated automatically. */

namespace orm
{
namespace cms
{
class Superadmin : public mysql_orm<Superadmin, superadminbase>
{
  public:
    Superadmin(std::string dbtag);
    Superadmin();
};
}// namespace cms
};// namespace orm
#endif
