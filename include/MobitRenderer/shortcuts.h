#pragma once

#include <raylib.h>

namespace mr {

struct keyboard_shortcut {
  KeyboardKey key;
  bool ctrl, shift, alt, holding;

  bool check(bool ctrl, bool shift, bool alt,
             bool holding = false) const noexcept;

  keyboard_shortcut();
  explicit keyboard_shortcut(KeyboardKey key, bool ctrl = false,
                             bool shift = false, bool alt = false,
                             bool holding = false);
};

}; // namespace mr
