#include <iostream>
#include <sysinfo.h>

int main(int argc, char *argv[])
{
    for (ProcessInfo p : ProcessList())
    {
        if (!p.cmdline.empty())
        {
            for (std::string arg : p.cmdline)
            {
                std::cout << arg << " ";
            }
            std::cout << std::endl;
            std::cout << p.pid << std::endl;
        }
    }
    std::cout << "nbProcess " << processCount() << std::endl;
    std::cout << "nbCPU " << getNbCPUs() << std::endl;
    std::cout << "nbCores " << getNbCores() << std::endl;

    std::cout << "testing unix sockets" << std::endl;
    std::vector<unix_socket_t> unix_socket_list = getSocketUNIX();
    for (int i = 0; i < unix_socket_list.size(); i++)
        std::cout << "path: " << unix_socket_list[i].path << std::endl;

    std::cout << "testing tcp sockets" << std::endl;
    std::vector<tcp_socket_t> tcp_socket_list = getSocketTCP();
    for (int i = 0; i < tcp_socket_list.size(); i++)
    {
        std::cout << tcp_socket_list[i].local_address << ":" << tcp_socket_list[i].local_port << " " << tcp_socket_list[i].rem_address << ":" << tcp_socket_list[i].rem_port << std::endl;
    }
}
