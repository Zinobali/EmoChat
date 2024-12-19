#include "timerbtn.h"
#include <QMouseEvent>
#include <QDebug>

TimerBtn::TimerBtn(QWidget *parent)
    : TimerBtn(10, parent) {}

TimerBtn::TimerBtn(int countdown, QWidget *parent)
    : QPushButton(parent), _timer(new QTimer(this)), _countdown(countdown), _counter(countdown)
{
    connect(_timer, &QTimer::timeout, this, &TimerBtn::onTimeout);
}

TimerBtn::~TimerBtn()
{
    _timer->stop();
}

void TimerBtn::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        _btn_text = this->text();
        setEnabled(false);
        setText(QString::number(_counter));
        _timer->start(1000);
        emit clicked(); // qt 5_15_2 版本需要手动触发
    }
    QPushButton::mouseReleaseEvent(event);
}

void TimerBtn::reset()
{
    _timer->stop();
    _counter = _countdown;  // 重置计数器
    setText(_btn_text);     // 恢复原始按钮文本
    setEnabled(true);       // 恢复按钮可用状态
}

void TimerBtn::onTimeout()
{
    if (--_counter <= 0) {
        reset();
    } else {
        setText(QString::number(_counter));
    }
}
