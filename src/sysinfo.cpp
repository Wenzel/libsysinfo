#include <algorithm>

#include "sysinfo.h"

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

std::vector<cpu_info_t> readCPUInfo()
{
    std::vector<cpu_info_t> cpuinfo = std::vector<cpu_info_t>();
    // open /proc/cpuinfo
    std::ifstream if_cpuinfo("/proc/cpuinfo");

    // read until end
    std::string cur_line;
    // add first cpu_info_t
    cpu_info_t cur_cpuinfo = cpu_info_t();
    // for each line
    while (std::getline(if_cpuinfo, cur_line))
    {
        if (cur_line.empty()) // blank line separator
        {
            cpuinfo.push_back(cur_cpuinfo);
            if (!if_cpuinfo.eof())
                cur_cpuinfo = cpu_info_t();
            continue;
        }
        std::istringstream line_stream(cur_line);
        // read key
        std::string key;
        std::getline(line_stream, key, ':');
        key = trim(key);
        // read value
        std::string value;
        value = trim(value);
        std::getline(line_stream, value);

        if (key == "processor")
            cur_cpuinfo.processor = std::stoi(value);
        else if (key == "vendor_id")
            cur_cpuinfo.vendor_id = value;
        else if (key == "cpu family")
            cur_cpuinfo.cpu_family = std::stoi(value);
        else if (key == "model")
            cur_cpuinfo.model = std::stoi(value);
        else if (key == "model name")
            cur_cpuinfo.model_name = value;
        else if (key == "stepping")
            cur_cpuinfo.stepping = std::stoi(value);
        else if (key == "microcode")
            cur_cpuinfo.microcode = std::stoi(value);
        else if (key == "cpu MHz")
            cur_cpuinfo.cpu_mhz = std::stol(value);
        else if (key == "cache size")
            cur_cpuinfo.cache_size = 0; // todo KB, MB
        else if (key == "physical id")
            cur_cpuinfo.physical_id = std::stoi(value);
        else if (key == "sibblings")
            cur_cpuinfo.siblings = std::stoi(value);
        else if (key == "core id")
            cur_cpuinfo.core_id = std::stoi(value);
        else if (key == "cpu cores")
            cur_cpuinfo.cpu_cores = std::stoi(value);
        else if (key == "apicid")
            cur_cpuinfo.apicid = std::stoi(value);
        else if (key == "initial apicid")
            cur_cpuinfo.initial_apicid = std::stoi(value);
        else if (key == "fpu")
            cur_cpuinfo.fpu = ((value == "yes") ? true : false);
        else if (key == "fpu_exception")
            cur_cpuinfo.fpu_exception = ((value == "yes") ? true : false);
        else if (key == "cpuid_level")
            cur_cpuinfo.cpuid_level = std::stoi(value);
        else if (key == "wp")
            cur_cpuinfo.wp = ((value == "yes") ? true : false);
        else if (key == "flags")
        {
            std::istringstream flags_stream(value);
            std::string cur_flag;
            while (flags_stream >> cur_flag)
                cur_cpuinfo.flags.push_back(cur_flag);
        }
        else if (key == "bugs")
            cur_cpuinfo.bugs = value;
        else if (key == "bogomips")
            cur_cpuinfo.bogomips = std::stol(value);
        else if (key == "clflush size")
            cur_cpuinfo.clflush_size = std::stoi(value);
        else if (key == "cache_alignement")
            cur_cpuinfo.cache_alignement = std::stoi(value);
        else if (key == "address sizes")
            cur_cpuinfo.address_sizes = value;
        else if (key == "power management")
            cur_cpuinfo.power_management = value;
    }


    // close
    if_cpuinfo.close();

    // return data
    return cpuinfo;
}

int getNbCPUs()
{
    std::vector<cpu_info_t> cpuinfo = readCPUInfo();

    int nbCPUs = 0;
    for (cpu_info_t core : cpuinfo)
    {
        if (core.core_id == 0) // new processor
            nbCPUs++;
    }

    return nbCPUs;
}

int getNbCores()
{
    std::vector<cpu_info_t> cpuinfo = readCPUInfo();

    return cpuinfo.size();
}
