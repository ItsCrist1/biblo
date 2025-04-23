#include "library.h"
#include <iostream>
#include <filesystem>

Library::Library(const std::string& SaveFile) : serializer(SaveFile) {
    if(std::filesystem::is_regular_file(SaveFile)) serializer.LoadData(users);
    else users = DefaultUsers, serializer.SaveData(users);
}