#ifndef IMSSAPI_H
#define IMSSAPI_H

// Internal API file

#if defined(IS_PS3) || defined(IS_PSP)
#include <string.h>
#endif

#if defined(IS_WII)
#include <string.h>
#include <math.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(IS_WIN32API) || defined(IS_WII)
  #pragma pack(push, 1)
#endif
#ifdef IS_32
  #if !defined(IS_WIN32API)
    #define ASMLINK
    #define ASMPARM
  #else
     #if defined(IS_XENON)
       #define ASMLINK __cdecl
       #define ASMPARM register
     #else
       #define ASMLINK __cdecl
       #define ASMPARM
     #endif
  #endif
#else
  #define ASMLINK WINAPI
  #define ASMPARM
#endif

#ifndef YES
#define YES 1
#endif

#ifndef NULL
#define NULL 0
#endif

#define MSSHIWORD(ptr) (((U32)ptr)>>16)
#define MSSLOWORD(ptr) ((U16)((U32)ptr))

#ifndef NO
#define NO  0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE  0
#endif

#ifdef IS_MAC

#if !defined(max)
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif
#if !defined(min)
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#endif

#ifdef IS_WATCOM

#if !defined(max) // Watcom stdlib.h doesn't define these for C++
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif
#if !defined(min)
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#endif

#ifdef IS_WIN64

  #define PTRFMT "0x%I64X"
  #define SINTFMT "%I64d"
  #define HPFMT "%ld"

#else

  #if defined(IS_PS3) || defined(IS_PSP) || defined(IS_3DS)

    #define PTRFMT "0x%lX"
    #define SINTFMT "%d"      // (GCC warns when ints printed with %ld)
    #define HPFMT "%ld"

  #else

    #define PTRFMT "0x%lX"
    #define SINTFMT "%ld"
    #define HPFMT "%ld"

  #endif

#endif

U32 AILCALL XMI_read_VLN(U8 const* *ptr);
S32 AILCALL XMI_message_size(S32 status);
void const * AILCALL XMI_find_sequence(U8 const *image, S32 sequence);
S32 AIL_process_WAV_image( AILSOUNDINFO const * info, HSAMPLE sample );

#ifdef IS_SPU
#define NTELEMETRY
#endif

#include "tmapi.h"

#ifdef NTELEMETRY
  #define miles_context 0
#else
  extern HTELEMETRY g_Miles_Tm;
  extern S32 g_tm_log;
  #define miles_context g_Miles_Tm
#endif

//############################################################################
//##                                                                        ##
//## API function prototypes                                                ##
//##                                                                        ##
//############################################################################

#define check_hardware_buffer( S, flags )
#define hardware_stop( S )
#define set_hardware_position( S )
#define hardware_spin_until_stopped( S )
#define set_hardware_volume( S )
#define set_master_hardware_volume( dig )
#define set_hardware_loop_region( S )
#define set_hardware_low_pass( S )

extern void AILCALL InMilesMutex(void);
extern void AILCALL OutMilesMutex(void);

#ifdef IS_WIN32API

#ifdef NTAPI
extern HWND AILCALL AIL_API_HWND                  (void);

void AILEXPORT API_timer (UINT  IDEvent,
                          UINT  uReserved,
                          DWORD dwUser,
                          DWORD dwReserved1,
                          DWORD dwReserved2);

#endif

extern S32    MilesCount;
extern U32 lastapitimerms;

  void AILCALL Set_thread_name( U32 thread_id, char const * thread_name );

#endif


#ifdef IS_MAC
  #if defined(__CARBON__)
     ProcessSerialNumber AILCALL AIL_API_Process(void);
  #endif
#endif


#ifdef _DEBUG

DXDEC void AILEXPORT AIL_debug_printf( char const *fmt, ...);

#else

#define AIL_debug_printf(...)

#endif

#ifndef MSS_SPU_PROCESS

//
// Global preference array
//
extern SINTa          AIL_preference   [N_PREFS];

#endif

extern char AIL_redist_directory[260];

extern S32 AILCALL match_suffix(char const* suffix,char const* file_suffix);

#if !defined(IS_X86) || !defined(IS_MAC) // use mix rib on x86 mac
DXDEC S32 AILCALL MIX_RIB_MAIN( HPROVIDER provider_handle, U32 up_down, RIB_ALLOC_PROVIDER_HANDLE_TYPE * rib_alloc, RIB_REGISTER_INTERFACE_TYPE * rib_reg, RIB_UNREGISTER_INTERFACE_TYPE * rib_unreg );
#endif

//
// High-level support services
//

extern void AILCALL AIL_API_startup               (void);
extern void AILCALL AIL_API_shutdown              (void);

extern SINTa AILCALL AIL_API_set_preference        (U32         number,
                                                    SINTa       value);

//
// Low-level support services
//

extern void AILCALL AIL_API_sleep                 (U32 ms);

extern S32  AILCALL AIL_API_background            (void);

extern void AILCALL AIL_API_mem_free_lock         (void   *ptr);

extern void* AILCALL AIL_API_mem_alloc_lock   (UINTa  size);

//
// Process services
//

extern HTIMER AILCALL AIL_API_register_timer      (AILTIMERCB  fn);

extern UINTa  AILCALL AIL_API_set_timer_user      (HTIMER      timer,
                                                   UINTa       user);

extern void   AILCALL AIL_API_set_timer_period    (HTIMER      timer,
                                                   U32         microseconds);

extern void   AILCALL AIL_API_set_timer_frequency (HTIMER      timer,
                                                   U32         hertz) ;

extern void   AILCALL AIL_API_set_timer_divisor   (HTIMER      timer,
                                                   U32         PIT_divisor);

extern void   AILCALL AIL_API_start_timer         (HTIMER      timer) ;
extern void   AILCALL AIL_API_start_all_timers    (void);

extern void   AILCALL AIL_API_stop_timer          (HTIMER      timer);
extern void   AILCALL AIL_API_stop_all_timers     (void);

extern void   AILCALL AIL_API_release_timer_handle(HTIMER      timer);
extern void   AILCALL AIL_API_release_all_timers  (void);

extern void        Container_FreeAll();
extern void*       Container_GetPreset(char const* i_PresetName);
extern void*       Container_GetEnvironment(char const* i_EnvironmentName);
extern S32         Container_GetSound(char const* i_SoundName, char* o_SoundFileName, MILESBANKSOUNDINFO* o_SoundInfo);
extern void*       Container_GetEvent(char const* i_EventName);
extern void*       Container_GetMarkerList(char const* i_SoundName);
extern S32         Container_GetLoadedCount();
extern S32         Container_GetMemUsage();
extern char const* Container_GetLoadedName(S32 i_Index);

extern const U8* AILCALL    MilesFindEventInternal(HMSOUNDBANK i_Bank, const char* i_EventName);
extern void AILCALL         MilesClearEventQueueInternal();
extern void AILCALL         MilesRegisterRandInternal(MSS_USER_RAND * rand);
extern U64 AILCALL          MilesEnqueueEventInternal(const U8* i_Event, void* i_UserBuffer, S32 i_UserBufferLen, S32 i_EnqueueFlags, U64 i_EventFilter);
extern U64 AILCALL          MilesEnqueueEventByNameInternal(char const* i_Name);
extern U64 AILCALL          MilesEnqueueEventContextInternal(HEVENTSYSTEM i_Context, const U8* i_Event, void* i_UserBuffer, S32 i_UserBufferLen, S32 i_EnqueueFlags, U64 i_EventFilter);

extern S32 AILCALL          MilesBeginEventQueueProcessingInternal();
extern S32 AILCALL          MilesCompleteEventQueueProcessingInternal();

extern S32 AILCALL          MilesEnumerateSoundInstancesInternal(HEVENTSYSTEM i_System, HMSSENUM* io_Next, S32 i_Status, const char* i_Labels, U64 search_for_ID, MILESEVENTSOUNDINFO* o_Info);
extern S32 AILCALL          MilesEnumeratePresetPersistsInternal(HEVENTSYSTEM i_System, HMSSENUM* io_Next, const char** o_Name);
extern HEVENTSYSTEM AILCALL MilesStartupEventSystemInternal(HDIGDRIVER i_Driver, S32 i_CommandBufferSize, char* i_Memory, S32 i_MemoryLen);
extern HEVENTSYSTEM AILCALL MilesAddEventSystemInternal(HDIGDRIVER i_Driver);
extern S32 AILCALL          MilesSetSoundLabelLimitsInternal(HEVENTSYSTEM i_System, U64 i_Id, const char* i_SoundLimits, char const* i_Name);


extern void AILCALL         MilesShutdownEventSystemInternal();
extern HMSOUNDBANK AILCALL  MilesAddSoundBankInternal(const char* i_FileName, char const* i_Name);
extern S32 AILCALL          MilesReleaseSoundBankInternal(HMSOUNDBANK i_Bank);
extern char* AILCALL        MilesTextDumpEventSystemInternal();
extern void AILCALL         MilesSetEventErrorCallbackInternal(AILEVENTERRORCB i_Callback);

extern void AILCALL         MilesSetVarIInternal(UINTa i_System, char const* i_Name, S32 i_Value);
extern void AILCALL         MilesSetVarFInternal(UINTa i_System, char const* i_Name, F32 i_Value);
extern S32 AILCALL          MilesGetVarFInternal(UINTa i_Context, char const* i_Name, F32* o_Value);
extern S32 AILCALL          MilesGetVarIInternal(UINTa i_Context, char const* i_Name, S32* o_Value);
extern void AILCALL         MilesSetSoundStartOffsetInternal(HMSSENUM i_Instance, S32 i_Offset, S32 i_IsMs);
extern void AILCALL         MilesRequeueAsyncsInternal();

extern AIL_file_open_callback MSS_open;
extern AIL_file_close_callback MSS_close;
extern AIL_file_seek_callback MSS_seek;
extern AIL_file_read_callback MSS_read;

extern S32 disk_err;
extern char MSS_Directory[260];


extern void stream_background(void); // background service for streaming

#define call_fetch_CB( ASI,usr,dest,bytes,offset ) ((AILASIFETCHCB)((ASI)->fetch_CB))(usr,dest,bytes,offset)

extern HPROVIDER find_ASI_provider(const C8 *attrib, const C8 *filename);


//
// M3D services
//

extern HDIGDRIVER AILCALL AIL_API_primary_digital_driver  (HDIGDRIVER new_primary);

extern void       AILCALL AIL_API_set_sample_3D_distances (HSAMPLE   samp,
                                                           F32       max_dist,
                                                           F32       min_dist,
                                                           S32       auto_3D_wet_atten);

extern void       AILCALL AIL_API_sample_3D_distances     (HSAMPLE   samp,
                                                           F32 * max_dist,
                                                           F32 * min_dist,
                                                           S32 * auto_3D_wet_atten);

extern void       AILCALL AIL_API_set_sample_obstruction (HSAMPLE S,
                                                             F32     obstruction);

extern void       AILCALL AIL_API_set_sample_exclusion   (HSAMPLE S,
                                                             F32     exclusion);

extern void       AILCALL AIL_API_set_sample_occlusion   (HSAMPLE S,
                                                             F32     occlusion);

extern void       AILCALL AIL_API_set_sample_3D_cone        (HSAMPLE S,
                                                             F32     inner_angle,
                                                             F32     outer_angle,
                                                             F32     outer_volume);

extern F32        AILCALL AIL_API_sample_obstruction (HSAMPLE S);

extern F32        AILCALL AIL_API_sample_occlusion   (HSAMPLE S);

extern F32        AILCALL AIL_API_sample_exclusion   (HSAMPLE S);

extern void       AILCALL AIL_API_sample_3D_cone        (HSAMPLE  S,
                                                         F32 *inner_angle,
                                                         F32 *outer_angle,
                                                         F32 *outer_volume);

extern  S32      AILCALL AIL_API_room_type                (HDIGDRIVER dig,
                                                           S32  bus_index);

extern  void     AILCALL AIL_API_set_room_type            (HDIGDRIVER dig,
                                                           S32        bus_index,
                                                           S32        EAX_room_type);

extern  F32      AILCALL AIL_API_3D_rolloff_factor        (HDIGDRIVER dig);

extern  void     AILCALL AIL_API_set_3D_rolloff_factor    (HDIGDRIVER dig,
                                                           F32       factor );

extern  F32      AILCALL AIL_API_3D_doppler_factor        (HDIGDRIVER dig);

extern  void     AILCALL AIL_API_set_3D_doppler_factor    (HDIGDRIVER dig,
                                                           F32        factor );

extern  F32      AILCALL AIL_API_3D_distance_factor       (HDIGDRIVER dig);

extern  void     AILCALL AIL_API_set_3D_distance_factor   (HDIGDRIVER dig,
                                                           F32        factor );

extern void       AILCALL AIL_API_set_sample_3D_position         (HSAMPLE S,
                                                                  F32     X,
                                                                  F32     Y,
                                                                  F32     Z);

extern void       AILCALL AIL_API_set_sample_3D_velocity         (HSAMPLE S,
                                                                  F32     dX_per_ms,
                                                                  F32     dY_per_ms,
                                                                  F32     dZ_per_ms,
                                                                  F32     magnitude);

extern void       AILCALL AIL_API_set_sample_3D_velocity_vector  (HSAMPLE S,
                                                                  F32     dX_per_ms,
                                                                  F32     dY_per_ms,
                                                                  F32     dZ_per_ms);

extern void       AILCALL AIL_API_set_sample_3D_orientation      (HSAMPLE S,
                                                                  F32     X_face,
                                                                  F32     Y_face,
                                                                  F32     Z_face,
                                                                  F32     X_up,
                                                                  F32     Y_up,
                                                                  F32     Z_up);

extern S32        AILCALL AIL_API_sample_3D_position             (HSAMPLE  S,
                                                                  F32 *X,
                                                                  F32 *Y,
                                                                  F32 *Z);

extern void       AILCALL AIL_API_sample_3D_velocity             (HSAMPLE  S,
                                                                  F32 *dX_per_ms,
                                                                  F32 *dY_per_ms,
                                                                  F32 *dZ_per_ms);

extern void       AILCALL AIL_API_sample_3D_orientation          (HSAMPLE  S,
                                                                  F32 *X_face,
                                                                  F32 *Y_face,
                                                                  F32 *Z_face,
                                                                  F32 *X_up,
                                                                  F32 *Y_up,
                                                                  F32 *Z_up);

extern void       AILCALL AIL_API_update_sample_3D_position      (HSAMPLE S,
                                                                  F32     dt_milliseconds);

extern void       AILCALL AIL_API_set_listener_3D_position(HDIGDRIVER dig,
                                                           F32     X,
                                                           F32     Y,
                                                           F32     Z);

extern void       AILCALL AIL_API_set_listener_3D_velocity(HDIGDRIVER dig,
                                                           F32     dX_per_ms,
                                                           F32     dY_per_ms,
                                                           F32     dZ_per_ms,
                                                           F32     magnitude);

