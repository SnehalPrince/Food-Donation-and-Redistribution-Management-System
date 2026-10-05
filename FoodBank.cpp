#include "FoodBank.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

// [ENCAPSULATION]
FoodBank::FoodBank(const Date& today) : m_today(today) {}

void FoodBank::setToday(const Date& today) {
    m_today = today;
}

void FoodBank::clear() {
    m_donors.clear();
    m_recipients.clear();
    m_donations.clear();
    m_requests.clear();
    m_deliveries.clear();
    m_donorIds.clear();
    m_recipientIds.clear();
    m_donationIds.clear();
    m_requestIds.clear();
    m_deliveryIds.clear();
}

static std::string formatId(const std::string& prefix, int num, int width = 3) {
    std::ostringstream oss;
    oss << prefix << std::setfill('0') << std::setw(width) << num;
    return oss.str();
}

std::string FoodBank::suggestNextDonorId() const {
    int idx = 1;
    while (m_donorIds.count(formatId("D", idx)) > 0) {
        idx++;
    }
    return formatId("D", idx);
}

std::string FoodBank::suggestNextRecipientId() const {
    int idx = 1;
    while (m_recipientIds.count(formatId("R", idx)) > 0) {
        idx++;
    }
    return formatId("R", idx);
}

std::string FoodBank::suggestNextDonationId() const {
    int idx = 101;
    while (m_donationIds.count(formatId("DON", idx)) > 0) {
        idx++;
    }
    return formatId("DON", idx);
}

std::string FoodBank::suggestNextRequestId() const {
    int idx = 101;
    while (m_requestIds.count(formatId("REQ", idx)) > 0) {
        idx++;
    }
    return formatId("REQ", idx);
}

std::string FoodBank::suggestNextDeliveryId() const {
    int idx = 1;
    while (m_deliveryIds.count(formatId("DEL", idx)) > 0) {
        idx++;
    }
    return formatId("DEL", idx);
}

bool FoodBank::addDonor(const Donor& donor) {
    // [STL: set] ID uniqueness check
    if (m_donorIds.count(donor.getId()) > 0) return false;
    m_donors.push_back(donor);
    m_donorIds.insert(donor.getId());
    return true;
}

bool FoodBank::addRecipient(const RecipientOrganization& recipient) {
    // [STL: set] ID uniqueness check
    if (m_recipientIds.count(recipient.getId()) > 0) return false;
    m_recipients.push_back(recipient);
    m_recipientIds.insert(recipient.getId());
    return true;
}

bool FoodBank::addDonation(const Donation& donation) {
    // [STL: set]
    if (m_donationIds.count(donation.getDonationId()) > 0) return false;
    if (m_donorIds.count(donation.getDonorId()) == 0) return false;
    if (donation.getTotalQuantity() <= 0.0 || donation.getTotalQuantity() > 100000.0) return false;
    if (!donation.getFoodItem() || !donation.getFoodItem()->validate()) return false;
    if (donation.getFoodItem()->getExpiryDate() < m_today) return false;

    m_donations.push_back(donation);
    m_donationIds.insert(donation.getDonationId());
    return true;
}

bool FoodBank::addRequest(const RecipientRequest& request) {
    // [STL: set]
    if (m_requestIds.count(request.getRequestId()) > 0) return false;
    if (m_recipientIds.count(request.getRecipientId()) == 0) return false;
    if (request.getRequiredQuantity() <= 0.0 || request.getRequiredQuantity() > 100000.0) return false;
    if (request.getPriority() < 1 || request.getPriority() > 3) return false;

    m_requests.push_back(request);
    m_requestIds.insert(request.getRequestId());
    return true;
}

bool FoodBank::addDelivery(const Delivery& delivery) {
    // [STL: set]
    if (m_deliveryIds.count(delivery.getDeliveryId()) > 0) return false;
    m_deliveries.push_back(delivery);
    m_deliveryIds.insert(delivery.getDeliveryId());
    return true;
}

const Donor* FoodBank::findDonor(const std::string& id) const {
    for (size_t i = 0; i < m_donors.size(); ++i) {
        if (m_donors[i].getId() == id) return &m_donors[i];
    }
    return NULL;
}

const RecipientOrganization* FoodBank::findRecipient(const std::string& id) const {
    for (size_t i = 0; i < m_recipients.size(); ++i) {
        if (m_recipients[i].getId() == id) return &m_recipients[i];
    }
    return NULL;
}

Donation* FoodBank::findDonation(const std::string& id) {
    for (size_t i = 0; i < m_donations.size(); ++i) {
        if (m_donations[i].getDonationId() == id) return &m_donations[i];
    }
    return NULL;
}

