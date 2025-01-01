#ifndef FRIENDLABEL_H
#define FRIENDLABEL_H

#include "qframe.h"
#include <QWidget>

namespace Ui
{
    class FriendLabel;
}

class FriendLabel : public QFrame
{
    Q_OBJECT

public:
    explicit FriendLabel(QWidget *parent = nullptr);
    ~FriendLabel();
    void SetText(QString text);
    QString Text();
    int Width();
    int Height();

private:
    Ui::FriendLabel *ui;
    QString _text;
    int _width;
    int _height;

public slots:
    void slot_close();

signals:
    void sig_close(QString);
};

#endif // FRIENDLABEL_H
