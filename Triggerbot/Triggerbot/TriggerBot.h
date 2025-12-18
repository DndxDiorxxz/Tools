// TriggerBot.h - UPDATED
#pragma once
#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>
#include "Detector.h"

class TriggerBot {
private:
    // State variables
    std::atomic<bool> running;
    std::atomic<bool> enabled;
    std::mutex configMutex;
    std::thread botThread;

    // Configuration
    int shootDelay;
    int rapidFireCount;
    int burstDelay;
    int clickDuration;
    int hotkey;
    int toggleKey;
    int panicKey;
    int detectionMode;
    bool teamCheck;
    bool smartAim;
    bool holdClick;
    bool burstMode;
    int reactionTime;
    int maxDistance;

    // Statistics
    std::atomic<int> shotsFired;
    std::atomic<int> enemiesDetected;
    std::atomic<int> successfulHits;
    std::atomic<double> clicksPerSecond;
    std::atomic<double> accuracy;
    std::atomic<int> totalRuntime;
    std::atomic<int> activeRuntime;

    // Detector
    Detector detector;
    std::chrono::steady_clock::time_point startTime;

    // Internal methods
    void BotLoop();
    void HandleHotkeys();
    bool ShouldShoot();
    bool IsValidTarget();

public:
    TriggerBot();
    ~TriggerBot();

    // Control methods
    void Start();
    void Stop();
    void Restart();
    void PanicStop();
    void EmergencyStop();

    // Click simulation
    void SimulateClick();
    void SimulateHumanizedClick();
    void SimulateBurstFire();
    void SimulateRapidClick(int count);
    void SimulateMousePress();
    void SimulateMouseRelease();

    // Speed presets
    void SetUltraFastMode();
    void SetCompetitiveMode();
    void SetNormalMode();
    void SetHumanizedMode();
    void SetCustomMode(int delay, int count, int burst);

    // Setters
    void SetShootDelay(int delay);
    void SetRapidFireCount(int count);
    void SetBurstDelay(int delay);
    void SetClickDuration(int duration);
    void SetHotkey(int key);
    void SetToggleKey(int key);
    void SetPanicKey(int key);
    void SetDetectionMode(int mode);
    void SetTeamCheck(bool check);
    void SetSmartAim(bool aim);
    void SetReactionTime(int ms);
    void SetMaxDistance(int pixels);
    void SetHoldClick(bool hold);
    void SetBurstMode(bool burst);
    void Enable();
    void Disable();
    void Toggle();

    // Getters
    bool IsRunning() const;
    bool IsEnabled() const;
    int GetShootDelay() const;
    int GetRapidFireCount() const;
    int GetBurstDelay() const;
    int GetClickDuration() const;
    int GetHotkey() const;
    int GetToggleKey() const;
    int GetPanicKey() const;
    int GetDetectionMode() const;
    double GetClicksPerSecond() const;
    double GetAccuracy() const;
    int GetShotsFired() const;
    int GetEnemiesDetected() const;
    int GetSuccessfulHits() const;
    bool IsRobloxDetected() const;
    bool IsRobloxInFocus() const;

    // New getters
    bool GetTeamCheck() const { return teamCheck; }
    bool GetSmartAim() const { return smartAim; }
    bool GetHoldClick() const { return holdClick; }
    bool GetBurstMode() const { return burstMode; }

    // Statistics
    double GetUptime() const;
    double GetActivePercentage() const;
    int GetRuntimeMinutes() const;
    void ResetStatistics();

    // Utility
    void TestDetection();
    void CalibrateColors();
};