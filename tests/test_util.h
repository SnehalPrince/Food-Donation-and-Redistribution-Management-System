#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>

// Test counters
static int g_tests_total = 0;
static int g_tests_passed = 0;
static int g_tests_failed = 0;

#define ASSERT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            std::cerr << "  FAILED: " << #cond << " at " << __FILE__ << ":" << __LINE__ << "\n"; \
            return false; \
        } \
    } while (0)

#define ASSERT_FALSE(cond) \
    do { \
        if (cond) { \
            std::cerr << "  FAILED: NOT(" << #cond << ") at " << __FILE__ << ":" << __LINE__ << "\n"; \
            return false; \
        } \
    } while (0)

#define ASSERT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            std::cerr << "  FAILED: expected [" << (expected) << "] but got [" << (actual) \
                      << "] at " << __FILE__ << ":" << __LINE__ << "\n"; \
            return false; \
        } \
    } while (0)

#define ASSERT_NEAR(expected, actual, eps) \
    do { \
        if (std::fabs((expected) - (actual)) > (eps)) { \
            std::cerr << "  FAILED: expected [" << (expected) << "] approx [" << (actual) \
                      << "] diff [" << std::fabs((expected) - (actual)) << " > " << (eps) \
                      << "] at " << __FILE__ << ":" << __LINE__ << "\n"; \
            return false; \
        } \
    } while (0)

#define RUN_TEST(test_func) \
    do { \
        g_tests_total++; \
        std::cout << "[RUN ] " << #test_func << " ... "; \
        if (test_func()) { \
            g_tests_passed++; \
            std::cout << "PASSED\n"; \
        } else { \
            g_tests_failed++; \
            std::cout << "FAILED\n"; \
        } \
    } while (0)

#define TEST_REPORT_SUMMARY() \
    do { \
        std::cout << "----------------------------------------\n"; \
        std::cout << "Tests run: " << g_tests_total \
                  << ", Passed: " << g_tests_passed \
                  << ", Failed: " << g_tests_failed << "\n"; \
        std::cout << "----------------------------------------\n"; \
        if (g_tests_failed > 0) { \
            return 1; \
        } \
        return 0; \
    } while (0)

#endif // TEST_UTIL_H
