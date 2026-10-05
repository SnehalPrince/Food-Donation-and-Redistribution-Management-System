#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>

#include "Date.h"
#include "InputHelper.h"
#include "FoodBank.h"
#include "ReportGenerator.h"
#include "FileManager.h"

static Date getSystemDate() {
    std::time_t t = std::time(NULL);
    std::tm* now = std::localtime(&t);
    if (!now) return Date(2026, 10, 5);
    return Date(now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
}

static void printMenu() {
    std::cout << "\n=============================================\n";
    std::cout << "   FOOD DONATION & REDISTRIBUTION SYSTEM     \n";
    std::cout << "=============================================\n";
    std::cout << "1. Register Donor\n";
    std::cout << "2. Register Recipient\n";
    std::cout << "3. Add Food Donation\n";
    std::cout << "4. View Donations\n";
    std::cout << "5. Add Recipient Requirement\n";
    std::cout << "6. View Pending Requests\n";
    std::cout << "7. Match Donation\n";
    std::cout << "8. Record Delivery\n";
    std::cout << "9. Search Records\n";
    std::cout << "10. Show Expiring Donations\n";
    std::cout << "11. Generate Summary Report\n";
    std::cout << "12. Save Data\n";
    std::cout << "13. Exit\n";
}

// 1. Register Donor
static void handleRegisterDonor(FoodBank& fb, bool& unsaved) {
    std::cout << "\n--- Register Donor ---\n";
    std::string suggestedId = fb.suggestNextDonorId();
    std::string id, name, contact, source;
    if (!InputHelper::readId("Enter Donor ID", suggestedId, id)) return;
    if (!InputHelper::readString("Enter Donor Name: ", name)) return;
    if (!InputHelper::readString("Enter Contact Info: ", contact)) return;
    if (!InputHelper::readString("Enter Source / Type (e.g. Restaurant, Canteen, Household): ", source)) return;

    Donor donor(id, name, contact, source);
    if (fb.addDonor(donor)) {
        std::cout << "Success: Donor " << id << " registered successfully.\n";
        unsaved = true;
    } else {
        std::cout << "Error: Donor ID " << id << " already exists!\n";
    }
}

// 2. Register Recipient
static void handleRegisterRecipient(FoodBank& fb, bool& unsaved) {
    std::cout << "\n--- Register Recipient Organization ---\n";
    std::string suggestedId = fb.suggestNextRecipientId();
    std::string id, name, contact, location;
    if (!InputHelper::readId("Enter Recipient ID", suggestedId, id)) return;
    if (!InputHelper::readString("Enter Organization Name: ", name)) return;
    if (!InputHelper::readString("Enter Contact Info: ", contact)) return;
    if (!InputHelper::readString("Enter Location: ", location)) return;

    RecipientOrganization rec(id, name, contact, location);
    if (fb.addRecipient(rec)) {
        std::cout << "Success: Recipient Organization " << id << " registered successfully.\n";
        unsaved = true;
    } else {
        std::cout << "Error: Recipient ID " << id << " already exists!\n";
    }
}

// 3. Add Food Donation
static void handleAddDonation(FoodBank& fb, bool& unsaved) {
    std::cout << "\n--- Add Food Donation ---\n";
    if (fb.getDonors().empty()) {
        std::cout << "Error: No donors registered yet. Please register a donor first.\n";
        return;
    }

    std::string suggestedId = fb.suggestNextDonationId();
    std::string donId, donorId, foodName, catChoiceStr;
    if (!InputHelper::readId("Enter Donation ID", suggestedId, donId)) return;
    if (!InputHelper::readString("Enter Donor ID: ", donorId)) return;
    if (!fb.findDonor(donorId)) {
        std::cout << "Error: Donor ID '" << donorId << "' not found.\n";
        return;
    }

    if (!InputHelper::readString("Enter Food Item Name: ", foodName)) return;

    std::cout << "Select Food Category:\n";
    std::cout << "  1. Cooked Food (COOKED)\n";
    std::cout << "  2. Packaged Food (PACKAGED)\n";
    std::cout << "  3. Bakery (BAKERY)\n";
    std::cout << "  4. Fruits & Vegetables (PRODUCE)\n";
    int catChoice = 1;
    if (!InputHelper::readInt("Enter choice (1-4): ", catChoice, 1, 4)) return;

    std::string category = "COOKED";
    if (catChoice == 2) category = "PACKAGED";
    else if (catChoice == 3) category = "BAKERY";
    else if (catChoice == 4) category = "PRODUCE";

    double quantity = 0.0;
    if (!InputHelper::readDouble("Enter Quantity (kg): ", quantity, 0.01, 100000.0)) return;

    Date prepDate = fb.getToday();
    Date expiryDate = fb.getToday();
    std::string batch = "";

    if (category == "COOKED" || category == "BAKERY") {
        if (!InputHelper::readDate("Enter Preparation Date", fb.getToday(), prepDate)) return;
        if (fb.getToday() < prepDate) {
            std::cout << "Error: Preparation date cannot be in the future.\n";
            return;
        }
        if (!InputHelper::readDateRequired("Enter Expiry Date", expiryDate)) return;
    } else {
        InputHelper::readDate("Enter Preparation / Packaging Date", fb.getToday(), prepDate);
        if (!InputHelper::readDateRequired("Enter Expiry Date", expiryDate)) return;
        InputHelper::readString("Enter Batch/Lot Number (optional, press Enter to skip): ", batch, true);
    }

    if (expiryDate < fb.getToday()) {
        std::cout << "Error: Cannot register donation whose expiry date (" << expiryDate.toString()
                  << ") is already in the past!\n";
        return;
    }

    std::shared_ptr<FoodItem> food = FoodItem::create(foodName, category, prepDate, expiryDate, batch);
    if (!food->validate()) {
        std::cout << "Error: Food item validation failed (e.g. prepared food validity exceeds allowed shelf-life).\n";
        return;
    }

    Donation don(donId, donorId, quantity, food, fb.getToday());
    if (fb.addDonation(don)) {
        std::cout << "Success: Donation " << donId << " registered (" << quantity << " kg of " << foodName << ").\n";
        unsaved = true;
    } else {
        std::cout << "Error: Failed to add donation. Check ID uniqueness or validity.\n";
    }
}

// 4. View Donations
static void handleViewDonations(const FoodBank& fb) {
    std::cout << "\n--- All Registered Donations ---\n";
    const std::vector<Donation>& donations = fb.getDonations();
    if (donations.empty()) {
        std::cout << "No donations registered yet.\n";
        return;
    }
    for (size_t i = 0; i < donations.size(); ++i) {
        donations[i].display(fb.getToday());
    }
}

// 5. Add Recipient Requirement
static void handleAddRequest(FoodBank& fb, bool& unsaved) {
    std::cout << "\n--- Add Recipient Requirement ---\n";
    if (fb.getRecipients().empty()) {
        std::cout << "Error: No recipients registered yet. Register a recipient first.\n";
        return;
    }

    std::string suggestedId = fb.suggestNextRequestId();
    std::string reqId, recId;
    if (!InputHelper::readId("Enter Request ID", suggestedId, reqId)) return;
    if (!InputHelper::readString("Enter Recipient ID: ", recId)) return;
    if (!fb.findRecipient(recId)) {
        std::cout << "Error: Recipient ID '" << recId << "' not found.\n";
        return;
    }

    std::cout << "Select Required Food Category:\n";
    std::cout << "  1. Cooked Food (COOKED)\n";
    std::cout << "  2. Packaged Food (PACKAGED)\n";
    std::cout << "  3. Bakery (BAKERY)\n";
    std::cout << "  4. Fruits & Vegetables (PRODUCE)\n";
    int catChoice = 1;
    if (!InputHelper::readInt("Enter choice (1-4): ", catChoice, 1, 4)) return;

    std::string category = "COOKED";
    if (catChoice == 2) category = "PACKAGED";
    else if (catChoice == 3) category = "BAKERY";
    else if (catChoice == 4) category = "PRODUCE";

    double quantity = 0.0;
    if (!InputHelper::readDouble("Enter Required Quantity (kg): ", quantity, 0.01, 100000.0)) return;

    int priority = 2;
    std::cout << "Select Priority:\n  1. High\n  2. Medium\n  3. Low\n";
    if (!InputHelper::readInt("Enter priority (1-3): ", priority, 1, 3)) return;

    RecipientRequest req(reqId, recId, category, quantity, priority, fb.getToday());
    if (fb.addRequest(req)) {
        std::cout << "Success: Request " << reqId << " registered successfully.\n";
        unsaved = true;
    } else {
        std::cout << "Error: Request ID " << reqId << " already exists!\n";
    }
}

// 6. View Pending Requests
static void handleViewPendingRequests(FoodBank& fb, bool& unsaved) {
    std::cout << "\n" << ReportGenerator::generatePendingRequestsReport(fb);
    std::cout << "Sub-menu options:\n";
    std::cout << "  1. Return to main menu\n";
    std::cout << "  2. Cancel a pending request\n";
    int choice = 1;
    if (!InputHelper::readInt("Enter choice (1-2): ", choice, 1, 2)) return;
    if (choice == 2) {
        std::string reqId;
        if (!InputHelper::readString("Enter Request ID to cancel: ", reqId)) return;
        std::string err;
        if (fb.cancelRequest(reqId, err)) {
            std::cout << "Success: Request " << reqId << " has been cancelled.\n";
            unsaved = true;
        } else {
            std::cout << "Error: " << err << "\n";
        }
    }
}

// 7. Match Donation
static void handleMatchDonation(FoodBank& fb, bool& unsaved) {
    std::cout << "\n--- Match Donation ---\n";
    std::cout << "1. Match next pending request (queue order)\n";
    std::cout << "2. Match a chosen request by ID\n";
    std::cout << "3. Auto-match all pending requests\n";
    int choice = 1;
    if (!InputHelper::readInt("Enter choice (1-3): ", choice, 1, 3)) return;

    if (choice == 1) {
        MatchResult res = fb.matchNextPendingRequest();
        std::cout << "\nResult: " << res.message << "\n";
        if (res.success) unsaved = true;
    } else if (choice == 2) {
        std::string reqId;
        if (!InputHelper::readString("Enter Request ID: ", reqId)) return;
        MatchResult res = fb.matchRequest(reqId);
        std::cout << "\nResult: " << res.message << "\n";
        if (res.success) unsaved = true;
    } else if (choice == 3) {
        int count = fb.autoMatchAllPending();
        std::cout << "\nAuto-match completed. Processed " << count << " match event(s).\n";
        if (count > 0) unsaved = true;
    }
}

// 8. Record Delivery
static void handleRecordDelivery(FoodBank& fb, bool& unsaved) {
    std::cout << "\n--- Record Delivery ---\n";
    const std::vector<Delivery>& deliveries = fb.getDeliveries();
    std::vector<const Delivery*> scheduled;
    for (size_t i = 0; i < deliveries.size(); ++i) {
        if (deliveries[i].getStatus() == DELIVERY_SCHEDULED) {
            scheduled.push_back(&deliveries[i]);
        }
    }

    if (scheduled.empty()) {
        std::cout << "No SCHEDULED deliveries awaiting confirmation.\n";
        return;
    }

    std::cout << "Scheduled Deliveries in Transit:\n";
    for (size_t i = 0; i < scheduled.size(); ++i) {
        scheduled[i]->display();
    }

    std::string delId;
    if (!InputHelper::readString("\nEnter Delivery ID to update: ", delId)) return;

    std::cout << "Select Action:\n";
    std::cout << "  1. Mark as DELIVERED (confirm receipt today)\n";
    std::cout << "  2. Mark as CANCELLED (reverse reservation)\n";
    int act = 1;
    if (!InputHelper::readInt("Enter choice (1-2): ", act, 1, 2)) return;

    std::string err;
    if (act == 1) {
        if (fb.confirmDelivery(delId, err)) {
            std::cout << "Success: Delivery " << delId << " confirmed as DELIVERED.\n";
            unsaved = true;
        } else {
            std::cout << "Error: " << err << "\n";
        }
    } else {
        if (fb.cancelDelivery(delId, err)) {
            std::cout << "Success: Delivery " << delId << " CANCELLED and reserved stock restored.\n";
            unsaved = true;
        } else {
            std::cout << "Error: " << err << "\n";
        }
    }
}

// 9. Search Records
static void handleSearch(const FoodBank& fb) {
    std::cout << "\n--- Search & Browse Records ---\n";
    std::cout << "1. Search Donation by Donation ID\n";
    std::cout << "2. Search Donations by Donor ID\n";
    std::cout << "3. Filter Donations by Category\n";
    std::cout << "4. Search Recipient by ID\n";
    std::cout << "5. Search Recipient by Name\n";
    std::cout << "6. Search Request by Request ID\n";
    std::cout << "7. Search Requests by Recipient ID\n";
    std::cout << "8. Search Requests by Status\n";
    std::cout << "9. View All Donors\n";
    std::cout << "10. View All Recipients\n";
    std::cout << "11. View All Deliveries\n";
    int choice = 1;
    if (!InputHelper::readInt("Enter choice (1-11): ", choice, 1, 11)) return;

    if (choice == 1) {
        std::string id;
        InputHelper::readString("Enter Donation ID: ", id);
        const Donation* d = fb.findDonation(id);
        if (d) d->display(fb.getToday());
        else std::cout << "Donation not found.\n";
    } else if (choice == 2) {
        std::string id;
        InputHelper::readString("Enter Donor ID: ", id);
        bool found = false;
        const std::vector<Donation>& dons = fb.getDonations();
        for (size_t i = 0; i < dons.size(); ++i) {
            if (dons[i].getDonorId() == id) {
                dons[i].display(fb.getToday());
                found = true;
            }
        }
        if (!found) std::cout << "No donations found for donor " << id << ".\n";
    } else if (choice == 3) {
        std::string cat;
        InputHelper::readString("Enter Category (COOKED/PACKAGED/BAKERY/PRODUCE): ", cat);
        const std::vector<Donation>& dons = fb.getDonations();
        bool found = false;
        for (size_t i = 0; i < dons.size(); ++i) {
            if (dons[i].getFoodItem() && dons[i].getFoodItem()->getCategory() == cat) {
                dons[i].display(fb.getToday());
                found = true;
            }
        }
        if (!found) std::cout << "No donations found in category " << cat << ".\n";
    } else if (choice == 4) {
        std::string id;
        InputHelper::readString("Enter Recipient ID: ", id);
        const RecipientOrganization* r = fb.findRecipient(id);
        if (r) r->display();
        else std::cout << "Recipient not found.\n";
    } else if (choice == 5) {
        std::string name;
        InputHelper::readString("Enter Recipient Name keyword: ", name);
        const std::vector<RecipientOrganization>& recs = fb.getRecipients();
        bool found = false;
        for (size_t i = 0; i < recs.size(); ++i) {
            if (recs[i].getName().find(name) != std::string::npos) {
                recs[i].display();
                found = true;
            }
        }
        if (!found) std::cout << "No recipients matching '" << name << "'.\n";
    } else if (choice == 6) {
        std::string id;
        InputHelper::readString("Enter Request ID: ", id);
        const RecipientRequest* r = fb.findRequest(id);
        if (r) r->display();
        else std::cout << "Request not found.\n";
    } else if (choice == 7) {
        std::string id;
        InputHelper::readString("Enter Recipient ID: ", id);
        const std::vector<RecipientRequest>& reqs = fb.getRequests();
        bool found = false;
        for (size_t i = 0; i < reqs.size(); ++i) {
            if (reqs[i].getRecipientId() == id) {
                reqs[i].display();
                found = true;
            }
        }
        if (!found) std::cout << "No requests found for recipient " << id << ".\n";
    } else if (choice == 8) {
        std::string st;
        InputHelper::readString("Enter Status (PENDING/PARTIALLY_FULFILLED/FULFILLED/CANCELLED): ", st);
        const std::vector<RecipientRequest>& reqs = fb.getRequests();
        bool found = false;
        for (size_t i = 0; i < reqs.size(); ++i) {
            if (requestStatusToString(reqs[i].getStatus()) == st) {
                reqs[i].display();
                found = true;
            }
        }
        if (!found) std::cout << "No requests found with status " << st << ".\n";
    } else if (choice == 9) {
        const std::vector<Donor>& donors = fb.getDonors();
        if (donors.empty()) std::cout << "No donors found.\n";
        for (size_t i = 0; i < donors.size(); ++i) donors[i].display();
    } else if (choice == 10) {
        const std::vector<RecipientOrganization>& recs = fb.getRecipients();
        if (recs.empty()) std::cout << "No recipients found.\n";
        for (size_t i = 0; i < recs.size(); ++i) recs[i].display();
    } else if (choice == 11) {
        const std::vector<Delivery>& dels = fb.getDeliveries();
        if (dels.empty()) std::cout << "No deliveries recorded.\n";
        for (size_t i = 0; i < dels.size(); ++i) dels[i].display();
    }
}

// 10. Show Expiring Donations
static void handleShowExpiring(const FoodBank& fb) {
    int days = 3;
    InputHelper::readInt("Enter days threshold for expiring-soon [default 3]: ", days, 1, 365);
    std::cout << "\n" << ReportGenerator::generateExpiringSoonReport(fb, days);
}

// 11. Reports Sub-Menu
static void handleReports(const FoodBank& fb) {
    std::cout << "\n--- Reports Sub-Menu ---\n";
    std::cout << "1. Full Summary & SDG Impact Report\n";
    std::cout << "2. Donor Ranking Report (Top Donors)\n";
    std::cout << "3. Category-Wise Distribution Report\n";
    std::cout << "4. Pending Requests Queue Report\n";
    std::cout << "5. Expiring-Soon Donations Report\n";
    int choice = 1;
    if (!InputHelper::readInt("Enter choice (1-5): ", choice, 1, 5)) return;

    if (choice == 1) {
        std::cout << "\n" << ReportGenerator::generateSummary(fb);
    } else if (choice == 2) {
        std::cout << "\n" << ReportGenerator::generateDonorRanking(fb);
    } else if (choice == 3) {
        std::cout << "\n" << ReportGenerator::generateCategoryReport(fb);
    } else if (choice == 4) {
        std::cout << "\n" << ReportGenerator::generatePendingRequestsReport(fb);
    } else if (choice == 5) {
        std::cout << "\n" << ReportGenerator::generateExpiringSoonReport(fb, 3);
    }
}

int main(int argc, char* argv[]) {
    std::string dataDir = "data";
    Date today = getSystemDate();

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--today" && i + 1 < argc) {
            Date parsed;
            if (Date::parse(argv[++i], parsed)) {
                today = parsed;
            }
        } else if (arg == "--data-dir" && i + 1 < argc) {
            dataDir = argv[++i];
        }
    }

    FoodBank fb(today);
    FileManager::loadAll(fb, dataDir);

    std::cout << "====================================================\n";
    std::cout << "Food Donation & Redistribution Management System\n";
    std::cout << "Today: " << fb.getToday().toString() << " | Data Directory: " << dataDir << "\n";
    std::cout << "Loaded: " << fb.getDonors().size() << " donors, "
              << fb.getRecipients().size() << " recipients, "
              << fb.getDonations().size() << " donations, "
              << fb.getRequests().size() << " requests, "
              << fb.getDeliveries().size() << " deliveries.\n";
    std::cout << "====================================================\n";

    bool unsavedChanges = false;
    bool running = true;

    while (running && !InputHelper::isEof()) {
        printMenu();
        int choice = 0;
        if (!InputHelper::readInt("Enter choice (1-13): ", choice, 1, 13)) {
            if (InputHelper::isEof()) break;
            continue;
        }

        switch (choice) {
            case 1: handleRegisterDonor(fb, unsavedChanges); break;
            case 2: handleRegisterRecipient(fb, unsavedChanges); break;
            case 3: handleAddDonation(fb, unsavedChanges); break;
            case 4: handleViewDonations(fb); break;
            case 5: handleAddRequest(fb, unsavedChanges); break;
            case 6: handleViewPendingRequests(fb, unsavedChanges); break;
            case 7: handleMatchDonation(fb, unsavedChanges); break;
            case 8: handleRecordDelivery(fb, unsavedChanges); break;
            case 9: handleSearch(fb); break;
            case 10: handleShowExpiring(fb); break;
            case 11: handleReports(fb); break;
            case 12: {
                if (FileManager::saveAll(fb, dataDir)) {
                    std::cout << "Success: All data saved to '" << dataDir << "' successfully.\n";
                    unsavedChanges = false;
                } else {
                    std::cout << "Error: Failed to save data.\n";
                }
                break;
            }
            case 13: {
                running = false;
                break;
            }
            default:
                std::cout << "Invalid choice. Please select 1-13.\n";
                break;
        }
    }

    if (unsavedChanges && !InputHelper::isEof()) {
        if (InputHelper::readConfirmation("You have unsaved changes. Save before exit?", true)) {
            FileManager::saveAll(fb, dataDir);
            std::cout << "Data saved successfully.\n";
        }
    }

    std::cout << "Exiting Food Donation & Redistribution Management System. Goodbye!\n";
    return 0;
}
