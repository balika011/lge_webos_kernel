/******************************************************************************
 *
 * Copyright 2013, Silicon Image, Inc.  All rights reserved.
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of
 * Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
 *
 *****************************************************************************/
/**
 * @file Sii977xLib.c
 *
 * @brief Defines the exported functions for the DLL application.
 *
 *****************************************************************************/
#define SII_DEBUG 3

/***** #include statements ***************************************************/

#include <stdio.h>
#include <string.h>

#include "si_datatypes.h"

#ifdef SII977X_LIB_ES0_RX
#include "sii9777rx_api.h"
#endif
#ifdef SII977X_LIB_ES0_TX
#include "sii9777tx_api.h"
#endif
#include "si_drv_cra_api.h"
#include "si_drv_isp_api.h"
#include "si_drv_ipc_api.h"
#include "si_sys_obj_api.h"
#include "si_sys_log_api.h"
#include "si_sys_time_api.h"
#include "sii9777_platform_api.h"

/***** Register Module name **************************************************/

SII_MODULE_NAME_SET(common_api);

/***** local macro definitions ***********************************************/

#define RESULT_CODE__CMD_SUCCESS        ((uint8_t)0x00)
#define RESULT_CODE__CMD_ERR_FAIL       ((uint8_t)0xFF)

/***** local type definitions ************************************************/

typedef enum
{
    FLASHING_STATE_ERASING,
    FLASHING_STATE_PROGRAMMING,
    FLASHING_STATE_DONE
} FlashingState_t;

typedef enum
{
    SII_EDID_SET = 0x002,
    SII_EDID_GET = 0x003,
    SII_INPUT_SELECT_SET = 0x004,
    SII_INPUT_SELECT_GET = 0x005,
    SII_EDID_REPLICATE_SET = 0x006,
    SII_EDID_REPLICATE_GET = 0x007,
    SII_SPLIT_MODE_SET = 0x008,
    SII_SPLIT_MODE_GET = 0x009,
    SII_CHIP_ID_STAT_GET = 0x00A,
    SII_CHIP_REV_STAT_GET = 0x00B,
    SII_PRODUCT_ID_STAT_GET = 0x00C,
    SII_FW_VER_STAT_GET = 0x00D,
    SII_FW_TIME_STAMP_STAT_GET = 0x00E,
    SII_DUAL_LINK_SET = 0x00F,
    SII_DUAL_LINK_GET = 0x010,
    SII_HPD_REPLICATE_SET = 0x011,
    SII_HPD_REPLICATE_GET = 0x012,
    SII_HPD_SET = 0x013,
    SII_HPD_GET = 0x014,
    SII_HDCP_TOPOLOGY_SET = 0x015,
    SII_HDCP_TOPOLOGY_GET = 0x016,
    SII_HDCP_BKSV_LIST_SET = 0x017,
    SII_HDCP_BKSV_LIST_GET = 0x018,
    SII_HDCP_RPTR_BIT_SET = 0x019,
    SII_HDCP_RPTR_BIT_GET = 0x01A,
    SII_DUAL_LINK_LUT_SET = 0x01B,
    SII_DUAL_LINK_LUT_GET = 0x01C,
    SII_DUAL_LINK_AUTO_SET = 0x01D,
    SII_DUAL_LINK_AUTO_GET = 0x01E,
    SII_AVLINK_STAT_GET = 0x01F,
    SII_HDCP_STAT_GET = 0x020,
    SII_AV_MUTE_STAT_GET = 0x021,
    SII_PLUS_5V_STAT_GET = 0x022,
    SII_VIDEO_TIMING_STAT_GET = 0x023,
    SII_VIDEO_FMT_STAT_GET = 0x024,
    SII_AUDIO_FMT_STAT_GET = 0x025,
    SII_PRIM_TX_PORT_SET = 0x026,
    SII_PRIM_TX_PORT_GET = 0x027,
    SII_TMDS_ENABLE_SET = 0x028,
    SII_TMDS_ENABLE_GET = 0x029,
    SII_AV_MUTE_SET = 0x02A,
    SII_AV_MUTE_GET = 0x02B,
    SII_HDCP_CONTENT_TYPE_SET = 0x02C,
    SII_HDCP_CONTENT_TYPE_GET = 0x02D,
    SII_VIDEO_FMT_SET = 0x02E,
    SII_VIDEO_FMT_GET = 0x02F,
    SII_AUDIO_FMT_SET = 0x030,
    SII_AUDIO_FMT_GET = 0x031,
    SII_EDID_STAT_GET = 0x032,
    SII_HDCP_CONTENT_TYPE_STAT_GET = 0x033,
    SII_HPD_STAT_GET = 0x034,
    SII_DUAL_LINK_STAT_GET = 0x035,
} SiiLibApiOpCode_t;

typedef struct
{
    uint32_t                          hInst;
    SiiLibEvtCbFunc_t                 siiLibEvtCbFunc;
    uint8_t                           devId;
    Sii9777Config_t*                  pConfig;
    bool_t                            bMessageReceived;
    bool_t                            bSemaphore;
    uint32_t		                  flashAddress;
    SiiDrvIpcTagCode_t                tagCode;
    SiiSysTimeMilli_t                 timeOut;
    bool_t                            bBootDone;
    FlashingState_t                   flashingState;
} Obj_t;

/***** local prototypes ******************************************************/

static void sHandleFlashProg( Obj_t* pObj );
static void sHandleInterrupts( Obj_t* pObj );
static void sIpcOpCodeSet( Obj_t* pObj, SiiDrvIpcMesInst_t mesInst, SiiDrvIpcOpCode_t opCode );
static void sTimeOutReset( Obj_t* pObj );
static bool_t sIpcRespMessageGet( Obj_t* pObj, SiiDrvIpcMesInst_t mesInst, SiiDrvIpcOpCode_t opCode );
static bool_t sInvalidRxEdidParameters( Sii9777RxPort_t port, uint16_t offset, uint16_t length );
static bool_t sInvalidTxEdidParameters( Sii9777TxPort_t port, uint16_t offset, uint16_t length );
static void sHandleSemaphoreTake( Obj_t* pObj );
static void sHandleSemaphoreRelease( Obj_t* pObj );

/***** local data objects ****************************************************/

static SiiSysObjListInst_t        sObjListInst = SII_INST_NULL;
static Obj_t*                     spObj        = NULL;

/***** call-back functions ***************************************************/

void SiiIpcCallBack(SiiDrvIpcEvent_t eventFlags)
{     
    Obj_t* pObj = spObj;

    if ( eventFlags & SII_DRV_IPC_EVENT__REQUEST )
    {
        pObj->bMessageReceived = true;
    }
}

/***** public functions ******************************************************/

SiiInst_t Sii9777Create( uint8_t devId, SiiLibEvtCbFunc_t pCallback, Sii9777Config_t* pConfig )
{
    SiiInst_t parentInst = SII_INST_NULL;
    Obj_t*    pObj       = NULL;

    if( !sObjListInst )
    {
        SII_ASSERT(!sObjListInst);
        sObjListInst = SiiSysObjListCreate(SII_MODULE_NAME_GET(), sizeof(Obj_t));
        SII_ASSERT(sObjListInst);
    }

    /* Allocate memory for object */
    pObj = (Obj_t*)SiiSysObjInstanceCreate(sObjListInst, parentInst, "TX");
    SII_ASSERT(pObj);
    SII_LOG1A("Create", pObj, ("\n"));

    pObj->devId            = devId, 
    /* allocate DrvAdapt handle */
    pObj->siiLibEvtCbFunc  = pCallback;
    pObj->pConfig          = pConfig;
    pObj->bMessageReceived = false;
    pObj->bSemaphore       = false;
    pObj->flashAddress     = 0;
    pObj->tagCode          = 0;
    pObj->bBootDone        = false;
    pObj->flashingState    = FLASHING_STATE_DONE;

    /* initialize IPC module */
    SiiDrvIpcCreate();

    spObj = pObj;
    return SII_OBJ2INST(pObj);
}
 
