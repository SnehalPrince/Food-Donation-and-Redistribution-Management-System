#ifndef RECIPIENTREQUEST_H
#define RECIPIENTREQUEST_H

#include <string>
#include "Date.h"

enum RequestStatus {
    REQUEST_PENDING,
    REQUEST_PARTIALLY_FULFILLED,
    REQUEST_FULFILLED,
    REQUEST_CANCELLED
};

std::string requestStatusToString(RequestStatus status);

// [ENCAPSULATION]
// Represents a food requirement from a recipient organization with priority-based allocation tracking.
class RecipientRequest {
public:
    static const double EPS;

private:
    std::string m_requestId;
    std::string m_recipientId;
    std::string m_foodCategory;
    double m_requiredQuantity;
    double m_allocatedQuantity;
    double m_deliveredQuantity;
    int m_priority; // 1 = High, 2 = Medium, 3 = Low
    Date m_requestDate;
    bool m_isCancelled;

public:
    RecipientRequest(const std::string& reqId,
                     const std::string& recId,
                     const std::string& category,
                     double quantity,
                     int priority,
                     const Date& reqDate);

    const std::string& getRequestId() const { return m_requestId; }
    const std::string& getRecipientId() const { return m_recipientId; }
    const std::string& getFoodCategory() const { return m_foodCategory; }
    double getRequiredQuantity() const { return m_requiredQuantity; }
    double getAllocatedQuantity() const { return m_allocatedQuantity; }
    double getDeliveredQuantity() const { return m_deliveredQuantity; }
    int getPriority() const { return m_priority; }
    const Date& getRequestDate() const { return m_requestDate; }
    bool isCancelled() const { return m_isCancelled; }

    double getRemainingNeed() const;
    void addAllocation(double qty);
    void removeAllocation(double qty);
    void recordDelivered(double qty);
    bool cancel();
    void setQuantities(double allocated, double delivered);
    void setCancelled(bool val) { m_isCancelled = val; }

    RequestStatus getStatus() const;
    void display() const;
};

#endif // RECIPIENTREQUEST_H
