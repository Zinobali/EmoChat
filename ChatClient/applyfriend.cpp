#include "applyfriend.h"
#include "ui_applyfriend.h"
#include <QEvent>
#include "usermgr.h"
#include <QScrollBar>
#include <QJsonDocument>
#include <QJsonObject>
#include "tcpmgr.h"

ApplyFriend::ApplyFriend(QWidget *parent)
    : QDialog(parent), ui(new Ui::ApplyFriend), _label_point(2, 6)
{
    ui->setupUi(this);
    initUi();
    initSignals();
    InitTipLbs();
}

ApplyFriend::~ApplyFriend()
{
    delete ui;
}

void ApplyFriend::InitTipLbs()
{
    int lines = 1;
    for (int i = 0; i < _tip_data.size(); ++i)
    {
        auto *lb = new ClickedLabel(ui->lb_list);
        lb->SetState("normal", "hover", "pressed",
                     "selected_normal", "selected_hover", "selected_pressed");
        lb->setObjectName("tipslb");
        lb->setText(_tip_data[i]);
        connect(lb, &ClickedLabel::clicked, this, &ApplyFriend::SlotChangeFriendLabelByTip);

        QFontMetrics fontMetrics(lb->font());                      // 获取QLabel控件的字体信息
        int textWidth = fontMetrics.horizontalAdvance(lb->text()); // 获取文本的宽度
        int textHeight = fontMetrics.height();                     // 获取文本的高度

        if (_tip_cur_point.x() + textWidth + tip_offset > ui->lb_list->width())
        {
            lines++;
            if (lines > 2)
            {
                // 默认超过两行不显示
                delete lb;
                return;
            }

            _tip_cur_point.setX(tip_offset);
            _tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);
        }
        auto next_point = _tip_cur_point;
        AddTipLbs(lb, _tip_cur_point, next_point, textWidth, textHeight);
        _tip_cur_point = next_point;
    }
}

void ApplyFriend::AddTipLbs(ClickedLabel *lb, QPoint cur_point, QPoint &next_point, int text_width, int text_height)
{
    lb->move(cur_point);
    lb->show();
    _add_labels.insert(lb->text(), lb);
    _add_label_keys.push_back(lb->text());
    next_point.setX(lb->pos().x() + text_width + 15);
    next_point.setY(lb->pos().y());
}

bool ApplyFriend::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->scrollArea && event->type() == QEvent::Enter)
    {
        ui->scrollArea->verticalScrollBar()->setHidden(false);
    }
    else if (watched == ui->scrollArea && event->type() == QEvent::Leave)
    {
        ui->scrollArea->verticalScrollBar()->setHidden(true);
    }
    return QDialog::eventFilter(watched, event);
}

void ApplyFriend::SetSearchInfo(std::shared_ptr<SearchInfo> si)
{
    _si = si;
    auto applyname = UserMgr::GetInstance()->name();
    auto bakname = si->_name;
    ui->name_ed->setText(applyname);
    ui->back_ed->setText(bakname);
}

void ApplyFriend::initUi()
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setModal(true);
    ui->name_ed->setPlaceholderText(tr("狂野之裤"));
    ui->lb_ed->setPlaceholderText("搜索 | 添加标签");
    ui->back_ed->setPlaceholderText("永雏塔菲");

    ui->lb_ed->move(2, 2);
    ui->lb_ed->setFixedHeight(20);
    ui->lb_ed->setMaxLength(10);
    ui->input_tip_wid->hide();
    ui->input_tip_wid->hide();
    _tip_cur_point = QPoint(5, 5);
    _tip_data = {"同学", "家人", "菜鸟教程", "C++ Primer", "Rust 程序设计",
                 "父与子学Python", "nodejs开发指南", "go 语言开发指南",
                 "游戏伙伴", "金融投资", "微信读书", "拼多多拼友"};
    ui->scrollArea->horizontalScrollBar()->setHidden(true);
    ui->scrollArea->verticalScrollBar()->setHidden(true);
    ui->scrollArea->installEventFilter(this);
    ui->sure_btn->SetState("normal", "hover", "press");
    ui->cancel_btn->SetState("normal", "hover", "press");
}

