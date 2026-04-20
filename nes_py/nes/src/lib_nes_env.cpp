//  Program:      nes-py
//  File:         lib_nes_env.cpp
//  Description:  file describes the outward facing ctypes API for Python
//
//  Copyright (c) 2019 Christian Kauten. All rights reserved.
//

#include <string>
#include "common.hpp"
#include "emulator.hpp"

// Windows-base systems
#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
    // setup the module initializer. required to link visual studio C++ ctypes
    void PyInit_lib_nes_env() { }
    // setup the function modifier to export in the DLL
    #define EXP __declspec(dllexport)
// Unix-like systems
#else
    // setup the modifier as a dummy
    #define EXP
#endif

// definitions of functions for the Python interface to access
extern "C" {
    /// Return the width of the NES.
    EXP int Width() {
        return NES::Emulator::WIDTH;
    }

    /// Return the height of the NES.
    EXP int Height() {
        return NES::Emulator::HEIGHT;
    }

    /// Initialize a new emulator and return a pointer to it
    EXP NES::Emulator* Initialize(wchar_t* path) {
        // convert the c string to a c++ std string data structure
        std::wstring ws_rom_path(path);
        std::string rom_path(ws_rom_path.begin(), ws_rom_path.end());
        // create a new emulator with the given ROM path
        return new NES::Emulator(rom_path);
    }

    /// Return a pointer to a controller on the machine
    EXP NES::NES_Byte* Controller(NES::Emulator* emu, int port) {
        return emu->get_controller(port);
    }

    /// Return the pointer to the screen buffer
    EXP NES::NES_Pixel* Screen(NES::Emulator* emu) {
        return emu->get_screen_buffer();
    }

    /// Return the pointer to the memory buffer
    EXP NES::NES_Byte* Memory(NES::Emulator* emu) {
        return emu->get_memory_buffer();
    }

    /// Reset the emulator
    EXP void Reset(NES::Emulator* emu) {
        emu->reset();
    }

    /// Perform a discrete step in the emulator (i.e., 1 frame)
    EXP void Step(NES::Emulator* emu) {
        emu->step();
    }

    /// Create a deep copy (i.e., a clone) of the given emulator
    EXP void Backup(NES::Emulator* emu) {
        emu->backup();
    }

    /// Create a deep copy (i.e., a clone) of the given emulator
    EXP void Restore(NES::Emulator* emu) {
        emu->restore();
    }

    /// Close the emulator, i.e., purge it from memory
    EXP void Close(NES::Emulator* emu) {
        delete emu;
    }

    /// Add a Game Genie cheat. `compare` is -1 to disable compare matching
    /// (6-letter code semantics), or 0..255 for 8-letter semantics. Returns
    /// the index of the added cheat (currently informational only).
    EXP int AddCheat(NES::Emulator* emu, unsigned int addr,
                     unsigned char value, int compare) {
        return emu->add_cheat(
            static_cast<NES::NES_Address>(addr & 0xFFFF),
            static_cast<NES::NES_Byte>(value),
            static_cast<int16_t>(compare)
        );
    }

    /// Remove the first cheat matching (addr, value, compare). Returns 1 if
    /// a cheat was removed, 0 otherwise.
    EXP int RemoveCheat(NES::Emulator* emu, unsigned int addr,
                        unsigned char value, int compare) {
        return emu->remove_cheat(
            static_cast<NES::NES_Address>(addr & 0xFFFF),
            static_cast<NES::NES_Byte>(value),
            static_cast<int16_t>(compare)
        );
    }

    /// Remove all active cheats.
    EXP void ClearCheats(NES::Emulator* emu) {
        emu->clear_cheats();
    }

    /// Return the number of active cheats.
    EXP int CheatCount(NES::Emulator* emu) {
        return emu->cheat_count();
    }
}

// un-define the macro
#undef EXP
