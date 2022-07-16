#include "common.h"
#include "event_loop.h"
#include "acceptor.h"
#include "tcp_connection.h"
#include "tcp_server.h"
#include "./service/myFunction.hpp"

// 连接建立之后的callback
int onConnectionCompleted(struct tcp_connection *tcpConnection)
{
    std::cout << "connection completed" << std::endl;
    return 0;
}

// 数据读到buffer之后的callback
int onMessage(struct buffer *input, struct tcp_connection *tcpConnection)
{
    std::cout << "get message from tcp connection " << tcpConnection->name << std::endl;

    // TODO:针对char*数据进行数据处理，完成对应的操作，再将对应数据发还给客户端
#if 1
    std::string recv_data;
    int _ = buffer_read_all(input, recv_data);

    std::string response;

    processContext(recv_data, response, tcpConnection->channel->fd);
    // std::cout << "response:" << response << ", len:" << response.length() << std::endl;

    struct buffer *output = buffer_new();
    int size = response.length();

    char *send_msg = new char[size];
    strcpy(send_msg, response.c_str());
    buffer_append_string(output, send_msg);

    delete[] send_msg;
    send_msg = nullptr;
#endif
    //将回复消息发还给客户端
    tcp_connection_send_buffer(tcpConnection, output);
    return 0;
}

//数据通过buffer写完之后的callback
int onWriteCompleted(struct tcp_connection *tcpConnection)
{
    std::cout << "write completed" << std::endl;
    return 0;
}

// 连接关闭之后的callback
int onConnectionClosed(struct tcp_connection *tcpConnection)
{
    std::cout << "connection closed" << std::endl;
    return 0;
}

int main(int argc, char **argv)
{
    std::cout << "This is a TCP-server Test!" << std::endl;
    //  主线程 event_loop
    struct event_loop *eventLoop = event_loop_init();

    //  初始化 acceptor
    struct acceptor *acceptor = acceptor_init(SERV_PORT);

    //初始化 tcp_server, 可以指定线程数目，这里线程是4，说明是一个acceptor线程，4个I/O线程
    // tcp_server自己带一个event_loop
    struct TCPserver *tcpServer = tcp_server_init(eventLoop, acceptor, onConnectionCompleted, onMessage,

                                                  onWriteCompleted, onConnectionClosed, 0);
    tcp_server_start(tcpServer);

    // main thread for acceptor
    event_loop_run(eventLoop);

    return 0;
}