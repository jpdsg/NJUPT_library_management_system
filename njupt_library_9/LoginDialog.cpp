#include "LoginDialog.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QFile>        // 新增
#include <QTextStream>  // 新增
#include <QDebug>       // 新增

// 1. 修改：静态成员变量初始化为空，我们在构造函数中加载
QMap<QString, QString> LoginDialog::s_userPasswords;

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent) {
    // 2. 新增：构造时尝试加载数据
    // 如果Map为空，说明是第一次运行或尚未加载，执行加载
    if (s_userPasswords.isEmpty()) {
        loadAccountData();
    }

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

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnLogin = new QPushButton("登录", this);
    btnLogin->setStyleSheet("background-color: #0066cc; color: white; padding: 5px;");
    QPushButton *btnChangePwd = new QPushButton("修改密码", this);
    btnChangePwd->setStyleSheet("padding: 5px;");

    btnLayout->addWidget(btnLogin);
    btnLayout->addWidget(btnChangePwd);
    layout->addLayout(btnLayout);

    connect(btnLogin, &QPushButton::clicked, this, &LoginDialog::onLogin);
    connect(btnChangePwd, &QPushButton::clicked, this, &LoginDialog::onChangePassword);
}

// --- 新增：加载账号数据 ---
void LoginDialog::loadAccountData() {
    QFile file("users.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        // 设置编码，防止中文乱码（虽然密码一般是英文，但以防万一）
        in.setCodec("UTF-8");
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split("|");
            if (parts.size() >= 2) {
                QString u = parts[0].trimmed();
                QString p = parts[1].trimmed();
                if (!u.isEmpty()) {
                    s_userPasswords[u] = p;
                }
            }
        }
        file.close();
    }

    // 确保管理员账号始终存在
    if (!s_userPasswords.contains("admin")) {
        s_userPasswords["admin"] = "123";
        saveAccountData(); // 如果文件不存在或没有admin，自动创建并保存
    }
}

// --- 新增：保存账号数据 ---
void LoginDialog::saveAccountData() {
    QFile file("users.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        // 遍历 Map 写入文件
        for (auto it = s_userPasswords.begin(); it != s_userPasswords.end(); ++it) {
            out << it.key() << "|" << it.value() << "\n";
        }
        file.close();
    }
}

void LoginDialog::onLogin() {
    QString user = userEdit->text().trimmed();
    QString pwd = pwdEdit->text();

    if (user.isEmpty() || pwd.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入账号和密码");
        return;
    }

    // 获取存储的密码，默认是123
    QString storedPwd = s_userPasswords.contains(user) ? s_userPasswords[user] : "123";

    if (pwd == storedPwd) {
        m_username = user;
        if (user == "admin") {
            m_role = ADMIN;
        } else {
            m_role = READER;
            // 3. 修改：如果是新用户（Map中不存在），记录并保存到文件
            if (!s_userPasswords.contains(user)) {
                s_userPasswords[user] = "123"; // 默认密码
                saveAccountData(); // 保存到文件
            }
        }
        accept();
    } else {
        QMessageBox::critical(this, "错误", "密码错误");
    }
}

void LoginDialog::onChangePassword() {
    QString user = userEdit->text().trimmed();
    if (user.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先输入账号");
        return;
    }

    // 必须先确认用户是否存在，如果是新用户未登录过，Map里可能没有
    // 这里保持你原有的逻辑，如果Map没有，默认视为密码123

    QDialog changePwdDialog(this);
    changePwdDialog.setWindowTitle("修改密码");
    changePwdDialog.setModal(true);
    // ... (UI布局代码保持不变) ...
    QFormLayout *formLayout = new QFormLayout(&changePwdDialog);
    QLineEdit *oldPwdEdit = new QLineEdit(&changePwdDialog); oldPwdEdit->setEchoMode(QLineEdit::Password);
    QLineEdit *newPwdEdit = new QLineEdit(&changePwdDialog); newPwdEdit->setEchoMode(QLineEdit::Password);
    QLineEdit *confirmPwdEdit = new QLineEdit(&changePwdDialog); confirmPwdEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("原密码:", oldPwdEdit);
    formLayout->addRow("新密码:", newPwdEdit);
    formLayout->addRow("确认新密码:", confirmPwdEdit);
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnOk = new QPushButton("确定");
    QPushButton *btnCancel = new QPushButton("取消");
    btnLayout->addStretch(); btnLayout->addWidget(btnOk); btnLayout->addWidget(btnCancel);
    formLayout->addRow(btnLayout);
    connect(btnCancel, &QPushButton::clicked, &changePwdDialog, &QDialog::reject);

    connect(btnOk, &QPushButton::clicked, [&]() {
        QString oldPwd = oldPwdEdit->text();
        QString newPwd = newPwdEdit->text();
        QString confirmPwd = confirmPwdEdit->text();

        QString storedPwd = s_userPasswords.contains(user) ? s_userPasswords[user] : "123";

        if (oldPwd != storedPwd) {
            QMessageBox::warning(&changePwdDialog, "错误", "原密码不正确");
            return;
        }
        if (newPwd.isEmpty()) {
            QMessageBox::warning(&changePwdDialog, "错误", "新密码不能为空");
            return;
        }
        if (newPwd != confirmPwd) {
            QMessageBox::warning(&changePwdDialog, "错误", "两次输入的新密码不一致");
            return;
        }

        // 4. 修改：更新内存Map 并 保存到文件
        s_userPasswords[user] = newPwd;
        saveAccountData(); // 保存更改

        QMessageBox::information(&changePwdDialog, "成功", "密码修改成功！");
        changePwdDialog.accept();
    });

    changePwdDialog.exec();
}

QString LoginDialog::getUsername() const { return m_username; }
LoginDialog::UserRole LoginDialog::getRole() const { return m_role; }
