#include <QString>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <QDebug>

#include "processmodel.h"

ProcessModel::ProcessModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_timerId = startTimer(1000);
    updateModel();

    m_header << "PID" << "Name" << "CPU Usage" << "Command Line";
}

void ProcessModel::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    updateModel();
}

void ProcessModel::updateModel()
{
    beginResetModel();

    m_processes = processList();
    // std::reverse(m_processes.begin(), m_processes.end());

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

void ProcessModel::sort(int column, Qt::SortOrder order)
{

}
