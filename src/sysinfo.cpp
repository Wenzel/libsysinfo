#include <algorithm>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "sysinfo.h"

// Private


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

static std::vector<int> getProcessList()
{
    std::vector<int> process_pid_list;
    boost::filesystem::path proc_path("/proc");
    boost::filesystem::directory_iterator end_itr;
    for (boost::filesystem::directory_iterator itr(proc_path);
         itr != end_itr;
         ++itr)
    {
        if (boost::filesystem::is_directory(itr->status())
                && std::regex_match(itr->path().filename().string(), std::regex("\\d+")))
        {
            int pid = std::stoi(itr->path().filename().string());
            process_pid_list.push_back(pid);
        }
    }
    return process_pid_list;
}

static void getProcess(int pid, struct process_info_t* pinfo)
{
    std::string process_path = "/proc/" + std::to_string(pid) + "/";

    // read cmdline
    std::ifstream if_cmdline(process_path + "cmdline");
    std::string cur_arg;
    while (getline(if_cmdline, cur_arg, '\0'))
        pinfo->cmdline.push_back(cur_arg);
    if_cmdline.close();


    // read cwd
    // cwd is a symlink to the process's current working directory
    boost::system::error_code ec;
    pinfo->cwd = boost::filesystem::read_symlink(process_path + "cwd", ec).string();

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
        pinfo->env[key] = value;
    }
    if_environ.close();

    // read exe
    // this file is a symbolic link containing the actual
    // pathname of the executed command
    pinfo->exe = boost::filesystem::read_symlink(process_path + "exe", ec).string();

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
            pinfo->io.rchar = std::stol(value);
        else if (key == "wchar")
            pinfo->io.wchar = sleeping; // TODO
        else if (key == "syscr")
            pinfo->io.syscr = std::stol(value);
        else if (key == "syscw")
            pinfo->io.syscw = std::stol(value);
        else if (key == "read_bytes")
            pinfo->io.read_bytes = std::stol(value);
        else if (key == "write_bytes")
            pinfo->io.write_bytes = std::stol(value);
        else if (key == "cancelled_write_bytes")
            pinfo->io.cancelled_write_bytes = std::stol(value);
    }
    if_io.close();

    // read root
    // root is a symlink to the process's filesystem root (chroot)
    pinfo->root = boost::filesystem::read_symlink(process_path + "root", ec).string();

    // read stat
    std::ifstream if_stat(process_path + "stat");
    // pid
    if_stat >> pinfo->pid;
    // name
    if_stat >> pinfo->name; // (name)
    pinfo->name.erase(pinfo->name.begin()); // remove parenthesis
    pinfo->name.pop_back();
    // status
    std::string status;
    if_stat >> status;
    if (status == "R")
        pinfo->status = running;
    else if (status == "S")
        pinfo->status = sleeping;
    else if (status == "D")
        pinfo->status = disk_sleep;
    else if (status == "Z")
        pinfo->status = zombie;
    else if (status == "T")
        pinfo->status = stopped;
    else if (status == "t")
        pinfo->status = tracing_stop;
    else if (status == "W")
        pinfo->status = waking;
    else if (status == "X" or status == "x")
        pinfo->status = dead;
    else if (status == "K")
        pinfo->status = wakekill;
    else if (status == "P")
        pinfo->status = parked;

    if_stat >> pinfo->ppid;
    if_stat >> pinfo->pgrp;
    if_stat >> pinfo->session;
    if_stat >> pinfo->tty_nr;
    if_stat >> pinfo->tpgid;
    if_stat >> pinfo->flags;
    if_stat >> pinfo->minflt;
    if_stat >> pinfo->cminflt;
    if_stat >> pinfo->cmajflt;
    if_stat >> pinfo->utime;
    if_stat >> pinfo->stime;
    if_stat >> pinfo->cutime;
    if_stat >> pinfo->cstime;
    if_stat >> pinfo->priority;
    if_stat >> pinfo->nice;
    if_stat >> pinfo->num_threads;
    if_stat >> pinfo->itrealvalue;
    if_stat >> pinfo->starttime;
    if_stat >> pinfo->vsize;
    if_stat >> pinfo->rss;
    if_stat >> pinfo->rsslim;
    if_stat >> pinfo->startcode;
    if_stat >> pinfo->endcode;
    if_stat >> pinfo->startstack;
    if_stat >> pinfo->kstkesp;
    if_stat >> pinfo->kstkeip;
    if_stat >> pinfo->signal;
    if_stat >> pinfo->blocked;
    if_stat >> pinfo->siginore;
    if_stat >> pinfo->sigcatch;
    if_stat >> pinfo->wchan;
    if_stat >> pinfo->nswap;
    if_stat >> pinfo->cnswap;
    if_stat >> pinfo->exit_signal;
    if_stat >> pinfo->processor;
    if_stat >> pinfo->rt_priority;
    if_stat >> pinfo->policy;
    if_stat >> pinfo->delayacct_blkio_ticks;
    if_stat >> pinfo->guest_time;
    if_stat >> pinfo->cguest_time;
    if_stat >> pinfo->start_data;
    if_stat >> pinfo->end_data;
    if_stat >> pinfo->start_brk;
    if_stat >> pinfo->arg_start;
    if_stat >> pinfo->arg_end;
    if_stat >> pinfo->env_start;
    if_stat >> pinfo->env_end;
    if_stat >> pinfo->exit_code;

    if_stat.close();
}

