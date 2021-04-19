#ifndef SESSIONTAB_H
#define SESSIONTAB_H

#include "ssh/SFTP.h"
#include "ssh/SFTPSession.h"
#include "ssh/SSHSession.h"

#include "backendthread.h"

#include <mutex>
#include <vector>
#include <QItemSelectionModel>
#include <QStandardItemModel>
#include <QWidget>

namespace Ui {
class SessionTab;
}

class SessionTab : public QWidget
{
    Q_OBJECT

public:
    explicit SessionTab(const ssh::SSHOption &option, QWidget *parent = nullptr);
    ~SessionTab();

    bool openDir(const QString& dir);

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_lineEdit_returnPressed();

    void on_preBtn_clicked();

private:
    void startSession();
    void updateModel(std::vector<ssh::SFTPAttr> &attrs);

private:
    Ui::SessionTab *ui;
    std::mutex m_mtx; // for m_model
    QStandardItemModel *m_model;
    QItemSelectionModel *m_selection;

    ssh::SSHOption *m_sshOption;
    ssh::SSHSession *m_sshSession;
    ssh::SFTPSession *m_sftpSession;
    ssh::SFTP *m_sftp;

    BackendThread &m_backend;
    QString m_curDir;
    std::vector<QString> m_historyDir;
};

#endif // SESSIONTAB_H
