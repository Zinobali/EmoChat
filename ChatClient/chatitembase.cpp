#include "chatitembase.h"

#include <QGridLayout>

ChatItemBase::ChatItemBase(ChatRole role, QWidget *parent)
    : QWidget{parent}, role_(role)
{
    // 昵称
    pNameLabel_ = new QLabel();
    pNameLabel_->setObjectName("chat_user_name");
    QFont font("Microsoft YaHei");
    font.setPointSize(9);
    pNameLabel_->setFont(font);
    pNameLabel_->setFixedHeight(20);
    // 头像
    pIconLabel_ = new QLabel();
    pIconLabel_->setScaledContents(true);
    pIconLabel_->setFixedSize(42, 42);
    // 聊天气泡
    pBubble_ = new QWidget();
    // 网格布局
    QGridLayout *pGLayout = new QGridLayout();
    pGLayout->setVerticalSpacing(3);
    pGLayout->setHorizontalSpacing(3);
    pGLayout->setMargin(3);
    // 弹簧(40*20)(水平策略:扩张)(垂直策略:保持最小)
    QSpacerItem *pSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    if (role_ == ChatRole::Self)
    {
        pNameLabel_->setContentsMargins(0, 0, 8, 0);
        pNameLabel_->setAlignment(Qt::AlignRight);
        // 加入布局
        pGLayout->addWidget(pNameLabel_, 0, 1, 1, 1);
        pGLayout->addWidget(pIconLabel_, 0, 2, 2, 1, Qt::AlignTop); // 头像在右
        pGLayout->addItem(pSpacer, 1, 0, 1, 1);
        pGLayout->addWidget(pBubble_, 1, 1, 1, 1);
        pGLayout->setColumnStretch(0, 2);
        pGLayout->setColumnStretch(1, 3);
    }
    else
    {
        pNameLabel_->setContentsMargins(8, 0, 0, 0);
        pNameLabel_->setAlignment(Qt::AlignLeft);
        pGLayout->addWidget(pIconLabel_, 0, 0, 2, 1, Qt::AlignTop); // 头像在左
        pGLayout->addWidget(pNameLabel_, 0, 1, 1, 1);
        pGLayout->addWidget(pBubble_, 1, 1, 1, 1);
        pGLayout->addItem(pSpacer, 2, 2, 1, 1);
        pGLayout->setColumnStretch(1, 3);
        pGLayout->setColumnStretch(2, 2);
    }
    this->setLayout(pGLayout);
}

void ChatItemBase::SetUserName(const QString &name)
{
    pNameLabel_->setText(name);
}

void ChatItemBase::SetUserIcon(const QPixmap &icon)
{
    pIconLabel_->setPixmap(icon);
}

void ChatItemBase::SetBubble(QWidget *w)
{
    QGridLayout *pGLayout = static_cast<QGridLayout *>(this->layout());
    pGLayout->replaceWidget(pBubble_, w); // 仅做布局交换，还需手动管理内存

    delete pBubble_;
    pBubble_ = w;
}
