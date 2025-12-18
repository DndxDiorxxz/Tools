// PlayerDetector.cpp
#include "PlayerDetector.h"
#include <iostream>
#include <fstream>

PlayerDetector::PlayerDetector() {
    FindRobloxWindow();

    // Default enemy colors (red spectrum)
    AddEnemyColor(150, 255, 0, 100, 0, 100);    // Red enemies
    AddEnemyColor(200, 255, 50, 150, 50, 150);  // Pink highlights
    AddEnemyColor(100, 200, 0, 50, 0, 50);      // Dark red outlines
}

bool PlayerDetector::FindRobloxWindow() {
    robloxWindow = FindWindowA(NULL, "Roblox");
    if (!robloxWindow) {
        robloxWindow = FindWindowA(NULL, "RobloxPlayerBeta");
    }
    if (!robloxWindow) {
        robloxWindow = FindWindowA(NULL, "RobloxStudio"); // For testing
    }
    return robloxWindow != nullptr;
}

void PlayerDetector::UpdateCrosshairPosition() {
    if (!robloxWindow) return;

    DWORD currentTime = GetTickCount();
    if (currentTime - lastPositionUpdate < 33) { // Update ~30 times/sec
        return;
    }

    RECT windowRect;
    if (GetWindowRect(robloxWindow, &windowRect)) {
        // Calculate center with padding for HUD elements
        crosshairX = windowRect.left + ((windowRect.right - windowRect.left) / 2);
        crosshairY = windowRect.top + ((windowRect.bottom - windowRect.top) / 2) + 5;
        lastPositionUpdate = currentTime;
    }
}

bool PlayerDetector::IsRobloxInFocus() const {
    if (!robloxWindow) return false;
    return GetForegroundWindow() == robloxWindow;
}

bool PlayerDetector::IsEnemyDetected() {
    if (!robloxWindow || !IsRobloxInFocus()) return false;

    UpdateCrosshairPosition();
    HDC hdc = GetDC(nullptr);
    if (!hdc) return false;

    bool enemyFound = false;

    // Smart scanning: Check fewer points but intelligently
    // Check center first (most likely)
    COLORREF centerColor = GetPixel(hdc, crosshairX, crosshairY);
    if (IsEnemyColor(centerColor)) {
        ReleaseDC(nullptr, hdc);
        return true;
    }

    // Check key points around crosshair in a pattern
    int pattern[][2] = {
        {0, -detectionRadius},    // Top
        {0, detectionRadius},     // Bottom
        {-detectionRadius, 0},    // Left
        {detectionRadius, 0},     // Right
        {-10, -10},               // Top-left
        {10, -10},                // Top-right
        {-10, 10},                // Bottom-left
        {10, 10}                  // Bottom-right
    };

    for (int i = 0; i < 8; i++) {
        int x = crosshairX + pattern[i][0];
        int y = crosshairY + pattern[i][1];

        COLORREF pixelColor = GetPixel(hdc, x, y);
        if (IsEnemyColor(pixelColor)) {
            enemyFound = true;
            break;
        }
    }

    ReleaseDC(nullptr, hdc);
    return enemyFound;
}

bool PlayerDetector::IsPlayerNameDetected() {
    if (!robloxWindow || !IsRobloxInFocus()) return false;

    UpdateCrosshairPosition();
    HDC hdc = GetDC(nullptr);
    if (!hdc) return false;

    // Name tags are usually above the head
    int nameAreaY = crosshairY - 25;
    int nameAreaHeight = 15;
    int nameAreaWidth = 60;

    bool nameFound = false;

    for (int x = crosshairX - (nameAreaWidth / 2); x <= crosshairX + (nameAreaWidth / 2); x += 3) {
        for (int y = nameAreaY; y <= nameAreaY + nameAreaHeight; y += 3) {
            COLORREF pixelColor = GetPixel(hdc, x, y);
            int red = GetRValue(pixelColor);
            int green = GetGValue(pixelColor);
            int blue = GetBValue(pixelColor);

            // Name tags are bright white/light colors
            if (red >= minNameTagBrightness &&
                green >= minNameTagBrightness &&
                blue >= minNameTagBrightness) {
                nameFound = true;
                break;
            }
        }
        if (nameFound) break;
    }

    ReleaseDC(nullptr, hdc);
    return nameFound;
}

