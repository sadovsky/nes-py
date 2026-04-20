//  Program:      nes-py (fork with Game Genie cheat support)
//  File:         cheat.hpp
//  Description:  Game Genie / cheat engine for intercepting CPU ROM reads.
//
//  Cheats are (address, value, compare) triples. When the CPU reads from
//  `address` (which must be in $8000-$FFFF for Game Genie semantics), if
//  `compare` is -1 the read is replaced with `value` unconditionally; if
//  `compare` is a byte, the read returns `value` only when the underlying
//  ROM byte at that address equals `compare`.
//
//  The table is shared across backup/restore via shared_ptr so that adding
//  or removing a cheat affects both live and backed-up state simultaneously,
//  which matches user expectation ("a cheat is a property of the session,
//  not of the game state").

#ifndef CHEAT_HPP
#define CHEAT_HPP

#include <cstdint>
#include <vector>
#include "common.hpp"

namespace NES {

struct CheatEntry {
    NES_Address address;  // $8000-$FFFF
    NES_Byte value;       // replacement byte
    int16_t compare;      // -1 = no compare (6-letter); else 0..255
};

/// A flat, linearly-scanned cheat table. Optimized for the common case
/// where there are zero or a small number of active cheats.
class CheatTable {
 public:
    std::vector<CheatEntry> entries;

    inline bool empty() const { return entries.empty(); }
    inline void clear() { entries.clear(); }

    /// Add a cheat. Returns the index of the newly added cheat.
    inline int add(NES_Address address, NES_Byte value, int16_t compare) {
        entries.push_back({address, value, compare});
        return static_cast<int>(entries.size() - 1);
    }

    /// Remove the first cheat matching (address, value, compare). Returns
    /// true if a cheat was removed.
    inline bool remove(NES_Address address, NES_Byte value, int16_t compare) {
        for (auto it = entries.begin(); it != entries.end(); ++it) {
            if (it->address == address && it->value == value
                && it->compare == compare) {
                entries.erase(it);
                return true;
            }
        }
        return false;
    }

    /// Look up a cheat for the given address, consulting `original_byte`
    /// for compare semantics. Returns true and writes into `out_value` if
    /// a matching cheat fires; else returns false.
    ///
    /// Matches first-wins, consistent with real Game Genie hardware.
    inline bool lookup(NES_Address address, NES_Byte original_byte,
                       NES_Byte& out_value) const {
        for (const auto& e : entries) {
            if (e.address != address) continue;
            if (e.compare < 0 || e.compare == original_byte) {
                out_value = e.value;
                return true;
            }
        }
        return false;
    }
};

}  // namespace NES

#endif  // CHEAT_HPP
