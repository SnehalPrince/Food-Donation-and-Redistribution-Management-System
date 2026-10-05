#ifndef DONATION_H
#define DONATION_H

#include <string>
#include <memory>
#include "FoodItem.h"
#include "Date.h"

enum DonationStatus {
    DONATION_AVAILABLE,
    DONATION_PARTIALLY_ALLOCATED,
    DONATION_FULLY_ALLOCATED,
    DONATION_EXPIRED
};

std::string donationStatusToString(DonationStatus status);

// [ENCAPSULATION]
// Represents a donation batch, acting as the single source of truth for food stock.
class Donation {
public:
    static const double EPS;

private:
    std::string m_donationId;
    std::string m_donorId;
    double m_totalQuantity;
    double m_availableQuantity;
    std::shared_ptr<FoodItem> m_foodItem;
    Date m_donationDate;

public:
    Donation(const std::string& donationId,
             const std::string& donorId,
             double quantity,
             std::shared_ptr<FoodItem> foodItem,
             const Date& donationDate);

    const std::string& getDonationId() const { return m_donationId; }
    const std::string& getDonorId() const { return m_donorId; }
    double getTotalQuantity() const { return m_totalQuantity; }
    double getAvailableQuantity() const { return m_availableQuantity; }
    std::shared_ptr<FoodItem> getFoodItem() const { return m_foodItem; }
    const Date& getDonationDate() const { return m_donationDate; }

    bool reserveQuantity(double qty);
    void releaseQuantity(double qty);
    void setAvailableQuantity(double qty) { m_availableQuantity = qty; }

    DonationStatus getStatus(const Date& today) const;
    void display(const Date& today) const;
};

#endif // DONATION_H
