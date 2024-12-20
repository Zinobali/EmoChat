#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "global.h"
#include <QMap>

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private:
    void showTip(QString str,bool ok);
    void initUiDesign();
    void initHttpHandlers();
    void initUiSignals();
    bool checkUserValid();
    bool checkPassValid();
    bool checkConfirmValid();
    bool checkEmailValid();
    bool checkVerifyValid();
    bool validateFields();
    void testEditInfo(); // 测试函数，后续删除
    void AddTipErr(const TipErr& tc,const QString& tips);
    void DelTipErr(const TipErr& tc);
    void ChangeTipPage();

private slots:
    void on_get_code_clicked();
    void slot_reg_mod_finish(RequestId id, QString res, ErrorCodes ec);
    void on_sure_btn_clicked();
    void on_return_btn_clicked();
    void on_cancle_btn_clicked();

signals:
    void sigSwitchLogin(QWidget* w);

private:
    Ui::RegisterDialog *ui;
    QMap<RequestId, std::function<void(const QJsonObject&)>> _handlers;
    QMap<TipErr,QString> _tip_errs;
    QTimer *_countdown_timer;
    int _countdown;
};

#endif // REGISTERDIALOG_H
