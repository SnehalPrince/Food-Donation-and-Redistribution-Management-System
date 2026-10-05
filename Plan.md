# Plan.md — C++ OOP Mini Project

# Food Donation and Redistribution Management System

## 1. Final Project Choice

**Project:** Food Donation and Redistribution Management System

**Type:** C++ Object-Oriented Programming mini project  
**Interface:** Console / menu-driven  
**Hardware:** None  
**Database:** None  
**External libraries:** None  
**Persistence:** Text-file handling  
**Difficulty:** Medium

### Why this is our best choice

This project fits the required C++ OOP scope while remaining realistic to finish. It naturally supports:

- Classes and objects
- Encapsulation
- Inheritance
- Polymorphism
- STL
- File handling
- Searching/sorting
- Data processing
- Summary/report generation

The project also has a clear real-world workflow instead of being only a basic CRUD application.

---

# 2. Project Goal

Build a software system that manages surplus food from donors and helps redistribute it to recipient organizations.

### Main flow

```text
Donor
  ↓
Food Donation Registered
  ↓
Availability + Quantity + Expiry Checked
  ↓
Recipient Requirement Added
  ↓
Suitable Donation Matched
  ↓
Delivery Recorded
  ↓
Inventory Updated
  ↓
Reports Generated
```

---

# 3. Problem We Are Solving

Food surplus from restaurants, canteens, events, households, and other sources may be wasted because donors and organizations that need food are not properly coordinated.

Our system will provide a structured way to:

1. Register donors.
2. Register recipient organizations.
3. Record food donations.
4. Track quantity and expiry.
5. Record recipient requirements.
6. Match suitable donations with requirements.
7. Record deliveries.
8. Update remaining quantities.
9. Generate useful statistics and reports.

---

# 4. Project Scope

## Core Features — MUST BUILD

These are the features required for the first complete version.

### A. Donor Management

- Add donor
- View donors
- Search donor by ID
- Prevent duplicate donor IDs

Suggested fields:

```text
Donor ID
Name
Contact
Organization / Source
```

---

### B. Recipient Management

- Add recipient organization
- View recipients
- Search recipient by ID
- Prevent duplicate recipient IDs

Suggested fields:

```text
Recipient ID
Organization Name
Contact
Location
```

---

### C. Food Donation Management

- Add donation
- View all donations
- Search donation by ID
- Filter by food category
- Track available quantity
- Track donation status

Suggested fields:

```text
Donation ID
Donor ID
Food Name
Food Type
Quantity
Preparation Date
Expiry Date
Status
```

Possible food types:

```text
Cooked Food
Packaged Food
Bakery
Fruits & Vegetables
```

---

### D. Recipient Requirement Management

- Add requirement
- View pending requirements
- Search requirement
- Set priority

Suggested fields:

```text
Request ID
Recipient ID
Food Type / Category
Required Quantity
Priority
Status
```

Priority:

```text
1 = High
2 = Medium
3 = Low
```

---

### E. Donation Matching

This is one of the main features.

The system should find a donation that:

- Is available
- Has matching food type/category
- Has sufficient quantity
- Has not expired

Example:

```text
Available:
D101 → Cooked Food → 50 kg

Request:
R201 → Cooked Food → 30 kg

Match:
30 kg allocated
20 kg remains
```

---

### F. Delivery Management

- Create delivery record
- Record delivered quantity
- Update donation quantity
- Mark request as fulfilled when completed
- Keep delivery history

Suggested fields:

```text
Delivery ID
Donation ID
Request ID
Quantity
Date
Status
```

---

### G. Reports

Generate a summary report containing:

```text
Total food donated
Total food redistributed
Total remaining food
Total deliveries
Pending requests
Fulfillment rate
Food wastage / unavailable quantity
Category-wise totals
Top donor
Expiring-soon donations
```

---

# 5. OOP Design

## Core Class Structure

```text
                    Person
                    /    \
                 Donor   RecipientOrganization

                   FoodItem
                  /       \
          CookedFood    PackagedFood

Donation
RecipientRequest
Delivery
FoodBank
ReportGenerator
FileManager
```

---

## 5.1 Person — Base Class

Common identity information.

```cpp
class Person {
protected:
    int id;
    string name;
    string contact;

public:
    virtual void display() const = 0;
    virtual ~Person() = default;
};
```

Purpose:

- Demonstrates abstraction
- Provides common attributes
- Creates a clean base for derived classes

---

## 5.2 Donor — Derived Class

Inherits from `Person`.

Responsibilities:

- Store donor information
- Display donor data

```text
Person
   ↓
Donor
```

---

## 5.3 RecipientOrganization — Derived Class

Can also inherit from `Person`, or use a shared user/entity design.

