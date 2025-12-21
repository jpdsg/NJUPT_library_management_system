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
#include <QLabel>
#include <QPixmap>
#include <QDebug>
#include <QApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QDate>
#include <QInputDialog>
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
    this->setWindowTitle("南邮图书馆管理系统");
    this->resize(1100, 600);
    QPixmap windowLogo("E:\\QtCreatorwork\\njupt.png");
    if (!windowLogo.isNull()) {
        this->setWindowIcon(QIcon(windowLogo));
    } else {
        qDebug() << "警告：加载窗口图标失败！";
    }
    // --- 1. 背景设置 ---
    QWidget* cw = new QWidget(this);
    setCentralWidget(cw);
    QVBoxLayout* mainL = new QVBoxLayout(cw);
    mainL->setContentsMargins(20, 20, 20, 20);

    QPixmap background("E:\\QtCreatorwork\\lib.png");
    if (!background.isNull()) {
        QPalette palette;
        palette.setBrush(QPalette::Window, QBrush(background.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
        cw->setAutoFillBackground(true);
        cw->setPalette(palette);
    } else {
        cw->setStyleSheet("background-color: #f0f4f8;");
    }

    // --- 2. 统一样式定义 ---
    // 通用按钮样式
    QString standardBtnStyle = "QPushButton { "
                               "  background-color: rgba(0, 102, 204, 0.9); "
                               "  color: white; "
                               "  padding: 6px 14px; "
                               "  border-radius: 4px; "
                               "  border: none; "
                               "  font-weight: bold; "
                               "}"
                               "QPushButton:hover { background-color: rgba(0, 82, 163, 0.95); }"
                               "QPushButton:pressed { background-color: rgba(0, 51, 102, 1); }"
                               "QPushButton:disabled { background-color: #aaaaaa; }";

    // 搜索框样式
    QString searchInputStyle = "QLineEdit { "
                               "  padding: 6px; "
                               "  border: 1px solid #0066cc; "
                               "  border-radius: 4px; "
                               "  background-color: rgba(255, 255, 255, 0.9); "
                               "  font-family: 'MicroSoft YaHei';"
                               "}";

    // --- 3. 头部标题栏 ---
    QHBoxLayout* headerLayout = new QHBoxLayout();
    QLabel* logoLabel = new QLabel();
    QPixmap logo("E:\\QtCreatorwork\\njupt.png");
    if (!logo.isNull()) {
        logoLabel->setPixmap(logo.scaled(70, 70, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        logoLabel->setText("<b>NJUPT</b>");
    }

    QLabel* titleLabel = new QLabel("<h2>南京邮电大学图书馆管理系统</h2>");
    titleLabel->setStyleSheet("color: #003366; font-family: 'MicroSoft YaHei';");

    QPushButton* btnSwitch = new QPushButton(" 切换账号 ");
    // 给退出/切换按钮保留不同的红色背景以便提醒用户区别于业务功能，如需纯绝对统一，改用 standardBtnStyle
    btnSwitch->setStyleSheet("QPushButton { background-color: #ff6666; color: white; padding: 5px 12px; border-radius: 4px; border: none; font-weight: bold; }"
                             "QPushButton:hover { background-color: #ff4d4d; }");

    headerLayout->addWidget(logoLabel);
    headerLayout->addSpacing(15);
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(btnSwitch);
    mainL->addLayout(headerLayout);

    // --- 4. 搜索工具栏 ---
    QHBoxLayout* topScrollL = new QHBoxLayout();
    searchInput = new QLineEdit();

    searchInput->setPlaceholderText("请输入书名、关键字搜索...");
    searchInput->setStyleSheet(searchInputStyle);

    QPushButton* btnSearch = new QPushButton("🔍 搜索");
    QPushButton* btnAll = new QPushButton("📂 全库显示");

    topScrollL->addWidget(searchInput, 4);
    topScrollL->addWidget(btnSearch, 1);
    topScrollL->addWidget(btnAll, 1);
    mainL->addLayout(topScrollL);

    // --- 5. 业务操作按钮栏 ---
    QHBoxLayout* btnL = new QHBoxLayout();
    QPushButton* b1 = new QPushButton("📚 上架功能");
    QPushButton* b2 = new QPushButton("🗑️ 下架图书");
    QPushButton* b3 = new QPushButton("📝 信息变更");
    QPushButton* b4 = new QPushButton("📘 图书借阅");
    QPushButton* b5 = new QPushButton("📗 确认归还");
    QPushButton* b6 = new QPushButton("📅 快到期书");
    QPushButton* b7 = new QPushButton("🔥 活跃热传");
    QPushButton* btnExport = new QPushButton("📥 导出清单");
    QPushButton* btnRenew  = new QPushButton("🚀 一键续借");
    QPushButton* btnReserve = new QPushButton("⏲️ 预借办理");

    // 存放所有需要样式化的按钮清单
    QList<QPushButton*> allUtilityButtons = {
        b1, b2, b3, b4, b5, b6, b7, btnExport, btnRenew, btnReserve, btnSearch, btnAll
    };

    // 应用统一化样式
    for (auto btn : allUtilityButtons) {
        btn->setStyleSheet(standardBtnStyle);
        btn->setCursor(Qt::PointingHandCursor);
    }

    // 布局组合及权限逻辑
    btnL->addWidget(b1); btnL->addWidget(b2); btnL->addWidget(b3); // 管理类
    btnL->addSpacing(15);
    btnL->addWidget(b4); btnL->addWidget(b5); // 流程类
    btnL->addSpacing(15);
    btnL->addWidget(b6); btnL->addWidget(b7); btnL->addWidget(btnRenew); btnL->addWidget(btnReserve);
    btnL->addStretch();
    btnL->addWidget(btnExport);

    mainL->addLayout(btnL);

    // 根据权限动态隐藏
    if (currentRole == LoginDialog::READER) {
        for(auto b : {b1, b2, b3, btnExport}) b->setVisible(false);
    } else if (currentRole == LoginDialog::ADMIN) {
        for(auto b : {btnRenew, btnReserve}) b->setVisible(false);
    }

    // --- 6. 数据列表 ---
    table = new QTableWidget();
    table->setColumnCount(11);
    table->setHorizontalHeaderLabels({"索引号", "书名", "存放位置", "类别", "单价", "入手日期", "当前状态", "借阅人", "借出日期", "应还期限", "流行指数"});

    // 行为设置
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    table->setStyleSheet(R"(
        QTableWidget {
            background-color: rgba(255, 255, 255, 0.9); /* 半透明背景 */
            border: 1px solid #cccccc;
            gridline-color: #f0f0f0;
            alternate-background-color: transparent; /* 确保交替行也是透明 */
        }
        QTableWidget::item {
            background-color: transparent; /* 确保每个 item 的背景是透明的 */
            color: black;
        }
        QTableWidget::item:selected {
            background-color: rgba(0, 91, 183, 200); /* 选中项的背景颜色 */
            color: white;
        }
        QScrollBar:vertical {
            width: 10px;
            background: #f0f0f0;
        }
        QScrollBar::handle:vertical {
            background: #bbb;
            min-height: 20px;
            border-radius: 5px;
        }
        QScrollBar::handle:vertical:hover {
            background: #999;
        }
    )");
    mainL->addWidget(table);

    // --- 7. 事件联姻 (Connect) ---
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
    connect(btnSwitch, &QPushButton::clicked, this, &MainWindow::onSwitchAccount);
    connect(btnRenew,  &QPushButton::clicked, this, &MainWindow::onRenew);
    connect(btnReserve,&QPushButton::clicked, this, &MainWindow::onReserve);
}


void MainWindow::updateTable(const std::vector<Book>& books) {
    table->setRowCount(0);
    // 增加列数，假设原表有: 索引,书名,位置,类别,价格,借阅状态,借阅人,还书日期
    // 现在管理员需要看: ..., 借阅人, 还书日期, 预借人, 续借次数
    QStringList headers;
    headers << "索引号" << "书名" << "位置" << "类别" << "价格" << "状态";

    if (currentRole == LoginDialog::ADMIN) {
        headers << "当前借阅人" << "应还日期" << "预借人" << "续借数";
    } else {
        // 读者视图：不显示具体借阅人姓名（隐私），显示到期信息
        headers << "借阅/到期信息";
    }

    table->setColumnCount(headers.size());
    table->setHorizontalHeaderLabels(headers);

    std::string currentUserStr = qtToStr(currentUser);
    for (const auto& book : books) {
        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(strToQt(book.getIndexNumber())));
        table->setItem(row, 1, new QTableWidgetItem(strToQt(book.getName())));
        table->setItem(row, 2, new QTableWidgetItem(strToQt(book.getLocation())));
        table->setItem(row, 3, new QTableWidgetItem(strToQt(book.getCategory())));
        table->setItem(row, 4, new QTableWidgetItem(QString::number(book.getPrice(), 'f', 2)));
        table->setItem(row, 5, new QTableWidgetItem(strToQt(book.getStatus())));

        if (currentRole == LoginDialog::ADMIN) {
            // 管理员看全貌
            table->setItem(row, 6, new QTableWidgetItem(strToQt(book.getBorrower())));
            table->setItem(row, 7, new QTableWidgetItem(strToQt(book.getReturnDate())));
            table->setItem(row, 8, new QTableWidgetItem(strToQt(book.getReserver())));
            table->setItem(row, 9, new QTableWidgetItem(QString::number(book.getRenewalCount())));
        } else {
            // 读者视图逻辑
            QString info;
            if (book.getBorrower().empty()) {
                info = "可借阅";
            } else {
                if (book.getBorrower() == currentUserStr) {
                    info = "我已借阅 (还期:" + strToQt(book.getReturnDate()) + ")";
                } else {
                    // 需求4：归还日前10天内公开
                    if (manager->isWithin10DaysReturn(book.getReturnDate())) {
                        info = "即将到期 (" + strToQt(book.getReturnDate()) + ") 可预借";
                    } else {
                        info = "已被借出"; // 隐藏具体日期和人名
                    }
                }
            }
            // 显示预借状态
            if (!book.getReserver().empty()) {
                if (book.getReserver() == currentUserStr) info += " [我已预订]";
                else info += " [已被预订]";
            }

            table->setItem(row, 6, new QTableWidgetItem(info));
        }
    }
    table->resizeColumnsToContents();  // 自动调整每列宽度以适应内容

}

std::string MainWindow::getSelectedId() {
    int r = table->currentRow();
    if (r < 0) return "";
    return qtToStr(table->item(r, 0)->text());
}

void MainWindow::onRefresh() {
    // 1. 先按索引号排序
   manager->sortByIndex();

    // 2. 获取数据并刷新表格
    // 如果是读者，可以保留之前的逻辑（只看全部或者只看可借），这里假设显示全部
   updateTable(manager->getAllBooks());
}

void MainWindow::onSearch() {
    std::string txt = qtToStr(searchInput->text());
    if (txt.empty()) onRefresh();
    else updateTable(manager->searchByName(txt));
}

// 在 MainWindow.cpp 中替换原有的 onAdd
// 在 MainWindow.cpp 中替换 onAdd 函数
void MainWindow::onAdd() {
    QDialog dlg(this);
    dlg.setWindowTitle("图书上架 (支持批量)");
    QFormLayout* layout = new QFormLayout(&dlg);

    // 1. 类别选择
    QComboBox* catCombo = new QComboBox();
    catCombo->addItems({"A-马列", "B-哲学", "C-社科", "D-政治", "E-军事", "F-经济", "G-文教", "H-语言", "I-文学", "J-艺术", "K-历史", "N-自然科学", "T-工业技术"});

    // 2. 书名输入
    QLineEdit* nameEdit = new QLineEdit();

    // 3. 馆藏地址 (下拉框)
    QComboBox* locCombo = new QComboBox();
    locCombo->addItem("仙林");
    locCombo->addItem("三牌楼");

    // 4. 价格输入
    QDoubleSpinBox* priceSpin = new QDoubleSpinBox();
    priceSpin->setRange(0.0, 9999.99); // 设置价格范围
    priceSpin->setValue(0.0);

    // 5. 新增：入库数量
    QSpinBox* countSpin = new QSpinBox();
    countSpin->setRange(1, 100); // 一次最多100本，防止误操作
    countSpin->setValue(1);      // 默认1本

    layout->addRow("类别:", catCombo);
    layout->addRow("书名:", nameEdit);
    layout->addRow("馆藏地址:", locCombo);
    layout->addRow("价格:", priceSpin);
    layout->addRow("入库数量:", countSpin);

    // 提示信息：日期自动生成
    QLabel* dateHint = new QLabel("入库日期将自动设为今天: " + QDate::currentDate().toString("yyyy-MM-dd"));
    dateHint->setStyleSheet("color: gray; font-size: 10px;");
    layout->addRow("", dateHint);

    QDialogButtonBox* btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addRow(btns);

    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        QString name = nameEdit->text().trimmed();
        if (name.isEmpty()) {
            QMessageBox::warning(this, "错误", "书名不能为空");
            return;
        }

        // 准备公共数据
        std::string catStr = qtToStr(catCombo->currentText());     // 类别字符串
        std::string catLetter = catStr.substr(0, 1);               // 类别首字母
        std::string nameStr = qtToStr(name);                       // 书名
        std::string locStr = qtToStr(locCombo->currentText());     // 地址
        double price = priceSpin->value();                         // 价格
        int quantity = countSpin->value();                         // 数量

        // 自动生成入库日期
        std::string dateStr = qtToStr(QDate::currentDate().toString("yyyy-MM-dd"));

        // 记录生成的第一个和最后一个索引号，用于提示
        QString firstIndex, lastIndex;

        // 循环添加多本图书
        for (int i = 0; i < quantity; ++i) {
            // 关键：每次循环都重新获取下一个索引号，因为addBook后最大索引号变了
            std::string newIndex = manager->getNextIndex(catLetter);

            if (i == 0) firstIndex = strToQt(newIndex);
            if (i == quantity - 1) lastIndex = strToQt(newIndex);

            // 创建图书对象
            // 构造参数：索引, 书名, 地址, 类别, 价格, 入库日, 归还日(空), 借阅次(0), 状态(可借)
            Book b(newIndex, nameStr, locStr, catStr, price, dateStr, "", 0, qtToStr("可借"));

            manager->addBook(b);
        }

        onRefresh(); // 刷新表格显示

        // 提示信息
        QString msg;
        if (quantity == 1) {
             msg = QString("成功上架 1 本图书！\n索引号：%1").arg(firstIndex);
        } else {
             msg = QString("成功批量上架 %1 本图书！\n索引号范围：%2 - %3")
                     .arg(quantity).arg(firstIndex).arg(lastIndex);
        }
        QMessageBox::information(this, "上架成功", msg);
    }
}

// 在 MainWindow.cpp 中替换 onDel 函数
void MainWindow::onDel() {
    // 1. 获取所有选中项
    QList<QTableWidgetItem*> selectedItems = table->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选择要下架的图书（按住 Ctrl 或 Shift 可多选）");
        return;
    }

    // 2. 提取不重复的索引号 (因为选中一行有多个单元格，所以用 Set 去重)
    QSet<QString> idsToDelete;
    for (auto item : selectedItems) {
        int row = item->row();
        // 假设第0列是索引号，获取该行的索引
        QTableWidgetItem* indexItem = table->item(row, 0);
        if (indexItem) {
            idsToDelete.insert(indexItem->text());
        }
    }

    if (idsToDelete.isEmpty()) return;

    // 3. 弹出确认框
    QString confirmMsg = QString("确定要批量下架选中的 %1 本图书吗？\n注意：处于“已借出”状态的图书将自动跳过。").arg(idsToDelete.size());
    if (QMessageBox::question(this, "确认下架", confirmMsg) != QMessageBox::Yes) {
        return;
    }

    // 4. 执行批量删除
    int successCount = 0;
    int failCount = 0; // 记录因借出而跳过的数量

    for (const QString& qId : idsToDelete) {
        std::string id = qtToStr(qId);

        // 先检查书籍状态
        Book* book = manager->findBookByIndex(id);
        if (book) {
            // 如果借阅人不为空，说明已借出，不能删除
            if (!book->getBorrower().empty()) {
                failCount++;
                continue;
            }

            // 执行删除
            if (manager->deleteBook(id)) {
                successCount++;
            }
        }
    }

    // 5. 刷新界面并报告结果
    onRefresh();

    QString resultMsg;
    if (failCount == 0) {
        resultMsg = QString("操作完成！\n成功下架：%1 本").arg(successCount);
    } else {
        resultMsg = QString("操作完成！\n成功下架：%1 本\n失败：%2 本 (因图书尚未归还，无法下架)")
                        .arg(successCount).arg(failCount);
    }

    QMessageBox::information(this, "下架结果", resultMsg);
}


