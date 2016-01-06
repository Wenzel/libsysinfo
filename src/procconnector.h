#ifndef PROCCONNECTOR_H
#define PROCCONNECTOR_H

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

    void listen();
private:
    void connect();
    void subscribe();
    void listenThread();


    int m_nl_sock;
    std::thread* m_listen_thread;
};

#endif // PROCCONNECTOR_H
