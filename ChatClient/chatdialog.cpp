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
#include "tcpfilemgr.h"
#include "logicmgr.h"

ChatDialog::ChatDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::ChatDialog), mode_(ChatUIMode::ChatMode), state_(ChatUIMode::ChatMode),
    b_loading_(false), last_widget_(nullptr) {
    ui->setupUi(this);
    initUI();
    initSignals();
}

ChatDialog::~ChatDialog() {
    delete ui;
}

void ChatDialog::ShowSearch(bool show) {
    if (show) {
        ui->chatting_list->hide();
        ui->contacts_list->hide();
        ui->search_list->show();
        mode_ = ChatUIMode::SearchMode;
    } else if (state_ == ChatUIMode::ChatMode) {
        ui->search_list->hide();
        ui->contacts_list->hide();
        ui->chatting_list->show();
        mode_ = ChatUIMode::ChatMode;
    } else if (state_ == ChatUIMode::ContactMode) {
        ui->search_list->hide();
        ui->chatting_list->hide();
        ui->contacts_list->show();
        mode_ = ChatUIMode::ContactMode;
    }
}

void ChatDialog::AddLabelGroup(StateWidget* label) {
    label_list_.push_back(label);
}

void ChatDialog::UpdateChatMsg(std::vector<std::shared_ptr<TextChatData>> msgdata) {
    for (auto& msg : msgdata) {
        // 不是正在聊天的人，不做处理
        if (msg->_from_uid != cur_chat_uid_) {
            // todo ... 添加红点或者消息数量提醒
            break;
        }

        // 添加到聊天页面
        ui->chat_page->AppendChatMsg(msg);
    }
}

bool ChatDialog::eventFilter(QObject* watched, QEvent* event) {
    if (ChatUIMode::SearchMode == mode_) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            handleGlobalMousePress(mouseEvent);
        }
    }

    return QDialog::eventFilter(watched, event);
}

void ChatDialog::initUI() {
    setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    ui->add_btn->SetState("normal", "hover", "press"); // 必须显式的设置状态，管理qss
    // 搜索框搜索图标
    QAction* searchAction = new QAction(ui->search_edit);
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
    // 设置软件打开默认行为
    ui->stackedWidget->setCurrentWidget(ui->chat_page);
    setSelectedChatItem(0);
    setSelectedChatPage(0);
}

void ChatDialog::initSignals() {
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
    // 联系人滚动条加载更多
    connect(ui->contacts_list, &ContactsList::sig_loading_contact_user, this, &ChatDialog::slot_loading_contact_user);
    // 联系人item点击信号
    connect(ui->contacts_list, &ContactsList::sig_switch_friend_info_page, this, &ChatDialog::slot_friend_info_page);
    // 申请界面
    connect(ui->contacts_list, &ContactsList::sig_switch_apply_friend_page, this, &ChatDialog::slot_switch_apply_friend_page);
    // 从好友信息页面跳转聊天
    connect(ui->friend_info_page, &FriendInfoPage::sig_jump_chat_item, this, &ChatDialog::slot_jump_chat_item_from_friend_info_page);
    // 点击聊天列表item
    connect(ui->chatting_list, &ChatUserList::itemClicked, this, &ChatDialog::slot_chat_item_clicked);
    // 自己发送的消息
    connect(ui->chat_page, &ChatPage::sig_append_send_chat_msg, this, &ChatDialog::slot_append_send_chat_msg);
    // 对端文本消息
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_text_chat_msg, this, &ChatDialog::slot_text_chat_msg);
    // 链接tcp文件系统和逻辑系统信号槽
    connect(TcpFileMgr::GetInstance().get(), &TcpFileMgr::sig_logic_process, LogicMgr::GetInstance().get(), &LogicMgr::sig_logic_process);
}

