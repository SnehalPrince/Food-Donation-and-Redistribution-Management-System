#ifndef DATE_H
#define DATE_H

#include <string>

// [ENCAPSULATION]
// Represents a calendar date in YYYY-MM-DD format with full leap-year validation.
class Date {
private:
    int m_year;
    int m_month;
    int m_day;

    static bool isLeapYear(int year);
    static int daysInMonth(int month, int year);
    int toDaysSinceEpoch() const;

public:
    Date();
    Date(int year, int month, int day);

    int getYear() const { return m_year; }
    int getMonth() const { return m_month; }
    int getDay() const { return m_day; }

    static bool isValid(int year, int month, int day);
    static bool parse(const std::string& str, Date& outDate);
    std::string toString() const;

    bool operator<(const Date& other) const;
    bool operator<=(const Date& other) const;
    bool operator>(const Date& other) const;
    bool operator>=(const Date& other) const;
    bool operator==(const Date& other) const;
    bool operator!=(const Date& other) const;

    static int daysBetween(const Date& d1, const Date& d2);
};

#endif // DATE_H
