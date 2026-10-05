#include <iostream>
#include "test_util.h"
#include "../Date.h"
#include "../FoodItem.h"
#include "../CookedFood.h"
#include "../PackagedFood.h"
#include "../Donation.h"

static bool testCookedFoodValidity() {
    Date prep(2026, 10, 5);
    Date expValid(2026, 10, 7);   // 2 days - OK
    Date expTooLong(2026, 10, 8); // 3 days - Exceeds max 2 days for COOKED

    CookedFood cGood("Rice & Curry", "COOKED", prep, expValid);
    ASSERT_TRUE(cGood.validate());

    CookedFood cBad("Rice & Curry", "COOKED", prep, expTooLong);
    ASSERT_FALSE(cBad.validate());

    // Bakery allows max 3 days
    Date expBakeryValid(2026, 10, 8); // 3 days - OK
    Date expBakeryBad(2026, 10, 9);   // 4 days - Bad
    CookedFood bGood("Croissant", "BAKERY", prep, expBakeryValid);
    ASSERT_TRUE(bGood.validate());

    CookedFood bBad("Croissant", "BAKERY", prep, expBakeryBad);
    ASSERT_FALSE(bBad.validate());

    // Expiry before prep
    Date expBefore(2026, 10, 4);
    CookedFood cBefore("Soup", "COOKED", prep, expBefore);
    ASSERT_FALSE(cBefore.validate());

    return true;
}

static bool testEligibilityBoundaries() {
    Date prep(2026, 10, 5);
    Date exp(2026, 10, 7);
    CookedFood cooked("Dal Makhani", "COOKED", prep, exp);

    Date beforePrep(2026, 10, 4);
    Date onPrep(2026, 10, 5);
    Date middle(2026, 10, 6);
    Date onExpiry(2026, 10, 7);
    Date afterExpiry(2026, 10, 8);

    // Eligible iff prep <= today <= expiry (inclusive)
    ASSERT_FALSE(cooked.isEligibleForDistribution(beforePrep));
    ASSERT_TRUE(cooked.isEligibleForDistribution(onPrep));
    ASSERT_TRUE(cooked.isEligibleForDistribution(middle));
    ASSERT_TRUE(cooked.isEligibleForDistribution(onExpiry));
    ASSERT_FALSE(cooked.isEligibleForDistribution(afterExpiry));

    // Packaged food is eligible iff today <= expiry
    PackagedFood pkg("Canned Beans", "PACKAGED", prep, exp, "BATCH-99");
    ASSERT_TRUE(pkg.isEligibleForDistribution(beforePrep));
    ASSERT_TRUE(pkg.isEligibleForDistribution(onPrep));
    ASSERT_TRUE(pkg.isEligibleForDistribution(onExpiry));
    ASSERT_FALSE(pkg.isEligibleForDistribution(afterExpiry));

    return true;
}

static bool testFoodFactory() {
    Date prep(2026, 10, 5);
    Date exp(2026, 10, 7);

    std::shared_ptr<FoodItem> f1 = FoodItem::create("Biryani", "COOKED", prep, exp);
    ASSERT_TRUE(f1 != NULL);
    ASSERT_EQ(std::string("COOKED"), f1->getCategory());
    ASSERT_TRUE(f1->validate());

    std::shared_ptr<FoodItem> f2 = FoodItem::create("Biscuits", "PACKAGED", prep, exp, "LOT123");
    ASSERT_TRUE(f2 != NULL);
    ASSERT_EQ(std::string("PACKAGED"), f2->getCategory());
    ASSERT_EQ(std::string("LOT123"), f2->serializeDetails());

    return true;
}

static bool testDonationStatusAndReservation() {
    Date today(2026, 10, 5);
    Date prep(2026, 10, 5);
    Date exp(2026, 10, 7);
    std::shared_ptr<FoodItem> food = FoodItem::create("Chapati", "COOKED", prep, exp);

    Donation don("DON101", "D001", 50.0, food, today);
    ASSERT_NEAR(50.0, don.getTotalQuantity(), 1e-9);
    ASSERT_NEAR(50.0, don.getAvailableQuantity(), 1e-9);
    ASSERT_EQ(DONATION_AVAILABLE, don.getStatus(today));

    // Reserve 30 kg
    ASSERT_TRUE(don.reserveQuantity(30.0));
    ASSERT_NEAR(20.0, don.getAvailableQuantity(), 1e-9);
    ASSERT_EQ(DONATION_PARTIALLY_ALLOCATED, don.getStatus(today));

    // Over-reservation should fail
    ASSERT_FALSE(don.reserveQuantity(25.0));
    ASSERT_NEAR(20.0, don.getAvailableQuantity(), 1e-9);

    // Reserve remaining 20 kg
    ASSERT_TRUE(don.reserveQuantity(20.0));
    ASSERT_NEAR(0.0, don.getAvailableQuantity(), 1e-9);
    ASSERT_EQ(DONATION_FULLY_ALLOCATED, don.getStatus(today));

    // Release 15 kg
    don.releaseQuantity(15.0);
    ASSERT_NEAR(15.0, don.getAvailableQuantity(), 1e-9);
    ASSERT_EQ(DONATION_PARTIALLY_ALLOCATED, don.getStatus(today));

    // Check expiration on date passing
    Date futureDate(2026, 10, 8); // Past expiry of Oct 7
    ASSERT_EQ(DONATION_EXPIRED, don.getStatus(futureDate));

    return true;
}

int main() {
    std::cout << "--- Running Food & Donation Module Tests ---\n";
    RUN_TEST(testCookedFoodValidity);
    RUN_TEST(testEligibilityBoundaries);
    RUN_TEST(testFoodFactory);
    RUN_TEST(testDonationStatusAndReservation);
    TEST_REPORT_SUMMARY();
}
