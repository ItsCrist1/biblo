#include "data.h"
#include <sstream>

Book::Book(const std::wstring& title, const std::wstring& author, 
           const std::wstring& description, const double price, const u32 quantity) : title(title), author(author), 
           description(description), price(price), quantity(quantity) {}

const std::wstring Book::getBookOutput(const u32 index) const {
    std::wstringstream wss;

    wss << title << L" by " << author << L" #" << index 
        << L" | " << quantity << L" available, $" << price 
        << L" each\n" << description << L"\n\n";

    return wss.str();
}

User::User(const std::wstring& name, const std::string& password, 
           const std::vector<std::pair<u32,u32>>& shoppingCart, 
           const std::vector<std::pair<u32,u32>>& boughtBooks, const double balance)
           : name(name), password(password), shoppingCart(shoppingCart) 
           , boughtBooks(boughtBooks), balance(balance) {}