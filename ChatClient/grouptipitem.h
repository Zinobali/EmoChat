#ifndef GROUPTIPITEM_H
#define GROUPTIPITEM_H

#include <listitembase.h>

namespace Ui
{
    class GroupTipItem;
}

class GroupTipItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit GroupTipItem(QWidget *parent = nullptr);
    ~GroupTipItem();
    virtual QSize sizeHint() const override;
    void SetGroupTip(QString str);

private:
    Ui::GroupTipItem *ui;
};

#endif // GROUPTIPITEM_H
