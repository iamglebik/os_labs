#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include "../include/employee.h"
#include "../include/FileHandler.h"

namespace fs = std::filesystem;

class IntegrationTest : public ::testing::Test {
protected:
    std::string testDir;
    std::string binaryFile;
    std::string reportFile;
    double hourlyRate = 25.5;

    void SetUp() override {
        testDir = "test_data_integration_" + std::to_string(time(nullptr));
        fs::create_directories(testDir);
        binaryFile = testDir + "/employees.bin";
        reportFile = testDir + "/report.txt";
    }

    void TearDown() override {
        fs::remove_all(testDir);
    }

    void createInputFile(const std::vector<std::string>& inputs) {
        std::string inputFile = testDir + "/input.txt";
        std::ofstream file(inputFile);
        for (const auto& input : inputs) {
            file << input << "\n";
        }
    }

    int runCreator(const std::string& filename, int count,
        const std::vector<std::string>& inputs) {
        createInputFile(inputs);
        std::string cmd = "..\\Release\\Creator.exe " + filename + " " +
            std::to_string(count) + " < " + testDir + "/input.txt";
        return system(cmd.c_str());
    }

    int runReporter(const std::string& binFile, const std::string& repFile,
        double rate) {
        std::string cmd = "..\\Release\\Reporter.exe " + binFile + " " +
            repFile + " " + std::to_string(rate);
        return system(cmd.c_str());
    }
};

TEST_F(IntegrationTest, CompleteWorkflow) {
    std::vector<std::string> creatorInputs = {
        "John", "40",
        "Jane", "35",
        "Bob", "42"
    };

    int creatorResult = runCreator(binaryFile, 3, creatorInputs);
    EXPECT_EQ(creatorResult, 0);

    auto employees = FileHandler::readBinaryFile(binaryFile);
    ASSERT_EQ(employees.size(), 3);

    int reporterResult = runReporter(binaryFile, reportFile, hourlyRate);
    EXPECT_EQ(reporterResult, 0);

    std::ifstream report(reportFile);
    std::string line;
    std::vector<std::string> reportLines;
    while (std::getline(report, line)) {
        reportLines.push_back(line);
    }

    EXPECT_EQ(reportLines.size(), 4);
    EXPECT_EQ(reportLines[0], "ID,Name,Hours,Salary");
}

TEST_F(IntegrationTest, CreatorOutputMatchesReporterInput) {
    std::vector<std::string> creatorInputs = {
        "Alice", "38",
        "Bob", "42"
    };

    runCreator(binaryFile, 2, creatorInputs);

    auto beforeReport = FileHandler::readBinaryFile(binaryFile);

    runReporter(binaryFile, reportFile, 20.0);

    auto afterReport = FileHandler::readBinaryFile(binaryFile);

    ASSERT_EQ(beforeReport.size(), afterReport.size());
    for (size_t i = 0; i < beforeReport.size(); ++i) {
        EXPECT_EQ(beforeReport[i].num, afterReport[i].num);
        EXPECT_EQ(beforeReport[i].getName(), afterReport[i].getName());
        EXPECT_DOUBLE_EQ(beforeReport[i].hours, afterReport[i].hours);
    }
}

TEST_F(IntegrationTest, LargeDataSet) {
    std::vector<std::string> creatorInputs;
    int count = 100;

    for (int i = 0; i < count; ++i) {
        creatorInputs.push_back("Emp" + std::to_string(i));
        creatorInputs.push_back(std::to_string(20 + (i % 40)));
    }

    int creatorResult = runCreator(binaryFile, count, creatorInputs);
    EXPECT_EQ(creatorResult, 0);

    int reporterResult = runReporter(binaryFile, reportFile, 25.0);
    EXPECT_EQ(reporterResult, 0);

    std::ifstream report(reportFile);
    int lineCount = 0;
    std::string line;
    while (std::getline(report, line)) {
        lineCount++;
    }

    EXPECT_EQ(lineCount, count + 1);
}

TEST_F(IntegrationTest, EdgeCaseWorkflow) {
    std::vector<std::string> creatorInputs = {
        "Max", "168",
        "Min", "1"
    };

    runCreator(binaryFile, 2, creatorInputs);

    runReporter(binaryFile, reportFile, 100.0);

    std::ifstream report(reportFile);
    std::string line;
    std::getline(report, line);
    std::getline(report, line);

    EXPECT_THAT(line, testing::HasSubstr("Max"));
    EXPECT_THAT(line, testing::HasSubstr("168"));
    EXPECT_THAT(line, testing::HasSubstr("16800"));

    std::getline(report, line);
    EXPECT_THAT(line, testing::HasSubstr("Min"));
    EXPECT_THAT(line, testing::HasSubstr("1"));
    EXPECT_THAT(line, testing::HasSubstr("100"));
}