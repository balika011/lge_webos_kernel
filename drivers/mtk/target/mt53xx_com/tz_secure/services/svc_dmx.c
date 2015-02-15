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
#include "dmx.h"
#include "dmx_mm.h"

#if defined(CC_MT5890)
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------
#define DATA_HEADER_LEN_MAX     4
#define DWORDSWAP(u4Tmp) (((u4Tmp & 0xff) << 24) | ((u4Tmp & 0xff00) << 8) | ((u4Tmp & 0xff0000) >> 8) | ((u4Tmp & 0xff000000) >> 24))

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define SECURE_PLAYBACK_PIDINDEX(pidx)         \
    (((pidx) >= 64)? 2 :((pidx) >= 32)? 1 :0)
	
#define SECUREPIDINDEX(pidx)         \
    (((pidx) >= 64)? ((pidx)-64) :((pidx) >= 32)? ((pidx)-32) : (pidx))

#define PID_BUFFER_SECURE_CONTROL(pidx)   \
    ((volatile UINT32*)(DMX_PIDBUFF_SECURE_CONTROL_BASE ))[SECURE_PLAYBACK_PIDINDEX(pidx)]

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
extern UINT32 TZ_DRAM_START;
extern UINT32 TZ_DRAM_END;

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static _DMX_MMSinature_Check(DMX_MMSECURITY_STRUCT_T *prDataInfo)
{
    UINT32 u4BufStart, u4BufEnd, u4Addr, u4Size, u4SkipLen;
    UINT32 u4DataHeader;
    UINT8 au1Buf[DATA_HEADER_LEN_MAX] = {0};
    UINT8 u1TsIdx;

    if (!prDataInfo)
    {
        return FALSE;
    }

    u4SkipLen = prDataInfo->u4SkipLen;
    u4BufStart = prDataInfo->u4BufAddr;
    u4BufEnd = prDataInfo->u4BufEnd;
    u4Addr = prDataInfo->u4SendAddr + u4SkipLen;
    u4Size = prDataInfo->u4SendSize;
    
    u1TsIdx = prDataInfo->u1TsIdx;

    if(u1TsIdx >= DMX_FRAMER_COUNT)
    {
        return FALSE;
    }
	// Check Buff range
	if (_chk_range_in_secure((void*)u4Addr, u4Size))
	{
        u4Size = DATA_HEADER_LEN_MAX;
        if (((u4Addr + u4Size) - 1) >= u4BufEnd)
    	{
    		UINT8 u1Len = (UINT8)(u4BufEnd - u4Addr);
    		if(u1Len >= u4Size)
    		{
    			return FALSE;
    		}
    		TZ_SWD_InvDCache(u4Addr, u1Len);
    		TZ_SWD_InvDCache(u4BufStart, (u4Size - u1Len));
    		memcpy_s2s_chk((void*)au1Buf, (void*)u4Addr, u1Len, NULL);
    		memcpy_s2s_chk((void*)(au1Buf + u1Len), (void*)u4BufStart, (u4Size - u1Len), NULL);
    		TZ_SWD_InvDCache(u4Addr, u1Len);
    		TZ_SWD_InvDCache(u4BufStart, (u4Size - u1Len));
    	}
    	else
    	{
    		TZ_SWD_InvDCache(u4Addr, u4Size);
    		memcpy_s2s_chk((void*)au1Buf, (void*)u4Addr, u4Size, NULL);
    		TZ_SWD_InvDCache(u4Addr, u4Size);
    	}
        memcpy_s2s_chk((void*)(&u4DataHeader), (void*)au1Buf, DATA_HEADER_LEN_MAX, NULL);
        u4DataHeader = DWORDSWAP(u4DataHeader);
	}
    else
    {
        u4Size = DATA_HEADER_LEN_MAX;
        if (((u4Addr + u4Size) - 1) >= u4BufEnd)
    	{
    		UINT8 u1Len = (UINT8)(u4BufEnd - u4Addr);
    		if(u1Len >= u4Size)
    		{
    			return FALSE;
    		}
    		TZ_SWD_InvDCache(u4Addr, u1Len);
    		TZ_SWD_InvDCache(u4BufStart, (u4Size - u1Len));
    		memcpy_n2s_chk((void*)au1Buf, (void*)u4Addr, u1Len, NULL);
    		memcpy_n2s_chk((void*)(au1Buf + u1Len), (void*)u4BufStart, (u4Size - u1Len), NULL);
    		TZ_SWD_InvDCache(u4Addr, u1Len);
    		TZ_SWD_InvDCache(u4BufStart, (u4Size - u1Len));
    	}
    	else
    	{
    		TZ_SWD_InvDCache(u4Addr, u4Size);
    		memcpy_n2s_chk((void*)au1Buf, (void*)u4Addr, u4Size, NULL);
    		TZ_SWD_InvDCache(u4Addr, u4Size);
    	}
        memcpy_s2s_chk((void*)(&u4DataHeader), (void*)au1Buf, DATA_HEADER_LEN_MAX, NULL);
        u4DataHeader = DWORDSWAP(u4DataHeader) + 1; //First 4 bytes of bit-stream +1
    }
    
    *(volatile UINT32*)(DMX_DMEM_MM_SECURE_BASE + (4 * (u1TsIdx + 1))) = u4DataHeader;
    
    return TRUE;
}

