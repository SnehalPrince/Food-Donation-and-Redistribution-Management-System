#ifndef DELIVERY_H
#define DELIVERY_H

#include <string>
#include "Date.h"

enum DeliveryStatus {
    DELIVERY_SCHEDULED,
    DELIVERY_DELIVERED,
    DELIVERY_CANCELLED
};

std::string deliveryStatusToString(DeliveryStatus status);

// [ENCAPSULATION]
// Represents a redistribution event linking a donation batch with a recipient request.
class Delivery {
private:
    std::string m_deliveryId;
    std::string m_donationId;
    std::string m_requestId;
    double m_quantity;
    Date m_scheduledDate;
    Date m_completedDate;
    DeliveryStatus m_status;

public:
    Delivery(const std::string& delId,
             const std::string& donId,
             const std::string& reqId,
             double quantity,
             const Date& scheduledDate,
             DeliveryStatus status = DELIVERY_SCHEDULED,
             const Date& completedDate = Date());

    const std::string& getDeliveryId() const { return m_deliveryId; }
    const std::string& getDonationId() const { return m_donationId; }
    const std::string& getRequestId() const { return m_requestId; }
    double getQuantity() const { return m_quantity; }
    const Date& getScheduledDate() const { return m_scheduledDate; }
    const Date& getCompletedDate() const { return m_completedDate; }
    DeliveryStatus getStatus() const { return m_status; }

    bool markDelivered(const Date& today);
    bool markCancelled();
    void display() const;
};

#endif // DELIVERY_H
