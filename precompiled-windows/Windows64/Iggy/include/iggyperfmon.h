// $$COPYRIGHT$$

#ifndef __RAD_INCLUDE_IGGYPERFMON_H__
#define __RAD_INCLUDE_IGGYPERFMON_H__

#include "rrCore.h"

#define IDOC

RADDEFSTART

#ifndef __RAD_HIGGYPERFMON_
#define __RAD_HIGGYPERFMON_
typedef void * HIGGYPERFMON;
#endif

//idoc(parent,IggyPerfmon_API)

typedef void * RADLINK iggyperfmon_malloc(void *handle, U32 size);
typedef void RADLINK iggyperfmon_free(void *handle, void *ptr);

IDOC RADEXPFUNC HIGGYPERFMON RADEXPLINK IggyPerfmonCreate(iggyperfmon_malloc *perf_malloc, iggyperfmon_free *perf_free, void *callback_handle);
/* Creates an IggyPerfmon.

You must supply allocator functions. The amount allocated depends on the complexity
of the Iggys being profiled. */

typedef struct Iggy Iggy;
typedef struct GDrawFunctions GDrawFunctions;

IDOC typedef union {
   U32 bits;
   struct {
      U32 dpad_up             :1;
      U32 dpad_down           :1;
      U32 dpad_left           :1;
      U32 dpad_right          :1;
      U32 button_up           :1;      // XBox Y, PS3 tri
      U32 button_down         :1;      // XBox A, PS3 X
      U32 button_left         :1;      // XBox X, PS3 square
      U32 button_right        :1;      // XBox B, PS3 circle
      U32 shoulder_left_hi    :1;      // LB/L1
      U32 shoulder_right_hi   :1;      // RB/R1
      U32 trigger_left_low    :1;
      U32 trigger_right_low   :1;
   } field;
} IggyPerfmonPad;

#define IggyPerfmonPadFromXInputStatePointer(pad, xis)                                       \
   (pad).bits = 0,                                                                           \
   (pad).field.dpad_up           = 0 != ((xis)->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP),        \
   (pad).field.dpad_down         = 0 != ((xis)->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN),      \
   (pad).field.dpad_left         = 0 != ((xis)->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT),      \
   (pad).field.dpad_right        = 0 != ((xis)->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT),     \
   (pad).field.button_up         = 0 != ((xis)->Gamepad.wButtons & XINPUT_GAMEPAD_Y),              \
   (pad).field.button_down       = 0 != ((xis)->Gamepad.wButtons & XINPUT_GAMEPAD_A),              \
   (pad).field.button_left       = 0 != ((xis)->Gamepad.wButtons & XINPUT_GAMEPAD_X),              \
   (pad).field.button_right      = 0 != ((xis)->Gamepad.wButtons & XINPUT_GAMEPAD_B),              \
   (pad).field.shoulder_left_hi  = 0 != ((xis)->Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER),  \
   (pad).field.shoulder_right_hi = 0 != ((xis)->Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER), \
   (pad).field.trigger_left_low  = 0 != ((xis)->Gamepad.bLeftTrigger  >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD), \
   (pad).field.trigger_right_low = 0 != ((xis)->Gamepad.bRightTrigger >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD)

// All positions in window coords
IDOC RADEXPFUNC void RADEXPLINK IggyPerfmonTickAndDraw(HIGGYPERFMON p, GDrawFunctions* gdraw_funcs,
                    const IggyPerfmonPad* pad,
                    int pm_tile_ul_x, int pm_tile_ul_y, int pm_tile_lr_x, int pm_tile_lr_y);
/* Draw and tick an IggyPerfmon.

$:p A perfmon context previously created with IggyPerfmonCreate
$:gdraw_functions The same GDraw handle used for rendering Iggy
$:pad An abstracted gamepad state structure. iggyperfmon.h
includes an example that initializes the abstract gamepad from a 360 controller
as defined by XInput; this will work on both Windows and the Xbox 360.
$:pm_tile_ul_x The left coordinate of the rectangle where the perfmon display should be drawn
$:pm_tile_ul_y The top coordinate of the rectangle where the perfmon display should be drawn
$:pm_tile_lr_x The right coordinate of the rectangle where the perfmon display should be drawn
$:pm_tile_lr_y The bottom coordinate of the rectangle where the perfmon display should be drawn

You should only call this function when you want Iggy Perfmon to be visible.
See $IggyPerfmon for more information. */

IDOC RADEXPFUNC void  RADEXPLINK IggyPerfmonDestroy(HIGGYPERFMON p, GDrawFunctions* iggy_draw);
/* Closes and destroys an IggyPerfmon */


RADDEFEND

#endif//__RAD_INCLUDE_IGGYPERFMON_H__