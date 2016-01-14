#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include <iostream>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>

#include <sys/types.h>
#include <pwd.h>

enum process_status
{
    running,
    sleeping,
    disk_sleep,
    stopped,
    tracing_stop,
    zombie,
    dead,
    wakekill,
    waking,
    parked
};

struct io_stat
{
    /* characters read */
    long unsigned int rchar;
    /* characters written */
    long unsigned int wchar;
    /* read syscalls */
    long unsigned int syscr;
    /* write syscalls */
    long unsigned int syscw;
    /* bytes read */
    long unsigned int read_bytes;
    /* bytes written */
    long unsigned int write_bytes;
    /*
     * this field represents the number of bytes which
     * this process caused to not happen, by truncating
     * agecache.  A task can cause "negative" I/O too.  If
     * this task truncates some dirty pagecache, some I/O
     * which another task has been accounted for (in its
     * write_bytes) will not be happening.
    */
    long unsigned int cancelled_write_bytes;

};

struct cgroup_hierarchy_t
{
    int hierarchy_id;
    std::vector<std::string> subsystems;
    std::string cgroup;

};

enum mapping_type
{
    shared,
    priv
};

struct memory_mapping_t
{
    std::string address_from;
    std::string address_to;
    bool perm_read;
    bool perm_write;
    bool perm_execute;
    enum mapping_type type;
    int offset;
    int dev_major;
    int dev_minor;
    long inode;
    std::string pathname;
    // smaps additional fields
    int size;
    int rss;
    int pss;
    int shared_clean;
    int shared_dirty;
    int private_clean;
    int private_dirty;
    int referenced;
    int anonymous;
    int anonhugepages;
    int swap;
    int kernelpagesize;
    int mmupagesize;
    int locked;
    std::vector<std::string> vmflags;

};

struct limits_t
{
    long cpu_time_soft_lmt;
    long file_size_soft_lmt;
    long data_size_soft_lmt;
    long stack_size_soft_lmt;
    long core_file_size_soft_lmt;
    long resident_set_soft_lmt;
    long processes_soft_lmt;
    long open_files_soft_lmt;
    long locked_memory_soft_lmt;
    long address_space_soft_lmt;
    long file_locks_soft_lmt;
    long pending_signals_soft_lmt;
    long msgqueue_size_soft_lmt;
    long nice_priority_soft_lmt;
    long realtime_priority_soft_lmt;
    long realtime_timeout_soft_lmt;

    long cpu_time_hard_lmt;
    long file_size_hard_lmt;
    long data_size_hard_lmt;
    long stack_size_hard_lmt;
    long core_file_size_hard_lmt;
    long resident_set_hard_lmt;
    long processes_hard_lmt;
    long open_files_hard_lmt;
    long locked_memory_hard_lmt;
    long address_space_hard_lmt;
    long file_locks_hard_lmt;
    long pending_signals_hard_lmt;
    long msgqueue_size_hard_lmt;
    long nice_priority_hard_lmt;
    long realtime_priority_hard_lmt;
    long realtime_timeout_hard_lmt;

};

struct stack_func_t
{
    std::string address;
    std::string function;
};

struct old_cpu_time_t
{
    long long unsigned cpu_total_time;
    long long unsigned proc_total_time;
};

class ProcessInfo
{

public:
    ProcessInfo(pid_t pid, bool detail = false);

    // getters
    pid_t pid() const;
    const std::string name() const;
    const std::string cmdline() const;
    const std::string cwd() const;
    const std::string root() const;
    const std::string exe() const;
    const std::unordered_map<std::string, std::string> environ() const;
    int cpuUsage() const;
    const std::string userName() const;
    long unsigned int vmSize() const;

private:
    // static
    static std::unordered_map<int, struct old_cpu_time_t> map_pid_usage;

    // functions
    void readSymlinks();
    void readCmdline();
    void readStat();
    void readStatus();
    void readEnviron();
    void readIo();
    void updateCPUUsage();
    void detailedInfo();
    void readFd();
    void readCgroup();
    void readSmaps();
    void readLimits();
    void readStack();


    // friend
    friend std::ostream & operator<<(std::ostream &os, const ProcessInfo& p);

