#ifndef __RAD_INCLUDE_IGGYPERFMON_ORBIS_H__
#define __RAD_INCLUDE_IGGYPERFMON_ORBIS_H__

// You still need to include regular iggyperfmon.h first. This is just for convenience.

#define IggyPerfmonPadFromScePadData(pad, paddata) \
   (pad).bits = 0,                                                                     \
   (pad).field.dpad_up           = 0 != ((paddata).buttons & SCE_PAD_BUTTON_UP),       \
   (pad).field.dpad_down         = 0 != ((paddata).buttons & SCE_PAD_BUTTON_DOWN),     \
   (pad).field.dpad_left         = 0 != ((paddata).buttons & SCE_PAD_BUTTON_LEFT),     \
   (pad).field.dpad_right        = 0 != ((paddata).buttons & SCE_PAD_BUTTON_RIGHT),    \
   (pad).field.button_up         = 0 != ((paddata).buttons & SCE_PAD_BUTTON_TRIANGLE), \
   (pad).field.button_down       = 0 != ((paddata).buttons & SCE_PAD_BUTTON_CROSS),    \
   (pad).field.button_left       = 0 != ((paddata).buttons & SCE_PAD_BUTTON_SQUARE),   \
   (pad).field.button_right      = 0 != ((paddata).buttons & SCE_PAD_BUTTON_CIRCLE),   \
   (pad).field.shoulder_left_hi  = 0 != ((paddata).buttons & SCE_PAD_BUTTON_L1),       \
   (pad).field.shoulder_right_hi = 0 != ((paddata).buttons & SCE_PAD_BUTTON_R1),       \
   (pad).field.trigger_left_low  = 0 != ((paddata).buttons & SCE_PAD_BUTTON_L2),       \
   (pad).field.trigger_right_low = 0 != ((paddata).buttons & SCE_PAD_BUTTON_R2)

#endif//__RAD_INCLUDE_IGGYPERFMON_ORBIS_H__