void MainWindow::onMod() {
    std::string id = getSelectedId();
    if (id.empty()) return;

    Book* book = manager->findBookByIndex(id);
    if (!book) return;

    if (!book->getBorrower().empty()) {
        QMessageBox::warning(this, "禁止", "该书已借出，无法修改信息！");
        return;
    }
    QDialog dlg(this);
    dlg.setWindowTitle("修改图书信息");
    QFormLayout* layout = new QFormLayout(&dlg);

    // 显示不可修改的信息
    layout->addRow("索引号:", new QLabel(strToQt(book->getIndexNumber())));
    layout->addRow("书名:", new QLabel(strToQt(book->getName())));
    layout->addRow("类别:", new QLabel(strToQt(book->getCategory())));

    // --- 修改点：仅提供地址修改 ---
    QComboBox* locCombo = new QComboBox();
    locCombo->addItem("仙林");
    locCombo->addItem("三牌楼");
    // 设置当前选中的地址
    QString currentLoc = strToQt(book->getLocation());
    locCombo->setCurrentText(currentLoc);

    layout->addRow("馆藏地址:", locCombo);
    // ---------------------------

    QDialogButtonBox* btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addRow(btns);

    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        // 创建新对象，只更新地址，其他保持原样
        Book newBook = *book;
        newBook.setLocation(qtToStr(locCombo->currentText()));

        manager->modifyBook(id, newBook);
        onRefresh();
        QMessageBox::information(this, "成功", "图书地址已更新");
    }
}

