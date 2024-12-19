#ifndef TIMERBTN_H
#define TIMERBTN_H

#include <QPushButton>
#include <QTimer>

class TimerBtn : public QPushButton
{
public:
    TimerBtn(QWidget *parent = nullptr);
    explicit TimerBtn(int countdown, QWidget *parent = nullptr);
    ~TimerBtn();

protected:
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    void reset();

protected slots:
    void onTimeout();

private:
    QTimer  *_timer;
    int _countdown;
    int _counter;
    QString _btn_text;
};

#endif // TIMERBTN_H