extern void       AILCALL AIL_API_set_listener_3D_velocity_vector  (HDIGDRIVER dig,
                                                                    F32     dX_per_ms,
                                                                    F32     dY_per_ms,
                                                                    F32     dZ_per_ms);

extern void       AILCALL AIL_API_set_listener_3D_orientation      (HDIGDRIVER dig,
                                                                    F32     X_face,
                                                                    F32     Y_face,
                                                                    F32     Z_face,
                                                                    F32     X_up,
                                                                    F32     Y_up,
                                                                    F32     Z_up);

extern void       AILCALL AIL_API_set_sample_3D_volume_falloff          (HSAMPLE S, MSSGRAPHPOINT* graph, S32 pointcount);
extern void       AILCALL AIL_API_set_sample_3D_lowpass_falloff         (HSAMPLE S, MSSGRAPHPOINT* graph, S32 pointcount);
extern void       AILCALL AIL_API_set_sample_3D_exclusion_falloff       (HSAMPLE S, MSSGRAPHPOINT* graph, S32 pointcount);
extern void       AILCALL AIL_API_set_sample_3D_spread_falloff          (HSAMPLE S, MSSGRAPHPOINT* graph, S32 pointcount);
extern void       AILCALL AIL_API_set_sample_3D_position_segments       (HSAMPLE S, MSSVECTOR3D* points, S32 pointcount);
extern void       AILCALL AIL_API_set_sample_3D_spread                  (HSAMPLE S, F32 spread);

extern void       AILCALL AIL_API_listener_3D_position             (HDIGDRIVER  dig,
                                                                    F32 *X,
                                                                    F32 *Y,
                                                                    F32 *Z);

extern void       AILCALL AIL_API_listener_3D_velocity             (HDIGDRIVER  dig,
                                                                    F32 *dX_per_ms,
                                                                    F32 *dY_per_ms,
                                                                    F32 *dZ_per_ms);

extern void       AILCALL AIL_API_listener_3D_orientation          (HDIGDRIVER  dig,
                                                                    F32 *X_face,
                                                                    F32 *Y_face,
                                                                    F32 *Z_face,
                                                                    F32 *X_up,
                                                                    F32 *Y_up,
                                                                    F32 *Z_up);

extern void       AILCALL AIL_API_update_listener_3D_position      (HDIGDRIVER dig,
                                                                    F32     dt_milliseconds);

extern S32        AILCALL AIL_API_calculate_3D_channel_levels      (HDIGDRIVER                   driver, //)
                                                                    F32                     *channel_levels,
                                                                    MSS_SPEAKER const * *speaker_array,
                                                                    MSSVECTOR3D             *src_pos,
                                                                    MSSVECTOR3D             *src_face,
                                                                    MSSVECTOR3D             *src_up,
                                                                    F32                          src_inner_angle,
                                                                    F32                          src_outer_angle,
                                                                    F32                          src_outer_volume,
                                                                    F32                          src_max_dist,
                                                                    F32                          src_min_dist,
                                                                    MSSVECTOR3D             *listen_pos, 
                                                                    MSSVECTOR3D             *listen_face,
                                                                    MSSVECTOR3D             *listen_up,  
                                                                    F32                          rolloff_factor,
                                                                    MSSVECTOR3D             *doppler_velocity,
                                                                    F32                     *doppler_shift);


#ifdef IS_GENERICDIG
extern HDIGDRIVER AILCALL AIL_API_open_generic_digital_driver( U32 frequency,
                                                       S32 bits,
                                                       S32 channel,
                                                       U32 flags,
                                                       RADSS_OPEN_FUNC dig_open);
#else
extern HDIGDRIVER AILCALL AIL_API_open_digital_driver( U32 frequency,
                                                       S32 bits,
                                                       S32 channel,
                                                       U32 flags );
#endif

extern void AILCALL AIL_API_close_digital_driver( HDIGDRIVER dig );

#if defined(IS_WINDOWS)

extern S32 AILCALL AIL_API_digital_handle_release(HDIGDRIVER drvr);

extern S32 AILCALL AIL_API_digital_handle_reacquire
                                                 (HDIGDRIVER drvr);
#endif

#if defined(IS_WINDOWS) || (defined(IS_MAC) && !defined(IS_IPHONE))


extern HDIGINPUT AILCALL AIL_API_open_input             (AIL_INPUT_INFO *info);

extern void      AILCALL AIL_API_close_input            (HDIGINPUT         dig);

extern AIL_INPUT_INFO *
                AILCALL AIL_API_get_input_info              (HDIGINPUT         dig);

extern S32       AILCALL AIL_API_set_input_state        (HDIGINPUT         dig,
                                                         S32               enable);

#endif

#ifdef MSS_FLT_SUPPORTED

void * SS_execute_filter_chain(HSAMPLE  S, //)
                                   void * src,
                                   void * temp_dest,
                                   S32      src_bytes,
                                   S32      is_stereo,
                                   S16 *dest_mono_sample_buffer);

//
// Internal FLT services
//

void FLT_init_list(void);

FLTPROVIDER *FLT_find_provider_instance(HPROVIDER  provider,
                                            HDIGDRIVER dig);

void FLT_disconnect_driver(HDIGDRIVER dig);

void FLT_call_premix_processors(HDIGDRIVER dig);

S32 FLT_call_postmix_processors(HDIGDRIVER dig, void *output_buffer);

//
// FLT services
//

extern S32        AILCALL AIL_API_enumerate_filters  (HMSSENUM  *next,
                                                      HPROVIDER *dest,
                                                      C8  * *name);

extern HDRIVERSTATE
                 AILCALL AIL_API_open_filter        (HPROVIDER  lib,
                                                     HDIGDRIVER dig);

extern void       AILCALL AIL_API_close_filter       (HDRIVERSTATE filter);






extern S32        AILCALL AIL_API_enumerate_filter_attributes
                                                (HPROVIDER                  lib,
                                                 HMSSENUM *             next,
                                                 RIB_INTERFACE_ENTRY *  dest);

extern void       AILCALL AIL_API_filter_attribute   (HPROVIDER  lib,
                                                      C8 const *   name,
                                                      void * val);

extern void       AILCALL AIL_API_set_filter_preference
                                                (HPROVIDER  lib,
                                                 C8 const *   name,
                                                 void const * val);





extern  S32      AILCALL AIL_API_enumerate_filter_sample_attributes
                                                (HPROVIDER                 lib,
                                                 HMSSENUM *            next,
                                                 RIB_INTERFACE_ENTRY * dest);

extern  void     AILCALL AIL_API_filter_sample_attribute
                                                (HSAMPLE      S,
                                                 C8 const *     name,
                                                 void *   val);

extern  void     AILCALL AIL_API_set_filter_sample_preference
                                                (HSAMPLE      S,
                                                 C8 const *     name,
                                                 void const *   val);
#endif

extern S32  AILCALL AIL_API_enumerate_digital_driver_attributes
                                               (HDIGDRIVER                 dig,
                                                HMSSENUM *             next,
                                                RIB_INTERFACE_ENTRY *  dest);

extern void AILCALL AIL_API_digital_driver_attribute   (HDIGDRIVER     dig,
                                                   C8 const * name,
                                                   void *     val);

extern S32  AILCALL AIL_API_enumerate_digital_driver_preferences
                                               (HDIGDRIVER                 dig,
                                                HMSSENUM *             next,
                                                RIB_INTERFACE_ENTRY *  dest);

extern void AILCALL AIL_API_set_digital_driver_preference(HDIGDRIVER        dig,
                                                     C8 const *    name,
                                                     void const *  val);

extern S32  AILCALL AIL_API_enumerate_sample_attributes
                                                (HSAMPLE                    S,
                                                 HMSSENUM *             next,
                                                 RIB_INTERFACE_ENTRY *  dest);

extern void AILCALL AIL_API_sample_attribute         (HSAMPLE          S,
                                                 C8 const *   name,
                                                 void *       val);

extern S32  AILCALL AIL_API_enumerate_sample_preferences
                                               (HSAMPLE                    S,
                                                HMSSENUM *             next,
                                                RIB_INTERFACE_ENTRY *  dest);

extern void AILCALL AIL_API_set_sample_preference (HSAMPLE          S,
                                              C8 const *   name,
                                              void const * val);

extern S32 AILCALL AIL_API_digital_CPU_percent(HDIGDRIVER dig);

extern S32 AILCALL AIL_API_digital_SPU_percent(HDIGDRIVER dig);

extern S32 AILCALL AIL_API_background_CPU_percent(void);

extern S32 AILCALL AIL_API_digital_latency(HDIGDRIVER dig);

extern HSAMPLE AILCALL AIL_API_allocate_sample_handle
                                                  (HDIGDRIVER dig);

extern HSAMPLE AILCALL AIL_API_allocate_bus(HDIGDRIVER dig);
extern HSAMPLE AILCALL AIL_API_bus_sample_handle(HDIGDRIVER dig, S32 bus_index);
extern S32 AILCALL AIL_API_install_bus_compressor(HDIGDRIVER dig, S32 bus_index, SAMPLESTAGE filter_stage, S32 input_bus_index);
DXDEC void AILCALL AIL_free_all_busses(HDIGDRIVER dig); // For miles studio.

extern HSAMPLE AILCALL AIL_API_allocate_file_sample
                                                  (HDIGDRIVER dig,
                                                   void const *file_image,
                                                   S32         block);

extern void  AILCALL AIL_API_set_speaker_configuration
                                                      (HDIGDRIVER       dig,
                                                       MSSVECTOR3D *array,
                                                       S32              n_channels,
                                                       F32              falloff_power);

extern MSSVECTOR3D * 
                    AILCALL AIL_API_speaker_configuration
                                                      (HDIGDRIVER       dig,
                                                       S32         *n_physical_channels,
                                                       S32         *n_logical_channels,
                                                       F32         *falloff_power,
                                                       MSS_MC_SPEC *channel_spec);

extern void  AILCALL AIL_API_set_listener_relative_receiver_array
                                                      (HDIGDRIVER             dig,
                                                       MSS_RECEIVER_LIST *array,
                                                       S32                    n_receivers);

extern MSS_RECEIVER_LIST * 
                    AILCALL AIL_API_listener_relative_receiver_array 
                                                      (HDIGDRIVER dig,
                                                       S32   *n_receivers);

extern void         AILCALL AIL_API_set_speaker_reverb_levels
                                                      (HDIGDRIVER       dig,
                                                       F32         *wet_array,
                                                       F32         *dry_array,
                                                       MSS_SPEAKER const *speaker_index_array,
                                                       S32              n_levels);

extern S32          AILCALL AIL_API_speaker_reverb_levels  (HDIGDRIVER                   dig,
                                                            F32               * *wet_array,
                                                            F32               * *dry_array,
                                                            MSS_SPEAKER const * *speaker_index_array);

extern void AILCALL AIL_API_release_sample_handle (HSAMPLE S);

extern S32 AILCALL AIL_API_set_sample_file        (HSAMPLE S,
                                                   void const *file_image,
                                                   S32     block);

extern S32 AILCALL AIL_API_set_sample_info        (HSAMPLE S,
                                                   AILSOUNDINFO const * info);

extern S32          AILCALL AIL_API_set_named_sample_file (HSAMPLE   S,
                                                           C8 const   *file_type_suffix,
                                                           void const *file_image,
                                                           U32       file_size,
                                                           S32       block);

extern HPROVIDER AILCALL AIL_API_set_sample_processor     (HSAMPLE     S,
                                                           SAMPLESTAGE pipeline_stage,
                                                           HPROVIDER   provider);

extern HPROVIDER AILCALL AIL_API_set_digital_driver_processor
                                                          (HDIGDRIVER  dig,
                                                           DIGDRVSTAGE pipeline_stage,
                                                           HPROVIDER   provider);

extern HPROVIDER AILCALL AIL_API_sample_processor     (HSAMPLE     S,
                                                       SAMPLESTAGE pipeline_stage);

extern HPROVIDER AILCALL AIL_API_digital_driver_processor
                                                      (HDIGDRIVER  dig,
                                                       DIGDRVSTAGE pipeline_stage);

extern void AILCALL AIL_API_set_sample_address    (HSAMPLE S,
                                                   void const *start,
                                                   U32     len);

extern S32 AILCALL AIL_API_init_sample           (HSAMPLE S,
                                                  S32     format);

extern void AILCALL AIL_API_set_sample_adpcm_block_size
                                                  (HSAMPLE S,
                                                   U32     blocksize);

extern void AILCALL AIL_API_start_sample            (HSAMPLE S);
extern void AILCALL AIL_API_stop_sample             (HSAMPLE S);
extern void AILCALL AIL_API_end_fade_sample         (HSAMPLE S);
extern void AILCALL AIL_API_resume_sample           (HSAMPLE S);
extern void AILCALL AIL_API_end_sample              (HSAMPLE S);

extern void AILCALL AIL_API_sample_group_operation  (HDIGDRIVER dig, S32 op, S32 start_id, S32 set_to_id);
extern void AILCALL AIL_API_schedule_start_sample   (HSAMPLE S, U64 mix_time_to_start);

extern void AILCALL AIL_API_push_system_state       (HDIGDRIVER dig, U32 flags, S16 crossfade_ms);
extern void AILCALL AIL_API_pop_system_state        (HDIGDRIVER dig, S16 crossfade_ms);

extern void AILCALL AIL_API_set_sample_playback_rate
                                                  (HSAMPLE S,
                                                   S32     playback_rate);

extern void AILCALL AIL_API_set_sample_playback_rate_factor
                                                  (HSAMPLE S,
                                                   F32     playback_rate_factor);

extern void AILCALL AIL_API_set_sample_playback_delay
                                                  (HSAMPLE S,
                                                   S32     playback_delay);

extern void AILCALL AIL_API_set_sample_volume_pan (HSAMPLE S,
                                                   F32     volume,
                                                   F32     pan);

extern S32  AILCALL AIL_API_sample_channel_count   (HSAMPLE S, U32 *mask);

extern S32 AILCALL AIL_API_set_sample_is_3D        (HSAMPLE                S, //)
                                                    S32                    onoff);

extern void AILCALL AIL_API_set_sample_speaker_scale_factors (HSAMPLE                 S, //)
                                                              MSS_SPEAKER const * dest_speaker_indexes,
                                                              F32         const * levels,
                                                              S32                     n_levels );

extern void AILCALL AIL_API_sample_speaker_scale_factors (HSAMPLE                 S, //)
                                                          MSS_SPEAKER const * dest_speaker_indexes,
                                                          F32               * levels,
                                                          S32                     n_levels );

extern void AILCALL AIL_API_set_sample_channel_levels (HSAMPLE                S,
                                                       MSS_SPEAKER const *source_speaker_indexes,
                                                       MSS_SPEAKER const *dest_speaker_indexes,
                                                       F32         const *levels,
                                                       S32                    n_levels);

