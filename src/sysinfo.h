#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <regex>
#include <unordered_map>
#include <arpa/inet.h>
#include <sys/socket.h>

// Process

struct cpu_info_t
{
    int processor;
    std::string vendor_id;
    int cpu_family;
    int model;
    std::string model_name;
    int stepping;
    int microcode;
    float cpu_mhz;
    int cache_size;
    int physical_id;
    int siblings;
    int core_id;
    int cpu_cores;
    int apicid;
    int initial_apicid;
    bool fpu;
    bool fpu_exception;
    int cpuid_level;
    bool wp;
    std::vector<std::string> flags;
    std::string bugs;
    float bogomips;
    int clflush_size;
    int cache_alignement;
    std::string address_sizes;
    std::string power_management;
};

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

struct process_info_t {

  /** The Process ID */
    pid_t pid;

    /** The parent process ID */
    pid_t ppid;

    /** The process group ID of the process */
    pid_t pgrp;

    /** The session ID of the process */
    int session;

    /** The controlling terminal of the process */
    int tty_nr;

    /** The ID of the foreground process group of the
     * controlling terminal of the process */
    int tpgid;

    /** The kernel flags word of the process */
    unsigned int flags;

    /** The number of minor faults the process has made
     * which have not required loading a memory page from
     * disk */
    long unsigned int minflt;

    /** The number of minor faults that the process's
     * waited-for children have made */
    long unsigned int cminflt;

    /** The number of major faults the process has made
     * which have required loading a memory page from disk */
    long unsigned int majflt;

    /** The number of major faults that the process's
     * waited-for children have made */
    long unsigned int cmajflt;

    /** Amount of time that this process has been scheduled
     * in user mode */
    long unsigned int utime;

    /** Amount of time that this process has been scheduled
     * in kernel mode */
    long unsigned int stime;

    /** Amount of time that this process's waited-for
     * children have been scheduled in user mode */
    long unsigned int cutime;

    /** Amount of time that this process's waited-for
     * children have been scheduled in kernel mode */
    long unsigned int cstime;

    long int priority;

    long int nice;

    long int num_threads;

    long int itrealvalue;

    long long unsigned int starttime;

    long unsigned int vsize;

    long int rss;

    long unsigned int rsslim;

    long unsigned int startcode;

    long unsigned int endcode;

    long unsigned int startstack;

    long unsigned int kstkesp;

    long unsigned int kstkeip;

    long unsigned int signal;

    long unsigned int blocked;

    long unsigned int siginore;

    long unsigned int sigcatch;

    long unsigned int wchan;

    long unsigned int nswap;

    long unsigned int cnswap;

    int exit_signal;

    int processor;

    unsigned int rt_priority;

    unsigned int policy;

    long long unsigned int delayacct_blkio_ticks;

    long unsigned int guest_time;

    long int cguest_time;

    long unsigned int start_data;

    long unsigned int end_data;

    long unsigned int start_brk;

    long unsigned int arg_start;

    long unsigned int arg_end;

    long unsigned int env_start;

    long unsigned int env_end;

    int exit_code;

    /** The real user ID */
    uid_t uid;

    /** The real group ID */
    gid_t gid;

    /** The process ID of any application that is debugging this one. 0 if none */
    pid_t tracerpid;

    /** A character description of the process status */
    process_status status;

    /** The tty the process owns */
    std::string tty;

    /**
    The nice level. The range should be -20 to 20. I'm not sure
    whether this is true for all platforms.
   */
    int niceLevel;

    /** The i/o scheduling class and priority. */
    int ioPriorityClass;  /**< 0 for none, 1 for realtime, 2 for best-effort, 3 for idle.  -1 for error. */
    int ioPriority;       /**< Between 0 and 7.  0 is highest priority, 7 is lowest.  -1 for error. */

    /**
    The total amount of virtual memory space that this process uses. This includes shared and
    swapped memory, plus graphics memory and mmap'ed files and so on.

    This is in KiB
   */
    unsigned long vmSize;

    /**
    The amount of physical memory the process currently uses, including the physical memory used by any
    shared libraries that it uses.  Hence 2 processes sharing a library will both report their vmRss as including
    this shared memory, even though it's only allocated once.

    This is in KiB
   */

    unsigned long vmRss;

    /** The amount of physical memory that is used by this process, not including any memory used by any shared libraries.
   *  This is in KiB */
    unsigned long vmURss;

    /**
    The number of 1/100 of a second the process has spend in user space.
    If a machine has an uptime of 1 1/2 years or longer this is not a
    good idea. I never thought that the stability of UNIX could get me
    into trouble! ;)
   */
    unsigned long userTime;

    /**
    The number of 1/100 of a second the process has spent in system space.
    If a machine has an uptime of 1 1/2 years or longer this is not a
    good idea. I never thought that the stability of UNIX could get me
    into trouble! ;)
   */
    unsigned long sysTime;

    /* NOTE:  To get the user/system percentage, record the userTime and sysTime from between calls, then use the difference divided by the difference in time measure in 100th's of a second */

    /** The name of the process */
    std::string name;

    /** The command used to start the process */
    std::vector<std::string> cmdline;

    /** The login name of the user that owns this process */
    std::string userName;

    /** Process's current working directory */
    std::string cwd;

    /** env */
    std::unordered_map<std::string, std::string> env;

    /** binary full path */
    std::string exe;

    /** io stat */
    struct io_stat io;

    /** root fileystem */
    std::string root;

    // details

    /** fs **/
    std::unordered_map<int, std::string> fds;

    /** cgroup **/
    std::vector<struct cgroup_hierarchy_t> cgroups;

    /** maps **/
    std::vector<struct memory_mapping_t> maps;

    /** limits **/
    struct limits_t limits;

    /** stack **/
    std::vector<struct stack_func_t> stack;
};


std::vector<cpu_info_t> readCPUInfo();
int getNbCPUs();
int getNbCores();
int processCount();
std::vector<process_info_t> processList();
struct process_info_t getProcessDetail(pid_t pid);


// network


enum socket_state {
    TCP_ESTABLISHED = 1,
    TCP_SYN_SENT,
    TCP_SYN_RECV,
    TCP_FIN_WAIT1,
    TCP_FIN_WAIT2,
    TCP_TIME_WAIT,
    TCP_CLOSE,
    TCP_CLOSE_WAIT,
    TCP_LAST_ACK,
    TCP_LISTEN,
    TCP_CLOSING,	/* Now a valid state */
    TCP_NEW_SYN_RECV,

    TCP_MAX_STATES	/* Leave at the end! */
};

struct unix_socket_t
{
    std::string num;
    int ref_count;
    int protocol;
    int flags;
    int type;
    enum socket_state state;
    int inode;
    std::string path;
};

struct tcp_socket_t
{
    int num;
    std::string local_address;
    int local_port;
    std::string rem_address;
    int rem_port;
    int state;
    int tx_queue;
    int rx_queue;
    int timer_active;
    int tm_when;
    int retrnsmt;
    int uid;
};

std::vector<struct unix_socket_t> getSocketUNIX();
std::vector<struct tcp_socket_t> getSocketTCP();