const Donation* FoodBank::findDonation(const std::string& id) const {
    for (size_t i = 0; i < m_donations.size(); ++i) {
        if (m_donations[i].getDonationId() == id) return &m_donations[i];
    }
    return NULL;
}

RecipientRequest* FoodBank::findRequest(const std::string& id) {
    for (size_t i = 0; i < m_requests.size(); ++i) {
        if (m_requests[i].getRequestId() == id) return &m_requests[i];
    }
    return NULL;
}

const RecipientRequest* FoodBank::findRequest(const std::string& id) const {
    for (size_t i = 0; i < m_requests.size(); ++i) {
        if (m_requests[i].getRequestId() == id) return &m_requests[i];
    }
    return NULL;
}

Delivery* FoodBank::findDelivery(const std::string& id) {
    for (size_t i = 0; i < m_deliveries.size(); ++i) {
        if (m_deliveries[i].getDeliveryId() == id) return &m_deliveries[i];
    }
    return NULL;
}

const Delivery* FoodBank::findDelivery(const std::string& id) const {
    for (size_t i = 0; i < m_deliveries.size(); ++i) {
        if (m_deliveries[i].getDeliveryId() == id) return &m_deliveries[i];
    }
    return NULL;
}

// [STL: queue] Builds processing queue sorted by Priority (1 High to 3 Low) then Request ID
std::queue<std::string> FoodBank::getPendingRequestQueue() const {
    std::vector<const RecipientRequest*> pending;
    for (size_t i = 0; i < m_requests.size(); ++i) {
        RequestStatus st = m_requests[i].getStatus();
        if (st == REQUEST_PENDING || st == REQUEST_PARTIALLY_FULFILLED) {
            if (m_requests[i].getRemainingNeed() > RecipientRequest::EPS) {
                pending.push_back(&m_requests[i]);
            }
        }
    }

    std::sort(pending.begin(), pending.end(), [](const RecipientRequest* a, const RecipientRequest* b) {
        if (a->getPriority() != b->getPriority()) {
            return a->getPriority() < b->getPriority();
        }
        if (a->getRequestDate() != b->getRequestDate()) {
            return a->getRequestDate() < b->getRequestDate();
        }
        return a->getRequestId() < b->getRequestId();
    });

    std::queue<std::string> q;
    for (size_t i = 0; i < pending.size(); ++i) {
        q.push(pending[i]->getRequestId());
    }
    return q;
}

MatchResult FoodBank::matchRequest(const std::string& requestId) {
    MatchResult result;
    RecipientRequest* req = findRequest(requestId);
    if (!req) {
        result.message = "Error: Request ID not found.";
        return result;
    }
    if (req->isCancelled()) {
        result.message = "Error: Cannot match a cancelled request.";
        return result;
    }
    double need = req->getRemainingNeed();
    if (need <= RecipientRequest::EPS) {
        result.message = "Request is already fully satisfied.";
        return result;
    }

    // [STL: vector] Collect candidates
    std::vector<Donation*> candidates;
    for (size_t i = 0; i < m_donations.size(); ++i) {
        Donation& don = m_donations[i];
        if (don.getFoodItem() && don.getFoodItem()->getCategory() == req->getFoodCategory()) {
            if (don.getStatus(m_today) != DONATION_EXPIRED &&
                don.getFoodItem()->isEligibleForDistribution(m_today) &&
                don.getAvailableQuantity() > Donation::EPS) {
                candidates.push_back(&don);
            }
        }
    }

    if (candidates.empty()) {
        result.message = "No eligible stock found for category '" + req->getFoodCategory() + "'.";
        return result;
    }

    // Earliest expiry first; tie-breaker: smaller donation ID
    std::sort(candidates.begin(), candidates.end(), [](const Donation* a, const Donation* b) {
        if (a->getFoodItem()->getExpiryDate() != b->getFoodItem()->getExpiryDate()) {
            return a->getFoodItem()->getExpiryDate() < b->getFoodItem()->getExpiryDate();
        }
        return a->getDonationId() < b->getDonationId();
    });

    double totalAllocated = 0.0;
    for (size_t i = 0; i < candidates.size() && need > RecipientRequest::EPS; ++i) {
        Donation* don = candidates[i];
        double avail = don->getAvailableQuantity();
        double alloc = std::min(need, avail);

        if (don->reserveQuantity(alloc)) {
            req->addAllocation(alloc);
            std::string delId = suggestNextDeliveryId();
            Delivery del(delId, don->getDonationId(), req->getRequestId(), alloc, m_today, DELIVERY_SCHEDULED);
            m_deliveries.push_back(del);
            m_deliveryIds.insert(delId);

            result.createdDeliveryIds.push_back(delId);
            totalAllocated += alloc;
            need -= alloc;
        }
    }

    result.success = (totalAllocated > 0.0);
    result.allocatedQuantity = totalAllocated;
    result.remainingNeed = req->getRemainingNeed();
    std::ostringstream oss;
    oss << "Allocated " << totalAllocated << " kg across " << result.createdDeliveryIds.size()
        << " delivery record(s). Balance pending: " << result.remainingNeed << " kg.";
    result.message = oss.str();
    return result;
}