void ChatDialog::addChatUserList() {
    // 数据库中的好友列表
    auto friend_list = UserMgr::GetInstance()->GetChatListPerPage();
    if (!friend_list.empty()) {
        for (auto& friend_info : friend_list) {
            auto find_iter = chat_items_map_.find(friend_info->_uid);
            if (find_iter == chat_items_map_.end()) {
                // 好友未重复
                auto* chat_user_item = new ChatUserItem();
                auto user_info = std::make_shared<UserInfo>(friend_info);
                chat_user_item->SetInfo(user_info);
                QListWidgetItem* item = new QListWidgetItem();
                item->setSizeHint(chat_user_item->sizeHint());
                ui->chatting_list->addItem(item);
                ui->chatting_list->setItemWidget(item, chat_user_item);
                chat_items_map_.insert(friend_info->_uid, item);
            }
        }
    }

    // 假数据
    for (int var = 0; var < 13; ++var) {
        // 生成测试随机好友信息
        int rand_val = QRandomGenerator::global()->bounded(100);
        int str_i = rand_val % strs.size();
        int head_i = rand_val % heads.size();
        int name_i = rand_val % names.size();

        // QListWidget的item
        auto* chat_user_item = new ChatUserItem();
        auto user_info = std::make_shared<UserInfo>(
            0, names[name_i], names[name_i],
            heads[head_i], 0, strs[str_i]);
        chat_user_item->SetInfo(user_info);

        QListWidgetItem* item = new QListWidgetItem();
        item->setSizeHint(chat_user_item->sizeHint());
        // 一定要先添加Item再替换Widget，否则无法显示自定义widget
        ui->chatting_list->addItem(item);
        ui->chatting_list->setItemWidget(item, chat_user_item);
    }
}

