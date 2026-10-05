#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cmath>
#include "test_util.h"
#include "../FoodBank.h"
#include "../ReportGenerator.h"

// Scenario: 50 kg donated -> 30 kg requested -> 30 kg matched -> 20 kg remains
static bool testExactMatchingScenario() {
    Date today(2026, 10, 5);
    FoodBank fb(today);

    Donor donor("D001", "MAIT Canteen", "canteen@mait.edu", "Canteen");
    RecipientOrganization recipient("R001", "Community Shelter", "555-1234", "Sector 14");
    ASSERT_TRUE(fb.addDonor(donor));
    ASSERT_TRUE(fb.addRecipient(recipient));

    Date prep(2026, 10, 5);
    Date exp(2026, 10, 7);
    std::shared_ptr<FoodItem> food = FoodItem::create("Cooked Rice", "COOKED", prep, exp);
    Donation don("DON101", "D001", 50.0, food, today);
    ASSERT_TRUE(fb.addDonation(don));

    RecipientRequest req("REQ101", "R001", "COOKED", 30.0, 1, today);
    ASSERT_TRUE(fb.addRequest(req));

    // Perform matching
    MatchResult match = fb.matchRequest("REQ101");
    ASSERT_TRUE(match.success);
    ASSERT_NEAR(30.0, match.allocatedQuantity, 1e-9);
    ASSERT_NEAR(0.0, match.remainingNeed, 1e-9);

    // Stock check
    const Donation* checkDon = fb.findDonation("DON101");
    ASSERT_TRUE(checkDon != NULL);
    ASSERT_NEAR(20.0, checkDon->getAvailableQuantity(), 1e-9);

    // Delivery check
    ASSERT_EQ(size_t(1), match.createdDeliveryIds.size());
    std::string delId = match.createdDeliveryIds[0];
    const Delivery* del = fb.findDelivery(delId);
    ASSERT_TRUE(del != NULL);
    ASSERT_EQ(DELIVERY_SCHEDULED, del->getStatus());
    ASSERT_NEAR(30.0, del->getQuantity(), 1e-9);

    // Record delivery
    std::string err;
    ASSERT_TRUE(fb.confirmDelivery(delId, err));
    ASSERT_EQ(DELIVERY_DELIVERED, fb.findDelivery(delId)->getStatus());

    const RecipientRequest* checkReq = fb.findRequest("REQ101");
    ASSERT_TRUE(checkReq != NULL);
    ASSERT_EQ(REQUEST_FULFILLED, checkReq->getStatus());
    ASSERT_NEAR(30.0, checkReq->getDeliveredQuantity(), 1e-9);

    return true;
}

static bool testEarliestExpiryMatching() {
    Date today(2026, 10, 5);
    FoodBank fb(today);

    Donor donor("D001", "City Caterers", "contact@city.com", "Caterer");
    RecipientOrganization recipient("R001", "Night Shelter", "555-9876", "Downtown");
    fb.addDonor(donor);
    fb.addRecipient(recipient);

    Date prep(2026, 10, 5);
    Date expLater(2026, 10, 7);
    Date expSooner(2026, 10, 6);

    std::shared_ptr<FoodItem> f1 = FoodItem::create("Stew", "COOKED", prep, expLater);
    Donation d1("DON101", "D001", 20.0, f1, today);
    fb.addDonation(d1);

    std::shared_ptr<FoodItem> f2 = FoodItem::create("Stew", "COOKED", prep, expSooner);
    Donation d2("DON102", "D001", 20.0, f2, today);
    fb.addDonation(d2);

    // Request 15 kg
    RecipientRequest req("REQ101", "R001", "COOKED", 15.0, 1, today);
    fb.addRequest(req);

    MatchResult match = fb.matchRequest("REQ101");
    ASSERT_TRUE(match.success);
    ASSERT_NEAR(15.0, match.allocatedQuantity, 1e-9);

    // Must have allocated from DON102 (expires sooner)
    const Donation* don1 = fb.findDonation("DON101");
    const Donation* don2 = fb.findDonation("DON102");
    ASSERT_NEAR(20.0, don1->getAvailableQuantity(), 1e-9);
    ASSERT_NEAR(5.0, don2->getAvailableQuantity(), 1e-9);

    return true;
}

static bool testDeliveryCancellationReversal() {
    Date today(2026, 10, 5);
    FoodBank fb(today);

    Donor donor("D001", "Bakery", "b@b.com", "Bakery");
    RecipientOrganization recipient("R001", "Youth Club", "y@y.com", "West");
    fb.addDonor(donor);
    fb.addRecipient(recipient);

    Date prep(2026, 10, 5);
    Date exp(2026, 10, 8);
    std::shared_ptr<FoodItem> food = FoodItem::create("Bread", "BAKERY", prep, exp);
    Donation don("DON101", "D001", 40.0, food, today);
    fb.addDonation(don);

    RecipientRequest req("REQ101", "R001", "BAKERY", 25.0, 2, today);
    fb.addRequest(req);

    MatchResult match = fb.matchRequest("REQ101");
    ASSERT_TRUE(match.success);
    ASSERT_NEAR(15.0, fb.findDonation("DON101")->getAvailableQuantity(), 1e-9);

    // Cancel delivery
    std::string err;
    std::string delId = match.createdDeliveryIds[0];
    ASSERT_TRUE(fb.cancelDelivery(delId, err));

    // Reservation must be completely restored
    ASSERT_NEAR(40.0, fb.findDonation("DON101")->getAvailableQuantity(), 1e-9);
    ASSERT_NEAR(25.0, fb.findRequest("REQ101")->getRemainingNeed(), 1e-9);
    ASSERT_EQ(DELIVERY_CANCELLED, fb.findDelivery(delId)->getStatus());

    return true;
}

