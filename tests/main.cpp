#include <iostream>
#include <sysinfo.h>

int main(int argc, char *argv[])
{
    std::cout << "nbCPU " << getNbCPUs() << std::endl;
    std::cout << "nbCores " << getNbCores() << std::endl;
}
