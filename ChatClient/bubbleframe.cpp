#include "bubbleframe.h"
#include "chatitembase.h"

#include <QPainter>

BubbleFrame::BubbleFrame(ChatRole role, QWidget* parent)
    :QFrame(parent), role_(role), margin_(3)
{
    pHLayout_ = new QHBoxLayout();
    if(role_ == ChatRole::Self){
        pHLayout_->setContentsMargins(margin_, margin_, WIDTH_TRIANGLE + margin_, margin_);
    }
    else{
        pHLayout_->setContentsMargins(WIDTH_TRIANGLE + margin_, margin_, margin_, margin_);
    }
    this->setLayout(pHLayout_);
}

void BubbleFrame::SetWidget(QWidget *w)
{
    if(pHLayout_->count() > 0){
        return ;
    }
    else{
        pHLayout_->addWidget(w);
    }
}

void BubbleFrame::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen); // 不会绘制任何边框或线条

    if (role_ == ChatRole::Self) {
        // 背景颜色
        QColor bg_color(158,234,106);
        painter.setBrush(QBrush(bg_color));
        // 画气泡
        QRect bg_rect = QRect(0, 0, this->width()-WIDTH_TRIANGLE, this->height());
        painter.drawRoundedRect(bg_rect, 5, 5); //圆角矩形
        // 画三角(小箭头)
        QPointF points[3] = {
            QPointF(bg_rect.x() + bg_rect.width(), 12),
            QPointF(bg_rect.x() + bg_rect.width(), 12 + WIDTH_TRIANGLE + 2),
            QPointF(bg_rect.x() + bg_rect.width() + WIDTH_TRIANGLE, 10 + WIDTH_TRIANGLE-WIDTH_TRIANGLE / 2),
        };
        painter.drawPolygon(points, 3);
    } else {
        //画气泡
        QColor bg_color(Qt::white);
        painter.setBrush(QBrush(bg_color));
        QRect bg_rect = QRect(WIDTH_TRIANGLE, 0, this->width()- WIDTH_TRIANGLE, this->height());
        painter.drawRoundedRect(bg_rect,5,5);
        //画小三角
        QPointF points[3] = {
            QPointF(bg_rect.x(), 12),
            QPointF(bg_rect.x(), 10+WIDTH_TRIANGLE +2),
            QPointF(bg_rect.x()- WIDTH_TRIANGLE, 10+WIDTH_TRIANGLE-WIDTH_TRIANGLE/2),
        };
        painter.drawPolygon(points, 3);
    }

    return QFrame::paintEvent(event);
}