extern void     AILCALL AIL_API_sample_channel_levels (HSAMPLE                S,
                                                       MSS_SPEAKER const *source_speaker_indexes,
                                                       MSS_SPEAKER const *dest_speaker_indexes,
                                                       F32               *levels,
                                                       S32                    n_levels);

extern F32      AILCALL AIL_API_sample_output_levels  (HSAMPLE            S,
                                                       MSS_SPEAKER const *source_speaker_indexes,
                                                       MSS_SPEAKER const *dest_speaker_indexes,
                                                       F32               *levels,
                                                       S32                n_levels);

extern void     AILCALL AIL_API_set_sample_51_volume_pan( HSAMPLE S,
                                                          F32     volume,
                                                          F32     pan,
                                                          F32     fb_pan,
                                                          F32     center_level,
                                                          F32     sub_level );

extern void     AILCALL AIL_API_sample_51_volume_pan    ( HSAMPLE S,
                                                          F32* volume,
                                                          F32* pan,
                                                          F32* fb_pan,
                                                          F32* center_level,
                                                          F32* sub_level );

extern  void    AILCALL AIL_API_set_sample_51_volume_levels( HSAMPLE S,
                                                             F32     f_left_level,
                                                             F32     f_right_level,
                                                             F32     b_left_level,
                                                             F32     b_right_level,
                                                             F32     center_level,
                                                             F32     sub_level );

extern  void    AILCALL AIL_API_sample_51_volume_levels    ( HSAMPLE S,
                                                             F32* f_left_level,
                                                             F32* f_right_level,
                                                             F32* b_left_level,
                                                             F32* b_right_level,
                                                             F32* center_level,
                                                             F32* sub_level );
extern void AILCALL AIL_API_set_sample_volume_levels(HSAMPLE S,
                                                     F32     left_level,
                                                     F32     right_level);

extern void AILCALL AIL_API_set_sample_reverb_levels(HSAMPLE S,
                                                     F32     dry_level,
                                                     F32     wet_level);

extern void AILCALL AIL_API_set_sample_low_pass_cut_off( HSAMPLE S,
                                                         S32 /*-1 or MSS_SPEAKER*/ input_channel,
                                                         F32         cutoff );

extern void AILCALL AIL_API_set_sample_loop_count (HSAMPLE S,
                                                   S32     loop_count);

extern void AILCALL AIL_API_set_sample_loop_block (HSAMPLE S,
                                                   S32     loop_start_offset,
                                                   S32     loop_end_offset);

extern S32  AILCALL AIL_API_sample_loop_block     (HSAMPLE S,
                                                   S32    *loop_start_offset,
                                                   S32    *loop_end_offset);

extern U32 AILCALL AIL_API_sample_status          (HSAMPLE S);

extern S32 AILCALL AIL_API_sample_playback_rate   (HSAMPLE S);

extern F32 AILCALL AIL_API_sample_playback_rate_factor (HSAMPLE S);

extern S32 AILCALL AIL_API_sample_playback_delay (HSAMPLE S);

extern void AILCALL AIL_API_sample_volume_pan     (HSAMPLE S,
                                                   F32* volume,
                                                   F32* pan);

extern void AILCALL AIL_API_sample_volume_levels(HSAMPLE S,
                                                 F32 * left_level,
                                                 F32 * right_level);

extern void AILCALL AIL_API_sample_reverb_levels(HSAMPLE S,
                                                 F32 * dry_level,
                                                 F32 * wet_level);

extern F32 AILCALL AIL_API_sample_low_pass_cut_off(HSAMPLE S, S32 /*-1 or MSS_SPEAKER*/ channel);

extern S32 AILCALL AIL_API_sample_loop_count      (HSAMPLE S);

extern void AILCALL AIL_API_set_digital_master_volume_level
                                                  (HDIGDRIVER dig,
                                                   F32         master_volume);

extern F32 AILCALL AIL_API_digital_master_volume_level  (HDIGDRIVER dig);

extern void AILCALL AIL_API_set_digital_master_volume_levels(HDIGDRIVER dig,
                                                     F32     left_level,
                                                     F32     right_level);

extern void AILCALL AIL_API_set_digital_master_reverb_levels(HDIGDRIVER dig,
                                                     S32     bus_index,
                                                     F32     dry_level,
                                                     F32     wet_level);

extern void AILCALL AIL_API_set_digital_master_room_type(HDIGDRIVER dig,
                                                         S32        bus_index,
                                                         S32        room_type);

extern void AILCALL AIL_API_digital_master_reverb_levels(HDIGDRIVER dig,
                                                         S32   bus_index,
                                                         F32 * dry_level,
                                                         F32 * wet_level);

extern void AILCALL AIL_API_set_digital_master_reverb(HDIGDRIVER dig,
                                                      S32     bus_index,
                                                      F32     reverb_time,
                                                      F32     reverb_predelay,
                                                      F32     reverb_damping);

extern void AILCALL AIL_API_digital_master_reverb(HDIGDRIVER dig,
                                                  S32    bus_index,
                                                  F32 *  reverb_time,
                                                  F32 *  reverb_predelay,
                                                  F32 *  reverb_damping);

extern S32 AILCALL AIL_API_minimum_sample_buffer_size
                                                  (HDIGDRIVER dig,
                                                   S32         playback_rate,
                                                   S32         format);

extern S32  AILCALL AIL_API_set_sample_buffer_count
                                                  (HSAMPLE S,
                                                   S32     n_buffers);

extern S32 AILCALL AIL_API_sample_buffer_count    (HSAMPLE S);

extern S32 AILCALL AIL_API_sample_buffer_available (HSAMPLE S);

extern S32 AILCALL AIL_API_sample_loaded_len( HSAMPLE S );

extern S32 AILCALL AIL_API_load_sample_buffer    (HSAMPLE S,
                                                   S32    buff_num,
                                                   void const *buffer,
                                                   U32    len);

extern S32 AILCALL AIL_API_sample_buffer_info     (HSAMPLE S,
                                                   S32     buff_num,
                                                   U32    *pos,
                                                   U32    *len,
                                                   S32    *head,
                                                   S32    *tail);

extern U32 AILCALL AIL_API_sample_granularity     (HSAMPLE S);

extern void AILCALL AIL_API_set_sample_position   (HSAMPLE S,
                                                   U32     pos);

extern U32 AILCALL AIL_API_sample_position        (HSAMPLE S);

extern AILSAMPLECB AILCALL AIL_API_register_SOB_callback
                                                  (HSAMPLE S,
                                                   AILSAMPLECB SOB);

extern AILSAMPLECB AILCALL AIL_API_register_EOB_callback
                                                  (HSAMPLE S,
                                                   AILSAMPLECB EOB);

extern AILSAMPLECB AILCALL AIL_API_register_EOS_callback
                                                  (HSAMPLE S,
                                                   AILSAMPLECB EOS);

extern AILFALLOFFCB AILCALL AIL_API_register_falloff_function_callback
                                                  (HSAMPLE S,
                                                   AILFALLOFFCB falloff_cb);

extern void AILCALL AIL_API_set_sample_user_data  (HSAMPLE S,
                                                   U32     index,
                                                   SINTa   value);

extern SINTa AILCALL AIL_API_sample_user_data       (HSAMPLE S,
                                                     U32     index);

extern S32 AILCALL AIL_API_active_sample_count    (HDIGDRIVER dig);

extern void AILCALL AIL_API_digital_configuration (HDIGDRIVER dig,
                                                   S32   *rate,
                                                   S32   *format,
                                                   char  *config);

//
// High-level XMIDI services
//

#if defined(IS_WINDOWS) 

extern S32 AILCALL AIL_API_midiOutOpen           (HMDIDRIVER *drvr,
                                                   LPHMIDIOUT *lphMidiOut,
                                                   U32            dwDeviceID);

extern void AILCALL AIL_API_midiOutClose          (HMDIDRIVER mdi);


extern S32 AILCALL AIL_API_MIDI_handle_release    (HMDIDRIVER mdi);

extern S32 AILCALL AIL_API_MIDI_handle_reacquire  (HMDIDRIVER mdi);

#else

extern HMDIDRIVER AILCALL AIL_API_open_XMIDI_driver( U32 flags );

extern void AILCALL AIL_API_close_XMIDI_driver( HMDIDRIVER mdi );

#endif

extern HSEQUENCE AILCALL AIL_API_allocate_sequence_handle
                                                  (HMDIDRIVER mdi);

extern void AILCALL AIL_API_release_sequence_handle
                                                  (HSEQUENCE S);

extern S32 AILCALL AIL_API_init_sequence          (HSEQUENCE S,
                                                   void const *start,
                                                   S32       sequence_num);

extern void AILCALL AIL_API_start_sequence        (HSEQUENCE S);
extern void AILCALL AIL_API_stop_sequence         (HSEQUENCE S);
extern void AILCALL AIL_API_resume_sequence       (HSEQUENCE S);
extern void AILCALL AIL_API_end_sequence          (HSEQUENCE S);

extern void AILCALL AIL_API_set_sequence_tempo    (HSEQUENCE S,
                                                   S32       tempo,
                                                   S32       milliseconds);

extern void AILCALL AIL_API_set_sequence_volume   (HSEQUENCE S,
                                                   S32       volume,
                                                   S32       milliseconds);

extern void AILCALL AIL_API_set_sequence_loop_count
                                                  (HSEQUENCE S,
                                                   S32       loop_count);

extern U32 AILCALL AIL_API_sequence_status        (HSEQUENCE S);

extern S32 AILCALL AIL_API_sequence_tempo         (HSEQUENCE S);
extern S32 AILCALL AIL_API_sequence_volume        (HSEQUENCE S);
extern S32 AILCALL AIL_API_sequence_loop_count    (HSEQUENCE S);

extern void AILCALL AIL_API_set_XMIDI_master_volume
                                                  (HMDIDRIVER mdi,
                                                   S32        master_volume);

extern S32 AILCALL AIL_API_XMIDI_master_volume    (HMDIDRIVER mdi);

//
// Low-level XMIDI services
//

extern S32 AILCALL AIL_API_active_sequence_count  (HMDIDRIVER mdi);

extern S32 AILCALL AIL_API_controller_value       (HSEQUENCE S,
                                                   S32       channel,
                                                   S32       controller_num);

extern S32 AILCALL AIL_API_channel_notes          (HSEQUENCE S,
                                                   S32       channel);

extern void AILCALL AIL_API_sequence_position     (HSEQUENCE S,
                                                   S32       *beat,
                                                   S32       *measure);

extern void AILCALL AIL_API_branch_index          (HSEQUENCE S,
                                                   U32       marker);

extern AILPREFIXCB AILCALL AIL_API_register_prefix_callback
                                                  (HSEQUENCE S,
                                                   AILPREFIXCB callback);

extern AILTRIGGERCB AILCALL AIL_API_register_trigger_callback
                                                  (HSEQUENCE S,
                                                   AILTRIGGERCB callback);

extern AILSEQUENCECB AILCALL AIL_API_register_sequence_callback
                                                  (HSEQUENCE S,
                                                   AILSEQUENCECB callback);

extern AILEVENTCB AILCALL AIL_API_register_event_callback
                                                  (HMDIDRIVER mdi,
                                                   AILEVENTCB callback);

extern AILBEATCB AILCALL AIL_API_register_beat_callback
                                                  (HSEQUENCE  S,
                                                   AILBEATCB callback);

extern AILTIMBRECB AILCALL AIL_API_register_timbre_callback
                                                  (HMDIDRIVER mdi,
                                                   AILTIMBRECB callback);

extern void AILCALL AIL_API_set_sequence_user_data(HSEQUENCE S,
                                                   U32       index,
                                                   SINTa     value);

extern SINTa AILCALL AIL_API_sequence_user_data     (HSEQUENCE S,
                                                     U32       index);

extern void AILCALL AIL_API_register_ICA_array    (HSEQUENCE S,
                                                   U8        *array);

extern S32 AILCALL AIL_API_lock_channel           (HMDIDRIVER mdi);

extern void AILCALL AIL_API_release_channel       (HMDIDRIVER mdi,
                                                   S32        channel);

extern void AILCALL AIL_API_map_sequence_channel  (HSEQUENCE S,
                                                   S32       seq_channel,
                                                   S32       new_channel);

extern S32 AILCALL AIL_API_true_sequence_channel  (HSEQUENCE S,
                                                   S32       seq_channel);

extern void AILCALL AIL_API_send_channel_voice_message
                                                  (HMDIDRIVER mdi,
                                                   HSEQUENCE  S,
                                                   S32        status,
                                                   S32        data_1,
                                                   S32        data_2);

extern void AILCALL AIL_API_send_sysex_message    (HMDIDRIVER mdi,
                                                   void const *buffer);

extern HWAVESYNTH
       AILCALL AIL_API_create_wave_synthesizer    (HDIGDRIVER dig,
                                                   HMDIDRIVER mdi,
                                                   void const *wave_lib,
                                                   S32        polyphony);

extern void AILCALL AIL_API_destroy_wave_synthesizer(HWAVESYNTH W);

extern S32 AILCALL AIL_API_MIDI_to_XMI            (void const* MIDI,
                                                   U32       MIDI_size,
                                                   void**XMIDI,
                                                   U32 * XMIDI_size,
                                                   S32       flags);

#if defined(IS_WIN32)

extern S32 AILCALL AIL_API_list_MIDI              (void const*  MIDI,
                                                   U32        MIDI_size,
                                                   char* *list,
                                                   U32 *  list_size,
                                                   S32        flags);

extern S32 AILCALL AIL_API_list_DLS               (void const* DLS,
                                                   char**list,
                                                   U32 * list_size,
                                                   S32       flags,
                                                   C8  * title);

#endif

extern char* AILCALL AIL_API_last_error       ( void );

extern void AILCALL AIL_API_set_error             ( char const* error_msg );

extern S32 AILCALL AIL_API_file_error             (void);

extern S32 AILCALL AIL_API_file_size              (char const   *filename,
                                                   char const   * caller,
                                                   U32 caller_line);

extern void * AILCALL AIL_API_file_read           (char const   *filename,
                                                   void *dest,
                                                   char const   * caller,
                                                   U32 caller_line);

extern S32 AILCALL AIL_API_file_write             (char const   *filename,
                                                   void const *buf,
                                                   U32       len);

extern S32 AILCALL AIL_API_WAV_file_write         (char const   *filename,
                                                   void const *buf,
                                                   U32       len,
                                                   S32       rate,
                                                   S32       format);

extern void AILCALL AIL_API_serve                 (void);


#ifdef IS_WINDOWS

extern HREDBOOK AILCALL AIL_API_redbook_open      (U32 which);

extern HREDBOOK AILCALL AIL_API_redbook_open_drive(S32 drive);

extern void AILCALL AIL_API_redbook_close         (HREDBOOK hand);

extern void AILCALL AIL_API_redbook_eject         (HREDBOOK hand);

extern void AILCALL AIL_API_redbook_retract       (HREDBOOK hand);

extern U32 AILCALL AIL_API_redbook_status         (HREDBOOK hand);

