#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint); // 通过设置无边框窗口达到让dialog嵌入到主窗口的效果
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_reg_btn_clicked()
{
    emit switchRegister();
}

