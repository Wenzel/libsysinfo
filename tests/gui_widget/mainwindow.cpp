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
    center();

    m_model = new ProcessModel(this);
    m_proxy = new QSortFilterProxyModel(this);
    m_proxy->setSourceModel(m_model);
    ui->processView->setModel(m_proxy);
}

void MainWindow::center()
{
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
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_model;
    delete m_proxy;
}
