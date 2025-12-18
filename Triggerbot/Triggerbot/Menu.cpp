// Menu.cpp - FIXED
#include "Menu.h"
#include <iostream>
#include <conio.h>
#include <iomanip>
#include <sstream>
#include <Windows.h>
#include <cstdlib>  // Add this for exit()

// Remove this conflicting global function
// void SetCursorPosition(int x, int y) {
//     COORD coord;
//     coord.X = x;
//     coord.Y = y;
//     SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
// }

Menu::Menu(TriggerBot& botRef) : bot(botRef), showAdvanced(false), selectedTab(0) {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTitleA("⚡ Roblox TriggerBot Pro v3.0 - Made By Diorxz ⚡");

    // Initialize tabs
    tabs = { "MAIN", "SPEED", "HOTKEYS", "DETECTION", "STATS", "ADVANCED" };
}

void Menu::Initialize() {
    SetConsoleSize(80, 45);

    mainMenuItems = {
        {"Start/Stop Bot", [this]() {
            if (bot.IsRunning()) bot.Stop();
            else bot.Start();
        }, false},
        {"Speed Settings", [this]() { ShowSpeedSettings(); }, false},
        {"Hotkey Settings", [this]() { ShowHotkeySettings(); }, false},
        {"Detection Settings", [this]() { ShowDetectionSettings(); }, false},
        {"View Statistics", [this]() { ShowStatistics(); }, false},
        {"Advanced Settings", [this]() { ShowAdvancedSettings(); }, false},
        {"Test Detection", [this]() { bot.TestDetection(); }, false},
        {"Calibrate Colors", [this]() { ShowCalibration(); }, false},
        {"Reset Statistics", [this]() { bot.ResetStatistics(); }, false},
        {"Exit Program", [this]() { exit(0); }, false}
    };
}

void Menu::SetConsoleSize(int width, int height) {
    SMALL_RECT rect;
    rect.Top = 0;
    rect.Left = 0;
    rect.Bottom = height - 1;
    rect.Right = width - 1;

    COORD coord;
    coord.X = width;
    coord.Y = height;

    SetConsoleScreenBufferSize(hConsole, coord);
    SetConsoleWindowInfo(hConsole, TRUE, &rect);
}

void Menu::ClearScreen() {
    COORD topLeft = { 0, 0 };
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    GetConsoleScreenBufferInfo(hConsole, &screen);
    DWORD consoleSize = screen.dwSize.X * screen.dwSize.Y;

    FillConsoleOutputCharacterA(hConsole, ' ', consoleSize, topLeft, &written);
    FillConsoleOutputAttribute(hConsole, 7, consoleSize, topLeft, &written);
    SetConsoleCursorPosition(hConsole, topLeft);
}

void Menu::DrawBox(int x, int y, int width, int height, int color, bool doubleLine) {
    SetConsoleTextAttribute(hConsole, color);

    char horizontal = doubleLine ? '═' : '─';
    char vertical = doubleLine ? '║' : '│';
    char topLeft = doubleLine ? '╔' : '┌';
    char topRight = doubleLine ? '╗' : '┐';
    char bottomLeft = doubleLine ? '╚' : '└';
    char bottomRight = doubleLine ? '╝' : '┘';

    // Draw corners and lines
    SetCursorPosition(x, y);
    std::cout << topLeft;
    for (int i = 0; i < width - 2; i++) std::cout << horizontal;
    std::cout << topRight;

    for (int i = 1; i < height - 1; i++) {
        SetCursorPosition(x, y + i);
        std::cout << vertical;
        SetCursorPosition(x + width - 1, y + i);
        std::cout << vertical;
    }

    SetCursorPosition(x, y + height - 1);
    std::cout << bottomLeft;
    for (int i = 0; i < width - 2; i++) std::cout << horizontal;
    std::cout << bottomRight;

    SetConsoleTextAttribute(hConsole, 7);
}

void Menu::SetCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(hConsole, coord);
}

void Menu::DrawText(int x, int y, const std::string& text, int color) {
    SetCursorPosition(x, y);
    SetConsoleTextAttribute(hConsole, color);
    std::cout << text;
    SetConsoleTextAttribute(hConsole, 7);
}

void Menu::DrawCenteredText(int y, const std::string& text, int color) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int x = (width - text.length()) / 2;

    DrawText(x, y, text, color);
}

