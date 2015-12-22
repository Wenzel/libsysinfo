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
    return m_processes.size();
}

QVariant ProcessModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= m_processes.size())
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


QVariant ProcessModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    qDebug() << "Hello World!";
}
