#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QDateEdit>
#include <QTextCodec>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QLabel>        // 标签（校徽、标题）
#include <QPushButton>   // 按钮（搜索、上架、借阅等）
#include <QTableWidget>  // 表格（展示图书信息）
#include <QVBoxLayout>   // 垂直布局
#include <QHBoxLayout>   // 水平布局
#include <QLineEdit>     // 输入框（搜索）
#include <QHeaderView>   // 表格表头
#include <QIcon>         // 窗口图标
#include <QPixmap>       // 图片（校徽）
#include <QWidget>       // 基础窗口部件
#include <QPainter>     // 新增：绘图工具
#include <QStyleOption> // 新增：样式兼容

MainWindow::MainWindow(LoginDialog::UserRole role, QString username, QWidget *parent)
    : QMainWindow(parent), currentRole(role), currentUser(username)
{
    manager = new LibraryManager("books.txt");
    setupUI();

    // 根据权限设置标题和隐藏按钮
    if (currentRole == LoginDialog::ADMIN) {
        setWindowTitle("图书馆管理系统 - 管理员: " + currentUser);
    } else {
        setWindowTitle("图书馆管理系统 - 读者: " + currentUser);
    }

    onRefresh();
}

MainWindow::~MainWindow() { delete manager; }

// --- 编码转换核心函数 ---
QString MainWindow::strToQt(const std::string& str) {
    return QString::fromLocal8Bit(str.c_str());
}

std::string MainWindow::qtToStr(const QString& str) {
    return std::string(str.toLocal8Bit().constData());
}
// -----------------------


