#include "library.h"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <numeric>

const std::wstring SuccessColor = getCol({ 0, 255, 0 });
const std::wstring ErrorColor = getCol({ 255, 0, 0 });
const std::wstring SelectedColor = getCol({ 245, 212, 66 });
const std::wstring UnselectedColor = getCol({ 112, 109, 96 });

static const u32 MinimumUsernameLength = 5;
static const u32 MaximumUsernameLength = 20;

static const u32 MinimumPasswordLength = 7;
static const u32 MaximumPasswordLength = 50;

Library::Library(const std::string& SaveFile, const Books& books) : serializer(SaveFile), books(books) {
    if(std::filesystem::is_regular_file(SaveFile)) serializer.LoadData(users);
    else serializer.SaveData(users);
}

std::shared_ptr<User> Library::isValidName(const std::wstring& name) const {
    auto it = std::find_if(users.begin(), users.end(), [&name](const std::shared_ptr<User> user) {
        return user->name == name;
    });

    return it == users.end() ? nullptr : *it;
}

const std::vector<u8> Library::getDigits(u32 num) const {
    std::vector<u8> digits;
    digits.reserve(std::log10(num) + 1);

    if (num == 0) return { 0 };

    while (num > 0)
        digits.push_back(num % 10),
        num /= 10;

    std::reverse(digits.begin(), digits.end());
    return digits;
}

const bool Library::isIntContained(const u32 value, const u32 target) const {
    const std::vector<u8> valueDigits = getDigits(value);
    const std::vector<u8> targetDigits = getDigits(target);
    
    if (targetDigits.empty()) return true;
    if (valueDigits.size() < targetDigits.size()) return false;
    
    for (size_t i=0; i <= valueDigits.size() - valueDigits.size(); ++i) {
        bool match = true;

        for (size_t j=0; j < targetDigits.size(); ++j)
            if (valueDigits[i + j] != targetDigits[j]) {
                match = false;
                break;
            }
            
        if (match) return true;
    } return false;
}

const bool Library::showPasswordError(const bool condition, const std::wstring& errorMessage) const {
    if (!condition) return false;

    std::wcout << ErrorColor << L'\n' << errorMessage << getCol();

    getCharV();
    return true;
}

const bool Library::isWstringInt(const std::wstring& str) const {
    return std::all_of(str.begin(), str.end(), [](wchar_t c) { return std::iswdigit(c); });
}

const std::vector<u32> Library::searchQuery(std::wstring query) const {
    std::replace(query.begin(), query.end(), L'_', L' ');

    std::vector<u32> results;

    if(isWstringInt(query)) {
        const u32 id = std::stoi(query);

        for(const auto& [bookId, _] : books) 
            if(isIntContained(bookId, id)) results.push_back(bookId);
    }

    for(const auto& [bookId, book] : books)
        if(book.title.find(query) != std::wstring::npos || book.author.find(query) != std::wstring::npos)
            results.push_back(bookId);

    return results;
}

void Library::searchMenu() {
    u32 idx = 0;
    bool inTextbox = true;
    std::wstring query = L"";

    while (true) {
        clearScreen();
        const u32 sz = books.GetSize();
        const std::vector<u32> ids = books.GetIDs();
        
        std::wcout << L"Query: " << query << L"\n\n";

        u32 currentBookIdx = 0;
        for(const u32 id : query == L"" ? ids : searchQuery(query)) {
            const Book& book = books.GetBook(id);
            std::wcout << (idx == currentBookIdx++ && !inTextbox ? SelectedColor : UnselectedColor) 
                       << currentBookIdx << L") " 
                       << book.getBookOutput(std::make_pair(id, book.quantity)) 
                       << getCol();
        }

        setCursorPos(8 + query.size(), 0); // Horizontal Offset: "Query: ".size() + query.size() + 1
        const char c = getChar(!inTextbox);

        if(inTextbox) {
            if (c == 13 || c == 9 || c == 27 ) inTextbox = false; // ENTER, TAB or ESC 
            else if (c == 127 && !query.empty()) query.pop_back(); // BACKSPACE
            else if (std::iswprint(c)) query += c;

            continue;
        }

        if (std::isdigit(c)) {
            const u8 digit = c - '0' - 1;

            if (digit > sz-1) {
                clearScreen();
                std::wcout << ErrorColor << L"Error: Digit input must be between 1 and " << sz-1 << getCol();
                getCharV();
            } else idx = digit;

            continue;
        }

        switch(c) {
            case 'w': case 'a':
            idx = idx == 0 ? sz-1 : idx - 1; break;

            case 's': case 'd':
            idx = idx == sz-1 ? 0 : idx + 1; break;

            case 9: inTextbox = true; break; // TAB
            case 'q': return;

            default:
            clearScreen();

            const u32 id = ids[idx];
            const std::wstring& title = books.GetBook(id).title;
            const u32 stock = books.GetBook(id).quantity;

            std::wcout << L"How many copies of " << title << L" do you wish to purchase? [0-" << stock << L"] ";
            u32 quantity;
            std::cin >> quantity;
            clearInputBuffer();

            if(quantity == 0) continue;

            if(quantity > stock) {
                std::wcout << ErrorColor << L"Error: You cannot buy more than " << stock << L" copies of " << title << getCol();
                getCharV();
                continue;
            }

            if(user->shoppingCart.find(id) == user->shoppingCart.end()) user->shoppingCart.insert(std::make_pair(id, quantity));
            else user->shoppingCart.at(id) += quantity;
            books.ChangeBook(id, -quantity);

            serializer.SaveData(users);
            WriteBooksToFile(books, BooksFile);

            break;
        }
    }
}

