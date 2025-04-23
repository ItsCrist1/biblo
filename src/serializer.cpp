#include "serializer.h"
#include <fstream>

void setBooksFromFile(std::vector<Book>& books, const std::string& file) {
    std::wifstream wis (file);

    size_t sz;
    wis >> sz;
    books = std::vector<Book> (sz);

    for(size_t i=0; i < sz; ++i) {
        Book& book = books[i];
        wis >> book.title >> book.author >> book.description >> book.price >> book.quantity;
    }

    wis.close();
}

Serializer::Serializer(const std::string& SaveFile) : SaveFile(SaveFile) {}

void Serializer::SaveBooks(std::ofstream& os, const std::vector<std::pair<u32,u32>>& books) const {
    writeBF<u32>(os, books.size());
    
    for(const std::pair<u32,u32> book : books)
        writeBF<u32>(os, book.first),
        writeBF<u32>(os, book.second);
}

void Serializer::LoadBooks(std::ifstream& is, std::vector<std::pair<u32,u32>>& books) const {
    books = std::vector<std::pair<u32,u32>> (readBF<u32>(is));

    for(std::pair<u32,u32>& book : books)
        book.first = readBF<u32>(is),
        book.second = readBF<u32>(is);
}

void Serializer::SaveUser(std::ofstream& os, const User& user) const {
    writeWstr(os, user.name);
    writeStr(os, user.password);

    SaveBooks(os, user.shoppingCart);
    SaveBooks(os, user.boughtBooks);

    writeBF<double>(os, user.balance);
}

void Serializer::LoadUser(std::ifstream& is, User& user) const {
    user.name = readWstr(is);
    user.password = readStr(is);

    LoadBooks(is, user.shoppingCart);
    LoadBooks(is, user.boughtBooks);

    user.balance = readBF<double>(is);
}

void Serializer::SaveData(const std::vector<User>& users) const {
    std::ofstream os (SaveFile, std::ios::binary);
    writeBF<u32>(os, users.size());

    for(const User& user : users) SaveUser(os, user);

    os.close();
}

void Serializer::LoadData(std::vector<User>& users) const {
    std::ifstream is (SaveFile, std::ios::binary);
    const size_t sz = readBF<u32>(is);
    users = std::vector<User> (sz);
    
    for(User& user : users) LoadUser(is, user);

    is.close();
}