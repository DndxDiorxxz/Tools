// Offsets.h
#pragma once
#include <cstdint>

namespace Offsets {
    // Placeholder offsets - these need to be found with Cheat Engine
    constexpr uintptr_t DATAMODEL = 0x00;
    constexpr uintptr_t WORKSPACE = 0x1000;
    constexpr uintptr_t PLAYERS = 0x2000;

    namespace Character {
        constexpr uintptr_t HUMANOID = 0x30;
        constexpr uintptr_t HEAD = 0x40;
        constexpr uintptr_t TORSO = 0x50;
        constexpr uintptr_t HEALTH = 0x60;
        constexpr uintptr_t MAXHEALTH = 0x64;
        constexpr uintptr_t TEAM = 0x70;
        constexpr uintptr_t POSITION = 0x80;
    }
}