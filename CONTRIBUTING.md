# Contributing to Food Donation & Redistribution Management System

Thank you for considering contributing to the **Food Donation & Redistribution Management System**! This document provides guidelines for contributing to this project.

---

## Code of Conduct

By participating in this project, you agree to abide by our [Code of Conduct](CODE_OF_CONDUCT.md). Please report any unacceptable behavior to [snehal.prince07@gmail.com](mailto:snehal.prince07@gmail.com).

---

## Architectural Principles & Hard Constraints

This project is built for rigorous academic evaluation and production-grade C++ quality. All contributions **must** adhere to these binding rules:

1. **Standard C++ Only:**
   - Must compile with **zero warnings** under `g++ -Wall -Wextra -pedantic` under both `-std=c++11` and `-std=c++17`.
   - Clang++ and MSVC compatibility is strictly maintained.
   - Forbidden features: `<filesystem>`, `std::optional`, structured bindings, generic lambdas, `std::make_unique`.
2. **Platform Independence & Terminal Compatibility:**
   - **No** `system("cls")`, `system("pause")`, `windows.h`, or ANSI escape codes. Plain ASCII console output only.
   - Cross-platform file paths and clean newline handling (`\r\n` and `\n`).
3. **Pure OOP & Encapsulation:**
   - All data members must be `private`.
   - Explicit constructors, const accessors/mutators.
   - No global variables. `FoodBank` is the sole owner of core collections.
   - Abstract base classes with virtual destructors (`Person`, `FoodItem`).
   - Runtime polymorphism via base pointers/references (`Person::display()`, `FoodItem::isEligibleForDistribution()`, `FoodItem::display()`, `FoodItem::serializeDetails()`).
4. **Greppable Traceability Tags:**
   Any added or modified code must maintain the project's greppable concept tags where applicable:
   - `// [ENCAPSULATION]`
   - `// [ABSTRACTION]`
   - `// [INHERITANCE]`
   - `// [POLYMORPHISM]`
   - `// [STL: vector]`, `// [STL: map]`, `// [STL: set]`, `// [STL: queue]`
   - `// [FILE-IO]`
5. **Function Length:**
   - Functions should be concise and focused ($\le 40$ lines where feasible).
6. **Project Structure:**
   - Flat layout in root directory for core headers and translation units (`*.h`, `*.cpp`).
   - Dedicated folders: `data/`, `sample_data/`, `tests/`, `docs/`, `.github/`.

---

## Development Workflow

### 1. Fork & Clone
```bash
git clone https://github.com/SnehalPrince/Food-Donation-and-Redistribution-Management-System.git
cd Food-Donation-and-Redistribution-Management-System
```

### 2. Create a Feature Branch
```bash
git checkout -b feature/your-feature-name
```

### 3. Build & Verify Locally
Always verify that your code compiles cleanly across both standards and passes all tests:

```bash
# Build the binary
make all

# Run full test suite (module unit tests + E2E + robustness)
make test

# Verify dual standards compliance (C++11 and C++17 with -Wall -Wextra -pedantic)
make check-std

# Run the Viva Demo Scenario
make demo
```

On Windows (PowerShell):
```powershell
.\build.ps1 -Target all -Std c++11
.\build.ps1 -Target test -Std c++17
```

---

## Adding New Tests

If you introduce new logic or fix a bug:
- Add targeted unit tests in the appropriate `tests/test_*.cpp` file.
- Use the assertions defined in `tests/test_util.h` (`ASSERT_TRUE`, `ASSERT_FALSE`, `ASSERT_EQ`, `ASSERT_DOUBLE_EQ`, `ASSERT_STR_EQ`).
- Ensure no test leaves persistent garbage in the repository.

---

## Commit Guidelines

We follow clear, descriptive commit messages:
```text
<type>: <short summary>

[optional detailed description]
```
Common types:
- `feat`: New feature or capability
- `fix`: Bug fix
- `test`: Adding or refining test suites
- `docs`: Documentation updates
- `refactor`: Code change that neither fixes a bug nor adds a feature
- `chore`: Build scripts, `.gitignore`, CI workflow adjustments

---

## Pull Request Process

1. Ensure all tests pass (`make test` and `make check-std`).
2. Update relevant documentation (`README.md`, `docs/DECISIONS.md`, etc.) if architecture changes.
3. Open a Pull Request referencing any open issue.
4. Fill in the Pull Request template completely.
