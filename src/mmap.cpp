#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>

#include "mmap.h"

MMap::MMap(std::stringstream& ss)
{
    std::string map_declaration;
    std::getline(ss, map_declaration);
    // parse declaration
    boost::regex regex_declare_mapping("^([[:xdigit:]]+)-([[:xdigit:]]+)\\s([r-])([w-])([x-])([sp])\\s([[:xdigit:]]+)\\s([[:xdigit:]]+):([[:xdigit:]]+)\\s([[:digit:]]+)\\s+(.*)$");
    boost::regex regex_key_value("^([[:alpha:]]+):\\s+(.*)$");
    boost::smatch match;
    if (boost::regex_match(map_declaration, match, regex_declare_mapping))
    {
        if (match.size() == 11 + 1)
        {
            m_address_from = match[1];
            m_address_to = match[2];
            (match[3] == "r") ? m_perm_read = true : m_perm_read = false;
            (match[4] == "w") ? m_perm_write = true : m_perm_write = false;
            (match[5] == "x") ? m_perm_execute = true : m_perm_execute = false;
            (match[6] == "p") ? m_type = priv : m_type = shared;
            m_offset = std::stol(match[7]);
            m_dev_major = std::stoi(match[8], 0, 16);
            m_dev_minor = std::stoi(match[9], 0, 16);
            m_inode = std::stol(match[10]);
            m_pathname = match[11];
        }
    }
    // parse attributes
    std::string line;
    while (std::getline(ss, line))
    {
        if (boost::regex_match(line, match, regex_key_value))
        {
            if (match.size() == 2 + 1)
            {
                std::string value(match[2]);
                boost::algorithm::trim(value);
                boost::regex regex_value("^([[:digit:]])\\s.*$");
                boost::smatch match_value;
                if (boost::regex_match(value, match_value, regex_value))
                {
                    // 4 kB
                    if (match[1] == "Size")
                        m_size = std::stoi(match_value[1]);
                    else if (match[1] == "Rss")
                        m_rss = std::stoi(match_value[1]);
                    else if (match[1] == "Pss")
                        m_pss = std::stoi(match_value[1]);
                    else if (match[1] == "Shared_Clean")
                        m_shared_clean = std::stoi(match_value[1]);
                    else if (match[1] == "Shared_Dirty")
                        m_shared_dirty = std::stoi(match_value[1]);
                    else if (match[1] == "Private_Clean")
                        m_private_clean = std::stoi(match_value[1]);
                    else if (match[1] == "Private_Dirty")
                        m_private_dirty = std::stoi(match_value[1]);
                    else if (match[1] == "Referenced")
                        m_referenced = std::stoi(match_value[1]);
                    else if (match[1] == "Anonymous")
                        m_anonymous = std::stoi(match_value[1]);
                    else if (match[1] == "AnonHugePages")
                        m_anonhugepages = std::stoi(match_value[1]);
                    else if (match[1] == "Swap")
                        m_swap = std::stoi(match_value[1]);
                    else if (match[1] == "KernelPageSize")
                        m_kernelpagesize = std::stoi(match_value[1]);
                    else if (match[1] == "MMUPageSize")
                        m_mmupagesize = std::stoi(match_value[1]);
                    else if (match[1] == "Locked")
                        m_locked = std::stoi(match_value[1]);
                } else
                {
                    // VmFlags special case
                    boost::split(m_vmflags, value, boost::is_any_of(" "));
                }
            }
        }
    }
    defineCategory();
}

// getters
const std::string& MMap::addressFrom() const { return m_address_from; }
const std::string& MMap::addressTo() const { return m_address_to; }
bool MMap::permRead() const { return m_perm_read; }
bool MMap::permWrite() const { return m_perm_write; }
bool MMap::permExecute() const { return m_perm_execute; }
const std::string MMap::permissions() const
{
    std::string permissions;
    (m_perm_read) ? permissions.append("r") : permissions.append("-");
    (m_perm_write) ? permissions.append("w") : permissions.append("-");
    (m_perm_execute) ? permissions.append("x") : permissions.append("-");
    return permissions;
}

const std::string MMap::type() const
{
    if (m_type == shared)
        return "Shared";
    else
        return "Private";
}

const std::vector<std::string>& MMap::vmFlags() const
{
    return m_vmflags;
}

const std::string& MMap::path() const { return m_pathname; }

int MMap::size() const { return m_size; }

const std::string& MMap::category() const
{
    return m_category;
}

// TODO check with ProcessInfo context
void MMap::defineCategory()
{
    if (m_pathname.find("/usr/bin") != std::string::npos
            || m_pathname.find("/usr/lib") != std::string::npos)
        m_category = "Image";
    else if (m_pathname.find("[heap]") != std::string::npos)
        m_category = "Heap";
    else if (m_pathname.find("[stack]") != std::string::npos)
        m_category = "Stack";
    else if (m_pathname.empty())
        m_category = "Private";
    else if (m_type == shared)
        m_category = "Shareable";
}
