#include "Delivery.h"
#include <iostream>
#include <iomanip>

std::string deliveryStatusToString(DeliveryStatus status) {
    switch (status) {
        case DELIVERY_SCHEDULED: return "SCHEDULED";
        case DELIVERY_DELIVERED: return "DELIVERED";
        case DELIVERY_CANCELLED: return "CANCELLED";
        default: return "UNKNOWN";
    }
}

// [ENCAPSULATION]
Delivery::Delivery(const std::string& delId,
                   const std::string& donId,
                   const std::string& reqId,
                   double quantity,
                   const Date& scheduledDate,
                   DeliveryStatus status,
                   const Date& completedDate)
    : m_deliveryId(delId),
      m_donationId(donId),
      m_requestId(reqId),
      m_quantity(quantity),
      m_scheduledDate(scheduledDate),
      m_completedDate(completedDate),
      m_status(status) {}

bool Delivery::markDelivered(const Date& today) {
    if (m_status != DELIVERY_SCHEDULED) {
        return false;
    }
    m_status = DELIVERY_DELIVERED;
    m_completedDate = today;
    return true;
}

bool Delivery::markCancelled() {
    if (m_status != DELIVERY_SCHEDULED) {
        return false;
    }
    m_status = DELIVERY_CANCELLED;
    return true;
}

void Delivery::display() const {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Delivery [" << m_deliveryId << "]"
              << " | Donation: " << m_donationId
              << " | Request: " << m_requestId
              << " | Quantity: " << m_quantity << " kg"
              << " | Scheduled: " << m_scheduledDate.toString()
              << " | Status: " << deliveryStatusToString(m_status);
    if (m_status == DELIVERY_DELIVERED) {
        std::cout << " (Delivered on " << m_completedDate.toString() << ")";
    }
    std::cout << "\n";
}
