#include "chatview.h"
#include "ui_chatview.h"
#include <QPainter>
#include <QScrollBar>
#include <QStyleOption>
#include <QTimer>
#include <climits>

ChatView::ChatView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatView)
{
    ui->setupUi(this);

    QVBoxLayout* VLayout_1 = new QVBoxLayout();
    VLayout_1->addWidget(new QWidget(), INT_MAX);
    ui->chat_bg->setLayout(VLayout_1);

    QScrollBar *pVScrollBar = ui->chat_area->verticalScrollBar();
    connect(pVScrollBar, &QScrollBar::rangeChanged,this, &ChatView::onVScrollBarMoved);

    QHBoxLayout *pHLayout_2 = new QHBoxLayout();
    pHLayout_2->addWidget(pVScrollBar, 0, Qt::AlignRight);
    pHLayout_2->setMargin(0);
    ui->chat_area->setLayout(pHLayout_2);
    ui->chat_area->setWidgetResizable(true);
    ui->chat_area->installEventFilter(this);
    pVScrollBar->setHidden(true);
}

ChatView::~ChatView()
{
    delete ui;
}

void ChatView::AppendChatItem(QWidget *item)
{
    QVBoxLayout* vLayout = static_cast<QVBoxLayout*>(ui->chat_area->widget()->layout());
    vLayout->insertWidget(vLayout->count()-1, item);
    b_appending = true;
}

void ChatView::removeAllItem()
{
    // todo ...
    // QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(m_pScrollArea->widget()->layout());

    // int count = layout->count();

    // for (int i = 0; i < count - 1; ++i) {
    //     QLayoutItem *item = layout->takeAt(0); // 始终从第一个控件开始删除
    //     if (item) {
    //         if (QWidget *widget = item->widget()) {
    //             delete widget;
    //         }
    //         delete item;
    //     }
    // }
}

bool ChatView::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Enter && watched == ui->chat_area) {
        ui->chat_area->verticalScrollBar()->setHidden(ui->chat_area->verticalScrollBar()->maximum() == 0);
    }else if (event->type() == QEvent::Leave && watched == ui->chat_area) {
        ui->chat_area->verticalScrollBar()->setHidden(true);
    }
    return QWidget::eventFilter(watched, event);
}

void ChatView::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChatView::onVScrollBarMoved(int min, int max)
{
    if (b_appending) {
        QScrollBar *pVScrollBar = ui->chat_area->verticalScrollBar();
        pVScrollBar->setSliderPosition(pVScrollBar->maximum());
        //500毫秒内可能调用多次
        QTimer::singleShot(500, [this]() {
            b_appending = false;
        });
    }
}
