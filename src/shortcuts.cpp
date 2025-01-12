#if defined(_WIN32) || defined(_WIN64)
  #define WIN32_LEAN_AND_MEAN
#endif

#include <raylib.h>

#include <MobitRenderer/shortcuts.h>

namespace mr {

bool keyboard_shortcut::check(bool ctrl, bool shift, bool alt,
                              bool holding) const noexcept {
  return this->holding == holding && this->ctrl == ctrl &&
         this->shift == shift && this->alt == alt;
}

keyboard_shortcut::keyboard_shortcut()
    : key(KEY_NULL), ctrl(false), shift(false), alt(false), holding(false) {}
keyboard_shortcut::keyboard_shortcut(KeyboardKey key, bool ctrl, bool shift,
                                     bool alt, bool holding)
    : key(key), ctrl(ctrl), shift(shift), alt(alt), holding(holding) {}

// Page shortcuts

struct global_shortcuts {};
struct geo_shortcuts {};
struct tile_shortcuts {};
struct camera_shortcuts {};
struct light_shortcuts {};
struct effect_shortcuts {};
struct prop_shortcuts {};

}; // namespace mr
