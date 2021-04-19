#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "centralframe.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_actionConnect_triggered();

private:
    void initUi();

private:
    Ui::MainWindow *ui;
    CentralFrame *mainFrame;
};
#endif // MAINWINDOW_H
