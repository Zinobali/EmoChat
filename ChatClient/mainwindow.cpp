#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpmgr.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    test_function(); // 测试函数，后续删除
    return; // test 测试函数，后续删除

    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    _login_dlg = new LoginDialog(this);
    setCentralWidget(_login_dlg);
    // 初始化_login_dlg信号槽
    connect(_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    connect(_login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_switch_chatdlg, this, &MainWindow::SlotSwitchChat);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SlotSwitchReg(QWidget* w)
{
    _reg_dlg = new RegisterDialog(this);
    setCentralWidget(_reg_dlg);
    if (w) {
        w->hide();
    }
    _reg_dlg->show();
    connect(_reg_dlg,  &RegisterDialog::sigSwitchLogin, this, &MainWindow::SlotSwitchLogin);
}

void MainWindow::SlotSwitchReset(QWidget* w)
{
    _reset_dlg = new ResetDialog(this);
    setCentralWidget(_reset_dlg);
    if (w) {
        w->hide();
    }
    _reset_dlg->show();
    //注册返回登录信号和槽函数
    connect(_reset_dlg, &ResetDialog::switchLogin, this, &MainWindow::SlotSwitchLogin);
}

void MainWindow::SlotSwitchLogin(QWidget* w)
{
    _login_dlg = new LoginDialog(this);
    setCentralWidget(_login_dlg);
    if (w) {
        w->hide();
    }
    _login_dlg->show();
    connect(_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    connect(_login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);
}

void MainWindow::SlotSwitchChat()
{
    // todo 切换到登录页面
    _chat_dlg = new ChatDialog(this);
    setCentralWidget(_chat_dlg);
    _chat_dlg->show();
    _login_dlg->hide();
    setMinimumSize(QSize(1050, 700));
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}

void MainWindow::test_function() // 测试函数，后续删除
{
    _chat_dlg = new ChatDialog(this);
    setCentralWidget(_chat_dlg);
    setMinimumSize(QSize(1050, 700));
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}
