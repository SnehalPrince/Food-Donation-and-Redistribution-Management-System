# Test Execution & Verification Report

**Project:** Food Donation and Redistribution Management System  
**Date:** 2026-10-05  
**Compiler:** GCC 16.1.0 (`x86_64-w64-mingw32`, MinGW-W64 UCRT)  
**Host OS:** Microsoft Windows  

---

## 1. Executive Summary

All unit test suites, integration tests, mathematical invariant checks (200 random operations), end-to-end (E2E) workflow tests, and robustness stress tests executed and passed with 100% success rate. The codebase compiled with zero warnings under both `-std=c++11` and `-std=c++17` with `-Wall -Wextra -pedantic`.

---

## 2. Evidence Table: Observed Command Outputs

In accordance with the evidence rule, all entries below reflect commands executed directly in this environment:

| Test / Command | Scope / Coverage | Real Observed Result | Status |
|---|---|---|---|
| `g++ -Wall -Wextra -pedantic -std=c++11 -x c++ -c <header>` | Tested all 14 header files independently | All 14 headers compiled standalone with zero warnings | **PASS** |
| `mingw32-make test-people` | `Person`, `Donor`, `RecipientOrganization`, polymorphic pointers | 3/3 tests passed | **PASS** |
| `mingw32-make test-food` | `CookedFood`, `PackagedFood`, validity rules, boundary conditions, `Donation` stock reserve/release | 4/4 tests passed | **PASS** |
| `mingw32-make test-requests` | `RecipientRequest`, priority ordering, `Delivery` status transitions | 3/3 tests passed | **PASS** |
| `mingw32-make test-files` | Pipe-delimited file round-trip, missing file handling, malformed lines | 3/3 tests passed | **PASS** |
| `mingw32-make test-foodbank` | Core matching engine, earliest expiry, delivery reversal, 200 random ops invariant stress test | 5/5 tests passed | **PASS** |
| `mingw32-make test-reports` | Metrics calculation, reconciliation, donor rankings, SDG 2 and SDG 12 blocks | 1/1 tests passed | **PASS** |
| `mingw32-make test-e2e` | End-to-end Plan.md §18 scenario, state persistence check, robustness & EOF handling | 3/3 tests passed | **PASS** |
| `mingw32-make check-std` | Full rebuild and test execution under both `-std=c++11` and `-std=c++17` | Zero warnings in both standards; all tests passed | **PASS** |
| `mingw32-make demo` | Plan.md §18 Viva Scenario with scripted stdin | Matched 30.00 kg, remaining 20.00 kg, stock reconciliation OK | **PASS** |
| `mingw32-make asan` | AddressSanitizer and UndefinedBehaviorSanitizer | MinGW linker error: `cannot find -lasan / -lubsan` (not distributed with this MinGW UCRT build) | **DOCUMENTED UNAVAILABLE** |

---

## 3. Detailed Test Module Breakdown

### 3.1 People Module (`test_people.cpp`)
- `testDonorBasics`: Verifies donor instantiation, encapsulated fields, and polymorphic virtual call via `const Person*`.
- `testRecipientBasics`: Verifies recipient organization fields and polymorphic virtual dispatch.
- `testPolymorphicArray`: Demonstrates runtime dynamic dispatch over an array of `const Person*` base pointers.

### 3.2 Food & Donation Module (`test_food.cpp`)
- `testCookedFoodValidity`: Asserts maximum shelf life: COOKED max 2 days, BAKERY max 3 days. Rejects preparation date after expiry date.
- `testEligibilityBoundaries`: Asserts inclusive eligibility window: `prepDate <= today <= expiryDate`. Asserts packaged food eligibility: `today <= expiryDate`.
- `testFoodFactory`: Verifies polymorphic factory construction without client downcasting.
- `testDonationStatusAndReservation`: Verifies quantity reservation and release mechanics, over-reservation protection, and derived statuses (`AVAILABLE`, `PARTIALLY_ALLOCATED`, `FULLY_ALLOCATED`, `EXPIRED`).

### 3.3 Requests & Deliveries Module (`test_requests.cpp`)
- `testRequestLifecycle`: Tracks `REQUEST_PENDING` -> `REQUEST_PARTIALLY_FULFILLED` -> `REQUEST_FULFILLED`.
- `testRequestCancellation`: Verifies cancellation allowed when no scheduled deliveries exist, but prevented when active stock is allocated.
- `testDeliveryTransitions`: Validates state machine from `SCHEDULED` to `DELIVERED` or `CANCELLED`.

### 3.4 Persistence Module (`test_files.cpp`)
- `testFileRoundTrip`: Saves donors, recipients, donations, requests, and deliveries to disk, creates a new `FoodBank` instance, loads all files, and asserts state equality.
- `testMissingFilesHandling`: Verifies loading from an empty or non-existent path initializes empty data without crashing.
- `testMalformedLinesSkipped`: Injects corrupted records without pipes; asserts parser logs file and line warning and cleanly skips corrupt entries.

### 3.5 FoodBank Core & Invariant Stress Test (`test_foodbank.cpp`)
- `testExactMatchingScenario`: Asserts 50 kg donation + 30 kg request -> 30 kg allocated, 20 kg remaining.
- `testEarliestExpiryMatching`: Asserts matching engine prioritizes donations expiring sooner.
- `testDeliveryCancellationReversal`: Verifies cancelling a `SCHEDULED` delivery restores available stock to donation and raises remaining need in request.
- `testExpiredFoodDeliveryBlocked`: Verifies food past expiry cannot be confirmed as delivered even if scheduled earlier.
- `testStockReconciliationInvariantStress`: Executes **200 seeded random operations** (add donation, add request, match, deliver, cancel) and asserts after every single operation:
  $$\text{donated} = \text{redistributed} + \text{in\_transit} + \text{available} + \text{wasted} \quad (\pm 10^{-9})$$
  Result: 200/200 checks reconciled perfectly.

### 3.6 Reports Module (`test_reports.cpp`)
- Verifies `StockMetrics` reconciliation flag (`isReconciled == true`).
- Verifies SDG 2: Zero Hunger metrics (redistributed kg, delivery missions, recipients served).
- Verifies SDG 12: Responsible Consumption metrics (kg diverted from landfill, kg lost to expiry).
- Verifies donor ranking sorting (`std::map` aggregation + `std::sort` descending).

### 3.7 E2E & Robustness Suite (`test_e2e.cpp`)
- `testVivaScenarioE2E`: Full process execution with simulated stdin matching Plan.md §18. Verifies stdout contains expected numbers. A subsequent execution against the populated data directory verifies that state persisted.
- `testRobustnessGarbageInput`: Passes negative numbers, letters where numbers expected, 10,000-character line strings, pipe characters (`|`), and invalid calendar dates (`2026-02-30`). The program rejected all invalid inputs and terminated cleanly with code 0.
- `testEofHandling`: Simulates immediate EOF on standard input; verifies graceful exit without hanging.
