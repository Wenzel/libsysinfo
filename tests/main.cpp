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
            std::cout << std::endl << std::endl;
        }
    }
    std::cout << "nbProcess " << processCount() << std::endl;
    std::cout << "nbCPU " << getNbCPUs() << std::endl;
    std::cout << "nbCores " << getNbCores() << std::endl;

}