// Public API

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
        else if (key == "bogomipinfo")
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
    return getProcessList().size();

}

std::vector<struct process_info_t> processList()
{
    std::vector<struct process_info_t> process_list;
    std::vector<int> process_pid_list = getProcessList();

    for (int pid : process_pid_list)
    {
        try {
            struct process_info_t pinfo;
            getProcess(pid, &pinfo);
            process_list.push_back(pinfo);
        } catch (std::string e)
        {
            continue;
        }

    }
    return process_list;
}

struct process_info_t getProcessDetail(pid_t pid)
{
    struct process_info_t pinfo;
    getProcess(pid, &pinfo);

    std::string process_path("/proc/" + std::to_string(pid));
    // read fd/*
    boost::filesystem::path fd_dir_path(process_path + "/fd");
    boost::filesystem::directory_iterator end_itr;
    for (boost::filesystem::directory_iterator itr(fd_dir_path);
         itr != end_itr;
         ++itr)
    {
        boost::system::error_code ec;
        boost::filesystem::path symlink = boost::filesystem::read_symlink(itr->path(), ec);
        pinfo.fds[std::stoi(itr->path().filename().string())] = symlink.string();
    }

    // TODO attr/*
    // cgroup
    std::ifstream if_cgroup(process_path + "/cgroup");
    if (if_cgroup.is_open())
    {
        // sample line :
        // 5:cpuacct,cpu,cpuset:/daemons
        std::string line;
        while (std::getline(if_cgroup, line))
        {
            struct cgroup_hierarchy_t cgroup;

            std::vector<std::string> splitted;
            boost::split(splitted, line, boost::is_any_of(":"));
            cgroup.hierarchy_id = std::stoi(splitted[0]);
            boost::split(cgroup.subsystems, splitted[1], boost::is_any_of(","));
            cgroup.cgroup = splitted[2];

            pinfo.cgroups.push_back(cgroup);
        }
    }

    // read maps
    std::ifstream if_maps(process_path + "/maps");
    if (if_maps.is_open())
    {
        // line sample
        // 00400000-00452000 r-xp 00000000 08:02 173521      /usr/bin/dbus-daemon
        std::string line;
        while (std::getline(if_maps, line))
        {
            std::regex regex("^([[:xdigit:]]+)-([[:xdigit:]]+)\\s([r-])([w-])([x-])([sp])\\s([[:xdigit:]]+)\\s([[:digit:]]+):([[:digit:]]+)\\s([[:digit:]]+)\\s+(.*)$");
            std::smatch match;
            if (std::regex_match(line, match, regex))
            {
                if (match.size() == 11 + 1)
                {
                    struct memory_mapping_t region;
                    region.address_from = match[1];
                    region.address_to = match[2];
                    (match[3] == "r") ? region.perm_read = true : region.perm_read = false;
                    (match[4] == "w") ? region.perm_write = true : region.perm_write = false;
                    (match[5] == "x") ? region.perm_execute = true : region.perm_execute = false;
                    (match[6] == "p") ? region.type = priv : region.type = shared;
                    region.offset = std::stol(match[7]);
                    region.dev_major = std::stoi(match[8]);
                    region.dev_minor = std::stoi(match[9]);
                    region.inode = std::stol(match[10]);
                    region.pathname = match[11];

                    pinfo.maps.push_back(region);
                }
            }
        }
    }

    return pinfo;
}


// Network
std::vector<struct unix_socket_t> getSocketUNIX()
{
    std::vector<struct unix_socket_t> unix_socket_list;
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
                struct unix_socket_t socket;
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

std::vector<struct tcp_socket_t> getSocketTCP()
{
    std::vector<struct tcp_socket_t> tcp_socket_list;
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
                struct tcp_socket_t socket;
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
