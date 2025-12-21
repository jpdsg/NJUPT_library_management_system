#include "LibraryManager.h"
#include <fstream>
#include <algorithm>
#include <ctime>
#include <cstdio>
#include <QDate>
#include <QDebug>

// 辅助：将 std::string 日期转为 QDate
QDate strToDate(const std::string& s) {
    return QDate::fromString(QString::fromStdString(s), "yyyy-MM-dd");
}

LibraryManager::LibraryManager(const std::string& filename) : dataFile(filename) {
    loadFromFile();
}

LibraryManager::~LibraryManager() {
    saveToFile();
}

void LibraryManager::addBook(const Book& book) {
    books.push_back(book);
    saveToFile();
}

bool LibraryManager::deleteBook(const std::string& idx) {
    for (auto it = books.begin(); it != books.end(); ++it) {
        if (it->getIndexNumber() == idx) {
            books.erase(it);
            saveToFile();
            return true;
        }
    }
    return false;
}

// 需求1：已借阅的图书不能修改信息
bool LibraryManager::modifyBook(const std::string& idx, const Book& newBook) {
    for (auto& book : books) {
        if (book.getIndexNumber() == idx) {
            // 检查状态，包含 "已借出" 字符则不可修改
            if (book.getStatus().find("已借出") != std::string::npos || !book.getBorrower().empty()) {
                return false;
            }
            book = newBook;
            saveToFile();
            return true;
        }
    }
    return false;
}

std::vector<Book> LibraryManager::searchByName(const std::string& name) const {
    std::vector<Book> res;
    for (const auto& b : books) {
        if (b.getName().find(name) != std::string::npos) res.push_back(b);
    }
    return res;
}

Book* LibraryManager::findBookByIndex(const std::string& idx) {
    for (auto& b : books) if (b.getIndexNumber() == idx) return &b;
    return nullptr;
}

const std::vector<Book>& LibraryManager::getAllBooks() const { return books; }

bool LibraryManager::isDueSoon(const std::string& dateStr) const {
    if (dateStr.empty()) return false;
    int y, m, d;
    if (sscanf(dateStr.c_str(), "%d-%d-%d", &y, &m, &d) != 3) return false;
    
    struct tm dueTm = {0};
    dueTm.tm_year = y - 1900; dueTm.tm_mon = m - 1; dueTm.tm_mday = d;
    time_t dueTime = mktime(&dueTm);
    time_t now = time(0);
    double seconds = difftime(dueTime, now);
    return (seconds >= 0 && seconds <= 3 * 24 * 3600);
}

std::vector<Book> LibraryManager::getDueBooks() const {
    std::vector<Book> res;
    for (const auto& b : books) if (isDueSoon(b.getReturnDate())) res.push_back(b);
    return res;
}

void LibraryManager::sortByBorrowCount() {
    std::sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
        return a.getBorrowCount() > b.getBorrowCount();
    });
}

// 需求2 & 5：借书限制
bool LibraryManager::borrowBook(const std::string& idx, const std::string& username) {
    // 1. 检查需求5：是否有逾期未还
    if (hasOverdueBooks(username)) {
        return false; // 有逾期，禁止借阅
    }
    // 2. 检查需求2：借阅数量不超过20
    if (getUserBorrowCount(username) >= 20) {
        return false; // 超额
    }
    Book* b = findBookByIndex(idx);
    // 检查是否有人预借 (如果预借人不是当前用户，则不能借)
    if (b && !b->getReserver().empty() && b->getReserver() != username) {
        return false;
    }
    if (b && b->getBorrower().empty()) {
        b->borrowBook(username);
        // 如果是预借人来借书（虽然通常通过还书触发，但防止直接借阅），清空预借
        if (b->getReserver() == username) {
            b->setReserver("");
        }
        saveToFile();
        return true;
    }
    return false;
}

bool LibraryManager::returnBook(const std:: string& idx, const std::string& username) {
    Book* b = findBookByIndex(idx);
    if (!b) {
        return false; // 书不存在
    }

    // 检查书是否已被借出
    if (b->getBorrower().empty()) {
        return false; // 未被借出，无法归还
    }

    // 只有借书人本人或管理员（由调用方保证合法性）才能归还
    if (b->getBorrower() != username && username != "admin") {
        return false; // 无权限归还
    }

    // 执行归还逻辑
    if (!b->getReserver().empty()) {
        // 有预借人：直接转借
        b->transferToReserver();
    } else {
        // 无预借人：正常归还
        b->returnBook();
    }

    saveToFile();
    return true;
}