void Menu::DrawProgressBar(int x, int y, int width, double percentage, int color) {
    int filled = static_cast<int>((percentage / 100.0) * (width - 2));

    SetCursorPosition(x, y);
    SetConsoleTextAttribute(hConsole, color);
    std::cout << "[";

    for (int i = 0; i < width - 2; i++) {
        if (i < filled) std::cout << "█";
        else std::cout << "░";
    }

    std::cout << "]";
    SetConsoleTextAttribute(hConsole, 7);
}

void Menu::DrawTabBar() {
    int startX = 2;
    int tabWidth = 12;

    for (size_t i = 0; i < tabs.size(); i++) {
        int x = startX + (i * tabWidth);
        int color = (i == selectedTab) ? 14 : 8;

        DrawBox(x, 2, tabWidth, 3, color);
        DrawText(x + 2, 3, tabs[i], color);
    }
}

int Menu::GetStatusColor(bool status) const {
    return status ? 10 : 12;
}

int Menu::GetSpeedColor(int delay) const {
    if (delay <= 10) return 12; // Red - very fast
    if (delay <= 50) return 14; // Yellow - fast
    if (delay <= 100) return 10; // Green - moderate
    return 8; // Gray - slow
}

void Menu::DrawHeader() {
    // Clear the top lines first
    SetCursorPosition(0, 0);
    SetConsoleTextAttribute(hConsole, 7);
    for (int i = 0; i < 5; i++) {
        std::cout << std::string(80, ' ') << std::endl;
    }

    SetCursorPosition(0, 0);
    SetConsoleTextAttribute(hConsole, 11);

    // Simple header without raw string literal
    DrawCenteredText(0, "╔══════════════════════════════════════════════════════════╗", 11);
    DrawCenteredText(1, "║                  ROBLOX TRIGGERBOT PRO                   ║", 11);
    DrawCenteredText(2, "║                     Version 3.0                          ║", 11);
    DrawCenteredText(3, "║                  Made with ❤️ by Diorxz                 ║", 11);
    DrawCenteredText(4, "╚══════════════════════════════════════════════════════════╝", 11);

    SetConsoleTextAttribute(hConsole, 7);
}

void Menu::DrawStatusPanel() {
    DrawBox(2, 6, 76, 8, 13);
    DrawText(4, 7, "⚡ STATUS PANEL", 14);

    int x = 4;
    int y = 8;

    // Roblox Status
    DrawText(x, y, "Roblox: ", 7);
    DrawText(x + 8, y, bot.IsRobloxDetected() ? "CONNECTED" : "NOT FOUND",
        GetStatusColor(bot.IsRobloxDetected()));
    DrawText(x + 25, y, bot.IsRobloxInFocus() ? "(IN FOCUS)" : "(BACKGROUND)",
        GetStatusColor(bot.IsRobloxInFocus()));

    // Bot Status
    y++;
    DrawText(x, y, "Bot: ", 7);
    DrawText(x + 8, y, bot.IsRunning() ? "RUNNING" : "STOPPED",
        GetStatusColor(bot.IsRunning()));
    DrawText(x + 25, y, bot.IsEnabled() ? "(ACTIVE)" : "(STANDBY)",
        GetStatusColor(bot.IsEnabled()));

    // Speed
    y++;
    DrawText(x, y, "Speed: ", 7);
    DrawText(x + 8, y, std::to_string(static_cast<int>(bot.GetClicksPerSecond())) + " CPS",
        GetSpeedColor(bot.GetShootDelay()));

    // Scale CPS for progress bar (max 20 CPS = 100%)
    double cpsPercentage = std::min(bot.GetClicksPerSecond() * 5.0, 100.0);
    DrawProgressBar(x + 25, y, 30, cpsPercentage, 11);
}

