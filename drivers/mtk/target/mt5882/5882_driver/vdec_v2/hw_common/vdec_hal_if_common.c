/********************************************************************************************
 *     LEGAL DISCLAIMER 
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES 
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED 
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS 
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, 
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY 
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, 
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK 
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION 
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *     
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH 
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, 
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE 
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
 *     
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS 
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.  
 ************************************************************************************************/
#include "vdec_hal_if_common.h"
#include "vdec_hw_common.h"
#include "vdec_hal_errcode.h"
#include "../vdec_debug.h"

#define VDEC_VLD_USE_USB  0
// **************************************************************************
// Function : INT32 i4VDEC_HAL_Common_Init(UINT32 u4ChipID);
// Description : Turns on video decoder HAL
// Parameter : u4ChipID
// Return      : >0: init OK.
//                  <0: init failed
// **************************************************************************
INT32 i4VDEC_HAL_Common_Init(UINT32 u4ChipID)
{
#if (!CONFIG_DRV_FPGA_BOARD)

    // reset common SRAM
    vVDecWriteVLD(0, RW_VLD_RESET_COMMOM_SRAM, 0x00000000);
    vVDecWriteVLD(0, RW_VLD_RESET_COMMOM_SRAM, 0x00000100);
    vVDecWriteVLD(0, RW_VLD_RESET_COMMOM_SRAM, 0x00000000);
    vVDecWriteVLD(1, RW_VLD_RESET_COMMOM_SRAM, 0x00000000);
    vVDecWriteVLD(1, RW_VLD_RESET_COMMOM_SRAM, 0x00000100);
    vVDecWriteVLD(1, RW_VLD_RESET_COMMOM_SRAM, 0x00000000);
#endif

    return VDEC_HAL_HANDLE_OK;
}

#if (!CONFIG_DRV_FPGA_BOARD)
void vVDEC_HAL_CLK_Set(UINT32 u4CodeType)
{
    vVDecSetVldMcClk(0,u4CodeType);
}
#endif



// **************************************************************************
// Function : void vDEC_HAL_COMMON_SetVLDPower(UINT32 u4VDecID, BOOL fgOn);
// Description :Turn on or turn off VLD power
// Parameter : u4VDecID : video decoder hardware ID
//                 fgOn : Flag to vld power on or off
// Return      : None
// **************************************************************************
void vDEC_HAL_COMMON_SetVLDPower(UINT32 u4VDecID, BOOL fgOn)
{
    if (fgOn)
    {
        vVDecWriteVLD(u4VDecID, RW_VLD_PWRSAVE, 0);
    }
    else
    {
        vVDecWriteVLD(u4VDecID, RW_VLD_PWRSAVE, 1);
    }
    return;
}


INT32 i4VDEC_HAL_Dram_Busy( UINT32 u4ChipID, UINT32 u4StartAddr, UINT32 u4Offset)
{

    vWriteReg(0x7210, (u4StartAddr << 4));
    vWriteReg(0x7214, (u4Offset << 4));
    vWriteReg(0x7104, 0x0);    
    vWriteReg(0x7218, 0x8e0f110d);    
    return 0;
}

INT32 i4VDEC_HAL_Dram_Busy_Off( UINT32 u4ChipID, UINT32 u4StartAddr, UINT32 u4Offset)
{

    vWriteReg(0x7210, (u4StartAddr << 4));
    vWriteReg(0x7214, (u4Offset << 4));
    //vWriteReg(0x7104, 0x0);    
    vWriteReg(0x7218, 0x860f110d);    
    return 0;
}

