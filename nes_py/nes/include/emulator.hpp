//  Program:      nes-py
//  File:         emulator.hpp
//  Description:  This class houses the logic and data for an NES emulator
//
//  Copyright (c) 2019 Christian Kauten. All rights reserved.
//

#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <string>
#include <memory>
#include "common.hpp"
#include "cartridge.hpp"
#include "cheat.hpp"
#include "controller.hpp"
#include "cpu.hpp"
#include "ppu.hpp"
#include "main_bus.hpp"
#include "picture_bus.hpp"

namespace NES {

/// An NES Emulator and OpenAI Gym interface
class Emulator {
 private:
    /// The number of cycles in 1 frame
    static const int CYCLES_PER_FRAME = 29781;
    /// the virtual cartridge with ROM and mapper data
    Cartridge cartridge;
    /// the 2 controllers on the emulator
    Controller controllers[2];

    /// the main data bus of the emulator
    MainBus bus;
    /// the picture bus from the PPU of the emulator
    PictureBus picture_bus;
    /// The emulator's CPU
    CPU cpu;
    /// the emulators' PPU
    PPU ppu;

    /// the main data bus of the emulator
    MainBus backup_bus;
    /// the picture bus from the PPU of the emulator
    PictureBus backup_picture_bus;
    /// The emulator's CPU
    CPU backup_cpu;
    /// the emulators' PPU
    PPU backup_ppu;

    /// Shared cheat table (single instance across live + backup buses).
    std::shared_ptr<CheatTable> cheats;

 public:
    /// The width of the NES screen in pixels
    static const int WIDTH = SCANLINE_VISIBLE_DOTS;
    /// The height of the NES screen in pixels
    static const int HEIGHT = VISIBLE_SCANLINES;

    /// Initialize a new emulator with a path to a ROM file.
    ///
    /// @param rom_path the path to the ROM for the emulator to run
    ///
    explicit Emulator(std::string rom_path);

    /// Return a 32-bit pointer to the screen buffer's first address.
    ///
    /// @return a 32-bit pointer to the screen buffer's first address
    ///
    inline NES_Pixel* get_screen_buffer() { return ppu.get_screen_buffer(); }

    /// Return a 8-bit pointer to the RAM buffer's first address.
    ///
    /// @return a 8-bit pointer to the RAM buffer's first address
    ///
    inline NES_Byte* get_memory_buffer() { return bus.get_memory_buffer(); }

    /// Return a pointer to a controller port
    ///
    /// @param port the port of the controller to return the pointer to
    /// @return a pointer to the byte buffer for the controller state
    ///
    inline NES_Byte* get_controller(int port) {
        return controllers[port].get_joypad_buffer();
    }

    /// Load the ROM into the NES.
    inline void reset() { cpu.reset(bus); ppu.reset(); }

    /// Perform a step on the emulator, i.e., a single frame.
    void step();

    /// Add a Game Genie cheat. compare = -1 means "no compare" (6-letter).
    /// Returns the index of the added cheat.
    inline int add_cheat(NES_Address addr, NES_Byte value, int16_t compare) {
        return cheats->add(addr, value, compare);
    }

    /// Remove the first matching cheat. Returns 1 if removed, 0 otherwise.
    inline int remove_cheat(NES_Address addr, NES_Byte value, int16_t compare) {
        return cheats->remove(addr, value, compare) ? 1 : 0;
    }

    /// Clear all cheats.
    inline void clear_cheats() { cheats->clear(); }

    /// Number of active cheats.
    inline int cheat_count() const {
        return static_cast<int>(cheats->entries.size());
    }

    /// Create a backup state on the emulator.
    inline void backup() {
        backup_bus = bus;
        backup_picture_bus = picture_bus;
        backup_cpu = cpu;
        backup_ppu = ppu;
        // bus's shared_ptr was copied verbatim; both buses still point at
        // the same CheatTable instance we own via `cheats`. Reassert to be
        // defensive against future copy-assignment changes.
        bus.set_cheat_table(cheats);
        backup_bus.set_cheat_table(cheats);
    }

    /// Restore the backup state on the emulator.
    inline void restore() {
        bus = backup_bus;
        picture_bus = backup_picture_bus;
        cpu = backup_cpu;
        ppu = backup_ppu;
        bus.set_cheat_table(cheats);
        backup_bus.set_cheat_table(cheats);
    }
};

}  // namespace NES

#endif  // EMULATOR_HPP
