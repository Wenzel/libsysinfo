#include <QString>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <QDebug>
#include  <iostream>

#include "processmodel.h"

ProcessModel::ProcessModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_header << "PID" << "Name" << "CPU Usage" << "Command Line";
    m_timerId = startTimer(1000);

    /* not working
    sysinfoInit();
    addCallbackProcessEvent(std::bind(&ProcessModel::callback, this, std::placeholders::_1));
    startProcessEventListening();
    */

    updateModel();
}

void ProcessModel::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    updateModel();
}

void ProcessModel::updateModel()
{
    beginResetModel();
    // fill m_processes first time
    m_processes = processList();

    endResetModel();
}

int ProcessModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return m_processes.size();
}

int ProcessModel::columnCount(const QModelIndex &parent) const
{
    return 4;
}

QVariant ProcessModel::data(const QModelIndex & index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    const struct process_info_t &process = m_processes[index.row()];

    switch (index.column())
    {
    case 0:
        return process.pid;
    case 1:
        return QString::fromUtf8(process.name.data(), process.name.size());
    case 2:
        return process.cpu_usage;
    case 3:
        const char* delim = " ";
        std::stringstream res;
        std::vector<std::string> cmdline = process.cmdline;
        std::copy(cmdline.begin(), cmdline.end(), std::ostream_iterator<std::string>(res, delim));
        return QString::fromUtf8(res.str().data(), res.str().size());
    }

    return QVariant();
}

QVariant ProcessModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal and role == Qt::DisplayRole)
        return m_header[section];
    return QVariant();
}

void ProcessModel::callback(proc_event event)
{
    pid_t pid;
    struct process_info_t pinfo;
    switch(event.what)
    {
    case event.PROC_EVENT_FORK:
    {
        pid = event.event_data.fork.child_pid;
        getProcess(pid, &pinfo);
        std::cout << "FORK , child :" << pid << ", " << pinfo.name << std::endl;
        // insert into m_processes
        int index = m_processes.size() - 1;
        beginInsertRows(QModelIndex(), index, index);
        m_processes.push_back(pinfo);
        endInsertRows();
        break;
    }
    case event.PROC_EVENT_EXIT:
    {
        pid = event.event_data.exit.process_pid;
        // remove from m_processes
        for (std::vector<struct process_info_t>::iterator it = m_processes.begin(); it != m_processes.end(); it++)
            if ((*it).pid == pid)
            {
                std::cout << "EXIT : " << pid << ", " << pinfo.name << std::endl;
                int index = std::distance(m_processes.begin(), it);
                beginRemoveRows(QModelIndex(), index, index);
                m_processes.erase(it);
                endRemoveRows();
                break;
            }
        break;
    }
    case event.PROC_EVENT_EXEC:
    {
        pid = event.event_data.exec.process_pid;
        getProcess(pid, &pinfo);
        std::cout << "EXEC " << pid << ", " << pinfo.name << std::endl;
        break;
    }
    default:
        break;
    }
}
