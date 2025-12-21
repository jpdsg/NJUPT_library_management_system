#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <vector>

class Book {
public:
    Book();
    Book(std::string index, std::string name, std::string loc, std::string cat,
             double price, std::string sDate, std::string rDate,
             int bCount, std::string status, std::string borrower = "", std::string bDate = "",
             int renewCount = 0, std::string reserverUser = ""); // 新增参数

    // Getters
    std::string getIndexNumber() const { return indexNumber; }
    std::string getName() const { return name; }
    std::string getLocation() const { return location; }
    std::string getCategory() const { return category; }
    double getPrice() const { return price; }
    std::string getStorageDate() const { return storageDate; }
    std::string getReturnDate() const { return returnDate; }
    int getBorrowCount() const { return borrowCount; }
    std::string getStatus() const { return status; }
    std::string getBorrowDate() const { return borrowDate; }
    std::string getBorrower() const { return currentBorrower; }

    // --- 新增 Getter ---
    int getRenewalCount() const { return renewalCount; }
    std::string getReserver() const { return reserver; }

    // Setters
    void setIndexNumber(const std::string& v) { indexNumber = v; }
    void setName(const std::string& v) { name = v; }
    void setLocation(const std::string& v) { location = v; }
    void setCategory(const std::string& v) { category = v; }
    void setPrice(double v) { price = v; }
    void setStorageDate(const std::string& v) { storageDate = v; }
    void setReturnDate(const std::string& v) { returnDate = v; }
    void setStatus(const std::string& v) { status = v; }

    // --- 新增 Setter ---
    void setRenewalCount(int v) { renewalCount = v; }
    void setReserver(const std::string& v) { reserver = v; }

    // 业务方法更新
    void borrowBook(const std::string& username);
    void returnBook();
    // 新增业务方法：转借给预订人
    void transferToReserver();

    // 序列化
    std::string toString() const;
    void fromString(const std::string& line);

private:
    std::string indexNumber, name, location, category, storageDate, returnDate, borrowDate, status;
    int borrowCount;
    double price;
    std::string currentBorrower;
    // --- 新增字段 ---
    int renewalCount;   // 续借次数
    std::string reserver; // 预借人
};

#endif // BOOK_H
