#include <algorithm>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>

#include "sysinfo.h"

// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<cpu_info_t> readCPUInfo()
{
    std::vector<cpu_info_t> cpuinfo = std::vector<cpu_info_t>();
    // open /proc/cpuinfo
    std::ifstream if_cpuinfo("/proc/cpuinfo");

    // read until end
    std::string cur_line;
    // add first cpu_info_t
    cpu_info_t cur_cpuinfo = cpu_info_t();
    // for each line
    while (std::getline(if_cpuinfo, cur_line))
    {
        if (cur_line.empty()) // blank line separator
        {
            cpuinfo.push_back(cur_cpuinfo);
            if (!if_cpuinfo.eof())
                cur_cpuinfo = cpu_info_t();
            continue;
        }
        std::istringstream line_stream(cur_line);
        // read key
        std::string key;
        std::getline(line_stream, key, ':');
        key = trim(key);
        // read value
        std::string value;
        value = trim(value);
        std::getline(line_stream, value);

        if (key == "processor")
            cur_cpuinfo.processor = std::stoi(value);
        else if (key == "vendor_id")
            cur_cpuinfo.vendor_id = value;
        else if (key == "cpu family")
            cur_cpuinfo.cpu_family = std::stoi(value);
        else if (key == "model")
            cur_cpuinfo.model = std::stoi(value);
        else if (key == "model name")
            cur_cpuinfo.model_name = value;
        else if (key == "stepping")
            cur_cpuinfo.stepping = std::stoi(value);
        else if (key == "microcode")
            cur_cpuinfo.microcode = std::stoi(value);
        else if (key == "cpu MHz")
            cur_cpuinfo.cpu_mhz = std::stol(value);
        else if (key == "cache size")
            cur_cpuinfo.cache_size = 0; // todo KB, MB
        else if (key == "physical id")
            cur_cpuinfo.physical_id = std::stoi(value);
        else if (key == "sibblings")
            cur_cpuinfo.siblings = std::stoi(value);
        else if (key == "core id")
            cur_cpuinfo.core_id = std::stoi(value);
        else if (key == "cpu cores")
            cur_cpuinfo.cpu_cores = std::stoi(value);
        else if (key == "apicid")
            cur_cpuinfo.apicid = std::stoi(value);
        else if (key == "initial apicid")
            cur_cpuinfo.initial_apicid = std::stoi(value);
        else if (key == "fpu")
            cur_cpuinfo.fpu = ((value == "yes") ? true : false);
        else if (key == "fpu_exception")
            cur_cpuinfo.fpu_exception = ((value == "yes") ? true : false);
        else if (key == "cpuid_level")
            cur_cpuinfo.cpuid_level = std::stoi(value);
        else if (key == "wp")
            cur_cpuinfo.wp = ((value == "yes") ? true : false);
        else if (key == "flags")
        {
            std::istringstream flags_stream(value);
            std::string cur_flag;
            while (flags_stream >> cur_flag)
                cur_cpuinfo.flags.push_back(cur_flag);
        }
        else if (key == "bugs")
            cur_cpuinfo.bugs = value;
        else if (key == "bogomips")
            cur_cpuinfo.bogomips = std::stol(value);
        else if (key == "clflush size")
            cur_cpuinfo.clflush_size = std::stoi(value);
        else if (key == "cache_alignement")
            cur_cpuinfo.cache_alignement = std::stoi(value);
        else if (key == "address sizes")
            cur_cpuinfo.address_sizes = value;
        else if (key == "power management")
            cur_cpuinfo.power_management = value;
    }


    // close
    if_cpuinfo.close();

    // return data
    return cpuinfo;
}

int getNbCPUs()
{
    std::vector<cpu_info_t> cpuinfo = readCPUInfo();

    int nbCPUs = 0;
    for (cpu_info_t core : cpuinfo)
    {
        if (core.core_id == 0) // new processor
            nbCPUs++;
    }

    return nbCPUs;
}

int getNbCores()
{
    std::vector<cpu_info_t> cpuinfo = readCPUInfo();

    return cpuinfo.size();
}

int processCount()
{
    int nbProcess = 0;

    DIR* proc;
    if ((proc = opendir("/proc")) != NULL)
    {
        struct dirent* ent;
        while ((ent = readdir(proc)) != NULL)
        {
            struct stat fileinfo;
            std::string entry_path = "/proc/" + std::string(ent->d_name);
            if (stat(entry_path.c_str(), &fileinfo) == 0 && S_ISDIR(fileinfo.st_mode))
                nbProcess++;
        }
    }
    return nbProcess;
}

