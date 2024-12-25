#ifndef CLICKEDBTN_H
#define CLICKEDBTN_H

#include <QPushButton>
#include <QString>

class ClickedBtn : public QPushButton
{
    Q_OBJECT
public:
    ClickedBtn(QWidget * parent = nullptr);
    void SetState(QString nomal, QString hover, QString press);

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;

private:
    QString normal_;
    QString hover_;
    QString press_;
};

#endif // CLICKEDBTN_H