void ApplyFriend::initSignals()
{
    connect(ui->more_lb, &ClickedOnceLabel::clicked, this, &ApplyFriend::ShowMoreLabel);
    // 链接输入标签回车事件
    connect(ui->lb_ed, &CustomizeEdit::returnPressed, this, &ApplyFriend::SlotLabelEnter);
    connect(ui->lb_ed, &CustomizeEdit::textChanged, this, &ApplyFriend::SlotLabelTextChange);
    connect(ui->lb_ed, &CustomizeEdit::editingFinished, this, &ApplyFriend::SlotLabelEditFinished);
    connect(ui->tip_lb, &ClickedOnceLabel::clicked, this, &ApplyFriend::SlotAddFirendLabelByClickTip);
    // 连接确认和取消按钮的槽函数
    connect(ui->cancel_btn, &QPushButton::clicked, this, &ApplyFriend::SlotApplyCancel);
    connect(ui->sure_btn, &QPushButton::clicked, this, &ApplyFriend::SlotApplySure);
}

void ApplyFriend::resetLabels()
{
    auto max_width = ui->gridWidget->width();
    auto label_height = 0;
    for (auto iter = _friend_labels.begin(); iter != _friend_labels.end(); iter++)
    {
        // todo... 添加宽度统计
        if (_label_point.x() + iter.value()->width() > max_width)
        {
            _label_point.setY(_label_point.y() + iter.value()->height() + 6);
            _label_point.setX(2);
        }

        iter.value()->move(_label_point);
        iter.value()->show();

        _label_point.setX(_label_point.x() + iter.value()->width() + 2);
        _label_point.setY(_label_point.y());
        label_height = iter.value()->height();
    }
    if (_friend_labels.isEmpty())
    {
        ui->lb_ed->move(_label_point);
        return;
    }

    if (_label_point.x() + MIN_APPLY_LABEL_ED_LEN > ui->gridWidget->width())
    {
        ui->lb_ed->move(2, _label_point.y() + label_height + 6);
    }
    else
    {
        ui->lb_ed->move(_label_point);
    }
}

void ApplyFriend::addLabel(const QString &name)
{
    if (_friend_labels.find(name) != _friend_labels.end())
    {
        return;
    }

    auto tmplabel = new FriendLabel(ui->gridWidget);
    tmplabel->SetText(name);
    tmplabel->setObjectName("FriendLabel");

    auto max_width = ui->gridWidget->width();
    // todo... 添加宽度统计

    if (_label_point.x() + tmplabel->width() > max_width)
    {
        _label_point.setY(_label_point.y() + tmplabel->height() + 6);
        _label_point.setX(2);
    }

    tmplabel->move(_label_point);
    tmplabel->show();
    _friend_labels[tmplabel->Text()] = tmplabel;
    _friend_label_keys.push_back(tmplabel->Text());
    connect(tmplabel, &FriendLabel::sig_close, this, &ApplyFriend::SlotRemoveFriendLabel);
    _label_point.setX(_label_point.x() + tmplabel->width() + 2);
    if (_label_point.x() + MIN_APPLY_LABEL_ED_LEN > ui->gridWidget->width())
    {
        ui->lb_ed->move(2, _label_point.y() + tmplabel->height() + 2);
    }
    else
    {
        ui->lb_ed->move(_label_point);
    }
    ui->lb_ed->clear();
    if (ui->gridWidget->height() < _label_point.y() + tmplabel->height() + 2)
    {
        ui->gridWidget->setFixedHeight(_label_point.y() + tmplabel->height() * 2 + 2);
    }
}