void ChatDialog::setSideBarUserHead(const QString& headUrl) {
    QPixmap head = QPixmap(headUrl);
    head = head.scaled(ui->side_head_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->side_head_lb->setPixmap(head);
    ui->side_head_lb->setScaledContents(true);
}

void ChatDialog::clearOtherLabelState(StateWidget* w) {
    foreach(auto & label, label_list_) {
        if (label == w) {
            continue;
        }
        label->ClearState();
    }
}

void ChatDialog::handleGlobalMousePress(QMouseEvent* event) {
    QPoint pos = ui->search_list->mapFromGlobal(event->globalPos());
    bool contains = ui->search_list->rect().contains(pos);
    if (!contains) {
        ui->search_edit->clear();
        ShowSearch(false);
    }
}

void ChatDialog::setSelectedChatItem(int uid) {
    if (ui->chatting_list->count() <= 0) {
        return;
    }

    if (uid == 0) {
        // 如果uid为0，则选中第一个item
        ui->chatting_list->setCurrentRow(0);
        QListWidgetItem* firstItem = ui->chatting_list->item(0);
        if (!firstItem) {
            return;
        }

        // 获取item中的widget
        QWidget* widget = ui->chatting_list->itemWidget(firstItem);
        if (!widget) {
            return;
        }

        auto chat_item = qobject_cast<ChatUserItem*>(widget);
        if (!chat_item) {
            return;
        }

        cur_chat_uid_ = chat_item->GetUserInfo()->_uid;

        return;
    }

    // 找不到对应的item
    auto iter = chat_items_map_.find(uid);
    if (iter == chat_items_map_.end()) {
        ui->chatting_list->setCurrentRow(0);
        return;
    }

    ui->chatting_list->setCurrentItem(iter.value());
    cur_chat_uid_ = uid;
    return;
}

void ChatDialog::setSelectedChatPage(int uid) {
    if (ui->chatting_list->count() <= 0) {
        return;
    }

    if (uid == 0) {
        auto item = ui->chatting_list->item(0);
        QWidget* widget = ui->chatting_list->itemWidget(item);
        if (!widget) {
            return;
        }

        auto chat_item = qobject_cast<ChatUserItem*>(widget);
        if (!chat_item) {
            return;
        }

        // 设置聊天页面的用户信息
        auto user_info = chat_item->GetUserInfo();
        ui->chat_page->SetUserInfo(user_info);
        return;
    }

    auto iter = chat_items_map_.find(uid);
    if (iter == chat_items_map_.end()) {
        return;
    }

    QWidget* widget = ui->chatting_list->itemWidget(iter.value());
    if (!widget) {
        return;
    }

    // 转换成基类的item
    ListItemBase* list_item = qobject_cast<ListItemBase*>(widget);
    if (!list_item) {
        return;
    }

    // 获取item的类型
    auto type = list_item->item_type();
    if (type == ListItemType::CHAT_USER_ITEM) {
        auto chat_item = qobject_cast<ChatUserItem*>(list_item);
        if (!chat_item) {
            return;
        }

        // 设置聊天页面的用户信息
        auto user_info = chat_item->GetUserInfo();
        ui->chat_page->SetUserInfo(user_info);
        return;
    }
}

void ChatDialog::loadMoreChatUser() {
    auto friend_list = UserMgr::GetInstance()->GetChatListPerPage();
    if (friend_list.empty()) {
        return;
    }

    for (auto& friend_info : friend_list) {
        auto find_iter = chat_items_map_.find(friend_info->_uid);
        if (find_iter == chat_items_map_.end()) {
            // 好友未重复
            auto* chat_user_item = new ChatUserItem();
            auto user_info = std::make_shared<UserInfo>(friend_info);
            chat_user_item->SetInfo(user_info);
            QListWidgetItem* item = new QListWidgetItem();
            item->setSizeHint(chat_user_item->sizeHint());
            ui->chatting_list->addItem(item);
            ui->chatting_list->setItemWidget(item, chat_user_item);
            chat_items_map_.insert(friend_info->_uid, item);
        }
    }
}

void ChatDialog::loadMoreContacts() {
    auto friend_list = UserMgr::GetInstance()->GetContactsPerPage();
    if (friend_list.empty()) {
        return;
    }

    for (auto& con : friend_list) {
        auto* con_user_wid = new ConUserItem();
        con_user_wid->SetInfo(con->_uid, con->_back, con->_icon);
        QListWidgetItem* item = new QListWidgetItem;
        item->setSizeHint(con_user_wid->sizeHint());
        ui->contacts_list->addItem(item);
        ui->contacts_list->setItemWidget(item, con_user_wid);
    }
}

void ChatDialog::slot_loading_chat_user() {
    if (b_loading_) {
        return;
    }

    b_loading_ = true;
    // todo ... 设计加载动画
    loadMoreChatUser();
    b_loading_ = false;
}

void ChatDialog::slot_loading_contact_user() {
    if (b_loading_) {
        return;
    }

    b_loading_ = true;
    // todo ... 设计加载动画
    loadMoreContacts();
    b_loading_ = false;
}

void ChatDialog::slot_search_text_changed(const QString& text) {
    if (!text.isEmpty()) {
        ShowSearch(true);
        return;
    }
    ShowSearch(false);
}

void ChatDialog::slot_side_chat_clicked() {
    clearOtherLabelState(ui->side_chat_lb);
    ui->stackedWidget->setCurrentWidget(ui->chat_page);
    // qDebug() << "stack widget index:" << ui->stackedWidget->currentIndex();
    state_ = ChatUIMode::ChatMode;
    ShowSearch(false);
}

void ChatDialog::slot_side_contact_clicked() {
    // clearOtherLabelState(ui->side_contacts_lb);
    // ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
    // qDebug() << "stack widget index:" << ui->stackedWidget->currentIndex();
    // state_ = ChatUIMode::ContactMode;
    // ShowSearch(false);

    clearOtherLabelState(ui->side_contacts_lb);
    if (last_widget_ == nullptr) {
        ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
        last_widget_ = ui->friend_apply_page;
    } else {
        ui->stackedWidget->setCurrentWidget(last_widget_);
    }
    // qDebug() << "stack widget index:" << ui->stackedWidget->currentIndex();
    state_ = ChatUIMode::ContactMode;
    ShowSearch(false);
}

void ChatDialog::slot_friend_info_page(std::shared_ptr<UserInfo> user_info) {
    last_widget_ = ui->friend_info_page;
    ui->stackedWidget->setCurrentWidget(ui->friend_info_page);
    ui->friend_info_page->SetInfo(user_info);
}

void ChatDialog::slot_switch_apply_friend_page() {
    last_widget_ = ui->friend_apply_page;
    ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
}

void ChatDialog::slot_jump_chat_item_from_friend_info_page(std::shared_ptr<UserInfo> user_info) {
    auto iter = chat_items_map_.find(user_info->_uid);
    if (iter != chat_items_map_.end()) {
        // 找到了已在聊天列表的item
        ui->chatting_list->scrollToItem(iter.value());
        ui->side_chat_lb->SetSelected(true);  // 跳转到聊天页面
        setSelectedChatItem(user_info->_uid); // 设置选中的item
        setSelectedChatPage(user_info->_uid); // 跳转对应的聊天窗口
        slot_side_chat_clicked();             // 设置侧边栏状态
        return;
    }

    // 没找到，创建新的item
    auto* chat_user_item = new ChatUserItem();
    chat_user_item->SetInfo(user_info);
    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(chat_user_item->sizeHint());
    ui->chatting_list->insertItem(0, item);
    ui->chatting_list->setItemWidget(item, chat_user_item); // 添加自定义item(widget)
    chat_items_map_.insert(user_info->_uid, item);

    ui->side_chat_lb->SetSelected(true);
    setSelectedChatItem(user_info->_uid);
    setSelectedChatPage(user_info->_uid);
    slot_side_chat_clicked();
}

void ChatDialog::slot_chat_item_clicked(QListWidgetItem* item) {
    QWidget* widget = ui->chatting_list->itemWidget(item);
    if (widget == nullptr) {
        return;
    }

    // 转为item基类
    auto base_item = qobject_cast<ListItemBase*>(widget);
    if (base_item == nullptr) {
        return;
    }

    auto type = base_item->item_type();
    switch (type) {
    case ListItemType::CHAT_USER_ITEM:
    {
        auto* chat_user_item = qobject_cast<ChatUserItem*>(base_item);
        if (chat_user_item == nullptr) {
            return;
        }

        auto user_info = chat_user_item->GetUserInfo();
        ui->chat_page->SetUserInfo(user_info);
        break;
    }
    default:
    {
        qDebug() << "unknown item type:" << type;
        return;
    }
    }
}

void ChatDialog::slot_append_send_chat_msg(std::shared_ptr<TextChatData> msg) {
    if (cur_chat_uid_ == 0) {
        return;
    }

    auto iter = chat_items_map_.find(cur_chat_uid_);
    if (iter == chat_items_map_.end()) {
        return;
    }

    QWidget* widget = ui->chatting_list->itemWidget(iter.value());
    if (!widget) {
        return;
    }

    auto* base_item = qobject_cast<ListItemBase*>(widget);
    if (!base_item) {
        return;
    }

    auto type = base_item->item_type();
    switch (type) {
    case ListItemType::CHAT_USER_ITEM:
    {
        auto* chat_user_item = qobject_cast<ChatUserItem*>(base_item);
        if (!chat_user_item) {
            return;
        }

        auto user_info = chat_user_item->GetUserInfo();
        user_info->_chat_msgs.push_back(msg); // 将信息添加到用户信息中
        std::vector<std::shared_ptr<TextChatData>> msgs;
        msgs.push_back(msg);
        // 将信息添加到好友信息中
        UserMgr::GetInstance()->AppendFriendChatMsg(cur_chat_uid_, msgs);
        break;
    }
    default:
    {
        qDebug() << "unknown item type:" << type;
        return;
    }
    }
}

void ChatDialog::slot_text_chat_msg(std::shared_ptr<TextChatMsg> msg) {
    auto iter = chat_items_map_.find(msg->_from_uid);
    if (iter != chat_items_map_.end()) {
        QWidget* widget = ui->chatting_list->itemWidget(iter.value());
        auto chat_user_item = qobject_cast<ChatUserItem*>(widget);
        if (!chat_user_item) {
            return;
        }
        chat_user_item->UpdateLastMsg(msg->_chat_msgs);
        // 更新聊天页面记录
        UpdateChatMsg(msg->_chat_msgs);
        UserMgr::GetInstance()->AppendFriendChatMsg(msg->_from_uid, msg->_chat_msgs);
        return;
    }

    // 没找到，创建新的item
    auto* chat_user_item = new ChatUserItem();
    auto friend_info = UserMgr::GetInstance()->GetFriendById(msg->_from_uid);
    chat_user_item->SetInfo(friend_info);
    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(chat_user_item->sizeHint());
    chat_user_item->UpdateLastMsg(msg->_chat_msgs);
    UserMgr::GetInstance()->AppendFriendChatMsg(msg->_from_uid, msg->_chat_msgs);
    ui->chatting_list->insertItem(0, item);
    ui->chatting_list->setItemWidget(item, chat_user_item);
    chat_items_map_.insert(msg->_from_uid, item);
}

void ChatDialog::slot_friend_apply(std::shared_ptr<AddFriendApply> apply) {
    // 检查是否已经申请
    bool is_exist = UserMgr::GetInstance()->AlreadyApply(apply->_from_uid);
    if (is_exist) {
        return;
    }

    UserMgr::GetInstance()->AddApplyList(std::make_shared<ApplyInfo>(apply));
    ui->side_contacts_lb->ShowRedPoint(true);
    ui->contacts_list->ShowRedPoint(true);
    ui->friend_apply_page->AddNewApply(apply);
}

void ChatDialog::slot_add_auth_firend(std::shared_ptr<AuthInfo> auth_info) {
    auto isExist = UserMgr::GetInstance()->CheckFriendById(auth_info->_uid);
    if (isExist) {
        return;
    }

    UserMgr::GetInstance()->AddFriend(auth_info);

    auto* chat_user_item = new ChatUserItem();
    auto user_info = std::make_shared<UserInfo>(auth_info);
    chat_user_item->SetInfo(user_info);
    auto* item = new QListWidgetItem();
    item->setSizeHint(chat_user_item->sizeHint());
    ui->chatting_list->insertItem(0, item);
    ui->chatting_list->setItemWidget(item, chat_user_item);
    chat_items_map_.insert(auth_info->_uid, item);
}

void ChatDialog::slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp) {
    auto isExist = UserMgr::GetInstance()->CheckFriendById(auth_rsp->_uid);
    if (isExist) {
        return;
    }

    UserMgr::GetInstance()->AddFriend(auth_rsp);

    auto* chat_user_item = new ChatUserItem();
    auto user_info = std::make_shared<UserInfo>(auth_rsp);
    chat_user_item->SetInfo(user_info);
    auto* item = new QListWidgetItem();
    item->setSizeHint(chat_user_item->sizeHint());
    ui->chatting_list->insertItem(0, item);
    ui->chatting_list->setItemWidget(item, chat_user_item);
    chat_items_map_.insert(auth_rsp->_uid, item);
}

void ChatDialog::slot_jump_chat_item(std::shared_ptr<SearchInfo> si) {
    auto iter = chat_items_map_.find(si->_uid);
    if (iter != chat_items_map_.end()) {
        // 找到了已在聊天列表的item
        ui->chatting_list->scrollToItem(iter.value());
        ui->side_chat_lb->SetSelected(true); // 跳转到聊天页面
        setSelectedChatItem(si->_uid);       // 设置选中的item
        setSelectedChatPage(si->_uid);       // 跳转对应的聊天窗口
        slot_side_chat_clicked();            // 设置侧边栏状态
        return;
    }

    // 没找到，创建新的item
    auto* chat_user_item = new ChatUserItem();
    auto user_info = std::make_shared<UserInfo>(si);
    chat_user_item->SetInfo(user_info);
    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(chat_user_item->sizeHint());
    ui->chatting_list->insertItem(0, item);
    ui->chatting_list->setItemWidget(item, chat_user_item); // 添加自定义item(widget)
    chat_items_map_.insert(si->_uid, item);

    ui->side_chat_lb->SetSelected(true);
    setSelectedChatItem(si->_uid);
    setSelectedChatPage(si->_uid);
    slot_side_chat_clicked();
}
