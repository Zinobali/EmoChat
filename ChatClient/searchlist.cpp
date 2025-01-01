#include "searchlist.h"
#include "tcpmgr.h"
#include "adduseritem.h"
#include <QEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include "findsuccessdlg.h"

SearchList::SearchList(QWidget *parent)
    : QListWidget(parent), _send_pending(false), _search_edit(nullptr), _find_dlg(nullptr)
{
    initUI();
    initSignals();
    // 安装事件过滤器
    viewport()->installEventFilter(this);
    // 添加item
    initTipItem();
}

void SearchList::CloseFindDlg()
{
}

void SearchList::SetSearchEdit(QWidget *edit)
{
}

bool SearchList::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == viewport())
    {
        if (event->type() == QEvent::Enter)
        {
            setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
        else if (event->type() == QEvent::Leave)
        {
            setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    if (watched == viewport() && event->type() == QEvent::Wheel)
    {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
        int numDegrees = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegrees / 15; // 计算滚动步数
        verticalScrollBar()->setValue(verticalScrollBar()->value() - numSteps);
        return true;
    }

    return QListWidget::eventFilter(watched, event);
}

void SearchList::initUI()
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void SearchList::initSignals()
{
    // item点击
    connect(this, &SearchList::itemClicked, this, &SearchList::slot_item_clicked);
    // 连接搜索item
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_user_search, this, &SearchList::slot_user_search);
}

void SearchList::waitPending(bool pending)
{
}

void SearchList::initTipItem()
{
    // 占位item
    auto *invalid_item = new QWidget();
    QListWidgetItem *item_tmp = new QListWidgetItem();
    item_tmp->setSizeHint(QSize(250, 10));
    this->addItem(item_tmp);
    invalid_item->setObjectName("invalid_item");
    this->setItemWidget(item_tmp, invalid_item);
    item_tmp->setFlags(item_tmp->flags() & ~Qt::ItemIsSelectable); // 设置不可选中

    // 添加用户item
    auto *add_user_item = new AddUserItem();
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(add_user_item->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, add_user_item);
    // qDebug() << "add user item";
}

void SearchList::slot_item_clicked(QListWidgetItem *item)
{
    QWidget *widget = this->itemWidget(item); // 获取自定义widget对象
    if (!widget)
    {
        qDebug() << "widget is null";
        return;
    }

    ListItemBase *customItem = dynamic_cast<ListItemBase *>(widget);
    if (!customItem)
    {
        qDebug() << "customItem is null";
        return;
    }

    auto item_type = customItem->item_type();
    if (item_type == ListItemType::INVALID_ITEM)
    {
        qDebug() << "slot invalid item clicked ";
        return;
    }

    if (item_type == ListItemType::ADD_USER_TIP_ITEM)
    {
        qDebug() << "slot add user item clicked ";
        _find_dlg = std::make_shared<FindSuccessDlg>();
        auto info = std::make_shared<SearchInfo>(0, "zinobali", "zinobali", "hello, amigo!", 0);
        auto dlg = std::dynamic_pointer_cast<FindSuccessDlg>(_find_dlg);
        if (dlg)
        {
            dlg->SetSearchInfo(info);
            dlg->show();
        }

        return;
    }
    qDebug() << "slot user item clicked but unknown item type";
}

void SearchList::slot_user_search(std::shared_ptr<SearchInfo> si)
{
}
