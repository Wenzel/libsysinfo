#include <fstream>
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

ProcessInfo::ProcessInfo(pid_t pid)
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
}

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
