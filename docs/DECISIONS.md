# Architectural & Binding Design Decisions (docs/DECISIONS.md)

This document records all deliberate binding decisions, deviations from initial snippets in `Plan.md`, and technical resolutions adopted during implementation.

---

## 1. Binding Design Decisions & Deliberate Deviations from Plan.md

1. **D1 — String-Based Prefixed IDs:**  
   Plan.md §5.1 / §7 suggested `int id`. Superseded by string IDs with domain type prefixes (`D001`, `R001`, `DON101`, `REQ101`, `DEL001`). Enforced unique via `std::set<std::string>` and case-insensitive alphanumeric validation (max 12 characters). Next free ID suggested on pressing Enter.
2. **D2 — Single Source of Truth for Quantities:**  
   Plan.md §5.4 / §6 included quantity in both `FoodItem` and `Donation`. Resolved so quantity lives exclusively in `Donation` as `double` kilograms (2 decimals, $0 < q \le 100000$). Food ID equals Donation ID. Double comparisons use shared `EPS = 1e-9` helper.
3. **D3 — Encapsulated Date Class & Dependency Injection:**  
   Plan.md suggested simple strings and system clock calls inside classes. Superseded by a self-contained `Date` class with full calendar and leap-year validation. `FoodBank` receives `today` via constructor/setter. CLI supports `--today YYYY-MM-DD` and `--data-dir <path>`. Food is valid through its expiry date, inclusive.
4. **D4 — Food Category Inheritance Hierarchy:**  
   Plan.md §4.C listed four standalone types. Mapped onto exactly two required polymorphic subclasses: `CookedFood` (covers COOKED and BAKERY, with max shelf-life of 2 and 3 days respectively; prep date required and cannot be future) and `PackagedFood` (covers PACKAGED and PRODUCE, driven by expiry date with optional batch/lot). Built via a static factory method. Donations past expiry are rejected at registration.
5. **D5 — Polymorphic Method Signatures:**  
   Plan.md specified parameterless `virtual bool isEligibleForDistribution() const = 0`. Updated to `isEligibleForDistribution(const Date& today) const` so eligibility dynamically evaluates against the injected date. `display() const` and `serializeDetails() const` are pure virtual, eliminating downcasting or `dynamic_cast` in `FileManager`.
6. **D6 — Two-Phase Transactional Redistribution (Reserve, Then Confirm):**  
   Plan.md directly deducted quantities on match. Superseded by a two-phase workflow: matching reserves stock (`SCHEDULED` delivery, raises request allocated qty, lowers donation available qty); Option 8 confirms delivery as `DELIVERED` or `CANCELLED` (reversing reservation exactly). Re-checks food eligibility before confirming delivery. All statuses are 100% derived from quantities and delivery records, never set manually.
7. **D7 — Multi-Donation & Earliest-Expiry-First Matching:**  
   Plan.md assumed single donation match. Superseded by greedy allocation across eligible candidate donations sorted by earliest expiry date (then lower donation ID), supporting partial allocations (`PARTIALLY_FULFILLED`) when available stock is less than required.
8. **D8 — Mathematical Stock Reconciliation Invariant & SDG Auditing:**  
   Plan.md suggested simple $donated - redistributed = remaining$. Superseded by rigorous accounting invariant:  
   $$\text{donated} = \text{redistributed} + \text{in\_transit} + \text{available} + \text{wasted} \quad (\pm 10^{-9})$$  
   Summary report prints `Stock reconciliation: OK` or `MISMATCH`. Includes dedicated impact blocks for **SDG 2 (Target 2.1)** and **SDG 12 (Target 12.3)** using verified real numbers.
9. **D9 — Plain Text Pipe-Delimited Persistence:**  
   Storage isolated exclusively in `FileManager` across `donors.txt`, `recipients.txt`, `donations.txt`, `requests.txt`, `deliveries.txt`. Pipe characters (`|`) and line breaks are strictly sanitized at input. Missing files load as empty without crashing; malformed lines are skipped with line-numbered warnings; orphan references are logged. Unsaved changes trigger a save prompt on exit.
10. **D10 — Menu Architecture & Safe Console Input:**  
    Plan.md relied on raw `cin >>`. Superseded by `InputHelper`, reading whole lines via `std::getline`, robust numerical/date parsing, and clean EOF termination without looping. 13-option top-level menu strictly matches Plan.md §9, with sub-menus for search/browse (Option 9) and reports (Option 11).

---

## 2. Technical & Toolchain Resolutions

1. **Windows MinGW `make` Alias:**  
   `make` is not bundled by default on MinGW-w64, which uses `mingw32-make`. Provided root `make.bat` wrapper and `build.ps1` so `make` works seamlessly across PowerShell and CMD.
2. **AddressSanitizer Availability on Windows:**  
   `make asan` failed with `cannot find -lasan` / `cannot find -lubsan`. Standard MinGW-w64 UCRT builds do not package GCC sanitizer static runtime libraries on Windows. As per Section 7 and 10 of the Master Prompt, this is documented as unavailable on this host while all code remains standard compliant.
3. **Execution Path in Windows CMD:**  
   When piping stdin via `<`, Windows CMD misinterprets forward-slash paths like `build/foodbank.exe < input.txt`. Solved in `Makefile` by introducing `$(call RUN_BIN,...)` converting forward slashes to backslashes on `Windows_NT`.
