#ifndef PROCESSMODEL_H
#define PROCESSMODEL_H

#include <QAbstractListModel>
#include <QHash>
#include <QByteArray>

#include "sysinfo.h"

class ProcessModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum ProcessRoles {
        PidRole = Qt::UserRole + 1,
        NameRole,
        CmdlineRole
    };

    ProcessModel(QObject *parent = 0);

    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
private:
    QList<struct process_info_t> m_processes;

};

#endif // PROCESSMODEL_H
