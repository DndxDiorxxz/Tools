// Process.h
#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <vector>
#include <cstdint>

class Process {
private:
    HANDLE hProcess = nullptr;
    DWORD processId = 0;
    std::string processName;

public:
    Process() = default;
    ~Process() { Close(); }

    bool Attach(const std::string& name) {
        processName = name;

        // Get process ID
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) return false;

        bool found = false;
        if (Process32First(snapshot, &entry)) {
            do {
                if (std::string(entry.szExeFile) == name) {
                    processId = entry.th32ProcessID;
                    found = true;
                    break;
                }
            } while (Process32Next(snapshot, &entry));
        }

        CloseHandle(snapshot);

        if (!found) return false;

        // Open process
        hProcess = OpenProcess(
            PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION |
            PROCESS_QUERY_INFORMATION,
            FALSE, processId);

        return hProcess != nullptr;
    }

    uintptr_t GetModuleBase(const std::string& moduleName) {
        MODULEENTRY32 entry;
        entry.dwSize = sizeof(MODULEENTRY32);

        HANDLE snapshot = CreateToolhelp32Snapshot(
            TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
            processId);

        if (snapshot == INVALID_HANDLE_VALUE) return 0;

        uintptr_t base = 0;
        if (Module32First(snapshot, &entry)) {
            do {
                if (std::string(entry.szModule) == moduleName) {
                    base = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
                    break;
                }
            } while (Module32Next(snapshot, &entry));
        }

        CloseHandle(snapshot);
        return base;
    }

    template<typename T>
    T Read(uintptr_t address) {
        T buffer;
        SIZE_T bytesRead;
        ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address),
            &buffer, sizeof(T), &bytesRead);
        return buffer;
    }

    template<typename T>
    bool Write(uintptr_t address, T value) {
        SIZE_T bytesWritten;
        return WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(address),
            &value, sizeof(T), &bytesWritten);
    }

    uintptr_t ReadPointerChain(std::vector<uintptr_t> offsets) {
        if (offsets.empty()) return 0;

        uintptr_t address = offsets[0];
        for (size_t i = 1; i < offsets.size(); i++) {
            address = Read<uintptr_t>(address);
            if (!address) return 0;
            address += offsets[i];
        }
        return address;
    }

    bool IsValid() const { return hProcess != nullptr; }
    DWORD GetPID() const { return processId; }
    HANDLE GetHandle() const { return hProcess; }

    void Close() {
        if (hProcess) {
            CloseHandle(hProcess);
            hProcess = nullptr;
        }
    }
};