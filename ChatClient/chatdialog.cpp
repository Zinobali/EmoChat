#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QAction>
#include <QElapsedTimer>
#include <QMovie>
#include <QProgressBar>
#include <QTimer>
#include <QDebug>
#include <vector>
#include <QVBoxLayout>
#include <QMouseEvent>
#include "global.h"
#include "tcpmgr.h"
#include "usermgr.h"
#include "chatuseritem.h"

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ChatDialog), mode_(ChatUIMode::ChatMode), state_(ChatUIMode::ChatMode),
      b_loading_(false), last_widget_(nullptr)
{
    ui->setupUi(this);
    initUI();
    initSignals();
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::ShowSearch(bool show)
{
    if (show)
    {
        ui->chatting_list->hide();
        ui->contacts_list->hide();
        ui->search_list->show();
        mode_ = ChatUIMode::SearchMode;
    }
    else if (state_ == ChatUIMode::ChatMode)
    {
        ui->search_list->hide();
        ui->contacts_list->hide();
        ui->chatting_list->show();
        mode_ = ChatUIMode::ChatMode;
    }
    else if (state_ == ChatUIMode::ContactMode)
    {
        ui->search_list->hide();
        ui->chatting_list->hide();
        ui->contacts_list->show();
        mode_ = ChatUIMode::ContactMode;
    }
}

void ChatDialog::AddLabelGroup(StateWidget *label)
{
    label_list_.push_back(label);
}

bool ChatDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (ChatUIMode::SearchMode == mode_)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            handleGlobalMousePress(mouseEvent);
        }
    }

    return QDialog::eventFilter(watched, event);
}

void ChatDialog::initUI()
{
    setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    ui->add_btn->SetState("normal", "hover", "press"); // 必须显式的设置状态，管理qss
    // 搜索框搜索图标
    QAction *searchAction = new QAction(ui->search_edit);
    searchAction->setIcon(QIcon(":/images/search.png"));
    ui->search_edit->addAction(searchAction, QLineEdit::LeadingPosition);
    ui->search_edit->setPlaceholderText(tr("搜索"));
    // list模式
    ShowSearch(false);
    // 添加模拟聊天列表
    addChatUserList();
    // 设置侧边栏用户头像
    setSideBarUserHead(":/images/head_1.png");
    ui->side_chat_lb->SetState("normal", "hover", "pressed", "selected_normal", "selected_hover", "selected_pressed");
    ui->side_contacts_lb->SetState("normal", "hover", "pressed", "selected_normal", "selected_hover", "selected_pressed");
    AddLabelGroup(ui->side_chat_lb);
    AddLabelGroup(ui->side_contacts_lb);
    // 直接显示聊天页面
    ui->side_chat_lb->SetSelected(true);
    installEventFilter(this);
    // 关联搜索编辑框
    ui->search_list->SetSearchEdit(ui->search_edit);
}

void ChatDialog::initSignals()
{
    connect(ui->chatting_list, &ChatUserList::sig_loading_chat_user, this, &ChatDialog::slot_loading_chat_user);
    connect(ui->side_chat_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_chat_clicked);
    connect(ui->side_contacts_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_contact_clicked);
    connect(ui->search_edit, &QLineEdit::textChanged, this, &ChatDialog::slot_search_text_changed);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_friend_apply, this, &ChatDialog::slot_friend_apply);
    // 连接对端同意好友认证后的通知信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_add_auth_friend, this, &ChatDialog::slot_add_auth_firend);
    // 连接自己同意好友认证后刷新界面
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_auth_rsp, this, &ChatDialog::slot_auth_rsp);
    // 连接聊天跳转信号
    connect(ui->search_list, &SearchList::sig_jump_chat_item, this, &ChatDialog::slot_jump_chat_item);
}

void ChatDialog::addChatUserList()
{
    for (int var = 0; var < 13; ++var)
    {

        // 生成测试随机好友信息
        int rand_val = QRandomGenerator::global()->bounded(100);
        int str_i = rand_val % strs.size();
        int head_i = rand_val % heads.size();
        int name_i = rand_val % names.size();
        qDebug() << "adding friend item : " << var;

        // QListWidget的item
        auto *chat_user_item = new ChatUserItem();
        auto user_info = std::make_shared<UserInfo>(
            0, names[name_i], names[name_i],
            heads[head_i], 0, strs[str_i]);
        chat_user_item->SetInfo(user_info);

        QListWidgetItem *item = new QListWidgetItem();
        item->setSizeHint(chat_user_item->sizeHint());
        // 一定要先添加Item再替换Widget，否则无法显示自定义widget
        ui->chatting_list->addItem(item);
        ui->chatting_list->setItemWidget(item, chat_user_item);
    }
}

