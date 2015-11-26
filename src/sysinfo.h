#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <regex>

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
    sleeping
};

typedef struct {

  /** The parent process ID */
  pid_t ppid;

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

  /** The scheduling priority. */
  int priority;

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

} ProcessInfo;

std::vector<cpu_info_t> readCPUInfo();

int getNbCPUs();
int getNbCores();
int processCount();
std::vector<ProcessInfo> ProcessList();
