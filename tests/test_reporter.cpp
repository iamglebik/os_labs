#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include "../include/employee.h"
#include "../include/FileHandler.h"

namespace fs = std::filesystem;

class ReporterTest : public ::testing::Test {
protected:
    std::string testDir;
    std::string binaryFile;
    std::string reportFile;
    double hourlyRate = 25.5;

    void SetUp() override {
        testDir = "test_data_reporter_" + std::to_string(time(nullptr));
        fs::create_directories(testDir);
        binaryFile = testDir + "/test.bin";
        reportFile = testDir + "/report.txt";
    }

    void TearDown() override {
        fs::remove_all(testDir);
    }

    Employee createEmployee(int num, const std::string& name, double hours) {
        Employee emp;
        emp.num = num;
        emp.setName(name);
        emp.hours = hours;
        return emp;
    }

    void createTestBinaryFile(const std::vector<Employee>& employees) {
        FileHandler::writeBinaryFile(binaryFile, employees);
    }

    void createStandardTestData() {
        std::vector<Employee> employees = {
            createEmployee(1, "John", 40.0),
            createEmployee(2, "Jane", 35.5),
            createEmployee(3, "Bob", 42.0)
        };
        createTestBinaryFile(employees);
    }

    int runReporter() {
        std::string cmd = "..\\Release\\Reporter.exe " + binaryFile + " " +
            reportFile + " " + std::to_string(hourlyRate);
        return system(cmd.c_str());
    }

    std::vector<std::string> getReportLines() {
        std::vector<std::string> lines;
        std::ifstream file(reportFile);
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        return lines;
    }
};

TEST_F(ReporterTest, CreatesReportFile) {
    createStandardTestData();

    int result = runReporter();

    EXPECT_EQ(result, 0);
    EXPECT_TRUE(fs::exists(reportFile));
    EXPECT_GT(fs::file_size(reportFile), 0);
}

TEST_F(ReporterTest, ReportHasCorrectHeader) {
    createStandardTestData();

    runReporter();

    auto lines = getReportLines();
    ASSERT_GE(lines.size(), 1);
    EXPECT_EQ(lines[0], "ID,Name,Hours,Salary");
}

TEST_F(ReporterTest, ContainsAllEmployees) {
    createStandardTestData();

    runReporter();

    auto lines = getReportLines();
    EXPECT_EQ(lines.size(), 4);
}

TEST_F(ReporterTest, CalculatesCorrectSalaries) {
    std::vector<Employee> testData = {
        createEmployee(1, "Test1", 40.0),
        createEmployee(2, "Test2", 20.5)
    };
    createTestBinaryFile(testData);
    hourlyRate = 10.0;

    runReporter();

    auto lines = getReportLines();
    ASSERT_EQ(lines.size(), 3);

    EXPECT_EQ(lines[1], "1,Test1,40,400");
    EXPECT_EQ(lines[2], "2,Test2,20.5,205");
}

TEST_F(ReporterTest, HandlesEmptyFile) {
    std::vector<Employee> empty;
    createTestBinaryFile(empty);

    int result = runReporter();

    EXPECT_EQ(result, 0);
    auto lines = getReportLines();
    ASSERT_EQ(lines.size(), 1);
    EXPECT_EQ(lines[0], "ID,Name,Hours,Salary");
}

TEST_F(ReporterTest, HandlesSingleEmployee) {
    std::vector<Employee> single = {
        createEmployee(5, "Single", 37.5)
    };
    createTestBinaryFile(single);
    hourlyRate = 20.0;

    runReporter();

    auto lines = getReportLines();
    ASSERT_EQ(lines.size(), 2);
    EXPECT_EQ(lines[1], "5,Single,37.5,750");
}

TEST_F(ReporterTest, HandlesMaximumHours) {
    std::vector<Employee> employees = {
        createEmployee(1, "Max", 168.0)
    };
    createTestBinaryFile(employees);
    hourlyRate = 50.0;

    runReporter();

    auto lines = getReportLines();
    ASSERT_EQ(lines.size(), 2);
    EXPECT_EQ(lines[1], "1,Max,168,8400");
}

TEST_F(ReporterTest, HandlesDecimalHours) {
    std::vector<Employee> employees = {
        createEmployee(1, "Precise", 37.75)
    };
    createTestBinaryFile(employees);
    hourlyRate = 25.5;

    runReporter();

    auto lines = getReportLines();
    std::string expected = "1,Precise,37.75," +
        std::to_string(static_cast<int>(37.75 * 25.5));
    EXPECT_EQ(lines[1], expected);
}

TEST_F(ReporterTest, MaintainsEmployeeOrder) {
    std::vector<Employee> employees = {
        createEmployee(3, "Third", 30.0),
        createEmployee(1, "First", 40.0),
        createEmployee(2, "Second", 35.0)
    };
    createTestBinaryFile(employees);

    runReporter();

    auto lines = getReportLines();
    ASSERT_EQ(lines.size(), 4);
    EXPECT_THAT(lines[1], testing::HasSubstr("Third"));
    EXPECT_THAT(lines[2], testing::HasSubstr("First"));
    EXPECT_THAT(lines[3], testing::HasSubstr("Second"));
}

TEST_F(ReporterTest, HandlesMissingFile) {
    std::string nonExistent = testDir + "/nonexistent.bin";
    std::string cmd = "..\\Release\\Reporter.exe " + nonExistent + " " +
        reportFile + " " + std::to_string(hourlyRate);

    int result = system(cmd.c_str());

    EXPECT_NE(result, 0);
    EXPECT_FALSE(fs::exists(reportFile));
}

TEST_F(ReporterTest, ValidatesHourlyRate) {
    createStandardTestData();

    std::string cmd = "..\\Release\\Reporter.exe " + binaryFile + " " +
        reportFile + " -10.0";
    int result = system(cmd.c_str());

    EXPECT_NE(result, 0);
}

TEST_F(ReporterTest, HandlesCorruptedFile) {
    {
        std::ofstream file(binaryFile, std::ios::binary);
        file << "This is not a valid binary file";
    }

    int result = runReporter();

    EXPECT_NE(result, 0);
}