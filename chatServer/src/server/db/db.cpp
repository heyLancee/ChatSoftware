#include "db.hpp"
#include <muduo/base/Logging.h>

MySql::MySql()
{
    mysql = mysql_init(nullptr);
}
MySql::~MySql()
{
    if (mysql != nullptr)
    {
        mysql_close(mysql);
    }
}

bool MySql::connect(const char *host, const char *user, const char *passwd, const char *db, int port)
{
    if (mysql_real_connect(mysql, host, user, passwd, db, port, NULL, 0) == NULL)
    {
        return false;
    }
    mysql_query(mysql, "set names gbk");
    return true;
}

bool MySql::update(string sql)
{
    if (mysql_query(mysql, sql.c_str()))
    {
        return false;
    }
    return true;
}

MYSQL_RES *MySql::query(string sql)
{
    if (mysql_query(mysql, sql.c_str()))
    {
        return nullptr;
    }
    return mysql_use_result(mysql);
}

MYSQL *MySql::getConnection()
{
    return this->mysql;
}
