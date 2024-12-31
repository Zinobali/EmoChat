#include "picturebubble.h"

#include <QLabel>
#include <QDebug>

constexpr int PIC_MAX_WIDTH = 160;
constexpr int PIC_MAX_HEIGHT = 90;

PictureBubble::PictureBubble(const QPixmap &picture, ChatRole role, QWidget *parent)
    :BubbleFrame(role, parent)
{
    QLabel *lb = new QLabel();
    lb->setScaledContents(true);
    QPixmap pix = picture.scaled(QSize(PIC_MAX_WIDTH, PIC_MAX_HEIGHT), Qt::KeepAspectRatio);
    lb->setPixmap(pix);
    SetWidget(lb);

    int left_margin = this->layout()->contentsMargins().left();
    int right_margin = this->layout()->contentsMargins().right();
    int v_margin = this->layout()->contentsMargins().bottom();
    int w = pix.width() + left_margin + right_margin;
    int h = pix.height() + v_margin * 2;
    setFixedSize(w, h);
}

PictureBubble::~PictureBubble()
{
}