static BOOL _DMX_PVRPlay_SetSingleMove(DMX_MMSECURITY_STRUCT_T *prDataInfo)
{
    UINT8 u1TsOutIdx;
    UINT32 u4PhyBufStart, u4PhyBufEnd, u4BufSize, u4PhyRp, u4PhyWp, u4FrameSize;

    if (!prDataInfo)
    {
        return FALSE;
    }

    u1TsOutIdx = prDataInfo->u1TsOutIdx;
    u4PhyBufStart = prDataInfo->u4BufAddr;
    u4PhyBufEnd = prDataInfo->u4BufEnd;
    u4BufSize = u4PhyBufEnd - u4PhyBufStart;
    u4PhyRp = prDataInfo->u4SendAddr;
    u4PhyWp = prDataInfo->u4SendAddr + prDataInfo->u4SendSize;
    if (u4PhyWp >= u4PhyBufEnd)
    {
        u4PhyWp -= u4BufSize;
    }
    u4FrameSize = prDataInfo->u4SendSize;
    
    if (u1TsOutIdx == 0)
    {
        DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_START, u4PhyBufStart);
        DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_END, u4PhyBufEnd - 1);
        DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_RP, u4PhyRp);
        DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_WP, u4PhyWp);
        DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_THRESHOLD, u4FrameSize);
    }
    else  if (u1TsOutIdx == 1)
    {
        DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_START2, u4PhyBufStart);
        DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_END2, u4PhyBufEnd - 1);
        DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_RP2, u4PhyRp);
        DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_WP2, u4PhyWp);
        DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_THRESHOLD2, u4FrameSize);
    }
	else
	{
	    DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_START3, u4PhyBufStart);
        DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_END3, u4PhyBufEnd - 1);
        DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_RP3, u4PhyRp);
        DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_WP3, u4PhyWp);
        DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_THRESHOLD3, u4FrameSize);
    }

    return TRUE;
}

static BOOL _DMX_DDI_SetSingleMove(DMX_MMSECURITY_STRUCT_T *prDataInfo)
{
    UINT8 u1TsOutIdx;
    UINT32 u4PhyBufStart, u4PhyBufEnd, u4BufSize, u4PhyRp, u4PhyWp;

    if (!prDataInfo)
    {
        return FALSE;
    }

    u4PhyBufStart = prDataInfo->u4BufAddr;
    u4PhyBufEnd = prDataInfo->u4BufEnd;
    u4BufSize = u4PhyBufEnd - u4PhyBufStart;
    u4PhyRp = prDataInfo->u4SendAddr;
    u4PhyWp = prDataInfo->u4SendAddr + prDataInfo->u4SendSize;
    if (u4PhyWp >= u4PhyBufEnd)
    {
        u4PhyWp -= u4BufSize;
    }
    
    // Set DDI ring buffer
    DDI_WRITE32(DDI_REG_DMA_BUF_START, u4PhyBufStart);      // Start pointer
    DDI_WRITE32(DDI_REG_DMA_BUF_END, u4PhyBufEnd);          // End pointer
    DDI_WRITE32(DDI_REG_DMA_RP_INIT, u4PhyRp);           // Initial RP
    DDI_WRITE32(DDI_REG_DMA_WP, u4PhyWp);                // Write pointer
    // DDI_WRITE32(DDI_REG_DMA_AP, u4PhyBufWp);             // Alert pointer

    return TRUE;
}