ClickedLabel *ApplyFriend::createLabel(const QString &text, QPoint &cur_point, QPoint &next_point, QWidget *parent, int &lines, int max_lines)
{
    // 创建标签
    auto *lb = new ClickedLabel(parent);
    lb->SetState("normal", "hover", "pressed",
                 "selected_normal", "selected_hover", "selected_pressed");
    lb->setObjectName("tipslb");
    lb->setText(text);
    // 获取标签的宽度和高度
    QFontMetrics fontMetrics(lb->font());
    int textWidth = fontMetrics.horizontalAdvance(lb->text());
    int textHeight = fontMetrics.height();
    // 设置标签的位置
    if (cur_point.x() + textWidth + tip_offset > parent->width())
    {
        cur_point.setX(tip_offset);
        cur_point.setY(cur_point.y() + textHeight + 15);
        lines++; // 增加行数
    }

    // 超过最大行数则不展示
    if (lines > max_lines)
    {
        delete lb; // 删除多余的标签
        return nullptr;
    }

    AddTipLbs(lb, cur_point, next_point, textWidth, textHeight);
    return lb;
}

void ApplyFriend::addOrUpdateLabel(const QString &text, QWidget *parent, bool selectState)
{
    // 查找是否已存在标签
    auto find_it = _add_labels.find(text);
    if (find_it != _add_labels.end())
    {
        if (selectState)
        {
            find_it.value()->SetCurState(ClickLbState::Selected);
        }
        else
        {
            find_it.value()->ResetNormalState();
        }
        return;
    }

    // 不存在则新增标签
    int fake_line = 2; // 此功能还未设计完毕，预留接口，勿用
    auto *lb = createLabel(text, _tip_cur_point, _tip_cur_point, parent, fake_line);
    _add_labels[text] = lb;
    _add_label_keys.push_back(text);

    if (selectState)
    {
        lb->SetCurState(ClickLbState::Selected);
    }
}

void ApplyFriend::ShowMoreLabel()
{
    ui->more_lb_wid->hide();
    ui->lb_list->setFixedWidth(325);
    _tip_cur_point = QPoint(5, 5);
    auto next_point = _tip_cur_point;
    int textWidth;
    int textHeight;
    // 重排现有的label
    for (auto &added_key : _add_label_keys)
    {
        auto added_lb = _add_labels[added_key];
        QFontMetrics fontMetrics(added_lb->font());                  // 获取QLabel控件的字体信息
        textWidth = fontMetrics.horizontalAdvance(added_lb->text()); // 获取文本的宽度
        textHeight = fontMetrics.height();                           // 获取文本的高度

        if (_tip_cur_point.x() + textWidth + tip_offset > ui->lb_list->width())
        {
            _tip_cur_point.setX(tip_offset);
            _tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);
        }
        added_lb->move(_tip_cur_point);

        next_point.setX(added_lb->pos().x() + textWidth + 15);
        next_point.setY(_tip_cur_point.y());
        _tip_cur_point = next_point;
    }
    // 添加新的label
    for (int i = 0; i < _tip_data.size(); i++)
    {
        auto iter = _add_labels.find(_tip_data[i]);
        if (iter != _add_labels.end())
        {
            continue;
        }

        auto *lb = new ClickedLabel(ui->lb_list);
        lb->SetState("normal", "hover", "pressed",
                     "selected_normal", "selected_hover", "selected_pressed");
        lb->setObjectName("tipslb");
        lb->setText(_tip_data[i]);
        connect(lb, &ClickedLabel::clicked, this, &ApplyFriend::SlotChangeFriendLabelByTip);

        QFontMetrics fontMetrics(lb->font());                      // 获取QLabel控件的字体信息
        int textWidth = fontMetrics.horizontalAdvance(lb->text()); // 获取文本的宽度
        int textHeight = fontMetrics.height();                     // 获取文本的高度

        if (_tip_cur_point.x() + textWidth + tip_offset > ui->lb_list->width())
        {
            _tip_cur_point.setX(tip_offset);
            _tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);
        }

        next_point = _tip_cur_point;
        AddTipLbs(lb, _tip_cur_point, next_point, textWidth, textHeight);
        _tip_cur_point = next_point;
    }
    int diff_height = next_point.y() + textHeight + tip_offset - ui->lb_list->height();
    ui->lb_list->setFixedHeight(next_point.y() + textHeight + tip_offset);
    ui->scrollcontent->setFixedHeight(ui->scrollcontent->height() + diff_height);
}

