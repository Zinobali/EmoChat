#include "chatdialog.h"
#include "ui_chatdialog.h"
#include "usermgr.h"
// #include <QString>

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
    initUI();
    ui->label->setText(UserMgr::GetInstance()->name());
    ui->label_2->setText(QString::number(UserMgr::GetInstance()->uid()));
    ui->label_3->setText(UserMgr::GetInstance()->token());

}

ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::initUI()
{
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
}
