/*
 * Copyright:
 * ----------------------------------------------------------------------------
 * This confidential and proprietary software may be used only as authorised
 * by a licensing agreement from ARM Limited.
 *      (C) COPYRIGHT 2008-2009 ARM Limited, ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised copies and
 * copies may only be made to the extent permitted by a licensing agreement
 * from ARM Limited.
 * ----------------------------------------------------------------------------
 * $LastChangedRevision: 2907 $
 * ----------------------------------------------------------------------------
 */

/* ============================================================================
    Includes
============================================================================ */
#include "gcpu.h"
#include "svc_registry.h"

#include "x_hal_5381.h"
#include "x_hal_arm.h"
#include "x_bim.h"
#include "tzst.h"

#include "x_chip_id.h"

#include "tee_dtcp.h"
#include "tz_dmx_if.h"


//static GCPU_HW_CMD_T *prPrevHwCmd = NULL;

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define AES_KEY_LEN_WRAPPER(len) (len == KEY_BIT_LEN_128)?0:(len == KEY_BIT_LEN_192)?1:2
#define IsValidSecureSlot(u4Ptr)    ((u4Ptr >= GCPU_SECURESLOT_PTR) && \
                                     (u4Ptr < GCPU_SECURESLOT_PTR + GCPU_SECURESLOT_NUM))

#undef LOG
#define LOG(level, fmt...)	

#define GCPU_SECURESLOT_PTR         (48)
#define GCPU_SECURESLOT_NUM         (48)

//-----------------------------------------------------------------------------
// External functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static UINT32 _arSecureKeyHoles[GCPU_SECURESLOT_NUM];

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static UINT32 _Gcpu_Align(UINT32 u4Addr, UINT32 u4Alignment)
{
	UINT32 u4Unaligned;

	if (u4Alignment <= 1)
	{
		return u4Addr;
	}

	u4Unaligned = u4Addr % u4Alignment;
	if (u4Unaligned != 0)
	{
		u4Addr += u4Alignment - u4Unaligned;
	}

	return u4Addr;
}


static BOOL _Gcpu_IsAligned(UINT32 u4Addr, UINT32 u4Alignment)
{
	return u4Addr == _Gcpu_Align(u4Addr, u4Alignment);
}

static BOOL _Gcpu_SaveSecureData(UINT32 u4SlotPtr, UINT32 u4Data)
{
    UINT32 u4Idx;

    if (u4SlotPtr >= GCPU_SECURESLOT_PTR + GCPU_SECURESLOT_NUM)
    {
        return FALSE;
    }

    u4Idx = u4SlotPtr - GCPU_SECURESLOT_PTR;
    _arSecureKeyHoles[u4Idx] = u4Data;

    return TRUE;
}

static BOOL _Gcpu_RestoreSecureSlots(void)
{
    UINT32 i, u4Key;

    for (i = 0; i < GCPU_SECURESLOT_NUM; i++)
    {
        u4Key = _arSecureKeyHoles[i];
        GCPUCMD_WRITE32(GCPU_REG_MEM_CMD + GCPU_SECURESLOT_PTR + i, u4Key);
    }

    return TRUE;
}

BOOL SvcEx_GCPU_Hw_CmdRun(UINT32 u4Addr, UINT32 u4Size)
{
    UINT32 i;
    const GCPU_HW_CMD_T *prHwCmd = (const GCPU_HW_CMD_T *)u4Addr;

    if (prHwCmd == NULL)
    {
        return FALSE;
    }

#if 0
    if (prPrevHwCmd != prHwCmd)
    {
        DMSG("[Run: prev: 0x%08x curr: 0x%08x]\n", (UINT32)prPrevHwCmd, (UINT32)prHwCmd);
    }
    prPrevHwCmd = prHwCmd;
#endif

    // Setup Parameter
    for(i = 1; i < GCPU_PARAM_NUM; i++)
    {
        GCPUCMD_WRITE32(GCPU_REG_MEM_CMD + i, prHwCmd->u4Param[i]);
    }

    // Clear/Enable GCPU Interrupt
    GCPUCMD_WRITE32(GCPU_REG_INT_CLR, GCPUCMD_READ32(GCPU_REG_INT_CLR));
    GCPUCMD_WRITE32(GCPU_REG_INT_EN, GCPU_INT_MASK);

    // GCPU Decryption Mode
    GCPUCMD_WRITE32(GCPU_REG_MEM_CMD, prHwCmd->u4Param[0]);

    // GCPU PC
    GCPUCMD_WRITE32(GCPU_REG_PC_CTL, 0x0);

    return TRUE;
}

BOOL SvcEx_GCPU_Hw_CmdReturn(UINT32 u4Addr, UINT32 u4Size)
{
    UINT32 i;
    GCPU_HW_CMD_T *prHwCmd = (GCPU_HW_CMD_T *)u4Addr;

    if (prHwCmd == NULL)
    {
        return FALSE;
    }

#if 0
    if (prPrevHwCmd != prHwCmd)
    {
        DMSG("[Return: prev: 0x%08x curr: 0x%08x]\n", (UINT32)prPrevHwCmd, (UINT32)prHwCmd);
    }
    prPrevHwCmd = prHwCmd;
#endif

    // read back param 0 - 31
    for(i = 0; i < 32; i++)
    {
        prHwCmd->u4Param[i] = GCPUCMD_READ32(GCPU_REG_MEM_CMD + i);
    }

    // read back param 32 - 47
    for(i = GCPU_PARAM_RET_PTR; i < GCPU_PARAM_RET_PTR + GCPU_PARAM_RET_NUM; i++)
    {
        prHwCmd->u4Param[i] = GCPUCMD_READ32(GCPU_REG_MEM_CMD + i);
    }

    return TRUE;
}

