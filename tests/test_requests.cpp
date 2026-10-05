#include <iostream>
#include "test_util.h"
#include "../Date.h"
#include "../RecipientRequest.h"
#include "../Delivery.h"

static bool testRequestLifecycle() {
    Date reqDate(2026, 10, 5);
    RecipientRequest req("REQ101", "R001", "COOKED", 30.0, 1, reqDate);

    ASSERT_EQ(std::string("REQ101"), req.getRequestId());
    ASSERT_EQ(std::string("R001"), req.getRecipientId());
    ASSERT_NEAR(30.0, req.getRequiredQuantity(), 1e-9);
    ASSERT_NEAR(30.0, req.getRemainingNeed(), 1e-9);
    ASSERT_EQ(REQUEST_PENDING, req.getStatus());

    // Allocate 20 kg
    req.addAllocation(20.0);
    ASSERT_NEAR(10.0, req.getRemainingNeed(), 1e-9);
    ASSERT_EQ(REQUEST_PARTIALLY_FULFILLED, req.getStatus());

    // Cannot cancel while allocation is active
    ASSERT_FALSE(req.cancel());

    // Record delivery of 20 kg
    req.recordDelivered(20.0);
    ASSERT_NEAR(20.0, req.getDeliveredQuantity(), 1e-9);
    ASSERT_NEAR(0.0, req.getAllocatedQuantity(), 1e-9);
    ASSERT_NEAR(10.0, req.getRemainingNeed(), 1e-9);
    ASSERT_EQ(REQUEST_PARTIALLY_FULFILLED, req.getStatus());

    // Allocate remaining 10 kg and deliver
    req.addAllocation(10.0);
    ASSERT_NEAR(0.0, req.getRemainingNeed(), 1e-9);
    req.recordDelivered(10.0);
    ASSERT_NEAR(30.0, req.getDeliveredQuantity(), 1e-9);
    ASSERT_EQ(REQUEST_FULFILLED, req.getStatus());

    return true;
}

static bool testRequestCancellation() {
    Date reqDate(2026, 10, 5);
    RecipientRequest req("REQ102", "R001", "PACKAGED", 15.0, 2, reqDate);

    ASSERT_EQ(REQUEST_PENDING, req.getStatus());
    ASSERT_TRUE(req.cancel());
    ASSERT_EQ(REQUEST_CANCELLED, req.getStatus());
    ASSERT_NEAR(0.0, req.getRemainingNeed(), 1e-9);

    return true;
}

static bool testDeliveryTransitions() {
    Date schedDate(2026, 10, 5);
    Delivery del("DEL001", "DON101", "REQ101", 30.0, schedDate);

    ASSERT_EQ(std::string("DEL001"), del.getDeliveryId());
    ASSERT_EQ(std::string("DON101"), del.getDonationId());
    ASSERT_EQ(std::string("REQ101"), del.getRequestId());
    ASSERT_NEAR(30.0, del.getQuantity(), 1e-9);
    ASSERT_EQ(DELIVERY_SCHEDULED, del.getStatus());

    // Mark as delivered today
    Date deliverDate(2026, 10, 5);
    ASSERT_TRUE(del.markDelivered(deliverDate));
    ASSERT_EQ(DELIVERY_DELIVERED, del.getStatus());
    ASSERT_EQ(deliverDate.toString(), del.getCompletedDate().toString());

    // Cannot mark delivered again or cancel already delivered
    ASSERT_FALSE(del.markDelivered(deliverDate));
    ASSERT_FALSE(del.markCancelled());

    // Fresh delivery for cancellation
    Delivery del2("DEL002", "DON101", "REQ101", 10.0, schedDate);
    ASSERT_TRUE(del2.markCancelled());
    ASSERT_EQ(DELIVERY_CANCELLED, del2.getStatus());
    ASSERT_FALSE(del2.markDelivered(deliverDate));

    return true;
}

int main() {
    std::cout << "--- Running Requests & Deliveries Module Tests ---\n";
    RUN_TEST(testRequestLifecycle);
    RUN_TEST(testRequestCancellation);
    RUN_TEST(testDeliveryTransitions);
    TEST_REPORT_SUMMARY();
}
