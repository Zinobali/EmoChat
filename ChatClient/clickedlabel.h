#ifndef CLICKEDLABEL_H
#define CLICKEDLABEL_H

#include <QLabel>

enum class ClickLbState {
    Normal = 0,
    Selected = 1
};

class ClickedLabel : public QLabel
{
    Q_OBJECT
public:
    ClickedLabel(QWidget *parent=nullptr);
    ClickLbState GetCurState() const;
    void SetState(QString normal = "", QString hover = "", QString press = "",
                  QString select = "", QString select_hover = "", QString select_press = "");
    void ResetNormalState();

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;


signals:
    void clicked();

private:
    QString _normal;
    QString _normal_hover;
    QString _normal_press;
    QString _selected;
    QString _selected_hover;
    QString _selected_press;
    ClickLbState _curstate;
};

#endif // CLICKEDLABEL_H