BOOL SvcEx_GCPU_EnableIOMMU(UINT32 u4Addr, UINT32 u4Size)
{
    GCPU_TZ_PARAM_T *prTzParam = (GCPU_TZ_PARAM_T *)u4Addr;
    UINT32 u4Src1Start, u4Src1End;
    UINT32 u4Src2Start, u4Src2End;
    UINT32 u4MmuTblAddr;

    if (prTzParam == NULL)
    {
        return FALSE;
    }

    u4Src1Start = prTzParam->u4MmuSrc1Start;
    u4Src1End = prTzParam->u4MmuSrc1End;
    u4Src2Start = prTzParam->u4MmuSrc2Start;
    u4Src2End = prTzParam->u4MmuSrc2End;
    u4MmuTblAddr = prTzParam->u4MmuTblAddr;

    // setup IOMMU
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG4, 0x0603000A);
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG4, 0x0010000A);
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG0, 0xFE | (0x01 << 0));
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG1, u4MmuTblAddr);
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG2, 0x00130011);

    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG9, (u4Src1Start & 0xfffff000) | 0x3);
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFGA, (u4Src1End - 1) & 0xfffff000);
    if(u4Src2Start == 0 || u4Src2End == 0)
    {
        IOMMU_GCPU_WRITE32(REG_IOMMU_CFGB, (u4Src1Start & 0xfffff000) | 0x1);
        IOMMU_GCPU_WRITE32(REG_IOMMU_CFGC, (u4Src1End - 1) & 0xfffff000);
    }
    else
    {
        IOMMU_GCPU_WRITE32(REG_IOMMU_CFGB, (u4Src2Start & 0xfffff000) | 0x1);
        IOMMU_GCPU_WRITE32(REG_IOMMU_CFGC, (u4Src2End - 1) & 0xfffff000);
    }

    //fire IOMMU cache
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG4, IOMMU_GCPU_READ32(REG_IOMMU_CFG4) | (1U << 31));
    while((IOMMU_GCPU_READ32(REG_IOMMU_CFG8) & (1 << 29)) != 0);

    return TRUE;
}

BOOL SvcEx_GCPU_DisableIOMMU(void)
{
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG0, 0);
    return TRUE;
}

BOOL SvcEx_GCPU_Hw_SetSlot(UINT32 u4Addr, UINT32 u4Size)
{
    GCPU_TZ_PARAM_T *prTzParam = (GCPU_TZ_PARAM_T *)u4Addr;
    UINT32 u4Idx, u4Data;
    BOOL fgRet;

    if (prTzParam == NULL)
    {
        return FALSE;
    }

    u4Idx = prTzParam->u4SlotIdx;
    u4Data = prTzParam->u4SlotData;

    fgRet = _Gcpu_SaveSecureData(u4Idx, u4Data);

    if (fgRet)
    {
        GCPUCMD_WRITE32(GCPU_REG_MEM_CMD + u4Idx, u4Data);
    }

    return fgRet;
}

BOOL SvcEx_GCPU_ClearIRQ(void)
{
    // Clear and disable interrupt
    GCPUCMD_WRITE32(GCPU_REG_INT_CLR, GCPU_INT_MASK);
    GCPUCMD_WRITE32(GCPU_REG_INT_EN, 0);
    return TRUE;
}

BOOL SvcEx_GCPU_ClearIommuIRQ(void)
{
    // Clear and disable interrupt, reset IOMMU
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG4, IOMMU_GCPU_READ32(REG_IOMMU_CFG4) | 0x06000005);
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG4, IOMMU_GCPU_READ32(REG_IOMMU_CFG4) & ~(0x0610000F));
    return TRUE;
}

BOOL SvcEx_GCPU_Hw_Reset(UINT32 u4Addr, UINT32 u4Size)
{
    UINT32 u4Value = 0;
    UINT32 *pu4RamCode = NULL, *pu4Trapping = NULL;
    UINT32 u4RamCodeSize = 0, u4TrapSize = 0;
    UINT32 u4PhyDramCodeAddr = 0;
    BOOL fgInitRam, fgInitDram;
    const GCPU_TZ_PARAM_T *prTzParam = (const GCPU_TZ_PARAM_T *)u4Addr;

    if (prTzParam == NULL)
    {
        return FALSE;
    }

    pu4RamCode = prTzParam->pu4RamCode;
    u4RamCodeSize = prTzParam->u4RamCodeSize;
    TZ_SWD_BufferStart((UINT32)pu4RamCode, TZ_CACHE_ALIGN(u4RamCodeSize));
    
    pu4Trapping = prTzParam->pu4Trapping;
    u4TrapSize = prTzParam->u4TrapSize;
    TZ_SWD_BufferStart((UINT32)pu4Trapping, TZ_CACHE_ALIGN(u4TrapSize));
    
    u4PhyDramCodeAddr = prTzParam->u4PhyDramCodeAddr;
    fgInitRam = (BOOL)prTzParam->u4InitRam;
    fgInitDram = (BOOL)prTzParam->u4InitDram;

    u4Value = GCPUCMD_READ32(GCPU_REG_CTL);
    u4Value &= ~(0x7);
    GCPUCMD_WRITE32(GCPU_REG_CTL, u4Value);
    u4Value &= ~(0x8);
    GCPUCMD_WRITE32(GCPU_REG_CTL, u4Value);

    u4Value |= 0xF;
    GCPUCMD_WRITE32(GCPU_REG_CTL, u4Value);

    if ((fgInitRam) &&
        (pu4RamCode != NULL) && (u4RamCodeSize != 0) &&
        (pu4Trapping != NULL) && (u4TrapSize != 0))
    {
        //Init Ram Code
        UINT32 i;

        // clear
        GCPUCMD_WRITE32(GCPU_REG_MEM_ADDR, 0x80002000);
        for (i = 0; i < 1024; i++)
        {
            GCPUCMD_WRITE32(GCPU_REG_MEM_DATA, 0);
        }

        GCPUCMD_WRITE32(GCPU_REG_MEM_ADDR, 0x80002000);
        for (i = 0; i < u4RamCodeSize; i++)
        {
            GCPUCMD_WRITE32(GCPU_REG_MEM_DATA, *(pu4RamCode + i));
        }

        // trapping
        for (i = 0; i < u4TrapSize; i++)
        {
            GCPUCMD_WRITE32((GCPU_REG_TRAP_START + i), *(pu4Trapping + i));
        }
    }

    if ((fgInitDram) && (u4PhyDramCodeAddr != 0))
    {
        //Init Dram Code
        GCPUCMD_WRITE32(GCPU_REG_MSC, GCPUCMD_READ32(GCPU_REG_MSC) | (1 << 12));
        GCPUCMD_WRITE32(GCPU_REG_DRAM_INST_BASE, u4PhyDramCodeAddr);
    }

    TZ_SWD_BufferEnd((UINT32)pu4RamCode, TZ_CACHE_ALIGN(u4RamCodeSize));
    TZ_SWD_BufferEnd((UINT32)pu4Trapping, TZ_CACHE_ALIGN(u4TrapSize));

    return TRUE;
}

BOOL SvcEx_GCPU_IrqHandle(UINT32 u4Addr, UINT32 u4Size)
{
    GCPU_TZ_PARAM_T *prTzParam = (GCPU_TZ_PARAM_T *)u4Addr;

    if (prTzParam == NULL)
    {
        return FALSE;
    }

    if (GCPUCMD_READ32(GCPU_REG_INT_CLR) & 0x80000000)
    {
        prTzParam->u4ReturnedValue = 1;
        SvcEx_GCPU_Hw_Reset((UINT32)prTzParam, u4Size);

        // Clear/Enable GCPU Interrupt
        GCPUCMD_WRITE32(GCPU_REG_INT_CLR, GCPUCMD_READ32(GCPU_REG_INT_CLR));
        GCPUCMD_WRITE32(GCPU_REG_INT_EN, GCPU_INT_MASK);

        // GCPU PC
        GCPUCMD_WRITE32(GCPU_REG_PC_CTL, 0x0);
    }
    else
    {
        // Clear interrupt
        GCPUCMD_WRITE32(GCPU_REG_INT_CLR, GCPUCMD_READ32(GCPU_REG_INT_CLR));
        GCPUCMD_WRITE32(GCPU_REG_INT_EN, 0);
    }

    return TRUE;
}

