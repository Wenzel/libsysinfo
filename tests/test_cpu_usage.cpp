#include <iostream>
#include <sysinfo.h>
#include <sys/types.h>
#include <unistd.h>
#include <boost/algorithm/string/join.hpp>

int main()
{
    ProcessInfo pinfo(17897);
    while (1)
    {
        std::cout << "cpu_usage : " << pinfo.cpuUsage() <<  std::endl;
        pinfo.needUpdate();
        sleep(1);
    }
}