void Sii9777Delete( SiiInst_t inst )
{
    Obj_t* pObj = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);

    SII_LOG1A("Delete", pObj, (""));
    SiiSysObjInstanceDelete(pObj);
    if( !SiiSysObjFirstGet(sObjListInst) )
    {
        /* Delete list of objects */
        SiiSysObjListDelete(sObjListInst);
        sObjListInst = SII_INST_NULL;
    }
}

void Sii9777Handle( SiiInst_t inst )
{
    Obj_t* pObj = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    sHandleSemaphoreTake(pObj);
    if( FLASHING_STATE_DONE != pObj->flashingState )
    {
        sHandleFlashProg(pObj);  /* Flash program handler */
    }
    else
    {
        if( !pObj->bBootDone )
        {
            if( SiiDrvCraRead16(0, 0x7020) & 0x0001 )
            {
                pObj->bBootDone = true;
                pObj->siiLibEvtCbFunc(SII_OBJ2INST(pObj), SII9777_EVENT_FLAGS__BOOT_DONE);
            }
        }
    }
    sHandleInterrupts(pObj);    /* Message Handler */	
    sHandleSemaphoreRelease(pObj);	
}

void Sii9777FirmwareInit( SiiInst_t inst )
{
    Obj_t* pObj = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);

    SII_LOG1A("FirmwareInit", pObj, (""));
    SiiDrvIspChipErase();
    pObj->flashingState = FLASHING_STATE_ERASING;
}

void Sii9777FirmwareUpdate( SiiInst_t inst, const uint8_t* pData, uint16_t length )
{
    Obj_t* pObj = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);

    SII_LOG1A("FirmwareUpdate", pObj, ("Offset=0x%lX, length=0x%x\n", pObj->flashAddress, length));
    SiiDrvIspBurstWrite(pObj->flashAddress, pData, length);
    pObj->flashingState = FLASHING_STATE_PROGRAMMING;
    pObj->flashAddress += length;
}

void Sii9777FirmwareRestart( SiiInst_t inst )
{
    Obj_t* pObj = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);

    SII_LOG1A("Reset", pObj, ("Software restart the board.\n"));
    SiiPlatformSleepMsec(1000);
    SiiDrvCraBitsMod8(pObj->devId, 0x7010, 0x01, 0);
    SiiPlatformSleepMsec(3000);
    SiiDrvCraBitsMod8(pObj->devId, 0x7010, 0x01, 1);
    SiiPlatformSleepMsec(1000);
}

void Sii9777ChipIdStatusGet( SiiInst_t inst, uint32_t* pChipIdStat )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_CHIP_ID_STAT_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_CHIP_ID_STAT_GET) )
    {
        *pChipIdStat = SiiDrvIpcLongPop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("ChipIdStatusGet", pObj, ("0x%08lX\n", *pChipIdStat));
}

void Sii9777ChipRevisionStatusGet( SiiInst_t inst, uint8_t* pChipRevStat )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_CHIP_REV_STAT_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_CHIP_REV_STAT_GET) )
    {
        *pChipRevStat = SiiDrvIpcBytePop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("Sii9777ChipRevisionStatusGet", pObj, ("0x%02X\n", (uint16_t)*pChipRevStat));
}

void Sii9777ProductIdStatusGet( SiiInst_t inst, uint32_t* pProdIdStat )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_PRODUCT_ID_STAT_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_PRODUCT_ID_STAT_GET) )
    {
        *pProdIdStat = SiiDrvIpcLongPop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("ProductIdStatusGet", pObj, ("0x%08lX\n", *pProdIdStat));
}

void Sii9777FirmwareVersionStatusGet( SiiInst_t inst, uint32_t* pVersionStat )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_FW_VER_STAT_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_FW_VER_STAT_GET) )
    {
        *pVersionStat = SiiDrvIpcLongPop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("FirmwareVersionStatusGet", pObj, ("0x%08lX\n", *pVersionStat));
}

void Sii9777FirmwareTimeStampStatusGet( SiiInst_t inst, uint32_t* pTimeStamp )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_FW_TIME_STAMP_STAT_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_FW_TIME_STAMP_STAT_GET) )
    {
        *pTimeStamp = SiiDrvIpcLongPop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("FirmwareTimeStampStatusGet", pObj, ("0x%08lX\n", *pTimeStamp));
}

void Sii9777EdidSet( SiiInst_t inst, Sii9777RxPort_t port, uint16_t offset, uint8_t *pData, uint16_t length )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    SII_LOG1A("EdidSet", pObj, ("port=%d, offset=%04X, length=%d\n", (uint16_t)port, offset, length));
    if( sInvalidRxEdidParameters(port, offset, length) )
    {
        return;
    }

    sIpcOpCodeSet(pObj, mesInst, SII_EDID_SET);
    SiiDrvIpcBytePush(mesInst, (uint8_t)port);
    SiiDrvIpcWordPush(mesInst, offset);
    SiiDrvIpcWordPush(mesInst, length);
    SiiDrvIpcArrayPush(mesInst, pData, length);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    sIpcRespMessageGet(pObj, mesInst, SII_EDID_SET);
    SiiDrvIpcMessageDelete(mesInst);
    while( length-- )
    {
        SII_LOG1B(("%02X ", (uint16_t)*(pData++)));
    }
}

void Sii9777EdidGet( SiiInst_t inst, Sii9777RxPort_t port, uint16_t offset, uint8_t *pData, uint16_t length )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    SII_LOG1A("EdidGet", pObj, ("port=%d, offset=%04X, length=%d\n", (uint16_t)port, offset, length));
    if( sInvalidRxEdidParameters(port, offset, length) )
    {
        return;
    }

    sIpcOpCodeSet(pObj, mesInst, SII_EDID_GET);
    SiiDrvIpcBytePush(mesInst, (uint8_t)port);
    SiiDrvIpcWordPush(mesInst, offset);
    SiiDrvIpcWordPush(mesInst, length);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_EDID_GET) )
    {
        SiiDrvIpcArrayPop(mesInst, pData, length);
    }
    SiiDrvIpcMessageDelete(mesInst);
    while( length-- )
    {
        SII_LOG1B(("%02X ", (uint16_t)*(pData++)));
    }
}

void Sii9777EdidReplicateSet( SiiInst_t inst, bool_t* pbOn )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_EDID_REPLICATE_SET);
    SiiDrvIpcBoolPush(mesInst, *pbOn);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    sIpcRespMessageGet(pObj, mesInst, SII_EDID_REPLICATE_SET);
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("EdidReplicateSet", pObj, ("%d\n", *pbOn));
}

void Sii9777EdidReplicateGet( SiiInst_t inst, bool_t* pbOn )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_EDID_REPLICATE_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_EDID_REPLICATE_GET) )
    {
        *pbOn = SiiDrvIpcBoolPop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("EdidReplicateGet", pObj, ("%d\n", *pbOn));
}

void Sii9777SplitModeSet( SiiInst_t inst, Sii9777SplitMode_t* pSplitMode )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_SPLIT_MODE_SET);
    SiiDrvIpcBytePush(mesInst, *pSplitMode);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    sIpcRespMessageGet(pObj, mesInst, SII_SPLIT_MODE_SET);
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("SplitModeSet", pObj, ("%d\n", *pSplitMode));
}

void Sii9777SplitModeGet( SiiInst_t inst, Sii9777SplitMode_t* pSplitMode )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_SPLIT_MODE_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_SPLIT_MODE_GET) )
    {
        *pSplitMode = (Sii9777SplitMode_t)SiiDrvIpcBytePop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("SplitModeGet", pObj, ("%d\n", *pSplitMode));
}

void Sii9777InputSelectSet( SiiInst_t inst, Sii9777RxPort_t* pPort )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_INPUT_SELECT_SET);
    SiiDrvIpcBytePush(mesInst, *pPort);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    sIpcRespMessageGet(pObj, mesInst, SII_INPUT_SELECT_SET);
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("InputSelectSet", pObj, ("%d\n", *pPort));
}

