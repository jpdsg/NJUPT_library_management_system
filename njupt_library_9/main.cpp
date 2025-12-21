#include "MainWindow.h"
#include "LoginDialog.h"
#include <QApplication>

// 定义重启状态码
const int RETCODE_RESTART = 777;

int main(int argc, char *argv[]) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication a(argc, argv);

    int currentExitCode = 0;

    do {
        // 1. 显示登录对话框
        LoginDialog login;
        if (login.exec() != QDialog::Accepted) {
            // 用户点击了关闭或取消，直接退出程序
            break;
        }

        // 2. 登录成功，获取信息
        LoginDialog::UserRole role = login.getRole();
        QString user = login.getUsername();

        // 3. 进入主窗口
        MainWindow w(role, user);
        w.show();

        // 4. 进入主事件循环
        currentExitCode = a.exec();

    // 如果退出代码是 777，说明用户点了“切换账号”，循环继续
    } while (currentExitCode == RETCODE_RESTART);

    return currentExitCode;
}
