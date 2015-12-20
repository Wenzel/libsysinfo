#include <iostream>
#include <sysinfo.h>
#include <sys/types.h>
#include <unistd.h>
#include <boost/algorithm/string/join.hpp>

int main(int argc, char *argv[])
{
    argc = argc;
    argv = argv;

    // get current pid
    pid_t pid = getpid();
    // get first process owned by the user
    struct process_info_t pinfo = getProcessDetail(pid);

    std::cout << "details for PID : " << pid << std::endl;
    std::cout << "fd : " << pinfo.fds.size() << std::endl;
    for (auto const &pair : pinfo.fds)
    {
        std::cout << pair.first << " -> " << pair.second << std::endl;
    }

    std::cout << "cgroup : " << pinfo.cgroups.size() << std::endl;
    for (struct cgroup_hierarchy_t cgroup : pinfo.cgroups)
        std::cout << cgroup.hierarchy_id << ", " << boost::algorithm::join(cgroup.subsystems, " ") << ", " << cgroup.cgroup << std::endl;

    std::cout << "maps : " << pinfo.maps.size() << std::endl;

    /*
    for (struct process_info_t pinfo : processList())
    {
        if (!pinfo.cmdline.empty())
        {
            for (std::string arg : pinfo.cmdline)
            {
                std::cout << arg << " ";
            }
            std::cout << std::endl;
            std::cout << pinfo.pid << std::endl;
        }
    }

    std::cout << "nbProcess " << processCount() << std::endl;
    std::cout << "nbCPU " << getNbCPUs() << std::endl;
    std::cout << "nbCores " << getNbCores() << std::endl;

    std::cout << "testing unix sockets" << std::endl;
    std::vector<struct unix_socket_t> unix_socket_list = getSocketUNIX();
    for (struct unix_socket_t unix_sock : unix_socket_list)
        std::cout << "path: " << unix_sock.path << std::endl;

    std::cout << "testing tcp sockets" << std::endl;
    std::vector<struct tcp_socket_t> tcp_socket_list = getSocketTCP();
    for (struct tcp_socket_t tcp_sock : tcp_socket_list)
    {
        std::cout << tcp_sock.local_address << ":" << tcp_sock.local_port << " " << tcp_sock.rem_address << ":" << tcp_sock.rem_port << std::endl;
    }
    */
}
