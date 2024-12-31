#include "textbubble.h"

#include <QTextBlock>
#include <QTextDocument>
#include <QDebug>

TextBubble::TextBubble(ChatRole role, const QString &text, QWidget *parent)
    : BubbleFrame(role, parent), last_text_height_(0)
{
    pTextEdit_ = new QTextEdit();
    pTextEdit_->setReadOnly(true);
    pTextEdit_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pTextEdit_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pTextEdit_->installEventFilter(this);
    QFont font("Microsoft YaHei");
    font.setPointSize(12);
    pTextEdit_->setFont(font);
    SetPlainText(text);    // 填充文本
    SetWidget(pTextEdit_); // 替换气泡
}

bool TextBubble::eventFilter(QObject *watched, QEvent *event)
{
    if (pTextEdit_ == watched && QEvent::Paint == event->type())
    {
        adjustTextHeight();
    }
    return BubbleFrame::eventFilter(watched, event);
}

void TextBubble::adjustTextHeight()
{
    qreal text_height = pTextEdit_->document()->size().height();
    if (qFuzzyCompare(text_height, last_text_height_))
    {
        return;
    }
    last_text_height_ = text_height;

    qreal doc_margin = pTextEdit_->document()->documentMargin();
    int vMargin = this->layout()->contentsMargins().top();
    // 设置这个气泡需要的高度 文本高+文本边距+TextEdit边框到气泡边框的距离
    setFixedHeight(text_height + doc_margin * 2 + vMargin * 2);
}

void TextBubble::SetPlainText(const QString &text)
{
    pTextEdit_->setPlainText(text);

    qreal doc_margin = pTextEdit_->document()->documentMargin();
    int margin_left = this->layout()->contentsMargins().left();
    int margin_right = this->layout()->contentsMargins().right();
    QFontMetricsF fm(pTextEdit_->font());

    QTextDocument *doc = pTextEdit_->document();
    int max_width = 0;

    // 遍历每一段找到 最宽的那一段
    for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next()) // 字体总长
    {
        auto txt = it.text();
        if (txt.isEmpty())
        {
            continue;
        }

        int txtW = int(fm.horizontalAdvance(it.text()));
        max_width = std::max(max_width, txtW); // 找到最长的那段
    }

    // 设置最大宽度时，考虑左右边距、文档边距等
    int effective_max_width = max_width + doc_margin * 2 + (margin_left + margin_right);
    setMaximumWidth(effective_max_width); // 设置最大宽度
}

TextBubble::~TextBubble()
{
}
