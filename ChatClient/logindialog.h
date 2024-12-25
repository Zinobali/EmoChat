#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "global.h"
#include "tcpmgr.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private:
    void testFunc();
    void initUiDesign();
    void initUiSignals();
    void initHttpHandlers();
    void AddTipErr(const TipErr& tc,const QString& tips);
    void DelTipErr(const TipErr& tc);
    void showTip(QString str, bool ok);
    bool checkEmailValid();
    bool checkPassValid();
    void enableBtn(bool enabled);

signals:
    void switchRegister(QWidget* w);
    void switchReset(QWidget* w);
    void sig_connect_tcp(ServerInfo);

private slots:
    void on_reg_btn_clicked();
    void slot_login_mod_finish(RequestId id, QString res, ErrorCodes err);
    void slot_tcp_con_finish(bool ok);
    void on_login_btn_clicked();
    void slot_login_failed(ErrorCodes err);

private:
    Ui::LoginDialog *ui;
    QMap<RequestId, std::function<void(const QJsonObject&)>> _handlers;
    QMap<TipErr,QString> _tip_errs;
    int uid_;
    QString token_;

};

#endif // LOGINDIALOG_H
