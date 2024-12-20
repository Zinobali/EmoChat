#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "global.h"

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


signals:
    void switchRegister(QWidget* w);
    void switchReset(QWidget* w);

private slots:
    void on_reg_btn_clicked();
    void slot_login_mod_finish(RequestId id, QString res, ErrorCodes err);

    void on_login_btn_clicked();

private:
    Ui::LoginDialog *ui;
    QMap<RequestId, std::function<void(const QJsonObject&)>> _handlers;
    QMap<TipErr,QString> _tip_errs;

};

#endif // LOGINDIALOG_H
