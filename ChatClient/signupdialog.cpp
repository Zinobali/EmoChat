#include "signupdialog.h"
#include "ui_signupdialog.h"

signUpDialog::signUpDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::signUpDialog)
{
    ui->setupUi(this);
    //界面
    setWindowTitle("User Registration");
    // 创建表单布局
    QFormLayout *formLayout = new QFormLayout(this);
    // 添加用户名和密码输入框
    usernameLineEdit = new QLineEdit(this);
    passwordLineEdit = new QLineEdit(this);
    confirmPasswordLineEdit = new QLineEdit(this);
    passwordLineEdit->setEchoMode(QLineEdit::Password); // 隐藏密码
    confirmPasswordLineEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("Username:", usernameLineEdit);
    formLayout->addRow("Password:", passwordLineEdit);
    formLayout->addRow("Confirm Password:", confirmPasswordLineEdit);
    // 添加注册按钮
    QPushButton *registerButton = new QPushButton("Register", this);
    connect(registerButton, &QPushButton::clicked, this, &signUpDialog::handleRegistration);
    formLayout->addWidget(registerButton);
    // 设置对话框大小
    resize(300, 150);
}

signUpDialog::~signUpDialog()
{
    delete ui;
}

void signUpDialog::handleRegistration()
{
    // 获取用户名、密码和确认密码
    QString username = usernameLineEdit->text();
    QString password = passwordLineEdit->text();
    QString confirmPassword = confirmPasswordLineEdit->text();

    // 验证两次密码是否相同
    if (password != confirmPassword)
    {
        QMessageBox::critical(this, "Error", "两次输入密码不匹配，请重试。");
        return;
    }

    // 在这里执行注册逻辑，例如将用户名和密码保存到数据库

    // 关闭对话框
    accept();
}
