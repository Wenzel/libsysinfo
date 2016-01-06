#ifndef PROCCONNECTOR_H
#define PROCCONNECTOR_H

#include <functional>
#include <vector>
#include <sys/socket.h>
#include <linux/cn_proc.h>
#include <linux/netlink.h>
#include <linux/connector.h>

class ProcConnector
{
public:
    ProcConnector();
    ~ProcConnector();

    void listen();
    void addCallback(std::function<void(struct proc_event)> callback);
private:
    void connect();
    void subscribe();

    void listenThread();


    int m_nl_sock;
    std::vector<std::function<void(struct proc_event)>> m_subscribers;
};

#endif // PROCCONNECTOR_H
