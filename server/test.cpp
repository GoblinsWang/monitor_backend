#include "../net/http/http_server.h"
#include "service.hpp"
using namespace cppServer;

void onFault(HttpResponse::ptr res)
{
    res->m_statusCode = NotFound;
    res->m_statusMessage = "Not Found";
    // res->m_closeConnection = 1;
}

// HttpCallback
void onRequest(HttpRequest::ptr req, HttpResponse::ptr res)
{
    int pos = req->m_url.find("?");

    std::string path = (pos == -1) ? req->m_url : req->m_url.substr(0, pos);

    res->m_statusCode = OK;
    res->m_statusMessage = "OK";
    res->m_contentType = "application/json";
    int find = 0;
    if (req->m_method == "GET")
    {
        LogDebug(KV(req->m_method));
        if (path == "/login" && login(req, res))
        {
            find = 1;
        }
        else if (path == "/route" && getRoute(req, res))
        {
            find = 1;
        }
    }
    else if (req->m_method == "PUT")
    {
        LogDebug(KV(req->m_method));
        if (path == "/ping" && ping(req, res))
        {
            find = 1;
        }
    }
    if (!find)
    {
        onFault(res);
    }
}

int main(int argc, char **argv)
{

    HttpServer http_server;

    http_server.listen("0.0.0.0", 12345, 0);

    http_server.setHttpCallback(onRequest);

    http_server.start();
}