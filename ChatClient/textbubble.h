#ifndef TEXTBUBBLE_H
#define TEXTBUBBLE_H

#include "bubbleframe.h"

#include <QTextEdit>

class TextBubble : public BubbleFrame
{
    Q_OBJECT
public:
    TextBubble(ChatRole role,  const QString &text,  QWidget *parent = nullptr);
    void SetPlainText(const QString &text);
    ~TextBubble();

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QTextEdit *pTextEdit_;
    qreal last_text_height_; // 普通成员变量

    void adjustTextHeight();
};

#endif // TEXTBUBBLE_H