void ApplyFriend::SlotLabelEnter()
{
    if (ui->lb_ed->text().isEmpty())
    {
        return;
    }

    auto text = ui->lb_ed->text();
    addLabel(ui->lb_ed->text());

    ui->input_tip_wid->hide();
    auto find_it = std::find(_tip_data.begin(), _tip_data.end(), text);
    // 找到已有标签,设置为选中
    if (find_it == _tip_data.end())
    {
        _tip_data.push_back(text);
    }

    // 判断标签展示栏是否有该标签
    auto find_add = _add_labels.find(text);
    if (find_add != _add_labels.end())
    {
        find_add.value()->SetCurState(ClickLbState::Selected);
        return;
    }

    // 标签展示栏也增加一个标签, 并设置绿色选中
    auto *lb = new ClickedLabel(ui->lb_list);
    lb->SetState("normal", "hover", "pressed", "selected_normal",
                 "selected_hover", "selected_pressed");
    lb->setObjectName("tipslb");
    lb->setText(text);
    connect(lb, &ClickedLabel::clicked, this, &ApplyFriend::SlotChangeFriendLabelByTip);
    qDebug() << "ui->lb_list->width() is " << ui->lb_list->width();
    qDebug() << "_tip_cur_point.x() is " << _tip_cur_point.x();

    QFontMetrics fontMetrics(lb->font());                      // 获取QLabel控件的字体信息
    int textWidth = fontMetrics.horizontalAdvance(lb->text()); // 获取文本的宽度
    int textHeight = fontMetrics.height();                     // 获取文本的高度
    qDebug() << "textWidth is " << textWidth;

    if (_tip_cur_point.x() + textWidth + tip_offset + 3 > ui->lb_list->width())
    {
        _tip_cur_point.setX(5);
        _tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);
    }

    auto next_point = _tip_cur_point;

    AddTipLbs(lb, _tip_cur_point, next_point, textWidth, textHeight);

    _tip_cur_point = next_point;

    int diff_height = next_point.y() + textHeight + tip_offset - ui->lb_list->height();
    ui->lb_list->setFixedHeight(next_point.y() + textHeight + tip_offset);
    lb->SetCurState(ClickLbState::Selected);
    ui->scrollcontent->setFixedHeight(ui->scrollcontent->height() + diff_height);
}

void ApplyFriend::SlotRemoveFriendLabel(QString name)
{
    qDebug() << "receive close signal";
    _label_point.setX(2);
    _label_point.setY(6);

    auto find_iter = _friend_labels.find(name);
    if (find_iter == _friend_labels.end())
    {
        return;
    }

    auto find_key = _friend_label_keys.end();
    for (auto iter = _friend_label_keys.begin(); iter != _friend_label_keys.end(); iter++)
    {
        if (*iter == name)
        {
            find_key = iter;
            break;
        }
    }

    if (find_key != _friend_label_keys.end())
    {
        _friend_label_keys.erase(find_key);
    }

    delete find_iter.value();
    _friend_labels.erase(find_iter);
    resetLabels();

    auto find_add = _add_labels.find(name);
    if (find_add == _add_labels.end())
    {
        return;
    }

    find_add.value()->ResetNormalState();
}

void ApplyFriend::SlotChangeFriendLabelByTip(QString lbtext, ClickLbState state)
{
    auto find_iter = _add_labels.find(lbtext);
    if (find_iter == _add_labels.end())
    {
        return;
    }

    if (state == ClickLbState::Selected)
    {
        // 编写添加逻辑
        addLabel(lbtext);
        return;
    }

    if (state == ClickLbState::Normal)
    {
        // 编写删除逻辑
        SlotRemoveFriendLabel(lbtext);
        return;
    }
}