BOOL SvcEx_GCPU_IOMMU_IrqHandle(UINT32 u4Addr, UINT32 u4Size)
{
    const GCPU_TZ_PARAM_T *prTzParam = (const GCPU_TZ_PARAM_T *)u4Addr;

    if (prTzParam == NULL)
    {
        return FALSE;
    }

    SvcEx_GCPU_Hw_Reset((UINT32)prTzParam, u4Size);

    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG4, IOMMU_GCPU_READ32(REG_IOMMU_CFG4) | 0x06000005);
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG4, IOMMU_GCPU_READ32(REG_IOMMU_CFG4) & ~(0x0610000F));

    return FALSE;
}

BOOL SvcEx_GCPU_GetRomCodeChecksum(UINT32 u4Addr, UINT32 u4Size)
{
    GCPU_TZ_PARAM_T *prTzParam = (GCPU_TZ_PARAM_T *)u4Addr;
    UINT32 i, res, tmp, d0, d1, d2, mask;

    if (prTzParam == NULL)
    {
        return FALSE;
    }

    SvcEx_GCPU_Hw_Reset((UINT32)prTzParam, u4Size);

    mask = 255; //0xff
    res = 0;

    GCPUCMD_WRITE32(GCPU_REG_MEM_ADDR, 0x00000000);

    for (i = 1; i <= GCPU_IMEM_ROM_SIZE; i++)
    {
        tmp = GCPUCMD_READ32(GCPU_REG_MEM_DATA);

        d0 = tmp & mask;
        d1 = (tmp >> 8) & mask;
        d2 = (tmp >> 16) & mask;
        if((i % 4) == 0)
        {
            tmp = (d2 << 16) + (d1 << 8) + d0;
        }
        if((i % 4) == 1)
        {
            tmp = (d1 << 16) + (d0 << 8) + d2;
        }
        if((i % 4) == 2)
        {
            tmp = (d0 << 16) + (d2 << 8) + d1;
        }
        if((i % 4) == 3)
        {
            tmp = (d0 << 16) + (d1 << 8) + d2;
        }

        res = res ^ tmp;
    }

    prTzParam->u4ReturnedValue = res;

    return TRUE;
}

BOOL SvcEx_GCPU_Hw_RestoreSecureKeys(void)
{
    INT32 i;
    UINT32 u4Data;
    UINT8 *p;
    UINT8 au1Key[16] = {};

    if (!_Gcpu_RestoreSecureSlots())
    {
        return FALSE;
    }

#ifdef CC_DMX_A1
    {
        #define REG_EFUSE_SID_0     (0x678)
        #define REG_EFUSE_SID_1     (0x67c)
        #define REG_EFUSE_SID_2     (0x680)
        #define REG_EFUSE_SID_3     (0x684)
        #define REG_EFUSE_KEY       (0x688)

        UINT32 au4SID[4] = {0};

        BIM_WRITE32(REG_EFUSE_KEY, 0x883);
        BIM_WRITE32(REG_EFUSE_KEY, 0x1404);

        au4SID[1] = BIM_READ32(REG_EFUSE_SID_1);
        au4SID[3] = BIM_READ32(REG_EFUSE_SID_3);
        au4SID[0] = BIM_READ32(REG_EFUSE_SID_0);
        au4SID[2] = BIM_READ32(REG_EFUSE_SID_2);

        x_memcpy((void*)au1Key, (void*)au4SID, 16);
    }
#else
    {
        x_memcpy(au1Key, (void *)BIM_MTK_KEY_ADDR, 16);
        for(i = 0; i < 16; i++)
        {
            au1Key[i] ^= 0x55;
        }
    }
#endif

    for(i = 0; i < 4; i++)
    {
        p = au1Key + i * 4;
        u4Data = KEY_WRAPPER(p);
        GCPUCMD_WRITE32(GCPU_REG_MEM_CMD + AES_MTD_SECURE_KEY_PTR + i, u4Data);
    }

    return TRUE;
}

INT32 GCPU_SetSecureSlot(UINT32 u4SlotPtr, UINT8* pu1Data, UINT32 u4Size)
{
    INT32 i4Result = S_GCPU_OK;
    UINT8 i, *p, u1Loop;
    UINT32 u4Key;

    if(u4Size > 32)
    {
        return E_GCPU_PARAM_WRONG;
    }

    if(u4SlotPtr > GCPU_SECURESLOT_PTR + GCPU_SECURESLOT_NUM)
    {
        return E_GCPU_PARAM_WRONG;
    }

    u1Loop = (u4Size + 3) / 4;
    for(i = 0; i < u1Loop; i++)
    {
        p = pu1Data + i * 4;
        u4Key = KEY_WRAPPER(p);
        GCPUCMD_WRITE32(GCPU_REG_MEM_CMD + u4SlotPtr + i, u4Key);
        if (!_Gcpu_SaveSecureData(u4SlotPtr + i, u4Key))
        {
            i4Result = E_GCPU_PARAM_WRONG;
            break;
        }
    }

    return i4Result;
}

BOOL TZ_GCPU_ReloadSecureKeyIv(GCPU_KEY_TYPE_T eKeyType, UINT32 u4KeyIdx,
                            UINT32 u4KeyPtr, UINT32 u4IvPtr)
{
    BOOL fgRet = TRUE;

    if (GCPU_KEY_TYPE_DTCP_KL == eKeyType)
    {
        if (TZ_SUCCESS != _teeDtcpSetLocalKeyToGCPU(u4KeyIdx, u4KeyPtr, u4IvPtr))
        {
            fgRet = FALSE;
            goto lbExit1;
        }
        else
        {
            DMSG("[TZ_GCPU]: Fail to reload, keyIdx=%u keyPtr=%u IvPtr=%u\n", u4KeyIdx, u4KeyPtr, u4IvPtr);
        }
    }
    else
    {
        DMSG("[TZ_GCPU]: unknown keyType=%u\n", eKeyType);
    }

lbExit1:
    return fgRet;
}

