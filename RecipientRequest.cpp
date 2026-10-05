#include "RecipientRequest.h"
#include <iostream>
#include <iomanip>
#include <cmath>

const double RecipientRequest::EPS = 1e-9;

std::string requestStatusToString(RequestStatus status) {
    switch (status) {
        case REQUEST_PENDING: return "PENDING";
        case REQUEST_PARTIALLY_FULFILLED: return "PARTIALLY_FULFILLED";
        case REQUEST_FULFILLED: return "FULFILLED";
        case REQUEST_CANCELLED: return "CANCELLED";
        default: return "UNKNOWN";
    }
}

// [ENCAPSULATION]
RecipientRequest::RecipientRequest(const std::string& reqId,
                                   const std::string& recId,
                                   const std::string& category,
                                   double quantity,
                                   int priority,
                                   const Date& reqDate)
    : m_requestId(reqId),
      m_recipientId(recId),
      m_foodCategory(category),
      m_requiredQuantity(quantity),
      m_allocatedQuantity(0.0),
      m_deliveredQuantity(0.0),
      m_priority(priority),
      m_requestDate(reqDate),
      m_isCancelled(false) {}

double RecipientRequest::getRemainingNeed() const {
    if (m_isCancelled) return 0.0;
    double remaining = m_requiredQuantity - (m_allocatedQuantity + m_deliveredQuantity);
    return remaining < EPS ? 0.0 : remaining;
}

void RecipientRequest::addAllocation(double qty) {
    if (qty > 0.0) {
        m_allocatedQuantity += qty;
    }
}

void RecipientRequest::removeAllocation(double qty) {
    if (qty > 0.0) {
        m_allocatedQuantity -= qty;
        if (m_allocatedQuantity < EPS) {
            m_allocatedQuantity = 0.0;
        }
    }
}

void RecipientRequest::recordDelivered(double qty) {
    if (qty > 0.0) {
        removeAllocation(qty);
        m_deliveredQuantity += qty;
    }
}

bool RecipientRequest::cancel() {
    if (m_allocatedQuantity > EPS) {
        return false; // Cannot cancel while deliveries are scheduled/in-transit
    }
    m_isCancelled = true;
    return true;
}

void RecipientRequest::setQuantities(double allocated, double delivered) {
    m_allocatedQuantity = allocated;
    m_deliveredQuantity = delivered;
}

RequestStatus RecipientRequest::getStatus() const {
    if (m_isCancelled) {
        return REQUEST_CANCELLED;
    }
    if (m_deliveredQuantity >= m_requiredQuantity - EPS) {
        return REQUEST_FULFILLED;
    }
    if (m_allocatedQuantity <= EPS && m_deliveredQuantity <= EPS) {
        return REQUEST_PENDING;
    }
    return REQUEST_PARTIALLY_FULFILLED;
}

void RecipientRequest::display() const {
    std::cout << std::fixed << std::setprecision(2);
    std::string prioStr = (m_priority == 1 ? "High" : (m_priority == 2 ? "Medium" : "Low"));
    std::cout << "Request [" << m_requestId << "]"
              << " | Recipient: " << m_recipientId
              << " | Category: " << m_foodCategory
              << " | Needed: " << m_requiredQuantity << " kg"
              << " | Allocated: " << m_allocatedQuantity << " kg"
              << " | Delivered: " << m_deliveredQuantity << " kg"
              << " | Priority: " << prioStr
              << " | Status: " << requestStatusToString(getStatus()) << "\n";
}
