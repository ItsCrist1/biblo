#pragma once
#include "utils.h"
#include <unordered_map>
#include <map>
#include <vector>
#include <utility>

enum class SortType { Name, Author, Id, Price, Quantity };

struct Settings {
    SortType sortType;
    bool sortingAscending, caseSensitive;

    Settings(const SortType, const bool, const bool);
    Settings();
};

struct Book {
    u32 id;
    std::wstring title, author, description;
    double price;
    u32 quantity;

    Book(const u32 id, const std::wstring& title=L"", const std::wstring& author=L"", const std::wstring& description=L"", 
         const double price = 0.0, const u32 quatity = 0);
    Book(const Book& other) = default;

    const std::wstring getBookOutput(const std::pair<u32,u32>) const;
};

class Books {
    std::unordered_map<u32, Book> books;
public:
    std::unordered_map<u32, Book>::const_iterator begin() const;
    std::unordered_map<u32, Book>::const_iterator end() const;
    
    const u32 GetSize() const;
    const std::vector<u32> GetIDs() const;
    const Book& GetBook(const u32) const;
    void ChangeBook(const u32, const i32);
    Book& AddBook(const Book&);
};

struct User {
    std::wstring name;
    std::string password;
    double balance;
    std::map<u32,u32> shoppingCart, boughtBooks;

    User(const std::wstring& name=L"", const std::string& password="", const double balance = 0.0,
         const std::map<u32,u32>& shoppingCart = {},
         const std::map<u32,u32>& boughtBooks = {});
    User(const User& other) = default;

    User& operator=(const User& other) = default;
};