void MainWindow::setupUI() {
    this->setWindowTitle("南邮图书馆管理系统 (MinGW版)");
    this->resize(1100, 600);

    // 设置窗口图标（可替换为南邮校徽图标）
    this->setWindowIcon(QIcon("E:\\QtCreatorwork\\fig_njupt.png"));

    QWidget* cw = new QWidget(this);
    setCentralWidget(cw);
    QVBoxLayout* mainL = new QVBoxLayout(cw);

    // 添加南邮头部横幅
    QHBoxLayout* headerLayout = new QHBoxLayout();
    QLabel* logoLabel = new QLabel();
    QPixmap logo("E:\\QtCreatorwork\\fig_njupt.png");  // 假设校徽图片放在项目根目录
    if (!logo.isNull()) {
        logoLabel->setPixmap(logo.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    QLabel* titleLabel = new QLabel("<h2><b>南京邮电大学图书馆管理系统</b></h2>");
    titleLabel->setStyleSheet("color: #003366;");  // 南邮蓝色

    headerLayout->addWidget(logoLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);
    headerLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    headerLayout->addStretch();
    mainL->addLayout(headerLayout);

    QHBoxLayout* topL = new QHBoxLayout();
    searchInput = new QLineEdit();
    searchInput->setPlaceholderText("输入书名搜索...");
    // 搜索框美化
    searchInput->setStyleSheet("padding: 5px; border: 1px solid #0066cc; border-radius: 4px;");

    QPushButton* btnSearch = new QPushButton("搜索");
    QPushButton* btnAll = new QPushButton("显示全部");

    topL->addWidget(searchInput);
    topL->addWidget(btnSearch);
    topL->addWidget(btnAll);

    QHBoxLayout* btnL = new QHBoxLayout();
    QPushButton* b1 = new QPushButton("上架");
    QPushButton* b2 = new QPushButton("下架");
    QPushButton* b3 = new QPushButton("修改图书信息");
    QPushButton* b4 = new QPushButton("借阅");
    QPushButton* b5 = new QPushButton("归还");
    QPushButton* b6 = new QPushButton("即将到期");
    QPushButton* b7 = new QPushButton("热门借阅");
    QPushButton* btnExport = new QPushButton("导出数据");

    // 按钮美化 - 使用南邮蓝色系
    QString btnStyle = "QPushButton { background-color: #0066cc; color: white; "
                      "padding: 6px 12px; border-radius: 4px; border: none; }"
                      "QPushButton:hover { background-color: #0052a3; }"
                      "QPushButton:pressed { background-color: #003366; }";

    b1->setStyleSheet(btnStyle);
    b2->setStyleSheet(btnStyle);
    b3->setStyleSheet(btnStyle);
    b4->setStyleSheet(btnStyle);
    b5->setStyleSheet(btnStyle);
    b6->setStyleSheet(btnStyle);
    b7->setStyleSheet(btnStyle);
    btnSearch->setStyleSheet(btnStyle);
    btnAll->setStyleSheet(btnStyle);
    btnExport->setStyleSheet(btnStyle);

    btnL->addWidget(b1); btnL->addWidget(b2); btnL->addWidget(b3);
    btnL->addSpacing(20);
    btnL->addWidget(b4); btnL->addWidget(b5);
    btnL->addSpacing(20);
    btnL->addWidget(b6); btnL->addWidget(b7);
    btnL->addWidget(btnExport);

    // 根据角色隐藏按钮
    if (currentRole == LoginDialog::ADMIN) {
        // 管理员：隐藏 借阅、归还
        // 管理员可以：添加、删除、修改、即将到期、热度、导出
        b4->hide();
        b5->hide();
    }
    else {
        // 读者：隐藏 上架、下架、修改、导出
        // 读者可以：借阅、归还、即将到期(看自己的)、热度
        b1->hide();
        b2->hide();
        b3->hide();
        btnExport->hide();
    }
    table = new QTableWidget();
    table->setColumnCount(10);
    table->setHorizontalHeaderLabels({"索引号", "名称", "馆藏地址", "类别", "数量", "价格", "入库", "归还", "借次", "状态"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 表格美化
    table->setStyleSheet("QTableWidget { border: 1px solid #dddddd; }"
                        "QHeaderView::section { background-color: #f0f5ff; color: #003366; padding: 5px; border: 1px solid #dddddd; }"
                        "QTableWidget::item:selected { background-color: #cce5ff; color: #000000; }");

    mainL->addLayout(topL);
    mainL->addLayout(btnL);
    mainL->addWidget(table);

    // 整体背景色
    cw->setStyleSheet("background-color: #f9fbff;");

    connect(btnSearch, &QPushButton::clicked, this, &MainWindow::onSearch);
    connect(btnAll, &QPushButton::clicked, this, &MainWindow::onRefresh);
    connect(b1, &QPushButton::clicked, this, &MainWindow::onAdd);
    connect(b2, &QPushButton::clicked, this, &MainWindow::onDel);
    connect(b3, &QPushButton::clicked, this, &MainWindow::onMod);
    connect(b4, &QPushButton::clicked, this, &MainWindow::onBorrow);
    connect(b5, &QPushButton::clicked, this, &MainWindow::onReturn);
    connect(b6, &QPushButton::clicked, this, &MainWindow::onDue);
    connect(b7, &QPushButton::clicked, this, &MainWindow::onSort);
    connect(btnExport, &QPushButton::clicked, this, &MainWindow::onExport);

}

void MainWindow::updateTable(const std::vector<Book>& books) {
    table->setRowCount(0);
    for (const auto& b : books) {
        int r = table->rowCount();
        table->insertRow(r);
        table->setItem(r, 0, new QTableWidgetItem(strToQt(b.getIndexNumber())));
        table->setItem(r, 1, new QTableWidgetItem(strToQt(b.getName())));
        table->setItem(r, 2, new QTableWidgetItem(strToQt(b.getLocation())));
        table->setItem(r, 3, new QTableWidgetItem(strToQt(b.getCategory())));
        table->setItem(r, 4, new QTableWidgetItem(QString::number(b.getQuantity())));
        table->setItem(r, 5, new QTableWidgetItem(QString::number(b.getPrice(), 'f', 2)));
        table->setItem(r, 6, new QTableWidgetItem(strToQt(b.getStorageDate())));
        table->setItem(r, 7, new QTableWidgetItem(strToQt(b.getReturnDate())));
        table->setItem(r, 8, new QTableWidgetItem(QString::number(b.getBorrowCount())));
        
        // 解决状态乱码问题
        QString statusText;
        bool isAvailable = (b.getQuantity() > 0); // 根据数量判断状态

        // 强制使用 Qt 内部编码生成中文，避免乱码
        if (isAvailable) {
            statusText = "可借";
        } else {
            statusText = "不可借";
        }

        auto* item = new QTableWidgetItem(statusText);

        // 设置颜色
        if (!isAvailable) {
            item->setForeground(Qt::red);
        } else {
            item->setForeground(Qt::green);
        }
        table->setItem(r, 9, item);
        //

    }
}

std::string MainWindow::getSelectedId() {
    int r = table->currentRow();
    if (r < 0) return "";
    return qtToStr(table->item(r, 0)->text());
}

void MainWindow::onRefresh() { updateTable(manager->getAllBooks()); }

void MainWindow::onSearch() {
    std::string txt = qtToStr(searchInput->text());
    if (txt.empty()) onRefresh();
    else updateTable(manager->searchByName(txt));
}

void MainWindow::onAdd() {
    QDialog d(this); d.setWindowTitle("添加图书");
    QFormLayout form(&d);
    QLineEdit *idx = new QLineEdit(&d);
    QLineEdit *name = new QLineEdit(&d);
    QLineEdit *loc = new QLineEdit(&d);
    QLineEdit *cat = new QLineEdit(&d);
    QLineEdit *qty = new QLineEdit(&d);
    QLineEdit *pr = new QLineEdit(&d);
    QDateEdit *date = new QDateEdit(QDate::currentDate(), &d);
    date->setDisplayFormat("yyyy-MM-dd");
    
    form.addRow("索引号:", idx); form.addRow("书名:", name);
    form.addRow("馆藏地址:", loc); form.addRow("类别:", cat);
    form.addRow("数量:", qty); form.addRow("价格:", pr);
    form.addRow("日期:", date);
    
    QDialogButtonBox box(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &d);
    form.addRow(&box);
    connect(&box, &QDialogButtonBox::accepted, &d, &QDialog::accept);
    connect(&box, &QDialogButtonBox::rejected, &d, &QDialog::reject);
    
    if (d.exec() == QDialog::Accepted) {
        if(idx->text().isEmpty()) { QMessageBox::warning(this, "错误", "索引号不能为空"); return; }
        Book b(qtToStr(idx->text()), qtToStr(name->text()), qtToStr(loc->text()),
               qtToStr(cat->text()), qty->text().toInt(), pr->text().toDouble(),
               qtToStr(date->text()), "", 0, qty->text().toInt() > 0 ? qtToStr("可借") :qtToStr ("不可借"));
        manager->addBook(b);
        onRefresh();
    }
}

void MainWindow::onDel() {
    std::string id = getSelectedId();
    if (id.empty()) return;
    if (QMessageBox::Yes == QMessageBox::question(this, "确认", "删除该书？")) {
        manager->deleteBook(id);
        onRefresh();
    }
}

void MainWindow::onMod() {
    std::string id = getSelectedId();
    if (id.empty()) return;
    Book* old = manager->findBookByIndex(id);
    if (!old) return;
    
    QDialog d(this); d.setWindowTitle("修改图书");
    QFormLayout form(&d);
    QLineEdit *name = new QLineEdit(strToQt(old->getName()), &d);
    QLineEdit *qty = new QLineEdit(QString::number(old->getQuantity()), &d);
    
    form.addRow("名称:", name);
    form.addRow("数量:", qty); // 演示仅修改部分，可自行补全
    
    QDialogButtonBox box(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &d);
    form.addRow(&box);
    connect(&box, &QDialogButtonBox::accepted, &d, &QDialog::accept);
    
    if (d.exec() == QDialog::Accepted) {
        Book n = *old;
        n.setName(qtToStr(name->text()));
        n.setQuantity(qty->text().toInt());
        manager->modifyBook(id, n);
        onRefresh();
    }
}

// 借书
void MainWindow::onBorrow() {
    std::string id = getSelectedId();
    if (id.empty()) return;
    // 传入当前用户名
    if (manager->borrowBook(id, qtToStr(currentUser))) {
        QMessageBox::information(this, "成功", "借阅成功！");
        onRefresh();
    } else {
        QMessageBox::warning(this, "失败", "库存不足或无法借阅");
    }
}
// 还书
void MainWindow::onReturn() {
    std::string id = getSelectedId();
    if (id.empty()) return;
    // 传入当前用户名
    if (manager->returnBook(id, qtToStr(currentUser))) {
        QMessageBox::information(this, "成功", "归还成功！");
        onRefresh();
    } else {
        QMessageBox::warning(this, "失败", "你没有借阅这本书或书号错误");
    }
}

// 即将到期 (区分管理员和读者)
void MainWindow::onDue() {
    std::vector<Book> allDue = manager->getDueBooks();
    std::vector<Book> displayBooks;
    if (currentRole == LoginDialog::ADMIN) {
        // 管理员看所有即将到期的书
        displayBooks = allDue;
    } else {
        // 读者只看 *自己* 借阅且即将到期的书
        std::string userStr = qtToStr(currentUser) + ";";
        for (const auto& book : allDue) {
            if (book.getBorrowers().find(userStr) != std::string::npos) {
                displayBooks.push_back(book);
            }
        }
    }
    updateTable(displayBooks);
    QMessageBox::information(this, "提示", QString("发现 %1 本即将到期的图书").arg(displayBooks.size()));
}

void MainWindow::onSort() { manager->sortByBorrowCount(); onRefresh(); }

void MainWindow::onExport() {
    // 1. 弹出保存文件对话框
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "导出图书信息",
                                                    "",
                                                    "CSV 表格文件 (*.csv)");

    if (fileName.isEmpty()) return;

    // 2. 打开文件准备写入
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法创建文件，请检查权限！");
        return;
    }

    QTextStream out(&file);

    // --- 设置输出为 GBK (适配 Excel) ---
    out.setCodec(QTextCodec::codecForName("GBK"));

    // --- 核心修改：表头使用 fromUtf8 包裹 ---
    out << QString::fromUtf8("索引号,名称,馆藏地址,类别,数量,价格,入库日期,归还日期,借阅次数,状态\n");

    // 4. 遍历所有图书并写入
    const auto& books = manager->getAllBooks();
    for (const auto& b : books) {
        // 数据部分已经是 QString 了，直接输出即可
        out << strToQt(b.getIndexNumber()) << ","
            << strToQt(b.getName()) << ","
            << strToQt(b.getLocation()) << ","
            << strToQt(b.getCategory()) << ","
            << b.getQuantity() << ","
            << b.getPrice() << ","
            << strToQt(b.getStorageDate()) << ","
            << strToQt(b.getReturnDate()) << ","
            << b.getBorrowCount() << ","
            << QString::fromUtf8(b.getQuantity() > 0 ? "可借" : "不可借") << "\n"; // 这里状态也建议加fromUtf8保险
    }

    file.close();
    QMessageBox::information(this, "成功", "文件导出成功！\n请用 Excel 打开查看。");
}

