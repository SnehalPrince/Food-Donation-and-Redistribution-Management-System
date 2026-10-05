#include "FoodItem.h"
#include "CookedFood.h"
#include "PackagedFood.h"

// [ABSTRACTION]
FoodItem::FoodItem(const std::string& name, const std::string& category, const Date& prepDate, const Date& expiryDate)
    : m_name(name), m_category(category), m_preparationDate(prepDate), m_expiryDate(expiryDate) {}

std::shared_ptr<FoodItem> FoodItem::create(const std::string& name,
                                           const std::string& category,
                                           const Date& prepDate,
                                           const Date& expiryDate,
                                           const std::string& extraDetails) {
    if (category == "COOKED" || category == "BAKERY") {
        return std::shared_ptr<FoodItem>(new CookedFood(name, category, prepDate, expiryDate));
    } else {
        return std::shared_ptr<FoodItem>(new PackagedFood(name, category, prepDate, expiryDate, extraDetails));
    }
}
