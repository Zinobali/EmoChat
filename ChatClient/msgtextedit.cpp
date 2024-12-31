#include "msgtextedit.h"

#include <QDragEnterEvent>
#include <QFileInfo>
#include <QMimeData>
#include <QMessageBox>
#include <QPainter>
#include <QIcon>
#include <QFileIconProvider>
// #include <QDebug>

MsgTextEdit::MsgTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
}

QVector<MsgInfo> MsgTextEdit::getMsgList()
{
    getMsgList_.clear();

    QString doc = this->document()->toPlainText(); // 获取纯文本
    QString text;
    int indexUrl = 0;
    int count = msgList_.size();
    // 逐字符处理
    for (int i = 0; i < doc.size(); i++)
    {
        // 某些无法显示的字符，可能是图像、表情符号、或者其他一些特殊符号
        if (doc[i] == QChar::ObjectReplacementCharacter)
        {
            // qDebug() << doc[i];
            if (!text.isEmpty())
            {
                QPixmap pix;
                insertMsgList(getMsgList_, MsgType::Text, text, pix);
                text.clear();
            }
            while (indexUrl < count)
            {
                MsgInfo msg = msgList_[indexUrl];
                if (this->document()->toHtml().contains(msg.content, Qt::CaseSensitive))
                {
                    indexUrl++;
                    getMsgList_.append(msg);
                    break;
                }
                indexUrl++;
            }
        }
        else
        {
            text.append(doc[i]);
        }
    }
    if (!text.isEmpty())
    {
        QPixmap pix;
        insertMsgList(getMsgList_, MsgType::Text, text, pix);
        text.clear();
    }
    msgList_.clear();
    this->clear();
    return getMsgList_;
}

void MsgTextEdit::keyPressEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) && !(event->modifiers() & Qt::ShiftModifier))
    {
        emit sig_send_msg();
        return;
    }
    QTextEdit::keyPressEvent(event);
}

void MsgTextEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->source() == this)
        event->ignore();
    else
        event->accept();
}

void MsgTextEdit::dropEvent(QDropEvent *event)
{
    insertFromMimeData(event->mimeData());
    event->accept();
}

void MsgTextEdit::insertFromMimeData(const QMimeData *source)
{
    QStringList urls = getUrl(source->text());

    if (urls.isEmpty())
        return;

    foreach (QString url, urls)
    {
        if (isImage(url))
            insertImages(url);
        else
            insertTextFile(url);
    }
}

void MsgTextEdit::insertImages(const QString &url)
{
    QImage image(url);
    // 按比例缩放图片
    if (image.width() > 120 || image.height() > 80)
    {
        if (image.width() > image.height())
        {
            image = image.scaledToWidth(120, Qt::SmoothTransformation);
        }
        else
            image = image.scaledToHeight(80, Qt::SmoothTransformation);
    }
    QTextCursor cursor = this->textCursor();
    // QTextDocument *document = this->document();
    // document->addResource(QTextDocument::ImageResource, QUrl(url), QVariant(image));
    cursor.insertImage(image, url);

    insertMsgList(msgList_, MsgType::Image, url, QPixmap::fromImage(image));
}

void MsgTextEdit::insertTextFile(const QString &url)
{
    QFileInfo fileInfo(url);
    if (fileInfo.isDir())
    {
        QMessageBox::information(this, "提示", "只允许拖拽单个文件!");
        return;
    }

    if (fileInfo.size() > MAX_FILE_SIZE)
    {
        QMessageBox::information(this, "提示", "发送的文件大小不能大于100M");
        return;
    }

    QPixmap pix = getFileIconPixmap(url);
    QTextCursor cursor = this->textCursor();
    cursor.insertImage(pix.toImage(), url);
    insertMsgList(msgList_, MsgType::File, url, pix);
}

bool MsgTextEdit::isImage(QString url)
{
    QString imageFormat = "bmp,jpg,jpeg,png,tif,gif,pcx,tga,exif,fpx,svg,psd,cdr,pcd,dxf,ufo,eps,ai,raw,wmf,webp";
    QStringList imageFormatList = imageFormat.split(",");
    QFileInfo fileInfo(url);
    QString suffix = fileInfo.suffix();
    if (imageFormatList.contains(suffix, Qt::CaseInsensitive))
    {
        return true;
    }
    return false;
}

void MsgTextEdit::insertMsgList(QVector<MsgInfo> &list, MsgType flag, QString text, QPixmap pix)
{
    MsgInfo msg;
    msg.msgFlag = flag;
    msg.content = text;
    msg.pixmap = pix;
    list.append(msg);
}

void MsgTextEdit::insertFileFromUrl(const QStringList &urls)
{
    if (urls.isEmpty())
        return;

    foreach (QString url, urls)
    {
        if (isImage(url))
            insertImages(url);
        else
            insertTextFile(url);
    }
}

QStringList MsgTextEdit::getUrl(QString text)
{
    QStringList urls;
    if (text.isEmpty())
        return urls;

    QStringList list = text.split("\n");
    foreach (QString url, list)
    {
        if (!url.isEmpty())
        {
            QStringList str = url.split("///");
            if (str.size() >= 2)
                urls.append(str.at(1));
        }
    }
    return urls;
}

QPixmap MsgTextEdit::getFileIconPixmap(const QString &url)
{
    QFileIconProvider provder;
    QFileInfo fileinfo(url);
    QIcon icon = provder.icon(fileinfo);

    QString strFileSize = getFileSize(fileinfo.size());
    // qDebug() << "FileSize=" << fileinfo.size();

    QFont font(QString("宋体"), 10, QFont::Normal, false);
    QFontMetrics fontMetrics(font);
    QSize textSize = fontMetrics.size(Qt::TextSingleLine, fileinfo.fileName());

    QSize FileSize = fontMetrics.size(Qt::TextSingleLine, strFileSize);
    int maxWidth = textSize.width() > FileSize.width() ? textSize.width() : FileSize.width();
    QPixmap pix(50 + maxWidth + 10, 50);
    pix.fill();

    QPainter painter;
    // painter.setRenderHint(QPainter::Antialiasing, true);
    // painter.setFont(font);
    painter.begin(&pix);
    // 文件图标
    QRect rect(0, 0, 50, 50);
    painter.drawPixmap(rect, icon.pixmap(40, 40));
    painter.setPen(Qt::black);
    // 文件名称
    QRect rectText(50 + 10, 3, textSize.width(), textSize.height());
    painter.drawText(rectText, fileinfo.fileName());
    // 文件大小
    QRect rectFile(50 + 10, textSize.height() + 5, FileSize.width(), FileSize.height());
    painter.drawText(rectFile, strFileSize);
    painter.end();
    return pix;
}

QString MsgTextEdit::getFileSize(qint64 size)
{
    QString Unit;
    double num;
    if (size < 1024)
    {
        num = size;
        Unit = "B";
    }
    else if (size < 1024 * 1224)
    {
        num = size / 1024.0;
        Unit = "KB";
    }
    else if (size < 1024 * 1024 * 1024)
    {
        num = size / 1024.0 / 1024.0;
        Unit = "MB";
    }
    else
    {
        num = size / 1024.0 / 1024.0 / 1024.0;
        Unit = "GB";
    }
    return QString::number(num, 'f', 2) + " " + Unit;
}
