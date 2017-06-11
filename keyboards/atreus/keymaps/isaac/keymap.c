// this is the style you want to emulate.
// This is the canonical layout file for the Quantum project. If you want to add another keyboard,

#include "atreus.h"
#include "keymap_nordic.h"

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.
#define _QW 0
#define _RS 1
#define _LW 2

#define M_PRN  0
#define M_CBR  1
#define M_QUOT 2
#define M_SECT 3
#define M_PIPE 4
#define M_AND  5
#define M_SLSH 6
#define M_HAT  7
#define M_PLUS 8
#define M_ASTR 9

// KC_SCLN ö
// KC_QUOT ä

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[_QW] = { /* Qwerty */
  {KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,   KC_TRNS,  KC_Y,    KC_U,    KC_I,      KC_O,      KC_P     },
  {KC_A,    KC_S,    KC_D,    KC_F,    KC_G,   KC_TRNS,  KC_H,    KC_J,    KC_K,      KC_L,      M(M_PRN) },
  {KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,   KC_LCTL,  KC_N,    KC_M,    KC_COMM,   KC_DOT,    M(M_CBR) },
  {KC_ESC, KC_TAB, KC_LGUI,  KC_LSFT, KC_BSPC,  KC_LALT, KC_SPC,  MO(_RS), M(M_SECT), M(M_QUOT), KC_ENT   }
},
/*
 *  !       @     up     {    }        ||     pgup    7     8     9    *
 *  #     left   down  right  $        ||     pgdn    4     5     6    +
 *  [       ]      (     )    &        ||       `     1     2     3    \
 * lower  insert super shift bksp ctrl || alt space   fn    .     0    =
 */
[_RS] = { /* [> RAISE <] */
  {M(M_PIPE), NO_AT,      KC_UP,   KC_HASH, M(M_AND),      KC_TRNS, KC_PGUP, KC_7,    KC_8,   KC_9, M(M_ASTR) },
  {NO_HALF,   KC_LEFT,    KC_DOWN, KC_RGHT, M(M_SLSH),     KC_TRNS, KC_PGDN, KC_4,    KC_5,   KC_6, M(M_PLUS) },
  {M(M_HAT),  LSFT(KC_5), KC_SCLN, KC_QUOT, LSFT(NO_MINS), KC_LCTL, KC_NO,   KC_1,    KC_2,   KC_3, NO_EQL    },
  {TG(_LW),   KC_INS,     KC_LGUI, KC_LSFT, KC_BSPC,       KC_LALT, KC_SPC,  KC_TRNS, KC_DOT, KC_0, KC_ENT    }
},
/*
 * insert home   up  end   pgup       ||      up     F7    F8    F9   F10
 *  del   left  down right pgdn       ||     down    F4    F5    F6   F11
 *       volup    ö    ä   reset      ||             F1    F2    F3   F12
 *       voldn  super shift bksp ctrl || alt space   L0  prtsc pause
 */
[_LW] = { /* [> LOWER <] */
  {KC_INS,  KC_HOME, KC_UP,   KC_END,  KC_PGUP, KC_TRNS, KC_UP,   KC_F7,   KC_F8,   KC_F9,   KC_F10},
  {KC_DELT, KC_LEFT, KC_DOWN, KC_RGHT, KC_DOWN, KC_TRNS, KC_DOWN, KC_F4,   KC_F5,   KC_F6,   KC_F11},
  {KC_TRNS, KC_VOLU, KC_SCLN, KC_QUOT, RESET,   KC_LCTL, KC_TRNS, KC_F1,   KC_F2,   KC_F3,   KC_F12},
  {KC_TRNS, KC_VOLD, KC_LGUI, KC_LSFT, KC_BSPC, KC_LALT, KC_SPC,  DF(_QW), KC_PSCR, KC_SLCK, KC_PAUS}
}};

const uint16_t PROGMEM fn_actions[] = {

};

// https://github.com/qmk/qmk_firmware/issues/401
void press_key_with_level_mods(uint16_t key) {
    const uint8_t interesting_mods = MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT) | MOD_BIT(KC_RALT);

    // Save the state
    const uint8_t real_mods = get_mods();
    const uint8_t weak_mods = get_weak_mods();
    const uint8_t macro_mods = get_macro_mods();

    uint8_t target_mods = (key >> 8) & (QK_MODS_MAX >> 8);
    // The 5th bit indicates that it's a right hand mod,
    // which needs some fixup
    if (target_mods & 0x10) {
        target_mods &= 0xF;
        target_mods <<= 4;
    }

    // Clear the mods that we are potentially going to modify,
    del_mods(interesting_mods);
    del_weak_mods(interesting_mods);
    del_macro_mods(interesting_mods);

    // Enable the mods that we need
    add_mods(target_mods & interesting_mods);

    // Press and release the key
    register_code(key & 0xFF);
    unregister_code(key & 0xFF);

    // Restore the previous state
    set_mods(real_mods);
    set_weak_mods(weak_mods);
    set_macro_mods(macro_mods);
    send_keyboard_report();
}

void override_key(keyrecord_t* record, uint16_t normal, uint16_t shifted) {
    const uint8_t shift = MOD_BIT(KC_LSFT) | MOD_BIT(KC_RSFT);
    if (record->event.pressed) {
        bool shift_pressed = keyboard_report->mods & shift;
        const uint16_t target = shift_pressed ? shifted : normal;
        uint8_t keycode = target & 0xFF;
        if (keycode == KC_NO) {
            return;
        }
        press_key_with_level_mods(target);
    }
}

const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt) {
    switch(id) {
        case M_PRN:
            override_key(record, NO_LPRN, NO_RPRN);
            break;
        case M_CBR:
            //override_key(record, NO_LCBR, NO_RCBR);
            override_key(record, LSFT(ALGR(KC_8)), LSFT(ALGR(KC_9)));
            break;
        case M_QUOT:
            override_key(record, NO_APOS, NO_QUO2);
            break;
        case M_SECT:
            override_key(record, KC_EXLM, NO_QUES);
            break;
        case M_PIPE:
            override_key(record, ALGR(KC_7), ALGR(KC_7));
            break;
        case M_AND:
            override_key(record, NO_AMPR, ALGR(KC_4));
            break;
        case M_SLSH:
            override_key(record, NO_SLSH, LSFT(ALGR(KC_7)));
            break;
        case M_HAT:
            override_key(record, LSFT(NO_QUOT), ALGR(NO_QUOT));
            break;
        case M_PLUS:
            override_key(record, NO_PLUS, NO_MINS);
            break;
        case M_ASTR:
            override_key(record, LSFT(KC_BSLS), NO_SLSH);
            break;
    }

    return MACRO_NONE;
}
