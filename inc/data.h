#pragma once
#include "utils.h"
#include <vector>
#include <utility>

struct Book {
    std::wstring title, author, description;
    double price;
    u32 quantity;

    Book(const std::wstring& title=L"", const std::wstring& author=L"", const std::wstring& description=L"", 
         const double price = 0.0, const u32 quatity = 0);
    Book(const Book& other) = default;

    const std::wstring getBookOutput(const u32) const;
};

struct User {
    std::wstring name;
    std::string password;
    std::vector<std::pair<u32,u32>> shoppingCart, boughtBooks;
    double balance;

    User(const std::wstring& name=L"", const std::string& password="", 
         const std::vector<std::pair<u32,u32>>& shoppingCart = {},
         const std::vector<std::pair<u32,u32>>& boughtBooks = {}, const double balance = 0.0);
    User(const User& other) = default;

    User& operator=(const User& other) = default;
};