#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <string>

#include "procconnector.h"

ProcConnector::ProcConnector()
{
    connect();
    subscribe();
}

ProcConnector::~ProcConnector()
{
    close(m_nl_sock);
}

void ProcConnector::connect()
{
    int rc;

    // create socket
    m_nl_sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);
    if (m_nl_sock == -1) {
        throw std::string(strerror(errno));
    }

    struct sockaddr_nl sa_nl;
    // fill sockaddr_nl
    sa_nl.nl_family = AF_NETLINK;
    sa_nl.nl_groups = CN_IDX_PROC;
    sa_nl.nl_pid = getpid();

    // bind
    rc = bind(m_nl_sock, (struct sockaddr *)&sa_nl, sizeof(sa_nl));
    if (rc == -1) {
        close(m_nl_sock);
        throw std::string(strerror(errno));
    }
}

void ProcConnector::subscribe()
{
    int rc;
    struct __attribute__ ((aligned(NLMSG_ALIGNTO))) {
        struct nlmsghdr nl_hdr;
        struct __attribute__ ((__packed__)) {
            struct cn_msg cn_msg;
            enum proc_cn_mcast_op cn_mcast;
        };
    } nlcn_msg;

    memset(&nlcn_msg, 0, sizeof(nlcn_msg));
    nlcn_msg.nl_hdr.nlmsg_len = sizeof(nlcn_msg);
    nlcn_msg.nl_hdr.nlmsg_pid = getpid();
    nlcn_msg.nl_hdr.nlmsg_type = NLMSG_DONE;

    nlcn_msg.cn_msg.id.idx = CN_IDX_PROC;
    nlcn_msg.cn_msg.id.val = CN_VAL_PROC;
    nlcn_msg.cn_msg.len = sizeof(enum proc_cn_mcast_op);

    nlcn_msg.cn_mcast = PROC_CN_MCAST_LISTEN; // PROC_CN_MCAST_IGNORE;

    rc = send(m_nl_sock, &nlcn_msg, sizeof(nlcn_msg), 0);
    if (rc == -1) {
        throw std::string(strerror(errno));
    }
}

void ProcConnector::addCallback(std::function<void(struct proc_event)> callback)
{
    m_subscribers.push_back(callback);
}

void ProcConnector::listen()
{
    int rc;
    struct __attribute__ ((aligned(NLMSG_ALIGNTO))) {
        struct nlmsghdr nl_hdr;
        struct __attribute__ ((__packed__)) {
            struct cn_msg cn_msg;
            struct proc_event proc_ev;
        };
    } nlcn_msg;

    while (1) {
        rc = recv(m_nl_sock, &nlcn_msg, sizeof(nlcn_msg), 0);
        if (rc == -1) {
            if (errno == EINTR)
                continue;
            else
            {
                throw std::string(std::strerror(errno));
                return;
            }
        }
        for (std::function<void(struct proc_event)> callback: m_subscribers)
            callback(nlcn_msg.proc_ev);
    }
}