void Sii9777InputSelectGet( SiiInst_t inst, Sii9777RxPort_t* pPort )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_INPUT_SELECT_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_INPUT_SELECT_GET) )
    {
        *pPort = (Sii9777RxPort_t)SiiDrvIpcBytePop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("InputSelectGet", pObj, ("%d\n", *pPort));
}

void Sii9777HpdReplicateSet( SiiInst_t inst, bool_t* pbOn )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_HPD_REPLICATE_SET);
    SiiDrvIpcBoolPush(mesInst, *pbOn);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    sIpcRespMessageGet(pObj, mesInst, SII_HPD_REPLICATE_SET);
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("HpdReplicateSet", pObj, ("%d\n", *pbOn));
}

void Sii9777HpdReplicateGet( SiiInst_t inst, bool_t* pbOn )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_HPD_REPLICATE_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_HPD_REPLICATE_GET) )
    {
        *pbOn = SiiDrvIpcBoolPop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("HpdReplicateGet", pObj, ("%d\n", *pbOn));
}

void Sii9777HpdSet( SiiInst_t inst, Sii9777RxPort_t port, bool_t *pbHpd )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_HPD_SET);
    SiiDrvIpcBytePush(mesInst, (uint8_t)port);
    SiiDrvIpcBoolPush(mesInst, *pbHpd);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    sIpcRespMessageGet(pObj, mesInst, SII_HPD_SET);
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("HpdSet", pObj, ("port=%d, %d\n", port, *pbHpd));
}

void Sii9777HpdGet( SiiInst_t inst, Sii9777RxPort_t port, bool_t *pbHpd )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_HPD_GET);
    SiiDrvIpcBytePush(mesInst, (uint8_t)port);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_HPD_GET) )
    {
        *pbHpd = SiiDrvIpcBoolPop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("HpdGet", pObj, ("port=%d, %d\n", port, *pbHpd));
}

void Sii9777HdcpTopologySet( SiiInst_t inst, Sii9777HdcpTopology_t *pTopology )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_HDCP_TOPOLOGY_SET);
    SiiDrvIpcBoolPush(mesInst, (uint8_t)pTopology->bHdcp1DeviceDs);
    SiiDrvIpcBoolPush(mesInst, (uint8_t)pTopology->bHdcp20DeviceDs);
    SiiDrvIpcBoolPush(mesInst, (uint8_t)pTopology->bMaxCascadeExceeded);
    SiiDrvIpcBoolPush(mesInst, (uint8_t)pTopology->bMaxDevsExceeded);
    SiiDrvIpcBytePush(mesInst, pTopology->deviceCount);
    SiiDrvIpcBytePush(mesInst, pTopology->depth);
    SiiDrvIpcLongPush(mesInst, pTopology->seqNumV);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    sIpcRespMessageGet(pObj, mesInst, SII_HDCP_TOPOLOGY_SET);
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("HdcpTopologySet", pObj, ("\n"));
    SII_LOG1B(("dcp1DeviceDs=%02X ", pTopology->bHdcp1DeviceDs));
    SII_LOG1B(("Hdcp20DeviceDs=%02X ", pTopology->bHdcp20DeviceDs));
    SII_LOG1B(("MaxCascadeExceeded=%02X ", pTopology->bMaxCascadeExceeded));
    SII_LOG1B(("MaxDevsExceeded=%02X ", pTopology->bMaxDevsExceeded));
    SII_LOG1B(("deviceCount=%02X ", pTopology->deviceCount));
    SII_LOG1B(("depth=%02X ", pTopology->depth));
    SII_LOG1B(("seqNumV=%08X\n", pTopology->seqNumV));
}

void Sii9777HdcpTopologyGet( SiiInst_t inst, Sii9777HdcpTopology_t *pTopology )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_HDCP_TOPOLOGY_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_HDCP_TOPOLOGY_GET) )
    {
        pTopology->bHdcp1DeviceDs = SiiDrvIpcBoolPop(mesInst);
        pTopology->bHdcp20DeviceDs = SiiDrvIpcBoolPop(mesInst);
        pTopology->bMaxCascadeExceeded = SiiDrvIpcBoolPop(mesInst);
        pTopology->bMaxDevsExceeded = SiiDrvIpcBoolPop(mesInst);
        pTopology->deviceCount = SiiDrvIpcBytePop(mesInst);
        pTopology->depth = SiiDrvIpcBytePop(mesInst);
        pTopology->seqNumV = SiiDrvIpcLongPop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("HdcpTopologyGet", pObj, ("\n"));
    SII_LOG1B(("dcp1DeviceDs=%02X ", pTopology->bHdcp1DeviceDs));
    SII_LOG1B(("Hdcp20DeviceDs=%02X ", pTopology->bHdcp20DeviceDs));
    SII_LOG1B(("MaxCascadeExceeded=%02X ", pTopology->bMaxCascadeExceeded));
    SII_LOG1B(("MaxDevsExceeded=%02X ", pTopology->bMaxDevsExceeded));
    SII_LOG1B(("deviceCount=%02X ", pTopology->deviceCount));
    SII_LOG1B(("depth=%02X ", pTopology->depth));
    SII_LOG1B(("seqNumV=%08X\n", pTopology->seqNumV));
}

uint8_t Sii9777HdcpBksvListSet( SiiInst_t inst, Sii9777HdcpKsv_t* pBksvList, uint8_t len )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();
    Sii9777HdcpKsv_t*   pCurrBksv = NULL;
    uint8_t             retLen = 0, length = len, i;

    SII_LOG1A("HdcpBksvListSet", pObj, ("length=%d\n", len));

    sIpcOpCodeSet(pObj, mesInst, SII_HDCP_BKSV_LIST_SET);
    SiiDrvIpcBytePush(mesInst, len);
    if (pBksvList != NULL)
    {
        pCurrBksv = pBksvList;
        for (i = 0; i < length; i++, pCurrBksv++)
        {
            SiiDrvIpcArrayPush(mesInst, &pCurrBksv->b[0], 5);
        }
    }
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if (sIpcRespMessageGet(pObj, mesInst, SII_HDCP_BKSV_LIST_SET))
    {
        retLen = SiiDrvIpcBytePop(mesInst);
        if (pBksvList != NULL)
        {
            if (retLen < len)
            {
                length = retLen;
            }
            else
            {
                length = len;
            }
        }
        else
        {
            length = 0;
        }
    }
    SiiDrvIpcMessageDelete(mesInst);
    if (pBksvList != NULL)
    {
        pCurrBksv = pBksvList;
        i = length;
        while( i-- )
        {
            SII_LOG1B(("%02X %02X %02X %02X %02X\n",
                (uint16_t)(pCurrBksv->b[0]),
                (uint16_t)(pCurrBksv->b[1]),
                (uint16_t)(pCurrBksv->b[2]),
                (uint16_t)(pCurrBksv->b[3]),
                (uint16_t)(pCurrBksv->b[4])));
            pCurrBksv++;
        }
    }

    return length;
}