// 修改：借阅按钮逻辑 (增加错误提示)
void MainWindow::onBorrow() {
    std::string id = getSelectedId();
    if (id.empty()) return;
    std::string targetUser = qtToStr(currentUser); // 默认借给当前登录者
    //如果是管理员，弹窗询问借给谁
    if (currentRole == LoginDialog::ADMIN) {
        bool ok;
        QString text = QInputDialog::getText(this, "管理员代借",
                                             "请输入读者用户名:", QLineEdit::Normal,
                                             "", &ok);
        if (ok && !text.isEmpty()) {
            targetUser = qtToStr(text.trimmed());
        } else {
            return; // 取消操作
        }
    }
    if (manager->borrowBook(id, targetUser)) {
        QMessageBox::information(this, "成功", "借阅成功！");
        onRefresh();
    } else {
        // 判断失败原因
        if (manager->hasOverdueBooks(qtToStr(currentUser))) {
             QMessageBox::warning(this, "失败", "您有逾期图书未还，账号已冻结！\n请线下归还后再试。");
        } else if (manager->getUserBorrowCount(qtToStr(currentUser)) >= 20) {
             QMessageBox::warning(this, "失败", "借阅数量已达上限(20本)。");
        } else {
             QMessageBox::warning(this, "失败", "借阅失败。\n可能原因：\n1.图书不可借\n2.已被他人预订\n3.系统错误");
        }
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

// 新增：续借
void MainWindow::onRenew() {
    std::string id = getSelectedId();
    if (id.empty()) return;
    int res = manager->renewBook(id, qtToStr(currentUser));
    if (res == 0) {
        QMessageBox::information(this, "成功", "续借成功！还书日期已延长15天。");
        onRefresh();
    } else if (res == 1) {
        QMessageBox::warning(this, "失败", "距离到期日不足10天，无法续借。");
    } else if (res == 2) {
        QMessageBox::warning(this, "失败", "续借次数已达上限(2次)。");
    } else {
        QMessageBox::warning(this, "失败", "操作失败，可能您未借阅此书。");
    }
}

// 新增：预借/取消预借
void MainWindow::onReserve() {
    std::string id = getSelectedId();
    if (id.empty()) return;

    Book* b = manager->findBookByIndex(id);
    if (!b) return;
    std::string user = qtToStr(currentUser);
    // 如果已经是预借人，则是取消操作
    if (b->getReserver() == user) {
        if (manager->cancelReserve(id, user)) {
            QMessageBox::information(this, "成功", "已取消预借。");
            onRefresh();
        }
        return;
    }
    // 否则是预借操作
    if (manager->reserveBook(id, user)) {
        QMessageBox::information(this, "成功", "预借成功！\n待图书归还后将自动转为您借阅。");
        onRefresh();
    } else {
        QMessageBox::warning(this, "失败", "无法预借。\n条件：\n1.图书需在归还前10天内\n2.当前无人预借\n3.不能预借自己手里的书");
    }
}
// 即将到期 (区分管理员和读者)
// 在 MainWindow.cpp 中替换原有的 onDue
void MainWindow::onDue() {
    // 获取即将到期的书 (LibraryManager中已有逻辑: 3天内)
    std::vector<Book> dueBooks = manager->getDueBooks();

    // 如果需要读者只能看自己即将到期的书，可以在这里再次过滤
    std::vector<Book> myDueBooks;
    if (currentRole == LoginDialog::READER) {
        for(const auto& b : dueBooks) {
            if(strToQt(b.getBorrower()) == currentUser) {
                myDueBooks.push_back(b);
            }
        }
    } else {
        myDueBooks = dueBooks;
    }

    if (myDueBooks.empty()) {
        QMessageBox::information(this, "提示", "没有即将到期的图书");
        // 确认后返回全部列表(刷新一下即可)
        onRefresh();
        return;
    }

    // 创建对话框显示列表
    QDialog dlg(this);
    dlg.setWindowTitle("即将到期图书列表");
    dlg.resize(800, 400);
    QVBoxLayout* layout = new QVBoxLayout(&dlg);

    QTableWidget* dueTable = new QTableWidget();
    dueTable->setColumnCount(5);
    dueTable->setHorizontalHeaderLabels({"索引号", "书名", "借阅人", "归还日期", "状态"});
    dueTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    dueTable->setRowCount(myDueBooks.size());

    for (int i = 0; i < myDueBooks.size(); ++i) {
        const Book& b = myDueBooks[i];
        dueTable->setItem(i, 0, new QTableWidgetItem(strToQt(b.getIndexNumber())));
        dueTable->setItem(i, 1, new QTableWidgetItem(strToQt(b.getName())));
        dueTable->setItem(i, 2, new QTableWidgetItem(strToQt(b.getBorrower())));

        QTableWidgetItem* dateItem = new QTableWidgetItem(strToQt(b.getReturnDate()));
        dateItem->setForeground(Qt::red); // 标红显示日期
        dueTable->setItem(i, 3, dateItem);

        dueTable->setItem(i, 4, new QTableWidgetItem("即将到期"));
    }

    layout->addWidget(dueTable);

    // 添加“确认”按钮
    QPushButton* btnOk = new QPushButton("确认 (返回全部图书)");
    connect(btnOk, &QPushButton::clicked, &dlg, &QDialog::accept);
    layout->addWidget(btnOk);

    dlg.exec();

    // 需求：确认OK后返回全部图书列表
    // 清空搜索框并刷新
    searchInput->clear();
    onRefresh();
}

// 在 MainWindow.cpp 中替换原有的 onSort
void MainWindow::onSort() {
    // 1. 统计相同书名的借阅次数
    QMap<QString, int> nameCountMap;
    const auto& allBooks = manager->getAllBooks();

    for (const auto& b : allBooks) {
        QString name = strToQt(b.getName());
        nameCountMap[name] += b.getBorrowCount();
    }

    // 2. 将Map转换为List以便排序
    typedef QPair<QString, int> DataPair;
    QList<DataPair> list;
    for (auto it = nameCountMap.begin(); it != nameCountMap.end(); ++it) {
        list.append(DataPair(it.key(), it.value()));
    }

    // 3. 降序排列
    std::sort(list.begin(), list.end(), [](const DataPair& a, const DataPair& b) {
        return a.second > b.second;
    });

    // 4. 创建弹窗显示结果
    QDialog dlg(this);
    dlg.setWindowTitle("热门借阅排行榜 (双击查看详情)");
    dlg.resize(400, 500);
    QVBoxLayout* layout = new QVBoxLayout(&dlg);

    QTableWidget* rankTable = new QTableWidget();
    rankTable->setColumnCount(2);
    rankTable->setHorizontalHeaderLabels({"书名", "累计借阅次数"});
    rankTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    rankTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // 禁止编辑
    rankTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    rankTable->setRowCount(list.size());
    for(int i=0; i<list.size(); ++i) {
        rankTable->setItem(i, 0, new QTableWidgetItem(list[i].first));
        rankTable->setItem(i, 1, new QTableWidgetItem(QString::number(list[i].second)));
    }

    layout->addWidget(rankTable);

    QPushButton* btnClose = new QPushButton("返回");
    layout->addWidget(btnClose);
    connect(btnClose, &QPushButton::clicked, &dlg, &QDialog::reject);

    // --- 实现跳转功能 ---
    connect(rankTable, &QTableWidget::cellDoubleClicked, [&](int row, int col) {
        QString bookName = rankTable->item(row, 0)->text();
        dlg.accept(); // 关闭排行榜

        // 在主界面执行搜索
        searchInput->setText(bookName);
        onSearch();
    });

    dlg.exec();
}

// 在 MainWindow.cpp 中替换 onExport 函数
// 在 MainWindow.cpp 中替换 onExport 函数
// 在 MainWindow.cpp 中替换 onExport 函数
void MainWindow::onExport() {
    // 1. 获取文件名保存路径
    QString fileName = QFileDialog::getSaveFileName(this, "导出数据", "", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件进行写入");
        return;
    }

    QTextStream out(&file);

    // --- 关键步骤 1：设置文件流为 UTF-8 ---
    out.setCodec("UTF-8");
    // --- 关键步骤 2：写入 BOM (Excel 打开不乱码的关键) ---
    out << QChar(0xFEFF);

    // --- 核心修复：使用 u8 前缀强制字符串为 UTF-8 编码 ---
    // 这种写法 (u8"...") 可以防止编译器把源码里的中文误读为 GBK
    QString header = QString::fromUtf8(u8"索引号,名称,馆藏地址,类别,价格,入库日期,状态,当前借阅人,借阅日期,应还日期,借阅次数");
    out << header << "\n";
    // --------------------------------------------------

    // 3. 遍历并写入数据
    const auto& books = manager->getAllBooks();
    for (const auto& b : books) {
        // 数据部分的转换保持不变，通常这部分是正常的
        QString index = strToQt(b.getIndexNumber());
        QString name = strToQt(b.getName());
        QString loc = strToQt(b.getLocation());
        QString cat = strToQt(b.getCategory());
        QString price = QString::number(b.getPrice(), 'f', 2);
        QString date = strToQt(b.getStorageDate());
        QString status = strToQt(b.getStatus());

        // 获取借阅人（如果未借出则为空）
        QString borrower = strToQt(b.getBorrower());

        QString bDate = strToQt(b.getBorrowDate());
        QString rDate = strToQt(b.getReturnDate());
        QString count = QString::number(b.getBorrowCount());

        // 写入一行数据 (使用英文逗号分隔)
        out << index << "," << name << "," << loc << "," << cat << ","
            << price << "," << date << "," << status << ","
            << borrower << "," << bDate << "," << rDate << "," << count << "\n";
    }

    file.close();
    QMessageBox::information(this, "成功", "数据已成功导出！");
}
void MainWindow::onSwitchAccount() {
    // 关闭当前窗口，并返回重启代码 777
    qApp->exit(777);
}
