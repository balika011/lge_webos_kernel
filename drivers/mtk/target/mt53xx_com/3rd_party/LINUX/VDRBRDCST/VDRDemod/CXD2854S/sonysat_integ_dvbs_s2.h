/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_integ_dvbs_s2.h

*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_INTEG_DVBS_S2_H
#define SONYSAT_INTEG_DVBS_S2_H

#include "sonysat_common.h"
#include "sonysat_tuner_sat.h"
#include "sonysat_demod.h"
#include "sonysat_demod_dvbs_s2.h"
#include "sonysat_demod_dvbs_s2_blindscan_seq.h"
#include "sonysat_demod_dvbs_s2_tune_srs.h"
#include "sonysat_demod_dvbs_s2_tune_srs_seq.h"
#include "sonysat_integ.h"

/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/
/**
 @brief Polling interval for Tune sequence in ms.
*/
#define SONYSAT_INTEG_DVBS_S2_TUNE_POLLING_INTERVAL        10

/**
 @brief Polling interval for BlindScan sequence in ms.
*/
#define SONYSAT_INTEG_DVBS_S2_BLINDSCAN_POLLING_INTERVAL   10

/**
 @brief Polling interval for TuneSRS sequence in ms.
*/
#define SONYSAT_INTEG_DVBS_S2_TUNE_SRS_POLLING_INTERVAL    10

/*------------------------------------------------------------------------------
 Enums
------------------------------------------------------------------------------*/
/**
 @brief Definition of event id for callback from BlindScan.
*/
typedef enum {
    SONYSAT_INTEG_DVBS_S2_BLINDSCAN_EVENT_DETECT,    /**< Detect channel. */
    SONYSAT_INTEG_DVBS_S2_BLINDSCAN_EVENT_PROGRESS   /**< Update progress. */
} sonysat_integ_dvbs_s2_blindscan_event_id_t;

/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief The TuneSRS functions parameters for a DVB-S/S2 signal.
*/
typedef struct {
    /**
     @brief Center frequency in KHz of the DVB-S/S2 channel.
    */
    uint32_t centerFreqKHz;
    /**
     @brief Frequency range found by TuneSRS.
    */
    sonysat_demod_dvbs_s2_tune_srs_freq_range_t freqRange;
} sonysat_integ_dvbs_s2_tune_srs_param_t;

/**
 @brief Definition of the parameters for BlindScan.
*/
typedef struct {
    /**
     @brief Min frequency of scan range in KHz.
    
            Please set the min frequency of channel which you would like to detect in KHz.
            (i.e.  950000)
    */
    uint32_t minFreqKHz;
    /**
     @brief Max frequency of scan range.
    
            Please set the min frequency of channel which you would like to detect in KHz.
            (i.e. 2150000)
    */
    uint32_t maxFreqKHz;
    /**
     @brief Min symbol rate of scan range.
     
            Please set the min symbol rate of channel which you would like to detect in KSps.
            (i.e.    1000)
    */
    uint32_t minSymbolRateKSps;
    /**
     @brief Max symbol rate of scan range.
     
            Please set the max symbol rate of channel which you would like to detect in KSps.
            (i.e.   45000)
    */
    uint32_t maxSymbolRateKSps;  

} sonysat_integ_dvbs_s2_blindscan_param_t;

/**
 @brief Definition of the result of BlindScan.
*/
typedef struct {
    /**
     @brief The cause of callback.

            When callback function was called, please check it first.
    */
    sonysat_integ_dvbs_s2_blindscan_event_id_t eventId;
    /**
     @brief Detected channel information.

            If "eventId == SONYSAT_INTEG_DVBS_S2_BLINDSCAN_EVENT_DETECT", this value is valid.
    */
    sonysat_dvbs_s2_tune_param_t tuneParam;
    /**
     @brief Progress.

            If "eventId == SONYSAT_INTEG_DVBS_S2_BLINDSCAN_EVENT_PROGRESS", this value is valid.
            The range of this value is 0 - 100 in percentage.
    */
    uint8_t progress;
}sonysat_integ_dvbs_s2_blindscan_result_t;

/*------------------------------------------------------------------------------
 Function Pointers
------------------------------------------------------------------------------*/
/**
 @brief Callback function that is called for every attempted frequency during a DVB-S/S2
        scan.  For successful channel results the function is called before TS lock
        is achieved.
 
 @param pInteg The driver instance.
 @param pResult The current scan result.
*/
typedef void (*sonysat_integ_dvbs_s2_blindscan_callback_t) (sonysat_integ_t * pInteg,
                                                         sonysat_integ_dvbs_s2_blindscan_result_t * pResult);

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/

