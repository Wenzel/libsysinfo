#ifndef PROCESSMODEL_H
#define PROCESSMODEL_H

#include <QAbstractListModel>
#include <QAbstractTableModel>
#include <QHash>
#include <QByteArray>

#include "sysinfo.h"

class ProcessModel : public QAbstractListModel
{
    Q_OBJECT
public:

    ProcessModel(QObject *parent = 0);

    void timerEvent(QTimerEvent *event);
    void updateModel();

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void callback(struct proc_event event);
private:


    std::vector<struct process_info_t> m_processes;
    int m_timerId;
    QStringList m_header;
};

#endif // PROCESSMODEL_H
