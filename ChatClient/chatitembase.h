#ifndef CHATITEMBASE_H
#define CHATITEMBASE_H

#include <QLabel>
#include <QWidget>

enum class ChatRole
{
    Self,
    Other
};

class ChatItemBase : public QWidget
{
    Q_OBJECT
public:
    explicit ChatItemBase(ChatRole role, QWidget *parent = nullptr);

    void SetUserName(const QString &name);
    void SetUserIcon(const QPixmap &icon);
    void SetBubble(QWidget *w);


private:
    ChatRole role_;
    QLabel* pNameLabel_;
    QLabel* pIconLabel_;
    QWidget* pBubble_;

signals:
};

#endif // CHATITEMBASE_H
