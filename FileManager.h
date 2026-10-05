#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>
#include "FoodBank.h"

// [FILE-IO]
// Central persistence manager handling pipe-separated text storage with robust error and orphan recovery.
class FileManager {
public:
    static bool saveAll(const FoodBank& fb, const std::string& dataDir);
    static bool loadAll(FoodBank& fb, const std::string& dataDir);

    static bool saveDonors(const std::vector<Donor>& donors, const std::string& filepath);
    static bool loadDonors(FoodBank& fb, const std::string& filepath);

    static bool saveRecipients(const std::vector<RecipientOrganization>& recipients, const std::string& filepath);
    static bool loadRecipients(FoodBank& fb, const std::string& filepath);

    static bool saveDonations(const std::vector<Donation>& donations, const std::string& filepath);
    static bool loadDonations(FoodBank& fb, const std::string& filepath);

    static bool saveRequests(const std::vector<RecipientRequest>& requests, const std::string& filepath);
    static bool loadRequests(FoodBank& fb, const std::string& filepath);

    static bool saveDeliveries(const std::vector<Delivery>& deliveries, const std::string& filepath);
    static bool loadDeliveries(FoodBank& fb, const std::string& filepath);
};

#endif // FILEMANAGER_H
