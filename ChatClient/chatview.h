#ifndef CHATVIEW_H
#define CHATVIEW_H

#include <QWidget>

namespace Ui {
class ChatView;
}

class ChatView : public QWidget
{
    Q_OBJECT

public:
    explicit ChatView(QWidget *parent = nullptr);
    ~ChatView();
    void AppendChatItem(QWidget *item);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onVScrollBarMoved(int min, int max);

private:
    Ui::ChatView *ui;
    bool b_appending;

};

#endif // CHATVIEW_H