Responsibilities:

- Store organization details
- Store location/contact
- Display recipient information

---

## 5.4 FoodItem — Base Class

Stores common food properties.

Suggested data:

```text
Food ID
Food Name
Quantity
Preparation Date
Expiry Date
Category
```

Use a virtual function such as:

```cpp
virtual bool isEligibleForDistribution() const = 0;
```

---

## 5.5 CookedFood — Derived Class

Specialized food type.

Possible rules:

- Must have a valid preparation date
- Usually has a shorter validity period
- Cannot be distributed after expiry

---

## 5.6 PackagedFood — Derived Class

Specialized food type.

Possible rules:

- Use expiry date
- Can store batch/package information
- Different validation from cooked food

### Why this inheritance is useful

The derived classes can implement different validation/eligibility behavior. This gives us **real polymorphism**, not inheritance added only to satisfy the syllabus.

---

# 6. Supporting Classes

## Donation

Represents a food donation transaction.

Contains:

```text
Donation ID
Donor ID
Food information
Quantity
Dates
Status
```

Suggested statuses:

```text
AVAILABLE
PARTIALLY_ALLOCATED
FULLY_ALLOCATED
EXPIRED
```

---

## RecipientRequest

Represents a request from a recipient.

Contains:

```text
Request ID
Recipient ID
Food category
Required quantity
Priority
Status
```

Possible statuses:

```text
PENDING
PARTIALLY_FULFILLED
FULFILLED
CANCELLED
```

---

## Delivery

Represents the final redistribution event.

Contains:

```text
Delivery ID
Donation ID
Request ID
Quantity
Date
Status
```

---

## FoodBank

Acts as the central manager/controller.

Responsibilities:

- Store donors
- Store recipients
- Store donations
- Store requests
- Store deliveries
- Perform matching
- Update records
- Call report functions

This should contain most of the application logic.

---

## ReportGenerator

Responsible only for generating reports.

Examples:

```text
generateSummary()
generateDonorReport()
generateCategoryReport()
generatePendingRequestReport()
generateExpiryReport()
```

This keeps reporting separate from core business logic.

---

## FileManager

Responsible for persistence.

Responsibilities:

- Save donors
- Load donors
- Save recipients
- Load recipients
- Save donations
- Load donations
- Save requests
- Load requests
- Save deliveries
- Load deliveries

This prevents file I/O code from being scattered throughout the program.

---

# 7. STL Usage

Use STL where it makes sense.

## vector

```cpp
vector<Donor> donors;
vector<RecipientOrganization> recipients;
vector<Donation> donations;
vector<RecipientRequest> requests;
vector<Delivery> deliveries;
```

Use it for the main collections.

---

## map

Examples:

```cpp
map<string, double> categoryTotals;
map<int, int> donorDonationCount;
```

Use maps for summaries and fast lookup by key.

---

## set

Use it for unique IDs or categories.

```cpp
set<int> donorIds;
set<int> recipientIds;
set<string> foodCategories;
```

---

## queue

Use for pending delivery/request processing.

```cpp
queue<int> pendingRequests;
```

Do not add containers just to show STL. Each container should have a clear purpose.

---

## sort

Use sorting for reports such as:

- Top donors
- Largest donations
- Highest-requested food categories
- Largest remaining quantities

Example:

```cpp
sort(donations.begin(), donations.end(), comparator);
```

---

# 8. File Structure

A clean implementation can use this structure:

```text
FoodDonationSystem/
│
├── main.cpp
│
├── Person.h
├── Donor.h
├── Recipient.h
├── FoodItem.h
├── CookedFood.h
├── PackagedFood.h
│
├── Donation.h
├── RecipientRequest.h
├── Delivery.h
│
├── FoodBank.h
├── ReportGenerator.h
├── FileManager.h
│
├── Person.cpp
├── Donor.cpp
├── Recipient.cpp
├── FoodItem.cpp
├── CookedFood.cpp
├── PackagedFood.cpp
├── Donation.cpp
├── RecipientRequest.cpp
├── Delivery.cpp
├── FoodBank.cpp
├── ReportGenerator.cpp
├── FileManager.cpp
│
└── data/
    ├── donors.txt
    ├── recipients.txt
    ├── donations.txt
    ├── requests.txt
    └── deliveries.txt
```

### Simpler alternative

If your teacher expects a smaller submission, start with:

```text
main.cpp
classes.h
classes.cpp
data/
```

Then split into more files only if needed.

---

# 9. Main Menu

Target menu:

