#ifndef APPLYFRIENDPAGE_H
#define APPLYFRIENDPAGE_H

#include <QWidget>
#include "applyfrienditem.h"
#include "userdata.h"

namespace Ui
{
    class ApplyFriendPage;
}

class ApplyFriendPage : public QWidget
{
    Q_OBJECT

public:
    explicit ApplyFriendPage(QWidget *parent = nullptr);
    ~ApplyFriendPage();
    void AddNewApply(std::shared_ptr<AddFriendApply> apply);

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    void loadApplyList();

private:
    Ui::ApplyFriendPage *ui;
    std::unordered_map<int, ApplyFriendItem *> _unauth_items;

public slots:
    void slot_auth_rsp(std::shared_ptr<AuthRsp>);

signals:
    void sig_show_search(bool);
};

#endif // APPLYFRIENDPAGE_H
