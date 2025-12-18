// PlayerDetector.h
#pragma once
#include <Windows.h>
#include <string>
#include <vector>

class PlayerDetector {
private:
    HWND robloxWindow = nullptr;

    // Detection settings
    int detectionRadius = 15;
    int minRedThreshold = 120;
    int maxOtherColors = 60;
    int minNameTagBrightness = 220;

    // Crosshair tracking
    int crosshairX = 0;
    int crosshairY = 0;
    DWORD lastPositionUpdate = 0;

    // Detection cache
    bool lastDetection = false;
    DWORD lastDetectionTime = 0;

    // Multi-color detection
    struct EnemyColor {
        int minR, maxR, minG, maxG, minB, maxB;
    };
    std::vector<EnemyColor> enemyColors;

public:
    PlayerDetector();
    ~PlayerDetector() = default;

    bool FindRobloxWindow();
    void UpdateCrosshairPosition();
    bool IsRobloxInFocus() const;

    // Detection methods
    bool IsEnemyDetected();
    bool IsPlayerNameDetected();
    bool IsPlayerDetected();
    bool IsTeamColor(COLORREF color) const;
    bool IsEnemyColor(COLORREF color) const;

    // Advanced detection
    bool ScanArea(int startX, int startY, int width, int height);
    bool CheckForOutline(COLORREF centerColor, COLORREF surroundingColor);

    // Configuration
    void SetDetectionRadius(int radius);
    void SetColorThresholds(int redMin, int otherMax);
    void SetNameTagBrightness(int brightness);
    void AddEnemyColor(int minR, int maxR, int minG, int maxG, int minB, int maxB);

    // Getters
    bool IsRobloxRunning() const;
    int GetCrosshairX() const { return crosshairX; }
    int GetCrosshairY() const { return crosshairY; }
    int GetDetectionRadius() const { return detectionRadius; }

    // Utility
    void SaveScreenshot(const std::string& filename);
};