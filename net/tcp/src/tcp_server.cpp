#include "../tcp_server.h"
#include "../tcp_connection.h"

using namespace cppServer;

TcpServer::TcpServer(Acceptor::ptr acceptor, int threadNum)
    : m_mapMutex(new mutex)
{

    m_eventloop = std::make_shared<EventLoop>("");
    m_acceptor = acceptor;
    m_threadNum = threadNum;
    m_threadPool = std::make_shared<EventLoopThreadPool>(m_eventloop, threadNum);
}

void TcpServer::start()
{
    // start thread pool
    m_threadPool->thread_pool_start();

    // set acceptor to main thread
    auto channel = std::make_shared<Channel>(m_acceptor->m_fd, EVENT_READ, m_eventloop.get());
    // currently, only need to set ReadCallback
    channel->setReadCallback(std::bind(&TcpServer::handleNewConnection, this));

    m_eventloop->add_channel_event(channel->m_fd, channel);

    m_eventloop->run();
}

void TcpServer::handleNewConnection()
{
    LogWarn(" enter handleNewConnection ......");
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int connected_fd = accept(m_acceptor->m_fd, (struct sockaddr *)&client_addr, &client_len);

    //设为非阻塞套接字描述符
    fcntl(connected_fd, F_SETFL, O_NONBLOCK);

    // choose event loop from thread pool
    auto eventloop = m_threadPool->getLoopFromThreadPool();
    LogDebug("new connection socket == " << connected_fd << KV(eventloop->m_thread_name));

    auto tcp_connection = std::make_shared<TcpConnection>(connected_fd, eventloop);
    LogDebug(KV(m_connectionMap.size()));
    m_mapMutex->lock();
    m_connectionMap[connected_fd] = tcp_connection;
    m_mapMutex->unlock();

    // set callback functions
    if (m_connectionCallback)
        tcp_connection->setConnectionCallback(m_connectionCallback);
    if (m_messageCallback)
        tcp_connection->setMessageCallback(m_messageCallback);
    if (m_writeCompleteCallback)
        tcp_connection->setWriteCompleteCallback(m_writeCompleteCallback);
    // it must be set.
    tcp_connection->setCloseCallback(std::bind(&TcpServer::handleCloseConnection, this, _1));

    tcp_connection->m_connectionCallback(tcp_connection.get());
}

void TcpServer::handleCloseConnection(TcpConnection *conn)
{
    m_mapMutex->lock();
    m_connectionMap.erase(conn->m_fd);
    m_mapMutex->unlock();
}