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
    // slot_side_chat_clicked();
    // 显示一个红点（测试用）
    ui->side_chat_lb->ShowRedPoint(true);
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
}

void ChatDialog::addChatUserList()
{
    for (int var = 0; var < 20; ++var)
    {
        int rand_val = QRandomGenerator::global()->bounded(100);
        int str_i = rand_val % strs.size();
        int head_i = rand_val % heads.size();
        int name_i = rand_val % names.size();

        // 生成一年内的随机时间
        QDateTime now = QDateTime::currentDateTime();
        int randomDays = QRandomGenerator::global()->bounded(365);      // 随机0到364天
        int randomSeconds = QRandomGenerator::global()->bounded(86400); // 随机一天中的秒数
        QDateTime randomTime = now.addDays(-randomDays).addSecs(-randomSeconds);

        auto *model = static_cast<ChatUserListModel *>(ui->chatting_list->model());
        model->AddChatUser(names[name_i], heads[head_i], strs[str_i], randomTime);
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

void ChatDialog::slot_loading_chat_user()
{
    if (b_loading_)
    {
        return;
    }

    qDebug() << "loading users ...";
    // 添加加载动画
    b_loading_ = true;

    std::vector<ChatUser> users;
    for (int var = 0; var < 5; ++var)
    {
        int rand_val = QRandomGenerator::global()->bounded(100);
        int str_i = rand_val % strs.size();
        int head_i = rand_val % heads.size();
        int name_i = rand_val % names.size();
        ChatUser u;
        u.name = names[name_i];
        u.head = heads[head_i];
        u.msg = strs[str_i];
        u.lastMsgTime = QDateTime::currentDateTime();
        users.push_back(u);
    }
    auto *model = static_cast<ChatUserListModel *>(ui->chatting_list->model());
    model->AddChatUsers(users.cbegin(), users.cend());

    b_loading_ = false;
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
