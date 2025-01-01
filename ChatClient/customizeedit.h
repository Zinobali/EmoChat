#ifndef CUSTOMIZEEDIT_H
#define CUSTOMIZEEDIT_H
#include <QLineEdit>

class CustomizeEdit : public QLineEdit
{
    Q_OBJECT
public:
    CustomizeEdit(QWidget *parent = nullptr);

protected:
    void focusOutEvent(QFocusEvent *event) override;

signals:
    void sig_foucus_out();
};

#endif // CUSTOMIZEEDIT_H
