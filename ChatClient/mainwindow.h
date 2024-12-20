#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "logindialog.h"
#include "registerdialog.h"
#include "resetdialog.h"

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

private:
    void SlotSwitchReg(QWidget* w);
    void SlotSwitchReset(QWidget* w);
    void SlotSwitchLogin(QWidget* w);

private:
    Ui::MainWindow *ui;
    LoginDialog* _login_dlg;
    RegisterDialog* _reg_dlg;
    ResetDialog* _reset_dlg;

};
#endif // MAINWINDOW_H
