#include <fstream>
#include <algorithm>
#include <functional>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>

#include "processinfo.h"

// static init
std::unordered_map<int, struct old_cpu_time_t> ProcessInfo::map_pid_usage;

// overload operator<<
std::ostream& operator<<(std::ostream& os, ProcessInfo& p)
{
    os << "Name : " << p.name() << std::endl;
    os << "PID : " << p.pid() << std::endl;
    os << "Parent PID : " << p.ppid() << std::endl;
    os << "Process Group ID : " << p.pgid() << std::endl;
    os << "Session ID : " << p.sid() << std::endl;
    os << "Controlling tty : " << "Major : " << MAJOR(p.m_tty_nr) << ", Minor : " << MINOR(p.m_tty_nr) << " (" << p.ttyNr() << ")" << std::endl;
    os << "starttime : " << "jiffies : " << p.m_starttime << ", diff : " << p.startTime() << std::endl;
    os << "Threads : " << p.m_num_threads << std::endl;
    os << "Opened Files :" << std::endl;
    const std::unordered_map<int, std::string>& fds = p.fds();
    std::unordered_map<int, std::string>::const_iterator it;
    for (it = fds.begin(); it != fds.end(); it++)
        os << "[" << it->first << "]" << " => " << it->second << std::endl;
    std::cout << "user name : " << p.userName() << std::endl;
    return os;
}

ProcessInfo::ProcessInfo()
{
    m_pid = 0;
}

ProcessInfo::ProcessInfo(pid_t pid)
{
    m_pid = pid;
    m_proc_path = "/proc/" + std::to_string(pid) + "/";
    m_need_update_stat = false;
    m_need_update_status = false;
    m_need_update_io = false;
    m_need_update_cwd = false;
    m_need_update_exe = false;
    m_need_update_cmdline = false;
    m_need_update_root = false;
    m_need_update_environ = false;
    m_need_update_fd = false;
    m_need_update_wchan = false;
    m_need_update_smaps = false;
    m_need_update_cpu_usage = false;

    this->needUpdate();
}

ProcessInfo::~ProcessInfo()
{
    map_pid_usage.erase(m_pid);
}

void ProcessInfo::needUpdate()
{
    m_need_update_stat = true;
    m_need_update_status = true;
    m_need_update_io = true;
    m_need_update_cwd = true;
    m_need_update_exe = true;
    m_need_update_cmdline = true;
    m_need_update_root = true;
    m_need_update_environ = true;
    m_need_update_fd = true;
    m_need_update_wchan = true;
    m_need_update_smaps = true;
    m_need_update_cpu_usage = true;
}

// getters
// from stat
pid_t ProcessInfo::pid() const { return m_pid; }

const std::string ProcessInfo::name()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return  m_name;
}

const std::string& ProcessInfo::state()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_state;
}

int ProcessInfo::ppid()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_ppid;
}

int ProcessInfo::pgid()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_pgid;
}

int ProcessInfo::sid()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_session;
}

const std::string ProcessInfo::ttyNr()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }

    // /dev/tty* ?
    boost::filesystem::path path("/dev");
    boost::filesystem::directory_iterator end_itr;
    for (boost::filesystem::directory_iterator itr(path);
         itr != end_itr;
         ++itr)
    {
        if (boost::regex_match(itr->path().filename().string(), boost::regex("^tty\\d*$")))
        {
            // stat
            struct stat st;
            if (stat(itr->path().string().c_str(), &st) == 0)
            {
                // corresponding st_dev ?
                if (m_tty_nr == st.st_rdev)
                    return itr->path().string();
            }
        }
    }
    // /dev/pts ?
    path = "/dev/pts";
    for (boost::filesystem::directory_iterator itr(path);
         itr != end_itr;
         ++itr)
    {
        // stat
        struct stat st;
        if (stat(itr->path().string().c_str(), &st) == 0)
        {
            // corresponding st_rdev ?
            if (m_tty_nr == st.st_rdev)
                return itr->path().string();
        }
    }
    return std::string();
}

int ProcessInfo::tpgid()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_tpgid;
}

unsigned int ProcessInfo::flags()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_flags;
}

long unsigned int ProcessInfo::minflt()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_minflt;
}

long unsigned int ProcessInfo::cminflt()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_cminflt;
}

long unsigned int ProcessInfo::majflt()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_majflt;
}

long unsigned int ProcessInfo::cmajflt()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_cmajflt;
}

long unsigned int ProcessInfo::utime()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_utime;
}

long unsigned int ProcessInfo::stime()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_stime;
}

long unsigned int ProcessInfo::cutime()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_cutime;
}

long unsigned int ProcessInfo::cstime()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_cstime;
}

long int ProcessInfo::priority()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_priority;
}

long int ProcessInfo::nice()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_nice;
}

long int ProcessInfo::numThreads()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_num_threads;
}

