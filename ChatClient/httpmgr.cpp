#include "httpmgr.h"
#include <QNetworkReply>
#include <QString>

HttpMgr::HttpMgr(QObject *parent)
    : QObject{parent}
{
    connect(this,&HttpMgr::sig_http_finish,this,&HttpMgr::slot_http_finish);
}

HttpMgr::~HttpMgr()
{
    qDebug() << "~HttpMgr()";
}

void HttpMgr::PostHttpReq(QUrl url, QJsonObject json, RequestId req_id, Modules mod)
{
    //创建一个HTTP POST请求，并设置请求头和请求体
    QByteArray data = QJsonDocument(json).toJson();
    //url请求
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));
    //发送请求，并处理响应，伪闭包
    auto self(shared_from_this());
    QNetworkReply* reply = _manager.post(request, data);
    // 连接信号槽
    connect(reply, &QNetworkReply::finished,[reply, self, req_id, mod](){
        //处理错误

        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->errorString();
            emit self->sig_http_finish(req_id, "", ErrorCodes::ERR_NETWORK, mod);

            reply->deleteLater();
            return;
        }
        //无错误则读回请求
        QString res = reply->readAll();
        //发送信号通知完成
        emit self->sig_http_finish(req_id, res, ErrorCodes::SUCCESS, mod);

        reply->deleteLater();
        return;
    });
}

void HttpMgr::slot_http_finish(RequestId id, QString res, ErrorCodes err, Modules mod)
{
    if (mod == Modules::REGISTERMOD) {
        emit sig_reg_mod_finish(id, res, err);
    }
    if (mod == Modules::RESETMOD) {
        emit sig_reset_mod_finish(id, res, err);
    }
    if (mod == Modules::LOGINMOD) {
        emit sig_login_mod_finish(id, res, err);
    }
}
