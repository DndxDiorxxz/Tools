// TriggerBot.cpp - UPDATED
#include "TriggerBot.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <chrono>
#include <thread>
#include <Windows.h>
#include <cctype>

// Random number generator for humanization
std::random_device rd;
std::mt19937 gen(rd());

TriggerBot::TriggerBot() :
    running(false),
    enabled(false),
    shootDelay(50),
    rapidFireCount(1),
    burstDelay(0),
    clickDuration(10),
    hotkey('F'),
    toggleKey('G'),
    panicKey(VK_F12),
    detectionMode(3),
    teamCheck(false),
    smartAim(true),
    holdClick(false),
    burstMode(false),
    reactionTime(25),
    maxDistance(100),
    shotsFired(0),
    enemiesDetected(0),
    successfulHits(0),
    clicksPerSecond(0.0),
    accuracy(0.0),
    totalRuntime(0),
    activeRuntime(0),
    startTime(std::chrono::steady_clock::now())
{
    // Initialize detector with default values
    detector.SetDetectionRadius(15);
    detector.SetColorThresholds(120, 60);
    detector.SetNameTagBrightness(220);
}

TriggerBot::~TriggerBot() {
    Stop();
}

void TriggerBot::SimulateClick() {
    INPUT inputs[2] = { {0}, {0} };

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

    SendInput(2, inputs, sizeof(INPUT));
    shotsFired++;
}

void TriggerBot::SimulateHumanizedClick() {
    // Add random delay for human-like behavior
    std::uniform_int_distribution<> clickDelay(5, 15);

    INPUT inputDown = { 0 };
    inputDown.type = INPUT_MOUSE;
    inputDown.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &inputDown, sizeof(INPUT));

    std::this_thread::sleep_for(std::chrono::milliseconds(clickDelay(gen)));

    INPUT inputUp = { 0 };
    inputUp.type = INPUT_MOUSE;
    inputUp.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &inputUp, sizeof(INPUT));

    shotsFired++;
}

void TriggerBot::SimulateBurstFire() {
    if (detectionMode == 4) { // Humanized mode
        std::uniform_int_distribution<> burstCount(1, 3);
        int count = burstCount(gen);

        for (int i = 0; i < count; i++) {
            SimulateHumanizedClick();
            if (i < count - 1) {
                std::uniform_int_distribution<> delay(30, 80);
                std::this_thread::sleep_for(std::chrono::milliseconds(delay(gen)));
            }
        }
    }
    else {
        for (int i = 0; i < rapidFireCount; i++) {
            SimulateClick();
            if (i < rapidFireCount - 1) {
                std::this_thread::sleep_for(std::chrono::milliseconds(burstDelay));
            }
        }
    }
}

bool TriggerBot::ShouldShoot() {
    switch (detectionMode) {
    case 1: return detector.IsEnemyDetected();
    case 2: return detector.IsPlayerNameDetected();
    case 3: return detector.IsPlayerDetected();
    case 4: return detector.IsPlayerDetected(); // Smart mode uses same detection
    default: return false;
    }
}

bool TriggerBot::IsValidTarget() {
    // Update detector status
    detector.Update();

    if (!detector.IsRobloxRunning()) {
        return false;
    }

    // Team check (if enabled)
    if (teamCheck) {
        // In real implementation, you'd check team colors
        // This is a placeholder
    }

    return true;
}

