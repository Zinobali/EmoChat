#include "customizeedit.h"

CustomizeEdit::CustomizeEdit(QWidget *parent) : QLineEdit(parent)
{
}

void CustomizeEdit::focusOutEvent(QFocusEvent *event)
{
    // 发送失去焦点得信号
    emit sig_foucus_out();
    QLineEdit::focusOutEvent(event);
}