/**
 @brief Performs acquisition to the channel specified by the pTuneParam struct.
        If the channel system is unknown set the pTuneParam.system to 
        ::SONYSAT_DTV_SYSTEM_ANY to enable a blind acquisition.
        In this case the tuned system can be obtained by calling
        ::sonysat_demod_dvbs_s2_monitor_System.

        Blocks the calling thread until the TS has locked or has timed out.
        Use ::sonysat_integ_Cancel to cancel the operation at any time.

        The frequency capture range depends on the target symbol rate and it
        is automatically handled by hardware.
        - Symbol Rate >= 20MSps : +/- 10MHz
        - Symbol Rate <  20MSps : Half of Symbol Rate.(i.e. 5MSps : +/- 2.5MHz)

        However, if Symbol Rate is less than 20 MSps and signal condition is good, 
        then frequency capture range is "equal to Symbol Rate". (i.e. 5MSps : +/- 5MHz)

 @param pInteg The driver instance.
 @param pTuneParam The parameters required for the tune.

 @return SONYSAT_RESULT_OK if tuned successfully to the channel.
*/
sonysat_result_t sonysat_integ_dvbs_s2_Tune (sonysat_integ_t * pInteg,
                                       sonysat_dvbs_s2_tune_param_t * pTuneParam);

/**
 @brief Tune SRS (Symbol Rate Search) performs a blind acquisition to the
        channel with wider range than ::sonysat_integ_dvbs_s2_Tune(). It captures 
        an available signal having Symbol Rate of 1 to 45 MSps and centred 
        within  +/- 10 MHz of the RF frequency specified by the pParam struct. 
        This enables acquisition to a DVB-S or DVB-S2 channel where the center 
        frequency or symbol rate is not precisely known.

        The located channel tune parameters are then written into the pTuneParam
        structure, which can subsequently be passed to ::sonysat_integ_dvbs_s2_Tune
        for quicker acquisition.

        Blocks the calling thread until the TS has locked or has timed out.
        Use ::sonysat_integ_Cancel to cancel the operation at any time.

 @param pInteg The driver instance.
 @param pSeq The sequence instance.
 @param pParam Parameter for TuneSRS.
 @param pTuneParam The detected channel information.

 @return SONYSAT_RESULT_OK if tuned successfully to the channel and pTuneParam is valid.
*/
sonysat_result_t sonysat_integ_dvbs_s2_TuneSRS (sonysat_integ_t * pInteg,
                                          sonysat_demod_dvbs_s2_tune_srs_seq_t * pSeq,
                                          sonysat_integ_dvbs_s2_tune_srs_param_t * pParam,
                                          sonysat_dvbs_s2_tune_param_t * pTuneParam);

/**
 @brief BlindScan searches channels within the frequency and symbol rate range set 
        by pParam. The whole process is carried out in three steps. In the first step, 
        it finds channels having Symbol Rate more than 20 MSps. In the second step, it 
        finds channels having Symbol Rate between 5 to 20 MSps. In the third step, it 
        finds channels having Symbol Rate between 1 to 5 MSps. 

        Callback function is called during each step at the following points.
          - Detected channel
          - Update progress
        The source of callback is determined by checking sonysat_integ_dvbs_s2_blindscan_result_t::eventId.
        Please see explanation in ::sonysat_integ_dvbs_s2_blindscan_result_t structure.

        Blocks the calling thread until the BlindScan has finished.
        Use ::sonysat_integ_Cancel to cancel the operation at any time.

 @param pInteg The integration part instance.
 @param pSeq The sequence instance.
 @param pParam Scan parameters.
 @param callback Callback function pointer.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_integ_dvbs_s2_BlindScan (sonysat_integ_t * pInteg,
                                            sonysat_demod_dvbs_s2_blindscan_seq_t * pSeq,
                                            sonysat_integ_dvbs_s2_blindscan_param_t * pParam,
                                            sonysat_integ_dvbs_s2_blindscan_callback_t callback);

/**
 @brief This function returns the estimated RF level based on either the demodulator IFAGC 
        level or a tuner internal RSSI monitor.  If any compensation for external hardware 
        such as, LNA, attenuators is required, then the user should make adjustment in the 
        corresponding function.

 @param pInteg The driver instance
 @param pRFLeveldB The RF Level estimation in dB * 1000

 @return SONYSAT_RESULT_OK if successful and pRFLeveldB is valid.
*/
sonysat_result_t sonysat_integ_dvbs_s2_monitor_RFLevel (sonysat_integ_t * pInteg,
                                                  int32_t * pRFLeveldB);

#endif /* SONYSAT_INTEG_DVBS_S2_H */