extern U32 AILCALL AIL_API_redbook_tracks         (HREDBOOK hand);

extern U32 AILCALL AIL_API_redbook_track          (HREDBOOK hand);

extern void AILCALL AIL_API_redbook_track_info    (HREDBOOK hand,
                                                   U32 tracknum,
                                                   U32* startmsec,
                                                   U32* endmsec);

extern U32 AILCALL AIL_API_redbook_id             (HREDBOOK hand);

extern U32 AILCALL AIL_API_redbook_position       (HREDBOOK hand);

extern U32 AILCALL AIL_API_redbook_play           (HREDBOOK hand,
                                                   U32 startmsec,
                                                   U32 endmsec);

extern U32 AILCALL AIL_API_redbook_stop           (HREDBOOK hand);

extern U32 AILCALL AIL_API_redbook_pause          (HREDBOOK hand);

extern U32 AILCALL AIL_API_redbook_resume         (HREDBOOK hand);

extern F32 AILCALL AIL_API_redbook_volume_level   (HREDBOOK hand);

extern F32 AILCALL AIL_API_redbook_set_volume_level(HREDBOOK hand,
                                                    F32 volume);

#endif

extern S32 AILCALL AIL_API_quick_startup          (S32         use_digital,
                                                   S32         use_MIDI,
                                                   U32         output_rate,
                                                   S32         output_bits,
                                                   S32         output_channels);

extern void AILCALL AIL_API_quick_shutdown        (void);

extern void AILCALL AIL_API_quick_handles         (HDIGDRIVER* pdig,
                                                   HMDIDRIVER* pmdi,
                                                   HDLSDEVICE* pdls);

extern HAUDIO AILCALL AIL_API_quick_load          (char const *filename);

extern HAUDIO AILCALL AIL_API_quick_load_mem      (void const *mem,
                                                   U32    size);

extern HAUDIO AILCALL AIL_API_quick_load_named_mem(void const *mem,
                                                   char const *filename,
                                                   U32    size);

extern void AILCALL AIL_API_quick_unload          (HAUDIO      audio);

extern S32 AILCALL AIL_API_quick_play             (HAUDIO      audio,
                                                   U32         loop_count);

extern void AILCALL AIL_API_quick_halt            (HAUDIO      audio);

extern S32 AILCALL AIL_API_quick_status           (HAUDIO      audio);

extern HAUDIO AILCALL AIL_API_quick_load_and_play (char const *filename,
                                                   U32         loop_count,
                                                   S32         wait_request);

extern void AILCALL AIL_API_quick_set_speed       (HAUDIO      audio,
                                                   S32         speed);

extern void AILCALL AIL_API_quick_set_volume      (HAUDIO      audio,
                                                   F32         volume,
                                                   F32         extravol);

extern void AILCALL AIL_API_quick_set_reverb_levels(HAUDIO  audio,
                                                    F32     dry_level,
                                                    F32     wet_level);

extern void AILCALL AIL_API_quick_set_low_pass_cut_off(HAUDIO  audio,
                                                       S32     channel,
                                                       F32     cut_off);

extern HAUDIO AILCALL AIL_API_quick_copy          (HAUDIO hand);

extern void AILCALL AIL_API_quick_set_ms_position (HAUDIO audio,
                                                   S32 milliseconds);

extern S32 AILCALL AIL_API_quick_ms_position      (HAUDIO audio);

extern S32 AILCALL AIL_API_quick_ms_length        (HAUDIO audio);

extern S32 AILCALL AIL_API_quick_type             (HAUDIO audio);

//
// High-level streaming services
//

void AILSTRM_shutdown(HDIGDRIVER driver);

extern HSTREAM AILCALL AIL_API_open_stream        (HDIGDRIVER dig,
                                                   char const* filename,
                                                   S32 stream_mem);

extern void AILCALL AIL_API_close_stream          (HSTREAM stream);

extern S32 AILCALL AIL_API_service_stream         (HSTREAM stream,
                                                   S32 fillup);

extern void AILCALL AIL_API_start_stream          (HSTREAM stream);

extern void AILCALL AIL_API_pause_stream          (HSTREAM stream,
                                                   S32 onoff);

extern void AILCALL AIL_API_set_stream_loop_block (HSTREAM S,
                                                   S32       loop_start_offset,
                                                   S32       loop_end_offset);

extern S32 AILCALL AIL_API_stream_loop_count      (HSTREAM stream);

extern void AILCALL AIL_API_set_stream_loop_count (HSTREAM stream,
                                                   S32 count);

extern S32 AILCALL AIL_API_stream_status          (HSTREAM stream);

extern F32 AILCALL AIL_API_stream_filled_percent  (HSTREAM stream);

extern void AILCALL AIL_API_set_stream_position   (HSTREAM stream,
                                                   S32 offset);

extern S32 AILCALL AIL_API_stream_position        (HSTREAM stream);

extern void AILCALL AIL_API_stream_info           (HSTREAM stream,
                                                   S32* datarate,
                                                   S32* sndtype,
                                                   S32* length,
                                                   S32* memory);

extern void AILCALL AIL_API_auto_service_stream   (HSTREAM stream,
                                                   S32 onoff);

extern AILSTREAMCB AILCALL AIL_API_register_stream_callback
                                                  (HSTREAM stream,
                                                   AILSTREAMCB callback);

extern void AILCALL AIL_API_set_stream_user_data  (HSTREAM S,
                                                   U32     index,
                                                   SINTa   value);

extern SINTa AILCALL AIL_API_stream_user_data       (HSTREAM S,
                                                     U32     index);

extern S32 AILCALL AIL_API_size_processed_digital_audio(
                                 U32             dest_rate,
                                 U32             dest_format,
                                 S32             num_srcs,
                                 AILMIXINFO const* src);

extern S32 AILCALL AIL_API_process_digital_audio(
                                 void       *dest_buffer,
                                 S32             dest_buffer_size,
                                 U32             dest_rate,
                                 U32             dest_format,
                                 S32             num_srcs,
                                 AILMIXINFO* src);

extern HDLSDEVICE AILCALL AIL_API_DLS_open        (HMDIDRIVER mdi,
                                                   HDIGDRIVER dig,
#ifdef IS_STATIC
                                                   AILSTATICDLS const* dls,
#else
                                                   char const* libname,
#endif
                                                   U32 flags,
                                                   U32 rate,
                                                   S32 bits,
                                                   S32 channels);


extern void AILCALL AIL_API_DLS_close             (HDLSDEVICE dls,
                                                   U32 flags);

extern HDLSFILEID AILCALL AIL_API_DLS_load_file   (HDLSDEVICE dls,
                                                   char const* filename,
                                                   U32 flags);

extern HDLSFILEID AILCALL AIL_API_DLS_load_memory (HDLSDEVICE dls,
                                                   void const* memfile,
                                                   U32 flags);

extern void AILCALL AIL_API_DLS_unload            (HDLSDEVICE dls,
                                                   HDLSFILEID dlsid);

extern void AILCALL AIL_API_DLS_compact           (HDLSDEVICE dls);

extern void AILEXPORT AIL_API_DLS_set_reverb_levels(HDLSDEVICE dls,
                                                    F32   dry_level,
                                                    F32   wet_level);

extern void AILEXPORT AIL_API_DLS_get_reverb_levels(HDLSDEVICE dls,
                                                    F32*    dry_level,
                                                    F32*    wet_level);

extern void AILCALL AIL_API_DLS_get_info          (HDLSDEVICE dls,
                                                   AILDLSINFO* info,
                                                   S32* PercentCPU);

extern S32 AILCALL AIL_API_extract_DLS            (void const*source_image, //)
                                                   U32      source_size,
                                                   void * *XMI_output_data,
                                                   U32        *XMI_output_size,
                                                   void * *DLS_output_data,
                                                   U32        *DLS_output_size,
                                                   AILLENGTHYCB    callback);

extern void AILCALL AIL_API_set_sequence_ms_position
                                                  (HSEQUENCE S, //)
                                                   S32       milliseconds);

extern void AILCALL AIL_API_sequence_ms_position  (HSEQUENCE S, //)
                                                   S32 *total_milliseconds,
                                                   S32 *current_milliseconds);

extern void AILCALL AIL_API_set_sample_ms_position(HSAMPLE S, //)
                                                   S32       milliseconds);

extern void AILCALL AIL_API_sample_ms_position    (HSAMPLE S, //)
                                                   S32    *total_milliseconds,
                                                   S32   *current_milliseconds);

extern U32  AILCALL AIL_API_sample_ms_lookup      (HSAMPLE S, //)
                                                   S32 milliseconds,
                                                   S32* actualms);

extern void AILCALL AIL_API_set_stream_ms_position(HSTREAM   S, //)
                                                   S32       milliseconds);

extern void AILCALL AIL_API_stream_ms_position    (HSTREAM   S, //)
                                                   S32  *total_milliseconds,
                                                   S32  *current_milliseconds);

extern S32 AILCALL AIL_API_WAV_info               (void const* data,
                                                   AILSOUNDINFO* info);

extern S32 AILCALL AIL_API_compress_ADPCM         (AILSOUNDINFO const* info, //)
                                                   void** outdata, U32* outsize);

extern S32 AILCALL AIL_API_decompress_ADPCM       (AILSOUNDINFO const* info,
                                                   void** outdata,
                                                   U32* outsize);

extern S32 AILCALL AIL_API_file_type              (void const* data,
                                                   U32 size);

extern S32 AILCALL AIL_API_file_type_named        (void const* data,
                                                   char const* filename,
                                                   U32 size);

extern S32 AILCALL AIL_API_find_DLS               (void const* data,
                                                   U32 size,
                                                   void**xmi,
                                                   U32* xmisize,
                                                   void**dls,
                                                   U32* dlssize);

//
// Internal MSS mixer RIB calls
//

extern U32  AILCALL MSS_MMX_available (void);

extern void AILCALL MSS_mixer_startup  (void);
extern void AILCALL MSS_mixer_shutdown (void);

extern void AILCALL MSS_mixer_flush(S32 *dest,
                                    S32      len
#ifdef IS_X86
                                   ,U32             MMX_available
#endif
                                    );

extern void AILCALL MSS_mixer_merge(void const * *src,
                                    U32        *src_fract,
                                    void const *src_end,
                                    S32  * *dest,
                                    void       *dest_end,
                                    S32        *left_val,
                                    S32        *right_val,
                                    S32             playback_ratio,
                                    S32             scale_left,
                                    S32             scale_right,
                                    U32             operation
#ifdef IS_X86
                                   ,U32             MMX_available
#endif
                                    );

extern void AILCALL MSS_mixer_copy(void const *src,
                                   S32       src_len,
                                   void *dest,
                                   U32       operation
#ifdef IS_BE
                                   ,S32       big_endian_output
#else
#ifdef IS_X86
                                   ,U32       MMX_available
#endif
#endif
                                   );

extern void AILCALL SS_set_speaker_configuration (D3DSTATE        *D3D,
                                                  MSSVECTOR3D *speaker_positions,
                                                  S32              n_channels,
                                                  S32              logical_channels_per_sample);

extern void SS_calculate_3D_channel_levels(D3DSTATE const *D3D, 
                                            S32                    logical_channels_per_sample,
                                            S3DSTATE          *S3D,
                                            F32               *channel_levels);

extern void AILCALL SS_update_driver_reverb_state(HDIGDRIVER dig);

extern void AILCALL SS_update_sample_reverb_state(HSAMPLE S);

extern void AILCALL SS_flush (HDIGDRIVER dig, S32 reverb_index);

extern void AILCALL SS_copy (HDIGDRIVER dig, S32 reverb_index, void *lpWaveAddr);


extern F32 AILCALLBACK SS_default_falloff_function_callback(HSAMPLE   S, //)
                                                            F32       distance,
                                                            F32       rolloff_factor,
                                                            F32       min_dist,
                                                            F32       max_dist);
extern S32 AILCALLBACK SS_stream_to_buffer(HSAMPLE  S,
                                           S16 *dest_mono_sample_buffer,
                                           S32      dest_buffer_size);


#ifdef MILES10
extern void SS_fill(HDIGDRIVER dig);
#else
extern void SS_fill(HDIGDRIVER dig, void *lpData);
#endif

extern void AILCALL SS_start_DIG_driver_playback(HSAMPLE S);

extern S32 AILCALL SS_set_sample_type( HSAMPLE S, S32 format, U32 channel_mask );

extern void AILCALL AIL_apply_reverb( HDIGDRIVER dig );

extern S32 AILCALL AIL_allocate_reverb_buffers( HDIGDRIVER dig, S32 index );

extern void AILCALL AIL_apply_lowpass( void* dest, void const* src, void const* src_end, LOWPASS_INFO* lp_in, S32 op );


#if defined(IS_WIN32) || defined(IS_GENERICDIG)
extern S32 AILCALL get_system_speaker_configuration(MSS_MC_SPEC *channel_spec);
#endif


