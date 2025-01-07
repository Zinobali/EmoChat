#include "loadingdlg.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMovie>

LoadingDlg::LoadingDlg(QWidget *parent)
    : QDialog(parent)
{
    resize(400, 300);
    // 创建垂直布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    // 创建标签并设置属性
    QLabel *loadingLabel = new QLabel(this);
    loadingLabel->setAlignment(Qt::AlignCenter); // 设置文本居中
    loadingLabel->setText("");                   // 初始文本为空
    layout->addWidget(loadingLabel);
    setLayout(layout);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint |
                   Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(parent->size());
    QMovie *movie = new QMovie(":/images/loading.gif");
    loadingLabel->setMovie(movie);
    movie->start();
    setModal(true);
}
