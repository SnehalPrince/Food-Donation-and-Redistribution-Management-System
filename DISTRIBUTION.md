# Software Distribution & Packaging Guide

This guide describes how to build, package, and distribute the **Food Donation & Redistribution Management System** for standalone deployment, academic submission, and evaluator review.

---

## 1. Zero External Dependencies

The application is written strictly in standard C++ using only the C++ Standard Template Library (STL).
- **No external third-party libraries** (e.g., Boost, nlohmann/json, or SQLite) are required.
- **No runtime server or database engine** is needed.
- All persistent state is maintained through clean, portable pipe-delimited text files within a local `data/` directory.

---

## 2. Portable Distribution Bundle Layout

When distributing a pre-compiled standalone release or submitting a project archive to an evaluator, use the following layout:

```text
FoodDonationSystem-v1.0.0/
├── foodbank.exe (Windows binary) OR foodbank (Linux/macOS binary)
├── run_demo.bat (Windows one-click launcher)
├── run_demo.sh  (Linux/macOS one-click launcher)
├── data/        (Active database directory with text files)
│   ├── donors.txt
│   ├── recipients.txt
│   ├── donations.txt
│   ├── requests.txt
│   ├── deliveries.txt
│   └── .gitkeep
├── sample_data/ (Pre-populated demonstration dataset)
│   ├── donors.txt
│   ├── recipients.txt
│   ├── donations.txt
│   ├── requests.txt
│   └── deliveries.txt
├── README.md
├── LICENSE
└── docs/
    └── VIVA_NOTES.md
```

---

## 3. Creating a Distribution Binary

### Windows (MinGW-w64 / GCC)
To produce a completely self-contained binary that does not depend on dynamic MinGW DLLs on machines without MinGW installed:

```powershell
# Compile with static standard runtime linkage
g++ -std=c++11 -Wall -Wextra -pedantic -static-libgcc -static-libstdc++ `
    Date.cpp InputHelper.cpp Person.cpp Donor.cpp Recipient.cpp `
    FoodItem.cpp CookedFood.cpp PackagedFood.cpp Donation.cpp `
    RecipientRequest.cpp Delivery.cpp FoodBank.cpp ReportGenerator.cpp `
    FileManager.cpp main.cpp -o build\foodbank.exe
```

Or using the project build wrapper:
```powershell
make all
```

### Linux / macOS
```bash
make clean
make all
```

---

## 4. One-Click Evaluator Launcher Scripts

For academic presentation or viva evaluation, include quick-launch scripts in the root of the distribution archive:

### Windows: `run_demo.bat`
```bat
@echo off
title Food Donation and Redistribution Management System
cls
if not exist "data" mkdir "data"
foodbank.exe --today 2026-10-05 --data-dir sample_data
pause
```

### Linux / macOS: `run_demo.sh`
```bash
#!/usr/bin/env bash
mkdir -p data
./foodbank --today 2026-10-05 --data-dir sample_data
```

---

## 5. Command-Line Flags for Evaluators

The distribution executable accepts two CLI flags:

| Flag | Parameter | Description |
| ---- | --------- | ----------- |
| `--today` | `YYYY-MM-DD` | Overrides the system clock with a fixed simulation date (ideal for repeatable evaluation). |
| `--data-dir` | `<directory_path>` | Points the system to a specific folder containing the text files (default: `data`). |

**Example Evaluation Invocations:**
```bash
# 1. Clean run using fresh empty data folder
./build/foodbank --data-dir data

# 2. Interactive run loaded with sample university & NGO records
./build/foodbank --today 2026-10-05 --data-dir sample_data

# 3. Automated non-interactive demo pipeline
./build/foodbank --today 2026-10-05 --data-dir tests/temp_demo_data < tests/demo_input.txt
```

---

## 6. Distribution Checklist for Academic Submissions

Before submitting the project zip or GitHub link to professors or evaluators:

- [x] Code compiles with **zero warnings** under `g++ -Wall -Wextra -pedantic` (`make check-std`).
- [x] All 7 automated test suites pass cleanly (`make test`).
- [x] `data/` directory contains all 5 required `.txt` files (even if 0 bytes).
- [x] `sample_data/` contains realistic test records demonstrating Cooked, Bakery, Packaged, and Produce categories.
- [x] `README.md` includes class hierarchy diagram and complete usage instructions.
- [x] `docs/VIVA_NOTES.md` is readily accessible for immediate defense preparation.
- [x] `LICENSE` is attached.
