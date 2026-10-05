#ifndef PACKAGEDFOOD_H
#define PACKAGEDFOOD_H

#include "FoodItem.h"

// [INHERITANCE]
// Represents packaged items and produce driven primarily by expiry date and optional batch number.
class PackagedFood : public FoodItem {
private:
    // [ENCAPSULATION]
    std::string m_batchNumber;

public:
    PackagedFood(const std::string& name, const std::string& category, const Date& prepDate, const Date& expiryDate, const std::string& batchNumber = "");
    virtual ~PackagedFood() {}

    const std::string& getBatchNumber() const { return m_batchNumber; }
    void setBatchNumber(const std::string& batch) { m_batchNumber = batch; }

    // [POLYMORPHISM]
    bool isEligibleForDistribution(const Date& today) const override;
    bool validate() const override;
    void display() const override;
    std::string serializeDetails() const override;
};

#endif // PACKAGEDFOOD_H
