#include "resetdialog.h"
#include "ui_resetdialog.h"
#include "httpmgr.h"
#include <QRegularExpression>
#include <QJsonObject>
#include <QUrl>

ResetDialog::ResetDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ResetDialog)
{
    ui->setupUi(this);
    initUiDesign();
    initUiSignals();
    initHttpHandlers();
}

ResetDialog::~ResetDialog()
{
    delete ui;
}

bool ResetDialog::validateFields()
{
    const auto validators = {&ResetDialog::checkEmailValid,
                             &ResetDialog::checkPassValid,
                             &ResetDialog::checkVerifyValid};
    // 遍历验证函数数组，逐个调用
    for (const auto &v : validators)
    {
        if (!(this->*v)())
        {
            return false;
        }
    }
    return true;
}

bool ResetDialog::checkEmailValid()
{
    //验证邮箱的地址正则表达式
    auto email = ui->email_edit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(!match){
        //提示邮箱不正确
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }
    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool ResetDialog::checkPassValid()
{
    auto pass = ui->pwd_edit->text();
    if(pass.length() < 6 || pass.length()>15){
        //提示长度不准确
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }
    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");
    bool match = regExp.match(pass).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;
    }
    DelTipErr(TipErr::TIP_PWD_ERR);
    return true;
}

bool ResetDialog::checkVerifyValid()
{
    if(ui->verify_edit->text().isEmpty()){
        AddTipErr(TipErr::TIP_VERIFY_ERR, tr("验证码不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_VERIFY_ERR);
    return true;
}

void ResetDialog::initUiDesign()
{
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    ui->tip_label->clear();
}

void ResetDialog::initUiSignals()
{
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reset_mod_finish, this, &ResetDialog::slot_reset_mod_finish);
}

void ResetDialog::initHttpHandlers()
{
    _handlers.insert(RequestId::ID_GET_VERIFY_CODE, [this](const QJsonObject &jsonObj) {
        auto error = static_cast<ErrorCodes>(jsonObj["error"].toInt());

        if (error != ErrorCodes::SUCCESS) {
            showTip(tr("参数错误"),false);
            return;
        }

        QString email = jsonObj["email"].toString();
        showTip(tr("验证码已发送到邮箱，注意查收"), true);
        qDebug()<< "email is" << email ;
    });

    _handlers.insert(RequestId::ID_RESET_PWD, [this](const QJsonObject &jsonObj) {
        auto error = static_cast<ErrorCodes>(jsonObj["error"].toInt());

        if (error != ErrorCodes::SUCCESS) {
            showTip(tr("参数错误"),false);
            return;
        }

        auto email = jsonObj["email"].toString();
        showTip(tr("重置成功,点击返回登录"), true);
        qDebug()<< "email is" << email ;
    });
}

void ResetDialog::AddTipErr(const TipErr &tc, const QString &tips)
{
    _tip_errs[tc] = tips;
    showTip(tips, false);
}

void ResetDialog::DelTipErr(const TipErr &tc)
{
    _tip_errs.remove(tc);
    if (_tip_errs.empty())
    {
        ui->tip_label->clear();
        return;
    }
    showTip(_tip_errs.first(), false);
}

void ResetDialog::showTip(QString str, bool ok)
{
    if (ok)
    {
        ui->tip_label->setProperty("state", "normal");
    }
    else
    {
        ui->tip_label->setProperty("state", "error");
    }
    ui->tip_label->setText(str);
    repolish(ui->tip_label);
}

void ResetDialog::slot_reset_mod_finish(RequestId id, QString res, ErrorCodes ec)
{
    if(ec != ErrorCodes::SUCCESS) {
        showTip(tr("网络请求错误"),false);
        return;
    }

    auto jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        showTip(tr("json解析错误"),false);
        return;
    }
    // 回调函数
    _handlers[id](jsonDoc.object());
}

void ResetDialog::on_return_btn_clicked()
{
    emit switchLogin(this);
}


void ResetDialog::on_verify_btn_clicked()
{
    if (!checkEmailValid()) {
        showTip(tr("邮箱地址不正确"), false);
        return;
    }
    QJsonObject json_obj;
    json_obj["email"] = ui->email_edit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/get_verifycode"),
                                        json_obj, RequestId::ID_GET_VERIFY_CODE, Modules::RESETMOD);
}


void ResetDialog::on_sure_btn_clicked()
{
    if (!validateFields())
    {
        return;
    }
    QJsonObject json_obj;
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = ui->pwd_edit->text();
    json_obj["verifycode"] = ui->verify_edit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/reset_pwd"),
                                        json_obj, RequestId::ID_RESET_PWD, Modules::RESETMOD);

}

