#ifndef SOCKETUNIXMODEL_H
#define SOCKETUNIXMODEL_H

#include <QAbstractListModel>
#include <QHash>
#include <QByteArray>

class SocketUNIXModel : public QAbstractListModel
{
    Q_OBJECT
public:
    SocketUNIXModel(QObject *parent = 0);

    enum ProcessRoles {
        NumRole = Qt::UserRole + 1,
        RefCountRole,
        ProtocolRole,
        FlagsRole,
        TypeRole,
        StateRole,
        InodeRole,
        PathRole
    };

    void timerEvent(QTimerEvent *event);
    void updateModel();


    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

private:
    std::vector<struct unix_socket_t> m_unix_socket_list;
    int m_timerId;
};

#endif // SOCKETUNIXMODEL_H
