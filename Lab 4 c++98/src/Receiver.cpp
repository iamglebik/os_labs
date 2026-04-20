#include "../headers/SharedQueue.h"
#include <iostream>
#include <string>
#include <vector>

using namespace solution;

static unsigned int to_uint(const string& s) {
    return static_cast<unsigned int>(atoi(s.c_str()));
}

int main() {
    cout << "=== Laboratory work #4: Process Synchronization ===\n";
    cout << "Author: Siniakou Hleb, group 12\n\n";
    cout << "=== Receiver ===\n";
    cout << "Input binary file name: ";
    string fileName;
    getline(cin, fileName);

    cout << "Input records count (capacity): ";
    string tmp;
    getline(cin, tmp);
    unsigned int capacity = to_uint(tmp);

    cout << "Input desired Sender process count: ";
    string tmp2;
    getline(cin, tmp2);
    unsigned int senderCount = to_uint(tmp2);

    SharedQueue queue;
    if (!queue.CreateAsReceiver(fileName, capacity, senderCount)) {
        cerr << "Error creating queue.\n";
        return 1;
    }

    cout << "\n=== Created message file ===\n";
    cout << "File: " << fileName << "\n";
    cout << "Capacity: " << capacity << " messages\n";
    cout << "Message length: up to " << MAX_MESSAGE_LEN << " characters\n";

    cout << "\nLaunching " << senderCount << " instances of sender process...\n";
    string exeName = ".\\sender.exe";
    for (unsigned int i = 0; i < senderCount; ++i) {
        string cmd = "\"" + exeName + "\" \"" + fileName + "\"";
        STARTUPINFOA si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof(pi));

        std::vector<char> buf(cmd.begin(), cmd.end());
        buf.push_back('\0');
        DWORD creationFlags = CREATE_NEW_CONSOLE;

        if (!CreateProcessA(NULL, &buf[0], NULL, NULL, FALSE, creationFlags, NULL, NULL, &si, &pi)) {
            PrintLastErrorA("CreateProcess sender");
        }
        else {
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        }
    }

    cout << "Waiting for all Senders to be ready...\n";
    queue.WaitAllSendersReady(INFINITE);
    cout << "\n=== All " << senderCount << " senders are ready ===\n";
    cout << "Commands: r - read message, q - quit\n\n";

    while (true) {
        cout << "[Receiver] Command (r/q): ";
        string cmdLine;
        if (!getline(cin, cmdLine)) break;
        if (cmdLine == "r") {
            string msg;
            bool ok = queue.PopMessage(msg, true);
            if (!ok) {
                if (queue.IsShuttingDown())
                    cout << "[Receiver] Quitting...\n";
                else
                    cout << "[Receiver] No messages\n";
            }
        }
        else if (cmdLine == "q") {
            cout << "[Receiver] Finishing. shutdown signal.\n";
            queue.SignalShutdown();
            break;
        }
        else {
            cout << "Unknown command.\n";
        }
    }

    cout << "\n=== Final file state ===\n";
    Sleep(100);
    while (true) {
        string msg;
        if (!queue.PopMessage(msg, false)) break;
        cout << "Message: " << msg << "\n";
    }

    return 0;
}