#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include "../include/employee.h"
#include "../include/FileHandler.h"

namespace fs = std::filesystem;

class CreatorTest : public ::testing::Test {
protected:
    std::string testDir;
    std::string testFile;

    void SetUp() override {
        testDir = "test_data_creator_" + std::to_string(time(nullptr));
        fs::create_directories(testDir);
        testFile = testDir + "/employees.bin";
    }

    void TearDown() override {
        fs::remove_all(testDir);
    }

    void createTestInputFile(const std::string& filename,
        const std::vector<std::string>& inputs) {
        std::ofstream file(filename);
        for (const auto& input : inputs) {
            file << input << "\n";
        }
    }

    int runCreatorWithInput(const std::string& filename,
        int count,
        const std::vector<std::string>& inputs) {
        std::string inputFile = testDir + "/input.txt";
        createTestInputFile(inputFile, inputs);

        std::string cmd = "..\\Release\\Creator.exe " + filename + " " +
            std::to_string(count) + " < " + inputFile;
        return system(cmd.c_str());
    }
};

TEST_F(CreatorTest, CreatesBinaryFile) {
    int count = 3;
    std::vector<std::string> inputs = { "John", "40", "Jane", "35", "Bob", "42" };

    int result = runCreatorWithInput(testFile, count, inputs);

    EXPECT_EQ(result, 0);
    EXPECT_TRUE(fs::exists(testFile));
    EXPECT_GT(fs::file_size(testFile), 0);
}

TEST_F(CreatorTest, WritesCorrectEmployeeData) {
    int count = 2;
    std::vector<std::string> inputs = { "Alice", "38", "Bob", "42" };

    runCreatorWithInput(testFile, count, inputs);

    auto employees = FileHandler::readBinaryFile(testFile);
    ASSERT_EQ(employees.size(), count);

    EXPECT_EQ(employees[0].num, 1);
    EXPECT_EQ(employees[0].getName(), "Alice");
    EXPECT_DOUBLE_EQ(employees[0].hours, 38.0);

    EXPECT_EQ(employees[1].num, 2);
    EXPECT_EQ(employees[1].getName(), "Bob");
    EXPECT_DOUBLE_EQ(employees[1].hours, 42.0);
}

TEST_F(CreatorTest, HandlesMaximumEmployees) {
    int count = 100;
    std::vector<std::string> inputs;
    for (int i = 0; i < count; ++i) {
        inputs.push_back("Emp" + std::to_string(i));
        inputs.push_back("40");
    }

    int result = runCreatorWithInput(testFile, count, inputs);

    EXPECT_EQ(result, 0);
    auto employees = FileHandler::readBinaryFile(testFile);
    EXPECT_EQ(employees.size(), count);
}

TEST_F(CreatorTest, RejectsInvalidHours) {
    int count = 1;
    std::vector<std::string> inputs = { "John", "200", "40" };

    int result = runCreatorWithInput(testFile, count, inputs);

    EXPECT_EQ(result, 0);
    auto employees = FileHandler::readBinaryFile(testFile);
    ASSERT_EQ(employees.size(), 1);
    EXPECT_DOUBLE_EQ(employees[0].hours, 40.0);
}

TEST_F(CreatorTest, RejectsLongName) {
    int count = 1;
    std::vector<std::string> inputs = {
        "VeryLongNameThatExceedsLimit",
        "John",
        "40"
    };

    int result = runCreatorWithInput(testFile, count, inputs);

    EXPECT_EQ(result, 0);
    auto employees = FileHandler::readBinaryFile(testFile);
    ASSERT_EQ(employees.size(), 1);
    EXPECT_EQ(employees[0].getName(), "John");
}

TEST_F(CreatorTest, OverwritesExistingFile) {
    std::vector<std::string> inputs1 = { "John", "40" };
    runCreatorWithInput(testFile, 1, inputs1);
    auto initialSize = fs::file_size(testFile);

    std::vector<std::string> inputs2 = { "Jane", "35" };
    runCreatorWithInput(testFile, 1, inputs2);

    auto employees = FileHandler::readBinaryFile(testFile);
    ASSERT_EQ(employees.size(), 1);
    EXPECT_EQ(employees[0].getName(), "Jane");
    EXPECT_EQ(fs::file_size(testFile), initialSize);
}

TEST_F(CreatorTest, HandlesZeroEmployees) {
    int count = 0;
    std::vector<std::string> inputs = {};

    int result = runCreatorWithInput(testFile, count, inputs);

    EXPECT_NE(result, 0);
    EXPECT_FALSE(fs::exists(testFile));
}

TEST_F(CreatorTest, GeneratesUniqueIds) {
    int count = 5;
    std::vector<std::string> inputs;
    for (int i = 0; i < count; ++i) {
        inputs.push_back("Emp" + std::to_string(i));
        inputs.push_back("40");
    }

    runCreatorWithInput(testFile, count, inputs);

    auto employees = FileHandler::readBinaryFile(testFile);
    std::set<int> ids;
    for (const auto& emp : employees) {
        ids.insert(emp.num);
    }
    EXPECT_EQ(ids.size(), employees.size());
}