    // main properties
    std::string m_proc_path;
    pid_t m_pid;
    std::string m_name;
    process_status m_status;
    std::vector<std::string> m_cmdline;
    std::string m_cwd;
    std::string m_root;
    std::string m_exe;
    std::unordered_map<std::string, std::string> m_environ;
    struct io_stat m_io;
    std::vector<int> m_uids;
    std::vector<int> m_gids;
    int m_cpu_usage;
    /** cgroup **/
    std::vector<struct cgroup_hierarchy_t> m_cgroups;
    /** fds **/
    std::unordered_map<int, std::string> m_fds;
    /** maps **/
    std::vector<struct memory_mapping_t> m_maps;
    /** limits **/
    struct limits_t m_limits;
    /** stack **/
    std::vector<struct stack_func_t> m_stack;

    // rest


    /** The parent process ID */
    pid_t m_ppid;

    /** The process group ID of the process */
    pid_t m_pgrp;

    /** The session ID of the process */
    int m_session;

    /** The controlling terminal of the process */
    int m_tty_nr;

    /** The ID of the foreground process group of the
       * controlling terminal of the process */
    int m_tpgid;

    /** The kernel flags word of the process */
    unsigned int m_flags;

    /** The number of minor faults the process has made
       * which have not required loading a memory page from
       * disk */
    long unsigned int m_minflt;

    /** The number of minor faults that the process's
       * waited-for children have made */
    long unsigned int m_cminflt;

    /** The number of major faults the process has made
       * which have required loading a memory page from disk */
    long unsigned int m_majflt;

    /** The number of major faults that the process's
       * waited-for children have made */
    long unsigned int m_cmajflt;

    /** Amount of time that this process has been scheduled
       * in user mode */
    long unsigned int m_utime;

    /** Amount of time that this process has been scheduled
       * in kernel mode */
    long unsigned int m_stime;

    /** Amount of time that this process's waited-for
       * children have been scheduled in user mode */
    long unsigned int m_cutime;

    /** Amount of time that this process's waited-for
       * children have been scheduled in kernel mode */
    long unsigned int m_cstime;

    long int m_priority;

    long int m_nice;

    long int m_num_threads;

    long int m_itrealvalue;

    long long unsigned int m_starttime;

    long unsigned int m_vmsize;

    long int m_rss;

    long unsigned int m_rsslim;

    long unsigned int m_startcode;

    long unsigned int m_endcode;

    long unsigned int m_startstack;

    long unsigned int m_kstkesp;

    long unsigned int m_kstkeip;

    long unsigned int m_signal;

    long unsigned int m_blocked;

    long unsigned int m_siginore;

    long unsigned int m_sigcatch;

    long unsigned int m_wchan;

    long unsigned int m_nswap;

    long unsigned int m_cnswap;

    int m_exit_signal;

    int m_processor;

    unsigned int m_rt_priority;

    unsigned int m_policy;

    long long unsigned int m_delayacct_blkio_ticks;

    long unsigned int m_guest_time;

    long int m_cguest_time;

    long unsigned int m_start_data;

    long unsigned int m_end_data;

    long unsigned int m_start_brk;

    long unsigned int m_arg_start;

    long unsigned int m_arg_end;

    long unsigned int m_env_start;

    long unsigned int m_env_end;

    int m_exit_code;

    /** The process ID of any application that is debugging this one. 0 if none */
    pid_t m_tracerpid;

    /** The tty the process owns */
    std::string m_tty;

    /**
      The nice level. The range should be -20 to 20. I'm not sure
      whether this is true for all platforms.
     */
    int m_niceLevel;

    /** The i/o scheduling class and priority. */
    int m_ioPriorityClass;  /**< 0 for none, 1 for realtime, 2 for best-effort, 3 for idle.  -1 for error. */
    int m_ioPriority;       /**< Between 0 and 7.  0 is highest priority, 7 is lowest.  -1 for error. */

    /**
      The amount of physical memory the process currently uses, including the physical memory used by any
      shared libraries that it uses.  Hence 2 processes sharing a library will both report their vmRss as including
      this shared memory, even though it's only allocated once.

      This is in KiB
     */

    unsigned long m_vmRss;

    /** The amount of physical memory that is used by this process, not including any memory used by any shared libraries.
     *  This is in KiB */
    unsigned long m_vmURss;

    /**
      The number of 1/100 of a second the process has spend in user space.
      If a machine has an uptime of 1 1/2 years or longer this is not a
      good idea. I never thought that the stability of UNIX could get me
      into trouble! ;)
     */
    unsigned long m_userTime;

    /**
      The number of 1/100 of a second the process has spent in system space.
      If a machine has an uptime of 1 1/2 years or longer this is not a
      good idea. I never thought that the stability of UNIX could get me
      into trouble! ;)
     */
    unsigned long m_sysTime;

};

#endif // PROCESSINFO_H
