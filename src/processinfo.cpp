#include <fstream>
#include <algorithm>
#include <functional>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "processinfo.h"

// static init
std::unordered_map<int, struct old_cpu_time_t> ProcessInfo::map_pid_usage;

// overload operator<<
std::ostream& operator<<(std::ostream& os, const ProcessInfo& p)
{

  return os;
}

ProcessInfo::ProcessInfo(pid_t pid, bool detail)
{
    this->m_pid = pid;
    this->m_proc_path = "/proc/" + std::to_string(pid) + "/";
    readSymlinks();
    readCmdline();
    readStat();
    readStatus();
    readEnviron();
    readIo();
    updateCPUUsage();

    if (detail)
        detailedInfo();
}

// getters
pid_t ProcessInfo::pid() { return m_pid; }
std::string ProcessInfo::name() { return  m_name; }
std::string ProcessInfo::cmdline()
{
    const char* delim = " ";
    std::stringstream res;
    std::copy(m_cmdline.begin(), m_cmdline.end(), std::ostream_iterator<std::string>(res, delim));
    return res.str();
}

std::string ProcessInfo::cwd() { return m_cwd; }
std::string ProcessInfo::root() { return m_root; }
std::string ProcessInfo::exe() { return m_exe; }
std::unordered_map<std::string, std::string> ProcessInfo::environ() { return m_environ; }
int ProcessInfo::cpuUsage() { return m_cpu_usage; }


void ProcessInfo::readSymlinks()
{
    // read cwd
    boost::system::error_code ec;
    this->m_cwd = boost::filesystem::read_symlink(m_proc_path + "cwd", ec).string();

    // read exe
    this->m_exe = boost::filesystem::read_symlink(m_proc_path + "exe", ec).string();

    // read root
    this->m_root = boost::filesystem::read_symlink(m_proc_path + "root", ec).string();
}

void ProcessInfo::readCmdline()
{
    std::ifstream if_cmdline(m_proc_path + "cmdline");
    std::string cur_arg;
    while (getline(if_cmdline, cur_arg, '\0'))
        this->m_cmdline.push_back(cur_arg);
    if_cmdline.close();
}

void ProcessInfo::readStat()
{
    std::ifstream if_stat(m_proc_path + "stat");
    // pid
    if_stat >> this->m_pid;
    // name
    if_stat >> this->m_name; // (name)
    this->m_name.erase(std::remove(this->m_name.begin(), this->m_name.end(), '('), this->m_name.end());
    this->m_name.erase(std::remove(this->m_name.begin(), this->m_name.end(), ')'), this->m_name.end());
    // status
    std::string status;
    if_stat >> status;
    if (status == "R")
        this->m_status = running;
    else if (status == "S")
        this->m_status = sleeping;
    else if (status == "D")
        this->m_status = disk_sleep;
    else if (status == "Z")
        this->m_status = zombie;
    else if (status == "T")
        this->m_status = stopped;
    else if (status == "t")
        this->m_status = tracing_stop;
    else if (status == "W")
        this->m_status = waking;
    else if (status == "X" or status == "x")
        this->m_status = dead;
    else if (status == "K")
        this->m_status = wakekill;
    else if (status == "P")
        this->m_status = parked;

    if_stat >> this->m_ppid;
    if_stat >> this->m_pgrp;
    if_stat >> this->m_session;
    if_stat >> this->m_tty_nr;
    if_stat >> this->m_tpgid;
    if_stat >> this->m_flags;
    if_stat >> this->m_minflt;
    if_stat >> this->m_cminflt;
    if_stat >> this->m_cmajflt;
    if_stat >> this->m_utime;
    if_stat >> this->m_stime;
    if_stat >> this->m_cutime;
    if_stat >> this->m_cstime;
    if_stat >> this->m_priority;
    if_stat >> this->m_nice;
    if_stat >> this->m_num_threads;
    if_stat >> this->m_itrealvalue;
    if_stat >> this->m_starttime;
    if_stat >> this->m_vsize;
    if_stat >> this->m_rss;
    if_stat >> this->m_rsslim;
    if_stat >> this->m_startcode;
    if_stat >> this->m_endcode;
    if_stat >> this->m_startstack;
    if_stat >> this->m_kstkesp;
    if_stat >> this->m_kstkeip;
    if_stat >> this->m_signal;
    if_stat >> this->m_blocked;
    if_stat >> this->m_siginore;
    if_stat >> this->m_sigcatch;
    if_stat >> this->m_wchan;
    if_stat >> this->m_nswap;
    if_stat >> this->m_cnswap;
    if_stat >> this->m_exit_signal;
    if_stat >> this->m_processor;
    if_stat >> this->m_rt_priority;
    if_stat >> this->m_policy;
    if_stat >> this->m_delayacct_blkio_ticks;
    if_stat >> this->m_guest_time;
    if_stat >> this->m_cguest_time;
    if_stat >> this->m_start_data;
    if_stat >> this->m_end_data;
    if_stat >> this->m_start_brk;
    if_stat >> this->m_arg_start;
    if_stat >> this->m_arg_end;
    if_stat >> this->m_env_start;
    if_stat >> this->m_env_end;
    if_stat >> this->m_exit_code;
    if_stat.close();
}

