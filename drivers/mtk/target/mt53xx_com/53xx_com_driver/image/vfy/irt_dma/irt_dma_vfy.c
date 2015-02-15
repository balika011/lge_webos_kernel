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

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------
//#include <stdlib.h>
//#include "sys_config.h"
#include "drv_config.h"
#include "x_printf.h"
#include "x_debug.h"
#include "drv_common.h"
#include "drv_thread.h"
#include "x_common.h"
#include "x_os.h"
//#include "x_bim.h"
#include "x_timer.h"
#if CONFIG_SYS_MEM_PHASE2
#include "x_mem_phase2.h"
#elif CONFIG_SYS_MEM_PHASE3
#include "x_kmem.h"
#endif
#include "x_rand.h"
#include "x_stl_lib.h"

#include "irt_dma_hw.h"
#include "irt_dma_sw.h"
#include "irt_dma_vfy.h"

//performance test
#if IRT_HW_SPEED_TEST
extern UINT32 _u4IRTHwSpeed_Cnt;
extern UINT64 _u8IRTHwSpeed_Time;
extern HAL_TIME_T  _IRTHwSpeed_Calc;

#define IRT_HW_SPEED_TEST_LOOP 100
#endif

volatile UINT32  g_u4IrtRandomData = 1;
volatile UINT32  g_u4IrtRandomSeed = 0;
volatile BOOL    g_fgIrtSrcDataInit = FALSE;
volatile BOOL    g_fgIrtDmaIsInit   = FALSE;
IRT_VFY_MEM_T g_rIrtVfyMem = {NULL};
IRT_IRT_EV_EXEC_FUNC g_prIrtExecFunc = NULL;
EV_IRT_INPUT_PARA_T  g_rIrtPara = {{0}, 0};
IRT_VFY_SRC_MODE_T   g_rIrtSrcMode = {0, 0, 1, 16, 32};
volatile BOOL    g_fgIrtThreadRunning = FALSE;


UINT32  _u4IrtDmaWH[36][2] = {
                  {16,32},
                  {2032,2016},
                  {64,32},
                  {1984,2016},
                  {1920,1080},
                  {960,540},
                  {480,270},
                  {1440,1080},
                  {720,540},
                  {360,270},
                  {1280,720},
                  {640,360},
                  {320,180},
                  {720,480},
                  {360,240},
                  {180,120},
                  {720,576},
                  {360,288},
                  {180,144},
                  {640,480},
                  {512,288},
                  {384,288},
                  {426,240},
                  {320,240},
                  {480,270},
                  {576,432},
                  {720,384},
                  {512,384},
                  {568,320},
                  {340,192},
                  {284,160},
                  {212,100},
                  {352,288},
                  {176,144},
                  {64,64},
                  {80,60}
                  };



/*
 *TV vfy alway enable it
 *BD disable it
 */
#define IRT_HUGEMEM_EN 1

#if IRT_HUGEMEM_EN
typedef struct _IRT_HUGEMEM_T {
   BOOL fgHMemEn;
   UINT32 i4GloalSA;
   UINT32 i4CurSA;
}IRT_HUGEMEM_T;
static IRT_HUGEMEM_T gHugeMemT;
#endif

/*************************************************************
*
*IRT_DMA Init Start Address
*
**************************************************************/
VOID *IrtDmaAlloc(UINT32 u4Size, UINT32 u4Align)
{
    VOID *p;

#if IRT_HUGEMEM_EN
	if(gHugeMemT.fgHMemEn == FALSE) {
	   gHugeMemT.i4GloalSA = (UINT32)BSP_AllocVfyReserveMemory(0, 2048, 100*1024*1024);
	   IRT_Printf(0, "HugeMem Gloal  = 0x%0x, PA 0x%0x \n",
		   	gHugeMemT.i4GloalSA, PHYSICAL(gHugeMemT.i4GloalSA));

	   gHugeMemT.i4CurSA = gHugeMemT.i4GloalSA;

	   gHugeMemT.fgHMemEn = TRUE;
	}

	p = (gHugeMemT.i4CurSA + u4Align-1)/u4Align*u4Align;

	gHugeMemT.i4CurSA += (u4Size + u4Align);

	/*IRT_Printf(0, "HugeMem Ret = 0x%0p , NextSA 0x%x \n",
		p, gHugeMemT.i4CurSA);
     */

#else /*BD code */

#if CONFIG_SYS_MEM_PHASE2
    p = (UINT8 *)x_alloc_aligned_mem(u4Size, u4Align);
#elif CONFIG_SYS_MEM_PHASE3
    p = (UINT8 *)x_mem_aligned_alloc(u4Size, u4Align);
#endif

#endif

    x_memset(p, 0, u4Size);

    return p;
}



#define DRAM1_REG_OFFSET                    (IO_VIRT + 0x6200)

/*clear dram setting, dram may protect some agent*/
void vIRTDma_ResetDramSetting(void)
{
    UINT32 u4Base = 0x0000;
    UINT32 u4Temp = 0;
    u4Base = DRAM1_REG_OFFSET;
    for (u4Temp = 0; u4Temp < 42; u4Temp ++)
    {
		*(volatile UINT32 *)( DRAM1_REG_OFFSET+u4Temp * 4) = 0;
    }
}


