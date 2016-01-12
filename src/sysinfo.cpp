#include <functional>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <sys/sysinfo.h>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "sysinfo.h"

// local variables
static ProcConnector* connector;
static std::thread* event_thread;

// Private

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
                && boost::regex_match(itr->path().filename().string(), boost::regex("\\d+")))
        {
            int pid = std::stoi(itr->path().filename().string());
            process_pid_list.push_back(pid);
        }
    }
    return process_pid_list;
}

// Public API


// init
void sysinfoInit()
{
    connector = new ProcConnector();
}

void startProcessEventListening()
{
    event_thread = new std::thread(&ProcConnector::listen, connector);
}

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

int processCount()
{
    return getProcessList().size();
}

std::vector<ProcessInfo> processList()
{
    std::vector<ProcessInfo> process_list;
    std::vector<int> process_pid_list = getProcessList();

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

// TODO
/*
struct process_info_t processDetail(pid_t pid)
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
    if_cgroup.close();

    // read smaps
    std::ifstream if_smaps(process_path + "/smaps");
    if (if_smaps.is_open())
    {
        // line sample
        // 00400000-00452000 r-xp 00000000 08:02 173521      /usr/bin/dbus-daemon
        // and
        // Shared_Clean:          0 kB
        std::string line;
        while (std::getline(if_smaps, line))
        {
            struct memory_mapping_t region;
            boost::regex regex_declare_mapping("^([[:xdigit:]]+)-([[:xdigit:]]+)\\s([r-])([w-])([x-])([sp])\\s([[:xdigit:]]+)\\s([[:digit:]]+):([[:digit:]]+)\\s([[:digit:]]+)\\s+(.*)$");
            boost::regex regex_key_value("^([[:alpha:]]+):\\s+(.*)$");
            boost::smatch match;
            if (boost::regex_match(line, match, regex_declare_mapping))
            {
                if (match.size() == 11 + 1)
                {
                    pinfo.maps.push_back(memory_mapping_t());
                    region = pinfo.maps.back();

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
                }
            } else if (boost::regex_match(line, match, regex_key_value))
            {
                if (match.size() == 2 + 1)
                {
                    std::string value(match[2]);
                    boost::algorithm::trim(value);
                    boost::regex regex_value("^([[:digit:]])\\s.*$");
                    boost::smatch match_value;
                    if (boost::regex_match(value, match_value, regex_value))
                    {
                        // 4 kB
                        if (match[1] == "Size")
                            region.size = std::stoi(match_value[1]);
                        else if (match[1] == "Rss")
                            region.rss = std::stoi(match_value[1]);
                        else if (match[1] == "Pss")
                            region.pss = std::stoi(match_value[1]);
                        else if (match[1] == "Shared_Clean")
                            region.shared_clean = std::stoi(match_value[1]);
                        else if (match[1] == "Shared_Dirty")
                            region.shared_dirty = std::stoi(match_value[1]);
                        else if (match[1] == "Private_Clean")
                            region.private_clean = std::stoi(match_value[1]);
                        else if (match[1] == "Private_Dirty")
                            region.private_dirty = std::stoi(match_value[1]);
                        else if (match[1] == "Referenced")
                            region.referenced = std::stoi(match_value[1]);
                        else if (match[1] == "Anonymous")
                            region.anonymous = std::stoi(match_value[1]);
                        else if (match[1] == "AnonHugePages")
                            region.anonhugepages = std::stoi(match_value[1]);
                        else if (match[1] == "Swap")
                            region.swap = std::stoi(match_value[1]);
                        else if (match[1] == "KernelPageSize")
                            region.kernelpagesize = std::stoi(match_value[1]);
                        else if (match[1] == "MMUPageSize")
                            region.mmupagesize = std::stoi(match_value[1]);
                        else if (match[1] == "Locked")
                            region.locked = std::stoi(match_value[1]);

                    } else
                    {
                        // VmFlags special case
                        boost::split(region.vmflags, value, boost::is_any_of(" "));
                    }
                }
            }

        }
    }
    if_smaps.close();

    // read limits
    std::ifstream if_limits(process_path + "/limits");
    if (if_limits.is_open())
    {
        // line sample
        // Max cpu time              unlimited            unlimited            seconds
        std::string line;
        std::getline(if_limits, line); // skip first line
        while (std::getline(if_limits, line))
        {
            boost::regex regex("^Max\\s(.*)\\s+(unlimited|[[:digit:]]+)\\s+(unlimited|[[:digit:]]+).*$");
            boost::smatch match;
            if (boost::regex_match(line, match, regex))
            {
                if (match.size() == 3 + 1)
                {
                    int soft_lmt;
                    int hard_lmt;
                    (match[2] == "unlimited") ? soft_lmt = -1 : soft_lmt = std::stoi(match[2]);
                    (match[3] == "unlimited") ? hard_lmt = -1 : hard_lmt = std::stoi(match[3]);
                    std::string field_name = match[1];
                    boost::algorithm::trim(field_name);
                    if (field_name == "cpu time")
                    {
                        pinfo.limits.cpu_time_soft_lmt = soft_lmt;
                        pinfo.limits.cpu_time_hard_lmt = hard_lmt;
                    } else if (field_name == "file size")
                    {
                        pinfo.limits.file_size_soft_lmt = soft_lmt;
                        pinfo.limits.file_size_hard_lmt = hard_lmt;
                    } else if (field_name == "data size")
                    {
                        pinfo.limits.data_size_soft_lmt = soft_lmt;
                        pinfo.limits.data_size_hard_lmt = hard_lmt;
                    } else if (field_name == "stack size")
                    {
                        pinfo.limits.stack_size_soft_lmt = soft_lmt;
                        pinfo.limits.stack_size_hard_lmt = hard_lmt;
                    } else if (field_name == "core file size")
                    {
                        pinfo.limits.core_file_size_soft_lmt = soft_lmt;
                        pinfo.limits.core_file_size_hard_lmt = hard_lmt;
                    } else if (field_name == "resident set")
                    {
                        pinfo.limits.resident_set_soft_lmt = soft_lmt;
                        pinfo.limits.resident_set_hard_lmt = hard_lmt;
                    } else if (field_name == "processes")
                    {
                        pinfo.limits.processes_soft_lmt = soft_lmt;
                        pinfo.limits.processes_hard_lmt = hard_lmt;
                    } else if (field_name == "open files")
                    {
                        pinfo.limits.open_files_soft_lmt = soft_lmt;
                        pinfo.limits.open_files_hard_lmt = hard_lmt;
                    } else if (field_name == "locked memory")
                    {
                        pinfo.limits.locked_memory_soft_lmt = soft_lmt;
                        pinfo.limits.locked_memory_hard_lmt = hard_lmt;
                    } else if (field_name == "address space")
                    {
                        pinfo.limits.address_space_soft_lmt = soft_lmt;
                        pinfo.limits.address_space_hard_lmt = hard_lmt;
                    } else if (field_name == "file locks")
                    {
                        pinfo.limits.file_locks_soft_lmt = soft_lmt;
                        pinfo.limits.file_locks_hard_lmt = hard_lmt;
                    } else if (field_name == "pending signals")
                    {
                        pinfo.limits.pending_signals_soft_lmt = soft_lmt;
                        pinfo.limits.pending_signals_hard_lmt = hard_lmt;
                    } else if (field_name == "msgqueue size")
                    {
                        pinfo.limits.msgqueue_size_soft_lmt = soft_lmt;
                        pinfo.limits.msgqueue_size_hard_lmt = hard_lmt;
                    } else if (field_name == "nice priority")
                    {
                        pinfo.limits.nice_priority_soft_lmt = soft_lmt;
                        pinfo.limits.nice_priority_hard_lmt = hard_lmt;
                    } else if (field_name == "realtime priority")
                    {
                        pinfo.limits.realtime_priority_soft_lmt = soft_lmt;
                        pinfo.limits.realtime_priority_hard_lmt = hard_lmt;
                    } else if (field_name == "realtime timeout")
                    {
                        pinfo.limits.realtime_timeout_soft_lmt = soft_lmt;
                        pinfo.limits.realtime_timeout_hard_lmt = hard_lmt;
                    }
                }
            }
        }
    }
    if_limits.close();

    // read stack
    std::ifstream if_stack(process_path + "/stack");
    if (if_stack.is_open())
    {
        std::string line;
        while (std::getline(if_stack, line))
        {
            boost::regex regex("^\\[<([[:xdigit:]]+)>\\]\\s(.*)$");
            boost::smatch match;
            if (boost::regex_match(line, match, regex))
            {
                if (match.size() == 2 + 1)
                {
                    struct stack_func_t func;
                    func.address = match[1];
                    func.function = match[2];

                    pinfo.stack.push_back(func);
                }
            }
        }
    }
    if_stack.close();


    return pinfo;
}

*/

void addCallbackProcessEvent(std::function<void (proc_event)> callback)
{
    connector->addCallback(callback);
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
