// Main.cpp - Simple version
#include "TriggerBot.h"
#include <iostream>

int main() {
    std::cout << "=== Roblox TriggerBot ===\n";
    std::cout << "Controls:\n";
    std::cout << "  F - Toggle bot\n";
    std::cout << "  G - Disable bot\n";
    std::cout << "  F12 - Panic stop\n";
    std::cout << "  CTRL - Simulate enemy detection\n";
    std::cout << "  SHIFT - Simulate name detection\n";
    std::cout << "  ESC - Exit\n\n";

    TriggerBot bot;
    bot.Start();

    bool running = true;
    while (running) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            running = false;
        }

        // Simple status display
        system("cls");
        std::cout << "=== Roblox TriggerBot Status ===\n\n";
        std::cout << "Bot Status: " << (bot.IsRunning() ? "RUNNING" : "STOPPED") << "\n";
        std::cout << "Bot Enabled: " << (bot.IsEnabled() ? "YES" : "NO") << "\n";
        std::cout << "Roblox Detected: " << (bot.IsRobloxDetected() ? "YES" : "NO") << "\n";
        std::cout << "Roblox In Focus: " << (bot.IsRobloxInFocus() ? "YES" : "NO") << "\n";
        std::cout << "Shots Fired: " << bot.GetShotsFired() << "\n";
        std::cout << "CPS: " << bot.GetClicksPerSecond() << "\n";
        std::cout << "Accuracy: " << bot.GetAccuracy() << "%\n";
        std::cout << "\nPress ESC to exit...\n";

        Sleep(100);
    }

    bot.Stop();
    std::cout << "\nProgram terminated.\n";

    return 0;
}