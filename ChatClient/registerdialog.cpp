#include "registerdialog.h"
#include "global.h"
#include "ui_registerdialog.h"
#include <QRegularExpression>
#include "httpmgr.h"


RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    ui->tip_label->setProperty("state","normal");
    repolish(ui->tip_label);
    connect(HttpMgr::GetInstance().get(),&HttpMgr::sig_reg_mod_finish,this,&RegisterDialog::slot_reg_mod_finish);
    initHttpHandlers();
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::showTip(QString str, bool ok)
{
    if (ok) {
        ui->tip_label->setProperty("state","normal");
    }else {

        ui->tip_label->setProperty("state","error");
    }
    ui->tip_label->setText(str);
    repolish(ui->tip_label);
}

void RegisterDialog::initHttpHandlers()
{
    //注册验证码回包逻辑
    _handlers.insert(RequestId::ID_GET_VARIFY_CODE, [this](const QJsonObject& jsonObj){
        auto error = static_cast<ErrorCodes>(jsonObj["error"].toInt());

        if (error != ErrorCodes::SUCCESS) {
            showTip(tr("参数错误"),false);
            return;
        }

        QString email = jsonObj["email"].toString();
        showTip(tr("验证码已发送到邮箱，注意查收"), true);
        qDebug()<< "email is" << email ;
    });

}

void RegisterDialog::on_get_code_clicked()
{
    auto email = ui->email_edit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    if (regex.match(email).hasMatch()) {
        //todo...发送http请求获取验证码
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/get_varifycode"),
                                            json_obj,RequestId::ID_GET_VARIFY_CODE,Modules::REGISTERMOD);

    }else {
        showTip(tr("邮箱地址不正确"),false);
    }
}

void RegisterDialog::slot_reg_mod_finish(RequestId id, QString res, ErrorCodes ec)
{
    if (ec != ErrorCodes::SUCCESS) {
        showTip(tr("网络请求错误"),false);
        return;
    }

    //解析json
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    //json解析错误
    if (jsonDoc.isNull()) {
        showTip(tr("json解析错误"),false);
        return;
    }

    //json解析错误
    if(!jsonDoc.isObject()){
        showTip(tr("json解析错误"),false);
        return;
    }

    if (_handlers.find(id)== _handlers.end()) {
        showTip(tr("获取验证码失败"),false);
        qDebug() << "handler"<< static_cast<int>(id) << "is not exist";
        return;
    }

    _handlers[id](jsonDoc.object());
    return;
}

