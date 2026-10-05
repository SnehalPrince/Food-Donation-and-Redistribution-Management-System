#include "ReportGenerator.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

// [ABSTRACTION]
StockMetrics ReportGenerator::calculateMetrics(const FoodBank& fb) {
    StockMetrics m;
    const std::vector<Donation>& donations = fb.getDonations();
    const std::vector<Delivery>& deliveries = fb.getDeliveries();
    const std::vector<RecipientRequest>& requests = fb.getRequests();
    Date today = fb.getToday();

    for (size_t i = 0; i < donations.size(); ++i) {
        const Donation& d = donations[i];
        m.totalDonated += d.getTotalQuantity();

        DonationStatus st = d.getStatus(today);
        if (st == DONATION_EXPIRED) {
            m.totalWasted += d.getAvailableQuantity();
        } else {
            m.totalAvailable += d.getAvailableQuantity();
        }
    }

    std::set<std::string> servedRecipients;
    for (size_t i = 0; i < deliveries.size(); ++i) {
        const Delivery& del = deliveries[i];
        if (del.getStatus() == DELIVERY_DELIVERED) {
            m.totalRedistributed += del.getQuantity();
            m.totalDeliveries++;
            const RecipientRequest* req = fb.findRequest(del.getRequestId());
            if (req) {
                servedRecipients.insert(req->getRecipientId());
            }
        } else if (del.getStatus() == DELIVERY_SCHEDULED) {
            m.totalInTransit += del.getQuantity();
        }
    }
    m.recipientOrgsServed = static_cast<int>(servedRecipients.size());
    m.remaining = m.totalDonated - m.totalRedistributed;

    m.totalRequests = static_cast<int>(requests.size());
    for (size_t i = 0; i < requests.size(); ++i) {
        RequestStatus st = requests[i].getStatus();
        if (st == REQUEST_FULFILLED) {
            m.fulfilledRequests++;
        } else if (st == REQUEST_PENDING || st == REQUEST_PARTIALLY_FULFILLED) {
            m.pendingRequests++;
        }
    }

    if (m.totalRequests > 0) {
        m.fulfillmentRate = (static_cast<double>(m.fulfilledRequests) / m.totalRequests) * 100.0;
    } else {
        m.fulfillmentRate = 0.0;
    }

    // Invariant: donated = redistributed + in_transit + available + wasted
    double sumComponents = m.totalRedistributed + m.totalInTransit + m.totalAvailable + m.totalWasted;
    m.isReconciled = (std::fabs(m.totalDonated - sumComponents) < Donation::EPS);

    return m;
}

std::string ReportGenerator::generateSummary(const FoodBank& fb) {
    StockMetrics m = calculateMetrics(fb);
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "====================================================\n";
    oss << "       FOOD REDISTRIBUTION SYSTEM SUMMARY REPORT    \n";
    oss << "====================================================\n";
    oss << "System Date: " << fb.getToday().toString() << "\n\n";

    oss << "--- INVENTORY & STOCK RECONCILIATION ---\n";
    oss << "Total Food Donated:        " << std::setw(10) << m.totalDonated << " kg\n";
    oss << "Total Food Redistributed:  " << std::setw(10) << m.totalRedistributed << " kg\n";
    oss << "Food In-Transit (Reserved):" << std::setw(10) << m.totalInTransit << " kg\n";
    oss << "Available In Stock:        " << std::setw(10) << m.totalAvailable << " kg\n";
    oss << "Expired / Wasted Food:     " << std::setw(10) << m.totalWasted << " kg\n";
    oss << "Net Remaining (Unredeemed):" << std::setw(10) << m.remaining << " kg\n";
    oss << "Stock reconciliation: " << (m.isReconciled ? "OK" : "MISMATCH") << "\n\n";

    oss << "--- OPERATIONS & FULFILLMENT ---\n";
    oss << "Total Recipient Requests:  " << std::setw(10) << m.totalRequests << "\n";
    oss << "Fulfilled Requests:        " << std::setw(10) << m.fulfilledRequests << "\n";
    oss << "Pending / Partial Requests:" << std::setw(10) << m.pendingRequests << "\n";
    oss << "Fulfillment Rate:          " << std::setw(9) << m.fulfillmentRate << " %\n";
    oss << "Total Completed Deliveries:" << std::setw(10) << m.totalDeliveries << "\n\n";

    oss << "--- SUSTAINABLE DEVELOPMENT GOALS (SDG) IMPACT ---\n";
    oss << "[SDG 2: Zero Hunger - Target 2.1]\n";
    oss << "  * Food Provided to People in Need: " << m.totalRedistributed << " kg\n";
    oss << "  * Completed Food Relief Missions:  " << m.totalDeliveries << "\n";
    oss << "  * Recipient Organizations Served:  " << m.recipientOrgsServed << "\n";
    oss << "[SDG 12: Responsible Consumption - Target 12.3]\n";
    oss << "  * Surplus Diverted from Landfill:  " << m.totalRedistributed << " kg\n";
    oss << "  * Surplus Lost to Spoilage/Expiry: " << m.totalWasted << " kg\n";
    oss << "====================================================\n";
    return oss.str();
}

