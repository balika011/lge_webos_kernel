/*************************************************************************************
                  Silicon Laboratories Broadcast Si2151 Layer 1 API

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.

   API properties definitions
   FILE: Si2151_L1_Properties.c
   Supported IC : Si2151
   Compiled for ROM 60 firmware 0_A_build_15
   Revision: 0.1
   Tag:  ROM60_0_A_build_15_V0.1
   Date: March 17 2014
  (C) Copyright 2014, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
#define   Si2151_COMMAND_PROTOTYPES

#include "Si2151_L1_API.h"
#include "tunerDebug.h"

/***********************************************************************************************************************
  Si2151_storeUserProperties function
  Use:        property preparation function
              Used to fill the prop structure with user values.
  Parameter: *prop    a property structure to be filled

  Returns:    void
 ***********************************************************************************************************************/
void          Si2151_storeUserProperties    (Si2151_PropObj   *prop) {
  mcDBG_MSG_L2(("Si2151_storeUserProperties\n"));
#ifdef    Si2151_ATV_AGC_SPEED_PROP
  prop->atv_agc_speed.if_agc_speed                = Si2151_ATV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO ; /* (default 'AUTO') */
#endif /* Si2151_ATV_AGC_SPEED_PROP */

#ifdef    Si2151_ATV_AGC_SPEED_LOW_RSSI_PROP
  prop->atv_agc_speed_low_rssi.if_agc_speed       = Si2151_ATV_AGC_SPEED_LOW_RSSI_PROP_IF_AGC_SPEED_158 ; /* (default '158') */
  prop->atv_agc_speed_low_rssi.thld               =  -128; /* (default  -128) */
#endif /* Si2151_ATV_AGC_SPEED_LOW_RSSI_PROP */

#ifdef    Si2151_ATV_ARTIFICIAL_SNOW_PROP
  prop->atv_artificial_snow.gain                  =    32; /* (default    32) */
  prop->atv_artificial_snow.period                = Si2151_ATV_ARTIFICIAL_SNOW_PROP_PERIOD_SHORT ; /* (default 'SHORT') */
#endif /* Si2151_ATV_ARTIFICIAL_SNOW_PROP */

#ifdef    Si2151_ATV_CONFIG_IF_PORT_PROP
  prop->atv_config_if_port.atv_out_type           = Si2151_ATV_CONFIG_IF_PORT_PROP_ATV_OUT_TYPE_LIF_DIFF_IF ; /* (default 'LIF_DIFF_IF') */
#endif /* Si2151_ATV_CONFIG_IF_PORT_PROP */

#ifdef    Si2151_ATV_EXT_AGC_PROP
  prop->atv_ext_agc.min_10mv                      =    50; /* (default    50) */
  prop->atv_ext_agc.max_10mv                      =   200; /* (default   200) */
#endif /* Si2151_ATV_EXT_AGC_PROP */

#ifdef    Si2151_ATV_HSYNC_OUT_PROP
  prop->atv_hsync_out.polarity                    = Si2151_ATV_HSYNC_OUT_PROP_POLARITY_LOW ; /* (default 'LOW') */
  prop->atv_hsync_out.width                       =    75; /* (default    75) */
  prop->atv_hsync_out.offset                      =     0; /* (default     0) */
#endif /* Si2151_ATV_HSYNC_OUT_PROP */

#ifdef    Si2151_ATV_LIF_FREQ_PROP
  prop->atv_lif_freq.offset                       =  5000; /* (default  5000) */
#endif /* Si2151_ATV_LIF_FREQ_PROP */

#ifdef    Si2151_ATV_LIF_OUT_PROP
  prop->atv_lif_out.offset                        =   148; /* (default   148) */
  prop->atv_lif_out.amp                           =   100; /* (default   100) */
#endif /* Si2151_ATV_LIF_OUT_PROP */

#ifdef    Si2151_ATV_RF_TOP_PROP
  prop->atv_rf_top.atv_rf_top                     = Si2151_ATV_RF_TOP_PROP_ATV_RF_TOP_AUTO ; /* (default 'AUTO') */
#endif /* Si2151_ATV_RF_TOP_PROP */

#ifdef    Si2151_ATV_RSQ_RSSI_THRESHOLD_PROP
  prop->atv_rsq_rssi_threshold.lo                 =   -70; /* (default   -70) */
#endif /* Si2151_ATV_RSQ_RSSI_THRESHOLD_PROP */

#ifdef    Si2151_ATV_VIDEO_MODE_PROP
  prop->atv_video_mode.video_sys                  = Si2151_ATV_VIDEO_MODE_PROP_VIDEO_SYS_GH              ; /* (default 'B') */
  prop->atv_video_mode.invert_spectrum            = Si2151_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_NORMAL ; /* (default 'INVERTED') */
#endif /* Si2151_ATV_VIDEO_MODE_PROP */

#ifdef    Si2151_ATV_VSNR_CAP_PROP
  prop->atv_vsnr_cap.atv_vsnr_cap                 =     0; /* (default     0) */
#endif /* Si2151_ATV_VSNR_CAP_PROP */

#ifdef    Si2151_CRYSTAL_TRIM_PROP
  prop->crystal_trim.xo_cap                       = Si2151_CRYSTAL_TRIM_PROP_XO_CAP_6P7PF ; /* (default '6p7pF') */
#endif /* Si2151_CRYSTAL_TRIM_PROP */

#ifdef    Si2151_XOUT_PROP
  prop->xout.amp                                  = Si2151_XOUT_PROP_AMP_HIGH ; /* (default 'HIGH') */
#endif /* Si2151_XOUT_PROP */

#ifdef    Si2151_DTV_AGC_AUTO_FREEZE_PROP
  prop->dtv_agc_auto_freeze.thld                  =   255; /* (default   255) */
  prop->dtv_agc_auto_freeze.timeout               =     1; /* (default     1) */
#endif /* Si2151_DTV_AGC_AUTO_FREEZE_PROP */

#ifdef    Si2151_DTV_AGC_SPEED_PROP
  prop->dtv_agc_speed.if_agc_speed                = Si2151_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO ; /* (default 'AUTO') */
  prop->dtv_agc_speed.agc_decim                   = Si2151_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF     ; /* (default 'OFF') */
#endif /* Si2151_DTV_AGC_SPEED_PROP */

#ifdef    Si2151_DTV_CONFIG_IF_PORT_PROP
  prop->dtv_config_if_port.dtv_out_type           = Si2151_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF ; /* (default 'LIF_IF') */
#endif /* Si2151_DTV_CONFIG_IF_PORT_PROP */

#ifdef    Si2151_DTV_EXT_AGC_PROP
  prop->dtv_ext_agc.min_10mv                      =    50; /* (default    50) */
  prop->dtv_ext_agc.max_10mv                      =   200; /* (default   200) */
#endif /* Si2151_DTV_EXT_AGC_PROP */

#ifdef    Si2151_DTV_INITIAL_AGC_SPEED_PROP
  prop->dtv_initial_agc_speed.if_agc_speed        = Si2151_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO ; /* (default 'AUTO') */
  prop->dtv_initial_agc_speed.agc_decim           = Si2151_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF     ; /* (default 'OFF') */
#endif /* Si2151_DTV_INITIAL_AGC_SPEED_PROP */

#ifdef    Si2151_DTV_INITIAL_AGC_SPEED_PERIOD_PROP
  prop->dtv_initial_agc_speed_period.period       =     0; /* (default     0) */
#endif /* Si2151_DTV_INITIAL_AGC_SPEED_PERIOD_PROP */

#ifdef    Si2151_DTV_INTERNAL_ZIF_PROP
  prop->dtv_internal_zif.atsc                     = Si2151_DTV_INTERNAL_ZIF_PROP_ATSC_ZIF   ; /* (default 'ZIF') */
  prop->dtv_internal_zif.qam_us                   = Si2151_DTV_INTERNAL_ZIF_PROP_QAM_US_ZIF ; /* (default 'ZIF') */
  prop->dtv_internal_zif.dvbt                     = Si2151_DTV_INTERNAL_ZIF_PROP_DVBT_ZIF   ; /* (default 'ZIF') */
  prop->dtv_internal_zif.dvbc                     = Si2151_DTV_INTERNAL_ZIF_PROP_DVBC_ZIF   ; /* (default 'ZIF') */
  prop->dtv_internal_zif.isdbt                    = Si2151_DTV_INTERNAL_ZIF_PROP_ISDBT_ZIF  ; /* (default 'ZIF') */
  prop->dtv_internal_zif.isdbc                    = Si2151_DTV_INTERNAL_ZIF_PROP_ISDBC_ZIF  ; /* (default 'ZIF') */
  prop->dtv_internal_zif.dtmb                     = Si2151_DTV_INTERNAL_ZIF_PROP_DTMB_LIF   ; /* (default 'LIF') */
#endif /* Si2151_DTV_INTERNAL_ZIF_PROP */

#ifdef    Si2151_DTV_LIF_FREQ_PROP
  prop->dtv_lif_freq.offset                       =  5000; /* (default  5000) */
#endif /* Si2151_DTV_LIF_FREQ_PROP */

#ifdef    Si2151_DTV_LIF_OUT_PROP
  prop->dtv_lif_out.offset                        =   148; /* (default   148) */
  prop->dtv_lif_out.amp                           =    27; /* (default    27) */
#endif /* Si2151_DTV_LIF_OUT_PROP */

#ifdef    Si2151_DTV_MODE_PROP
  prop->dtv_mode.bw                               = Si2151_DTV_MODE_PROP_BW_BW_8MHZ              ; /* (default 'BW_8MHZ') */
  prop->dtv_mode.modulation                       = Si2151_DTV_MODE_PROP_MODULATION_DVBT         ; /* (default 'DVBT') */
  prop->dtv_mode.invert_spectrum                  = Si2151_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL  ; /* (default 'NORMAL') */
#endif /* Si2151_DTV_MODE_PROP */

#ifdef    Si2151_DTV_PGA_LIMITS_PROP
  prop->dtv_pga_limits.min                        =    -1; /* (default    -1) */
  prop->dtv_pga_limits.max                        =    -1; /* (default    -1) */
#endif /* Si2151_DTV_PGA_LIMITS_PROP */

#ifdef    Si2151_DTV_RF_AGC_SPEED_PROP
  prop->dtv_rf_agc_speed.attack                   =     0; /* (default     0) */
  prop->dtv_rf_agc_speed.decay                    =     0; /* (default     0) */
#endif /* Si2151_DTV_RF_AGC_SPEED_PROP */

#ifdef    Si2151_DTV_RF_TOP_PROP
  prop->dtv_rf_top.dtv_rf_top                     = Si2151_DTV_RF_TOP_PROP_DTV_RF_TOP_AUTO ; /* (default 'AUTO') */
#endif /* Si2151_DTV_RF_TOP_PROP */

#ifdef    Si2151_DTV_RSQ_RSSI_THRESHOLD_PROP
  prop->dtv_rsq_rssi_threshold.lo                 =   -80; /* (default   -80) */
#endif /* Si2151_DTV_RSQ_RSSI_THRESHOLD_PROP */

#ifdef    Si2151_TUNER_BLOCKED_VCO_PROP
  prop->tuner_blocked_vco.vco_code                = 0x8000; /* (default 0x8000) */
#endif /* Si2151_TUNER_BLOCKED_VCO_PROP */

#ifdef    Si2151_TUNER_BLOCKED_VCO2_PROP
  prop->tuner_blocked_vco2.vco_code               = 0x8000; /* (default 0x8000) */
#endif /* Si2151_TUNER_BLOCKED_VCO2_PROP */

#ifdef    Si2151_TUNER_BLOCKED_VCO3_PROP
  prop->tuner_blocked_vco3.vco_code               = 0x8000; /* (default 0x8000) */
#endif /* Si2151_TUNER_BLOCKED_VCO3_PROP */

#ifdef    Si2151_TUNER_BLOCKED_VCO_CONFIG_PROP
  prop->tuner_blocked_vco_config.distance_target  =   100; /* (default   100) */
#endif /* Si2151_TUNER_BLOCKED_VCO_CONFIG_PROP */

#ifdef    Si2151_TUNER_LO_INJECTION_PROP
  prop->tuner_lo_injection.band_1                 = Si2151_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE ; /* (default 'HIGH_SIDE') */
  prop->tuner_lo_injection.band_2                 = Si2151_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE  ; /* (default 'LOW_SIDE') */
  prop->tuner_lo_injection.band_3                 = Si2151_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE  ; /* (default 'LOW_SIDE') */
#endif /* Si2151_TUNER_LO_INJECTION_PROP */

#ifdef    Si2151_TUNER_RETURN_LOSS_PROP
  prop->tuner_return_loss.config                  = Si2151_TUNER_RETURN_LOSS_PROP_CONFIG_127         ; /* (default '127') */
  prop->tuner_return_loss.mode                    = Si2151_TUNER_RETURN_LOSS_PROP_MODE_TERRESTRIAL   ; /* (default 'TERRESTRIAL') */
#endif /* Si2151_TUNER_RETURN_LOSS_PROP */

}
 /************************************************************************************************************************
  Si2151_storeUserConfigPins function
  Parameter:  *api     the Si2151 context
  Returns:    NO_Si2151_ERROR if successful
************************************************************************************************************************/
int           Si2151_storeUserConfigPins           (L1_Si2151_Context *api) {
    int return_code = 0;
	mcDBG_MSG_L2(("Si2151_storeUserConfigPins\n"));
#ifdef Si2151_ATV_DTV_AGC_EXTERNAL_CONTROL_AGC1_PIN
    if((return_code = Si2151_L1_CONFIG_PINS (api,
		                                     Si2151_CONFIG_PINS_CMD_GPIO1_MODE_NO_CHANGE,
		                                     Si2151_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_GPIO2_MODE_NO_CHANGE,
		                                     Si2151_CONFIG_PINS_CMD_GPIO2_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_AGC1_MODE_ATV_DTV_AGC,
		                                     Si2151_CONFIG_PINS_CMD_AGC1_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_AGC2_MODE_NO_CHANGE,
		                                     Si2151_CONFIG_PINS_CMD_AGC2_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_XOUT_MODE_NO_CHANGE)) != NO_Si2151_ERROR)
	{
		mcDBG_MSG_ERR(("Si2151_L1_CONFIG_PINS, ATV_DTV_AGC1, error 0x%02x: %s\n", return_code, Si2151_L1_API_ERROR_TEXT(return_code)));
		return return_code;
	}
#endif /* Si2151_ATV_DTV_AGC_EXTERNAL_CONTROL_AGC1_PIN */
#ifdef Si2151_ATV_DTV_AGC_EXTERNAL_CONTROL_AGC2_PIN
    if((return_code = Si2151_L1_CONFIG_PINS (api,
		                                     Si2151_CONFIG_PINS_CMD_GPIO1_MODE_NO_CHANGE,
		                                     Si2151_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_GPIO2_MODE_NO_CHANGE,
		                                     Si2151_CONFIG_PINS_CMD_GPIO2_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_AGC1_MODE_NO_CHANGE,
		                                     Si2151_CONFIG_PINS_CMD_AGC1_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_AGC2_MODE_ATV_DTV_AGC,
		                                     Si2151_CONFIG_PINS_CMD_AGC2_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_XOUT_MODE_NO_CHANGE)) != NO_Si2151_ERROR)
	{
		mcDBG_MSG_ERR(("Si2151_L1_CONFIG_PINS, ATV_DTV_AGC2, error 0x%02x: %s\n", return_code, Si2151_L1_API_ERROR_TEXT(return_code)));
		return return_code;
	}
#endif /* Si2151_ATV_DTV_AGC_EXTERNAL_CONTROL_AGC2_PIN */
#ifdef Si2151_ATV_AGC_EXTERNAL_CONTROL_AGC1_PIN
    if((return_code = Si2151_L1_CONFIG_PINS (api,
		                                     Si2151_CONFIG_PINS_CMD_GPIO1_MODE_NO_CHANGE,
		                                     Si2151_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_GPIO2_MODE_NO_CHANGE,
		                                     Si2151_CONFIG_PINS_CMD_GPIO2_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_AGC1_MODE_ATV_AGC,
		                                     Si2151_CONFIG_PINS_CMD_AGC1_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_AGC2_MODE_NO_CHANGE,
		                                     Si2151_CONFIG_PINS_CMD_AGC2_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_XOUT_MODE_NO_CHANGE)) != NO_Si2151_ERROR)
	{
		mcDBG_MSG_ERR(("Si2151_L1_CONFIG_PINS, ATV_AGC1, error 0x%02x: %s\n", return_code, Si2151_L1_API_ERROR_TEXT(return_code)));
		return return_code;
	}
#endif /* Si2151_ATV_AGC_EXTERNAL_CONTROL_AGC1_PIN */
#ifdef Si2151_ATV_AGC_EXTERNAL_CONTROL_AGC2_PIN
    if((return_code = Si2151_L1_CONFIG_PINS (api,
		                                     Si2151_CONFIG_PINS_CMD_GPIO1_MODE_NO_CHANGE,
		                                     Si2151_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_GPIO2_MODE_NO_CHANGE,
		                                     Si2151_CONFIG_PINS_CMD_GPIO2_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_AGC1_MODE_NO_CHANGE,
		                                     Si2151_CONFIG_PINS_CMD_AGC1_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_AGC2_MODE_ATV_AGC,
		                                     Si2151_CONFIG_PINS_CMD_AGC2_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_XOUT_MODE_NO_CHANGE)) != NO_Si2151_ERROR)
	{
		mcDBG_MSG_ERR(("Si2151_L1_CONFIG_PINS, ATV_AGC2, error 0x%02x: %s\n", return_code, Si2151_L1_API_ERROR_TEXT(return_code)));
		return return_code;
	}
#endif /* Si2151_ATV_AGC_EXTERNAL_CONTROL_AGC2_PIN */
#ifdef Si2151_DTV_AGC_EXTERNAL_CONTROL_AGC1_PIN
    if((return_code = Si2151_L1_CONFIG_PINS (api,
		                                     Si2151_CONFIG_PINS_CMD_GPIO1_MODE_NO_CHANGE,
		                                     Si2151_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_GPIO2_MODE_NO_CHANGE,
		                                     Si2151_CONFIG_PINS_CMD_GPIO2_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_AGC1_MODE_DTV_AGC,
		                                     Si2151_CONFIG_PINS_CMD_AGC1_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_AGC2_MODE_NO_CHANGE,
		                                     Si2151_CONFIG_PINS_CMD_AGC2_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_XOUT_MODE_NO_CHANGE)) != NO_Si2151_ERROR)
	{
		mcDBG_MSG_ERR(("Si2151_L1_CONFIG_PINS, DTV_AGC1, error 0x%02x: %s\n", return_code, Si2151_L1_API_ERROR_TEXT(return_code)));
		return return_code;
	}
#endif /* Si2151_DTV_AGC_EXTERNAL_CONTROL_AGC1_PIN */
#ifdef Si2151_DTV_AGC_EXTERNAL_CONTROL_AGC2_PIN
    if((return_code = Si2151_L1_CONFIG_PINS (api,
		                                     Si2151_CONFIG_PINS_CMD_GPIO1_MODE_NO_CHANGE,
		                                     Si2151_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_GPIO2_MODE_NO_CHANGE,
		                                     Si2151_CONFIG_PINS_CMD_GPIO2_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_AGC1_MODE_NO_CHANGE,
		                                     Si2151_CONFIG_PINS_CMD_AGC1_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_AGC2_MODE_DTV_AGC,
		                                     Si2151_CONFIG_PINS_CMD_AGC2_READ_DO_NOT_READ,
		                                     Si2151_CONFIG_PINS_CMD_XOUT_MODE_NO_CHANGE)) != NO_Si2151_ERROR)
	{
		mcDBG_MSG_ERR(("Si2151_L1_CONFIG_PINS, DTV_AGC2, error 0x%02x: %s\n", return_code, Si2151_L1_API_ERROR_TEXT(return_code)));
		return return_code;
	}
#endif /* Si2151_DTV_AGC_EXTERNAL_CONTROL_AGC2_PIN */
	return NO_Si2151_ERROR;
}
