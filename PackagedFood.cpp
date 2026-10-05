#include "PackagedFood.h"
#include <iostream>

// [INHERITANCE]
PackagedFood::PackagedFood(const std::string& name, const std::string& category, const Date& prepDate, const Date& expiryDate, const std::string& batchNumber)
    : FoodItem(name, category, prepDate, expiryDate), m_batchNumber(batchNumber) {}

// [POLYMORPHISM]
bool PackagedFood::isEligibleForDistribution(const Date& today) const {
    return today <= getExpiryDate();
}

bool PackagedFood::validate() const {
    return getPreparationDate() <= getExpiryDate();
}

void PackagedFood::display() const {
    std::cout << "Food: " << getName()
              << " [" << getCategory() << "]"
              << " | Batch: " << (m_batchNumber.empty() ? "N/A" : m_batchNumber)
              << " | Expiry: " << getExpiryDate().toString() << "\n";
}

std::string PackagedFood::serializeDetails() const {
    return m_batchNumber.empty() ? "-" : m_batchNumber;
}
