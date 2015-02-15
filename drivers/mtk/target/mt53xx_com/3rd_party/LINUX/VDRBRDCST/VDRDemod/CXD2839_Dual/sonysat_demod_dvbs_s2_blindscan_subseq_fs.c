/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_common.h"
#include "sonysat_demod_dvbs_s2_monitor.h"
#include "sonysat_demod_dvbs_s2_blindscan.h"
#include "sonysat_demod_dvbs_s2_blindscan_data.h"
#include "sonysat_demod_dvbs_s2_blindscan_seq.h"
#include "sonysat_demod_dvbs_s2_blindscan_subseq_fs.h"
#include "sonysat_demod_dvbs_s2_blindscan_subseq_bt.h"

/*----------------------------------------------------------------------------
  Static Functions
----------------------------------------------------------------------------*/
static sonysat_result_t finish_ok (sonysat_demod_dvbs_s2_blindscan_subseq_fs_t * pSeq);
static sonysat_result_t updateProgress (sonysat_demod_dvbs_s2_blindscan_subseq_fs_t * pSeq);

/*----------------------------------------------------------------------------
  Functions
----------------------------------------------------------------------------*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_fs_Initialize (sonysat_demod_dvbs_s2_blindscan_subseq_fs_t * pSeq,
                                                                 sonysat_demod_dvbs_s2_blindscan_subseq_bt_t * pSeqBT,
                                                                 sonysat_demod_dvbs_s2_blindscan_seq_common_t * pCommonParams)
{
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_subseq_fs_Initialize");

    if ((!pSeq) || (!pSeqBT) || (!pCommonParams)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pSeq->isEnable = 0;
    pSeq->state = FS_STATE_START;
    pSeq->pSeqBT = pSeqBT;
    pSeq->pCommonParams = pCommonParams;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_fs_Start (sonysat_demod_dvbs_s2_blindscan_subseq_fs_t * pSeq,
                                                            sonysat_demod_dvbs_s2_blindscan_data_t * pCandList,
                                                            uint32_t minSymbolRateKSps,
                                                            uint32_t maxSymbolRateKSps,
                                                            sonysat_demod_dvbs_s2_blindscan_data_t * pChannelList)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_demod_dvbs_s2_blindscan_data_t * pCurrent = NULL;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_subseq_fs_Start");

    if ((!pSeq) || (!pCandList) || (!pChannelList)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pSeq->isEnable = 1;
    pSeq->state = FS_STATE_START;
    pSeq->pCandList = pCandList;
    pSeq->pCandCurrent = pSeq->pCandList->pNext;
    pSeq->pChannelList = pChannelList;
    pSeq->minSymbolRateKSps = minSymbolRateKSps;
    pSeq->maxSymbolRateKSps = maxSymbolRateKSps;

    pSeq->candIndex = 1;
    pSeq->candCount = 1;
    pCurrent = pCandList->pNext;
    while(pCurrent){
        pSeq->candCount++;
        pCurrent = pCurrent->pNext;
    }
    result = updateProgress (pSeq);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    pSeq->pChannelLast = pChannelList;
    while(pSeq->pChannelLast->pNext){
        pSeq->pChannelLast = pSeq->pChannelLast->pNext;
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_fs_Sequence (sonysat_demod_dvbs_s2_blindscan_subseq_fs_t * pSeq)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_demod_dvbs_s2_blindscan_data_t * pTemp = NULL;
    sonysat_demod_dvbs_s2_blindscan_data_t * pChannelCurrent = NULL;
    uint32_t candFreq = 0;
    uint32_t candSR = 0;
    uint32_t candMinSR = 0;
    uint32_t candMaxSR = 0;
    uint32_t channelFreq = 0;
    uint32_t channelSR = 0;
    uint8_t isTry = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_subseq_fs_Sequence");

    switch(pSeq->state)
    {
    case FS_STATE_START:
        result = updateProgress (pSeq);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        if (pSeq->pCandCurrent){
            /* Cand is exist */
            candFreq = pSeq->pCandCurrent->data.candidate.centerFreqKHz;
            candSR = pSeq->pCandCurrent->data.candidate.symbolRateKSps;
            candMinSR = pSeq->pCandCurrent->data.candidate.minSymbolRateKSps;
            candMaxSR = pSeq->pCandCurrent->data.candidate.maxSymbolRateKSps;

            isTry = 1;
            if (candMaxSR > pSeq->minSymbolRateKSps){
                pChannelCurrent = pSeq->pChannelList->pNext;
                while(pChannelCurrent){
                    channelFreq = pChannelCurrent->data.candidate.centerFreqKHz;
                    channelSR = pChannelCurrent->data.candidate.symbolRateKSps;
    
                    if (((channelFreq - (channelSR/2)) <= candFreq) && (candFreq <= (channelFreq + (channelSR/2)))){
                        /* duplicated */
                        isTry = 0;
                        break;
                    }
                    /* Next channel */
                    pChannelCurrent = pChannelCurrent->pNext;
                }
            } else {
                isTry = 0;
            }

            pSeq->candIndex++;
            pSeq->pCandCurrent = pSeq->pCandCurrent->pNext;

            if ((isTry) && (candMaxSR > 1000)) {
                result = sonysat_demod_dvbs_s2_blindscan_subseq_bt_Start (pSeq->pSeqBT,
                                                                       candFreq,
                                                                       candSR,
                                                                       candMinSR,
                                                                       candMaxSR,
                                                                       pSeq->minSymbolRateKSps,
                                                                       pSeq->maxSymbolRateKSps);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
                pSeq->state = FS_STATE_WAIT_BTFIN;
                pSeq->pCommonParams->waitTime =0;
            }
        } else {
            /* Finish */
            result = finish_ok(pSeq);
        }
        break;

    case FS_STATE_WAIT_BTFIN:
        /* Check result */
        if (pSeq->pSeqBT->isLocked){
            uint8_t isExist = 0;
            sonysat_demod_dvbs_s2_blindscan_data_t * pCurrent = pSeq->pChannelList->pNext;
            uint32_t detFrequencyKHz = (uint32_t)((int32_t)(pSeq->pCommonParams->tuneReq.frequencyKHz) + pSeq->pSeqBT->detCarrierOffsetKHz);
            uint32_t detSymbolRateKSps = (pSeq->pSeqBT->detSymbolRateSps + 500) / 1000;
            uint32_t chFrequencyKHz = 0;
            uint32_t chSymbolRateKSps = 0;

            isExist = 0;
            while(pCurrent){

                chFrequencyKHz = pCurrent->data.channelInfo.centerFreqKHz;
                chSymbolRateKSps = pCurrent->data.channelInfo.symbolRateKSps/2;

                if ((detFrequencyKHz >= (chFrequencyKHz - chSymbolRateKSps)) &&
                    (detFrequencyKHz <= (chFrequencyKHz + chSymbolRateKSps))){
                    /* It is already detected channel */
                    isExist = 1;
                    break;
                }
                pCurrent = pCurrent->pNext;
            }

            if (isExist == 0){

                /* Detect signal */
                pSeq->pCommonParams->detInfo.isDetect = 1;
                pSeq->pCommonParams->detInfo.centerFreqKHz = detFrequencyKHz;
                pSeq->pCommonParams->detInfo.symbolRateKSps = detSymbolRateKSps;
                pSeq->pCommonParams->detInfo.system = pSeq->pSeqBT->detSystem;

                result = sonysat_demod_dvbs_s2_blindscan_AllocData (&(pSeq->pCommonParams->storage), &pTemp); 
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }

                pTemp->data.channelInfo.centerFreqKHz = pSeq->pCommonParams->detInfo.centerFreqKHz;
                pTemp->data.channelInfo.symbolRateKSps = pSeq->pCommonParams->detInfo.symbolRateKSps;
                pTemp->data.channelInfo.system = pSeq->pCommonParams->detInfo.system;

                pSeq->pChannelLast->pNext = pTemp;
                pSeq->pChannelLast = pSeq->pChannelLast->pNext;
            }
        } else {
            /* Unlock */
            pSeq->pCommonParams->detInfo.isDetect = 0;
        }
        pSeq->state = FS_STATE_START;
        pSeq->pCommonParams->waitTime = 0;
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t finish_ok (sonysat_demod_dvbs_s2_blindscan_subseq_fs_t * pSeq)
{
    SONYSAT_TRACE_ENTER ("finish_ok");
    pSeq->isEnable = 0;
    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t updateProgress (sonysat_demod_dvbs_s2_blindscan_subseq_fs_t * pSeq)
{
    SONYSAT_TRACE_ENTER ("updateProgress");
    if (pSeq->candIndex > pSeq->candCount){
        pSeq->candIndex = pSeq->candCount;
    }
    pSeq->pCommonParams->progressInfo.minorProgress = (uint8_t)((pSeq->candIndex * 100) / pSeq->candCount);
    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}
