#ifndef SERVICE_HPP
#define SERVICE_HPP
#include "../log/logger.h"
#include "../net/http/http_request.h"
#include "../database/database.h"

using namespace cppServer;

bool login(HttpRequest::ptr req, HttpResponse::ptr res)
{
    try
    {
        auto db = Database::getInstance();
        std::string userId = req->getHeader("userId");
        std::string mId = req->getHeader("mId");
        LogDebug(KV(userId) << KV(mId));

        if (Database::updateIpandPort(db, userId, mId, req->m_ip, req->m_port))
            return true;
        return false;
    }
    catch (const exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
}
bool getRoute(HttpRequest::ptr req, HttpResponse::ptr res)
{
    try
    {
        auto db = Database::getInstance();

        std::string userId = req->getHeader("userId");
        std::string mId = req->getHeader("mId");
        std::string ip = "";
        std::string port = "";

        if (Database::getUserInfo(db, userId, mId, ip, port))
        {
            LogDebug(KV(ip) << KV(port));
            res->setHead("ip", std::move(ip));
            res->setHead("port", std::move(port));
            return true;
        }
        return false;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
}

bool ping(HttpRequest::ptr req, HttpResponse::ptr res)
{
    try
    {
        auto db = Database::getInstance();

        std::string userId = req->getHeader("userId");
        std::string mId = req->getHeader("mId");

        if (Database::updateIpandPort(db, userId, mId, req->m_ip, req->m_port))
            return true;
        return false;
    }
    catch (const exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
}

#endif // SERVICE_HPP