#include "findsuccessdlg.h"
#include "ui_findsuccessdlg.h"
#include <QDir>
#include "applyfriend.h"

FindSuccessDlg::FindSuccessDlg(QWidget *parent)
    : QDialog(parent), ui(new Ui::FindSuccessDlg), _parent(parent)
{
    ui->setupUi(this);
    initUi();
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

void FindSuccessDlg::initUi()
{
    _close_btn = new ClickedBtn(this);
    _close_btn->setFixedSize(30, 30);
    _close_btn->setText("X");
    _close_btn->setObjectName("fd_suc_dlg_cls_btn");
    QPoint pos = QPoint(this->width() - _close_btn->width(), 0);
    _close_btn->move(pos);
    _close_btn->SetState("normal", "hover", "press");
    connect(_close_btn, &ClickedBtn::clicked, [=]()
            { this->close(); });
}

void FindSuccessDlg::on_add_friend_btn_clicked()
{
    this->hide();
    // 弹出加好友界面
    auto applyFriend = new ApplyFriend(_parent);
    applyFriend->SetSearchInfo(_si);
    applyFriend->setModal(true);
    applyFriend->show();
}
