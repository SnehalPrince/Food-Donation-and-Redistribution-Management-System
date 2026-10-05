#ifndef INPUTHELPER_H
#define INPUTHELPER_H

#include <string>
#include "Date.h"

// [ENCAPSULATION]
// Provides safe, validated console input helpers preventing crashes, loops, and injection.
class InputHelper {
public:
    static bool readString(const std::string& prompt, std::string& out, bool allowEmpty = false, size_t maxLen = 100);
    static bool readId(const std::string& prompt, const std::string& defaultId, std::string& out);
    static bool readDouble(const std::string& prompt, double& out, double minVal, double maxVal);
    static bool readInt(const std::string& prompt, int& out, int minVal, int maxVal);
    static bool readDate(const std::string& prompt, const Date& defaultDate, Date& out);
    static bool readDateRequired(const std::string& prompt, Date& out);
    static bool readConfirmation(const std::string& prompt, bool defaultYes = true);
    static bool isEof();
    static void resetEof();
};

#endif // INPUTHELPER_H
