/*************************************************************************************
                  Silicon Laboratories Broadcast TDSY_G230D Layer 1 API
   API functions prototypes used by commands and properties
   FILE: TDSY_G230D_Commands_Prototypes.h
   Supported IC : TDSY_G230D
   Compiled for ROM 33 firmware 2_0_build_4
   Revision: 0.1
   Date: July 30 2012
  (C) Copyright 2012, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
#ifndef    TDSY_G230D_COMMANDS_PROTOTYPES_H
#define    TDSY_G230D_COMMANDS_PROTOTYPES_H

unsigned char TDSY_G230D_CurrentResponseStatus (L1_TDSY_G230D_Context *api, unsigned char ptDataBuffer);
unsigned char TDSY_G230D_pollForCTS            (L1_TDSY_G230D_Context *api);
unsigned char TDSY_G230D_pollForResponse       (L1_TDSY_G230D_Context *api, unsigned int nbBytes, unsigned char *pByteBuffer);
unsigned char TDSY_G230D_L1_SendCommand2       (L1_TDSY_G230D_Context *api, unsigned int cmd_code);
unsigned char TDSY_G230D_L1_SetProperty        (L1_TDSY_G230D_Context *api, unsigned int prop, int  data);
unsigned char TDSY_G230D_L1_GetProperty        (L1_TDSY_G230D_Context *api, unsigned int prop, int *data);
unsigned char TDSY_G230D_L1_SetProperty2       (L1_TDSY_G230D_Context *api, unsigned int prop_code);
unsigned char TDSY_G230D_L1_GetProperty2       (L1_TDSY_G230D_Context *api, unsigned int prop_code);

#ifdef    TDSY_G230D_GET_PROPERTY_STRING
unsigned char    TDSY_G230D_L1_GetPropertyString        (L1_TDSY_G230D_Context *api, unsigned int prop_code, const char *separator, char *msg);
#endif /* TDSY_G230D_GET_PROPERTY_STRING */

#ifdef    TDSY_G230D_GET_COMMAND_STRINGS
  unsigned char   TDSY_G230D_L1_GetCommandResponseString(L1_TDSY_G230D_Context *api, unsigned int cmd_code, const char *separator, char *msg);
#endif /* TDSY_G230D_GET_COMMAND_STRINGS */

/* _commands_prototypes_insertion_start */
#ifdef    TDSY_G230D_AGC_OVERRIDE_CMD
unsigned char TDSY_G230D_L1_AGC_OVERRIDE    (L1_TDSY_G230D_Context *api,
                                         unsigned char   force_max_gain,
                                         unsigned char   force_top_gain);
#endif /* TDSY_G230D_AGC_OVERRIDE_CMD */
#ifdef    TDSY_G230D_ATV_CW_TEST_CMD
unsigned char TDSY_G230D_L1_ATV_CW_TEST     (L1_TDSY_G230D_Context *api,
                                         unsigned char   pc_lock);
#endif /* TDSY_G230D_ATV_CW_TEST_CMD */
#ifdef    TDSY_G230D_ATV_RESTART_CMD
unsigned char TDSY_G230D_L1_ATV_RESTART     (L1_TDSY_G230D_Context *api);
#endif /* TDSY_G230D_ATV_RESTART_CMD */
#ifdef    TDSY_G230D_ATV_STATUS_CMD
unsigned char TDSY_G230D_L1_ATV_STATUS      (L1_TDSY_G230D_Context *api,
                                         unsigned char   intack);
#endif /* TDSY_G230D_ATV_STATUS_CMD */
#ifdef    TDSY_G230D_CONFIG_CLOCKS_CMD
unsigned char TDSY_G230D_L1_CONFIG_CLOCKS   (L1_TDSY_G230D_Context *api,
                                         unsigned char   subcode,
                                         unsigned char   clock_mode,
                                         unsigned char   en_xout);
#endif /* TDSY_G230D_CONFIG_CLOCKS_CMD */
#ifdef    TDSY_G230D_CONFIG_PINS_CMD
unsigned char TDSY_G230D_L1_CONFIG_PINS     (L1_TDSY_G230D_Context *api,
                                         unsigned char   gpio1_mode,
                                         unsigned char   gpio1_read,
                                         unsigned char   gpio2_mode,
                                         unsigned char   gpio2_read,
                                         unsigned char   reserved1,
                                         unsigned char   reserved2,
                                         unsigned char   reserved3);
#endif /* TDSY_G230D_CONFIG_PINS_CMD */
#ifdef    TDSY_G230D_DTV_RESTART_CMD
unsigned char TDSY_G230D_L1_DTV_RESTART     (L1_TDSY_G230D_Context *api);
#endif /* TDSY_G230D_DTV_RESTART_CMD */
#ifdef    TDSY_G230D_DTV_STATUS_CMD
unsigned char TDSY_G230D_L1_DTV_STATUS      (L1_TDSY_G230D_Context *api,
                                         unsigned char   intack);