#ifdef IS_BE

  #ifdef IS_PPC

    #ifdef IS_XENON

      #define LE_SWAP32( ptr ) __loadwordbytereverse(0,ptr)

      #define LE_SWAP32_OFS( ptr,ofs ) __loadwordbytereverse(ofs,ptr)

      #define STORE_LE_SWAP32( ptr, val )  __storewordbytereverse ( (U32)(val), 0, ptr )

    #elif (defined(IS_PS3) && !defined(IS_SPU)) || defined(IS_MAC)
      #define LE_SWAP32( ptr ) ld_le32(ptr)
      #define LE_SWAP32_OFS( ptr, ofs ) ld_le32_ofs(ptr,ofs)
                                                   // the char* is not required, but works around a GCC bug
      #define STORE_LE_SWAP32( ptr, val )  st_le32( (char*)(ptr), (U32)(val) )
    #elif defined(IS_WIIU)
    
      #define LE_SWAP32(ptr) (*(__bytereversed unsigned int *)(ptr))
      #define LE_SWAP32_OFS(ptr, ofs) (*(__bytereversed unsigned int *)AIL_ptr_add(ptr, ofs))

      #define STORE_LE_SWAP32( ptr, val ) (*(__bytereversed unsigned int *)(ptr) = (val))

      #define STORE_LE_SWAP16_OFS( ptr, val, const_ofs ) (*(__bytereversed unsigned short*)(AIL_ptr_add(ptr, const_ofs))) = (val)
    #else
      #define LE_SWAP32( ptr ) __lwbrx((void*)(ptr),0)
      #define LE_SWAP32_OFS( ptr,ofs ) __lwbrx((void*)(ptr),ofs)

      #define STORE_LE_SWAP32( ptr, val )  __stwbrx( (U32)(val), ptr, 0 )
    #endif

  #elif defined(IS_SPU)

    static U32 inline LE_SWAP32(void const * src) __attribute__((always_inline));
    static U32 inline LE_SWAP32(void const * src)
    {
      U32 i = (*((U32*)src));
      vec_uint4 v = spu_promote( i, 0 );
      v = (vec_uint4)spu_shuffle( v, v, ((vec_uchar16){ 3,  2,  1,  0,  4,  5, 6, 7,
                                                        8,  9, 10, 11, 12, 13, 14, 15}));
      i = spu_extract( v, 0 );                     
      return i;
    }

    static U32 inline LE_SWAP32_OFS(void const * src,S32 ofs) __attribute__((always_inline));
    static U32 inline LE_SWAP32_OFS(void const * src,S32 ofs)
    {
      U32 i = (*((U32*)(((char*)src)+ofs)));
      vec_uint4 v = spu_promote( i, 0 );
      v = (vec_uint4)spu_shuffle( v, v, ((vec_uchar16){ 3,  2,  1,  0,  4,  5, 6, 7,
                                                        8,  9, 10, 11, 12, 13, 14, 15}));
      i = spu_extract( v, 0 );                     
      return i;
    }

    #define STORE_LE_SWAP32( ptr, val )\
    {                                   \
      U32 tmp = (U32) val;  \
      vec_uint4 v = spu_promote( tmp, 0 ); \
      v = (vec_uint4)spu_shuffle( v, v, ((vec_uchar16){ 3,  2,  1,  0,  4,  5, 6, 7, \
                                                        8,  9, 10, 11, 12, 13, 14, 15})); \
      tmp = spu_extract( v, 0 );                     \
      *((U32 *)(((char*)ptr)+0)) = tmp; \
    }

    #define STORE_LE_SWAP32_OFS( ptr, val, ofs )\
    {                                   \
      U32 tmp = (U32) val;  \
      vec_uint4 v = spu_promote( tmp, 0 ); \
      v = (vec_uint4)spu_shuffle( v, v, ((vec_uchar16){ 3,  2,  1,  0,  4,  5, 6, 7, \
                                                        8,  9, 10, 11, 12, 13, 14, 15})); \
      tmp = spu_extract( v, 0 );                     \
      *((U32 *)(((char*)ptr)+ofs)) = tmp; \
    }

  #else

    #define LE_SWAP32(ptr) \
                     ( ( ( ( *((U32 *)(ptr) ) ) << 24 ) ) | \
                       ( ( ( *((U32 *)(ptr) ) ) <<  8 ) & 0x00FF0000 ) | \
                       ( ( ( *((U32 *)(ptr) ) ) >>  8 ) & 0x0000FF00 ) | \
                       ( ( ( *((U32 *)(ptr) ) ) >> 24 ) ) )

    #define LE_SWAP32_OFS(ptr,ofs) \
                     ( ( ( ( *((U32 *)AIL_ptr_add(ptr,ofs) ) ) << 24 ) ) | \
                       ( ( ( *((U32 *)AIL_ptr_add(ptr,ofs) ) ) <<  8 ) & 0x00FF0000 ) | \
                       ( ( ( *((U32 *)AIL_ptr_add(ptr,ofs) ) ) >>  8 ) & 0x0000FF00 ) | \
                       ( ( ( *((U32 *)AIL_ptr_add(ptr,ofs) ) ) >> 24 ) ) )

    #define STORE_LE_SWAP32( ptr, val )  { *((U32 *)ptr) = \
                     ( ( ( ( ((U32)(val) ) ) << 24 ) ) | \
                       ( ( ( ((U32)(val) ) ) <<  8 ) & 0x00FF0000 ) | \
                       ( ( ( ((U32)(val) ) ) >>  8 ) & 0x0000FF00 ) | \
                       ( ( ( ((U32)(val) ) ) >> 24 ) ) );  }

  #endif

  #if defined( IS_PPC )

    #ifdef IS_XENON
      unsigned short      __loadshortbytereverse  (int offset, const void * base);
      unsigned long       __loadwordbytereverse   (int offset, const void * base);

      void            __storeshortbytereverse (unsigned short val, int offset, void * base);
      void            __storewordbytereverse  (unsigned int   val, int offset, void * base);

      #define LE_SWAP16( ptr ) __loadshortbytereverse (0,(S16 *)ptr)

      #define LE_SWAP16_OFS( ptr, const_ofs ) __loadshortbytereverse (const_ofs,(S16 *)ptr)

      #define STORE_LE_SWAP16( ptr, val )  __storeshortbytereverse ( (U16)(val), 0, (S16 *)ptr )

      #define STORE_LE_SWAP16_OFS( ptr, val, const_ofs )  __storeshortbytereverse ( (U16)(val), const_ofs, (S16 *)ptr )
    #else

      #if (defined(IS_PS3) && !defined(IS_SPU)) || defined(IS_MAC)

        #define __fsel( outf, cmp, inf1, inf2 ) \
        __asm__ __volatile__ ("fsel %0,%1,%2,%3" : "=f" (outf) : "f" (cmp), "f" (inf1), "f" (inf2));

        static inline float fclip1(float sample )
        {
          float ret;

          __fsel( ret, sample + 1.0f, sample, -1.0f );
          __fsel( ret, sample - 1.0f, 1.0f, ret );
          return( ret );
        }

      static __inline__ U16 ld_le16(const S16 *addr)
      {
        U16 val;
        __asm__ __volatile__ ("lhbrx %0,0,%1" : "=r" (val) : "r" (addr));
        return val;
      }

      static __inline__ U16 ld_le16_ofs(const S16 *addr, U64 const_ofs )
      {
        U16 val;
        __asm__ __volatile__ ("lhbrx %0,%1,%2" : "=r" (val) : "b" (const_ofs), "r" (addr));
        return val;
      }

      static __inline__ void st_le16(S16 *addr, const U16 val)
      {
        __asm__ __volatile__ ("sthbrx %1,0,%2" : "=m" (*addr) : "r" (val), "r" (addr));
      }

      static __inline__ void st_le16_ofs(S16 *addr, U64 const_ofs, const U16 val)
      {
        __asm__ __volatile__ ("sthbrx %2,%1,%3" : "=m" (*addr) : "b" (const_ofs), "r" (val), "r" (addr));
      }

      static __inline__ U32 ld_le32(const void *addr)
      {
        U32 val;

        __asm__ __volatile__ ("lwbrx %0,0,%1" : "=r" (val) : "r" ((U32*)addr) );
        return val;
      }

      static __inline__ U32 ld_le32_ofs(const void *addr, U64 const_ofs )
      {
        U32 val;
        __asm__ __volatile__ ("lwbrx %0,%1,%2" : "=r" (val) : "b" (const_ofs), "r" (addr));
        return val;
      }

/*      static __inline__ void st_le32_ofs(void *addr, U64 const_ofs, const U32 val)
      {
        __asm__ __volatile__ ("stwbrx %2,%1,%3" : "=m" (*(U32*)addr) : "b" (const_ofs), "r" (val), "r" ((U32*)addr));
      }*/

      static __inline__ void st_le32(void *addr, const U32 val)
      {                                                        //todo, weird hacks to make this work with GCC
        __asm__ __volatile__ ("stwbrx %1,%3,%2" : "=m" (*(U32*)addr) : "r" (val), "r" ((U32*)addr), "O"(0) );
      }

      #define LE_SWAP16( ptr ) ld_le16 ((S16 *)ptr)

      #define LE_SWAP16_OFS( ptr, const_ofs ) ld_le16_ofs ((S16 *)ptr, const_ofs)

      #define STORE_LE_SWAP16( ptr, val )  st_le16( (S16 *)ptr, (U16)(val) )

      #define STORE_LE_SWAP16_OFS( ptr, val, const_ofs ) st_le16_ofs( (S16 *)ptr, const_ofs, (U16)(val) )
    #elif defined(IS_WIIU)

      #define LE_SWAP16(ptr) (*(__bytereversed unsigned short *)(ptr))
      #define LE_SWAP16_OFS(ptr, ofs) (*(__bytereversed unsigned short *)AIL_ptr_add(ptr, ofs))

      #define STORE_LE_SWAP16( ptr, val ) (*(__bytereversed unsigned short *)(ptr) = (val))

    #else
      #define LE_SWAP16( ptr ) __lhbrx((S16 *)ptr,0)

      #define LE_SWAP16_OFS( ptr, const_ofs ) __lhbrx((S16 *)ptr,const_ofs)

      #define STORE_LE_SWAP16( ptr, val )  __sthbrx( (U16)(val), (S16 *)ptr, 0 )

      #define STORE_LE_SWAP16_OFS( ptr, val, const_ofs )  __sthbrx( (U16)(val), (S16 *)ptr, const_ofs )
    #endif
  #endif

  #elif defined( IS_SPU )

    static U32 inline LE_SWAP16(void const * src) __attribute__((always_inline));
    static U32 inline LE_SWAP16(void const * src)
    {
      U32 i = (*((U16*)src));
      vec_uint4 v = spu_promote( i, 0 );
      v = (vec_uint4)spu_shuffle( v, v, ((vec_uchar16){ 0,  1,  3,  2,  4,  5, 6, 7,
                                                        8,  9, 10, 11, 12, 13, 14, 15}));
      i = spu_extract( v, 0 );                     
      return i;
    }
    
    static U32 inline LE_SWAP16_OFS(void const * src, S32 ofs) __attribute__((always_inline));
    static U32 inline LE_SWAP16_OFS(void const * src, S32 ofs)
    {
      U32 i = (*((U16*)(((char*)src)+ofs)));
      vec_uint4 v = spu_promote( i, 0 );
      v = (vec_uint4)spu_shuffle( v, v, ((vec_uchar16){ 0,  1,  3,  2,  4,  5, 6, 7,
                                                        8,  9, 10, 11, 12, 13, 14, 15}));
      i = spu_extract( v, 0 );                     
      return i;
    }

    #define STORE_LE_SWAP16( ptr, val )\
    {                                   \
      U32 tmp = (U16) val;  \
      vec_uint4 v = spu_promote( tmp, 0 ); \
      v = (vec_uint4)spu_shuffle( v, v, ((vec_uchar16){ 0,  1,  3,  2,  4,  5, 6, 7, \
                                                        8,  9, 10, 11, 12, 13, 14, 15})); \
      tmp = spu_extract( v, 0 );                     \
      *((U16 *)(((char*)ptr)+0)) = tmp; \
    }

    #define STORE_LE_SWAP16_OFS( ptr, val, ofs )\
    {                                   \
      U32 tmp = (U16) val;  \
      vec_uint4 v = spu_promote( tmp, 0 ); \
      v = (vec_uint4)spu_shuffle( v, v, ((vec_uchar16){ 0,  1,  3,  2,  4,  5, 6, 7, \
                                                        8,  9, 10, 11, 12, 13, 14, 15})); \
      tmp = spu_extract( v, 0 );                     \
      *((U16 *)(((char*)ptr)+ofs)) = tmp;\
    }

  #else

    #define LE_SWAP16(ptr) \
                     ( ( U16 ) \
                       ( ( ( ( *((U16 *)(ptr) ) ) << 8 ) ) | \
                         ( ( ( *((U16 *)(ptr) ) ) >> 8 ) ) ) \
                     )

    #define LE_SWAP16_OFS(ptr,const_ofs) \
                     ( ( U16 ) \
                       ( ( ( ( *((U16 *)(AIL_ptr_add(ptr,const_ofs)) ) ) << 8 ) ) | \
                         ( ( ( *((U16 *)(AIL_ptr_add(ptr,const_ofs)) ) ) >> 8 ) ) ) \
                     )

    #define STORE_LE_SWAP16( ptr, val )  { *((U16*)ptr) = ( \
                       ( ( ( ((U16)(val) ) ) <<  8 ) ) | \
                       ( ( ( ((U16)(val) ) ) >>  8 ) ) ); }

    #define STORE_LE_SWAP16_OFS( ptr, val, const_ofs )  { *((U16*)AIL_ptr_add(ptr,const_ofs)) = ( \
                       ( ( ( ((U16)(val) ) ) <<  8 ) ) | \
                       ( ( ( ((U16)(val) ) ) >>  8 ) ) ); }

  #endif

  #define BE_SWAP32( ptr ) ( *((U32 *)(ptr) ) )
  #define BE_SWAP16( ptr ) ( *((U16 *)(ptr) ) )

  #define MEM_LE_SWAP32(n) *((U32*)n) = LE_SWAP32(n);
  #define MEM_LE_SWAP16(n) *((U16*)n) = (U16) LE_SWAP16(n);

  #define MEM_BE_SWAP32(n)
  #define MEM_BE_SWAP16(n)

  // unaligned versions
  #define BEU_SWAP32( ptr ) ( *((U32 *)(ptr) ) )
  #define BEU_SWAP16( ptr ) ( *((U16 *)(ptr) ) )

  #define LEU_SWAP32(ptr) LE_SWAP32(ptr)
  #define LEU_SWAP16(ptr) LE_SWAP16(ptr)

  #define STORE_LEU_SWAP32( ptr, val )  STORE_LE_SWAP32( ptr, val )
  #define STORE_LEU_SWAP16( ptr, val )  STORE_LE_SWAP16( ptr, val )

  #define MEM_LEU_SWAP32(n) *((U32*)n) = LEU_SWAP32(n);
  #define MEM_LEU_SWAP16(n) *((U32*)n) = LEU_SWAP32(n);

#else // IS_BE

  #define LE_SWAP32( ptr ) ( *((U32 *)(ptr) ) )
  #define LE_SWAP32_OFS( ptr,const_ofs ) ( *((U32 *)AIL_ptr_add(ptr,const_ofs) ) )
  #define LE_SWAP16( ptr ) ( *((U16 *)(ptr) ) )

  #define LE_SWAP16_OFS( ptr, const_ofs ) ( *((U16 *)(AIL_ptr_add(ptr,const_ofs)) ) )

  #define STORE_LE_SWAP32( ptr, val )  { *((U32*)(ptr))=(U32)(val); }
  #define STORE_LE_SWAP16( ptr, val )  { *((U16*)(ptr))=(U16)(val); }
  #define STORE_LE_SWAP16_OFS( ptr, val, const_ofs )  { *((U16*)((((U8*)ptr)+const_ofs)))=(U16)(val); }

  #define BE_SWAP32(ptr) \
                     ( ( ( ( *((U32 *)(ptr) ) ) << 24 ) ) | \
                       ( ( ( *((U32 *)(ptr) ) ) <<  8 ) & 0x00FF0000 ) | \
                       ( ( ( *((U32 *)(ptr) ) ) >>  8 ) & 0x0000FF00 ) | \
                       ( ( ( *((U32 *)(ptr) ) ) >> 24 ) ) )

  #define BE_SWAP16(ptr) \
                     ( ( U16 ) \
                       ( ( ( ( *((U16 *)(ptr) ) ) << 8 ) ) | \
                         ( ( ( *((U16 *)(ptr) ) ) >> 8 ) ) ) \
                     )

