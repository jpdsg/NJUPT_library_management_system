#include "Book.h"
#include <sstream>
#include <ctime>
#include <iomanip>
#include <QString>
#include <QDate> // 使用Qt日期类简化计算

std::string toLocal(const char* str) {
    return QString::fromUtf8(str).toLocal8Bit().toStdString();
}

// 构造函数初始化新增字段
Book::Book() : price(0.0), borrowCount(0), status("不可借"), renewalCount(0) {}

Book::Book(std::string index, std::string name, std::string loc, std::string cat,
           double price, std::string sDate, std::string rDate,
           int bCount, std::string status, std::string borrower, std::string bDate,
           int renewCount, std::string reserverUser)
    : indexNumber(index), name(name), location(loc), category(cat),
      price(price), storageDate(sDate), returnDate(rDate),
      borrowCount(bCount), status(status), currentBorrower(borrower), borrowDate(bDate),
      renewalCount(renewCount), reserver(reserverUser) {}

std::string Book::toString() const {
    std::ostringstream oss;
    // 格式追加：...|续借次数|预借人
    oss << indexNumber << "|" << name << "|" << location << "|" << category << "|"
        << "1" << "|" << price << "|" << storageDate << "|" << returnDate << "|"
        << borrowCount << "|" << status << "|" << currentBorrower << "|" << borrowDate << "|"
        << renewalCount << "|" << reserver;
    return oss.str();
}

void Book::fromString(const std::string& line) {
    std::istringstream iss(line);
    std::string temp;
    auto get = [&](std::string& out) { std::getline(iss, out, '|'); };

    get(indexNumber); get(name); get(location); get(category);
    get(temp); // quantity
    get(temp); price = std::stod(temp);
    get(storageDate); get(returnDate);
    get(temp); borrowCount = std::stoi(temp);
    get(status);
    get(currentBorrower);
    get(borrowDate);

    // 读取新增字段 (使用try-catch防止旧数据文件崩溃)
    try {
        if (std::getline(iss, temp, '|')) renewalCount = std::stoi(temp);
        else renewalCount = 0;

        if (std::getline(iss, temp, '|')) reserver = temp;
        else reserver = "";
    } catch (...) {
        renewalCount = 0;
        reserver = "";
    }
}

void Book::borrowBook(const std::string& username) {
    if (currentBorrower.empty()) {
        borrowCount++;
        status = toLocal("已借出");
        currentBorrower = username;

        // 借出时重置续借次数
        renewalCount = 0;
        reserver = ""; // 理论上借出时预借应该为空或就是这个人

        QDate now = QDate::currentDate();
        borrowDate = now.toString("yyyy-MM-dd").toStdString();
        returnDate = now.addDays(30).toString("yyyy-MM-dd").toStdString();
    }
}

void Book::returnBook() {
    // 归还：重置状态
    status = toLocal("可借");
    currentBorrower = "";
    returnDate = "";
    borrowDate = "";
    renewalCount = 0;
    // 注意：reserver在这里不清除，因为如果有人预借，Manager层会处理
}

// 专门处理预借转借出的逻辑
void Book::transferToReserver() {
    if (!reserver.empty()) {
        // 直接从还书状态转为借给预借人
        borrowCount++;
        status = toLocal("已借出");
        currentBorrower = reserver;
        reserver = ""; // 预借达成，清空预借人
        renewalCount = 0;

        QDate now = QDate::currentDate();
        borrowDate = now.toString("yyyy-MM-dd").toStdString();
        returnDate = now.addDays(30).toString("yyyy-MM-dd").toStdString();
    }
}
