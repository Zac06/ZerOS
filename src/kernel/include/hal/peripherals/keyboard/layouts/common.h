#pragma once

#include <stdint.h>

/**
 * Structure of keyboard layout char lookup table:
 * 
 * 
| Keycodes/modifiers | ...  | KC_Q | KC_W | KC_E | ...  |
| ------------------ | ---- | ---- | ---- | ---  | ---  |
| KC_INVALID         |      | 'q'  | 'w'  | 'e'  | ...  |
| KC_LCTRL           |      |      |      |      | ...  |
| KC_LSHIFT          |      | 'Q'  | 'W'  | 'E'  | ...  |
| KC_RSHIFT          |      | 'Q'  | 'W'  | 'E'  | ...  |
| KC_LALT            |      |      |      |      | ...  |
| KC_CAPSLOCK        |      | 'Q'  | 'W'  | 'E'  | ...  |
| KC_NUMLOCK         |      |      |      |      | ...  |
| KC_SCROLLLOCK      |      |      |      |      | ...  |
| KC_RCTRL           |      |      |      |      | ...  |
| KC_ALTGR           |      |      |      |      | ...  |
| KC_LAST_MOD        |      |      |      |      | ...  |

 */
typedef const uint8_t* keyboard_layout_map[];