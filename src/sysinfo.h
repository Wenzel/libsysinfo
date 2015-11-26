#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <sstream>

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

std::vector<cpu_info_t> readCPUInfo();

int getNbCPUs();
int getNbCores();
