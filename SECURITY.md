# Security Policy

## Supported Versions

| Version | Supported          | Notes |
| ------- | ------------------ | ----- |
| 1.0.x   | :white_check_mark: | Current active release |
| < 1.0.0 | :x:                | Deprecated / Pre-release |

---

## Scope & Design Boundaries

The **Food Donation & Redistribution Management System** is a standalone, offline, console-driven C++ application designed for local operation and academic evaluation.

- **Networking:** Out of scope. No network listening sockets, HTTP interfaces, or remote RPCs are utilized.
- **Input Sanitization:** The system implements strict defensive validation via `InputHelper`, including rejection of pipe delimiters (`|`), boundary checking on all numerical quantities ($0 < q \le 100,000$), strict calendar validations for leap years and month lengths, and graceful EOF termination.
- **File System:** Text storage is managed strictly within the specified data directory (`data/` by default or custom via `--data-dir`).

---

## Reporting a Vulnerability

If you discover a security vulnerability or crash vulnerability (e.g., buffer overflow, out-of-bounds array access, arithmetic overflow, or denial-of-service crash):

1. **Do not create a public GitHub issue.**
2. Send an email to the project maintainer:
   - **Email:** [snehal.prince07@gmail.com](mailto:snehal.prince07@gmail.com)
   - **Subject:** `[SECURITY] Vulnerability Report - Food Donation System`
3. Include:
   - A description of the issue.
   - Exact steps to reproduce (e.g., input sequence or malformed text file).
   - Expected vs. actual behavior.
   - Operating system and compiler version.

### Response Time
- **Initial acknowledgment:** Within 48 hours.
- **Assessment & patch:** Within 7 business days.
