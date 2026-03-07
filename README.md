# Employee Records System v2.0

## 📋 Overview
Modern C++ application for managing employee records with binary file storage and report generation.

## ✨ Features
- **Creator**: Create binary files with employee data
- **Reporter**: Generate formatted reports with salary calculations
- **Main**: Interactive program combining both tools
- **Comprehensive Testing**: 7+ Google Test cases
- **Modern C++**: C++17 features, RAII, exception safety

## 🏗️ Architecture
- **FileHandler**: RAII-based file operations
- **InputValidator**: Robust input validation
- **ReportGenerator**: Flexible report formatting
- **Employee**: Enhanced struct with methods

## 📦 Requirements
- CMake 3.14+
- C++17 compiler
- Google Test (auto-downloaded)

## 🔧 Building
```bash
chmod +x run_tests.sh
./run_tests.sh