void ProcessInfo::readStatus()
{
    std::ifstream if_status(m_proc_path + "status");
    std::string line;
    while (std::getline(if_status, line))
    {
        // read uids and gids
        boost::regex regex("^.?id:\\s+([[:digit:]]+)\\s+([[:digit:]])\\s+([[:digit:]]+)\\s+([[:digit:]]+)\\s*$");
        boost::smatch match;
        if (boost::regex_match(line, match, regex))
        {
            if (match.size() == 4 + 1)
            {
                if (line.at(0) == 'U') // Uid
                    for (int i = 0 ; i < 3 ; i++)
                        this->m_uids.push_back(std::stoi(match[i + 1]));
                else // Gid
                    for (int i = 0 ; i < 3 ; i++)
                        this->m_gids.push_back(std::stoi(match[i + 1]));
            }
        }
    }
    if_status.close();
}

void ProcessInfo::readEnviron()
{
    std::ifstream if_environ(m_proc_path + "environ");
    std::string cur_env;
    while (getline(if_environ, cur_env, '\0'))
    {
        std::istringstream stream(cur_env);
        std::string key;
        std::string value;
        // split on '='
        getline(stream, key, '=');
        getline(stream, value);
        this->m_environ[key] = value;
    }
    if_environ.close();
}

void ProcessInfo::readIo()
{
    std::ifstream if_io(m_proc_path + "io");
    std::string line;
    while (getline(if_io, line))
    {
        std::string key;
        std::string value;
        // parse key: value
        std::istringstream stream(line);
        getline(stream, key, ':');
        getline(stream, value);
        boost::algorithm::trim(value);
        if (key == "rchar")
            this->m_io.rchar = std::stol(value);
        else if (key == "wchar")
            this->m_io.wchar = sleeping; // TODO
        else if (key == "syscr")
            this->m_io.syscr = std::stol(value);
        else if (key == "syscw")
            this->m_io.syscw = std::stol(value);
        else if (key == "read_bytes")
            this->m_io.read_bytes = std::stol(value);
        else if (key == "write_bytes")
            this->m_io.write_bytes = std::stol(value);
        else if (key == "cancelled_write_bytes")
            this->m_io.cancelled_write_bytes = std::stol(value);
    }
    if_io.close();
}

void ProcessInfo::updateCPUUsage()
{
    int cpu_usage = 0;

    // read /proc/stat
    std::ifstream if_cpu_stat("/proc/stat");
    std::string tmp;
    if_cpu_stat >> tmp;
    long long unsigned user,nice,system,idle,cpu_total_time;
    user = 0;
    if_cpu_stat >> user;
    if_cpu_stat >> nice;
    if_cpu_stat >> system;
    if_cpu_stat >> idle;
    if_cpu_stat.close();
    // get cpu_total_time
    cpu_total_time = user + nice + system + idle;

    // get process_total_time
    long long unsigned process_total_time = this->m_utime + this->m_stime;
    struct old_cpu_time_t old_cpu_time;


    if (ProcessInfo::map_pid_usage.find(this->m_pid) == map_pid_usage.end())
    {
        // insert
        old_cpu_time.cpu_total_time = cpu_total_time;
        old_cpu_time.proc_total_time = process_total_time;
        ProcessInfo::map_pid_usage[this->m_pid] = old_cpu_time;
    }
    else
    {
        // retrieve old value
        old_cpu_time = ProcessInfo::map_pid_usage[this->m_pid];
        long long unsigned delta_cpu_time = cpu_total_time - old_cpu_time.cpu_total_time;
        long long unsigned delta_process_time = process_total_time - old_cpu_time.proc_total_time;
        if (delta_cpu_time != 0)
            cpu_usage = 100 * 8 * delta_process_time / delta_cpu_time; // TODO cache getNbCores()

        // update old values
        old_cpu_time.cpu_total_time = cpu_total_time;
        old_cpu_time.proc_total_time = process_total_time;
        ProcessInfo::map_pid_usage[this->m_pid] = old_cpu_time;
    }

    this->m_cpu_usage = cpu_usage;
}