long long unsigned int ProcessInfo::startTime()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    // convert jiffies to seconds
    long hz = sysconf(_SC_CLK_TCK);
    long long unsigned int starttime_sec = m_starttime / hz;
    // get uptime
    struct sysinfo info;
    sysinfo(&info);
    long long unsigned int diff = info.uptime - starttime_sec;
    return diff;

}

long unsigned int ProcessInfo::vmSize()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_vmsize;
}

long unsigned int ProcessInfo::startCode()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_startcode;
}

long unsigned int ProcessInfo::endCode()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_endcode;
}

long unsigned int ProcessInfo::startStack()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_startstack;
}

long unsigned int ProcessInfo::kstkEsp()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_kstkesp;
}

long unsigned int ProcessInfo::kstkEip()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_kstkeip;
}

long unsigned int ProcessInfo::wchanAddr()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_wchan_addr;
}

int ProcessInfo::processor()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_processor;
}

unsigned int ProcessInfo::rtPriority()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_rt_priority;
}

std::string ProcessInfo::policy()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }

    switch (m_policy)
    {
    case SCHED_NORMAL:
        return "SCHED_NORMAL";
    case SCHED_FIFO:
        return "SCHED_FIFO";
    case SCHED_RR:
        return "SCHED_RR";
    case SCHED_BATCH:
        return "SCHED_BATCH";
    case SCHED_IDLE:
        return "SCHED_IDLE";
    case SCHED_DEADLINE:
        return "SCHED_DEADLINE";
    }
    return std::string();
}

long long unsigned int ProcessInfo::delayacctBlkioTicks()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_delayacct_blkio_ticks;
}

long unsigned int ProcessInfo::guestTime()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_guest_time;
}

long unsigned int ProcessInfo::cguestTime()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_cguest_time;
}

long unsigned int ProcessInfo::startData()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_start_data;
}

long unsigned int ProcessInfo::endData()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_end_data;
}

long unsigned int ProcessInfo::startBrk()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_start_brk;
}

long unsigned int ProcessInfo::startArg()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_arg_start;
}

long unsigned int ProcessInfo::endArg()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_arg_end;
}

long unsigned int ProcessInfo::startEnv()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_env_start;
}

long unsigned int ProcessInfo::endEnv()
{
    if (m_need_update_stat)
    {
        readStat();
        m_need_update_stat = false;
    }
    return m_env_end;
}

// from cmdline
const std::vector<std::string>& ProcessInfo::cmdline()
{
    if (m_need_update_cmdline)
    {
        readCmdline();
        m_need_update_cmdline = false;
    }
    return m_cmdline;
}

// from cwd
const std::string ProcessInfo::cwd()
{
    if (m_need_update_cwd)
    {
        readCwd();
        m_need_update_cwd = false;
    }
    return m_cwd;
}

// from exe
const std::string ProcessInfo::exe()
{
    if (m_need_update_exe)
    {
        readExe();
        m_need_update_exe = false;
    }
    return m_exe;
}

// from root
const std::string ProcessInfo::root()
{
    if (m_need_update_root)
    {
        readRoot();
        m_need_update_root = false;
    }
    return m_root;
}

// from environ
const std::unordered_map<std::string, std::string> ProcessInfo::environ()
{
    if (m_need_update_environ)
    {
        readEnviron();
        m_need_update_environ = false;
    }
    return m_environ;
}

int ProcessInfo::cpuUsage()
{
    if (m_need_update_cpu_usage)
    {
        updateCPUUsage();
        m_need_update_cpu_usage = false;
    }
    return m_cpu_usage;
}

// from status
const std::string ProcessInfo::userName()
{
    if (m_need_update_status)
    {
        readStatus();
        m_need_update_status = false;
    }
    std::string username;
    struct passwd* pass = nullptr;
    pass = getpwuid(m_uids[0]); // real uid
    if (pass == nullptr)
    {
        // the process is in a container, but is still visible
        // returns the uid as the username for now
        username = std::to_string(m_uids[0]);
    }
    else
        username = std::string(pass->pw_name);
    return username;
}

const std::vector<int>& ProcessInfo::uids()
{
    if (m_need_update_status)
    {
        readStatus();
        m_need_update_status = false;
    }
    return m_uids;
}

const std::vector<int>& ProcessInfo::gids()
{
    if (m_need_update_status)
    {
        readStatus();
        m_need_update_status = false;
    }
    return m_gids;
}

long unsigned int ProcessInfo::vmPeak()
{
    if (m_need_update_status)
    {
        readStatus();
        m_need_update_status = false;
    }
    return m_vm_peak;
}

long unsigned int ProcessInfo::vmLck()
{
    if (m_need_update_status)
    {
        readStatus();
        m_need_update_status = false;
    }
    return m_vm_lck;
}

