#ifndef __RAD_INCLUDE_IGGYPERFMON_PS3_H__
#define __RAD_INCLUDE_IGGYPERFMON_PS3_H__

// You still need to include regular iggyperfmon.h first. This is just for convenience.

#define IggyPerfmonPadFromCellPadData(pad, paddata) \
   (pad).bits = 0,                                                               \
   (pad).field.dpad_up           = 0 != ((paddata).button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_UP),       \
   (pad).field.dpad_down         = 0 != ((paddata).button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_DOWN),     \
   (pad).field.dpad_left         = 0 != ((paddata).button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_LEFT),     \
   (pad).field.dpad_right        = 0 != ((paddata).button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_RIGHT),    \
   (pad).field.button_up         = 0 != ((paddata).button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_TRIANGLE), \
   (pad).field.button_down       = 0 != ((paddata).button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_CROSS),    \
   (pad).field.button_left       = 0 != ((paddata).button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_SQUARE),   \
   (pad).field.button_right      = 0 != ((paddata).button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_CIRCLE),   \
   (pad).field.shoulder_left_hi  = 0 != ((paddata).button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_L1),       \
   (pad).field.shoulder_right_hi = 0 != ((paddata).button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_R1),       \
   (pad).field.trigger_left_low  = 0 != ((paddata).button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_L2),       \
   (pad).field.trigger_right_low = 0 != ((paddata).button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_R2)

#endif//__RAD_INCLUDE_IGGYPERFMON_PS3_H__
