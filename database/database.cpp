#include "database.h"

sqlite3 *Database::m_database = nullptr;
std::mutex *Database::m_mutex = new std::mutex();

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
sqlite3 *Database::getInstance(std::string filename)
{
    m_mutex->lock();
    if (m_database == nullptr)
    {
        int rc = sqlite3_open(filename.c_str(), &m_database);
        if (rc)
        {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(m_database));
            exit(0);
        }
    }
    m_mutex->unlock();
    return m_database;
}

// 执行sql语句
bool Database::execute_sql(sqlite3 *db, std::string &sql)
{
    char *zErrMsg = 0;
    /* Execute SQL statement */
    int rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
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
void Database::initTable(sqlite3 *db)
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
bool Database::getUserInfo(sqlite3 *db, std::string &userId, std::string &mId, std::string &ip, std::string &port)
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
                ip = (char *)sqlite3_column_text(pStmt, 4);
                port = (char *)sqlite3_column_text(pStmt, 5);
            }
            if (mId == "zero")
            {
                ip = (char *)sqlite3_column_text(pStmt, 2);
                port = (char *)sqlite3_column_text(pStmt, 3);
            }
        }
    }

    sqlite3_finalize(pStmt); //销毁一个SQL语句对象
    // sqlite3_close(db);
    return true;
}

bool Database::updateIpandPort(sqlite3 *db, std::string &userId, std::string &mId, std::string &ip, std::string &port)
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
