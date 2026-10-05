#include "CookedFood.h"
#include <iostream>

const int CookedFood::MAX_COOKED_VALIDITY_DAYS;
const int CookedFood::MAX_BAKERY_VALIDITY_DAYS;

// [INHERITANCE]
CookedFood::CookedFood(const std::string& name, const std::string& category, const Date& prepDate, const Date& expiryDate)
    : FoodItem(name, category, prepDate, expiryDate) {}

// [POLYMORPHISM]
bool CookedFood::isEligibleForDistribution(const Date& today) const {
    return (getPreparationDate() <= today) && (today <= getExpiryDate());
}

bool CookedFood::validate() const {
    if (getExpiryDate() < getPreparationDate()) return false;
    int diff = Date::daysBetween(getPreparationDate(), getExpiryDate());
    if (getCategory() == "COOKED" && diff > MAX_COOKED_VALIDITY_DAYS) return false;
    if (getCategory() == "BAKERY" && diff > MAX_BAKERY_VALIDITY_DAYS) return false;
    return true;
}

void CookedFood::display() const {
    std::cout << "Food: " << getName()
              << " [" << getCategory() << "]"
              << " | Prepared: " << getPreparationDate().toString()
              << " | Expiry: " << getExpiryDate().toString() << "\n";
}

std::string CookedFood::serializeDetails() const {
    return "-";
}
