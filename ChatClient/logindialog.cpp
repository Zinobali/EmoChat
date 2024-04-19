#include "logindialog.h"
#include "ui_logindialog.h"
#include <QDebug>
#include "commonutils.h"
#include <QVariant>
#include "signupdialog.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    // 界面设置
    setWindowFlags(Qt::FramelessWindowHint | windowFlags()); // 去窗口边框
    setAttribute(Qt::WA_TranslucentBackground);              // 设置窗口的透明背景
    setFixedSize(this->width(), this->height());             // 设置固定大小
    // 禁用发送按钮
    ui->signInBtn->setEnabled(false);
    ui->usrLineEdit->setFocus(); // 设置焦点
    ui->signUpLabel->installEventFilter(this);
    readIni(); // 读配置文件

    // 连接登录信号槽
    connect(ui->signInBtn, &QPushButton::clicked, this, &LoginDialog::SignIn);
    connect(this, &LoginDialog::enterKeyPressed, this, &LoginDialog::SignIn);
    connect(ui->rembrCheckBox, &QCheckBox::stateChanged, this, &LoginDialog::ChangeRemberStateSlots);
    connect(this, &LoginDialog::signUpLabelClicked, this, &LoginDialog::goToSignUp);

    if (connectToChatServer())
    {
        // 启用发送按钮
        ui->signInBtn->setEnabled(true);
        // 连接成功
        qDebug() << "连接服务器成功";
        // connect(m_socket, &MsgSocket::readyRead, this, &LoginDialog::handleMessage);
        connect(m_socket, &MsgSocket::messageReady, this, &LoginDialog::processMessage);
        connect(m_socket, &MsgSocket::disconnected, [=](){
            m_socket->deleteLater();
            m_socket = nullptr;
            qDebug() << "服务器关闭"; });
    }
    else
    {
        // 连接失败
        qDebug() << "连接服务器失败";
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
    if(m_socket!= nullptr)
    {
        delete m_socket;
        m_socket = nullptr;
    }
}

void LoginDialog::SignIn()
{
    // 检查是否为空
    if (ui->usrLineEdit->text().isEmpty())
    {
        shakeWindow();
        return;
    }
    if (ui->pwdLineEdit->text().isEmpty())
    {
        shakeWindow();
        return;
    }
    sendSignInMessage();
}

void LoginDialog::on_closeBtn_clicked()
{
    close();
}

void LoginDialog::processMessage(qint64 type, QByteArray data)
{
    Message::TYPE msgType = static_cast<Message::TYPE>(type);
    MessageFactory factory;
    Message *msg = factory.createMessage(msgType, data);
    if (msgType == Message::TYPE::SIGN_IN_REPLY)
    {
        SignInReplyMessage *signInReply = qobject_cast<SignInReplyMessage *>(msg);
        if (signInReply->isValid())
        {
            emit loginSuccess(signInReply->user(), m_socket);
            m_socket = nullptr;
            // qDebug() << "登录窗口输出：";
            // qDebug() << "user" << signInReply->user().id();
            // qDebug() << "user" << signInReply->user().username();
            // qDebug() << "socket" << m_socket->socketDescriptor();
            if (m_IsRemember)
            {
                writeIni();
            }
            else
            {
                CommonUtils::clearIni(); // 清空
            }
            done(QDialog::Accepted);
        }
    }
}

void LoginDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        // 鼠标相对于窗口左上角的偏移量 = 鼠标在屏幕上的全局坐标 - 窗口的左上角在屏幕上的坐标
        m_Pos = event->globalPosition() - this->frameGeometry().topLeft();
    }
}

void LoginDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) // buttons!!!
    {
        move((event->globalPosition() - m_Pos).toPoint());
    }
}

void LoginDialog::keyPressEvent(QKeyEvent *event)
{
    // Qt::Key_Return 表示的是大键盘上的回车键，而 Qt::Key_Enter 表示的是小键盘上的确认键
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        emit enterKeyPressed();
    }
}

bool LoginDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->signUpLabel)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton)
            {
                emit signUpLabelClicked();
                return true;
            }
        }
    }
    return QDialog::eventFilter(watched, event);
}

void LoginDialog::goToSignUp()
{
    signUpDialog signUp;
    if (signUp.exec() == QDialog::Accepted)
    {
        qDebug() << "注册成功";
    }
}

void LoginDialog::ChangeRemberStateSlots()
{
    if (ui->rembrCheckBox->checkState() == Qt::Unchecked)
    {
        m_IsRemember = false;
    }
    else
    {
        m_IsRemember = true;
    }
}

void LoginDialog::writeIni()
{
    CommonUtils::writeIni("username", ui->usrLineEdit->text());
    CommonUtils::writeIni("password", ui->pwdLineEdit->text());
    CommonUtils::writeIni("checkState", QVariant(m_IsRemember).toString());
}

void LoginDialog::readIni()
{
    QString checkState = CommonUtils::readIni("checkState");
    if (checkState.isEmpty())
        return;
    ui->rembrCheckBox->setChecked(QVariant(checkState).toBool()); // 读记住状态

    if (m_IsRemember)
    {
        ui->usrLineEdit->setText(CommonUtils::readIni("username"));
        ui->pwdLineEdit->setText(CommonUtils::readIni("password"));
    }
}

void LoginDialog::shakeWindow()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "pos");
    animation->setDuration(100);                        // 动画持续事件
    animation->setLoopCount(3);                         // 动画循环次数
    animation->setEasingCurve(QEasingCurve::InOutQuad); // 缓动曲线

    int shakeDistance = 10; // 设置摇晃的幅度
    animation->setKeyValueAt(0.0, pos());
    animation->setKeyValueAt(0.5, pos() + QPoint(shakeDistance, 0));
    animation->setKeyValueAt(1.0, pos());
    // 动画开始，(停止自动删除)
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void LoginDialog::clearInput()
{
    ui->usrLineEdit->clear();
    ui->pwdLineEdit->clear();
    ui->rembrCheckBox->setChecked(false);
}

bool LoginDialog::connectToChatServer()
{
    // 建立服务器连接
    m_socket = new MsgSocket();
    m_socket->connectToHost(QHostAddress::LocalHost, 45678);
    return m_socket->waitForConnected();
}

void LoginDialog::sendSignInMessage() // 发送登录信息给服务器
{
    Message *msg = new SignInMessage(ui->usrLineEdit->text().toInt(), 0, ui->pwdLineEdit->text(), this);
    qDebug() << "id" << msg->senderId();
    m_socket->sendMsg(msg);
    msg->deleteLater();
}

// void LoginDialog::handleMessage()
// {
//     qDebug() << "收到消息";
//     Message *msg = m_socket->readMsg();
//     switch (msg->type())
//     {
//     case Message::SIGN_IN_REPLY:
//     {
//         SignInReplyMessage *replyMsg = qobject_cast<SignInReplyMessage *>(msg);
//         qDebug() << "SIGN_IN_REPLY";
//         qDebug() << "replyMsg->isValid():" << replyMsg->isValid();
//         qDebug() << "uid" << replyMsg->user().id();
//         qDebug() << "uname" << replyMsg->user().username();
//     }
//     break;

//     default:
//         break;
//     }
//     delete msg;
// }
