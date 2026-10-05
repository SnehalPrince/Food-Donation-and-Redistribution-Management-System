# AGENTS.md — Food Donation & Redistribution Management System

## 1. Hard Constraints
- **Language:** Standard C++ only, STL only. Must compile with zero warnings under `g++ -Wall -Wextra -pedantic` for both `-std=c++11` and `-std=c++17`. Clang++ and MSVC friendly.
- **Forbidden:** No `<filesystem>`, no `std::optional`, no structured bindings, no generic lambdas, no `std::make_unique`. No `system("cls")`/`system("pause")`, no `windows.h`, no ANSI escape codes. Plain ASCII console only.
- **Layout:** Flat layout in project root: headers, sources, `data/`, `tests/`, `docs/`. No `src/` or `include/` folders.
- **OOP Requirements:**
  - *Encapsulation:* All data members private. Explicit constructors, const accessors/mutators. No global variables. `FoodBank` is sole owner of collections.
  - *Abstraction & Inheritance:* `Person` and `FoodItem` are abstract base classes with virtual destructors.
  - *Runtime Polymorphism:* Virtual functions called through base pointers/references (`Person::display()`, `FoodItem::isEligibleForDistribution()`, `FoodItem::display()`, `FoodItem::serializeDetails()`).
  - *STL Purpose:* `vector` (main collections), `map` (totals, counts, rankings), `set` (unique IDs), `queue` (pending request order), `sort` with comparator lambdas.
  - *File I/O:* Isolated exclusively in `FileManager`.
  - *Readability:* Functions <= 40 lines. Greppable tags required:
    `// [ENCAPSULATION]`, `// [ABSTRACTION]`, `// [INHERITANCE]`, `// [POLYMORPHISM]`, `// [STL: map]`, `// [STL: vector]`, `// [STL: set]`, `// [STL: queue]`, `// [FILE-IO]`.
- **Out of Scope:** No GUI, networking, databases, login, threads, external JSON/CSV libraries.

## 2. Binding Design Decisions (D1 - D10)
- **D1 (IDs):** String IDs with fixed prefixes: `D001` (donor), `R001` (recipient), `DON101` (donation), `REQ101` (request), `DEL001` (delivery). Max 12 chars, letters/digits/`_`/`-`, stored uppercase. Enter accepts suggested next free ID. Unique checked with `std::set<std::string>`.
- **D2 (Quantities):** Kilograms as `double`, 2 decimal display, 0 < q <= 100000. Single source of truth in `Donation` (FoodItem does not store quantity; Food ID == Donation ID). Double comparisons use shared `EPS = 1e-9`.
- **D3 (Dates):** `Date` class validates calendar, leap years, `YYYY-MM-DD`, comparisons, `daysBetween`. `FoodBank` takes `today` via parameter/setter; only `main` reads clock. CLI flags `--today YYYY-MM-DD` and `--data-dir <path>`. Food valid through expiry date inclusive. "Expiring soon" within N days (default 3).
- **D4 (Food Model):** 4 categories mapped to 2 subclasses:
  - `CookedFood` (COOKED, BAKERY): prep date required, prep <= today <= expiry. Max validity: COOKED = 2 days, BAKERY = 3 days.
  - `PackagedFood` (PACKAGED, PRODUCE): expiry date driven, optional batch/lot, today <= expiry.
  Factory function instantiates subclasses based on category string tag.
- **D5 (Polymorphic Surface):** `Person::display() const = 0`. `FoodItem::isEligibleForDistribution(const Date& today) const = 0`, `validate() const = 0`, `display() const = 0`, `serializeDetails() const = 0` (no `dynamic_cast` in `FileManager`).
- **D6 (Reserve & Confirm):** Matching reserves stock (`SCHEDULED` delivery, raises request allocated qty, lowers donation available qty). Menu 8 confirms delivery as `DELIVERED` or `CANCELLED` (reversing reservation). Eligibility re-checked before `DELIVERED`. Statuses are strictly derived, never hand-set:
  - Donation: `AVAILABLE`, `PARTIALLY_ALLOCATED`, `FULLY_ALLOCATED`, `EXPIRED`.
  - Request: `PENDING`, `PARTIALLY_FULFILLED`, `FULFILLED`, `CANCELLED`.
  All statuses recalculated from deliveries upon file loading.