static VOID  vIrtDmaEvInit(VOID)
{
    IRT_APIEntryEx("%d", g_fgIrtDmaIsInit);

    if (g_fgIrtDmaIsInit)
    {
        IRT_DMA_HwReset();
    }
    else
    {
        IRT_DMA_HwInit();
    }

	vIRTDma_ResetDramSetting();

    if (!g_fgIrtDmaIsInit)
    {
        EV_IRTDMA_SRC_Y_SA    = (UINT32 *)IrtDmaAlloc(EV_MEM_SLOT_SIZE, 1024);
        EV_IRTDMA_SRC_C_SA    = (UINT32 *)IrtDmaAlloc(EV_MEM_SLOT_SIZE, 1024);
        EV_IRTDMA_MB420_SRC_Y_SA = (UINT32 *)IrtDmaAlloc(EV_MEM_SLOT_SIZE, 1024);
        EV_IRTDMA_MB420_SRC_C_SA = (UINT32 *)IrtDmaAlloc(EV_MEM_SLOT_SIZE, 1024);
        EV_IRTDMA_5351_SRC_Y_SA  = (UINT32 *)IrtDmaAlloc(EV_MEM_SLOT_SIZE, 1024);
        EV_IRTDMA_5351_SRC_C_SA  = (UINT32 *)IrtDmaAlloc(EV_MEM_SLOT_SIZE, 1024);

        EV_IRTDMA_HW_DST_Y_SA    = (UINT32 *)IrtDmaAlloc(EV_MEM_SLOT_SIZE, 1024);  //16M for bigger bmp.
        EV_IRTDMA_HW_DST_C_SA    = (UINT32 *)IrtDmaAlloc(EV_MEM_SLOT_SIZE, 1024);
        EV_IRTDMA_SW_DST_Y_SA    = (UINT32 *)IrtDmaAlloc(EV_MEM_SLOT_SIZE, 1024);
        EV_IRTDMA_SW_DST_C_SA    = (UINT32 *)IrtDmaAlloc(EV_MEM_SLOT_SIZE, 1024);
        EV_IRTDMA_SW_DST_TEMP_SA = (UINT32 *)IrtDmaAlloc(EV_MEM_SLOT_SIZE, 1024);

        VERIFY(EV_IRTDMA_SRC_Y_SA     != NULL);
        VERIFY(EV_IRTDMA_SRC_C_SA     != NULL);
        VERIFY(EV_IRTDMA_HW_DST_Y_SA     != NULL);
        VERIFY(EV_IRTDMA_HW_DST_C_SA     != NULL);
        VERIFY(EV_IRTDMA_SW_DST_Y_SA     != NULL);
        VERIFY(EV_IRTDMA_SW_DST_C_SA     != NULL);
        VERIFY(EV_IRTDMA_SW_DST_TEMP_SA  != NULL);

        EV_IRTDMA_HW_SRC_Y_SA = EV_IRTDMA_SRC_Y_SA;
        EV_IRTDMA_HW_SRC_C_SA = EV_IRTDMA_SRC_C_SA;

        IRT_Printf(0, "EV_IRTDMA_SRC_Y_SA       = 0x%0x \n", EV_IRTDMA_SRC_Y_SA);
        IRT_Printf(0, "EV_IRTDMA_SRC_C_SA       = 0x%0x \n", EV_IRTDMA_SRC_C_SA);
        IRT_Printf(0, "EV_IRTDMA_HW_DST_Y_SA    = 0x%0x \n", EV_IRTDMA_HW_DST_Y_SA);
        IRT_Printf(0, "EV_IRTDMA_HW_DST_C_SA    = 0x%0x \n", EV_IRTDMA_HW_DST_C_SA);
        IRT_Printf(0, "EV_IRTDMA_SW_DST_Y_SA    = 0x%0x \n", EV_IRTDMA_SW_DST_Y_SA);
        IRT_Printf(0, "EV_IRTDMA_SW_DST_C_SA    = 0x%0x \n", EV_IRTDMA_SW_DST_C_SA);
        IRT_Printf(0, "EV_IRTDMA_SW_DST_TEMP_SA = 0x%0x \n", EV_IRTDMA_SW_DST_TEMP_SA);

        g_fgIrtDmaIsInit = TRUE;
    }

    IRT_APILeave();
}

INT32 i4IrtDmaEvInit(INT32 i4Argc, const CHAR ** szArgv)
{
    //UINT32  u4Val;
    INT32   i4Ret = RET_OK;

    IRT_APIEntry();

    vIrtDmaEvInit();
    IRT_DMA_HwSrcModeSet(g_rIrtSrcMode.fg5351Mode, g_rIrtSrcMode.uc5351ModeSel, g_rIrtSrcMode.fgScanLineMode);

    IRT_APILeave();

    return (i4Ret);
}

INT32 i4IrtDmaEvReset(INT32 i4Argc, const CHAR ** szArgv)
{
    INT32   i4Ret = RET_OK;

    IRT_APIEntry();

    vIrtDmaEvInit();

    x_thread_delay(EV_DELAY_CNT);

    IRT_DMA_HwReset();

    IRT_APILeave();

    return (i4Ret);
}

INT32 i4IrtDmaEvStop(INT32 i4Argc, const CHAR ** szArgv)
{
    if (g_fgIrtThreadRunning)
    {
        g_fgIrtThreadRunning = FALSE;
        IRT_Printf(0, "***************Irt test thread will be stopped...\n");
    }

    return (0);
}

static INT32 i4IrtDmaEvMemCompare(UINT8 *pu1HwSa, UINT8 *pu1SwSa, UINT32 u4Num)
{
    INT32  i4Error = 0;
    UINT32 u4Temp  = 0;
    UINT32 u4TotalPixels = u4Num;
    UINT32 u4MaxDiff = 0;

    UINT32  *pu4Sw = (UINT32 *) pu1SwSa;
    UINT32  *pu4Hw = (UINT32 *) pu1HwSa;

    IRT_APIEntry("0x%08X, 0x%08X, %u", pu1HwSa, pu1SwSa, u4Num);

    u4TotalPixels = u4Num / 4;
    while (u4TotalPixels--)
    {
        if (*pu4Sw != *pu4Hw)
        {
            ++i4Error;
            if (i4Error < 10)
            {
                IRT_Printf(0, " Mem Compare error (%d) :\n", i4Error);
                IRT_Printf(0, "IrtDma HW     --> [0x%08x] = 0x%08x\n", pu4Hw, *pu4Hw);
                IRT_Printf(0, "IrtDma SW     --> [0x%08x] = 0x%08x\n", pu4Sw, *pu4Sw);
            }
            else if (i4Error > 100)
            {
                break;
            }

            if (*pu4Sw > *pu4Hw)
            {
                u4Temp = *pu4Sw - *pu4Hw;
            }
            else
            {
                u4Temp = *pu4Hw - *pu4Sw;
            }

            if (u4Temp > u4MaxDiff)
            {
                u4MaxDiff = u4Temp;
            }
        }
        pu4Sw++;
        pu4Hw++;
    }

    //BIM_WAIT_WALE();

    if (i4Error!=0)//(1)//
    {
        IRT_Printf(0, " HW/SW Comparison Fail => Error Cnt = %d (MaxDiff = 0x%x)\n", i4Error, u4MaxDiff);

        IRT_APILeaveEx("0");

        return (RET_ERR_UNKNOWN);
    }

    IRT_APILeave();

    return (RET_OK);
}


