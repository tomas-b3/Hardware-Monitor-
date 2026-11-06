#include <windows.h>
#include <iostream>
#include <chrono>
#include <TlHelp32.h> // For number of processes

int main() {
    // ==========================================
    // STATIC HARDWARE INFO (Queried only once!)
    // ==========================================
    
    // 1. Get CPU Core Count once
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    int logicalCores = sysInfo.dwNumberOfProcessors;

    // 2. Get Total RAM once
    MEMORYSTATUSEX staticMem = {};
    staticMem.dwLength = sizeof(staticMem);
    GlobalMemoryStatusEx(&staticMem);
    unsigned long long totalRamMB = staticMem.ullTotalPhys / (1024 * 1024);

    while (true) {

        // Get LIVE RAM information
        MEMORYSTATUSEX mem = {};
        mem.dwLength = sizeof(mem);
        GlobalMemoryStatusEx(&mem);

        // Get initial CPU times
        FILETIME idleTime;
        FILETIME kernelTime;
        FILETIME userTime;
        GetSystemTimes(&idleTime, &kernelTime, &userTime);

        // Wait one second before checking again
        Sleep(1000);

        FILETIME idleTime2;
        FILETIME kernelTime2;
        FILETIME userTime2;
        GetSystemTimes(&idleTime2, &kernelTime2, &userTime2);

        // Convert the CPU times into 64-bit values
        ULARGE_INTEGER idle1;
        idle1.LowPart = idleTime.dwLowDateTime;
        idle1.HighPart = idleTime.dwHighDateTime;

        ULARGE_INTEGER idle2;
        idle2.LowPart = idleTime2.dwLowDateTime;
        idle2.HighPart = idleTime2.dwHighDateTime;

        ULARGE_INTEGER kernel1;
        kernel1.LowPart = kernelTime.dwLowDateTime;
        kernel1.HighPart = kernelTime.dwHighDateTime;

        ULARGE_INTEGER kernel2;
        kernel2.LowPart = kernelTime2.dwLowDateTime;
        kernel2.HighPart = kernelTime2.dwHighDateTime;

        ULARGE_INTEGER user1;
        user1.LowPart = userTime.dwLowDateTime;
        user1.HighPart = userTime.dwHighDateTime;

        ULARGE_INTEGER user2;
        user2.LowPart = userTime2.dwLowDateTime;
        user2.HighPart = userTime2.dwHighDateTime;

        // Calculate the CPU time used during the one-second interval
        unsigned long long idle = idle2.QuadPart - idle1.QuadPart;
        unsigned long long kernel = kernel2.QuadPart - kernel1.QuadPart;
        unsigned long long user = user2.QuadPart - user1.QuadPart;
        unsigned long long total = kernel + user;
        unsigned long long busy = total - idle;

        // Calculate CPU usage as a percentage
        double cpuUsage = 0.0;
        if (total > 0) {
            cpuUsage = (double)busy / total * 100.0;
        }

        // Calculate Running Processes
        int processCount = 0;
        HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        
        if (hProcessSnap != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32 pe32;
            pe32.dwSize = sizeof(PROCESSENTRY32);
            
            // Get the first process
            if (Process32First(hProcessSnap, &pe32)) {
                // Loop through all remaining processes
                do {
                    processCount++;
                } while (Process32Next(hProcessSnap, &pe32));
            }
            // Clean up the handle
            CloseHandle(hProcessSnap);
        }

        // Display system information
        system("cls");

        // Print Info


        std::cout << "Total RAM: "
                  << totalRamMB
                  << " MB\n";

        // Print
        std::cout << "Available RAM: "
                  << mem.ullAvailPhys / (1024 * 1024)
                  << " MB\n";

        std::cout << "Used RAM: "
                  << (staticMem.ullTotalPhys - mem.ullAvailPhys) / (1024 * 1024)
                  << " MB\n";

        std::cout << "RAM usage: "
                  << mem.dwMemoryLoad
                  << "%\n";

        std::cout << "CPU usage: "
                  << cpuUsage
                  << "%\n";

        std::cout << "System Uptime: "
                  << GetTickCount64() / 1000 / 60 
                  << " minutes\n";

        std::cout << "Processes: "
                  << processCount
                  << "\n";

        std::cout << "\nPress Ctrl+C to exit...\n";
    }

    return 0;
}