```text
=============================================
   FOOD DONATION & REDISTRIBUTION SYSTEM
=============================================

1. Register Donor
2. Register Recipient
3. Add Food Donation
4. View Donations
5. Add Recipient Requirement
6. View Pending Requests
7. Match Donation
8. Record Delivery
9. Search Records
10. Show Expiring Donations
11. Generate Summary Report
12. Save Data
13. Exit

Enter choice:
```

---

# 10. Search Features

At minimum support:

### Search donation

- By donation ID
- By donor ID
- By food category

### Search recipient

- By recipient ID
- By name

### Search request

- By request ID
- By recipient ID
- By status

---

# 11. Matching Logic

## Basic matching algorithm

```text
Input Request
      ↓
Find PENDING request
      ↓
Search available donations
      ↓
Check category/type match
      ↓
Check expiry
      ↓
Check available quantity
      ↓
Select suitable donation
      ↓
Allocate required quantity
      ↓
Update donation quantity
      ↓
Update request quantity/status
      ↓
Create delivery record
```

### Matching rule

Start simple:

1. Match food category.
2. Ignore expired donations.
3. Prefer donations expiring sooner.
4. If multiple donations are valid, select the one with the earliest expiry.
5. Allocate only the quantity required.

This makes the project more interesting without requiring advanced algorithms.

---

# 12. Report Calculations

These calculations should be implemented as functions.

## Total donated

```text
sum(all registered donation quantities)
```

## Total redistributed

```text
sum(all successful delivery quantities)
```

## Remaining quantity

```text
total donated - total redistributed
```

## Fulfillment rate

```text
fulfilled requests / total requests × 100
```

## Category-wise distribution

```text
Cooked Food       → 520 kg
Packaged Food     → 310 kg
Bakery            → 140 kg
Fruits            → 110 kg
```

## Donor ranking

Example:

```text
1. ABC Restaurant    280 kg
2. MAIT Canteen      210 kg
3. XYZ Bakery        165 kg
```

## Expiring-soon report

Display donations whose expiry date is close.

For the first version, a simple date-string comparison or a controlled date format such as:

```text
YYYY-MM-DD
```

is enough. Avoid building a complicated calendar system unless required.

---

# 13. Development Roadmap

## Phase 1 — Project Setup

Create:

```text
main.cpp
headers
source files
data/
```

Implement:

- Basic menu
- `Person`
- `Donor`
- `RecipientOrganization`

### Goal

Program compiles and can add/display donors and recipients.

---

## Phase 2 — Food Model

Implement:

- `FoodItem`
- `CookedFood`
- `PackagedFood`

Add:

- Inheritance
- Virtual functions
- Food validation

### Goal

Demonstrate polymorphism successfully.

---

## Phase 3 — Donation Module

Implement:

- `Donation`
- Add donation
- View donation
- Search donation
- Update quantity/status

### Goal

Complete donation management.

---

## Phase 4 — Recipient Requests

Implement:

- `RecipientRequest`
- Add request
- View pending requests
- Priority

### Goal

System can represent demand as well as supply.

---

## Phase 5 — Matching Engine

Implement:

- Match request with donation
- Check expiry
- Check category
- Check quantity
- Allocate food

### Goal

Complete the central business logic.

---

## Phase 6 — Delivery Module

Implement:

- `Delivery`
- Record delivery
- Update donation quantity
- Update request status

### Goal

Complete end-to-end redistribution.

---

## Phase 7 — STL + Reports

Implement:

- `map`
- `set`
- `queue`
- `sort`
- Summary calculations

### Goal

Make STL usage visible and meaningful.

---

## Phase 8 — File Handling

Implement:

```text
save()
load()
```

for all major entities.

### Goal

Data should survive after closing and reopening the program.

---

## Phase 9 — Validation

Add checks for:

- Duplicate IDs
- Negative quantities
- Zero quantities
- Invalid menu choices
- Invalid dates
- Missing donor IDs
- Missing recipient IDs
- Expired donations
- Insufficient available quantity

### Goal

Make the program reliable instead of just functional.

---

## Phase 10 — Final Polish

Add:

- Clear console formatting
- Consistent menus
- Error messages
- Confirmation messages
- Better report formatting
- Sample data
- Comments for important OOP logic

---

# 14. MVP vs Extra Features

## MVP — Finish these first

```text
[ ] Donor registration
[ ] Recipient registration
[ ] Donation registration
[ ] Request registration
[ ] Donation matching
[ ] Delivery recording
[ ] Quantity updates
[ ] File saving/loading
[ ] Summary report
[ ] Search
```

## Extra Features — Add only after MVP works

```text
[ ] Priority-based request matching
[ ] Earliest-expiry-first allocation
[ ] Donor ranking
[ ] Category statistics
[ ] Expiring-soon alerts
[ ] Partial fulfillment
[ ] Better report formatting
[ ] Sample/demo dataset
```

