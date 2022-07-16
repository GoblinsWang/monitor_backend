#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <error.h>
#include <iostream>
#include <string>

/*
    typedef int (*sqlite3_callback)(
    void*,    // Data provided in the 4th argument of sqlite3_exec()
    int,      // The number of columns in row
    char**,   // An array of strings representing fields in the row
    char**    // An array of strings representing column names
    );
*/
static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for (i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

//打开数据库文件
sqlite3 *open_database(std::string &filename)
{
    sqlite3 *db;
    int rc = sqlite3_open(filename.c_str(), &db);
    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }
    return db;
}

// 执行sql语句
bool execute_sql(sqlite3 *db, std::string &sql)
{
    char *zErrMsg = 0;
    /* Execute SQL statement */
    int rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    sqlite3_close(db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return false;
    }
    else
        return true;
}

// 初始化数据(create, insert)
void init_table(sqlite3 *db)
{
    // 创建一张表
    std::string sql = "CREATE TABLE AppUser ("
                      "id          INTEGER PRIMARY KEY,"
                      "userId    VARCHAR UNIQUE,"
                      "zero_ip       VARCHAR,"
                      "zero_port    VARCHAR,"
                      "one_ip       VARCHAR,"
                      "one_port    VARCHAR"
                      ");";
    execute_sql(db, sql);
    std::cout << "after create table!" << std::endl;

    // 添加一些测试信息
    sql = "INSERT INTO AppUser (userId, zero_ip, zero_port, one_ip, one_port)"
          "VALUES ('admin', '', '', '', '');"
          "INSERT INTO AppUser (userId, zero_ip, zero_port, one_ip, one_port)"
          "VALUES ('xiaoming', '', '', '', '');"
          "INSERT INTO AppUser (userId, zero_ip, zero_port, one_ip, one_port)"
          "VALUES ('wangwu', '', '', '', '');"
          "INSERT INTO AppUser (userId, zero_ip, zero_port, one_ip, one_port)"
          "VALUES ('zhangsan', '', '', '', '');";
    execute_sql(db, sql);
}

//获取用户信息(select)
bool getUserInfo(sqlite3 *db, std::string &userId, std::string &mId, std::string &ip, std::string &port)
{
    int rc = 0; //返回值
    std::string sql = "SELECT * from AppUser where userId = '" + userId + "';";

    /*step 2: sql语句对象。*/
    sqlite3_stmt *pStmt;

    rc = sqlite3_prepare_v2(
        db,           //数据库连接对象
        sql.c_str(),  //指向原始sql语句字符串
        sql.length(), //
        &pStmt,
        NULL);
    if (rc != SQLITE_OK)
    {
        perror("sqlite3_prepare_v2 error:");
        return false;
    }

    while (1)
    {
        /* ---------------执行SQL语句-------------------*/
        rc = sqlite3_step(pStmt);
        if (rc == SQLITE_DONE) //执行完成
        {
            break;
        }
        else if (rc == SQLITE_ROW) //获取到一行的结果
        {
            if (mId == "one")
            {
                ip = (char *)sqlite3_column_text(pStmt, 2);
                port = (char *)sqlite3_column_text(pStmt, 3);
            }
            if (mId == "zero")
            {
                ip = (char *)sqlite3_column_text(pStmt, 2);
                port = (char *)sqlite3_column_text(pStmt, 3);
            }
        }
    }

    std::cout << "ip:" << ip << " port:" << port << std::endl;
    sqlite3_finalize(pStmt); //销毁一个SQL语句对象
    sqlite3_close(db);
    return true;
}

bool updateIpandPort(sqlite3 *db, std::string &userId, std::string &mId, std::string &ip, std::string &port)
{
    std::string sql;
    if (mId == "one")
    {
        sql = "UPDATE AppUser set one_ip = '" + ip + "', one_port = '" + port + "' where userId = '" + userId + "';";
    }
    if (mId == "zero")
    {
        sql = "UPDATE AppUser set zero_ip = '" + ip + "', zero_port = '" + port + "' where userId = '" + userId + "';";
    }
    std::cout << sql << std::endl;
    if (!execute_sql(db, sql))
        return false;
    return true;
}

#if 0
    int main(int argc, char *argv[])
    {
        sqlite3 *db;

        // Open database
        std::string filename = "test.db";
        db = open_database(filename);

        // Init table information
        init_table(db);

        // TODO:what you want to implement
        get_allusers(db);

        std::string pswd = "12345";
        std::string username = "admin";
        update_email(db, username, pswd);

        sqlite3_close(db);
        return 0;
    }
#endif