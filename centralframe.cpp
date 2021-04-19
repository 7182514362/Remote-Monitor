#include "centralframe.h"
#include "sessiontab.h"
#include "ssh/SSHSession.h"
#include <string>
#include <QDebug>
#include <QMessageBox>
#include <QSizePolicy>

CentralFrame::CentralFrame(QWidget *parent)
    : QWidget(parent)
{
    initUI();
}

void CentralFrame::initUI()
{
    vBoxLayout = new QVBoxLayout(this);
    splitter = new QSplitter(Qt::Horizontal, this);
    vBoxLayout->addWidget(splitter);

    connListWidget = new QListWidget(this);
    connListWidget->setMaximumWidth(200);
    connListWidget->setMinimumWidth(150);
    connListWidget->setMinimumHeight(400);
    splitter->addWidget(connListWidget);

    connect(connListWidget, &QListWidget::itemDoubleClicked, this, &CentralFrame::on_item_double_clicked);

    connTabWidget = new QTabWidget(this);
    connTabWidget->setMinimumWidth(400);
    connTabWidget->setMinimumHeight(400);
    connTabWidget->setTabsClosable(true);
    splitter->addWidget(connTabWidget);

    connect(connTabWidget, &QTabWidget::tabCloseRequested, this, &CentralFrame::on_tabCloseRequested);
}

void CentralFrame::addSession(const ssh::SSHOption &option)
{
    QString url(option.user.c_str());
    url.append("@");
    url.append(option.host.c_str());

    // add to list
    auto items = connListWidget->findItems(url, Qt::MatchExactly);
    if (items.empty()) {
        QListWidgetItem *item = new QListWidgetItem(connListWidget);
        item->setText(url);
        connListWidget->addItem(item);
        m_optionMap[url] = std::make_shared<ssh::SSHOption>(option);
    }

    // add to tab
    SessionTab *tab = new SessionTab(option, connTabWidget);
    int idx = connTabWidget->addTab(tab, "...");
    m_sessionMap[idx] = tab;

    // config
    connTabWidget->setTabText(idx, url);
}

void CentralFrame::on_item_double_clicked(QListWidgetItem *item)
{
    qDebug() << item->text();
    auto itor = m_optionMap.find(item->text());
    if (itor != m_optionMap.end()) {
        ssh::SSHOption* option = itor->second.get();

        QString url(option->user.c_str());
        url.append("@");
        url.append(option->host.c_str());

        SessionTab *tab = new SessionTab(*option, connTabWidget);
        int idx = connTabWidget->addTab(tab, url);
        m_sessionMap[idx] = tab;
    }
}

void CentralFrame::on_tabCloseRequested(int idx)
{
    connTabWidget->removeTab(idx);
    SessionTab *session = m_sessionMap[idx];
    if (session) {
        delete session;
        m_sessionMap.erase(idx);
    }
}
