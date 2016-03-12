#ifndef PROCCONNECTOR_H
#define PROCCONNECTOR_H

#include <functional>
#include <vector>
#include <thread>

#include <sys/socket.h>
#include <linux/cn_proc.h>
#include <linux/netlink.h>
#include <linux/connector.h>

class ProcConnector
{
public:
    ProcConnector();
    ~ProcConnector();

    void listen(bool block = false);
    void addCallback(std::function<void(struct proc_event)> callback);
private:
    void connect();
    void subscribe();
    void processEvent();
    void listenBlock();

    int m_nl_sock;
    std::thread* m_thread_listen;
    std::vector<std::function<void(struct proc_event)>> m_subscribers;
};

#endif // PROCCONNECTOR_H