INT32 i4IrtDmaHwRotateMirror(UINT32 *u4SrcYSa,UINT32 *u4SrcCSa,UINT32 *u4DstYSa,UINT32 *u4DstCSa,UINT32 u4Width, UINT32 u4Height, IRT_DMA_MODE_T eMode)
{
    IRT_APIEntry();

    VERIFY(u4SrcYSa != NULL);
    VERIFY(u4SrcCSa != NULL);
    VERIFY(u4DstYSa != NULL);
    VERIFY(u4DstCSa != NULL);

    IRT_DMA_HwSrcModeSet(g_rIrtSrcMode.fg5351Mode,
		g_rIrtSrcMode.uc5351ModeSel,
		g_rIrtSrcMode.fgScanLineMode);

    if (IRT_DMA_HwSrcSa(u4SrcYSa,u4SrcCSa))
    {
        IRT_Printf(0, "IrtDma_SrcSa Failed \n");
    }
    if (IRT_DMA_HwDstSa(u4DstYSa,u4DstCSa))
    {
        IRT_Printf(0, "IrtDma_DstSa Failed \n");
    }
    if (IRT_DMA_HwWROffset(u4Width,u4Height,eMode))
    {
        IRT_Printf(0, "IRT_DMA_HwWROffset Failed \n");
    }
    if (IRT_DMA_HwTrig())
    {
        IRT_Printf(0, "IrtDma_Trig Failed \n");
    }

    IRT_APILeave();

    return (RET_OK);
}

static VOID i4IrtDmaEvEncodePara(INT32 i4Argc, const CHAR ** szArgv, EV_IRT_INPUT_PARA_T *prPara)
{
    UINT16 u2Cnt = 0;
    UINT32 u4ParaCnt = sizeof(prPara->u4Para)/sizeof(prPara->u4Para[0]);

    IRT_APIEntry();

    for (u2Cnt = 0; u2Cnt < u4ParaCnt; u2Cnt++)
    {
        prPara->u4Para[u2Cnt] = 0;
    }
    prPara->u2ParaDigit = 0;

    for (u2Cnt = 0; u2Cnt < (i4Argc-1) && u2Cnt < u4ParaCnt; u2Cnt++)
    {
        prPara->u4Para[u2Cnt] = (UINT32) StrToInt(szArgv[u2Cnt+1]);
        prPara->u2ParaDigit |= (1 << u2Cnt);
    }

    IRT_APILeave();
}

static VOID i4IrtDmaEvDecodePara(EV_IRT_INPUT_PARA_T *prPara,
                                 UINT32 *pu4Para2, UINT32 *pu4Para3, UINT32 *pu4Para4, UINT32 *pu4Para5,
                                 UINT32 *pu4Para6, UINT32 *pu4Para7, UINT32 *pu4Para8, UINT32 *pu4Para9,
                                 UINT32 *pu4Para10, UINT32 *pu4Para11)
{
    IRT_APIEntry();

    IRT_DMA_EV_DECODE_PARA(2);
    IRT_DMA_EV_DECODE_PARA(3);
    IRT_DMA_EV_DECODE_PARA(4);
    IRT_DMA_EV_DECODE_PARA(5);
    IRT_DMA_EV_DECODE_PARA(6);
    IRT_DMA_EV_DECODE_PARA(7);
    IRT_DMA_EV_DECODE_PARA(8);
    IRT_DMA_EV_DECODE_PARA(9);
    IRT_DMA_EV_DECODE_PARA(10);
    IRT_DMA_EV_DECODE_PARA(11);

    IRT_APILeave();
}

static void vIrtExecute(void *pvArg)
{
    IRT_APIEntry();

    g_fgIrtThreadRunning = TRUE;

    (g_prIrtExecFunc)(&g_rIrtPara);

    g_fgIrtThreadRunning = FALSE;

    IRT_APILeave();
}

static INT32 i4IrtEvExecute(INT32 i4Argc, const CHAR ** szArgv, IRT_IRT_EV_EXEC_FUNC prExecFunc)
{
    HANDLE_T hThread;
    INT32 i4Ret;

    IRT_APIEntry();

    if (g_fgIrtThreadRunning)
    {
        IRT_Printf(0, "****** Irt thread has been running already, please stop it firstly!\n");
        IRT_APILeaveEx("0");
        return (RET_OK);
    }

    if (!szArgv || !prExecFunc)
    {
        IRT_Printf(0, "NULL parameter is not allowed!\n");
        IRT_APILeaveEx("0");
        return (RET_OK);
    }

    i4IrtDmaEvEncodePara(i4Argc, szArgv, &g_rIrtPara);
    g_prIrtExecFunc = prExecFunc;

    i4Ret = x_thread_create(&hThread, "IrtThread", 20*1024, PRIORITY_CLASS_IDLE, vIrtExecute, 0, NULL);

    IRT_APILeave();

    return (i4Ret);
}


