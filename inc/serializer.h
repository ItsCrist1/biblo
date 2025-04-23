#pragma once
#include "data.h"
#include <vector>

void setBooksFromFile(std::vector<Book>&, const std::string&);

class Serializer {
    const std::string SaveFile;
    
    void SaveBooks(std::ofstream&, const std::vector<std::pair<u32,u32>>&) const;
    void LoadBooks(std::ifstream&, std::vector<std::pair<u32,u32>>&) const;

    void SaveUser(std::ofstream&, const User&) const;
    void LoadUser(std::ifstream&, User&) const;

public:

    Serializer(const std::string&);

    void SaveData(const std::vector<User>&) const;
    void LoadData(std::vector<User>&) const;
};