bool PlayerDetector::IsPlayerDetected() {
    // Cache detection for 50ms to improve performance
    DWORD currentTime = GetTickCount();
    if (currentTime - lastDetectionTime < 50) {
        return lastDetection;
    }

    bool detected = IsEnemyDetected() || IsPlayerNameDetected();
    lastDetection = detected;
    lastDetectionTime = currentTime;

    return detected;
}

bool PlayerDetector::IsEnemyColor(COLORREF color) const {
    int red = GetRValue(color);
    int green = GetGValue(color);
    int blue = GetBValue(color);

    // Check against predefined enemy color ranges
    for (const auto& enemyColor : enemyColors) {
        if (red >= enemyColor.minR && red <= enemyColor.maxR &&
            green >= enemyColor.minG && green <= enemyColor.maxG &&
            blue >= enemyColor.minB && blue <= enemyColor.maxB) {
            return true;
        }
    }

    // Fallback: Simple red detection
    if (red >= minRedThreshold && green <= maxOtherColors && blue <= maxOtherColors) {
        return true;
    }

    return false;
}

bool PlayerDetector::IsTeamColor(COLORREF color) const {
    int red = GetRValue(color);
    int green = GetGValue(color);
    int blue = GetBValue(color);

    // Team colors are usually blue/green
    if (blue > red + 50 && blue > green + 50) return true;  // Blue team
    if (green > red + 50 && green > blue + 50) return true;  // Green team

    return false;
}

bool PlayerDetector::ScanArea(int startX, int startY, int width, int height) {
    HDC hdc = GetDC(nullptr);
    if (!hdc) return false;

    for (int x = startX; x < startX + width; x += 2) {
        for (int y = startY; y < startY + height; y += 2) {
            if (IsEnemyColor(GetPixel(hdc, x, y))) {
                ReleaseDC(nullptr, hdc);
                return true;
            }
        }
    }

    ReleaseDC(nullptr, hdc);
    return false;
}

bool PlayerDetector::CheckForOutline(COLORREF centerColor, COLORREF surroundingColor) {
    // Check if center is bright and surrounded by enemy color
    int centerBrightness = (GetRValue(centerColor) + GetGValue(centerColor) + GetBValue(centerColor)) / 3;
    int surroundBrightness = (GetRValue(surroundingColor) + GetGValue(surroundingColor) + GetBValue(surroundingColor)) / 3;

    return centerBrightness > 180 && surroundBrightness < 100;
}

void PlayerDetector::SetDetectionRadius(int radius) {
    detectionRadius = (radius < 5) ? 5 : (radius > 50 ? 50 : radius);
}

void PlayerDetector::SetColorThresholds(int redMin, int otherMax) {
    minRedThreshold = redMin;
    maxOtherColors = otherMax;
}

void PlayerDetector::SetNameTagBrightness(int brightness) {
    minNameTagBrightness = (brightness < 180) ? 180 : (brightness > 255 ? 255 : brightness);
}

void PlayerDetector::AddEnemyColor(int minR, int maxR, int minG, int maxG, int minB, int maxB) {
    enemyColors.push_back({ minR, maxR, minG, maxG, minB, maxB });
}

bool PlayerDetector::IsRobloxRunning() const {
    return robloxWindow != nullptr;
}

void PlayerDetector::SaveScreenshot(const std::string& filename) {
    if (!robloxWindow) return;

    RECT rect;
    GetWindowRect(robloxWindow, &rect);

    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
    SelectObject(hdcMem, hBitmap);

    BitBlt(hdcMem, 0, 0, width, height, hdcScreen, rect.left, rect.top, SRCCOPY);

    // Save bitmap to file (simplified)
    // In real implementation, you'd save as PNG

    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
}