VOID  vIrtDmaEvMemDummy(UINT32 u4W, UINT32 u4H)
{
    HAL_TIME_T rTime;
    UINT32* pu4Src_Y  = (UINT32 *)   EV_IRTDMA_SRC_Y_SA;
    UINT32* pu4Src_C  = (UINT32 *)   EV_IRTDMA_SRC_C_SA;
    UINT32 u4Tmp = 0;
    UINT32  u4Cnt = EV_IrtDma_MEM_SLOT_SIZE / 4;
    UINT32 u4Size;

    IRT_APIEntry();

    if (FALSE == g_fgIrtSrcDataInit)
    {
        HAL_GetTime(&rTime);
        u4Tmp = rTime.u4Seconds + rTime.u4Micros;
        if (0 != g_u4IrtRandomSeed)
            u4Tmp = g_u4IrtRandomSeed;

        srand(u4Tmp);
        IRT_Printf(0, "seed = %u, 0x%08X\n", u4Tmp, u4Tmp);

        if (g_u4IrtRandomData)
        {
            while (u4Cnt--)
            { // Also Rand
                u4Tmp = rand();
                *pu4Src_Y++ = u4Tmp;
                u4Tmp = rand();
                *pu4Src_C++ = u4Tmp;
            }
        }
        else
        {
            while (u4Cnt--)
            {
                *pu4Src_Y++   = 0x12345678;
                *pu4Src_C++   = 0x9ABCDEF0;
            }
        }

        g_fgIrtSrcDataInit = TRUE;
    }

    u4Size = u4W * u4H;
    x_memset(EV_IRTDMA_HW_DST_Y_SA, 0, u4Size);
    x_memset(EV_IRTDMA_HW_DST_C_SA, 0, u4Size);
    x_memset(EV_IRTDMA_SW_DST_Y_SA, 0, u4Size);
    x_memset(EV_IRTDMA_SW_DST_C_SA, 0, u4Size);
    x_memset(EV_IRTDMA_SW_DST_TEMP_SA, 0, EV_IrtDma_MEM_SLOT_SIZE);

    IRT_FlushDCacheRange((UINT32)EV_IRTDMA_SRC_Y_SA, EV_IrtDma_MEM_SLOT_SIZE);
    IRT_FlushDCacheRange((UINT32)EV_IRTDMA_SRC_C_SA, EV_IrtDma_MEM_SLOT_SIZE);
    IRT_FlushDCacheRange((UINT32)EV_IRTDMA_HW_DST_Y_SA, EV_IrtDma_MEM_SLOT_SIZE);
    IRT_FlushDCacheRange((UINT32)EV_IRTDMA_HW_DST_C_SA, EV_IrtDma_MEM_SLOT_SIZE);
    IRT_FlushDCacheRange((UINT32)EV_IRTDMA_SW_DST_Y_SA, EV_IrtDma_MEM_SLOT_SIZE);
    IRT_FlushDCacheRange((UINT32)EV_IRTDMA_SW_DST_C_SA, EV_IrtDma_MEM_SLOT_SIZE);
    IRT_FlushDCacheRange((UINT32)EV_IRTDMA_SW_DST_TEMP_SA, EV_IrtDma_MEM_SLOT_SIZE);

    IRT_APILeave();
}

static INT32 i4IrtDmaCheckSrcMode(UINT32 *pu4ScanLineMode, UINT32 *pu45351Mode)
{
   *pu4ScanLineMode = *pu4ScanLineMode % 2;
   *pu45351Mode = *pu45351Mode % 2;

    g_rIrtSrcMode.fgScanLineMode = (BOOL)(*pu4ScanLineMode);
    g_rIrtSrcMode.fg5351Mode = (BOOL)(*pu45351Mode);

    if (g_rIrtSrcMode.fgScanLineMode)
    {
        EV_IRTDMA_W_ALIGN = 16;
        EV_IRTDMA_H_ALIGN = 32;

        g_rIrtSrcMode.uc5351ModeSel = 0;
        g_rIrtSrcMode.fg5351Mode = FALSE;
        *pu45351Mode = 0;
    }
    else if (g_rIrtSrcMode.fg5351Mode)
    {
        EV_IRTDMA_W_ALIGN = 64;
        EV_IRTDMA_H_ALIGN = 32;

        g_rIrtSrcMode.uc5351ModeSel = 2; // only support mb 64x32.
        g_rIrtSrcMode.fgScanLineMode = FALSE;
        *pu4ScanLineMode = 0;
    }
    else
    {
        EV_IRTDMA_W_ALIGN = 16;
        EV_IRTDMA_H_ALIGN = 32;

        g_rIrtSrcMode.uc5351ModeSel = 0;
    }

    return 0;
}

static INT32 i4IrtDmaUpdateHwSrcBuffer(UINT32 u4W, UINT32 u4H)
{
    if (g_rIrtSrcMode.fgScanLineMode)
    {
        EV_IRTDMA_HW_SRC_Y_SA = EV_IRTDMA_SRC_Y_SA;
        EV_IRTDMA_HW_SRC_C_SA = EV_IRTDMA_SRC_C_SA;
    }
    else if (g_rIrtSrcMode.fg5351Mode)
    {
        i4IrtDmaSwConvertSL420To5351Mode(u4W, u4H, EV_IRTDMA_SRC_Y_SA, EV_IRTDMA_SRC_C_SA,
            EV_IRTDMA_5351_SRC_Y_SA, EV_IRTDMA_5351_SRC_C_SA);

        IRT_FlushDCacheRange((UINT32)EV_IRTDMA_5351_SRC_Y_SA, EV_IrtDma_MEM_SLOT_SIZE);
        IRT_FlushDCacheRange((UINT32)EV_IRTDMA_5351_SRC_C_SA, EV_IrtDma_MEM_SLOT_SIZE);

        EV_IRTDMA_HW_SRC_Y_SA = EV_IRTDMA_5351_SRC_Y_SA;
        EV_IRTDMA_HW_SRC_C_SA = EV_IRTDMA_5351_SRC_C_SA;
    }
    else
    {
        i4IrtDmaSwConvertSL420ToMB420(u4W, u4H, EV_IRTDMA_SRC_Y_SA, EV_IRTDMA_SRC_C_SA,
            EV_IRTDMA_MB420_SRC_Y_SA, EV_IRTDMA_MB420_SRC_C_SA);

        IRT_FlushDCacheRange((UINT32)EV_IRTDMA_MB420_SRC_Y_SA, EV_IrtDma_MEM_SLOT_SIZE);
        IRT_FlushDCacheRange((UINT32)EV_IRTDMA_MB420_SRC_C_SA, EV_IrtDma_MEM_SLOT_SIZE);

        EV_IRTDMA_HW_SRC_Y_SA = EV_IRTDMA_MB420_SRC_Y_SA;
        EV_IRTDMA_HW_SRC_C_SA = EV_IRTDMA_MB420_SRC_C_SA;
    }

    return 0;
}

