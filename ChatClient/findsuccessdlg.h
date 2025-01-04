#ifndef FINDSUCCESSDLG_H
#define FINDSUCCESSDLG_H

#include <QDialog>
#include "tcpmgr.h"
#include "clickedbtn.h"

namespace Ui
{
    class FindSuccessDlg;
}

class FindSuccessDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FindSuccessDlg(QWidget *parent = nullptr);
    ~FindSuccessDlg();
    void SetSearchInfo(std::shared_ptr<SearchInfo> si);

private:
    void initUi();

private:
    Ui::FindSuccessDlg *ui;
    QWidget *_parent;
    std::shared_ptr<SearchInfo> _si;
    ClickedBtn *_close_btn;

private slots:
    void on_add_friend_btn_clicked();
};

#endif // FINDSUCCESSDLG_H
