#ifndef THREADS_H
#define THREADS_H

#include <windows.h>
#include "Array.h"

DWORD WINAPI MinMaxThread(LPVOID lpParam);
DWORD WINAPI AverageThread(LPVOID lpParam);

#endif
