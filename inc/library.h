#pragma once
#include "serializer.h"

static const std::vector<User> DefaultUsers {
    User (L"usernamee", "passwordd")
};

class Library {
    std::vector<User> users;
    Serializer serializer;
    
public:
    Library(const std::string&);
};