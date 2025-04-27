#pragma once
#include "data.h"
#include <vector>
#include <memory>

void ReadBooksFromFile(Books&, const std::string&);
void WriteBooksToFile(const Books&, const std::string&);

class Serializer {
    const std::string SaveFile, SettingsFile;
    
    void SaveBooks(std::ofstream&, const std::map<u32,u32>&) const;
    void LoadBooks(std::ifstream&, std::map<u32,u32>&) const;

    void SaveUser(std::ofstream&, const std::shared_ptr<User>) const;
    void LoadUser(std::ifstream&, std::shared_ptr<User>) const;

public:

    Serializer(const std::string&, const std::string&);

    void SaveData(const std::vector<std::shared_ptr<User>>) const;
    void LoadData(std::vector<std::shared_ptr<User>>&) const;

    void SaveSettings(const Settings&) const;
    void LoadSettings(Settings&) const;
};