#if BANDWIDTH_MEASURE
void vVDEC_HAL_BANDWID_MEAS_DumpReg(UINT32 u4VDecID)
{
    UTIL_Printf("[BANDWIDTH_MEASURE]:%d,%d,%d,%d,%d,%d,%d\n",
                        u4VDecReadMC(u4VDecID,0x770),
                        u4VDecReadMC(u4VDecID,0x8b8),
                        u4VDecReadMC(u4VDecID,0x28),
                        u4VDecReadMC(u4VDecID,0x2c),
                        u4VDecReadMC(u4VDecID,0x9e0),
                        u4VDecReadMC(u4VDecID,0x9e4),
                        (u4VDecReadMISC(u4VDecID,0xf0)&1));
}
#endif
#if PARA8_BEHAVIOR_TEST
void vVDEC_HAL_PARA8_BEHAVIOR_DumpReg(UINT32 u4VDecID)
{
    //UTIL_Printf("NBM_DLE_NUM,ESA_REQ_DATA_NUM,MC_REQ_DATA_NUM,MC_MBX,MC_MBY,CYC_SYS,INTRA_CNT,LAT_BUF_BYPASS,CYCLE_DRAM,Y_BLK_CNT,C_BLK_CNT,WAIT_CNT,PARA8_BEHAVIOR\n");
    UTIL_Printf("[PARA8_BEHAVIOR]:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                        u4VDecReadMC(u4VDecID,0x770),
                        u4VDecReadMC(u4VDecID,0x8b8),
                        u4VDecReadMC(u4VDecID,0xa28),
                        u4VDecReadMC(u4VDecID,0x28),
                        u4VDecReadMC(u4VDecID,0x2c),
                        u4VDecReadMC(u4VDecID,0x9e0),
                        u4VDecReadMC(u4VDecID,0x9e4),
                        (u4VDecReadMISC(u4VDecID,0xf0)&1),
                        u4VDecReadMC(u4VDecID,0x778),
                        u4VDecReadMC(u4VDecID,0x9e8),
                        u4VDecReadMC(u4VDecID,0x9ec),
                        u4VDecReadMC(u4VDecID,0x9f0),
                        u4VDecReadMC(u4VDecID,0x998));
}
#endif

#if VDEC_SUPPORT_IOMMU 
void vDEC_HAL_COMMON_IOMMUInit(void)
{
#if 1 //mpeg2/4/wmv/.... 
    vVDecWriteIOMMU(0, RW_IOMMU_CFG0, 0xF);
    vVDecWriteIOMMU(0, RW_IOMMU_CFG1, u4HalGetTTB1());
    vVDecWriteIOMMU(0, RW_IOMMU_CFG2, 0x001d001b);
    vVDecWriteIOMMU(0, RW_IOMMU_CFG4, 0x0600000a);
    x_thread_delay(2);
    vVDecWriteIOMMU(0, RW_IOMMU_CFG4, 0x0010000a);
    x_thread_delay(20);
#else //vp8
    vVDecWriteIOMMU(0, RW_IOMMU_CFG0, 0xFF);
    vVDecWriteIOMMU(0, RW_IOMMU_CFG1, u4HalGetTTB1());
    vVDecWriteIOMMU(0, RW_IOMMU_CFG2, 0x001b0013);
    vVDecWriteIOMMU(0, RW_IOMMU_CFG3, 0x0000001d);
    vVDecWriteIOMMU(0, RW_IOMMU_CFG4, 0x0600000a);
    x_thread_delay(2);
    vVDecWriteIOMMU(0, RW_IOMMU_CFG4, 0x0010000a);
    x_thread_delay(20);
#endif
#if 0
         UINT32 u4RegVal = 0;
         IOMMU_WRITE32(REG_RW_IOMMU_CFG4, IOMMU_PNG, 0x030010A);  
         u4RegVal = 0xFE|(0x1<<0);
         IOMMU_WRITE32(REG_RW_IOMMU_CFG0, IOMMU_PNG, u4RegVal);
         IOMMU_WRITE32(REG_RW_IOMMU_CFG1, IOMMU_PNG, u4HalGetTTB1()); 
         IOMMU_WRITE32(REG_RW_IOMMU_CFG2, IOMMU_PNG, 0x1B0019);//0x1B0019 
#endif
}

void vDEC_HAL_COMMON_SetMMUUsrTable(void)
{

}

void vDEC_HAL_COMMON_MMUFlush(void)
{
    UINT32 u4VdeRegVal;
    u4VdeRegVal = u4VDecReadIOMMU(0, RW_IOMMU_CFG4);
    vVDecWriteIOMMU(0, RW_IOMMU_CFG4, u4VdeRegVal|0x80000000);
    while (u4VDecReadIOMMU(0, RW_IOMMU_CFG8)&0x20000000);
}
#endif

typedef struct _GetBitCount_Mask_{
    UINT32 u4Mask;                // for getting how many bit from MSB
    UINT32 u4RemoveMask; // for keeping how many bit from LSB
}GetBitCount_Mask;

