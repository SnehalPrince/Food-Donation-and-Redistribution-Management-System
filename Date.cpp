#include "Date.h"
#include <sstream>
#include <iomanip>
#include <cstdlib>

// [ENCAPSULATION]
Date::Date() : m_year(2026), m_month(1), m_day(1) {}

Date::Date(int year, int month, int day) : m_year(year), m_month(month), m_day(day) {
    if (!isValid(year, month, day)) {
        m_year = 2026;
        m_month = 1;
        m_day = 1;
    }
}

bool Date::isLeapYear(int year) {
    if (year % 400 == 0) return true;
    if (year % 100 == 0) return false;
    return (year % 4 == 0);
}

int Date::daysInMonth(int month, int year) {
    if (month < 1 || month > 12) return 0;
    static const int days[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (month == 2 && isLeapYear(year)) return 29;
    return days[month];
}

bool Date::isValid(int year, int month, int day) {
    if (year < 1900 || year > 2100) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > daysInMonth(month, year)) return false;
    return true;
}

bool Date::parse(const std::string& str, Date& outDate) {
    if (str.length() != 10) return false;
    if (str[4] != '-' || str[7] != '-') return false;

    for (size_t i = 0; i < str.length(); ++i) {
        if (i == 4 || i == 7) continue;
        if (str[i] < '0' || str[i] > '9') return false;
    }

    int y = std::atoi(str.substr(0, 4).c_str());
    int m = std::atoi(str.substr(5, 2).c_str());
    int d = std::atoi(str.substr(8, 2).c_str());

    if (!isValid(y, m, d)) return false;

    outDate = Date(y, m, d);
    return true;
}

std::string Date::toString() const {
    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(4) << m_year << "-"
        << std::setw(2) << m_month << "-"
        << std::setw(2) << m_day;
    return oss.str();
}

int Date::toDaysSinceEpoch() const {
    int y = m_year;
    int m = m_month;
    int d = m_day;
    if (m <= 2) {
        y -= 1;
        m += 12;
    }
    int era = (y >= 0 ? y : y - 399) / 400;
    int yoe = y - era * 400;
    int doy = (153 * (m - 3) + 2) / 5 + d - 1;
    int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe;
}

bool Date::operator<(const Date& other) const {
    if (m_year != other.m_year) return m_year < other.m_year;
    if (m_month != other.m_month) return m_month < other.m_month;
    return m_day < other.m_day;
}

bool Date::operator<=(const Date& other) const {
    return (*this < other) || (*this == other);
}

bool Date::operator>(const Date& other) const {
    return !(*this <= other);
}

bool Date::operator>=(const Date& other) const {
    return !(*this < other);
}

bool Date::operator==(const Date& other) const {
    return m_year == other.m_year && m_month == other.m_month && m_day == other.m_day;
}

bool Date::operator!=(const Date& other) const {
    return !(*this == other);
}

int Date::daysBetween(const Date& d1, const Date& d2) {
    return d2.toDaysSinceEpoch() - d1.toDaysSinceEpoch();
}