// 需求3：续借
int LibraryManager::renewBook(const std::string& idx, const std::string& username) {
    Book* b = findBookByIndex(idx);
    if (!b || b->getBorrower() != username) return 3; // 书不对或人不对
    QDate rDate = strToDate(b->getReturnDate());
    QDate now = QDate::currentDate();
    // 检查是否在到期前10天以内 (daysTo 返回 rDate - now 的天数)
    // 需求：到期前10天之前可以续借 -> 剩余天数 > 10
    if (now.daysTo(rDate) <= 10) {
        return 1; // 距离到期太近，无法续借
    }
    // 检查次数
    if (b->getRenewalCount() >= 2) {
        return 2; // 次数超限
    }
    // 执行续借：延长15天，次数+1
    b->setReturnDate(rDate.addDays(15).toString("yyyy-MM-dd").toStdString());
    b->setRenewalCount(b->getRenewalCount() + 1);
    saveToFile();
    return 0; // 成功
}

// 需求4：预借
bool LibraryManager::reserveBook(const std::string& idx, const std::string& username) {
    Book* b = findBookByIndex(idx);
    if (!b) return false;
    // 只有已借出且没人预借的书才能预借
    if (!b->getBorrower().empty() && b->getReserver().empty()) {
        // 检查是否在归还日前10天内
        if (isWithin10DaysReturn(b->getReturnDate())) {
             // 限制：自己不能预借自己正在借的书
             if (b->getBorrower() == username) return false;
             b->setReserver(username);
             saveToFile();
             return true;
        }
    }
    return false;
}
bool LibraryManager::cancelReserve(const std::string& idx, const std::string& username) {
    Book* b = findBookByIndex(idx);
    if (b && b->getReserver() == username) {
        b->setReserver("");
        saveToFile();
        return true;
    }
    return false;
}
// 辅助功能实现
bool LibraryManager::hasOverdueBooks(const std::string& username) const {
    QDate now = QDate::currentDate();
    for (const auto& b : books) {
        if (b.getBorrower() == username) {
            QDate rDate = strToDate(b.getReturnDate());
            if (rDate < now) { // 应该还的日期 小于 今天 -> 逾期
                return true;
            }
        }
    }
    return false;
}
int LibraryManager::getUserBorrowCount(const std::string& username) const {
    int count = 0;
    for (const auto& b : books) {
        if (b.getBorrower() == username) count++;
    }
    return count;
}
bool LibraryManager::isWithin10DaysReturn(const std::string& returnDateStr) const {
    if (returnDateStr.empty()) return false;
    QDate rDate = strToDate(returnDateStr);
    QDate now = QDate::currentDate();
    qint64 days = now.daysTo(rDate);
    // 归还日前10天以内：剩余天数 <= 10 且 >= 0 (未逾期)
    return (days >= 0 && days <= 10);
}
bool LibraryManager::loadFromFile() {
    std::ifstream inFile(dataFile);
    if (!inFile.is_open()) return false;
    books.clear();
    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        Book b;
        try { b.fromString(line); books.push_back(b); } catch (...) {}
    }
    return true;
}

bool LibraryManager::saveToFile() const {
    std::ofstream outFile(dataFile);
    if (!outFile.is_open()) return false;
    for (const auto& b : books) outFile << b.toString() << std::endl;
    return true;
}
// 实现按索引号排序 (字典序)
void LibraryManager::sortByIndex() {
    std::sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
        return a.getIndexNumber() < b.getIndexNumber();
    });
}

// 实现自动生成索引号
std::string LibraryManager::getNextIndex(const std::string& categoryLetter) {
    int maxId = -1; // 初始为 -1，这样第一本就是 0000

    // 遍历所有书，找到同类别的最大编号
    for (const auto& book : books) {
        std::string idx = book.getIndexNumber();
        // 确保索引号不为空且首字母匹配
        if (idx.size() > 1 && idx.substr(0, 1) == categoryLetter) {
            try {
                // 截取后四位数字 (例如 A0003 -> 3)
                // 假设格式标准，直接转int
                int id = std::stoi(idx.substr(1));
                if (id > maxId) maxId = id;
            } catch (...) {
                // 忽略格式不标准的旧数据
            }
        }
    }

    // 生成新ID：字母 + (最大值+1) 补齐4位
    char buffer[20];
    sprintf(buffer, "%s%04d", categoryLetter.c_str(), maxId + 1);
    return std::string(buffer);
}
