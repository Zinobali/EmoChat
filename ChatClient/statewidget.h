#ifndef STATEWIDGET_H
#define STATEWIDGET_H

#include <QWidget>
#include "clickedlabel.h"

class StateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StateWidget(QWidget *parent = nullptr);

    void SetState(QString normal = "", QString hover = "", QString press = "",
                  QString select = "", QString select_hover = "", QString select_press = "");

    ClickLbState GetCurState();
    void ClearState();
    void SetSelected(bool isSelected);
    void InitRedPoint();
    void ShowRedPoint(bool show);

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

private:
    QString normal_;
    QString normal_hover_;
    QString normal_press_;
    QString selected_;
    QString selected_hover_;
    QString selected_press_;

    ClickLbState curState_;
    QLabel *redPoint_;

signals:
    void clicked();
    // void clicked(StateWidget *widget);
};

#endif // STATEWIDGET_H