uint8_t Sii9777HdcpBksvListGet( SiiInst_t inst, Sii9777HdcpKsv_t* pBksvList, uint8_t len )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();
    Sii9777HdcpKsv_t*   pCurrBksv = NULL;
    uint8_t             retLen = 0, length = 0, i;

    SII_LOG1A("HdcpBksvListGet", pObj, ("length=%d\n", len));
    sIpcOpCodeSet(pObj, mesInst, SII_HDCP_BKSV_LIST_GET);
    SiiDrvIpcBytePush(mesInst, len);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_HDCP_BKSV_LIST_GET) )
    {
        retLen = SiiDrvIpcBytePop(mesInst);
        if (pBksvList != NULL)
        {
            pCurrBksv = pBksvList;
            if (retLen < len)
            {
                length = retLen;
            }
            else
            {
                length = len;
            }
            for (i = 0; i < length; i++, pCurrBksv++)
            {
                SiiDrvIpcArrayPop(mesInst, &pCurrBksv->b[0], 5);
            }
        }
        else
        {
            length = 0;
        }
    }
    SiiDrvIpcMessageDelete(mesInst);
    if (pBksvList != NULL)
    {
        pCurrBksv = pBksvList;
        i = length;
        while( i-- )
        {
            SII_LOG1B(("%02X %02X %02X %02X %02X\n",
                (uint16_t)(pCurrBksv->b[0]),
                (uint16_t)(pCurrBksv->b[1]),
                (uint16_t)(pCurrBksv->b[2]),
                (uint16_t)(pCurrBksv->b[3]),
                (uint16_t)(pCurrBksv->b[4])));
            pCurrBksv++;
        }
    }

    return length;
}

void Sii9777HdcpRepeaterBitSet( SiiInst_t inst, bool_t *pRepeaterBit )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_HDCP_RPTR_BIT_SET);
    SiiDrvIpcBoolPush(mesInst, *pRepeaterBit);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    sIpcRespMessageGet(pObj, mesInst, SII_HDCP_RPTR_BIT_SET);
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("HdcpRptrBitSet", pObj, ("RepeaterBit=%d\n", *pRepeaterBit));
}

void Sii9777HdcpRepeaterBitGet( SiiInst_t inst, bool_t *pRepeaterBit )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_HDCP_RPTR_BIT_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_HDCP_RPTR_BIT_GET) )
    {
        *pRepeaterBit = SiiDrvIpcBoolPop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("HdcpRptrBitGet", pObj, ("RepeaterBit=%d\n", *pRepeaterBit));
}

void Sii9777DualLinkLutSet( SiiInst_t inst, SII9777VidRes_t* pVidRes, uint8_t len )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();
    SII9777VidRes_t*    pCurrVidRes = NULL;
    uint8_t             length = len, i;

    sIpcOpCodeSet(pObj, mesInst, SII_DUAL_LINK_LUT_SET);
    SiiDrvIpcBytePush(mesInst, len);
    if (pVidRes != NULL)
    {
        pCurrVidRes = pVidRes;
        for (i = 0; i < length; i++, pCurrVidRes++)
        {
            SiiDrvIpcBytePush(mesInst, (uint8_t)(*pCurrVidRes));
        }
    }
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    sIpcRespMessageGet(pObj, mesInst, SII_DUAL_LINK_LUT_SET);
    SiiDrvIpcMessageDelete(mesInst);

    SII_LOG1A("DualLinkLutSet", pObj, ("length=%d\n", len));
    if (pVidRes != NULL)
    {
        pCurrVidRes = pVidRes;
        i = length;
        while( i-- )
        {
            SII_LOG1B(("%02X ", (uint8_t)(*pCurrVidRes)));
            pCurrVidRes++;
        }
        SII_LOG1B(("\n"));
    }
}

void Sii9777DualLinkLutGet( SiiInst_t inst, SII9777VidRes_t* pVidRes, uint8_t len )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();
    SII9777VidRes_t*    pCurrVidRes = NULL;
    uint8_t             i;

    sIpcOpCodeSet(pObj, mesInst, SII_DUAL_LINK_LUT_GET);
    SiiDrvIpcBytePush(mesInst, len);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_DUAL_LINK_LUT_GET) )
    {
        if (pVidRes != NULL)
        {
            pCurrVidRes = pVidRes;
            for (i = 0; i < len; i++, pCurrVidRes++)
            {
                *pCurrVidRes = (SII9777VidRes_t)SiiDrvIpcBytePop(mesInst);
            }
        }
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("DualLinkLutGet", pObj, ("length=%d\n", len));
    if (pVidRes != NULL)
    {
        pCurrVidRes = pVidRes;
        i = len;
        while( i-- )
        {
            SII_LOG1B(("%02X ", (uint8_t)(*pCurrVidRes)));
            pCurrVidRes++;
        }
        SII_LOG1B(("\n"));
    }
}

void Sii9777DualLinkSet( SiiInst_t inst, bool_t* bDualLink )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_DUAL_LINK_SET);
    SiiDrvIpcBoolPush(mesInst, *bDualLink);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    sIpcRespMessageGet(pObj, mesInst, SII_DUAL_LINK_SET);
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("DualLinkSet", pObj, ("DualLink=%d\n", *bDualLink));
}

void Sii9777DualLinkGet( SiiInst_t inst, bool_t* bDualLink )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_DUAL_LINK_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_DUAL_LINK_GET) )
    {
        *bDualLink = SiiDrvIpcBoolPop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("DualLinkGet", pObj, ("DualLink=%d\n", *bDualLink));
}

void Sii9777DualLinkAutoSet( SiiInst_t inst, bool_t* bAuto )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_DUAL_LINK_AUTO_SET);
    SiiDrvIpcBoolPush(mesInst, *bAuto);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    sIpcRespMessageGet(pObj, mesInst, SII_DUAL_LINK_AUTO_SET);
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("DualLinkAutoSet", pObj, ("DualLinkAuto=%d\n", *bAuto));
}

void Sii9777DualLinkAutoGet( SiiInst_t inst, bool_t* bAuto )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_DUAL_LINK_AUTO_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_DUAL_LINK_AUTO_GET) )
    {
        *bAuto = SiiDrvIpcBoolPop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("DualLinkAutoGet", pObj, ("DualLinkAuto=%d\n", *bAuto));
}

#ifdef SII977X_LIB_ES0_RX
void Sii9777AvLinkStatusGet( SiiInst_t inst, Sii9777AvLink_t *pAvLink )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_AVLINK_STAT_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_AVLINK_STAT_GET) )
    {
        *pAvLink = (Sii9777AvLink_t)SiiDrvIpcBytePop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("AvLinkStatus", pObj, ("AvLinkStatus=%d\n", *pAvLink));
}
#endif

#ifdef SII977X_LIB_ES0_TX
void Sii9777AvLinkStatusGet( SiiInst_t inst, Sii9777TxPort_t port, Sii9777AvLink_t *pAvLink )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_AVLINK_STAT_GET);
    SiiDrvIpcBytePush(mesInst, (uint8_t)port);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_AVLINK_STAT_GET) )
    {
        *pAvLink = (Sii9777AvLink_t)SiiDrvIpcBytePop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("AvLinkStatus", pObj, ("AvLinkStatus=%d\n", *pAvLink));
}
#endif

#ifdef SII977X_LIB_ES0_RX
void Sii9777HdcpStatusGet( SiiInst_t inst, Sii9777HdcpStatus_t *pHdcpStat )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_HDCP_STAT_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_HDCP_STAT_GET) )
    {
        *pHdcpStat = (Sii9777AvLink_t)SiiDrvIpcBytePop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("HdcpStatus", pObj, ("HdcpStatus=%d\n", *pHdcpStat));
}
#endif

#ifdef SII977X_LIB_ES0_TX
void Sii9777HdcpStatusGet( SiiInst_t inst, Sii9777TxPort_t port, Sii9777HdcpStatus_t *pHdcpStat )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_HDCP_STAT_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_HDCP_STAT_GET) )
    {
        *pHdcpStat = (Sii9777AvLink_t)SiiDrvIpcBytePop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("HdcpStatus", pObj, ("HdcpStatus=%d\n", *pHdcpStat));
}
#endif

void Sii9777AvMuteStatusGet( SiiInst_t inst, bool_t *pbAvMute )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_AV_MUTE_STAT_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_AV_MUTE_STAT_GET) )
    {
        *pbAvMute = SiiDrvIpcBoolPop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("AvMuteStatus", pObj, ("AvMuteStatus=%d\n", *pbAvMute));
}

void Sii9777Plus5vStatusGet( SiiInst_t inst, bool_t *pbPlus5v )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_PLUS_5V_STAT_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_PLUS_5V_STAT_GET) )
    {
        *pbPlus5v = SiiDrvIpcBoolPop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("Plus5vStatus", pObj, ("Plus5vStatus=%d\n", *pbPlus5v));
}

