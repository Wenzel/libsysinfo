#include <iostream>
#include <sys/types.h>
#include <unistd.h>

#include <sysinfo.h>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "test_process_detail <PID>" << std::endl;
        return 1;
    }

    int pid = atoi(argv[1]);
    ProcessInfo pinfo(pid);
    std::cout << pinfo << std::endl;

    return 0;
}
