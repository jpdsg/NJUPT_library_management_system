#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <vector>

class Book {
public:
    Book();
    Book(std::string index, std::string name, std::string loc, std::string cat,
         int qty, double price, std::string sDate, std::string rDate, 
         int bCount, std::string status);

    // Getters
    std::string getIndexNumber() const { return indexNumber; }
    std::string getName() const { return name; }
    std::string getLocation() const { return location; }
    std::string getCategory() const { return category; }
    int getQuantity() const { return quantity; }
    double getPrice() const { return price; }
    std::string getStorageDate() const { return storageDate; }
    std::string getReturnDate() const { return returnDate; }
    int getBorrowCount() const { return borrowCount; }
    std::string getStatus() const { return status; }
    std::string getBorrowers() const { return borrowers; }

    // Setters
    void setIndexNumber(const std::string& v) { indexNumber = v; }
    void setName(const std::string& v) { name = v; }
    void setLocation(const std::string& v) { location = v; }
    void setCategory(const std::string& v) { category = v; }
    void setQuantity(int v);
    void setPrice(double v) { price = v; }
    void setStorageDate(const std::string& v) { storageDate = v; }
    void setReturnDate(const std::string& v) { returnDate = v; }
    void setStatus(const std::string& v) { status = v; }
    void setBorrowers(const std::string& v) { borrowers = v; }

    // 业务方法
    void borrowBook(const std::string& username);
    void returnBook(const std::string& username);

    // 序列化
    std::string toString() const;
    void fromString(const std::string& line);

private:
    std::string indexNumber, name, location, category, storageDate, returnDate, status;
    int quantity, borrowCount;
    double price;
    std::string borrowers; // 记录借阅人，例如 "admin;tom;"

};

#endif // BOOK_H
