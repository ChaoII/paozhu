#ifndef ORM_CMS_SYSUSER_H
#define ORM_CMS_SYSUSER_H
#include "mysqlorm.hpp"
#include "cms/include/sysuserbase.h"
#include "cms/include/Article.h"
/* 如果此文件存在不会自动覆盖，没有则会自动生成。
 *If this file exists, it will not be overwritten automatically. If not, it will be generated automatically. */

namespace orm
{
namespace cms
{
class Sysuser : public mysql_orm<Sysuser, sysuserbase>
{
  public:
    Sysuser(std::string dbtag);
    Sysuser();
    std::vector<orm::cms::articlebase::meta> gettoparticle(int userid);
};
}// namespace cms
};// namespace orm
#endif
