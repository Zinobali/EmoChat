#include "registerdialog.h"
#include "global.h"
#include "ui_registerdialog.h"
#include <QRegularExpression>
#include "httpmgr.h"

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::RegisterDialog),
    _countdown_timer(new QTimer(this))
{
    initUiDesign();
    initHttpHandlers();
    initUiSignals();
    testEditInfo(); // 测试函数，后续删除
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::showTip(QString str, bool ok)
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

void RegisterDialog::initUiDesign()
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    ui->tip_label->setProperty("state", "normal");
    ui->tip_label->clear();
    repolish(ui->tip_label);
    ui->pass_visible->SetState("unvisible", "unvisible_hover", "",
                               "visible", "visible_hover", "");
    ui->confirm_visible->SetState("unvisible", "unvisible_hover", "",
                                  "visible", "visible_hover", "");
}

void RegisterDialog::initHttpHandlers()
{
    // 注册验证码回包逻辑
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
    // 注册注册用户回包逻辑
    _handlers.insert(RequestId::ID_REG_USER, [this](const QJsonObject &jsonObj) {
        auto error = static_cast<ErrorCodes>(jsonObj["error"].toInt());
        if(error!= ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }
        auto email = jsonObj["email"].toString();
        auto user = jsonObj["user"].toString();
        showTip(tr("用户注册成功"), true);
        qDebug() << "email is" << email ;
        qDebug() << "user is" << user;
        ChangeTipPage();
    });

}

void RegisterDialog::initUiSignals()
{
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish, this, &RegisterDialog::slot_reg_mod_finish);
    connect(ui->user_edit, &QLineEdit::editingFinished, this, [this]() {
        checkUserValid();
    });
    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this]() {
        checkEmailValid();
    });
    connect(ui->pass_edit, &QLineEdit::editingFinished, this, [this]() {
        checkPassValid();
    });
    connect(ui->confirm_edit, &QLineEdit::editingFinished, this, [this]() {
        checkConfirmValid();
    });
    connect(ui->verify_edit, &QLineEdit::editingFinished, this, [this]() {
        checkVerifyValid();
    });
    connect(ui->pass_visible, &ClickedLabel::clicked, this, [this](){
        if(ui->pass_visible->GetCurState() == ClickLbState::Normal) {
            ui->pass_edit->setEchoMode(QLineEdit::Password);
            return;
        }
        ui->pass_edit->setEchoMode(QLineEdit::Normal);
    });
    connect(ui->confirm_visible, &ClickedLabel::clicked, this, [this](){
        if(ui->confirm_visible->GetCurState() == ClickLbState::Normal) {
            ui->confirm_edit->setEchoMode(QLineEdit::Password);
            return;
        }
        ui->confirm_edit->setEchoMode(QLineEdit::Normal);
    });
    connect(_countdown_timer, &QTimer::timeout, this, [this](){
        if (_countdown <= 0) {
            _countdown_timer->stop();
            emit sigSwitchLogin(this);
            return;
        }
        _countdown--;
        auto tip_str = QString("注册成功，%1 s后返回登录").arg(_countdown);
        ui->reg_ok_lb->setText(tip_str);
    });
}

