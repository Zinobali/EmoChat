#ifndef BUBBLEFRAME_H
#define BUBBLEFRAME_H

#include <QFrame>
#include <QHBoxLayout>

constexpr int WIDTH_TRIANGLE = 8;

enum class ChatRole;
class BubbleFrame : public QFrame
{
    Q_OBJECT
public:
    BubbleFrame(ChatRole role, QWidget* parent = nullptr);
    void SetWidget(QWidget *w);

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    QHBoxLayout *pHLayout_;
    ChatRole role_;
    int margin_;

};

#endif // BUBBLEFRAME_H
