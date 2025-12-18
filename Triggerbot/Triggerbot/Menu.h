#pragma once
#include "TriggerBot.h"
#include <Windows.h>
#include <string>
#include <vector>
#include <functional>

class Menu {
private:
    TriggerBot& bot;
    HANDLE hConsole;
    bool showAdvanced;
    int selectedTab;

    struct MenuItem {
        std::string text;
        std::function<void()> action;
        bool isSelected;
    };

    std::vector<MenuItem> mainMenuItems;
    std::vector<std::string> tabs;

    // Drawing functions
    void DrawBox(int x, int y, int width, int height, int color, bool doubleLine = false);
    void DrawText(int x, int y, const std::string& text, int color = 7);
    void DrawCenteredText(int y, const std::string& text, int color = 7);
    void DrawProgressBar(int x, int y, int width, double percentage, int color);
    void DrawTabBar();

    // Color helpers
    int GetStatusColor(bool status) const;
    int GetSpeedColor(int delay) const;

    // Menu sections
    void DrawHeader();
    void DrawStatusPanel();
    void DrawSpeedPanel();
    void DrawStatisticsPanel();
    void DrawControlsPanel();
    void DrawAdvancedPanel();
    void DrawFooter();

    // Input handling
    char GetKeyPress();
    void ProcessInput(char key);

public:
    Menu(TriggerBot& botRef);
    ~Menu() = default;

    void Initialize();
    void Display();
    void Run();

    // Sub-menus
    void ShowSpeedSettings();
    void ShowHotkeySettings();
    void ShowDetectionSettings();
    void ShowAdvancedSettings();
    void ShowStatistics();
    void ShowCalibration();

    // Utility
    void ClearScreen();
    void SetConsoleSize(int width, int height);

    // Add the SetCursorPosition method declaration here
    void SetCursorPosition(int x, int y);  // Declaration
};