INT32 TZ_GCPU_MISC_CMD(UINT32 u4Handle, UINT32 u4Id, void* prParam, BOOL fgIOMMU)
{
    UINT32 i;
    UINT8 *p;
    GCPU_HW_CMD_T rHwCmd;

    if(prParam == NULL)
    {
        return E_GCPU_PARAM_WRONG;
    }

    UNUSED(fgIOMMU);

    // setup command
    switch(u4Id)
    {
    case GCPU_TSDESC:
        {
            static UINT8 au1WorkBuf[GCPU_FIFO_ALIGNMENT + GCPU_TSDESC_WORKBUF_SIZE + GCPU_FIFO_ALIGNMENT];
            UINT8 *pu1WorkBuf = au1WorkBuf;
            TSDESC_PARAM_T *prLParam = (TSDESC_PARAM_T *)prParam;
            TSDESC_PARAM_T rLParam = *prLParam;
            UINT32 u4OddKeyPtr, u4EvenKeyPtr, u4KeyOff;
            TSDESC_DESC_SCHEME eScheme;

            if (!_Gcpu_IsAligned(rLParam.u4SrcBufStart, GCPU_FIFO_ALIGNMENT) ||
                !_Gcpu_IsAligned(rLParam.u4SrcBufEnd, GCPU_FIFO_ALIGNMENT) ||
                !_Gcpu_IsAligned(rLParam.u4DstBufStart, GCPU_FIFO_ALIGNMENT) ||
                !_Gcpu_IsAligned(rLParam.u4DstBufEnd, GCPU_FIFO_ALIGNMENT))
            {
                return E_GCPU_PARAM_WRONG;
            }

            if (!_GCPU_IsInRegion(rLParam.u4SrcStartAddr, rLParam.u4SrcBufStart,
                    rLParam.u4SrcBufEnd) ||
                !_GCPU_IsInRegion(rLParam.u4DstStartAddr, rLParam.u4DstBufStart,
                    rLParam.u4DstBufEnd))
            {
                return E_GCPU_PARAM_WRONG;
            }

            if (prLParam->eKeyBitLen != KEY_BIT_LEN_128)
            {
                return E_GCPU_PARAM_WRONG;
            }
            
            memset(&rHwCmd, 0, sizeof(GCPU_HW_CMD_T));

            if (prLParam->eCipher == TSDESC_CIPHER_ECB)
            {
                u4KeyOff = 1;
                u4EvenKeyPtr = GCPU_PARAM_TSDESC_EVEN_EX;
                u4OddKeyPtr = GCPU_PARAM_TSDESC_ODD_EX;
                eScheme = TSDESC_DESC_SCHEME_EVEN_ODD_KEY;
                rHwCmd.u4Param[GCPU_PARAM_TSDESC_EVEN_EX] = 0x0 + 0x0; // ECB mode + 128 bits
                rHwCmd.u4Param[GCPU_PARAM_TSDESC_ODD_EX] = 0x0 + 0x0; // ECB mode + 128 bits
            }
            else if (prLParam->eCipher == TSDESC_CIPHER_CBC)
            {
                u4KeyOff = 1;
                u4EvenKeyPtr = GCPU_PARAM_TSDESC_EVEN_EX;
                u4OddKeyPtr = GCPU_PARAM_TSDESC_ODD_EX;
                eScheme = TSDESC_DESC_SCHEME_EVEN_ODD_KEY;
                rHwCmd.u4Param[GCPU_PARAM_TSDESC_EVEN_EX] = 0x10 + 0x0; // CBC mode + 128 bits
                rHwCmd.u4Param[GCPU_PARAM_TSDESC_ODD_EX] = 0x10 + 0x0; // CBC mode + 128 bits
            }
            else if (prLParam->eCipher == TSDESC_CIPHER_CBC_SCTE52)
            {
                u4KeyOff = 0;
                u4EvenKeyPtr = GCPU_PARAM_TSDESC_EVEN;
                u4OddKeyPtr = GCPU_PARAM_TSDESC_ODD;
                eScheme = TSDESC_DESC_SCHEME_CBC_SCTE52;
            }
            else if (prLParam->eCipher == TSDESC_CIPHER_CTR)
            {
                u4KeyOff = 0;
                u4EvenKeyPtr = GCPU_PARAM_TSDESC_EVEN;
                u4OddKeyPtr = GCPU_PARAM_TSDESC_ODD;
                eScheme = TSDESC_DESC_SCHEME_CTR;
            }
            else
            {
                return E_GCPU_PARAM_WRONG;
            }

            rHwCmd.u4Param[0] = TS_DESC;

            // setup parameter
            rHwCmd.u4Param[1] = rLParam.u4SrcBufStart;
            rHwCmd.u4Param[2] = rLParam.u4SrcBufEnd;
            rHwCmd.u4Param[3] = rLParam.u4SrcStartAddr;
            rHwCmd.u4Param[4] = rLParam.u4DstBufStart;
            rHwCmd.u4Param[5] = rLParam.u4DstBufEnd;
            rHwCmd.u4Param[6] = rLParam.u4DstStartAddr;
            rHwCmd.u4Param[7] = rLParam.u4DatLen;

            rHwCmd.u4Param[8] = ((rLParam.eForm) & 0x1) | ((rLParam.eMode << 1) & 0x2);
            rHwCmd.u4Param[9] = 2;
            rHwCmd.u4Param[10] = GCPU_PARAM_PTR;

            pu1WorkBuf = (UINT8 *)(((UINT32)pu1WorkBuf + GCPU_FIFO_ALIGNMENT - 1) & ~(GCPU_FIFO_ALIGNMENT - 1));
            TZ_SWD_InvDCache((UINT32)pu1WorkBuf, GCPU_TSDESC_WORKBUF_SIZE);
        
            rHwCmd.u4Param[11] = (UINT32)pu1WorkBuf;
            rHwCmd.u4Param[12] = prLParam->u4TsState[0];
            rHwCmd.u4Param[13] = prLParam->u4TsState[1];
            rHwCmd.u4Param[14] = prLParam->u4TsState[2];
            rHwCmd.u4Param[15] = prLParam->u4TsState[3];

            // even key
            rHwCmd.u4Param[GCPU_PARAM_PTR + 0] = 0x80000047;
            rHwCmd.u4Param[GCPU_PARAM_PTR + 1] = 0xC0000000;
            rHwCmd.u4Param[GCPU_PARAM_PTR + 2] = (eScheme << 28) | u4EvenKeyPtr;
            rHwCmd.u4Param[GCPU_PARAM_PTR + 3] = 0;

            for(i = 0; i < 4; i++)
            {
                p = rLParam.au1Key[0] + i * 4;
                rHwCmd.u4Param[u4EvenKeyPtr + u4KeyOff + i] = KEY_WRAPPER(p);
            }

            for(i = 0; i < 4; i++)
            {
                p = rLParam.au1Iv[0] + i * 4;
                rHwCmd.u4Param[u4EvenKeyPtr + u4KeyOff + 4 + i] = KEY_WRAPPER(p);
            }

            // odd key
            rHwCmd.u4Param[GCPU_PARAM_PTR + 4] = 0xC0000047;
            rHwCmd.u4Param[GCPU_PARAM_PTR + 5] = 0xC0000000;
            rHwCmd.u4Param[GCPU_PARAM_PTR + 6] = (eScheme << 28) | u4OddKeyPtr;
            rHwCmd.u4Param[GCPU_PARAM_PTR + 7] = 0;

            for(i = 0; i < 4; i++)
            {
                p = rLParam.au1Key[1] + i * 4;
                rHwCmd.u4Param[u4OddKeyPtr + u4KeyOff + i] = KEY_WRAPPER(p);
            }

            for(i = 0; i < 4; i++)
            {
                p = rLParam.au1Iv[1] + i * 4;
                rHwCmd.u4Param[u4OddKeyPtr + u4KeyOff + 4 + i] = KEY_WRAPPER(p);
            }

            TZ_SWD_FlushInvDCache(prLParam->u4SrcBufStart, prLParam->u4SrcBufEnd - prLParam->u4SrcBufStart);
            TZ_SWD_InvDCache(prLParam->u4DstBufStart, prLParam->u4DstBufEnd - prLParam->u4DstBufStart);

            // doing _GCPU_Hw_CmdEx(&rHwCmd, &rIoBuf, u4Flags);
            {
                IOMMU_GCPU_WRITE32(REG_IOMMU_CFG0, 0);

                // Setup Parameter
                for(i = 1; i < GCPU_PARAM_NUM; i++)
                {
                    GCPUCMD_WRITE32(GCPU_REG_MEM_CMD + i, rHwCmd.u4Param[i]);
                }

                // Clear GCPU Interrupt
                GCPUCMD_WRITE32(GCPU_REG_INT_CLR, GCPUCMD_READ32(GCPU_REG_INT_CLR));
                GCPUCMD_WRITE32(GCPU_REG_INT_EN, 0);

                // GCPU Decryption Mode
                GCPUCMD_WRITE32(GCPU_REG_MEM_CMD, rHwCmd.u4Param[0]);

                // GCPU PC
                GCPUCMD_WRITE32(GCPU_REG_PC_CTL, 0x0);

                while((GCPUCMD_READ32(GCPU_REG_INT_SET) & 0x1) == 0x0);
                while((GCPUCMD_READ32(262) & 0x1) == 0x0);
                GCPUCMD_WRITE32(GCPU_REG_INT_CLR, GCPUCMD_READ32(GCPU_REG_INT_CLR));
                GCPUCMD_WRITE32(GCPU_REG_INT_EN, 0);

                // read back param 0 - 31
                for(i = 0; i < 32; i++)
                {
                    rHwCmd.u4Param[i] = GCPUCMD_READ32(GCPU_REG_MEM_CMD + i);
                }

                // read back param 32 - 47
                for(i = GCPU_PARAM_RET_PTR; i < GCPU_PARAM_RET_PTR + GCPU_PARAM_RET_NUM; i++)
                {
                    rHwCmd.u4Param[i] = GCPUCMD_READ32(GCPU_REG_MEM_CMD + i);
                }
            }

            TZ_SWD_InvDCache(prLParam->u4DstBufStart, prLParam->u4DstBufEnd - prLParam->u4DstBufStart);

            prLParam->u4TsState[0] = rHwCmd.u4Param[12];
            prLParam->u4TsState[1] = rHwCmd.u4Param[13];
            prLParam->u4TsState[2] = rHwCmd.u4Param[14];
            prLParam->u4TsState[3] = rHwCmd.u4Param[15];
        }
        break;
    }

    return S_GCPU_OK;
}

