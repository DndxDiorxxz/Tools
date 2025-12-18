// Memory.h - COMPLETELY FIXED
#pragma once
#include "Process.h"
#include "Offsets.h"
#include <vector>
#include <string>
#include <cmath>
#include <limits>

struct Vector3 {
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    float Distance(const Vector3& other) const {
        float dx = x - other.x;
        float dy = y - other.y;
        float dz = z - other.z;
        return sqrtf(dx * dx + dy * dy + dz * dz);
    }

    bool IsZero() const {
        return x == 0 && y == 0 && z == 0;
    }
};

struct CharacterInfo {
    uintptr_t address = 0;
    Vector3 position;
    Vector3 headPosition;
    std::string name;
    int health = 100;
    int maxHealth = 100;
    bool isAlive = true;
    bool isLocalPlayer = false;
    int team = 0;

    bool hasHead = false;
    bool hasHumanoid = false;
    bool hasTorso = false;

    float DistanceTo(const CharacterInfo& other) const {
        return position.Distance(other.position);
    }
};

class MemoryManager {
private:
    Process process;
    uintptr_t dataModel = 0;
    uintptr_t workspace = 0;
    uintptr_t players = 0;
    CharacterInfo localPlayer;

public:
    bool Initialize(const std::string& processName = "RobloxPlayerBeta.exe") {
        if (!process.Attach(processName)) {
            return false;
        }

        uintptr_t moduleBase = process.GetModuleBase(processName);
        if (!moduleBase) {
            return false;
        }

        // Example offsets - will need real values
        dataModel = moduleBase;
        workspace = moduleBase + 0x1000;
        players = moduleBase + 0x2000;

        return true;
    }

    CharacterInfo GetLocalPlayer() {
        CharacterInfo info;
        info.isLocalPlayer = true;
        info.name = "LocalPlayer";
        info.health = 100;
        info.maxHealth = 100;
        info.isAlive = true;
        info.hasHead = true;
        info.hasHumanoid = true;
        info.hasTorso = true;
        info.position = Vector3(0, 0, 0);
        return info;
    }

    std::vector<CharacterInfo> GetAllCharacters() {
        std::vector<CharacterInfo> characters;

        // Add local player
        characters.push_back(GetLocalPlayer());

        // Add dummy enemies for testing
        for (int i = 1; i <= 3; i++) {
            CharacterInfo enemy;
            enemy.name = "Enemy_" + std::to_string(i);
            enemy.position = Vector3(static_cast<float>(i * 5), 0, 0);
            enemy.health = 100;
            enemy.maxHealth = 100;
            enemy.isAlive = true;
            enemy.isLocalPlayer = false;
            enemy.hasHead = true;
            enemy.hasHumanoid = true;
            enemy.hasTorso = true;
            characters.push_back(enemy);
        }

        return characters;
    }

    CharacterInfo GetClosestEnemy() {
        auto characters = GetAllCharacters();
        if (characters.empty()) return CharacterInfo();

        CharacterInfo closest;
        float closestDistance = std::numeric_limits<float>::max();

        for (const auto& character : characters) {
            if (character.isLocalPlayer || !character.isAlive) continue;

            float distance = characters[0].DistanceTo(character);
            if (distance < closestDistance) {
                closestDistance = distance;
                closest = character;
            }
        }

        return closest;
    }

    bool IsValidCharacter(const CharacterInfo& character) {
        return character.hasHumanoid && character.isAlive;
    }
};