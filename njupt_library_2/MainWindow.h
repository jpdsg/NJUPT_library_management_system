#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include "LibraryManager.h"
#include <QPaintEvent>   // 新增：绘图事件头文件
#include <QPixmap>       // 新增：图片处理
#include "LoginDialog.h" // 引入登录定义的Role

class QLabel;
class QPushButton;
class QTableWidget;
class QLineEdit;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    // 修改构造函数，接收角色和用户名
    MainWindow(LoginDialog::UserRole role, QString username, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onRefresh();
    void onSearch();
    void onAdd();
    void onDel();
    void onMod();
    void onBorrow();
    void onReturn();
    void onDue();
    void onSort();
    void onExport();

private:
    LibraryManager* manager;
    QTableWidget* table;
    QLineEdit* searchInput;
    QPixmap bgLogo;  // 背景Logo图片对象
    void setupUI();
    void updateTable(const std::vector<Book>& books);
    std::string getSelectedId();
    LoginDialog::UserRole currentRole; // 当前角色
    QString currentUser;               // 当前用户
    // 专门用于处理 MinGW/Windows 下的字符串编码转换
    QString strToQt(const std::string& str);
    std::string qtToStr(const QString& str);
};
#endif
