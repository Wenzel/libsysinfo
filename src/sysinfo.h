#ifndef SYSINFO_H
#define SYSINFO_H

#include <thread>
#include <functional>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <regex>
#include <unordered_map>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "procconnector.h"
#include "processinfo.h"

// fwd
class ProcessInfo;

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

// System information
std::vector<cpu_info_t> readCPUInfo();
int getNbCPUs();
int getNbCores();

// Process

std::vector<int> processListPid();
int processCount();
std::vector<ProcessInfo> processList();
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

#endif