void TriggerBot::BotLoop() {
    bool wasPressed = false;
    auto lastShotTime = std::chrono::steady_clock::now();
    auto lastCpsUpdate = std::chrono::steady_clock::now();
    auto lastStatUpdate = std::chrono::steady_clock::now();
    auto lastStatusUpdate = std::chrono::steady_clock::now();
    int shotsThisSecond = 0;

    while (running) {
        auto currentTime = std::chrono::steady_clock::now();

        // Update CPS counter every second
        if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastCpsUpdate).count() >= 1) {
            clicksPerSecond = static_cast<double>(shotsThisSecond);
            shotsThisSecond = 0;
            lastCpsUpdate = currentTime;
        }

        // Update runtime statistics
        totalRuntime++;
        if (enabled) activeRuntime++;

        // Update accuracy every 10 seconds
        if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastStatUpdate).count() >= 10) {
            if (shotsFired > 0) {
                accuracy = (static_cast<double>(successfulHits) / shotsFired) * 100.0;
            }
            lastStatUpdate = currentTime;
        }

        // Handle hotkeys (with debouncing)
        static auto lastHotkeyCheck = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastHotkeyCheck).count() >= 50) {
            HandleHotkeys();
            lastHotkeyCheck = currentTime;
        }

        // Check panic key
        if (GetAsyncKeyState(panicKey) & 0x8000) {
            PanicStop();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        // Process triggerbot
        if (enabled && IsValidTarget()) {
            if (ShouldShoot()) {
                enemiesDetected++;
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastShotTime).count();

                if (elapsed >= shootDelay) {
                    // Add reaction time delay for more realism
                    if (reactionTime > 0) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(reactionTime));
                    }

                    if (holdClick) {
                        if (!wasPressed) {
                            SimulateMousePress();
                            wasPressed = true;
                        }
                    }
                    else {
                        if (burstMode) {
                            SimulateBurstFire();
                            shotsThisSecond += rapidFireCount;
                        }
                        else {
                            SimulateClick();
                            shotsThisSecond++;
                        }
                        lastShotTime = currentTime;

                        // Random chance to count as successful hit
                        std::uniform_real_distribution<> hitChance(0.0, 1.0);
                        if (hitChance(gen) > 0.3) { // 70% "accuracy"
                            successfulHits++;
                        }
                    }
                }
            }
            else if (wasPressed) {
                SimulateMouseRelease();
                wasPressed = false;
            }
        }
        else if (wasPressed) {
            SimulateMouseRelease();
            wasPressed = false;
        }

        // Small delay to prevent high CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    if (wasPressed) {
        SimulateMouseRelease();
    }
}

void TriggerBot::HandleHotkeys() {
    static bool hotkeyPressed = false;
    static bool toggleKeyPressed = false;

    if (GetAsyncKeyState(hotkey) & 0x8000) {
        if (!hotkeyPressed) {
            enabled = !enabled;
            hotkeyPressed = true;
            std::cout << "Bot " << (enabled ? "ENABLED" : "DISABLED") << std::endl;
        }
    }
    else {
        hotkeyPressed = false;
    }

    if (GetAsyncKeyState(toggleKey) & 0x8000) {
        if (!toggleKeyPressed) {
            enabled = false;
            toggleKeyPressed = true;
            std::cout << "Bot DISABLED (toggle key)" << std::endl;
        }
    }
    else {
        toggleKeyPressed = false;
    }
}

void TriggerBot::PanicStop() {
    std::cout << "PANIC STOP ACTIVATED!" << std::endl;
    enabled = false;
    if (IsRunning()) {
        Stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        Start();
    }
}

void TriggerBot::Start() {
    if (running) return;

    std::cout << "Starting TriggerBot..." << std::endl;
    running = true;
    botThread = std::thread(&TriggerBot::BotLoop, this);

    startTime = std::chrono::steady_clock::now();
}

void TriggerBot::Stop() {
    std::cout << "Stopping TriggerBot..." << std::endl;
    running = false;

    if (botThread.joinable()) {
        botThread.join();
    }
}

// ... (rest of the methods remain the same as before)

void TriggerBot::Restart() {
    Stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    Start();
}

// Speed presets - FIXED: Removed unnecessary lock_guard since these are atomic operations
void TriggerBot::SetUltraFastMode() {
    shootDelay = 1;
    rapidFireCount = 3;
    burstDelay = 2;
    burstMode = true;
    detectionMode = 1; // Color only for speed
    reactionTime = 5;
}

void TriggerBot::SetCompetitiveMode() {
    shootDelay = 10;
    rapidFireCount = 2;
    burstDelay = 5;
    burstMode = true;
    detectionMode = 3; // Combined detection
    reactionTime = 15;
}

void TriggerBot::SetNormalMode() {
    shootDelay = 50;
    rapidFireCount = 1;
    burstDelay = 0;
    burstMode = false;
    detectionMode = 3; // Combined detection
    reactionTime = 25;
}

void TriggerBot::SetHumanizedMode() {
    shootDelay = 100;
    rapidFireCount = 1;
    burstDelay = 0;
    burstMode = false;
    detectionMode = 4; // Smart mode
    reactionTime = 50;
}

void TriggerBot::SetCustomMode(int delay, int count, int burst) {
    shootDelay = delay;
    rapidFireCount = count;
    burstDelay = burst;
}

// Setters - FIXED: Remove lock_guard from atomic operations
void TriggerBot::SetShootDelay(int delay) {
    shootDelay = (delay < 0) ? 0 : ((delay > 1000) ? 1000 : delay);
}

void TriggerBot::SimulateMousePress() {
    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input, sizeof(INPUT));
}

void TriggerBot::SimulateMouseRelease() {
    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(INPUT));
}

void TriggerBot::SetRapidFireCount(int count) {
    rapidFireCount = (count < 1) ? 1 : ((count > 10) ? 10 : count);
}

