#include "../tcp_connection.h"

using namespace cppServer;

void cppServer::defaultConnectionCallback(TcpConnection *conn)
{
    //
    LogTrace("connection completed");
}

void cppServer::defaultMessageCallback(TcpConnection *conn)
{
    // TODO:
}

void cppServer::defaultWriteCompleteCallback(TcpConnection *conn)
{
    //
    LogTrace("write completed");
}

TcpConnection::TcpConnection(int connected_fd, EventLoop::ptr eventloop)
    : m_fd(connected_fd), m_eventloop(eventloop),
      m_connectionCallback(defaultConnectionCallback),
      m_messageCallback(defaultMessageCallback),
      m_writeCompleteCallback(defaultWriteCompleteCallback)
{

    int size = 1024;
    m_write_buffer = std::make_shared<TcpBuffer>(size);
    m_read_buffer = std::make_shared<TcpBuffer>(size);

    m_name = "connection-" + std::to_string(connected_fd);

    // add event read for the new connection
    m_channel = std::make_shared<Channel>(connected_fd, EVENT_READ, eventloop.get());
    // set callback functions
    m_channel->setReadCallback(
        std::bind(&TcpConnection::handleRead, this));
    m_channel->setWriteCallback(
        std::bind(&TcpConnection::handleWrite, this));
    m_channel->setCloseCallback(
        std::bind(&TcpConnection::handleClose, this));
    m_channel->setErrorCallback(
        std::bind(&TcpConnection::handleError, this));

    // connectionCompletedCallBack callback
    m_eventloop->add_channel_event(connected_fd, m_channel);
}

// return the num of char had writen
int TcpConnection::sendBuffer()
{
    int total_size = m_write_buffer->readAble();
    int read_index = m_write_buffer->readIndex();

    size_t nwrited = 0;
    size_t nleft = total_size;
    int fault = 0;
    // Try to send data to the socket first
    if (!m_channel->isWriteEventEnabled())
    {
        nwrited = write(m_channel->m_fd, &(m_write_buffer->m_buffer[read_index]), total_size);
        if (nwrited >= 0)
        {
            nleft = nleft - nwrited;
        }
        else
        {
            LogError("sendBuffer failed");
            nwrited = 0;
            if (errno != EWOULDBLOCK)
            {
                if (errno == EPIPE || errno == ECONNRESET)
                {
                    fault = 1;
                }
            }
        }
    }
    m_write_buffer->recycleRead(nwrited);
    // If there is any remaining data not sent, add a write event
    if (!fault && nleft > 0)
    {
        LogTrace("add a write event");
        if (!m_channel->isWriteEventEnabled())
        {
            m_channel->enableWriteEvent();
        }
    }
    return nwrited;
}

void TcpConnection::handleRead()
{

    // reads the data in the buffer, and you can process data in this callback function.
    int rt = m_read_buffer->readFromSocket(m_channel->m_fd);
    if (rt > 0)
    {
        LogDebug("TcpConnection->m_messageCallback in ......");
        LogDebug(KV(this->m_fd));
        m_messageCallback(this);
    }
    else
    {
        LogTrace("TcpConnection->handClose in ......");
        this->handleClose();
    }
    LogDebug("handleRead over ......");
}

// Write out the data in the sending buffer
void TcpConnection::handleWrite()
{
    // assertInSameThread(eventLoop);
    if (m_eventloop->m_owner_thread_id != pthread_self())
    {
        exit(-1);
    }

    ssize_t nwrited = this->sendBuffer();
    if (nwrited > 0)
    {
        // nwrited bytes had read
        m_write_buffer->recycleWrite(nwrited);

        // If the data is completely sent out, there is no need to continue
        if (m_write_buffer->readAble() == 0)
        {
            m_channel->disableWriteEvent();
        }

        // Call callback function
        m_writeCompleteCallback(this);
    }
}

void TcpConnection::handleClose()
{
    // TODO: here, use close to handle Fin, maybe some data is still in writebuffer.
    LogDebug("in handlecolse .... fd = " << m_fd);
    m_eventloop->remove_channel_event(m_channel->m_fd, m_channel);
    m_closeCallback(this);
    LogDebug("after m_closeCallback ...");
    close(m_channel->m_fd);
    // LogDebug("shared_from_this().use_count: " << shared_from_this().use_count());
}

void TcpConnection::handleError()
{
    // TODO:
}

void TcpConnection::shutDown()
{
    // TODO: there may be a channel with write event.
    if (shutdown(m_fd, SHUT_WR) < 0)
    {
        LogTrace("tcp_connection_shutdown failed, socket == " << m_fd);
    }
}

void TcpConnection::getIpAndPort(std::string &ip, std::string &port)
{
    struct sockaddr_in addrClient;
    int addrClientlen = sizeof(addrClient);

    // get the client ip and port from connnection.
    getpeername(m_fd, (sockaddr *)&addrClient, (socklen_t *)&addrClientlen);

    // convert ip and port to normal form.
    char client_ip[20];
    inet_ntop(AF_INET, &addrClient.sin_addr, client_ip, sizeof(client_ip));
    ip = client_ip;
    port = to_string(ntohs(addrClient.sin_port)); //将整形转成string
}