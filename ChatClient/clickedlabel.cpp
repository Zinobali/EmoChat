#include "clickedlabel.h"
#include <QVariant>
#include "global.h"
#include <QMouseEvent>

ClickedLabel::ClickedLabel(QWidget *parent)
    :QLabel(parent), _curstate(ClickLbState::Normal) {}

ClickLbState ClickedLabel::GetCurState() const {
    return _curstate;
}

void ClickedLabel::SetState(QString normal, QString hover, QString press, QString select, QString select_hover, QString select_press)
{
    _normal = normal;
    _normal_hover = hover;
    _normal_press = press;

    _selected = select;
    _selected_hover = select_hover;
    _selected_press = select_press;

    setProperty("state", normal);
    repolish(this);
}

void ClickedLabel::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if(_curstate == ClickLbState::Normal) {
            _curstate = ClickLbState::Selected;
            setProperty("state", _selected_hover);
            qDebug()<<"LeftButton clicked: "<< _selected_hover;
        } else {
            _curstate = ClickLbState::Normal;
            setProperty("state", _normal_hover);
            qDebug()<<"LeftButton clicked: "<< _normal_hover;
        }
        repolish(this);
        emit clicked();
    }
    QLabel::mousePressEvent(event);
}

void ClickedLabel::enterEvent(QEvent *event) {
    // change state to hover
    if(_curstate == ClickLbState::Normal) {
        setProperty("state", _normal_hover);
        qDebug()<<"mouse enter: "<< _normal_hover;
    } else {
        setProperty("state", _selected_hover);
        qDebug()<<"mouse enter: "<< _selected_hover;
    }
    repolish(this);
    QLabel::enterEvent(event);
}

void ClickedLabel::leaveEvent(QEvent *event) {
    if (_curstate == ClickLbState::Normal) {
        setProperty("state", _normal);
        qDebug()<<"mouse leave: "<< _normal;
    } else {
        setProperty("state", _selected);
        qDebug()<<"mouse leave: "<< _selected;
    }
    repolish(this);
    QLabel::leaveEvent(event);
}
