#pragma once
#include "serializer.h"
#include <memory>

class Library {
    std::unique_ptr<User> user;
    std::vector<User> users;
    Serializer serializer;
    
    std::unique_ptr<User> isValidName(const std::wstring&) const;
    bool showPasswordError(const bool, const std::wstring&) const;
    void signMenu(const bool);
    void mainMenu();
    
public:
    Library(const std::string&);
    void GreetMenu();
};