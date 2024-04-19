#include "chatwidget.h"
#include "ui_chatwidget.h"


ChatWidget::ChatWidget(const std::shared_ptr<User> &currentUser, const std::shared_ptr<User> &friendUser, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatWidget)
{
    ui->setupUi(this);
    m_currentUser = currentUser;
    m_friendUser = friendUser;
    ui->chatListView->setCurrentUser(m_currentUser);
    ui->chatListView->setFriendUser(m_friendUser);

    m_model = new QStandardItemModel(this);
    ui->chatListView->setModel(m_model);//绑定

    // qDebug()<<"ChatWidget的currentUser" << m_currentUser->username();

    ui->transPushButton->setIcon(style()->standardIcon(QStyle::SP_DirIcon));
    ui->friendNameLabel->setFont(QFont("微软雅黑", 20));
    ui->friendNameLabel->setText(m_friendUser->username());

    readJson();


}

ChatWidget::~ChatWidget()
{
    delete ui;
}

void ChatWidget::readJson()
{
    m_model->clear();
    //打开json文件
    QFile file("chatHistory.json");
    if(!file.open(QIODevice::ReadOnly)){
        qDebug()<<"打开json文件失败";
        return;
    }
    else
    {
        qDebug()<<"打开json文件成功";
    }
    QByteArray jsonData = file.readAll();
    file.close();
    if(jsonData.isEmpty()){
        qDebug()<<"jsonData为空";
        return;
    }
    QJsonArray jsonArray = QJsonDocument::fromJson(jsonData).array();
    for(int i = 0; i < jsonArray.size(); i++)
    {
        QJsonObject jsonObject = jsonArray.at(i).toObject();
        int id = jsonObject.value("uid").toInt();
        // QString username = jsonObject.value("username").toString();
        QString message = jsonObject.value("message").toString();
        QStandardItem *item = new QStandardItem;
        item->setData(id , Qt::UserRole);
        // item->setData(username,Qt::UserRole+1);//不显示名字
        item->setData(message,Qt::UserRole+1);

        m_model->appendRow(item);
    }

}