void getProcess(int pid, ProcessInfo* ps)
{
    std::string process_path = "/proc/" + std::to_string(pid) + "/";

    // read cmdline
    std::ifstream if_cmdline(process_path + "cmdline");
    std::string cur_arg;
    while (getline(if_cmdline, cur_arg, '\0'))
        ps->cmdline.push_back(cur_arg);
    if_cmdline.close();


    // read cwd
    // cwd is a symlink to the process's current working directory
    char buff[PATH_MAX];
    std::string cwd_path = process_path + "/cwd";
    ssize_t len = readlink(cwd_path.c_str(), buff, sizeof(buff) - 1);
    if (len == -1)
        throw std::string("Exception while reading /proc/<pid>/cwd");
    ps->cwd = std::string(buff);

    // read environ
    std::ifstream if_environ(process_path + "environ");
    std::string cur_env;
    while (getline(if_environ, cur_env, '\0'))
    {
        std::istringstream stream(cur_env);
        std::string key;
        std::string value;
        // split on '='
        getline(stream, key, '=');
        getline(stream, value);
        ps->env[key] = value;
    }
    if_environ.close();

    // read exe
    // this file is a symbolic link containing the actual
    // pathname of the executed command
    std::string exe_path = process_path + "/exe";
    len = readlink(exe_path.c_str(), buff, sizeof(buff) - 1);
    if (len == -1)
        throw std::string("Exception while reading /proc/<pid>/exe");

    ps->exe = std::string(buff);

    // read io
    std::ifstream if_io(process_path + "io");
    std::string line;
    while (getline(if_io, line))
    {
        std::string key;
        std::string value;
        // parse key: value
        std::istringstream stream(line);
        getline(stream, key, ':');
        getline(stream, value);
        value = trim(value);
        if (key == "rchar")
            ps->io.rchar = std::stol(value);
        else if (key == "wchar")
            ps->io.wchar = sleeping; // TODO
        else if (key == "syscr")
            ps->io.syscr = std::stol(value);
        else if (key == "syscw")
            ps->io.syscw = std::stol(value);
        else if (key == "read_bytes")
            ps->io.read_bytes = std::stol(value);
        else if (key == "write_bytes")
            ps->io.write_bytes = std::stol(value);
        else if (key == "cancelled_write_bytes")
            ps->io.cancelled_write_bytes = std::stol(value);
    }
    if_io.close();

    // read root
    // root is a symlink to the process's filesystem root (chroot)
    std::string root_path = process_path + "/root";
    len = readlink(root_path.c_str(), buff, sizeof(buff) - 1);
    if (len == -1)
        throw std::string("Exception while reading /proc/<pid>/root");

    ps->root = std::string(buff);

    // read stat
    std::ifstream if_stat(process_path + "stat");
    // pid
    if_stat >> ps->pid;
    // name
    if_stat >> ps->name; // (name)
    ps->name.erase(ps->name.begin()); // remove parenthesis
    ps->name.pop_back();
    // status
    std::string status;
    if_stat >> status;
    if (status == "R")
        ps->status = running;
    else if (status == "S")
        ps->status = sleeping;
    else if (status == "D")
        ps->status = disk_sleep;
    else if (status == "Z")
        ps->status = zombie;
    else if (status == "T")
        ps->status = stopped;
    else if (status == "t")
        ps->status = tracing_stop;
    else if (status == "W")
        ps->status = waking;
    else if (status == "X" or status == "x")
        ps->status = dead;
    else if (status == "K")
        ps->status = wakekill;
    else if (status == "P")
        ps->status = parked;

    if_stat >> ps->ppid;
    if_stat >> ps->pgrp;
    if_stat >> ps->session;
    if_stat >> ps->tty_nr;
    if_stat >> ps->tpgid;
    if_stat >> ps->flags;
    if_stat >> ps->minflt;
    if_stat >> ps->cminflt;
    if_stat >> ps->cmajflt;
    if_stat >> ps->utime;
    if_stat >> ps->stime;
    if_stat >> ps->cutime;
    if_stat >> ps->cstime;
    if_stat >> ps->priority;
    if_stat >> ps->nice;
    if_stat >> ps->num_threads;
    if_stat >> ps->itrealvalue;
    if_stat >> ps->starttime;
    if_stat >> ps->vsize;
    if_stat >> ps->rss;
    if_stat >> ps->rsslim;
    if_stat >> ps->startcode;
    if_stat >> ps->endcode;
    if_stat >> ps->startstack;
    if_stat >> ps->kstkesp;
    if_stat >> ps->kstkeip;
    if_stat >> ps->signal;
    if_stat >> ps->blocked;
    if_stat >> ps->siginore;
    if_stat >> ps->sigcatch;
    if_stat >> ps->wchan;
    if_stat >> ps->nswap;
    if_stat >> ps->cnswap;
    if_stat >> ps->exit_signal;
    if_stat >> ps->processor;
    if_stat >> ps->rt_priority;
    if_stat >> ps->policy;
    if_stat >> ps->delayacct_blkio_ticks;
    if_stat >> ps->guest_time;
    if_stat >> ps->cguest_time;
    if_stat >> ps->start_data;
    if_stat >> ps->end_data;
    if_stat >> ps->start_brk;
    if_stat >> ps->arg_start;
    if_stat >> ps->arg_end;
    if_stat >> ps->env_start;
    if_stat >> ps->env_end;
    if_stat >> ps->exit_code;

    if_stat.close();
}

