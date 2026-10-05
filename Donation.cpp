#include "Donation.h"
#include <iostream>
#include <iomanip>
#include <cmath>

const double Donation::EPS = 1e-9;

std::string donationStatusToString(DonationStatus status) {
    switch (status) {
        case DONATION_AVAILABLE: return "AVAILABLE";
        case DONATION_PARTIALLY_ALLOCATED: return "PARTIALLY_ALLOCATED";
        case DONATION_FULLY_ALLOCATED: return "FULLY_ALLOCATED";
        case DONATION_EXPIRED: return "EXPIRED";
        default: return "UNKNOWN";
    }
}

// [ENCAPSULATION]
Donation::Donation(const std::string& donationId,
                   const std::string& donorId,
                   double quantity,
                   std::shared_ptr<FoodItem> foodItem,
                   const Date& donationDate)
    : m_donationId(donationId),
      m_donorId(donorId),
      m_totalQuantity(quantity),
      m_availableQuantity(quantity),
      m_foodItem(foodItem),
      m_donationDate(donationDate) {}

bool Donation::reserveQuantity(double qty) {
    if (qty <= 0.0) return false;
    if (qty > m_availableQuantity + EPS) return false;

    m_availableQuantity -= qty;
    if (m_availableQuantity < EPS) {
        m_availableQuantity = 0.0;
    }
    return true;
}

void Donation::releaseQuantity(double qty) {
    if (qty <= 0.0) return;
    m_availableQuantity += qty;
    if (m_availableQuantity > m_totalQuantity) {
        m_availableQuantity = m_totalQuantity;
    }
}

DonationStatus Donation::getStatus(const Date& today) const {
    if (m_availableQuantity <= EPS) {
        return DONATION_FULLY_ALLOCATED;
    }
    if (m_foodItem && m_foodItem->getExpiryDate() < today) {
        return DONATION_EXPIRED;
    }
    if (m_availableQuantity < m_totalQuantity - EPS) {
        return DONATION_PARTIALLY_ALLOCATED;
    }
    return DONATION_AVAILABLE;
}

void Donation::display(const Date& today) const {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Donation [" << m_donationId << "]"
              << " | Donor: " << m_donorId
              << " | Food: " << (m_foodItem ? m_foodItem->getName() : "Unknown")
              << " (" << (m_foodItem ? m_foodItem->getCategory() : "Unknown") << ")"
              << " | Available: " << m_availableQuantity << " / " << m_totalQuantity << " kg"
              << " | Status: " << donationStatusToString(getStatus(today)) << "\n";
}