bool RegisterDialog::checkUserValid()
{
    if (ui->user_edit->text().isEmpty())
    {
        AddTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}

bool RegisterDialog::checkPassValid()
{
    auto pass = ui->pass_edit->text();

    if (pass.length() < 6 || pass.length() > 15)
    {
        // 提示长度不准确
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");
    bool match = regExp.match(pass).hasMatch();
    if (!match)
    {
        // 提示字符非法
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;
    }
    DelTipErr(TipErr::TIP_PWD_ERR);
    return true;
}

bool RegisterDialog::checkConfirmValid()
{

    if (ui->confirm_edit->text().isEmpty())
    {
        AddTipErr(TipErr::TIP_CONFIRM_ERR, tr("确认密码不能为空"));
        return false;
    }
    if (ui->confirm_edit->text() != ui->pass_edit->text())
    {
        AddTipErr(TipErr::TIP_CONFIRM_ERR, tr("密码和确认密码不匹配"));
        return false;
    }
    DelTipErr(TipErr::TIP_CONFIRM_ERR);
    return true;
}

bool RegisterDialog::checkEmailValid()
{
    auto email = ui->email_edit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if (!match)
    {
        // 提示邮箱不正确
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }
    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool RegisterDialog::checkVerifyValid()
{
    if (ui->verify_edit->text().isEmpty())
    {
        AddTipErr(TipErr::TIP_VERIFY_ERR, tr("验证码不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_VERIFY_ERR);
    return true;
}

bool RegisterDialog::validateFields()
{
    const auto validators = {
                             &RegisterDialog::checkUserValid,
                             &RegisterDialog::checkEmailValid,
                             &RegisterDialog::checkPassValid,
                             &RegisterDialog::checkConfirmValid,
                             &RegisterDialog::checkVerifyValid};
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

void RegisterDialog::testEditInfo() // 测试函数，后续删除
{
    ui->user_edit->setText("zinobali");
    ui->email_edit->setText("1665799927@qq.com");
    ui->pass_edit->setText("123456");
    ui->confirm_edit->setText("123456");
    ui->verify_edit->setText("25de37");
    connect(ui->get_code, &TimerBtn::clicked, this, [](){
        qDebug() << "TimerBtn::clicked";
    });
}

void RegisterDialog::AddTipErr(const TipErr &tc, const QString &tips)
{
    _tip_errs[tc] = tips;
    showTip(tips, false);
}

void RegisterDialog::DelTipErr(const TipErr &tc)
{
    _tip_errs.remove(tc);
    if (_tip_errs.empty())
    {
        ui->tip_label->clear();
        return;
    }
    showTip(_tip_errs.first(), false);
}

void RegisterDialog::ChangeTipPage()
{
    _countdown_timer->stop();
    _countdown = 5;
    ui->stackedWidget->setCurrentWidget(ui->tip_page);
    // 启动定时器，设置间隔为1000毫秒（1秒）
    _countdown_timer->start(1000);
}

void RegisterDialog::on_get_code_clicked()
{
    if (!checkEmailValid()) {
        showTip(tr("邮箱地址不正确"), false);
        return;
    }

    QJsonObject json_obj;
    json_obj["email"] = ui->email_edit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/get_verifycode"),
                                        json_obj, RequestId::ID_GET_VERIFY_CODE, Modules::REGISTERMOD);
}

void RegisterDialog::slot_reg_mod_finish(RequestId id, QString res, ErrorCodes ec)
{
    if (ec != ErrorCodes::SUCCESS)
    {
        showTip(tr("网络请求错误"), false);
        return;
    }

    // 解析json
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    // json解析错误
    if (jsonDoc.isNull())
    {
        showTip(tr("json解析错误"), false);
        return;
    }

    // json解析错误
    if (!jsonDoc.isObject())
    {
        showTip(tr("json解析错误"), false);
        return;
    }

    if (_handlers.find(id) == _handlers.end())
    {
        showTip(tr("获取验证码失败"), false);
        qDebug() << "handler" << static_cast<int>(id) << "is not exist";
        return;
    }

    _handlers[id](jsonDoc.object());
    return;
}

void RegisterDialog::on_sure_btn_clicked()
{
    if (!validateFields())
    {
        return;
    }
    // 发送注册post请求
    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = ui->pass_edit->text();
    json_obj["confirm"] = ui->confirm_edit->text();
    json_obj["verifycode"] = ui->verify_edit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/user_register"),
                                        json_obj, RequestId::ID_REG_USER, Modules::REGISTERMOD);
}

void RegisterDialog::on_return_btn_clicked()
{
    _countdown_timer->stop();
    emit sigSwitchLogin(this);
}


void RegisterDialog::on_cancle_btn_clicked()
{
    _countdown_timer->stop();
    emit sigSwitchLogin(this);
}

