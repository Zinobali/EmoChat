#include "contactslist.h"
#include <QEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QDebug>
#include "grouptipitem.h"
#include <QListWidgetItem>
#include "global.h"
#include "tcpmgr.h"
#include "usermgr.h"

ContactsList::ContactsList(QWidget *parent)
    : QListWidget(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    viewport()->installEventFilter(this);

    // 模拟后端联系人列表
    addContactUserList();
    connect(this, &ContactsList::itemClicked, this, &ContactsList::slot_item_clicked);
    // 连接对端同意好友认证后的通知信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_add_auth_friend, this, &ContactsList::slot_add_auth_firend);
    // 连接自己同意好友认证后刷新界面
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_auth_rsp, this, &ContactsList::slot_auth_rsp);
}

void ContactsList::ShowRedPoint(bool bshow)
{
    _add_friend_item->ShowRedPoint(bshow);
}

bool ContactsList::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this->viewport())
    {
        if (event->type() == QEvent::Enter)
        {
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
        else if (event->type() == QEvent::Leave)
        {
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    if (watched == this->viewport() && event->type() == QEvent::Wheel)
    {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
        int numDegrees = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegrees / 15; // 计算滚动步数
        // 设置滚动幅度
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);
        // 检查是否滚动到底部
        QScrollBar *scrollBar = this->verticalScrollBar();
        int maxScrollValue = scrollBar->maximum();
        int currentValue = scrollBar->value();
        // int pageSize = 10; // 每页加载的联系人数量
        if (maxScrollValue - currentValue <= 0)
        {
            // 发送信号通知聊天界面加载更多聊天内容
            emit sig_loading_contact_user();
        }
        return true; // 停止事件传递
    }
    return QListWidget::eventFilter(watched, event);
}

void ContactsList::addContactUserList()
{
    // 新的朋友分组
    auto *groupTip = new GroupTipItem();
    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(groupTip->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, groupTip);
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable); // 禁用选中
    // 新的朋友item
    _add_friend_item = new ConUserItem();
    _add_friend_item->setObjectName("new_friend_item");
    _add_friend_item->SetInfo(0, tr("新的朋友"), ":/images/add_friend.png");
    _add_friend_item->setItem_type(ListItemType::APPLY_FRIEND_ITEM);

    QListWidgetItem *add_item = new QListWidgetItem();
    add_item->setSizeHint(_add_friend_item->sizeHint());
    this->addItem(add_item);
    this->setItemWidget(add_item, _add_friend_item);
    // 默认设置新的朋友申请条目被选中
    this->setCurrentItem(add_item);

    // 联系人分组
    auto *groupCon = new GroupTipItem();
    groupCon->SetGroupTip(tr("联系人"));
    _groupitem = new QListWidgetItem();
    _groupitem->setSizeHint(groupCon->sizeHint());
    this->addItem(_groupitem);
    this->setItemWidget(_groupitem, groupCon);
    _groupitem->setFlags(_groupitem->flags() & ~Qt::ItemIsSelectable);

    for (int i = 0; i < 13; i++)
    {
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int str_i = randomValue % strs.size();
        int head_i = randomValue % heads.size();
        int name_i = randomValue % names.size();
        auto *con_user_wid = new ConUserItem();
        con_user_wid->SetInfo(0, names[name_i], heads[head_i]);
        QListWidgetItem *item = new QListWidgetItem;
        // qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(con_user_wid->sizeHint());
        this->addItem(item);
        this->setItemWidget(item, con_user_wid);
    }
}

void ContactsList::slot_item_clicked(QListWidgetItem *item)
{
    QWidget *widget = this->itemWidget(item); // 获取自定义widget对象
    if (!widget)
    {
        qDebug() << "slot item clicked widget is nullptr";
        return;
    }

    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase *customItem = qobject_cast<ListItemBase *>(widget);
    if (!customItem)
    {
        qDebug() << "slot item clicked widget is nullptr";
        return;
    }

    auto itemType = customItem->item_type();
    switch (itemType)
    {
    case ListItemType::APPLY_FRIEND_ITEM:
    {
        qDebug() << "apply friend item clicked ";
        emit sig_switch_apply_friend_page();
        break;
    }
    case ListItemType::CONTACT_USER_ITEM:
    {
        qDebug() << "contact user item clicked ";
        emit sig_switch_friend_info_page();
        break;
    }
    default:
        qDebug() << "slot invalid item clicked ";
        break;
    }
}

void ContactsList::slot_add_auth_firend(std::shared_ptr<AuthInfo> auth_info)
{
    auto isExist = UserMgr::GetInstance()->CheckFriendById(auth_info->_uid);
    if (isExist)
    {
        return;
    }

    auto *con_user_wid = new ConUserItem();
    con_user_wid->SetInfo(auth_info);
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(con_user_wid->sizeHint());

    // 获取联系人分组的行号
    int index = row(_groupitem);
    // 插到分组的第一行
    this->insertItem(index + 1, item);
    this->setItemWidget(item, con_user_wid); // 最后setItemWidget
}

void ContactsList::slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp)
{
    auto isExist = UserMgr::GetInstance()->CheckFriendById(auth_rsp->_uid);
    if (isExist)
    {
        return;
    }

    auto *con_user_wid = new ConUserItem();
    con_user_wid->SetInfo(auth_rsp);
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(con_user_wid->sizeHint());

    // 获取联系人分组的行号
    int index = row(_groupitem);
    // 插到分组的第一行
    this->insertItem(index + 1, item);
    this->setItemWidget(item, con_user_wid); // 最后setItemWidget
}