void Library::cartMenu() {
    u32 idx = 0;

    while(true) {
        clearScreen();

        std::wcout << SuccessColor << L"Shopping Cart:\n\n" << getCol();

        if(user->shoppingCart.empty()) {
            std::wcout << ErrorColor << L"Your shopping cart is empty\n" << getCol();
            getCharV();
            return;
        }

        u32 currentBookIdx = 0;
        for(const std::pair<u32,u32>& entry : user->shoppingCart) {
            const Book& book = books.GetBook(entry.first);
            std::wcout << (currentBookIdx++ == idx ? SelectedColor : UnselectedColor) << currentBookIdx << L") " << book.getBookOutput(entry) << getCol();
        }

        const char c = getChar();
        const u32 sz = user->shoppingCart.size();

        if(std::isdigit(c)) {
            const u32 digit = c - '0' - 1;

            if(digit > sz-1) {
                clearScreen();
                std::wcout << ErrorColor << L"Error: Digit input must be between 1 and " << sz-1 << getCol() << '\n';
                getCharV();
            } else idx = digit;
            
            continue;
        }

        switch(c) {
            case 'w':
            case 'd':
            idx = idx == 0 ? sz-1 : idx - 1; break;
            break;

            case 's':
            case 'a':
            idx = idx == sz-1 ? 0 : idx + 1; break;
            break;

            case 'j': {
                const std::vector<u32> ids = books.GetIDs();

                clearScreen();

                const u32 id = std::next(user->shoppingCart.begin(), idx)->first;
                const std::wstring& title = books.GetBook(id).title;
                const u32 inCart = user->shoppingCart[id];

                std::wcout << L"How many copies of " << title << L" do you wish to remove? [0-" << inCart << L"] ";

                u32 quantity;
                std::cin >> quantity;
                clearInputBuffer();

                if(quantity == 0) continue;
                if(quantity >= inCart)
                    quantity = inCart, 
                    user->shoppingCart.erase(id),
                    idx = idx == sz-1 ? sz-2 : idx;
                else user->shoppingCart.at(id) -= quantity;
                books.ChangeBook(id, quantity);

                serializer.SaveData(users);
                WriteBooksToFile(books, BooksFile);
            } break;

            case 'q': return;

            default:
            const double price = std::accumulate(user->shoppingCart.begin(), user->shoppingCart.end(), 0.0, 
                                [this](double sum, const std::pair<u32,u32>& entry) {
                                    return sum + books.GetBook(entry.first).price * entry.second;
                                });

            if(user->balance < price) {
                std::wcout << ErrorColor << L"Error: You are $" << price-user->balance << L" away from being able to afford all books in the cart" << getCol();
                getCharV();
                continue;
            }
            
            user->balance -= price;
            user->boughtBooks.insert(user->shoppingCart.begin(), user->shoppingCart.end());
            user->shoppingCart.clear();
        
            serializer.SaveData(users);
            WriteBooksToFile(books, BooksFile);

            std::wcout << SuccessColor << L"Successfully bought $" << price << L" worth of books\n" << getCol();
            getCharV();
            return;
        }
    }
}

void Library::returnMenu() {
    u32 idx = 0;

    while(true) {
        const u32 sz = user->boughtBooks.size();

        clearScreen();

        std::wcout << SuccessColor << L"Bought books:\n\n" << getCol();

        u32 currentBookIdx = 0;
        for(const std::pair<u32,u32>& entry : user->boughtBooks) {
            const Book& book = books.GetBook(entry.first);

            std::wcout << (idx == currentBookIdx++ ? SelectedColor : UnselectedColor) << currentBookIdx << L") " << book.getBookOutput(entry) << getCol();
        }

        
        const char c = getChar();
        
        if(std::isdigit(c)) {
            const u32 digit = c - '0' - 1;
            
            if(idx > sz-1) {
                clearScreen();
                std::wcout << ErrorColor << L"Error: Digit input must be between 1 and " << sz-1 << getCol() << '\n';
                getCharV();
            } else idx = digit;
            
            continue;
        }
        
        switch(c) {
            case 'w':
            case 'd':
            idx = idx == 0 ? sz-1 : idx - 1; break;
            break;
            
            case 's':
            case 'a':
            idx = idx == sz-1 ? 0 : idx + 1; break;
            break;
            
            case 'q': return;
            
            default:
            
            break;
        }
    }

    serializer.SaveData(users);
}

void Library::execMainMenu(const u8 idx) {
    switch (idx) {
        case 0: {
            std::wcout << L"Enter an amount to add: ";

            double amount;
            std::cin >> amount;
            clearInputBuffer();

            if (amount < 0) std::wcout << ErrorColor << L"Invalid amount\n" << getCol(), 
                               getCharV();
            else user->balance += amount;
        } break;

        case 1: searchMenu(); break;
        case 2: cartMenu(); break;
        case 3: returnMenu(); break;
    }

    serializer.SaveData(users);
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
        
        const char c = getChar();

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

            default: execMainMenu(idx); break;

            case 'q': return;
        }
    }

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
        else if (!hasAccount && std::find_if(users.begin(), users.end(), [&name](const std::shared_ptr<User> user) {
            return user->name == name;
            }) != users.end()) {
            std::wcout << ErrorColor << L"\nUsername already exists\n" << getCol();
            getCharV();
        } else if (hasAccount) {
            std::shared_ptr<User> result = isValidName(name);

            if (result == nullptr) {
                std::wcout << ErrorColor << L"\nThere is nobody with that username" << getCol();
                getCharV();
                continue;
            }

            user = result;
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

    if (!hasAccount) 
        users.push_back(std::make_shared<User>(name, password)), user = users.back(),
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