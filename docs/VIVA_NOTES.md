# Viva & Interview Preparation Notes (docs/VIVA_NOTES.md)

This guide provides concise, exam-ready answers to every viva question in `Plan.md` §19, linking directly to the source code files and functions tagged with `[TAG]` markers.

---

## 1. Object-Oriented Programming (OOP)

### Q1: What is encapsulation, and where is it used?
**Answer:** Encapsulation is the bundling of data and the methods that operate on that data into a single unit (class), while hiding internal state from outside interference through access specifiers. In our system, all class fields across `Person`, `FoodItem`, `Donation`, `RecipientRequest`, `Delivery`, and `FoodBank` are strictly `private`. External components can only read or mutate state via validated member functions and const accessors.  
**Code Reference:** `Person.h` (`// [ENCAPSULATION]`), `Donation.h`, `FoodBank.h`.

### Q2: Why did you use inheritance?
**Answer:** Inheritance enables code reuse and models natural real-world "is-a" relationships. `Donor` and `RecipientOrganization` inherit common identity attributes (`id`, `name`, `contact`) from the abstract base class `Person`. Similarly, `CookedFood` and `PackagedFood` inherit base attributes (`name`, `category`, `preparationDate`, `expiryDate`) from `FoodItem` while extending behavior for their respective shelf-life models.  
**Code Reference:** `Donor.h` (`// [INHERITANCE]`), `Recipient.h`, `CookedFood.h`, `PackagedFood.h`.

### Q3: What is polymorphism, and where is runtime polymorphism used?
**Answer:** Polymorphism allows objects of different derived classes to be treated through a common base class interface, executing the appropriate derived behavior at runtime via dynamic dispatch. We use runtime polymorphism when iterating over collections of `const Person*` to call `display()`, and when evaluating food eligibility through `FoodItem::isEligibleForDistribution(today)` inside `Donation` and `FoodBank`. Derived classes override these methods without the caller knowing the concrete derived type.  
**Code Reference:** `Person.h:26` (`// [POLYMORPHISM]`), `CookedFood.cpp:11`, `PackagedFood.cpp:8`, `tests/test_people.cpp:32`.

### Q4: Why are member functions declared `virtual`, and why does the base class need a virtual destructor?
**Answer:** Declaring member functions `virtual` tells the compiler to use late/dynamic binding via a virtual method table (vtable) so the runtime type's implementation is called through base pointers. Base class destructors are declared `virtual` to ensure that when a derived object is deleted through a base class pointer (such as `std::shared_ptr<FoodItem>`), the derived destructor runs first followed by the base destructor, preventing resource leaks.  
**Code Reference:** `Person.h:17`, `FoodItem.h:21`.

---

## 2. Standard Template Library (STL)

### Q5: Why did you use `std::vector`?
**Answer:** `std::vector` provides contiguous, dynamically resizable storage with $O(1)$ random access and cache locality. It is used as the primary backbone collection inside `FoodBank` to store all registered donors, recipients, donations, requests, and deliveries.  
**Code Reference:** `FoodBank.h:30` (`// [STL: vector]`), `FoodBank.cpp:191`.

### Q6: Why did you use `std::map`?
**Answer:** `std::map` provides associative key-value storage implemented as a self-balancing Red-Black binary search tree with $O(\log N)$ lookup. It is used in `ReportGenerator` to aggregate donation totals per donor ID for rankings, and to accumulate distributed quantities by food category.  
**Code Reference:** `ReportGenerator.cpp:119` (`// [STL: map]`), `ReportGenerator.cpp:160`.

### Q7: Why did you use `std::set`?
**Answer:** `std::set` stores unique keys in logarithmic time, preventing duplicates. We use `std::set<std::string>` inside `FoodBank` to index registered donor IDs, recipient IDs, donation IDs, and request IDs, instantly rejecting duplicates.  
**Code Reference:** `FoodBank.h:37` (`// [STL: set]`), `FoodBank.cpp:52`.

### Q8: Why did you use `std::queue`?
**Answer:** `std::queue` provides strict First-In, First-Out (FIFO) semantics for order-dependent processing. In `FoodBank::getPendingRequestQueue()`, pending requests are prioritized (1 High to 3 Low) and enqueued so the matching engine redistributes surplus fairly in queue order.  
**Code Reference:** `FoodBank.h:68` (`// [STL: queue]`), `FoodBank.cpp:177`.

### Q9: Where did you use `std::sort()`?
**Answer:** `std::sort()` with custom comparator lambdas is used in `FoodBank::matchRequest()` to order available donations by earliest expiry date first (ensuring food closest to spoilage is redistributed first). It is also used in `ReportGenerator::generateDonorRanking()` to rank donors in descending order of donated kilograms.  
**Code Reference:** `FoodBank.cpp:218`, `ReportGenerator.cpp:138`.

---

## 3. File Handling & Persistence

### Q10: Why use files for persistence?
**Answer:** Text files provide zero-dependency, human-readable, cross-platform persistence that survives program restarts without requiring external database servers, drivers, or network connectivity.  
**Code Reference:** `FileManager.h:8` (`// [FILE-IO]`), `FileManager.cpp`.

