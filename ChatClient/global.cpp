#include "global.h"

// 刷新QSS并重绘图形
std::function<void(QWidget*)> repolish = [](QWidget* w){
    w->style()->unpolish(w);
    w->style()->polish(w); // 用于QSS 样式更新
    w->update(); // 用于请求重新绘制控件本身的内容，会自动调用其 paintEvent()
};

QString gate_url_prefix  = "";