static INT32 i4IrtDmaEvRotateMirrorFixSize(EV_IRT_INPUT_PARA_T *prPara)
{
    UINT32  u4BmpWidth, u4BmpHeight,u4ModeOpt;
    UINT32  u4ScanLineMode, u45351Mode;
    UINT32  u4BmpWidth0, u4BmpHeight0,u4ModeOpt0;
    INT32   i4Ret = RET_OK;
    BOOL    fgFailed = FALSE;

    UINT32  u4ScanlineIdx = 0;
    UINT32  u45351ModeIdx = 0;
    UINT32  u4RotateModeIdx = 0;
    UINT32  u4WHIdx = 0;

    IRT_APIEntry();

    vIrtDmaEvInit();

    for (u4ScanlineIdx = 0; u4ScanlineIdx < 2; u4ScanlineIdx ++)
    {
        for (u45351ModeIdx = 0; u45351ModeIdx < 2; u45351ModeIdx ++)
        {
            if ((1 == u4ScanlineIdx) && (1 == u45351ModeIdx))
            {
                break;
            }

            for (u4RotateModeIdx = 0; u4RotateModeIdx < 8; u4RotateModeIdx ++)
            {
                if ((0 == u4ScanlineIdx) && (1 == u45351ModeIdx))
                {
                    u4WHIdx = 2;
                }
                else
                {
                    u4WHIdx = 0;
                }

            #if IRT_HW_SPEED_TEST
                for (u4WHIdx = 0; u4WHIdx < IRT_HW_SPEED_TEST_LOOP; u4WHIdx ++)
            #else
                for (; u4WHIdx < 36; u4WHIdx ++)
            #endif
                {
                    u4ScanLineMode = u4ScanlineIdx;
                    u45351Mode = u45351ModeIdx;
                    u4ModeOpt = u4RotateModeIdx;

                #if IRT_HW_SPEED_TEST
                    u4BmpWidth = 1920;
                    u4BmpHeight = 1080;
                #else
                    u4BmpWidth = _u4IrtDmaWH[u4WHIdx][0];
                    u4BmpHeight = _u4IrtDmaWH[u4WHIdx][1];
                #endif

                    fgFailed = FALSE;

                    if (!g_fgIrtThreadRunning)
                    {
                        IRT_Printf(0, "\n.\n.\nIrt thread stopped by user.\n.\n.\n");
                        break;
                    }

                    do
                    { //Get random value
                        i4IrtDmaCheckSrcMode(&u4ScanLineMode, &u45351Mode);

                        // validity check
                        u4BmpWidth  = (u4BmpWidth  % (EV_IRT_MAX_WIDTH + 1));
                        u4BmpHeight = (u4BmpHeight % (EV_IRT_MAX_HEIGHT + 1));
                        u4ModeOpt   = (u4ModeOpt % IRT_DMA_MODE_MAX);

                    } while (!u4BmpWidth || !u4BmpHeight);

                    //x_thread_delay(EV_DELAY_CNT);

                    {
                        if(0 != (u4BmpWidth % EV_IRTDMA_W_ALIGN))
                        {
                            u4BmpWidth0 = ((u4BmpWidth / EV_IRTDMA_W_ALIGN + 1) * EV_IRTDMA_W_ALIGN);

                            if (u4BmpWidth0 > EV_IRT_MAX_WIDTH)
                            {
                                u4BmpWidth0 -= EV_IRTDMA_W_ALIGN;
                            }
                        }
                        else
                        {
                            u4BmpWidth0 = u4BmpWidth;
                        }

                        if(0 != (u4BmpHeight % EV_IRTDMA_H_ALIGN))
                        {
                            u4BmpHeight0 = ((u4BmpHeight / EV_IRTDMA_H_ALIGN + 1) * EV_IRTDMA_H_ALIGN);

                            if (u4BmpHeight0 > EV_IRT_MAX_HEIGHT)
                            {
                                u4BmpHeight0 -= EV_IRTDMA_H_ALIGN;
                            }
                        }
                        else
                        {
                            u4BmpHeight0 = u4BmpHeight;
                        }

                        u4ModeOpt0 = (u4ModeOpt % IRT_DMA_MODE_MAX);
                    }

                    if(u4ModeOpt0 == IRT_DMA_MODE_CBCR_ALIGN)
                    {
                        UTIL_Printf("IRT_DMA in BDP don't support this mode\n");
                        ASSERT(0);

                        if(0 !=((u4BmpHeight0/2)%16))
                        {
                            u4BmpHeight0=(u4BmpHeight0/2 + 8);
                        }
                        else
                        {
                            u4BmpHeight0=u4BmpHeight0/2;
                        }

                        IRT_Printf(0, "Paras: u4BmpWidth0=%d, u4BmpHeight0=%d, u4ModeOpt0=%d \n", u4BmpWidth0, u4BmpHeight0, u4ModeOpt0);
                        vIrtDmaEvMemDummy(u4BmpWidth0, u4BmpHeight0);

                        i4IrtDmaHwRotateMirror(EV_IRTDMA_HW_SRC_Y_SA, EV_IRTDMA_HW_SRC_C_SA, EV_IRTDMA_HW_DST_Y_SA,EV_IRTDMA_HW_DST_C_SA, u4BmpWidth0, u4BmpHeight0,u4ModeOpt0);
                        IRT_InvDCacheRange((UINT32)EV_IRTDMA_HW_DST_Y_SA, EV_IrtDma_MEM_SLOT_SIZE);
                        IRT_InvDCacheRange((UINT32)EV_IRTDMA_HW_DST_C_SA, EV_IrtDma_MEM_SLOT_SIZE);

                        i4IrtDmaSwCbCrAlignMode(EV_IRTDMA_SRC_Y_SA, EV_IRTDMA_SRC_C_SA, EV_IRTDMA_SW_DST_Y_SA,u4BmpWidth0, u4BmpHeight0);

                        i4Ret = i4IrtDmaEvMemCompare((UINT8 *)EV_IRTDMA_HW_DST_Y_SA,
                            (UINT8 *)EV_IRTDMA_SW_DST_Y_SA,
                            u4BmpWidth0 * u4BmpHeight0);

                        if (RET_OK != i4Ret)
                        {
                            IRT_Printf(0, " ~~~~~ IRTDMA CBCR Data Compare Failed ~~~~~ \n");
                        }
                        else
                        {
                            IRT_Printf(0, "IRTDMA CBCR Data Compare OK \n");
                        }
                    }
                    else
                    {
                    #if !(IRT_HW_SPEED_TEST)
                        IRT_Printf(0, "Paras: ScanLine=%d, 5351Mode=%d, W=%d, H=%d, Mode=%d..Align=%dx%d\n",
                            u4ScanLineMode, u45351Mode, u4BmpWidth0, u4BmpHeight0, u4ModeOpt0, EV_IRTDMA_W_ALIGN, EV_IRTDMA_H_ALIGN);
                    #endif

                        vIrtDmaEvMemDummy(u4BmpWidth0, u4BmpHeight0);
                        i4IrtDmaUpdateHwSrcBuffer(u4BmpWidth0, u4BmpHeight0);
                        i4IrtDmaHwRotateMirror(EV_IRTDMA_HW_SRC_Y_SA, EV_IRTDMA_HW_SRC_C_SA, EV_IRTDMA_HW_DST_Y_SA,EV_IRTDMA_HW_DST_C_SA, u4BmpWidth0, u4BmpHeight0,u4ModeOpt0);
                        IRT_InvDCacheRange((UINT32)EV_IRTDMA_HW_DST_Y_SA, EV_IrtDma_MEM_SLOT_SIZE);
                        IRT_InvDCacheRange((UINT32)EV_IRTDMA_HW_DST_C_SA, EV_IrtDma_MEM_SLOT_SIZE);

                        i4IrtDmaSwYRotateMirror(EV_IRTDMA_SRC_Y_SA, EV_IRTDMA_SW_DST_Y_SA, u4BmpWidth0, u4BmpHeight0, u4ModeOpt0);

                        i4Ret = i4IrtDmaEvMemCompare((UINT8 *)EV_IRTDMA_HW_DST_Y_SA,
                            (UINT8 *)EV_IRTDMA_SW_DST_Y_SA,
                            u4BmpWidth0 * u4BmpHeight0);

                        if (RET_OK != i4Ret)
                        {
                            IRT_Printf(0, "~~~~~ Y Data Compare Failed ~~~~~ \n");
                            fgFailed = TRUE;
                        }
                        else
                        {
                            IRT_Printf(0, "Y Data Compare OK \n");
                        }

                        i4IrtDmaSwCRotateMirror(EV_IRTDMA_SRC_C_SA, EV_IRTDMA_SW_DST_C_SA, u4BmpWidth0, u4BmpHeight0, u4ModeOpt0);

                        i4Ret = i4IrtDmaEvMemCompare((UINT8 *)EV_IRTDMA_HW_DST_C_SA,
                            (UINT8 *)EV_IRTDMA_SW_DST_C_SA,
                            (u4BmpWidth0 * u4BmpHeight0)/2);

                        if (RET_OK != i4Ret)
                        {
                            IRT_Printf(0, "~~~~~ C Data Compare Failed ~~~~~ \n");
                            fgFailed = TRUE;
                        }
                        else
                        {
                            IRT_Printf(0, "C Data Compare OK \n");
                        }

                    #if IRT_HW_SPEED_TEST
                        IRT_Printf(0, "SL=%d, 5351=%d, W=%d, H=%d, Mode=%d, Align=%dx%d, Cnt = %d, loop %d/%d, Time = (%u, %u)\n",
                            u4ScanLineMode, u45351Mode, u4BmpWidth0, u4BmpHeight0, u4ModeOpt0, EV_IRTDMA_W_ALIGN, EV_IRTDMA_H_ALIGN,
                            _u4IRTHwSpeed_Cnt,
                            (u4WHIdx+1), IRT_HW_SPEED_TEST_LOOP,
                            _IRTHwSpeed_Calc.u4Seconds, _IRTHwSpeed_Calc.u4Micros);
                    #endif

                        if (fgFailed)
                        {
                            IRT_Printf(0, "Paras: ScanLine=%d, 5351Mode=%d, W=%d, H=%d, Mode=%d..Align=%dx%d [NG]\n",
                                u4ScanLineMode, u45351Mode, u4BmpWidth0, u4BmpHeight0, u4ModeOpt0, EV_IRTDMA_W_ALIGN, EV_IRTDMA_H_ALIGN);

                            ASSERT(0);
                        }
                    }
					x_thread_delay(100);
                }
            }
        }
    }

#if IRT_HW_SPEED_TEST
    IRT_Printf(0, "[IRT] hw speed test complete, count = %d, TotleTime = %lld\n",
            _u4IRTHwSpeed_Cnt, _u8IRTHwSpeed_Time);
#else
    IRT_Printf(0, "[IRT] fix size test complete\n");
#endif

    IRT_APILeave();

    return (i4Ret);
}

