#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include <iostream>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <ctime>

#include <sys/types.h>
#include <pwd.h>
#include <linux/kdev_t.h>

#include "mmap.h"
#include "sysinfo.h"

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
    // time when we last read the file
    std::time_t last_read;
};

struct cgroup_hierarchy_t
{
    int hierarchy_id;
    std::vector<std::string> subsystems;
    std::string cgroup;

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
    ProcessInfo();
    ProcessInfo(pid_t pid);
    ~ProcessInfo();

    void needUpdate();

    // getters
    // from stat
    pid_t pid() const;
    const std::string name();
    const std::string& state();
    pid_t ppid();
    int pgid();
    int sid();
    const std::string ttyNr();
    int tpgid();
    unsigned int flags();
    long unsigned int minflt();
    long unsigned int cminflt();
    long unsigned int majflt();
    long unsigned int cmajflt();
    long unsigned int utime();
    long unsigned int stime();
    long unsigned int cutime();
    long unsigned int cstime();
    long int priority();
    long int nice();
    long int numThreads();
    long long unsigned int startTime();
    long unsigned int vmSize();
    long unsigned int startCode();
    long unsigned int endCode();
    long unsigned int startStack();
    long unsigned int kstkEsp();
    long unsigned int kstkEip();
    long unsigned int wchanAddr();
    int processor();
    unsigned int rtPriority();
    std::string policy();
    long long unsigned int delayacctBlkioTicks();
    long unsigned int guestTime();
    long unsigned int cguestTime();
    long unsigned int startData();
    long unsigned int endData();
    long unsigned int startBrk();
    long unsigned int startArg();
    long unsigned int endArg();
    long unsigned int startEnv();
    long unsigned int endEnv();

    // from status
    const std::vector<int>& uids();
    const std::vector<int>& gids();
    long unsigned int vmPeak();
    long unsigned int vmLck();
    long unsigned int vmPin();
    long unsigned int vmHwm();
    long unsigned int vmRss();
    long unsigned int vmData();
    long unsigned int vmStk();
    long unsigned int vmExe();
    long unsigned int vmLib();
    long unsigned int vmPte();
    long unsigned int vmPmd();
    long unsigned int vmSwap();

    // from cmdline
    const std::vector<std::string>& cmdline();

    // from cwd
    const std::string cwd();

    // from exe
    const std::string exe();

    // from root
    const std::string root();

    // from environ
    const std::unordered_map<std::string, std::string> environ();

    // from io
    long unsigned int readBytes();
    long unsigned int writeBytes();

    // from fd/
    const std::unordered_map<int, std::string>& fds();

    // from wchan
    const std::string wchanName();

    // from smaps
    const std::vector<MMap>& maps();

    // computed
    int cpuUsage();
    const std::string userName();

private:
    // static
    static std::unordered_map<int, struct old_cpu_time_t> map_pid_usage;

    // functions
    void readCwd();
    void readCmdline();
    void readExe();
    void readRoot();
    void readStat();
    void readStatus();
    void readEnviron();
    void readIo();
    void readWchan();
    void updateCPUUsage();
    void readFd();
    void readCgroup();
    void readSmaps();
    void readLimits();
    void readStack();


    // friend
    friend std::ostream & operator<<(std::ostream &os, ProcessInfo& p);

    // properties
    std::string m_proc_path;
    bool m_need_update_stat;
    bool m_need_update_status;
    bool m_need_update_io;
    bool m_need_update_cwd;
    bool m_need_update_exe;
    bool m_need_update_cmdline;
    bool m_need_update_root;
    bool m_need_update_environ;
    bool m_need_update_fd;
    bool m_need_update_wchan;
    bool m_need_update_smaps;
    bool m_need_update_cpu_usage;

    // from stat
    pid_t m_pid;
    std::string m_name;
    std::string m_state;
    pid_t m_ppid;
    pid_t m_pgid;
    int m_session;
    int m_tty_nr;
    int m_tpgid;
    unsigned int m_flags;
    long unsigned int m_minflt;
    long unsigned int m_cminflt;
    long unsigned int m_majflt;
    long unsigned int m_cmajflt;
    long unsigned int m_utime;
    long unsigned int m_stime;
    long unsigned int m_cutime;
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
    long unsigned int m_wchan_addr;
    long unsigned int m_nswap;
    long unsigned int m_cnswap;
    int m_exit_signal;
    int m_processor;
    unsigned int m_rt_priority;
    unsigned int m_policy;
    long long unsigned int m_delayacct_blkio_ticks;
    long unsigned int m_start_data;
    long unsigned int m_end_data;
    long unsigned int m_start_brk;
    long unsigned int m_arg_start;
    long unsigned int m_arg_end;
    long unsigned int m_env_start;
    long unsigned int m_env_end;
    int m_exit_code;
    long unsigned int m_guest_time;
    long int m_cguest_time;

    // from cmdline
    std::vector<std::string> m_cmdline;
    // from cwd
    std::string m_cwd;
    // from exe
    std::string m_exe;
    // from root
    std::string m_root;
    // from environ
    std::unordered_map<std::string, std::string> m_environ;
    // from io
    struct io_stat m_io;
    // from wchan
    std::string m_wchan_name;
    // from status
    pid_t m_tracerpid;
    std::vector<int> m_uids;
    std::vector<int> m_gids;
    long unsigned int m_vm_peak;
    long unsigned int m_vm_lck;
    long unsigned int m_vm_pin;
    long unsigned int m_vm_hwm;
    long unsigned int m_vm_rss;
    long unsigned int m_vm_data;
    long unsigned int m_vm_stk;
    long unsigned int m_vm_exe;
    long unsigned int m_vm_lib;
    long unsigned int m_vm_pte;
    long unsigned int m_vm_pmd;
    long unsigned int m_vm_swap;

    // from cgroup
    std::vector<struct cgroup_hierarchy_t> m_cgroups;
    // from fd
    std::unordered_map<int, std::string> m_fds;
    // from smaps
    std::vector<MMap> m_maps;
    // from limits
    struct limits_t m_limits;
    // from stack
    std::vector<struct stack_func_t> m_stack;
    // computed
    int m_cpu_usage;


    /**
      The nice level. The range should be -20 to 20. I'm not sure
      whether this is true for all platforms.
      /
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
