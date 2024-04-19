#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_socket = nullptr;
    initUI();
    test();
    // ui->userListTableWidget->setColumnCount(1);
    // ui->userListTableWidget->setIconSize(QSize(60,60));
    // ui->userListTableWidget->verticalHeader()->setDefaultSectionSize(70);
    // ui->userListTableWidget->setColumnWidth(0, 250);
    // ui->userListTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 默认头像
    m_defaultAvatar = QPixmap(":/icon/img/ultraman.png").scaled(ui->avatarLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->chatDefaultPage->setEnabled(false);
    // 隐藏子控件
    //  ui->chatWidget->hide();
    //  发送消息按钮信号槽连接
    // connect(ui->sendPushButton, &QPushButton::clicked, this, &MainWindow::sendPushButtonSlot);

    // 测试
    connect(ui->friendsListView, &FriendListView::doubleClicked, this, &MainWindow::onItemDoubleClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
    if (m_socket != nullptr)
    {
        m_socket->deleteLater();
    }
}

void MainWindow::test()
{
    m_user = std::make_shared<User>(1, "张三", "123", "");
    // qDebug() << "m_user->id(): " <<  m_user->id();
}

void MainWindow::loginSuccessSlot(User user, MsgSocket *socket) // 相当于构造函数
{
    m_user = std::make_shared<User>(user);
    m_socket = socket;
    setWindowTitle("当前登录用户：" + m_user->username());
    connect(m_socket, &MsgSocket::disconnected, this, [=]()
            {
                m_socket->deleteLater();
                m_socket = nullptr; });
    sendOnlineMessage();
    initUserInfo();
    initThread();
}

void MainWindow::processMessage(qint64 type, QByteArray data)
{
    // 用线程处理
}

void MainWindow::initUserInfo()
{
    // if (m_user.avatar().isEmpty())
    // {
    // ui->avatarLabel->setPixmap(m_defaultAvatar);
    // }
    // else
    // {
    //     // 读头像
    // }
    QPixmap pixmap = QPixmap(QString(":/icon/img/avatar%1.jpg").arg(m_user->id())).scaled(ui->avatarLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->avatarLabel->setPixmap(pixmap);
    ui->userNameLabel->setText(m_user->username());
}

void MainWindow::initThread()
{
    MessageProcessor *messageProcessor = new MessageProcessor(m_socket);
    connect(messageProcessor, &MessageProcessor::friendListUpdated, this, &MainWindow::updateFriendList);
    connect(messageProcessor, &MessageProcessor::textMessageReceived, this, &MainWindow::textMessageSlot);
    connect(m_socket, &MsgSocket::messageReady, messageProcessor, &MessageProcessor::processMessage);
    QThread *sub = new QThread;
    messageProcessor->moveToThread(sub);
    sub->start();
}

void MainWindow::sendOnlineMessage()
{
    onlineMessage *msg = new onlineMessage(m_user->id(), Message::SYSTEM_ID);
    m_socket->sendMsg(msg);
}

void MainWindow::initUI()
{
    // 隐藏菜单栏 状态栏
    ui->menubar->hide();
    ui->statusbar->hide();
    // 设置窗口高度
    setFixedHeight(ui->userWidget->height());
    setFixedWidth(width());
    // ui->chatWidget->setEnabled(false);
}

void MainWindow::textMessageSlot(int id, QString text)
{
    // 遍历好友id
    for (int row = 0; row < ui->friendsListView->model()->rowCount(); ++row)
    {
        QModelIndex index = ui->friendsListView->model()->index(row, 0);
        auto friendUser = index.data(Qt::UserRole).value<std::shared_ptr<User>>();
        if (friendUser->id() == id)
        {
            // 先尝试取出chatWidget
            ChatWidget *chatWidget = index.data(Qt::UserRole + 1).value<ChatWidget *>();
            if (chatWidget == nullptr)
            {
                chatWidget = new ChatWidget(m_user, friendUser, this);
                ui->chatStackedWidget->addWidget(chatWidget);
                ui->friendsListView->model()->setData(index, QVariant::fromValue(chatWidget), Qt::UserRole + 1);
            }
            else
            {
                ui->chatStackedWidget->setCurrentWidget(chatWidget);
            }
            // chatWidget
        }
    }

    // 存入消息

    // 判断是否是当前聊天窗口
    /*if(id == ui->userListTableWidget->currentItem()->data(Qt::UserRole).toInt())
    {
        ui->chatTextBrowser->append(QString("<p style='color:green'>%1: %2</p>").arg(ui->friendNameLabel->text()).arg(text));
    }
    else
    {
        m_messages.insert(id, text);
        setReddot(id);
    }*/
}

void MainWindow::updateFriendList(QVector<User> friends)
{
    ui->friendsListView->setFriends(friends);
    /*m_friends = friends;
    // 更新userListTableWidget
    //  ui->userListTableWidget->clear();
    ui->userListTableWidget->setRowCount(m_friends.size());

    for (User user : m_friends)
    {
        int row = 0;
        QTableWidgetItem *item = new QTableWidgetItem(user.username());
        item->setData(Qt::UserRole, user.id());
        // 如果头像为空，设置默认头像
        if (user.avatar().isEmpty())
        {
            item->setIcon(QIcon(QPixmap(QString(":/icon/img/avatar%1.jpg").arg(user.id()))));
            //icon大小设置为60*60
        }
        else
        {
            // 用户头像
        }
        ui->userListTableWidget->setItem(row, 0, item);
        // QString avatar = QString(":/icon/img/avatar%1.jpg").arg(user.id());
        // FriendListItem *flitem = new FriendListItem(user.username(), avatar ,this);
        // ui->userListTableWidget->setCellWidget();
        // 双击item qdebug输出data(Qt::UserRole)
        connect(ui->userListTableWidget, &QTableWidget::itemDoubleClicked, this, &MainWindow::itemDoubleClickedSlot);
        row++;
    }*/

    // 更新视图
    //  ui->userListTableWidget->update();
}

void MainWindow::itemDoubleClickedSlot(QTableWidgetItem *item)
{
    int uid = item->data(Qt::UserRole).toInt();
    // 获取消息
    QList<QString> messagesForId = m_messages.values(uid);

    // ui->userListTableWidget->setCurrentItem(item); // 关键关联
    qDebug() << "选中用户id:" << uid;
    // 传用户名给聊天窗口
    // ui->friendNameLabel->setText(item->text());
    // ui->chatWidget->setEnabled(true);
    // 显示消息
    if (messagesForId.isEmpty())
    {
        // ui->chatTextBrowser->clear();
    }
    else
    {
        // ui->chatTextBrowser->clear();
        for (QString message : messagesForId)
        {
            // 好友消息
            // ui->chatTextBrowser->append(QString("<p style='color:green'>%1: %2</p>").arg(ui->friendNameLabel->text()).arg(message));
        }
    }
}

void MainWindow::onItemDoubleClicked(const QModelIndex &index)
{
    auto friendUser = index.data(Qt::UserRole).value<std::shared_ptr<User>>();
    // 先尝试取出chatWidget
    ChatWidget *chatWidget = index.data(Qt::UserRole + 1).value<ChatWidget *>();
    if (chatWidget == nullptr)
    {
        // qDebug() << "用户:" << user.username() << "还没有chatWidget";
        // 新建窗口
        chatWidget = new ChatWidget(m_user, friendUser, this);
        ui->chatStackedWidget->addWidget(chatWidget);
        ui->chatStackedWidget->setCurrentWidget(chatWidget);
        ui->friendsListView->model()->setData(index, QVariant::fromValue(chatWidget), Qt::UserRole + 1);
    }
    else
    {
        // qDebug() << "用户:" << user.username() << "已经存在chatWidget";
        ui->chatStackedWidget->setCurrentWidget(chatWidget);
    }
}

void MainWindow::sendPushButtonSlot()
{
    QString content;
    // QString content = ui->chatTextEdit->toPlainText();
    if (content.isEmpty())
    {
        return;
    }
    if (false)
    // if (ui->friendNameLabel->text().isEmpty())
    {
        return;
    }
    if (m_socket != nullptr)
    {
        // 发送消息
        // 获取当前选中的用户
        /*QTableWidgetItem *item = ui->userListTableWidget->currentItem();
        int uid = item->data(Qt::UserRole).toInt();
        TextMessage *msg = new TextMessage(m_user.id(), uid, content);
        m_socket->sendMsg(msg);
        ui->chatTextEdit->clear();
        // 添加到聊天记录
        ui->chatTextBrowser->append(QString("<p style='color:blue'>%1: %2</p>").arg(m_user.username()).arg(content));
    */
    }
}

void MainWindow::setReddot(int id)
{
    // for (int i = 0; i < ui->userListTableWidget->rowCount(); i++)
    // {
    //     QTableWidgetItem *item = ui->userListTableWidget->item(i, 0);
    //     if (item->data(Qt::UserRole).toInt() == id)
    //     {
    //         if (item->text().contains("*"))
    //         {
    //             continue;
    //         }
    //         item->setText(item->text() + "*");
    //     }
    // }
}

void MainWindow::on_pushButton_clicked()
{
}