#if defined(IS_PSP)

  #define putunaligned4(temp, s ) \
  __asm__("swl %1,3+%0" : "+m"(*(s)): "r"(temp));  \
  __asm__("swr %1,%0" : "+m"(*(s)) : "r"(temp));

  #define getunaligned4(temp, s ) \
  __asm__("lwl %0,3+%1" : "=r"(temp) : "o"(*(U32*)(s)));  \
  __asm__("lwr %0,%1" : "+r"(temp) : "o"(*(U32*)(s)) );

  static inline U32 LEU_SWAP32( void * ptr )
  {
    U32 ret;
    getunaligned4( ret, ptr );
    return( ret );
  }

  static inline U32 BEU_SWAP32( void * ptr )
  {
    U32 ret;

    getunaligned4( ret, ptr );
    ret = ( ret << 24 ) | ( ( ret << 8 ) & 0x00ff0000 ) | ( ( ret >> 8 ) & 0x0000ff00 ) | ( ret >> 24 );
    return( ret );
  }

  #define LEU_SWAP16(ptr) \
                     ( ( U16 ) \
                       ( ( ( ( ((U32)(((U8 *)(ptr))[1]) ) ) << 8 ) ) | \
                         ( ( ( ((U32)(((U8 *)(ptr))[0]) ) ) ) ) ) \
                     )
  #define BEU_SWAP16(ptr) \
                     ( ( U16 ) \
                       ( ( ( ( ((U32)(((U8 *)(ptr))[0]) ) ) << 8 ) ) | \
                         ( ( ( ((U32)(((U8 *)(ptr))[1]) ) ) ) ) ) \
                     )

  #define STORE_LEU_SWAP32( ptr, val )  { register U32 __v = (U32)val; register U32 * __p = (U32*)ptr; putunaligned4( __v, __p ); }
  #define STORE_LEU_SWAP16( ptr, val )  { register U16 __v = (U16)val; ((U8*)(ptr))[0]=(U8)val; ((U8*)(ptr))[1]=(U8)(val>>8);}

#else

  #define LEU_SWAP32(ptr) LE_SWAP32(ptr)
  #define LEU_SWAP16(ptr) LE_SWAP16(ptr)

  #define BEU_SWAP32(ptr) BE_SWAP32(ptr)
  #define BEU_SWAP16(ptr) BE_SWAP16(ptr)

  #define STORE_LEU_SWAP32( ptr, val )  STORE_LE_SWAP32( ptr, val )
  #define STORE_LEU_SWAP16( ptr, val )  STORE_LE_SWAP16( ptr, val )

#endif

  #define MEM_LEU_SWAP32(n)
  #define MEM_LEU_SWAP16(n)

  #define MEM_LE_SWAP32(n)
  #define MEM_LE_SWAP16(n)

  #define MEM_BE_SWAP32(n) *((U32*)n) = BE_SWAP32(n);
  #define MEM_BE_SWAP16(n) *((U16*)n) = BE_SWAP16(n);

#endif



#if defined(IS_MAC) || defined(IS_WII) || defined(IS_IPHONE)

#ifdef IS_X86

static inline U32 mult64addsubandshift( U32 mt1, U32 mt2, U32 addv, U32 subv, U32 shift )
{
  U32 retv;

  unsigned long long value;
  value = (unsigned long long)mt1 * mt2;
  value += addv;
  value -= subv;
  value = (signed long long )value >> (signed long long )shift;
  retv = (U32)value;

  return( retv );

}


static U32 __inline mult64anddiv(U32 a,U32 b, U32 c)
{
  U32 retv;
  unsigned long long value;
  value = (unsigned long long)a * b;
  value /= c;
  retv = (U32)value;
  return( retv );
}

static U32 __inline shift64addsubanddiv( U32 val, U32 shift,U32 addv, U32 subv, U32 divv)
{
  U32 retv;
  unsigned long long value;
  value = (unsigned long long)val;
  value <<= shift;
  value += addv;
  value -= subv;
  value = (signed long long ) value / (signed long long ) divv;
  retv = (U32)value;
  return( retv );
}

#elif defined(IS_IPHONE)

#define WRITE_MONO_SAMPLE( dest, fOut )                      \
{                                                            \
   if (fOut > 32767.0F)       *dest++ =  32767;              \
   else if (fOut < -32768.0F) *dest++ = -32768;              \
   else *dest++ = (S16) fOut;                                \
}

static U32 __inline mult64addsubandshift( U32 mt1, U32 mt2, U32 addv, U32 subv, U32 shift )
{
  U32 retv;
  unsigned long long value;
  value = (unsigned long long)mt1 * mt2;
  value += addv;
  value -= subv;
  value = (signed long long )value >> (signed long long )shift;
  retv = (U32)value;
  return( retv );
}

static U32 __inline mult64anddiv(U32 a,U32 b, U32 c)
{
  U32 retv;
  unsigned long long value;
  value = (unsigned long long)a * b;
  value /= c;
  retv = (U32)value;
  return( retv );
}

static U32 __inline shift64addsubanddiv( U32 val, U32 shift,U32 addv, U32 subv, U32 divv)
{
  U32 retv;
  unsigned long long value;
  value = (unsigned long long)val;
  value <<= shift;
  value += addv;
  value -= subv;
  value = (signed long long ) value / (signed long long ) divv;
  retv = (U32)value;
  return( retv );
}

#else

//
// These three dudes help deal with u64s in hi,lo format
//

#define mul64hilo(_hi,_lo,_mt1,_mt2)    \
  __asm                                 \
  {                                     \
    mulhwu _hi, _mt1, _mt2;             \
    mullw _lo, _mt1, _mt2               \
  }                                     \


#define imul64hilo(_hi,_lo,_mt1,_mt2)    \
  __asm                                 \
  {                                     \
    mulhw _hi, _mt1, _mt2;             \
    mullw _lo, _mt1, _mt2               \
  }                                     \


#define add64hilo(_hi,_lo,_hisub,_losub)    \
  __asm                                     \
  {                                         \
    addc _lo, _losub, _lo;                  \
    adde _hi, _hisub, _hi                   \
  }                                         \


#define sub64hilo(_hi,_lo,_hisub,_losub)    \
  __asm                                     \
  {                                         \
    subfc _lo, _losub, _lo;                 \
    subfe _hi, _hisub, _hi                  \
  }                                         \

#define carry_check( q, r, c, d ) \
  if ( r < c )                    \
  {                               \
    --q;                          \
    r += d;                       \
    if ( r >= d )                 \
    {                             \
      if ( r < c )                \
      {                           \
        --q;                      \
        r += d;                   \
      }                           \
    }                             \
  }

// The PPC mac doesnt' have intrinsics like CW
#ifdef IS_MAC
static U32 __cntlzw(U32 in)
{
    U32 ret;
    asm ("cntlzw %0, %1\n": "=r" (ret): "r" (in));
    return ret;
}
#endif

static inline U32 div64with16( U32 nlo, U32 nhi, U32 d )
{
  U32 dlo, dhi;
  U32 rlo, rhi;
  U32 qhi, qlo;
  U32 carry;

  dhi = d >> 16;
  dlo = d & 0xffff;

  qhi = nhi / dhi;
  rhi = nhi % dhi;

  carry = qhi * dlo;

  rhi = ( rhi << 16 ) | ( nlo >> 16 );

  carry_check( qhi, rhi, carry, d );
  rhi -= carry;

  qlo = rhi / dhi;
  rlo = rhi % dhi;
  carry = qlo * dlo;

  qhi <<= 16;

  rlo = ( rlo << 16 ) |  ( nlo & 0xffff );
  carry_check( qlo, rlo, carry, d );

//  rlo -= carry;

  return( qhi | qlo );
}

static U32 inline mult64anddiv( register U32 a, register U32 b,U32 d )
{
/*  register U32 hi, lo;
  register U32 mt1, mt2, d;

  mt1=m1;
  mt2=m2;
  d=dv;

  mul64hilo( hi, lo, mt1, mt2 );

  return( div64(hi,lo,d) );
*/
  U32 lz;
  register U32 nhi=0, nlo=0;

  mul64hilo( nhi, nlo, a, b );

  if ( ( d & ( d - 1 ) ) == 0 )
  {
    lz = (U32) __cntlzw( d );

    // Shift for powers of 2.
    return( ( nhi << ( lz + 1 ) ) | ( nlo >> ( 31 - lz ) ) );
  }

  if ( nhi == 0 )
  {
    return( nlo / d );
  }

  lz = (U32) __cntlzw( d );

  d <<= lz;
  nhi = ( nhi << lz ) + ( nlo >> ( 32 - lz ) );
  nlo <<= lz;

  return( div64with16( nlo, nhi, d ) );

}

static U32 inline mult64andshift( U32 m1,U32 m2,U32 shift )
{
  register U32 hi, lo;
  register U32 mt1, mt2;

  mt1=m1;
  mt2=m2;

  mul64hilo( hi, lo, mt1, mt2 );

  return( ( hi << (32 - shift ) ) + ( lo >> shift ) );
}

static U32 inline mult64addsubandshift( U32 m1, U32 m2, U32 av, U32 sv, U32 shift )
{
  register U32 hi=0, lo=0;
  register U32 mt1, mt2;
  register U32 addv;
  register U32 subv;
  register U32 zero = 0;

  mt1=m1;
  mt2=m2;
  addv=av;
  subv=sv;

  mul64hilo(hi,lo,mt1,mt2);
  add64hilo( hi, lo, zero, addv );
  sub64hilo( hi, lo, zero, subv );

  return( ( hi << (32 - shift ) ) + ( lo >> shift ) );
}

static U32 __inline shift64addsubanddiv( U32 val, U32 shift,U32 av, U32 sv, U32 divv)
{
  register U32 hi, lo;
  register U32 addv;
  register U32 subv;
  register U32 d;
  register U32 zero = 0;
  U32 lz;

  addv=av;
  subv=sv;
  d=divv;

  hi = val >> ( 32 - shift );
  lo = val << shift;
  add64hilo( hi, lo, zero, addv );
  sub64hilo( hi, lo, zero, subv );

  if ( hi & 0x80000000 )
  {
    register U32 ihi = hi;
    register U32 ilo = lo;
    hi = lo = 0;
    sub64hilo( hi, lo, ihi, ilo );

    if ( ( d & ( d - 1 ) ) == 0 )
    {
      lz = (U32) __cntlzw( d );

      // Shift for powers of 2.
      return( (U32) -(S32) ( ( hi << ( lz + 1 ) ) | ( lo >> ( 31 - lz ) ) ) );
    }

    if ( hi == 0 )
    {
      return( (U32) -(S32) ( lo / d ) );
    }

    return( (U32) -(S32) div64with16( lo,hi,d ) );
  }

  if ( ( d & ( d - 1 ) ) == 0 )
  {
    lz = (U32) __cntlzw( d );

    // Shift for powers of 2.
    return( ( hi << ( lz + 1 ) ) | ( lo >> ( 31 - lz ) ) );
  }

  if ( hi == 0 )
  {
    return( lo / d );
  }

  return( div64with16( lo,hi,d ) );
}

#define WRITE_MONO_SAMPLE( dest, fOut )                        \
{                                                              \
   if (fOut > 32767.0F)       STORE_LE_SWAP16( dest, 32767 );  \
   else if (fOut < -32768.0F) STORE_LE_SWAP16( dest, -32768 ); \
   else STORE_LE_SWAP16( dest, (S16) fOut );                   \
   ++dest;                                                     \
}

#endif

#else

#if ( defined(IS_X86) && defined(IS_WIN32API) ) || defined(IS_LINUX) || defined(__RADSEKRIT2__)

//
// Macros to aid in writing to build buffers
//

#define WRITE_MONO_SAMPLE( dest, fOut )                      \
{                                                            \
   if (fOut > 32767.0F)       *dest++ =  32767;              \
   else if (fOut < -32768.0F) *dest++ = -32768;              \
   else *dest++ = (S16) fOut;                                \
}

static U32 __inline mult64addsubandshift( U32 mt1, U32 mt2, U32 addv, U32 subv, U32 shift )
{
  U32 retv;

#ifdef __GNUC__
  unsigned long long value;
  value = (unsigned long long)mt1 * mt2;
  value += addv;
  value -= subv;
  value = (signed long long )value >> (signed long long )shift;
  retv = (U32)value;
#else
#ifdef IS_WIN64
  U64 value;
  value = (U64) mt1 * mt2;
  value += addv;
  value -= subv;
  value = (S64) value >> (S64) shift;
  retv = (U32) value;
#else
  __asm
  {
    mov eax,[mt1]
    mov ecx,[mt2]
    mul ecx
    add eax,[addv]
    adc edx,0
    sub eax,[subv]
    sbb edx,0
    mov ecx,[shift]
    shrd eax,edx,cl
    mov [retv], eax
  }
#endif
#endif
  return( retv );
}

static U32 __inline mult64anddiv(U32 a,U32 b, U32 c)
{
  U32 retv;

#ifdef __GNUC__
  unsigned long long value;
  value = (unsigned long long)a * b;
  value /= c;
  retv = (U32)value;
#else
#ifdef IS_WIN64
  U64 value;
  value = (U64) a * b;
  value /= c;
  retv = (U32) value;
#else
  __asm
  {
    mov eax,[a]
    mov ecx,[b]
    mul ecx
    mov ecx,[c]
    div ecx
    mov [retv], eax
  }
#endif
#endif
  return( retv );
}

static U32 __inline shift64addsubanddiv( U32 val, U32 shift,U32 addv, U32 subv, U32 divv)
{
  U32 retv;
#ifdef __GNUC__
  unsigned long long value;
  value = (unsigned long long)val;
  value <<= shift;
  value += addv;
  value -= subv;
  value = (signed long long ) value / (signed long long ) divv;
  retv = (U32)value;
#else
#ifdef IS_WIN64
  U64 value;
  value = (U64) val;
  value <<= shift;
  value += addv;
  value -= subv;
  value = (S64) value / (S64) divv;
  retv = (U32) value;
#else
  __asm
  {
    xor edx, edx
    mov eax, [val]
    mov ecx, [shift]
    shld edx, eax, cl
    shl eax, cl
    add eax, [addv]
    adc edx, 0
    sub eax, [subv]
    sbb edx, 0
    mov ecx, [divv]
    idiv ecx
    mov [retv], eax
  }
#endif
#endif
  return( retv );
}

#else

#ifdef __WATCOMC__

#else

#if defined( IS_XENON )

#define toU64(v) ((U64)((U32)(v)))
#define toS64(v) ((S64)((S32)(v)))

#define mult64anddiv(mt1,mt2,d) ((U32)((toU64(mt1)*toU64(mt2))/toU64(d)))

#define mult64addsubandshift(mt1,mt2,addv,subv,d) ((U32)((((S64)((toU64(mt1)*toU64(mt2))+toU64(addv)))-toS64(subv))>>toS64(d)))

#define shift64addsubanddiv(val,shift,addv,subv,divv) ((U32)(((S64)((toU64(val)<<toU64(shift))+toU64(addv)-toU64(subv)))/toS64(divv)))

