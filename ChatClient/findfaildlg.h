#ifndef FINDFAILDLG_H
#define FINDFAILDLG_H

#include <QDialog>

namespace Ui
{
    class FindFailDlg;
}

class FindFailDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FindFailDlg(QWidget *parent = nullptr);
    ~FindFailDlg();
    void SetTip1Text(const QString &text);
    void SetTip2Text(const QString &text);

private slots:
    void on_fail_sure_btn_clicked();

private:
    Ui::FindFailDlg *ui;
};

#endif // FINDFAILDLG_H
