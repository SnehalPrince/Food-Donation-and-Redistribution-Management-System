#ifndef FOODITEM_H
#define FOODITEM_H

#include <string>
#include <memory>
#include "Date.h"

// [ABSTRACTION]
// Abstract base class representing food metadata and expiry validation rules.
class FoodItem {
private:
    // [ENCAPSULATION]
    std::string m_name;
    std::string m_category;
    Date m_preparationDate;
    Date m_expiryDate;

public:
    FoodItem(const std::string& name, const std::string& category, const Date& prepDate, const Date& expiryDate);
    virtual ~FoodItem() {}

    const std::string& getName() const { return m_name; }
    const std::string& getCategory() const { return m_category; }
    const Date& getPreparationDate() const { return m_preparationDate; }
    const Date& getExpiryDate() const { return m_expiryDate; }

    void setPreparationDate(const Date& date) { m_preparationDate = date; }
    void setExpiryDate(const Date& date) { m_expiryDate = date; }

    // [POLYMORPHISM]
    virtual bool isEligibleForDistribution(const Date& today) const = 0;
    virtual bool validate() const = 0;
    virtual void display() const = 0;
    virtual std::string serializeDetails() const = 0;

    // Factory method for creating polymorphic FoodItem derived instances
    static std::shared_ptr<FoodItem> create(const std::string& name,
                                           const std::string& category,
                                           const Date& prepDate,
                                           const Date& expiryDate,
                                           const std::string& extraDetails = "");
};

#endif // FOODITEM_H