long unsigned int ProcessInfo::vmPin()
{
    if (m_need_update_status)
    {
        readStatus();
        m_need_update_status = false;
    }
    return m_vm_pin;
}

long unsigned int ProcessInfo::vmHwm()
{
    if (m_need_update_status)
    {
        readStatus();
        m_need_update_status = false;
    }
    return m_vm_hwm;
}

long unsigned int ProcessInfo::vmRss()
{
    if (m_need_update_status)
    {
        readStatus();
        m_need_update_status = false;
    }
    return m_vm_rss;
}

long unsigned int ProcessInfo::vmData()
{
    if (m_need_update_status)
    {
        readStatus();
        m_need_update_status = false;
    }
    return m_vm_data;
}

long unsigned int ProcessInfo::vmStk()
{
    if (m_need_update_status)
    {
        readStatus();
        m_need_update_status = false;
    }
    return m_vm_stk;
}

long unsigned int ProcessInfo::vmExe()
{
    if (m_need_update_status)
    {
        readStatus();
        m_need_update_status = false;
    }
    return m_vm_exe;
}

long unsigned int ProcessInfo::vmLib()
{
    if (m_need_update_status)
    {
        readStatus();
        m_need_update_status = false;
    }
    return m_vm_lib;
}

long unsigned int ProcessInfo::vmPte()
{
    if (m_need_update_status)
    {
        readStatus();
        m_need_update_status = false;
    }
    return m_vm_pte;
}

long unsigned int ProcessInfo::vmPmd()
{
    if (m_need_update_status)
    {
        readStatus();
        m_need_update_status = false;
    }
    return m_vm_pmd;
}

long unsigned int ProcessInfo::vmSwap()
{
    if (m_need_update_status)
    {
        readStatus();
        m_need_update_status = false;
    }
    return m_vm_swap;
}


// from fd
const std::unordered_map<int, std::string>& ProcessInfo::fds()
{
    if (m_need_update_fd)
    {
        readFd();
        m_need_update_fd = false;
    }
    return m_fds;
}

// from wchan
const std::string ProcessInfo::wchanName()
{
    if (m_need_update_wchan)
    {
        readWchan();
        m_need_update_wchan = false;
    }
    return m_wchan_name;
}

// from smaps
const std::vector<MMap>& ProcessInfo::maps()
{
    if (m_need_update_smaps)
    {
        readSmaps();
        m_need_update_smaps = false;
    }
    return m_maps;
}

// read*
void ProcessInfo::readCwd()
{
    boost::system::error_code ec;
    this->m_cwd = boost::filesystem::read_symlink(m_proc_path + "cwd", ec).string();
}

void ProcessInfo::readExe()
{
    boost::system::error_code ec;
    this->m_exe = boost::filesystem::read_symlink(m_proc_path + "exe", ec).string();
}

void ProcessInfo::readRoot()
{
    boost::system::error_code ec;
    this->m_root = boost::filesystem::read_symlink(m_proc_path + "root", ec).string();
}