INT32 TZ_GCPU_AES_CMD(UINT32 u4Handle, UINT32 u4Id, void* prParam, BOOL fgIOMMU)
{
    UINT32 u4Tmp, i;//fix complie warning , u4Flags = 0;
    GCPU_HW_CMD_T rHwCmd;
    UINT8 *p;
    GCPU_STRUCT_T *prGcpu;

    if(prParam == NULL)
    {
        return E_GCPU_PARAM_WRONG;
    }

    // setup command
    switch(u4Id)
    {
    case GCPU_AES:
        {
            AES_PARAM_T *prLParam = (AES_PARAM_T *)prParam;
            AES_PARAM_T rLPParam = *prLParam;

            if (!_Gcpu_IsAligned(rLPParam.u4SrcBufStart, GCPU_FIFO_ALIGNMENT) ||
                !_Gcpu_IsAligned(rLPParam.u4SrcBufEnd, GCPU_FIFO_ALIGNMENT) ||
                !_Gcpu_IsAligned(rLPParam.u4DstBufStart, GCPU_FIFO_ALIGNMENT) ||
                !_Gcpu_IsAligned(rLPParam.u4DstBufEnd, GCPU_FIFO_ALIGNMENT))
            {
                return E_GCPU_PARAM_WRONG;
            }

            if ((!_GCPU_IsInRegion(rLPParam.u4SrcStartAddr, rLPParam.u4SrcBufStart,
                    rLPParam.u4SrcBufEnd)) ||
                (!_GCPU_IsInRegion(rLPParam.u4DstStartAddr, rLPParam.u4DstBufStart,
                    rLPParam.u4DstBufEnd)))
            {
                return E_GCPU_PARAM_WRONG;
            }

            memset(&rHwCmd, 0, sizeof(GCPU_HW_CMD_T));

            if(u4Handle != 0)
            {
                prGcpu = (GCPU_STRUCT_T *)u4Handle;
                LOG(7, "Use GCPU handler\n");
            }
            else
            {
                prGcpu = NULL;
            }

            // setup command
            switch(rLPParam.eMode)
            {
            case BLOCK_CIPHER_MODE_ECB:
                if(rLPParam.fgEncrypt)
                {
                    rHwCmd.u4Param[0] = AES_EPAK;
                }
                else
                {
                    rHwCmd.u4Param[0] = AES_DPAK;
                }
                break;
            case BLOCK_CIPHER_MODE_CTR:
                // the same operation for encryption and decryption
                rHwCmd.u4Param[0] = AES_CTR;
                break;
            case BLOCK_CIPHER_MODE_OFB:
                // the same operation for encryption and decryption
                rHwCmd.u4Param[0] = AES_OFB;
                break;
            case BLOCK_CIPHER_MODE_CBC:
                if(rLPParam.fgEncrypt)
                {
                    rHwCmd.u4Param[0] = AES_ECBC;
                }
                else
                {
                    rHwCmd.u4Param[0] = AES_DCBC;
                }
                break;

            default:
                return E_GCPU_UNEXPECT;
            }

            // setup parameter
            rHwCmd.u4Param[1] = rLPParam.u4SrcStartAddr;
            rHwCmd.u4Param[2] = rLPParam.u4DstStartAddr;
            if(rLPParam.eMode == BLOCK_CIPHER_MODE_CBC ||
                rLPParam.eMode == BLOCK_CIPHER_MODE_ECB)
            {
                rHwCmd.u4Param[3] = rLPParam.u4DatLen / 16;
            }
            else
            {
                rHwCmd.u4Param[3] = rLPParam.u4DatLen;
            }

            rHwCmd.u4Param[4] = AES_KEY_LEN_WRAPPER(rLPParam.eKeyBitLen);

            if(prGcpu != NULL)
            {
                if(prGcpu->u4AesKeyPtr != 0)
                {
                    if(IsValidSecureSlot(prGcpu->u4AesKeyPtr))
                    {
                        LOG(7, "Use secure aes key ptr\n");
                        rHwCmd.u4Param[5] = prGcpu->u4AesKeyPtr;
                    }
                    else
                    {
                        LOG(3, "Wrong seucre aes key slot ptr\n");
                        rHwCmd.u4Param[5] = GCPU_PARAM_AES_KEY_PTR;
                    }
                }
                else
                {
                    LOG(5, "Use normal aes key slot ptr\n");
                    rHwCmd.u4Param[5] = GCPU_PARAM_AES_KEY_PTR;
                }

                if(prGcpu->u4AesIvPtr != 0)
                {
                    if(IsValidSecureSlot(prGcpu->u4AesIvPtr))
                    {
                        LOG(7, "Use secure aes iv ptr\n");
                        rHwCmd.u4Param[6] = prGcpu->u4AesIvPtr;
                    }
                    else
                    {
                        LOG(3, "Wrong seucre aes iv slot ptr\n");
                        rHwCmd.u4Param[6] = GCPU_PARAM_AES_IV_PTR;
                    }
                }
                else
                {
                    LOG(5, "Use normal aes iv slot ptr\n");
                    rHwCmd.u4Param[6] = GCPU_PARAM_AES_IV_PTR;
                }

                if(prGcpu->u4AesRetIvPtr != 0)
                {
                    if(IsValidSecureSlot(prGcpu->u4AesRetIvPtr))
                    {
                        LOG(7, "Use secure aes ret iv ptr\n");
                        rHwCmd.u4Param[7] = prGcpu->u4AesRetIvPtr;
                    }
                    else
                    {
                        LOG(3, "Wrong seucre aes ret iv slot ptr\n");
                        rHwCmd.u4Param[7] = GCPU_PARAM_AES_RET_IV_PTR;
                    }
                }
                else
                {
                    LOG(5, "Use normal aes ret iv slot ptr\n");
                    rHwCmd.u4Param[7] = GCPU_PARAM_AES_RET_IV_PTR;
                }
            }
            else
            {
                rHwCmd.u4Param[5] = GCPU_PARAM_AES_KEY_PTR;
                rHwCmd.u4Param[6] = GCPU_PARAM_AES_IV_PTR;
                rHwCmd.u4Param[7] = GCPU_PARAM_AES_RET_IV_PTR;
            }

            rHwCmd.u4Param[8] = rLPParam.u4SrcBufStart;
            rHwCmd.u4Param[9] = rLPParam.u4SrcBufEnd;
            rHwCmd.u4Param[10] = rLPParam.u4DstBufStart;
            rHwCmd.u4Param[11] = rLPParam.u4DstBufEnd;

            //key
            for(i = 0; i < GCPU_PARAM_AES_KEY_NUM; i++)
            {
                p = rLPParam.au1Key + i * 4;
                rHwCmd.u4Param[GCPU_PARAM_AES_KEY_PTR + i] = KEY_WRAPPER(p);
            }

            //iv
            for(i = 0; i < GCPU_PARAM_AES_IV_NUM; i++)
            {
                p = rLPParam.au1Iv + i * 4;
                rHwCmd.u4Param[GCPU_PARAM_AES_IV_PTR + i] = KEY_WRAPPER(p);
            }

            TZ_SWD_FlushInvDCache(prLParam->u4SrcBufStart, prLParam->u4SrcBufEnd - prLParam->u4SrcBufStart);
            
            if (prLParam->u4DstBufStart != prLParam->u4SrcBufStart)
            {
                TZ_SWD_FlushInvDCache(prLParam->u4DstBufStart, prLParam->u4DstBufEnd - prLParam->u4DstBufStart);
            }

            // doing _GCPU_Hw_CmdEx(&rHwCmd, &rIoBuf, u4Flags);
            {
                IOMMU_GCPU_WRITE32(REG_IOMMU_CFG0, 0);

                // Setup Parameter
                for(i = 1; i < GCPU_PARAM_NUM; i++)
                {
                    GCPUCMD_WRITE32(GCPU_REG_MEM_CMD + i, rHwCmd.u4Param[i]);
                }

                // Clear GCPU Interrupt
                GCPUCMD_WRITE32(GCPU_REG_INT_CLR, GCPUCMD_READ32(GCPU_REG_INT_CLR));
                GCPUCMD_WRITE32(GCPU_REG_INT_EN, 0);

                // GCPU Decryption Mode
                GCPUCMD_WRITE32(GCPU_REG_MEM_CMD, rHwCmd.u4Param[0]);

                // GCPU PC
                GCPUCMD_WRITE32(GCPU_REG_PC_CTL, 0x0);

                while((GCPUCMD_READ32(GCPU_REG_INT_SET) & 0x1) == 0x0);
                while((GCPUCMD_READ32(262) & 0x1) == 0x0);
                GCPUCMD_WRITE32(GCPU_REG_INT_CLR, GCPUCMD_READ32(GCPU_REG_INT_CLR));
                GCPUCMD_WRITE32(GCPU_REG_INT_EN, 0);

                // read back param 0 - 31
                for(i = 0; i < 32; i++)
                {
                    rHwCmd.u4Param[i] = GCPUCMD_READ32(GCPU_REG_MEM_CMD + i);
                }

                // read back param 32 - 47
                for(i = GCPU_PARAM_RET_PTR; i < GCPU_PARAM_RET_PTR + GCPU_PARAM_RET_NUM; i++)
                {
                    rHwCmd.u4Param[i] = GCPUCMD_READ32(GCPU_REG_MEM_CMD + i);
                }
            }

            TZ_SWD_InvDCache(prLParam->u4DstBufStart, prLParam->u4DstBufEnd - prLParam->u4DstBufStart);

            //return result
            for(i = 0; i < GCPU_PARAM_AES_IV_NUM; i++)
            {
                u4Tmp = rHwCmd.u4Param[GCPU_PARAM_AES_RET_IV_PTR + i];
                prLParam->au1Iv[i * 4] = (u4Tmp & 0xff);
                prLParam->au1Iv[i * 4 + 1] = ((u4Tmp >> 8) & 0xff);
                prLParam->au1Iv[i * 4 + 2] = ((u4Tmp >> 16) & 0xff);
                prLParam->au1Iv[i * 4 + 3] = ((u4Tmp >> 24) & 0xff);
            }
        }
        break;
    }

    return S_GCPU_OK;
}


