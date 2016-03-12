#include <functional>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <sys/sysinfo.h>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "sysinfo.h"

// Public API

// System info
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
        boost::algorithm::trim(key);
        // read value
        std::string value;
        boost::algorithm::trim(value);
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

// process

std::vector<int> processListPid()
{
    std::vector<int> process_pid_list;
    boost::filesystem::path proc_path("/proc");
    boost::filesystem::directory_iterator end_itr;
    for (boost::filesystem::directory_iterator itr(proc_path);
         itr != end_itr;
         ++itr)
    {
        if (boost::filesystem::is_directory(itr->status())
                && boost::regex_match(itr->path().filename().string(), boost::regex("\\d+")))
        {
            int pid = std::stoi(itr->path().filename().string());
            process_pid_list.push_back(pid);
        }
    }
    return process_pid_list;
}

int processCount()
{
    return processListPid().size();
}

std::vector<ProcessInfo> processList()
{
    std::vector<ProcessInfo> process_list;
    std::vector<int> process_pid_list = processListPid();

    for (int pid : process_pid_list)
    {
        try {
            ProcessInfo pinfo(pid);
            process_list.push_back(pinfo);
        } catch (std::string e)
        {
            continue;
        }

    }
    return process_list;
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
            boost::regex regex("^([[:xdigit:]]+):\\s([[:digit:]]+)\\s([[:digit:]]+)\\s([[:digit:]]+)\\s([[:digit:]]+)\\s([[:digit:]]+)\\s([[:digit:]]+)\\s(.*)$");
            boost::smatch match;
            if (boost::regex_match(line, match, regex))
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
                    unix_socket_list.push_back(socket);
                }
            }
        }
    }
    if_unix.close();

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
            boost::regex regex("^\\s+([[:digit:]]+):\\s([[:xdigit:]]+):([[:xdigit:]]+)\\s([[:xdigit:]]+):([[:xdigit:]]+).*$");
            boost::smatch match;
            if (boost::regex_match(line, match, regex))
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
    if_tcp.close();

    return tcp_socket_list;
}