#define WRITE_MONO_SAMPLE( dest, fOut )                        \
{                                                              \
   if (fOut > 32767.0F)       STORE_LE_SWAP16( dest, 32767 );  \
   else if (fOut < -32768.0F) STORE_LE_SWAP16( dest, -32768 ); \
   else STORE_LE_SWAP16( dest, (S16) fOut );                   \
   ++dest;                                                     \
}


#else

#if defined( IS_PSP ) || defined(IS_3DS) || defined(IS_PSP2) || defined(__RADANDROID__)


#define toU64(v) ((U64)((U32)(v)))
#define toS64(v) ((S64)((S32)(v)))

#define mult64anddiv(mt1,mt2,d) ((U32)((toU64(mt1)*toU64(mt2))/toU64(d)))

#define mult64addsubandshift(mt1,mt2,addv,subv,d) ((U32)((((S64)((toU64(mt1)*toU64(mt2))+toU64(addv)))-toS64(subv))>>toS64(d)))

#define shift64addsubanddiv(val,shift,addv,subv,divv) ((U32)(((S64)((toU64(val)<<toU64(shift))+toU64(addv)-toU64(subv)))/toS64(divv)))

#define WRITE_MONO_SAMPLE( dest, fOut )                      \
{                                                            \
   if (fOut > 32767.0F)       *dest++ =  32767;              \
   else if (fOut < -32768.0F) *dest++ = -32768;              \
   else *dest++ = (S16) fOut;                                \
}

#elif defined(IS_WIIU)

#define toU64(v) ((U64)((U32)(v)))
#define toS64(v) ((S64)((S32)(v)))

#define mult64anddiv(mt1,mt2,d) ((U32)((toU64(mt1)*toU64(mt2))/toU64(d)))

#define mult64addsubandshift(mt1,mt2,addv,subv,d) ((U32)((((S64)((toU64(mt1)*toU64(mt2))+toU64(addv)))-toS64(subv))>>toS64(d)))

#define shift64addsubanddiv(val,shift,addv,subv,divv) ((U32)(((S64)((toU64(val)<<toU64(shift))+toU64(addv)-toU64(subv)))/toS64(divv)))

#define WRITE_MONO_SAMPLE( dest, fOut )                      \
{                                                            \
   if (fOut > 32767.0F)       STORE_LE_SWAP16( dest, 32767 );  \
   else if (fOut < -32768.0F) STORE_LE_SWAP16( dest, -32768 ); \
   else STORE_LE_SWAP16( dest, (S16) fOut );                   \
   ++dest;                                                     \
}



#elif defined( IS_PS3 ) && !defined( IS_SPU )

#define toU64(v) ((U64)((U32)(v)))
#define toS64(v) ((S64)((S32)(v)))

#define mult64anddiv(mt1,mt2,d) ((U32)((toU64(mt1)*toU64(mt2))/toU64(d)))

#define mult64addsubandshift(mt1,mt2,addv,subv,d) ((U32)((((S64)((toU64(mt1)*toU64(mt2))+toU64(addv)))-toS64(subv))>>toS64(d)))

#define shift64addsubanddiv(val,shift,addv,subv,divv) ((U32)(((S64)((toU64(val)<<toU64(shift))+toU64(addv)-toU64(subv)))/toS64(divv)))

static inline float fclip32K(float sample )
{
  float ret;

  __fsel( ret, sample + 32767.0f, sample, -32767.0f );
  __fsel( ret, sample - 32767.0f, 32767.0f, ret );
  return( ret );
}

#define WRITE_MONO_SAMPLE( dest, fOut )                        \
{                                                              \
   STORE_LE_SWAP16( dest, (S16) fclip32K(fOut) );              \
   ++dest;                                                     \
}

#else

U32 mult64addsubandshift(U32 mt1,U32 mt2,U32 addv, U32 subv, U32 shift);
U32 shift64addsubanddiv(U32 val,U32 shift,U32 addv, U32 subv, U32 divv);

#if defined( IS_SPU )

#define WRITE_MONO_SAMPLE( dest, fOut )                        \
{                                                              \
   if (fOut > 32767.0F)       *dest = (S16)(U16)0xff7f;        \
   else if (fOut < -32768.0F) *dest = (S16)(U16)0x0080;        \
   else STORE_LE_SWAP16( dest, (S16) fOut )                    \
   ++dest;                                                     \
}

#define toU64(v) ((U64)((U32)(v)))
#define toS64(v) ((S64)((S32)(v)))

#define mult64anddiv(mt1,mt2,d) ((U32)((toU64(mt1)*toU64(mt2))/toU64(d)))

#define mult64addsubandshift(mt1,mt2,addv,subv,d) ((U32)((((S64)((toU64(mt1)*toU64(mt2))+toU64(addv)))-toS64(subv))>>toS64(d)))

#define shift64addsubanddiv(val,shift,addv,subv,divv) ((U32)(((S64)((toU64(val)<<toU64(shift))+toU64(addv)-toU64(subv)))/toS64(divv)))

#endif

#endif

#endif


#endif

#endif

#endif

#ifdef MSS_SPU_PROCESS

#define SPU_NAME( flt, name ) extern "C" void AILCALL flt##_##name

//#define DEBUGSPUMEM
#ifdef DEBUGSPUMEM

  void * spu_alloc( U32 bytes );

  #ifdef __cplusplus

  extern "C" U32 num_mals;
  extern "C" void free_mals( int level );

  class stackmark
  {
    public:
     int level;
    stackmark()
    {
      level = num_mals;
    }
    ~stackmark()
    {
      free_mals( level );
    }
  };
  #define addstackmark() stackmark sm;
  #else
  #define addstackmark()
  #endif

#else

  #define addstackmark()

  #ifdef IS_SPU
    #include <alloca.h>

    static int stacksizehas( int v )
    {
      vector signed int val;
      __asm__("or %0,$1,$1" : "=r"(val) );

      if ( val[1] < ( v + 2048 ) ) { MSSBreakPoint(); }
      return v;
    }
    #define spu_alloc(bytes) (void*)alloca( stacksizehas( ( ( bytes ) + 15 ) & ~15 ) )
  #else
    #include <malloc.h>
    #define spu_alloc(bytes) (void*)alloca( ( ( bytes ) + 15 ) & ~15 )
  #endif

#endif

#else

#define SPU_NAME( flt, name ) static void AILCALL name
#define addstackmark()

#endif

//
// low-level utility memory file routines
//

typedef struct _MEMDUMP
{
  void* buffer[1024]; // up to 64 MB
  U8* current;
  U32 size;
  U32 totalsize;
  U32 curbufnum;
  U32 lastbufnum;
  U32 curpos;
  S32 error;
  S32 expandable;
  U32 eachbuf;
  U32 firstbuf;
} MEMDUMP;

// This typedef occurs in mss.h as well.
//typedef struct _MEMDUMP* HMEMDUMP;

DXDEC HMEMDUMP AILCALL AIL_mem_open(void* addr,U32 size);

DXDEC HMEMDUMP AILCALL AIL_mem_create(void);
DXDEC HMEMDUMP AILCALL AIL_mem_create_from_existing(void* addr, U32 size); // do not delete pointer passed in

DXDEC S32 AILCALL AIL_mem_close(HMEMDUMP m, void** buf, U32* size);

DXDEC U32
#if defined(IS_WIN32API)
__cdecl
#endif
AIL_mem_printf(HMEMDUMP m, char const* fmt, ...);

DXDEC U32 AILCALL AIL_mem_printc(HMEMDUMP m, char c);

DXDEC U32 AILCALL AIL_mem_prints(HMEMDUMP m, char const* s);

DXDEC U32 AILCALL AIL_mem_write(HMEMDUMP m, void const* s, U32 bytes);

DXDEC U32 AILCALL AIL_mem_read(HMEMDUMP m, void* s, U32 bytes);

DXDEC U32 AILCALL AIL_mem_seek(HMEMDUMP m, U32 pos);

DXDEC U32 AILCALL AIL_mem_size(HMEMDUMP m);

DXDEC U32 AILCALL AIL_mem_pos(HMEMDUMP m);

DXDEC S32 AILCALL AIL_mem_error(HMEMDUMP m);



#define DIG_PROCESS_BUFFER_SIZE 2048

extern U8 *ASI_mem_src_ptr;
extern S32     ASI_mem_src_len;
extern S32     ASI_mem_src_pos;


extern S32 AILCALLBACK ASI_mem_stream_CB(UINTa     user, //)
                                         void *dest,
                                         S32       bytes_requested,
                                         S32       offset);

#if 1

#if 0 //used to be a IS_LINUX

#else

DXDEC S32  AILCALL  AIL_stricmp(const char *s1, const char *s2);
DXDEC S32  AILCALL  AIL_strnicmp( const char *s1,  const char *s2, U32 maxlen);

#if defined(IS_MAC) || defined(IS_LINUX) || defined(IS_3DS) || defined(IS_IPHONE) || defined(IS_PSP2) || defined(IS_WIIU) || defined(__RADSEKRIT2__) || defined(__RADANDROID__)

#ifdef IS_PSP2
#ifdef __cplusplus
}
#endif
#endif

#include <string.h>
#include <stdlib.h> // for abs().

#ifdef IS_PSP2
#ifdef __cplusplus
extern "C" {
#endif
#endif


#define AIL_memcpy memcpy
#define AIL_memmove memmove
#define AIL_strcpy strcpy
#define AIL_strcmp strcmp
#define AIL_strlen strlen
#define AIL_strcat strcat
#define AIL_memset memset
#define AIL_ptr_add(ptr,off) ((void*)(((U8*)(ptr))+(off)))
#define AIL_ptr_dif(p1,p2) ((SINTa)((SINTa) p1) - ((SINTa) p2))
#define AIL_ptr_alloc_clone(x) ((void*)x)
#define AIL_ptr_free_clone(x)

#ifdef __cplusplus
}
  template <typename T> inline T AILptradd( T ptr, U32 off ) { return( T(AIL_ptr_add( ptr, off )) ); }
  #define AIL_ptr_inc_clone(x,y) x=AILptradd(x,y)
extern "C" {
#endif

#define AIL_ptr_fixup_clone(x)
#define AIL_ptr_lt(x,y) (((UINTa)x) < ((UINTa)y))
#define AIL_ptr_ge(x,y) (((UINTa)x) >= ((UINTa)y))
#define AIL_ptr_eq(x,y) (((UINTa)x) == ((UINTa)y))
#define AIL_ptr_ne(x,y) (((UINTa)x) != ((UINTa)y))
#define AIL_ptr_lin_addr(x) ((UINTa)(x))

#define AIL_abs abs
#define AIL_memcmp memcmp

#else // not mac or linux

#ifdef IS_WIN32API

#define AIL_abs abs

#ifdef IS_XENON

typedef void *PVOID;
typedef int                 INT;
typedef  unsigned long ULONG_PTR;
typedef ULONG_PTR SIZE_T;

PVOID
__stdcall
XMemSet(
         PVOID                       pDest,
          INT                         c,
          SIZE_T                      count
    );

PVOID
__stdcall
XMemCpy(
         PVOID                       pDest,
          const void*                 pSrc,
          SIZE_T                      count
    );

char * __cdecl strcpy( char *_Dest,  const char * _Source);

#define AIL_memcpy XMemCpy
#define AIL_memset XMemSet

#else

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(__NT__) || defined(__WIN32__)
  #include <string.h>
  void *  __cdecl memcpy(void *, const void *, size_t);
  void *  __cdecl memset(void *, int, size_t);
  char *  __cdecl strcpy(char *, const char *);
#endif

#define AIL_memcpy memcpy
#define AIL_memset memset

#endif

#if defined(IS_XENON) 
#include <string.h>
#endif

#define AIL_memmove memmove

#define AIL_memcmp memcmp

#define AIL_strcpy strcpy

#define AIL_strcmp strcmp

#define AIL_strlen strlen

#define AIL_strcat strcat

#define AIL_ptr_add(ptr,off) ((void*)(((U8*)(ptr))+(off)))

#define AIL_ptr_dif(p1,p2) ((SINTa)((SINTa) p1) - ((SINTa) p2))

#define AIL_ptr_alloc_clone(x) ((void*)x)

#define AIL_ptr_free_clone(x)

#ifdef __cplusplus
}
  template <typename T> inline T AILptradd( T ptr, U32 off ) { return( T(AIL_ptr_add( ptr, off )) ); }
  #define AIL_ptr_inc_clone(x,y) x=AILptradd(x,y)
extern "C" {
#endif

#define AIL_ptr_fixup_clone(x)

#define AIL_ptr_lt(x,y) (((UINTa)x) < ((UINTa)y))

#define AIL_ptr_ge(x,y) (((UINTa)x) >= ((UINTa)y))

#define AIL_ptr_eq(x,y) (((UINTa)x) == ((UINTa)y))

#define AIL_ptr_ne(x,y) (((UINTa)x) != ((UINTa)y))

#define AIL_ptr_lin_addr(x) ((UINTa)(x))

#else

#if defined(IS_PSP)

#define AIL_memmove memmove

#define AIL_memcpy memcpy

#define AIL_memset memset

#define AIL_memcmp memcmp

#define AIL_strcpy strcpy

#define AIL_strcmp strcmp

#define AIL_stricmp strcasecmp

#define AIL_strnicmp strncasecmp

#define AIL_strlen strlen

#define AIL_strcat strcat

#define AIL_ptr_add(ptr,off) ((void*)(((U8*)(ptr))+(off)))

#define AIL_ptr_dif(p1,p2) ((SINTa)((SINTa) p1) - ((SINTa) p2))

#define AIL_ptr_alloc_clone(x) ((void*)x)

#define AIL_ptr_free_clone(x)

#ifdef __cplusplus
}
  template <typename T> inline T AILptradd( T ptr, U32 off ) { return( T(AIL_ptr_add( ptr, off )) ); }
  #define AIL_ptr_inc_clone(x,y) x=AILptradd(x,y)
extern "C" {
#endif

#define AIL_ptr_fixup_clone(x)

#define AIL_ptr_lt(x,y) (((UINTa)x) < ((UINTa)y))

#define AIL_ptr_ge(x,y) (((UINTa)x) >= ((UINTa)y))

#define AIL_ptr_eq(x,y) (((UINTa)x) == ((UINTa)y))

#define AIL_ptr_ne(x,y) (((UINTa)x) != ((UINTa)y))

#define AIL_ptr_lin_addr(x) ((UINTa)(x))

static inline S32 AIL_abs(S32 v) { return( ( v >=0 ) ? v : -v ); }


#elif defined(IS_PS3)

#define AIL_memmove memmove

#define AIL_memcpy memcpy

#define AIL_memset memset

#define AIL_memcmp memcmp

#define AIL_strcpy strcpy

#define AIL_strcmp strcmp

#define AIL_stricmp strcasecmp_ascii

#define AIL_strnicmp strncasecmp_ascii

#define AIL_strlen strlen

#define AIL_strcat strcat

#define AIL_ptr_add(ptr,off) ((void*)(((U8*)(ptr))+(off)))

#define AIL_ptr_dif(p1,p2) ((SINTa)((SINTa) p1) - ((SINTa) p2))

#define AIL_ptr_alloc_clone(x) ((void*)x)

#define AIL_ptr_free_clone(x)

#ifdef __cplusplus
}
  template <typename T> inline T AILptradd( T ptr, U32 off ) { return( T(AIL_ptr_add( ptr, off )) ); }
  #define AIL_ptr_inc_clone(x,y) x=AILptradd(x,y)