### Q11: What happens when the program starts and when it saves?
**Answer:** On startup, `main()` calls `FileManager::loadAll()`, which reads `donors.txt`, `recipients.txt`, `donations.txt`, `requests.txt`, and `deliveries.txt`, populates `FoodBank`, and triggers `recalculateState()` to rebuild inventory balances from deliveries. When saving (Option 12 or on exit), `FileManager::saveAll()` serializes current collections into pipe-delimited records.  
**Code Reference:** `main.cpp:322`, `FileManager.cpp:265`.

### Q12: What happens if a data file does not exist or has malformed data?
**Answer:** If a file does not exist, `FileManager` handles it gracefully by initializing an empty collection without throwing exceptions or crashing. If a file contains malformed lines (e.g. missing pipes or invalid dates), the parser logs a descriptive warning naming the file and 1-based line number and safely skips the corrupted line.  
**Code Reference:** `FileManager.cpp:34`, `tests/test_files.cpp:66`.

---

## 4. Business & Domain Logic

### Q13: How does donation matching work?
**Answer:** A pending request specifies a food category and quantity. The matching engine filters donations that share the exact category, are currently eligible (`isEligibleForDistribution(today)` is true), are not expired, and have available stock $> 0$. It sorts candidate donations by earliest expiry date and greedily allocates $\min(\text{remaining\_need}, \text{available})$ from each until the request is fulfilled or stock is exhausted, creating a `SCHEDULED` delivery record for each batch.  
**Code Reference:** `FoodBank.cpp:189-239` (`FoodBank::matchRequest`).

### Q14: How do you prevent expired food from being distributed?
**Answer:** Protection is enforced at three levels: (1) new donations with an expiry date in the past are rejected at registration; (2) candidate matching filters out donations where `isEligibleForDistribution(today)` is false; (3) before confirming a delivery (Option 8), `FoodBank::confirmDelivery()` re-evaluates the food's eligibility on today's date, blocking delivery if the food spoiled while scheduled.  
**Code Reference:** `FoodBank.cpp:94`, `FoodBank.cpp:202`, `FoodBank.cpp:273`.

### Q15: How are quantities updated, and what is the stock reconciliation invariant?
**Answer:** Quantity lives exclusively in `Donation` (the single source of truth). Matching reserves stock (lowering available quantity without reducing total quantity); delivery confirmation finalizes it. The stock reconciliation invariant mathematically guarantees that after every transaction:
$$\text{Total Donated} = \text{Redistributed} + \text{In-Transit} + \text{Available In Stock} + \text{Wasted (Expired)}$$
Our system verifies this invariant within $10^{-9}$ double precision and prints `Stock reconciliation: OK`.  
**Code Reference:** `Donation.cpp:34`, `ReportGenerator.cpp:55`, `tests/test_foodbank.cpp:142`.

### Q16: How is the fulfillment percentage calculated?
**Answer:** $\text{Fulfillment Rate} = \frac{\text{Fulfilled Requests}}{\text{Total Requests}} \times 100$, guarded with a zero-division check returning $0.0\%$ if no requests exist. Requests with partial fulfillment or awaiting delivery are counted separately as pending.  
**Code Reference:** `ReportGenerator.cpp:48`.

---

## 5. Viva Demo Script & Exact Keystrokes

This sequence executes the Plan.md §18 scenario step-by-step:

```text
1              -> Option 1: Register Donor
D001           -> Donor ID (press Enter to accept D001)
MAIT Canteen   -> Donor Name
canteen@mait.edu -> Contact Info
Canteen        -> Source Type

3              -> Option 3: Add Food Donation
DON101         -> Donation ID
D001           -> Donor ID
Cooked Meals   -> Food Item Name
1              -> Category: 1 = Cooked Food
50             -> Quantity: 50 kg
[Enter]        -> Preparation Date (accept default today: 2026-10-05)
2026-10-07     -> Expiry Date

2              -> Option 2: Register Recipient Organization
R001           -> Recipient ID
Community Shelter -> Organization Name
shelter@community.org -> Contact Info
Sector 14      -> Location

5              -> Option 5: Add Recipient Requirement
REQ101         -> Request ID
R001           -> Recipient ID
1              -> Category: 1 = Cooked Food
30             -> Quantity: 30 kg
1              -> Priority: 1 = High

7              -> Option 7: Match Donation
1              -> Sub-choice: 1 = Match next pending request
                  [Expected Output: Allocated 30 kg across 1 delivery record(s). Balance pending: 0 kg.]

8              -> Option 8: Record Delivery
DEL001         -> Delivery ID
1              -> Action: 1 = Mark as DELIVERED

11             -> Option 11: Reports Sub-Menu
1              -> Sub-choice: 1 = Full Summary & SDG Impact Report
                  [Expected Output:
                   Total Food Donated: 50.00 kg
                   Total Food Redistributed: 30.00 kg
                   Available In Stock: 20.00 kg
                   Stock reconciliation: OK
                   SDG 2 Diverted: 30.00 kg]

12             -> Option 12: Save Data
13             -> Option 13: Exit
```
These keystrokes match `tests/demo_input.txt` exactly.
