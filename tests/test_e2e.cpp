#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "test_util.h"

static std::string readFile(const std::string& path) {
    std::ifstream in(path.c_str());
    if (!in.is_open()) return "";
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

static bool testVivaScenarioE2E() {
    #if defined(_WIN32)
      system("if exist tests\\temp_e2e_data rmdir /s /q tests\\temp_e2e_data");
      system("if not exist tests\\temp_e2e_data mkdir tests\\temp_e2e_data");
      int code = system("build\\foodbank.exe --today 2026-10-05 --data-dir tests/temp_e2e_data < tests/demo_input.txt > tests/temp_e2e_out1.txt");
    #else
      system("rm -rf tests/temp_e2e_data");
      system("mkdir -p tests/temp_e2e_data");
      int code = system("build/foodbank --today 2026-10-05 --data-dir tests/temp_e2e_data < tests/demo_input.txt > tests/temp_e2e_out1.txt");
    #endif

    ASSERT_EQ(0, code);

    std::string out1 = readFile("tests/temp_e2e_out1.txt");
    ASSERT_TRUE(out1.find("Allocated 30 kg across 1 delivery record(s)") != std::string::npos);
    ASSERT_TRUE(out1.find("Available In Stock:             20.00 kg") != std::string::npos);
    ASSERT_TRUE(out1.find("Total Food Redistributed:       30.00 kg") != std::string::npos);
    ASSERT_TRUE(out1.find("Stock reconciliation: OK") != std::string::npos);
    ASSERT_TRUE(out1.find("SDG 2: Zero Hunger - Target 2.1") != std::string::npos);
    ASSERT_TRUE(out1.find("SDG 12: Responsible Consumption - Target 12.3") != std::string::npos);

    // Run 2: Verify persistence from tests/temp_e2e_data
    #if defined(_WIN32)
      std::ofstream qin("tests/temp_exit_input.txt");
      qin << "13\n";
      qin.close();
      int code2 = system("build\\foodbank.exe --today 2026-10-05 --data-dir tests/temp_e2e_data < tests/temp_exit_input.txt > tests/temp_e2e_out2.txt");
    #else
      std::ofstream qin("tests/temp_exit_input.txt");
      qin << "13\n";
      qin.close();
      int code2 = system("build/foodbank --today 2026-10-05 --data-dir tests/temp_e2e_data < tests/temp_exit_input.txt > tests/temp_e2e_out2.txt");
    #endif

    ASSERT_EQ(0, code2);
    std::string out2 = readFile("tests/temp_e2e_out2.txt");
    ASSERT_TRUE(out2.find("Loaded: 1 donors, 1 recipients, 1 donations, 1 requests, 1 deliveries.") != std::string::npos);

    return true;
}

static bool testRobustnessGarbageInput() {
    // Generate robustness input with extreme inputs
    std::ofstream out("tests/robustness_input.txt");
    // Invalid menu choices (letters, negative, out of range)
    out << "abc\n";
    out << "-99\n";
    out << "99999\n";
    // Try Option 1 (Register Donor) with pipe injection, huge string, whitespace
    out << "1\n";
    out << "BAD|PIPE|ID\n";
    out << "D099\n";
    std::string huge(10000, 'A');
    out << huge << "\n"; // Too long
    out << "Valid Huge Test Donor\n";
    out << "contact@test.com\n";
    out << "Donor|With|Pipes\n"; // Pipe rejected
    out << "Direct Charity\n";
    // Try Option 3 (Add Donation) with negative quantity, bad date
    out << "3\n";
    out << "DON999\n";
    out << "D099\n";
    out << "Bread\n";
    out << "3\n"; // Bakery
    out << "-50.0\n"; // Negative quantity rejected
    out << "0.0\n";    // Zero quantity rejected
    out << "25.0\n";
    out << "\n";       // default prep date
    out << "2026-02-30\n"; // Invalid date (Feb 30th)
    out << "2026-10-08\n"; // Valid expiry
    // Exit safely
    out << "13\n";
    out.close();

    #if defined(_WIN32)
      int code = system("build\\foodbank.exe --today 2026-10-05 --data-dir tests/temp_robustness < tests/robustness_input.txt > tests/temp_robustness_out.txt");
    #else
      int code = system("build/foodbank --today 2026-10-05 --data-dir tests/temp_robustness < tests/robustness_input.txt > tests/temp_robustness_out.txt");
    #endif

    ASSERT_EQ(0, code);
    std::string robOut = readFile("tests/temp_robustness_out.txt");
    ASSERT_TRUE(robOut.find("Error: ID cannot contain '|'") != std::string::npos);
    ASSERT_TRUE(robOut.find("Error: Input is too long") != std::string::npos);
    ASSERT_TRUE(robOut.find("Error: Value must be between 0.01 and 100000") != std::string::npos);
    ASSERT_TRUE(robOut.find("Error: Invalid date format or calendar date") != std::string::npos);
    ASSERT_TRUE(robOut.find("Goodbye!") != std::string::npos);

    return true;
}

static bool testEofHandling() {
    // Immediate EOF input
    #if defined(_WIN32)
      int code = system("build\\foodbank.exe --today 2026-10-05 --data-dir tests/temp_e2e_data < nul > tests/temp_eof_out.txt");
    #else
      int code = system("build/foodbank --today 2026-10-05 --data-dir tests/temp_e2e_data < /dev/null > tests/temp_eof_out.txt");
    #endif
    ASSERT_EQ(0, code);
    std::string eofOut = readFile("tests/temp_eof_out.txt");
    ASSERT_TRUE(eofOut.find("Goodbye!") != std::string::npos);
    return true;
}

int main() {
    std::cout << "--- Running E2E and Robustness Tests ---\n";
    RUN_TEST(testVivaScenarioE2E);
    RUN_TEST(testRobustnessGarbageInput);
    RUN_TEST(testEofHandling);
    TEST_REPORT_SUMMARY();
}