static INT32 i4IrtDmaEvRotateMirror(EV_IRT_INPUT_PARA_T *prPara)
{
    UINT32  u4Loop,u4BmpWidth, u4BmpHeight,u4ModeOpt;
    UINT32  u4ScanLineMode, u45351Mode;
    UINT32  u4BmpWidth0, u4BmpHeight0,u4ModeOpt0;
    UINT32  u4i;
    INT32   i4Ret = RET_OK;
    BOOL    fgFailed = FALSE;

    IRT_APIEntry();

    vIrtDmaEvInit();

    u4Loop = EV_LOOP_DEF_CNT;
    if ((prPara->u2ParaDigit & EV_IRT_PARA_INPUT_1))
    {
        u4Loop = prPara->u4Para[0];
    }

	IRT_Printf(0, "Begin Test 0x%x 0x%x\n", prPara->u2ParaDigit, prPara->u4Para[0]);

    for (u4i = 0; u4i < u4Loop; u4i++)
    {
        fgFailed = FALSE;

        if (!g_fgIrtThreadRunning)
        {
            IRT_Printf(0, "\n.\n.\nIrt thread stopped by user.\n.\n.\n");
            break;
        }

		IRT_Printf(0, "Test Loop %d/%d\n", u4i, u4Loop);

        do
        { //Get random value
            i4IrtDmaEvDecodePara(prPara, &u4ScanLineMode, &u45351Mode, &u4BmpWidth, &u4BmpHeight, &u4ModeOpt, NULL, NULL, NULL, NULL, NULL);
            i4IrtDmaCheckSrcMode(&u4ScanLineMode, &u45351Mode);

            // validity check
            u4BmpWidth  = (u4BmpWidth  % (EV_IRT_MAX_WIDTH + 1));
            u4BmpHeight = (u4BmpHeight % (EV_IRT_MAX_HEIGHT + 1));
            u4ModeOpt   = (u4ModeOpt % IRT_DMA_MODE_MAX);

        } while (!u4BmpWidth || !u4BmpHeight);

        //x_thread_delay(EV_DELAY_CNT);

        {
            if(0 != (u4BmpWidth % EV_IRTDMA_W_ALIGN))
            {
                u4BmpWidth0 = ((u4BmpWidth / EV_IRTDMA_W_ALIGN + 1) * EV_IRTDMA_W_ALIGN);

                if (u4BmpWidth0 > EV_IRT_MAX_WIDTH)
                {
                    u4BmpWidth0 -= EV_IRTDMA_W_ALIGN;
                }
            }
            else
            {
                u4BmpWidth0 = u4BmpWidth;
            }

            if(0 != (u4BmpHeight % EV_IRTDMA_H_ALIGN))
            {
                u4BmpHeight0 = ((u4BmpHeight / EV_IRTDMA_H_ALIGN + 1) * EV_IRTDMA_H_ALIGN);

                if (u4BmpHeight0 > EV_IRT_MAX_HEIGHT)
                {
                    u4BmpHeight0 -= EV_IRTDMA_H_ALIGN;
                }
            }
            else
            {
                u4BmpHeight0 = u4BmpHeight;
            }

            u4ModeOpt0 = (u4ModeOpt % IRT_DMA_MODE_MAX);
        }

        if(u4ModeOpt0 == IRT_DMA_MODE_CBCR_ALIGN)
        {
            UTIL_Printf("IRT_DMA in BDP don't support this mode\n");
            ASSERT(0);

            if(0 !=((u4BmpHeight0/2)%16))
            {
                u4BmpHeight0=(u4BmpHeight0/2 + 8);
            }
            else
            {
                u4BmpHeight0=u4BmpHeight0/2;
            }

            IRT_Printf(0, "[%u/%u] Paras: u4BmpWidth0=%d, u4BmpHeight0=%d, u4ModeOpt0=%d \n", u4i+1, u4Loop, u4BmpWidth0, u4BmpHeight0, u4ModeOpt0);
            vIrtDmaEvMemDummy(u4BmpWidth0, u4BmpHeight0);

            i4IrtDmaHwRotateMirror(EV_IRTDMA_HW_SRC_Y_SA, EV_IRTDMA_HW_SRC_C_SA, EV_IRTDMA_HW_DST_Y_SA,EV_IRTDMA_HW_DST_C_SA, u4BmpWidth0, u4BmpHeight0,u4ModeOpt0);
            IRT_InvDCacheRange((UINT32)EV_IRTDMA_HW_DST_Y_SA, EV_IrtDma_MEM_SLOT_SIZE);
            IRT_InvDCacheRange((UINT32)EV_IRTDMA_HW_DST_C_SA, EV_IrtDma_MEM_SLOT_SIZE);

            i4IrtDmaSwCbCrAlignMode(EV_IRTDMA_SRC_Y_SA, EV_IRTDMA_SRC_C_SA, EV_IRTDMA_SW_DST_Y_SA,u4BmpWidth0, u4BmpHeight0);

            i4Ret = i4IrtDmaEvMemCompare((UINT8 *)EV_IRTDMA_HW_DST_Y_SA,
                (UINT8 *)EV_IRTDMA_SW_DST_Y_SA,
                u4BmpWidth0 * u4BmpHeight0);

            if (RET_OK != i4Ret)
            {
                IRT_Printf(0, " ~~~~~ IRTDMA CBCR Data Compare Failed ~~~~~ \n");
            }
            else
            {
                IRT_Printf(0, "IRTDMA CBCR Data Compare OK \n");
            }
        }
        else
        {
            IRT_Printf(0, "[%u/%u] Paras: ScanLine=%d, 5351Mode=%d, W=%d, H=%d, Mode=%d..Align=%dx%d\n",
                u4i+1, u4Loop, u4ScanLineMode, u45351Mode, u4BmpWidth0, u4BmpHeight0,
                u4ModeOpt0, EV_IRTDMA_W_ALIGN, EV_IRTDMA_H_ALIGN);

            vIrtDmaEvMemDummy(u4BmpWidth0, u4BmpHeight0);
            i4IrtDmaUpdateHwSrcBuffer(u4BmpWidth0, u4BmpHeight0);
            i4IrtDmaHwRotateMirror(EV_IRTDMA_HW_SRC_Y_SA, EV_IRTDMA_HW_SRC_C_SA,
						EV_IRTDMA_HW_DST_Y_SA,EV_IRTDMA_HW_DST_C_SA,
						u4BmpWidth0, u4BmpHeight0,u4ModeOpt0);
            IRT_InvDCacheRange((UINT32)EV_IRTDMA_HW_DST_Y_SA, EV_IrtDma_MEM_SLOT_SIZE);
            IRT_InvDCacheRange((UINT32)EV_IRTDMA_HW_DST_C_SA, EV_IrtDma_MEM_SLOT_SIZE);

            i4IrtDmaSwYRotateMirror(EV_IRTDMA_SRC_Y_SA, EV_IRTDMA_SW_DST_Y_SA, u4BmpWidth0, u4BmpHeight0, u4ModeOpt0);

            i4Ret = i4IrtDmaEvMemCompare((UINT8 *)EV_IRTDMA_HW_DST_Y_SA,
                (UINT8 *)EV_IRTDMA_SW_DST_Y_SA,
                u4BmpWidth0 * u4BmpHeight0);

            if (RET_OK != i4Ret)
            {
                IRT_Printf(0, "~~~~~ Y Data Compare Failed ~~~~~ \n");
                fgFailed = TRUE;
            }
            else
            {
                IRT_Printf(0, "Y Data Compare OK \n");
            }

            i4IrtDmaSwCRotateMirror(EV_IRTDMA_SRC_C_SA, EV_IRTDMA_SW_DST_C_SA, u4BmpWidth0, u4BmpHeight0, u4ModeOpt0);

            i4Ret = i4IrtDmaEvMemCompare((UINT8 *)EV_IRTDMA_HW_DST_C_SA,
                (UINT8 *)EV_IRTDMA_SW_DST_C_SA,
                (u4BmpWidth0 * u4BmpHeight0)/2);

            if (RET_OK != i4Ret)
            {
                IRT_Printf(0, "~~~~~ C Data Compare Failed ~~~~~ \n");
                fgFailed = TRUE;
            }
            else
            {
                IRT_Printf(0, "C Data Compare OK \n");
            }

            if (fgFailed)
            {
                IRT_Printf(0, "[%u/%u] Paras: ScanLine=%d, 5351Mode=%d, W=%d, H=%d, Mode=%d..Align=%dx%d [NG]\n",
                    u4i+1, u4Loop, u4ScanLineMode, u45351Mode, u4BmpWidth0, u4BmpHeight0, u4ModeOpt0, EV_IRTDMA_W_ALIGN, EV_IRTDMA_H_ALIGN);
            }
        }

		x_thread_delay(100);
    }

    IRT_APILeave();

    return (i4Ret);
}