void Sii9777VideoTimingStatusGet( SiiInst_t inst, Sii9777VideoTiming_t *pVideoTiming )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_VIDEO_TIMING_STAT_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_VIDEO_TIMING_STAT_GET) )
    {
        pVideoTiming->interlaced = SiiDrvIpcBoolPop(mesInst);
        pVideoTiming->posHsync = SiiDrvIpcBoolPop(mesInst);
        pVideoTiming->posVsync = SiiDrvIpcBoolPop(mesInst);
        pVideoTiming->pixelFreq = SiiDrvIpcLongPop(mesInst);
        pVideoTiming->vTotal = SiiDrvIpcWordPop(mesInst);
        pVideoTiming->vActive = SiiDrvIpcWordPop(mesInst);
        pVideoTiming->vSync = SiiDrvIpcWordPop(mesInst);
        pVideoTiming->vFrontPorch = SiiDrvIpcWordPop(mesInst);
        pVideoTiming->hTotal = SiiDrvIpcWordPop(mesInst);
        pVideoTiming->hActive = SiiDrvIpcWordPop(mesInst);
        pVideoTiming->hSync = SiiDrvIpcWordPop(mesInst);
        pVideoTiming->hFrontPorch = SiiDrvIpcWordPop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("VideoTimingStatusGet", pObj, ("\n"));
    SII_LOG1B(("interlaced=%02X ", pVideoTiming->interlaced));
    SII_LOG1B(("posHsync=%02X ", pVideoTiming->posHsync));
    SII_LOG1B(("posVsync=%02X ", pVideoTiming->posVsync));
    SII_LOG1B(("pixelFreq=%08X ", pVideoTiming->pixelFreq));
    SII_LOG1B(("vTotal=%04X ", pVideoTiming->vTotal));
    SII_LOG1B(("vActive=%04X ", pVideoTiming->vActive));
    SII_LOG1B(("vSync=%04X ", pVideoTiming->vSync));
    SII_LOG1B(("vFrontPorch=%04X ", pVideoTiming->vFrontPorch));
    SII_LOG1B(("hTotal=%04X ", pVideoTiming->hTotal));
    SII_LOG1B(("hActive=%04X ", pVideoTiming->hActive));
    SII_LOG1B(("hSync=%04X ", pVideoTiming->hSync));
    SII_LOG1B(("hFrontPorch=%04X\n", pVideoTiming->hFrontPorch));
}

Sii9777Data_t Sii9777VideoFmtStatusGet( SiiInst_t inst, Sii9777VideoFmt_t *poVideoFmt )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();
    Sii9777Data_t       retData;

    sIpcOpCodeSet(pObj, mesInst, SII_VIDEO_FMT_STAT_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_VIDEO_FMT_STAT_GET) )
    {
        poVideoFmt->vidRes = (SII9777VidRes_t)SiiDrvIpcBytePop(mesInst);
        poVideoFmt->mode3D = (Sii97773dMode_t)SiiDrvIpcBytePop(mesInst);
        poVideoFmt->pictureAR = (Sii9777PictAR_t)SiiDrvIpcBytePop(mesInst);
        poVideoFmt->pixelRepetition = SiiDrvIpcBytePop(mesInst);
        poVideoFmt->pixFrm.clrFmt = (Sii9777ClrFmt_t)SiiDrvIpcBytePop(mesInst);
        poVideoFmt->pixFrm.crSmpl = (Sii9777CrSmpl_t)SiiDrvIpcBytePop(mesInst);
        poVideoFmt->pixFrm.bitDepth = (Sii9777BitDepth_t)SiiDrvIpcBytePop(mesInst);
        poVideoFmt->activeAR = SiiDrvIpcBytePop(mesInst);
        poVideoFmt->picScaling = SiiDrvIpcBytePop(mesInst);
        poVideoFmt->scanInfo = SiiDrvIpcBytePop(mesInst);
        poVideoFmt->contentType = SiiDrvIpcBytePop(mesInst);
        poVideoFmt->isITContent = SiiDrvIpcBytePop(mesInst);
        poVideoFmt->isActiveFormat = SiiDrvIpcBytePop(mesInst);
        poVideoFmt->isVertBarInfoValid = SiiDrvIpcBytePop(mesInst);
        poVideoFmt->isHorizBarInfoValid = SiiDrvIpcBytePop(mesInst);
        poVideoFmt->vertBarInfo.lineNumEndOfTopBar = SiiDrvIpcWordPop(mesInst);
        poVideoFmt->vertBarInfo.lineNumStartOfBottomBar = SiiDrvIpcWordPop(mesInst);
        poVideoFmt->horizBarInfo.pixNumEndOfLeftBar = SiiDrvIpcWordPop(mesInst);
        poVideoFmt->horizBarInfo.pixNumStartOfRightBar = SiiDrvIpcWordPop(mesInst);
        retData = SII9777_DATA__SUCCESS;
    }
    else
    {
        retData = SII9777_DATA__UNAVAILABLE;
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("VideoFmtStatusGet", pObj, (""));
    SII_LOG1B(("return value=%02X\n", retData));
    if (retData == SII9777_DATA__SUCCESS)
    {
        SII_LOG1B(("vidRes=%02X ", poVideoFmt->vidRes));
        SII_LOG1B(("mode3D=%02X ", poVideoFmt->mode3D));
        SII_LOG1B(("pictureAR=%02X ", poVideoFmt->pictureAR));
        SII_LOG1B(("pixelRepetition=%02X ", poVideoFmt->pixelRepetition));
        SII_LOG1B(("pixFrm.clrFmt=%02X ", poVideoFmt->pixFrm.clrFmt));
        SII_LOG1B(("pixFrm.crSmpl=%02X ", poVideoFmt->pixFrm.crSmpl));
        SII_LOG1B(("pixFrm.bitDepth=%02X ", poVideoFmt->pixFrm.bitDepth));
        SII_LOG1B(("activeAR=%02X ", poVideoFmt->activeAR));
        SII_LOG1B(("picScaling=%02X ", poVideoFmt->picScaling));
        SII_LOG1B(("scanInfo=%02X ", poVideoFmt->scanInfo));
        SII_LOG1B(("contentType=%02X ", poVideoFmt->contentType));
        SII_LOG1B(("isITContent=%02X ", poVideoFmt->isITContent));
        SII_LOG1B(("isActiveFormat=%02X ", poVideoFmt->isActiveFormat));
        SII_LOG1B(("isVertBarInfoValid=%02X ", poVideoFmt->isVertBarInfoValid));
        SII_LOG1B(("isHorizBarInfoValid=%02X ", poVideoFmt->isHorizBarInfoValid));
        SII_LOG1B(("vertBarInfo.lineNumEndOfTopBar=%04X ", poVideoFmt->vertBarInfo.lineNumEndOfTopBar));
        SII_LOG1B(("vertBarInfo.lineNumStartOfBottomBar=%04X ", poVideoFmt->vertBarInfo.lineNumStartOfBottomBar));
        SII_LOG1B(("horizBarInfo.pixNumEndOfLeftBar=%04X ", poVideoFmt->horizBarInfo.pixNumEndOfLeftBar));
        SII_LOG1B(("horizBarInfo.pixNumStartOfRightBar=%04X\n", poVideoFmt->horizBarInfo.pixNumStartOfRightBar));
    }

    return (retData);
}

