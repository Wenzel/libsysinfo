#include <QString>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>

#include "processmodel.h"

ProcessModel::ProcessModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_timerId = startTimer(1000);
    updateModel();
}

void ProcessModel::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    updateModel();
}

void ProcessModel::updateModel()
{
    m_processes.clear();

    beginResetModel();
    // get data
    std::vector<struct process_info_t> processes = processList();

    // update model (insert in reversed order to see new processes)
    for (int i = processes.size() - 1; i >= 0; i--)
        m_processes.append(processes[i]);

    endResetModel();
}

QHash<int, QByteArray> ProcessModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[PidRole] = "pid";
    roles[NameRole] = "name";
    roles[CmdlineRole] = "cmdline";
    return roles;
}

int ProcessModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_processes.count();
}

QVariant ProcessModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= m_processes.count())
        return QVariant();

    const struct process_info_t &process = m_processes[index.row()];
    switch (role)
    {
    case PidRole:
        return process.pid;
    case NameRole:
        return QString::fromUtf8(process.name.data(), process.name.size());
    case CmdlineRole:
        const char* delim = " ";
        std::stringstream res;
        std::vector<std::string> cmdline = process.cmdline;
        std::copy(cmdline.begin(), cmdline.end(), std::ostream_iterator<std::string>(res, delim));
        return QString::fromUtf8(res.str().data(), res.str().size());
    }

    return QVariant();
}
/*
QVariant ProcessModel::headerData(int section, Qt::Orientation orientation, int role) const
{

}
*/