void Menu::DrawSpeedPanel() {
    DrawBox(2, 15, 37, 12, 10);
    DrawText(4, 16, "⚙️ SPEED SETTINGS", 14);

    int x = 4;
    int y = 17;

    DrawText(x, y, "Shoot Delay: ", 7);
    DrawText(x + 14, y, std::to_string(bot.GetShootDelay()) + " ms",
        GetSpeedColor(bot.GetShootDelay()));

    y++;
    DrawText(x, y, "Rapid Fire: ", 7);
    DrawText(x + 14, y, std::to_string(bot.GetRapidFireCount()) + " shots", 11);

    y++;
    DrawText(x, y, "Burst Delay: ", 7);
    DrawText(x + 14, y, std::to_string(bot.GetBurstDelay()) + " ms", 11);

    y++;
    DrawText(x, y, "Detection Mode: ", 7);
    std::string mode;
    switch (bot.GetDetectionMode()) {
    case 1: mode = "COLOR"; break;
    case 2: mode = "NAME TAG"; break;
    case 3: mode = "COMBINED"; break;
    case 4: mode = "SMART"; break;
    default: mode = "UNKNOWN";
    }
    DrawText(x + 17, y, mode, 13);

    // Speed presets
    y += 2;
    DrawText(x, y, "[1] Ultra Fast", GetSpeedColor(1));
    DrawText(x + 20, y, "[2] Competitive", GetSpeedColor(10));
    y++;
    DrawText(x, y, "[3] Normal", GetSpeedColor(50));
    DrawText(x + 20, y, "[4] Humanized", GetSpeedColor(100));
}

void Menu::DrawStatisticsPanel() {
    DrawBox(41, 15, 37, 12, 11);
    DrawText(43, 16, "📊 STATISTICS", 14);

    int x = 43;
    int y = 17;

    DrawText(x, y, "Shots Fired: ", 7);
    DrawText(x + 13, y, std::to_string(bot.GetShotsFired()), 11);

    y++;
    DrawText(x, y, "Enemies Detected: ", 7);
    DrawText(x + 19, y, std::to_string(bot.GetEnemiesDetected()), 10);

    y++;
    DrawText(x, y, "Successful Hits: ", 7);
    DrawText(x + 18, y, std::to_string(bot.GetSuccessfulHits()), 10);

    y++;
    DrawText(x, y, "Accuracy: ", 7);
    std::stringstream accuracy;
    accuracy << std::fixed << std::setprecision(1) << bot.GetAccuracy() << "%";
    DrawText(x + 11, y, accuracy.str(), bot.GetAccuracy() > 70.0 ? 10 : 12);

    y++;
    DrawText(x, y, "Runtime: ", 7);
    DrawText(x + 10, y, std::to_string(bot.GetRuntimeMinutes()) + " minutes", 11);

    y++;
    DrawText(x, y, "Active Time: ", 7);
    std::stringstream active;
    active << std::fixed << std::setprecision(1) << bot.GetActivePercentage() << "%";
    DrawText(x + 14, y, active.str(), 11);
}

void Menu::DrawControlsPanel() {
    DrawBox(2, 28, 76, 10, 12);
    DrawText(4, 29, "🎮 CONTROLS", 14);

    int x = 4;
    int y = 30;

    DrawText(x, y, "Toggle Bot: ", 7);
    DrawText(x + 12, y, "[" + std::string(1, static_cast<char>(bot.GetHotkey())) + "]", 10);

    DrawText(x + 20, y, "Disable: ", 7);
    DrawText(x + 28, y, "[" + std::string(1, static_cast<char>(bot.GetToggleKey())) + "]", 12);

    DrawText(x + 40, y, "Panic Stop: ", 7);
    DrawText(x + 52, y, "[F12]", 12);

    y++;
    DrawText(x, y, "Menu Navigation: ", 7);
    DrawText(x + 18, y, "[↑↓] Move  [ENTER] Select  [ESC] Back", 11);

    y += 2;
    DrawText(x, y, "🔄 Quick Actions:", 14);
    DrawText(x + 20, y, "[F1] Start/Stop  [F2] Speed Menu  [F3] Hotkeys  [F4] Detection", 11);
}

void Menu::DrawFooter() {
    SetCursorPosition(2, 39);
    SetConsoleTextAttribute(hConsole, 8);
    std::cout << "Made with ❤️ by Diorxz | Roblox TriggerBot Pro v3.0 | ";

    if (bot.IsRunning()) {
        SetConsoleTextAttribute(hConsole, 10);
        std::cout << "RUNNING";
    }
    else {
        SetConsoleTextAttribute(hConsole, 12);
        std::cout << "STOPPED";
    }

    SetConsoleTextAttribute(hConsole, 7);
    SetCursorPosition(2, 41);
    std::cout << ">> Select option [1-0]: ";
}

void Menu::Display() {
    ClearScreen();
    DrawHeader();
    DrawTabBar();
    DrawStatusPanel();
    DrawSpeedPanel();
    DrawStatisticsPanel();
    DrawControlsPanel();
    DrawFooter();
}

