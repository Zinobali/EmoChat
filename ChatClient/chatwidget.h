#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QStyle>
#include "user.h"
#include <memory>
#include "message.h"
#include <QStandardItemModel>

namespace Ui {
class ChatWidget;
}

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(const std::shared_ptr<User> &currentUser, const std::shared_ptr<User> &friendUser, QWidget *parent = nullptr);
    ~ChatWidget();
    //读json
    void readJson();

private:
    Ui::ChatWidget *ui;
    std::shared_ptr<User> m_currentUser;
    std::shared_ptr<User> m_friendUser;
    QStandardItemModel *m_model;
    
    
};

#endif // CHATWIDGET_H