INT32 i4IrtDmaEvArgRotateMirror(INT32 i4Argc, const CHAR ** szArgv)
{
    return i4IrtEvExecute(i4Argc, szArgv, i4IrtDmaEvRotateMirror);
}

INT32 i4IrtDmaEvArgRotateMirrorFixSize(INT32 i4Argc, const CHAR ** szArgv)
 {
#if IRT_HW_SPEED_TEST
    _u4IRTHwSpeed_Cnt = 0;
    _u8IRTHwSpeed_Time = 0;
#endif

	 return i4IrtEvExecute(i4Argc, szArgv,
	 	i4IrtDmaEvRotateMirrorFixSize);
 }


 INT32 i4IrtDmaHwEVRotateMirror(INT32 i4Argc, const CHAR ** szArgv)
{
    EV_IRT_INPUT_PARA_T  rPara;

    UINT32  u4BmpWidth, u4BmpHeight,u4ModeOpt;
    UINT32  u4BmpWidth0, u4BmpHeight0,u4ModeOpt0;
    UINT32  u4SrcAddY, u4SrcAddC;
    //UINT32 u4VdoRegBase,u4FmtRegBase;

    INT32   i4Ret = RET_OK;

    IRT_APIEntry();

    // Init IrtDma
    vIrtDmaEvInit();

    x_thread_delay(EV_DELAY_CNT);

    if ((i4Argc < 6) || (szArgv == NULL))
    {
        IRT_Printf(0, "Please call set mode again before using this command\n");
        IRT_APILeave();
        return 0;
    }

    i4IrtDmaEvEncodePara(i4Argc, szArgv, &rPara);
    //Get random value
    i4IrtDmaEvDecodePara(&rPara, &u4SrcAddY, &u4SrcAddC, &u4BmpWidth, &u4BmpHeight, &u4ModeOpt, NULL, NULL, NULL, NULL, NULL);
    // validity check

    {
        if(0 !=(u4BmpWidth%16))
        {
            u4BmpWidth0=((u4BmpWidth/16 + 1)*16);
        }
        else
        {
            u4BmpWidth0=u4BmpWidth;
        }

        if(0 !=(u4BmpHeight%16))
        {
            u4BmpHeight0=((u4BmpHeight/16 + 1)*16);
        }
        else
        {
            u4BmpHeight0=u4BmpHeight;
        }

        u4ModeOpt0=(u4ModeOpt%IRT_DMA_MODE_MAX);
    }

    i4IrtDmaHwRotateMirror(((UINT32 *)u4SrcAddY),((UINT32 *)u4SrcAddC),EV_IRTDMA_HW_DST_Y_SA,EV_IRTDMA_HW_DST_C_SA,u4BmpWidth0, u4BmpHeight0,u4ModeOpt0);

    IRT_APILeave();

    return (i4Ret);
}


