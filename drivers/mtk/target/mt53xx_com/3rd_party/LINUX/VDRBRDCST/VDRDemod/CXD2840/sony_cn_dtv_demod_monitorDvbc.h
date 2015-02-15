/*------------------------------------------------------------------------------

 Copyright 2011-2012 Sony Corporation

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
 No part of this file may be copied, modified, sold, and distributed in any
 form or by any means without prior explicit permission in writing from
 Sony Corporation.

 Date: 2012/07/19
 Revision: 1.0.1.0

------------------------------------------------------------------------------*/
#ifndef SONY_CN_DTV_DEMOD_MONITORDVBC_H
#define SONY_CN_DTV_DEMOD_MONITORDVBC_H

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
  Monitor Sync Status

  pDemod    : Address of demodulator instance.
  pArLock   : Address of demodulator lock flag.(0:UNLOCK, 1:LOCK ; in Scan 0:searching, 1:DVB-C signal detected)
  pTsLock   : Address of TS lock flag(new).(0:UNLOCK, 1:LOCK)
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_SyncStat(sony_cn_dtv_demod_t* pDemod,
                                                     uint8_t* pArLock,
                                                     uint8_t* pTsLock);

/*--------------------------------------------------------------------
  Monitor ifagc output level

  pDemod : Address of demodulator instance.
  pIfAgc : Address of AGC output level.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_IfAgcout(sony_cn_dtv_demod_t* pDemod,
                                                     uint32_t* pIfAgc);

/*--------------------------------------------------------------------
  Monitor Constellation

  pDemod : Address of demodulator instance.
  pQam   : Address of constellation.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_QAM(sony_cn_dtv_demod_t* pDemod,
                                                sony_cn_dtv_dvbc_cnst_t* pQam);

/*--------------------------------------------------------------------
  Monitor symbol rate

  pDemod : Address of demodulator instance.
  pSymbolRate : Address of symbol rate.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_SymbolRate(sony_cn_dtv_demod_t* pDemod,
                                                       uint32_t* pSymbolRate);

/*--------------------------------------------------------------------
  Monitor carrier offset

  pDemod : Address of demodulator instance.
  pOffset : Address of carrier offset.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_CarrierOffset(sony_cn_dtv_demod_t* pDemod,
                                                          int32_t* pOffset);

/*--------------------------------------------------------------------
  Monitor SNR

  pDemod : Address of demodulator instance.
  pSnr   : Address of SNR * 1e3.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_SNR(sony_cn_dtv_demod_t* pDemod,
                                                int32_t* pSnr);

/*--------------------------------------------------------------------
  Monitor SNR register

  pDemod : Address of demodulator instance.
  pSnrReg : Address of SNR register value.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_SNRReg(sony_cn_dtv_demod_t* pDemod,
                                                   uint32_t* pSnrReg,
                                                   sony_cn_dtv_dvbc_cnst_t* pQam);

/*--------------------------------------------------------------------
  Monitor BER before Reed-Solomon

  pDemod : Address of demodulator instance.
  pBer   : Address of BER * 1e7.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_PreRSBER(sony_cn_dtv_demod_t* pDemod,
                                                     uint32_t* pBer);

/*--------------------------------------------------------------------
  Monitor the registers of BER before Reed-Solomon

  pDemod   : Address of demodulator instance.
  pBiteCnt : Address of IREG_CWBER_BITECNT registers value.
  pPeriod  : Address of BER measurement period.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_PreRSBitError(sony_cn_dtv_demod_t* pDemod,
                                                          uint32_t* pBiteCnt,
                                                          uint8_t* pPeriod);

/*--------------------------------------------------------------------
  Monitor Reed-Solomon error count

  pDemod : Address of demodulator instance.
  pErrorCnt : Address of RS errors per second.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_RSErrorNum(sony_cn_dtv_demod_t* pDemod,
                                                       uint32_t* pErrorCnt);

/*--------------------------------------------------------------------
  Monitor spectrum sense

  pDemod    : Address of demodulator instance.
  pIsInvert : Address of spectrum sense. (0:Normal, 1:Invert)
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_SpectrumSense(sony_cn_dtv_demod_t* pDemod,
                                                          uint8_t* pIsInvert);


/*--------------------------------------------------------------------
  Monitor PER

  pDemod : Address of demodulator instance.
  pBer   : Address of PER * 1e6.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_PER(sony_cn_dtv_demod_t* pDemod,
                                                uint32_t* pPer);

#endif /* SONY_CN_DTV_DEMOD_MONITORDVBC_H */
