#include "sysinfo.h"
#include "socketunixmodel.h"

SocketUNIXModel::SocketUNIXModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_timerId = startTimer(1000);
    updateModel();
}

void SocketUNIXModel::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    updateModel();
}

void SocketUNIXModel::updateModel()
{
    beginResetModel();

    m_unix_socket_list = getSocketUNIX();

    endResetModel();
}

QHash<int, QByteArray> SocketUNIXModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NumRole] = "num";
    roles[RefCountRole] = "refcount";
    roles[ProtocolRole] = "protocol";
    roles[FlagsRole] = "flags";
    roles[TypeRole] = "type";
    roles[StateRole] = "state";
    roles[InodeRole] = "inode";
    roles[PathRole] = "path";
    return roles;
}

int SocketUNIXModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_unix_socket_list.size();
}

QVariant SocketUNIXModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= m_unix_socket_list.size())
        return QVariant();

    const struct unix_socket_t &unix_socket = m_unix_socket_list[index.row()];
    switch (role)
    {
    case NumRole:
        return QString::fromUtf8(unix_socket.num.data(), unix_socket.num.size());
    case RefCountRole:
        return unix_socket.ref_count;
    case ProtocolRole:
        return unix_socket.protocol;
    case FlagsRole:
        return unix_socket.flags;
    case TypeRole:
        return unix_socket.type;
    case StateRole:
        return unix_socket.state;
    case InodeRole:
        return unix_socket.inode;
    case PathRole:
        return QString::fromUtf8(unix_socket.path.data(), unix_socket.path.size());
    }

    return QVariant();
}