char Menu::GetKeyPress() {
    if (_kbhit()) {
        return _getch();
    }
    return 0;
}

void Menu::ProcessInput(char key) {
    switch (key) {
    case '1': bot.SetUltraFastMode(); break;
    case '2': bot.SetCompetitiveMode(); break;
    case '3': bot.SetNormalMode(); break;
    case '4': bot.SetHumanizedMode(); break;
    case 'F': case 'f': showAdvanced = !showAdvanced; break;
    case 27: // ESC
        if (showAdvanced) showAdvanced = false;
        break;
    case 59: // F1
        if (bot.IsRunning()) bot.Stop();
        else bot.Start();
        break;
    case 60: // F2
        ShowSpeedSettings();
        break;
    case 61: // F3
        ShowHotkeySettings();
        break;
    case 62: // F4
        ShowDetectionSettings();
        break;
    }
}

void Menu::Run() {
    Initialize();

    // Don't auto-start the bot
    // bot.Start();

    bool running = true;
    while (running) {
        Display();

        if (_kbhit()) {
            char key = _getch();

            // Handle special keys (F1-F4)
            if (key == 0 || key == 0xE0) {
                key = _getch(); // Get extended key code
                switch (key) {
                case 59: // F1
                    if (bot.IsRunning()) bot.Stop();
                    else bot.Start();
                    break;
                case 60: // F2
                    ShowSpeedSettings();
                    break;
                case 61: // F3
                    ShowHotkeySettings();
                    break;
                case 62: // F4
                    ShowDetectionSettings();
                    break;
                }
            }
            else {
                switch (key) {
                case '1': bot.SetUltraFastMode(); break;
                case '2': bot.SetCompetitiveMode(); break;
                case '3': bot.SetNormalMode(); break;
                case '4': bot.SetHumanizedMode(); break;
                case 's': case 'S':
                    if (bot.IsRunning()) bot.Stop();
                    else bot.Start();
                    break;
                case '0':
                case 27: // ESC
                    running = false;
                    break;
                default:
                    // Handle number keys for menu items
                    if (key >= '1' && key <= '9') {
                        int index = key - '1';
                        if (index < mainMenuItems.size()) {
                            mainMenuItems[index].action();
                        }
                    }
                    break;
                }
            }
        }

        Sleep(100); // Refresh rate
    }

    bot.Stop();
    std::cout << "\nProgram terminated.\n";
    system("pause");
}

// Sub-menus implementation
void Menu::ShowSpeedSettings() {
    ClearScreen();
    DrawBox(10, 5, 60, 20, 14, true);
    DrawCenteredText(7, "⚙️ SPEED SETTINGS MENU", 14);

    DrawText(12, 9, "Adjust shoot delay (1-1000 ms):", 7);
    DrawText(12, 11, "Current: " + std::to_string(bot.GetShootDelay()) + " ms", 11);
    DrawText(12, 13, "[+] Increase   [-] Decrease", 10);
    DrawText(12, 15, "[ESC] Back to main menu", 8);

    bool inMenu = true;
    while (inMenu) {
        if (_kbhit()) {
            char key = _getch();
            switch (key) {
            case '+':
                bot.SetShootDelay(bot.GetShootDelay() + 10);
                break;
            case '-':
                bot.SetShootDelay(bot.GetShootDelay() - 10);
                break;
            case 27: // ESC
                inMenu = false;
                break;
            }

            // Update display
            ClearScreen();
            DrawBox(10, 5, 60, 20, 14, true);
            DrawCenteredText(7, "⚙️ SPEED SETTINGS MENU", 14);
            DrawText(12, 9, "Adjust shoot delay (1-1000 ms):", 7);
            DrawText(12, 11, "Current: " + std::to_string(bot.GetShootDelay()) + " ms", 11);
            DrawText(12, 13, "[+] Increase   [-] Decrease", 10);
            DrawText(12, 15, "[ESC] Back to main menu", 8);
        }
        Sleep(50);
    }
}

