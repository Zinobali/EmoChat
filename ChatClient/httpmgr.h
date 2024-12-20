#ifndef HTTPMGR_H
#define HTTPMGR_H

#include <QObject>
#include "singleton.h"
#include "global.h"
#include <QUrl>
#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>

class HttpMgr : public QObject, public Singleton<HttpMgr>, public std::enable_shared_from_this<HttpMgr>
{
    friend class Singleton<HttpMgr>;
    Q_OBJECT

public:
    ~HttpMgr();
    void PostHttpReq(QUrl url, QJsonObject json, RequestId req_id, Modules mod);


private:
    HttpMgr(QObject *parent = nullptr);

    QNetworkAccessManager _manager;

private slots:
    void slot_http_finish(RequestId id, QString res, ErrorCodes err, Modules mod);

signals:
    void sig_http_finish(RequestId id, QString res, ErrorCodes err, Modules mod);
    void sig_reg_mod_finish(RequestId id, QString res, ErrorCodes err);
    void sig_reset_mod_finish(RequestId id, QString res, ErrorCodes err);
    void sig_login_mod_finish(RequestId id, QString res, ErrorCodes err);

};

#endif // HTTPMGR_H
