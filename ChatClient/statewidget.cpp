#include "statewidget.h"
#include <QVariant>
#include "global.h"
#include <QVBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <QMouseEvent>

StateWidget::StateWidget(QWidget *parent)
    : QWidget{parent}
{
    setCursor(Qt::PointingHandCursor);
    InitRedPoint();
    ClearState();
}

void StateWidget::SetState(QString normal, QString hover, QString press, QString select, QString select_hover, QString select_press)
{
    normal_ = normal;
    normal_hover_ = hover;
    normal_press_ = press;
    selected_ = select;
    selected_hover_ = select_hover;
    selected_press_ = select_press;
    setProperty("state", normal_);
    repolish(this);
}

ClickLbState StateWidget::GetCurState()
{
    return curState_;
}

void StateWidget::ClearState()
{
    curState_ = ClickLbState::Normal;
    setProperty("state", normal_);
    repolish(this);
}

void StateWidget::SetSelected(bool isSelected)
{
    if (isSelected)
    {
        curState_ = ClickLbState::Selected;
        setProperty("state", selected_);
    }
    else
    {
        curState_ = ClickLbState::Normal;
        setProperty("state", normal_);
    }
    repolish(this);
}

void StateWidget::InitRedPoint()
{
    redPoint_ = new QLabel(this);
    redPoint_->setObjectName("red_point");
    QVBoxLayout *layout = new QVBoxLayout(this);
    redPoint_->setAlignment(Qt::AlignCenter);
    layout->addWidget(redPoint_);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    redPoint_->hide();
}

void StateWidget::ShowRedPoint(bool show)
{
    redPoint_->setVisible(show);
}

void StateWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        // 已经被选中无需处理
        if (curState_ == ClickLbState::Selected)
        {
            QWidget::mousePressEvent(event);
            return;
        }

        if (curState_ == ClickLbState::Normal)
        {
            curState_ = ClickLbState::Selected;
            setProperty("state", selected_press_);
            repolish(this);
        }
        return;
    }
    QWidget::mousePressEvent(event);
}

void StateWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (curState_ == ClickLbState::Normal)
        {
            setProperty("state", normal_hover_);
        }
        else
        {
            setProperty("state", selected_hover_);
        }
        repolish(this);
        emit clicked();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void StateWidget::enterEvent(QEvent *event)
{
    if (curState_ == ClickLbState::Normal)
    {
        setProperty("state", normal_hover_);
    }
    else
    {
        setProperty("state", selected_hover_);
    }
    repolish(this);
    QWidget::enterEvent(event);
}

void StateWidget::leaveEvent(QEvent *event)
{
    if (curState_ == ClickLbState::Normal)
    {
        setProperty("state", normal_);
    }
    else
    {
        setProperty("state", selected_);
    }
    repolish(this);
    QWidget::leaveEvent(event);
}

void StateWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
