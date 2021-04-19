#ifndef CENTRALFRAME_H
#define CENTRALFRAME_H

#include <memory>
#include <QBoxLayout>
#include <QListWidget>
#include <QSplitter>
#include <QWidget>
#include <map>

namespace ssh {
struct SSHOption;
}

class SessionTab;

class CentralFrame : public QWidget
{
    Q_OBJECT

public:
    explicit CentralFrame(QWidget *parent = nullptr);
    ~CentralFrame() {}
    void addSession(const ssh::SSHOption &option);
private:
    void initUI();
    void on_tabCloseRequested(int tabIdx);
    void on_item_double_clicked(QListWidgetItem *item);
private:
    QVBoxLayout *vBoxLayout;
    QSplitter *splitter;
    QListWidget *connListWidget;
    QTabWidget *connTabWidget;
    // tab id to sessionTab
    std::map<int,SessionTab*> m_sessionMap;
    // list item to SSHOption
    std::map<QString,std::shared_ptr<ssh::SSHOption>> m_optionMap;
};

#endif // CENTRALFRAME_H