void ProcessInfo::readCmdline()
{
    std::ifstream if_cmdline(m_proc_path + "cmdline");
    m_cmdline.clear();
    std::string cur_arg;
    while (getline(if_cmdline, cur_arg, '\0'))
        m_cmdline.push_back(cur_arg);
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
    std::string state;
    if_stat >> state;
    if (state == "R")
        this->m_state = "Running";
    else if (state == "S")
        this->m_state = "Sleeping";
    else if (state == "D")
        this->m_state = "Disk sleep";
    else if (state == "Z")
        this->m_state = "Zombie";
    else if (state == "T")
        this->m_state = "Stopped";
    else if (state == "W")
        this->m_state = "Waking";
    else
        this->m_state = state;

    if_stat >> this->m_ppid;
    if_stat >> this->m_pgid;
    if_stat >> this->m_session;
    if_stat >> this->m_tty_nr;
    if_stat >> this->m_tpgid;
    if_stat >> this->m_flags;
    if_stat >> this->m_minflt;
    if_stat >> this->m_cminflt;
    if_stat >> this->m_majflt;
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
    if_stat >> this->m_vmsize;
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
    if_stat >> this->m_wchan_addr;
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
        // extract key / value
        std::istringstream stream(line);
        std::string key;
        std::getline(stream, key, ':');
        std::string value;
        std::getline(stream, value);

        // regex
        boost::regex regex_uids("^\\s+([[:digit:]]+)\\s+([[:digit:]]+)\\s+([[:digit:]]+)\\s+([[:digit:]]+)\\s*$");
        boost::regex regex_vm("^\\s+([[:digit:]]+)\\skB\\s*$");
        boost::smatch match;
        if (key == "Uid")
        {
            if (boost::regex_match(value, match, regex_uids))
            {
                if (match.size() == 4 + 1)
                    for (int i = 0 ; i < 4 ; i++)
                        m_uids.push_back(std::stoi(match[i + 1]));
            }
            continue;
        }
        if (key == "Gid")
        {
            if (boost::regex_match(value, match, regex_uids))
            {
                if (match.size() == 4 + 1)
                    for (int i = 0 ; i < 4 ; i++)
                        m_gids.push_back(std::stoi(match[i + 1]));
            }
            continue;
        }
        if (key == "VmPeak")
        {
            if (boost::regex_match(value, match, regex_vm))
            {
                if (match.size() == 1 + 1)
                    m_vm_peak = std::stoi(match[1]);
            }
            continue;
        }
        if (key == "VmLck")
        {
            if (boost::regex_match(value, match, regex_vm))
            {
                if (match.size() == 1 + 1)
                    m_vm_lck = std::stoi(match[1]);
            }
            continue;
        }
        if (key == "VmPin")
        {
            if (boost::regex_match(value, match, regex_vm))
            {
                if (match.size() == 1 + 1)
                    m_vm_pin = std::stoi(match[1]);
            }
            continue;
        }
        if (key == "VmHWM")
        {
            if (boost::regex_match(value, match, regex_vm))
            {
                if (match.size() == 1 + 1)
                    m_vm_hwm = std::stoi(match[1]);
            }
            continue;
        }
        if (key == "VmRSS")
        {
            if (boost::regex_match(value, match, regex_vm))
            {
                if (match.size() == 1 + 1)
                    m_vm_rss = std::stoi(match[1]);
            }
            continue;
        }
        if (key == "VmData")
        {
            if (boost::regex_match(value, match, regex_vm))
            {
                if (match.size() == 1 + 1)
                    m_vm_data = std::stoi(match[1]);
            }
            continue;
        }
        if (key == "VmStk")
        {
            if (boost::regex_match(value, match, regex_vm))
            {
                if (match.size() == 1 + 1)
                    m_vm_stk = std::stoi(match[1]);
            }
            continue;
        }
        if (key == "VmExe")
        {
            if (boost::regex_match(value, match, regex_vm))
            {
                if (match.size() == 1 + 1)
                    m_vm_exe = std::stoi(match[1]);
            }
            continue;
        }
        if (key == "VmLib")
        {
            if (boost::regex_match(value, match, regex_vm))
            {
                if (match.size() == 1 + 1)
                    m_vm_lib = std::stoi(match[1]);
            }
            continue;
        }
        if (key == "VmPTE")
        {
            if (boost::regex_match(value, match, regex_vm))
            {
                if (match.size() == 1 + 1)
                    m_vm_pte = std::stoi(match[1]);
            }
            continue;
        }
        if (key == "VmPMD")
        {
            if (boost::regex_match(value, match, regex_vm))
            {
                if (match.size() == 1 + 1)
                    m_vm_pmd = std::stoi(match[1]);
            }
            continue;
        }
        if (key == "VmSwap")
        {
            if (boost::regex_match(value, match, regex_vm))
            {
                if (match.size() == 1 + 1)
                    m_vm_swap = std::stoi(match[1]);
            }
            continue;
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
    // update time since last read
    m_io.last_read = time(nullptr);
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
            this->m_io.wchar = std::stol(value);
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

void ProcessInfo::readWchan()
{
    std::ifstream if_wchan(m_proc_path + "wchan");
    std::getline(if_wchan, m_wchan_name);
    if_wchan.close();
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
    m_maps.clear();
    std::ifstream if_smaps(m_proc_path + "/smaps");
    if (if_smaps.is_open())
    {
        // line sample
        // 00400000-00452000 r-xp 00000000 08:02 173521      /usr/bin/dbus-daemon
        // or
        // Shared_Clean:          0 kB
        std::string line;
        std::stringstream map_stream;
        while (std::getline(if_smaps, line))
        {
            // new declaration ?
            if (line.find('-') != std::string::npos)
            {
                if (!map_stream.str().empty())
                {
                    // create map
                    MMap map = MMap(map_stream);
                    // append
                    m_maps.push_back(map);
                    // reset stream
                    map_stream.clear();
                    map_stream.str("");
                }
            }
            map_stream << line;
            map_stream << "\n";
        }
        // append last map
        MMap map = MMap(map_stream);
        m_maps.push_back(map);
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


void ProcessInfo::updateCPUUsage()
{
    static int nb_core = getNbCores();
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
    long long unsigned process_total_time = utime() + stime();
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
            cpu_usage = 100 * nb_core * delta_process_time / delta_cpu_time;

        // update old values
        old_cpu_time.cpu_total_time = cpu_total_time;
        old_cpu_time.proc_total_time = process_total_time;
        ProcessInfo::map_pid_usage[this->m_pid] = old_cpu_time;
    }

    this->m_cpu_usage = cpu_usage;
}
