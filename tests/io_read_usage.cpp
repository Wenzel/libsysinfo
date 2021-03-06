#include <iostream>
#include <sysinfo.h>
#include <sys/types.h>
#include <unistd.h>
#include <boost/algorithm/string/join.hpp>
#include <chrono>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "test_process_detail <PID>" << std::endl;
        return 1;
    }
    int pid = atoi(argv[1]);

    ProcessInfo pinfo(pid);
    while (1)
    {
        std::cout << "io total usage : " << pinfo.ioTotalUsage() << " B/s" << std::endl;
        pinfo.needUpdate();
        sleep(1);
    }
}
