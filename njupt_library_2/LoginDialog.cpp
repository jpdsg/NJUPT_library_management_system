#include "LoginDialog.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QRadioButton>
#include <QGroupBox>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("系统登录");
    resize(300, 200);

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("账号:"));
    userEdit = new QLineEdit(this);
    userEdit->setPlaceholderText("管理员:admin / 读者:任意");
    layout->addWidget(userEdit);

    layout->addWidget(new QLabel("密码:"));
    pwdEdit = new QLineEdit(this);
    pwdEdit->setEchoMode(QLineEdit::Password);
    pwdEdit->setPlaceholderText("默认123");
    layout->addWidget(pwdEdit);

    QPushButton *btnLogin = new QPushButton("登录", this);
    btnLogin->setStyleSheet("background-color: #0066cc; color: white; padding: 5px;");
    layout->addWidget(btnLogin);

    connect(btnLogin, &QPushButton::clicked, this, &LoginDialog::onLogin);
}

void LoginDialog::onLogin() {
    QString user = userEdit->text().trimmed();
    QString pwd = pwdEdit->text();

    if (user.isEmpty() || pwd.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入账号和密码");
        return;
    }

    // 这里简单模拟数据库验证
    // 实际项目中应读取 users.txt 文件
    if (pwd == "123") {
        m_username = user;
        if (user == "admin") {
            m_role = ADMIN;
        } else {
            m_role = READER;
        }
        accept(); // 关闭对话框并返回 QDialog::Accepted
    } else {
        QMessageBox::critical(this, "错误", "密码错误 (默认: 123)");
    }
}

QString LoginDialog::getUsername() const { return m_username; }
LoginDialog::UserRole LoginDialog::getRole() const { return m_role; }
