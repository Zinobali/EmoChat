#include "clickedbtn.h"
#include <QVariant>
#include "global.h"

ClickedBtn::ClickedBtn(QWidget *parent)
    : QPushButton(parent)
{
}

void ClickedBtn::SetState(QString nomal, QString hover, QString press)
{
    normal_ = nomal;
    hover_ = hover;
    press_ = press;
    setProperty("state", normal_);
    repolish(this);
}

void ClickedBtn::mousePressEvent(QMouseEvent *event)
{
    setProperty("state", press_);
    repolish(this);
    QPushButton::mousePressEvent(event);
}

void ClickedBtn::mouseReleaseEvent(QMouseEvent *event)
{
    setProperty("state", hover_);
    repolish(this);
    QPushButton::mouseReleaseEvent(event);
}

void ClickedBtn::enterEvent(QEvent *event)
{
    setProperty("state", hover_);
    repolish(this);
    QPushButton::enterEvent(event);
}

void ClickedBtn::leaveEvent(QEvent *event)
{
    setProperty("state", normal_);
    repolish(this);
    QPushButton::leaveEvent(event);
}
