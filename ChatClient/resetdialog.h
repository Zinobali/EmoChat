#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <QDialog>
#include "global.h"
#include <QMap>

namespace Ui {
class ResetDialog;
}

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = nullptr);
    ~ResetDialog();

private:
    bool validateFields();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkVerifyValid();
    void initUiDesign();
    void initUiSignals();
    void initHttpHandlers();
    void AddTipErr(const TipErr& tc,const QString& tips);
    void DelTipErr(const TipErr& tc);
    void showTip(QString str, bool ok);

signals:
    void switchLogin(QWidget* w);

private slots:
    void slot_reset_mod_finish(RequestId id, QString res, ErrorCodes ec);

    void on_return_btn_clicked();
    void on_verify_btn_clicked();

    void on_sure_btn_clicked();

private:
    Ui::ResetDialog *ui;
    QMap<RequestId, std::function<void(const QJsonObject&)>> _handlers;
    QMap<TipErr,QString> _tip_errs;
};

#endif // RESETDIALOG_H
