#include "Book.h"
#include <sstream>
#include <ctime>
#include <iomanip>
#include <QString>

// 放在构造函数前面即可
std::string toLocal(const char* str) {
    // 假设源码是 UTF-8，转为 Windows 本地编码 (GBK)
    return QString::fromUtf8(str).toLocal8Bit().toStdString();
}


Book::Book() : quantity(0), price(0.0), borrowCount(0), status("不可借") {}

Book::Book(std::string index, std::string name, std::string loc, std::string cat,
           int qty, double price, std::string sDate, std::string rDate,
           int bCount, std::string status)
    : indexNumber(index), name(name), location(loc), category(cat),
      quantity(qty), price(price), storageDate(sDate), returnDate(rDate),
      borrowCount(bCount), status(status) {}

void Book::setQuantity(int v) {
    quantity = v;
    status = (quantity > 0) ? toLocal("可借") :toLocal("不可借");
}

std::string Book::toString() const {
    std::ostringstream oss;
    oss << indexNumber << "|" << name << "|" << location << "|" << category << "|"
        << quantity << "|" << price << "|" << storageDate << "|" << returnDate << "|"
        << borrowCount << "|" << status<<"|"<<borrowers;
    return oss.str();
}

void Book::fromString(const std::string& line) {
    std::istringstream iss(line);
    std::string temp;
    auto get = [&](std::string& out) { std::getline(iss, out, '|'); };
    
    get(indexNumber); get(name); get(location); get(category);
    get(temp); quantity = std::stoi(temp);
    get(temp); price = std::stod(temp);
    get(storageDate); get(returnDate);
    get(temp); borrowCount = std::stoi(temp);
    get(status);
    get(borrowers);
}

void Book::borrowBook(const std::string& username) {
    if (quantity > 0) {
        quantity--;
        borrowCount++;
        status = (quantity > 0) ? toLocal("可借") :toLocal("不可借");
        
        // 记录借阅人（追加）
        borrowers += username + ";";

        // 计算30天后的日期 （简单处理：所有副本共享一个归还日期，实际项目需拆分记录）
        time_t now = time(0);
        now += 30 * 24 * 60 * 60; // +30天
        tm *ltm = localtime(&now);
        
        char buffer[20];
        strftime(buffer, 20, "%Y-%m-%d", ltm);
        returnDate = std::string(buffer);
    }
}

void Book::returnBook(const std::string& username) {
    // 检查该用户是否借过
    std::string search = username + ";";
    size_t pos = borrowers.find(search);
    if (pos != std::string::npos) {
        quantity++;
        status = toLocal("可借");
        // 从借阅人列表中移除该用户
        borrowers.replace(pos, search.length(), "");
        if (borrowers.empty()) returnDate = "";
    }
}
