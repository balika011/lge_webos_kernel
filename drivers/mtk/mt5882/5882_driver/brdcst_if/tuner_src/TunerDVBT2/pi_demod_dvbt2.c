/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2008, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file pi_demod.c
 *  DvbT2 demod related API implementation
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "t2_com_defs.h"

#include "pi_dvbt2_if.h"
#include "pd_common.h"
//#include "com_defs.h"
#include "pi_demod.h"


#include "PD_Def_dbg.h"  /*need include RISC IF macro, if re-ware RISC IF , remove this*/
#include "x_hal_5381.h"
#include "ctrl_bus.h"
#include "spi_if.h"
#include "tuner_interface_if.h"
#include "drvcust_if.h"
#include "code_img_MT5136T2.h"

#include "eeprom_if.h"
#include "pd_API.h"





//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define fcToolx_I2C           0
//#define fcADD_REG_RESET         1
//#define fcCHK_TUNER_PLL         0
#define fcUP_50M_CLK            1
//#define fcPLL_POLLING_TIME      10          //  in ms
#define fcMEASURE_DRIVER_TIME   0
#define fcLOAD_CODE_TIME        (0 && fcINTERNAL_TEST)



//-----------------------------------------------------------------------------
//static void  vSetIntfPara(DEMOD_CTX_T2 *psDemodCtx, UCHAR *para, UCHAR len); // LL
//void  vSetChSearchMode(DEMOD_CTX_T2 *psDemodCtx);

// Configurations
//-----------------------------------------------------------------------------
#define fcMEAS_HOST_CMD_TIME    (0 && fcINTERNAL_TEST)
#define fcSKIP_LOAD_CODE        (0 && fcINTERNAL_TEST)
#define fcADD_HOSTCMD_CHKSUM    0

UINT8 ucCodeImage_MT5136T2[] =
{
//#include "CodeImg.h"
 00,00
};



//Only for Sony si Tuner
#define C_RE201_AGC_IF_SLP              -1.0
#define C_RE201_AGC_IF_INT              0.0
#define C_RE201_AGC_IF_MAX              0.499  // 0.5 will overflow  //0v
#define C_RE201_AGC_IF_MIN              -0.29  //2.6v
#define C_RE201_AGC_IF_SLP_SGN          (S8)((C_RE201_AGC_IF_SLP>0)?1:-1) 
#define C_RE201_AGC_IF_LVL_MAX          0.4774    //-90dBm    127/256
#define C_RE201_AGC_IF_LVL_MIN          0.08203125   //-60dBm  21/256
#define C_U8_RE201_AGC_IF_SLP           (S8)(C_RE201_AGC_IF_SLP *32 )
#define C_U8_RE201_AGC_IF_INT           (S8)(C_RE201_AGC_IF_INT *64 )
#define C_U8_RE201_AGC_IF_MAX           (S8)(C_RE201_AGC_IF_MAX *256)
#define C_U8_RE201_AGC_IF_MIN           (S8)(C_RE201_AGC_IF_MIN *256)
#define C_U8_RE201_AGC_IF_LVL_MAX       (S8)(C_RE201_AGC_IF_LVL_MAX *256)
#define C_U8_RE201_AGC_IF_LVL_MIN       (S8)(C_RE201_AGC_IF_LVL_MIN *256)



UINT32 DVBT2_LOG10(UINT32 u4InputVale)
{
    UINT32 u4TempValue = u4InputVale;
    UINT32 u4ReturnValue = 0;
    UINT8  u1Index = 0;

    while(u4TempValue>=10)
    {
        u4TempValue = u4TempValue/10;
        u1Index++;
    }
    switch(u4TempValue)
    {
        case 0:
        case 1:
            u4ReturnValue = u1Index*100;
            break;
        case 2: 
            u4ReturnValue = u1Index*100+30;
            break;
        case 3: 
            u4ReturnValue = u1Index*100+48;
            break;
        case 4: 
            u4ReturnValue = u1Index*100+60;
            break;
        case 5: 
            u4ReturnValue = u1Index*100+70;
            break;
        case 6: 
            u4ReturnValue = u1Index*100+78;
            break;
        case 7: 
            u4ReturnValue = u1Index*100+85;
            break;
        case 8: 
            u4ReturnValue = u1Index*100+90;
            break;
        case 9: 
            u4ReturnValue = u1Index*100+95;
            break;
    }
    
    return u4ReturnValue;/* The Return Value Enlarge 100*/
}



//----------------------------------------------------------------------------- 
/*
 *  DVBT2_DemodCtxCreate
 *  This function creates demod context and returns the context pointer.
 *  @param  void.
 *  @retval   DEMOD_CTX_T2 * : pointer of demod context.   
 */
//-----------------------------------------------------------------------------
DEMOD_CTX_T2 *DVBT2_DemodCtxCreate(void)
{
    DEMOD_CTX_T2 *p = NULL;

    CREATE_OBJECT(p, DEMOD_CTX_T2);

    // error handling
    if (!p)
       return NULL;
    
    // zero memory
    memset(p, 0, sizeof(DEMOD_CTX_T2));
    
    return p;
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_DemodCtxDestroy
 *  This function destroys context you have created and releases the memory.
 *  @param  p  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   void.   
 */
//-----------------------------------------------------------------------------
void DVBT2_DemodCtxDestroy(DEMOD_CTX_T2 *p)
{
    DESTROY_OBJECT(p);
}

#if fcToolx_I2C
#define mcSET_REG_PAGE(ucAddr)              \
{                                           \
    ucAddr = mcHIGH_BYTE(u2RegAddr);        \
    /*    if (ucPrevPage != ucAddr) */      \
    {                                       \
    /*    ucPrevPage = ucAddr;  */          \
        if (ucI2cWrite(u1I2cAddr, MT5136T2_REG_PAGE_NUM, &ucAddr, 1))   \
            u1RetSts = 1;                   \
    }                                       \
}
#else
#define mcSET_REG_PAGE(ucAddr)
#endif

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_GetDemodReg
 *  This function will read demod registers through host command.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  u2RegAddr  Demod register address (16-bit).
 *  @param  pucBuf  Pointer of data buffer.
 *  @param  ucLen  Total bytes to be written. (<=4)
 *  @retval   1 : I2C error.
 *  @retval   0 : I2C OK.   
 */
//-----------------------------------------------------------------------------
UCHAR DVBT2_GetDemodReg(DEMOD_CTX_T2 *psDemodCtx, UINT16 u2Addr, UCHAR *pucBuf, UCHAR ucLen)
{
UCHAR   aucData[cMAX_READ_NUM + 1];
UCHAR   ii = 0;

    if (!psDemodCtx)
        return 1;
    if ( (ucLen == 0) || (ucLen > 4) )
        return 1;
    if (!pucBuf)
        return 1;

    mccSET_CMD2(aucData) = ccCMD_ID_DEMOD_GET_REG;
    mccSET_QUEUE2(aucData, ccOFST_REG_NUM) = ucLen;
    mccSET_QUEUE2(aucData, ccOFST_REG1_ADDR_L) = mcLOW_BYTE( u2Addr);
    mccSET_QUEUE2(aucData, ccOFST_REG1_ADDR_H) = mcHIGH_BYTE(u2Addr);
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_REG1_SIZE + ucLen - 1, ccOFST_REG1_SIZE+1))
    {
        for (ii = 0; ii < ucLen; ii++)
             *(pucBuf+ii) = mccGET_QUEUE(aucData, ccOFST_REG1_DATA+ii);
         return 0;
    }
    
    return 1;
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_SetDemodReg
 *  This function will write demod registers through host command.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  u2RegAddr  Demod register address (16-bit).
 *  @param  pucBuf  Pointer of data buffer.
 *  @param  ucLen  Total bytes to be written. (<=4)
 *  @retval   1 : I2C error.
 *  @retval   0 : I2C OK.   
 */
//-----------------------------------------------------------------------------
UCHAR DVBT2_SetDemodReg(DEMOD_CTX_T2 *psDemodCtx, UINT16 u2Addr, UCHAR *pucBuf, UCHAR ucLen)
{
UCHAR   aucData[cMAX_READ_NUM + 1];
UCHAR   ii = 0;

    if (!psDemodCtx)
        return 1;
    if ( (ucLen == 0) || (ucLen > 4) )
        return 1;
    if (!pucBuf)
        return 1;

    mccSET_CMD2(aucData) = ccCMD_ID_DEMOD_SET_REG;
    mccSET_QUEUE2(aucData, ccOFST_REG_NUM) = ucLen;
    mccSET_QUEUE2(aucData, ccOFST_REG1_ADDR_L) = mcLOW_BYTE( u2Addr);
    mccSET_QUEUE2(aucData, ccOFST_REG1_ADDR_H) = mcHIGH_BYTE(u2Addr);
    for (ii = 0; ii < ucLen; ii++)
        mccSET_QUEUE2(aucData, ccOFST_REG1_DATA+ii) = *(pucBuf+ii);

    if (mcHOST_CMD(psDemodCtx, aucData, 0, ccOFST_REG1_SIZE+ucLen-1+1))
         return 0;
        
    return 1;
}

//----------------------------------------------------------------------------- 
/*
 *  u1I2cDemWrite
 *  This function will write demod registers.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  u1I2cAddr  Demod I2C address.
 *  @param  u2RegAddr  Demod register address (16-bit).
 *  @param  pu1Buffer  Pointer of data buffer.
 *  @param  u2ByteCount  Total bytes to be written.
 *  @retval   1 : I2C error.
 *  @retval   0 : I2C OK.   
 */
//-----------------------------------------------------------------------------
UINT8 u1I2cDemWrite(DEMOD_CTX_T2 *psDemodCtx, UINT8 u1I2cAddr, UINT16 u2RegAddr, UINT8 *pu1Buffer, UINT16 u2ByteCount)
{
#if fcToolx_I2C
    UINT8 u1RegAddr;
    UINT8 u1RetSts = 0;
    
    mcSET_REG_PAGE(u1RegAddr);
    u1RegAddr = mcLOW_BYTE(u2RegAddr);

    if (ucI2cWrite(u1I2cAddr, u1RegAddr, pu1Buffer, u2ByteCount))
    {
        u1RetSts = 1;
    }

    return u1RetSts;
#else
    return 0;
#endif
}

//----------------------------------------------------------------------------- 
/*
 *  u1I2cDemRead
 *  This function will read demod registers.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  u1I2cAddr  Demod I2C address.
 *  @param  u2RegAddr  Demod register address (16-bit).
 *  @param  pu1Buffer  Pointer of data buffer.
 *  @param  u2ByteCount  Total bytes to be written.
 *  @retval   1 : I2C error.
 *  @retval   0 : I2C OK.   
 */
//-----------------------------------------------------------------------------
UINT8 u1I2cDemRead(DEMOD_CTX_T2 *psDemodCtx, UINT8 u1I2cAddr, UINT16 u2RegAddr, UINT8 *pu1Buffer, UINT16 u2ByteCount)
{

#if fcToolx_I2C
    UINT8 u1RegAddr;
    UINT8 u1RetSts = 0;

    mcSET_REG_PAGE(u1RegAddr);
    u1RegAddr = mcLOW_BYTE(u2RegAddr);

    if (ucI2cRead(u1I2cAddr, u1RegAddr, pu1Buffer, u2ByteCount))  
    {
        u1RetSts = 1;
    }  
    return u1RetSts;
#else
    return 0;
#endif
}

/***********************************************************************/
/*                  Private (static) Function Prototypes               */
/***********************************************************************/
//----------------------------------------------------------------------------- 
/*
 *  MT5136_DVBT2_SetReg
 *  This function set demod register.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  u2RegAddr  Demod register address (16-bit).
 *  @param  pu1Buffer  Pointer of data buffer.
 *  @param  u2ByteCount  Total bytes to be written.
 *  @retval   1 : I2C error.
 *  @retval   0 : I2C OK.   
 */
//-----------------------------------------------------------------------------
UINT8 MT5136_DVBT2_SetReg(DEMOD_CTX_T2 *psDemodCtx, UINT16 u2RegAddr, UINT8 *pu1Buffer, UINT16 u2ByteCount)
{

#if fcToolx_I2C
    if (u1I2cDemWrite(psDemodCtx, psDemodCtx->I2cAddress, u2RegAddr, pu1Buffer, u2ByteCount))
#else    
    if(ICtrlBus_DemodWrite(psDemodCtx->I2cAddress, u2RegAddr, pu1Buffer, u2ByteCount))      
#endif        
    {
        mcSHOW_HW_MSG((" [MT5136_DVBT2_SetReg]_@IF_IIC Bus Write Error: %02X-%03X!\n",
                       psDemodCtx->I2cAddress, u2RegAddr));
        return 1;
    }

    return 0;
}

//----------------------------------------------------------------------------- 
/*
 *  MT5136_DVBT2_GetReg
 *  This function get demod register.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  u2RegAddr  Demod register address (16-bit).
 *  @param  pu1Buffer  Pointer of data buffer.
 *  @param  u2ByteCount  Total bytes to be written.
 *  @retval   1 : I2C error.
 *  @retval   0 : I2C OK.   
 */
//-----------------------------------------------------------------------------
UINT8 MT5136_DVBT2_GetReg(DEMOD_CTX_T2 *psDemodCtx, UINT16 u2RegAddr, UINT8 *pu1Buffer, UINT16 u2ByteCount)
{

#if fcToolx_I2C
    if (u1I2cDemRead(psDemodCtx, psDemodCtx->I2cAddress, u2RegAddr, pu1Buffer, u2ByteCount))
#else
    if(ICtrlBus_DemodRead(psDemodCtx->I2cAddress, u2RegAddr, pu1Buffer, u2ByteCount))
#endif
    {
        mcSHOW_DBG_MSG((" _@IF_IIC Bus Read Error: %02X-%02X!\n",
                       psDemodCtx->I2cAddress, u2RegAddr));
        return 1;
    }

    return 0;
}

UINT8 DVBT2_SetReg(DEMOD_CTX_T2 *psDemodCtx, UINT16 u2RegAddr, UINT8 *pu1Buffer, UINT16 u2ByteCount)
{

#if fcToolx_I2C
    if (u1I2cDemWrite(psDemodCtx, psDemodCtx->I2cAddress, u2RegAddr, pu1Buffer, u2ByteCount))
#else
    if(ICtrlBus_DemodWrite(psDemodCtx->I2cAddress, u2RegAddr, pu1Buffer, u2ByteCount))
#endif
    {
        mcSHOW_HW_MSG((" [DVBT2_SetReg]_@IF_IIC Bus Write Error: %02X-%03X!\n",
                       psDemodCtx->I2cAddress, u2RegAddr));
        return 1;
    }

    return 0;
}

UINT8 DVBT2_GetReg(DEMOD_CTX_T2 *psDemodCtx, UINT16 u2RegAddr, UINT8 *pu1Buffer, UINT16 u2ByteCount)
{

#if fcToolx_I2C
    if (u1I2cDemRead(psDemodCtx, psDemodCtx->I2cAddress, u2RegAddr, pu1Buffer, u2ByteCount))
#else
    if(ICtrlBus_DemodRead(psDemodCtx->I2cAddress, u2RegAddr, pu1Buffer, u2ByteCount))
#endif
    {
        mcSHOW_DBG_MSG((" _@IF_IIC Bus Read Error: %02X-%02X!\n",
                       psDemodCtx->I2cAddress, u2RegAddr));
        return 1;
    }

    return 0;
}

/************************************************************************/
#define cMAX_TRY_CNT            2
#define cMAX_WAIT_LOOP          10
//#define cWAIT_TIME_INTERVAL     200         // ms
#define cWAIT_TIME_INTERVAL     10         // ms

//----------------------------------------------------------------------------- 
/*
 *  fgHostCmd
 *  Host command interface API between host and demod.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  aucCmdData  Data buffer.
 *  @param  ucReadParacnt  Read parameter byte counts.
 *  @param  ucWriteCmdcnt  Write command byte counts.
 *  @retval   TRUE : Host command success.
 *  @retval   FALSE : Host command fail.   
 */
//-----------------------------------------------------------------------------
BOOL fgHostCmd(DEMOD_CTX_T2 *psDemodCtx, UCHAR *aucCmdData, UCHAR ucReadParacnt, UCHAR ucWriteCmdcnt)
{
    UCHAR   ucWriteParacnt;
    UINT16  ii;
    UINT8   ucValue;
    UCHAR   ucParaStart, ucRegSetOfst;
    BOOL    fgRetStatus = TRUE;
#if fcMEAS_HOST_CMD_TIME
    UINT32  u4TickStart;
    UINT32  u4TickCnt;
#endif
    UCHAR   ucReadChkCnt;
    //#if fcCHIP_TYPE
    //UCHAR   ucChipID;
    //#endif
#if fcADD_HOSTCMD_CHKSUM
    UCHAR   ucChkSum;
#endif
    UCHAR   jj;

#if fcMEAS_HOST_CMD_TIME
    u4TickStart = mcGET_SYS_TICK();
#endif

    if (MT5136_DVBT2_GetReg(psDemodCtx, MT5136T2_REG_uP_Ctrl, &ucValue, 1))
    {
        //psDemodCtx->u1I2cNakCnt ++;
        mcSHOW_HW_MSG((" _I2_ "));
        mcSHOW_HW_MSG(("Cmd%02x ", mccSET_CMD2(aucCmdData)));
        return FALSE;                       // I2C fail
    }
	
    if (!mcTEST_BIT(ucValue, ccpUP_ENABLE))
    {
        psDemodCtx->u1UpChkCnt ++;
		mcSHOW_HW_MSG(("Cmd %02X Para !", mccSET_CMD2(aucCmdData)));
        //mcSHOW_HW_MSG(("uP not enabled!\n"));
        mcSHOW_HW_MSG((" _UE_ "));
        
        return FALSE;
    }
	
    if (MT5136_DVBT2_GetReg(psDemodCtx, MT5136T2_REG_uP_GP_REG06, &ucValue, 1))
    {       
        return FALSE;                       // I2C fail
    }
	
    if (ucValue != ccFW_RUN_TAG)
    {
        psDemodCtx->u1UpChkCnt ++;
        if (ucValue == ccFW_WDT_TAG)
        {
            //mcSHOW_HW_MSG(("Watchdog reset!\n"));
            mcSHOW_HW_MSG((" _WR_ "));
        }
        else
        {
            //mcSHOW_HW_MSG(("Fw not running!\n"));
            mcSHOW_HW_MSG((" _FR_ "));
        }
        
        return FALSE;
    }
    
    //#if fcCHIP_TYPE
    //    // Get Chip Type
    //    MT5136_DVBT2_GetReg(psDemodCtx, MT5136T2_REG_Chip_ID, &ucChipID, 1);
    //    ucChipID &= mcMASKS(4, 4);
    //#endif
    
    if (ucWriteCmdcnt)
    {
        ucWriteParacnt = ucWriteCmdcnt - 1;
        for (ii = 0; ii < cMAX_WAIT_LOOP; ii ++)
        {
#if 1
            MT5136_DVBT2_GetReg(psDemodCtx, MT5136T2_REG_uP_CMDR_CFG, &ucValue, 1);
#else
            if (DVBT2_GetReg(psDemodCtx, MT5136T2_REG_uP_CMDR_CFG, &ucValue, 1))
            {                               // I2C fail
                if (ii < (cMAX_WAIT_LOOP - 1))
                {
                    mcDELAY_MS(cWAIT_TIME_INTERVAL);
                    continue;
                }                
                return FALSE;
            }
#endif
            // This while loop will cause dead-lock for Combo-Tuner (when I2C_Block = TRUE)
#if 1
            jj = 0;
            while (mcTEST_BIT(ucValue, ccpUP_RSP_TX))
            {
                //#if fcCHIP_TYPE
                //    if (ucChipID == CHIP_TYPE_1)
                //        MT5136_DVBT2_GetReg(psDemodCtx, MT5136T2_REG_RFA4CF, &ucValue, 1);
                //    else
                //#endif
                MT5136_DVBT2_GetReg(psDemodCtx, MT5136T2_REG_RSICIF_RSP7, &ucValue, 1);
                MT5136_DVBT2_GetReg(psDemodCtx, MT5136T2_REG_uP_CMDR_CFG, &ucValue, 1);
                jj ++;
                if (jj >= cMAX_WAIT_LOOP)
                    break;
            }
#endif
            if (!mcTEST_BIT(ucValue, ccpUP_CMD_RX))
                break;
            mcDELAY_MS(cWAIT_TIME_INTERVAL);
        }

#if fcMEAS_HOST_CMD_TIME
        u4TickCnt = mcGET_SYS_TICK() - u4TickStart;
        mcSHOW_TIME_MSG((" ** Cmd%02X %d (%3u)", mccSET_CMD2(aucCmdData), ii, u4TickCnt * mcGET_TICK_PERIOD()));
#endif

        if (ucWriteParacnt > ccCMD_REG_NUM - 1)
        {
            mcSHOW_HW_MSG(("Cmd %02X Para Cnt %d > %d!",
                            mccSET_CMD2(aucCmdData), ucWriteParacnt, ccCMD_REG_NUM - 1));
            ucWriteParacnt = ccCMD_REG_NUM - 1;
        }

#if fcADD_HOSTCMD_CHKSUM
        if (ucWriteParacnt == (ccCMD_REG_NUM - 1))
        {
            mcSHOW_HW_MSG(("Cmd %02X Para Cnt %d == %d!",
                            mccSET_CMD2(aucCmdData), ucWriteParacnt, ccCMD_REG_NUM - 1));
        }
        else
        {
            ucChkSum = 0;
            for (ii = 0; ii < ccCMD_REG_NUM - 1; ii ++)
            {
                ucChkSum ^= aucCmdData[ccCMD_REG_NUM - 1 - ii + 1];
            }
            aucCmdData[1] = ucChkSum;
            ucWriteParacnt = ccCMD_REG_NUM - 1;
        }
#endif

        if (ii == cMAX_WAIT_LOOP)
            psDemodCtx->u1UpNakCnt ++;
    
        ucParaStart = ccCMD_REG_NUM - ucWriteParacnt - 1;
        ucRegSetOfst = 1;

        //#if fcCHIP_TYPE
        //    if (ucChipID == CHIP_TYPE_1)
        //        MT5136_DVBT2_SetReg(psDemodCtx, MT5136T2_REG_RFA4C0 + ucParaStart,
        //                      aucCmdData + ucParaStart + ucRegSetOfst,
        //                      ucWriteParacnt + 1);
        //    else
        //#endif
        MT5136_DVBT2_SetReg(psDemodCtx, (UINT16) (MT5136T2_REG_RSICIF_CMD0 + ucParaStart),
                      aucCmdData + ucParaStart + ucRegSetOfst,
                      ucWriteParacnt + 1);

#if fcMEAS_HOST_CMD_TIME
        u4TickCnt = mcGET_SYS_TICK() - u4TickStart;
        mcSHOW_TIME_MSG((" Para %d (%3u)", ucWriteParacnt, u4TickCnt * mcGET_TICK_PERIOD()));
#endif

#if fcSHOW_HOST_CMD
        if (fgShowHostCmd)
        {
            //mcSHOW_USER_MSG(("HostCmd: (%d) -->", ucParaStart));
            mcSHOW_USER_MSG(("HostCmd -->"));
            for (ii = 0; ii < ucWriteParacnt + 1; ii ++)
                mcSHOW_USER_MSG((" %02X", aucCmdData[ucParaStart + ucRegSetOfst + ii]));
            mcSHOW_USER_MSG(("\n           "));
            MT5136_DVBT2_GetReg(psDemodCtx, MT5136T2_REG_uP_CMDR00 + ucParaStart,
                      aucCmdData + ucParaStart + ucRegSetOfst,
                      ucWriteParacnt + 1);
            for (ii = 0; ii < ucWriteParacnt + 1; ii ++)
                mcSHOW_USER_MSG((" %02X", aucCmdData[ucParaStart + ucRegSetOfst + ii]));
            mcSHOW_USER_MSG(("\n"));
        }
#endif
    }

    if (ucReadParacnt)
    {
        UCHAR   ucCmdId;

        ucCmdId = mccSET_CMD2(aucCmdData);
        fgRetStatus = FALSE;
        for (jj = 0; jj < cMAX_TRY_CNT; jj ++)
        {
            for (ii = 0; ii < cMAX_WAIT_LOOP; ii ++)
            {
#if 1
                MT5136_DVBT2_GetReg(psDemodCtx, MT5136T2_REG_uP_CMDR_CFG, &ucValue, 1);
#else
                if (DVBT2_GetReg(psDemodCtx, MT5136T2_REG_uP_CMDR_CFG, &ucValue, 1))
                {                           // I2C fail
                    if (ii < (cMAX_WAIT_LOOP - 1))
                    {
                        mcDELAY_MS(cSHORT_SLEEP_TIME);
                        continue;
                    }
                    return FALSE;
                }
#endif
                if (mcTEST_BIT(ucValue, ccpUP_RSP_TX))
                    break;
                mcDELAY_MS(cSHORT_SLEEP_TIME);
            }

            //Get(MT5136T2_REG_uP_RSPR00 + cPARA_NUM_POS, &ucReadParacnt, 1);
            if (ucReadParacnt > ccCMD_REG_NUM - 1)
            {
                mcSHOW_HW_MSG(("Rsp %02X Para Cnt %d > %d!",
                                mccSET_CMD2(aucCmdData), ucReadParacnt, ccCMD_REG_NUM - 1));
                ucReadParacnt = ccCMD_REG_NUM - 1;
            }
            ucReadChkCnt = ucReadParacnt;

#if fcADD_HOSTCMD_CHKSUM
            if (ucReadChkCnt < ccCMD_REG_NUM - 1)
                ucReadChkCnt ++;
#endif

            if (ii < cMAX_WAIT_LOOP)
            {
                UINT16  u2AddrOfst = 0;

#if fcMEAS_HOST_CMD_TIME
                u4TickCnt = mcGET_SYS_TICK() - u4TickStart;
                mcSHOW_TIME_MSG((" Rsp %d_%d (%3u)", jj, ii, u4TickCnt * mcGET_TICK_PERIOD()));
#endif
                ucParaStart = ccCMD_REG_NUM - ucReadChkCnt - 1;

                //#if fcCHIP_TYPE
                //    if (ucChipID == CHIP_TYPE_1)
                //        u2AddrOfst = MT5136T2_REG_RFA4C8 - MT5136T2_REG_uP_RSPR00;
                //#endif
                
                if (MT5136_DVBT2_GetReg(psDemodCtx,
                                  (UINT16) (MT5136T2_REG_RSICIF_RSP0 + u2AddrOfst + ucParaStart),
                                  aucCmdData + ucParaStart, ucReadChkCnt + 1))
                {
                    return FALSE;
                }

#if fcMEAS_HOST_CMD_TIME
                u4TickCnt = mcGET_SYS_TICK() - u4TickStart;
                mcSHOW_TIME_MSG((" Para %d (%3u)", ucReadChkCnt, u4TickCnt * mcGET_TICK_PERIOD()));
#endif

                if (mccGET_CMD(aucCmdData) == ucCmdId)
                {
                    fgRetStatus = TRUE;
#if fcSHOW_HOST_CMD
                    if (fgShowHostCmd)
                    {
                        mcSHOW_USER_MSG(("HostRsp -->"));
                        for (ii = 0; ii < ucReadChkCnt + 1; ii ++)
                            mcSHOW_USER_MSG((" %02X", aucCmdData[ucParaStart + ii]));
                        mcSHOW_USER_MSG(("\n"));
                    }
#endif

#if fcADD_HOSTCMD_CHKSUM
                    if (ucReadParacnt < ccCMD_REG_NUM - 1)
                    {
                        ucChkSum = 0;
                        for (ii = 0; ii < ucReadChkCnt + 1; ii ++)
                        {
                            ucChkSum ^= aucCmdData[ccCMD_REG_NUM - 1 - ii];
                        }
                        if (ucChkSum)
                        {
                            fgRetStatus = FALSE;
                            mcSHOW_HW_MSG((" _@CS_ "));
                            MT5136_DVBT2_GetReg(psDemodCtx, MT5136T2_REG_uP_RSPR00 + ucParaStart,
                                          aucCmdData + ucParaStart, ucReadChkCnt + 1);
                            mcSHOW_USER_MSG(("-->"));
                            for (ii = 0; ii < ucReadChkCnt + 1; ii ++)
                                mcSHOW_USER_MSG((" %02X", aucCmdData[ucParaStart + ii]));
                            mcSHOW_USER_MSG(("\n"));
                            psDemodCtx->u1UpMisCnt ++;
                        }
                    }
#endif
                    break;
                }
                else
                    psDemodCtx->u1UpMisCnt ++;
            }
            else
            {
                psDemodCtx->u1UpNakCnt ++;
                break;
            }
        }
            //mcSHOW_HW_MSG(("Error uP Rsp!"));
    }

#if fcMEAS_HOST_CMD_TIME
    u4TickCnt = mcGET_SYS_TICK() - u4TickStart;
    mcSHOW_TIME_MSG((" %3uH ", u4TickCnt * mcGET_TICK_PERIOD()));
    mcSHOW_TIME_MSG(("\n"));
#endif

    return fgRetStatus;
}

#if 0
void vGetAsicVer(DEMOD_CTX_T2 *psDemodCtx)
{
    UCHAR   aucData[cMAX_READ_NUM + 1];         // For Host Cmd

    MT5112_GetReg(psDemodCtx, MT5112_REG_Chip_ID, aucData, 1);
    mcSHOW_USER_MSG(("MT5112 ASIC Ver %d.%d",
                    mcGET_FIELD_CMB(aucData[0], CHIP_ID_GEN) + 1,   // Chip Generation, 0 = MT5111
                    mcGET_FIELD_CMB(aucData[0], CHIP_ID_MASK) + 1));// Mask Rev., 1 = BO62350A

    mccSET_CMD2(aucData) = cCMD_ID_GET_LABEL;
    if (mcHOST_CMD(psDemodCtx, aucData, cOFST_LABEL_SIZE, 1))
    {
        mcSHOW_USER_MSG((" %c%02X%02X-%02X%02X",
                        mcGET_QUEUE(aucData, cOFST_LABEL_BRANCH_TYPE), 
                        mcGET_QUEUE(aucData, cOFST_LABEL_BRANCH_ID_1), mcGET_QUEUE(aucData, cOFST_LABEL_BRANCH_ID_2),
                        mcGET_QUEUE(aucData, cOFST_LABEL_ID_1), mcGET_QUEUE(aucData, cOFST_LABEL_ID_2)));
    }

    mccSET_CMD2(aucData) = cCMD_ID_GET_VER;
    if (mcHOST_CMD(psDemodCtx, aucData, cOFST_VER_SIZE, 1))
    {
        mcSHOW_USER_MSG((" %02X%02X-%02X%02X\n",
                         mcGET_QUEUE(aucData, cOFST_VER_YEAR_MONTH), mcGET_QUEUE(aucData, cOFST_VER_DATE),
                         mcGET_QUEUE(aucData, cOFST_VER_HOUR), mcGET_QUEUE(aucData, cOFST_VER_MINUTE)));
    }

}
#endif

#if 0
/**********************************************************************/
BOOL fgGetRegs(DEMOD_CTX_T2 *psDemodCtx, UINT16 u2Addr, UCHAR *pucData, UCHAR ucCnt)
{

UCHAR   ii;
UCHAR   aucData[cMAX_READ_NUM + 1];         // For Host Cmd

    mccSET_CMD2(aucData) = ccCMD_ID_DEMOD_GET_REG;
    mccSET_QUEUE2(aucData, ccOFST_REG_NUM) = ucCnt;
    mccSET_QUEUE2(aucData, ccOFST_REG1_ADDR_L) = mcLOW_BYTE(u2Addr);
    mccSET_QUEUE2(aucData, ccOFST_REG1_ADDR_H) = mcHIGH_BYTE(u2Addr);
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_REG1_SIZE + ucCnt - 1, ccOFST_REG1_SIZE + 1))
    {
        for (ii = 0; ii < ucCnt; ii ++)
            pucData[ii] = mccGET_QUEUE(aucData, ccOFST_REG1_DATA + ii);
    }
    else
        return FALSE;

    return TRUE;
}

UINT8 ucGetReg(DEMOD_CTX_T2 *psDemodCtx, UINT16 u2Addr)
{

UCHAR   ucValue = 0;

    fgGetRegs(psDemodCtx, u2Addr, &ucValue, 1);
    return ucValue;
}
#endif

#if 0
//----------------------------------------------------------------------------- 
/*
 *  fgGetFwVars
 *  Get firmware variables.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  u2Addr  firmware variable address.
 *  @param  pucData  Pointer of data buffer.
 *  @param  ucCnt  Length of data (ucCnt<=4).
 *  @retval   TRUE : Success.
 *  @retval   FALSE : Fail.   
 */
//-----------------------------------------------------------------------------
static BOOL fgGetFwVars(DEMOD_CTX_T2 *psDemodCtx, UINT16 u2Addr, UCHAR *pucData, UCHAR ucCnt)
{
UCHAR   ii;
UCHAR   aucData[cMAX_READ_NUM + 1];         // For Host Cmd

    mccSET_CMD2(aucData) = ccCMD_ID_DEMOD_GET_VAR;
    mccSET_QUEUE2(aucData, ccOFST_REG_NUM) = ucCnt;
    mccSET_QUEUE2(aucData, ccOFST_REG1_ADDR_L) = mcLOW_BYTE(u2Addr);
    //mccSET_QUEUE2(aucData, ccOFST_REG1_ADDR_H) = mcHIGH_BYTE(u2Addr);
    mccSET_QUEUE2(aucData, ccOFST_REG1_ADDR_H) = 0;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_REG1_SIZE + ucCnt - 1, ccOFST_REG1_SIZE + 1))
    {
        for (ii = 0; ii < ucCnt; ii ++)
            pucData[ii] = mccGET_QUEUE(aucData, ccOFST_REG1_DATA + ii);
    }
    else
        return FALSE;
    return TRUE;
}

//----------------------------------------------------------------------------- 
/*
 *  ucGetFwVar
 *  Get firmware one-byte variable.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  u2Addr  firmware variable address.
 *  @retval   1-byte data.   
 */
//-----------------------------------------------------------------------------
static UINT8 ucGetFwVar(DEMOD_CTX_T2 *psDemodCtx, UINT16 u2Addr)
{
UCHAR   ucValue = 0;

    fgGetFwVars(psDemodCtx, u2Addr, &ucValue, 1);
    return ucValue;
}


//----------------------------------------------------------------------------- 
/*
 *  fgSetFwVars
 *  Set firmware variables.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  u2Addr  firmware variable address.
 *  @param  pucData  Pointer of data buffer.
 *  @param  ucCnt  Length of data (ucCnt<=4).
 *  @retval   TRUE : Success.
 *  @retval   FALSE : Fail.   
 */
//-----------------------------------------------------------------------------
static BOOL fgSetFwVars(DEMOD_CTX_T2 *psDemodCtx, UINT16 u2Addr, UCHAR *pucData, UCHAR ucCnt)
{
UCHAR   ii;
UCHAR   aucData[cMAX_READ_NUM + 1];         // For Host Cmd

    mccSET_CMD2(aucData) = ccCMD_ID_DEMOD_SET_VAR;
    mccSET_QUEUE2(aucData, ccOFST_REG_NUM) = ucCnt;
    mccSET_QUEUE2(aucData, ccOFST_REG1_ADDR_L) = mcLOW_BYTE(u2Addr);
    //mccSET_QUEUE2(aucData, ccOFST_REG1_ADDR_H) = mcHIGH_BYTE(u2Addr);
        mccSET_QUEUE2(aucData, ccOFST_REG1_ADDR_H) = 0;
    for (ii = 0; ii < ucCnt; ii ++)
        mccSET_QUEUE2(aucData, ccOFST_REG1_DATA + ii) = pucData[ii];
    //if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_REG1_SIZE + ucCnt - 1, ccOFST_REG1_SIZE + 1))
    if (mcHOST_CMD(psDemodCtx, aucData, 0, ccOFST_REG1_SIZE + ucCnt -1 + 1))
    {
        return TRUE;
    }
    return FALSE;
}

//----------------------------------------------------------------------------- 
/*
 *  vSetFwVar
 *  Set firmware one-byte variable.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  u2Addr  firmware variable address.
 *  @param  ucValue  data to be set
 *  @retval   void.   
 */
//-----------------------------------------------------------------------------
static void vSetFwVar(DEMOD_CTX_T2 *psDemodCtx, UINT16 u2Addr, UINT8 ucValue)
{
    fgSetFwVars(psDemodCtx, u2Addr, &ucValue, 1);
}
#endif


/***********************************************************************/
/*                  Public Functions (not static)                      */
/***********************************************************************/

/**********************************************************************/
/*                  Initialization APIs                               */
/**********************************************************************/
//Pluto
#if 0
static char *cMsgUpErr[] =
{
    "",
    cMSG_UP_ERR_I2C,
    cMSG_UP_ERR_HW_RDY,
    cMSG_UP_ERR_ROM_CHKSUM_RDY,
    cMSG_UP_ERR_ROM_CHKSUM_OK,
    cMSG_UP_ERR_SW_CHKSUM_OK,
    cMSG_UP_ERR_CHKSUM_RDY,
    cMSG_UP_ERR_CHKSUM_OK
};
#endif

#if 1
//----------------------------------------------------------------------------- 
/*
 *  s4LoadCode
 *  Download firmware code.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  u4I2cClkDiv  Unused.
 *  @param  u1CodeImage  Pointer of fimware code image.
 *  @param  u2CurRomLen  Firmware code size in byte.
 *  @param  u2MaxI2cLen  Maximum I2C length per transfer.
 *  @param  fgClk50  Flag to switch if 48MHz or 24MHz clock is used.
 *  @retval   cUP_LOAD_OK : Load code OK.
 *  @retval   cUP_LOAD_ERR_I2C : I2C error.
 *  @retval   cUP_LOAD_ERR_HW_RDY : HW not ready.
 *  @retval   cUP_LOAD_ERR_CHKSUM_RDY : Checksum not ready.
 *  @retval   cUP_LOAD_ERR_CHKSUM_OK : Checksum not OK.   
 */
//-----------------------------------------------------------------------------
static INT32 s4LoadCode(DEMOD_CTX_T2 *psDemodCtx, UINT32 u4I2cClkDiv, UINT8 *u1CodeImage,
                 UINT16 u2CurRomLen, UINT16 u2MaxI2cLen, BOOL fgClk50)
{
#if 0
    UCHAR   ucValue, ucI2cAuto, ucUpCtrl, ii;
    UINT16  u2CurI2cLen, u2CurRamAdr;
    UCHAR   ucI2cAddr;
#else
    UCHAR   ucValue, ucUpCtrl, ii, ucI2cAuto;
    UINT16  u2CurRamAdr;
#endif

#if fcLOAD_CODE_TIME
    TIME_TICK_T u4TickStart, u4TickCur;
    TIME_DIFF_T u4TickCnt;
#endif

    UINT16  u2SramSize;

    mcSHOW_USER_MSG(("Start Load DVBT2 FW...."));

    if (DVBT2_GetReg(psDemodCtx, MT5136T2_REG_uP_Ctrl, &ucUpCtrl, 1))
        return cUP_LOAD_ERR_I2C;
    mcSHOW_USER_MSG(("[at start] uP 0x00 is 0x%x!\n", ucUpCtrl));
	// SRAM clear
    ucUpCtrl = mcBIT(ccpUP_MEM_INIT_DONE);
    DVBT2_SetReg(psDemodCtx, MT5136T2_REG_uP_Ctrl, &ucUpCtrl, 1);
    mcDELAY_MS(10);
    DVBT2_GetReg(psDemodCtx, MT5136T2_REG_uP_Ctrl, &ucUpCtrl, 1);

    if (!mcTEST_BIT(ucUpCtrl, ccpUP_MEM_INIT_DONE))
    {
        return cUP_LOAD_ERR_HW_RDY;
    }

    // There is no need to do the rom code checksum for 5136 project	??
#if 0    
    if(psDemodCtx->m_fgRomPatch)
    {
        // check ROM code checksum
        ucUpCtrl = mcBIT(ccpUP_RESTART_CHKSUM) | mcBIT(ccpUP_ROMCODE_CHKSUM);
        MT5136_DVBT2_SetReg(psDemodCtx, MT5136T2_REG_uP_Ctrl, &ucUpCtrl, 1);

        for (ii = 0; ii < 8; ii++)
        {
            mcDELAY_MS(10);                        // Unit: ms
            MT5136_DVBT2_GetReg(psDemodCtx, MT5136T2_REG_uP_Ctrl, &ucUpCtrl, 1);
            if (mcTEST_BIT(ucUpCtrl, ccpUP_CHKSUM_DONE))
                break;
        }
        if (!mcTEST_BIT(ucUpCtrl, ccpUP_CHKSUM_DONE))
        {
            return cUP_LOAD_ERR_ROM_CHKSUM_RDY;
        }
        if (!mcTEST_BIT(ucUpCtrl, ccpUP_CHKSUM_OK))
        {
            return cUP_LOAD_ERR_ROM_CHKSUM_OK;
        }
    }
#endif

    ucUpCtrl = mcBIT(ccpUP_DN_FREQ) | mcBIT(ccpUP_RESET_CHKSUM);  // Disable uP
    DVBT2_SetReg(psDemodCtx, MT5136T2_REG_uP_Ctrl, &ucUpCtrl, 1);
    ucValue = 0;
    DVBT2_SetReg(psDemodCtx, MT5136T2_REG_uP_GP_REG06, &ucValue, 1);

    // for hw , encript/discript
    for (ii = 0; ii < 4; ii ++)
    {
        MT5136_DVBT2_GetReg(psDemodCtx, (UINT16) (MT5136T2_REG_uP_PNGP_H + ii), &ucValue, 1);
        MT5136_DVBT2_SetReg(psDemodCtx, (UINT16) (MT5136T2_REG_uP_PNGP_H + ii), &ucValue, 1);
    }

    // Set max code size
    u2SramSize = (UINT16)(psDemodCtx->ucSramSize) * 1024 - 1;
    ucValue = mcHIGH_BYTE(u2SramSize);
    DVBT2_SetReg(psDemodCtx, MT5136T2_REG_uP_LPA_H, &ucValue, 1);
    ucValue = mcLOW_BYTE(u2SramSize);
    DVBT2_SetReg(psDemodCtx, MT5136T2_REG_uP_LPA_L, &ucValue, 1);

    // Turn on I2C auto-increment, no need for spi
    DVBT2_GetReg(psDemodCtx, MT5136T2_REG_I2C_Cfg, &ucI2cAuto, 1);
    mcCLR_BIT(ucI2cAuto, ccpI2C_AUTO_INC);
    DVBT2_SetReg(psDemodCtx, MT5136T2_REG_I2C_Cfg, &ucI2cAuto, 1);

    // Set Load code RAM Starting Addr
#if fcLOAD_CODE_TIME
    mcSHOW_USER_MSG(("Code Size = %d ", u2CurRomLen));
    mcGET_SYS_TIME(u4TickStart);
    ii = 0;
#endif

    u2CurRamAdr = psDemodCtx->m_u2RomSize;
    ucValue = mcHIGH_BYTE(u2CurRamAdr);
    DVBT2_SetReg(psDemodCtx, MT5136T2_REG_uP_IRA_H, &ucValue, 1);
    ucValue = mcLOW_BYTE(u2CurRamAdr);
    DVBT2_SetReg(psDemodCtx, MT5136T2_REG_uP_IRA_L, &ucValue, 1);

	if (DVBT2_Loadcode(psDemodCtx, u1CodeImage, u2CurRomLen))
    {
        mcSHOW_USER_MSG(("Download I2C fail! (%d)\n"));      
        return 1;
    }

    // Turn on I2C auto-increment
    mcSET_BIT(ucI2cAuto, ccpI2C_AUTO_INC);
    DVBT2_SetReg(psDemodCtx, MT5136T2_REG_I2C_Cfg, &ucI2cAuto, 1);

    // There is no need to do the rom code checksum in MT5136 project
#if 0  
    if (psDemodCtx->m_fgRomPatch)
    {
        MT5136_DVBT2_SetReg(psDemodCtx, MT5136T2_REG_uP_Ctrl, &ucUpCtrl, 1);
        if (!mcTEST_BIT(ucUpCtrl, ccpUP_CHKSUM_OK))
        {
            return cUP_LOAD_ERR_SW_CHKSUM_OK;
        }
    }
#endif
    ucUpCtrl = 0;
    mcSET_BIT(ucUpCtrl, ccpUP_RESTART_CHKSUM);
    mcCLR_BIT(ucUpCtrl, ccpUP_MEM_INIT_DONE);
    MT5136_DVBT2_SetReg(psDemodCtx, MT5136T2_REG_uP_Ctrl, &ucUpCtrl, 1);
    for (ii = 0; ii < 8; ii++)
    {
        mcDELAY_MS(10);                        // Unit: ms
        MT5136_DVBT2_GetReg(psDemodCtx, MT5136T2_REG_uP_Ctrl, &ucUpCtrl, 1);
        if (mcTEST_BIT(ucUpCtrl, ccpUP_CHKSUM_DONE))
            break;
    }
    if (!mcTEST_BIT(ucUpCtrl, ccpUP_CHKSUM_DONE))
    {
         mcSHOW_USER_MSG(("uP Check Sum DONE Error!"));
        return cUP_LOAD_ERR_CHKSUM_RDY;
    }
    if (!mcTEST_BIT(ucUpCtrl, ccpUP_CHKSUM_OK))
    {
         mcSHOW_USER_MSG(("uP Check Sum Error!"));    
        return cUP_LOAD_ERR_CHKSUM_OK;
    }


	ucUpCtrl = mcBIT(ccpUP_ENABLE);          // 50 MHz Clock
    if (!fgClk50)
        ucUpCtrl = mcBIT(ccpUP_ENABLE) | mcBIT(ccpUP_DN_FREQ);    // 25 MHz Clock
    DVBT2_SetReg(psDemodCtx, MT5136T2_REG_uP_Ctrl, &ucUpCtrl, 1);

    // delay for waiting FW initialization done, may have I2C timeout at first
    for (ii = 0; ii < 5; ii++)
    {
        mcDELAY_MS(3);
        DVBT2_GetReg(psDemodCtx, MT5136T2_REG_uP_GP_REG06, &ucValue, 1);
        if (ucValue == ccFW_RUN_TAG)
            break;
    }

    if(ii == 5)
    {
        mcSHOW_HW_MSG(("uP Startup Error!\n"));
    }   

    // enable HOST_EN for host to be able to access cmd/rsp register
    // for stand alone IC
    //#ifndef defined(CC_MT5136)
    #if defined (CC_MT5136) //Mingsheng mark for build code error
        ucValue = 0x01;
        DVBT2_SetReg(psDemodCtx, MT5136T2_REG_RFA4A0, &ucValue, 1);
    #endif
     
    DVBT2_GetReg(psDemodCtx, 0x00, &ucValue, 1);
	mcSHOW_USER_MSG(("uP 0x00 is 0x%x!\n", ucValue));
    mcSHOW_USER_MSG(("uP Startup!\n"));
    
#if fcLOAD_CODE_TIME
    mcGET_SYS_TIME(u4TickCur);
    mcGET_DIFF_TIME(u4TickCnt, u4TickStart, u4TickCur);
    u4TickCnt = mcCONV_SYS_TIME(u4TickCnt);
    mcSHOW_USER_MSG((" (%3u ms)\n", u4TickCnt));
    if (u4TickCnt > 0)
        return -((INT32) u4TickCnt);
    else
#endif

    return cUP_LOAD_OK;
}
#endif




//----------------------------------------------------------------------------- 
/*
 *  vDVBT2GetStatus
 *  Get demod status and update demod related context.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   void.   
 */
//-----------------------------------------------------------------------------
void vDVBT2GetStatus(DEMOD_CTX_T2 *psDemodCtx)
{
    UCHAR   aucData[cMAX_READ_NUM + 1];
	UINT8   uc_data[cMAX_READ_NUM + 1];
	UINT8   ucTmp = 0;

    
	DVBT2_GetReg(psDemodCtx,0x1805,&ucTmp,1);
	mcSHOW_DBG_MSG(("[vDVBT2GetStatus]before  hostcmd 0x1805=0x%02x\n!", ucTmp)); 
    mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_STATUS1;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_STATUS1_SIZE, 1))
    {   
        // Signal 
        psDemodCtx->s1IfAgc           = (S8)mccGET_QUEUE(aucData, ccOFST_DVBT2_STATUS1_IF_AGC);
		//AGC
        psDemodCtx->fgAgcLock         = mccGET_QUEUE(aucData, ccOFST_DVBT2_STATUS1_AGC_LOCK);
        // Early break indicator
        psDemodCtx->fgEarlyBreak      = mccGET_QUEUE(aucData, ccOFST_DVBT2_STATUS1_EARLY_BREAK);
		// System controller status
		psDemodCtx->ucT2SysCtrlStatus = mccGET_QUEUE(aucData, ccOFST_DVBT2_STATUS1_SYSCTRL_STATE);
		// L1 status
		psDemodCtx->ucT2L1Status      = mccGET_QUEUE(aucData,ccOFST_DVBT2_STATUS1_L1_STATE);
        // TS state
        psDemodCtx->fgT2TsLock        = mccGET_QUEUE(aucData, ccOFST_DVBT2_STATUS1_TS_LOCK);
		DVBT2_GetReg(psDemodCtx,0x1805,&ucTmp,1);
		mcSHOW_DBG_MSG(("[vDVBT2GetStatus]after hostcmd 0x1805=0x%02x\n!", ucTmp)); 
		mcSHOW_DBG_MSG4(("Update T2 Status : System controller status ==> %d\n!", psDemodCtx->ucT2SysCtrlStatus));
		mcSHOW_DBG_MSG4(("Update T2 Status : Early break ==> %d\n!", psDemodCtx->fgEarlyBreak));
        mcSHOW_DBG_MSG(("[vDVBT2GetStatus]Update T2 Status : L1 Status ==> %d\n!", psDemodCtx->ucT2L1Status)); 
		mcSHOW_DBG_MSG4(("Update T2 Status : TS ==> %d\n!", psDemodCtx->fgT2TsLock));
    }
	mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_FW_STATE;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_GET_FW_STATE_SIZE, 1))
    { 
    	psDemodCtx->ucResetCnt = mccGET_QUEUE(aucData, ccOFST_DVBT2_GET_FW_RESET_COUNT);
    }

	
	mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO10;
	   if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO10_SIZE, 1))
	   {
		   // SNR
		   //uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO10_SNR_L);
		   //uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO10_SNR_H);
		   //psDemodCtx->s2Snr = (S16) mcSIGN_2BYTE(uc_data[1], uc_data[0], 8, 8);
	
		   // SNR, snr*512
		   uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_REG_CSI_MID10_SNR_L);
		   uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_REG_CSI_MID10_SNR_H);
		   psDemodCtx->s2Snr = (S16) mcSIGN_2BYTE(uc_data[1], uc_data[0], 8, 8);
	   }

	 mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO1;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO1_SIZE, 1))
    {
        psDemodCtx->u1Ver1    = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO1_VER1);
        psDemodCtx->u1Ver0    = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO1_VER0);
        ucTmp                 = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO1_MD_STATUS0),  0x7F, 3);
		ucTmp >>= 1;
		psDemodCtx->u1P1D_Fft = ucTmp;
        psDemodCtx->u1MD_GI   = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO1_MD_STATUS1),0x7, 0);
       mcSHOW_DBG_MSG4(("[vDVBT2GetStatus]psDemodCtx->u1P1D_Fft=%d\n",psDemodCtx->u1P1D_Fft));                                     
    }

	mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_FW_STATE;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_GET_FW_STATE_SIZE, 1))
    { 
    	psDemodCtx->ucResetCnt = mccGET_QUEUE(aucData, ccOFST_DVBT2_GET_FW_RESET_COUNT);
    }
	//mcDELAY_MS(150); // 20120705 Hauze : There is a critical timing problem in MPLP scanning and need a delay time to avoid this problem
	//mcDELAY_MS(100); 
}

//----------------------------------------------------------------------------- 
/*
 *  vDVBT2GetInfo
 *  Get demod information and update demod related context.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   void.   
 */
//-----------------------------------------------------------------------------
void vDVBT2GetInfo(DEMOD_CTX_T2 *psDemodCtx)
{
    UINT8   aucData[cMAX_READ_NUM + 1];
    UINT8   uc_data[4];
    INT32   u4Cfo;
	UINT8 ucTemp, ucTemp1;
    UINT32 u4LDPC_iteration_sum;    		// LDPC2 iteration sum
    // LDPC2 packet error rate
    UINT16 u2LDPC_Pkt_No; //, u2LDPC_AVG_Iteration;
    
    //UINT8   ucLGCY_gain_shf_table[16] = {0, 1, 2, 4, 5, 6, 7, 0, -7, -6, -5, -4, -3, -2, -1};
    INT8   ucLGCY_gain_shf_table[16] = {0, 1, 2, 4, 5, 6, 7, 0, -7, -6, -5, -4, -3, -2, -1, 0};

	//Packet for COMMON PLP
    mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO4;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO4_SIZE, 1))
    {
        //total error packets
        uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO4_BCH1_ERR_PKT0);
        uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO4_BCH1_ERR_PKT1);
		uc_data[2] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO4_BCH1_ERR_PKT2);
        psDemodCtx->u4BCHErrorPkt1 = mcUNSIGN_3BYTE(uc_data[2], uc_data[1], uc_data[0], 4);  

        //total packets
        uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO4_BCH1_TOT_PKT0);
        uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO4_BCH1_TOT_PKT1);
		uc_data[2] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO4_BCH1_TOT_PKT2);
        psDemodCtx->u4BCHTotalPkt1 = mcUNSIGN_3BYTE(uc_data[2], uc_data[1], uc_data[0], 4);

        //total error bits
	    DVBT2_GetDemodReg(psDemodCtx, MT5136T2_REG_BCH_MONIT6, uc_data, 3);
		psDemodCtx->u4PstBCHErrorBit1 = mcUNSIGN_3BYTE(uc_data[2], uc_data[1], uc_data[0], 8);

		DVBT2_GetDemodReg(psDemodCtx, MT5136T2_REG_BCH_MONIT9, uc_data, 3);
		psDemodCtx->u4PstBCHCorBit1= mcUNSIGN_3BYTE(uc_data[2], uc_data[1], uc_data[0], 8);
		psDemodCtx->u4PreBCHErrorBit1 = psDemodCtx->u4PstBCHCorBit1 + psDemodCtx->u4PstBCHErrorBit1;		
//		psDemodCtx->u4BCHErrorBit1 += u4_data;		
    }

	//Packet for DATA PLP    
    mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO5;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO5_SIZE, 1))
    {
        //total error packets
        uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO5_BCH2_ERR_PKT0);
        uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO5_BCH2_ERR_PKT1);
		uc_data[2] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO5_BCH2_ERR_PKT2);
        psDemodCtx->u4BCHErrorPkt2 = mcUNSIGN_3BYTE(uc_data[2], uc_data[1], uc_data[0], 4);  

        //total packets
        uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO5_BCH2_TOT_PKT0);
        uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO5_BCH2_TOT_PKT1);
		uc_data[2] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO5_BCH2_TOT_PKT2);
        psDemodCtx->u4BCHTotalPkt2 = mcUNSIGN_3BYTE(uc_data[2], uc_data[1], uc_data[0], 4);

		//total error bits
		DVBT2_GetDemodReg(psDemodCtx, MT5136T2_REG_BCH1_MONIT6, uc_data, 3);
		psDemodCtx->u4PstBCHErrorBit2 = mcUNSIGN_3BYTE(uc_data[2], uc_data[1], uc_data[0], 8);

		DVBT2_GetDemodReg(psDemodCtx, MT5136T2_REG_BCH1_MONIT9, uc_data, 3);
		psDemodCtx->u4PstBCHCorBit2 = mcUNSIGN_3BYTE(uc_data[2], uc_data[1], uc_data[0], 8);
		psDemodCtx->u4PreBCHErrorBit2 = psDemodCtx->u4PstBCHCorBit2 + psDemodCtx->u4PstBCHErrorBit2;
//		psDemodCtx->u4BCHErrorBit2 += u4_data;

		psDemodCtx->u4TotalPkt2[psDemodCtx->ii] = psDemodCtx->u4BCHTotalPkt2;
		psDemodCtx->u4ErrorBit2[psDemodCtx->ii] = psDemodCtx->u4PreBCHErrorBit2;
		psDemodCtx->ii = psDemodCtx->ii + 1;
		if(psDemodCtx->ii == 5)
		{
			psDemodCtx->ii = 0;
		}
		mcSHOW_DBG_MSG4(("[vDVBT2GetInfo]****psDemodCtx->ii=%d\n",psDemodCtx->ii));


    }
    
    //Update LDPC info
	mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO21;
	if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO21_SIZE, 1))
	{
		psDemodCtx->u4LDPC_Cur1= mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO21_LDPC1_ITERATION_CUR);
		psDemodCtx->u4LDPC_Cur2= mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO21_LDPC2_ITERATION_CUR);

		psDemodCtx->u4LDPC_OverFlow1 = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO21_LDPC1_OVERFLOW_CHECK);
		psDemodCtx->u4LDPC_OverFlow2 = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO21_LDPC2_OVERFLOW_CHECK);

		psDemodCtx->u4LDPC_MaxIter1 = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO21_LDPC1_MAX_ITERATION);
		psDemodCtx->u4LDPC_MaxIter2 = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO21_LDPC2_MAX_ITERATION);
	}

/******************** LDPC1 information ********************/
	mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO19;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO19_SIZE, 1))
    {

		u4LDPC_iteration_sum = 0;
		if (!(mcTEST_BIT(psDemodCtx->u4LDPC_OverFlow1, 2)))
		{
			uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO19_LDPC1_ITERATION_SUM0);
			uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO19_LDPC1_ITERATION_SUM1);
			uc_data[2] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO19_LDPC1_ITERATION_SUM2);
			u4LDPC_iteration_sum = mcUNSIGN_3BYTE(uc_data[2], uc_data[1], uc_data[0], 8);
		}


		if (!(mcTEST_BIT(psDemodCtx->u4LDPC_OverFlow1, 1)))
		{
			uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO19_LDPC1_PKT_NO0);
			uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO19_LDPC1_PKT_NO1);
			u2LDPC_Pkt_No = mcSIGN_2BYTE(uc_data[1], uc_data[0], 8, 8);

			if (u2LDPC_Pkt_No) 
			{
				psDemodCtx->u4LDPC_AVGIter1 = (U16) (u4LDPC_iteration_sum / u2LDPC_Pkt_No);
			}
		}		
    }
/******************** LDPC2 information ********************/
	mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO20;
	if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO20_SIZE, 1))
	{

		u4LDPC_iteration_sum = 0;
		if (!(mcTEST_BIT(psDemodCtx->u4LDPC_OverFlow2 , 2)))
		{
			uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO20_LDPC2_ITERATION_SUM0);
			uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO20_LDPC2_ITERATION_SUM1);
			uc_data[2] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO20_LDPC2_ITERATION_SUM2);
			u4LDPC_iteration_sum = mcUNSIGN_3BYTE(uc_data[2], uc_data[1], uc_data[0], 8);
		}
	
		if (!(mcTEST_BIT(psDemodCtx->u4LDPC_OverFlow2 , 1)))
		{
			uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO20_LDPC2_PKT_NO0);
			uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO20_LDPC2_PKT_NO1);
			u2LDPC_Pkt_No = mcSIGN_2BYTE(uc_data[1], uc_data[0], 8, 8);

			if (u2LDPC_Pkt_No) 
			{
				psDemodCtx->u4LDPC_AVGIter2 = (U16) (u4LDPC_iteration_sum / u2LDPC_Pkt_No);
			}
		}
	
	}

    
    mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO6;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO6_SIZE, 1))
    {
		//***************** CFO Initial *********************
        uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO6_IFS_ADJ_LO);
        uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO6_IFS_ADJ_HI);
        psDemodCtx->s2Ifo = (S16)mcSIGN_2BYTE(uc_data[1], uc_data[0], 3, 8);  // 11 bits for 5391
    }

	mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO7;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO7_SIZE, 1))
    {
    //***************** CFO Tracking *********************
        uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO7_FREQ_ADJ_0);
        uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO7_FREQ_ADJ_1);
        uc_data[2] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO7_FREQ_ADJ_2);
	  uc_data[3] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO7_FREQ_ADJ_3);//LL 
	  //mcSHOW_USER_CRI_MSG(("30e=0x%02x, 30d=0x%02x, 30c=0x%02x, 30b=0x%02x,\n",uc_data[3],uc_data[2],uc_data[1],uc_data[0]));
		// freq_offset = reg_r_dc_freq_adjust * (BW (KHz) * 8 / 7 ) / fft_size
		u4Cfo = mcSIGN_4BYTE(uc_data[3], uc_data[2], uc_data[1], uc_data[0], 5);
		//utmp_Cfo = mcSIGN_4BYTE(uc_data[3], uc_data[2], uc_data[1], uc_data[0], 5);
         // mcSHOW_USER_CRI_MSG(("mcSIGN_4BYTE u4Cfo=%d , \n",u4Cfo));
		//uc_data[3] >> 
		#if 0
		if(mcTEST_BIT(uc_data[3],4))
			{ stmp_Cfo = (utmp_Cfo - (1<<29) )/(1<<14);}
		else
			{stmp_Cfo = utmp_Cfo / (1 << 14);}
		mcSHOW_USER_CRI_MSG(("mcTEST_BIT stmp_Cfo=%d\n",stmp_Cfo));
		#endif
		//psDemodCtx->s4Cfo = (float) (u4Cfo >> 14);
		psDemodCtx->s4Cfo= u4Cfo *(-1)/ (1 << 14);
		//mcSHOW_USER_CRI_MSG(("right shift u4Cfo=%d\n",psDemodCtx->s4Cfo));

		
       // psDemodCtx->s4Cfo *= psDemodCtx->u1Bw * 8;
		switch(psDemodCtx->u1Bw)
			{
			case 0: psDemodCtx->s4Cfo *= 6*1024*8;
				break;
			case 1: psDemodCtx->s4Cfo *= 7*1024*8;
				break;
			case 2: psDemodCtx->s4Cfo *= 8*1024*8;
				break;
			default:break;
			}
		switch (((DEMOD_CTX_T2 *) psDemodCtx)->u1P1D_Fft)
        {
			case 0: psDemodCtx->s4Cfo /= 2 * 7;   break;
                  case 1: psDemodCtx->s4Cfo /= 8 * 7;   break;
                  case 2: psDemodCtx->s4Cfo /= 4 * 7;   break;
			case 3: psDemodCtx->s4Cfo /= 1 * 7;   break;
			case 4: psDemodCtx->s4Cfo /= 16 * 7;  break;
			case 5: psDemodCtx->s4Cfo /= 32 * 7;  break;
			case 6: psDemodCtx->s4Cfo /= 8 * 7;    break;
			case 7: psDemodCtx->s4Cfo /= 32 * 7;  break;
            default: psDemodCtx->s4Cfo = 10^6;   break;
        }
	// mcSHOW_USER_CRI_MSG(("cfo=%dHZ, BW=%d, fft=%d\n",psDemodCtx->s4Cfo,psDemodCtx->u1Bw, ((DEMOD_CTX_T2 *) psDemodCtx)->u1P1D_Fft));
    #if 0
		//***************** CFO Tracking *********************
        uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO7_FREQ_ADJ_0);
        uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO7_FREQ_ADJ_1);
        uc_data[2] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO7_FREQ_ADJ_2);
		uc_data[3] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO7_FREQ_ADJ_2);
		
		// freq_offset = reg_r_dc_freq_adjust * (BW (KHz) * 8 / 7 ) / fft_size
		u4Cfo = mcSIGN_4BYTE(uc_data[3], uc_data[2], uc_data[1], uc_data[0], 4);
		//psDemodCtx->s4Cfo = (float) (u4Cfo >> 14);
		psDemodCtx->s4Cfo = (u4Cfo >> 14);
        psDemodCtx->s4Cfo *= psDemodCtx->u1Bw * 8;

		switch (((DEMOD_CTX_T2 *) psDemodCtx)->u1P1D_Fft)
        {
			case 0: psDemodCtx->s4Cfo /= 2 * 7;   break;
            case 1: psDemodCtx->s4Cfo /= 8 * 7;   break;
            case 2: psDemodCtx->s4Cfo /= 4 * 7;   break;
			case 3: psDemodCtx->s4Cfo /= 1 * 7;   break;
			case 4: psDemodCtx->s4Cfo /= 16 * 7;  break;
			case 5: psDemodCtx->s4Cfo /= 32 * 7;  break;
            default: psDemodCtx->s4Cfo = 10^6;   break;
        }
		#endif
	}

    mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO10;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO10_SIZE, 1))
    {
        // SNR
        //uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO10_SNR_L);
        //uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO10_SNR_H);
        //psDemodCtx->s2Snr = (S16) mcSIGN_2BYTE(uc_data[1], uc_data[0], 8, 8);

		// SNR, snr*512
        uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_REG_CSI_MID10_SNR_L);
        uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_REG_CSI_MID10_SNR_H);
        psDemodCtx->s2Snr = (S16) mcSIGN_2BYTE(uc_data[1], uc_data[0], 8, 8);
    }

    mccSET_CMD2(aucData) = ccCMD_ID_SLD_PRA;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_SLD_PRA_SIZE, 1))
    {
        uc_data[0] = mccGET_QUEUE(aucData, ccOFST_SLD_PRA_STATUS_MONITOR);
        psDemodCtx->ucSldPraState = mcGET_FIELD(uc_data[0], 0x60, 5);

        uc_data[1] = mccGET_QUEUE(aucData, ccOFST_SLD_PRA_RFAGC_CTRL_WORD_H);
        uc_data[0] = mccGET_QUEUE(aucData, ccOFST_SLD_PRA_RFAGC_CTRL_WORD_L);
        psDemodCtx->s2SldPraRfagcCtrlWord = mcSIGN_2BYTE(uc_data[1], uc_data[0], 8, 5);

        uc_data[1] = mccGET_QUEUE(aucData, ccOFST_SLD_PRA_IFAGC_CTRL_WORD_H);
        uc_data[0] = mccGET_QUEUE(aucData, ccOFST_SLD_PRA_IFAGC_CTRL_WORD_L);
        psDemodCtx->s2SldPraIfagcCtrlWord = mcSIGN_2BYTE(uc_data[1], uc_data[0], 8, 5);

        uc_data[0] = mccGET_QUEUE(aucData, ccOFST_SLD_PRA_PGA_GAIN_INDEX);
        psDemodCtx->ucSldPraGainIdx = mcGET_FIELD(uc_data[0], 0x1f, 0);
    }

    mccSET_CMD2(aucData) = ccCMD_ID_SLD_PRA1;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_SLD_PRA1_SIZE, 1))
    {
        uc_data[1] = mccGET_QUEUE(aucData, ccOFST_SLD_PRA_MAG_REF_OUT_H);
        uc_data[0] = mccGET_QUEUE(aucData, ccOFST_SLD_PRA_MAG_REF_OUT_L);
        psDemodCtx->s2SldPraOut = mcSIGN_2BYTE(uc_data[1], uc_data[0], 3, 8);

        uc_data[1] = mccGET_QUEUE(aucData, ccOFST_LEGACY_GAIN_BIT_SHIFT_T2);
        uc_data[0] = mccGET_QUEUE(aucData, ccOFST_LEGACY_GAIN_Cur_Level_T2);
        psDemodCtx->s2LegPraOut = uc_data[0] << ucLGCY_gain_shf_table[uc_data[1]];

		uc_data[1] = mccGET_QUEUE(aucData, ccOFST_LEGACY_GAIN_INPUT_LEVEL_T2_H);
        uc_data[0] = mccGET_QUEUE(aucData, ccOFST_LEGACY_GAIN_INPUT_LEVEL_T2_L);
		psDemodCtx->s2LegInputLevel = mcSIGN_2BYTE(uc_data[1], uc_data[0], 0, 0);
    }

	mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_TS_INIT_BITRATE;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_TS_INIT_BITRATE_SIZE, 1))
    {
		UINT32 u4Period;

		uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_TS_INIT_BITRATE_0);
        uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_TS_INIT_BITRATE_1);
		uc_data[2] = mccGET_QUEUE(aucData, ccOFST_DVBT2_TS_INIT_BITRATE_2);
        u4Period = (U32)mcUNSIGN_3BYTE(uc_data[2], uc_data[1], uc_data[0], 7);
		if (u4Period > 0)
		{
			//fPeriod = (float) (188 * 8 * 62 * pow(10,6)) / (float) u4Period;    // for FPGA
			u4Period = (188 * 8 * 162) * (1000000 / u4Period);    // for ASIC
			psDemodCtx->u4InitTSBitRate = u4Period;
		}

		uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_TSIF_OUT_BITRATE_0);
		uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_TSIF_OUT_BITRATE_1);
		psDemodCtx->u4TSIFBitRate = (U32)mcUNSIGN_2BYTE(uc_data[1], uc_data[0], 8, 8);
		psDemodCtx->u4TSIFBitRate *= 10000;
	}


	DVBT2_GetReg(psDemodCtx, 0x9A0, &ucTemp, 1);
	mcSET_BIT(ucTemp, 7);
	mcSET_BIT(ucTemp, 6);
	DVBT2_SetReg(psDemodCtx, 0x9A0, &ucTemp, 1);
	DVBT2_GetReg(psDemodCtx, 0x9A5, &ucTemp, 1);
	DVBT2_GetReg(psDemodCtx, 0x9A6, &ucTemp1, 1);
	DVBT2_GetReg(psDemodCtx, 0x9A5, &ucTemp, 1);
	psDemodCtx->u2RfAgcRssi = ((UINT16)ucTemp)<<2 | (ucTemp1&0x03);

	 mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO11;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO11_SIZE, 1))
    {
        INT32   u4Period;

		uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO11_BITRATE_0);
        uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO11_BITRATE_1);
		uc_data[2] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO11_BITRATE_2);
        u4Period = (S32)mcUNSIGN_3BYTE(uc_data[2], uc_data[1], uc_data[0], 7);
		if (u4Period > 0)
		{
			//fPeriod = (float) (188 * 8 * 62 * pow(10,6)) / (float) u4Period;    // for FPGA
			u4Period = (188 * 8 * 162) * (1000000 / u4Period);    // for ASIC
			psDemodCtx->u4DJB_TSBitRate = u4Period;
		}
    }
#if 0
	mccSET_CMD2(aucData)= ccCMD_ID_DEMOD_GET_INFO9;
	if(mcHOST_CMD(psDemodCtx, aucData, ccOFST_DEMOD_INFO9_SIZE, 1))
	{
		uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DEMOD_INFO9_TOTAL_PEAK);
		uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DEMOD_INFO9_TOTAL_COMB);
		uc_data[2] = mccGET_QUEUE(aucData, ccOFST_DEMOD_INFO9_BEST_COMB);
		mcSHOW_DBG_MSG(("m_total_valid_peak=%d,m_total_valid_combination=%d,m_em_min_ber_combination=%d\n",uc_data[0],uc_data[1],uc_data[2]));
		uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DEMOD_INFO9_IFFT_DIFF0);
		uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DEMOD_INFO9_IFFT_DIFF1);
		mcSHOW_DBG_MSG(("m_Ifft_Diff=%d\n",mcSIGN_2BYTE(uc_data[0],uc_data[1],8,8)))
	}
	
	mccSET_CMD2(aucData)= ccCMD_ID_DEMOD_GET_IFFT;
	if(mcHOST_CMD(psDemodCtx, aucData, ccOFST_DEMOD_IFFT_RSIZE, 1))
	{
		uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DEMOD_IFFT_POS_L);
		uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DEMOD_IFFT_POS_H);
		uc_data[2] = mccGET_QUEUE(aucData, ccOFST_DEMOD_IFFT_VAL_L);
		uc_data[3] = mccGET_QUEUE(aucData, ccOFST_DEMOD_IFFT_VAL_H);		
		mcSHOW_DBG_MSG(("ifft_position=%d,ifft_value=%d\n",mcSIGN_2BYTE(uc_data[1],uc_data[0],8,8),mcSIGN_2BYTE(uc_data[3],uc_data[2],8,8)))
	}
#endif
	mccSET_CMD2(aucData)= ccCMD_ID_DVBT2_STA_INFO0;
	if(mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_STA_INFO0_RSIZE, 1))
	{
		uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_STA_STC_WND_OFFSET_H);
		uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_STA_STC_WND_OFFSET_L);
		uc_data[2] = mccGET_QUEUE(aucData, ccOFST_DVBT2_STA_CH_LENGTH_H);
		uc_data[3] = mccGET_QUEUE(aucData, ccOFST_DVBT2_STA_CH_LENGTH_L);		
		mcSHOW_DBG_MSG4(("m_stc_window_offset=%d,i2_largest_distance=%d\n",mcUNSIGN_2BYTE(uc_data[0],uc_data[1],8,8),mcSIGN_2BYTE(uc_data[2],uc_data[3],8,8)));
		uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_STA_INFO_0);
		uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_STA_INFO_1);
		uc_data[2] = mccGET_QUEUE(aucData, ccOFST_DVBT2_STA_INFO_2);
		//mcSHOW_DBG_MSG(("m_total_valid_peak=%d,i2_largest_distance=%d,=%d\n",uc_data[0],uc_data[1],uc_data[2]));
		mcSHOW_DBG_MSG4(("m_total_valid_peak=%d,u_ch_profile_idx_P2=%d,\n",(uc_data[0]>>4),((uc_data[0]<<4)>>4)));
		mcSHOW_DBG_MSG4(("u_ch_profile_idx_DATA=%d,u_ch_profile_idx_FDI_ONLY=%d,\n",(uc_data[1]>>4),((uc_data[1]<<4)>>4)));
		mcSHOW_DBG_MSG4(("u_ch_profile_idx_tx2_P2=%d,u_ch_profile_idx_tx2_DATA=%d,\n",(uc_data[2]>>4),((uc_data[2]<<4)>>4)));
	}
}

//----------------------------------------------------------------------------- 
/*
 *  vDVBT2GetPara
 *  Get demod system parameters and update demod related context.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   void.   
 */
//-----------------------------------------------------------------------------   
void vDVBT2GetPara(DEMOD_CTX_T2 *psDemodCtx)
{
    UINT8   aucData[cMAX_READ_NUM + 1], ucTmp, ucTmp1;
    U32     u4Temp;

	// P1 parameters
	mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO1;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO1_SIZE, 1))
    {
        psDemodCtx->u1Ver1    = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO1_VER1);
        psDemodCtx->u1Ver0    = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO1_VER0);
        ucTmp                 = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO1_MD_STATUS0), 
                                            0x7F, 3);
		ucTmp >>= 1;
		psDemodCtx->u1P1D_Fft = ucTmp;
        psDemodCtx->u1MD_GI   = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO1_MD_STATUS1), 
                                            0x7, 0);
    }
	
	// L1-Pre parameters
    mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO14;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO14_SIZE, 1))
    {
        psDemodCtx->u1Fft      = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO14_L1_PRE_REG1), 
                                         0xF, 1);     
		psDemodCtx->u1BWExt    = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO14_L1_PRE_REG1), 
                                         0x80, 7);
		psDemodCtx->u1SISOMISO = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO14_L1_PRE_REG1), 
                                         0x70, 4);
        psDemodCtx->u1GI       = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO14_L1_PRE_REG2), 
                                            0x70, 4);
		psDemodCtx->u1PAPR     = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO14_L1_PRE_REG2), 
                                            0xF, 0);
		psDemodCtx->u1L1MOD    = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO14_L1_PRE_REG3), 
                                            0xF0, 4);
		psDemodCtx->u1Pilot    = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO14_L1_PRE_REG8), 
                                            0xF, 0);
		psDemodCtx->u1NumT2Frm = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO14_L1_PRE_REG16);
		ucTmp                  = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO14_L1_PRE_REG17);
		ucTmp                <<= 4;
		ucTmp1                 = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO14_L1_PRE_REG18),
											0xF0, 4);
		psDemodCtx->u2NumDataSym = (UINT16) ucTmp + (UINT16) ucTmp1;
    }

	// L1-Post parameters
    mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO15;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO15_SIZE, 1))
    {
		psDemodCtx->u2SubSlice = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO15_L1_POST_HEAD_0), 
                                         0xFF, 0);   
		psDemodCtx->u2SubSlice <<= 7;
		ucTmp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO15_L1_POST_HEAD_1), 0xFE, 1);
		psDemodCtx->u2SubSlice |= ucTmp;

		ucTmp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO15_L1_POST_HEAD_1), 0x1, 0);
		ucTmp <<= 7;
		psDemodCtx->u1NumPLP = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO15_L1_POST_HEAD_2), 
                                         0xFE, 1);
		psDemodCtx->u1NumPLP |= ucTmp;
		mcSHOW_DBG_MSG(("[vDVBT2GetPara]1821=0x%02x,1822=0x%02x,numPLP=%d\n",mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO15_L1_POST_HEAD_1),mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO15_L1_POST_HEAD_2),psDemodCtx->u1NumPLP));
		ucTmp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO15_L1_POST_HEAD_2), 0x1, 0);
		ucTmp <<= 3;
		psDemodCtx->u1NumAUX = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO15_L1_POST_HEAD_3), 
                                         0xE0, 5);
		psDemodCtx->u1NumAUX |= ucTmp;

		ucTmp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO15_L1_POST_HEAD_3), 0x1F, 0);
		ucTmp <<= 3;
		psDemodCtx->u1AUXCFG = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO15_L1_POST_HEAD_4), 
                                         0xE0, 5);
		psDemodCtx->u1AUXCFG |= ucTmp;
	}

	// PLP0 parameters
	mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO16;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO16_SIZE, 1))
    {
		psDemodCtx->u1PLP0ID   = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_0); 
		psDemodCtx->u1PLP0CR   = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_4), 
                                            0xE, 1);  
		ucTmp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_4), 
                                            0x1, 0);
		ucTmp <<= 2;
		psDemodCtx->u1PLP0MOD  = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_5), 
                                            0xC0, 6);
		psDemodCtx->u1PLP0MOD |= ucTmp;

		psDemodCtx->u1PLP0ROT  = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_5), 
                                            0x20, 5);
		psDemodCtx->u1PLP0FEC  = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_5), 
                                            0x18, 3);

		ucTmp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_6), 
                                            0x1, 0);
		ucTmp <<= 7;
		psDemodCtx->u1PLP0FRAME_INTERV = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_7), 
                                            0xFE, 1);
		psDemodCtx->u1PLP0FRAME_INTERV |= ucTmp;

		psDemodCtx->u1PLP0IL_TYPE = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_8), 
                                            0x1, 0);
		
		ucTmp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_7), 
                                            0x1, 0);
		ucTmp <<= 7;
		psDemodCtx->u1PLP0IL_LEN = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_8), 
                                            0xFE, 1);
		psDemodCtx->u1PLP0IL_LEN |= ucTmp;

		psDemodCtx->u1PLP0INBAND = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_9), 
                                            0x80, 7);
        
    }

	// PLP1 parameters
	if (psDemodCtx->u1NumPLP > 1)
	{
		mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO17;
		if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO17_SIZE, 1))
		{
			psDemodCtx->u1PLP1ID   = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_0); 
			psDemodCtx->u1PLP1CR   = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_4), 
				                                0xE, 1);  
			ucTmp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_4), 
				                                0x1, 0);
			ucTmp <<= 2;
			psDemodCtx->u1PLP1MOD  = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_5), 
				                                0xC0, 6);
			psDemodCtx->u1PLP1MOD |= ucTmp;

			psDemodCtx->u1PLP1ROT  = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_5), 
				                                0x20, 5);
			psDemodCtx->u1PLP1FEC  = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_5), 
				                                0x18, 3);	

			ucTmp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_6), 
				                                0x1, 0);
			ucTmp <<= 7;
			psDemodCtx->u1PLP1FRAME_INTERV = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_7), 
				                                0xFE, 1);
			psDemodCtx->u1PLP1FRAME_INTERV |= ucTmp;

			psDemodCtx->u1PLP1IL_TYPE = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_8), 
				                                0x1, 0);
		
			ucTmp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_7), 
				                                0x1, 0);
			ucTmp <<= 7;
			psDemodCtx->u1PLP1IL_LEN = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_8), 
				                                0xFE, 1);
			psDemodCtx->u1PLP1IL_LEN |= ucTmp;

			psDemodCtx->u1PLP1INBAND = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_9), 
				                                0x80, 7);
        
		}
	}

	// FEF parameters
	mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO18;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO18_SIZE, 1))
    {
		psDemodCtx->u1FEF_INTERV = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO18_L1_POST_CFG_FEF_5), 
                                            0x3F, 0); 
		psDemodCtx->u1FEF_INTERV <<= 2;
		ucTmp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO18_L1_POST_CFG_FEF_6), 
                                            0xCF, 6); 
		psDemodCtx->u1FEF_INTERV |= ucTmp;

		u4Temp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO18_L1_POST_CFG_FEF_2), 
                                            0xF, 0);
		u4Temp <<= 18;
		psDemodCtx->u1FEF_LEN = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO18_L1_POST_CFG_FEF_2), 
                                            0xFF, 0);
		psDemodCtx->u1FEF_LEN <<= 10;
		psDemodCtx->u1FEF_LEN |= u4Temp;
		u4Temp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO18_L1_POST_CFG_FEF_2), 
                                            0xFF, 0);
		u4Temp <<= 2;
		psDemodCtx->u1FEF_LEN |= u4Temp;
		u4Temp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO18_L1_POST_CFG_FEF_2), 
                                            0xC0, 6);
		psDemodCtx->u1FEF_LEN |= u4Temp;
    }
}




//----------------------------------------------------------------------------- 
/** vDVBT2GetPLPInfo
 *  Get DVBT2 MPLP ID
 *  @param  psDemodCtx  Pointer of context created by DVBT_DemodCtxCreate.
 *  @param  pPLPID               poiner of PLP ID table
 *  @param  u1PLP_StartIdx    PLP ID start index
 *  @retval   UINT8   
 */
//-----------------------------------------------------------------------------
UINT8 vDVBT2GetPLPInfo(DEMOD_CTX_T2 *psDemodCtx, UINT8* pPLPID, UINT8 u1PLP_StartIdx)
{
	UINT8   aucData[cMAX_READ_NUM + 1];

	mcSHOW_DBG_MSG4(("[MPLP]u1PLP_StartIdx = %d\n", u1PLP_StartIdx));
	
	mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_TS_PLP_ID_FB;
	mccSET_QUEUE2(aucData, ccOFST_DVBT2_PLP_ID_START) = u1PLP_StartIdx;
	
	if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_PLP_4ID_SIZE, ccOFST_DVBT2_PLP_4ID_SIZE + 1))
	{
		pPLPID[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_PLP_ID0); 
		pPLPID[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_PLP_ID1); 
		pPLPID[2] = mccGET_QUEUE(aucData, ccOFST_DVBT2_PLP_ID2); 
		pPLPID[3] = mccGET_QUEUE(aucData, ccOFST_DVBT2_PLP_ID3); 
	}	

	return (mccGET_QUEUE(aucData, ccOFST_DVBT2_PLP_ID_AVALIABLE)); 
}

void vDVBT2FECReset(DEMOD_CTX_T2 *psDemodCtx, UINT8 MPLPSetEn)
{
	UINT8   aucData[cMAX_READ_NUM + 1];

	mcSHOW_DBG_MSG(("vDVBT2FEC Reset and mult PLP setting status is  = %d\n", MPLPSetEn));
	
	mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_FEC_RESET;
	mccSET_QUEUE2(aucData, ccOFST_DVBT2_DJB_ENABEL) = MPLPSetEn;
	
	mcHOST_CMD(psDemodCtx, aucData, 0, ccOFST_DVBT2_DJB_TSIZE + 1);

}


void vDVBT2SetTunerSSICond(DEMOD_CTX_T2 *psDemodCtx)
{
    INT16 InOutVal[5];

    if (psDemodCtx->fgT2TsLock)
    {
        InOutVal[0]= (INT16)psDemodCtx->u1PLP0MOD;
        InOutVal[1]= (INT16)psDemodCtx->u1PLP0CR;

        InOutVal[2]= (INT16)psDemodCtx->u2RfAgcRssi;
        InOutVal[3]= (INT16)psDemodCtx->ucSldPraGainIdx;
        InOutVal[4]= psDemodCtx->s2SldPraOut;
    }
    else/* no signal */
    {
        InOutVal[0]= DVBT2_Qam_Mode_Unknow;
        InOutVal[1]= (INT16)psDemodCtx->u1PLP0CR;

        InOutVal[2]= (INT16)psDemodCtx->u2RfAgcRssi;
        InOutVal[3]= (INT16)psDemodCtx->ucSldPraGainIdx;
        InOutVal[4]= psDemodCtx->s2SldPraOut;
        mcSHOW_DBG_MSG3(("vDVBT2SetTunerSSICond: TPS unlock, no signal\n"));
    }
    //mcSHOW_DBG_MSG(("DVBT2: Mod=%d, Code Rate=%d, SLD_RFAGC=%d, SLD_IFAGC=%d, Legacy_Index=%d\n",InOutVal[0],InOutVal[1],InOutVal[2],InOutVal[3],InOutVal[4]));
	
	//DigTunerBypassI2C(TRUE);
    ITuner_OP( ITunerGetCtx(), itSetSSICond,0, &InOutVal);
	//DigTunerBypassI2C(FALSE);
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_UpdateInfo
 *  Get demod status, information and system parameters 
 *  and update demod related context.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   void.   
 */
//-----------------------------------------------------------------------------
void DVBT2_UpdateInfo(DEMOD_CTX_T2 *psDemodCtx)
{
    vDVBT2GetStatus(psDemodCtx);
    vDVBT2GetInfo(psDemodCtx);
    vDVBT2GetPara(psDemodCtx);
	vDVBT2SetTunerSSICond(psDemodCtx);
	
    //mcSHOW_DBG_MSG(("Update T2 Information\n!"));    
}

void   DVBT2_UpdateSimpleInfo(DEMOD_CTX_T2 *psDemodCtx)
{       
    UINT8   aucData[cMAX_READ_NUM + 1] = {0};
	UINT8   ucTmp = 0;
	UINT8	ucData1 = 0;
	UINT8	ucData2 = 0;

	#if 0
	UINT8   uc_data[4] = {0};


	    mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO5;
		if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO5_SIZE, 1))
		{
			//total error packets
			uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO5_BCH2_ERR_PKT0);
			uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO5_BCH2_ERR_PKT1);
			uc_data[2] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO5_BCH2_ERR_PKT2);
			psDemodCtx->u4BCHErrorPkt2 = mcUNSIGN_3BYTE(uc_data[2], uc_data[1], uc_data[0], 4);  
	
			//total packets
			uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO5_BCH2_TOT_PKT0);
			uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO5_BCH2_TOT_PKT1);
			uc_data[2] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO5_BCH2_TOT_PKT2);
			psDemodCtx->u4BCHTotalPkt2 = mcUNSIGN_3BYTE(uc_data[2], uc_data[1], uc_data[0], 4);
	
			//total error bits
			DVBT2_GetDemodReg(psDemodCtx, MT5136T2_REG_BCH1_MONIT6, uc_data, 3);
			psDemodCtx->u4PstBCHErrorBit2 = mcUNSIGN_3BYTE(uc_data[2], uc_data[1], uc_data[0], 8);
	
			DVBT2_GetDemodReg(psDemodCtx, MT5136T2_REG_BCH1_MONIT9, uc_data, 3);
			psDemodCtx->u4PstBCHCorBit2 = mcUNSIGN_3BYTE(uc_data[2], uc_data[1], uc_data[0], 8);
			psDemodCtx->u4PreBCHErrorBit2 = psDemodCtx->u4PstBCHCorBit2 + psDemodCtx->u4PstBCHErrorBit2;
	//		psDemodCtx->u4BCHErrorBit2 += u4_data;
		}

		#endif
		DVBT2_GetReg(psDemodCtx, 0x1821, &ucData1, 1);
		DVBT2_GetReg(psDemodCtx, 0x1822, &ucData2, 1);
		#if 0
		ucTmp = mcGET_FIELD(ucData1, 0x1, 0);
		ucTmp <<= 7;
		psDemodCtx->u1NumPLP = mcGET_FIELD(ucData2, 0xFE, 1);
		psDemodCtx->u1NumPLP |= ucTmp;
		#endif
		mcSHOW_DBG_MSG(("[DVBT2_UpdateSimpleInfo]not show numPLP Before host cmd @@@@@@@@@@0x1821 is 0x%0x,0x1822 is 0x%0x\n", ucData1,ucData2));

		mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO15;
			if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO15_SIZE, 1))
			{
				psDemodCtx->u2SubSlice = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO15_L1_POST_HEAD_0), 
												 0xFF, 0);	 
				psDemodCtx->u2SubSlice <<= 7;
				ucTmp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO15_L1_POST_HEAD_1), 0xFE, 1);
				psDemodCtx->u2SubSlice |= ucTmp;
		
				ucTmp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO15_L1_POST_HEAD_1), 0x1, 0);
				ucTmp <<= 7;
				psDemodCtx->u1NumPLP = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO15_L1_POST_HEAD_2), 
												 0xFE, 1);
				psDemodCtx->u1NumPLP |= ucTmp;
				mcSHOW_DBG_MSG(("[DVBT2_UpdateSimpleInfo]host cmd head1=%d,head2=%d,u1NumPLP=%d\n",mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO15_L1_POST_HEAD_1),mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO15_L1_POST_HEAD_2),psDemodCtx->u1NumPLP));
				ucTmp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO15_L1_POST_HEAD_2), 0x1, 0);
				ucTmp <<= 3;
				psDemodCtx->u1NumAUX = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO15_L1_POST_HEAD_3), 
												 0xE0, 5);
				psDemodCtx->u1NumAUX |= ucTmp;
		
				ucTmp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO15_L1_POST_HEAD_3), 0x1F, 0);
				ucTmp <<= 3;
				psDemodCtx->u1AUXCFG = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO15_L1_POST_HEAD_4), 
												 0xE0, 5);
				psDemodCtx->u1AUXCFG |= ucTmp;

				//get numPLP from reg 0x1821 1822,
				//numPLP = 0x1821[0]+0x1822[7:1]
			
			DVBT2_GetReg(psDemodCtx, 0x1821, &ucData1, 1);
			DVBT2_GetReg(psDemodCtx, 0x1822, &ucData2, 1);
            #if 0
			ucTmp = mcGET_FIELD(ucData1, 0x1, 0);
			ucTmp <<= 7;
			psDemodCtx->u1NumPLP = mcGET_FIELD(ucData2, 0xFE, 1);
			psDemodCtx->u1NumPLP |= ucTmp;
			#endif
			mcSHOW_DBG_MSG(("[DVBT2_UpdateSimpleInfo]after host cmd @@@@@@@@@@0x1821 is 0x%0x,0x1822 is 0x%0x,u1NumPLP=%d\n", ucData1,ucData2,psDemodCtx->u1NumPLP));
			}

				// PLP0 parameters
		mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO16;
	    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO16_SIZE, 1))
	    {
			psDemodCtx->u1PLP0ID   = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_0); 
			psDemodCtx->u1PLP0CR   = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_4), 
	                                            0xE, 1);  
			ucTmp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_4), 
	                                            0x1, 0);
			ucTmp <<= 2;
			psDemodCtx->u1PLP0MOD  = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_5), 
	                                            0xC0, 6);
			psDemodCtx->u1PLP0MOD |= ucTmp;

			psDemodCtx->u1PLP0ROT  = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_5), 
	                                            0x20, 5);
			psDemodCtx->u1PLP0FEC  = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_5), 
	                                            0x18, 3);

			ucTmp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_6), 
	                                            0x1, 0);
			ucTmp <<= 7;
			psDemodCtx->u1PLP0FRAME_INTERV = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_7), 
	                                            0xFE, 1);
			psDemodCtx->u1PLP0FRAME_INTERV |= ucTmp;

			psDemodCtx->u1PLP0IL_TYPE = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_8), 
	                                            0x1, 0);
			
			ucTmp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_7), 
	                                            0x1, 0);
			ucTmp <<= 7;
			psDemodCtx->u1PLP0IL_LEN = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_8), 
	                                            0xFE, 1);
			psDemodCtx->u1PLP0IL_LEN |= ucTmp;

			psDemodCtx->u1PLP0INBAND = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO16_L1_POST_CFG_PLP0_9), 
	                                            0x80, 7);
	        
	    }

		// PLP1 parameters
		if (psDemodCtx->u1NumPLP > 1)
		{
			mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO17;
			if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO17_SIZE, 1))
			{
				psDemodCtx->u1PLP1ID   = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_0); 
				psDemodCtx->u1PLP1CR   = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_4), 
					                                0xE, 1);  
				ucTmp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_4), 
					                                0x1, 0);
				ucTmp <<= 2;
				psDemodCtx->u1PLP1MOD  = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_5), 
					                                0xC0, 6);
				psDemodCtx->u1PLP1MOD |= ucTmp;

				psDemodCtx->u1PLP1ROT  = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_5), 
					                                0x20, 5);
				psDemodCtx->u1PLP1FEC  = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_5), 
					                                0x18, 3);	

				ucTmp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_6), 
					                                0x1, 0);
				ucTmp <<= 7;
				psDemodCtx->u1PLP1FRAME_INTERV = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_7), 
					                                0xFE, 1);
				psDemodCtx->u1PLP1FRAME_INTERV |= ucTmp;

				psDemodCtx->u1PLP1IL_TYPE = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_8), 
					                                0x1, 0);
			
				ucTmp = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_7), 
					                                0x1, 0);
				ucTmp <<= 7;
				psDemodCtx->u1PLP1IL_LEN = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_8), 
					                                0xFE, 1);
				psDemodCtx->u1PLP1IL_LEN |= ucTmp;

				psDemodCtx->u1PLP1INBAND = mcGET_FIELD(mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO17_L1_POST_CFG_PLP1_9), 
					                                0x80, 7);
	        
			}
		}
	vDVBT2SetTunerSSICond(psDemodCtx);

}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_TunerI2c
 *  Switch tuner bypass mode 
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  fgSwitchOn  Flag to switch bypass mode on/off (TRUE: ON, FALSE: OFF)
 *  @retval   void.   
 */
//-----------------------------------------------------------------------------
void DVBT2_TunerI2c(DEMOD_CTX_T2 *psDemodCtx, BOOL fgSwitchOn)
{
#if ((defined(CC_MT5136)&&defined(CC_MT5880)) || (defined(CC_MT5136)&&defined(CC_MT5881)) || (defined(CC_MT5136)&&defined(CC_MT5398))) //LL for 5136+5881
    UCHAR   uData;
    //UCHAR   aucData[cMAX_READ_NUM + 1];     // For Host Cmd
    #if 0
    if (fgSwitchOn)                         // Access Tuner
    {
        mccSET_CMD2(aucData) = ccCMD_ID_TUNER_I2C_ONOFF;
		mccSET_QUEUE2(aucData, ccOFST_TUNER_I2C_PULLUP_DEL) = 0x14; // tune for MT5136T2 ASIC
        mccSET_QUEUE2(aucData, ccOFST_TUNER_I2C_ONOFF) = 1;
        if (psDemodCtx->ucEnFpga)
            mccSET_QUEUE2(aucData, ccOFST_TUNER_I2C_DEGLITCH) = 0x04; // FPGA:0x04
        else
            mccSET_QUEUE2(aucData, ccOFST_TUNER_I2C_DEGLITCH) = 0x07; // ASIC:0x07
    }
    else                                    // Bypass Tuner
    {
        mccSET_CMD2(aucData) = ccCMD_ID_TUNER_I2C_ONOFF;
		mccSET_QUEUE2(aucData, ccOFST_TUNER_I2C_PULLUP_DEL) = 0x14;
        mccSET_QUEUE2(aucData, ccOFST_TUNER_I2C_ONOFF) = 0;
        mccSET_QUEUE2(aucData, ccOFST_TUNER_I2C_DEGLITCH) = 0x07;
    }

    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_TUNER_I2C_ONOFF_RSIZE, ccOFST_TUNER_I2C_ONOFF_SIZE+1))
    {
      //  mcDELAY_MS(20);  //Mingsheng 2012 7/27 ; Remove redundant code to decrease the lock time performance.
        //mcDELAY_MS(150); // 20120627 Hauze : 20ms delay will make I2C to be failed in ATD with the combination of MT5880 + MT5136A + SI2158
    }
	#else
		if (fgSwitchOn) 
	    {
	        /* ByPass */ 
	        uData = 0x07;
	        DVBT2_SetReg(psDemodCtx, 0x45, &uData , 1);
	        uData = 0x14;
	        DVBT2_SetReg(psDemodCtx, 0x46, &uData , 1);
	        DVBT2_GetReg(psDemodCtx, 0x44, &uData , 1);
			uData |= 0x01;
			DVBT2_SetReg(psDemodCtx, 0x44, &uData , 1);
			mcSHOW_DBG_MSG(("[MS DEBUG]Tuner Bypass on\n"));
    }
	    else
	    {
	        /* No ByPass */
		    DVBT2_GetReg(psDemodCtx, 0x44, &uData , 1);
			uData &= 0xFE;
			DVBT2_SetReg(psDemodCtx, 0x44, &uData , 1);
	        uData = 0x07;
	        DVBT2_SetReg(psDemodCtx, 0x45, &uData , 1);
	        uData = 0x14;
	        DVBT2_SetReg(psDemodCtx, 0x46, &uData , 1);

			mcSHOW_DBG_MSG(("[MS DEBUG]Tuner Bypass off\n"));
	    }
		#endif
#else
    mcSHOW_USER_MSG(("T2 Tuner Bypass call do nothing\n!"));
#endif	
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_DriverStart
 *  Start demod driver in firmware.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  u1Mon1Sel  Monitor1 driver select. (0: None, 1: Auto-Reacquisition).
 *  @param  u1Mon2Sel  Monitor2 driver select. (0: None, 1: TPS sync, 2: SPS sync auto, 3: SPS sync manual
 *  @retval   void.   
 */
//-----------------------------------------------------------------------------
void DVBT2_DriverStart(DEMOD_CTX_T2 *psDemodCtx, UINT8 u1Mon1Sel, UINT8 u1Mon2Sel)
{
    UCHAR   aucData[cMAX_READ_NUM + 1];

    mccSET_CMD2(aucData) = ccCMD_ID_START_DRIVER;
    // 0:none 
    // 1:Adv ReAcq
    mccSET_QUEUE2(aucData, ccOFST_SET_DRIVER_MON1) = u1Mon1Sel;   
    // 0:none
    // 1:TPS sync
    // 2:SPS sync auto
    // 3:SPS sync manual
    mccSET_QUEUE2(aucData, ccOFST_SET_DRIVER_MON2) = u1Mon2Sel;   
    mcHOST_CMD(psDemodCtx, aucData, 0, ccOFST_SET_DRIVER_SIZE + 1);
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_DriverStop
 *  Stop demod driver in firmware.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   void.   
 */
//-----------------------------------------------------------------------------
void DVBT2_DriverStop(DEMOD_CTX_T2 *psDemodCtx)
{
UCHAR   aucData[cMAX_READ_NUM + 1];

    mccSET_CMD2(aucData) = ccCMD_ID_STOP_DRIVER;
    mcHOST_CMD(psDemodCtx, aucData, 0, 1);
}


void DVBT2_SetEarlyBreak(DEMOD_CTX_T2 *psDemodCtx, UINT8 EarlyBreakCnt_Max)
{
    // UINT16 u2_data = 0;
    UCHAR   aucData[cMAX_READ_NUM + 1];
    UINT8 DVBT2_EarlyBreak_current;

    // u2_data = clk_div & 0x0fff;
//   if(ucEarlyBreakCnt_Max)
    mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_SET_EARLYBREAK;
    mccSET_QUEUE2(aucData, ccOFST_DVBT2_SET_EARLYBREAK_MAX) = EarlyBreakCnt_Max;
    //mccSET_QUEUE2(aucData, ccOFST_I2C_CLK_DIV_L) = mcLOW_BYTE( u2_data);
    if(mcHOST_CMD(psDemodCtx, aucData, 1, ccOFST_DVBT2_SET_EARLYBREAK_RSIZE+1))
    {
        DVBT2_EarlyBreak_current = mccGET_QUEUE(aucData, ccOFST_DVBT2_SET_EARLYBREAK_CURRENT);
        mcSHOW_USER_CRI_MSG(("[DVBT2]current earlybreak = DVBT2_EarlyBreak_current*20=%dms, set = %dms\n", DVBT2_EarlyBreak_current*20, EarlyBreakCnt_Max*20));
    }

}
//-----------------------------------------------------------------------------
/*
 *  DVBT2_MemoryRepair
 *  This function perform memory repair before DVBT2_ChipInit & s4LoadCode.
 *  @retval   1 : I2C error.
 *  @retval   0 : OK.
 */
//-----------------------------------------------------------------------------

UINT8 DVBT2_MemoryRepair(DEMOD_CTX_T2 *psDemodCtx)
{
    UCHAR	ucValue;

    UCHAR	ucCheck[2];

    //UINT8 ucTmp = 0;

    //UINT8 ii = 0;

    mcSHOW_USER_CRI_MSG(("[DVBT2_MemoryRepair]************************\n"));
#if 0
    DVBT2_GetReg(psDemodCtx, 0xAA5, &ucTmp, 1);
    mcSHOW_USER_CRI_MSG(("\033[42;30m[DVBT2_MemoryRepair]0xAA5= 0x%02x\033[0m\n", ucTmp));
    mcSET_BIT(ucTmp, 6);
    mcSET_BIT(ucTmp, 4);
    mcCLR_BIT(ucTmp, 7);
    mcCLR_BIT(ucTmp, 5);
    DVBT2_SetReg(psDemodCtx, 0xAA5, &ucTmp, 1);
    DVBT2_GetReg(psDemodCtx, 0xAA5, &ucTmp, 1);
    mcSHOW_USER_CRI_MSG(("\033[42;30m[DVBT2_MemoryRepair]0xAA5= 0x%02x\033[0m\n", ucTmp));
#endif

	//mcDELAY_MS(5);

    //PLL setting
    //ucValue = 0x9F;
    //DVBT2_SetReg(psDemodCtx, 0x0910, &ucValue,1);	//pll setting
    //mcSHOW_USER_CRI_MSG(("[DVBT2_MemoryRepair]%d\n", ii));
    ucValue = 0xC0;
    DVBT2_SetReg(psDemodCtx, 0x0918, &ucValue,1);

    ucValue = 0x20;
    DVBT2_SetReg(psDemodCtx, 0x0917, &ucValue,1);

    ucValue = 0x2F;
    DVBT2_SetReg(psDemodCtx, 0x0914, &ucValue,1);

    ucValue = 0x97;
    DVBT2_SetReg(psDemodCtx, 0x0915, &ucValue,1);

    ucValue = 0x1F;
    DVBT2_SetReg(psDemodCtx, 0x0910, &ucValue,1);

    //wait
    mcDELAY_MS(5);
    ucValue = 0x2E;
    DVBT2_SetReg(psDemodCtx, 0x0914, &ucValue,1);

    ucValue = 0x20;
    DVBT2_SetReg(psDemodCtx, 0x0917, &ucValue,1);

    ucValue = 0x00;
    DVBT2_SetReg(psDemodCtx, 0x0909, &ucValue,1);

    ucValue = 0x82;
    DVBT2_SetReg(psDemodCtx, 0x0903, &ucValue,1);

    ucValue = 0xA0;
    DVBT2_SetReg(psDemodCtx, 0x0902, &ucValue,1);
    ucValue = 0xA8;
    DVBT2_SetReg(psDemodCtx, 0x0902, &ucValue,1);
    ucValue = 0x48;
    DVBT2_SetReg(psDemodCtx, 0x0921, &ucValue,1);
    ucValue = 0x27;
    DVBT2_SetReg(psDemodCtx, 0x0924, &ucValue,1);
    ucValue = 0x67;
    DVBT2_SetReg(psDemodCtx, 0x0924, &ucValue,1);
    ucValue = 0x27;
    DVBT2_SetReg(psDemodCtx, 0x0927, &ucValue,1);
    ucValue = 0x67;
    DVBT2_SetReg(psDemodCtx, 0x0927, &ucValue,1);
    //clock set
    ucValue = 0x00;
    DVBT2_SetReg(psDemodCtx, 0x0E50, &ucValue,1);	
    //ucValue = 0x01;
    //DVBT2_SetReg(psDemodCtx, 0x0E72, &ucValue,1);
    //ucValue = 0x01;
    //DVBT2_SetReg(psDemodCtx, 0x0E73, &ucValue,1);
    ucValue = 0x01;
    DVBT2_SetReg(psDemodCtx, 0x0E51, &ucValue,1);
    ucValue = 0x01;
    DVBT2_SetReg(psDemodCtx, 0x0E52, &ucValue,1);
    ucValue = 0x03;
    DVBT2_SetReg(psDemodCtx, 0x0E55, &ucValue,1);
    ucValue = 0x01;
    DVBT2_SetReg(psDemodCtx, 0x0E56, &ucValue,1);
    ucValue = 0x01;
    DVBT2_SetReg(psDemodCtx, 0x0E70, &ucValue,1);
    ucValue = 0x01;
    DVBT2_SetReg(psDemodCtx, 0x0E71, &ucValue,1);
    ucValue = 0x01;
    DVBT2_SetReg(psDemodCtx, 0x0E6C, &ucValue,1);
    ucValue = 0x01;
    DVBT2_SetReg(psDemodCtx, 0x0E6D, &ucValue,1);

    //ucValue = 0x01;
    //DVBT2_SetReg(psDemodCtx, 0x0E59, &ucValue,1);mcSHOW_USER_CRI_MSG(("[DVBT2_MemoryRepair]%d\n", ii++));
    //ucValue = 0x01;
    //DVBT2_SetReg(psDemodCtx, 0x0E5A, &ucValue,1);mcSHOW_USER_CRI_MSG(("[DVBT2_MemoryRepair]%d\n", ii++));
    //ucValue = 0x05;
    //DVBT2_SetReg(psDemodCtx, 0x0E66, &ucValue,1);mcSHOW_USER_CRI_MSG(("[DVBT2_MemoryRepair]%d\n", ii++));
    //ucValue = 0x01;
    //DVBT2_SetReg(psDemodCtx, 0x0E67, &ucValue,1);mcSHOW_USER_CRI_MSG(("[DVBT2_MemoryRepair]%d\n", ii++));
    //ucValue = 0x60;
    //DVBT2_SetReg(psDemodCtx, 0x0DD2, &ucValue,1);mcSHOW_USER_CRI_MSG(("[DVBT2_MemoryRepair]%d\n", ii++));
    #if 1
    //mbist reset release
    ucValue = 0x01;
    DVBT2_SetReg(psDemodCtx, 0x0E37, &ucValue,1);	//mbist reset release
    ucValue = 0x55;
    DVBT2_SetReg(psDemodCtx, 0x0E38, &ucValue,1);
    ucValue = 0xAA;
    DVBT2_SetReg(psDemodCtx, 0x0E39, &ucValue,1);
    ucValue = 0x15;
    DVBT2_SetReg(psDemodCtx, 0x0E1B, &ucValue,1);
    ucValue = 0x40;
    DVBT2_SetReg(psDemodCtx, 0x0E1C, &ucValue,1);
    //mbist hw repair enable
    ucValue = 0xF6;
    DVBT2_SetReg(psDemodCtx, 0x0AA1, &ucValue,1);	//mbist hw repair enable
    ucValue = 0x76;
    DVBT2_SetReg(psDemodCtx, 0x0ACD, &ucValue,1);
    //mbist enable
    ucValue = 0x3F;
    DVBT2_SetReg(psDemodCtx, 0x0E1B, &ucValue,1);	//mbist enable
    ucValue = 0xC0;
    DVBT2_SetReg(psDemodCtx, 0x0E1C, &ucValue,1);
    ucValue = 0xF6;
    DVBT2_SetReg(psDemodCtx, 0x0ACD, &ucValue,1);
    //mbist hw repair start
    ucValue = 0xFE;
    DVBT2_SetReg(psDemodCtx, 0x0AA1, &ucValue,1);	//mbist hw repair start
    ucValue = 0xFE;
    DVBT2_SetReg(psDemodCtx, 0x0ACD, &ucValue,1);
    //wait delay or register ABB[0] and AD7[4] == 1
    mcDELAY_MS(6);

    DVBT2_GetReg(psDemodCtx,0xABB,&ucCheck[0],1);
    DVBT2_GetReg(psDemodCtx,0xAD7,&ucCheck[1],1);
    mcSHOW_USER_CRI_MSG(("[DVBT2_MemoryRepair]0xABB[0]=%d,0xAD7[4]=%d\n",mcTEST_BIT(ucCheck[0],0),mcTEST_BIT(ucCheck[1],4)));
    mcSHOW_USER_CRI_MSG(("[DVBT2_MemoryRepair]0xABB=0x%02x,0xAD7=0x%02x\n",ucCheck[0],ucCheck[1]));
    if(!mcTEST_BIT(ucCheck[0],0) || !mcTEST_BIT(ucCheck[1],4))
    {
        mcDELAY_MS(5);
    }

#if 0
    while(!mcTEST_BIT(ucCheck[0],0) || !mcTEST_BIT(ucCheck[1],4))
    {
        DVBT2_GetReg(psDemodCtx,0xABB,&ucCheck[0],1);
        DVBT2_GetReg(psDemodCtx,0xAD7,&ucCheck[1],1);
        mcSHOW_USER_CRI_MSG(("\033[42;30m[DVBT2_MemoryRepair]0xABB[0]=%d,0xAD7[4]=%d\033[0m\n",mcTEST_BIT(ucCheck[0],0),mcTEST_BIT(ucCheck[1],4)));
        mcSHOW_USER_CRI_MSG(("\033[42;30m[DVBT2_MemoryRepair]0xABB=0x%02x,0xAD7[4]=0x%02x\033[0m\n",ucCheck[0],ucCheck[1]));
        ii++;
        if ( ii >= 100 )
        {
            break;
        }
    }
#endif


    //mbist disable
    ucValue = 0x00;
    DVBT2_SetReg(psDemodCtx, 0x0E1B, &ucValue,1);	//mbist disable
    ucValue = 0x00;
    DVBT2_SetReg(psDemodCtx, 0x0E1C, &ucValue,1);
    ucValue = 0xE6;
    DVBT2_SetReg(psDemodCtx, 0x0AA1, &ucValue,1);
    ucValue = 0x66;
    DVBT2_SetReg(psDemodCtx, 0x0ACD, &ucValue,1);

    //wait for result
    //ii = 0;
    DVBT2_GetReg(psDemodCtx,0xABB,&ucCheck[0],1);
    DVBT2_GetReg(psDemodCtx,0xAD7,&ucCheck[1],1);
    mcSHOW_USER_CRI_MSG(("[DVBT2_MemoryRepair]0xABB[1]=%d,0xAD7[5]=%d\n",mcTEST_BIT(ucCheck[0],1),mcTEST_BIT(ucCheck[1],5)));
	mcSHOW_USER_CRI_MSG(("[DVBT2_MemoryRepair]0xABB[1]=0x%02x,0xAD7[5]=0x%02x\n",ucCheck[0],ucCheck[1]));

#if 0
	//read reg for J
	ucValue = 0x00;
    DVBT2_SetReg(psDemodCtx, 0x0AA6, &ucValue,1);
	for(ii=0;ii<18;ii++)
	{
		DVBT2_GetReg(psDemodCtx,0xAB0,&ucCheck[0],1);		
		mcSHOW_USER_CRI_MSG(("[DVBT2_MemoryRepair]ii=%d,0xAB0[1]=0x%02x\n",ii,ucCheck[0]));
	}
	for(ii=0;ii<13;ii++)//0xAB1~0xAC3
	{
		DVBT2_GetReg(psDemodCtx,0xAB1,&ucCheck[0],1);
		mcSHOW_USER_CRI_MSG(("[DVBT2_MemoryRepair]ii=%d,0x%x=0x%02x\n",ii,(0xAB1+ii),ucCheck[0]));
	}
	for(ii=0;ii<10;ii++)//0xAD0~0xAD9
	{
		DVBT2_GetReg(psDemodCtx,0xAD0,&ucCheck[0],1);
		mcSHOW_USER_CRI_MSG(("[DVBT2_MemoryRepair]ii=%d,0x%x=0x%02x\n",ii,(0xAD0+ii),ucCheck[0]));
	}
#endif
#if 0
    while(!mcTEST_BIT(ucCheck[0],1) || !mcTEST_BIT(ucCheck[1],5))
    {
        DVBT2_GetReg(psDemodCtx,0xABB,&ucCheck[0],1);
        DVBT2_GetReg(psDemodCtx,0xAD7,&ucCheck[1],1);
        mcSHOW_USER_CRI_MSG(("\033[44;30m[DVBT2_MemoryRepair]0xABB[1]=%d,0xAD7[5]=%d\033[0m\n",mcTEST_BIT(ucCheck[0],1),mcTEST_BIT(ucCheck[1],5)));
        mcSHOW_USER_CRI_MSG(("\033[44;30m[DVBT2_MemoryRepair]0xABB=0x%02x, 0xAD7=0x%02x\033[0m\n",ucCheck[0],ucCheck[1]));
        ii++;
        if ( ii >= 100) //here, reg value may be not expected,  driver flow must go on. so, max read reg times = 100.
        {
            break;
        }
    }
#endif

  #endif

    return 0;

}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_Initialize
 *  Initialize demod context.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  ucI2cAddress  Demod I2C address.
 *  @param  fgLoadCode  Flag to check if need to downlaod firmware code. Unused.
 *  @retval   DRVAPI_TUNER_OK : OK.
 *  @retval   DRVAPI_TUNER_ERROR : Fail.   
 */
//-----------------------------------------------------------------------------
INT32 DVBT2_Initialize(DEMOD_CTX_T2 *psDemodCtx, UINT8 ucI2cAddress, BOOL fgLoadCode)
{
    //UCHAR ucData ;
    //UCHAR aucData[cMAX_READ_NUM + 1];
    
	#if defined(CC_MT5136)
	UINT8                   u1Val = 0 ;
	//UINT32                  u4Ofst = 0;
	#endif

    psDemodCtx->u2ErrorPkt = 0;
    psDemodCtx->u2TotalPkt = 0;
    psDemodCtx->u4CorrBit = 0;
    psDemodCtx->u1Bw = MT5136T2_BW_8MHz;
    psDemodCtx->fgEn24MClk = FALSE;
    psDemodCtx->ucEnFpga = FALSE;
    psDemodCtx->I2cAddress = ucI2cAddress;
    psDemodCtx->ucConnType = PORT_CFG_MTK;
    psDemodCtx->fgDemodReset = FALSE;
    psDemodCtx->ucTsIfFormat = 0x8C; // LL 0x8c to 0x8 for 5136+5881  //changed from 0x0C to 0x8C due to MT5360B, and no impact to MT539x and MT5360A
    psDemodCtx->ucTsIfCtrl = 0x00;
    psDemodCtx->ucTsIfRate = 0x07;
    psDemodCtx->u1ChSearchMode = CH_SEARCH_BLIND;
    psDemodCtx->fgSelectANIR = TRUE;
    psDemodCtx->u1MdMode = 0xF0;
    psDemodCtx->s1FreqBias40khz = 0;
    //psDemodCtx->sTunerCtx.m_DemodVdd = 33;
    //psDemodCtx->sTunerCtx.m_Xtal = 0;
    psDemodCtx->m_u2RomSize = 0x0000;
    psDemodCtx->m_fgRomPatch = TRUE;
	psDemodCtx->s2snr_cc= 0;
	#if DVBT2_FROM_PHILIPS
	psDemodCtx->u1_wSnr = 20;
	psDemodCtx->u1_wBer = 80;
	psDemodCtx->i2_topBer = 133000;
	#endif

	psDemodCtx->fgT2TsLockPre = FALSE;//for SQI 2.3	
	for(psDemodCtx->ii=0;psDemodCtx->ii<5;psDemodCtx->ii++)
	{
		psDemodCtx->u4TotalPkt2[psDemodCtx->ii]=0;
		psDemodCtx->u4ErrorBit2[psDemodCtx->ii]=0;
	}
	psDemodCtx->ii=0;

#if 0
    //psDemodCtx->sTunerCtx.m_aucPara[ 4] = (U8)( 0);
    //psDemodCtx->sTunerCtx.m_aucPara[ 5] = (U8)(-128);
    //psDemodCtx->sTunerCtx.m_aucPara[ 8] = (U8)(-31);//(U8)C_U8_DNOS404_MAX_IF_GAIN_SET;
    //psDemodCtx->sTunerCtx.m_aucPara[ 9] = (U8)(-97);//(U8)C_U8_DNOS404_MIN_IF_GAIN_SET;
    //psDemodCtx->sTunerCtx.m_aucPara[10] = 0;//(U8)C_U8_DNOS404_MAX_RF_GAIN_SET;
    //psDemodCtx->sTunerCtx.m_aucPara[11] = 10;//(U8)C_U8_DNOS404_MIN_RF_GAIN_SET;
    //psDemodCtx->sTunerCtx.m_aucPara[12] = 50;//(U8)C_U8_DNOS404_IF_GAIN_RANGE;
    //psDemodCtx->sTunerCtx.m_aucPara[13] = 30;//(U8)C_U8_DNOS404_RF_GAIN_RANGE;
    //psDemodCtx->sTunerCtx.m_aucPara[14] = 26;//C_DNOS404_POWER_CALIBRATE;
#else
    // For Sony si Tuner.
    //psDemodCtx->sTunerCtx.m_aucPara[2] = (U8)C_U8_RE201_AGC_IF_SLP;
    //psDemodCtx->sTunerCtx.m_aucPara[3] = (U8)C_U8_RE201_AGC_IF_INT;
    //psDemodCtx->sTunerCtx.m_aucPara[4] = (U8)C_U8_RE201_AGC_IF_MAX;
    //psDemodCtx->sTunerCtx.m_aucPara[5] = (U8)C_U8_RE201_AGC_IF_MIN;
    //psDemodCtx->sTunerCtx.m_aucPara[7] = 36;
#endif

    psDemodCtx->m_TunerMode = 0;
    psDemodCtx->ucSramSize = 56;

	
	DVBT2_MemoryRepair(psDemodCtx);
	#if defined(CC_MT5136)
		//Mingsheng test bradcust type
		//u4Ofst = 0x51e;
		//u4Ofst = DRVCUST_OptGet(eBrdcstSrcOfst0);
		//VERIFY(0 == EEPROM_Read((UINT64)u4Ofst, (UINT32)(void *)&u1Val, 1));
		//mcSHOW_USER_MSG(("[MS debug]DVBT2 TunerOpen EEPROM_Read 0x%0x = 0x%0x!\n",u4Ofst, u1Val));
		u1Val = DVBT2_DVBT_GetEEPromBrdcstType(psDemodCtx);
	#endif
	mcSHOW_USER_MSG(("[MS1] u1Val is 0x%x , cDEMOD_DVBT2_MODE is 0x%x!\n", u1Val, cDEMOD_DVBT2_MODE));
	    
	if (u1Val !=  cDEMOD_DVBT2_MODE)
	{   
	    mcSHOW_USER_MSG(("[MS]No  DVBT2_Initialize, RETUEN\n"));
		return (DRVAPI_TUNER_OK);
	}
	else 
	{   
	  mcSHOW_USER_MSG(("[MS1]Need DVBT/T2_Initialize\n"));
	}

#if 1
    DVBT2_ChipInit(psDemodCtx);

    if (fgLoadCode)
    {   
        DVBT2_DVBT_SetEEPromBrdcstType(psDemodCtx);
        if (s4LoadCode(psDemodCtx, 33, ucDvbt2CodeImage, sizeof(ucDvbt2CodeImage), cMAX_I2C_LEN, TRUE))
        {            
            return (DRVAPI_TUNER_ERROR);
        }
    	if(DVBT2_T_T2_Init_Config(psDemodCtx))
    	{
        	mcSHOW_USER_MSG(("psDemodCtx NULL OK!\n"));
    	}

        // Set TS IF
        #if 0 // LL for db10-2
            	#if  (defined(CC_MT5136) && defined (CC_MT5881))
        DVBT_SetTsIf(psDemodCtx, psDemodCtx->ucTsIfFormat, psDemodCtx->ucTsIfCtrl, psDemodCtx->ucTsIfRate);// LL test
       DVBT2_SetTsIf(psDemodCtx, psDemodCtx->ucTsIfFormat, psDemodCtx->ucTsIfCtrl, psDemodCtx->ucTsIfRate);// LL test
       mcSHOW_USER_CRI_MSG(("[DVBT2_Initialize]DVBT_SetTsIf DVBT2_SetTsIf\n"));
      #endif
	  #endif
        // AGC crrve setting
//        UCHAR   ucPara[ccCMD_REG_NUM];
//        ucPara[0] = ccCMD_ID_SET_TUNER_PARA0;
        //for (UCHAR ii = 0; ii < 4; ii++)
        //    ucPara[ii+1] = psDemodCtx->sTunerCtx.m_aucPara[ii+2];  // write m_aucPara[2-5]
//        vSetIntfPara(psDemodCtx, ucPara, ccOFST_SET_TUNER_PARA0_SIZE+1);
        
        // set agc parameters
//        ucPara[0] = ccCMD_ID_SET_TUNER_PARA1;
        //ucPara[1] = psDemodCtx->sTunerCtx.m_aucPara[7];
//        ucPara[2] = 0x7F;//(psDemodCtx->sTunerCtx.m_aucPara[6] & 0x3F) | 0x40;            // kp, ki; LF_CTRL
        //ucPara[3] = mcLOW_BYTE(psDemodCtx->sTunerCtx.m_IF_Freq);   // IF_FREQ_LOW
        //ucPara[4] = mcHIGH_BYTE(psDemodCtx->sTunerCtx.m_IF_Freq);  // IF_FREQ_HIGH
//        vSetIntfPara(psDemodCtx, ucPara, ccOFST_SET_TUNER_PARA1_SIZE+1);
        
        // set to blind search mode
//        vSetChSearchMode(psDemodCtx);
		#if 0 //no need to get FW version,lei130823
        // Ken, GetVersion for previous host command complete, 090114
        mccSET_CMD2(aucData) = ccCMD_ID_GET_VER;
        if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_VER_SIZE, 1))
        {
            mcSHOW_USER_MSG(("DVBTx_Initialize OK!\n"));
        }        
		#endif

    }

    return (DRVAPI_TUNER_OK);
#endif	
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_DemodReset
 *  Demod Reset.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   DRVAPI_TUNER_OK : OK.
 *  @retval   DRVAPI_TUNER_ERROR : Fail.   
 */
//-----------------------------------------------------------------------------
INT32 DVBT2_DemodReset(DEMOD_CTX_T2 *psDemodCtx)
{
	UCHAR   aucData[cMAX_READ_NUM + 1];

    if (!psDemodCtx)
        return DRVAPI_TUNER_ERROR;

    mccSET_CMD2(aucData) = ccCMD_ID_DEMOD_RESET;
    if (!mcHOST_CMD(psDemodCtx, aucData, ccOFST_DEMOD_RESET_SIZE, 1))
    {
        return DRVAPI_TUNER_ERROR;
    }
    
    psDemodCtx->fgDemodReset = TRUE;

    return (DRVAPI_TUNER_OK);
}

/**********************************************************************/
/*                  Query APIs                                        */
/**********************************************************************/

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_GetIfAgcVol
 *  Get IF AGC voltage.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   (UINT16) IF AGC voltage in mV.   
 */
//-----------------------------------------------------------------------------
UINT16 DVBT2_GetIfAgcVol(DEMOD_CTX_T2 *psDemodCtx)
{
    UINT16 u2IfAgcVol = 0;
    INT32  s4Temp;

    s4Temp = (psDemodCtx->s1IfAgc) * 3300;
    s4Temp = s4Temp >> 8; // s4Temp /= 256;
    u2IfAgcVol = s4Temp + 1650;

    return u2IfAgcVol;
}


//----------------------------------------------------------------------------- 
/*
 *  DVBT2_GetSignalLevel
 *  Get signal level from 0 (lowest) to 100 (highest).
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   (UINT8) 0~100.   
 */
//-----------------------------------------------------------------------------
/*
UINT8 DVBT2_GetSignalLevel(DEMOD_CTX_T2 *psDemodCtx)
{
    INT8 s1AgcMax = 0; //(INT8)psDemodCtx->sTunerCtx.m_aucPara[4];   
    INT8 s1AgcMin = 0; //(INT8)psDemodCtx->sTunerCtx.m_aucPara[5];

    if (s1AgcMax- s1AgcMin)
    {
        INT8 s1SignalStrength = (psDemodCtx->s1IfAgc-s1AgcMin) * 100 / (s1AgcMax-s1AgcMin); // IF AGC gain
        s1SignalStrength = 100 - s1SignalStrength;

        if (psDemodCtx->fgTsLock == TRUE)
        {
            if (s1SignalStrength < 0)
                return 0;
            else if (s1SignalStrength > 100)
                return 100;
            else
                return (UINT8)s1SignalStrength;
        }
    }
    else
    {
        return 0;    
    }
  
}

*/
//----------------------------------------------------------------------------- 
/*
 *  DVBT2_GetSignalSNR
 *  Get signal SNR.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   (UINT8) SNR in dB.   
 */
//-----------------------------------------------------------------------------
UINT16 DVBT2_GetSignalSNR(DEMOD_CTX_T2 *psDemodCtx)
{
    INT16 s2_data=0;

    if (psDemodCtx->fgT2TsLock == TRUE)  // while L1 is ready, then open this condition
    {
		s2_data = (psDemodCtx->s2Snr) / 51;
        if (s2_data < 0)
            return 0;
        else
            return (UINT16) s2_data;
    }
    else
    	return 0;
    	
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_GetTotPkt
 *  Get total TS packets.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval  (UINT32)total TS packet count.   
 */
//-----------------------------------------------------------------------------
UINT32 DVBT2_GetTotPkt(DEMOD_CTX_T2 *psDemodCtx)  
{
    return (UINT32)psDemodCtx->u4BCHTotalPkt2;
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_GetErrPkt
 *  Get error TS packets.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval  (UINT32)error TS packet count.   
 */
//-----------------------------------------------------------------------------
UINT32 DVBT2_GetErrPkt(DEMOD_CTX_T2 *psDemodCtx)  
{
    return (UINT32)psDemodCtx->u4BCHErrorPkt2;    
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_GetPstBCHFER
 *  Get TS packet error rate.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   (INT32) PER*10^5.   
 */
//-----------------------------------------------------------------------------
INT32 DVBT2_GetPstBCHFER(DEMOD_CTX_T2 *psDemodCtx)  // return value = PER*10^5
{
    UINT32 LDPCTotPkt, LDPCErrPkt;
//    LDPCErrPkt = (UINT32)psDemodCtx->u4BCHErrorBit2;  
    LDPCErrPkt = (UINT32)psDemodCtx->u4BCHErrorPkt2;  
    LDPCTotPkt = (UINT32)psDemodCtx->u4BCHTotalPkt2;

    if (LDPCTotPkt <= 0)
        return -1;
    else    
        return (INT32)((LDPCErrPkt * 100000) / LDPCTotPkt);
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_GetTpsLock
 *  Get TPS lock.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   TRUE : Lock.
 *  @retval   FALSE : Unlock.   
 */
//-----------------------------------------------------------------------------
BOOL DVBT2_GetTpsLock(DEMOD_CTX_T2 *psDemodCtx)
{
    return psDemodCtx->fgTpsLock;
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_GetTsLock
 *  Get TS lock.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   TRUE : Lock.
 *  @retval   FALSE : Unlock.   
 */
//-----------------------------------------------------------------------------
BOOL DVBT2_GetTsLock(DEMOD_CTX_T2 *psDemodCtx)
{
    return psDemodCtx->fgT2TsLock;
}


//----------------------------------------------------------------------------- 
/*
 *  DVBT2_GetL1CRC
 *  Get L1 CRC.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   TRUE : Lock.
 *  @retval   FALSE : Unlock.   
 */
//-----------------------------------------------------------------------------
BOOL DVBT2_GetL1CRC(DEMOD_CTX_T2 *psDemodCtx)
{
	if (psDemodCtx->ucT2L1Status != 3)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
}
}

#if DVBT2_FROM_PHILIPS
void DVBT2_GetSldTargetLevel(DEMOD_CTX_T2 *psDemodCtx)
{
	
	UINT8	aucData[cMAX_READ_NUM + 1];
	UINT8	uTemp[6]={0,0,0,0,0,0};
	INT16     u2SLD[5]={0,0,0,0,0};
	mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_SLD_TL_1;
	if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_GET_SLD_TL_1_RSIZE, ccOFST_DVBT2_GET_SLD_TL_1_RSIZE + 1))
	{
		uTemp[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_GET_SLD_TL1H); 
		uTemp[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_GET_SLD_TL1L); 
		uTemp[2] = mccGET_QUEUE(aucData, ccOFST_DVBT2_GET_SLD_TL2H); 
		uTemp[3] = mccGET_QUEUE(aucData, ccOFST_DVBT2_GET_SLD_TL2L); 
		uTemp[4] = mccGET_QUEUE(aucData, ccOFST_DVBT2_GET_SLD_TL3H); 
		uTemp[5] = mccGET_QUEUE(aucData, ccOFST_DVBT2_GET_SLD_TL3L); 
		
		u2SLD[0] = mcSIGN_2BYTE(uTemp[1], uTemp[0], 8, 8);
		u2SLD[1] = mcSIGN_2BYTE(uTemp[3], uTemp[2], 8, 8);
		u2SLD[2] = mcSIGN_2BYTE(uTemp[5], uTemp[4], 8, 8);
	}	


		mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_SLD_TL_2 ;
	if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_GET_SLD_TL_2_RSIZE, ccOFST_DVBT2_GET_SLD_TL_2_RSIZE + 1))
	{
		uTemp[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_GET_SLD_TL4H); 
		uTemp[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_GET_SLD_TL4L); 
		uTemp[2] = mccGET_QUEUE(aucData, ccOFST_DVBT2_GET_SLD_TL5H); 
		uTemp[3] = mccGET_QUEUE(aucData, ccOFST_DVBT2_GET_SLD_TL5L); 

		u2SLD[3] = mcSIGN_2BYTE(uTemp[1], uTemp[0], 8, 8);
		u2SLD[4] = mcSIGN_2BYTE(uTemp[3], uTemp[2], 8, 8);
	}
	mcSHOW_USER_MSG(("[DVBT2_GetSldTargetLevel]psDemodCtx->fgEarlyBreak=%d,%d,%d,%d,%d,%d\n",psDemodCtx->fgEarlyBreak,u2SLD[0],u2SLD[1],u2SLD[2],u2SLD[3],u2SLD[4]));
}
#endif

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_GetRfAgcWidebandPow
 *  Get wide-band power.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   (INT8) value.   
 */
//-----------------------------------------------------------------------------
INT8 DVBT2_GetRfAgcWidebandPow(DEMOD_CTX_T2 *psDemodCtx)
{
    return psDemodCtx->m_s1AciPow;
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_GetCellId
 *  Get Cell ID.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   (UINT16) Cell ID.   
 */
//-----------------------------------------------------------------------------
UINT16 DVBT2_GetCellId(DEMOD_CTX_T2 *psDemodCtx)
{
    return psDemodCtx->u2CellId;
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_GetFreqOffset_kHz
 *  Get frequency offset in kHz.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   (INT32) freq_offset * 256.   
 */
//-----------------------------------------------------------------------------
INT32 DVBT2_GetFreqOffset_kHz(DEMOD_CTX_T2 *psDemodCtx)
{
    return psDemodCtx->s4Cfo;
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_GetBW
 *  Get channel bandwidth.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   0 : 6M.
 *  @retval   1 : 7M.
 *  @retval   2 : 8M.
 *  @retval   3 : 5M.
 */
//-----------------------------------------------------------------------------
UINT8 DVBT2_GetBW(DEMOD_CTX_T2 *psDemodCtx)
{
    return (UINT8) psDemodCtx->u1Bw;
}

//----------------------------------------------------------------------------- 
/*
 *  vSetIntfPara
 *  Set host command interface API.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  para  Pointer of parameters buffer. (para[0] : CmdId, para[1~(len-1)] : parameters).
 *  @param  len  Length of parameters
 *  @retval   void.
 */
//-----------------------------------------------------------------------------
#if 1 // LL
static void vSetIntfPara(DEMOD_CTX_T2 *psDemodCtx, UCHAR *para, UCHAR len)
{
UCHAR   aucData[cMAX_READ_NUM + 1];
UCHAR   ii;

	if (len > 8) { // for klocwork warning
        return;
    }
	   
    mccSET_CMD2(aucData) = para[0];
    if (len > 1)
    {
        for (ii = 0; ii < (len-1); ii++)
            mccSET_QUEUE2(aucData, ii) = para[ii+1];
    }

    mcHOST_CMD(psDemodCtx, aucData, 1, len);
}
#endif
//----------------------------------------------------------------------------- 
/*
 *  DVBT2_SetBW
 *  Set channel bandwidth.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  ucBW  Channel bandwidth.
 *  @retval   void.
 */
//-----------------------------------------------------------------------------
void DVBT2_SetBW(DEMOD_CTX_T2 *psDemodCtx, UINT8 ucBW)
{
    UCHAR   aucData[cMAX_READ_NUM + 1];
    UCHAR   ii;
    UCHAR   jj;

    UCHAR  uc_iir_coef_8M[] = { 0x36, 0x0C, 0x2A, 0x1B, 0x6F, 0x1F, 
                                0xF1, 0x0C, 0xB6, 0x19, 0x32, 0x14, 
                                0x7E, 0x0D, 0xA4, 0x18, 0xB8, 0x12, 
                                0xE5, 0x0D, 0x20, 0x19, 0x07, 0x11 };
    UCHAR  uc_iir_coef_7M[] = { 0xB8, 0x0C, 0xCF, 0x1A, 0xBE, 0x1D, 
                                0x6B, 0x0D, 0x8D, 0x19, 0x6E, 0x13, 
                                0xF5, 0x0D, 0x99, 0x18, 0x2C, 0x12, 
                                0x82, 0x0E, 0xD7, 0x18, 0xB0, 0x10 };
    UCHAR  uc_iir_coef_6M[] = { 0x39, 0x0D, 0x6F, 0x1A, 0xEB, 0x1B, 
                                0xDD, 0x0D, 0x62, 0x19, 0xB9, 0x12, 
                                0x61, 0x0E, 0x8D, 0x18, 0xAE, 0x11, 
                                0xD8, 0x0E, 0xCB, 0x18, 0x6B, 0x10 };

    UCHAR* uc_iir_coef = uc_iir_coef_8M;


    psDemodCtx->u1Bw = ucBW;

    mccSET_CMD2(aucData) = ccCMD_ID_FAST_SET_BW;
    mccSET_QUEUE2(aucData, ccOFST_FAST_SET_BW_BW) = ucBW;
    mccSET_QUEUE2(aucData, ccOFST_FAST_SET_BW_IIR) = psDemodCtx->fgSelectANIR;
    mcHOST_CMD(psDemodCtx, aucData, ccOFST_FAST_SET_BW_RSIZE, ccOFST_FAST_SET_BW_SIZE + 1);

    // CCI notch filter coeff.
    if(!psDemodCtx->fgSelectANIR)
    {
        switch (ucBW)
        {
            case MT5136T2_BW_8MHz:
                uc_iir_coef = uc_iir_coef_8M;
                break;
            case MT5136T2_BW_7MHz:
                uc_iir_coef = uc_iir_coef_7M;
                break;
            case MT5136T2_BW_6MHz:
                uc_iir_coef = uc_iir_coef_6M;
                break;
            default:
                uc_iir_coef = uc_iir_coef_8M;
                break;
        }
        for (jj = 0; jj < 4; jj++)
        {
            mccSET_CMD2(aucData) = ccCMD_ID_CCI_NOTCH;
            mccSET_QUEUE2(aucData, ccOFST_CCI_NOTCH_IDX) = jj;
            for (ii = 0; ii < 6; ii++)
                mccSET_QUEUE2(aucData, ccOFST_CCI_NOTCH_IDX+ii+1) = uc_iir_coef[jj*6+ii];
            mcHOST_CMD(psDemodCtx, aucData, 0, ccOFST_CCI_NOTCH_RSIZE + 1);
        }       
    }
    // add modified register settings here
    // ...
    DVBT2_DemodReset(psDemodCtx);
}

void DVBT2_SetIF(DEMOD_CTX_T2 *psDemodCtx, UINT8 ucBW)
//void DVBT2_SetIF(DEMOD_CTX_T2 *psDemodCtx)    
{

#if 0 //Pluto
UCHAR   aucData[cMAX_READ_NUM + 1];

    // Add for Sony-RE210 si tuner. Hui @ 20100720
    // AGC crrve setting
    aucData[0] = ccCMD_ID_SET_TUNER_PARA0;
    for (UCHAR ii = 0; ii < 4; ii++)
        aucData[ii+1] = psDemodCtx->sTunerCtx.m_aucPara[ii+2];  // write m_aucPara[2-5]
    vSetIntfPara(psDemodCtx, aucData, ccOFST_SET_TUNER_PARA0_SIZE+1);

    // Set IF
    switch (ucBW)
    {
        case MT5136T2_BW_8MHz:
            //psDemodCtx->sTunerCtx.m_IF_Freq = 4850;
            break;
        case MT5136T2_BW_7MHz:
            //psDemodCtx->sTunerCtx.m_IF_Freq = 4350;
            break;
        case MT5136T2_BW_6MHz:
            //psDemodCtx->sTunerCtx.m_IF_Freq = 3800;
            break;
    }
    aucData[0] = ccCMD_ID_SET_TUNER_PARA1;
    aucData[1] = psDemodCtx->sTunerCtx.m_aucPara[7];
    aucData[3] = mcLOW_BYTE(psDemodCtx->sTunerCtx.m_IF_Freq);
    aucData[4] = mcHIGH_BYTE(psDemodCtx->sTunerCtx.m_IF_Freq);
    vSetIntfPara(psDemodCtx, aucData, ccOFST_SET_TUNER_PARA1_SIZE+1);

#else
    
#endif

}




//----------------------------------------------------------------------------- 
/*
 *  DVBT_GetTsFmt
 *  Get demod TS output format.
 *  @param  psDemodCtx  Pointer of context created by DVBT_DemodCtxCreate.
 *  @retval   
 */
//-----------------------------------------------------------------------------
TS_FMT_T DVBT2_GetTsFmt(DEMOD_CTX_T2 *psDemodCtx)
{
#if 0
    UCHAR    ucData, ucClk;
    TS_FMT_T fgTsFmt;
    UCHAR ucData1;

    DVBT_GetReg(psDemodCtx, MT5391_REG_TSIF_CTRL, &ucData, 1);
    DVBT2_GetReg(psDemodCtx, 0x15ce, &ucData1, 1);
    mcSHOW_USER_CRI_MSG(("[DVBT2_GetTsFmt]0x192=%0x%02x, 0x15ce=0x%02x\n",ucData,ucData1));
    mcSHOW_DBG_MSG(("Change FSM Setting %d\n", ucData));
    //DVBT_GetReg(psDemodCtx, 0x15ce, &ucData1, 1);
    if (ucData1 & 0x01)
        fgTsFmt.SerOrPar = SP_PARALLEL;
    else
        fgTsFmt.SerOrPar = SP_SERIAL;    

    if (ucData & 0x01)
        fgTsFmt.ClockPolarity = POL_LOW_ACTIVE;//
    else
        fgTsFmt.ClockPolarity = POL_HIGH_ACTIVE;

    if (ucData & 0x02)
        fgTsFmt.fgGatedClock = BOOL_FALSE;
    else
        fgTsFmt.fgGatedClock = BOOL_TRUE;

#if defined(CC_MT5135) || defined(CC_MT5135DVBT)
    UINT32   u4Temp;
    SPI_SDARead(&u4Temp, 0x0004);
    if (u4Temp & 0x00080000)
        fgTsFmt.fgTristate = BOOL_FALSE;
    else
        fgTsFmt.fgTristate = BOOL_TRUE;
#elif defined(CC_MT5396)|| defined(CC_MT5368)//lei_code_110513;
    //add the code here future
    fgTsFmt.fgTristate = BOOL_FALSE;
#else
    //UINT32   u4Temp;
    //u4Temp = mcRISC_Reg(CKGEN_BASE, 0x408);

    //if (u4Temp & 0x00004000)
        fgTsFmt.fgTristate = BOOL_FALSE;
    //else
    //    fgTsFmt.fgTristate = BOOL_TRUE;
#endif

    DVBT_GetReg(psDemodCtx, 0x15CD, &ucClk, 1);
    if (ucData & 0x04)
    {
        /* Parallel */
        fgTsFmt.u4TsClockInKHz = 54000/(ucClk+1);// clock rate
    }
    else
    {
        /* Serial */
        fgTsFmt.u4TsClockInKHz = 54000*8/(ucClk+1);
    }

    /* In-house DTD demod,  fgExternalDemod is BOOL_FALSE! */
#if (defined(CC_MT5136) && defined (CC_MT5880)) || (defined(CC_MT5136) && defined (CC_MT5881)) // LL for 5136+5881
	fgTsFmt.fgExternalDemod = BOOL_TRUE;
#else
	fgTsFmt.fgExternalDemod = BOOL_FALSE;
#endif


    return fgTsFmt;
#endif
UCHAR    ucData, ucClk;
    TS_FMT_T fgTsFmt;

    DVBT_GetReg(psDemodCtx, MT5391_REG_TSIF_CTRL, &ucData, 1);

    mcSHOW_DBG_MSG(("Change FSM Setting %d\n", ucData));
    
    if (ucData & 0x04)
        fgTsFmt.SerOrPar = SP_PARALLEL;
    else
        fgTsFmt.SerOrPar = SP_SERIAL;    

    if (ucData & 0x01)
        fgTsFmt.ClockPolarity = POL_LOW_ACTIVE;
    else
        fgTsFmt.ClockPolarity = POL_HIGH_ACTIVE;

    if (ucData & 0x02)
        fgTsFmt.fgGatedClock = BOOL_FALSE;
    else
        fgTsFmt.fgGatedClock = BOOL_TRUE;

#if defined(CC_MT5135) || defined(CC_MT5135DVBT)
    UINT32   u4Temp;
    SPI_SDARead(&u4Temp, 0x0004);
    if (u4Temp & 0x00080000)
        fgTsFmt.fgTristate = BOOL_FALSE;
    else
        fgTsFmt.fgTristate = BOOL_TRUE;
#elif defined(CC_MT5396)|| defined(CC_MT5368)//lei_code_110513;
    //add the code here future
    fgTsFmt.fgTristate = BOOL_FALSE;
#else
    //UINT32   u4Temp;
    //u4Temp = mcRISC_Reg(CKGEN_BASE, 0x408);

    //if (u4Temp & 0x00004000)
        fgTsFmt.fgTristate = BOOL_FALSE;
    //else
    //    fgTsFmt.fgTristate = BOOL_TRUE;
#endif

    DVBT_GetReg(psDemodCtx, 0x15CD, &ucClk, 1);
    if (ucData & 0x04)
    {
        /* Parallel */
        fgTsFmt.u4TsClockInKHz = 54000/(ucClk+1);
    }
    else
    {
        /* Serial */
        fgTsFmt.u4TsClockInKHz = 54000*8/(ucClk+1);
    }

    /* In-house DTD demod,  fgExternalDemod is BOOL_FALSE! */
#if (defined(CC_MT5136) && defined (CC_MT5880)) || (defined(CC_MT5136) && defined (CC_MT5881)) || (defined(CC_MT5136) && defined (CC_MT5398))
	fgTsFmt.fgExternalDemod = BOOL_TRUE;
	mcSHOW_USER_MSG(("[DVBT2_GetTsFmt]fgExternalDemod = BOOL_TRUE\n"));
#else
	fgTsFmt.fgExternalDemod = BOOL_FALSE;
#endif


    return fgTsFmt;

}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_SetTsIf
 *  Set TS interface parameters.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  tsif_format  TS interface format.
 *  TsIfFormat:
 *  b7  Reserve
 *  b6  VALID   Active polarity selection of tsif_val
 *      0 -> tsif_val is high active
 *      1 -> tsif_val is low active
 *  b5  ERR Active polarity selection of tsif_err
 *      0 -> tsif_err is high active
 *      1 -> tsif_err is low active
 *  b4  SYNC    Active polarity selection of tsif_sync
 *      0 -> tsif_sync is high active
 *      1 -> tsif_sync is low active
 *  b3  TEI Transport stream error indicator
 *      0 -> TEI bit will not be changed by tsif
 *      1 -> TEI bit will automatic set by tsif 
 *  b2  SP  Parrallel / Serial mode selection
 *      0 -> Serial mode
 *      1 -> Parralle mode
 *  b1  CLK_GATED   Output clock gated selection of tsif_clk
 *      0 -> tsif_clk is gated
 *      1 -> tsif_clk is non-gated
 *  b0  CLK_INV Output clock edge selection of tsif_clk
 *      0 -> tsif_data is valid at positive edge clock
 *      1 -> tsif_data is valid at negative edge clock
 *  @param  tsif_ctrl  TS interface control.
 *  TsIfControl:
 *  b7  Reserve
 *  b6  FRACTIONAL_PKT  Allow MPEG fractional packet generated
 *      0 -> Output packet is always 188 bytes
 *      1 -> Output packet may not be 188 bytes when channel changed
 *  b5  SERIAL_SYNC  Control the width of TSIF_SYNC signal. Only valid in serial mode
 *      0 -> SYNC duration = first 8 bit
 *      1 -> SYNC duration = first 1 bit
 *  b4  DISABLE_HDR  Disable output header 'h47
 *      0 -> Normal
 *      1 -> Disable header 'h47 in TS
 *  b3  DATA_FLIP  Control data flip of TSIF_DATA[7:0]
 *      0 -> Normal
 *      1 -> Data flip of TSIF_DATA[7:0]
 *  b2  ERR_DROP_PKT  Drop packet if error packet occur
 *      0 -> Normal
 *      1 -> Drop packet if TSIF_ERR = 1'b1
 *  b1  ERR_SHORT  Define TSIF_ERR duration
 *      0 -> Normal, TSIF_ERR duration is 188 bytes
 *      1 -> TSIF_ERR duration is 1 byte
 *  b0  ERR_GATED  Define TSIF_ERR gated
 *      0 -> TSIF_ERR is gated by data enable
 *      1 -> TSIF_ERR is not gated between packet
 *  @param  tsif_rate  TS interface clock rate. Output data rate = (48MHz/(TSIF_RATE+1)). The value should be odd number.
 *  @retval   void.
 */
//-----------------------------------------------------------------------------
void DVBT2_SetTsIf(DEMOD_CTX_T2 *psDemodCtx, UCHAR tsif_format, UCHAR tsif_ctrl, UCHAR tsif_rate)
{
#if 1
    UCHAR ucPara[4];

    ucPara[0] = ccCMD_ID_SET_TSIF;
    ucPara[1] = tsif_format;
    ucPara[2] = tsif_ctrl;
    ucPara[3] = tsif_rate;
    
    psDemodCtx->ucTsIfFormat = ucPara[1];
    psDemodCtx->ucTsIfCtrl = ucPara[2];
    psDemodCtx->ucTsIfRate = ucPara[3];
    vSetIntfPara(psDemodCtx, ucPara, ccOFST_SET_TSIF_SIZE+1);
#else
   UCHAR ucData;
   DVBT2_GetDemodReg(psDemodCtx, 0x15CE, &ucData, 1);
   mcSHOW_USER_CRI_MSG(("[DVBT2_SetTsIf] 0x15CE = 0x%02x \n", ucData));
   mcCLR_BIT(ucData, 0);
   mcSHOW_USER_CRI_MSG(("[DVBT2_SetTsIf] 0x15CE = 0x%02x \n", ucData));
   DVBT2_SetReg(psDemodCtx, 0x15CE, &ucData, 1);
#endif
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_SetSeachParam
 *  Set system parameters of channel scan or channel up/down mode.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  search_mode  search mode select. (CH_SEARCH_BLIND : search mode, CH_SEARCH_UPDOWN : channel up/down mode).
 *  @param  mdMode  MD mode select.
 *  b・7: Enable 2k mode
 *  b・6: Enable 4k mode
 *  b・5: Enable 8k mode
 *  [b・4:b・2] GI
 *   000: 1/32
 *   001: 1/16
 *   010: 1/8
 *   011: 1/4
 *  [b・1:b・0] Spectrum inversion mode
 *   01: inverted
 *   10: non-inverted
 *  @param  tpsCr  TPS Code Rate.
 *  b・7: 0
 *  b・6: 0
 *  [b・5:b・3] TPS LP code rate info
 *   000: 1/2
 *   001: 2/3
 *   010: 3/4
 *   011: 5/6
 *   100: 7/8
 *   101~111: Reserved
 *  [b・2:b・0] TPS HP code rate info
 *   000: 1/2
 *   001: 2/3
 *   010: 3/4
 *   011: 5/6
 *   100: 7/8
 *   101~111: Reserved
 *  @param  tpsCons  TPS constellation.
 *  0: QPSK
 *  1: 16QAM
 *  2: 64QAM
 *  Other values: Reserved
 *  @param  tpsHier  TPS hierarchy.
 *  0: native, non-hierarchy
 *  1: native, alpha = 1
 *  2: native, alpha = 2
 *  3: native, alpha = 4
 *  4: indepth, non-hierarchy
 *  5: indepth, alpha = 1
 *  6: indepth, alpha = 2
 *  7: indepth, alpha = 4
 *  Other values: Reserved
 *  @param  freqBias_40kHz  Frequency bias in 40kHz. (-18~+18).
 *  @retval   void.
 */
//-----------------------------------------------------------------------------
void DVBT2_SetSearchParam(DEMOD_CTX_T2 *psDemodCtx, UCHAR search_mode, UCHAR mdMode, 
                          UCHAR tpsCr, UCHAR tpsCons, UCHAR tpsHier, INT8 freqBias_40kHz)
{
    psDemodCtx->u1ChSearchMode = search_mode;
    psDemodCtx->u1MdMode  = mdMode;
    psDemodCtx->u1TpsCr   = tpsCr;
    psDemodCtx->u1TpsCons = tpsCons;
    psDemodCtx->u1TpsHier = tpsHier;
    psDemodCtx->s1FreqBias40khz = freqBias_40kHz;
}

#if 0 // It is not suite for DVBT2
//----------------------------------------------------------------------------- 
/*
 *  DVBT2_GetDvbt2NotExist
 *  Check if really no DVB-T signal exists.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   TRUE  : It's confirmed that no DVB-T signal exists.   
 *  @retval   FALSE : DVB-T signal exists or it's not confirmed yet.
 */
//-----------------------------------------------------------------------------
BOOL DVBT2_GetDvbt2NotExist(DEMOD_CTX_T2 *psDemodCtx)
{
    if (!psDemodCtx)
        return FALSE;   
    
    if ((psDemodCtx->u1ControlStatus > 4) && (psDemodCtx->ucMdFftMode == 3))
        return TRUE;
    else
        return FALSE;
}
#endif




//----------------------------------------------------------------------------- 
/*
 *  DVBT2_GetPstLDPCBER
 *  Get post Viterbi BER.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   Post Viterbi BER * 10^5.   
 */
//-----------------------------------------------------------------------------
//INT32 DVBT2_GetPostVBer(DEMOD_CTX_T2 *psDemodCtx) // return value = PostVBer * 10^5
INT32 DVBT2_GetPstLDPCBERSQI(DEMOD_CTX_T2 *psDemodCtx) // return value = PostVBer * 10^5
{
//    INT32 RSTotPkt, RSErrPkt, RSCorrBit;
    UINT32 BCHTotPkt=0;
    UINT32 LDPCErrBit=0;
    INT32 i4Nbch_normal [] = {32400, 38880, 43200, 48600, 51840, 54000};
    INT32 i4Nbch_short [] = {7200, 9720, 10800, 11880, 12600, 13320};    
    INT32 i4temp_E9 = 0;
    UINT8 uDataPLPEFC = 0;
    UINT8 uPLPCR = 0;   
	UINT8 jj = 0;

    if (!psDemodCtx)
        return -1;

    //BCHTotPkt = (INT32)psDemodCtx->u4BCHTotalPkt2;
    //LDPCErrBit = (INT32)psDemodCtx->u4PreBCHErrorBit2;
	if((psDemodCtx->fgT2TsLock==TRUE) && (psDemodCtx->fgT2TsLockPre==FALSE))
	{		
		mcSHOW_DBG_MSG(("[DVBT2_GetPstLDPCBERSQI]****Ts=ture,Tspre=false,psDemodCtx->ii=%d\n",psDemodCtx->ii));
		for(jj=0;jj<5;jj++)
		{
		mcSHOW_DBG_MSG(("[DVBT2_GetPstLDPCBERSQI]u4TotalPkt2[%d]=%d,u4ErrorBit2[%d]=%d,ii=%d\n",jj,psDemodCtx->u4TotalPkt2[jj],jj,psDemodCtx->u4ErrorBit2[jj],psDemodCtx->ii));
		}
		psDemodCtx->u4ErrorBit2[0] = 0;
		//psDemodCtx->fgT2TsLockPre = psDemodCtx->fgT2TsLock;
	}
	psDemodCtx->fgT2TsLockPre = psDemodCtx->fgT2TsLock;
	
	if(psDemodCtx->fgT2TsLock )
{
	for(jj=0;jj<5;jj++)
	{
		BCHTotPkt=BCHTotPkt+psDemodCtx->u4TotalPkt2[jj];
		LDPCErrBit = LDPCErrBit+psDemodCtx->u4ErrorBit2[jj];
		mcSHOW_DBG_MSG(("[DVBT2_GetPstLDPCBERSQI]u4TotalPkt2[%d]=%d,u4ErrorBit2[%d]=%d,ii=%d\n",jj,psDemodCtx->u4TotalPkt2[jj],jj,psDemodCtx->u4ErrorBit2[jj],psDemodCtx->ii));
	}
}
else
{
		for(jj=0;jj<5;jj++)
	{
		psDemodCtx->u4TotalPkt2[jj]=0;
		psDemodCtx->u4ErrorBit2[jj]=0;
	}
	psDemodCtx->ii = 0;
	mcSHOW_DBG_MSG(("[DVBT2_GetPstLDPCBERSQI]Not lock,set to 0\n"));
}
	psDemodCtx->u4AveTotalPkt2 = BCHTotPkt;// for toolx qtest min cn
		psDemodCtx->u4AveErrorBit2 = LDPCErrBit;// for toolx qtest min cn

	BCHTotPkt = (INT32)(BCHTotPkt/5);
	LDPCErrBit = (INT32)(LDPCErrBit/5);
    if (BCHTotPkt <= 0)
    	return(-1);
    else
    {
        if(psDemodCtx ->u1NumPLP == 1)  // when single PLP, data PLP is PLP 0
        {
            uDataPLPEFC = psDemodCtx->u1PLP0FEC;
            uPLPCR = psDemodCtx->u1PLP0CR;
            mcSHOW_DBG_MSG(("uDataPLPEFC: psDemodCtx->u1PLP0FEC = %d\n", psDemodCtx->u1PLP0FEC));                
        }
        else if(psDemodCtx ->u1NumPLP > 1)    // when multiPLP, data PLP is PLP 1 adn common PLP is PLP 0
        {
            uDataPLPEFC = psDemodCtx->u1PLP1FEC;        	
            uPLPCR = psDemodCtx->u1PLP1CR;            
            mcSHOW_DBG_MSG(("uDataPLPEFC: psDemodCtx->u1PLP1FEC = %d\n", psDemodCtx->u1PLP1FEC));                            
        }
        else
        {
            mcSHOW_DBG_MSG(("uDataPLPEFC: psDemodCtx->u1NumPLP = 0\n"));                                    
        }
        
        if(uDataPLPEFC == 0)
        {
            i4temp_E9 = 1000000000/i4Nbch_short[uPLPCR];            	       
        }
        else if(uDataPLPEFC == 1)
        {       
            i4temp_E9 = 1000000000/i4Nbch_normal[uPLPCR];            	     	            
        }
        else
        {
           return (-1);        	        
        }
        i4temp_E9 = i4temp_E9 / BCHTotPkt;		
		mcSHOW_USER_MSG(("[DVBT2_GetPstLDPCBERSQI]5s BER for =%d*10^-9\n",LDPCErrBit * i4temp_E9));
        return(LDPCErrBit * i4temp_E9);        
    }     
    
}


//----------------------------------------------------------------------------- 
/*
 *  DVBT2_GetPstLDPCBER
 *  Get post Viterbi BER.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   Post Viterbi BER * 10^5.   
 */
//-----------------------------------------------------------------------------
//INT32 DVBT2_GetPostVBer(DEMOD_CTX_T2 *psDemodCtx) // return value = PostVBer * 10^5
INT32 DVBT2_GetPstLDPCBER(DEMOD_CTX_T2 *psDemodCtx) // return value = PostVBer * 10^5
{
//    INT32 RSTotPkt, RSErrPkt, RSCorrBit;
    INT32 BCHTotPkt, LDPCErrBit;
    INT32 i4Nbch_normal [] = {32400, 38880, 43200, 48600, 51840, 54000};
    INT32 i4Nbch_short [] = {7200, 9720, 10800, 11880, 12600, 13320};    
    INT32 i4temp_E9 = 0;
    UINT8 uDataPLPEFC = 0;
    UINT8 uPLPCR = 0;    

    if (!psDemodCtx)
        return -1;

    BCHTotPkt = (INT32)psDemodCtx->u4BCHTotalPkt2;
    LDPCErrBit = (INT32)psDemodCtx->u4PreBCHErrorBit2;

    if (BCHTotPkt <= 0)
    	return(-1);
    else
    {
        if(psDemodCtx ->u1NumPLP == 1)  // when single PLP, data PLP is PLP 0
        {
            uDataPLPEFC = psDemodCtx->u1PLP0FEC;
            uPLPCR = psDemodCtx->u1PLP0CR;
            mcSHOW_DBG_MSG4(("uDataPLPEFC: psDemodCtx->u1PLP0FEC = %d\n", psDemodCtx->u1PLP0FEC));                
        }
        else if(psDemodCtx ->u1NumPLP > 1)    // when multiPLP, data PLP is PLP 1 adn common PLP is PLP 0
        {
            uDataPLPEFC = psDemodCtx->u1PLP1FEC;        	
            uPLPCR = psDemodCtx->u1PLP1CR;            
            mcSHOW_DBG_MSG4(("uDataPLPEFC: psDemodCtx->u1PLP1FEC = %d\n", psDemodCtx->u1PLP1FEC));                            
        }
        else
        {
            mcSHOW_DBG_MSG4(("uDataPLPEFC: psDemodCtx->u1NumPLP = 0\n"));                                    
        }
        
        if(uDataPLPEFC == 0)
        {
            i4temp_E9 = 1000000000/i4Nbch_short[uPLPCR];            	       
        }
        else if(uDataPLPEFC == 1)
        {       
            i4temp_E9 = 1000000000/i4Nbch_normal[uPLPCR];            	     	            
        }
        else
        {
           return (-1);        	        
        }
        i4temp_E9 = i4temp_E9 / BCHTotPkt;
		
		mcSHOW_DBG_MSG4(("[DVBT2_GetPstLDPCBER] BER  =%d*10^-9\n",LDPCErrBit * i4temp_E9));
		mcSHOW_DBG_MSG4(("[DVBT2_GetPstLDPCBER]return BER for UI=%d*10^-6\n",LDPCErrBit * i4temp_E9/1000));
        return(LDPCErrBit * i4temp_E9);        
    }     
    
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_GetSearchParam
 *  Get and update system parameters of channel scan or channel up/down mode.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  mdMode  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  tpsCr  Pointer of code rate value.
 *  @param  tpsCons  Pointer of TPS value.
 *  @param  tpsHier  Pointer of hierarchical mode value.
 *  @param  freqBias_40kHz  Pointer of frequency offset in 40kHz .
 *  @retval   void.   
 */
//-----------------------------------------------------------------------------
void DVBT2_GetSearchParam(DEMOD_CTX_T2 *psDemodCtx, UCHAR *mdMode, UCHAR *tpsCr, 
                           UCHAR *tpsCons, UCHAR *tpsHier, INT8 *freqBias_40kHz)
{
    vDVBT2GetPara(psDemodCtx);
    vDVBT2GetInfo(psDemodCtx);

    *mdMode = 0;
    // FFT
    switch (psDemodCtx->u1Fft)
    {
    case 0:   // 2k
        mcSET_BIT(*mdMode, 7);
        break;
    case 1:   // 8k
        mcSET_BIT(*mdMode, 5);
        break;
    case 2:   // 4k
        mcSET_BIT(*mdMode, 6);
        break;
    default:
        mcSET_BIT(*mdMode, 7);
        break;
    }
    // GI
    mcSET_FIELD(*mdMode, psDemodCtx->u1GI, 0x1C, 2);
    // SI
    mcSET_FIELD(*mdMode, psDemodCtx->u1Si, 0x03, 0);

    *tpsCr = psDemodCtx->u1HpCR;
    mcSET_FIELD(*tpsCr, psDemodCtx->u1LpCR, MT5136T2_MASK_TPS_LP_CR, 3);

    *tpsCons = psDemodCtx->u1Mod;
    *tpsHier = psDemodCtx->u1Hier;
    *freqBias_40kHz = DVBT2_GetFreqOffset_kHz(psDemodCtx)/256/40;
}

/**********************************************************************/
// This function will get SNR immediately from MT5133, you don't need
// to call DVBT2_GetInfo in advance
// return value ranges from 0~255 (mapping from 0~31.5dB)
//----------------------------------------------------------------------------- 
/*
 *  DVBT2_GetSnrInfo
 *  Get SNR value, need not to call DVBT2_GetInfo in advance.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   SNR * 8(dB).   
 */
//-----------------------------------------------------------------------------
UINT8 DVBT2_GetSnrInfo(DEMOD_CTX_T2 *psDemodCtx)
{
    UINT8   aucData[cMAX_READ_NUM + 1];
    UINT8   uc_data[2];
    INT16   s2_data;

    if (!psDemodCtx)
        return 0;

    mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_GET_INFO10;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_INFO10_SIZE, 1))
    {
        // SNR, snr*512
        uc_data[0] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO10_SNR_L);
        uc_data[1] = mccGET_QUEUE(aucData, ccOFST_DVBT2_INFO10_SNR_H);
        psDemodCtx->s2Snr = (S16) mcSIGN_2BYTE(uc_data[1], uc_data[0], 8, 8);
    }

    s2_data = (psDemodCtx->s2Snr) >> 3;   // 0~255 (0~31.5dB)
    if (s2_data < 0)
        return 0;
    else
        return (UINT8) s2_data;
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_SetConnType
 *  Set connection type.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  type  Connection type. (Default is 0).
 *  @retval   void. 
 */
//-----------------------------------------------------------------------------
void DVBT2_SetConnType(DEMOD_CTX_T2 *psDemodCtx, UINT8 type)
{
    psDemodCtx->ucConnType = type;
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_SetPowerMode
 *  Set power mode.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  ucMode  Unused.
 *  @retval   0. 
 */
//-----------------------------------------------------------------------------
UCHAR DVBT2_SetPowerMode(DEMOD_CTX_T2 *psDemodCtx, UCHAR ucMode)
{
    return 0;
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_SetTunerMode
 *  Set tuner mode.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  ucMode  Unused for can tuner.
 *  @retval   void. 
 */
//-----------------------------------------------------------------------------
void DVBT2_SetTunerMode(DEMOD_CTX_T2 *psDemodCtx, UCHAR ucMode)
{
    psDemodCtx->m_TunerMode = ucMode;
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_SetI2cMasterClock
 *  Set I2C master clock.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  clk_div  clk=48MHz/clk_div, clk_div total 12bits.
 *  @retval   void. 
 */
//-----------------------------------------------------------------------------
// clk = 48MHz/clk_div, clk_div total 12bits
void DVBT2_SetI2cMasterClock(DEMOD_CTX_T2 *psDemodCtx, UINT16 clk_div)
{
    UINT16 u2_data = 0;
    UCHAR   aucData[cMAX_READ_NUM + 1];

    u2_data = clk_div & 0x0fff;
    mccSET_CMD2(aucData) = ccCMD_ID_I2C_CLK_DIV;
    mccSET_QUEUE2(aucData, ccOFST_I2C_CLK_DIV_H) = mcHIGH_BYTE(u2_data);
    mccSET_QUEUE2(aucData, ccOFST_I2C_CLK_DIV_L) = mcLOW_BYTE( u2_data);
    mcHOST_CMD(psDemodCtx, aucData, 0, ccOFST_I2C_CLK_DIV_SIZE+1);
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_NormalMode
 *  Set Demod back to normal mode.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   0 : Success.   
 *  @retval   1 : Fail.
 */
//-----------------------------------------------------------------------------
UINT8 DVBT2_NormalMode(UINT8 i2cDeviceAddr)
{
#if 0
UCHAR   ucData = 0;

    // switch to bank 1
    ucData = 1;
    if (ucI2cWrite(i2cDeviceAddr, 0xFF, &ucData, 1))
        return 1;

    // write data
    if (ucI2cRead(i2cDeviceAddr, 0xB4, &ucData, 1))
        return 1;
    mcCLR_BIT(ucData, 3);
    if (ucI2cWrite(i2cDeviceAddr, 0xB4, &ucData, 1))
        return 1;

    // write data
    if (ucI2cRead(i2cDeviceAddr, 0xB5, &ucData, 1))
        return 1;
    mcCLR_BIT(ucData, 7);
    if (ucI2cWrite(i2cDeviceAddr, 0xB5, &ucData, 1))
        return 1;

    mcDELAY_MS(1);

    // write data
    if (ucI2cRead(i2cDeviceAddr, 0xBE, &ucData, 1))
        return 1;
    mcCLR_BIT(ucData, 7);
    if (ucI2cWrite(i2cDeviceAddr, 0xBE, &ucData, 1))
        return 1;

    // write data
    if (ucI2cRead(i2cDeviceAddr, 0x48, &ucData, 1))
        return 1;
    mcSET_BIT(ucData, 6);
    if (ucI2cWrite(i2cDeviceAddr, 0x48, &ucData, 1))
        return 1;
    
    // set TS continuos clock
    // write data
    if (ucI2cRead(i2cDeviceAddr, 0x92, &ucData, 1))
        return 1;
    mcSET_BIT(ucData, 1);
    if (ucI2cWrite(i2cDeviceAddr, 0x92, &ucData, 1))
        return 1;

    // write data
    if (ucI2cRead(i2cDeviceAddr, 0x43, &ucData, 1))
        return 1;
    mcSET_BIT(ucData, 7);
    mcSET_MASK(ucData, 0x07);
    if (ucI2cWrite(i2cDeviceAddr, 0x43, &ucData, 1))
        return 1;

    // switch to bank 0
    ucData = 0;
    if (ucI2cWrite(i2cDeviceAddr, 0xFF, &ucData, 1))
        return 1;
    // write data
    if (ucI2cRead(i2cDeviceAddr, 0x00, &ucData, 1))
        return 1;
    mcSET_BIT(ucData, 0);
    if (ucI2cWrite(i2cDeviceAddr, 0x00, &ucData, 1))
        return 1;
        
    mcDELAY_MS(20);
#endif
    return 0;
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_StandbyMode
 *  Set Demod to standby mode.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   0 : Success.   
 *  @retval   1 : Fail.
 */
//-----------------------------------------------------------------------------
UINT8 DVBT2_StandbyMode(UINT8 i2cDeviceAddr)
{
#if 0
UCHAR   ucPara[4];
UCHAR   ucData = 0;

    // set IF AGC curve
    // switch to bank 1
    ucData = 1;
    if (ucI2cWrite(i2cDeviceAddr, 0xFF, &ucData, 1))
        return 1;
    // write data
    ucPara[0] = 0x20;   // SLP
    ucPara[1] = 0x00;   // INT
    ucPara[2] = 0x80;   // MAX
    ucPara[3] = 0x80;   // MIN
    if (ucI2cWrite(i2cDeviceAddr, 0xAC, ucPara, 4))
        return 1;

    // set TS gated clock
    // write data
    if (ucI2cRead(i2cDeviceAddr, 0x92, &ucData, 1))
        return 1;
    mcCLR_BIT(ucData, 1);
    if (ucI2cWrite(i2cDeviceAddr, 0x92, &ucData, 1))
        return 1;

    // switch to bank 0
    ucData = 0;
    if (ucI2cWrite(i2cDeviceAddr, 0xFF, &ucData, 1))
        return 1;
    // write data
    if (ucI2cRead(i2cDeviceAddr, 0x00, &ucData, 1))
        return 1;
    mcCLR_BIT(ucData, 0);
    if (ucI2cWrite(i2cDeviceAddr, 0x00, &ucData, 1))
        return 1;

    // switch to bank 1
    ucData = 1;
    if (ucI2cWrite(i2cDeviceAddr, 0xFF, &ucData, 1))
        return 1;   
    // write data
    if (ucI2cRead(i2cDeviceAddr, 0x43, &ucData, 1))
        return 1;
    mcCLR_BIT(ucData, 7);
    mcCLR_MASK(ucData, 0x07);
    if (ucI2cWrite(i2cDeviceAddr, 0x43, &ucData, 1))
        return 1;

    // write data
    if (ucI2cRead(i2cDeviceAddr, 0x48, &ucData, 1))
        return 1;
    mcCLR_BIT(ucData, 6);
    if (ucI2cWrite(i2cDeviceAddr, 0x48, &ucData, 1))
        return 1;

    // write data
    if (ucI2cRead(i2cDeviceAddr, 0xBE, &ucData, 1))
        return 1;
    mcSET_BIT(ucData, 7);
    if (ucI2cWrite(i2cDeviceAddr, 0xBE, &ucData, 1))
        return 1;

    // write data
    if (ucI2cRead(i2cDeviceAddr, 0xB5, &ucData, 1))
        return 1;
    mcSET_BIT(ucData, 7);
    if (ucI2cWrite(i2cDeviceAddr, 0xB5, &ucData, 1))
        return 1;

    // write data
    if (ucI2cRead(i2cDeviceAddr, 0xB4, &ucData, 1))
        return 1;
    mcSET_BIT(ucData, 3);
    if (ucI2cWrite(i2cDeviceAddr, 0xB4, &ucData, 1))
        return 1;
#endif
    return 0;
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_DumpRegisters
 *  Dump Demod registers.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  pucBuf  Pointer of data buffer.
 *  @param  bank_no  Register bank number.
 *  @param  max_i2c_len  Max I2C read byte at one time
 *  @retval   1 : I2C error.
 *  @retval   0 : I2C OK.   
 */
//-----------------------------------------------------------------------------
UINT8 DVBT2_DumpRegisters(DEMOD_CTX_T2 *psDemodCtx, UINT8 *buffer, UINT8 bank_no, UINT16 max_i2c_len)
{
    UINT16  u2CurI2cLen, u2CurAddr, u2StartAddr = 0;
    UINT16  u2CurLen = 256;

    if (!psDemodCtx)
        return 1;
    if (!buffer)
        return 1;
    if (bank_no > 2)
        return 1;

    u2CurAddr = 0;
    u2StartAddr = (UINT16) bank_no << 8;
    while (u2CurLen)
    {
        u2CurI2cLen = u2CurLen;
        if (u2CurI2cLen > max_i2c_len)
            u2CurI2cLen = max_i2c_len;

        if (DVBT2_GetReg(psDemodCtx, u2StartAddr + u2CurAddr,
                          buffer + u2CurAddr, u2CurI2cLen))
        {
            return 1;
        }
        u2CurAddr += u2CurI2cLen;
        u2CurLen  -= u2CurI2cLen;
    }

    return 0;
}

INT16 DVBT2_GetSldRfAgc(DEMOD_CTX_T2 *psDemodCtx)
{
    return psDemodCtx->s2SldPraRfagcCtrlWord;
}

INT16 DVBT2_GetSldIfAgc(DEMOD_CTX_T2 *psDemodCtx)
{
    return psDemodCtx->s2SldPraIfagcCtrlWord;
}

UINT8 DVBT2_GetSldGainIdx(DEMOD_CTX_T2 *psDemodCtx)
{
    return psDemodCtx->ucSldPraGainIdx;
}

void DVBT2_SetAbistClock(DEMOD_CTX_T2 *psDemodCtx, UINT32 Clock)
{
    UCHAR   aucData[cMAX_READ_NUM + 1];

    mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_SET_ABIST_CLOCK;
    mccSET_QUEUE2(aucData, ccOFST_DVBT2_SET_ABIST_CLOCK_H1) = (UINT8)(Clock>>24);
    mccSET_QUEUE2(aucData, ccOFST_DVBT2_SET_ABIST_CLOCK_H2) = (UINT8)(Clock>>16);
    mccSET_QUEUE2(aucData, ccOFST_DVBT2_SET_ABIST_CLOCK_H3) = (UINT8)(Clock>>8);
    mccSET_QUEUE2(aucData, ccOFST_DVBT2_SET_ABIST_CLOCK_H4) = (UINT8)(Clock);
    mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBT2_SET_ABIST_CLOCK_RSIZE, ccOFST_DVBT2_SET_ABIST_CLOCK_TSIZE+1);
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_Connect
 *  Start demod module to acquire to a channel.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  ucBW  Channel bandwidth.
 *  @retval   1 : I2C error.
 *  @retval   0 : I2C OK.   
 */
//-----------------------------------------------------------------------------
UINT8 DVBT2_Connect(DEMOD_CTX_T2 *psDemodCtx, UINT8 ucBW)
{
    UINT8 fgTRet = 0, fgT2Ret = 0;

    mcSHOW_DBG_MSG1(("DVBT2_Connect!!\n"));
    // Set 48/96 MHz from PLL
    //mcSET_SHOW_REG(CKGEN_BASE, 0x214, 0x00000102);
    // init MT8297
    
    //Reconfig the HW in the case of disconnect before     
    mcSHOW_DBG_MSG4(("//DVBT2_Connect_HWConfig(psDemodCtx);\n"));
//    DVBT2_Connect_HWConfig(psDemodCtx);    

    mcSHOW_DBG_MSG4(("DVBT2_ChipInit(psDvbt2DemodCtx);\n"));
    DVBT2_ChipInit(psDemodCtx);

    mcSHOW_DBG_MSG4((" DVBT2_FwSwitch(psDemodCtx);"));
    DVBT2_FwSwitch(psDemodCtx); /* For ReMap Use */
    
    // stop driver
    DVBT2_DriverStop(psDemodCtx);   

    fgTRet = DVBT_SetNormalMode(psDemodCtx);
    fgT2Ret = DVBT2_SetNormalMode(psDemodCtx);

    DVBT_SetArCoef(psDemodCtx, 0x3C); 
    
    // Set HpLp
    DVBT_SetHpLp(psDemodCtx, EN_HP_DECODE);
    
    // Set PLP ID
   
    // set BW
    DVBT2_SetBW(psDemodCtx, ucBW);
    // driver start
    DVBT_DriverStart(psDemodCtx, REACQ_AUTO, FAST_SYNC_AUTO);
    DVBT2_DriverStart(psDemodCtx, REACQ_AUTO, 2);    

    return fgTRet || fgT2Ret;
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_GetSync
 *  Get demod lock status.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  u1Hier  hierarchical mode.
 *  @retval   DVBT2_DEMOD_STATE_UNLOCK : Demod unlock.
 *  @retval   DVBT2_DEMOD_STATE_LOCK   : Demod locked.   
 *  @retval   DVBT2_DEMOD_STATE_UNKNON : Demod status unknown.
 */
//-----------------------------------------------------------------------------
UINT8 DVBT2_GetSync(DEMOD_CTX_T2 *psDemodCtx)
{
    UINT8 u1Dvbt2DemodState = DVBT2_DEMOD_STATE_UNKNOWN;
		UCHAR ucData1 = 0;
		UCHAR TEMP = 0;
	DVBT2_GetReg(psDemodCtx, 0x1881, &ucData1, 1);
    mcSHOW_DBG_MSG(("[DVBT2_GetSync]*****************13818 0528 2013ucComPLPID 0x1881 is 0x%0x\n", ucData1));
	DVBT2_GetReg(psDemodCtx, 0x1882, &ucData1, 1);
    mcSHOW_DBG_MSG(("[DVBT2_GetSync]ucDataPLPID 0x1882 is 0x%0x\n", ucData1));
			DVBT2_GetReg(psDemodCtx, 0x1821, &ucData1, 1);
            mcSHOW_DBG_MSG(("[DVBT2_GetSync]@@@@@@@@@@0x1821 is 0x%0x\n", ucData1));
			DVBT2_GetReg(psDemodCtx, 0x1822, &ucData1, 1);
            mcSHOW_DBG_MSG(("[DVBT2_GetSync]@@@@@@@@@@0x1822 is 0x%0x\n", ucData1));
        #if 0
	UCHAR ucData = 0;
//	UINT8   aucData[cMAX_READ_NUM + 1] = {0};

	DVBT2_GetReg(psDemodCtx, 0x1805, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x1805 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0xe40, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0xe40 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x1400, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x1400 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x1492, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x1492 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x1493, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x1493 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x008, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x008 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x1834, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x1834 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x1835, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x1835 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x1840, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x1840 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x1841, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x1841 is 0x%0x\n", ucData));

	
	mcSHOW_DBG_MSG(("/***********LDAGC**************/\n"));
	DVBT2_GetReg(psDemodCtx, 0x1354, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x1354 gain bitshift is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x1355, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x1355 gain norm is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x1346, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x1346 LDAGC on-going mode is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x1347, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x1347 LDAGC tracking mode is 0x%0x\n", ucData));
	
	DVBT2_GetReg(psDemodCtx, 0xfe6, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0xfe6  is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0xfe7, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0xfe7  is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0xfb7, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0xfb7  is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0xfb8, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0xfb8  is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0xfb9, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0xfb9  is 0x%0x\n", ucData));


	mcSHOW_DBG_MSG(("/***********P1D**************/\n"));
	DVBT2_GetReg(psDemodCtx, 0x13cb, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x13cb s1/s2 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x13cc, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x13cc fre_offset_1 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x13cd, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x13cd fre_offset_2 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x1578, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x1578 noise_pow_1 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x1579, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x1579 noise_pow_2 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x157a, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x157a noise_pow_3 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x157b, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x157b signal_pow_1 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x157c, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x157c signal_pow_2 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x157d, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x157d signal_pow_3 is 0x%0x\n", ucData));

	mcSHOW_DBG_MSG(("/***********MD**************/\n"));
	DVBT2_GetReg(psDemodCtx, 0x136d, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x136d GI/FFT mode is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x14c0, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x14c0 freq_off_1 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x14c1, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x14c1 freq_off_2 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x13b4, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x13b4 scod_1 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x13b5, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x13b5 scod_2 is 0x%0x\n", ucData));
	
	mcSHOW_DBG_MSG(("/***********P1S**************/\n"));
	DVBT2_GetReg(psDemodCtx, 0x135a, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x135a freq_frac_offset_1 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x135b, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x135a freq_frac_offset_2 is 0x%0x\n", ucData));

	mcSHOW_DBG_MSG(("/***********IFS**************/\n"));
	DVBT2_GetReg(psDemodCtx, 0x138b, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x138b IFS is 0x%0x\n", ucData));
	
	mcSHOW_DBG_MSG(("/***********FTT**************/\n"));
	DVBT2_GetReg(psDemodCtx, 0x11c4, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x11c4 freq_offset_1 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x11c5, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x11c5 freq_offset_2 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x11c6, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x11c6 freq_offset_3 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x11cd, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x11cd freq_offset_valid is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x11c9, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x11c9 time_offset_1 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x11ca, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x11ca time_offset_2 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x11cd, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x11cd time_offset_valid is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x11cb, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x11cb phase_offset_1 is 0x%0x\n", ucData));
	DVBT2_GetReg(psDemodCtx, 0x11cc, &ucData, 1);
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]0x11cc phase_offset_2 is 0x%0x\n", ucData));
        #endif
    vDVBT2GetStatus(psDemodCtx);//psDemodCtx->ucResetCnt
    
	if ((psDemodCtx->u1P1D_Fft == 5) || (psDemodCtx->u1P1D_Fft ==7))
	{
		//TEMP = FTT_LF_FREQ_LB;
		TEMP = 0x25;
		DVBT2_SetReg(psDemodCtx, 0x145c, &TEMP,1);
		
		mcSHOW_DBG_MSG(("[DVBT2_GetSync]@@@@@@@@@@@@@@If 32K ,ALWAYS change the FFT trcking,set 0x145c=0x%02x\n",TEMP));
		//mcSHOW_DBG_MSG(("[DVBT2_TunerAcq]@@@@@@@@@@@@@@1st in dvbt2 turnacq , change the FFT trcking,set 0x145c=0x%02x\n",TEMP));
		//gInitDVBT2Bit = False;
	}
	else
	{
		//gInitDVBT2Bit = FALSE;
		TEMP = 0x47;
		DVBT2_SetReg(psDemodCtx, 0x145c, &TEMP,1);
		mcSHOW_DBG_MSG(("[DVBT2_GetSync]@@@@@@@@@@@@@@If not 32k,set FTT to default  , ,set 0x145c=0x%02x\n",TEMP));
		//mcSHOW_DBG_MSG(("[DVBT2_TunerAcq]1st in dvbt2 turnacq without 32k, sp no need ftt patch"));
	}
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]psDemodCtx->ucResetCnt is %d\n", psDemodCtx->ucResetCnt)); 
    //if (DVBT2_GetTsLock(psDemodCtx) && DVBT2_GetL1CRC(psDemodCtx))
	mcSHOW_DBG_MSG(("[DVBT2_GetSync]psDemodCtx->s2Snr is %d\n", (psDemodCtx->s2Snr) / 51)); 
	mcSHOW_DBG_MSG4(("[DVBT2_GetSync] psDemodCtx->fgT2TsLock is %d\n!", psDemodCtx->fgT2TsLock));
    if (DVBT2_GetL1CRC(psDemodCtx))
    {
        u1Dvbt2DemodState = DVBT2_DEMOD_STATE_LOCK;
		psDemodCtx->u1DVBSystem = 1; // Set flag as DVBT2
		mcSHOW_DBG_MSG(("[DVBT2_GetSync]DVBT2 TS check lock %d!\n", DVBT2_DEMOD_STATE_LOCK));

    }	
    else
    {
    	if (psDemodCtx->fgEarlyBreak) 
		{	
			u1Dvbt2DemodState = DVBT2_DEMOD_STATE_UNLOCK;  // for early break
			mcSHOW_DBG_MSG(("[DVBT2_GetSync]DVBT2 early break!\n"));
	    }	
	    else
	    {
			mcSHOW_DBG_MSG(("[DVBT2_GetSync]DVBT2 unknown!\n"));	
	    }	
    }	
	
    
    return u1Dvbt2DemodState;
}

//----------------------------------------------------------------------------- 
/** DVBT2_GetSwVer
 *  Get SW version.
 *  @retval   string of SW version.
 */
//-----------------------------------------------------------------------------
#if 0
CHAR*  DVBT2_GetSwVer(void)
{
    return ((CHAR*)MT5136T2_DVBT2_SW_VER);
}
#endif

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_SetTsPLPCombine(DEMOD_CTX_T2 *psDemodCtx, 
 *                        		UINT8 ucCommonPLPID, 
 *                        		UINT8 ucDataPLPID, 
 *                        		UINT8 ucCommonDataExchange, 
 *                        		UINT8 ucTSPIDSelectionEn, 
 *                        		UINT8 ucFPGA_ASIC_Selection,
 *						UINT8 ucToolx_SOC_Selection)
 *
 *  Set what combination of PLP IDs to generate TS output
 *  @param  psDemodCtx            Pointer of context created by DVBT2_DemodCtxCreate.
 *          ucCommonPLPID         T2 common PLP ID
 *          ucDataPLPID           T2 data PLP ID
 *          ucCommonDataExchange  T2 Common PLP and data PLP path exchange
 *			ucTSPIDSelectionEn    T2 TS PID selection enable
 *          ucFPGA_ASIC_Selection FPGA / ASIC selection
 *			ucToolx_SOC_Selection Select demod only or demod with SOC
 *  @retval   void.
 */
//-----------------------------------------------------------------------------
void DVBT2_SetTsPLPCombine(DEMOD_CTX_T2 *psDemodCtx, 
								  	UINT8 ucCommonPLPID, 
								  	UINT8 ucDataPLPID, 
								  	UINT8 ucCommonDataExchange, 
								  	UINT8 ucTSPIDSelectionEn, 
								  	UINT8 ucFPGA_ASIC_Selection,
								  	UINT8 ucToolx_SOC_Selection)
{
    UCHAR   aucData[cMAX_READ_NUM + 1];

    mccSET_CMD2(aucData) = ccCMD_ID_DVBT2_TS_PLP_COMBINE;
    mccSET_QUEUE2(aucData, ccCMD_ID_DVBT2_TS_PLP_COMMON)       = ucCommonPLPID;
    mccSET_QUEUE2(aucData, ccCMD_ID_DVBT2_TS_PLP_DATA)         = ucDataPLPID;
	mccSET_QUEUE2(aucData, ccCMD_ID_DVBT2_TS_PATH_EXCHANGE)    = ucCommonDataExchange;
	mccSET_QUEUE2(aucData, ccCMD_ID_DVBT2_TS_PID_SELECT_EN)    = ucTSPIDSelectionEn;
	mccSET_QUEUE2(aucData, ccCMD_ID_DVBT2_FPGA_ASIC_SELECT)    = ucFPGA_ASIC_Selection;
	mccSET_QUEUE2(aucData, ccCMD_ID_DVBT2_DEMOD_ONLY_WITH_SOC) = ucToolx_SOC_Selection;
    mcHOST_CMD(psDemodCtx, aucData, 0, ccCMD_ID_DVBT2_TS_PLP_COMBINE_SIZE + 1);
}




//----------------------------------------------------------------------------- 
/*
 *  DVBT2_ChipInit
 *  This function initialize DVB-T related registers in the chip.
 *  @retval   1 : I2C error.
 *  @retval   0 : OK.   
 */
//-----------------------------------------------------------------------------
UINT8 DVBT2_ChipInit(DEMOD_CTX_T2 *psDemodCtx)
{
    UINT8 ucTmp;

    if (!psDemodCtx)
    {
       return 1;
    }

    // Pull-down IO CUP if NC in application
    //0x0806	=	0x60	;// PAD_PVR_TS_DATA1
    ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x806, &ucTmp, 1);
    //0x0808	=	0x60	;// PAD_PVR_TS_DATA0
    ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x808, &ucTmp, 1);
    //0x080A	=	0x60	;// PAD_PVR_TS_SYNC
    ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x80A, &ucTmp, 1);
    //0x080C	=	0x60	;// PAD_PVR_TS_VAL
    ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x80C, &ucTmp, 1);
    //0x080E	=	0x60	;// PAD_PVR_TS_CLK
    ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x80E, &ucTmp, 1);
    //0x0828	=	0x60	;// PAD_EXT_TS_CLK
    ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x828, &ucTmp, 1);
    //0x082A	=	0x60	;// PAD_EXT_TS_SYNC
    ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x82A, &ucTmp, 1);
    //0x082C	=	0x60	;// PAD_EXT_TS_VAL
    ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x82C, &ucTmp, 1);
    //0x082E	=	0x60	;// PAD_EXT_TS_DATA0
    ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x82E, &ucTmp, 1);
    //0x0830	=	0x60	;// PAD_EXT_TS_DATA1
    ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x830, &ucTmp, 1);
    //0x0832	=	0x60	;// PAD_EXT_TS_DATA2
    ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x832, &ucTmp, 1);
    //0x0834	=	0x60	;// PAD_EXT_TS_DATA3
    ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x834, &ucTmp, 1);
    //0x0836	=	0x60	;// PAD_EXT_TS_DATA4
    ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x836, &ucTmp, 1);
    //0x0838	=	0x60	;// PAD_EXT_TS_DATA5
    ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x838, &ucTmp, 1);
    //0x083A	=	0x60	;// PAD_EXT_TS_DATA6
    ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x83A, &ucTmp, 1);
    //0x083C	=	0x60	;// PAD_EXT_TS_DATA7
    ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x83C, &ucTmp, 1);

    // if (0x08FB, bit[5:4]=2'b01), QFN48
    DVBT2_GetReg(psDemodCtx, 0x8FB,  &ucTmp, 1);
    ucTmp = (ucTmp & 0x30) >> 4;
    if(ucTmp == 0x01)
    {  
        mcSHOW_DBG_MSG(("QFN48 Config \n"));     
        // Pull-down NC IO CUP  
        //0x0800	=	0x60	// PAD_SPI_CLE
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x800, &ucTmp, 1);        
        //0x0802	=	0x60	// PAD_SPI_DATA
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x802, &ucTmp, 1);                
        //0x0804	=	0x60	// PAD_SPI_CLK
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x804, &ucTmp, 1);                        
        //0x0810	=	0x60	// PAD_CI_TS_DATA0
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x810, &ucTmp, 1);                        
        //0x0812	=	0x60	// PAD_CI_TS_SYNC
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x812, &ucTmp, 1);                        
        //0x0814	=	0x60	// PAD_CI_TS_VAL
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x814, &ucTmp, 1);                        
        //0x0816	=	0x60	// PAD_CI_TS_CLK
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x816, &ucTmp, 1);                        
        //0x0818	=	0x60	// PAD_DEMOD_TS_DATA0
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x818, &ucTmp, 1);                        
        //0x081A	=	0x60	// PAD_DEMOD_TS_SYNC
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x81A, &ucTmp, 1);                        
        //0x081C	=	0x60	// PAD_DEMOD_TS_VAL
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x81C, &ucTmp, 1);                        
        //0x081E	=	0x60	// PAD_DEMOD_TS_CLK
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x81E, &ucTmp, 1);                        
        //0x0820	=	0x60	// PAD_TUNER_DATA
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x820, &ucTmp, 1);                        
        //0x0822	=	0x60	// PAD_TUNER_CLK
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x822, &ucTmp, 1);                        
        //0x0824	=	0x60	// PAD_IF_AGC
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x824, &ucTmp, 1);                        
        //0x0826	=	0x60	// PAD_RF_AGC
        //ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x826, &ucTmp, 1);
		//ucTmp = 0x4C;   DVBT2_SetReg(psDemodCtx, 0x826, &ucTmp, 1); //  //Mingsheng 1200815, for FEF issue
        //0x0844	=	0x60	// PAD_CD1_
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x844, &ucTmp, 1);                        
        //0x0846	=	0x60	// PAD_D3
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x846, &ucTmp, 1);                        
        //0x0848	=	0x60	// PAD_D11
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x848, &ucTmp, 1);                        
        //0x084A	=	0x60	// PAD_D4
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x84A, &ucTmp, 1);                        
        //0x0854	=	0x60	// PAD_D14
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x854, &ucTmp, 1);                        
        //0x0856	=	0x60	// PAD_D7
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x856, &ucTmp, 1);                        
        //0x0858	=	0x60	// PAD_D15
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x858, &ucTmp, 1);                        
        //0x085A	=	0x60	// PAD_CE1_
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x85A, &ucTmp, 1);                        
        //0x085C	=	0x60	// PAD_A10
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x85C, &ucTmp, 1);                        
        //0x085E	=	0x60	// PAD_VS1_
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x85E, &ucTmp, 1);
        //0x0860	=	0x60	// PAD_OE_
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x860, &ucTmp, 1);        
        //0x0862	=	0x60	// PAD_IORD_
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x862, &ucTmp, 1);        
        //0x0864	=	0x60	// PAD_A11
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x864, &ucTmp, 1);
        //0x0866	=	0x60	// PAD_IOWR_
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x866, &ucTmp, 1);        
        //0x0868	=	0x60	// PAD_A9
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x868, &ucTmp, 1);        
        //0x086A	=	0x60	// PAD_A17
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x86A, &ucTmp, 1);        
        //0x086C	=	0x60	// PAD_A8
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x86C, &ucTmp, 1);        
        //0x086E	=	0x60	// PAD_A18
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x86E, &ucTmp, 1);        
        //0x087C	=	0x60	// PAD_READY
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x87C, &ucTmp, 1);        
        //0x0880	=	0x60	// PAD_A16
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x880, &ucTmp, 1);        
        //0x0882	=	0x60	// PAD_A23
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x882, &ucTmp, 1);        
        //0x0884	=	0x60	// PAD_A15
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x884, &ucTmp, 1);        
        //0x0886	=	0x60	// PAD_A24
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x886, &ucTmp, 1);        
        //0x0888	=	0x60	// PAD_A12
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x888, &ucTmp, 1);        
        //0x088A	=	0x60	// PAD_A25
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x88A, &ucTmp, 1);        
        //0x088C	=	0x60	// PAD_A7
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x88C, &ucTmp, 1);        
        //0x088E	=	0x60	// PAD_VS2_
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x88E, &ucTmp, 1);        
        //0x0890	=	0x60	// PAD_A6
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x890, &ucTmp, 1);        
        //0x0892	=	0x60	// PAD_PCMCIA_RESET
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x892, &ucTmp, 1);        
        //0x0894	=	0x60	// PAD_A5
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x894, &ucTmp, 1);        
        //0x0896	=	0x60	// PAD_WAIT_
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x896, &ucTmp, 1);        
        //0x0898	=	0x60	// PAD_A4
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x898, &ucTmp, 1);        
        //0x089C	=	0x60	// PAD_REG_
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x89C, &ucTmp, 1);        
        //0x089E	=	0x60	// PAD_A2
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x89E, &ucTmp, 1);        
        //0x08A0	=	0x60	// PAD_BVD2
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x8A0, &ucTmp, 1);
        //0x08A2	=	0x60	// PAD_A1
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x8A2, &ucTmp, 1);        
        //0x08A4	=	0x60	// PAD_BVD1
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x8A4, &ucTmp, 1);        
        //0x08A6	=	0x60	// PAD_A0
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x8A6, &ucTmp, 1);        
        //0x08B4	=	0x60	// PAD_CD2_
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x8B4, &ucTmp, 1);        
        //0x08B6	=	0x60	// PAD_CI_INT
        ucTmp = 0x60;   DVBT2_SetReg(psDemodCtx, 0x8B6, &ucTmp, 1);        
        
    }
    ucTmp = 0x01;   DVBT2_SetReg(psDemodCtx, 0x8EE, &ucTmp, 1);   //Mingsheng 1200815, for FEF issue
    ucTmp = 0x4C;   DVBT2_SetReg(psDemodCtx, 0x826, &ucTmp, 1); //  //Mingsheng 1200815, for FEF issue
    
    // Peripheral soft reset
    //0x0E3B	=	0x00	;// Reset peripheral
    ucTmp = 0x20;   DVBT2_SetReg(psDemodCtx, 0xE3B, &ucTmp, 1);    
//    ucTmp = 0x00;   DVBT2_SetReg(psDemodCtx, 0xE3B, &ucTmp, 1);        
    //0x0E3C	=	0x00	;// Reset peripheral
    ucTmp = 0x00;   DVBT2_SetReg(psDemodCtx, 0xE3C, &ucTmp, 1);    
    //0x0E3B	=	0xFF	;// Release peripheral
    ucTmp = 0xFF;   DVBT2_SetReg(psDemodCtx, 0xE3B, &ucTmp, 1);    
    //0x0E3C	=	0xFF	;// Release peripheral 
    ucTmp = 0xFF;   DVBT2_SetReg(psDemodCtx, 0xE3C, &ucTmp, 1);
 
    // IFPLL settings				
    //0x0910	=	0x9F	;// RG_ULJPLL_PWD=1; (bit[7]=1'b1)
//    ucTmp = 0x9F;   DVBT2_SetReg(psDemodCtx, 0x910, &ucTmp, 1);
    //0x0918	=	0xC0	;// RG_CLKEN=1; RG_DCKEN=1; (bit[7:6]=2'b11)
    ucTmp = 0xC0;   DVBT2_SetReg(psDemodCtx, 0x918, &ucTmp, 1);
    //0x0917	=	0x20	;// RG_ULJPLL_MONEN=1; (bit[5]=1b'1)
    ucTmp = 0x20;   DVBT2_SetReg(psDemodCtx, 0x917, &ucTmp, 1);
    //0x0914	=	0x2F	;// RG_ULJPLL_AUTOK_VCO=1; (bit[0]=1'b1)
    ucTmp = 0x2F;   DVBT2_SetReg(psDemodCtx, 0x914, &ucTmp, 1);    
    //0x0915	=	0x97	;// RG_ULJPLL_AUTOK_LOAD=1; RG_ULJPLL_LOAD_RSTB=1; (bit[7]=1'b1; bit[0]=1'b1)
    ucTmp = 0x97;   DVBT2_SetReg(psDemodCtx, 0x915, &ucTmp, 1);
    //0x0910	=	0x1F	;// RG_ULJPLL_PWD=0; (bit[7]=1'b0)
    ucTmp = 0x1F;   DVBT2_SetReg(psDemodCtx, 0x910, &ucTmp, 1);

    // delay 100us ==> By applying stepIII script	
    mcDELAY_MS(2);
  
    // post ANA regsiter after 100us delay				
    //0x0914	=	0x2E	;// RG_ULJPLL_AUTOK_VCO=0; (bit[0]=1'b0)
    ucTmp = 0x2E;   DVBT2_SetReg(psDemodCtx, 0x914, &ucTmp, 1);    
    //0x0917	=	0x20	;// RG_ULJPLL_DIV3_RSTB=0; (bit[3]=1b'0)
    ucTmp = 0x20;   DVBT2_SetReg(psDemodCtx, 0x917, &ucTmp, 1);     
    // PGA power up				
    //0x0909	=	0x00	;// RG_PGAVOCM_PWD=0; RG_PGA_PWD=0; (bit[2:1]=2'b00)
    ucTmp = 0x00;   DVBT2_SetReg(psDemodCtx, 0x909, &ucTmp, 1);     
    // ADC power up				
    //0x0903	=	0x82	;// RG_ADC_PWD_DEMOD=0; (bit[0]=1'b0)
    ucTmp = 0x82;   DVBT2_SetReg(psDemodCtx, 0x903, &ucTmp, 1);     
    // ADC deadlock prevention				
    //0x0902	=	0xA0	;// RG_CKRATE_DEMOD[1:0]=2'b00; (bit[3:2]=2'b00)
    ucTmp = 0xA0;   DVBT2_SetReg(psDemodCtx, 0x902, &ucTmp, 1);     
    //0x0902	=	0xA8	;// RG_CKRATE_DEMOD[1:0]=2'b10; (bit[3:2]=2'b10)
    ucTmp = 0xA8;   DVBT2_SetReg(psDemodCtx, 0x902, &ucTmp, 1);     
  
    // PLLGP settings				
    //0x0921	=	0x48	;// RG_HADDS2_PWD=0; (bit[7]=1'b0)
    ucTmp = 0x48;   DVBT2_SetReg(psDemodCtx, 0x921, &ucTmp, 1);     
    //0x0924	=	0x27	;// RG_DDS1_DDS_PWDB=1; (bit[5]=1'b1)
    ucTmp = 0x27;   DVBT2_SetReg(psDemodCtx, 0x924, &ucTmp, 1);     
    //0x0924	=	0x67	;// RG_DDS1_DDS_RSTB=1; (bit[6]=1'b1)
    ucTmp = 0x67;   DVBT2_SetReg(psDemodCtx, 0x924, &ucTmp, 1);     
    //0x0927	=	0x27	;// RG_DDS2_DDS_PWDB=1; (bit[5]=1'b1)
    ucTmp = 0x27;   DVBT2_SetReg(psDemodCtx, 0x927, &ucTmp, 1);     
    //0x0927	=	0x67	;// RG_DDS2_DDS_RSTB=1; (bit[6]=1'b1)
    ucTmp = 0x67;   DVBT2_SetReg(psDemodCtx, 0x927, &ucTmp, 1);     

    // CKGEN				
    //0x0E50	=	0x00	;// Datapath PLL source: 0x00 for ADCPLL(432MHz); 0x10 for SYSPLL(648MHz)
    ucTmp = 0x00;   DVBT2_SetReg(psDemodCtx, 0xE50, &ucTmp, 1);     
    //0x0E51	=	0x01	;// 54M source: 0x01 for ADCPLL; 0x03 for SYSPLL
    ucTmp = 0x01;   DVBT2_SetReg(psDemodCtx, 0xE51, &ucTmp, 1);     
    //0x0E52	=	0x01	;// Apply 54MHz deglitch change
    ucTmp = 0x01;   DVBT2_SetReg(psDemodCtx, 0xE52, &ucTmp, 1);     
    //0x0E55	=	0x03	;// 86.4M level 1 source: 0x03 for ADCPLL; 0x00 for SYSPLL
    ucTmp = 0x03;   DVBT2_SetReg(psDemodCtx, 0xE55, &ucTmp, 1);     
    //0x0E56	=	0x01	;// Apply 86.4MHz level 1 deglitch change
    ucTmp = 0x01;   DVBT2_SetReg(psDemodCtx, 0xE56, &ucTmp, 1);     
    //0x0E57	=	0x03	;// 36M source: 0x03 for ADCPLL; 0x05 for SYSPLL
    ucTmp = 0x03;   DVBT2_SetReg(psDemodCtx, 0xE57, &ucTmp, 1);     
    //0x0E58	=	0x01	;// Apply 36MHz deglitch change
    ucTmp = 0x01;   DVBT2_SetReg(psDemodCtx, 0xE58, &ucTmp, 1);     
    //0x0E6A	=	0x01	;// ADCLK Div4 (54M) source
    ucTmp = 0x01;   DVBT2_SetReg(psDemodCtx, 0xE6A, &ucTmp, 1);     
    //0x0E6B	=	0x01	;// Apply ADCLK Div4 deglitch change
    ucTmp = 0x01;   DVBT2_SetReg(psDemodCtx, 0xE6B, &ucTmp, 1);     
    //0x0E6C	=	0x01	;// ADCLK source
    ucTmp = 0x01;   DVBT2_SetReg(psDemodCtx, 0xE6C, &ucTmp, 1);     
    //0x0E6D	=	0x01	;// Apply ADCLK deglitch change
    ucTmp = 0x01;   DVBT2_SetReg(psDemodCtx, 0xE6D, &ucTmp, 1);     
    //0x0E70	=	0x01	;// 162M level 1 source
    ucTmp = 0x01;   DVBT2_SetReg(psDemodCtx, 0xE70, &ucTmp, 1);     
    //0x0E71	=	0x01	;// Apply 162M level 1 deglitch change
    ucTmp = 0x01;   DVBT2_SetReg(psDemodCtx, 0xE71, &ucTmp, 1);     

    // SLD ANAIF				
    //0x0F80	=	0x03	;// AD_CKPHSEL=1 (bit[1]=1'b1); AD_FMT(binary-offset)=1 (bit[0]=1'b1)
    ucTmp = 0x03;   DVBT2_SetReg(psDemodCtx, 0xF80, &ucTmp, 1);     
    //0x0E01	=	0x00	;// Reset ANAIF
    ucTmp = 0x00;   DVBT2_SetReg(psDemodCtx, 0xE01, &ucTmp, 1);     
    //0x0E01	=	0x80	;// Release ANAIF
    ucTmp = 0x80;   DVBT2_SetReg(psDemodCtx, 0xE01, &ucTmp, 1);     
 
    //SLD IF/RF AGC setting
    //RF_AGC_PAD_EN: 0xF62[3] = 0 (hiZ)
    //IF_AGC_PAD_EN: 0xF62[2] = 1 (Enable)
    DVBT2_GetReg(psDemodCtx, 0xF62, &ucTmp, 1);
    mcCLR_BIT(ucTmp, 3);
    mcSET_BIT(ucTmp, 2);
    DVBT2_SetReg(psDemodCtx, 0xF62, &ucTmp, 1);    

    // System initial (CH switch) ==> Apply stepIV
/*
	//lei120427 added for larger the clk and data output driver 
	//TS Clock
    ucTmp = 0x44;   DVBC_SetReg(psDemodCtx, 0x81E, &ucTmp, 1);
    //TS Data 0
    ucTmp = 0x44;   DVBC_SetReg(psDemodCtx, 0x818, &ucTmp, 1);
    // System initial (CH switch) ==> Apply stepIV
*/    
	
	//============== DVBT or T/T2 auto detection mode START ======================
	//0x0E55	=	0x23	;// 86.4M level 2 source: System initial @54M domain (bit[5]=1'b1)							
	ucTmp = 0x23;	MT5136_DVBT2_SetReg(psDemodCtx, 0xE55, &ucTmp, 1);	  
	//0x0E56	=	0x04	;// Apply 86.4MHz level 2 deglitch change				
	ucTmp = 0x04;	MT5136_DVBT2_SetReg(psDemodCtx, 0xE56, &ucTmp, 1);	  
	//0x0E70	=	0x09	;// 162M level 2 source: System initial @54M domain (bit[4:3]=2'b01)							
	ucTmp = 0x09;	MT5136_DVBT2_SetReg(psDemodCtx, 0xE70, &ucTmp, 1);	  
	//0x0E71	=	0x04	;// Apply 162M level 2 deglitch change
	ucTmp = 0x04;	MT5136_DVBT2_SetReg(psDemodCtx, 0xE71, &ucTmp, 1);	  
	//0x0E72	=	0x00	;// MPLP source: System initial @54M domain (bit[0]=1'b0)
	ucTmp = 0x00;	MT5136_DVBT2_SetReg(psDemodCtx, 0xE72, &ucTmp, 1);	  
	//0x0E73	=	0x01	;// Apply MPLP deglitch change
	ucTmp = 0x01;	MT5136_DVBT2_SetReg(psDemodCtx, 0xE73, &ucTmp, 1);
	//============== DVBT or T/T2 auto detection mode END ======================
    return 0;
}


//----------------------------------------------------------------------------- 
/*
 *  DVBT2_T_T2_Init_Config(DEMOD_CTX_T2 *psDemodCtx)
 *  This function initialize setting after up start.
 *  @retval   1 : I2C error.
 *  @retval   0 : OK.   
 */
//-----------------------------------------------------------------------------
UINT8 DVBT2_T_T2_Init_Config(DEMOD_CTX_T2 *psDemodCtx)
{
    UCHAR ucTmp  = 0;


    if (!psDemodCtx)
    {
       return 1;
    }

	mcSHOW_DBG_MSG4(("psDemodCtx->u1ChBw = %d\n", psDemodCtx->u1ChBw));

	
    //   use the T/T2 mode at the beginning. 
#if 0 // Need double check with Chihau      
  //0x0E51	=	0x01	// DVBT 54M power down (bit[6]=1'b1); DVBT2 54M/162M power down (bit[4]=1'b1]): 0x51 for ADCPLL; 0x53 for SYSPLL
  ucTmp = 0x01;   DVBC_SetReg(psDemodCtx, 0xE51, &ucTmp, 1);    
#endif

    //0x08D5	=	0x00	;// Turn off pad_tstmd_en[11:10] (bit[3:2]= 2'b00)
    ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x8D5, &ucTmp, 1);      
    //0x08D6	=	0x00	;// Turn off pad_tstmd_en[16] (bit[0]=1'b0)
    ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x8D6, &ucTmp, 1);      
    //0x08DA	=	0x00	;// Turn off pad_tstmd_en[55:50] (bit[7:2]=6'h00)
    ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x8DA, &ucTmp, 1);      
    //0x08D2	=	0x00	;// Turn off ci_gpio[7:0]
    ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x8D2, &ucTmp, 1);      
    //0x08D3	=	0x00	;// Turn off ci_gpio[15:8] 
    ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x8D3, &ucTmp, 1);      
    //0x08DD	=	0x2C	;// 0x34: Parallel MUX TS Path I; Serial CI TS; 
    ucTmp = 0x2C;   MT5136_DVBT2_SetReg(psDemodCtx, 0x8DD, &ucTmp, 1);

    //[MD register]
    //MD timing offset Compensation (using one constant value to compensate the symbol timing offset)
    //0x136E = 0xFB; //the value of register to compensate symbol timing offset
    ucTmp = 0xFB;   MT5136_DVBT2_SetReg(psDemodCtx, 0x136E, &ucTmp, 1);    
    //0x136F = 0x40; //bit4: enable symbol timing offset in the register '0x136E/136F'
    ucTmp = 0x40;   MT5136_DVBT2_SetReg(psDemodCtx, 0x136F, &ucTmp, 1);    

    //[ANIR]
    //0x1332 = 0xFF;//bit[7;5] the step size of weight update, default value [7:5] = 0x06
    ucTmp = 0xFF;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1332, &ucTmp, 1);

    //[CSI fine tune]
    //FDI compensation
    //;0x1701, bit4: hw L1 pre bypass bit3: sw L1 pre bypass, bit2: iq_pn_comp_en, bit1: csi_pn_comp_en, bit0: csi_sym_en
    //0x1701 = 0x77; // average mode
    ucTmp = 0x77;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1701, &ucTmp, 1);      
    //;0x1701 = 0x17; // burst mode

    //;0x1719 = 0x14;  //-2dB, disable pn_csi_comp when the compensated value is above the value of this register U(2, -5)
    //;0x171A = 0x33; //2 dB, disable pn_csi_comp when the compensated value is below the value of this register U(2, -5)
    //;0x171D = 0x00; //U(-5, -12), the threshold to compensate phase noise
    //0x171B = 0x14;  //-3dB, disable csi_wei_comp when the compensated value is above the value of this register U(2, -5)
    ucTmp = 0x14;   MT5136_DVBT2_SetReg(psDemodCtx, 0x171B, &ucTmp, 1);      
    //0x171C = 0x33; //3 dB, disable csi_wei_comp when the compensated value is below the value of this register U(2, -5)
    ucTmp = 0x33;   MT5136_DVBT2_SetReg(psDemodCtx, 0x171C, &ucTmp, 1);      

    //must enable when testing CCI
    //;0x1509 = 0xFB; //enable CSI_AR auto-adjustment, bit[5] depend on CCI,  bit[0] depends on symbol noise

    //;fix CSI: bit7: csi_fix_en, bit5-0: fix_csi_val 
    //;0x1700 = 0x2B ; 21dB

    //;change the AR of CSI
    //;0x1508 = 0x0C; using default value 127/128

    //;0x14EA = 0xC4; using the edge filter debias coefficient 
    //;[noise edge protection]
    //;0x1497 = 0x00; //[15:8]  
    //;0x14A4 = 0x01; //[7: 0] the lower bound of noise power near the left edge
    //0x19F0 = 0x00;  //[15:8]
    ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19F0, &ucTmp, 1);      
    //0x19F1 = 0x01;  //[7: 0]
    ucTmp = 0x01;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19F1, &ucTmp, 1);      

    //;0x1509 = 0x5a; CSI using noise profile
    //;0x1510 = 0xff; // raise bad pilot threshold caused by mid-platform tuner non-flat response

	if (psDemodCtx->u1ChBw == MT5136T2_BW_5MHz)  // 5MHz bandwidth
	{
    	//0x14B3 = 0x21; //[7], P2 on/off, [6], sp on/off, [5] cp on/off (FW use correct SNR, ECO2 may modify)
    	ucTmp = 0x21;   MT5136_DVBT2_SetReg(psDemodCtx, 0x14B3, &ucTmp, 1);      
				
		// DJB Elementary Period Factor
		// 0x15B9 = 0x07
		ucTmp = 0x07;	MT5136_DVBT2_SetReg(psDemodCtx, 0x15B9, &ucTmp, 1);      
		// 0x15BA = 0x16
		ucTmp = 0x16;	MT5136_DVBT2_SetReg(psDemodCtx, 0x15BA, &ucTmp, 1);      
	
	}
	else if (psDemodCtx->u1ChBw == MT5136T2_BW_6MHz)  // 6MHz bandwidth
	{
    	//0x14B3 = 0xC1; //[7], P2 on/off, [6], sp on/off, [5] cp on/off (FW use correct SNR, ECO2 may modify)
    	ucTmp = 0xC1;   MT5136_DVBT2_SetReg(psDemodCtx, 0x14B3, &ucTmp, 1);      
        
		
		// DJB Elementary Period Factor
		// 0x15B9 = 0x05
		ucTmp = 0x05;	MT5136_DVBT2_SetReg(psDemodCtx, 0x15B9, &ucTmp, 1);      
		// 0x15BA = 0xEB
		ucTmp = 0xEB;	MT5136_DVBT2_SetReg(psDemodCtx, 0x15BA, &ucTmp, 1);      
	}
	else if (psDemodCtx->u1ChBw == MT5136T2_BW_7MHz)  // 7MHz bandwidth
	{
    	//0x14B3 = 0x21; //[7], P2 on/off, [6], sp on/off, [5] cp on/off (FW use correct SNR, ECO2 may modify)
    	ucTmp = 0x21;   MT5136_DVBT2_SetReg(psDemodCtx, 0x14B3, &ucTmp, 1);      
        		
		// DJB Elementary Period Factor
		// 0x15B9 = 0x05
		ucTmp = 0x05;	MT5136_DVBT2_SetReg(psDemodCtx, 0x15B9, &ucTmp, 1);      
		// 0x15BA = 0x10
		ucTmp = 0x10;	MT5136_DVBT2_SetReg(psDemodCtx, 0x15BA, &ucTmp, 1);      
	
	}
	else if (psDemodCtx->u1ChBw == MT5136T2_BW_8MHz)  // 8MHz bandwidth
	{
    	//0x14B3 = 0xC0; //[7], P2 on/off, [6], sp on/off, [5] cp on/off (FW use correct SNR, ECO2 may modify)
    	ucTmp = 0xC0;   MT5136_DVBT2_SetReg(psDemodCtx, 0x14B3, &ucTmp, 1);      
        
		
		// DJB Elementary Period Factor
		// 0x15B9 = 0x04
		ucTmp = 0x04;	MT5136_DVBT2_SetReg(psDemodCtx, 0x15B9, &ucTmp, 1);      
		// 0x15BA = 0x6E
		ucTmp = 0x6E;	MT5136_DVBT2_SetReg(psDemodCtx, 0x15BA, &ucTmp, 1);      
	}
	else if (psDemodCtx->u1ChBw == MT5136T2_BW_1p7MHz)  // 1.7MHz bandwidth
	{
    	//0x14B3 = 0xC0; //[7], P2 on/off, [6], sp on/off, [5] cp on/off (FW use correct SNR, ECO2 may modify)
    	ucTmp = 0xC0;   MT5136_DVBT2_SetReg(psDemodCtx, 0x14B3, &ucTmp, 1);      
        
		
		// DJB Elementary Period Factor
		// 0x15B9 = 0x04
		ucTmp = 0x15;	MT5136_DVBT2_SetReg(psDemodCtx, 0x15B9, &ucTmp, 1);      
		// 0x15BA = 0x6E
		ucTmp = 0xF3;	MT5136_DVBT2_SetReg(psDemodCtx, 0x15BA, &ucTmp, 1);      
	}
	
    //0x1507 = 0x54; //ignore the edge pilot of 1.5 times of Dx*11
    ucTmp = 0x54;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1507, &ucTmp, 1);

    //;[CE register]
    //;0x14e0 = 0xC0; //bit[7:6], 0:zero order, 1: linear, 2: fix coefficient, 3: LS
    //0x14DD = 0x03; //bit 0, CE AR EN, default is 0x03 (bit 1: TX2)
    //ucTmp = 0x03;   MT5136_DVBT2_SetReg(psDemodCtx, 0x14DD, &ucTmp, 1);      
    ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x14DD, &ucTmp, 1);      
    //;0x14DC = 0x54; //defaut(0x56), AR_Coef[1:0], 0:3/4, 1:7/8, 2:15/16, 3:31/32, 
    //0x14A8 = 0x2D; //using FDI = 0.75 Nyquist rate for all channel length, both for SISO and MISO
    ucTmp = 0x2D;   MT5136_DVBT2_SetReg(psDemodCtx, 0x14A8, &ucTmp, 1);      
    //0x1501 = 0x2E; //debias, depend on 0x14A8
    ucTmp = 0x2D;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1501, &ucTmp, 1);      
    //0x14A1 = 0x80; //TDCE last 3 symbols output interval 32samples
    ucTmp = 0x80;   MT5136_DVBT2_SetReg(psDemodCtx, 0x14A1, &ucTmp, 1);     
    //0x14A0 = 0x20; //TDCE last 3 symbols output interval 32samples
    ucTmp = 0x20;   MT5136_DVBT2_SetReg(psDemodCtx, 0x14A0, &ucTmp, 1);  

    //;[FTT register fine tune]
    //;Loop filter of Frequency offset, default 0x47, fix burst error
    //;may be caused by RTL bug.
    //0x145C = 0x47; // Phil: lower down FTT tracking speed (to solve NDGC2_07, may fine tune in the future)
    ucTmp = 0x47;   MT5136_DVBT2_SetReg(psDemodCtx, 0x145C, &ucTmp, 1);  

    //;[system controller register]
    //0x1421 = 0xA0; //bit4: use new P2 symbol in acquisition stage, bit5: use min chir offset
    ucTmp = 0xA0;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1421, &ucTmp, 1);  
    //;0x14B7 = 0x08; // force OFDM symbol timing from P1S

    //;[TDI/FDI rate control]
    //; FDI output rate
    //0x171e = 0x44
    ucTmp = 0x44;   MT5136_DVBT2_SetReg(psDemodCtx, 0x171E, &ucTmp, 1);     
    //0x171f = 0x51
    ucTmp = 0x50;   MT5136_DVBT2_SetReg(psDemodCtx, 0x171F, &ucTmp, 1);     
    //; FDI firmware waiting count, no need to set for PLP extraction time is longer than watch dog timeout. 
    //0x1704 = 0x0a;
    ucTmp = 0x78;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1704, &ucTmp, 1); 

    //; disable watch dog. PLP extraction time is longer than watch dog timeout. Hauze, 20120510
    //0x1702[0] = 1'b0; 
    MT5136_DVBT2_GetReg(psDemodCtx, 0x1702, &ucTmp, 1); 
    mcCLR_BIT(ucTmp, 0);
    MT5136_DVBT2_SetReg(psDemodCtx, 0x1702, &ucTmp, 1); 
    

    //; Disable TDI out manual rate mode
    //0x178e = 0x70
    ucTmp = 0x73;   MT5136_DVBT2_SetReg(psDemodCtx, 0x178E, &ucTmp, 1);     

    //; TDI output rate
    //0x178f = 0xcc	;QPSK
    //ucTmp = 0x40;   MT5136_DVBT2_SetReg(psDemodCtx, 0x178F, &ucTmp, 1); 
	ucTmp = 0x52;   MT5136_DVBT2_SetReg(psDemodCtx, 0x178F, &ucTmp, 1);  //Mingsheng 120815, 0x40->0x52 for DTG112/120 Picture break;
    //0x1790 = 0x1f	;QPSK
    ucTmp = 0x1F;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1790, &ucTmp, 1);     
    //0x1791 = 0xe6	;16QAM
    ucTmp = 0xA0;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1791, &ucTmp, 1);     
    //0x1792 = 0x0f	;16QAM
    ucTmp = 0x0F;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1792, &ucTmp, 1);     
    //0x1793 = 0x97	;64QAM
    ucTmp = 0x69;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1793, &ucTmp, 1);     
    //0x1794 = 0x0a	;64QAM
    ucTmp = 0x0A;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1794, &ucTmp, 1);     
    //0x1795 = 0xf3	;256QAM
    ucTmp = 0xd0;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1795, &ucTmp, 1);     
    //0x1796 = 0x07	;256QAM
    ucTmp = 0x07;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1796, &ucTmp, 1);     

    //0x1797 = 0xcc	;QPSK_EXT
    ucTmp = 0x40;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1797, &ucTmp, 1); 
    //0x1798 = 0x1f	;QPSK_EXT
    ucTmp = 0x1F;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1798, &ucTmp, 1); 
    //0x1799 = 0xe6	;16QAM_EXT
    ucTmp = 0xA0;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1799, &ucTmp, 1); 
    //0x179a = 0x0f	;16QAM_EXT
    ucTmp = 0x0F;   MT5136_DVBT2_SetReg(psDemodCtx, 0x179A, &ucTmp, 1); 
    //0x179b = 0x97	;64QAM_EXT
    ucTmp = 0x69;   MT5136_DVBT2_SetReg(psDemodCtx, 0x179B, &ucTmp, 1); 
    //0x179c = 0x0a	;64QAM_EXT
    ucTmp = 0x0A;   MT5136_DVBT2_SetReg(psDemodCtx, 0x179C, &ucTmp, 1); 
    //0x179d = 0xf3	;256QAM_EXT
    ucTmp = 0xd0;   MT5136_DVBT2_SetReg(psDemodCtx, 0x179D, &ucTmp, 1); 
    //0x179e =0x07	;256QAM_EXT
    ucTmp = 0x07;   MT5136_DVBT2_SetReg(psDemodCtx, 0x179E, &ucTmp, 1); 

    //0x179f = 0xcc	;QPSK
    //ucTmp = 0x40;   MT5136_DVBT2_SetReg(psDemodCtx, 0x179F, &ucTmp, 1);
	ucTmp = 0x52;   MT5136_DVBT2_SetReg(psDemodCtx, 0x179F, &ucTmp, 1);//Mingsheng 120815,0x40->0x52 for DTG112/120 Picture break;
    //0x17a0 = 0x1f	;QPSK
    ucTmp = 0x1F;   MT5136_DVBT2_SetReg(psDemodCtx, 0x17A0, &ucTmp, 1); 
    //0x17a1 = 0xe6	;16QAM
    ucTmp = 0xA0;   MT5136_DVBT2_SetReg(psDemodCtx, 0x17A1, &ucTmp, 1); 
    //0x17a2 = 0x0f	;16QAM
    ucTmp = 0x0F;   MT5136_DVBT2_SetReg(psDemodCtx, 0x17A2, &ucTmp, 1); 
    //0x17a3 = 0x97	;64QAM
    ucTmp = 0x69;   MT5136_DVBT2_SetReg(psDemodCtx, 0x17A3, &ucTmp, 1); 
    //0x17a4 = 0x0a	;64QAM
    ucTmp = 0x0A;   MT5136_DVBT2_SetReg(psDemodCtx, 0x17A4, &ucTmp, 1); 
    //0x17a5 = 0xf3	;256QAM
    ucTmp = 0xd0;   MT5136_DVBT2_SetReg(psDemodCtx, 0x17A5, &ucTmp, 1); 
    //0x17a6 = 0x07	;256QAM
    ucTmp = 0x07;   MT5136_DVBT2_SetReg(psDemodCtx, 0x17A6, &ucTmp, 1); 

    //0x17a7 = 0xcc	;QPSK_EXT
    ucTmp = 0x40;   MT5136_DVBT2_SetReg(psDemodCtx, 0x17A7, &ucTmp, 1);     
    //0x17a8 = 0x1f	;QPSK_EXT
    ucTmp = 0x1F;   MT5136_DVBT2_SetReg(psDemodCtx, 0x17A8, &ucTmp, 1);     
    //0x17a9 = 0xe6	;16QAM_EXT
    ucTmp = 0xA0;   MT5136_DVBT2_SetReg(psDemodCtx, 0x17A9, &ucTmp, 1);     
    //0x17aa = 0x0f	;16QAM_EXT
    ucTmp = 0x0F;   MT5136_DVBT2_SetReg(psDemodCtx, 0x17AA, &ucTmp, 1);     
    //0x17ab = 0x97	;64QAM_EXT
    ucTmp = 0x69;   MT5136_DVBT2_SetReg(psDemodCtx, 0x17AB, &ucTmp, 1);     
    //0x17ac = 0x0a	;64QAM_EXT
    ucTmp = 0x0A;   MT5136_DVBT2_SetReg(psDemodCtx, 0x17AC, &ucTmp, 1);     
    //0x17ad = 0xf3	;256QAM_EXT
    ucTmp = 0xd0;   MT5136_DVBT2_SetReg(psDemodCtx, 0x17AD, &ucTmp, 1);     
    //0x17ae = 0x07	;256QAM_EXT
    ucTmp = 0x07;   MT5136_DVBT2_SetReg(psDemodCtx, 0x17AE, &ucTmp, 1); 
    
    //;[firmware control]
    //;ANIR Bypass
    //0x1330 = 0x45; // ANIR bypass, temp
    ucTmp = 0x45;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1330, &ucTmp, 1); 
    //0x0009 = 0x10; // bypass ANIR control, temp
    ucTmp = 0x08;   MT5136_DVBT2_SetReg(psDemodCtx, 0x0009, &ucTmp, 1); 

    //;[Monitor]
    //;SNR monitor setting
    //0x1491 = 0x02; //bit[2:1], 0: CP, 1: CP_AR, 2: SP, 3:P2, wo sym_idx
    ucTmp = 0x02;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1491, &ucTmp, 1);     
    //;0x14B3 = 0xC0; //[7], P2 on/off, [6], sp on/off, [5] cp on/off
    //0x19F2 = 0x0A; //bit[4:3], 0: CP, 1: CP_AR, 2: SP, 3:P2, w sym_idx
    ucTmp = 0x0A;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19F2, &ucTmp, 1);     
    //0x000A = 0x04; //Need to input the symbol index for the displayed symbol SNR
    ucTmp = 0x04;   MT5136_DVBT2_SetReg(psDemodCtx, 0x000A, &ucTmp, 1);     
    //0x19d7 = 0x10;// LDPC monotor
    ucTmp = 0x10;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19D7, &ucTmp, 1);  

#if 0
    //;debug port setting
    //0x0e02 = 0x00 ; SLD 0x03, DVB FE 0x04, DVB sync 0x00
    ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0xE02, &ucTmp, 1);     
    //0x1336 = 0x14 ; for DVB FE, active when 0x0e02 = 0x04
    ucTmp = 0x14;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1336, &ucTmp, 1);     
    //0x1440 = 0x87 ; for DVBT2, active when 0x0e02 = 0x00
    ucTmp = 0x87;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1440, &ucTmp, 1); 
#endif

    //; DJB
    //;0x15cf = 0x00

    //;emulate FW PLP selection
    //0x1704 = 0x0a; 
    ucTmp = 0x0A;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1704, &ucTmp, 1); 
    
    //; Analog AD setting (FPGA only)
    //;0x0E0E = 0x07  

    //; reset & release SLD_ANAIF setting
    //;0x0E01 = 0x00
    //;0x0E01 = 0x80

    //; Turn on SLD_INR
    //;0x0F71 = 0x60

    //;0x0FFD bit7 enable fef
    //0x0FFD = 0xC0;  // for b7=1 :  FEF control PRA enable, b6 = 1 : Beacon valid enable
    ucTmp = 0xC0;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFFD, &ucTmp, 1);
    
    //;0x0FDF bit0 , fix PGA gain
    //;0x0FDF = 0x01;

    //; SLD_FE setting
    //0x0F90 = 0x48 
    ucTmp = 0x48;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF90, &ucTmp, 1);
    //0x0FB2 = 0x03 
    ucTmp = 0x03;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFB2, &ucTmp, 1);
    //0x0FB3 = 0x60 
    ucTmp = 0x60;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFB3, &ucTmp, 1);
    //0x0FB5 = 0x40 
    ucTmp = 0x40;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFB5, &ucTmp, 1);
    //0x0FC0 = 0x7A 
    ucTmp = 0x7A;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFC0, &ucTmp, 1);
    //0x0FC1 = 0x0A 
    ucTmp = 0x0A;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFC1, &ucTmp, 1);
    //0x0FC2 = 0x21 
    ucTmp = 0x21;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFC2, &ucTmp, 1);
    //0x0FC3 = 0x34 
    ucTmp = 0x34;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFC3, &ucTmp, 1);
    //0x0FCA = 0x77 
    ucTmp = 0x77;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFCA, &ucTmp, 1);
    //0x0FCB = 0x00 
    ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFCB, &ucTmp, 1);
    //0x0FCC = 0x00 
    ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFCC, &ucTmp, 1);
    //0x0FD4 = 0x04 
    ucTmp = 0x04;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFD4, &ucTmp, 1);
	
#if 1 // Modfied by Chihau (Check why there is problem in DVBT sync)	
    //0x0FD5 = 0xE0 
//    ucTmp = 0xE0;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFD5, &ucTmp, 1);
    //0x0FD6 = 0xE0 
//    ucTmp = 0xE0;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFD6, &ucTmp, 1);
    //0x0FD7 = 0xEC 
    ucTmp = 0xEC;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFD7, &ucTmp, 1);
    //0x0FD8 = 0xD0 
//    ucTmp = 0xD0;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFD8, &ucTmp, 1);
//    ucTmp = 0xF7;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFD8, &ucTmp, 1);
    //0x0FD9 = 0xD0 
//    ucTmp = 0xD0;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFD9, &ucTmp, 1);
//    ucTmp = 0xF7;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFD9, &ucTmp, 1);
    //0x0FDA = 0x7F 
    ucTmp = 0x7F;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFDA, &ucTmp, 1);
    //0x0FDB = 0x7A 
//    ucTmp = 0x7A;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFDB, &ucTmp, 1);
    //0x0FDC = 0x80 
    ucTmp = 0x80;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFDC, &ucTmp, 1);
    //0x0F91 = 0x40 
    ucTmp = 0x40;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF91, &ucTmp, 1);
    //0x0F97 = 0x22 
    ucTmp = 0x22;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF97, &ucTmp, 1);
    //0x0F9A = 0x7A 
    ucTmp = 0x7A;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF9A, &ucTmp, 1);
    //0x0F9B = 0x0A 
    ucTmp = 0x0A;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF9B, &ucTmp, 1);
    //0x0F9C = 0x21 
    ucTmp = 0x21;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF9C, &ucTmp, 1);
    //0x0F9D = 0x1C 
    ucTmp = 0x1C;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF9D, &ucTmp, 1);
    //0x0FDD = 0x80 
//    ucTmp = 0x80;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFDD, &ucTmp, 1);
    //0x0FDE = 0x80 
//    ucTmp = 0x80;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFDE, &ucTmp, 1);
    //0x0FDF = 0x20 
//    ucTmp = 0x20;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFDF, &ucTmp, 1);
#else
    //0x0FD5 = 0xE0 
    ucTmp = 0xE0;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFD5, &ucTmp, 1);
    //0x0FD6 = 0xE0 
    ucTmp = 0xE0;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFD6, &ucTmp, 1);
    //0x0FD7 = 0xEC 
    ucTmp = 0xEC;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFD7, &ucTmp, 1);
    //0x0FD8 = 0xD0 
    //ucTmp = 0xD0;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFD8, &ucTmp, 1);
    ucTmp = 0xF7;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFD8, &ucTmp, 1);
    //0x0FD9 = 0xD0 
    //ucTmp = 0xD0;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFD9, &ucTmp, 1);
    ucTmp = 0xF7;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFD9, &ucTmp, 1);
    //0x0FDA = 0x7F 
    ucTmp = 0x7F;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFDA, &ucTmp, 1);
    //0x0FDB = 0x7A 
    ucTmp = 0x7A;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFDB, &ucTmp, 1);
    //0x0FDC = 0x80 
    ucTmp = 0x80;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFDC, &ucTmp, 1);
    //0x0F91 = 0x40 
    ucTmp = 0x40;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF91, &ucTmp, 1);
    //0x0F97 = 0x22 
    ucTmp = 0x22;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF97, &ucTmp, 1);
    //0x0F9A = 0x7A 
    ucTmp = 0x7A;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF9A, &ucTmp, 1);
    //0x0F9B = 0x0A 
    ucTmp = 0x0A;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF9B, &ucTmp, 1);
    //0x0F9C = 0x21 
    ucTmp = 0x21;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF9C, &ucTmp, 1);
    //0x0F9D = 0x1C 
    ucTmp = 0x1C;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF9D, &ucTmp, 1);
    //0x0FDD = 0x80 
    ucTmp = 0x80;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFDD, &ucTmp, 1);
    //0x0FDE = 0x80 
    ucTmp = 0x80;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFDE, &ucTmp, 1);
    //0x0FDF = 0x20 
    ucTmp = 0x20;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFDF, &ucTmp, 1);
#endif

    //0x0FE3 = 0x68 
    ucTmp = 0x68;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFE3, &ucTmp, 1);
    //0x0FB0 = 0xA3 
    ucTmp = 0xA3;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFB0, &ucTmp, 1);
    //0x0FB7 = 0x0F 
    ucTmp = 0x0F;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFB7, &ucTmp, 1);
    //0x0FB8 = 0x0A 
    ucTmp = 0x0A;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFB8, &ucTmp, 1);
    //0x0FB9 = 0x0C 
    ucTmp = 0x0C;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFB9, &ucTmp, 1);
    //0x0FB6 = 0x55 
    ucTmp = 0x55;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFB6, &ucTmp, 1);
    //0x0F94 = 0x10 
    ucTmp = 0x10;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF94, &ucTmp, 1);
    //0x0FA4 = 0x10 
    ucTmp = 0x10;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFA4, &ucTmp, 1);
    //0x0F80 = 0x03 
    ucTmp = 0x03;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF80, &ucTmp, 1);
    //0x0F82 = 0x10 
    ucTmp = 0x10;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF82, &ucTmp, 1);

    //; set register 0x0F96 bit7 to 0 and keep another bits  to be default value.
    //; use fixed dagc gain, and the default gain is 1.  0x0F98, 0x0F99
    //0x0F96 = 0x68
    ucTmp = 0x68;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF96, &ucTmp, 1);
    //0x0F65 = 0x85 
    ucTmp = 0x85;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF65, &ucTmp, 1);    
    //0x0F60 = 0xFF 
    ucTmp = 0xFF;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF60, &ucTmp, 1);
    
    //; Reset DVB_FE datapath from GPM
    //0x0E3B = 0xFD
    ucTmp = 0xFD;   MT5136_DVBT2_SetReg(psDemodCtx, 0xE3B, &ucTmp, 1); 

	if (psDemodCtx->u1ChBw == MT5136T2_BW_5MHz)  // 5MHz bandwidth
	{
    	//; DVB_FE change to bank 1 from FPGA1_1109
    	//; FE_INT ; 5M: 0x25CCCCCD 6M: 0x3F000000 (default), 7M: 0x36000000
    	//; D732
    	//;0x1320 = 0xCD
    	ucTmp = 0xCD;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1320, &ucTmp, 1);
    	//;0x1321 = 0xCC
    	ucTmp = 0xCC;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1321, &ucTmp, 1);
    	//;0x1322 = 0xCC
    	ucTmp = 0xCC;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1322, &ucTmp, 1);
    	//;0x1323 = 0x25
    	ucTmp = 0x25;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1323, &ucTmp, 1);

    	//;DC rate conversion 8M:0x56B, 7M: 0x4BE, 6M: 0x410 5M: 0x363
    	//;0x1304 = 0x63
    	ucTmp = 0x63;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1304, &ucTmp, 1);
    	//;0x1305 = 0x03
    	ucTmp = 0x03;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1305, &ucTmp, 1);
		//;0x1310 = 0x0a
    	ucTmp = 0x0a;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1310, &ucTmp, 1);
	}
	else if (psDemodCtx->u1ChBw == MT5136T2_BW_6MHz)  // 6MHz bandwidth
	{
    	//; DVB_FE change to bank 1 from FPGA1_1109
    	//; FE_INT ; 6M: 0x3F000000 (default), 7M: 0x36000000
    	//; D732
    	//;0x1320 = 0x00
    	ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1320, &ucTmp, 1);
    	//;0x1321 = 0x00
    	ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1321, &ucTmp, 1);
    	//;0x1322 = 0x00
    	ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1322, &ucTmp, 1);
    	//;0x1323 = 0x36
    	ucTmp = 0x3F;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1323, &ucTmp, 1);

    	//;DC rate conversion 8M:0x56B, 7M: 0x4BE, 6M: 0x410
    	//;0x1304 = 0xBE
    	ucTmp = 0x10;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1304, &ucTmp, 1);
    	//;0x1305 = 0x04
    	ucTmp = 0x04;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1305, &ucTmp, 1);
	}
	else if (psDemodCtx->u1ChBw == MT5136T2_BW_7MHz)  // 7MHz bandwidth
	{
    	//; DVB_FE change to bank 1 from FPGA1_1109
    	//; FE_INT ; 7M: 0x36000000 (default), 7M: 0x36000000
    	//; D732
    	//;0x1320 = 0x00
    	ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1320, &ucTmp, 1);
    	//;0x1321 = 0x00
    	ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1321, &ucTmp, 1);
    	//;0x1322 = 0x00
    	ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1322, &ucTmp, 1);
    	//;0x1323 = 0x36
    	ucTmp = 0x36;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1323, &ucTmp, 1);

    	//;DC rate conversion 8M:0x56B, 7M: 0x4BE
    	//;0x1304 = 0xBE
    	ucTmp = 0xBE;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1304, &ucTmp, 1);
    	//;0x1305 = 0x04
    	ucTmp = 0x04;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1305, &ucTmp, 1);
	}
	else if (psDemodCtx->u1ChBw == MT5136T2_BW_1p7MHz)  // 1.7MHz bandwidth
	{
    	//; DVB_FE change to bank 1 from FPGA1_1109
    	//; FE_INT ; 7M: 0x36000000 (default), 7M: 0x36000000
    	//; D732
    	//;0x1320 = 0x00
    	ucTmp = 0x3D;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1320, &ucTmp, 1);
    	//;0x1321 = 0x00
    	ucTmp = 0xCB;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1321, &ucTmp, 1);
    	//;0x1322 = 0x00
    	ucTmp = 0x88;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1322, &ucTmp, 1);
    	//;0x1323 = 0x36
    	ucTmp = 0x3A;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1323, &ucTmp, 1);

    	//;DC rate conversion 8M:0x56B, 7M: 0x4BE
    	//;0x1304 = 0xBE
    	ucTmp = 0x18;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1304, &ucTmp, 1);
    	//;0x1305 = 0x04
    	ucTmp = 0x01;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1305, &ucTmp, 1);

	ucTmp = 0x03;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1310, &ucTmp, 1);
	}
    //; DC (central frequency)  //2A69217  2AEBB7A
    //;  2AE3453, 2AA83D5, 2AAD181 2AAF857 2AB4603 25ED098,2F684BE
    //;0x1300 = 0x55
    //;0x1301 = 0x55
    //;0x1302 = 0x55
    //;0x1303 = 0xFD

    //;0x1300 = 0x00 ;BE
    //;0x1301 = 0x84
    //;0x1302 = 0xFF
    //;0x1303 = 0x02

    //; 2AE3DA6
    //;0x1300 = 0x21
    //;0x1301 = 0x20
    //;0x1302 = 0xAE
    //;0x1303 = 0x02

    //; P2 ST TOL
    //;0x13b1 = 0x01

    //; Rate conv
    //;0x1702 = 0x39

    //; CDI
    //;0x1574 = 0x07

    //; DVBT2_SOFT_DEMAP
    //;0x1582 = 0x09;

    //; Enable PLP0
    //0x1770 = 0x28
    ucTmp = 0x28;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1770, &ucTmp, 1);   
    //0x1881 = 0x00
    ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1881, &ucTmp, 1);    
    //0x1882 = 0x00
    ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1882, &ucTmp, 1);    

    //; Switch TDI output buffer to ch2
    //0x17af = 0x02
    ucTmp = 0x02;   MT5136_DVBT2_SetReg(psDemodCtx, 0x17AF, &ucTmp, 1);
    
    //for PLP data
    //; 256QAM, 64K
    //0x19b0 = 0x0e;
    ucTmp = 0x0E;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19B0, &ucTmp, 1);    
    //0x19bd = 0x32;        ;1/2	
    ucTmp = 0x31;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19BD, &ucTmp, 1);        
    //0x19be = 0x2c;        ;3/5	
    ucTmp = 0x2b;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19BE, &ucTmp, 1);        
    //0x19bf = 0x38;        ;2/3	
    ucTmp = 0x37;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19BF, &ucTmp, 1);        
    //0x19c0 = 0x38;        ;3/4	
    ucTmp = 0x37;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19C0, &ucTmp, 1);        
    //0x19c1 = 0x38;        ;4/5
    ucTmp = 0x37;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19C1, &ucTmp, 1);        
    //0x19c2 = 0x38;        ;5/6      
    ucTmp = 0x37;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19C2, &ucTmp, 1);        


	
    //; 256QAM, 16K
    //0x19b0 = 0x06;
    ucTmp = 0x06;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19B0, &ucTmp, 1);        
    //0x19bd = 0x2d;        ;1/2
    ucTmp = 0x2C;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19BD, &ucTmp, 1);        
    //0x19be = 0x32;        ;3/5
    ucTmp = 0x31;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19BE, &ucTmp, 1);        
    //0x19bf = 0x37;        ;2/3      
    ucTmp = 0x36;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19BF, &ucTmp, 1);        
    //0x19c0 = 0x37;        ;3/4
    ucTmp = 0x36;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19C0, &ucTmp, 1);        
    //0x19c1 = 0x42;        ;4/5
    ucTmp = 0x41;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19C1, &ucTmp, 1);        
    //0x19c2 = 0x3a;        ;5/6    
    ucTmp = 0x39;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19C2, &ucTmp, 1);    
    
    //; 64QAM, 64K
    //0x19b0 = 0x0c;
    ucTmp = 0x0C;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19B0, &ucTmp, 1);        
    //0x19bd = 0x3b;        ;1/2
    ucTmp = 0x3a;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19BD, &ucTmp, 1);        
    //0x19be = 0x39;        ;3/5
    ucTmp = 0x38;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19BE, &ucTmp, 1);        
    //0x19bf = 0x49;        ;2/3
    ucTmp = 0x48;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19BF, &ucTmp, 1);        
    //0x19c0 = 0x49;        ;3/4
    ucTmp = 0x48;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19C0, &ucTmp, 1);        
    //0x19c1 = 0x4b;        ;4/5      
    ucTmp = 0x4A;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19C1, &ucTmp, 1);        
    //0x19c2 = 0x4a;        ;5/6
    ucTmp = 0x49;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19C2, &ucTmp, 1);        
    //; 64QAM, 16K
    //;0x19b0 = 0x04;
    //; 16QAM, 64K
    //0x19b0 = 0x0a;
    ucTmp = 0x0A;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19B0, &ucTmp, 1);        
    //0x19bd = 0x5a;        ;1/2
    ucTmp = 0x59;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19BD, &ucTmp, 1);        
    //0x19be = 0x57;        ;3/5
    ucTmp = 0x56;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19BE, &ucTmp, 1);        
    //0x19bf = 0x6F;        ;2/3
    ucTmp = 0x6E;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19BF, &ucTmp, 1);        
    //0x19c0 = 0x6F;        ;3/4
    ucTmp = 0x6E;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19C0, &ucTmp, 1);        
    //0x19c1 = 0x72;        ;4/5
    ucTmp = 0x71;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19C1, &ucTmp, 1);        
    //0x19c2 = 0x71;        ;5/6
    ucTmp = 0x70;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19C2, &ucTmp, 1);        
    //; 16QAM, 16K
    //;0x19b0 = 0x02;

    //; QPSK, 64K
    //0x19b0 = 0x08;
    ucTmp = 0x08;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19B0, &ucTmp, 1);     
    //0x19bd = 0xB8;        ;1/2
    ucTmp = 0xB7;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19BD, &ucTmp, 1);     
    //0x19be = 0xB2;        ;3/5
    ucTmp = 0xB1;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19BE, &ucTmp, 1);     
    //0x19bf = 0xE4;        ;2/3	
    ucTmp = 0xE3;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19BF, &ucTmp, 1);     
    //0x19c0 = 0xE3;        ;3/4
    ucTmp = 0xE2;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19C0, &ucTmp, 1);     
    //0x19c1 = 0xE7;        ;4/5
    ucTmp = 0xE6;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19C1, &ucTmp, 1);     
    //0x19c2 = 0xE7;        ;5/6
    ucTmp = 0xE6;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19C2, &ucTmp, 1);     
    //; QPSK, 16K
    //;0x19b0 = 0x00;

    //;L1pre
    //;0x19c3 = 0x06;
    //;L1post
    //;0x19c4 = 0x09;
    //; iteration L1+DATA; hold maximum iteration for LDPC
    //0x19b0 = 0x01;
    ucTmp = 0x01;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19B0, &ucTmp, 1);     
    //;0x19d9 = 0x03;	// b[1] : enable freeze iteration at L1
    //0x19d9 = 0x01;
    ucTmp = 0x01;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19D9, &ucTmp, 1);     
    //;0x19da = 0x0A;  // data iteration number when freeze mode
    //0x19da = 0x12;  // data iteration number when freeze mode	
    ucTmp = 0x12;   MT5136_DVBT2_SetReg(psDemodCtx, 0x19DA, &ucTmp, 1); 
    
    //;GPM
    //;0x15a0 = 0x4;	;BCH FIFO recover enable

    //; Release DVB_FE datapath reset from GPM
    //0x0E3B = 0xFF;
    ucTmp = 0xFF;   MT5136_DVBT2_SetReg(psDemodCtx, 0xE3B, &ucTmp, 1); 
    
    //; swap DJB
    //0x15b0 = 0xc3;
    ucTmp = 0xC3;   MT5136_DVBT2_SetReg(psDemodCtx, 0x15B0, &ucTmp, 1); 

	// CCI removal bypass --> Harry 20120610 (no level limitation to saturate CCI)
    //0x1145 = 0x50;
    ucTmp = 0x50;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1145, &ucTmp, 1); 
	  	// Harry fine tune 20120610
	//0x19B3 = 0x38; 
	ucTmp = 0x0f;	MT5136_DVBT2_SetReg(psDemodCtx, 0x19B0, &ucTmp, 1); 
	ucTmp = 0x27;	MT5136_DVBT2_SetReg(psDemodCtx, 0x19B3, &ucTmp, 1); 	
	//0x19B8 = 0x55; 
	ucTmp = 0x55;	MT5136_DVBT2_SetReg(psDemodCtx, 0x19B8, &ucTmp, 1); 
	//0x19B7 = 0x43; 
	ucTmp = 0x43;	MT5136_DVBT2_SetReg(psDemodCtx, 0x19B7, &ucTmp, 1); 
	//0x19B2 = 0x1D; 
	ucTmp = 0x1D;	MT5136_DVBT2_SetReg(psDemodCtx, 0x19B2, &ucTmp, 1); 
	//0x19B4 = 0x2F; 
	ucTmp = 0x2F;	MT5136_DVBT2_SetReg(psDemodCtx, 0x19B4, &ucTmp, 1);
	ucTmp = 0x65;	MT5136_DVBT2_SetReg(psDemodCtx, 0x19B8, &ucTmp, 1); 
    //; FPGA slow
    //;0x0ef1 = 0xd9;
    //;0x0ef2 = 0x99;
    //;0x0ef3 = 0x9a;
    //;0x0ef4 = 0x15;
    //;0x0ef5 = 0x55;
    //;0x0ef6 = 0x55;

    //; Release dvbt2_md_en
    //0x1143 = 0x01   
    //ucTmp = 0x01;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1143, &ucTmp, 1);     

	if (psDemodCtx->u1ChBw == MT5136T2_BW_6MHz || // 6MHz bandwidth
		psDemodCtx->u1ChBw == MT5136T2_BW_8MHz)   // 8MHz bandwidth
	{
		//; SLD setting
    	//0x1340 = 0X98
    	ucTmp = 0x98;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1340, &ucTmp, 1);       	
		//0x1349 = 0x00
    	ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1349, &ucTmp, 1);       	
	}

    //; SLD setting
    //0x0F90 = 0x48
    ucTmp = 0x48;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF90, &ucTmp, 1);       	
	//0x0F93 = 0x0C
    ucTmp = 0x0C;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF93, &ucTmp, 1);       	
	//0x0F94 = 0x30
	ucTmp = 0x30;	MT5136_DVBT2_SetReg(psDemodCtx, 0xF94, &ucTmp, 1);		 
	//0x0FA4 = 0x30
	ucTmp = 0x30;	MT5136_DVBT2_SetReg(psDemodCtx, 0xFA4, &ucTmp, 1);		 
	//0x0FB0 = 0xA8
	ucTmp = 0xA8;	MT5136_DVBT2_SetReg(psDemodCtx, 0xFB0, &ucTmp, 1);		 
	//0x0FB1 = 0x21
	ucTmp = 0x21;	MT5136_DVBT2_SetReg(psDemodCtx, 0xFB1, &ucTmp, 1);		 
	//0x0FB2 = 0x03
	ucTmp = 0x03;	MT5136_DVBT2_SetReg(psDemodCtx, 0xFB2, &ucTmp, 1);		 
	//0x0FB3 = 0x60
	ucTmp = 0x60;	MT5136_DVBT2_SetReg(psDemodCtx, 0xFB3, &ucTmp, 1);		 
	//0x0FB5 = 0x40
	ucTmp = 0x40;	MT5136_DVBT2_SetReg(psDemodCtx, 0xFB5, &ucTmp, 1);		 
	//0x0FB6 = 0x5D
	ucTmp = 0x5D;	MT5136_DVBT2_SetReg(psDemodCtx, 0xFB6, &ucTmp, 1);		 
	//0x0FC0 = 0x7A
	ucTmp = 0x7A;	MT5136_DVBT2_SetReg(psDemodCtx, 0xFC0, &ucTmp, 1);		 
	//0x0FC2 = 0x21
	ucTmp = 0x21;	MT5136_DVBT2_SetReg(psDemodCtx, 0xFC2, &ucTmp, 1);		 
	//0x0FCA = 0x79
	ucTmp = 0x79;	MT5136_DVBT2_SetReg(psDemodCtx, 0xFCA, &ucTmp, 1);		 
	//0x0FCB = 0x00
	ucTmp = 0x00;	MT5136_DVBT2_SetReg(psDemodCtx, 0xFCB, &ucTmp, 1);		 
	//0x0FCC = 0x00
	ucTmp = 0x00;	MT5136_DVBT2_SetReg(psDemodCtx, 0xFCC, &ucTmp, 1);		 

#if 0 // Check why there is a problem in DVBT sync
	//0x0FD5 = 0xFF
	ucTmp = 0xFF;	MT5136_DVBT2_SetReg(psDemodCtx, 0xFD5, &ucTmp, 1);		 
	//0x0FD6 = 0xFF
	ucTmp = 0xFF;	MT5136_DVBT2_SetReg(psDemodCtx, 0xFD6, &ucTmp, 1);		 
	//0x0FD8 = 0xF7
	ucTmp = 0xF7;	MT5136_DVBT2_SetReg(psDemodCtx, 0xFD8, &ucTmp, 1);		 
	//0x0FD9 = 0xF7
	ucTmp = 0xF7;	MT5136_DVBT2_SetReg(psDemodCtx, 0xFD9, &ucTmp, 1);		 
	//0x0FDB = 0x41
	ucTmp = 0x41;	MT5136_DVBT2_SetReg(psDemodCtx, 0xFDB, &ucTmp, 1);		 
	//0x0FDD = 0xA7
	ucTmp = 0xA7;	MT5136_DVBT2_SetReg(psDemodCtx, 0xFDD, &ucTmp, 1);		 
	//0x0FDE = 0xA7
	ucTmp = 0xA7;	MT5136_DVBT2_SetReg(psDemodCtx, 0xFDE, &ucTmp, 1);		 
#endif

	// IF AGC On and PGA Off
	//0x0FDF = 0x11
	//ucTmp = 0x71;	MT5136_DVBT2_SetReg(psDemodCtx, 0xFDF, &ucTmp, 1);		 
	ucTmp = 0x11;	MT5136_DVBT2_SetReg(psDemodCtx, 0xFDF, &ucTmp, 1);	

	// Rotator Setting
	//0x0F82 = 0xD0
	ucTmp = 0xD0;	MT5136_DVBT2_SetReg(psDemodCtx, 0xF82, &ucTmp, 1);	
	
#if 0
	//8.5MHz for DVBT
	//0x011A = 0xCC
	ucTmp = 0xCC;	MT5136_DVBT2_SetReg(psDemodCtx, 0x011A, &ucTmp, 1);		 
	//0x011B = 0x5E
	ucTmp = 0x5E;	MT5136_DVBT2_SetReg(psDemodCtx, 0x011B, &ucTmp, 1);		 
	//0x011C = 0x42
	ucTmp = 0x42;	MT5136_DVBT2_SetReg(psDemodCtx, 0x011C, &ucTmp, 1);		 
	//0x011D = 0x01
	ucTmp = 0x01;	MT5136_DVBT2_SetReg(psDemodCtx, 0x011D, &ucTmp, 1);		 
	
	// -8.5MHz for DVBT2
	//0x1300 = 0xBE
	ucTmp = 0xBE;	MT5136_DVBT2_SetReg(psDemodCtx, 0x1300, &ucTmp, 1);		 
	//0x1301 = 0x84
	ucTmp = 0x84;	MT5136_DVBT2_SetReg(psDemodCtx, 0x1301, &ucTmp, 1);		 
	//0x1302 = 0xF6
	ucTmp = 0xF6;	MT5136_DVBT2_SetReg(psDemodCtx, 0x1302, &ucTmp, 1);		 
	//0x1303 = 0x1A
	ucTmp = 0x1A;	MT5136_DVBT2_SetReg(psDemodCtx, 0x1303, &ucTmp, 1);		 
#endif	

	//0x014E = 0x01
	ucTmp = 0x01;	MT5136_DVBT2_SetReg(psDemodCtx, 0x014E, &ucTmp, 1); 	 
	//0x014F = 0x21
	ucTmp = 0x21;	MT5136_DVBT2_SetReg(psDemodCtx, 0x014F, &ucTmp, 1); 	 

	
	//0x0F71 = 0x21
	ucTmp = 0x21;	MT5136_DVBT2_SetReg(psDemodCtx, 0x0F71, &ucTmp, 1); 	 
	//0x0F80 = 0x03
	ucTmp = 0x03;	MT5136_DVBT2_SetReg(psDemodCtx, 0x0F80, &ucTmp, 1); 	 

	//0x011E = 0x00
	ucTmp = 0x00;	MT5136_DVBT2_SetReg(psDemodCtx, 0x011E, &ucTmp, 1); 	 
	//0x011F = 0x00
	ucTmp = 0x00;	MT5136_DVBT2_SetReg(psDemodCtx, 0x011F, &ucTmp, 1); 	 
	//0x0120 = 0xE8
	ucTmp = 0xE8;	MT5136_DVBT2_SetReg(psDemodCtx, 0x0120, &ucTmp, 1); 	 
	//0x0121 = 0x05
	ucTmp = 0x05;	MT5136_DVBT2_SetReg(psDemodCtx, 0x0121, &ucTmp, 1); 	 
	//0x0122 = 0x14
	ucTmp = 0x14;	MT5136_DVBT2_SetReg(psDemodCtx, 0x0122, &ucTmp, 1); 	 
	//0x019A = 0x88
	ucTmp = 0x88;	MT5136_DVBT2_SetReg(psDemodCtx, 0x019A, &ucTmp, 1); 	 
	//0x0F73 = 0x4A
	ucTmp = 0x4A;	MT5136_DVBT2_SetReg(psDemodCtx, 0x0F73, &ucTmp, 1); 	 
	//0x0F75 = 0xB6
	ucTmp = 0xB6;	MT5136_DVBT2_SetReg(psDemodCtx, 0x0F75, &ucTmp, 1); 	 

#if 0
	// Hz_tran should be in driver
	//0x0132 = 0x81
	ucTmp = 0x88;	MT5136_DVBT2_SetReg(psDemodCtx, 0x0132, &ucTmp, 1); 	 
	//0x0133 = 0xB5
	ucTmp = 0xB5;	MT5136_DVBT2_SetReg(psDemodCtx, 0x0133, &ucTmp, 1); 	 
	//0x0128 = 0x20
	ucTmp = 0x20;	MT5136_DVBT2_SetReg(psDemodCtx, 0x0128, &ucTmp, 1); 	 
#endif
	
	//0x0F60 = 0x00
	ucTmp = 0x00;	MT5136_DVBT2_SetReg(psDemodCtx, 0x0F60, &ucTmp, 1);		 
	//0x0F60 = 0xF8
	ucTmp = 0xF8;	MT5136_DVBT2_SetReg(psDemodCtx, 0x0F60, &ucTmp, 1);		 
	
	// New PGA index
	//0x0FE0 = 0x00
	//ucTmp = 0x00;	MT5136_DVBT2_SetReg(psDemodCtx, 0x0FE0, &ucTmp, 1); 	 
	ucTmp = 0x0F;	MT5136_DVBT2_SetReg(psDemodCtx, 0x0FE0, &ucTmp, 1); 
		
    //0xF60 = 0x00
    ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF60, &ucTmp, 1);       
    //0xF60 = 0xF8
    ucTmp = 0xF8;   MT5136_DVBT2_SetReg(psDemodCtx, 0xF60, &ucTmp, 1);       
    //0xFE0 = 0x01
    ucTmp = 0x0F;   MT5136_DVBT2_SetReg(psDemodCtx, 0xFE0, &ucTmp, 1);       

	// DVBT demod reset
	//0x0143 = 0x00
	ucTmp = 0x00;	MT5136_DVBT2_SetReg(psDemodCtx, 0x0143, &ucTmp, 1); 	  
	//0x0143 = 0xFF
	ucTmp = 0xFF;	MT5136_DVBT2_SetReg(psDemodCtx, 0x0143, &ucTmp, 1); 	  

	// DVBT2 demod reset
	//0x1143 = 0x00
    ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1143, &ucTmp, 1);       
    //0x1143 = 0x01
    ucTmp = 0x01;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1143, &ucTmp, 1);       

    //TS Serial Mode
    //ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x15CE, &ucTmp, 1);       
    ucTmp = 0x01;   MT5136_DVBT2_SetReg(psDemodCtx, 0x15D3, &ucTmp, 1);      
    //TS Clock
    ucTmp = 0x44;   MT5136_DVBT2_SetReg(psDemodCtx, 0x81E, &ucTmp, 1);
    //TS Data 0
    ucTmp = 0x44;   MT5136_DVBT2_SetReg(psDemodCtx, 0x818, &ucTmp, 1);  

	//0x0e00 = 0x00
	ucTmp = 0x00;	MT5136_DVBT2_SetReg(psDemodCtx, 0xE00, &ucTmp, 1);	   
	//0x0e03 = 0x00
	ucTmp = 0x00;	MT5136_DVBT2_SetReg(psDemodCtx, 0xE03, &ucTmp, 1);	   

    return 0;
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_DisConnect
 *  power down demod module.  
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 */
//-----------------------------------------------------------------------------
VOID DVBT2_DisConnect(DEMOD_CTX_T2 *psDemodCtx)
{
    UINT8 ucTmp = 0;
    mcSHOW_DBG_MSG(("PI DVBT2 Disconnect!!\n"));

    //SLD IF/RF AGC setting
    //RF_AGC_PAD_EN: 0xF62[3] = 0 (hiZ)
    //IF_AGC_PAD_EN: 0xF62[2] = 0 (hiZ)
    DVBT2_GetReg(psDemodCtx, 0xF62, &ucTmp, 1);
    mcCLR_BIT(ucTmp, 3);
    mcCLR_BIT(ucTmp, 2);
    DVBT2_SetReg(psDemodCtx, 0xF62, &ucTmp, 1);    
    
    // stop driver
    mcSHOW_DBG_MSG4(("DVBT2_DriverStop(psDemodCtx);\n"));    
    DVBT2_DriverStop(psDemodCtx);

    mcSHOW_DBG_MSG4(("DVBT2_SetTsOutput(psDemodCtx, FALSE);\n")); 
    DVBT_SetTsOutput(psDemodCtx, FALSE);   
    DVBT2_SetTsOutput(psDemodCtx, FALSE);

    mcSHOW_DBG_MSG4(("DVBT2_DisConnect_HWConfig(psDemodCtx);\n"));    
    DVBT2_DisConnect_HWConfig(psDemodCtx);
    
    // disable AGC => AGC = 0V

    // delay for SB reset, at least one complete TS packet
    //mcDELAY_MS(10);

    // set demod54m_ck to TVDPLL/10 and demod60m_ck to TVDPLL/9

    // power down DVBT2 most modules except CTRL path clock (uP_CLK)

    // power down AD clock (if with external DTD, such as DVBC)
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_DisConnect
 *  power down demod module.  
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 */
//-----------------------------------------------------------------------------
VOID   DVBT2_WO_FW_DisConnect(DEMOD_CTX_T2 *psDemodCtx)
{
	
	UINT8 ucTmp = 0;
	UCHAR ucData = 0;
	mcSHOW_DBG_MSG(("PI  DVBT2_WO_FW_DisConnect!!\n"));

	#if defined(CC_MT5136) 
	//SLD IF/RF AGC setting
    //RF_AGC_PAD_EN: 0xF62[3] = 0 (hiZ)
    //IF_AGC_PAD_EN: 0xF62[2] = 0 (hiZ)
    DVBT2_GetReg(psDemodCtx, 0xF62, &ucTmp, 1);
    mcCLR_BIT(ucTmp, 3);
    mcCLR_BIT(ucTmp, 2);
    DVBT2_SetReg(psDemodCtx, 0xF62, &ucTmp, 1);  

    //TS output disable 
    //DVBT
		ucData = 0xC9;
        DVBT_SetReg(psDemodCtx, 0x203, &ucData, 1);
        mcDELAY_MS(10);
        ucData = 0x49;
        DVBT_SetReg(psDemodCtx, 0x203, &ucData, 1);
		
   //DVBT2 
		ucData = 0x06;
        DVBT2_SetReg(psDemodCtx, 0x15d2, &ucData, 1);
		mcDELAY_MS(10);
        ucData = 0x02;
        DVBT2_SetReg(psDemodCtx, 0x15d2, &ucData, 1);
   //Pad mux	
   
        DVBT2_GetReg(psDemodCtx, 0x8FB,  &ucData, 1);
	    ucData = (ucData & 0x30) >> 4;
	    if(ucData == 0x01) //48pin
	    { 
	    	DVBT2_GetReg(psDemodCtx, 0x8E1, &ucData, 1);
			ucData = ucData & 0x0F;
			DVBT2_SetReg(psDemodCtx, 0x8E1, &ucData, 1);
			ucData = 0;
			DVBT2_SetReg(psDemodCtx, 0x8E2, &ucData, 1);
			mcSHOW_DBG_MSG((" 48pin ts output disable\n"));
	    }
		else if(ucData == 0x02)  //128 pin
		{
			DVBT2_GetReg(psDemodCtx, 0x8E1, &ucData, 1);
			ucData = ucData & 0x0F;
			DVBT2_SetReg(psDemodCtx, 0x8E1, &ucData, 1);
			mcSHOW_DBG_MSG(("128pin ts output disable\n"));
		}
		else
		{
		    mcSHOW_DBG_MSG(("Error, not 48pin and 128pin\n"));
        
		}
	#endif    
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_DisConnect_HW
 *  HW config when disconnect
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 */
//-----------------------------------------------------------------------------
void DVBT2_DisConnect_HWConfig(DEMOD_CTX_T2 *psDemodCtx)
{
    UINT8 ucTmp = 0;

    // PGA PWD:
    //RG_PGAVOCM_PWD: 909h[2]=1
    //RG_PGA_PWD: 909h[1]=1
    //RG_PGAVIOPEN_EN: 909h[0]=1
    DVBT2_GetReg(psDemodCtx, 0x909, &ucTmp, 1);
    mcSET_BIT(ucTmp, 2);
    mcSET_BIT(ucTmp, 1);
    mcSET_BIT(ucTmp, 0);    
    DVBT2_SetReg(psDemodCtx, 0x909, &ucTmp, 1);
    
    // ADC PWD:
    //RG_HPF_PWD_DEMOD: 903h[1]=1
    //RG_ADC_PWD_DEMOD: 903h[0]=1
    DVBT2_GetReg(psDemodCtx, 0x903, &ucTmp, 1);
    mcSET_BIT(ucTmp, 1);
    mcSET_BIT(ucTmp, 0);    
    DVBT2_SetReg(psDemodCtx, 0x903, &ucTmp, 1);    
    
    // RSSIADC PWD:
    //RG_SRV_PWD_B: 920h[4]=0
    DVBT2_GetReg(psDemodCtx, 0x920, &ucTmp, 1);
    mcCLR_BIT(ucTmp, 4);
    DVBT2_SetReg(psDemodCtx, 0x920, &ucTmp, 1);        
    #if 0  //sync from Pana Topup
    // PD 54M gate
    //54M_DVBC_PD: 0xE51[7] = 1
    //54M_DVBT_PD: 0xE51[6] = 1
    //54M_DVBTC_PD: 0xE51[5] = 1
    //54M_DVBT2_PD: 0xE51[4] = 1
    //54M_FE_PD: 0xE51[3] = 1
    DVBT2_GetReg(psDemodCtx, 0xE51, &ucTmp, 1);
    mcSET_BIT(ucTmp, 7);
    mcSET_BIT(ucTmp, 6);
    mcSET_BIT(ucTmp, 5);
    mcSET_BIT(ucTmp, 4);
    mcSET_BIT(ucTmp, 3);    
    DVBT2_SetReg(psDemodCtx, 0xE51, &ucTmp, 1);        

    // PD 86p4M gate
    //86P4M_PD: 0xE55[7] = 1
    DVBT2_GetReg(psDemodCtx, 0xE55, &ucTmp, 1);
    mcSET_BIT(ucTmp, 7);
    DVBT2_SetReg(psDemodCtx, 0xE55, &ucTmp, 1);      
    
    //PD 36M gate
    //36M_PD: 0xE57[6] = 1    
    DVBT2_GetReg(psDemodCtx, 0xE57, &ucTmp, 1);
    mcSET_BIT(ucTmp, 6);    
    DVBT2_SetReg(psDemodCtx, 0xE57, &ucTmp, 1);       
    
    // PD162M gate
    //162M_PD: 0xE70[7] = 1
    DVBT2_GetReg(psDemodCtx, 0xE70, &ucTmp, 1);
    mcSET_BIT(ucTmp, 7);
    DVBT2_SetReg(psDemodCtx, 0xE70, &ucTmp, 1);       

    // PD MPLP gate
    //MPLP_PD: 0xE72[7] = 1
    DVBT2_GetReg(psDemodCtx, 0xE72, &ucTmp, 1);
    mcSET_BIT(ucTmp, 7);
    DVBT2_SetReg(psDemodCtx, 0xE72, &ucTmp, 1);       

    // PD ADCLK
    //ADCLK_DIV4_PD: 0xE6A[6] = 1
    //ADCLK_PD: 0xE6C[6] = 1
    DVBT2_GetReg(psDemodCtx, 0xE6A, &ucTmp, 1);
    mcSET_BIT(ucTmp, 6);
    DVBT2_SetReg(psDemodCtx, 0xE6A, &ucTmp, 1);       
    
    DVBT2_GetReg(psDemodCtx, 0xE6C, &ucTmp, 1);
    mcSET_BIT(ucTmp, 6);
    DVBT2_SetReg(psDemodCtx, 0xE6C, &ucTmp, 1);           
	#endif
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_Connect_HW
 *  HW config when Connect
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 */
//-----------------------------------------------------------------------------
void DVBT2_Connect_HWConfig(DEMOD_CTX_T2 *psDemodCtx)
{
    UINT8 ucTmp;

    // PGA PWO:
    //RG_PGAVOCM_PWD: 909h[2]=0
    //RG_PGA_PWD: 909h[1]=0
    //RG_PGAVIOPEN_EN: 909h[0]=0
    DVBT2_GetReg(psDemodCtx, 0x909, &ucTmp, 1);
    mcCLR_BIT(ucTmp, 2);
    mcCLR_BIT(ucTmp, 1);
    mcCLR_BIT(ucTmp, 0);    
    DVBT2_SetReg(psDemodCtx, 0x909, &ucTmp, 1);
    
    // ADC PWO:
    //RG_HPF_PWD_DEMOD: 903h[1]=0
    //RG_ADC_PWD_DEMOD: 903h[0]=0   
    DVBT2_GetReg(psDemodCtx, 0x903, &ucTmp, 1);
    mcCLR_BIT(ucTmp, 1);
    mcCLR_BIT(ucTmp, 0);    
    DVBT2_SetReg(psDemodCtx, 0x903, &ucTmp, 1);    
    
    // RSSIADC PWO:
    //RG_SRV_PWD_B: 920h[4]=1
    DVBT2_GetReg(psDemodCtx, 0x920, &ucTmp, 1);
    mcSET_BIT(ucTmp, 4);
    DVBT2_SetReg(psDemodCtx, 0x920, &ucTmp, 1);        
    
    // PO 54M gate (DVBT/T2)
    //54M_DVBC_PD: 0xE51[7] = 0
    //54M_DVBT_PD: 0xE51[6] = 0
    //54M_DVBTC_PD: 0xE51[5] = 0
    //54M_DVBT2_PD: 0xE51[4] = 0
    //54M_FE_PD: 0xE51[3] = 0
    DVBT2_GetReg(psDemodCtx, 0xE51, &ucTmp, 1);
    mcCLR_BIT(ucTmp, 7);
    mcCLR_BIT(ucTmp, 6);
    mcCLR_BIT(ucTmp, 5);
    mcCLR_BIT(ucTmp, 4);
    mcCLR_BIT(ucTmp, 3);    
    DVBT2_SetReg(psDemodCtx, 0xE51, &ucTmp, 1);        

    // PO 86p4M gate
    //86P4M_PD: 0xE55[7] = 0
    DVBT2_GetReg(psDemodCtx, 0xE55, &ucTmp, 1);
    mcCLR_BIT(ucTmp, 7);
    DVBT2_SetReg(psDemodCtx, 0xE55, &ucTmp, 1);

    //PO 36M gate
    //36M_PD: 0xE57[6] = 0
    DVBT2_GetReg(psDemodCtx, 0xE57, &ucTmp, 1);
    mcCLR_BIT(ucTmp, 6);    
    DVBT2_SetReg(psDemodCtx, 0xE57, &ucTmp, 1);       
    
    // PO162M gate
    //162M_PD: 0xE70[7] = 0
    DVBT2_GetReg(psDemodCtx, 0xE70, &ucTmp, 1);
    mcCLR_BIT(ucTmp, 7);
    DVBT2_SetReg(psDemodCtx, 0xE70, &ucTmp, 1);       

    // PO MPLP gate
    //MPLP_PD: 0xE72[7] = 0
    DVBT2_GetReg(psDemodCtx, 0xE72, &ucTmp, 1);
    mcCLR_BIT(ucTmp, 7);
    DVBT2_SetReg(psDemodCtx, 0xE72, &ucTmp, 1);       

    // PD ADCLK
    //ADCLK_DIV4_PD: 0xE6A[6] = 0
    DVBT2_GetReg(psDemodCtx, 0xE6A, &ucTmp, 1);
    mcCLR_BIT(ucTmp, 6);
    DVBT2_SetReg(psDemodCtx, 0xE6A, &ucTmp, 1);  
	
    //ADCLK_PD: 0xE6C[6] = 0
    DVBT2_GetReg(psDemodCtx, 0xE6C, &ucTmp, 1);
    mcCLR_BIT(ucTmp, 6);
    DVBT2_SetReg(psDemodCtx, 0xE6C, &ucTmp, 1);           


    //SLD IF/RF AGC setting
    //RF_AGC_PAD_EN: 0xF62[3] = 0 (hiZ)
    //IF_AGC_PAD_EN: 0xF62[2] = 1 (Enable)
    DVBT2_GetReg(psDemodCtx, 0xF62, &ucTmp, 1);
    mcCLR_BIT(ucTmp, 3);
    mcSET_BIT(ucTmp, 2);
    DVBT2_SetReg(psDemodCtx, 0xF62, &ucTmp, 1);    
    
    

    // SLD ANAIF				
    //0x0F80	=	0x03	;// AD_CKPHSEL=1 (bit[1]=1'b1); AD_FMT(binary-offset)=1 (bit[0]=1'b1)
    ucTmp = 0x03;   DVBT2_SetReg(psDemodCtx, 0xF80, &ucTmp, 1);     
    //0x0E01	=	0x00	;// Reset ANAIF
    ucTmp = 0x00;   DVBT2_SetReg(psDemodCtx, 0xE01, &ucTmp, 1);     
    //0x0E01	=	0x80	;// Release ANAIF
    ucTmp = 0x80;   DVBT2_SetReg(psDemodCtx, 0xE01, &ucTmp, 1);     

}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_SetArCoef
 *  Set demod AR coefficient.  
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @param  ucData Value of AR coefficient
 *  @retval   1 : I2C error.
 *  @retval   0 : I2C OK.   
 */
//-----------------------------------------------------------------------------
UINT8 DVBT2_SetArCoef(DEMOD_CTX_T2 *psDemodCtx, UINT8 ucData)
{
#if 0 //Pluto
    BOOL fgRetStatus = TRUE;

    fgRetStatus = fgSetFwVars(psDemodCtx, MT5391_REG_VARYING_AUTO_AR_ON, &ucData, 1);

    if (fgRetStatus)
        return 0;
    else
        return 1;
#else
    return 1;
#endif
}

//----------------------------------------------------------------------------- 
/*
 *  DVBT2_SetNormalMode
 *  Set demod to normal mode.
 *  @param  psDemodCtx  Pointer of context created by DVBT2_DemodCtxCreate.
 *  @retval   1 : I2C error.
 *  @retval   0 : I2C OK.   
 */
//-----------------------------------------------------------------------------
UINT8 DVBT2_SetNormalMode(DEMOD_CTX_T2 *psDemodCtx)
{
    return 0;
}

VOID DVBT2_SetTsOutput(DEMOD_CTX_T2 *psDemodCtx, BOOL fgOutputEnable)
{   
    #if defined(CC_MT5136)
    UCHAR ucData = 0;
	#endif

	#if defined(CC_MT5136)
    if(fgOutputEnable)
    {   
        DVBT2_GetReg(psDemodCtx, 0x8FB,  &ucData, 1);
	    ucData = (ucData & 0x30) >> 4;
	    if(ucData == 0x01) //48pin
	    { 
	    	DVBT2_GetReg(psDemodCtx, 0x8E1, &ucData, 1);
			ucData = ucData | 0xF0;
			DVBT2_SetReg(psDemodCtx, 0x8E1, &ucData, 1);
			ucData = 0xff;
			DVBT2_SetReg(psDemodCtx, 0x8E2, &ucData, 1);
        
			mcSHOW_DBG_MSG(("48pin ts output on \n"));
	    }
		else if(ucData == 0x02)  //128 pin
		{
			DVBT2_GetReg(psDemodCtx, 0x8E1, &ucData, 1);
			ucData = ucData | 0xF0;
			#if defined(CC_MT5398)
			ucData = ucData & 0xF0;//lei131212 for Cobra+MT5136 128pin demux can't receive packet issue
			#endif
			DVBT2_SetReg(psDemodCtx, 0x8E1, &ucData, 1);
			mcSHOW_DBG_MSG(("128pin ts output on \n"));
		}
		else
		{
		    mcSHOW_DBG_MSG(("Error, not 48pin and 128pin\n"));
        
		}

		//DVBT2
        ucData = 0x06;
        DVBT2_SetReg(psDemodCtx, 0x15d2, &ucData, 1);
		mcDELAY_MS(10);
        ucData = 0x05;
        DVBT2_SetReg(psDemodCtx, 0x15d2, &ucData, 1);
	
    }
    else
    {
        ucData = 0x06;
        DVBT2_SetReg(psDemodCtx, 0x15d2, &ucData, 1);
		mcDELAY_MS(10);
        ucData = 0x02;
        DVBT2_SetReg(psDemodCtx, 0x15d2, &ucData, 1);

	    DVBT2_GetReg(psDemodCtx, 0x8FB,  &ucData, 1);
	    ucData = (ucData & 0x30) >> 4;
	    if(ucData == 0x01) //48pin
	    { 
	    	DVBT2_GetReg(psDemodCtx, 0x8E1, &ucData, 1);
			ucData = ucData & 0x0F;
			DVBT2_SetReg(psDemodCtx, 0x8E1, &ucData, 1);
			ucData = 0;
			DVBT2_SetReg(psDemodCtx, 0x8E2, &ucData, 1);
			mcSHOW_DBG_MSG((" 48pin ts output disable\n"));
	    }
		else if(ucData == 0x02)  //128 pin
		{
			DVBT2_GetReg(psDemodCtx, 0x8E1, &ucData, 1);
			ucData = ucData & 0x0F;
			#if defined(CC_MT5398)
			ucData = ucData | 0x0F;//lei131212 for Cobra+MT5136 128pin demux can't receive packet issue
			#endif
			DVBT2_SetReg(psDemodCtx, 0x8E1, &ucData, 1);
			mcSHOW_DBG_MSG((" 128pin ts output disable\n"));
		}
		else
		{
		    mcSHOW_DBG_MSG(("Error, not 48pin and 128pin\n"));
        
		}
    }
	#endif
}

//----------------------------------------------------------------------------- 
/** DVBT2_SetTsPathMode
 *  Set TS path mode
 *  @param  fgEnable  set TS path to tri-state or pass-through. TRUE: pass-through, FALSE: tri-state
 *  @retval   void.
 */
//-----------------------------------------------------------------------------
UINT8 DVBT2_SetTsPathMode(DEMOD_CTX_T2 *psDemodCtx, BOOL fgEnable)
{
#if defined(CC_MT5136) || defined(CC_MT5135) || defined(CC_MT5135DVBT2)
    //UINT32 u4Temp;

	//Pluto
    return (UINT8) 0;
    #if 0
    SPI_SDARead(&u4Temp, 0x0001);
    if (fgEnable)
        u4Temp |= 0x000C0000;
    else
        u4Temp &= 0xFFF3FFFF;
    SPI_SDAWrite(&u4Temp, 0x0001);
	#endif
#elif defined(CC_MT5396) || defined(CC_MT5368) //lei_code_110513;
//add the code here future
#else
    UINT32 u4Temp;

    if ((DVBT2_GetSyncAfterAcq(psDemodCtx) == DVBT2_DEMOD_STATE_UNLOCK) && (fgEnable == TRUE)) {
        mcSHOW_DBG_MSG(("DVBT2_SetTsOutputMode : Can't enable TS due to demod unlock\n"));
        return DVBT2_DEMOD_STATE_UNLOCK;
    }

    u4Temp = mcRISC_Reg(CKGEN_BASE, 0x408);
    mcSHOW_DBG_MSG(("DVBT2_SetTsPathMode: fgEnable = %d\n", fgEnable));
    mcSHOW_DBG_MSG(("DVBT2_SetTsPathMode: read 0x408 = 0x%08X\n", u4Temp));

    if (fgEnable)
        u4Temp |= 0x00004000;
    else
        u4Temp &= 0xffffbfff;

    mcSHOW_DBG_MSG(("DVBT2_SetTsPathMode: write 0x408 = 0x%08X\n", u4Temp));
    mcSET_SHOW_REG(CKGEN_BASE, 0x408, u4Temp);
	return DVBT2_DEMOD_STATE_LOCK;
#endif
  //  return DVBT2_DEMOD_STATE_LOCK;
    
}



//----------------------------------------------------------------------------- 
/** download image

 *  @param  
 *  @param  
 *  @retval   others : Error.
 *  @retval   0 : OK.   
 */
//-----------------------------------------------------------------------------
UINT8 DVBT2_Loadcode(DEMOD_CTX_T2 *psDemodCtx, UINT8 *pu1Buffer, UINT16 u2CurRomLen)
{
    if(ICtrlBus_DemodLoadCode(psDemodCtx->I2cAddress, MT5136_REG_uP_IRData, pu1Buffer, u2CurRomLen))
    {
        /*fail case*/
        mcSHOW_POP_MSG(("Download Code fail!\n"));
        return 1;
    }

    return 0;
}

/***********************************************************************/
CHAR *DVBT2_GetVer(DEMOD_CTX_T2 *psDemodCtx)
{
    CHAR * cString1;

    vDVBT2_GetAsicVer(psDemodCtx);

    ITuner_OP(psDemodCtx->psTunerCtx,itGetVer,0,&cString1);
    mcSHOW_USER_MSG(("%s\n", cString1));

    return NULL;
}



/***********************************************************************/
#if 1

BOOL DVBT2_FwSwitch(DEMOD_CTX_T2 *psDemodCtx)
{

    UCHAR ucValue;
    UCHAR aucData[cMAX_READ_NUM + 1], ii;

/*
    UCHAR ucUpCtrl = 1;// Enable uP //EC_debug_dB506
    DVBT_SetReg(psDemodCtx, MT5391_REG_uP_Ctrl, &ucUpCtrl, 1);
    mcSHOW_DBG_MSG(("enable DVBT UP!\n"));    
    mcDELAY_MS(10);
*/
    //UCHAR ucData = 0x00;
    
    // check current code is DVBT/T2 or DVBC, if last time connect is not T/T2, need to down load FW and do TC switch again
    DVBT_GetReg(psDemodCtx, MT5391_REG_uP_GP_REG07, &ucValue, 1);
    mcSHOW_USER_MSG(("[MS DEBUG]DVBT2 0x00f is %d!\n", ucValue));
        
    if(0x02 != ucValue)
    {    
        DVBT2_DVBT_SetEEPromBrdcstType(psDemodCtx);
        if (s4LoadCode(psDemodCtx, 33, ucDvbt2CodeImage, sizeof(ucDvbt2CodeImage), cMAX_I2C_LEN, TRUE))
        {            
            return FALSE;
        }      

		#if 0
    	#if  (defined(CC_MT5136) && defined (CC_MT5881))
psDemodCtx->ucTsIfFormat = 0x88; // LL 0x8c to 0x8 for 5136+5881  //changed from 0x0C to 0x8C due to MT5360B, and no impact to MT539x and MT5360A
    psDemodCtx->ucTsIfCtrl = 0x00;
    psDemodCtx->ucTsIfRate = 0x07;
	DVBT_SetTsIf(psDemodCtx, psDemodCtx->ucTsIfFormat, psDemodCtx->ucTsIfCtrl, psDemodCtx->ucTsIfRate);// LL test
	DVBT2_SetTsIf(psDemodCtx, psDemodCtx->ucTsIfFormat, psDemodCtx->ucTsIfCtrl, psDemodCtx->ucTsIfRate);// LL test
	mcSHOW_USER_CRI_MSG(("[DVBT2_FwSwitch]DVBT_SetTsIf DVBT2_SetTsIf\n"));
#endif
		#endif
		

        mccSET_CMD2(aucData) = ccCMD_ID_DVBTC_SWITCH;
        mccSET_QUEUE2(aucData, ccOFST_DVBTC_SWITCH) = 0x02;//0x01:DVBC, 0x02: DVBT
        mcHOST_CMD(psDemodCtx, aucData, ccOFST_DVBTC_SWITCH_RSIZE, ccOFST_DVBTC_SWITCH_SIZE+1);

        // delay for waiting FW initialization done, may have I2C timeout at first
        for (ii = 0; ii < 5; ii++)
        {
            mcDELAY_MS(3);
            DVBT_GetReg(psDemodCtx, MT5391_REG_uP_GP_REG06, &ucValue, 1);
            if (ucValue == ccFW_RUN_TAG)
                break;
        }
        mcSHOW_DBG_MSG4(("FW switch to DVBT!"));
	
		//mcSHOW_DBG_MSG4(("DVBT2_T_T2_Init_Config(psDemodCtx)\n"));
		if(DVBT2_T_T2_Init_Config(psDemodCtx))
		{
			mcSHOW_USER_MSG(("psDemodCtx NULL OK!\n"));
		}

	}
    DVBT2_BW_Set(psDemodCtx);

	#if defined(CC_MT5396)|| defined(CC_MT5368)
    // RFAGC internal control by tuner, set RFAGC pin to be HiZ
    if(!psDemodCtx->psTunerCtx->fgRFTuner)//Sawless tuner
    {
		DVBT_GetReg(psDemodCtx, 0xf62, &ucData, 1);
	    mcSET_BIT(ucData, 3);
		mcSET_BIT(ucData, 2);
	    DVBT_SetReg(psDemodCtx, 0xf62, &ucData, 1);
    }
	#endif		
    // set agc parameters
    //DVBT_SetIF(psDemodCtx);     // set IF when FW switch


    return TRUE;
}
#endif

void   DVBT2_BW_Set(DEMOD_CTX_T2 *psDemodCtx)
{   
	UCHAR ucTmp  = 0;

	if (psDemodCtx->u1ChBw == MT5136T2_BW_5MHz)  // 5MHz bandwidth
	{
    	//0x14B3 = 0x21; //[7], P2 on/off, [6], sp on/off, [5] cp on/off (FW use correct SNR, ECO2 may modify)
    	ucTmp = 0x21;   MT5136_DVBT2_SetReg(psDemodCtx, 0x14B3, &ucTmp, 1);      
				
		// DJB Elementary Period Factor
		// 0x15B9 = 0x07
		ucTmp = 0x07;	MT5136_DVBT2_SetReg(psDemodCtx, 0x15B9, &ucTmp, 1);      
		// 0x15BA = 0x16
		ucTmp = 0x16;	MT5136_DVBT2_SetReg(psDemodCtx, 0x15BA, &ucTmp, 1);      
	
	}
	else if (psDemodCtx->u1ChBw == MT5136T2_BW_6MHz)  // 6MHz bandwidth
	{
    	//0x14B3 = 0xC1; //[7], P2 on/off, [6], sp on/off, [5] cp on/off (FW use correct SNR, ECO2 may modify)
    	ucTmp = 0xC1;   MT5136_DVBT2_SetReg(psDemodCtx, 0x14B3, &ucTmp, 1);      
        
		
		// DJB Elementary Period Factor
		// 0x15B9 = 0x05
		ucTmp = 0x05;	MT5136_DVBT2_SetReg(psDemodCtx, 0x15B9, &ucTmp, 1);      
		// 0x15BA = 0xEB
		ucTmp = 0xEB;	MT5136_DVBT2_SetReg(psDemodCtx, 0x15BA, &ucTmp, 1);      
	}
	else if (psDemodCtx->u1ChBw == MT5136T2_BW_7MHz)  // 7MHz bandwidth
	{
    	//0x14B3 = 0x21; //[7], P2 on/off, [6], sp on/off, [5] cp on/off (FW use correct SNR, ECO2 may modify)
    	ucTmp = 0x21;   MT5136_DVBT2_SetReg(psDemodCtx, 0x14B3, &ucTmp, 1);      
        		
		// DJB Elementary Period Factor
		// 0x15B9 = 0x05
		ucTmp = 0x05;	MT5136_DVBT2_SetReg(psDemodCtx, 0x15B9, &ucTmp, 1);      
		// 0x15BA = 0x10
		ucTmp = 0x10;	MT5136_DVBT2_SetReg(psDemodCtx, 0x15BA, &ucTmp, 1);      
	
	}
	else if (psDemodCtx->u1ChBw == MT5136T2_BW_8MHz)  // 8MHz bandwidth
	{
    	//0x14B3 = 0xC0; //[7], P2 on/off, [6], sp on/off, [5] cp on/off (FW use correct SNR, ECO2 may modify)
    	ucTmp = 0xC0;   MT5136_DVBT2_SetReg(psDemodCtx, 0x14B3, &ucTmp, 1);      
        
		
		// DJB Elementary Period Factor
		// 0x15B9 = 0x04
		ucTmp = 0x04;	MT5136_DVBT2_SetReg(psDemodCtx, 0x15B9, &ucTmp, 1);      
		// 0x15BA = 0x6E
		ucTmp = 0x6E;	MT5136_DVBT2_SetReg(psDemodCtx, 0x15BA, &ucTmp, 1);      
	}
	else if (psDemodCtx->u1ChBw == MT5136T2_BW_1p7MHz)  // 8MHz bandwidth
	{
    	//0x14B3 = 0xC0; //[7], P2 on/off, [6], sp on/off, [5] cp on/off (FW use correct SNR, ECO2 may modify)
    	ucTmp = 0xC0;   MT5136_DVBT2_SetReg(psDemodCtx, 0x14B3, &ucTmp, 1);      
        
		
		// DJB Elementary Period Factor
		// 0x15B9 = 0x04
		ucTmp = 0x15;	MT5136_DVBT2_SetReg(psDemodCtx, 0x15B9, &ucTmp, 1);      
		// 0x15BA = 0x6E
		ucTmp = 0xF3;	MT5136_DVBT2_SetReg(psDemodCtx, 0x15BA, &ucTmp, 1);      
	}


	if (psDemodCtx->u1ChBw == MT5136T2_BW_5MHz)  // 5MHz bandwidth
	{
    	//; DVB_FE change to bank 1 from FPGA1_1109
    	//; FE_INT ; 5M: 0x25CCCCCD 6M: 0x3F000000 (default), 7M: 0x36000000
    	//; D732
    	//;0x1320 = 0xCD
    	ucTmp = 0xCD;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1320, &ucTmp, 1);
    	//;0x1321 = 0xCC
    	ucTmp = 0xCC;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1321, &ucTmp, 1);
    	//;0x1322 = 0xCC
    	ucTmp = 0xCC;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1322, &ucTmp, 1);
    	//;0x1323 = 0x25
    	ucTmp = 0x25;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1323, &ucTmp, 1);

    	//;DC rate conversion 8M:0x56B, 7M: 0x4BE, 6M: 0x410 5M: 0x363
    	//;0x1304 = 0x63
    	ucTmp = 0x63;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1304, &ucTmp, 1);
    	//;0x1305 = 0x03
    	ucTmp = 0x03;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1305, &ucTmp, 1);
		//;0x1310 = 0x0a
    	ucTmp = 0x0a;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1310, &ucTmp, 1);
	}
	else if (psDemodCtx->u1ChBw == MT5136T2_BW_6MHz)  // 6MHz bandwidth
	{
    	//; DVB_FE change to bank 1 from FPGA1_1109
    	//; FE_INT ; 6M: 0x3F000000 (default), 7M: 0x36000000
    	//; D732
    	//;0x1320 = 0x00
    	ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1320, &ucTmp, 1);
    	//;0x1321 = 0x00
    	ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1321, &ucTmp, 1);
    	//;0x1322 = 0x00
    	ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1322, &ucTmp, 1);
    	//;0x1323 = 0x36
    	ucTmp = 0x3F;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1323, &ucTmp, 1);

    	//;DC rate conversion 8M:0x56B, 7M: 0x4BE, 6M: 0x410
    	//;0x1304 = 0xBE
    	ucTmp = 0x10;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1304, &ucTmp, 1);
    	//;0x1305 = 0x04
    	ucTmp = 0x04;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1305, &ucTmp, 1);
	}
	else if (psDemodCtx->u1ChBw == MT5136T2_BW_7MHz)  // 7MHz bandwidth
	{
    	//; DVB_FE change to bank 1 from FPGA1_1109
    	//; FE_INT ; 7M: 0x36000000 (default), 7M: 0x36000000
    	//; D732
    	//;0x1320 = 0x00
    	ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1320, &ucTmp, 1);
    	//;0x1321 = 0x00
    	ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1321, &ucTmp, 1);
    	//;0x1322 = 0x00
    	ucTmp = 0x00;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1322, &ucTmp, 1);
    	//;0x1323 = 0x36
    	ucTmp = 0x36;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1323, &ucTmp, 1);

    	//;DC rate conversion 8M:0x56B, 7M: 0x4BE
    	//;0x1304 = 0xBE
    	ucTmp = 0xBE;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1304, &ucTmp, 1);
    	//;0x1305 = 0x04
    	ucTmp = 0x04;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1305, &ucTmp, 1);
	}
	else if (psDemodCtx->u1ChBw == MT5136T2_BW_1p7MHz)  // 1.7MHz bandwidth
	{
    	//; DVB_FE change to bank 1 from FPGA1_1109
    	//; FE_INT ; 7M: 0x36000000 (default), 7M: 0x36000000
    	//; D732
    	//;0x1320 = 0x00
    	ucTmp = 0x3D;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1320, &ucTmp, 1);
    	//;0x1321 = 0x00
    	ucTmp = 0xCB;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1321, &ucTmp, 1);
    	//;0x1322 = 0x00
    	ucTmp = 0x88;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1322, &ucTmp, 1);
    	//;0x1323 = 0x36
    	ucTmp = 0x3A;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1323, &ucTmp, 1);

    	//;DC rate conversion 8M:0x56B, 7M: 0x4BE
    	//;0x1304 = 0xBE
    	ucTmp = 0x18;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1304, &ucTmp, 1);
    	//;0x1305 = 0x04
    	ucTmp = 0x01;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1305, &ucTmp, 1);

	ucTmp = 0x03;   MT5136_DVBT2_SetReg(psDemodCtx, 0x1310, &ucTmp, 1);
	}

}

/***********************************************************************/
/*              Public Functions                                       */
/***********************************************************************/
VOID vDVBT2_GetAsicVer(DEMOD_CTX_T2 *psDemodCtx)
{
	UCHAR	aucData[cMAX_READ_NUM + 1]; 		// For Host Cmd
    UCHAR	ucTemp;
    //mcSHOW_USER_CRI_MSG((REDYELLOW"vDVBT_GetAsicVer"DEFAULTCOLOR"\n"));
    mcSHOW_USER_CRI_MSG(("vDVBT2_GetAsicVer\n"));


    DVBT2_GetReg(psDemodCtx, 0x00, &ucTemp, 1);
    mcSHOW_USER_CRI_MSG(("0x00=0x%02x\n", ucTemp));

#if 1
    mccSET_CMD2(aucData) = ccCMD_ID_GET_VER;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_VER_SIZE, 1))
    {
        mcSHOW_USER_CRI_MSG(("FW build date: %02X%02X-%02X%02X\n",
                             mccGET_QUEUE(aucData, ccOFST_VER_YEAR_MONTH), mccGET_QUEUE(aucData, ccOFST_VER_DATE),
                             mccGET_QUEUE(aucData, ccOFST_VER_HOUR), mccGET_QUEUE(aucData, ccOFST_VER_MINUTE)));
    }
    else
    {
        mcSHOW_USER_CRI_MSG(("host cmd fail\n"));
    }

    //40732 ; add it for FW version control;
    mccSET_CMD2(aucData) = ccCMD_ID_GET_LABEL;
    if (mcHOST_CMD(psDemodCtx, aucData, ccOFST_LABEL_SIZE, 1))
    {
        mcSHOW_USER_CRI_MSG(("FW update ver: %c %02X%02X\n",
                             mccGET_QUEUE(aucData, ccOFST_LABEL_BRANCH_TYPE), mccGET_QUEUE(aucData, ccOFST_LABEL_ID1),
                             mccGET_QUEUE(aucData, ccOFST_LABEL_ID2)));
    }

    else
    {
        mcSHOW_USER_CRI_MSG(("host cmd fail\n"));
    }
#endif
}


void DVBT2_DVBT_SetEEPromBrdcstType(DEMOD_CTX_T2 *psDemodCtx)
{   
    #if defined(CC_MT5136)
	UINT8					u1Val = 0 ;
	UINT32					u4Ofst = 0;

	u4Ofst = DRVCUST_OptGet(eBrdcstSrcOfst0);
	u1Val = cDEMOD_DVBT2_MODE;
	VERIFY(0 == EEPROM_Write((UINT64)u4Ofst, (UINT32)(void *)&u1Val, 1));
	mcSHOW_USER_MSG(("[MS debug]DVBT2/T Set brdcst source  0x%0x = 0x%0x!\n",u4Ofst, u1Val));
	#endif

}

UINT8 DVBT2_DVBT_GetEEPromBrdcstType(DEMOD_CTX_T2 *psDemodCtx)
{   
	UINT8					u1Val = 0 ;
	#if defined(CC_MT5136)
	UINT32					u4Ofst = 0;

	u4Ofst = DRVCUST_OptGet(eBrdcstSrcOfst0);
	VERIFY(0 == EEPROM_Read((UINT64)u4Ofst, (UINT32)(void *)&u1Val, 1));
	mcSHOW_USER_MSG(("[MS debug]DVBT2/T Get brdcst source 0x%0x = 0x%0x!\n",u4Ofst, u1Val));
	#endif
	return u1Val;
}

void DVBT2_Patch_Demod_Rst(DEMOD_CTX_T2 *psDemodCtx, UCHAR fgRst)
{
    UCHAR   aucData[cMAX_READ_NUM + 1];

    mccSET_CMD2(aucData) = ccCMD_ID_PATCH_DEMOD_RST;
    mccSET_QUEUE2(aucData, ccOFST_PATCH_DEMOD_RST) = fgRst;   
    mcHOST_CMD(psDemodCtx, aucData, 0, ccOFST_PATCH_DEMOD_RST_TSIZE + 1);
}



UINT32 DVBT2_GetSQI3(DEMOD_CTX_T2 *psDemodCtx)
{
    UINT16 CN_NorDigP1_PP2[] =
    {
        35,
        47,
        56,
        66,
        72,
        77,
        87,
        101,
        114,
        125,
        133,
        138,
        130,
        148,
        162,
        177,
        187,
        194,
        170,
        194,
        208,
        229,
        243,
        251,
    };

	UINT16 CN_NorDigP1_PP4[] =
    {
        31,
        43,
        52,
        62,
        68,
        73,
        83,
        97,
        110,
        121,
        129,
        134,
        126,
        144,
        157,
        173,
        183,
        189,
        165,
        189,
        204,
        224,
        238,
        246,
    };
	UINT16 CN_NorDigP1_PP6[] =
    {
        26,
        38,
        47,
        57,
        63,
        68,
        78,
        92,
        105,
        116,
        124,
        129,
        121,
        139,
        153,
        168,
        178,
        184,
        161,
        184,
        199,
        219,
        233,
        241,
    };
	UINT16 CN_NorDigP1_PP7[] =
    {
        24,
        36,
        45,
        55,
        61,
        66,
        76,
        90,
        104,
        115,
        123,
        128,
        120,
        138,
        151,
        166,
        177,
        183,
        159,
        183,
        197,
        217,
        232,
        239,
    };
    UINT16 u2CN_NorDigP1 = 0;
    UINT16 u2CN_Rec = 0;
    INT16  i2CN_Rel = 0;
    INT32  i2Post_Viterbi_BER = 0;
    UINT32 u4BER_SQI = 0;
    UINT32 u4SQI_Indicator = 0;
 

    mcSHOW_DBG_MSG4(("DVBT2_GetSQI: PP=%d, Mode=%d, CR=%d\n", psDemodCtx->u1Pilot,psDemodCtx->u1PLP0MOD, psDemodCtx->u1PLP0CR));
    
    /* Get NorDigP1 Value */
	if(psDemodCtx->u1PLP0MOD >= DVBT2_Qam_Mode_Size)
	{	        
		mcSHOW_DBG_MSG4(("DVBT2_GetSQI: Unknown Mode"));
        return u4SQI_Indicator;
	}
	else
	{
		if(psDemodCtx->u1Pilot == 1)/* PP2 */
	    {
	    	//u2CN_NorDigP1 = CN_NorDigP1_PP2[(psDemodCtx->u1PLP0MOD)*DVBT2_CR_Mode_Size + psDemodCtx->u1PLP0CR];

			//Mingsheng mark it for fix clocwork 20120810;
	    	 if(psDemodCtx->u1PLP0MOD == 0)/*QPSK*/
	        {           
	            if(psDemodCtx->u1PLP0CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[0];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[1];
	            }
	            else if(psDemodCtx->u1PLP0CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[2];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[3];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[4];
	            }
				else if(psDemodCtx->u1PLP0CR == 5)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[5];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG4(("DVBT2_GetSQI in QPSK: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }       
	        else if(psDemodCtx->u1PLP0MOD == 1)/*16QAM*/
	        {           
	            if(psDemodCtx->u1PLP0CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[6];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[7];
	            }
	            else if(psDemodCtx->u1PLP0CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[8];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[9];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[10];
	            }
				else if(psDemodCtx->u1PLP0CR == 5)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[11];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG4(("DVBT2_GetSQI in 16QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }       
	        else if(psDemodCtx->u1PLP0MOD == 2)/* 64QAM */
	        {           
	            if(psDemodCtx->u1PLP0CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[12];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[13];
	            }
	            else if(psDemodCtx->u1PLP0CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[14];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[15];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[16];
	            }
				else if(psDemodCtx->u1PLP0CR == 5)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[17];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG4(("DVBT2_GetSQI in 16QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }
			else if(psDemodCtx->u1PLP0MOD == 3)/* 256QAM */
	        {           
	            if(psDemodCtx->u1PLP0CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[18];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[19];
	            }
	            else if(psDemodCtx->u1PLP0CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[20];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[21];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[22];
	            }
				else if(psDemodCtx->u1PLP0CR == 5)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[23];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG4(("DVBT2_GetSQI in 256QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }
		}       
	    else if(psDemodCtx->u1Pilot == 3)/* PP4 */
	    {           
			//u2CN_NorDigP1 = CN_NorDigP1_PP4[(psDemodCtx->u1PLP0MOD)*DVBT2_CR_Mode_Size + psDemodCtx->u1PLP0CR];
			if(psDemodCtx->u1PLP0MOD == 0)/*QPSK*/
	        {           
	            if(psDemodCtx->u1PLP0CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[0];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[1];
	            }
	            else if(psDemodCtx->u1PLP0CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[2];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[3];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[4];
	            }
				else if(psDemodCtx->u1PLP0CR == 5)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[5];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG4(("DVBT2_GetSQI in QPSK: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }       
	        else if(psDemodCtx->u1PLP0MOD == 1)/*16QAM*/
	        {           
	            if(psDemodCtx->u1PLP0CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[6];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[7];
	            }
	            else if(psDemodCtx->u1PLP0CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[8];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[9];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[10];
	            }
				else if(psDemodCtx->u1PLP0CR == 5)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[11];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG4(("DVBT2_GetSQI in 16QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }       
	        else if(psDemodCtx->u1PLP0MOD == 2)/* 64QAM */
	        {           
	            if(psDemodCtx->u1PLP0CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[12];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[13];
	            }
	            else if(psDemodCtx->u1PLP0CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[14];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[15];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[16];
	            }
				else if(psDemodCtx->u1PLP0CR == 5)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[17];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG4(("DVBT2_GetSQI in 16QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }
			else if(psDemodCtx->u1PLP0MOD == 3)/* 256QAM */
	        {           
	            if(psDemodCtx->u1PLP0CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[18];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[19];
	            }
	            else if(psDemodCtx->u1PLP0CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[20];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[21];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[22];
	            }
				else if(psDemodCtx->u1PLP0CR == 5)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[23];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG4(("DVBT2_GetSQI in 256QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }
	    }       
	    else if(psDemodCtx->u1Pilot == 5)/* PP6 */
	    {           
			//u2CN_NorDigP1 = CN_NorDigP1_PP6[(psDemodCtx->u1PLP0MOD)*DVBT2_CR_Mode_Size + psDemodCtx->u1PLP0CR];
			if(psDemodCtx->u1PLP0MOD == 0)/*QPSK*/
	        {           
	            if(psDemodCtx->u1PLP0CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[0];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[1];
	            }
	            else if(psDemodCtx->u1PLP0CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[2];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[3];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[4];
	            }
				else if(psDemodCtx->u1PLP0CR == 5)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[5];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG4(("DVBT2_GetSQI in QPSK: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }       
	        else if(psDemodCtx->u1PLP0MOD == 1)/*16QAM*/
	        {           
	            if(psDemodCtx->u1PLP0CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[6];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[7];
	            }
	            else if(psDemodCtx->u1PLP0CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[8];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[9];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[10];
	            }
				else if(psDemodCtx->u1PLP0CR == 5)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[11];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG4(("DVBT2_GetSQI in 16QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }       
	        else if(psDemodCtx->u1PLP0MOD == 2)/* 64QAM */
	        {           
	            if(psDemodCtx->u1PLP0CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[12];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[13];
	            }
	            else if(psDemodCtx->u1PLP0CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[14];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[15];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[16];
	            }
				else if(psDemodCtx->u1PLP0CR == 5)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[17];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG4(("DVBT2_GetSQI in 16QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }
			else if(psDemodCtx->u1PLP0MOD == 3)/* 256QAM */
	        {           
	            if(psDemodCtx->u1PLP0CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[18];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[19];
	            }
	            else if(psDemodCtx->u1PLP0CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[20];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[21];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[22];
	            }
				else if(psDemodCtx->u1PLP0CR == 5)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[23];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG4(("DVBT2_GetSQI in 256QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }
	    }
		else if(psDemodCtx->u1Pilot == 6)/* PP7 */
	    {           
			//u2CN_NorDigP1 = CN_NorDigP1_PP7[(psDemodCtx->u1PLP0MOD)*DVBT2_CR_Mode_Size + psDemodCtx->u1PLP0CR];
			if(psDemodCtx->u1PLP0MOD == 0)/*QPSK*/
	        {           
	            if(psDemodCtx->u1PLP0CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[0];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[1];
	            }
	            else if(psDemodCtx->u1PLP0CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[2];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[3];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[4];
	            }
				else if(psDemodCtx->u1PLP0CR == 5)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[5];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG4(("DVBT2_GetSQI in QPSK: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }       
	        else if(psDemodCtx->u1PLP0MOD == 1)/*16QAM*/
	        {           
	            if(psDemodCtx->u1PLP0CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[6];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[7];
	            }
	            else if(psDemodCtx->u1PLP0CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[8];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[9];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[10];
	            }
				else if(psDemodCtx->u1PLP0CR == 5)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[11];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG4(("DVBT2_GetSQI in 16QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }       
	        else if(psDemodCtx->u1PLP0MOD == 2)/* 64QAM */
	        {           
	            if(psDemodCtx->u1PLP0CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[12];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[13];
	            }
	            else if(psDemodCtx->u1PLP0CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[14];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[15];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[16];
	            }
				else if(psDemodCtx->u1PLP0CR == 5)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[17];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG4(("DVBT2_GetSQI in 16QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }
			else if(psDemodCtx->u1PLP0MOD == 3)/* 256QAM */
	        {           
	            if(psDemodCtx->u1PLP0CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[18];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[19];
	            }
	            else if(psDemodCtx->u1PLP0CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[20];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[21];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[22];
	            }
				else if(psDemodCtx->u1PLP0CR == 5)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[23];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG4(("DVBT2_GetSQI in 256QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }
	    }
	    else/* PP4 */
	    {     
			//u2CN_NorDigP1 = CN_NorDigP1_PP4[(psDemodCtx->u1PLP0MOD)*DVBT2_CR_Mode_Size + psDemodCtx->u1PLP0CR];
			if(psDemodCtx->u1PLP0MOD == 0)/*QPSK*/
	        {           
	            if(psDemodCtx->u1PLP0CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[0];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[1];
	            }
	            else if(psDemodCtx->u1PLP0CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[2];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[3];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[4];
	            }
				else if(psDemodCtx->u1PLP0CR == 5)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[5];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG4(("DVBT2_GetSQI in QPSK: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }       
	        else if(psDemodCtx->u1PLP0MOD == 1)/*16QAM*/
	        {           
	            if(psDemodCtx->u1PLP0CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[6];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[7];
	            }
	            else if(psDemodCtx->u1PLP0CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[8];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[9];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[10];
	            }
				else if(psDemodCtx->u1PLP0CR == 5)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[11];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG4(("DVBT2_GetSQI in 16QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }       
	        else if(psDemodCtx->u1PLP0MOD == 2)/* 64QAM */
	        {           
	            if(psDemodCtx->u1PLP0CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[12];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[13];
	            }
	            else if(psDemodCtx->u1PLP0CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[14];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[15];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[16];
	            }
				else if(psDemodCtx->u1PLP0CR == 5)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[17];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG4(("DVBT2_GetSQI in 16QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }
			else if(psDemodCtx->u1PLP0MOD == 3)/* 256QAM */
	        {           
	            if(psDemodCtx->u1PLP0CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[18];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[19];
	            }
	            else if(psDemodCtx->u1PLP0CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[20];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[21];
	            }           
	            else if(psDemodCtx->u1PLP0CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[22];
	            }
				else if(psDemodCtx->u1PLP0CR == 5)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[23];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG4(("DVBT2_GetSQI in 256QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }
	    }	
	}


    /* Get CN_rec Value */      
	mcSHOW_DBG_MSG4(("[DVBT2_GetSQI]@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@SSSSSSS\n"));
    u2CN_Rec = DVBT2_GetSignalSNR(psDemodCtx);
	mcSHOW_DBG_MSG4(("[DVBT2_GetSQI]u2CN_Rec=%d\n",u2CN_Rec));
	#if 0
    if(u2CN_Rec > 0)
    {
      u2CN_Rec = u2CN_Rec - psDemodCtx->s2snr_cc;
    }
	#endif
    /* Get Post_BER Value */            
    i2Post_Viterbi_BER = DVBT2_GetPstLDPCBERSQI(psDemodCtx)/100;//*10000; //BER*10^7
	mcSHOW_DBG_MSG4(("[DVBT2_GetSQI]i2Post_Viterbi_BER=%d\n",i2Post_Viterbi_BER));
    /* Get CN_Rel Value */
    i2CN_Rel = u2CN_Rec - u2CN_NorDigP1;
	mcSHOW_DBG_MSG4(("[DVBT2_GetSQI]i2CN_Rel: %d = %d - %d\n",i2CN_Rel,u2CN_Rec,u2CN_NorDigP1));
    /* Get BER_SQI Value */
    if(i2Post_Viterbi_BER > 1000) //10^-4 / 
    {
        u4BER_SQI = 0;
		mcSHOW_DBG_MSG4(("[DVBT2_GetSQI]i2Post_Viterbi_BER>1000=%d,u4BER_SQI=%d\n",i2Post_Viterbi_BER,u4BER_SQI));
    }
    else if((i2Post_Viterbi_BER >= 1) && (i2Post_Viterbi_BER <= 1000))
    {
        //u4BER_SQI = (UINT32)((20*DVBT2_LOG10((UINT32)(100000/i2Post_Viterbi_BER))-4000));  
        u4BER_SQI = 100/15;
		mcSHOW_DBG_MSG4(("[DVBT2_GetSQI]0<i2Post_Viterbi_BER<1000=%d,u4BER_SQI=%d\n",i2Post_Viterbi_BER,u4BER_SQI));
    }
    else if(i2Post_Viterbi_BER < 1)
    {
        //u4BER_SQI = 100*100;
        u4BER_SQI=100/6;
	  mcSHOW_DBG_MSG4(("[DVBT2_GetSQI]i2Post_Viterbi_BER<0=%d,u4BER_SQI=%d\n",i2Post_Viterbi_BER,u4BER_SQI));
    }
    /* Cal SQI Indicator*/
    if((i2CN_Rel) < -30)
    {
        u4SQI_Indicator = 0;
		mcSHOW_DBG_MSG4(("[DVBT2_GetSQI](i2CN_Rel) < -30,i2CN_Rel=%d,u4SQI_Indicator=%d,u4BER_SQI=%d\n",i2CN_Rel,u4SQI_Indicator,u4BER_SQI));
    }
    else if(((i2CN_Rel) >= -30) && ((i2CN_Rel) <= 30))
    {
        //u4SQI_Indicator = (i2CN_Rel+30)*u4BER_SQI/10;
		//mcSHOW_DBG_MSG(("[DVBT2_GetSQI]-300<=i2CN_Rel<=300,u4SQI_Indicator=%d\n",u4SQI_Indicator));
		if(i2Post_Viterbi_BER > 1000)
		{
			u4SQI_Indicator = (i2CN_Rel+30)*0;
			mcSHOW_DBG_MSG4(("[DVBT2_GetSQI]BER=%d>1000,-30<=i2CN_Rel<=30,i2CN_Rel=%d,u4SQI_Indicator=%d,u4BER_SQI=%d\n",i2Post_Viterbi_BER,i2CN_Rel,u4SQI_Indicator,u4BER_SQI));
		}
		else if((i2Post_Viterbi_BER >= 1) && (i2Post_Viterbi_BER <= 1000))
		{
			u4SQI_Indicator = (i2CN_Rel+30)*100/15/10;
			mcSHOW_DBG_MSG4(("[DVBT2_GetSQI]1=<BER=%d,i2CN_Rel<1000,-30<=i2CN_Rel<=30,=%d,u4SQI_Indicator=%d,u4BER_SQI=%d\n",i2Post_Viterbi_BER,i2CN_Rel,u4SQI_Indicator,u4BER_SQI));
		}
		else if(i2Post_Viterbi_BER < 1)
		{
			u4SQI_Indicator = (i2CN_Rel+30)*100/6/10;
			mcSHOW_DBG_MSG4(("[DVBT2_GetSQI]BER=%d<1,-30=<i2CN_Rel<=30,i2CN_Rel=%d,u4SQI_Indicator=%d,u4BER_SQI=%d\n",i2Post_Viterbi_BER,i2CN_Rel,u4SQI_Indicator,u4BER_SQI));
		}
		//mcSHOW_DBG_MSG(("[DVBT2_GetSQI]-300<=i2CN_Rel<=300,i2CN_Rel=%d,u4SQI_Indicator=%d,u4BER_SQI=%d\n",i2CN_Rel,u4SQI_Indicator,u4BER_SQI));
    }
    else if((i2CN_Rel) > 30)
    {        
      u4SQI_Indicator=100;
	  mcSHOW_DBG_MSG4(("[DVBT2_GetSQI](i2CN_Rel) >= 300,i2CN_Rel=%d,u4SQI_Indicator=%d,u4BER_SQI=%d\n",i2CN_Rel,u4SQI_Indicator,u4BER_SQI));
    }

    mcSHOW_DBG_MSG4(("DVBT2_GetSQI: CN_NorDigP1=%d, CN_Rec=%d, BER=%d, SQI=%d\n", u2CN_NorDigP1, u2CN_Rec, i2Post_Viterbi_BER, u4SQI_Indicator));
	
    mcSHOW_DBG_MSG4(("[DVBT2_GetSQI]:i2CN_Rel=%d/10,CN_NorDigP1=%d/10, CN_Rec=%d/10, BER=%d*10^-7,BER_SQI=%d,SQI=%d\n",i2CN_Rel, u2CN_NorDigP1, u2CN_Rec, i2Post_Viterbi_BER, u4BER_SQI,u4SQI_Indicator));
    mcSHOW_DBG_MSG4(("[DVBT2_GetSQI]@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@EEEEEEEEEEEEE\n"));
    return u4SQI_Indicator;
}


UINT32 DVBT2_GetSQI2(DEMOD_CTX_T2 *psDemodCtx)
{

UINT16 min_cn[4][6]={
	  {22,32,39,50,55,60},
	  {70,84,96,108,115,119},
	  {111,131,144,158,170,176},
	  {153,170,190,204,220,230}
  };
UINT16 max_cn[4][6]={
	  {30,41,49,57,60,69},
	  {75,92,105,115,123,128},
	  {121,140,150,164,175,182},
	  {158,185,194,215,226,240}
  };

INT32 i2_ber = 0;
UINT32 u4_sqi = 0;
UINT32 u2_sqi_tmp = 0;

UINT16 u2_snr = 0;
UINT32 u4_tmp = 0;
UINT16 u2_min_cn =0;
  UINT16 u2_max_cn=0;
//  UINT8 u1_mod = psDemodCtx->u1PLP0MOD;//u1Mod; //build warning:usigned char can't be smaller than 0
//  UINT8 u1_cr = psDemodCtx->u1PLP0CR;//u1CR;
INT16  u1_mod = psDemodCtx->u1PLP0MOD;//u1Mod;
INT16  u1_cr = psDemodCtx->u1PLP0CR;//u1CR;


if( (u1_mod<=3) || (u1_mod>=0) || (u1_cr >=0) || (u1_cr<=5) )
{
  u2_min_cn = min_cn[u1_mod][u1_cr];
  u2_max_cn = max_cn[u1_mod][u1_cr]-2;
  mcSHOW_USER_CRI_MSG(("@@@@ u1mod=%d,u1cr=%d, min_cn=%d, max_cn=%d,\n",u1_mod,u1_cr,u2_min_cn, u2_max_cn));
}

u2_snr = DVBT2_GetSignalSNR(psDemodCtx);
mcSHOW_USER_CRI_MSG(("@@@ from DVBT2_GetSignalSNR  u2_snr=%d,\n",u2_snr));


if(u2_snr >= u2_max_cn)
{
u2_sqi_tmp = 100;
}
else if(u2_snr <= u2_min_cn)
{
u2_sqi_tmp = 0;
}
else
{
u2_sqi_tmp =(UINT32) ( (u2_snr-u2_min_cn)*100/(u2_max_cn-u2_min_cn) );
}

i2_ber = DVBT2_GetPstLDPCBER(psDemodCtx);
mcSHOW_USER_CRI_MSG(("@@@ @@@@from DVBT2_GetPstLDPCBER  i2_ber=%d,\n",i2_ber));

//mcSHOW_USER_CRI_MSG(("u2_snr=%d, i2_ber=%d\n",u2_snr,i2_ber));

//u2_snr = (u2_snr+350)*100/950;// snr 0~100
if( (i2_ber < 3000) && (i2_ber >=0))				// ber 0~100
{
	u4_tmp = 0;
	mcSHOW_USER_CRI_MSG(("  0 =<i2_ber< 3000,topber=%d\n",psDemodCtx->i2_topBer));
}
else if( i2_ber > (psDemodCtx->i2_topBer))
//else if( i2_ber > 133000 )
{
	u4_tmp = 100;
	mcSHOW_USER_CRI_MSG((" i2_ber > %d,\n",psDemodCtx->i2_topBer));

}
else if( (i2_ber >=3000) && (i2_ber <= psDemodCtx->i2_topBer))
{
	u4_tmp= (i2_ber-3000)/1300 ;
	mcSHOW_USER_CRI_MSG((" u4_tmp= (i2_ber-3000)/1300 ;,topber=%d\n",psDemodCtx->i2_topBer));
}
else
{
    u4_sqi = 0;
	mcSHOW_USER_CRI_MSG(("@@@@@@@@@@@i2_ber < 0\n"));
	return u4_sqi;
}

u4_sqi = (u2_sqi_tmp*psDemodCtx->u1_wSnr + (100-u4_tmp)*psDemodCtx->u1_wBer)/100;

//if( i2_ber < 0)// if ts unlock , ber=-1
//{
//	u4_sqi = 0;
//	mcSHOW_USER_CRI_MSG(("@@@@@@@@@@@i2_ber < 0\n"));
//}
mcSHOW_USER_CRI_MSG(("@@@@@@@@@@@@u2_sqi_tmp=%d,u4_tmp = %d, i2_ber=%d,u4_sqi=%d\n",u2_sqi_tmp,u4_tmp,i2_ber,u4_sqi));
mcSHOW_USER_CRI_MSG(("************************ ws=%d,wb=%d,topber=%d,\n",psDemodCtx->u1_wSnr,psDemodCtx->u1_wBer,psDemodCtx->i2_topBer));

return u4_sqi;
}


//----------------------------------------------------------------------------- 
/*
 *  DVBT_GetSQI
 *  calculate Signal Quality Indicator.
 *  @param  psDemodCtx  Pointer of context created by DVBT_DemodCtxCreate.
 *  @retval   
 */
//-----------------------------------------------------------------------------
#if 1
UINT32 DVBT2_GetSQI(DEMOD_CTX_T2 *psDemodCtx)
{
    UINT16 CN_NorDigP1_PP2[] =
    {
        35,
        47,
        56,
        66,
        72,
        77,
        87,
        101,
        114,
        125,
        133,
        138,
        130,
        148,
        162,
        177,
        187,
        194,
        170,
        194,
        208,
        229,
        243,
        251,
    };

	UINT16 CN_NorDigP1_PP4[] =
    {
        31,
        43,
        52,
        62,
        68,
        73,
        83,
        97,
        110,
        121,
        129,
        134,
        126,
        144,
        157,
        173,
        183,
        189,
        165,
        189,
        204,
        224,
        238,
        246,
    };
	UINT16 CN_NorDigP1_PP6[] =
    {
        26,
        38,
        47,
        57,
        63,
        68,
        78,
        92,
        105,
        116,
        124,
        129,
        121,
        139,
        153,
        168,
        178,
        184,
        161,
        184,
        199,
        219,
        233,
        241,
    };
	UINT16 CN_NorDigP1_PP7[] =
    {
        24,
        36,
        45,
        55,
        61,
        66,
        76,
        90,
        104,
        115,
        123,
        128,
        120,
        138,
        151,
        166,
        177,
        183,
        159,
        183,
        197,
        217,
        232,
        239,
    };
    UINT16 u2CN_NorDigP1 = 0;
    UINT16 u2CN_Rec = 0;
    INT16  i2CN_Rel = 0;
    INT32  i2Post_Viterbi_BER = 0;
    UINT32 u4BER_SQI = 0;
    UINT32 u4SQI_Indicator = 0;


    mcSHOW_DBG_MSG(("DVBT2_GetSQI: PP=%d, Mode=%d, CR=%d\n", psDemodCtx->u1Pilot,psDemodCtx->u1PLP0MOD, psDemodCtx->u1PLP0CR));
    
    /* Get NorDigP1 Value */
	if(psDemodCtx->u1PLP0MOD >= DVBT2_Qam_Mode_Size)
	{	        
		mcSHOW_DBG_MSG(("DVBT2_GetSQI: Unknown Mode"));
        return u4SQI_Indicator;
	}
	else
	{
		if(psDemodCtx->u1Pilot == 1)/* PP2 */
	    {
	    	//u2CN_NorDigP1 = CN_NorDigP1_PP2[(psDemodCtx->u1PLP0MOD)*DVBT2_CR_Mode_Size + psDemodCtx->u1PLP0CR];

			//Mingsheng mark it for fix clocwork 20120810;
	    	 if(psDemodCtx->u1Mod == 0)/*QPSK*/
	        {           
	            if(psDemodCtx->u1CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[0];
	            }           
	            else if(psDemodCtx->u1CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[1];
	            }
	            else if(psDemodCtx->u1CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[2];
	            }           
	            else if(psDemodCtx->u1CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[3];
	            }           
	            else if(psDemodCtx->u1CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[4];
	            }
				else if(psDemodCtx->u1CR == 4)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[5];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG(("DVBT2_GetSQI in QPSK: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }       
	        else if(psDemodCtx->u1Mod == 1)/*16QAM*/
	        {           
	            if(psDemodCtx->u1CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[6];
	            }           
	            else if(psDemodCtx->u1CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[7];
	            }
	            else if(psDemodCtx->u1CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[8];
	            }           
	            else if(psDemodCtx->u1CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[9];
	            }           
	            else if(psDemodCtx->u1CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[10];
	            }
				else if(psDemodCtx->u1CR == 4)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[11];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG(("DVBT2_GetSQI in 16QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }       
	        else if(psDemodCtx->u1Mod == 2)/* 64QAM */
	        {           
	            if(psDemodCtx->u1CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[12];
	            }           
	            else if(psDemodCtx->u1CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[13];
	            }
	            else if(psDemodCtx->u1CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[14];
	            }           
	            else if(psDemodCtx->u1CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[15];
	            }           
	            else if(psDemodCtx->u1CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[16];
	            }
				else if(psDemodCtx->u1CR == 4)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[17];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG(("DVBT2_GetSQI in 16QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }
			else if(psDemodCtx->u1Mod == 3)/* 256QAM */
	        {           
	            if(psDemodCtx->u1CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[18];
	            }           
	            else if(psDemodCtx->u1CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[19];
	            }
	            else if(psDemodCtx->u1CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[20];
	            }           
	            else if(psDemodCtx->u1CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[21];
	            }           
	            else if(psDemodCtx->u1CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[22];
	            }
				else if(psDemodCtx->u1CR == 4)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[23];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG(("DVBT2_GetSQI in 256QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }
		}       
	    else if(psDemodCtx->u1Pilot == 3)/* PP4 */
	    {           
			//u2CN_NorDigP1 = CN_NorDigP1_PP4[(psDemodCtx->u1PLP0MOD)*DVBT2_CR_Mode_Size + psDemodCtx->u1PLP0CR];
			if(psDemodCtx->u1Mod == 0)/*QPSK*/
	        {           
	            if(psDemodCtx->u1CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[0];
	            }           
	            else if(psDemodCtx->u1CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[1];
	            }
	            else if(psDemodCtx->u1CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[2];
	            }           
	            else if(psDemodCtx->u1CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[3];
	            }           
	            else if(psDemodCtx->u1CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[4];
	            }
				else if(psDemodCtx->u1CR == 4)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[5];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG(("DVBT2_GetSQI in QPSK: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }       
	        else if(psDemodCtx->u1Mod == 1)/*16QAM*/
	        {           
	            if(psDemodCtx->u1CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[6];
	            }           
	            else if(psDemodCtx->u1CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[7];
	            }
	            else if(psDemodCtx->u1CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[8];
	            }           
	            else if(psDemodCtx->u1CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[9];
	            }           
	            else if(psDemodCtx->u1CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[10];
	            }
				else if(psDemodCtx->u1CR == 4)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[11];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG(("DVBT2_GetSQI in 16QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }       
	        else if(psDemodCtx->u1Mod == 2)/* 64QAM */
	        {           
	            if(psDemodCtx->u1CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[12];
	            }           
	            else if(psDemodCtx->u1CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[13];
	            }
	            else if(psDemodCtx->u1CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[14];
	            }           
	            else if(psDemodCtx->u1CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP2[15];
	            }           
	            else if(psDemodCtx->u1CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[16];
	            }
				else if(psDemodCtx->u1CR == 4)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[17];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG(("DVBT2_GetSQI in 16QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }
			else if(psDemodCtx->u1Mod == 3)/* 256QAM */
	        {           
	            if(psDemodCtx->u1CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[18];
	            }           
	            else if(psDemodCtx->u1CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[19];
	            }
	            else if(psDemodCtx->u1CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[20];
	            }           
	            else if(psDemodCtx->u1CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[21];
	            }           
	            else if(psDemodCtx->u1CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[22];
	            }
				else if(psDemodCtx->u1CR == 4)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[23];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG(("DVBT2_GetSQI in 256QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }
	    }       
	    else if(psDemodCtx->u1Pilot == 5)/* PP6 */
	    {           
			//u2CN_NorDigP1 = CN_NorDigP1_PP6[(psDemodCtx->u1PLP0MOD)*DVBT2_CR_Mode_Size + psDemodCtx->u1PLP0CR];
			if(psDemodCtx->u1Mod == 0)/*QPSK*/
	        {           
	            if(psDemodCtx->u1CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[0];
	            }           
	            else if(psDemodCtx->u1CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[1];
	            }
	            else if(psDemodCtx->u1CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[2];
	            }           
	            else if(psDemodCtx->u1CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[3];
	            }           
	            else if(psDemodCtx->u1CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[4];
	            }
				else if(psDemodCtx->u1CR == 4)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[5];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG(("DVBT2_GetSQI in QPSK: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }       
	        else if(psDemodCtx->u1Mod == 1)/*16QAM*/
	        {           
	            if(psDemodCtx->u1CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[6];
	            }           
	            else if(psDemodCtx->u1CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[7];
	            }
	            else if(psDemodCtx->u1CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[8];
	            }           
	            else if(psDemodCtx->u1CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[9];
	            }           
	            else if(psDemodCtx->u1CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[10];
	            }
				else if(psDemodCtx->u1CR == 4)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[11];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG(("DVBT2_GetSQI in 16QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }       
	        else if(psDemodCtx->u1Mod == 2)/* 64QAM */
	        {           
	            if(psDemodCtx->u1CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[12];
	            }           
	            else if(psDemodCtx->u1CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[13];
	            }
	            else if(psDemodCtx->u1CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[14];
	            }           
	            else if(psDemodCtx->u1CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[15];
	            }           
	            else if(psDemodCtx->u1CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[16];
	            }
				else if(psDemodCtx->u1CR == 4)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[17];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG(("DVBT2_GetSQI in 16QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }
			else if(psDemodCtx->u1Mod == 3)/* 256QAM */
	        {           
	            if(psDemodCtx->u1CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[18];
	            }           
	            else if(psDemodCtx->u1CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[19];
	            }
	            else if(psDemodCtx->u1CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[20];
	            }           
	            else if(psDemodCtx->u1CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[21];
	            }           
	            else if(psDemodCtx->u1CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[22];
	            }
				else if(psDemodCtx->u1CR == 4)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP6[23];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG(("DVBT2_GetSQI in 256QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }
	    }
		else if(psDemodCtx->u1Pilot == 6)/* PP7 */
	    {           
			//u2CN_NorDigP1 = CN_NorDigP1_PP7[(psDemodCtx->u1PLP0MOD)*DVBT2_CR_Mode_Size + psDemodCtx->u1PLP0CR];
			if(psDemodCtx->u1Mod == 0)/*QPSK*/
	        {           
	            if(psDemodCtx->u1CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[0];
	            }           
	            else if(psDemodCtx->u1CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[1];
	            }
	            else if(psDemodCtx->u1CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[2];
	            }           
	            else if(psDemodCtx->u1CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[3];
	            }           
	            else if(psDemodCtx->u1CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[4];
	            }
				else if(psDemodCtx->u1CR == 4)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[5];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG(("DVBT2_GetSQI in QPSK: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }       
	        else if(psDemodCtx->u1Mod == 1)/*16QAM*/
	        {           
	            if(psDemodCtx->u1CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[6];
	            }           
	            else if(psDemodCtx->u1CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[7];
	            }
	            else if(psDemodCtx->u1CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[8];
	            }           
	            else if(psDemodCtx->u1CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[9];
	            }           
	            else if(psDemodCtx->u1CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[10];
	            }
				else if(psDemodCtx->u1CR == 4)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[11];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG(("DVBT2_GetSQI in 16QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }       
	        else if(psDemodCtx->u1Mod == 2)/* 64QAM */
	        {           
	            if(psDemodCtx->u1CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[12];
	            }           
	            else if(psDemodCtx->u1CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[13];
	            }
	            else if(psDemodCtx->u1CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[14];
	            }           
	            else if(psDemodCtx->u1CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[15];
	            }           
	            else if(psDemodCtx->u1CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[16];
	            }
				else if(psDemodCtx->u1CR == 4)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[17];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG(("DVBT2_GetSQI in 16QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }
			else if(psDemodCtx->u1Mod == 3)/* 256QAM */
	        {           
	            if(psDemodCtx->u1CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[18];
	            }           
	            else if(psDemodCtx->u1CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[19];
	            }
	            else if(psDemodCtx->u1CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[20];
	            }           
	            else if(psDemodCtx->u1CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[21];
	            }           
	            else if(psDemodCtx->u1CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[22];
	            }
				else if(psDemodCtx->u1CR == 4)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP7[23];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG(("DVBT2_GetSQI in 256QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }
	    }
	    else/* PP4 */
	    {     
			//u2CN_NorDigP1 = CN_NorDigP1_PP4[(psDemodCtx->u1PLP0MOD)*DVBT2_CR_Mode_Size + psDemodCtx->u1PLP0CR];
			if(psDemodCtx->u1Mod == 0)/*QPSK*/
	        {           
	            if(psDemodCtx->u1CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[0];
	            }           
	            else if(psDemodCtx->u1CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[1];
	            }
	            else if(psDemodCtx->u1CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[2];
	            }           
	            else if(psDemodCtx->u1CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[3];
	            }           
	            else if(psDemodCtx->u1CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[4];
	            }
				else if(psDemodCtx->u1CR == 4)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[5];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG(("DVBT2_GetSQI in QPSK: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }       
	        else if(psDemodCtx->u1Mod == 1)/*16QAM*/
	        {           
	            if(psDemodCtx->u1CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[6];
	            }           
	            else if(psDemodCtx->u1CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[7];
	            }
	            else if(psDemodCtx->u1CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[8];
	            }           
	            else if(psDemodCtx->u1CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[9];
	            }           
	            else if(psDemodCtx->u1CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[10];
	            }
				else if(psDemodCtx->u1CR == 4)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[11];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG(("DVBT2_GetSQI in 16QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }       
	        else if(psDemodCtx->u1Mod == 2)/* 64QAM */
	        {           
	            if(psDemodCtx->u1CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[12];
	            }           
	            else if(psDemodCtx->u1CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[13];
	            }
	            else if(psDemodCtx->u1CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[14];
	            }           
	            else if(psDemodCtx->u1CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[15];
	            }           
	            else if(psDemodCtx->u1CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[16];
	            }
				else if(psDemodCtx->u1CR == 4)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[17];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG(("DVBT2_GetSQI in 16QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }
			else if(psDemodCtx->u1Mod == 3)/* 256QAM */
	        {           
	            if(psDemodCtx->u1CR == 0)/* Code Rate == 1/2*/  
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[18];
	            }           
	            else if(psDemodCtx->u1CR == 1)/* Code Rate == 3/5*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[19];
	            }
	            else if(psDemodCtx->u1CR == 2)/* Code Rate == 2/3*/ 
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[20];
	            }           
	            else if(psDemodCtx->u1CR == 3)/* Code Rate == 3/4*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[21];
	            }           
	            else if(psDemodCtx->u1CR == 4)/* Code Rate == 4/5*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[22];
	            }
				else if(psDemodCtx->u1CR == 4)/* Code Rate == 5/6*/
	            {
	                u2CN_NorDigP1 = CN_NorDigP1_PP4[23];
	            }
	            else
	            {
	                mcSHOW_DBG_MSG(("DVBT2_GetSQI in 256QAM: Unknown CR "));
	                return u4SQI_Indicator;
	            }
	        }
	    }	
	}


    /* Get CN_rec Value */      
    u2CN_Rec = DVBT2_GetSignalSNR(psDemodCtx);
    if(u2CN_Rec > 0)
    {
      u2CN_Rec = u2CN_Rec - psDemodCtx->s2snr_cc;
    }
	
    /* Get Post_BER Value */            
    i2Post_Viterbi_BER = DVBT2_GetPstLDPCBER(psDemodCtx)*10000;
    /* Get CN_Rel Value */
    i2CN_Rel = u2CN_Rec - u2CN_NorDigP1;
    /* Get BER_SQI Value */
    if(i2Post_Viterbi_BER > 100)
    {
        u4BER_SQI = 0;
    }
    else if((i2Post_Viterbi_BER > 0) && (i2Post_Viterbi_BER <= 100))
    {
        u4BER_SQI = (UINT32)((20*DVBT2_LOG10((UINT32)(100000/i2Post_Viterbi_BER))-4000));                        
    }
    else
    {
        u4BER_SQI = 100*100;
    }
    /* Cal SQI Indicator*/
    if((i2CN_Rel) < -70)
    {
        u4SQI_Indicator = 0;
    }
    else if(((i2CN_Rel) >= -70) && ((i2CN_Rel) < 30))
    {
        u4SQI_Indicator = (UINT32)(((i2CN_Rel + 70) * u4BER_SQI)/10000);
    }
    else if((i2CN_Rel) >= 30)
    {
        u4SQI_Indicator = (UINT32)(u4BER_SQI/100);
    }

    mcSHOW_DBG_MSG(("DVBT2_GetSQI: CN_NorDigP1=%d, CN_Rec=%d, BER=%d, SQI=%d\n", u2CN_NorDigP1, u2CN_Rec, i2Post_Viterbi_BER, u4SQI_Indicator));
       
    return u4SQI_Indicator;
}

#endif