- **D7 (Matching):** Candidates: same category, not EXPIRED, eligible today, available > EPS. Sorted by earliest expiry, then lower donation ID. Allocates `min(remaining_need, available)` across donations. Partial allocation supported.
- **D8 (Reports & Invariant):**
  - Invariant: `donated = redistributed (DELIVERED) + in_transit (SCHEDULED) + available + wasted (EXPIRED available)`.
  - Report asserts invariant: prints `Stock reconciliation: OK` or `MISMATCH`.
  - Fulfilment rate = `(FULFILLED / total_requests) * 100`.
  - SDG 2: kg delivered, delivery count, recipients served.
  - SDG 12: kg diverted (= delivered), kg lost (= wasted).
- **D9 (Files):** Plain text, pipe-separated (`|`), one per line: `donors.txt`, `recipients.txt`, `donations.txt`, `requests.txt`, `deliveries.txt`. Auto-load on start, missing files load empty, malformed lines skipped with warning, orphan references reported. Save with menu 12; offer save on exit if modified.
- **D10 (Menu & Input):** 13 top-level options. Option 6: pending queue (priority 1 High to 3 Low). Option 7: match chosen or next pending. Option 9: browse/search/sort sub-menu. Option 11: report sub-menu. All prompts via `InputHelper` (getline, robust parsing, EOF exit cleanly).

## 3. File Ownership Table
| Role | Files Owned | Exit Verification |
|---|---|---|
| **Lead** | `AGENTS.md`, `Makefile`, `make.bat`, `build.ps1`, `.gitignore`, all `*.h` (contract), `Date.*`, `InputHelper.*`, `tests/test_util.h`, `data/*.txt` | Headers compile standalone; `make` links |
| **People** | `Person.cpp`, `Donor.cpp`, `Recipient.cpp`, `tests/test_people.cpp` | `make test-people` |
| **Food & Donation** | `FoodItem.cpp`, `CookedFood.cpp`, `PackagedFood.cpp`, `Donation.cpp`, `tests/test_food.cpp` | `make test-food` |
| **Requests & Deliveries** | `RecipientRequest.cpp`, `Delivery.cpp`, `tests/test_requests.cpp` | `make test-requests` |
| **Persistence** | `FileManager.cpp`, `tests/test_files.cpp` | `make test-files` |
| **Core** | `FoodBank.cpp`, `tests/test_foodbank.cpp` | `make test-foodbank` |
| **Reports** | `ReportGenerator.cpp`, `tests/test_reports.cpp` | `make test-reports` |
| **Menu** | `main.cpp` | `make demo` |
| **QA** | `tests/` runners, inputs, `docs/TEST_REPORT.md` | `make test`, `make check-std` |
| **Reviewer** | Read-only analysis; `docs/REVIEW.md` | Verification against Plan.md |
| **Docs** | `README.md`, `docs/VIVA_NOTES.md`, `docs/DECISIONS.md` | Definition of Done complete |

## 4. Build & Test Commands
Headers are frozen after Wave 0. Subagents build privately using `BUILD_DIR=build/<role>`.
- Build application: `mingw32-make all` (or `make all`)
- Build & run tests: `mingw32-make test` (or `make test`)
- Run module test: `mingw32-make test-<module>` (e.g. `test-people`, `test-food`, etc.)
- Standards check: `mingw32-make check-std` (validates both `-std=c++11` and `-std=c++17`)
- Run Viva demo: `mingw32-make demo`
- Clean artifacts: `mingw32-make clean`
