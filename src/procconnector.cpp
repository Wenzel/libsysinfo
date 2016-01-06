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

void ProcConnector::listen()
{
    m_listen_thread = new std::thread(&ProcConnector::listenThread, this);
}

void ProcConnector::listenThread()
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
        if (rc == 0) {
            /* shutdown? */
            return;
        } else if (rc == -1) {
            if (errno == EINTR)
                continue;
            else
            {
                throw std::string(std::strerror(errno));
                return;
            }
        }
        switch (nlcn_msg.proc_ev.what) {
        case nlcn_msg.proc_ev.PROC_EVENT_NONE:
            printf("set mcast listen ok\n");
            break;
        case nlcn_msg.proc_ev.PROC_EVENT_FORK:
            printf("fork: parent tid=%d pid=%d -> child tid=%d pid=%d\n",
                   nlcn_msg.proc_ev.event_data.fork.parent_pid,
                   nlcn_msg.proc_ev.event_data.fork.parent_tgid,
                   nlcn_msg.proc_ev.event_data.fork.child_pid,
                   nlcn_msg.proc_ev.event_data.fork.child_tgid);
            break;
        case nlcn_msg.proc_ev.PROC_EVENT_EXEC:
            printf("exec: tid=%d pid=%d\n",
                   nlcn_msg.proc_ev.event_data.exec.process_pid,
                   nlcn_msg.proc_ev.event_data.exec.process_tgid);
            break;
        case nlcn_msg.proc_ev.PROC_EVENT_UID:
            printf("uid change: tid=%d pid=%d from %d to %d\n",
                   nlcn_msg.proc_ev.event_data.id.process_pid,
                   nlcn_msg.proc_ev.event_data.id.process_tgid,
                   nlcn_msg.proc_ev.event_data.id.r.ruid,
                   nlcn_msg.proc_ev.event_data.id.e.euid);
            break;
        case nlcn_msg.proc_ev.PROC_EVENT_GID:
            printf("gid change: tid=%d pid=%d from %d to %d\n",
                   nlcn_msg.proc_ev.event_data.id.process_pid,
                   nlcn_msg.proc_ev.event_data.id.process_tgid,
                   nlcn_msg.proc_ev.event_data.id.r.rgid,
                   nlcn_msg.proc_ev.event_data.id.e.egid);
            break;
        case nlcn_msg.proc_ev.PROC_EVENT_EXIT:
            printf("exit: tid=%d pid=%d exit_code=%d\n",
                   nlcn_msg.proc_ev.event_data.exit.process_pid,
                   nlcn_msg.proc_ev.event_data.exit.process_tgid,
                   nlcn_msg.proc_ev.event_data.exit.exit_code);
            break;
        default:
            printf("unhandled proc event\n");
            break;
        }
    }
}