void ApplyFriend::SlotLabelTextChange(const QString &text)
{
    if (text.isEmpty())
    {
        ui->tip_lb->setText("");
        ui->input_tip_wid->hide();
        return;
    }

    auto iter = std::find(_tip_data.begin(), _tip_data.end(), text);
    if (iter == _tip_data.end())
    {
        auto new_text = add_prefix + text;
        ui->tip_lb->setText(new_text);
        ui->input_tip_wid->show();
        return;
    }
    ui->tip_lb->setText(text);
    ui->input_tip_wid->show();
}

void ApplyFriend::SlotLabelEditFinished()
{
    ui->input_tip_wid->hide();
}

void ApplyFriend::SlotAddFirendLabelByClickTip(QString text)
{
    int index = text.indexOf(add_prefix);
    if (index != -1)
    {
        text = text.mid(index + add_prefix.length());
    }
    addLabel(text);

    auto find_it = std::find(_tip_data.begin(), _tip_data.end(), text);
    // 找到了就只需设置状态为选中即可
    if (find_it == _tip_data.end())
    {
        _tip_data.push_back(text);
    }

    // 判断标签展示栏是否有该标签
    auto find_add = _add_labels.find(text);
    if (find_add != _add_labels.end())
    {
        find_add.value()->SetCurState(ClickLbState::Selected);
        return;
    }

    // 标签展示栏也增加一个标签, 并设置绿色选中
    auto *lb = new ClickedLabel(ui->lb_list);
    lb->SetState("normal", "hover", "pressed",
                 "selected_normal", "selected_hover", "selected_pressed");
    lb->setObjectName("tipslb");
    lb->setText(text);
    connect(lb, &ClickedLabel::clicked, this, &ApplyFriend::SlotChangeFriendLabelByTip);
    qDebug() << "ui->lb_list->width() is " << ui->lb_list->width();
    qDebug() << "_tip_cur_point.x() is " << _tip_cur_point.x();

    QFontMetrics fontMetrics(lb->font());                      // 获取QLabel控件的字体信息
    int textWidth = fontMetrics.horizontalAdvance(lb->text()); // 获取文本的宽度
    int textHeight = fontMetrics.height();                     // 获取文本的高度
    qDebug() << "textWidth is " << textWidth;

    if (_tip_cur_point.x() + textWidth + tip_offset + 3 > ui->lb_list->width())
    {
        _tip_cur_point.setX(5);
        _tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);
    }

    auto next_point = _tip_cur_point;
    AddTipLbs(lb, _tip_cur_point, next_point, textWidth, textHeight);
    _tip_cur_point = next_point;

    int diff_height = next_point.y() + textHeight + tip_offset - ui->lb_list->height();
    ui->lb_list->setFixedHeight(next_point.y() + textHeight + tip_offset);
    lb->SetCurState(ClickLbState::Selected);
    ui->scrollcontent->setFixedHeight(ui->scrollcontent->height() + diff_height);
}

void ApplyFriend::SlotApplySure()
{
    // 创建申请json
    QJsonObject jsonObj;
    auto uid = UserMgr::GetInstance()->uid();
    jsonObj["uid"] = uid;
    auto name = ui->name_ed->text();
    if (name.isEmpty())
    {
        return;
    }

    jsonObj["applyname"] = name;

    auto backName = ui->back_ed->text();
    if (backName.isEmpty())
    {
        return;
    }

    jsonObj["bakname"] = backName;
    jsonObj["touid"] = _si->_uid;

    QJsonDocument doc(jsonObj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    // 发送请求
    emit TcpMgr::GetInstance()->sig_send_data(RequestId::ID_ADD_FRIEND_REQ, data);

    this->hide();
    deleteLater();
}

void ApplyFriend::SlotApplyCancel()
{
    this->hide();
    deleteLater();
}
