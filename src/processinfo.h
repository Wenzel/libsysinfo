#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include <iostream>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>

#include <sys/types.h>
#include <pwd.h>
#include <linux/kdev_t.h>

/* stolen from linux/sched.h
* Per process flags
*/
#define PF_EXITING      0x00000004      /* getting shut down */
#define PF_EXITPIDONE   0x00000008      /* pi exit done on shut down */
#define PF_VCPU         0x00000010      /* I'm a virtual CPU */
#define PF_WQ_WORKER    0x00000020      /* I'm a workqueue worker */
#define PF_FORKNOEXEC   0x00000040      /* forked but didn't exec */
#define PF_MCE_PROCESS  0x00000080      /* process policy on mce errors */
#define PF_SUPERPRIV    0x00000100      /* used super-user privileges */
#define PF_DUMPCORE     0x00000200      /* dumped core */
#define PF_SIGNALED     0x00000400      /* killed by a signal */
#define PF_MEMALLOC     0x00000800      /* Allocating memory */
#define PF_NPROC_EXCEEDED 0x00001000    /* set_user noticed that RLIMIT_NPROC was exceeded */
#define PF_USED_MATH    0x00002000      /* if unset the fpu must be initialized before use */
#define PF_USED_ASYNC   0x00004000      /* used async_schedule*(), used by module init */
#define PF_NOFREEZE     0x00008000      /* this thread should not be frozen */
#define PF_FROZEN       0x00010000      /* frozen for system suspend */
#define PF_FSTRANS      0x00020000      /* inside a filesystem transaction */
#define PF_KSWAPD       0x00040000      /* I am kswapd */
#define PF_MEMALLOC_NOIO 0x00080000     /* Allocating memory without IO involved */
#define PF_LESS_THROTTLE 0x00100000     /* Throttle me less: I clean memory */
#define PF_KTHREAD      0x00200000      /* I am a kernel thread */
#define PF_RANDOMIZE    0x00400000      /* randomize virtual address space */
#define PF_SWAPWRITE    0x00800000      /* Allowed to write to swap */
#define PF_NO_SETAFFINITY 0x04000000    /* Userland is not allowed to meddle with cpus_allowed */
#define PF_MCE_EARLY    0x08000000      /* Early kill for mce process policy */
#define PF_MUTEX_TESTER 0x20000000      /* Thread belongs to the rt mutex tester */
#define PF_FREEZER_SKIP 0x40000000      /* Freezer should not count it as freezable */
#define PF_SUSPEND_TASK 0x80000000      /* this thread called freeze_processes and should not be frozen */


#define SCHED_NORMAL            0
#define SCHED_FIFO              1
#define SCHED_RR                2
#define SCHED_BATCH             3
/* SCHED_ISO: reserved but not implemented yet */
#define SCHED_IDLE              5
#define SCHED_DEADLINE          6

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
    ProcessInfo(pid_t pid);

    // getters
    pid_t pid() const;
    pid_t ppid() const;
    int pgid() const;
    int sid() const;
    const std::string ttyNr() const;
    int tpgid() const;
    const std::vector<int>& uids() const;
    const std::vector<int>& gids() const;
    const std::string name() const;
    const std::vector<std::string>& cmdline() const;
    const std::string cwd() const;    const std::string root() const;
    const std::string exe() const;
    const std::unordered_map<std::string, std::string> environ();
    int cpuUsage() const;
    const std::string userName() const;
    long unsigned int vmSize() const;
    long long unsigned int startTime() const;
    const std::unordered_map<int, std::string>& fds();
    const std::string& state() const;
    unsigned int flags() const;
    long unsigned int minflt() const;
    long unsigned int cminflt() const;
    long unsigned int majflt() const;
    long unsigned int cmajflt() const;
    long unsigned int utime() const;
    long unsigned int stime() const;
    long unsigned int cutime() const;
    long unsigned int cstime() const;
    long unsigned int guestTime() const;
    long unsigned int cguestTime() const;
    long int priority() const;
    unsigned int rtPriority() const;
    long int nice() const;
    long int numThreads() const;
    int processor() const;
    long long unsigned int delayacctBlkioTicks() const;
    std::string policy() const;


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
    void readFd();
    void readCgroup();
    void readSmaps();
    void readLimits();
    void readStack();


    // friend
    friend std::ostream & operator<<(std::ostream &os, ProcessInfo& p);

    // main properties
    std::string m_proc_path;
    pid_t m_pid;
    std::string m_name;
    std::string m_state;
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
    pid_t m_pgid;

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
