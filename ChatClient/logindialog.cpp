#include "logindialog.h"
#include "ui_logindialog.h"
#include "httpmgr.h"
#include <QRegularExpression>
#include "global.h"


LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    initUiDesign();
    initUiSignals();
    initHttpHandlers();
    testFunc();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::initUiDesign()
{
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint); // 通过设置无边框窗口达到让dialog嵌入到主窗口的效果
    ui->forget_label->SetState("normal","hover","","selected","selected_hover","");
}

void LoginDialog::initUiSignals()
{
    connect(ui->forget_label, &ClickedLabel::clicked, this, [this](){
        emit switchReset(this);
    });
    //连接登录回包信号
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_login_mod_finish, this, &LoginDialog::slot_login_mod_finish);
    //连接tcp连接请求的信号和槽函数
    connect(this, &LoginDialog::sig_connect_tcp, TcpMgr::GetInstance().get(), &TcpMgr::slot_tcp_connect);
    //TcpMgr连接成功信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_con_success, this, &LoginDialog::slot_tcp_con_finish);
}

void LoginDialog::initHttpHandlers()
{
    _handlers.insert(RequestId::ID_LOGIN_USER, [this](const QJsonObject &jsonObj) {
        auto error = static_cast<ErrorCodes>(jsonObj["error"].toInt());

        if (error != ErrorCodes::SUCCESS) {
            showTip(tr("参数错误"), false);
            enableBtn(true);
            return;
        }

        ServerInfo s;
        s.Uid = jsonObj["uid"].toInt();
        s.Host = jsonObj["host"].toString();
        s.Port = jsonObj["port"].toString();
        s.Token = jsonObj["token"].toString();

        uid_ = s.Uid;
        token_ = s.Token;
        qDebug()<< "user is " << jsonObj["user"].toString() << " uid is " << s.Uid <<" host is "
                 << s.Host << " Port is " << s.Port << " Token is " << s.Token;
        emit sig_connect_tcp(s);
    });
}

void LoginDialog::AddTipErr(const TipErr &tc, const QString &tips)
{
    _tip_errs[tc] = tips;
    showTip(tips, false);
}

void LoginDialog::DelTipErr(const TipErr &tc)
{
    _tip_errs.remove(tc);
    if (_tip_errs.empty())
    {
        ui->err_tip->clear();
        return;
    }
    showTip(_tip_errs.first(), false);
}

void LoginDialog::showTip(QString str, bool ok)
{
    if (ok)
    {
        ui->err_tip->setProperty("state", "normal");
    }
    else
    {
        ui->err_tip->setProperty("state", "error");
    }
    ui->err_tip->setText(str);
    repolish(ui->err_tip);
}

bool LoginDialog::checkEmailValid()
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

bool LoginDialog::checkPassValid()
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

void LoginDialog::enableBtn(bool enabled)
{
    ui->login_btn->setEnabled(enabled);
    ui->reg_btn->setEnabled(enabled);
}

void LoginDialog::slot_login_mod_finish(RequestId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS){
        showTip(tr("网络请求错误"), false);
        enableBtn(true);
        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    if(jsonDoc.isNull() || !jsonDoc.isObject()){
        showTip(tr("json解析错误"),false);
        return;
    }
    _handlers[id](jsonDoc.object());
}

void LoginDialog::slot_tcp_con_finish(bool ok)
{
    if (!ok) {
        showTip(tr("网络异常"),false);
        enableBtn(true);
        return;
    }

    showTip(tr("聊天服务器连接成功，正在登录..."),true);
    QJsonObject jsonObj;
    jsonObj["uid"] = uid_;
    jsonObj["token"] = token_;

    QJsonDocument doc(jsonObj);
    auto jsonStr = QString::fromUtf8(doc.toJson(QJsonDocument::Indented));

    emit TcpMgr::GetInstance()->sig_send_data(RequestId::ID_CHAT_LOGIN, jsonStr);
}

void LoginDialog::on_reg_btn_clicked()
{
    emit switchRegister(this);
}

void LoginDialog::testFunc()
{
    ui->email_edit->setText("1665799927@qq.com");
    ui->pwd_edit->setText("123456");
}


void LoginDialog::on_login_btn_clicked()
{
    if (!checkEmailValid() || !checkPassValid()) {
        return;
    }

    enableBtn(false);
    QJsonObject json_obj;
    json_obj["email"] =  ui->email_edit->text();
    json_obj["passwd"] = ui->pwd_edit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_login"),
                                        json_obj, RequestId::ID_LOGIN_USER, Modules::LOGINMOD);
}