Sii9777Data_t Sii9777AudioFmtStatusGet( SiiInst_t inst, Sii9777AudioFrm_t *pAudioFmt )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();
    Sii9777Data_t       retData;

    SII_LOG1A("Sii9777AudioFmtStatusGet", pObj, (""));
    sIpcOpCodeSet(pObj, mesInst, SII_AUDIO_FMT_STAT_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_AUDIO_FMT_STAT_GET) )
    {
        SiiDrvIpcArrayPop(mesInst, pAudioFmt->aifData, 10);
        SiiDrvIpcArrayPop(mesInst, pAudioFmt->csData, 10);
        retData = SII9777_DATA__SUCCESS;
    }
    else
    {
        retData = SII9777_DATA__UNAVAILABLE;
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1B(("return value=%02X\n", retData));
    if (retData == SII9777_DATA__SUCCESS)
    {
        uint8_t *pData;
        int length;
        length = 10;
        pData = pAudioFmt->aifData;
        SII_LOG1B(("aifData: "));
        while( length-- )
        {
            SII_LOG1B(("%02X ", (uint16_t)*(pData++)));
        }
        SII_LOG1B(("\n"));
        length = 10;
        pData = pAudioFmt->csData;
        SII_LOG1B(("csData: "));
        while( length-- )
        {
            SII_LOG1B(("%02X ", (uint16_t)*(pData++)));
        }
        SII_LOG1B(("\n"));
    }

    return (retData);
}

void Sii9777PrimTxPortSet( SiiInst_t inst, Sii9777TxPort_t* pPort )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_PRIM_TX_PORT_SET);
    SiiDrvIpcBytePush(mesInst, *pPort);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    sIpcRespMessageGet(pObj, mesInst, SII_PRIM_TX_PORT_SET);
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("PrimTxPortSet", pObj, ("%d\n", *pPort));
}

void Sii9777PrimTxPortGet( SiiInst_t inst, Sii9777TxPort_t* pPort )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_PRIM_TX_PORT_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_PRIM_TX_PORT_GET) )
    {
        *pPort = (Sii9777TxPort_t)SiiDrvIpcBytePop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("PrimTxPortGet", pObj, ("%d\n", *pPort));
}

void Sii9777TmdsEnableSet( SiiInst_t inst, Sii9777TxPort_t port, bool_t* bTmds )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_TMDS_ENABLE_SET);
    SiiDrvIpcBytePush(mesInst, (uint8_t)port);
    SiiDrvIpcBoolPush(mesInst, *bTmds);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    sIpcRespMessageGet(pObj, mesInst, SII_TMDS_ENABLE_SET);
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("TmdsEnableSet", pObj, ("port=%d, %d\n", port, *bTmds));
}

void Sii9777TmdsEnableGet( SiiInst_t inst, Sii9777TxPort_t port, bool_t* bTmds )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_TMDS_ENABLE_GET);
    SiiDrvIpcBytePush(mesInst, (uint8_t)port);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_TMDS_ENABLE_GET) )
    {
        *bTmds = SiiDrvIpcBoolPop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("TmdsEnableGet", pObj, ("port=%d, %d\n", port, *bTmds));
}

void Sii9777AvMuteSet( SiiInst_t inst, Sii9777TxPort_t port, bool_t* pbAvMute )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_AV_MUTE_SET);
    SiiDrvIpcBytePush(mesInst, (uint8_t)port);
    SiiDrvIpcBoolPush(mesInst, *pbAvMute);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    sIpcRespMessageGet(pObj, mesInst, SII_AV_MUTE_SET);
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("AvMuteSet", pObj, ("port=%d, %d\n", port, *pbAvMute));
}

void Sii9777AvMuteGet( SiiInst_t inst, Sii9777TxPort_t port, bool_t* pbAvMute )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_AV_MUTE_GET);
    SiiDrvIpcBytePush(mesInst, (uint8_t)port);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_AV_MUTE_GET) )
    {
        *pbAvMute = SiiDrvIpcBoolPop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("AvMuteGet", pObj, ("port=%d, %d\n", port, *pbAvMute));
}

void Sii9777HdcpContentTypeSet( SiiInst_t inst, Sii9777HdcpContentType_t* pContentType )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_HDCP_CONTENT_TYPE_SET);
    SiiDrvIpcBytePush(mesInst, *pContentType);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    sIpcRespMessageGet(pObj, mesInst, SII_HDCP_CONTENT_TYPE_SET);
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("HdcpContentTypeSet", pObj, ("%d\n", *pContentType));
}

void Sii9777HdcpContentTypeGet( SiiInst_t inst, Sii9777HdcpContentType_t* pContentType )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_HDCP_CONTENT_TYPE_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_HDCP_CONTENT_TYPE_GET) )
    {
        *pContentType = (Sii9777HdcpContentType_t)SiiDrvIpcBytePop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("HdcpContentTypeGet", pObj, ("%d\n", *pContentType));
}

void Sii9777VideoFmtSet( SiiInst_t inst, Sii9777VideoFmt_t *pVideoFmt )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_VIDEO_FMT_SET);
    SiiDrvIpcBytePush(mesInst, (uint8_t)pVideoFmt->vidRes);
    SiiDrvIpcBytePush(mesInst, (uint8_t)pVideoFmt->mode3D);
    SiiDrvIpcBytePush(mesInst, (uint8_t)pVideoFmt->pictureAR);
    SiiDrvIpcBytePush(mesInst, pVideoFmt->pixelRepetition);
    SiiDrvIpcBytePush(mesInst, (uint8_t)pVideoFmt->pixFrm.clrFmt);
    SiiDrvIpcBytePush(mesInst, (uint8_t)pVideoFmt->pixFrm.crSmpl);
    SiiDrvIpcBytePush(mesInst, (uint8_t)pVideoFmt->pixFrm.bitDepth);
    SiiDrvIpcBytePush(mesInst, (uint8_t)pVideoFmt->activeAR);
    SiiDrvIpcBytePush(mesInst, (uint8_t)pVideoFmt->picScaling);
    SiiDrvIpcBytePush(mesInst, (uint8_t)pVideoFmt->scanInfo);
    SiiDrvIpcBytePush(mesInst, (uint8_t)pVideoFmt->contentType);
    SiiDrvIpcBoolPush(mesInst, (uint8_t)pVideoFmt->isITContent);
    SiiDrvIpcBoolPush(mesInst, (uint8_t)pVideoFmt->isActiveFormat);
    SiiDrvIpcBoolPush(mesInst, (uint8_t)pVideoFmt->isVertBarInfoValid);
    SiiDrvIpcBoolPush(mesInst, (uint8_t)pVideoFmt->isHorizBarInfoValid);
    SiiDrvIpcWordPush(mesInst, pVideoFmt->vertBarInfo.lineNumEndOfTopBar);
    SiiDrvIpcWordPush(mesInst, pVideoFmt->vertBarInfo.lineNumStartOfBottomBar);
    SiiDrvIpcWordPush(mesInst, pVideoFmt->horizBarInfo.pixNumEndOfLeftBar);
    SiiDrvIpcWordPush(mesInst, pVideoFmt->horizBarInfo.pixNumStartOfRightBar);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    sIpcRespMessageGet(pObj, mesInst, SII_VIDEO_FMT_SET);
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("VideoFmtSet", pObj, ("\n"));
    SII_LOG1B(("vidRes=%02X ", pVideoFmt->vidRes));
    SII_LOG1B(("mode3D=%02X ", pVideoFmt->mode3D));
    SII_LOG1B(("pictureAR=%02X ", pVideoFmt->pictureAR));
    SII_LOG1B(("pixelRepetition=%02X ", pVideoFmt->pixelRepetition));
    SII_LOG1B(("pixFrm.clrFmt=%02X ", pVideoFmt->pixFrm.clrFmt));
    SII_LOG1B(("pixFrm.crSmpl=%02X ", pVideoFmt->pixFrm.crSmpl));
    SII_LOG1B(("pixFrm.bitDepth=%02X ", pVideoFmt->pixFrm.bitDepth));
    SII_LOG1B(("activeAR=%02X ", pVideoFmt->activeAR));
    SII_LOG1B(("picScaling=%02X ", pVideoFmt->picScaling));
    SII_LOG1B(("scanInfo=%02X ", pVideoFmt->scanInfo));
    SII_LOG1B(("contentType=%02X ", pVideoFmt->contentType));
    SII_LOG1B(("isITContent=%02X ", pVideoFmt->isITContent));
    SII_LOG1B(("isActiveFormat=%02X ", pVideoFmt->isActiveFormat));
    SII_LOG1B(("isVertBarInfoValid=%02X ", pVideoFmt->isVertBarInfoValid));
    SII_LOG1B(("isHorizBarInfoValid=%02X ", pVideoFmt->isHorizBarInfoValid));
    SII_LOG1B(("vertBarInfo.lineNumEndOfTopBar=%04X ", pVideoFmt->vertBarInfo.lineNumEndOfTopBar));
    SII_LOG1B(("vertBarInfo.lineNumStartOfBottomBar=%04X ", pVideoFmt->vertBarInfo.lineNumStartOfBottomBar));
    SII_LOG1B(("horizBarInfo.pixNumEndOfLeftBar=%04X ", pVideoFmt->horizBarInfo.pixNumEndOfLeftBar));
    SII_LOG1B(("horizBarInfo.pixNumStartOfRightBar=%04X\n", pVideoFmt->horizBarInfo.pixNumStartOfRightBar));
}