INT32 TZ_PLAYREADY_AES_CTR_Ex(UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len, UINT8 *pu1Ctr, UINT8 *pu1RetCtr, BOOL fgAud)
{
    AES_PARAM_T rAes;
    INT32 i4Ret;
    UINT8 i;
    static GCPU_STRUCT_T rGcpu;
    static BOOL fgInited = FALSE;

    rAes.u4SrcStartAddr = (UINT32)pu1Src;
    rAes.u4SrcBufStart = GCPU_LINER_BUFFER_START((UINT32)pu1Src);
    rAes.u4SrcBufEnd = GCPU_LINER_BUFFER_END((UINT32)pu1Src + u4Len);
    rAes.u4DstStartAddr = (UINT32)pu1Dst;
    rAes.u4DstBufStart = GCPU_LINER_BUFFER_START((UINT32)pu1Dst);
    rAes.u4DstBufEnd = GCPU_LINER_BUFFER_END((UINT32)pu1Dst + u4Len);
    rAes.u4DatLen = u4Len;
    memcpy_s2s_chk(rAes.au1Iv, pu1Ctr, 16, NULL);
    rAes.eKeyBitLen = KEY_BIT_LEN_128;
    rAes.eMode = BLOCK_CIPHER_MODE_CTR;

    if(!fgInited)
    {
        memset(&rGcpu, 0, sizeof(GCPU_STRUCT_T));
        fgInited = TRUE;
    }

    if(fgAud)
    {
        rGcpu.u4AesKeyPtr = AES_PLAYREADY_AUD_KEY_PTR;
    }
    else
    {
        rGcpu.u4AesKeyPtr = AES_PLAYREADY_KEY_PTR;
    }

    i4Ret = TZ_GCPU_AES_CMD((UINT32)&rGcpu, GCPU_AES, &rAes, FALSE);

    for(i = 0; i < 16; i++)
    {
        pu1RetCtr[i] = rAes.au1Iv[i];
    }

    return i4Ret;
}