void ChatDialog::setSideBarUserHead(const QString &headUrl)
{
    QPixmap head = QPixmap(headUrl);
    head = head.scaled(ui->side_head_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->side_head_lb->setPixmap(head);
    ui->side_head_lb->setScaledContents(true);
}

void ChatDialog::clearOtherLabelState(StateWidget *w)
{
    foreach (auto &label, label_list_)
    {
        if (label == w)
        {
            continue;
        }
        label->ClearState();
    }
}

void ChatDialog::handleGlobalMousePress(QMouseEvent *event)
{
    QPoint pos = ui->search_list->mapFromGlobal(event->globalPos());
    bool contains = ui->search_list->rect().contains(pos);
    if (!contains)
    {
        ui->search_edit->clear();
        ShowSearch(false);
    }
}

void ChatDialog::setSelectedChatItem(int uid)
{
    if (ui->chatting_list->count() <= 0)
    {
        return;
    }

    if (uid == 0)
    {
        // 如果uid为0，则选中第一个item
        ui->chatting_list->setCurrentRow(0);
        QListWidgetItem *firstItem = ui->chatting_list->item(0);
        if (!firstItem)
        {
            return;
        }

        // 获取item中的widget
        QWidget *widget = ui->chatting_list->itemWidget(firstItem);
        if (!widget)
        {
            return;
        }

        auto chat_item = qobject_cast<ChatUserItem *>(widget);
        if (!chat_item)
        {
            return;
        }

        cur_chat_uid_ = chat_item->GetUserInfo()->_uid;

        return;
    }

    // 找不到对应的item
    auto iter = chat_items_map_.find(uid);
    if (iter == chat_items_map_.end())
    {
        ui->chatting_list->setCurrentRow(0);
        return;
    }

    ui->chatting_list->setCurrentItem(iter.value());
    cur_chat_uid_ = uid;
    return;
}

void ChatDialog::setSelectedChatPage(int uid)
{
    if (ui->chatting_list->count() <= 0)
    {
        return;
    }

    if (uid == 0)
    {
        auto item = ui->chatting_list->item(0);
        QWidget *widget = ui->chatting_list->itemWidget(item);
        if (!widget)
        {
            return;
        }

        auto chat_item = qobject_cast<ChatUserItem *>(widget);
        if (!chat_item)
        {
            return;
        }

        // 设置聊天页面的用户信息
        auto user_info = chat_item->GetUserInfo();
        ui->chat_page->SetUserInfo(user_info);
        return;
    }

    auto iter = chat_items_map_.find(uid);
    if (iter == chat_items_map_.end())
    {
        return;
    }

    QWidget *widget = ui->chatting_list->itemWidget(iter.value());
    if (!widget)
    {
        return;
    }

    // 转换成基类的item
    ListItemBase *list_item = qobject_cast<ListItemBase *>(widget);
    if (!list_item)
    {
        return;
    }

    // 获取item的类型
    auto type = list_item->item_type();
    if (type == ListItemType::CHAT_USER_ITEM)
    {
        auto chat_item = qobject_cast<ChatUserItem *>(widget);
        if (!chat_item)
        {
            return;
        }

        // 设置聊天页面的用户信息
        auto user_info = chat_item->GetUserInfo();
        ui->chat_page->SetUserInfo(user_info);
        return;
    }
}

void ChatDialog::slot_loading_chat_user()
{
    // if (b_loading_)
    // {
    //     return;
    // }

    qDebug() << "loading users ...";
    // 添加加载动画
    // b_loading_ = true;
    // b_loading_ = false;
}

void ChatDialog::slot_search_text_changed(const QString &text)
{
    if (!text.isEmpty())
    {
        ShowSearch(true);
        return;
    }
    ShowSearch(false);
}

void ChatDialog::slot_side_chat_clicked()
{
    clearOtherLabelState(ui->side_chat_lb);
    ui->stackedWidget->setCurrentWidget(ui->chat_page);
    // qDebug() << "stack widget index:" << ui->stackedWidget->currentIndex();
    state_ = ChatUIMode::ChatMode;
    ShowSearch(false);
}

void ChatDialog::slot_side_contact_clicked()
{
    // clearOtherLabelState(ui->side_contacts_lb);
    // ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
    // qDebug() << "stack widget index:" << ui->stackedWidget->currentIndex();
    // state_ = ChatUIMode::ContactMode;
    // ShowSearch(false);

    clearOtherLabelState(ui->side_contacts_lb);
    if (last_widget_ == nullptr)
    {
        ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
        last_widget_ = ui->friend_apply_page;
    }
    else
    {
        ui->stackedWidget->setCurrentWidget(last_widget_);
    }
    // qDebug() << "stack widget index:" << ui->stackedWidget->currentIndex();
    state_ = ChatUIMode::ContactMode;
    ShowSearch(false);
}

void ChatDialog::slot_friend_apply(std::shared_ptr<AddFriendApply> apply)
{
    // 检查是否已经申请
    bool is_exist = UserMgr::GetInstance()->AlreadyApply(apply->_from_uid);
    if (is_exist)
    {
        return;
    }

    UserMgr::GetInstance()->AddApplyList(std::make_shared<ApplyInfo>(apply));
    ui->side_contacts_lb->ShowRedPoint(true);
    ui->contacts_list->ShowRedPoint(true);
    ui->friend_apply_page->AddNewApply(apply);
}

void ChatDialog::slot_add_auth_firend(std::shared_ptr<AuthInfo> auth_info)
{
    auto isExist = UserMgr::GetInstance()->CheckFriendById(auth_info->_uid);
    if (isExist)
    {
        return;
    }

    UserMgr::GetInstance()->AddFriend(auth_info);

    auto *chat_user_item = new ChatUserItem();
    auto user_info = std::make_shared<UserInfo>(auth_info);
    chat_user_item->SetInfo(user_info);
    auto *item = new QListWidgetItem();
    item->setSizeHint(chat_user_item->sizeHint());
    ui->chatting_list->insertItem(0, item);
    ui->chatting_list->setItemWidget(item, chat_user_item);
    chat_items_map_.insert(auth_info->_uid, item);
}

void ChatDialog::slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp)
{
    auto isExist = UserMgr::GetInstance()->CheckFriendById(auth_rsp->_uid);
    if (isExist)
    {
        return;
    }

    UserMgr::GetInstance()->AddFriend(auth_rsp);

    auto *chat_user_item = new ChatUserItem();
    auto user_info = std::make_shared<UserInfo>(auth_rsp);
    chat_user_item->SetInfo(user_info);
    auto *item = new QListWidgetItem();
    item->setSizeHint(chat_user_item->sizeHint());
    ui->chatting_list->insertItem(0, item);
    ui->chatting_list->setItemWidget(item, chat_user_item);
    chat_items_map_.insert(auth_rsp->_uid, item);
}

void ChatDialog::slot_jump_chat_item(std::shared_ptr<SearchInfo> si)
{
    auto iter = chat_items_map_.find(si->_uid);
    if (iter != chat_items_map_.end())
    {
        // 找到了已在聊天列表的item
        ui->chatting_list->scrollToItem(iter.value());
        ui->side_chat_lb->SetSelected(true); // 跳转到聊天页面
        setSelectedChatItem(si->_uid);       // 设置选中的item
        setSelectedChatPage(si->_uid);       // 跳转对应的聊天窗口
        slot_side_chat_clicked();            // 设置侧边栏状态
        return;
    }

    // 没找到，创建新的item
    auto *chat_user_item = new ChatUserItem();
    auto user_info = std::make_shared<UserInfo>(si);
    chat_user_item->SetInfo(user_info);
    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(chat_user_item->sizeHint());
    ui->chatting_list->insertItem(0, item);
    ui->chatting_list->setItemWidget(item, chat_user_item); // 添加自定义item(widget)
    chat_items_map_.insert(si->_uid, item);

    ui->side_chat_lb->SetSelected(true);
    setSelectedChatItem(si->_uid);
    setSelectedChatPage(si->_uid);
    slot_side_chat_clicked();
}