MatchResult FoodBank::matchNextPendingRequest() {
    std::queue<std::string> q = getPendingRequestQueue();
    if (q.empty()) {
        MatchResult res;
        res.message = "No pending requests in queue.";
        return res;
    }
    return matchRequest(q.front());
}

int FoodBank::autoMatchAllPending() {
    int matchedCount = 0;
    while (true) {
        std::queue<std::string> q = getPendingRequestQueue();
        if (q.empty()) break;
        bool progressed = false;
        while (!q.empty()) {
            std::string reqId = q.front();
            q.pop();
            MatchResult res = matchRequest(reqId);
            if (res.success) {
                matchedCount++;
                progressed = true;
            }
        }
        if (!progressed) break;
    }
    return matchedCount;
}

bool FoodBank::confirmDelivery(const std::string& deliveryId, std::string& errorMsg) {
    Delivery* del = findDelivery(deliveryId);
    if (!del) {
        errorMsg = "Delivery ID not found.";
        return false;
    }
    if (del->getStatus() != DELIVERY_SCHEDULED) {
        errorMsg = "Delivery is not in SCHEDULED status.";
        return false;
    }
    Donation* don = findDonation(del->getDonationId());
    RecipientRequest* req = findRequest(del->getRequestId());
    if (!don || !req) {
        errorMsg = "Associated donation or request not found.";
        return false;
    }

    // Re-check food eligibility before confirming delivery
    if (!don->getFoodItem()->isEligibleForDistribution(m_today)) {
        errorMsg = "Food item has expired and cannot be delivered. Please cancel the delivery instead.";
        return false;
    }

    del->markDelivered(m_today);
    req->recordDelivered(del->getQuantity());
    return true;
}

bool FoodBank::cancelDelivery(const std::string& deliveryId, std::string& errorMsg) {
    Delivery* del = findDelivery(deliveryId);
    if (!del) {
        errorMsg = "Delivery ID not found.";
        return false;
    }
    if (del->getStatus() != DELIVERY_SCHEDULED) {
        errorMsg = "Only SCHEDULED deliveries can be cancelled.";
        return false;
    }
    Donation* don = findDonation(del->getDonationId());
    RecipientRequest* req = findRequest(del->getRequestId());
    if (!don || !req) {
        errorMsg = "Associated donation or request not found.";
        return false;
    }

    // Reverse reservation exactly
    don->releaseQuantity(del->getQuantity());
    req->removeAllocation(del->getQuantity());
    del->markCancelled();
    return true;
}

bool FoodBank::cancelRequest(const std::string& requestId, std::string& errorMsg) {
    RecipientRequest* req = findRequest(requestId);
    if (!req) {
        errorMsg = "Request ID not found.";
        return false;
    }
    if (req->getAllocatedQuantity() > RecipientRequest::EPS) {
        errorMsg = "Cannot cancel request with active scheduled deliveries. Cancel deliveries first.";
        return false;
    }
    if (!req->cancel()) {
        errorMsg = "Failed to cancel request.";
        return false;
    }
    return true;
}

void FoodBank::recalculateState() {
    // Reset all donations
    for (size_t i = 0; i < m_donations.size(); ++i) {
        m_donations[i].setAvailableQuantity(m_donations[i].getTotalQuantity());
    }
    // Reset all requests
    for (size_t i = 0; i < m_requests.size(); ++i) {
        m_requests[i].setQuantities(0.0, 0.0);
    }
    // Replay deliveries
    for (size_t i = 0; i < m_deliveries.size(); ++i) {
        Delivery& del = m_deliveries[i];
        Donation* don = findDonation(del.getDonationId());
        RecipientRequest* req = findRequest(del.getRequestId());
        if (!don || !req) continue;

        if (del.getStatus() == DELIVERY_SCHEDULED) {
            don->reserveQuantity(del.getQuantity());
            req->addAllocation(del.getQuantity());
        } else if (del.getStatus() == DELIVERY_DELIVERED) {
            don->reserveQuantity(del.getQuantity());
            req->recordDelivered(del.getQuantity());
        }
    }
}
