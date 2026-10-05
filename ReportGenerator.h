#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include <string>
#include <vector>
#include <map>
#include "FoodBank.h"

struct StockMetrics {
    double totalDonated;
    double totalRedistributed;
    double totalInTransit;
    double totalAvailable;
    double totalWasted;
    double remaining;
    double fulfillmentRate;
    int totalRequests;
    int fulfilledRequests;
    int pendingRequests;
    int totalDeliveries;
    int recipientOrgsServed;
    bool isReconciled;

    StockMetrics() : totalDonated(0.0), totalRedistributed(0.0), totalInTransit(0.0),
                     totalAvailable(0.0), totalWasted(0.0), remaining(0.0),
                     fulfillmentRate(0.0), totalRequests(0), fulfilledRequests(0),
                     pendingRequests(0), totalDeliveries(0), recipientOrgsServed(0),
                     isReconciled(false) {}
};

// [ABSTRACTION]
// Generates analytical, statistical, and SDG audit reports from FoodBank state.
class ReportGenerator {
public:
    static StockMetrics calculateMetrics(const FoodBank& fb);

    static std::string generateSummary(const FoodBank& fb);
    static std::string generateDonorRanking(const FoodBank& fb);
    static std::string generateCategoryReport(const FoodBank& fb);
    static std::string generatePendingRequestsReport(const FoodBank& fb);
    static std::string generateExpiringSoonReport(const FoodBank& fb, int daysThreshold = 3);
};

#endif // REPORTGENERATOR_H
