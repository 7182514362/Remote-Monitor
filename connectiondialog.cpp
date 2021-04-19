#include "connectiondialog.h"
#include "ui_connectiondialog.h"

ConnectionDialog::ConnectionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ConnectionDialog)
{
    ui->setupUi(this);
}

ConnectionDialog::~ConnectionDialog()
{
    delete ui;
}

QString ConnectionDialog::host() const
{
    return ui->editHost->text();
}
int ConnectionDialog::port() const
{
    return ui->editPort->value();
}
QString ConnectionDialog::user() const
{
    return ui->editUser->text();
}
QString ConnectionDialog::password() const
{
    return ui->editPassword->text();
}

void ConnectionDialog::on_okButton_clicked()
{
    accept();
}

void ConnectionDialog::on_cancelButton_clicked()
{
    reject();
}
