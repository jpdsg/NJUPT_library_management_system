#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QString>
#include <QMap>

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    // 定义用户角色
    enum UserRole { ADMIN, READER };

    explicit LoginDialog(QWidget *parent = nullptr);

    QString getUsername() const;
    UserRole getRole() const;

private slots:
    void onLogin();
    void onChangePassword();  // 新增修改密码槽函数
private:
    QLineEdit *userEdit;
    QLineEdit *pwdEdit;
    QString m_username;
    UserRole m_role;
    static QMap<QString, QString> s_userPasswords;
    //文件读写辅助函数
    void loadAccountData();
    void saveAccountData();
};

#endif
