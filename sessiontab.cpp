#include "sessiontab.h"
#include "Singleton.h"
#include "ui_sessiontab.h"
#include <algorithm>
#include <string>
#include <time.h>
#include <QDebug>
#include <QMessageBox>

SessionTab::SessionTab(const ssh::SSHOption &option, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SessionTab)
    , m_mtx()
    , m_sshOption(new ssh::SSHOption(option))
    , m_backend(Singleton<BackendThread>::getInstance())

{
    ui->setupUi(this);

    m_model = new QStandardItemModel(ui->tableView);
    m_model->setColumnCount(4);
    m_model->setHeaderData(0, Qt::Horizontal, "Name");
    m_model->setHeaderData(1, Qt::Horizontal, "Type");
    m_model->setHeaderData(2, Qt::Horizontal, "size");
    m_model->setHeaderData(3, Qt::Horizontal, "modified time");

    //m_selection = new QItemSelectionModel(m_model);
    //ui->tableView->setSelectionModel(m_selection);
    //ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableView->setModel(m_model);

    m_backend.addTask(std::bind(&SessionTab::startSession, this));
}

SessionTab::~SessionTab()
{
    delete ui;
    delete m_sftpSession;
    delete m_sshSession;
    delete m_sshOption;
}

void SessionTab::startSession()
{
    m_sshSession = new ssh::SSHSession(m_sshOption);

    if (!m_sshSession->connect()) {
        qDebug() << "connect failed";
        //        QMessageBox::warning(nullptr,
        //                             "Warning",
        //                             "Connect failed",
        //                             QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes);
        return;
    }

    if (!m_sshSession->authentication(true)) {
        qDebug() << "authentication failed";
        //        QMessageBox::warning(nullptr,
        //                             "Warning",
        //                             "Authenticate failed",
        //                             QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes);
        return;
    }

    m_sftpSession = new ssh::SFTPSession(m_sshSession);
    if (!m_sftpSession->init()) {
        qDebug() << "SFTPSession init failed";
        //        QMessageBox::warning(nullptr,
        //                             "Warning",
        //                             "SFTPSession init failed",
        //                             QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes);
        return;
    }

    m_sftp = new ssh::SFTP(m_sshSession, m_sftpSession);

    m_curDir.clear();
    m_curDir.append("/home/");
    m_curDir.append(m_sshOption->user.c_str());
    if (openDir(m_curDir)) {
        m_historyDir.push_back(m_curDir);
        ui->lineEdit->setText(m_curDir);
    }
}

void SessionTab::updateModel(std::vector<ssh::SFTPAttr> &attrs)
{
    qDebug() << "updateModel";

    std::lock_guard<std::mutex> lock(m_mtx);
    m_model->removeRows(0, m_model->rowCount());
    int i = 0;
    for (auto &attr : attrs) {
        if (attr.name[0] == '.') {
            continue;
        }
        m_model->insertRows(i, 1);
        m_model->setData(m_model->index(i, 0), attr.name.c_str(), Qt::DisplayRole);
        m_model->setData(m_model->index(i, 1), attr.type, Qt::DisplayRole);

        int size_kb = (attr.size + 1023) / 1024;
        m_model->setData(m_model->index(i, 2), size_kb, Qt::DisplayRole);

        char buf[64];
        long long t = attr.mtime;
        struct tm *info = localtime(&t);
        strftime(buf, sizeof buf, "%Y-%m-%d %H:%M:%S", info);
        m_model->setData(m_model->index(i, 3), buf, Qt::DisplayRole);
        ++i;
    }
}

bool SessionTab::openDir(const QString &dir)
{
    std::vector<ssh::SFTPAttr> attrs;
    bool success = m_sftp->listDir(dir.toStdString(), attrs);
    if (success) {
        updateModel(attrs);
        ui->lineEdit->setText(dir);
    }
    return success;
}

void SessionTab::on_tableView_doubleClicked(const QModelIndex &index)
{
    QAbstractItemModel *model = ui->tableView->model();
    QModelIndex idx = model->index(index.row(), 0);
    QModelIndex idx2 = model->index(index.row(), 1);
    QString str = model->data(idx).toString();
    int type = model->data(idx2).toInt();

    if (type == 2) {
        m_curDir.append('/');
        m_curDir.append(str);
        if (openDir(m_curDir)) {
            m_historyDir.push_back(m_curDir);
        }
    }
}

void SessionTab::on_lineEdit_returnPressed()
{
    m_curDir = ui->lineEdit->text();
    openDir(m_curDir);
}

void SessionTab::on_preBtn_clicked()
{
    for (auto &str : m_historyDir) {
        qDebug() << str;
    }

    int size = m_historyDir.size();
    if (size > 1) {
        m_curDir = m_historyDir[size - 2];
        m_historyDir.pop_back();
        openDir(m_curDir);
    }
}
