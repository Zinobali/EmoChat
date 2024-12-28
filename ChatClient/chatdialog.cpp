#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QAction>
#include <QElapsedTimer>
#include <QMovie>
#include <QProgressBar>
#include <QRandomGenerator>
#include <QTimer>
#include <QDebug>
#include <vector>


ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChatDialog), mode_(ChatUIMode::ChatMode), state_(ChatUIMode::ChatMode), b_loading_(false)
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
    if (show) {
        ui->chatting_list->hide();
        ui->contacts_list->hide();
        ui->search_list->show();
        mode_ = ChatUIMode::SearchMode;
    }else if (state_ == ChatUIMode::ChatMode) {
        ui->search_list->hide();
        ui->contacts_list->hide();
        ui->chatting_list->show();
        mode_ = ChatUIMode::ChatMode;
    }else if (state_ == ChatUIMode::ContactMode) {
        ui->search_list->hide();
        ui->chatting_list->hide();
        ui->contacts_list->show();
        mode_ = ChatUIMode::ContactMode;
    }
}

void ChatDialog::initUI()
{
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    ui->add_btn->SetState("normal", "hover", "press"); //必须显示设置
    // 搜索框搜索图标
    QAction* searchAction = new QAction(ui->search_edit);
    searchAction->setIcon(QIcon(":/images/search.png"));
    ui->search_edit->addAction(searchAction, QLineEdit::LeadingPosition);
    // list模式
    ShowSearch(false);
    // 添加模拟聊天列表
    addChatUserList();
}

void ChatDialog::initSignals()
{
    connect(ui->chatting_list, &ChatUserList::sig_loading_chat_user, this, &ChatDialog::slot_loading_chat_user);
}

void ChatDialog::addChatUserList()
{
    for (int var = 0; var < 20; ++var) {
        int rand_val = QRandomGenerator::global()->bounded(100);
        int str_i = rand_val % strs.size();
        int head_i = rand_val % heads.size();
        int name_i = rand_val % names.size();

        // 生成一年内的随机时间
        QDateTime now = QDateTime::currentDateTime();
        int randomDays = QRandomGenerator::global()->bounded(365);  // 随机0到364天
        int randomSeconds = QRandomGenerator::global()->bounded(86400); // 随机一天中的秒数
        QDateTime randomTime = now.addDays(-randomDays).addSecs(-randomSeconds);

        auto* model = static_cast<ChatUserListModel*>(ui->chatting_list->model());
        model->AddChatUser(names[name_i], heads[head_i], strs[str_i], randomTime);
    }
}

void ChatDialog::slot_loading_chat_user()
{
    if (b_loading_) {
        return;
    }

    qDebug() << "loading users ...";
    // 添加加载动画
    b_loading_ = true;

    std::vector<ChatUser> users;
    for (int var = 0; var < 5; ++var) {
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
    auto* model = static_cast<ChatUserListModel*>(ui->chatting_list->model());
    model->AddChatUsers(users.cbegin(), users.cend());

    b_loading_ = false;
}

std::vector<QString> ChatDialog::strs = {
    "hello world !",
    "nice to meet u",
    "New year，new life",
    "You have to love yourself",
    "My love is written in the wind ever since the whole world is you"
};

std::vector<QString> ChatDialog::heads = {
    ":/images/head_1.png",
    ":/images/head_2.png",
    ":/images/head_3.png",
    ":/images/head_4.png",
    ":/images/head_5.png",
    ":/images/head_6.png",
    ":/images/head_7.png",
    ":/images/head_8.png"
};

std::vector<QString> ChatDialog::names = {
    "llfc",
    "zack",
    "golang",
    "cpp",
    "java",
    "nodejs",
    "python",
    "rust"
};





