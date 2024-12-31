#ifndef MSGTEXTEDIT_H
#define MSGTEXTEDIT_H

#include <QTextEdit>

constexpr size_t MAX_FILE_SIZE = 1024 * 1024 * 100; // 100M

enum MsgType
{
    Text,
    Image,
    File
};

struct MsgInfo
{
    MsgType msgFlag; // Text,Image,File
    QString content; // 表示文件和图像的url,文本信息
    QPixmap pixmap;  // 文件和图片的缩略图
};

class MsgTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    MsgTextEdit(QWidget *parent = nullptr);
    QVector<MsgInfo> getMsgList();

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;
    virtual void insertFromMimeData(const QMimeData *source) override;

private:
    void insertImages(const QString &url);
    void insertTextFile(const QString &url);
    bool isImage(QString url); // 判断文件是否为图片
    void insertMsgList(QVector<MsgInfo> &list, MsgType flag, QString text, QPixmap pix);
    void insertFileFromUrl(const QStringList &urls);
    QStringList getUrl(QString text);
    QPixmap getFileIconPixmap(const QString &url); // 获取文件图标及大小信息，并转化成图片
    QString getFileSize(qint64 size);              // 获取文件大小

private:
    QVector<MsgInfo> msgList_;
    QVector<MsgInfo> getMsgList_;

signals:
    void sig_send_msg();
};

#endif // MSGTEXTEDIT_H
