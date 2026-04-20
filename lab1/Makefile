CXX = x86_64-w64-mingw32-g++
CXXFLAGS = -static -Wall -std=c++11
TARGETS = Main.exe Creator.exe Reporter.exe

all: $(TARGETS)

Main.exe: src/Main.cpp src/employee.h
	$(CXX) $(CXXFLAGS) src/Main.cpp -o Main.exe

Creator.exe: src/Creator.cpp src/employee.h
	$(CXX) $(CXXFLAGS) src/Creator.cpp -o Creator.exe

Reporter.exe: src/Reporter.cpp src/employee.h
	$(CXX) $(CXXFLAGS) src/Reporter.cpp -o Reporter.exe

clean:
	rm -f *.exe *.bin *.txt
	rm -rf build/ allure-report/

run: all
	wine Main.exe

.PHONY: all clean run
