#ifndef COOKEDFOOD_H
#define COOKEDFOOD_H

#include "FoodItem.h"

// [INHERITANCE]
// Represents prepared food (cooked meals, bakery) with strict preparation date and shelf-life constraints.
class CookedFood : public FoodItem {
public:
    static const int MAX_COOKED_VALIDITY_DAYS = 2;
    static const int MAX_BAKERY_VALIDITY_DAYS = 3;

    CookedFood(const std::string& name, const std::string& category, const Date& prepDate, const Date& expiryDate);
    virtual ~CookedFood() {}

    // [POLYMORPHISM]
    bool isEligibleForDistribution(const Date& today) const override;
    bool validate() const override;
    void display() const override;
    std::string serializeDetails() const override;
};

#endif // COOKEDFOOD_H