INT32 TZ_PLAYREADY_AES_CTR(UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len, UINT8 *pu1Ctr, UINT8 *pu1RetCtr)
{
    return TZ_PLAYREADY_AES_CTR_Ex(pu1Src, pu1Dst, u4Len, pu1Ctr, pu1RetCtr, FALSE);
}


INT32 TZ_SVP_AES_CTR(UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len, UINT8 *pu1Ctr, UINT8 *pu1RetCtr)
{
    AES_PARAM_T rAes;
    INT32 i4Ret;
    UINT8 i;
    static GCPU_STRUCT_T rGcpu;
    static BOOL fgInited = FALSE;

    rAes.u4SrcStartAddr = (UINT32)pu1Src;
    rAes.u4SrcBufStart = GCPU_LINER_BUFFER_START((UINT32)pu1Src);
    rAes.u4SrcBufEnd = GCPU_LINER_BUFFER_END((UINT32)pu1Src + u4Len);
    rAes.u4DstStartAddr = (UINT32)pu1Dst;
    rAes.u4DstBufStart = GCPU_LINER_BUFFER_START((UINT32)pu1Dst);
    rAes.u4DstBufEnd = GCPU_LINER_BUFFER_END((UINT32)pu1Dst + u4Len);
    rAes.u4DatLen = u4Len;
    memcpy_s2s_chk(rAes.au1Iv, pu1Ctr, 16, NULL);
    rAes.eKeyBitLen = KEY_BIT_LEN_128;
    rAes.eMode = BLOCK_CIPHER_MODE_CTR;

    if(!fgInited)
    {
        memset(&rGcpu, 0, sizeof(GCPU_STRUCT_T));
        rGcpu.u4AesKeyPtr = AES_SVP_CTR_KEY_PTR;

        fgInited = TRUE;
    }

    i4Ret = TZ_GCPU_AES_CMD((UINT32)&rGcpu, GCPU_AES, &rAes, FALSE);

    for(i = 0; i < 16; i++)
    {
        pu1RetCtr[i] = rAes.au1Iv[i];
    }

    return i4Ret;
}

BOOL TZ_GCPU_AES_CBC(const UINT8 *pu1Key, UINT32 u4Keylen, 
                    const UINT8 *pu1Iv, UINT8 *pu1RetIv,
                    const UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len,
                    BOOL fgEncrypt)
{
    AES_PARAM_T rAes = {0};
    KEY_BIT_LEN eKeyLen;
    BOOL fgRet = TRUE;
    UINT8 i;

    if (!pu1Key || !pu1Iv || !pu1RetIv || !pu1Src || !pu1Dst)
    {
        return FALSE;
    }

    switch (u4Keylen)
    {
    case 16:
        eKeyLen = KEY_BIT_LEN_128;
        break;
    case 24:
        eKeyLen = KEY_BIT_LEN_192;
        break;
    case 32:
        eKeyLen = KEY_BIT_LEN_256;
        break;
    default:
        return FALSE;
    }

    memcpy(rAes.au1Key, pu1Key, u4Keylen);
    memcpy(rAes.au1Iv, pu1Iv, 16);

    rAes.u4SrcStartAddr = (UINT32)pu1Src;
    rAes.u4SrcBufStart = GCPU_LINER_BUFFER_START((UINT32)pu1Src);
    rAes.u4SrcBufEnd = GCPU_LINER_BUFFER_END((UINT32)pu1Src + u4Len);
    rAes.u4DstStartAddr = (UINT32)pu1Dst;
    rAes.u4DstBufStart = GCPU_LINER_BUFFER_START((UINT32)pu1Dst);
    rAes.u4DstBufEnd = GCPU_LINER_BUFFER_END((UINT32)pu1Dst + u4Len);
    rAes.u4DatLen = u4Len;
    rAes.eKeyBitLen = eKeyLen;
    rAes.eMode = BLOCK_CIPHER_MODE_CBC;
    rAes.fgEncrypt = fgEncrypt;

    if (S_GCPU_OK != TZ_GCPU_AES_CMD(0, GCPU_AES, &rAes, FALSE))
    {
        fgRet = FALSE;
    }
    else
    {
        if (pu1RetIv)
        {
            for(i = 0; i < 16; i++)
            {
                pu1RetIv[i] = rAes.au1Iv[i];
            }
        }
    }

    return fgRet;
}