void Menu::ShowHotkeySettings() {
    ClearScreen();
    DrawBox(10, 5, 60, 20, 14, true);
    DrawCenteredText(7, "🎮 HOTKEY SETTINGS", 14);

    DrawText(12, 9, "Press a key to set as toggle hotkey:", 7);
    DrawText(12, 10, "Current: " + std::string(1, static_cast<char>(bot.GetHotkey())), 11);
    DrawText(12, 12, "Press ESC to cancel", 8);

    bool keyPressed = false;
    while (!keyPressed) {
        if (_kbhit()) {
            char key = _getch();
            if (key == 27) { // ESC
                break;
            }
            if (key >= 32 && key <= 126) { // Printable characters
                bot.SetHotkey(key);
                keyPressed = true;
            }
        }
        Sleep(50);
    }
}

void Menu::ShowDetectionSettings() {
    ClearScreen();
    DrawBox(10, 5, 60, 20, 14, true);
    DrawCenteredText(7, "🎯 DETECTION SETTINGS", 14);

    DrawText(12, 9, "Select detection mode:", 7);
    DrawText(12, 11, "[1] Color Detection", bot.GetDetectionMode() == 1 ? 10 : 7);
    DrawText(12, 12, "[2] Name Tag Detection", bot.GetDetectionMode() == 2 ? 10 : 7);
    DrawText(12, 13, "[3] Combined Detection", bot.GetDetectionMode() == 3 ? 10 : 7);
    DrawText(12, 14, "[4] Smart Detection", bot.GetDetectionMode() == 4 ? 10 : 7);
    DrawText(12, 16, "[ESC] Back to main menu", 8);

    bool inMenu = true;
    while (inMenu) {
        if (_kbhit()) {
            char key = _getch();
            switch (key) {
            case '1': bot.SetDetectionMode(1); break;
            case '2': bot.SetDetectionMode(2); break;
            case '3': bot.SetDetectionMode(3); break;
            case '4': bot.SetDetectionMode(4); break;
            case 27: // ESC
                inMenu = false;
                break;
            }

            // Update display
            ClearScreen();
            DrawBox(10, 5, 60, 20, 14, true);
            DrawCenteredText(7, "🎯 DETECTION SETTINGS", 14);
            DrawText(12, 9, "Select detection mode:", 7);
            DrawText(12, 11, "[1] Color Detection", bot.GetDetectionMode() == 1 ? 10 : 7);
            DrawText(12, 12, "[2] Name Tag Detection", bot.GetDetectionMode() == 2 ? 10 : 7);
            DrawText(12, 13, "[3] Combined Detection", bot.GetDetectionMode() == 3 ? 10 : 7);
            DrawText(12, 14, "[4] Smart Detection", bot.GetDetectionMode() == 4 ? 10 : 7);
            DrawText(12, 16, "[ESC] Back to main menu", 8);
        }
        Sleep(50);
    }
}

void Menu::ShowAdvancedSettings() {
    ClearScreen();
    DrawBox(10, 5, 60, 25, 13, true);
    DrawCenteredText(7, "🔧 ADVANCED SETTINGS", 14);

    DrawText(12, 9, "Team Check: " + std::string(bot.GetTeamCheck() ? "ENABLED" : "DISABLED"), 7);
    DrawText(12, 10, "Smart Aim: " + std::string(bot.GetSmartAim() ? "ENABLED" : "DISABLED"), 7);
    DrawText(12, 11, "Hold Click: " + std::string(bot.GetHoldClick() ? "ENABLED" : "DISABLED"), 7);
    DrawText(12, 12, "Burst Mode: " + std::string(bot.GetBurstMode() ? "ENABLED" : "DISABLED"), 7);

    DrawText(12, 14, "[T] Toggle Team Check", 10);
    DrawText(12, 15, "[S] Toggle Smart Aim", 10);
    DrawText(12, 16, "[H] Toggle Hold Click", 10);
    DrawText(12, 17, "[B] Toggle Burst Mode", 10);
    DrawText(12, 19, "[ESC] Back to main menu", 8);

    bool inMenu = true;
    while (inMenu) {
        if (_kbhit()) {
            char key = _getch();
            switch (key) {
            case 't': case 'T':
                bot.SetTeamCheck(!bot.GetTeamCheck());
                break;
            case 's': case 'S':
                bot.SetSmartAim(!bot.GetSmartAim());
                break;
            case 'h': case 'H':
                bot.SetHoldClick(!bot.GetHoldClick());
                break;
            case 'b': case 'B':
                bot.SetBurstMode(!bot.GetBurstMode());
                break;
            case 27: // ESC
                inMenu = false;
                break;
            }

            // Update display
            ClearScreen();
            DrawBox(10, 5, 60, 25, 13, true);
            DrawCenteredText(7, "🔧 ADVANCED SETTINGS", 14);
            DrawText(12, 9, "Team Check: " + std::string(bot.GetTeamCheck() ? "ENABLED" : "DISABLED"), 7);
            DrawText(12, 10, "Smart Aim: " + std::string(bot.GetSmartAim() ? "ENABLED" : "DISABLED"), 7);
            DrawText(12, 11, "Hold Click: " + std::string(bot.GetHoldClick() ? "ENABLED" : "DISABLED"), 7);
            DrawText(12, 12, "Burst Mode: " + std::string(bot.GetBurstMode() ? "ENABLED" : "DISABLED"), 7);

            DrawText(12, 14, "[T] Toggle Team Check", 10);
            DrawText(12, 15, "[S] Toggle Smart Aim", 10);
            DrawText(12, 16, "[H] Toggle Hold Click", 10);
            DrawText(12, 17, "[B] Toggle Burst Mode", 10);
            DrawText(12, 19, "[ESC] Back to main menu", 8);
        }
        Sleep(50);
    }
}

