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
    void initHttpHandlers();

private slots:
    void on_get_code_clicked();
    void slot_reg_mod_finish(RequestId id, QString res, ErrorCodes ec);


private:
    Ui::RegisterDialog *ui;
    QMap<RequestId, std::function<void(const QJsonObject&)>> _handlers;
};

#endif // REGISTERDIALOG_H
