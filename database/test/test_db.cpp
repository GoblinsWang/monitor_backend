#include <iostream>
#include "database.hpp"
using namespace std;

// g++ test.cpp database.hpp -o test -lsqlite3

int main(int argc, char *argv[])
{
    sqlite3 *db;

    // Open database
    std::string filename = "../build/test.db";
    db = open_database(filename);

    // Init table information
    // init_table(db);

    // TODO:what you want to implement
    std::string userId = "admin";
    std::string mId = "one";
    std::string ip;
    std::string port;
    getUserInfo(db, userId, mId, ip, port);
    sqlite3_close(db);
    return 0;
}