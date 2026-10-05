# Changelog

All notable changes to the **Food Donation & Redistribution Management System** will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [1.0.0] - 2026-10-05

### Added
- **Entities & Architecture (Wave 0 & 1):**
  - Abstract base classes `Person` and `FoodItem` with virtual destructors.
  - Concrete domain entities: `Donor`, `RecipientOrganization`, `CookedFood`, `PackagedFood`, `Donation`, `RecipientRequest`, and `Delivery`.
  - Polymorphic interfaces: `Person::display()`, `FoodItem::isEligibleForDistribution()`, `FoodItem::display()`, `FoodItem::serializeDetails()`.
  - Date engine (`Date`) supporting calendar validation, leap years, string parsing (`YYYY-MM-DD`), comparisons, and `daysBetween`.
  - Robust console input parser (`InputHelper`) enforcing numeric bounds, rejection of pipe characters, trimming, and clean EOF termination.
- **Redistribution Core & Invariants (Wave 2):**
  - Central controller `FoodBank` managing collections (`std::vector`), unique identifiers (`std::set`), and pending queues (`std::queue`).
  - Two-phase redistribution algorithm (Earliest-Expiry-First matching reserves stock via `SCHEDULED` deliveries; delivery confirmation transitions to `DELIVERED` or cancellation restores inventory).
  - Mathematical stock reconciliation asserting:
    $$\text{Total Donated} = \text{Redistributed} + \text{In-Transit} + \text{Available} + \text{Wasted (Expired)}$$
  - Report generator (`ReportGenerator`) calculating live metrics, donor rankings, category breakdowns, expiry alerts, and United Nations Sustainable Development Goals (SDG 2 and SDG 12) impact statistics.
- **Persistence & CLI (Wave 2 & 3):**
  - Pipe-delimited file persistence (`FileManager`) across `donors.txt`, `recipients.txt`, `donations.txt`, `requests.txt`, and `deliveries.txt`.
  - Deterministic command-line arguments: `--today YYYY-MM-DD` and `--data-dir <path>`.
  - 13-option interactive console menu.
- **Comprehensive Quality Assurance (Wave 3):**
  - Unit test runners for people, food, requests, files, foodbank, and reports.
  - End-to-end (E2E) integration test runner simulating two-phase redistribution, file re-loading, and persistence roundtrips.
  - Robustness fuzzing suite testing negative numbers, invalid dates, malformed files, and EOF streams.
  - Zero-warning verification across both `-std=c++11` and `-std=c++17` using `g++ -Wall -Wextra -pedantic`.
- **Academic & Defense Documentation (Wave 4 & 5):**
  - Comprehensive `README.md` with architectural Mermaid diagrams, build instructions, and sample runs.
  - `docs/VIVA_NOTES.md` providing 22 anticipated examiner viva questions with referenced answers.
  - `docs/DECISIONS.md` logging binding architectural decisions (D1–D10).
  - `docs/REVIEW.md` and `docs/TEST_REPORT.md` validating 100% compliance with requirements.
