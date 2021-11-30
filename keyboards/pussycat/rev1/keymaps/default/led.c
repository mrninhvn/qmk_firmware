/*
Copyright 2021 mrninhvn (https://github.com/mrninhvn)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include <stdio.h>
#include "rgblight_list.h"
#include "rgblight.h"


// caps led on
const rgblight_segment_t PROGMEM capslock_layer_on[] = RGBLIGHT_LAYER_SEGMENTS(
    {15, 1, 128, 255, 100}  // Cyan
);

// caps led off
const rgblight_segment_t PROGMEM capslock_layer_off[] = RGBLIGHT_LAYER_SEGMENTS(
    {15, 1, HSV_BLACK}
);

// rgb light layers
const rgblight_segment_t* const PROGMEM rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    capslock_layer_on,
    capslock_layer_off
);

void keyboard_post_init_user(void) {
  rgblight_set_effect_range(0, 14);
  // Enable the LED layers
  rgblight_layers = rgb_layers;
}

bool led_update_user(led_t led_state) {
    rgblight_set_layer_state(0, led_state.caps_lock);
    rgblight_set_layer_state(1, !led_state.caps_lock);
    return true;
}