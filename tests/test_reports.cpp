#include <iostream>
#include "test_util.h"
#include "../FoodBank.h"
#include "../ReportGenerator.h"

static bool testReportMetricsAndSDG() {
    Date today(2026, 10, 5);
    FoodBank fb(today);

    Donor d1("D001", "MAIT Canteen", "contact@mait.edu", "Canteen");
    Donor d2("D002", "Grand Hotel", "contact@grand.com", "Hotel");
    RecipientOrganization r1("R001", "Community Shelter", "555-1234", "North");
    RecipientOrganization r2("R002", "Children Care", "555-5678", "South");
    fb.addDonor(d1);
    fb.addDonor(d2);
    fb.addRecipient(r1);
    fb.addRecipient(r2);

    Date prep(2026, 10, 5);
    Date expCooked(2026, 10, 7);
    Date expPackaged(2026, 10, 15);

    std::shared_ptr<FoodItem> f1 = FoodItem::create("Fried Rice", "COOKED", prep, expCooked);
    std::shared_ptr<FoodItem> f2 = FoodItem::create("Canned Fruit", "PACKAGED", prep, expPackaged, "LOT-01");

    fb.addDonation(Donation("DON101", "D001", 60.0, f1, today));
    fb.addDonation(Donation("DON102", "D002", 40.0, f2, today));

    RecipientRequest req1("REQ101", "R001", "COOKED", 40.0, 1, today);
    RecipientRequest req2("REQ102", "R002", "PACKAGED", 20.0, 2, today);
    fb.addRequest(req1);
    fb.addRequest(req2);

    // Match both requests
    MatchResult m1 = fb.matchRequest("REQ101");
    MatchResult m2 = fb.matchRequest("REQ102");
    ASSERT_TRUE(m1.success);
    ASSERT_TRUE(m2.success);

    // Confirm delivery 1, leave delivery 2 in transit
    std::string err;
    ASSERT_TRUE(fb.confirmDelivery(m1.createdDeliveryIds[0], err));

    StockMetrics metrics = ReportGenerator::calculateMetrics(fb);
    ASSERT_NEAR(100.0, metrics.totalDonated, 1e-9);
    ASSERT_NEAR(40.0, metrics.totalRedistributed, 1e-9);
    ASSERT_NEAR(20.0, metrics.totalInTransit, 1e-9);
    ASSERT_NEAR(40.0, metrics.totalAvailable, 1e-9); // 20 kg Cooked + 20 kg Packaged available
    ASSERT_NEAR(0.0, metrics.totalWasted, 1e-9);
    ASSERT_TRUE(metrics.isReconciled);

    // Check summary report text
    std::string summary = ReportGenerator::generateSummary(fb);
    ASSERT_TRUE(summary.find("Stock reconciliation: OK") != std::string::npos);
    ASSERT_TRUE(summary.find("SDG 2: Zero Hunger") != std::string::npos);
    ASSERT_TRUE(summary.find("SDG 12: Responsible Consumption") != std::string::npos);
    ASSERT_TRUE(summary.find("40.00 kg") != std::string::npos);

    // Check donor ranking report
    std::string ranking = ReportGenerator::generateDonorRanking(fb);
    ASSERT_TRUE(ranking.find("Top Donor: MAIT Canteen (60.00 kg)") != std::string::npos);

    // Check category report
    std::string categoryRep = ReportGenerator::generateCategoryReport(fb);
    ASSERT_TRUE(categoryRep.find("COOKED") != std::string::npos);
    ASSERT_TRUE(categoryRep.find("PACKAGED") != std::string::npos);

    return true;
}

int main() {
    std::cout << "--- Running Reports Module Tests ---\n";
    RUN_TEST(testReportMetricsAndSDG);
    TEST_REPORT_SUMMARY();
}
