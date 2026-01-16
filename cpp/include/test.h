#include <unordered_map>
#include <string>
#include <iostream>

class Tester {
private:
    std::unordered_map<std::string, bool (*)()> tests;

public:
    /*
     * Add a test to the test suite.
     * Arguments:
     *     const std::string test_name - The name of the test.
     *     bool (*test_func)() - Function pointer (returns bool, no args).
     */
    void add_test(const std::string test_name, bool (*test_func)()) {
        tests[test_name] = test_func;
    }

    /*
     * Run all the tests in the test suite.
     * Arguments:
     *     bool output_stderr - Should tests output?
     * Returns:
     *     bool - True if all tests pass, false otherwise.
     */
    bool run_tests(bool output_stderr=false) { 
        std::cerr << "Running tests." << std::endl;

        bool test_passed = true;
        for(const auto it : tests) {
            if(output_stderr) std::cerr << "Running " << it.first << "...";
            bool result = it.second();
            if(result) { if(output_stderr) { std::cerr << "PASSED."; } }
            else { test_passed = false; if(output_stderr) { std::cerr << "FAILED."; } }
            std::cerr << std::endl;
        }

        return test_passed;
    }

    /* Helper functions for assertions. */
    static bool assert(bool test) {
        return test;
    }
    static bool assert_fail(bool test) {
        return !test;
    }
};