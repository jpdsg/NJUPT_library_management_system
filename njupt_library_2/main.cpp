#include "MainWindow.h"
#include "LoginDialog.h"
#include <QApplication>

int main(int argc, char *argv[]) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication a(argc, argv);

    // 1. 创建登录对话框
    LoginDialog login;

    // 2. 如果登录成功 (返回 Accepted)
    if (login.exec() == QDialog::Accepted) {
        // 3. 获取用户信息
        LoginDialog::UserRole role = login.getRole();
        QString user = login.getUsername();

        // 4. 启动主窗口，传入用户信息
        MainWindow w(role, user);
        w.show();

        return a.exec();
    }

    return 0;
}
