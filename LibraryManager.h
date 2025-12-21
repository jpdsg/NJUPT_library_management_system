#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H
#include "Book.h"
#include <vector>
#include <QString>
#include <QDate>

class LibraryManager {
public:
    LibraryManager(const std::string& filename);
    ~LibraryManager();

    void addBook(const Book& book);
    bool deleteBook(const std::string& indexNumber);
    bool modifyBook(const std::string& indexNumber, const Book& newBook);
    
    std::vector<Book> searchByName(const std::string& name) const;
    Book* findBookByIndex(const std::string& indexNumber);
    const std::vector<Book>& getAllBooks() const;
    std::vector<Book> getDueBooks() const;

    bool borrowBook(const std::string& indexNumber, const std::string& username);
    bool returnBook(const std::string& indexNumber, const std::string& username);

    // 续借 (需求3)
    // 返回值: 0=成功, 1=距离到期小于10天, 2=次数超限, 3=其他错误
    int renewBook(const std::string& indexNumber, const std::string& username);
    // 预借 (需求4)
    bool reserveBook(const std::string& indexNumber, const std::string& username);
    bool cancelReserve(const std::string& indexNumber, const std::string& username);
    // 辅助：检查是否有逾期书 (需求5)
    bool hasOverdueBooks(const std::string& username) const;
    // 辅助：获取当前借阅数量 (需求2)
    int getUserBorrowCount(const std::string& username) const;
    // 辅助：检查是否在归还日前10天内
    bool isWithin10DaysReturn(const std::string& returnDateStr) const;

    void sortByBorrowCount();

    bool loadFromFile();
    bool saveToFile() const;
    // 新增：按索引号排序
    void sortByIndex();
       // 新增：根据类别首字母生成下一个索引号 (例如 'A' -> "A0005")
    std::string getNextIndex(const std::string& categoryLetter);
private:
    std::vector<Book> books;
    std::string dataFile;
    bool isDueSoon(const std::string& dateStr) const;
};
#endif
