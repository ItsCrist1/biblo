#include "library.h"
#include <iostream>
#include <filesystem>
#include <algorithm>

const std::wstring SuccessColor = getCol({ 0, 255, 0 });
const std::wstring ErrorColor = getCol({ 255, 0, 0 });
const std::wstring SelectedColor = getCol({ 245, 212, 66 });
const std::wstring UnselectedColor = getCol({ 112, 109, 96 });

static const u32 MinimumUsernameLength = 5;
static const u32 MaximumUsernameLength = 20;

static const u32 MinimumPasswordLength = 7;
static const u32 MaximumPasswordLength = 50;

Library::Library(const std::string& SaveFile) : serializer(SaveFile) {
    if(std::filesystem::is_regular_file(SaveFile)) serializer.LoadData(users);
    else serializer.SaveData(users);
}

std::unique_ptr<User> Library::isValidName(const std::wstring& name) const {
    auto it = std::find_if(users.begin(), users.end(), [&name](const User& user) {
        return user.name == name;
    });

    return it == users.end() ? nullptr : std::make_unique<User>(*it);
}

bool Library::showPasswordError(const bool condition, const std::wstring& errorMessage) const {
    if (!condition) return false;

    std::wcout << ErrorColor << L'\n' << errorMessage << getCol();

    getCharV();
    return true;
}

void Library::mainMenu() {
    u8 idx = 0;

    while (true) {
        clearScreen();

        std::wcout << L"--- Library System ---\n"
            << L"Funds: $" << user->balance << L"\n\n"
            << (idx == 0 ? SuccessColor : UnselectedColor) << L"1) Add Funds\n"
            << (idx == 1 ? SuccessColor : UnselectedColor) << L"2) Search Books\n"
            << (idx == 2 ? SuccessColor : UnselectedColor) << L"3) View Shopping Cart\n"
            << (idx == 3 ? SuccessColor : UnselectedColor) << L"4) Return Books\n"
            << getCol();
        
        char c = getChar();

        if (std::isdigit(c)) {
            const u8 digit = c - '0' - 1;

            if (digit > 3) {
                clearScreen();
                std::wcout << ErrorColor << L"Error: Digit input must be between 1 and 4\n" << getCol();
                getCharV();
            } else idx = digit;

            continue;
        }

        switch(c) {
            case 'w': case 'a':
            idx = idx == 0 ? 3 : idx - 1; break;

            case 's': case 'd':
            idx = idx == 3 ? 0 : idx + 1; break;

            default:

            break;

            case 'q': return;
        }
    }

    serializer.SaveData(users);
}

void Library::signMenu(const bool hasAccount) {
    std::wstring name;

    while (true) {
        clearScreen();
        std::wcout << L"Sign " << (hasAccount ? L"In" : L"Up") << L"\n\nEnter a username ("
            << MinimumUsernameLength << L'-' << MaximumUsernameLength
            << L" characters): ";

        std::string temp;
        std::cin >> temp;
        name = stw(temp);
        clearInputBuffer();

        if (const u32 sz = name.length(); !(sz >= MinimumUsernameLength && sz <= MaximumUsernameLength)) {
            std::wcout << ErrorColor << L"\nInvalid username length " << getCol() << L"(Must be between "
                << MinimumUsernameLength << L'-' << MaximumUsernameLength
                << L" characters)" << getCol();
            getCharV();
        }
        else if (!hasAccount && std::find_if(users.begin(), users.end(), [&name](const User& user) {
            return user.name == name;
            }) != users.end()) {
            std::wcout << ErrorColor << L"\nUsername already exists\n" << getCol();
            getCharV();
        } else if (hasAccount) {
            std::unique_ptr<User> result = isValidName(name);

            if (result == nullptr) {
                std::wcout << ErrorColor << L"\nThere is nobody with that username" << getCol();
                getCharV();
                continue;
            }

            user = std::move(result);
            break;
        } else break;
    }

    std::string password;

    while (true) {
        clearScreen();
        std::wcout << L"Sign " << (hasAccount ? L"In" : L"Up") << L"\nUsername: " << name
            << L"\n\nEnter a password ("
            << MinimumPasswordLength << L'-' << MaximumPasswordLength
            << L" characters): ";

        std::cin >> password;
        clearInputBuffer();

        if (const u32 sz = password.length(); !(sz >= MinimumPasswordLength && sz <= MaximumPasswordLength)) {
            std::wcout << ErrorColor << L"\nInvalid password length " << getCol() << L"(Must be between "
                << MinimumPasswordLength << L'-' << MaximumPasswordLength
                << L" characters)" << getCol();

            getCharV();
            continue;
        }
        else if (hasAccount && password != user->password) {
            std::wcout << ErrorColor << L"\nInvalid password for user " << user->name << getCol();

            getCharV();
            continue;
        }

        bool hasLower = false, hasUpper = false, hasDigit = false, hasSymbol = false;

        for (const char c : password)
            if (!hasLower && std::islower(c)) hasLower = true;
            else if (!hasUpper && std::isupper(c)) hasUpper = true;
            else if (!hasDigit && std::isdigit(c)) hasDigit = true;
            else if (!hasSymbol && !isalnum(c) && !std::isspace(c)) hasSymbol = true;

        if (showPasswordError(!hasLower, L"Password must contain at least one lowercase letter") ||
            showPasswordError(!hasUpper, L"Password must contain at least one uppercase letter") ||
            showPasswordError(!hasDigit, L"Password must contain at least one digit") ||
            showPasswordError(!hasSymbol, L"Password must contain at least one symbol"))
            continue;

        break;
    }

    if (!hasAccount) users.emplace_back(name, password), user = std::make_unique<User>(users.back());

    serializer.SaveData(users);
    mainMenu();
}

void Library::GreetMenu() {
    if(users.empty()) {
        signMenu(false);
        return;
    }

    u8 idx = 0;
    bool running = true;

    while (running) {
        clearScreen();

        std::wcout << L"--- Library System ---\n\n"
            << L"Do you have an existing account?"
            << (idx == 0 ? SuccessColor : UnselectedColor) << L"\n1) Yes\n"
            << (idx == 1 ? ErrorColor : UnselectedColor) << L"2) No\n"
            << getCol();

        const char c = getChar();

        if (std::isdigit(c)) {
            const u8 digit = c - '0';

            if (digit < 1 || digit > 2) {
                clearScreen();
                std::wcout << ErrorColor << L"Error: Digit input must be between 1 and 2\n" << getCol();
                getCharV();
            } else signMenu(digit == 1);

            break;
        }

        switch (c) {
            case 'w': case 's':
            case 'a': case 'd':
            idx = idx == 0 ? 1 : 0; break;

            case 'q': running = false; break;

            default:
            signMenu(idx == 0);
            running = false;
            break;
        }
    }
}