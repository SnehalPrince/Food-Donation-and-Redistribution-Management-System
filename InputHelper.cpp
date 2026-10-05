#include "InputHelper.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

// [ENCAPSULATION]
static bool g_eof_detected = false;

static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

static std::string toUpper(const std::string& str) {
    std::string res = str;
    for (size_t i = 0; i < res.length(); ++i) {
        res[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(res[i])));
    }
    return res;
}

bool InputHelper::isEof() {
    return g_eof_detected || std::cin.eof();
}

void InputHelper::resetEof() {
    g_eof_detected = false;
    std::cin.clear();
}

bool InputHelper::readString(const std::string& prompt, std::string& out, bool allowEmpty, size_t maxLen) {
    while (true) {
        if (isEof()) return false;
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            g_eof_detected = true;
            return false;
        }
        std::string clean = trim(line);
        if (clean.find('|') != std::string::npos) {
            std::cout << "Error: Input cannot contain the pipe character ('|'). Try again.\n";
            continue;
        }
        if (clean.empty() && !allowEmpty) {
            std::cout << "Error: Input cannot be empty. Try again.\n";
            continue;
        }
        if (clean.length() > maxLen) {
            std::cout << "Error: Input is too long (max " << maxLen << " characters). Try again.\n";
            continue;
        }
        out = clean;
        return true;
    }
}

bool InputHelper::readId(const std::string& prompt, const std::string& defaultId, std::string& out) {
    while (true) {
        if (isEof()) return false;
        std::cout << prompt;
        if (!defaultId.empty()) {
            std::cout << " [" << defaultId << "]: ";
        } else {
            std::cout << ": ";
        }
        std::string line;
        if (!std::getline(std::cin, line)) {
            g_eof_detected = true;
            return false;
        }
        std::string clean = trim(line);
        if (clean.empty() && !defaultId.empty()) {
            out = defaultId;
            return true;
        }
        clean = toUpper(clean);
        if (clean.find('|') != std::string::npos) {
            std::cout << "Error: ID cannot contain '|'. Try again.\n";
            continue;
        }
        if (clean.length() < 1 || clean.length() > 12) {
            std::cout << "Error: ID must be 1 to 12 characters. Try again.\n";
            continue;
        }
        bool validChars = true;
        for (size_t i = 0; i < clean.length(); ++i) {
            char c = clean[i];
            if (!std::isalnum(static_cast<unsigned char>(c)) && c != '-' && c != '_') {
                validChars = false;
                break;
            }
        }
        if (!validChars) {
            std::cout << "Error: ID must only contain letters, digits, '-', or '_'. Try again.\n";
            continue;
        }
        out = clean;
        return true;
    }
}

bool InputHelper::readDouble(const std::string& prompt, double& out, double minVal, double maxVal) {
    while (true) {
        if (isEof()) return false;
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            g_eof_detected = true;
            return false;
        }
        std::string clean = trim(line);
        if (clean.empty()) {
            std::cout << "Error: Input cannot be empty. Try again.\n";
            continue;
        }
        std::istringstream iss(clean);
        double val;
        char extra;
        if (!(iss >> val) || (iss >> extra)) {
            std::cout << "Error: Please enter a valid numerical value. Try again.\n";
            continue;
        }
        if (val < minVal || val > maxVal) {
            std::cout << "Error: Value must be between " << minVal << " and " << maxVal << ". Try again.\n";
            continue;
        }
        out = val;
        return true;
    }
}

bool InputHelper::readInt(const std::string& prompt, int& out, int minVal, int maxVal) {
    while (true) {
        if (isEof()) return false;
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            g_eof_detected = true;
            return false;
        }
        std::string clean = trim(line);
        if (clean.empty()) {
            std::cout << "Error: Input cannot be empty. Try again.\n";
            continue;
        }
        std::istringstream iss(clean);
        int val;
        char extra;
        if (!(iss >> val) || (iss >> extra)) {
            std::cout << "Error: Please enter a valid integer. Try again.\n";
            continue;
        }
        if (val < minVal || val > maxVal) {
            std::cout << "Error: Choice must be between " << minVal << " and " << maxVal << ". Try again.\n";
            continue;
        }
        out = val;
        return true;
    }
}

bool InputHelper::readDate(const std::string& prompt, const Date& defaultDate, Date& out) {
    while (true) {
        if (isEof()) return false;
        std::cout << prompt << " (YYYY-MM-DD) [" << defaultDate.toString() << "]: ";
        std::string line;
        if (!std::getline(std::cin, line)) {
            g_eof_detected = true;
            return false;
        }
        std::string clean = trim(line);
        if (clean.empty()) {
            out = defaultDate;
            return true;
        }
        Date parsed;
        if (!Date::parse(clean, parsed)) {
            std::cout << "Error: Invalid date format or calendar date. Must be YYYY-MM-DD. Try again.\n";
            continue;
        }
        out = parsed;
        return true;
    }
}

bool InputHelper::readDateRequired(const std::string& prompt, Date& out) {
    while (true) {
        if (isEof()) return false;
        std::cout << prompt << " (YYYY-MM-DD): ";
        std::string line;
        if (!std::getline(std::cin, line)) {
            g_eof_detected = true;
            return false;
        }
        std::string clean = trim(line);
        Date parsed;
        if (!Date::parse(clean, parsed)) {
            std::cout << "Error: Invalid date format or calendar date. Must be YYYY-MM-DD. Try again.\n";
            continue;
        }
        out = parsed;
        return true;
    }
}

bool InputHelper::readConfirmation(const std::string& prompt, bool defaultYes) {
    while (true) {
        if (isEof()) return false;
        std::cout << prompt << (defaultYes ? " [Y/n]: " : " [y/N]: ");
        std::string line;
        if (!std::getline(std::cin, line)) {
            g_eof_detected = true;
            return false;
        }
        std::string clean = toUpper(trim(line));
        if (clean.empty()) return defaultYes;
        if (clean == "Y" || clean == "YES") return true;
        if (clean == "N" || clean == "NO") return false;
        std::cout << "Error: Please respond with 'y' or 'n'.\n";
    }
}
