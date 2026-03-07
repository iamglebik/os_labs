#!/bin/bash

echo "=============================================="
echo "  Employee Records System - Test Suite v2.0"
echo "=============================================="

# Clean and build
rm -rf build
mkdir build && cd build

echo -e "\n[1/4] Configuring CMake..."
cmake .. || exit 1

echo -e "\n[2/4] Building..."
make -j4 || exit 1

echo -e "\n[3/4] Running Tests..."
echo "=============================================="

# Create test data directory
mkdir -p test_data

# Run Creator tests
echo -e "\n>>> Creator Tests"
./test_runner --gtest_filter=CreatorTest.*

# Run Reporter tests
echo -e "\n>>> Reporter Tests"
./test_runner --gtest_filter=ReporterTest.*

echo -e "\n=============================================="
echo -e "[4/4] Test Summary"
echo "=============================================="

# Count tests
TOTAL_TESTS=$(./test_runner --gtest_list_tests | grep -c "^ ")
PASSED_TESTS=$(./test_runner 2>&1 | grep -c "PASSED")

echo "Total tests: $TOTAL_TESTS"
echo "Passed: $PASSED_TESTS"
echo "Failed: $((TOTAL_TESTS - PASSED_TESTS))"

if [ $PASSED_TESTS -eq $TOTAL_TESTS ]; then
    echo -e "\n✅ ALL TESTS PASSED!"
    exit 0
else
    echo -e "\n❌ SOME TESTS FAILED!"
    exit 1
fi