#include "mainwindow.h"
#include "connectiondialog.h"
#include "ssh/SSHSession.h"
#include "ui_mainwindow.h"
#include "Singleton.h"
#include "backendthread.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initUi();
    //connect(ui->on_actionConnect,&QAction::triggered,this,&MainWindow::connectTo);
}

MainWindow::~MainWindow()
{
    Singleton<BackendThread>::getInstance().stop();
    delete ui;
}

void MainWindow::initUi()
{
    mainFrame = new CentralFrame(this);
    ui->verticalLayout->addWidget(mainFrame);
    setCentralWidget(mainFrame);
}

// slots

void MainWindow::on_actionConnect_triggered()
{
    ConnectionDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    ssh::SSHOption option(dialog.host().toStdString(),dialog.port(), dialog.user().toStdString(), dialog.password().toStdString());
    mainFrame->addSession(option);
}
