#include "FileManager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>

// [FILE-IO]
static std::vector<std::string> splitLine(const std::string& line, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

static std::string joinPath(const std::string& dir, const std::string& file) {
    if (dir.empty()) return file;
    char last = dir[dir.length() - 1];
    if (last == '/' || last == '\\') return dir + file;
    return dir + "/" + file;
}

bool FileManager::saveDonors(const std::vector<Donor>& donors, const std::string& filepath) {
    std::ofstream out(filepath.c_str());
    if (!out.is_open()) return false;
    for (size_t i = 0; i < donors.size(); ++i) {
        const Donor& d = donors[i];
        out << d.getId() << "|"
            << d.getName() << "|"
            << d.getContact() << "|"
            << d.getSource() << "\n";
    }
    return true;
}

bool FileManager::loadDonors(FoodBank& fb, const std::string& filepath) {
    std::ifstream in(filepath.c_str());
    if (!in.is_open()) return true; // Missing file means empty data (no crash)
    std::string line;
    int lineNum = 0;
    while (std::getline(in, line)) {
        lineNum++;
        if (line.empty()) continue;
        std::vector<std::string> t = splitLine(line, '|');
        if (t.size() < 4) {
            std::cerr << "Warning [" << filepath << ":" << lineNum << "]: Malformed donor line skipped.\n";
            continue;
        }
        Donor d(t[0], t[1], t[2], t[3]);
        if (!fb.addDonor(d)) {
            std::cerr << "Warning [" << filepath << ":" << lineNum << "]: Duplicate donor ID '" << t[0] << "' skipped.\n";
        }
    }
    return true;
}

bool FileManager::saveRecipients(const std::vector<RecipientOrganization>& recipients, const std::string& filepath) {
    std::ofstream out(filepath.c_str());
    if (!out.is_open()) return false;
    for (size_t i = 0; i < recipients.size(); ++i) {
        const RecipientOrganization& r = recipients[i];
        out << r.getId() << "|"
            << r.getName() << "|"
            << r.getContact() << "|"
            << r.getLocation() << "\n";
    }
    return true;
}

bool FileManager::loadRecipients(FoodBank& fb, const std::string& filepath) {
    std::ifstream in(filepath.c_str());
    if (!in.is_open()) return true;
    std::string line;
    int lineNum = 0;
    while (std::getline(in, line)) {
        lineNum++;
        if (line.empty()) continue;
        std::vector<std::string> t = splitLine(line, '|');
        if (t.size() < 4) {
            std::cerr << "Warning [" << filepath << ":" << lineNum << "]: Malformed recipient line skipped.\n";
            continue;
        }
        RecipientOrganization r(t[0], t[1], t[2], t[3]);
        if (!fb.addRecipient(r)) {
            std::cerr << "Warning [" << filepath << ":" << lineNum << "]: Duplicate recipient ID '" << t[0] << "' skipped.\n";
        }
    }
    return true;
}

bool FileManager::saveDonations(const std::vector<Donation>& donations, const std::string& filepath) {
    std::ofstream out(filepath.c_str());
    if (!out.is_open()) return false;
    out << std::fixed << std::setprecision(2);
    for (size_t i = 0; i < donations.size(); ++i) {
        const Donation& d = donations[i];
        if (!d.getFoodItem()) continue;
        out << d.getDonationId() << "|"
            << d.getDonorId() << "|"
            << d.getFoodItem()->getName() << "|"
            << d.getFoodItem()->getCategory() << "|"
            << d.getTotalQuantity() << "|"
            << d.getFoodItem()->getPreparationDate().toString() << "|"
            << d.getFoodItem()->getExpiryDate().toString() << "|"
            << d.getFoodItem()->serializeDetails() << "|"
            << d.getDonationDate().toString() << "\n";
    }
    return true;
}

bool FileManager::loadDonations(FoodBank& fb, const std::string& filepath) {
    std::ifstream in(filepath.c_str());
    if (!in.is_open()) return true;
    std::string line;
    int lineNum = 0;
    while (std::getline(in, line)) {
        lineNum++;
        if (line.empty()) continue;
        std::vector<std::string> t = splitLine(line, '|');
        if (t.size() < 9) {
            std::cerr << "Warning [" << filepath << ":" << lineNum << "]: Malformed donation line skipped.\n";
            continue;
        }
        std::string donId = t[0];
        std::string donorId = t[1];
        std::string foodName = t[2];
        std::string category = t[3];
        double qty = std::atof(t[4].c_str());
        Date prepDate, expDate, donDate;
        if (!Date::parse(t[5], prepDate) || !Date::parse(t[6], expDate) || !Date::parse(t[8], donDate)) {
            std::cerr << "Warning [" << filepath << ":" << lineNum << "]: Invalid date in donation record skipped.\n";
            continue;
        }
        std::string extra = (t[7] == "-" ? "" : t[7]);

        if (!fb.findDonor(donorId)) {
            std::cerr << "Warning [" << filepath << ":" << lineNum << "]: Orphan donation referencing unknown donor '" << donorId << "'.\n";
        }

        std::shared_ptr<FoodItem> food = FoodItem::create(foodName, category, prepDate, expDate, extra);
        Donation don(donId, donorId, qty, food, donDate);
        if (!fb.addDonation(don)) {
            std::cerr << "Warning [" << filepath << ":" << lineNum << "]: Failed to add donation '" << donId << "' (duplicate or invalid).\n";
        }
    }
    return true;
}

bool FileManager::saveRequests(const std::vector<RecipientRequest>& requests, const std::string& filepath) {
    std::ofstream out(filepath.c_str());
    if (!out.is_open()) return false;
    out << std::fixed << std::setprecision(2);
    for (size_t i = 0; i < requests.size(); ++i) {
        const RecipientRequest& r = requests[i];
        out << r.getRequestId() << "|"
            << r.getRecipientId() << "|"
            << r.getFoodCategory() << "|"
            << r.getRequiredQuantity() << "|"
            << r.getPriority() << "|"
            << r.getRequestDate().toString() << "|"
            << (r.isCancelled() ? 1 : 0) << "\n";
    }
    return true;
}

bool FileManager::loadRequests(FoodBank& fb, const std::string& filepath) {
    std::ifstream in(filepath.c_str());
    if (!in.is_open()) return true;
    std::string line;
    int lineNum = 0;
    while (std::getline(in, line)) {
        lineNum++;
        if (line.empty()) continue;
        std::vector<std::string> t = splitLine(line, '|');
        if (t.size() < 7) {
            std::cerr << "Warning [" << filepath << ":" << lineNum << "]: Malformed request line skipped.\n";
            continue;
        }
        std::string reqId = t[0];
        std::string recId = t[1];
        std::string cat = t[2];
        double qty = std::atof(t[3].c_str());
        int prio = std::atoi(t[4].c_str());
        Date reqDate;
        if (!Date::parse(t[5], reqDate)) {
            std::cerr << "Warning [" << filepath << ":" << lineNum << "]: Invalid request date skipped.\n";
            continue;
        }
        bool cancelled = (std::atoi(t[6].c_str()) == 1);

        if (!fb.findRecipient(recId)) {
            std::cerr << "Warning [" << filepath << ":" << lineNum << "]: Orphan request referencing unknown recipient '" << recId << "'.\n";
        }

        RecipientRequest req(reqId, recId, cat, qty, prio, reqDate);
        req.setCancelled(cancelled);
        if (!fb.addRequest(req)) {
            std::cerr << "Warning [" << filepath << ":" << lineNum << "]: Failed to add request '" << reqId << "'.\n";
        }
    }
    return true;
}

bool FileManager::saveDeliveries(const std::vector<Delivery>& deliveries, const std::string& filepath) {
    std::ofstream out(filepath.c_str());
    if (!out.is_open()) return false;
    out << std::fixed << std::setprecision(2);
    for (size_t i = 0; i < deliveries.size(); ++i) {
        const Delivery& d = deliveries[i];
        out << d.getDeliveryId() << "|"
            << d.getDonationId() << "|"
            << d.getRequestId() << "|"
            << d.getQuantity() << "|"
            << d.getScheduledDate().toString() << "|"
            << deliveryStatusToString(d.getStatus()) << "|"
            << (d.getStatus() == DELIVERY_DELIVERED ? d.getCompletedDate().toString() : "-") << "\n";
    }
    return true;
}

bool FileManager::loadDeliveries(FoodBank& fb, const std::string& filepath) {
    std::ifstream in(filepath.c_str());
    if (!in.is_open()) return true;
    std::string line;
    int lineNum = 0;
    while (std::getline(in, line)) {
        lineNum++;
        if (line.empty()) continue;
        std::vector<std::string> t = splitLine(line, '|');
        if (t.size() < 7) {
            std::cerr << "Warning [" << filepath << ":" << lineNum << "]: Malformed delivery line skipped.\n";
            continue;
        }
        std::string delId = t[0];
        std::string donId = t[1];
        std::string reqId = t[2];
        double qty = std::atof(t[3].c_str());
        Date schedDate, compDate;
        if (!Date::parse(t[4], schedDate)) {
            std::cerr << "Warning [" << filepath << ":" << lineNum << "]: Invalid scheduled date skipped.\n";
            continue;
        }
        DeliveryStatus st = DELIVERY_SCHEDULED;
        if (t[5] == "DELIVERED") st = DELIVERY_DELIVERED;
        else if (t[5] == "CANCELLED") st = DELIVERY_CANCELLED;

        if (st == DELIVERY_DELIVERED && t[6] != "-") {
            Date::parse(t[6], compDate);
        }

        Delivery del(delId, donId, reqId, qty, schedDate, st, compDate);
        if (!fb.findDonation(donId)) {
            std::cerr << "Warning [" << filepath << ":" << lineNum << "]: Orphan delivery referencing unknown donation '" << donId << "'.\n";
        }
        if (!fb.findRequest(reqId)) {
            std::cerr << "Warning [" << filepath << ":" << lineNum << "]: Orphan delivery referencing unknown request '" << reqId << "'.\n";
        }
        fb.addDelivery(del);
    }
    return true;
}

bool FileManager::saveAll(const FoodBank& fb, const std::string& dataDir) {
    bool ok = true;
    ok = saveDonors(fb.getDonors(), joinPath(dataDir, "donors.txt")) && ok;
    ok = saveRecipients(fb.getRecipients(), joinPath(dataDir, "recipients.txt")) && ok;
    ok = saveDonations(fb.getDonations(), joinPath(dataDir, "donations.txt")) && ok;
    ok = saveRequests(fb.getRequests(), joinPath(dataDir, "requests.txt")) && ok;
    ok = saveDeliveries(fb.getDeliveries(), joinPath(dataDir, "deliveries.txt")) && ok;
    return ok;
}

bool FileManager::loadAll(FoodBank& fb, const std::string& dataDir) {
    fb.clear();
    bool ok = true;
    ok = loadDonors(fb, joinPath(dataDir, "donors.txt")) && ok;
    ok = loadRecipients(fb, joinPath(dataDir, "recipients.txt")) && ok;
    ok = loadDonations(fb, joinPath(dataDir, "donations.txt")) && ok;
    ok = loadRequests(fb, joinPath(dataDir, "requests.txt")) && ok;
    ok = loadDeliveries(fb, joinPath(dataDir, "deliveries.txt")) && ok;
    fb.recalculateState();
    return ok;
}
