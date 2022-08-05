#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <error.h>
#include <iostream>
#include <string>
#include <mutex>

class Database
{
public:
    static sqlite3 *getInstance(std::string filename = "test.db");

    static void initTable(sqlite3 *db);

    static bool getUserInfo(sqlite3 *db, std::string &userId, std::string &mId, std::string &ip, std::string &port);

    static bool updateIpandPort(sqlite3 *db, std::string &userId, std::string &mId, std::string &ip, std::string &port);

private:
    Database() = delete;

    static bool execute_sql(sqlite3 *db, std::string &sql);

private:
    static sqlite3 *m_database;
    static std::mutex *m_mutex;
};