extern "C" {
#endif

#define AIL_ptr_fixup_clone(x)

#define AIL_ptr_lt(x,y) (((UINTa)x) < ((UINTa)y))

#define AIL_ptr_ge(x,y) (((UINTa)x) >= ((UINTa)y))

#define AIL_ptr_eq(x,y) (((UINTa)x) == ((UINTa)y))

#define AIL_ptr_ne(x,y) (((UINTa)x) != ((UINTa)y))

#define AIL_ptr_lin_addr(x) ((UINTa)(x))

static inline S32 AIL_abs(S32 v) { return( ( v >=0 ) ? v : -v ); }

#else

#ifdef IS_WII

#define AIL_memcpy memcpy

#define AIL_memmove memmove

#define AIL_strcpy strcpy

#define AIL_strcmp strcmp

#define AIL_strlen strlen

#define AIL_strcat strcat

#define AIL_memset memset

#define AIL_ptr_add(ptr,off) ((void*)(((U8*)(ptr))+(off)))

#define AIL_ptr_dif(p1,p2) ((SINTa)((SINTa) p1) - ((SINTa) p2))

#define AIL_ptr_alloc_clone(x) ((void*)x)

#define AIL_ptr_free_clone(x)

#ifdef __cplusplus
}
  template <typename T> inline T AILptradd( T ptr, U32 off ) { return( T(AIL_ptr_add( ptr, off )) ); }
  #define AIL_ptr_inc_clone(x,y) x=AILptradd(x,y)
extern "C" {
#endif

#define AIL_ptr_fixup_clone(x)

#define AIL_ptr_lt(x,y) (((UINTa)x) < ((UINTa)y))

#define AIL_ptr_ge(x,y) (((UINTa)x) >= ((UINTa)y))

#define AIL_ptr_eq(x,y) (((UINTa)x) == ((UINTa)y))

#define AIL_ptr_ne(x,y) (((UINTa)x) != ((UINTa)y))

#define AIL_ptr_lin_addr(x) ((UINTa)(x))

#ifndef __cplusplus
int abs( int );
#endif

#define AIL_abs abs
#define AIL_memcmp memcmp

#endif

#endif

#endif

#endif

#endif

#endif

#ifndef AIL_ptr_inc_clone
#define AIL_ptr_inc_clone(x,y) AIL_ptr_inc_clone_ref((void * *)(void*) &(x),y)
#endif
#define AIL_ptr_from_clone(clone,hptr) (AIL_ptr_add(hptr,AIL_ptr_dif(clone,hptr)))


#ifdef IS_32

#define MSS_do_cb1(type,addr,ds,param1) \
  (addr)(param1)

#define MSS_do_cb3(type,addr,ds,param1,param2,param3) \
  (addr)(param1,param2,param3)

#define MSS_do_cb4(type,addr,ds,param1,param2,param3,param4) \
  (addr)(param1,param2,param3,param4)

#define MSS_do_cb1_with_ret(ret,type,addr,ds,param1) \
  ret = (addr)(param1)

#define MSS_do_cb3_with_ret(ret,type,addr,ds,param1,param2,param3) \
  ret = (addr)(param1,param2,param3)

#define MSS_do_cb4_with_ret(ret,type,addr,ds,param1,param2,param3,param4) \
  ret = (addr)(param1,param2,param3,param4)

#define MSS_do_cb5_with_ret(ret,type,addr,ds,param1,param2,param3,param4,param5) \
  ret = (addr)(param1,param2,param3,param4,param5)

#endif

#if (defined(IS_MAC) && defined( IS_X86 )) || defined(IS_LINUX)


#define MSS_CB_STACK_ALIGN( name )                        \
void MSS_CALLBACK_ALIGNED_NAME(name)(void)\
{\
  asm (\
    "mov $16,%%eax\n"                                  \
    "sub $64,%%esp\n"                                     \
    "add %%esp, %%eax\n"                                   \
    "and $0xfffffff0,%%eax\n"                     \
    "movups 8(%%ebp), %%xmm0\n"               \
    "movaps %%xmm0, (%%eax)\n"                      \
    "movl %%esp, 32(%%eax)\n"                   \
    "mov %%eax,%%esp\n"                          \
    "call *%0\n"                               \
    "movl 32(%%esp), %%esp\n"                   \
    "add $64,%%esp\n"                             \
    : : "r" (name) : "eax");                                \
}

#define MSS_DEF_CB_STACK_ALIGN( name ) DXDEF MSS_CB_STACK_ALIGN( name )
#define MSS_CB_STACK_ALIGN_DEC( name ) void MSS_CALLBACK_ALIGNED_NAME(name)(void);

#else

#define MSS_CB_STACK_ALIGN( name )
#define MSS_DEF_CB_STACK_ALIGN( name )
#define MSS_CB_STACK_ALIGN_DEC( name ) 

#endif

#ifdef IS_XENON

#define  XWAVE_FORMAT_XMA 0x0165

typedef MSS_STRUCT XXMASTREAMFORMAT
{
    U32   PsuedoBytesPerSec;  // Used by encoder
    U32   SampleRate;         // Sample rate for the stream.
    U32   LoopStart;          // Loop start offset (in bits).
    U32   LoopEnd;            // Loop end offset (in bits).

    // Format for SubframeData: eeee ssss.
    // e: Subframe number of loop end point [0,3].
    // s: Number of subframes to skip before decoding and outputting at the loop start point [1,4].

    U8    SubframeData;       // Data for decoding subframes.  See above.
    U8    Channels;           // Number of channels in the stream (1 or 2).
    U16   ChannelMask;        // Channel assignments for the channels in the stream (same as
                                // lower 16 bits of dwChannelMask in WAVEFORMATEXTENSIBLE).
} XXMASTREAMFORMAT;

typedef MSS_STRUCT XXMAWAVEFORMAT
{
    U16            FormatTag;     // Audio format type (always WAVE_FORMAT_XMA).
    U16            BitsPerSample; // Bit depth (currently required to be 16).
    U16            EncodeOptions; // Options for XMA encoder/decoder.
    U16            LargestSkip;   // Largest skip used in interleaving streams.
    U16            NumStreams;    // Number of interleaved audio streams.
    U8             LoopCount;     // Number of loop repetitions (255 == infinite).
    U8             Version;       // Version of the encoder that generated this.
    XXMASTREAMFORMAT XmaStreams[1]; // Format info for each stream (can grow based on wNumStreams).
} XXMAWAVEFORMAT;

#endif

extern U32 SS_granularity(HSAMPLE S);

extern S32 MC_open_output_filter(C8 const  *name, //)
                                 HDIGDRIVER driver,
                                 S32        is_matrix_filter);

extern void AILCALL MSS_mixer_mc_copy ( MSS_BB * build,
                                        S32 n_build_buffers,
                                        void * lpWaveAddr,
                                        S32 hw_format,
#ifdef IS_X86
                                        S32 use_MMX,
#endif
                                        S32 samples_per_buffer,
                                        S32 physical_channels_per_sample );

extern void AILCALL MSS_mixer_adpcm_decode( void * dest,
                                            void const * in,
                                            S32 out_len,
                                            S32 in_len,
                                            S32 input_format,
                                            ADPCMDATA *adpcm_data );
//
// Prototypes for ADPCM decode routines
//

extern void ASMLINK DecodeADPCM_STEREO( ASMPARM void       *out,
                                 ASMPARM void const *in,
                                 ASMPARM S32             out_len,
                                 ASMPARM S32             in_len,
                                 ASMPARM ADPCMDATA  *adpcm_data);

extern void ASMLINK DecodeADPCM_MONO( ASMPARM void       *out, 
                               ASMPARM void const *in,
                               ASMPARM S32             out_len,
                               ASMPARM S32             in_len,
                               ASMPARM ADPCMDATA  *adpcm_data);

extern void ASMLINK DecodeADPCM_MONO_8( ASMPARM void       *out, 
                                 ASMPARM void const *in,
                                 ASMPARM S32             out_len,
                                 ASMPARM S32             in_len,
                                 ASMPARM ADPCMDATA  *adpcm_data);


//
// .VOC file header
//

typedef MSS_STRUCT
{
   S8  ID_string[20];

   U16 data_offset;
   U16 version;
   U16 ID_code;
}
VOC;

typedef MSS_STRUCT _ADPCMOUT {
  U32 riffmark;
  U32 rifflen;
  U32 wavemark;
  U32 fmtmark;
  U32 fmtlen;
  U16 fmttag;
  U16 channels;
  U32 sampersec;
  U32 avepersec;
  U16 blockalign;
  U16 bitspersam;
  S16 extra;
  S16 samples_per_block;
  U32 factmark;
  U32 factlen;
  U32 samples;
  U32 datamark;
  U32 datalen;
} ADPCMOUT;

typedef MSS_STRUCT _WAVEOUT {
  U32 riffmark;
  U32 rifflen;
  U32 wavemark;
  U32 fmtmark;
  U32 fmtlen;
  U16 fmttag;
  U16 channels;
  U32 sampersec;
  U32 avepersec;
  U16 blockalign;
  U16 bitspersam;
  U32 datamark;
  U32 datalen;
} WAVEOUT;

typedef MSS_STRUCT _WAVEOUTEXT {
  U32 riffmark;
  U32 rifflen;
  U32 wavemark;
  U32 fmtmark;
  U32 fmtlen;
  U16 fmttag;
  U16 channels;
  U32 sampersec;
  U32 avepersec;
  U16 blockalign;
  U16 bitspersam;
  U16 size;
  U16 sampbits;
  U32 chanmask;
  U8  subfmt[16];
  U32 datamark;
  U32 datalen;
} WAVEOUTEXT;

F32 evaluate_graph(MSSGRAPHPOINT const* graph, S32 cnt, F32 x);

//
// platform allocators
//

void * AILCALLBACK platform_alloc(UINTa size);
void AILCALLBACK platform_free( void * ptr );


//
// Abstracted IO structures.
//

#ifdef IS_WII
#define MAX_PLATFILE_SPECIFIC_SIZE 512
#else
#define MAX_PLATFILE_SPECIFIC_SIZE 32
#endif
typedef struct PLATFORM_FILE
{
    char plat_specific[MAX_PLATFILE_SPECIFIC_SIZE]; // needs to be at the front for wii
    U32 offset; // current offset in to our "file"
    U32 start_pos; // offset of our "file" from the start of the physical file.
    U32 file_length; // length of our "file"
    S32 dont_close; // nonzero if we don't own the file handle, so when done, don't close.
    U32 raw_length; // length of the entire file that we might be "inside of"
} PLATFORM_FILE;

typedef struct FNVALUES
{
  char const * filename;
  U32 size;
  U32 start;
} FNVALUES;

typedef struct FHVALUES
{
  S32 hand;
  S32 length;
  S32 pos;
} FHVALUES;

//
// Functions implemented per platform.
//
extern int Platform_OpenFile(struct PLATFORM_FILE* i_File, char const * fn);
extern void Platform_SeekFromBeginning(PLATFORM_FILE* i_File, S32 i_Offset);
extern U32 Platform_ReadFile(PLATFORM_FILE* i_File, void* o_Buffer, U32 i_ReadBytes);
extern void Platform_CloseFile(PLATFORM_FILE* i_File);

DXDEC void AILCALL AIL_set_event_settings(void* i_Settings);

// Auditioner Declarations
//-----------------------------------------------------------------------------
S32     Audition_Status();
S32     Audition_Pump();
void*   Audition_OpenBank(char const* i_FileName);
S32     Audition_OpenComplete(void* i_Bank);
void    Audition_CloseBank(void* i_Bank);

void    Audition_Suppress(S32 i_IsSuppressed);
void    Audition_FrameStart();
void    Audition_FrameEnd();
void    Audition_DefragStart();
void    Audition_SetBlend(U64 i_EventId, char const* i_Name);
void    Audition_SetPersist(U64 i_EventId, char const* i_Name, char const* i_Preset);
void    Audition_Event(char const* i_EventName, U64 i_EventId, U64 i_Filter, S32 i_Exists, void* i_InitBlock, S32 i_InitBlockLen);
void    Audition_Sound(U64 i_EventId, U64 i_SoundId, char const* i_Sound, char const* i_Label, float i_Volume, S32 i_Delay, float i_Pitch);
void    Audition_SoundComplete(U64 i_SoundId);
void    Audition_SoundPlaying(U64 i_SoundId);
void    Audition_SoundFlags(U64 i_SoundId, S32 i_Flags);
void    Audition_SoundLimited(U64 i_SoundId, char const* i_Label);
void    Audition_SoundEvicted(U64 i_SoundId, U64 i_ForSound, S32 i_Reason);
void    Audition_Control(U64 i_EventId, char const* i_Labels, U8 i_ControlType, U64 i_Filter);
void    Audition_SoundBus(U64 i_SoundId, U8 i_BusIndex);

void    Audition_Error(U64 i_Id, char const* i_Details);

void    Audition_AsyncQueued(U64 i_RelevantId, S32 i_AsyncId, char const* i_Asset);
void    Audition_AsyncLoad(S32 i_AsyncId, S32 i_ExpectedData);
void    Audition_AsyncError(S32 i_AsyncId);
void    Audition_AsyncComplete(S32 i_AsyncId, S32 i_DataLoaded);
void    Audition_AsyncCancel(S32 i_AsyncId);
void    Audition_ListenerPosition(float x, float y, float z);
void    Audition_SoundPosition(U64 i_Sound, float x, float y, float z);
void    Audition_SendCPU(HDIGDRIVER i_Driver);
void    Audition_UpdateDataCount(S32 i_CurrentDataLoaded);
void    Audition_SendCount(S32 i_SoundCount);
void    Audition_HandleSystemLoad(S32 i_Avail, S32 i_Total);
void    Audition_VarState(char const* i_Var, U64 i_SoundId, S32 i_Int, void* i_4ByteValue);
void    Audition_RampState(char const* i_Ramp, U64 i_SoundId, S32 i_Type, float i_Current);
void    Audition_SoundState(U64 i_SoundId, float i_FinalVol, float i_3DVol, float i_BlendVol, float i_BlendPitch, float i_RampVol, float i_RampWet, float i_RampLp, float i_RampRate);
void    Audition_ClearState();
void    Audition_CompletionEvent(U64 i_CompletionEventId, U64 i_ParentSoundId);
void    Audition_AddRamp(U64 i_ParentSoundId, S32 i_Type, char const* i_Name, char const* i_LabelQuery, U64 i_EventId);

#if defined(IS_WIN32API) || defined(IS_WII)
  #pragma pack(pop)
#endif
#ifdef __cplusplus
}
#endif

#endif

