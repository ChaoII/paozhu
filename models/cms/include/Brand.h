#ifndef ORM_CMS_BRAND_H
#define ORM_CMS_BRAND_H
#include "mysqlorm.hpp"
#include "cms/include/brandbase.h"

/* 如果此文件存在不会自动覆盖，没有则会自动生成。
 *If this file exists, it will not be overwritten automatically. If not, it will be generated automatically. */

namespace orm
{
namespace cms
{
class Brand : public mysql_orm<Brand, brandbase>
{
  public:
    Brand(std::string dbtag);
    Brand();
};
}// namespace cms
};// namespace orm
#endif