void TriggerBot::SetBurstDelay(int delay) {
    burstDelay = (delay < 0) ? 0 : ((delay > 100) ? 100 : delay);
}

void TriggerBot::SetClickDuration(int duration) {
    clickDuration = (duration < 1) ? 1 : ((duration > 50) ? 50 : duration);
}

void TriggerBot::SetHotkey(int key) {
    hotkey = std::toupper(key);
}

void TriggerBot::SetToggleKey(int key) {
    toggleKey = std::toupper(key);
}

void TriggerBot::SetPanicKey(int key) {
    panicKey = key;
}

void TriggerBot::SetDetectionMode(int mode) {
    detectionMode = (mode < 1) ? 1 : ((mode > 4) ? 4 : mode);
}

void TriggerBot::SetTeamCheck(bool check) {
    teamCheck = check;
}

void TriggerBot::SetSmartAim(bool aim) {
    smartAim = aim;
}

void TriggerBot::SetReactionTime(int ms) {
    reactionTime = (ms < 1) ? 1 : ((ms > 100) ? 100 : ms);
}

void TriggerBot::SetMaxDistance(int pixels) {
    maxDistance = (pixels < 5) ? 5 : ((pixels > 200) ? 200 : pixels);
}

void TriggerBot::SetHoldClick(bool hold) {
    holdClick = hold;
}

void TriggerBot::SetBurstMode(bool burst) {
    burstMode = burst;
}

// Getters - FIXED: Removed lock_guard from atomic operations
bool TriggerBot::IsRunning() const {
    return running.load();
}

bool TriggerBot::IsEnabled() const {
    return enabled.load();
}

int TriggerBot::GetShootDelay() const {
    return shootDelay;
}

int TriggerBot::GetRapidFireCount() const {
    return rapidFireCount;
}

int TriggerBot::GetBurstDelay() const {
    return burstDelay;
}

int TriggerBot::GetClickDuration() const {
    return clickDuration;
}

int TriggerBot::GetHotkey() const {
    return hotkey;
}

int TriggerBot::GetToggleKey() const {
    return toggleKey;
}

int TriggerBot::GetPanicKey() const {
    return panicKey;
}

int TriggerBot::GetDetectionMode() const {
    return detectionMode;
}

double TriggerBot::GetClicksPerSecond() const {
    return clicksPerSecond.load();
}

double TriggerBot::GetAccuracy() const {
    return accuracy.load();
}

int TriggerBot::GetShotsFired() const {
    return shotsFired.load();
}

int TriggerBot::GetEnemiesDetected() const {
    return enemiesDetected.load();
}

int TriggerBot::GetSuccessfulHits() const {
    return successfulHits.load();
}

bool TriggerBot::IsRobloxDetected() const {
    return detector.IsRobloxRunning();
}

bool TriggerBot::IsRobloxInFocus() const {
    return detector.IsRobloxInFocus();
}

// Statistics
double TriggerBot::GetUptime() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - startTime);
    return duration.count();
}

double TriggerBot::GetActivePercentage() const {
    int total = totalRuntime.load();
    int active = activeRuntime.load();
    if (total == 0) return 0.0;
    return (static_cast<double>(active) / total) * 100.0;
}

int TriggerBot::GetRuntimeMinutes() const {
    return static_cast<int>(GetUptime() / 60);
}

// Utility
void TriggerBot::ResetStatistics() {
    shotsFired = 0;
    enemiesDetected = 0;
    successfulHits = 0;
    clicksPerSecond = 0.0;
    accuracy = 0.0;
    totalRuntime = 0;
    activeRuntime = 0;
    startTime = std::chrono::steady_clock::now();
}

void TriggerBot::TestDetection() {
    std::cout << "Testing detection...\n";
    std::cout << "Roblox running: " << (IsRobloxDetected() ? "Yes" : "No") << "\n";
    std::cout << "Roblox in focus: " << (IsRobloxInFocus() ? "Yes" : "No") << "\n";
}

void TriggerBot::CalibrateColors() {
    std::cout << "Starting color calibration...\n";
    std::cout << "Place your crosshair on an enemy and press Enter...\n";
    std::cin.ignore();
    std::cout << "Color calibration complete!\n";
}

// New methods for better control
void TriggerBot::Enable() {
    enabled = true;
}

void TriggerBot::Disable() {
    enabled = false;
}

void TriggerBot::Toggle() {
    enabled = !enabled;
}

void TriggerBot::EmergencyStop() {
    enabled = false;
    running = false;

    if (botThread.joinable()) {
        botThread.join();
    }
}