void Sii9777VideoFmtGet( SiiInst_t inst, Sii9777VideoFmt_t *pVideoFmt )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_VIDEO_FMT_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_VIDEO_FMT_GET) )
    {
        pVideoFmt->vidRes = (SII9777VidRes_t)SiiDrvIpcBytePop(mesInst);
        pVideoFmt->mode3D = (Sii97773dMode_t)SiiDrvIpcBytePop(mesInst);
        pVideoFmt->pictureAR = (Sii9777PictAR_t)SiiDrvIpcBytePop(mesInst);
        pVideoFmt->pixelRepetition = SiiDrvIpcBytePop(mesInst);
        pVideoFmt->pixFrm.clrFmt = (Sii9777ClrFmt_t)SiiDrvIpcBytePop(mesInst);
        pVideoFmt->pixFrm.crSmpl = (Sii9777CrSmpl_t)SiiDrvIpcBytePop(mesInst);
        pVideoFmt->pixFrm.bitDepth = (Sii9777BitDepth_t)SiiDrvIpcBytePop(mesInst);
        pVideoFmt->activeAR = SiiDrvIpcBytePop(mesInst);
        pVideoFmt->picScaling = SiiDrvIpcBytePop(mesInst);
        pVideoFmt->scanInfo = SiiDrvIpcBytePop(mesInst);
        pVideoFmt->contentType = SiiDrvIpcBytePop(mesInst);
        pVideoFmt->isITContent = SiiDrvIpcBoolPop(mesInst);
        pVideoFmt->isActiveFormat = SiiDrvIpcBoolPop(mesInst);
        pVideoFmt->isVertBarInfoValid = SiiDrvIpcBoolPop(mesInst);
        pVideoFmt->isHorizBarInfoValid = SiiDrvIpcBoolPop(mesInst);
        pVideoFmt->vertBarInfo.lineNumEndOfTopBar = SiiDrvIpcWordPop(mesInst);
        pVideoFmt->vertBarInfo.lineNumStartOfBottomBar = SiiDrvIpcWordPop(mesInst);
        pVideoFmt->horizBarInfo.pixNumEndOfLeftBar = SiiDrvIpcWordPop(mesInst);
        pVideoFmt->horizBarInfo.pixNumStartOfRightBar = SiiDrvIpcWordPop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("VideoFmtGet", pObj, ("\n"));
    SII_LOG1B(("vidRes=%02X ", pVideoFmt->vidRes));
    SII_LOG1B(("mode3D=%02X ", pVideoFmt->mode3D));
    SII_LOG1B(("pictureAR=%02X ", pVideoFmt->pictureAR));
    SII_LOG1B(("pixelRepetition=%02X ", pVideoFmt->pixelRepetition));
    SII_LOG1B(("pixFrm.clrFmt=%02X ", pVideoFmt->pixFrm.clrFmt));
    SII_LOG1B(("pixFrm.crSmpl=%02X ", pVideoFmt->pixFrm.crSmpl));
    SII_LOG1B(("pixFrm.bitDepth=%02X ", pVideoFmt->pixFrm.bitDepth));
    SII_LOG1B(("activeAR=%02X ", pVideoFmt->activeAR));
    SII_LOG1B(("picScaling=%02X ", pVideoFmt->picScaling));
    SII_LOG1B(("scanInfo=%02X ", pVideoFmt->scanInfo));
    SII_LOG1B(("contentType=%02X ", pVideoFmt->contentType));
    SII_LOG1B(("isITContent=%02X ", pVideoFmt->isITContent));
    SII_LOG1B(("isActiveFormat=%02X ", pVideoFmt->isActiveFormat));
    SII_LOG1B(("isVertBarInfoValid=%02X ", pVideoFmt->isVertBarInfoValid));
    SII_LOG1B(("isHorizBarInfoValid=%02X ", pVideoFmt->isHorizBarInfoValid));
    SII_LOG1B(("vertBarInfo.lineNumEndOfTopBar=%04X ", pVideoFmt->vertBarInfo.lineNumEndOfTopBar));
    SII_LOG1B(("vertBarInfo.lineNumStartOfBottomBar=%04X ", pVideoFmt->vertBarInfo.lineNumStartOfBottomBar));
    SII_LOG1B(("horizBarInfo.pixNumEndOfLeftBar=%04X ", pVideoFmt->horizBarInfo.pixNumEndOfLeftBar));
    SII_LOG1B(("horizBarInfo.pixNumStartOfRightBar=%04X\n", pVideoFmt->horizBarInfo.pixNumStartOfRightBar));
}

void Sii9777AudioFmtSet( SiiInst_t inst, Sii9777AudioFrm_t *pAudioFmt )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_AUDIO_FMT_SET);
    SiiDrvIpcArrayPush(mesInst, pAudioFmt->aifData, 10);
    SiiDrvIpcArrayPush(mesInst, pAudioFmt->csData, 10);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    sIpcRespMessageGet(pObj, mesInst, SII_AUDIO_FMT_SET);
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("AudioFmtSet", pObj, ("\n"));
    {
        uint8_t *pData;
        int length;
        length = 10;
        pData = pAudioFmt->aifData;
        SII_LOG1B(("aifData: "));
        while( length-- )
        {
            SII_LOG1B(("%02X ", (uint16_t)*(pData++)));
        }
        SII_LOG1B(("\n"));
        length = 10;
        pData = pAudioFmt->csData;
        SII_LOG1B(("csData: "));
        while( length-- )
        {
            SII_LOG1B(("%02X ", (uint16_t)*(pData++)));
        }
        SII_LOG1B(("\n"));
    }
}

void Sii9777AudioFmtGet( SiiInst_t inst, Sii9777AudioFrm_t *pAudioFmt )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_AUDIO_FMT_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_AUDIO_FMT_GET) )
    {
        SiiDrvIpcArrayPop(mesInst, pAudioFmt->aifData, 10);
        SiiDrvIpcArrayPop(mesInst, pAudioFmt->csData, 10);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("AudioFmtGet", pObj, ("\n"));
    {
        uint8_t *pData;
        int length;
        length = 10;
        pData = pAudioFmt->aifData;
        SII_LOG1B(("aifData: "));
        while( length-- )
        {
            SII_LOG1B(("%02X ", (uint16_t)*(pData++)));
        }
        SII_LOG1B(("\n"));
        length = 10;
        pData = pAudioFmt->csData;
        SII_LOG1B(("csData: "));
        while( length-- )
        {
            SII_LOG1B(("%02X ", (uint16_t)*(pData++)));
        }
        SII_LOG1B(("\n"));
    }
}