void ProcessInfo::detailedInfo()
{
    readFd();
    readCgroup();
    readSmaps();
    readLimits();
    readStack();
}

void ProcessInfo::readFd()
{
    boost::filesystem::path fd_dir_path(m_proc_path + "/fd");
    boost::filesystem::directory_iterator end_itr;
    for (boost::filesystem::directory_iterator itr(fd_dir_path);
         itr != end_itr;
         ++itr)
    {
        boost::system::error_code ec;
        boost::filesystem::path symlink = boost::filesystem::read_symlink(itr->path(), ec);
        this->m_fds[std::stoi(itr->path().filename().string())] = symlink.string();
    }
}

void ProcessInfo::readCgroup()
{
    std::ifstream if_cgroup(m_proc_path + "/cgroup");
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

            this->m_cgroups.push_back(cgroup);
        }
    }
    if_cgroup.close();
}

void ProcessInfo::readSmaps()
{
    std::ifstream if_smaps(m_proc_path + "/smaps");
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
                    this->m_maps.push_back(memory_mapping_t());
                    region = this->m_maps.back();

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
}

void ProcessInfo::readLimits()
{
    std::ifstream if_limits(m_proc_path + "/limits");
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
                        this->m_limits.cpu_time_soft_lmt = soft_lmt;
                        this->m_limits.cpu_time_hard_lmt = hard_lmt;
                    } else if (field_name == "file size")
                    {
                        this->m_limits.file_size_soft_lmt = soft_lmt;
                        this->m_limits.file_size_hard_lmt = hard_lmt;
                    } else if (field_name == "data size")
                    {
                        this->m_limits.data_size_soft_lmt = soft_lmt;
                        this->m_limits.data_size_hard_lmt = hard_lmt;
                    } else if (field_name == "stack size")
                    {
                        this->m_limits.stack_size_soft_lmt = soft_lmt;
                        this->m_limits.stack_size_hard_lmt = hard_lmt;
                    } else if (field_name == "core file size")
                    {
                        this->m_limits.core_file_size_soft_lmt = soft_lmt;
                        this->m_limits.core_file_size_hard_lmt = hard_lmt;
                    } else if (field_name == "resident set")
                    {
                        this->m_limits.resident_set_soft_lmt = soft_lmt;
                        this->m_limits.resident_set_hard_lmt = hard_lmt;
                    } else if (field_name == "processes")
                    {
                        this->m_limits.processes_soft_lmt = soft_lmt;
                        this->m_limits.processes_hard_lmt = hard_lmt;
                    } else if (field_name == "open files")
                    {
                        this->m_limits.open_files_soft_lmt = soft_lmt;
                        this->m_limits.open_files_hard_lmt = hard_lmt;
                    } else if (field_name == "locked memory")
                    {
                        this->m_limits.locked_memory_soft_lmt = soft_lmt;
                        this->m_limits.locked_memory_hard_lmt = hard_lmt;
                    } else if (field_name == "address space")
                    {
                        this->m_limits.address_space_soft_lmt = soft_lmt;
                        this->m_limits.address_space_hard_lmt = hard_lmt;
                    } else if (field_name == "file locks")
                    {
                        this->m_limits.file_locks_soft_lmt = soft_lmt;
                        this->m_limits.file_locks_hard_lmt = hard_lmt;
                    } else if (field_name == "pending signals")
                    {
                        this->m_limits.pending_signals_soft_lmt = soft_lmt;
                        this->m_limits.pending_signals_hard_lmt = hard_lmt;
                    } else if (field_name == "msgqueue size")
                    {
                        this->m_limits.msgqueue_size_soft_lmt = soft_lmt;
                        this->m_limits.msgqueue_size_hard_lmt = hard_lmt;
                    } else if (field_name == "nice priority")
                    {
                        this->m_limits.nice_priority_soft_lmt = soft_lmt;
                        this->m_limits.nice_priority_hard_lmt = hard_lmt;
                    } else if (field_name == "realtime priority")
                    {
                        this->m_limits.realtime_priority_soft_lmt = soft_lmt;
                        this->m_limits.realtime_priority_hard_lmt = hard_lmt;
                    } else if (field_name == "realtime timeout")
                    {
                        this->m_limits.realtime_timeout_soft_lmt = soft_lmt;
                        this->m_limits.realtime_timeout_hard_lmt = hard_lmt;
                    }
                }
            }
        }
    }
    if_limits.close();
}

void ProcessInfo::readStack()
{
    std::ifstream if_stack(m_proc_path + "/stack");
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

                    this->m_stack.push_back(func);
                }
            }
        }
    }
    if_stack.close();
}
