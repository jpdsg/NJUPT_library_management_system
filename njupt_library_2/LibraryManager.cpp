#include "LibraryManager.h"
#include <fstream>
#include <algorithm>
#include <ctime>
#include <cstdio>

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

bool LibraryManager::modifyBook(const std::string& idx, const Book& newBook) {
    for (auto& book : books) {
        if (book.getIndexNumber() == idx) {
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

bool LibraryManager::borrowBook(const std::string& idx, const std::string& username) {
    Book* b = findBookByIndex(idx);
    if (b && (b->getStatus() == "可借" || b->getQuantity() > 0)) {
        b->borrowBook(username);
        saveToFile();
        return true;
    }
    return false;
}

bool LibraryManager::returnBook(const std::string& idx, const std::string& username) {
    Book* b = findBookByIndex(idx);
    if (b) {
        // 只有借过的人才能还
        if (b->getBorrowers().find(username + ";") != std::string::npos) {
            b->returnBook(username);
            saveToFile();
            return true;
        }
    }
    return false;
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