#endif /* TDSY_G230D_DTV_STATUS_CMD */
#ifdef    TDSY_G230D_EXIT_BOOTLOADER_CMD
unsigned char TDSY_G230D_L1_EXIT_BOOTLOADER (L1_TDSY_G230D_Context *api,
                                         unsigned char   func,
                                         unsigned char   ctsien);
#endif /* TDSY_G230D_EXIT_BOOTLOADER_CMD */
#ifdef    TDSY_G230D_FINE_TUNE_CMD
unsigned char TDSY_G230D_L1_FINE_TUNE       (L1_TDSY_G230D_Context *api,
                                         unsigned char   persistence,
                                         unsigned char   apply_to_lif,
                                                   int   offset_500hz);
#endif /* TDSY_G230D_FINE_TUNE_CMD */
#ifdef    TDSY_G230D_GET_PROPERTY_CMD
unsigned char TDSY_G230D_L1_GET_PROPERTY    (L1_TDSY_G230D_Context *api,
                                         unsigned char   reserved,
                                         unsigned int    prop);
#endif /* TDSY_G230D_GET_PROPERTY_CMD */
#ifdef    TDSY_G230D_GET_REV_CMD
unsigned char TDSY_G230D_L1_GET_REV         (L1_TDSY_G230D_Context *api);
#endif /* TDSY_G230D_GET_REV_CMD */
#ifdef    TDSY_G230D_PART_INFO_CMD
unsigned char TDSY_G230D_L1_PART_INFO       (L1_TDSY_G230D_Context *api);
#endif /* TDSY_G230D_PART_INFO_CMD */
#ifdef    TDSY_G230D_POWER_DOWN_CMD
unsigned char TDSY_G230D_L1_POWER_DOWN      (L1_TDSY_G230D_Context *api);
#endif /* TDSY_G230D_POWER_DOWN_CMD */
#ifdef    TDSY_G230D_POWER_DOWN_HW_CMD
unsigned char TDSY_G230D_L1_POWER_DOWN_HW   (L1_TDSY_G230D_Context *api,
                                         unsigned char   subcode,
                                         unsigned char   pd_xo_osc,
                                         unsigned char   reserved1,
                                         unsigned char   en_xout,
                                         unsigned char   reserved2,
                                         unsigned char   pd_ldo,
                                         unsigned char   reserved3,
                                         unsigned char   reserved4,
                                         unsigned char   reserved5,
                                         unsigned char   reserved6,
                                         unsigned char   reserved7,
                                         unsigned char   reserved8);
#endif /* TDSY_G230D_POWER_DOWN_HW_CMD */
#ifdef    TDSY_G230D_POWER_UP_CMD
unsigned char TDSY_G230D_L1_POWER_UP        (L1_TDSY_G230D_Context *api,
                                         unsigned char   subcode,
                                         unsigned char   clock_mode,
                                         unsigned char   en_xout,
                                         unsigned char   pd_ldo,
                                         unsigned char   reserved2,
                                         unsigned char   reserved3,
                                         unsigned char   reserved4,
                                         unsigned char   reserved5,
                                         unsigned char   reserved6,
                                         unsigned char   reserved7,
                                         unsigned char   reset,
                                         unsigned char   clock_freq,
                                         unsigned char   reserved8,
                                         unsigned char   func,
                                         unsigned char   ctsien,
                                         unsigned char   wake_up);
#endif /* TDSY_G230D_POWER_UP_CMD */
#ifdef    TDSY_G230D_RAM_CRC_CMD
unsigned char TDSY_G230D_L1_RAM_CRC         (L1_TDSY_G230D_Context *api);
#endif /* TDSY_G230D_RAM_CRC_CMD */
#ifdef    TDSY_G230D_SET_PROPERTY_CMD
unsigned char TDSY_G230D_L1_SET_PROPERTY    (L1_TDSY_G230D_Context *api,
                                         unsigned char   reserved,
                                         unsigned int    prop,
                                         unsigned int    data);
#endif /* TDSY_G230D_SET_PROPERTY_CMD */
#ifdef    TDSY_G230D_STANDBY_CMD
unsigned char TDSY_G230D_L1_STANDBY         (L1_TDSY_G230D_Context *api,
                                         unsigned char   type);
#endif /* TDSY_G230D_STANDBY_CMD */
#ifdef    TDSY_G230D_TUNER_STATUS_CMD
unsigned char TDSY_G230D_L1_TUNER_STATUS    (L1_TDSY_G230D_Context *api,
                                         unsigned char   intack);
#endif /* TDSY_G230D_TUNER_STATUS_CMD */
#ifdef    TDSY_G230D_TUNER_TUNE_FREQ_CMD
unsigned char TDSY_G230D_L1_TUNER_TUNE_FREQ (L1_TDSY_G230D_Context *api,
                                         unsigned char   mode,
                                         unsigned long   freq);
#endif /* TDSY_G230D_TUNER_TUNE_FREQ_CMD */
/* _commands_prototypes_insertion_point */

#endif /* TDSY_G230D_COMMANDS_PROTOTYPES_H */











