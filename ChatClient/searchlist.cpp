#include "searchlist.h"
#include "tcpmgr.h"
#include "adduseritem.h"
#include <QEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include "findsuccessdlg.h"
#include "customizeedit.h"
#include <QJsonDocument>
#include "findfaildlg.h"
#include "usermgr.h"

SearchList::SearchList(QWidget *parent)
    : QListWidget(parent), _send_pending(false), _search_edit(nullptr), _loadingDialog(nullptr)
    ,_find_dlg(nullptr)
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
    if (_find_dlg)
    {
        _find_dlg->close();
        _find_dlg = nullptr;
    }
}

void SearchList::SetSearchEdit(QWidget *edit)
{
    _search_edit = edit;
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
    if (pending)
    {
        if (!_loadingDialog)
        {
            _loadingDialog = new LoadingDlg(this);
        }
        _loadingDialog->show();
        _send_pending = true;
    }
    else
    {
        if (_loadingDialog)
        {
            _loadingDialog->hide();
            _loadingDialog->deleteLater(); // 标记为删除
            _loadingDialog = nullptr;      // 防止悬挂指针
        }
        _send_pending = false;
    }
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

    ListItemBase *customItem = dynamic_cast<ListItemBase *>(widget); // 将item转换为基类ListItemBase
    if (!customItem)
    {
        qDebug() << "customItem is null";
        return;
    }

    auto item_type = customItem->item_type();
    if (item_type == ListItemType::INVALID_ITEM) // 无效
    {
        qDebug() << "slot invalid item clicked ";
        return;
    }

    if (item_type == ListItemType::ADD_USER_TIP_ITEM) // 添加用户
    {
        if (_send_pending)
        {
            return;
        }

        if (!_search_edit)
        {
            return;
        }
        waitPending(true);
        auto search_edit = dynamic_cast<CustomizeEdit *>(_search_edit);
        auto uid_str = search_edit->text(); // 搜索框的文本，可以是uid或name

        // 准备发送搜索用户请求
        QJsonObject req_obj;
        req_obj["uid"] = uid_str;
        QJsonDocument req_doc(req_obj);
        auto req_data = req_doc.toJson(QJsonDocument::Compact);

        emit TcpMgr::GetInstance() -> sig_send_data(RequestId::ID_SEARCH_USER_REQ, req_data);

        // qDebug() << "slot add user item clicked ";
        // _find_dlg = std::make_shared<FindSuccessDlg>();
        // auto info = std::make_shared<SearchInfo>(0, "zinobali", "zinobali", "hello, amigo!", 0);
        // auto dlg = std::dynamic_pointer_cast<FindSuccessDlg>(_find_dlg);
        // if (dlg)
        // {
        //     dlg->SetSearchInfo(info);
        //     dlg->show();
        // }

        return;
    }
    qDebug() << "slot user item clicked but unknown item type";
    CloseFindDlg();
}

void SearchList::slot_user_search(std::shared_ptr<SearchInfo> si)
{
    waitPending(false);
    Defer show([this]() {
        _find_dlg->show();
    });
    if (si == nullptr)
    {
        qDebug() << "user not found";
        _find_dlg = std::make_shared<FindFailDlg>();
        return;
    }

    // 检查是不是自己
    auto self_uid = UserMgr::GetInstance()->uid();
    if (si->_uid == self_uid)
    {
        _find_dlg = std::make_shared<FindFailDlg>();
        std::static_pointer_cast<FindFailDlg>(_find_dlg)->SetTip1Text("不能搜索自己");
        return;
    }

    // 检查是否已经在好友列表
    bool exist = UserMgr::GetInstance()->CheckFriendById(si->_uid);
    if (exist)
    {
        _find_dlg = std::make_shared<FindFailDlg>();
        std::static_pointer_cast<FindFailDlg>(_find_dlg)->SetTip1Text("该用户已经是好友");
        emit sig_jump_chat_item(si);
        return;
    }

    // 准备弹窗信息
    _find_dlg = std::make_shared<FindSuccessDlg>();
    std::static_pointer_cast<FindSuccessDlg>(_find_dlg)->SetSearchInfo(si);
}
