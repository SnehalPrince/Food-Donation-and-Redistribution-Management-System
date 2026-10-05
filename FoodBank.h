#ifndef FOODBANK_H
#define FOODBANK_H

#include <vector>
#include <map>
#include <set>
#include <queue>
#include <string>
#include <memory>

#include "Date.h"
#include "Donor.h"
#include "Recipient.h"
#include "Donation.h"
#include "RecipientRequest.h"
#include "Delivery.h"

struct MatchResult {
    bool success;
    double allocatedQuantity;
    double remainingNeed;
    std::vector<std::string> createdDeliveryIds;
    std::string message;

    MatchResult() : success(false), allocatedQuantity(0.0), remainingNeed(0.0), message("") {}
};

// [ENCAPSULATION]
// Central coordinator managing donors, recipients, inventory, requests, matching, and deliveries.
class FoodBank {
private:
    // [STL: vector] Main collections
    std::vector<Donor> m_donors;
    std::vector<RecipientOrganization> m_recipients;
    std::vector<Donation> m_donations;
    std::vector<RecipientRequest> m_requests;
    std::vector<Delivery> m_deliveries;

    // [STL: set] Enforcing unique IDs
    std::set<std::string> m_donorIds;
    std::set<std::string> m_recipientIds;
    std::set<std::string> m_donationIds;
    std::set<std::string> m_requestIds;
    std::set<std::string> m_deliveryIds;

    Date m_today;

public:
    FoodBank(const Date& today = Date());

    const Date& getToday() const { return m_today; }
    void setToday(const Date& today);

    // ID Generators suggesting next available ID
    std::string suggestNextDonorId() const;
    std::string suggestNextRecipientId() const;
    std::string suggestNextDonationId() const;
    std::string suggestNextRequestId() const;
    std::string suggestNextDeliveryId() const;

    // Registrations
    bool addDonor(const Donor& donor);
    bool addRecipient(const RecipientOrganization& recipient);
    bool addDonation(const Donation& donation);
    bool addRequest(const RecipientRequest& request);
    bool addDelivery(const Delivery& delivery);

    // Lookups
    const Donor* findDonor(const std::string& id) const;
    const RecipientOrganization* findRecipient(const std::string& id) const;
    Donation* findDonation(const std::string& id);
    const Donation* findDonation(const std::string& id) const;
    RecipientRequest* findRequest(const std::string& id);
    const RecipientRequest* findRequest(const std::string& id) const;
    Delivery* findDelivery(const std::string& id);
    const Delivery* findDelivery(const std::string& id) const;

    // [STL: queue] Matching and redistribution workflow
    std::queue<std::string> getPendingRequestQueue() const;
    MatchResult matchRequest(const std::string& requestId);
    MatchResult matchNextPendingRequest();
    int autoMatchAllPending();

    // Delivery confirmation and cancellation
    bool confirmDelivery(const std::string& deliveryId, std::string& errorMsg);
    bool cancelDelivery(const std::string& deliveryId, std::string& errorMsg);
    bool cancelRequest(const std::string& requestId, std::string& errorMsg);

    // Derived status refresh and invariant verification
    void recalculateState();
    void clear();

    // Const collection accessors for reporting and persistence
    const std::vector<Donor>& getDonors() const { return m_donors; }
    const std::vector<RecipientOrganization>& getRecipients() const { return m_recipients; }
    const std::vector<Donation>& getDonations() const { return m_donations; }
    const std::vector<RecipientRequest>& getRequests() const { return m_requests; }
    const std::vector<Delivery>& getDeliveries() const { return m_deliveries; }
};

#endif // FOODBANK_H
