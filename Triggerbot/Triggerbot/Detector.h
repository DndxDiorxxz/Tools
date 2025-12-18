// Detector.h - UPDATED
#pragma once
#include <atomic>
#include <Windows.h>
#include <TlHelp32.h>
#include <string>

class Detector {
private:
    int detectionRadius;
    int colorThresholdLow;
    int colorThresholdHigh;
    int nameTagBrightness;
    std::atomic<bool> robloxRunning;
    std::atomic<bool> robloxInFocus;
    HWND robloxWindow;

    bool FindRobloxWindow() {
        // Look for Roblox windows
        robloxWindow = FindWindowA(NULL, "Roblox");
        if (!robloxWindow) {
            robloxWindow = FindWindowA(NULL, "RobloxPlayerBeta");
        }
        if (!robloxWindow) {
            robloxWindow = FindWindowA("WINDOWSCLIENT", NULL); // Alternative class name
        }
        return robloxWindow != NULL;
    }

    bool IsProcessRunning(const std::string& processName) {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) return false;

        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnapshot, &pe32)) {
            do {
                std::string currentProcess(pe32.szExeFile);
                if (currentProcess.find(processName) != std::string::npos) {
                    CloseHandle(hSnapshot);
                    return true;
                }
            } while (Process32Next(hSnapshot, &pe32));
        }

        CloseHandle(hSnapshot);
        return false;
    }

public:
    Detector() :
        detectionRadius(15),
        colorThresholdLow(120),
        colorThresholdHigh(60),
        nameTagBrightness(220),
        robloxRunning(false),
        robloxInFocus(false),
        robloxWindow(NULL)
    {
    }

    // Update Roblox status
    void Update() {
        // Check if Roblox process is running
        bool processRunning = IsProcessRunning("RobloxPlayerBeta.exe") ||
            IsProcessRunning("Windows10Universal.exe") ||
            IsProcessRunning("Roblox.exe");

        // Check if Roblox window exists and is in focus
        bool windowFound = FindRobloxWindow();

        robloxRunning.store(processRunning && windowFound);

        if (robloxWindow) {
            HWND foregroundWindow = GetForegroundWindow();
            robloxInFocus.store(foregroundWindow == robloxWindow);
        }
        else {
            robloxInFocus.store(false);
        }
    }

    // Setters
    void SetDetectionRadius(int radius) { detectionRadius = radius; }
    void SetColorThresholds(int low, int high) {
        colorThresholdLow = low;
        colorThresholdHigh = high;
    }
    void SetNameTagBrightness(int brightness) { nameTagBrightness = brightness; }

    // Getters
    bool IsEnemyDetected() const {
        // Placeholder - this would be your actual detection logic
        // For testing, return true when a key is pressed
        return (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    }

    bool IsPlayerNameDetected() const {
        // Placeholder
        return (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    }

    bool IsPlayerDetected() const {
        // Combined detection
        return IsEnemyDetected() || IsPlayerNameDetected();
    }

    bool IsRobloxRunning() const {
        return robloxRunning.load();
    }

    bool IsRobloxInFocus() const {
        return robloxInFocus.load();
    }

    HWND GetRobloxWindow() const {
        return robloxWindow;
    }
};