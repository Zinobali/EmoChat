#include "findfaildlg.h"
#include "ui_findfaildlg.h"

FindFailDlg::FindFailDlg(QWidget *parent)
    : QDialog(parent), ui(new Ui::FindFailDlg)
{
    ui->setupUi(this);
    setWindowTitle("添加");
    // 隐藏对话框标题栏
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    this->setObjectName("FindFailDlg");
    ui->fail_sure_btn->SetState("normal", "hover", "press");
    this->setModal(true);
}

FindFailDlg::~FindFailDlg()
{
    delete ui;
}

void FindFailDlg::SetTip1Text(const QString &text)
{
    ui->fail_tip->setText(text);
}

void FindFailDlg::SetTip2Text(const QString &text)
{
    ui->fail_tip2->setText(text);
}

void FindFailDlg::on_fail_sure_btn_clicked()
{
    this->close();
}
