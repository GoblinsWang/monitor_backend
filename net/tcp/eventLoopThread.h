#ifndef net_tcp_EventLoopThread_h
#define net_tcp_EventLoopThread_h

#include "../common.h"
#include "eventLoop.h"

namespace cppServer
{

    class EventLoopThread
    {
    public:
        using ptr = std::shared_ptr<EventLoopThread>;
        // 线程初始化
        EventLoopThread(int num);
        // 线程启动
        void threadStart();
        // 线程执行的函数
        static void *threadRun(void *arg);

    public:
        EventLoop::ptr m_eventloop;
        long m_threadcount;     /* # connections handled */
        pthread_t m_thread_tid; /* thread ID */
        pthread_mutex_t m_mutex;
        pthread_cond_t m_cond;
        std::string m_threadname;
    };
}
#endif