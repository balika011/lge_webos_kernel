/*------------------------------------------------------------------------------

 Copyright 2011-2012 Sony Corporation

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
 No part of this file may be copied, modified, sold, and distributed in any
 form or by any means without prior explicit permission in writing from
 Sony Corporation.

 Date: 2011/07/24
 Revision: 1.0.1.0

------------------------------------------------------------------------------*/
#ifndef SONY_CN_DTV_DEMOD_MONITORDTMB_H
#define SONY_CN_DTV_DEMOD_MONITORDTMB_H

#include "sony_cn_dtv_demod.h"

/*------------------------------------------------------------------------------
  Enums
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------
  Monitor chip info

  pDemod  : Address of demodulator instance.
  pChipId : Address of chip id. (8'h29=CXD2840x, 8'h12=CXD2830x)
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_ChipID(sony_cn_dtv_demod_t* pDemod,
                                                   uint8_t* pChipId);

/*--------------------------------------------------------------------
  Monitor sync status

  pDemod   : Address of demodulator instance.
  pDmdLock : Address of demodulator lock flag. (0:UNLOCK, 1:LOCK)
  pTsLock  : Address of TS lock flag. (0:UNLOCK, 1:LOCK)
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_SyncStat(sony_cn_dtv_demod_t* pDemod,
                                                     uint8_t* pDmdLock,
                                                     uint8_t* pTsLock,
                                                     uint8_t* pUnlock);

/*--------------------------------------------------------------------
  Monitor ifagc output level

  pDemod : Address of demodulator instance.
  pIfAgc : Address of AGC ouput level.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_IfAgcout(sony_cn_dtv_demod_t* pDemod,
                                                     uint32_t* pIfAgc);

/*--------------------------------------------------------------------
  Monitor carrier mode

  pDemod       : Address of demodulator instance.
  pCarrierMode : Address of carrier mode.(C1/C3780)
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_CarrierMode(sony_cn_dtv_demod_t* pDemod,
                                                        sony_cn_dtv_dtmb_carrier_mode_t* pCarrierMode);

/*--------------------------------------------------------------------
  Monitor pseudo noise information

  pDemod  : Address of demodulator instance.
  pPnInfo : Address of pseudo noise information.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PNInfo(sony_cn_dtv_demod_t* pDemod,
                                                   sony_cn_dtv_dtmb_pseudo_noise_t* pPnInfo);

/*--------------------------------------------------------------------
  Monitor system information

  pDemod      : Address of demodulator instance.
  pSystemInfo : Address of system information.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_SystemInfo(sony_cn_dtv_demod_t* pDemod,
                                                       sony_cn_dtv_dtmb_system_info_t* pSystemInfo);

/*--------------------------------------------------------------------
  Monitor demodulation sequence state

  pDemod    : Address of demodulator instance.
  pSeqState : Address of demodulator sequence state.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_SeqState(sony_cn_dtv_demod_t* pDemod,
                                                     sony_cn_dtv_demod_dtmb_seq_state_t* pSeqState);

/*--------------------------------------------------------------------
  Monitor spectrum sense

  pDemod    : Address of demodulator instance.
  pIsInvert : Address of spectrum sense. (0:Normal, 1:Invert)
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_SpectrumSense(sony_cn_dtv_demod_t* pDemod,
                                                          uint8_t* pIsInvert);

/*--------------------------------------------------------------------
  Monitor carrier offset

  pDemod  : Address of demodulator instance.
  pOffset : Address of carrier offset(KHz).
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_CarrierOffset(sony_cn_dtv_demod_t* pDemod,
                                                          int32_t* pOffset);

/*--------------------------------------------------------------------
  Monitor MER(Modulation Error Ratio)

  pDemod : Address of demodulator instance.
  pMer   : Address of MER (* 1e3)
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_MER(sony_cn_dtv_demod_t* pDemod,
                                                int32_t* pMer);

/*--------------------------------------------------------------------
  Monitor MERDT

  pDemod : Address of demodulator instance.
  pMerDt : Address of MER_DT registers value.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_MERDT(sony_cn_dtv_demod_t* pDemod,
                                                  uint32_t* pMerDt);

/*--------------------------------------------------------------------
  Monitor BER before LDPC

  pDemod : Address of demodulator instance.
  pBer   : Address of BER * 1e7.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PreLdpcBER(sony_cn_dtv_demod_t* pDemod,
                                                       uint32_t* pBer);

/*--------------------------------------------------------------------
  Monitor  the registers of BER before LDPC

  pDemod           : Address of demodulator instance.
  pBitError        : Address of IBER1_BITERR registers value.
  pLberMes         : Address of OREG_LBER_MES register value.
  pLberInfoOnlyReg : Address of OREG_LBER_INFO_ONLY register value.
  pRate            : Address of LDPC code rate.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PreLdpcBitError(sony_cn_dtv_demod_t* pDemod,
                                                            uint32_t* pBitError,
                                                            uint8_t* pLberMes,
                                                            uint8_t* pLberInfoOnlyReg,
                                                            sony_cn_dtv_dtmb_ldpc_rate_t* pRate);

/*--------------------------------------------------------------------
  Monitor FER after LDPC

  pDemod : Address of demodulator instance.
  pFer   : Address of FER * 1e6.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PostLdpcFER(sony_cn_dtv_demod_t* pDemod,
                                                        uint32_t* pFer);

/*--------------------------------------------------------------------
  Monitor the registers of FER after LDPC

  pDemod : Address of demodulator instance.
  pFbError : Address of IBER0_FBERR registers value.
  pLberMes : Address of OREG_LBER_MES register value.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PostLdpcFbError(sony_cn_dtv_demod_t* pDemod,
                                                           uint32_t* pFbError,
                                                           uint8_t* pLberMes);

/*--------------------------------------------------------------------
  Monitor BER before BCH

  pDemod : Address of demodulator instance.
  pBer   : Address of BER * 1e9.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PreBchBER(sony_cn_dtv_demod_t* pDemod,
                                                      uint32_t* pBer);

/*--------------------------------------------------------------------
  Monitor the registers of BER before BCH

  pDemod : Address of demodulator instance.
  pBitError : Address of IBER0_BITERR registers value.
  pBberMes  : Address of OREG_BBER_MES register value.
  pLberMes  : Address of OREG_LBER_MES register value.
  pBberSep  : Address of OREG_BBER_SEP register value.
  pRate     : Address of LDPC code rate.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PreBchBitError(sony_cn_dtv_demod_t* pDemod,
                                                           uint32_t* pBitError,
                                                           uint8_t* pBberMes,
                                                           uint8_t* pLberMes,
                                                           uint8_t* pBberSep,
                                                           sony_cn_dtv_dtmb_ldpc_rate_t* pRate);

/*--------------------------------------------------------------------
  Monitor FER after BCH

  pDemod : Address of demodulator instance.
  pFer   : Address of FER * 1e6.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PostBchFER(sony_cn_dtv_demod_t* pDemod,
                                                       uint32_t* pFer);

/*--------------------------------------------------------------------
  Monitor the registers of FER after BCH

  pDemod : Address of demodulator instance.
  pFbError : Address of IBER2_FBERR registers value.
  pBberMes : Address of OREG_BBER_MES register value.
  pLberMes  : Address of OREG_LBER_MES register value.
  pBberSep  : Address of OREG_BBER_SEP register value.
  pRate     : Address of LDPC code rate.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PostBchFbError(sony_cn_dtv_demod_t* pDemod,
                                                          uint32_t* pFbError,
                                                           uint8_t* pBberMes,
                                                           uint8_t* pLberMes,
                                                           uint8_t* pBberSep,
                                                           sony_cn_dtv_dtmb_ldpc_rate_t* pRate);

/*--------------------------------------------------------------------
  Monitor PER after BCH

  pDemod : Address of demodulator instance.
  pPer   : Address of PER * 1e6.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PostBchPER(sony_cn_dtv_demod_t* pDemod,
                                                       uint32_t* pPer);

/*--------------------------------------------------------------------
  Monitor the registers of PER after BCH

  pDemod       : Address of demodulator instance.
  pPacketError : Address of IBER2_PKTERR registers value.
  pBberMes     : Address of OREG_BBER_MES register value.
  pLberMes     : Address of OREG_LBER_MES register value.
  pBberSep     : Address of OREG_BBER_SEP register value.
  pRate        : Address of LDPC code rate.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PostBchPacketError(sony_cn_dtv_demod_t* pDemod,
                                                               uint32_t* pPacketError,
                                                               uint8_t* pBberMes,
                                                               uint8_t* pLberMes,
                                                               uint8_t* pBberSep,
                                                               sony_cn_dtv_dtmb_ldpc_rate_t* pRate);

/*--------------------------------------------------------------------
  Monitor packet error numbers after BCH

  pDemod : Address of demodulator instance.
  pPacketErrorNum : Address of BCH errors per second.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PostBchPktErrorNum(sony_cn_dtv_demod_t* pDemod,
                                                               uint32_t* pPacketErrorNum);

/*--------------------------------------------------------------------
  Monitor preset information

  pDemod      : Address of demodulator instance.
  pPresetInfo : Address of preset information.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_Preset(sony_cn_dtv_demod_t* pDemod,
                                                   sony_cn_dtv_demod_preset_info_t* pPresetInfo);

#endif /* SONY_CN_DTV_DEMOD_MONITORDTMB_H */

