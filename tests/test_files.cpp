#include <iostream>
#include <fstream>
#include "test_util.h"
#include "../FoodBank.h"
#include "../FileManager.h"

static bool testFileRoundTrip() {
    Date today(2026, 10, 5);
    FoodBank fb(today);

    // Register donor & recipient
    Donor d("D001", "MAIT Canteen", "canteen@mait.edu", "Canteen");
    RecipientOrganization r("R001", "Community Shelter", "555-0199", "Sector 14");
    ASSERT_TRUE(fb.addDonor(d));
    ASSERT_TRUE(fb.addRecipient(r));

    // Register donation
    Date prep(2026, 10, 5);
    Date exp(2026, 10, 7);
    std::shared_ptr<FoodItem> food = FoodItem::create("Dal & Rice", "COOKED", prep, exp);
    Donation don("DON101", "D001", 50.0, food, today);
    ASSERT_TRUE(fb.addDonation(don));

    // Register request
    RecipientRequest req("REQ101", "R001", "COOKED", 30.0, 1, today);
    ASSERT_TRUE(fb.addRequest(req));

    // Perform matching (creates SCHEDULED delivery)
    MatchResult m = fb.matchRequest("REQ101");
    ASSERT_TRUE(m.success);
    ASSERT_NEAR(30.0, m.allocatedQuantity, 1e-9);

    // Confirm delivery
    std::string err;
    ASSERT_TRUE(fb.confirmDelivery("DEL001", err));

    // Save to test directory
    std::string testDir = "tests/temp_test_data";
    #if defined(_WIN32)
      system("if not exist tests\\temp_test_data mkdir tests\\temp_test_data");
    #else
      system("mkdir -p tests/temp_test_data");
    #endif

    ASSERT_TRUE(FileManager::saveAll(fb, testDir));

    // Load into a brand new FoodBank instance
    FoodBank fbLoaded(today);
    ASSERT_TRUE(FileManager::loadAll(fbLoaded, testDir));

    ASSERT_EQ(size_t(1), fbLoaded.getDonors().size());
    ASSERT_EQ(std::string("MAIT Canteen"), fbLoaded.getDonors()[0].getName());
    ASSERT_EQ(size_t(1), fbLoaded.getRecipients().size());
    ASSERT_EQ(std::string("Community Shelter"), fbLoaded.getRecipients()[0].getName());
    ASSERT_EQ(size_t(1), fbLoaded.getDonations().size());
    ASSERT_NEAR(50.0, fbLoaded.getDonations()[0].getTotalQuantity(), 1e-9);
    ASSERT_NEAR(20.0, fbLoaded.getDonations()[0].getAvailableQuantity(), 1e-9); // 30 delivered, 20 remaining!
    ASSERT_EQ(size_t(1), fbLoaded.getRequests().size());
    ASSERT_EQ(REQUEST_FULFILLED, fbLoaded.getRequests()[0].getStatus());
    ASSERT_EQ(size_t(1), fbLoaded.getDeliveries().size());
    ASSERT_EQ(DELIVERY_DELIVERED, fbLoaded.getDeliveries()[0].getStatus());

    return true;
}

static bool testMissingFilesHandling() {
    FoodBank fb(Date(2026, 10, 5));
    // Loading from non-existent directory should not crash
    ASSERT_TRUE(FileManager::loadAll(fb, "tests/non_existent_dir"));
    ASSERT_EQ(size_t(0), fb.getDonors().size());
    ASSERT_EQ(size_t(0), fb.getRecipients().size());
    return true;
}

static bool testMalformedLinesSkipped() {
    std::string badDir = "tests/temp_malformed_data";
    #if defined(_WIN32)
      system("if not exist tests\\temp_malformed_data mkdir tests\\temp_malformed_data");
    #else
      system("mkdir -p tests/temp_malformed_data");
    #endif

    // Write a file with one good line and two bad lines
    std::ofstream out("tests/temp_malformed_data/donors.txt");
    out << "D001|Good Donor|1234|Restaurant\n";
    out << "MalformedLineWithoutPipes\n";
    out << "D002|Another Good|5678|Canteen\n";
    out.close();

    FoodBank fb(Date(2026, 10, 5));
    ASSERT_TRUE(FileManager::loadDonors(fb, "tests/temp_malformed_data/donors.txt"));
    ASSERT_EQ(size_t(2), fb.getDonors().size());
    ASSERT_EQ(std::string("D001"), fb.getDonors()[0].getId());
    ASSERT_EQ(std::string("D002"), fb.getDonors()[1].getId());

    return true;
}

int main() {
    std::cout << "--- Running Persistence Module Tests ---\n";
    RUN_TEST(testFileRoundTrip);
    RUN_TEST(testMissingFilesHandling);
    RUN_TEST(testMalformedLinesSkipped);
    TEST_REPORT_SUMMARY();
}
