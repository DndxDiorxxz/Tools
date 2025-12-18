// ConsoleHelper.h - COMPLETELY FIXED
#pragma once
#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <string>

class ConsoleHelper {
private:
    HANDLE hConsole;

public:
    ConsoleHelper() {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTitleA("Roblox External TriggerBot");
    }

    void Clear() {
        COORD topLeft = { 0, 0 };
        DWORD charsWritten;
        CONSOLE_SCREEN_BUFFER_INFO screen;
        DWORD consoleSize;

        GetConsoleScreenBufferInfo(hConsole, &screen);
        consoleSize = screen.dwSize.X * screen.dwSize.Y;

        FillConsoleOutputCharacter(hConsole, ' ', consoleSize, topLeft, &charsWritten);
        FillConsoleOutputAttribute(hConsole, 7, consoleSize, topLeft, &charsWritten);
        SetConsoleCursorPosition(hConsole, topLeft);
    }

    void SetColor(int color) {
        SetConsoleTextAttribute(hConsole, color);
    }

    void PrintHeader() {
        SetColor(10);
        std::cout << "\n";
        std::cout << "  _____       _           _        _____      _                  \n";
        std::cout << " |  __ \\     | |         | |      |  __ \\    | |                 \n";
        std::cout << " | |__) |___ | |__   ___ | |_ ___ | |__) |___| |_ _ __ ___  ___  \n";
        std::cout << " |  _  // _ \\| '_ \\ / _ \\| __/ _ \\|  _  // _ \\ __| '__/ _ \\/ _ \\ \n";
        std::cout << " | | \\ \\ (_) | |_) | (_) | || (_) | | \\ \\  __/ |_| | |  __/  __/ \n";
        std::cout << " |_|  \\_\\___/|_.__/ \\___/ \\__\\___/|_|  \\_\\___|\\__|_|  \\___|\\___| \n";
        std::cout << "                                                                  \n";

        SetColor(11);
        std::cout << "           External TriggerBot v1.0\n";
        std::cout << "          Character Detection Mode\n";
        std::cout << "             Made By Diorxz\n";

        SetColor(7);
        std::cout << "================================================\n\n";
    }

    void PrintStatus(bool isRunning, bool isEnabled, char hotkey,
        float distance, int delay) {
        SetColor(14);
        std::cout << ">> STATUS <<\n";
        SetColor(15);
        std::cout << "  [";
        SetColor(isRunning ? 10 : 12);
        std::cout << (isRunning ? "RUNNING" : "STOPPED");
        SetColor(15);
        std::cout << "] Bot\n";

        std::cout << "  [";
        SetColor(isEnabled ? 10 : 8);
        std::cout << (isEnabled ? "ENABLED" : "DISABLED");
        SetColor(15);
        std::cout << "] Trigger\n";

        std::cout << "  [Hotkey] " << hotkey << "\n";
        std::cout << "  [Range] " << distance << " studs\n";
        std::cout << "  [Delay] " << delay << "ms\n\n";
    }

    void PrintControls() {
        SetColor(13);
        std::cout << ">> CONTROLS <<\n";
        SetColor(15);
        std::cout << "  F1 .......... Start/Stop Bot\n";
        std::cout << "  F2 .......... Change Hotkey\n";
        std::cout << "  F3 .......... Change Max Distance\n";
        std::cout << "  F4 .......... Change Shoot Delay\n";
        std::cout << "  F5 .......... Toggle Hold Click\n";
        std::cout << "  ESC ......... Exit Program\n\n";
    }

    void PrintInfo() {
        SetColor(9);
        std::cout << ">> INFO <<\n";
        SetColor(7);
        std::cout << "  * Currently in TEST MODE\n";
        std::cout << "  * Hotkey: Press 'T' to toggle\n";
        std::cout << "  * Max distance: Adjust with F3\n";
        std::cout << "  * Shoot delay: Adjust with F4\n";
        std::cout << "  * Hold click mode: Toggle with F5\n\n";
    }

    void PrintError(const std::string& message) {
        SetColor(12);
        std::cout << "[!] ERROR: " << message << "\n";
        SetColor(15);
    }

    void PrintSuccess(const std::string& message) {
        SetColor(10);
        std::cout << "[+] " << message << "\n";
        SetColor(15);
    }

    void PrintWarning(const std::string& message) {
        SetColor(14);
        std::cout << "[*] " << message << "\n";
        SetColor(15);
    }

    void WaitForKey() {
        std::cout << "\nPress any key to continue...";
        // Clear input buffer first
        while (_kbhit()) _getch();
        _getch();
    }
};