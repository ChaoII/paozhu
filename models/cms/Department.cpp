
#include "mysqlmodel.hpp" 
#include "cms/include/departmentbase.h"
#include "cms/include/Department.h"

/* 如果此文件存在不会自动覆盖，没有则会自动生成。
*If this file exists, it will not be overwritten automatically. If not, it will be generated automatically. */

	 
 namespace orm{
	 namespace cms{  

			 Department::Department(std::string dbtag):mysqlclientDB(dbtag){}
			 Department::Department():mysqlclientDB(){}


		} 

	  }