static BOOL _DMX_EnablePidBufferSecureControl(BOOL bEnable, DMX_MMSECURITY_STRUCT_T *prDataInfo)
{
    UINT8 u1PidIdx;
    UINT32 u4PhyBufStart, u4PhyBufEnd, u4BufSize;
    
    if (!prDataInfo)
    {
        return FALSE;
    }

    u1PidIdx = prDataInfo->u1PidIdx;

    if(bEnable == TRUE)
    {
        u4PhyBufStart = prDataInfo->u4BufAddr;
        u4PhyBufEnd = prDataInfo->u4BufEnd;
        u4BufSize = u4PhyBufEnd - u4PhyBufStart;
        
        PID_S_W(u1PidIdx, 5) = u4PhyBufStart;         // FIFO start
        PID_S_W(u1PidIdx, 7) = u4PhyBufStart;         // PES_start
        PID_S_W(u1PidIdx, 8) = u4PhyBufStart;         // Write pointer
        PID_S_W(u1PidIdx, 9) = u4PhyBufStart;         // Read pointer
        PID_S_W(u1PidIdx, 6) = u4PhyBufEnd - 1; // End pointer
        if (_chk_range_in_secure((void*)u4PhyBufStart, u4BufSize))
        {
            PID_BUFFER_SECURE_CONTROL(u1PidIdx) |= (1 << SECUREPIDINDEX(u1PidIdx));
        }
    }
    else
    {
        PID_S_W(u1PidIdx, 5) = 0;         // FIFO start
        PID_S_W(u1PidIdx, 7) = 0;         // PES_start
        PID_S_W(u1PidIdx, 8) = 0;         // Write pointer
        PID_S_W(u1PidIdx, 9) = 0;         // Read pointer
        PID_S_W(u1PidIdx, 6) = 0; // End pointer
        PID_BUFFER_SECURE_CONTROL(u1PidIdx) &= ~(1 << SECUREPIDINDEX(u1PidIdx));
    }
    return TRUE;
}
//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------
BOOL Svc_DMX_MMSecurityInit(VOID)
{
    //Init Secure PlayBack Pid buff
    DMX_SECURE_PLAYBACK_PIDINDEX0 = 0x0;
    DMX_SECURE_PLAYBACK_PIDINDEX1 = 0x0;
    DMX_SECURE_PLAYBACK_PIDINDEX2 = 0x0;

    //Enable MM Signature check
    DMX_MM_SECURE_W(0) = 0x1;
    DMXCMD_WRITE32(DMX_REG_SECURE_DRAM_START_ADDR, PHYSICAL(TZ_DRAM_START)); //start
    DMXCMD_WRITE32(DMX_REG_SECURE_DRAM_END_ADDR, PHYSICAL(TZ_DRAM_END)); //end
    DMXCMD_WRITE32(DMX_REG_SECURE_PLAYBACK_PATTERN, 0x00); //pattern
    return TRUE;
}
BOOL SvcEx_DMX_SetMMSecurity(UINT8 *pu1Param)
{
    DMX_MMSECURITY_STRUCT_T *prDataInfo = (DMX_MMSECURITY_STRUCT_T *)pu1Param;
    DMX_MMSECURITY_SET_TYPE_T  e_SetType;

    if (!prDataInfo)
    {
        return FALSE;
    }
    
    e_SetType = prDataInfo->e_SetType;
    switch(e_SetType)
    {
        case DMX_MMSECURITY_MOVEDATA_DDI:
            _DMX_MMSinature_Check(prDataInfo);
            _DMX_DDI_SetSingleMove(prDataInfo);
            break;
        case DMX_MMSECURITY_MOVEDATA_PVR:
            _DMX_MMSinature_Check(prDataInfo);
            _DMX_PVRPlay_SetSingleMove(prDataInfo);
            break;
        case DMX_MMSECURITY_SETBUFFER:
            _DMX_EnablePidBufferSecureControl(TRUE, prDataInfo);
            break;
        case DMX_MMSECURITY_FREEBUFFER:
            _DMX_EnablePidBufferSecureControl(FALSE, prDataInfo);
            break;
        default:
            return FALSE;
    }
    
    return TRUE;
}
#else
BOOL Svc_DMX_MMSecurityInit(VOID)
{
    //not support
    return TRUE;
}
BOOL SvcEx_DMX_SetMMSecurity(UINT8 *pu1Param)
{
    //not support
    return TRUE;
}
#endif//DMX_DMEM_MM_SECURE_BASE
