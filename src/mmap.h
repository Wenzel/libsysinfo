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
    const std::string& addressFrom() const;
    const std::string& addressTo() const;
    bool permRead() const;
    bool permWrite() const;
    bool permExecute() const;
    const std::string permissions() const;
    const std::string type() const;
    const std::string& path() const;
    const std::vector<std::string>& vmFlags() const;
    int size() const;
    const std::string& category() const;

private:

    void defineCategory();

    // properties
    std::string m_address_from;
    std::string m_address_to;
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
    std::string m_category;


};

#endif // MMAP_H
