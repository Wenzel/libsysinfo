#ifndef MMAP_H
#define MMAP_H

#include <string>
#include <vector>
#include <sstream>

enum mapping_type
{
    shared,
    priv
};

class MMap
{
public:
    MMap(std::stringstream& ss);

    // getters
    long unsigned int addressFrom();
    long unsigned int addressTo();
    bool permRead();
    bool permWrite();
    bool permExecute();
    const std::string& path();

private:

    // properties
    long unsigned int m_address_from;
    long unsigned int m_address_to;
    bool m_perm_read;
    bool m_perm_write;
    bool m_perm_execute;
    enum mapping_type m_type;
    int m_offset;
    int m_dev_major;
    int m_dev_minor;
    long m_inode;
    std::string m_pathname;
    // smaps additional fields
    int m_size;
    int m_rss;
    int m_pss;
    int m_shared_clean;
    int m_shared_dirty;
    int m_private_clean;
    int m_private_dirty;
    int m_referenced;
    int m_anonymous;
    int m_anonhugepages;
    int m_swap;
    int m_kernelpagesize;
    int m_mmupagesize;
    int m_locked;
    std::vector<std::string> m_vmflags;
};

#endif // MMAP_H
