#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H
#include "Book.h"
#include <vector>

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

    // 修改 borrowBook 和 returnBook 接收用户名
    bool borrowBook(const std::string& indexNumber, const std::string& username);
    bool returnBook(const std::string& indexNumber, const std::string& username);

    void sortByBorrowCount();

    bool loadFromFile();
    bool saveToFile() const;

private:
    std::vector<Book> books;
    std::string dataFile;
    bool isDueSoon(const std::string& dateStr) const;
};
#endif
