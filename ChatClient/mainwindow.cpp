#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    _login_dlg = new LoginDialog(this);
    setCentralWidget(_login_dlg);
    // 初始化_login_dlg信号槽
    connect(_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    connect(_login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);

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
