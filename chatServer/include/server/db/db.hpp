#ifndef DB_H
#define DB_H

#include <mysql/mysql.h>
#include <iostream>
#include <string.h>

using namespace std;

class MySql
{
public:
    MySql();
    ~MySql();

    bool connect(const char *host = "localhost", const char *user = "root",
                 const char *passwd = "12345678", const char *db = "chat", int port = 3031);

    bool update(string sql);

    MYSQL_RES *query(string sql);

    MYSQL *getConnection();

private:
    MYSQL *mysql;
};

#endif