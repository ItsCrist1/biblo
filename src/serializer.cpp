#include "serializer.h"
#include <fstream>

void ReadBooksFromFile(Books& books, const std::string& file) {
    std::wifstream wis (file);
    
    u32 idx;
    while (wis >> idx) {
        Book& book = books.AddBook(idx);
        wis >> book.title >> book.author >> book.description >> book.price >> book.quantity;
    }

    wis.close(); 
}

void WriteBooksToFile(const Books& books, const std::string& file) {
    std::wofstream wos (file);
    
    for (const auto& [id, book] : books)
        wos << id << L' ' << book.title << L' ' << book.author << L' ' 
            << book.description << L' ' << book.price << L' ' 
            << book.quantity << L'\n';

    wos.close();
}

Serializer::Serializer(const std::string& SaveFile, const std::string& SettingsFile) 
: SaveFile(SaveFile), SettingsFile(SettingsFile) {}

void Serializer::SaveBooks(std::ofstream& os, const std::map<u32,u32>& books) const {
    writeBF<u32>(os, books.size());
    
    for(const auto [id,quantity] : books)
        writeBF<u32>(os, id),
        writeBF<u32>(os, quantity);
}

void Serializer::LoadBooks(std::ifstream& is, std::map<u32,u32>& books) const {
    const u32 sz = readBF<u32>(is);
    books.clear();

    for (u32 i=0; i < sz; ++i) {
        const u32 id = readBF<u32>(is);
        const u32 quantity = readBF<u32>(is);

        books[id] = quantity;
    }
}

void Serializer::SaveUser(std::ofstream& os, const std::shared_ptr<User> user) const {
    writeWstr(os, user->name);
    writeStr(os, user->password);
    writeBF<double>(os, user->balance);

    SaveBooks(os, user->shoppingCart);
    SaveBooks(os, user->boughtBooks);
}

void Serializer::LoadUser(std::ifstream& is, std::shared_ptr<User> user) const {
    user->name = readWstr(is);
    user->password = readStr(is);
    user->balance = readBF<double>(is);

    LoadBooks(is, user->shoppingCart);
    LoadBooks(is, user->boughtBooks);
}

void Serializer::SaveData(const std::vector<std::shared_ptr<User>> users) const {
    std::ofstream os (SaveFile, std::ios::binary);
    writeBF<u32>(os, users.size());

    for(const std::shared_ptr<User> user : users) SaveUser(os, user);

    os.close();
}

void Serializer::LoadData(std::vector<std::shared_ptr<User>>& users) const {
    std::ifstream is (SaveFile, std::ios::binary);
    const size_t sz = readBF<u32>(is);
    users = std::vector<std::shared_ptr<User>> (sz, std::make_shared<User>());
    
    for(std::shared_ptr<User> user : users) LoadUser(is, user);

    is.close();
}

void Serializer::SaveSettings(const Settings& settings) const {
    std::ofstream os (SettingsFile, std::ios::binary);

    writeBF<u32>(os, static_cast<u32>(settings.sortType));
    writeBF<bool>(os, settings.sortingAscending);
    writeBF<bool>(os, settings.caseSensitive);

    os.close();
}

void Serializer::LoadSettings(Settings& settings) const {
    std::ifstream is (SettingsFile, std::ios::binary);

    settings.sortType = static_cast<SortType>(readBF<u32>(is));
    settings.sortingAscending = readBF<bool>(is);
    settings.caseSensitive = readBF<bool>(is);

    is.close();
}