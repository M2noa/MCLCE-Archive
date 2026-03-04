#ifndef __RAD_INCLUDE_IGGYPERFMON_PSP2_H__
#define __RAD_INCLUDE_IGGYPERFMON_PSP2_H__

// You still need to include regular iggyperfmon.h first. This is just for convenience.

#define IggyPerfmonDPadWithShift(ctrldata, testfor) ((testfor) == ((ctrldata).buttons & ((testfor) | SCE_CTRL_L | SCE_CTRL_R)))

// From SceCtrlData (built-in controller)
#define IggyPerfmonPadFromSceCtrlData(pad, ctrldata) \
   (pad).bits = 0,                                                                               \
   (pad).field.dpad_up           = IggyPerfmonDPadWithShift(ctrldata, SCE_CTRL_UP),              \
   (pad).field.dpad_down         = IggyPerfmonDPadWithShift(ctrldata, SCE_CTRL_DOWN),            \
   (pad).field.dpad_left         = IggyPerfmonDPadWithShift(ctrldata, SCE_CTRL_LEFT),            \
   (pad).field.dpad_right        = IggyPerfmonDPadWithShift(ctrldata, SCE_CTRL_RIGHT),           \
   (pad).field.button_up         = 0 != ((ctrldata).buttons & SCE_CTRL_TRIANGLE),                \
   (pad).field.button_down       = 0 != ((ctrldata).buttons & SCE_CTRL_CROSS),                   \
   (pad).field.button_left       = 0 != ((ctrldata).buttons & SCE_CTRL_SQUARE),                  \
   (pad).field.button_right      = 0 != ((ctrldata).buttons & SCE_CTRL_CIRCLE),                  \
   (pad).field.shoulder_left_hi  = IggyPerfmonDPadWithShift(ctrldata, SCE_CTRL_LEFT|SCE_CTRL_L), \
   (pad).field.shoulder_right_hi = IggyPerfmonDPadWithShift(ctrldata, SCE_CTRL_RIGHT|SCE_CTRL_L),\
   (pad).field.trigger_left_low  = IggyPerfmonDPadWithShift(ctrldata, SCE_CTRL_LEFT|SCE_CTRL_R), \
   (pad).field.trigger_right_low = IggyPerfmonDPadWithShift(ctrldata, SCE_CTRL_RIGHT|SCE_CTRL_R)

// From SceCtrlData2 (wireless controller)
#define IggyPerfmonPadFromSceCtrlData2(pad, ctrldata) \
   (pad).bits = 0,                                                                \
   (pad).field.dpad_up           = 0 != ((ctrldata).buttons & SCE_CTRL_UP),       \
   (pad).field.dpad_down         = 0 != ((ctrldata).buttons & SCE_CTRL_DOWN),     \
   (pad).field.dpad_left         = 0 != ((ctrldata).buttons & SCE_CTRL_LEFT),     \
   (pad).field.dpad_right        = 0 != ((ctrldata).buttons & SCE_CTRL_RIGHT),    \
   (pad).field.button_up         = 0 != ((ctrldata).buttons & SCE_CTRL_TRIANGLE), \
   (pad).field.button_down       = 0 != ((ctrldata).buttons & SCE_CTRL_CROSS),    \
   (pad).field.button_left       = 0 != ((ctrldata).buttons & SCE_CTRL_SQUARE),   \
   (pad).field.button_right      = 0 != ((ctrldata).buttons & SCE_CTRL_CIRCLE),   \
   (pad).field.shoulder_left_hi  = 0 != ((ctrldata).buttons & SCE_CTRL_L1),       \
   (pad).field.shoulder_right_hi = 0 != ((ctrldata).buttons & SCE_CTRL_R1),       \
   (pad).field.trigger_left_low  = 0 != ((ctrldata).buttons & SCE_CTRL_L2),       \
   (pad).field.trigger_right_low = 0 != ((ctrldata).buttons & SCE_CTRL_R2)

#endif//__RAD_INCLUDE_IGGYPERFMON_PSP2_H__
