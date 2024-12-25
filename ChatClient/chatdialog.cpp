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


}

ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::initUI()
{
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    ui->add_btn->SetState("normal", "hover", "press"); //必须显示设置
}