INT32 i4IrtDmaSwEVRotateMirror(INT32 i4Argc, const CHAR ** szArgv)
{
    EV_IRT_INPUT_PARA_T  rPara;

    UINT32  u4BmpWidth, u4BmpHeight,u4ModeOpt;
    UINT32  u4SrcAddY, u4SrcAddC;

    INT32   i4Ret = RET_OK;

    IRT_APIEntry();

    // Init IrtDma
    vIrtDmaEvInit();

    x_thread_delay(EV_DELAY_CNT);

    if ((i4Argc < 6) || (szArgv == NULL))
    {
        IRT_Printf(0, "Please call set mode again before using this command\n");
        IRT_APILeave();
        return 0;
    }

    i4IrtDmaEvEncodePara(i4Argc, szArgv, &rPara);
    //Get random value
    i4IrtDmaEvDecodePara(&rPara, &u4SrcAddY, &u4SrcAddC, &u4BmpWidth, &u4BmpHeight, &u4ModeOpt, NULL, NULL, NULL, NULL, NULL);
    // validity check

    IRT_Printf(0, "Paras: u4SrcAddY=0x%08x, u4SrcAddC=0x%08x, u4BmpWidth=%d, u4BmpHeight=%d, u4ModeOpt=%d\n",
        u4SrcAddY, u4SrcAddC, u4BmpWidth, u4BmpHeight, u4ModeOpt);

    i4IrtDmaSwYRotateMirror(((UINT32 *)u4SrcAddY),EV_IRTDMA_HW_DST_Y_SA,u4BmpWidth, u4BmpHeight,u4ModeOpt);

    i4IrtDmaSwCRotateMirror(((UINT32 *)u4SrcAddC),EV_IRTDMA_HW_DST_C_SA,u4BmpWidth, u4BmpHeight,u4ModeOpt);

    IRT_APILeave();

    return (i4Ret);
}

#if 0
// cmd table
CLI_EXEC_T _arIrtdmaVrfCmdTbl[] =
{
    {"init",          "i",   i4IrtDmaEvInit,            NULL, "irt_dma module init", CLI_GUEST},
    {"reset",         "rst", i4IrtDmaEvReset,           NULL, "Reset irt_dma function", CLI_GUEST},
    {"stop",          "stop",i4IrtDmaEvStop,            NULL, "stop test thread", CLI_GUEST},
    {"RotateFlip",    "rf",  i4IrtDmaEvArgRotateMirror, NULL, "Rotate Mirror(Flip horizontal) output", CLI_GUEST},
    {"HwRotateFlip",  "hrf", i4IrtDmaHwEVRotateMirror,  NULL, "Only Hw Rotate Mirror(Flip horizontal) output", CLI_GUEST},
    {"SwRotateFlip",  "srf", i4IrtDmaSwEVRotateMirror,  NULL, "Only Sw Rotate Mirror(Flip horizontal) output", CLI_GUEST},

    {NULL, NULL, NULL, NULL, NULL, CLI_GUEST}
};

static CLI_EXEC_T _rIrtdmaModCmdTbl =
{
    "irt_dma",
    NULL,
    NULL,
    _arIrtdmaVrfCmdTbl,
    "Irt_dma Verify command",
    CLI_GUEST
};

CLI_EXEC_T* GetIrtDmaCmdTbl(void)
{
    return &_rIrtdmaModCmdTbl;
}
#endif

