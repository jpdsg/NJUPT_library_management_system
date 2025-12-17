#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QString>

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

private:
    QLineEdit *userEdit;
    QLineEdit *pwdEdit;
    QString m_username;
    UserRole m_role;
};

#endif