// [STL: map] and [STL: sort]
std::string ReportGenerator::generateDonorRanking(const FoodBank& fb) {
    std::map<std::string, double> donorTotals;
    const std::vector<Donation>& donations = fb.getDonations();

    for (size_t i = 0; i < donations.size(); ++i) {
        donorTotals[donations[i].getDonorId()] += donations[i].getTotalQuantity();
    }

    struct DonorRank {
        std::string donorId;
        std::string name;
        double total;
    };

    std::vector<DonorRank> ranks;
    for (std::map<std::string, double>::const_iterator it = donorTotals.begin(); it != donorTotals.end(); ++it) {
        DonorRank r;
        r.donorId = it->first;
        const Donor* d = fb.findDonor(it->first);
        r.name = d ? d->getName() : "Unknown";
        r.total = it->second;
        ranks.push_back(r);
    }

    std::sort(ranks.begin(), ranks.end(), [](const DonorRank& a, const DonorRank& b) {
        if (std::fabs(a.total - b.total) > Donation::EPS) {
            return a.total > b.total;
        }
        return a.donorId < b.donorId;
    });

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "====================================================\n";
    oss << "               DONOR RANKING REPORT                 \n";
    oss << "====================================================\n";
    if (ranks.empty()) {
        oss << "No donor donations recorded yet.\n";
    } else {
        oss << "Top Donor: " << ranks[0].name << " (" << ranks[0].total << " kg)\n\n";
        oss << std::left << std::setw(6) << "Rank"
            << std::setw(10) << "Donor ID"
            << std::setw(24) << "Name"
            << std::right << std::setw(12) << "Total Donated" << "\n";
        oss << "----------------------------------------------------\n";
        for (size_t i = 0; i < ranks.size(); ++i) {
            oss << std::left << std::setw(6) << (i + 1)
                << std::setw(10) << ranks[i].donorId
                << std::setw(24) << ranks[i].name
                << std::right << std::setw(9) << ranks[i].total << " kg\n";
        }
    }
    oss << "====================================================\n";
    return oss.str();
}

