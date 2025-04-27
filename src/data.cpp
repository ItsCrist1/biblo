#include "data.h"
#include <sstream>

Settings::Settings(const SortType sortType, const bool sortingAscending, const bool caseSensitive)
: sortType(sortType), sortingAscending(sortingAscending), caseSensitive(caseSensitive) {}

Settings::Settings() : sortType(SortType::Name), sortingAscending(true), caseSensitive(true) {}

Book::Book(const u32 id, const std::wstring& title, const std::wstring& author, 
           const std::wstring& description, const double price, const u32 quantity) 
           : id(id),  title(title), author(author), description(description)
           , price(price), quantity(quantity) {}

const std::wstring Book::getBookOutput(const std::pair<u32,u32> entry) const {
    std::wstringstream wss;

    wss << title << L" by " << author << L" #" << entry.first 
        << L" | " << entry.second << L" available, $" << price 
        << L" each\n" << description << L"\n\n";

    return wss.str();
}

std::unordered_map<u32, Book>::const_iterator Books::begin() const { return books.begin(); }
std::unordered_map<u32, Book>::const_iterator Books::end() const { return books.end(); }
const u32 Books::GetSize() const { return books.size(); }

const std::vector<u32> Books::GetIDs() const {
    std::vector<u32> ids;
    ids.reserve(books.size());

    for (const auto& [id, _] : books) ids.push_back(id);
    return ids;
}

const Book& Books::GetBook(const u32 id) const {
    return books.at(id);
}

void Books::ChangeBook(const u32 id, const i32 quantity) {
    books.at(id).quantity += quantity;
}

Book& Books::AddBook(const Book& book) {
    books.insert_or_assign(book.id, book);
    return books.at(book.id);
}
    
User::User(const std::wstring& name, const std::string& password, const double balance,
           const std::map<u32,u32>& shoppingCart, 
           const std::map<u32,u32>& boughtBooks)
           : name(name), password(password), balance(balance)
           , shoppingCart(shoppingCart), boughtBooks(boughtBooks) {}