#pragma once
#include "serializer.h"

static const std::string BooksFile = "books.txt";

class Library { 
    Books books;
    std::shared_ptr<User> user;
    std::vector<std::shared_ptr<User>> users;
    Serializer serializer;
    
    std::shared_ptr<User> isValidName(const std::wstring&) const;
    const bool showPasswordError(const bool, const std::wstring&) const;
    void signMenu(const bool);
    const bool isWstringInt(const std::wstring&) const;
    const std::vector<uint8_t> getDigits(u32) const;
    const bool isIntContained(const u32, const u32) const;
    const std::vector<u32> searchQuery(std::wstring) const;
    void searchMenu();
    void cartMenu();
    void returnMenu();
    void execMainMenu(const u8);
    void mainMenu();
    
public:
    Library(const std::string&, const Books&);
    void GreetMenu();
};