static GetBitCount_Mask GetBitCount_MaskIs[] = {
    {0x00000000, 0x00000000}, // 0 bit
    {0x80000000, 0x00000001}, // 1 bit
    {0xC0000000, 0x00000003}, // 2 bits
    {0xE0000000, 0x00000007}, // 3 bits
    {0xF0000000, 0x0000000F}, // 4 bits
    {0xF8000000, 0x0000001F}, // 5 bits
    {0xFC000000, 0x0000003F}, // 6 bits
    {0xFE000000, 0x0000007F}, // 7 bits
    {0xFF000000, 0x000000FF}, // 8 bits
    {0xFF800000, 0x000001FF}, // 9 bits
    {0xFFC00000, 0x000003FF}, // 10 bits
    {0xFFE00000, 0x000007FF}, // 11 bits
    {0xFFF00000, 0x00000FFF}, // 12 bits
    {0xFFF80000, 0x00001FFF}, // 13 bits
    {0xFFFC0000, 0x00003FFF}, // 14 bits
    {0xFFFE0000, 0x00007FFF}, // 15 bits
    {0xFFFF0000, 0x0000FFFF}, // 16 bits
    {0xFFFF8000, 0x0001FFFF}, // 17 bits
    {0xFFFFC000, 0x0003FFFF}, // 18 bits
    {0xFFFFE000, 0x0007FFFF}, // 19 bits
    {0xFFFFF000, 0x000FFFFF}, // 20 bits
    {0xFFFFF800, 0x001FFFFF}, // 21 bits
    {0xFFFFFC00, 0x003FFFFF}, // 22 bits
    {0xFFFFFE00, 0x007FFFFF}, // 23 bits
    {0xFFFFFF00, 0x00FFFFFF}, //24 bits
    {0xFFFFFF80, 0x01FFFFFF}, // 25 bits
    {0xFFFFFFC0, 0x03FFFFFF}, // 26 bits
    {0xFFFFFFE0, 0x07FFFFFF}, // 27 bits
    {0xFFFFFFF0, 0x0FFFFFFF}, // 28 bits
    {0xFFFFFFF8, 0x1FFFFFFF}, //29 bits
    {0xFFFFFFFC, 0x3FFFFFFF}, //30 bits
    {0xFFFFFFFE, 0x7FFFFFFF}, //31 bits
    {0xFFFFFFFF, 0xFFFFFFFF}, //32 bits
};


static UINT32 RBSPCheckBitCountInCache(pRBSP_BitStream pRBSP_BitStreamInst)
{
    return(pRBSP_BitStreamInst->u4BitLeft);
}

