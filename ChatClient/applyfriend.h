#ifndef APPLYFRIEND_H
#define APPLYFRIEND_H

#include <QDialog>
#include "tcpmgr.h"
#include "clickedlabel.h"
#include "friendlabel.h"

constexpr int tip_offset = 5;
const QString add_prefix = "添加标签 ";
//申请好友标签输入框最低长度
constexpr int MIN_APPLY_LABEL_ED_LEN = 40;

namespace Ui
{
    class ApplyFriend;
}

class ApplyFriend : public QDialog
{
    Q_OBJECT

public:
    explicit ApplyFriend(QWidget *parent = nullptr);
    ~ApplyFriend();

    void InitTipLbs();
    void AddTipLbs(ClickedLabel *lb, QPoint cur_point, QPoint &next_point, int text_width, int text_height);
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    void SetSearchInfo(std::shared_ptr<SearchInfo> si);

private:
    void initUi();
    void initSignals();
    void resetLabels();
    void addLabel(const QString &name);

private:
    Ui::ApplyFriend *ui;
    // 已有标签
    QMap<QString, ClickedLabel *> _add_labels;
    QVector<QString> _add_label_keys;
    QPoint _label_point;
    // 新增标签
    QMap<QString, FriendLabel *> _friend_labels;
    QVector<QString> _friend_label_keys;
    QVector<QString> _tip_data;
    QPoint _tip_cur_point;
    std::shared_ptr<SearchInfo> _si;

public slots:
    // 显示更多label标签
    void ShowMoreLabel();
    // 输入label按下回车触发将标签加入展示栏
    void SlotLabelEnter();
    // 点击关闭，移除展示栏好友便签
    void SlotRemoveFriendLabel(QString name);
    // 通过点击tip实现增加和减少好友便签
    void SlotChangeFriendLabelByTip(QString lbtext, ClickLbState state);
    // 输入框文本变化显示不同提示
    void SlotLabelTextChange(const QString &text);
    // 输入框输入完成
    void SlotLabelEditFinished();
    // 输入标签显示提示框，点击提示框内容后添加好友便签
    void SlotAddFirendLabelByClickTip(QString text);
    // 处理确认回调
    void SlotApplySure();
    // 处理取消回调
    void SlotApplyCancel();
};

#endif // APPLYFRIEND_H
