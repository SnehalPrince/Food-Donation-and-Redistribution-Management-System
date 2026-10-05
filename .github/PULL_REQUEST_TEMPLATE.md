## Summary of Changes

A concise explanation of the changes proposed in this Pull Request.

- Fixes #(issue)
- Relevant module(s): [e.g., FoodBank, FileManager, InputHelper, ReportGenerator]

---

## Architectural Compliance Checklist

Please ensure your pull request conforms to the project's binding design constraints:

- [ ] **Standard C++ Only:** Compiles with **zero warnings** under `g++ -Wall -Wextra -pedantic`.
- [ ] **Dual Standard Support:** Tested with both `-std=c++11` and `-std=c++17`.
- [ ] **Forbidden Features Avoided:** No `<filesystem>`, no `std::optional`, no structured bindings, no generic lambdas, no `std::make_unique`.
- [ ] **Terminal Compatibility:** Plain ASCII console only (no `windows.h`, no `system("cls")`, no ANSI colors).
- [ ] **Encapsulation:** All class attributes are `private` with explicit constructors and const correctness.
- [ ] **Greppable Tags:** Appropriate tags maintained (`// [ENCAPSULATION]`, `// [POLYMORPHISM]`, `// [STL: ...]`, etc.).
- [ ] **Test Coverage:** All unit tests (`make test`) pass cleanly.
- [ ] **Viva Demo Scenario:** `make demo` runs and finishes with zero discrepancies.

---

## Testing Performed

Describe the testing you executed locally:
```bash
make clean && make STD=c++11 all test
make clean && make STD=c++17 all test
make demo
```
Output:
```text
(Paste relevant test execution summary here)
```
