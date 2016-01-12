#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include <iostream>

#include <sys/types.h>

class ProcessInfo
{

public:
    ProcessInfo(pid_t pid);

private:
    friend std::ostream & operator<<(std::ostream &os, const ProcessInfo& p);

};

#endif // PROCESSINFO_H
