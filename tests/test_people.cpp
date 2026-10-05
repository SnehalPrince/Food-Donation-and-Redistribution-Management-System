#include <iostream>
#include "test_util.h"
#include "../Person.h"
#include "../Donor.h"
#include "../Recipient.h"

// [ABSTRACTION] and [POLYMORPHISM]
static bool testDonorBasics() {
    Donor d("D001", "MAIT Canteen", "canteen@mait.edu", "Canteen");
    ASSERT_EQ(std::string("D001"), d.getId());
    ASSERT_EQ(std::string("MAIT Canteen"), d.getName());
    ASSERT_EQ(std::string("canteen@mait.edu"), d.getContact());
    ASSERT_EQ(std::string("Canteen"), d.getSource());

    // Polymorphism check via Person pointer
    const Person* p = &d;
    ASSERT_EQ(std::string("D001"), p->getId());
    p->display(); // stdout inspection
    return true;
}

static bool testRecipientBasics() {
    RecipientOrganization r("R001", "Community Shelter", "555-1234", "Sector 14");
    ASSERT_EQ(std::string("R001"), r.getId());
    ASSERT_EQ(std::string("Community Shelter"), r.getName());
    ASSERT_EQ(std::string("555-1234"), r.getContact());
    ASSERT_EQ(std::string("Sector 14"), r.getLocation());

    // Polymorphism check via Person pointer
    const Person* p = &r;
    ASSERT_EQ(std::string("R001"), p->getId());
    p->display();
    return true;
}

static bool testPolymorphicArray() {
    Donor d("D002", "Sunrise Bakery", "contact@bakery.com", "Bakery");
    RecipientOrganization r("R002", "Hope Orphanage", "hope@ngo.org", "Block C");

    const Person* people[2];
    people[0] = &d;
    people[1] = &r;

    ASSERT_EQ(std::string("D002"), people[0]->getId());
    ASSERT_EQ(std::string("R002"), people[1]->getId());
    for (int i = 0; i < 2; ++i) {
        people[i]->display();
    }
    return true;
}

int main() {
    std::cout << "--- Running People Module Tests ---\n";
    RUN_TEST(testDonorBasics);
    RUN_TEST(testRecipientBasics);
    RUN_TEST(testPolymorphicArray);
    TEST_REPORT_SUMMARY();
}