void Sii9777EdidStatusGet( SiiInst_t inst, Sii9777TxPort_t port, uint16_t offset, uint8_t *pData, uint16_t length )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    SII_LOG1A("EdidStatusGet", pObj, ("port=%d, offset=%04X, length=%d\n", (uint16_t)port, offset, length));
    if( sInvalidTxEdidParameters(port, offset, length) )
    {
        return;
    }

    sIpcOpCodeSet(pObj, mesInst, SII_EDID_STAT_GET);
    SiiDrvIpcBytePush(mesInst, (uint8_t)port);
    SiiDrvIpcWordPush(mesInst, offset);
    SiiDrvIpcWordPush(mesInst, length);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_EDID_STAT_GET) )
    {
        SiiDrvIpcArrayPop(mesInst, pData, length);
    }
    SiiDrvIpcMessageDelete(mesInst);
    while( length-- )
    {
        SII_LOG1B(("%02X ", (uint16_t)*(pData++)));
    }
}

#ifdef SII977X_LIB_ES0_TX
void Sii9777HpdStatusGet( SiiInst_t inst, Sii9777TxPort_t port, bool_t *pbHpd )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_HPD_STAT_GET);
    SiiDrvIpcBytePush(mesInst, (uint8_t)port);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_HPD_STAT_GET) )
    {
        *pbHpd = SiiDrvIpcBoolPop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("HpdGet", pObj, ("port=%d, %d\n", port, *pbHpd));
}
#endif

void Sii9777HdcpContentTypeStatusGet( SiiInst_t inst, Sii9777HdcpContentType_t* pContentType )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_HDCP_CONTENT_TYPE_STAT_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_HDCP_CONTENT_TYPE_STAT_GET) )
    {
        *pContentType = (Sii9777HdcpContentType_t)SiiDrvIpcBytePop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("HdcpContentTypeStatusGet", pObj, ("%d\n", *pContentType));
}

#ifdef SII977X_LIB_ES0_RX
void Sii9777HpdStatusGet( SiiInst_t inst, Sii9777RxPort_t port, bool_t *pbHpd )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_HPD_STAT_GET);
    SiiDrvIpcBytePush(mesInst, (uint8_t)port);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_HPD_STAT_GET) )
    {
        *pbHpd = SiiDrvIpcBoolPop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("HpdStatusGet", pObj, ("port=%d, %d\n", port, *pbHpd));
}
#endif

void Sii9777DualLinkStatusGet( SiiInst_t inst, bool_t* bDualLink )
{
    Obj_t*              pObj    = (Obj_t*)SII_SYS_OBJ_CHECK(sObjListInst, inst);
    SiiDrvIpcMesInst_t  mesInst = SiiDrvIpcMessageCreate();

    sIpcOpCodeSet(pObj, mesInst, SII_DUAL_LINK_STAT_GET);
    SiiDrvIpcMessageSend(mesInst);
    SiiDrvIpcMessageClear(mesInst);

    /* wait for response from 8051 firmware and check result */
    if( sIpcRespMessageGet(pObj, mesInst, SII_DUAL_LINK_STAT_GET) )
    {
        *bDualLink = SiiDrvIpcBoolPop(mesInst);
    }
    SiiDrvIpcMessageDelete(mesInst);
    SII_LOG1A("DualLinkStatusGet", pObj, ("DualLink=%d\n", *bDualLink));
}

/***** local functions *******************************************************/

static void sHandleFlashProg( Obj_t* pObj )
{
    if( SiiDrvIspOperationDone() )
    {
        pObj->siiLibEvtCbFunc(SII_OBJ2INST(pObj), SII9777_EVENT_FLAGS__FLASH_DONE);
    }
}

static void sHandleInterrupts( Obj_t* pObj )
{
    SiiDrvIpcIsrFlags_t intFlags = 0;

    /* Handle IPC interrupts */
    while( true )
    {
        SiiDrvIpcIsrFlags_t intReg = (SiiDrvIpcIsrFlags_t)SiiDrvCraRead8(pObj->devId, 0x7062);
        if( 0 == intReg )
            break;
		
        /* Clear new interrupt Flags */
        SiiDrvCraWrite8(pObj->devId, 0x7062, intReg);

        /* Collect interrupt Flags */
        intFlags |= intReg;
    }

    if( intFlags )
    {
        SiiDrvIpcHandler(intFlags);
        sTimeOutReset(pObj);
    }
}

static void sIpcOpCodeSet( Obj_t* pObj, SiiDrvIpcMesInst_t mesInst, SiiDrvIpcOpCode_t opCode )
{
    SiiDrvIpcTypeSet(mesInst, SII_DRV_IPC_TYPE__REQUEST);
    SiiDrvIpcOpCodeSet(mesInst, opCode);
    pObj->tagCode++;
    SiiDrvIpcTagCodeSet(mesInst, pObj->tagCode);
}

static void sTimeOutReset( Obj_t* pObj )
{
    SiiSysTimeOutMilliSet(&pObj->timeOut, 4000); /* 4 second time out */
}

static bool_t sIpcRespMessageGet( Obj_t* pObj, SiiDrvIpcMesInst_t mesInst, SiiDrvIpcOpCode_t opCode )
{
    bool_t bResult = false;
    
    /* Wait for respondse message from slave */
    sTimeOutReset(pObj); /* 4 second time out */
    while( !pObj->bMessageReceived && !SiiSysTimeOutMilliIs(&pObj->timeOut) )
    {
    	printf("sleep 100ms\n");
        sHandleSemaphoreTake(pObj);
        sHandleInterrupts(pObj);
        sHandleSemaphoreRelease(pObj);
        SiiPlatformSleepMsec(100);
    }
    printf("break while  \n");   
    /* Check for valid response message */
    if( pObj->bMessageReceived )
    {
        SiiDrvIpcMessageReceive(mesInst);	// fill in received message
        if( SII_DRV_IPC_TYPE__RESPONSE == SiiDrvIpcTypeGet(mesInst) )
        {
            if( opCode == SiiDrvIpcOpCodeGet(mesInst) )
            {
                if( pObj->tagCode == SiiDrvIpcTagCodeGet(mesInst) )
                {
                    if( RESULT_CODE__CMD_SUCCESS == SiiDrvIpcBytePop(mesInst) )
                    {
                        bResult = true;
                    }
                }
            }
        }
        pObj->bMessageReceived = false;
    }
	else
	{
		printf("time out and return\n");
	}
    return bResult;
}

static bool_t sInvalidRxEdidParameters( Sii9777RxPort_t port, uint16_t offset, uint16_t length )
{
    if( 255 < offset )
    {
        SII_LOG1B(("Error: offset > 255\n"));
        return true;
    }

    if( 256 < (length+offset) )
    {
        SII_LOG1B(("Error: (offset+length) > 255\n"));
        return true;
    }

    switch(port )
    {
#ifdef SII9777_REF_ES1
        case SII9777_RX_PORT__3 :
        case SII9777_RX_PORT__2 :
#endif
        case SII9777_RX_PORT__1 :
        case SII9777_RX_PORT__0 : break;

        default :
            SII_LOG1B(("Error: unknown port\n"));
            return true;
    }

    return false;
}

static bool_t sInvalidTxEdidParameters( Sii9777TxPort_t port, uint16_t offset, uint16_t length )
{
    if ( 255 < offset )
    {
        SII_LOG1B(("Error: offset > 255\n"));
        return true;
    }

    if ( 256 < (length+offset) )
    {
        SII_LOG1B(("Error: (offset+length) > 255\n"));
        return true;
    }

    switch ( port )
    {
        case SII9777_TX_PORT__2 :
        case SII9777_TX_PORT__1 :
        case SII9777_TX_PORT__0 : break;

        default :
            SII_LOG1B(("Error: unknown port\n"));
            return true;
    }

    return false;
}

static void sHandleSemaphoreTake( Obj_t* pObj )
{
//    while( pObj->bSemaphore )
//    {
//        SiiPlatformSleepMsec(10);
//    }
    pObj->bSemaphore = true;
}

static void sHandleSemaphoreRelease( Obj_t* pObj )
{
    pObj->bSemaphore = false;
}

/***** end of file ***********************************************************/
