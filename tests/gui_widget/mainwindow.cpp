#include <QStyle>
#include <QDesktopWidget>
#include <QTableView>
#include <sysinfo.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // size 70%
    QDesktopWidget dw;

    int x=dw.width()*0.7;
    int y=dw.height()*0.7;
    this->setFixedSize(x,y);

    // center
    this->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            this->size(),
            qApp->desktop()->availableGeometry()
        )
    );

    timerId = startTimer(1000);
    model = new QStandardItemModel(this);
    model->setHorizontalHeaderItem(0, new QStandardItem(QString("PID")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString("Name")));
    model->setHorizontalHeaderItem(2, new QStandardItem(QString("Command Line")));



    updateModel();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete model;
}

void MainWindow::updateModel()
{

    for (int i = 0; i < model->rowCount(); i++)
        for (int j = 0; j < model->columnCount(); j++)
            delete model->takeItem(i, j);

    std::vector<struct process_info_t> proc_list = processList();
    for (unsigned int i = 0; i < proc_list.size(); i++)
    {
        model->setItem(i, 0, new QStandardItem(QString::number(proc_list[i].pid)));
        model->setItem(i, 1, new QStandardItem(QString::fromStdString(proc_list[i].name)));
        QStringList cmdline;
        for (std::string arg: proc_list[i].cmdline)
            cmdline << QString::fromStdString(arg);
        model->setItem(i, 2, new QStandardItem(cmdline.join(' ')));
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    updateModel();
}