void Menu::ShowStatistics() {
    ClearScreen();
    DrawBox(10, 5, 60, 20, 11, true);
    DrawCenteredText(7, "📊 DETAILED STATISTICS", 14);

    DrawText(12, 9, "Total Shots Fired: " + std::to_string(bot.GetShotsFired()), 7);
    DrawText(12, 10, "Enemies Detected: " + std::to_string(bot.GetEnemiesDetected()), 7);
    DrawText(12, 11, "Successful Hits: " + std::to_string(bot.GetSuccessfulHits()), 7);
    DrawText(12, 12, "Accuracy: " + std::to_string(static_cast<int>(bot.GetAccuracy())) + "%", 7);
    DrawText(12, 13, "Clicks Per Second: " + std::to_string(static_cast<int>(bot.GetClicksPerSecond())), 7);
    DrawText(12, 14, "Runtime: " + std::to_string(bot.GetRuntimeMinutes()) + " minutes", 7);
    DrawText(12, 15, "Active Time: " + std::to_string(static_cast<int>(bot.GetActivePercentage())) + "%", 7);

    DrawText(12, 17, "[R] Reset Statistics", 12);
    DrawText(12, 18, "[ESC] Back to main menu", 8);

    bool inMenu = true;
    while (inMenu) {
        if (_kbhit()) {
            char key = _getch();
            switch (key) {
            case 'r': case 'R':
                bot.ResetStatistics();
                break;
            case 27: // ESC
                inMenu = false;
                break;
            }

            // Update display
            ClearScreen();
            DrawBox(10, 5, 60, 20, 11, true);
            DrawCenteredText(7, "📊 DETAILED STATISTICS", 14);
            DrawText(12, 9, "Total Shots Fired: " + std::to_string(bot.GetShotsFired()), 7);
            DrawText(12, 10, "Enemies Detected: " + std::to_string(bot.GetEnemiesDetected()), 7);
            DrawText(12, 11, "Successful Hits: " + std::to_string(bot.GetSuccessfulHits()), 7);
            DrawText(12, 12, "Accuracy: " + std::to_string(static_cast<int>(bot.GetAccuracy())) + "%", 7);
            DrawText(12, 13, "Clicks Per Second: " + std::to_string(static_cast<int>(bot.GetClicksPerSecond())), 7);
            DrawText(12, 14, "Runtime: " + std::to_string(bot.GetRuntimeMinutes()) + " minutes", 7);
            DrawText(12, 15, "Active Time: " + std::to_string(static_cast<int>(bot.GetActivePercentage())) + "%", 7);

            DrawText(12, 17, "[R] Reset Statistics", 12);
            DrawText(12, 18, "[ESC] Back to main menu", 8);
        }
        Sleep(50);
    }
}

void Menu::ShowCalibration() {
    ClearScreen();
    DrawBox(10, 5, 60, 20, 10, true);
    DrawCenteredText(7, "🎨 COLOR CALIBRATION", 14);

    DrawText(12, 9, "Color calibration not implemented yet.", 7);
    DrawText(12, 10, "This feature would help you calibrate", 7);
    DrawText(12, 11, "enemy detection colors for your game.", 7);

    DrawText(12, 13, "Press any key to continue...", 8);

    // Wait for key press
    while (!_kbhit()) {
        Sleep(50);
    }
    _getch(); // Clear the key
}