BOOL TZ_GCPU_SECURE_AES_CBC(const UINT32 u4KeyPtr, UINT32 u4Keylen,
                        const UINT32 u4IvPtr,
                        const UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len,
                        BOOL fgEncrypt)
{
    AES_PARAM_T rAes = {0};
    GCPU_STRUCT_T rGcpu = {0};
    KEY_BIT_LEN eKeyLen;
    BOOL fgRet = TRUE;
    UINT8 i;

    if (!pu1Src || !pu1Dst)
    {
        return FALSE;
    }
                    
    if (!IsValidSecureSlot(u4KeyPtr))
    {
        return FALSE;
    }

    switch (u4Keylen)
    {
    case 16:
        eKeyLen = KEY_BIT_LEN_128;
        break;
    case 24:
        eKeyLen = KEY_BIT_LEN_192;
        break;
    case 32:
        eKeyLen = KEY_BIT_LEN_256;
        break;
    default:
        return FALSE;
    }

    rGcpu.u4AesKeyPtr = u4KeyPtr;
    rGcpu.u4AesIvPtr = u4IvPtr;
    rGcpu.u4AesRetIvPtr = u4IvPtr;

    rAes.u4SrcStartAddr = (UINT32)pu1Src;
    rAes.u4SrcBufStart = GCPU_LINER_BUFFER_START((UINT32)pu1Src);
    rAes.u4SrcBufEnd = GCPU_LINER_BUFFER_END((UINT32)pu1Src + u4Len);
    rAes.u4DstStartAddr = (UINT32)pu1Dst;
    rAes.u4DstBufStart = GCPU_LINER_BUFFER_START((UINT32)pu1Dst);
    rAes.u4DstBufEnd = GCPU_LINER_BUFFER_END((UINT32)pu1Dst + u4Len);
    rAes.u4DatLen = u4Len;
    rAes.eKeyBitLen = eKeyLen;
    rAes.eMode = BLOCK_CIPHER_MODE_CBC;
    rAes.fgEncrypt = fgEncrypt;

    if (S_GCPU_OK != TZ_GCPU_AES_CMD((UINT32)&rGcpu, GCPU_AES, &rAes, FALSE))
    {
        fgRet = FALSE;
    }

    return fgRet;
}

BOOL TZ_GCPU_DECRYPT_TS(const UINT8 *pu1Key, UINT32 u4Keylen, const UINT8 *pu1Iv,
                        const UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len, 
                        TSDESC_CIPHER_MODE eCipher, BOOL fgTTS)
{
    TSDESC_PARAM_T rTSParam = {0};
    KEY_BIT_LEN eKeyLen;
    BOOL fgRet = TRUE;
    UINT32 i;

    if (!pu1Key || !pu1Src || !pu1Dst)
    {
        return FALSE;
    }

    switch (u4Keylen)
    {
    case 16:
        eKeyLen = KEY_BIT_LEN_128;
        break;
    default:
        return FALSE;
    }


    rTSParam.eCipher = eCipher;
    rTSParam.eKeyBitLen = eKeyLen;
    rTSParam.eForm = fgTTS ? TSDESC_192_PACKET_FORM : TSDESC_188_PACKET_FORM;
    rTSParam.eMode = TSDESC_FIRST_PACKET_MODE;

    for (i = 0; i < u4Keylen; i++)
    {
        rTSParam.au1Key[0][i] = pu1Key[i]; 
        rTSParam.au1Key[1][i] = pu1Key[i];
    }

    if (pu1Iv)
    {
        for (i = 0; i < 16; i++)
        {
            rTSParam.au1Iv[0][i] = pu1Iv[i];
            rTSParam.au1Iv[1][i] = pu1Iv[i];
        }
    }

    rTSParam.u4SrcStartAddr = (UINT32)pu1Src;
    rTSParam.u4SrcBufStart = GCPU_LINER_BUFFER_START((UINT32)pu1Src);
    rTSParam.u4SrcBufEnd = GCPU_LINER_BUFFER_END((UINT32)pu1Src + u4Len);
    rTSParam.u4DstStartAddr = (UINT32)pu1Dst;
    rTSParam.u4DstBufStart = GCPU_LINER_BUFFER_START((UINT32)pu1Dst);
    rTSParam.u4DstBufEnd = GCPU_LINER_BUFFER_END((UINT32)pu1Dst + u4Len);
    rTSParam.u4DatLen = u4Len;
    rTSParam.u4TsState[0] = 0;
    rTSParam.u4TsState[1] = 0;
    rTSParam.u4TsState[2] = 0;
    rTSParam.u4TsState[3] = 0;

    if (S_GCPU_OK != TZ_GCPU_MISC_CMD(0, GCPU_TSDESC, &rTSParam, FALSE))
    {
        fgRet = FALSE;
    }

    return fgRet;
}


#if 0
INT32 TZ_SVP_AES_CBC(UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len, 
        BOOL fgEncrypt, UINT8 *pu1Iv, UINT8 *pu1RetIv)
{
    AES_PARAM_T rAes;
    INT32 i4Ret;
    UINT8 i;
    static GCPU_STRUCT_T rGcpu;
    static BOOL fgInited = FALSE;

    rAes.u4SrcStartAddr = (UINT32)pu1Src;
    rAes.u4SrcBufStart = GCPU_LINER_BUFFER_START((UINT32)pu1Src);
    rAes.u4SrcBufEnd = GCPU_LINER_BUFFER_END((UINT32)pu1Src + u4Len);
    rAes.u4DstStartAddr = (UINT32)pu1Dst;
    rAes.u4DstBufStart = GCPU_LINER_BUFFER_START((UINT32)pu1Dst);
    rAes.u4DstBufEnd = GCPU_LINER_BUFFER_END((UINT32)pu1Dst + u4Len);
    rAes.u4DatLen = u4Len;
    memcpy(rAes.au1Iv, pu1Iv, 16);
    rAes.eKeyBitLen = KEY_BIT_LEN_128;
    rAes.eMode = BLOCK_CIPHER_MODE_CBC;
    rAes.fgEncrypt = fgEncrypt;

    if(!fgInited)
    {
        memset(&rGcpu, 0, sizeof(GCPU_STRUCT_T));
        rGcpu.u4AesKeyPtr = AES_SVP_CBC_KEY_PTR;

        fgInited = TRUE;
    }

    i4Ret = TZ_GCPU_AES_CMD((UINT32)&rGcpu, GCPU_AES, &rAes, FALSE);

    for(i = 0; i < 16; i++)
    {
        pu1RetIv[i] = rAes.au1Iv[i];
    }

    return i4Ret;
}
#endif
