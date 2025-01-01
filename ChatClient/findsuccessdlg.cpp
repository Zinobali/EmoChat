#include "findsuccessdlg.h"
#include "ui_findsuccessdlg.h"
#include <QDir>

FindSuccessDlg::FindSuccessDlg(QWidget *parent)
    : QDialog(parent), ui(new Ui::FindSuccessDlg)
{
    ui->setupUi(this);
    setWindowTitle("添加");
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint); // 隐藏对话框标题栏
    QString app_path = QCoreApplication::applicationDirPath();
    QString pic_path = app_path + QDir::separator() + "static" + QDir::separator() + "head_5.png";
    QString native_path = QDir::toNativeSeparators(pic_path);
    QPixmap head_pix(native_path);
    head_pix = head_pix.scaled(ui->head_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->head_lb->setPixmap(head_pix);
    ui->add_friend_btn->SetState("normal", "hover", "press");
    this->setModal(true);
}

FindSuccessDlg::~FindSuccessDlg()
{
    delete ui;
}

void FindSuccessDlg::SetSearchInfo(std::shared_ptr<SearchInfo> si)
{
    ui->name_lb->setText(si->_name);
    _si = si;
}

void FindSuccessDlg::on_add_friend_btn_clicked()
{
    // todo... 添加好友界面弹出
}