std::vector<ProcessInfo> ProcessList()
{
    std::vector<ProcessInfo> process_list;
    DIR* proc = NULL;
    if ((proc = opendir("/proc")) != NULL)
    {
        struct dirent* ent = NULL;
        while ((ent = readdir(proc)) != NULL)
        {
            struct stat fileinfo;
            std::string entry_path = "/proc/" + std::string(ent->d_name);
            if (stat(entry_path.c_str(), &fileinfo) == 0)
            {
                // if is_directory and is_digit
                if (S_ISDIR(fileinfo.st_mode) && std::regex_match(ent->d_name, std::regex("\\d+")))
                {
                    int pid = std::stoi(ent->d_name);
                    ProcessInfo ps;
                    try
                    {
                        getProcess(pid, &ps);
                    }
                    catch (std::string& e)
                    {
                        continue;
                    }
                    process_list.push_back(ps);
                }
            }
        }
    }
    return process_list;
}

// Network
std::vector<unix_socket_t> getSocketUNIX()
{
    std::vector<unix_socket_t> unix_socket_list;
    std::ifstream if_unix("/proc/net/unix");
    if (if_unix.is_open())
    {
        // skip first line
        // Num       RefCount Protocol Flags    Type St Inode Path
        std::string line;
        while (getline(if_unix, line))
        {
            // line sample
            // ffff8800c6110000: 00000002 00000000 00010000 0001 01 29692 @/tmp/.ICE-unix/3006
            std::regex regex("^([[:xdigit:]]+):\\s([[:digit:]]+)\\s([[:digit:]]+)\\s([[:digit:]]+)\\s([[:digit:]]+)\\s([[:digit:]]+)\\s([[:digit:]]+)\\s(.*)$");
            std::smatch match;
            if (std::regex_match(line, match, regex))
            {
                unix_socket_t socket;
                if (match.size() == 8 + 1) // first match represents whole line
                {
                    socket.num = match[1]; // TODO xdigit
                    socket.ref_count = std::stoi(match[2], 0, 16);
                    socket.protocol = std::stoi(match[3]);
                    socket.flags = std::stoi(match[4]);
                    socket.type = std::stoi(match[5]);
                    socket.state = static_cast<enum socket_state>(std::stoi(match[6]));
                    socket.inode = std::stoi(match[7]);
                    socket.path = match[8];
                    std::cout << socket.path << std::endl;
                    unix_socket_list.push_back(socket);
                }
            }
        }
    }
    return unix_socket_list;
}

std::vector<tcp_socket_t> getSocketTCP()
{
    std::vector<tcp_socket_t> tcp_socket_list;
    std::ifstream if_tcp("/proc/net/tcp");
    if (if_tcp.is_open())
    {
        // skip first line
        //   sl  local_address rem_address   st tx_queue rx_queue tr tm->when retrnsmt   uid  timeout inode
        std::string line;
        while (getline(if_tcp, line))
        {
            // line sample
            //   0: 0ABCDEF:0035 00000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 29706 1 ffff8800c63f8000 100 0 0 10 0
            std::regex regex("^\\s+([[:digit:]]+):\\s([[:xdigit:]]+):([[:xdigit:]]+)\\s([[:xdigit:]]+):([[:xdigit:]]+).*$");
            std::smatch match;
            if (std::regex_match(line, match, regex))
            {
                tcp_socket_t socket;
                if (match.size() == 5 + 1) // first match represents whole line
                {
                    struct in_addr addr;

                    socket.num = std::stoi(match[1]);
                    addr.s_addr = htonl(std::stol(match[2], 0, 16));
                    socket.local_address = std::string(inet_ntoa(addr));
                    socket.local_port = std::stoi(match[3], 0, 16);

                    addr.s_addr = htonl(std::stol(match[4], 0, 16));
                    socket.rem_address = std::string(inet_ntoa(addr));
                    socket.rem_port = std::stoi(match[5], 0, 16);

                    tcp_socket_list.push_back(socket);
                }
            }
        }
    }
    return tcp_socket_list;
}
