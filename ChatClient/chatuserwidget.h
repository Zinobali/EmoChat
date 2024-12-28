#ifndef CHATUSERWIDGET_H
#define CHATUSERWIDGET_H

#include <QWidget>
#include "listitembase.h"

namespace Ui {
class ChatUserWidget;
}

class ChatUserWidget : public ListItemBase
{
    Q_OBJECT

public:
    explicit ChatUserWidget(QWidget *parent = nullptr);
    ~ChatUserWidget();

    void SetInfo(QString name, QString head, QString msg);
    QSize sizeHint() const override;

private:
    Ui::ChatUserWidget *ui;
    QString name_;
    QString head_;
    QString msg_;
};

#endif // CHATUSERWIDGET_H
