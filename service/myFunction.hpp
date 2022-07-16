#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H
#include <iostream>
#include <sys/socket.h>
#include "json.hpp"
#include "../database/database.hpp"

using namespace std;
using json_t = nlohmann::json;

void getPeerInfo(int &fd, std::string &ip, std::string &port); // 获取客户端的路由信息

bool checkUser(sqlite3 *db, json_t &info, int &clientFd, json_t &j_response); // 验证用户，并存下用户的ip和port

bool getRoute(sqlite3 *db, json_t &info, json_t &j_response); // 获取客户端对端的路由信息

bool checkConnection(sqlite3 *db, json_t &info, int &clientFd, json_t &j_response); // 接受客户端的心跳包，实时更新路由信息

void processContext(std::string context, std::string &response, int &fd)
{
    json_t j_response;
    try
    {
        std::cout << "the length of context:" << context.length() << endl;
        std::cout << "context:" << context << endl;
        auto j_context = json_t::parse(context);
        std::cout << "userId:" << j_context["userId"] << std::endl;

        // TODO:针对json数据进程用户认证，路由转发等等
        std::string filename = "../database/test.db";
        sqlite3 *db = open_database(filename);
        std::string type = j_context["type"];

        json_t j_response;
        if (type == "login")
        {
            // updateIpandPort
            std::cout << "login" << std::endl;
            if (!checkUser(db, j_context, fd, j_response))
            {
                throw std::runtime_error("login failed");
            }
        }
        else if (type == "getRoute")
        {
            // getRoute
            std::cout << "getRoute" << std::endl;
            if (!getRoute(db, j_context, j_response))
            {
                throw std::runtime_error("getRoute failed");
            }
        }
        else if (type == "checkConnection")
        {
            // TODO:
            std::cout << "checkConnection" << std::endl;
            if (!checkConnection(db, j_context, fd, j_response))
            {
                throw std::runtime_error("checkConnection failed");
            }
        }

        response = j_response.dump();
    }
    catch (const exception &e) //上述操作失败后，统一在这里返回
    {
        std::cout << e.what() << std::endl;
        json_t j_response;
        j_response["status"] = "failed";
        // j_response["err_msg"] = e.what();
        response = j_response.dump();
    }
}

bool checkUser(sqlite3 *db, json_t &info, int &clientFd, json_t &j_response)
{

    // TODO:check this userId,if userId in database, return true
    std::string userId = info["userId"];
    std::string mId = info["mId"];
    std::string ip = "";
    std::string port = "";

    getPeerInfo(clientFd, ip, port); //获取当前连接的ip和port

    if (updateIpandPort(db, userId, mId, ip, port)) //将ip和port更新进数据库
    {
        j_response["status"] = "ok";
        return true;
    }
    return false;
}

bool getRoute(sqlite3 *db, json_t &info, json_t &j_response)
{

    // TODO：根据客户端类型，将其对端的IP和PORT发给他
    std::string userId = info["userId"];
    std::string mId = info["mId"];
    std::string ip = "";
    std::string port = "";

    if (getUserInfo(db, userId, mId, ip, port))
    {
        j_response["status"] = "ok";
        j_response["ip"] = ip;
        j_response["port"] = port;
        return true;
    }

    return false;
}

void getPeerInfo(int &fd, std::string &ip, std::string &port)
{
    struct sockaddr_in addrClient;
    int addrClientlen = sizeof(addrClient);

    // get the client ip and port from connnection.
    getpeername(fd, (sockaddr *)&addrClient, (socklen_t *)&addrClientlen);

    // convert ip and port to normal form.
    char client_ip[20];
    inet_ntop(AF_INET, &addrClient.sin_addr, client_ip, sizeof(client_ip));
    ip = client_ip;
    port = to_string(ntohs(addrClient.sin_port)); //将整形转成string
}

bool checkConnection(sqlite3 *db, json_t &info, int &clientFd, json_t &j_response)
{

    // TODO:check this userId,if userId in database, return true
    std::string userId = info["userId"];
    std::string mId = info["mId"];
    std::string ip = "";
    std::string port = "";

    getPeerInfo(clientFd, ip, port); //获取当前连接的ip和port

    if (updateIpandPort(db, userId, mId, ip, port)) //将ip和port更新进数据库
    {
        return true;
    }
    return false;
}
/*
int main()
{

    // json_t j; // JSON对象

    // j["age"] = 23;               // "age":23
    // j["name"] = "spiderman";     // "name":"spiderman"
    // j["gear"]["suits"] = "2099"; // "gear":{"suits":"2099"}
    // j["jobs"] = {"superhero"};   // "jobs":["superhero"]

    // vector<int> v = {1, 2, 3}; // vector容器
    // j["numbers"] = v;          // "numbers":[1,2,3]

    // map<string, int> m =          // map容器
    //     {{"one", 1}, {"two", 2}}; // 初始化列表
    // j["kv"] = m;                  // "kv":{"one":1,"two":2}

    // std::cout << j.dump() << std::endl;
    // std::cout << j.dump(2) << std::endl;

    string str = "{\"name\": \"peter\",\"age\" : 23,\"married\" : true}";
    cout << str << endl;
    auto j = json_t::parse(str); // 从字符串反序列化
    assert(j["age"] == 23);      // 验证序列化是否正确
    assert(j["name"] == "peter");
    std::cout << j.dump(2) << std::endl;

    return 0;
}
*/

#endif // AUTHORIZATION_H