Do NOT add GUI, networking, database, login systems, APIs, or advanced optimization before the core console version is complete.

---

# 15. Suggested Functions

Possible function list:

```cpp
// Donors
addDonor();
displayDonors();
searchDonor();

// Recipients
addRecipient();
displayRecipients();
searchRecipient();

// Donations
addDonation();
displayDonations();
searchDonation();
showExpiringDonations();

// Requests
addRequest();
displayPendingRequests();

// Matching
matchDonation();

// Delivery
recordDelivery();

// Reports
generateSummary();
generateDonorReport();
generateCategoryReport();

// Persistence
saveAllData();
loadAllData();
```

Keep functions small. Avoid putting the whole project inside `main()`.

---

# 16. Coding Rules for the Project

1. Use `private` data members wherever possible.
2. Use constructors to initialize objects.
3. Use getters/setters where appropriate.
4. Use inheritance only where it makes sense.
5. Use `virtual` functions to demonstrate polymorphism.
6. Keep business logic inside manager classes.
7. Keep file handling inside `FileManager`.
8. Avoid global variables.
9. Avoid huge functions.
10. Compile and test after every module.

---

# 17. Testing Plan

Before final submission, test these cases:

### Donor tests

```text
Add valid donor → should succeed
Duplicate donor ID → should fail
```

### Donation tests

```text
Positive quantity → succeed
Zero quantity → reject
Negative quantity → reject
Unknown donor ID → reject
Expired donation → cannot be matched
```

### Request tests

```text
Valid request → succeed
Unknown recipient → reject
Invalid quantity → reject
```

### Matching tests

```text
Matching category + enough quantity → success
Matching category + insufficient quantity → partial allocation / pending balance
Wrong category → no match
Expired donation → no match
```

### File tests

```text
Save → close → reopen → records should still exist
```

---

# 18. Demo Scenario for Viva

Prepare one clean demonstration.

### Step 1

Register:

```text
Donor:
D001 - MAIT Canteen
```

### Step 2

Add:

```text
Donation:
DON101
Cooked Food
50 kg
```

### Step 3

Register:

```text
Recipient:
R001 - Community Shelter
```

### Step 4

Add request:

```text
REQ101
Cooked Food
30 kg
High Priority
```

### Step 5

Run matching.

Expected:

```text
30 kg matched successfully
20 kg remaining
```

### Step 6

Record delivery.

### Step 7

Generate report.

This one scenario demonstrates most of the project in a few minutes.

---

# 19. What We Need to Be Able to Explain in Viva

Be ready to answer:

### OOP

- What is encapsulation?
- Where is encapsulation used?
- Why did you use inheritance?
- What is polymorphism?
- Where is runtime polymorphism used?
- Why are functions virtual?

### STL

- Why `vector`?
- Why `map`?
- Why `set`?
- Why `queue`?
- Where did you use `sort()`?

### File Handling

- Why use files?
- What happens when the program starts?
- How are records saved?
- How are records loaded?
- What happens if a file does not exist?

### Project Logic

- How does donation matching work?
- How do you prevent expired food from being distributed?
- How are quantities updated?
- How is the fulfillment percentage calculated?

---

# 20. Final Architecture

The overall architecture should look like this:

```text
                    +----------------+
                    |    main.cpp    |
                    +--------+-------+
                             |
                             v
                    +----------------+
                    |    FoodBank    |
                    +--------+-------+
                             |
       +---------------------+----------------------+
       |                     |                      |
       v                     v                      v
   Donors             Donations/Requests        Deliveries
       |                     |                      |
       +---------------------+----------------------+
                             |
                             v
                    +----------------+
                    | ReportGenerator|
                    +----------------+

                    +----------------+
                    |  FileManager   |
                    +----------------+
                             |
                             v
                         data/*.txt
```

---

# 21. Final Definition of the Project

> **The Food Donation and Redistribution Management System is a console-based C++ OOP application that manages food donors, recipient organizations, food donations, recipient requirements, matching, redistribution, inventory updates, and reports using classes, inheritance, polymorphism, STL containers, and file handling.**

---

# 22. Recommended Build Order

**Do not try to code everything at once.**

Build in exactly this order:

```text
1. Person
2. Donor
3. RecipientOrganization
4. FoodItem
5. CookedFood
6. PackagedFood
7. Donation
8. RecipientRequest
9. Delivery
10. FoodBank
11. Matching logic
12. ReportGenerator
13. FileManager
14. Validation
15. Final menu + polish
```

### Golden rule

**First make it work. Then make it clean. Then make it impressive.**

Do not add advanced features until the basic donation → request → match → delivery flow works correctly.