// each time we only can get one byte
static INT32 RBSPGetMoreBits(pRBSP_BitStream pRBSP_BitStreamInst)
{
    INT32 i4RetValue = 1;
    UCHAR *pBuffer = NULL;
    int i4DataSize = 0;
    int i4DataSize1 = 0;
    unsigned int u4tmpData = 0;
    //if cache1 has bit, then consume it first, then return
    if(pRBSP_BitStreamInst->u4BitLeft1 > 0){
        //push bits into cache
        i4DataSize = CACHE_BUFF_SIZE - pRBSP_BitStreamInst->u4BitLeft;
         if(i4DataSize >= pRBSP_BitStreamInst->u4BitLeft1){
             //just push all cache1 bit into cache
            #if DEBUG_INFO
                LOG(0,"[Debug] 1.1B. cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
            #endif
             pRBSP_BitStreamInst->u4Cache = (pRBSP_BitStreamInst->u4Cache << pRBSP_BitStreamInst->u4BitLeft1) | (pRBSP_BitStreamInst->u4Cache1);
             pRBSP_BitStreamInst->u4BitLeft += pRBSP_BitStreamInst->u4BitLeft1;
             pRBSP_BitStreamInst->u4Cache1 = 0;
             pRBSP_BitStreamInst->u4BitLeft1 = 0;
             #if DEBUG_INFO
                LOG(0,"[Debug] 1.1A. cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
            #endif
         }
         else{             
             #if DEBUG_INFO
                LOG(0,"[Debug] 1.2B. cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
            #endif
             u4tmpData = pRBSP_BitStreamInst->u4Cache1;
             i4DataSize1 = pRBSP_BitStreamInst->u4BitLeft1 - i4DataSize;
             u4tmpData = u4tmpData >> i4DataSize1;
             pRBSP_BitStreamInst->u4Cache = (pRBSP_BitStreamInst->u4Cache << i4DataSize) | (u4tmpData);
             pRBSP_BitStreamInst->u4BitLeft += i4DataSize;
             pRBSP_BitStreamInst->u4Cache1 = pRBSP_BitStreamInst->u4Cache1& GetBitCount_MaskIs[i4DataSize1].u4RemoveMask;
             pRBSP_BitStreamInst->u4BitLeft1 = i4DataSize1;
             #if DEBUG_INFO
                LOG(0,"[Debug] 1.2A. cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
            #endif
         }
        return i4RetValue;
    }

FILLMORE:
    pBuffer = pRBSP_BitStreamInst->pStartBuff;
    if(pRBSP_BitStreamInst->pEndBuff - pRBSP_BitStreamInst->pStartBuff >= 3){
        if((pBuffer[0] == 0x0) && (pBuffer[1] == 0x0) && (pBuffer[2] == 0x3)){//need remove 03
            #if DEBUG_INFO
                LOG(0,"[Info] remove 03!!\n");
            #endif
            //only get 2 bytes data, and remove 3rd byte (03)
            if(pRBSP_BitStreamInst->u4BitLeft<=16){                
                #if DEBUG_INFO
                    LOG(0,"[Debug] 2.1B. cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
                #endif 
                pRBSP_BitStreamInst->u4Cache = (pRBSP_BitStreamInst->u4Cache <<16) | (pBuffer[0] << 8) | (pBuffer[1]);
                pRBSP_BitStreamInst->u4BitLeft +=16; // 2 bytes,
                #if DEBUG_INFO
                    LOG(0,"[Debug] 2.1A. cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
                #endif
            }
            else{ //need to keep in cache1
                if(pRBSP_BitStreamInst->u4BitLeft<=24){//only one byte can push to cache and other byte save to cache1
                    #if DEBUG_INFO
                        LOG(0,"[Debug] 2.2B. cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
                    #endif
                    pRBSP_BitStreamInst->u4Cache = (pRBSP_BitStreamInst->u4Cache << 8) | (pBuffer[0]);
                    pRBSP_BitStreamInst->u4BitLeft +=8; // 1 bytes,
                    pRBSP_BitStreamInst->u4Cache1 = 0x0;
                    pRBSP_BitStreamInst->u4Cache1 = (pBuffer[1]);
                    pRBSP_BitStreamInst->u4BitLeft1 = 8;
                     #if DEBUG_INFO
                        LOG(0,"[Debug] 2.2A. cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
                    #endif
                }
                else{
                    // save to cache1 first
                    #if DEBUG_INFO
                        LOG(0,"[Debug] 2.3B. cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
                    #endif
                    pRBSP_BitStreamInst->u4Cache1 = 0x0;
                    pRBSP_BitStreamInst->u4Cache1 = (pBuffer[0] << 8) | (pBuffer[1]);
                    pRBSP_BitStreamInst->u4BitLeft1 = 16;
                    #if DEBUG_INFO
                        LOG(0,"[Debug] 2.3A. cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
                    #endif                    
                }
            }
            pRBSP_BitStreamInst->u4ByteLeft = pRBSP_BitStreamInst->u4ByteLeft  - 3; //include '03'
            pRBSP_BitStreamInst->pStartBuff = pBuffer + 3;
        }
        else{
            //each time only push 1 byte
            if(pRBSP_BitStreamInst->u4BitLeft<=24){
                #if DEBUG_INFO
                    LOG(0,"[Debug] 3.1B. cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
                #endif
                pRBSP_BitStreamInst->u4Cache = (pRBSP_BitStreamInst->u4Cache << 8) | (pBuffer[0]);
                pRBSP_BitStreamInst->u4BitLeft +=8;
                #if DEBUG_INFO
                    LOG(0,"[Debug] 3.1A. cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
                #endif
                if(pRBSP_BitStreamInst->u4BitLeft <= 24){
                    pRBSP_BitStreamInst->u4ByteLeft -= 1;
                    pRBSP_BitStreamInst->pStartBuff += 1;
                    goto FILLMORE; //fill more for cache
                }
            }
            else{
                // save to cache1 first
                #if DEBUG_INFO
                    LOG(0,"[Debug] 3.2B. cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
                #endif
                pRBSP_BitStreamInst->u4Cache1 = 0x0;
                pRBSP_BitStreamInst->u4Cache1 = (pBuffer[0]);
                pRBSP_BitStreamInst->u4BitLeft1 = 8;
                #if DEBUG_INFO
                    LOG(0,"[Debug] 3.2A. cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
                #endif
            }
            pRBSP_BitStreamInst->u4ByteLeft -= 1;
            pRBSP_BitStreamInst->pStartBuff += 1;
        }
    }
    else{
        //each time only push 1 byte
        if(pRBSP_BitStreamInst->u4BitLeft<=24){       
            #if DEBUG_INFO
                LOG(0,"[Debug] 4.1B. cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
            #endif
            pRBSP_BitStreamInst->u4Cache = (pRBSP_BitStreamInst->u4Cache << 8) | (pBuffer[0]);
            pRBSP_BitStreamInst->u4BitLeft +=8;
            #if DEBUG_INFO
                LOG(0,"[Debug] 4.1A. cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
            #endif
        }
        else{
            // save to cache1 first            
            #if DEBUG_INFO
                LOG(0,"[Debug] 4.2B. cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
            #endif
            pRBSP_BitStreamInst->u4Cache1 = 0x0;
            pRBSP_BitStreamInst->u4Cache1 = (pBuffer[0]);
            pRBSP_BitStreamInst->u4BitLeft1 = 8;            
            #if DEBUG_INFO
                LOG(0,"[Debug] 4.2A. cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
            #endif
        }
        pRBSP_BitStreamInst->u4ByteLeft -= 1;
        pRBSP_BitStreamInst->pStartBuff += 1;
    }
    return i4RetValue;
}

INT32 initRBSPBitStream(pRBSP_BitStream pRBSP_BitStreamInst, UCHAR *pBuffer, UINT32 u4BuffSize)
{
    //UINT32 u4DataSize = 0;
    INT32 i4RetValue = 1;
    //pRBSP_BitStream pRBSP_BitStreamInst = NULL;
    
    if((NULL == pBuffer) || (u4BuffSize < MIN_NALU_SIZE)){
        i4RetValue = -1;
        goto GoOut;
    }

    /*u4DataSize = sizeof(RBSP_BitStream);
    LOG(0, "u4DataSize %d\n", u4DataSize);
    pRBSP_BitStreamInst = (pRBSP_BitStream)x_mem_alloc_virtual(u4DataSize);
    if(NULL == pRBSP_BitStreamInst){
        i4RetValue = -2;
        goto GoOut;
    }*/
    pRBSP_BitStreamInst->pStartBuff = pBuffer;
    pRBSP_BitStreamInst->pEndBuff = pBuffer+u4BuffSize;
    pRBSP_BitStreamInst->u4Cache = 0;
    pRBSP_BitStreamInst->u4BitLeft = 0;
    pRBSP_BitStreamInst->u4Cache1 = 0;
    pRBSP_BitStreamInst->u4BitLeft1 = 0;
    pRBSP_BitStreamInst->u4BitAccumulated = 0;
    pRBSP_BitStreamInst->u4BitTotalBuffBitCount = u4BuffSize*8;
    LOG(0, "pStartBuff %x, pEndBuff %x, u4BitTotalBuffBitCount %d\n", 
            pRBSP_BitStreamInst->pStartBuff, pRBSP_BitStreamInst->pEndBuff, 
            pRBSP_BitStreamInst->u4BitTotalBuffBitCount);
    
    if(RBSPGetMoreBits(pRBSP_BitStreamInst) <= 0){
         i4RetValue = -3;
        goto GoOut;
    }

    //(*ppRBSP_BitStreamRet) = pRBSP_BitStreamInst;

    return i4RetValue;
GoOut:
    return i4RetValue;
}

INT32 destroyRBSPBitStream(pRBSP_BitStream *ppRBSP_BitStreamInst)
{
    pRBSP_BitStream pRBSP_BitStreamInst = NULL;

    pRBSP_BitStreamInst = (*ppRBSP_BitStreamInst);
    if(NULL != pRBSP_BitStreamInst){
        x_mem_free(pRBSP_BitStreamInst);
        (*ppRBSP_BitStreamInst) = 0;
        return 1;
    }
    return -1;
}


BOOL RBSPMoreData(pRBSP_BitStream pRBSP_BitStreamInst)
{
    unsigned int u4RemainedBits = 0;
    unsigned int u4CurrentByte = 0;
    unsigned int u4RBSPTrailingBits = 0;
    int i4ctr_bit = 0;
    #if DEBUG_INFO
        LOG(0,"[Info] >>  cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
    #endif
    // rest bits more than one byte then return true
    if((pRBSP_BitStreamInst->u4BitTotalBuffBitCount - pRBSP_BitStreamInst->u4BitAccumulated) > 8){
         return TRUE;
    }

    u4RemainedBits = (pRBSP_BitStreamInst->u4BitTotalBuffBitCount - pRBSP_BitStreamInst->u4BitAccumulated);
    #if DEBUG_INFO
        LOG(0,"u4RemainedBits %d\n", u4RemainedBits);
    #endif
    //get current byte
    u4CurrentByte = pRBSP_BitStreamInst->u4Cache;
    #if DEBUG_INFO
        LOG(0,"u4CurrentByte %d\n", u4CurrentByte);
    #endif
    i4ctr_bit = (u4CurrentByte >> (--u4RemainedBits)) & 0x01;
    #if DEBUG_INFO
        LOG(0,"i4ctr_bit %d\n", i4ctr_bit);
    #endif
    if(0 == i4ctr_bit){// stop bit equals to 0
        return TRUE;
    }
    else{
        u4RBSPTrailingBits = u4CurrentByte & GetBitCount_MaskIs[--u4RemainedBits].u4RemoveMask;
        #if DEBUG_INFO
            LOG(0,"u4RBSPTrailingBits 0x%x\n", u4RBSPTrailingBits);
        #endif
        if( 0 == u4RBSPTrailingBits){//is trailing bits
            return FALSE;
        }
        else{
            return TRUE;
        }
        
    }
    return FALSE;
}

INT32 RBSPGetBitOffset(pRBSP_BitStream pRBSP_BitStreamInst)
{
    if(NULL == pRBSP_BitStreamInst){
        return -1;
    }

    return pRBSP_BitStreamInst->u4BitAccumulated;
}

INT32 RBSPGetBits(pRBSP_BitStream pRBSP_BitStreamInst, UINT32 u4BitCount)
{
//    INT32 i4RetValue = 0;
    UINT32 u4DataSize = 0;
    UINT32 u4tmpData = 0;
    int iCounting = 0;
    #if DEBUG_INFO
    LOG(0,"[Debug] %s. Get Bit Count (%d), Accmu (%d), Buff Total (%d)\n", __FUNCTION__, u4BitCount, pRBSP_BitStreamInst->u4BitAccumulated,pRBSP_BitStreamInst->u4BitTotalBuffBitCount);
    #endif
    if (NULL == pRBSP_BitStreamInst)
    {
        LOG(0, "%s, %d, NULL Pointer\n", __FUNCTION__, __LINE__);
        return 0;
    }
    pRBSP_BitStreamInst->u4BitAccumulated += u4BitCount;
    if (pRBSP_BitStreamInst->u4BitAccumulated > pRBSP_BitStreamInst->u4BitTotalBuffBitCount){
        pRBSP_BitStreamInst->bError = TRUE;
    #if DEBUG_INFO
        LOG(0,"[Err] %s. Overflow target Accmu (%d), Buff total (%d)\n", __FUNCTION__, pRBSP_BitStreamInst->u4BitAccumulated, pRBSP_BitStreamInst->u4BitTotalBuffBitCount);
    #endif
    }

    if(((u4BitCount >  0) && (u4BitCount <= CACHE_BUFF_SIZE))){
GetBitsAgain:
        if(pRBSP_BitStreamInst->u4BitLeft >= u4BitCount){//cached data larger- equal  than required bits
            #if DEBUG_INFO
                LOG(0,"[Debug] 5.1B. cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
            #endif
            //just remove current have
            u4DataSize = pRBSP_BitStreamInst->u4BitLeft - u4BitCount; //rest bit count in cache
            u4tmpData = pRBSP_BitStreamInst->u4Cache;
            if(u4DataSize == 0){
                // use all cache data
                u4tmpData = u4tmpData & GetBitCount_MaskIs[pRBSP_BitStreamInst->u4BitLeft].u4RemoveMask;
                pRBSP_BitStreamInst->u4Cache = 0;
                pRBSP_BitStreamInst->u4BitLeft = 0;
                #if DEBUG_INFO
                    LOG(0,"[Debug] 5.2A. cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
                #endif
                //get more bits :TODO?
                RBSPGetMoreBits(pRBSP_BitStreamInst);
                return u4tmpData;
            }
            else{
                u4tmpData = u4tmpData & GetBitCount_MaskIs[pRBSP_BitStreamInst->u4BitLeft].u4RemoveMask;
                u4tmpData = u4tmpData >> u4DataSize;
                pRBSP_BitStreamInst->u4Cache = pRBSP_BitStreamInst->u4Cache & GetBitCount_MaskIs[u4DataSize].u4RemoveMask;
                pRBSP_BitStreamInst->u4BitLeft -= u4BitCount;
                #if DEBUG_INFO
                    LOG(0,"[Debug] 5.3A. cache (%08X,%d) cache1 (%08X, %d)\n", pRBSP_BitStreamInst->u4Cache, pRBSP_BitStreamInst->u4BitLeft, pRBSP_BitStreamInst->u4Cache1, pRBSP_BitStreamInst->u4BitLeft1);
                #endif
                return u4tmpData;
            }
        }
        else{//cached data less  than required bits
            //get more bits
            RBSPGetMoreBits(pRBSP_BitStreamInst);
            while(RBSPCheckBitCountInCache(pRBSP_BitStreamInst) <= u4BitCount){
                iCounting++;
                if(iCounting > 5){
                    #if DEBUG_INFO
                        LOG(0,"[Err] something wrong!!! %d\n",iCounting);
                    #endif
                    return 0;
                }
                break;
            }
            goto GetBitsAgain;
        }
        #if DEBUG_INFO
            LOG(0,"[Err] NEVER HERE [%s:%d]!!!\n",__FUNCTION__, __LINE__);
        #endif
        return 0;
    }
    else{
        #if DEBUG_INFO
            LOG(0,"[Err] invalid bit-stream or get bit count %d!!!\n",u4BitCount);
        #endif
        return 0;
    }
}

UINT32 RBSPGetUE(pRBSP_BitStream pRBSP_BitStreamInst)
{
    int i4BitCountOfZero = 0;
    int i4CodeNum = 0;
    int i4PowerTwo = 0x1;
    while(0 == RBSPGetBits(pRBSP_BitStreamInst, 1)){
//        H264_DRV_RBSP_CHECK_ERROR(pRBSP_BitStreamInst);
        i4BitCountOfZero++;
    }
    #if DEBUG_INFO
        LOG(0,"[Debug] Zero Count %d\n", i4BitCountOfZero);
    #endif
    if(i4BitCountOfZero == 0){
        return 0;
     }
    else if(((i4BitCountOfZero >= 1)) &&(i4BitCountOfZero <= 32)){
        i4CodeNum = (i4PowerTwo << i4BitCountOfZero) - 1;
        return (i4CodeNum + RBSPGetBits(pRBSP_BitStreamInst, i4BitCountOfZero));
    }
    else{
        #if DEBUG_INFO
            LOG(0,"[Err] the ZeroCount bigger than 32, %d\n", i4BitCountOfZero);
        #endif
        return 0;
    }

    return 0;
}

INT32 FindNextSync(CHAR *pData, INT32 iBufferSize, CHAR **ppRetData)
{
    INT32 iRetValue = 0;
    INT32 iIndexer = 0;
    INT32 iFound = 0;

    if (NULL == pData  ){
        iRetValue = -1;
        goto GoOut;
    }

    if (iBufferSize <= 2) {
        iRetValue = -10; // there is no sufficenct data for parsing
        goto GoOut;
    }

    iFound = 0;
    for (iIndexer = 0; iIndexer <= (iBufferSize - 3); iIndexer++) {
        if ((pData[iIndexer] == 0x00) && (pData[iIndexer+1] == 0x00) && (pData[iIndexer+2] == 0x01)) {
            if (iIndexer != 0) {
                if (0x00 == pData[iIndexer-1]) {
                iIndexer -= 1;
                }
            }
            iFound = 1;
            break;
        }
    }

    if (1 != iFound) {
        iRetValue = -11; // cannot found
        goto GoOut;
    }
    else { //found sync point
        //printf("Found sync point %d\n", iIndexer);
        (*ppRetData) = pData + iIndexer;
    }

    return iIndexer;
GoOut:
    return iRetValue;
}

INT32 RBSPGetSE(pRBSP_BitStream pRBSP_BitStreamInst)
{
    int i4CodeNum = 0;
    int i4Sign = 0;
    i4CodeNum = RBSPGetUE(pRBSP_BitStreamInst);
    // convert the code number to signed....
    i4Sign = (i4CodeNum&1)-1;
    return (((i4CodeNum + 1)>>1)^i4Sign)-i4Sign;
}