// [STL: map]
std::string ReportGenerator::generateCategoryReport(const FoodBank& fb) {
    std::map<std::string, double> donatedMap;
    std::map<std::string, double> redistributedMap;
    std::map<std::string, double> availableMap;

    const std::vector<Donation>& donations = fb.getDonations();
    for (size_t i = 0; i < donations.size(); ++i) {
        const Donation& d = donations[i];
        if (d.getFoodItem()) {
            std::string cat = d.getFoodItem()->getCategory();
            donatedMap[cat] += d.getTotalQuantity();
            if (d.getStatus(fb.getToday()) != DONATION_EXPIRED) {
                availableMap[cat] += d.getAvailableQuantity();
            }
        }
    }

    const std::vector<Delivery>& deliveries = fb.getDeliveries();
    for (size_t i = 0; i < deliveries.size(); ++i) {
        const Delivery& del = deliveries[i];
        if (del.getStatus() == DELIVERY_DELIVERED) {
            const Donation* don = fb.findDonation(del.getDonationId());
            if (don && don->getFoodItem()) {
                redistributedMap[don->getFoodItem()->getCategory()] += del.getQuantity();
            }
        }
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "====================================================\n";
    oss << "           CATEGORY-WISE BREAKDOWN REPORT           \n";
    oss << "====================================================\n";
    oss << std::left << std::setw(16) << "Category"
        << std::right << std::setw(12) << "Donated"
        << std::setw(14) << "Redistributed"
        << std::setw(12) << "Available" << "\n";
    oss << "----------------------------------------------------\n";

    static const char* cats[] = { "COOKED", "PACKAGED", "BAKERY", "PRODUCE" };
    for (size_t i = 0; i < 4; ++i) {
        std::string cat = cats[i];
        oss << std::left << std::setw(16) << cat
            << std::right << std::setw(9) << donatedMap[cat] << " kg"
            << std::setw(11) << redistributedMap[cat] << " kg"
            << std::setw(9) << availableMap[cat] << " kg\n";
    }
    oss << "====================================================\n";
    return oss.str();
}

std::string ReportGenerator::generatePendingRequestsReport(const FoodBank& fb) {
    std::queue<std::string> q = fb.getPendingRequestQueue();
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "====================================================\n";
    oss << "       PENDING REQUESTS IN PROCESSING QUEUE         \n";
    oss << "====================================================\n";
    if (q.empty()) {
        oss << "No pending requests awaiting fulfillment.\n";
    } else {
        int idx = 1;
        while (!q.empty()) {
            std::string reqId = q.front();
            q.pop();
            const RecipientRequest* r = fb.findRequest(reqId);
            if (r) {
                std::string prioStr = (r->getPriority() == 1 ? "High" : (r->getPriority() == 2 ? "Medium" : "Low"));
                oss << idx++ << ". Request [" << r->getRequestId() << "]"
                    << " Priority: " << prioStr
                    << " | Needed: " << r->getRemainingNeed() << " kg of " << r->getFoodCategory()
                    << " | Recipient: " << r->getRecipientId() << "\n";
            }
        }
    }
    oss << "====================================================\n";
    return oss.str();
}

std::string ReportGenerator::generateExpiringSoonReport(const FoodBank& fb, int daysThreshold) {
    const std::vector<Donation>& donations = fb.getDonations();
    Date today = fb.getToday();

    std::vector<const Donation*> expiring;
    for (size_t i = 0; i < donations.size(); ++i) {
        const Donation& d = donations[i];
        if (d.getAvailableQuantity() > Donation::EPS && d.getFoodItem()) {
            Date exp = d.getFoodItem()->getExpiryDate();
            if (today <= exp) {
                int daysLeft = Date::daysBetween(today, exp);
                if (daysLeft <= daysThreshold) {
                    expiring.push_back(&d);
                }
            }
        }
    }

    std::sort(expiring.begin(), expiring.end(), [](const Donation* a, const Donation* b) {
        return a->getFoodItem()->getExpiryDate() < b->getFoodItem()->getExpiryDate();
    });

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "====================================================\n";
    oss << "       DONATIONS EXPIRING SOON (WITHIN " << daysThreshold << " DAYS)    \n";
    oss << "====================================================\n";
    if (expiring.empty()) {
        oss << "No active stock expiring within " << daysThreshold << " days.\n";
    } else {
        for (size_t i = 0; i < expiring.size(); ++i) {
            const Donation* d = expiring[i];
            int daysLeft = Date::daysBetween(today, d->getFoodItem()->getExpiryDate());
            oss << "Donation [" << d->getDonationId() << "]: "
                << d->getFoodItem()->getName() << " (" << d->getFoodItem()->getCategory() << ")"
                << " | Available: " << d->getAvailableQuantity() << " kg"
                << " | Expiry: " << d->getFoodItem()->getExpiryDate().toString()
                << " (" << daysLeft << " day(s) remaining)\n";
        }
    }
    oss << "====================================================\n";
    return oss.str();
}