static bool testExpiredFoodDeliveryBlocked() {
    Date today(2026, 10, 5);
    FoodBank fb(today);

    Donor donor("D001", "Hotel", "h@h.com", "Hotel");
    RecipientOrganization recipient("R001", "Shelter", "s@s.com", "East");
    fb.addDonor(donor);
    fb.addRecipient(recipient);

    Date prep(2026, 10, 5);
    Date exp(2026, 10, 6);
    std::shared_ptr<FoodItem> food = FoodItem::create("Soup", "COOKED", prep, exp);
    Donation don("DON101", "D001", 10.0, food, today);
    fb.addDonation(don);

    RecipientRequest req("REQ101", "R001", "COOKED", 10.0, 1, today);
    fb.addRequest(req);

    MatchResult match = fb.matchRequest("REQ101");
    ASSERT_TRUE(match.success);
    std::string delId = match.createdDeliveryIds[0];

    // Fast-forward date to Oct 7 (past expiry Oct 6)
    fb.setToday(Date(2026, 10, 7));

    // Confirm delivery should be blocked
    std::string err;
    ASSERT_FALSE(fb.confirmDelivery(delId, err));
    ASSERT_EQ(DELIVERY_SCHEDULED, fb.findDelivery(delId)->getStatus());

    // But cancellation should be allowed
    ASSERT_TRUE(fb.cancelDelivery(delId, err));
    ASSERT_EQ(DELIVERY_CANCELLED, fb.findDelivery(delId)->getStatus());

    return true;
}

// 200 random operations stress-testing the stock reconciliation invariant:
// donated = redistributed + in_transit + available + wasted
static bool testStockReconciliationInvariantStress() {
    Date today(2026, 10, 5);
    FoodBank fb(today);

    // Seed donors & recipients
    for (int i = 1; i <= 5; ++i) {
        std::ostringstream od, orc;
        od << "D" << i;
        orc << "R" << i;
        fb.addDonor(Donor(od.str(), "Donor" + od.str(), "123", "Source"));
        fb.addRecipient(RecipientOrganization(orc.str(), "Recipient" + orc.str(), "456", "Loc"));
    }

    std::srand(12345);
    const char* cats[] = { "COOKED", "PACKAGED", "BAKERY", "PRODUCE" };

    int opCount = 200;
    int donIdCounter = 1;
    int reqIdCounter = 1;

    for (int step = 0; step < opCount; ++step) {
        int action = std::rand() % 4;
        if (action == 0) {
            // Add random donation
            std::ostringstream donId, donrId;
            donId << "DON" << (donIdCounter++);
            donrId << "D" << (1 + (std::rand() % 5));
            int catIdx = std::rand() % 4;
            double qty = 10.0 + (std::rand() % 90);
            int validDays = (catIdx == 0 ? 2 : (catIdx == 2 ? 3 : 10));
            Date exp(2026, 10, 5 + validDays);
            std::shared_ptr<FoodItem> food = FoodItem::create("Food" + donId.str(), cats[catIdx], today, exp);
            fb.addDonation(Donation(donId.str(), donrId.str(), qty, food, today));
        } else if (action == 1) {
            // Add random request
            std::ostringstream reqId, recId;
            reqId << "REQ" << (reqIdCounter++);
            recId << "R" << (1 + (std::rand() % 5));
            int catIdx = std::rand() % 4;
            double qty = 5.0 + (std::rand() % 50);
            int prio = 1 + (std::rand() % 3);
            fb.addRequest(RecipientRequest(reqId.str(), recId.str(), cats[catIdx], qty, prio, today));
        } else if (action == 2) {
            // Match next or random request
            fb.matchNextPendingRequest();
        } else if (action == 3) {
            // Deliver or cancel a scheduled delivery
            const std::vector<Delivery>& dels = fb.getDeliveries();
            for (size_t i = 0; i < dels.size(); ++i) {
                if (dels[i].getStatus() == DELIVERY_SCHEDULED) {
                    std::string err;
                    if (std::rand() % 2 == 0) {
                        fb.confirmDelivery(dels[i].getDeliveryId(), err);
                    } else {
                        fb.cancelDelivery(dels[i].getDeliveryId(), err);
                    }
                    break;
                }
            }
        }

        // Verify invariant after every single operation
        StockMetrics m = ReportGenerator::calculateMetrics(fb);
        ASSERT_TRUE(m.isReconciled);
    }

    return true;
}

int main() {
    std::cout << "--- Running FoodBank Core Module Tests ---\n";
    RUN_TEST(testExactMatchingScenario);
    RUN_TEST(testEarliestExpiryMatching);
    RUN_TEST(testDeliveryCancellationReversal);
    RUN_TEST(testExpiredFoodDeliveryBlocked);
    RUN_TEST(testStockReconciliationInvariantStress);
    TEST_REPORT_SUMMARY();
}
