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
#include "x_printf.h"
#include "x_stl_lib.h"
#include "x_drv_cli.h"
#include "x_hal_926.h"
#include "x_hal_5381.h"
#include "x_assert.h"
#include "x_ckgen.h"

#include "vdec_if.h"
#include "vdec_drvif.h"
#include "drv_dram.h"
#include "x_timer.h"

#include "x_mid.h"

#define DEFINE_IS_LOG VDEC_IsLog
#include "x_debug.h"

#include "drv_t32.h"
//#define CC_EMULATION
//for emulation
#ifdef CC_EMULATION
  #ifdef CC_DRIVER_DEMO
  #include "drv_t32.h"
  #endif
  //#include "vdec_ide.h"
  #include <string.h>

  //#define MEMRW_TEST
#endif

/******************************************************************************
* Macro            : helper macros to reduce code
******************************************************************************/

#define STRESS_TESTING
//#define RR_TEST
//#define IDE_READ_SUPPORT
//#define IDE_WRITE_SUPPORT

//wmv spork
//#define MEM_TEST
#define DO_COMPARE_FRAME
#define DUMP_MEM
#define START_COMPARE_FRAME (0)//440//532
#define END_COMPARE_FRAME (600)


#define FILE_INFO_SIZE 0x40000
#define GOLDEN_SIZE (1920 * 1088)
#define MAX_FILE_NUM 2000
#define MAX_FILENAME_LEN 256

#ifdef MEM_TEST
#define VFIFO_S 0x179c000    //0x2e24000    //should get from dmx fbm
#define VFIFO_E 0x679c000    //80M
#else
//#define VFIFO_S 0x2ff0000    //0x2e80000    //0x2e24000    //should get from dmx fbm
//#define VFIFO_E 0x7ff0000    //0x7ffc000    //81.48M
#define VFIFO_S 0x5BBD800
#define VFIFO_E 0x7FF0000
//#define VFIFO_S 0x3ff0000      //539x
//#define VFIFO_E 0x7ff0000      //539x
#endif

#define NO_PIC 0
#define TOP_FIELD 1
#define BOTTOM_FIELD 2
#define FRAME 3

typedef enum
{
    DISP_IDX = 0,
    BS_TYPE,
    RRMODE,
    OUTPUT_DISP_ORDER,
    REPEAT_FRM,
    ECC_MODE,
    ECC_TYPE,
    DRAM_BANDWITH_SIMU,
    LOG_SIMU_PAT,
    WAIT_FB_TIME,
    AV_SYNC_INFO,
    TIME_PROFILE,
    TIME_PROFILE2,
    TIME_PROFILE3
}   H264_PARAM_T;

/******************************************************************************
* Prototype        : cli "vdec" commands
******************************************************************************/

//low level commands
CLI_EXEC_T* GetVdecCmdTbl(void);

static INT32 _VdecCmdInit(INT32 i4Argc, const CHAR ** szArgv);

static INT32 _VdecCmdPlay(INT32 i4Argc, const CHAR ** szArgv);

static INT32 _VdecCmdStop(INT32 i4Argc, const CHAR ** szArgv);

static INT32 _VdecCmdSetRRMode(INT32 i4Argc, const CHAR ** szArgv);

static INT32 _VdecCmdSyncStc (INT32 i4Argc, const CHAR ** szArgv);

static INT32 _VdecCmdQuery (INT32 i4Argc, const CHAR ** szArgv);

static INT32 _VdecCmdECMode(INT32 i4Argc, const CHAR ** szArgv);

static INT32 _VdecCmdRstProfile(INT32 i4Argc, const CHAR ** szArgv);

static INT32 _VdecCmdDramUsage(INT32 i4Argc, const CHAR ** szArgv);

void _VdecCapNotify(UINT32 u4CompId, UINT32 u4Cond, UINT32 u4Data1, UINT32 u4Data2);

static INT32  _VdecCmdCapture(INT32 i4Argc, const CHAR ** szArgv);

static INT32 _VdecCmdDramBWSimu(INT32 i4Argc, const CHAR ** szArgv);

#ifdef CC_EMULATION

void _VdecNotify(UINT32 u4Arg1, UINT32 u4Arg2, UINT32 u4Arg3, UINT32 u4Arg4);

void _VdecMPEG4Notify(UINT32 u4Arg1, UINT32 u4Arg2, UINT32 u4Arg3, UINT32 u4Arg4);

void _VdecWMVNotify(UINT32 u4Arg1, UINT32 u4Arg2, UINT32 u4Arg3, UINT32 u4Arg4);

static BOOL _LoadFile(const char* pcFileName, UINT32 u4Addr);

#ifdef IDE_WRITE_SUPPORT
static BOOL _IDEComp(const char* pcFileName, UINT32 u4Addr, UINT32 u4Len);
#endif

static BOOL _VdecVerifyDecData(UINT32* pu4GoldData, VDEC_DISP_PIC_INFO_T rDispInfo, BOOL bIsYcomp);

static void _Vdec1389B2R(UINT8* pu1RasterData, VDEC_DISP_PIC_INFO_T rDispInfo, BOOL bIsYcomp);

static void _VdecSendH264Pattern(void* pvArg);

static void _VdecSendMPEG4Pattern(void* pvArg);

static void _VdecSendWMVPattern(void* pvArg);

static INT32 _VdecCmdSendPattern(INT32 i4Argc, const CHAR ** szArgv);

static INT32 _VdecCmdSaveDecOut(INT32 i4Argc, const CHAR ** szArgv);

static INT32 _VdecCmdSaveSimPat(INT32 i4Argc, const CHAR ** szArgv);

static INT32 _VdecCmdSetTestMode(INT32 i4Argc, const CHAR ** szArgv);

static INT32 _VdecCmdSetRepeatFrm(INT32 i4Argc, const CHAR ** szArgv);

//wmv
void Dump_Mem(VDEC_DISP_PIC_INFO_T*);
void print_checksum(BOOL fgRead);
UINT32 u4CalculatePixelAddress_Y(
  UINT32 u4YBase,                           ///< [IN] frame buffer Y component address
  UINT32 u4XPos,                             ///< [IN] x position of pixel in frame buffer
  UINT32 u4YPos,                             ///< [IN] y position of pixel in frame buffer
  UINT32 u4FrameWidth,                 ///< [IN] Frame buffer width
  BOOL fgBlock,                               ///< [IN] MTK block / raster scan
  UCHAR bBlockW
);
UINT32 u4CalculatePixelAddress_C(
  UINT32 u4CBase,                           ///< [IN] frame buffer CbCr component address
  UINT32 u4XPos,                             ///< [IN] x position of pixel in frame buffer
  UINT32 u4YPos,                             ///< [IN] y position of pixel in frame buffer
  UINT32 u4FrameWidth,                 ///< [IN] Frame buffer width
  BOOL fgBlock,                               ///< [IN] MTK block / raster scan
  BOOL fg420,                                   ///< [IN] 420 / 422
  UCHAR bBlockW
);
#endif


//wmv end


//guangjun vld mc clock setting
//#define CKGEN_BASE       0xF000d000    
#define VDEC_CLK_SEL_MC  0x228
#define VDEC_CLK_SEL_SYS 0x224
#define VDEC_MC_MC_CLK   0x2       //216MHz
#define VDEC_MC_SYS_CLK  0x2     //216MHz


//david

//#define LOG_FOR_MC_MEASURE             // define this to enable mc performance measure in 5396
#ifdef LOG_FOR_MC_MEASURE
#define RW_MC_PARA_BEHAVIOR_SETTING 0//PARA8_BEHAVIOR
#endif

//#define MID_CHK              // define this to enable MID in 5396

//#define POWER_SAVING_TEST             // define this to enable power saving mode in 5368

//#define DDR3_TEST             // define this to test DDR3 mode in 5365

#define CRC_MODE              // define this to enable CRC in 5395

#define CC_MT5363
#define CC_MT5365
#define MT8222_ON_MT5381      // define this to run the MT8222 state machine for vdec.t 981

//#define MT8222_FAKE_DISPLAY   // define this to enable fake display

#define MC_PP_MODE            1   // 1 => 136 = 1, 148 = 1 (MC -> PP -> DRAM)
                                  // 0 => 136 = 148 = 0 (MC -> DRAM) or 136 = 148 = 1, depending on DFP of the picture

#define DEC_WAIT_CNT          6000000  // time to wait for decode finish

#define PIXEL_ERR_LOG_CNT     10       // max number of log lines printed when pixel error

#define LOG_PROGRESS          100

#define GOLDEN_4                  // compare 4 bytes of golden a time

//#define MB_GOLDEN                 // golden in MB arrangement

#define RPR_RACING                      // RPR racing mode
#define RPR_RAC_LOOK_CNT      20        // number of pictures to search in advance
#define RPR_RAC_WAIT_CNT      30000000  // time to wait for RPR racing finish

#define IDE_RETRY             1


#define DRAM_NONCACHE_OFFSET  0//0x30000000

#define IO_VIRT                            0xF0000000
#define VLD_BASE                           (IO_VIRT + 0x02000)
#define VLD_TOP_BASE    (IO_VIRT + 0x2800) // 5368
#define MISC_BASE    (IO_VIRT + 0x3000) // 5368
#define MC_BASE                            (IO_VIRT + 0x0a000)
#define AVC_VLD_BASE                       (IO_VIRT + 0x30000)
#define RV_VLD_BASE                        (IO_VIRT + 0x30C00)
#define AVC_MV_BASE                        (IO_VIRT + 0x31000)
#define CRC_BASE                           (IO_VIRT + 0x03000)
#define PP_BASE                           (IO_VIRT + 0x2f000)
#define IMG_RESZ_BASE  (IO_VIRT + 0x20000)

#define vVDecWriteVLD(addr, val) RISCWrite(VLD_BASE + (addr), (val))
#define u4VDecReadVLD(addr) RISCRead(VLD_BASE + (addr))

#define RISCWrite(addr, val)   (*(volatile UINT32 *)(addr)) = (val)
#define RISCRead(addr)         (*(volatile UINT32 *)(addr))

#define MC_READ32(offset)           IO_READ32 (MC_BASE, (offset))
#define MC_WRITE32(offset, value)   IO_WRITE32  (MC_BASE, (offset), (value))

// from spec: maximum resolution of real video is 2048x1152
#define MAX_W (2048)
#define MAX_H (1152)
#define ALIGN_MASK(VAL, MSK) (((VAL) + (MSK)) & (~(MSK)))
#define RV_BUF_ALIGN_MASK (2048 - 1)

#define RMEMU_BASE_ADDR  0xAA00000 //0x299d000  //0xa9fc000  //0x397C000
#define RMEMU_SIZE       (MAX_W * MAX_H * 3 / 2 * 4) //0x1a74800  //0xC03000

#define RMEMU_BASE_ADDR2 ALIGN_MASK(RMEMU_BASE_ADDR + RMEMU_SIZE, RV_BUF_ALIGN_MASK) // 0x4712000  //0xb900000  //(0x397C000 + 0xC10000)
#define RMEMU_SIZE2      (0x1200000) // 0x1100000  //0xBEFCE0

#define PPY_OUT_BUF1        ALIGN_MASK(RMEMU_BASE_ADDR, RV_BUF_ALIGN_MASK)
#define PPY_OUT_BUF1_SZ     (MAX_W * MAX_H)                                // 2048*1152 = 0x240000
#define PPC_OUT_BUF1        ALIGN_MASK(PPY_OUT_BUF1 + PPY_OUT_BUF1_SZ, RV_BUF_ALIGN_MASK)
#define PPC_OUT_BUF1_SZ     (MAX_W * MAX_H / 2)                            // 2048*1152/2 = 0x120000

#define PPY_OUT_BUF2        ALIGN_MASK(PPC_OUT_BUF1 + PPC_OUT_BUF1_SZ, RV_BUF_ALIGN_MASK)
#define PPY_OUT_BUF2_SZ     (MAX_W * MAX_H)                                // 2048*1152 = 0x240000
#define PPC_OUT_BUF2        ALIGN_MASK(PPY_OUT_BUF2 + PPY_OUT_BUF2_SZ, RV_BUF_ALIGN_MASK)
#define PPC_OUT_BUF2_SZ     (MAX_W * MAX_H / 2)                            // 2048*1152/2 = 0x120000

#define PPY_OUT_BUF3        ALIGN_MASK(PPC_OUT_BUF2 + PPC_OUT_BUF2_SZ, RV_BUF_ALIGN_MASK)
#define PPY_OUT_BUF3_SZ     (MAX_W * MAX_H)                                // 2048*1152 = 0x240000
#define PPC_OUT_BUF3        ALIGN_MASK(PPY_OUT_BUF3 + PPY_OUT_BUF3_SZ, RV_BUF_ALIGN_MASK)
#define PPC_OUT_BUF3_SZ     (MAX_W * MAX_H / 2)                            // 2048*1152/2 = 0x120000

#define PPY_OUT_BUFR        ALIGN_MASK(PPC_OUT_BUF3 + PPC_OUT_BUF3_SZ, RV_BUF_ALIGN_MASK)
#define PPY_OUT_BUFR_SZ     (MAX_W * MAX_H)                                // 2048*1152 = 0x240000
#define PPC_OUT_BUFR        ALIGN_MASK(PPY_OUT_BUFR + PPY_OUT_BUFR_SZ, RV_BUF_ALIGN_MASK)
#define PPC_OUT_BUFR_SZ     (MAX_W * MAX_H / 2)                            // 2048*1152/2 = 0x120000

#if 0
#define MC_REF_OFFSET       (PPC_OUT_BUFR + PPC_OUT_BUFR_SZ)
#define MC_REF_SZ           0
#endif

#if (PPC_OUT_BUFR + PPC_OUT_BUFR_SZ) > (RMEMU_BASE_ADDR + RMEMU_SIZE)
#error "Memory pool 1 overflow!!!"
#endif

#define MV_DIRECT_ADDR      ALIGN_MASK(RMEMU_BASE_ADDR2, RV_BUF_ALIGN_MASK)
#define MV_DIRECT_SZ        (MAX_W * MAX_H * 16 / 16 / 16)                                 // 2048*1152*16/16/16 = 0x24000

#define VLD_PRED_OFFSET     ALIGN_MASK(MV_DIRECT_ADDR + MV_DIRECT_SZ, RV_BUF_ALIGN_MASK)
#define VLD_PRED_OFFSET_SZ  0xC000                                 // 3*128*128 = 49152 => should to be 49152 bits = 6144 bytes

#define VFIFO_ADDR          ALIGN_MASK(VLD_PRED_OFFSET + VLD_PRED_OFFSET_SZ, RV_BUF_ALIGN_MASK)
#define VFIFO_SZ            0x200000                                // 2MB

#define FRMINFO_ADDR        ALIGN_MASK(VFIFO_ADDR + VFIFO_SZ, RV_BUF_ALIGN_MASK)
#define FRMINFO_SZ          0x4F1A00                                // 30 * 7200 * 24 = 5184000 //??

#define GOLD_ADDR           ALIGN_MASK(FRMINFO_ADDR + FRMINFO_SZ, RV_BUF_ALIGN_MASK)
#define GOLD_SZ             0x900000
// for CRC => 0x24 * 260000 = 0x8ED280
// for frame => (MAX_W * MAX_H * 3 / 2) = 2048*1152*3/2 = 0x360000

#define EXELIST_ADDR        ALIGN_MASK(GOLD_ADDR + GOLD_SZ, RV_BUF_ALIGN_MASK)
#define EXELIST_SZ          0x100000

#define TMP_ADDR            ALIGN_MASK(EXELIST_ADDR + EXELIST_SZ, RV_BUF_ALIGN_MASK)
#define TMP_SZ              6144 //0x1800 ??

#define DRAM_BUSY_ADDR            ALIGN_MASK(TMP_ADDR + TMP_SZ, RV_BUF_ALIGN_MASK)
#define DRAM_BUSYSZ            0x10000

//#define MC_DUMP_ADDR        (TMP_ADDR + TMP_SZ)
//#define MC_DUMP_SZ          10485760                                // 10MB

#if (TMP_ADDR + TMP_SZ) > (RMEMU_BASE_ADDR2 + RMEMU_SIZE2)
#error "Memory pool 2 overflow!!!"
#endif

// hw related definitions
#define WAIT_THRD 0x1000

// *********************************************************************
// VLD Registers define
// *********************************************************************
#define RO_VLD_BARL      0x00

#define RW_VLD_VDOUFM    0x84
    #define VLD_VDOUFM       ((UINT32)0x1 << 0)
    #define VLD_MXOFF        ((UINT32)0x1 << 8)
    #define VLD_ENSTCNT      ((UINT32)0x1 << 9)
    #define VLD_AATO         ((UINT32)0x1 << 10)
    #define VLD_RAAF         ((UINT32)0x1 << 11)

#define RW_VLD_PROC        0x8C
  #define VLD_SSCBIT       ((UINT32)0x1 << 16)
  #define VLD_SSCBYTE      ((UINT32)0x1 << 17)
  #define VLD_ABRT         ((UINT32)0x1 << 18)
  #define VLD_PSUP         ((UINT32)0x1 << 19)
  #define VLD_INIFET       ((UINT32)0x1 << 20)
  #define VLD_MBDATA       ((UINT32)0x1 << 21)  // for 1389 MP ECO
  #define VLD_PDHW         ((UINT32)0x1 << 22)
  #define VLD_INIBR        ((UINT32)0x1 << 23)
  #define VLD_PERRCON      ((UINT32)0x1 << 24)
  #define VLD_RTERR        ((UINT32)0x1 << 25)
  #define VLD_EOFR         ((UINT32)0x1 << 26)
  #define VLD_IGBCL        ((UINT32)0x1 << 27)
  #define VLD_DECTOP       ((UINT32)0x1 << 29)
  #define VLD_DECBTM       ((UINT32)0x2 << 29)
  #define VLD_FPRBS        ((UINT32)0x3 << 29)
  #define VLD_B21EN        ((UINT32)0x1 << 31)

#define RW_VLD_RPTR      0xB0
    #define RPTR_ALIGN   16
    #define WPTR_OFFSET  512
#define RW_VLD_VSTART    0xB4
#define RW_VLD_VEND      0xB8

#define RW_VLD_RDY_SWTICH   0xC0
   #define READY_TO_RISC       (0x1 << 17)
   #define READY_TO_RISC_1     (0x1 << 18)
   #define READY_TO_RISC_2     (0x1 << 20)

#define RW_VLD_PIC_W_MB     0xC8

#define RW_VLD_ASYNC   0xD0
    #define WPTR_SETBY_RISC_EN ((UINT32)0x1 << 18)

#define RO_VLD_FETCHOK   0xE8
    #define VLD_FETCH_OK        ((UINT32)0x1 << 0)
    #define VLD_DRAM_REQ_FIN    ((UINT32)0x1 << 2)

#define RO_VLD_VBAR      0xEC
#define RO_VLD_SRAMCTRL  0xF4
    #define AA_FIT_TARGET_SCLK (1<<0)
    #define AA_FIT_TARGET_D (1<<1)
    #define PROCESS_FLAG (1<<15)
#define RO_VLD_VWPTR     0xF8
#define RO_VLD_VRPTR     0xFC

#define WO_VLD_SRST      0x108
#define WO_VLD_WPTR      0x110
    #define WPTR_LSH_BITS  3
    #define VLD_EWPL        ((UINT32)0x1 << 2)
    #define VLD_RISC_WR_EN      ((UINT32)0x1 << 1)
    #define VLD_CLEAR_PROCESS_EN ((UINT32)0x1 << 0)

#define RO_VLD_SUM              0x114
#define RW_VLD_VP6_ABS  0x23C

#define RW_VLD_SCL_ADDR      0x260 
#define RW_VLD_SCL_DATA      0x264

#define RW_VLD_MBDRAM_SEL    0x324

// *********************************************************************
// MC Registers define
// *********************************************************************

#ifdef LOG_FOR_MC_MEASURE
#define RO_MC_CYCLE_DRAM 0x778
#define RO_MC_DLE_NUM 0x774
#define RO_MC_MBX 0x28
#define RO_MC_MBY 0x2C
#define RO_MC_CYC_SYS 0x9E0
#define RO_MC_INTRA_CNT 0x9E4
#define RO_MC_Y_BLK_CNT 0x9E8
#define RO_MC_C_BLK_CNT 0x9EC
#define RO_MC_WAIT_CNT 0x9F0
#define RO_MC_REQ_CNT 0x7B4
#define RW_MC_PARA_BEHAVIOR 0x998
    #define PARA4_BEHAVIOR        ((UINT32)0x1 << 0)
    #define PARA8_BEHAVIOR        ((UINT32)0x1 << 4)
#endif


#define FRMINFO_BASE_DIR "r:/rm_emu/frminf"
#define GOLDEN_BASE_DIR  "r:/rm_emu/golden"

#define DUMP_BASE_DIR    "r:/rm_emu"

typedef enum
{
	RV_INTRAPIC,        /* 0 (00) */
	RV_FORCED_INTRAPIC, /* 1 (01) */
	RV_INTERPIC,        /* 2 (10) */
	RV_TRUEBPIC         /* 3 (11) */
} EnumRVPicCodType;

typedef struct
{
  BOOL   fgRV9;  // TRUE -> RV9  FALSE -> RV8
  EnumRVPicCodType ePtype;
  UINT32 u4OrgWidth;
  UINT32 u4OrgHeight;
  UINT32 u4Width;
  UINT32 u4Height;
  UINT32 u4PctszSize;
  UINT32 u4Pctsz;
  UINT32 u4Pquant;
  UINT32 u4Oquant;
  UINT32 u4DFP;
  UINT32 u4Tr;
  UINT32 u4MbaSize;
  UINT32 u4Mba;
  UINT32 u4Rtype;
  UINT32 u4Iratio;
  UINT32 u4HdrSkip;
  UINT32 u4NumSlice;
  UINT32 au4SliceSize[128];
  UINT32 u4BstLength;
} RvPicAttr;

static RvPicAttr rPic;
static RvPicAttr rTmpPic;

typedef enum
{
  RPR_RAC_NOTHING,
  RPR_RAC_SCP2_DTCED,
  RPR_RAC_SCP1_PENDING,
  RPR_RAC_SCP1_DONE
} EnumRprRacStt;

EnumRprRacStt _eRprRacStt = RPR_RAC_NOTHING;

static UINT32 _u4Rv8TblNum = 1;
static UINT32 _u4RefQpMb0 = 0;
static UINT32 _u4FrmInfoPtr = 0;
#ifdef CRC_MODE
static UINT32 _u4CRCPtr = 0;
#endif

static UINT32 _u4OutBufY = 0;
static UINT32 _u4OutBufC = 0;
static UINT32 _u4FwdBufY = 0;
static UINT32 _u4FwdBufC = 0;
static UINT32 _u4BwdBufY = 0;
static UINT32 _u4BwdBufC = 0;
static BOOL _fgBwdIsI = FALSE;

static UINT32 _u4PrevDecWidth;
static UINT32 _u4PrevDecHeight;
static UINT32 _u4PrevDispWidth;
static UINT32 _u4PrevDispHeight;

static UINT32 _u4SumTotal = 0;
static UINT32 _u4SumErrCnt = 0;
#ifndef CRC_MODE
static UINT32 _u4ErrPos = 0xFFFFFFFF;
#endif
static UINT32 _u4NumFrmToTest = 0xFFFFFFFF;
static UINT32 _u4Round = 1;
static BOOL   _fgDecTimeout;
static UINT32 _u4VldWindow;
static UINT32 _u4BeginCompare;
static UINT32 _u4BeginDecode = 0;

static BOOL _fgShowDiff = TRUE;
static BOOL _fgStopImmed = TRUE;
static BOOL _fgGoldenCompare = TRUE;
static UINT32 _u4ErrConceal = 0;
static UINT32 _u4FastParsing = 0x40000000;
#if 1 // 5368
static UINT32 _u4CycleCnt = 1;
#else
static UINT32 _u4CycleCnt = 0x00800000;
#endif
static UINT32 _u4MaxCycleCnt = 0;
static UINT32 _u4MaxCycleCntPicNum = 0xFFFFFFFF;
static BOOL _fgFileFromIDE = FALSE;
static BOOL _fgLightLog = FALSE;
static BOOL _fgCmpOnPC = FALSE;
static BOOL _fgRprRacing = TRUE;
static BOOL _fgDummyAgent = TRUE;
static BOOL _fgDirectMode = FALSE;
static BOOL _fgDumpReg = TRUE;

static UINT32 _u4DumpRegFrameNo = (UINT32)-1; // david: dump register of frame #_u4DumpRegFrameNo
static UINT32 _u4SetAvcMv134OfFrameNo = (UINT32)-1; // david: set AVC_MV 134 of frame #_u4SetAvcMv134OfFrameNo because it could be set only for first time
static BOOL _fgSetAvcMv134 = FALSE;
static BOOL _fgSetMc474 = FALSE;
static BOOL _fgShowMc = FALSE;

// 8222 on 5381 emulation
volatile int _fg8253Test = FALSE;
volatile int _fgFrameRdy = FALSE;
BOOL _fgPlayEnd = FALSE;
UINT32 _u4FrameLen = 0;
UINT32 _dwPrVWp, _dwPrVRp;
UINT8 *_pbVFifo;
BOOL _fgMpvTest = FALSE;
UINT32 _u4Uop = 0;
BOOL _fgScan982 = FALSE;
//static UINT32 _u4MpvStateCnt = 0;
//static UINT32 _u4MpvLoop = 50;
#ifdef LOG_FOR_MC_MEASURE
char *_pcLoadedBitstream;
#endif

#define RM_LOG(a)   if (!_fgLightLog) printf##a

void _DmxIdeReset(void){}
BOOL _DmxIdeRead(UINT32 u4Addr, UINT32 u4Size){return 0;}
void _DmxIdeGetNextCmd(UINT8 *pu1Msg){}
BOOL _DmxIdeGetFile(UINT32 u4Addr, UINT32 u4Type){return 0;}
BOOL _DmxIdeSendName(char *pcItem){return 0;}
BOOL _DmxIdeSendNum(UINT32 n){return 0;}
void _DmxIdeSendDataLen(UINT32 u4Len){}
void _DmxIdeSendData(UINT32 u4Addr, UINT32 u4Len){}
BOOL _DmxIdeGetCmpResult(void){return 0;}
void _DmxIdeTestTxRate(UINT32 u4Addr){}
void _DmxIdeTestRxRate(UINT32 u4Addr){}
void _DmxIdeGetPrm(UINT32 u4Addr){}
BOOL _DmxIdeGetPartFile(UINT32 u4Addr, UINT32 u4Ofst, UINT32 u4Len){return 1;}
BOOL _DmxIdeSendProp(char *pcItem, UINT32 u4Width, UINT32 u4Height, BOOL fgSendName){return 1;}
UINT32 _DmxIdeGetUop(void){return 0;}


static void vLoadBitstream(UINT32 u4PicNo)
{
  switch (u4PicNo)
  {
  case 1:
//  	Printf("d.load.binary \"d:/8226/rm/bitstream/RV8_002_1st_I.bin\" 0x%08X /ny\n", VFIFO_ADDR);
// d.load.binary "d:/8226/rm/bitstream/RV8_002_1st_I.bin" 0x045B7E00 /ny
    UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"d:/8226/rm/bitstream/RV8_002_1st_I.bin\" 0x%08X /ny", VFIFO_ADDR));
    break;
  case 2:
    UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"d:/8226/rm/bitstream/RV8_002_2nd_P.bin\" 0x%08X /ny", VFIFO_ADDR));
    break;
  case 3:
    UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"d:/8226/rm/bitstream/RV8_008_1st_I.bin\" 0x%08X /ny", VFIFO_ADDR));
    break;
  case 4:
    UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"d:/8226/rm/bitstream/RV8_008_2nd_P.bin\" 0x%08X /ny", VFIFO_ADDR));
    break;
  case 5:
    UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"d:/frminfo/frm00299.bin\" 0x%08X /ny", VFIFO_ADDR));
    break;
  case 6:
    UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"d:/frminfo/frm00300.bin\" 0x%08X /ny", VFIFO_ADDR));
    break;
  case 7:
    UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"d:/8226/rm/bitstream/frminfo/rv8_001/frm00000.bin\" 0x%08X /ny", VFIFO_ADDR));
    break;
  }
}

#ifdef CRC_MODE
static void vLoadCRC(char *szDir)
{
  if (_fgFileFromIDE)
  {
#if 0
    _DmxIdeSendName(szDir);
    
    _DmxIdeGetFile(FRMINFO_ADDR, 7);
#else
    while (1);  // not yet
#endif
  }
  else
  {
    printf("d.load.binary \"%s/%s/CrcResult.crc\" 0x%08X /ny\n", GOLDEN_BASE_DIR, szDir, GOLD_ADDR);
    UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"%s/%s/CrcResult.crc\" 0x%08X /ny", GOLDEN_BASE_DIR, szDir, GOLD_ADDR));
  }
}
#endif

static void vLoadFrmInfo(char *szDir)
{
  if (_fgFileFromIDE)
  {
    _DmxIdeSendName(szDir);
    
    _DmxIdeGetFile(FRMINFO_ADDR, 7);
  }
  else
  {
    printf("d.load.binary \"%s/%s/frminfo.bin\" 0x%08X /ny\n", FRMINFO_BASE_DIR, szDir, FRMINFO_ADDR);
    UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"%s/%s/frminfo.bin\" 0x%08X /ny", FRMINFO_BASE_DIR, szDir, FRMINFO_ADDR));
  }
}

static void vLoadRvPicBitstream(UINT32 n, char *szDir)
{
#ifdef MID_CHK
    UINT32 u4LowAddr = VFIFO_ADDR, u4HighAddr = VFIFO_ADDR + VFIFO_SZ;

    VERIFY(MID_SetRegion(0, MID_AGENT_CPU, u4LowAddr, u4HighAddr));
    VERIFY(MID_EnableRegionProtect(0));
#endif

  if (_fgFileFromIDE)
  {
    // send number to IDE first
    _DmxIdeSendNum(n);
    
    _DmxIdeGetFile(VFIFO_ADDR, 0);
  }
  else
  {
    //printf("d.load.binary \"%s/%s/f%07d.bin\" 0x%08X /ny", FRMINFO_BASE_DIR, szDir, n, VFIFO_ADDR);
    UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"%s/%s/f%07d.bin\" 0x%08X /ny", FRMINFO_BASE_DIR, szDir, n, VFIFO_ADDR));
  }
#ifdef VDEC_L2_CACHE
  HalFlushInvalidateDCacheMultipleLine(VFIFO_ADDR, VFIFO_SZ);
#else
    HalFlushInvalidateDCache();
#endif

#ifdef MID_CHK
    VERIFY(MID_SetRegion(0, MID_AGENT_AUDIO, u4LowAddr, u4HighAddr));
    VERIFY(MID_EnableRegionProtect(0));
#endif
}

static void vDumpVldCkSum(void)
{
  if (!_fgDumpReg) return;

  printf("RV VLD 33: 0x%08X\n", RISCRead(RV_VLD_BASE + 33 * 4));
  printf("RV VLD 34: 0x%08X\n", RISCRead(RV_VLD_BASE + 34 * 4));
  printf("RV VLD 35: 0x%08X\n", RISCRead(RV_VLD_BASE + 35 * 4));
  printf("RV VLD 36: 0x%08X\n", RISCRead(RV_VLD_BASE + 36 * 4));
  printf("RV VLD 37: 0x%08X\n", RISCRead(RV_VLD_BASE + 37 * 4));
  printf("RV VLD 43: 0x%08X\n", RISCRead(RV_VLD_BASE + 43 * 4));
  printf("RV VLD 44: 0x%08X\n", RISCRead(RV_VLD_BASE + 44 * 4));
  printf("RV VLD 48: 0x%08X\n", RISCRead(RV_VLD_BASE + 48 * 4));
  printf("RV VLD 59: 0x%08X\n", RISCRead(RV_VLD_BASE + 59 * 4));
  printf("RV VLD 60: 0x%08X\n", RISCRead(RV_VLD_BASE + 60 * 4));
  printf("RV VLD 67: 0x%08X\n", RISCRead(RV_VLD_BASE + 67 * 4));
  printf("RV VLD 63: 0x%08X\n", RISCRead(RV_VLD_BASE + 63 * 4));
  printf("RV VLD 64: 0x%08X\n", RISCRead(RV_VLD_BASE + 64 * 4));
  printf("RV VLD 65: 0x%08X\n", RISCRead(RV_VLD_BASE + 65 * 4));
  printf("RV VLD 196: 0x%08X\n", RISCRead(RV_VLD_BASE + 196 * 4));
}

static void vDumpAvcMvCkSum(void)
{
  if (!_fgDumpReg) return;

  printf("AVC MV 148: 0x%08X\n", RISCRead(AVC_MV_BASE + 148 * 4));
  printf("AVC MV 149: 0x%08X\n", RISCRead(AVC_MV_BASE + 149 * 4));
  printf("AVC MV 150: 0x%08X\n", RISCRead(AVC_MV_BASE + 150 * 4));
  printf("AVC MV 151: 0x%08X\n", RISCRead(AVC_MV_BASE + 151 * 4));
}

static void vDumpMcCkSum(void)
{
  if (!_fgDumpReg) return;

  printf("MC 378: 0x%08X\n", RISCRead(MC_BASE + 378 * 4));
  printf("MC 379: 0x%08X\n", RISCRead(MC_BASE + 379 * 4));
  printf("MC 380: 0x%08X\n", RISCRead(MC_BASE + 380 * 4));
  printf("MC 381: 0x%08X\n", RISCRead(MC_BASE + 381 * 4));
  printf("MC 382: 0x%08X\n", RISCRead(MC_BASE + 382 * 4));
  printf("MC 388: 0x%08X\n", RISCRead(MC_BASE + 388 * 4));
  printf("MC 390: 0x%08X\n", RISCRead(MC_BASE + 390 * 4));
  printf("MC 391: 0x%08X\n", RISCRead(MC_BASE + 391 * 4));
  printf("MC 392: 0x%08X\n", RISCRead(MC_BASE + 392 * 4));
  printf("MC 393: 0x%08X\n", RISCRead(MC_BASE + 393 * 4));
  printf("MC 446: 0x%08X\n", RISCRead(MC_BASE + 446 * 4));
  printf("MC 447: 0x%08X\n", RISCRead(MC_BASE + 447 * 4));
}

static void vDumpPpCkSum(void)
{
  int i;
  
  if (!_fgDumpReg) return;

  printf("PP   5: 0x%08X\n", RISCRead(PP_BASE +   5 * 4));
  printf("PP   6: 0x%08X\n", RISCRead(PP_BASE +   6 * 4));
  printf("PP  67: 0x%08X\n", RISCRead(PP_BASE +  67 * 4));

  for (i = 69; i <= 76; i++)
    printf("PP %3d: 0x%08X\n", i, RISCRead(PP_BASE + i * 4));
    
  for (i = 161; i <= 178; i++)
    printf("PP %3d: 0x%08X\n", i, RISCRead(PP_BASE + i * 4));

  printf("PP  67: 0x%08X\n", RISCRead(PP_BASE +  67 * 4));
  printf("PP 199: 0x%08X\n", RISCRead(PP_BASE + 199 * 4));
  printf("PP 200: 0x%08X\n", RISCRead(PP_BASE + 200 * 4));
  printf("PP 201: 0x%08X\n", RISCRead(PP_BASE + 201 * 4));
  printf("PP 227: 0x%08X\n", RISCRead(PP_BASE + 227 * 4));
}

static void vDumpReg(void)
{
  if (!_fgDumpReg) return;

  printf("AVC_MV 131: 0x%08X\n", RISCRead(AVC_MV_BASE + 131 * 4));
  printf("AVC_MV 134: 0x%08X\n", RISCRead(AVC_MV_BASE + 134 * 4));
  printf("AVC_MV 135: 0x%08X\n", RISCRead(AVC_MV_BASE + 135 * 4));
  
  printf("MC 136: 0x%08X\n", RISCRead(MC_BASE + 136 * 4));
  printf("MC 137: 0x%08X\n", RISCRead(MC_BASE + 137 * 4));
  printf("MC 138: 0x%08X\n", RISCRead(MC_BASE + 138 * 4));
  printf("MC 139: 0x%08X\n", RISCRead(MC_BASE + 139 * 4));
  printf("MC 142: 0x%08X\n", RISCRead(MC_BASE + 142 * 4));
  printf("MC 148: 0x%08X\n", RISCRead(MC_BASE + 148 * 4));
  printf("MC 152: 0x%08X\n", RISCRead(MC_BASE + 152 * 4));
  printf("MC 153: 0x%08X\n", RISCRead(MC_BASE + 153 * 4));
  printf("PP 2: 0x%08X\n", RISCRead(PP_BASE + 2 * 4));

  printf("MC 420: 0x%08X\n", RISCRead(MC_BASE + 420 * 4));
  printf("MC 130: 0x%08X\n", RISCRead(MC_BASE + 130 * 4));
  printf("MC 131: 0x%08X\n", RISCRead(MC_BASE + 131 * 4));
  printf("VLD 36: 0x%08X\n", RISCRead(VLD_BASE + 36 * 4));
  printf("VLD 50: 0x%08X\n", RISCRead(VLD_BASE + 50 * 4));
  printf("MC 0: 0x%08X\n", RISCRead(MC_BASE + 0 * 4));
  printf("MC 1: 0x%08X\n", RISCRead(MC_BASE + 1 * 4));
  printf("MC 2: 0x%08X\n", RISCRead(MC_BASE + 2 * 4));
  printf("MC 3: 0x%08X\n", RISCRead(MC_BASE + 3 * 4));
  printf("MC 39: 0x%08X\n", RISCRead(MC_BASE + 39 * 4));
  printf("MC 40: 0x%08X\n", RISCRead(MC_BASE + 40 * 4));
  printf("MC 9: 0x%08X\n", RISCRead(MC_BASE + 9 * 4));
  printf("MC 28: 0x%08X\n", RISCRead(MC_BASE + 28 * 4));
  printf("MC 36: 0x%08X\n", RISCRead(MC_BASE + 36 * 4));
  printf("MC 6: 0x%08X\n", RISCRead(MC_BASE + 6 * 4));
  printf("MC 38: 0x%08X\n", RISCRead(MC_BASE + 38 * 4));
  printf("MC 114: 0x%08X\n", RISCRead(MC_BASE + 114 * 4));

  printf("RV VLD 39: 0x%08X\n", RISCRead(RV_VLD_BASE + 39 * 4));
  printf("RV VLD 66: 0x%08X\n", RISCRead(RV_VLD_BASE + 66 * 4));
  printf("RV VLD 33: 0x%08X\n", RISCRead(RV_VLD_BASE + 33 * 4));
  printf("RV VLD 34: 0x%08X\n", RISCRead(RV_VLD_BASE + 34 * 4));
  printf("RV VLD 35: 0x%08X\n", RISCRead(RV_VLD_BASE + 35 * 4));
  printf("RV VLD 36: 0x%08X\n", RISCRead(RV_VLD_BASE + 36 * 4));
  printf("RV VLD 37: 0x%08X\n", RISCRead(RV_VLD_BASE + 37 * 4));
/*
  printf("RV VLD 61: 0x%08X\n", RISCRead(RV_VLD_BASE + 39 * 4));
  printf("RV VLD 62: 0x%08X\n", RISCRead(RV_VLD_BASE + 39 * 4));
  printf("RV VLD 63: 0x%08X\n", RISCRead(RV_VLD_BASE + 39 * 4));
  printf("RV VLD 62: 0x%08X\n", RISCRead(RV_VLD_BASE + 39 * 4));
  printf("RV VLD 63: 0x%08X\n", RISCRead(RV_VLD_BASE + 39 * 4));
*/
  printf("RV VLD 42: 0x%08X\n", RISCRead(RV_VLD_BASE + 42 * 4));
  printf("RV VLD window: 0x%08X\n", _u4VldWindow);
#if 1
{
  UINT32 i;

  for (i = 0; i <= 0xb00; i+=4)
  {
      printf("MC(%d) = 0x%08X\n", i >> 2, RISCRead(MC_BASE + i));
  }

  for (i = 0; i <= 0x80; i+=4)
  {
      printf("PP(%d) = 0x%08X\n", i >> 2, RISCRead(PP_BASE + i));
  }

  for (i = 0; i <= 0x80; i+=4)
  {
      printf("TOP(%d) = 0x%08X\n", i >> 2, RISCRead(VLD_TOP_BASE + i));
  }
}
#endif
}

static void vShowErrDumpHint(int n, char *szDir, UINT32 u4Width, UINT32 u4Height, UINT32 u4BstLength)
{
  UINT32 u4NewHeight, u4NewWidth;
  UINT32 u4YSize, u4CSize, u4GoldSize;

  if (!_fgDumpReg) return;

#ifdef DDR3_TEST
    u4NewWidth = (u4Width + 63) & ~63;
#else
    u4NewWidth = u4Width;
#endif

#if 1
  u4NewHeight = (u4Height + 31) & ~31;
#else
  u4NewHeight = u4Height / 16;
  u4NewHeight = 16 * (u4NewHeight % 2 ? u4NewHeight + 1 : u4NewHeight);
#endif
  u4YSize = u4NewWidth * u4NewHeight - 1;
  u4CSize = u4NewWidth * u4NewHeight / 2 - 1;
  
  u4GoldSize = u4Width * u4Height;
  u4GoldSize = u4GoldSize + u4GoldSize / 2 - 1;

  printf("d.save.b %s/%s_%07d_Y_bwd.raw 0x%X--0x%X\n", DUMP_BASE_DIR, szDir, n, _u4BwdBufY, _u4BwdBufY + u4YSize);
  printf("d.save.b %s/%s_%07d_C_bwd.raw 0x%X--0x%X\n", DUMP_BASE_DIR, szDir, n, _u4BwdBufC, _u4BwdBufC + u4CSize);
  printf("d.save.b %s/%s_%07d_Y_fwd.raw 0x%X--0x%X\n", DUMP_BASE_DIR, szDir, n, _u4FwdBufY, _u4FwdBufY + u4YSize);
  printf("d.save.b %s/%s_%07d_C_fwd.raw 0x%X--0x%X\n", DUMP_BASE_DIR, szDir, n, _u4FwdBufC, _u4FwdBufC + u4CSize);
  printf("d.save.b %s/%s_%07d_Y.raw 0x%X--0x%X\n", DUMP_BASE_DIR, szDir, n, _u4OutBufY, _u4OutBufY + u4YSize);
  printf("d.save.b %s/%s_%07d_C.raw 0x%X--0x%X\n", DUMP_BASE_DIR, szDir, n, _u4OutBufC, _u4OutBufC + u4CSize);
  printf("d.save.b %s/%s_%07d_bst.bin 0x%X--0x%X\n", DUMP_BASE_DIR, szDir, n, VFIFO_ADDR, VFIFO_ADDR + u4BstLength - 1);
  printf("d.save.b %s/%s_%07d_G.raw 0x%X--0x%X\n", DUMP_BASE_DIR, szDir, n, GOLD_ADDR, GOLD_ADDR + u4GoldSize);
}

void vDispViaIde(UINT32 u4DecWidth, UINT32 u4DecHeight)
{
    UINT32 u4NewHeight = u4DecHeight / 16;
    UINT32 u4Sz;
    UINT32 u4OutBufY, u4OutBufC;
    UINT32 _dwDecBufIdx;
#ifdef MT8222_FAKE_DISPLAY
    UINT32 _dwDispBufIdx;
#endif
    UINT8 *_pbPic0Y, *_pbPic0C;
    UINT8 *_pbPic1Y, *_pbPic1C;
    UINT8 *_pbPic2Y, *_pbPic2C;
    
  #ifdef MT8222_FAKE_DISPLAY
    switch (_dwDispBufIdx)
    {
    case 0:
      u4OutBufY = (UINT32)_pbPic0Y - 0x30000000;
      u4OutBufC = (UINT32)_pbPic0C - 0x30000000;
      break;
      
    case 1:
      u4OutBufY = (UINT32)_pbPic1Y - 0x30000000;
      u4OutBufC = (UINT32)_pbPic1C - 0x30000000;
      break;
      
    case 2:
      u4OutBufY = (UINT32)_pbPic2Y - 0x30000000;
      u4OutBufC = (UINT32)_pbPic2C - 0x30000000;
      break;
    }
  #else
    switch (_dwDecBufIdx)
    {
#if 1
    // display order
    case 0:
      u4OutBufY = (UINT32)_pbPic1Y - 0x30000000;
      u4OutBufC = (UINT32)_pbPic1C - 0x30000000;
      break;
      
    case 1:
      u4OutBufY = (UINT32)_pbPic0Y - 0x30000000;
      u4OutBufC = (UINT32)_pbPic0C - 0x30000000;
      break;
#else
    // decode order
    case 0:
      u4OutBufY = (UINT32)_pbPic0Y - 0x30000000;
      u4OutBufC = (UINT32)_pbPic0C - 0x30000000;
      break;
      
    case 1:
      u4OutBufY = (UINT32)_pbPic1Y - 0x30000000;
      u4OutBufC = (UINT32)_pbPic1C - 0x30000000;
      break;
#endif
    case 2:
      u4OutBufY = (UINT32)_pbPic2Y - 0x30000000;
      u4OutBufC = (UINT32)_pbPic2C - 0x30000000;
      break;
    }
  #endif
    
    if (u4NewHeight % 2)
      u4NewHeight = u4NewHeight * 16 + 16;
    else
      u4NewHeight = u4NewHeight * 16;
    u4Sz = u4DecWidth * u4NewHeight;
    
    _DmxIdeSendDataLen(u4Sz);
    _DmxIdeSendData(u4OutBufY, u4Sz);
    _DmxIdeSendData(u4OutBufC, u4Sz >> 1);
    
    _DmxIdeGetCmpResult();

    // !!! tmp placed here
    if (_u4Uop != 0) return;  // not handled yet
    _u4Uop = _DmxIdeGetUop();
    if (_u4Uop != 0)
    {
      printf("Got UOP: 0x%08X\n", _u4Uop);
      //_u4Uop = 0;
    }
}

#ifndef CRC_MODE
#ifdef GOLDEN_4
static UINT32 mb_to_line_y(UINT32 buf, int width, int x, int y)
{
  UINT32 val = 0;
  int mb_x;
  int mb_y;
  int offset_x;
  int offset_y;
  int offset;

#ifdef DDR3_TEST
  mb_x = x >> 6;
#else
  mb_x = x >> 4;
#endif
  mb_y = y >> 4;

#ifdef DDR3_TEST
  offset_x = x % 64;
#else
  offset_x = x % 16;
#endif
  offset_y = y % 32;

/*
  offset = offset_x + 
           offset_y * 16 +
           mb_x * 16 * 32 +
           (mb_y >> 1) * width * 32;
*/
#ifdef DDR3_TEST
  offset = offset_x + 
           (offset_y << 6) +
           (mb_x << 11) +
           (((mb_y >> 1) * width) << 5);
#else
  offset = offset_x + 
           (offset_y << 4) +
           (mb_x << 9) +
           (((mb_y >> 1) * width) << 5);
#endif
           
  val = *(UINT32 *)(DRAM_NONCACHE_OFFSET + buf + offset);
  _u4ErrPos = (UINT32)buf + offset;
  
  return val;
}

static void mb_to_line_c(UINT32 *pu4Val1, UINT32 *pu4Val2, UINT32 buf, int width, int x, int y)
{
  int mb_x;
  int mb_y;
  int offset_x;
  int offset_y;
  int offset;

#ifdef DDR3_TEST
  mb_x = x >> 5;
#else
  mb_x = x >> 3;
#endif
  mb_y = y >> 3;

#ifdef DDR3_TEST
  offset_x = x % 32;
#else
  offset_x = x % 8;
#endif
  offset_y = y % 16;

/*
  offset = offset_x * 2 + 
           offset_y * 16 +
           mb_x * 16 * 16 +
           (mb_y >> 1) * width * 16;
*/
#ifdef DDR3_TEST
  offset = (offset_x << 1) + 
           (offset_y << 6) +
           (mb_x << 10) +
           (((mb_y >> 1) * width) << 4);
#else
  offset = (offset_x << 1) + 
           (offset_y << 4) +
           (mb_x << 8) +
           (((mb_y >> 1) * width) << 4);
#endif

  *pu4Val1 = *(UINT32 *)(DRAM_NONCACHE_OFFSET + buf + offset);
  *pu4Val2 = *(UINT32 *)(DRAM_NONCACHE_OFFSET + buf + offset + 4);
  _u4ErrPos = (UINT32)buf + offset;
}
#else  // GOLDEN_4
static UINT8 mb_to_line_y(UINT32 buf, int width, int x, int y)
{
  UINT8 val = 0;
  int mb_x;
  int mb_y;
  int offset_x;
  int offset_y;
  int offset;

  mb_x = x >> 4;
  mb_y = y >> 4;

  offset_x = x % 16;
  offset_y = y % 32;

/*
  offset = offset_x + 
           offset_y * 16 +
           mb_x * 16 * 32 +
           (mb_y >> 1) * width * 32;
*/
  offset = offset_x + 
           (offset_y << 4) +
           (mb_x << 9) +
           (((mb_y >> 1) * width) << 5);
           
  val = *(UINT8 *)(DRAM_NONCACHE_OFFSET + buf + offset);
  _u4ErrPos = (UINT32)buf + offset;
  
  return val;
}

static UINT8 mb_to_line_u(UINT32 buf, int width, int x, int y)
{
  UINT8 val = 0;
  int mb_x;
  int mb_y;
  int offset_x;
  int offset_y;
  int offset;

  mb_x = x >> 3;
  mb_y = y >> 3;

  offset_x = x % 8;
  offset_y = y % 16;

/*
  offset = offset_x * 2 + 
           offset_y * 16 +
           mb_x * 16 * 16 +
           (mb_y >> 1) * width * 16;
*/
  offset = (offset_x << 1) + 
           (offset_y << 4) +
           (mb_x << 8) +
           (((mb_y >> 1) * width) << 4);

  val = *(UINT8 *)(DRAM_NONCACHE_OFFSET + buf + offset);
  _u4ErrPos = (UINT32)buf + offset;

  return val;
}

static UINT8 mb_to_line_v(UINT32 buf, int width, int x, int y)
{
  UINT8 val = 0;
  int mb_x;
  int mb_y;
  int offset_x;
  int offset_y;
  int offset;

  mb_x = x >> 3;
  mb_y = y >> 3;

  offset_x = x % 8;
  offset_y = y % 16;

/*
  offset = 1 +
           offset_x * 2 + 
           offset_y * 16 +
           mb_x * 16 * 16 +
           (mb_y >> 1) * width * 16;
*/
  offset = 1 +
           (offset_x << 1) + 
           (offset_y << 4) +
           (mb_x << 8) +
           (((mb_y >> 1) * width) << 4);

  val = *(UINT8 *)(DRAM_NONCACHE_OFFSET + buf + offset);
  _u4ErrPos = (UINT32)buf + offset;

  return val;
}
#endif  // GOLDEN_4
#endif // CRC_MODE


#ifdef GOLDEN_4
static BOOL fgCompareGold(UINT32 n, char *szDir, UINT32 u4DecWidth, UINT32 u4DecHeight, UINT32 u4Width, UINT32 u4Height, BOOL fgFirstTime)
{
#ifdef CRC_MODE
  UINT32 u4Val;
  UINT32 i;
  
  //printf("CRC Y: %08X %08X %08X %08X\n", RISCRead(CRC_BASE + 2*4), RISCRead(CRC_BASE + 3*4), RISCRead(CRC_BASE + 4*4), RISCRead(CRC_BASE + 5*4));
  //printf("CRC C: %08X %08X %08X %08X\n", RISCRead(CRC_BASE + 6*4), RISCRead(CRC_BASE + 7*4), RISCRead(CRC_BASE + 8*4), RISCRead(CRC_BASE + 9*4));
  
  u4Val = *(UINT32 *)(GOLD_ADDR + _u4CRCPtr);  // frame no
  _u4CRCPtr += 4;
  if (u4Val != n)
  {
      printf("!!! CRC frame number mismatch !!!\n");
      return FALSE;
  }
  
  for (i = 0; i < 4; i++)
  {
      UINT32 u4CRC= RISCRead(CRC_BASE + (2+i)*4);
      u4Val = *(UINT32 *)(GOLD_ADDR + _u4CRCPtr);
      _u4CRCPtr += 4;
      if (u4Val != u4CRC)
      {
          printf("!!! CRC Y%d mismatch, %08X v.s %08X !!!\n", i, u4Val, u4CRC);
          return FALSE;
      }
  }

  for (i = 0; i < 4; i++)
  {
      UINT32 u4CRC= RISCRead(CRC_BASE + (6+i)*4);
      u4Val = *(UINT32 *)(GOLD_ADDR + _u4CRCPtr);
      _u4CRCPtr += 4;
      if (u4Val != u4CRC)
      {
          printf("!!! CRC C%d mismatch, %08X v.s %08X !!!\n", i, u4Val, u4CRC);
          return FALSE;
      }
  }

  return TRUE;
#else  // !CRC_MODE
  BOOL fgRet = TRUE;
  BOOL fgYOK = TRUE;
  BOOL fgUOK = TRUE;
  BOOL fgVOK = TRUE;
  int x, y;
  UINT32 u4ErrVal, u4ErrVal2;
  UINT32 u4GolVal, u4GolVal2;
  UINT32 u4UVal, u4VVal;
  UINT32 u4ErrCntY, u4ErrCntC;
  UINT32 u4Total;
  UINT32 u4GoldYSz, u4GoldYUSz;
  int ideRetryCnt;
#ifdef DDR3_TEST
  UINT32 u4DecWidthAlign;
  UINT32 u4WMBSize = u4DecWidth / 16;
  
  if (u4WMBSize % 4)
  {
    u4WMBSize = u4WMBSize + (4 - (u4WMBSize % 4));
  }

  u4DecWidthAlign = u4WMBSize * 16;
#endif

#ifdef VDEC_L2_CACHE
    HalFlushInvalidateDCacheMultipleLine(GOLD_ADDR, GOLD_SZ);
#else
  HalFlushInvalidateDCache();
#endif
  
  if (_fgCmpOnPC)
  {
    UINT32 u4NewHeight = u4DecHeight / 16;
    UINT32 u4Sz;
    BOOL fgRet;
    
    if (u4NewHeight % 2)
      u4NewHeight = u4NewHeight * 16 + 16;
    else
      u4NewHeight = u4NewHeight * 16;
    u4Sz = u4DecWidth * u4NewHeight;
    
    _DmxIdeSendDataLen(u4Sz);
    _DmxIdeSendData(_u4OutBufY, u4Sz);
    _DmxIdeSendData(_u4OutBufC, u4Sz >> 1);
    
    fgRet = _DmxIdeGetCmpResult();
    
    return fgRet;
  }
  
  u4GoldYSz = u4DecWidth * u4DecHeight;
  u4GoldYUSz = u4GoldYSz + u4GoldYSz / 4;
  
  ideRetryCnt = 0;

CMP_BEGIN:
  fgYOK = fgUOK = fgVOK = TRUE;
  
  if (fgFirstTime || ideRetryCnt > 0)
  {
    if (_fgFileFromIDE)
    {
#ifndef MB_GOLDEN
      _DmxIdeGetFile(GOLD_ADDR, 6);
#else
      _DmxIdeGetFile(GOLD_ADDR, 4);
      _DmxIdeGetFile(GOLD_ADDR + u4GoldYSz, 5);
#endif
    }
    else
    {
      UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"%s/%s/g%07d.yuv\" 0x%08X /ny", GOLDEN_BASE_DIR, szDir, n, GOLD_ADDR));
    }
  }
  
  u4ErrCntY = u4Total = 0;
  for (y = 0; y < u4Height; y++)
  {
    for (x = 0; x < u4Width; x += 4)
    {
#ifndef MB_GOLDEN
#ifdef DDR3_TEST
      u4ErrVal = mb_to_line_y(_u4OutBufY, u4DecWidthAlign, x, y);
#else
      u4ErrVal = mb_to_line_y(_u4OutBufY, u4DecWidth, x, y);
#endif
#else
      _u4ErrPos = _u4OutBufY + y * u4DecWidth + x;
      u4ErrVal = *(UINT32 *)(DRAM_NONCACHE_OFFSET + _u4ErrPos);
#endif
      u4GolVal = *(UINT32 *)(GOLD_ADDR + y * u4DecWidth + x);
      if (u4ErrVal != u4GolVal)
      {
        u4ErrCntY++;
        if (_fgShowDiff)
        {
          if (u4ErrCntY == PIXEL_ERR_LOG_CNT + 1)
          {
            printf("!!! too many errors !!!\n");
          }
          else if (u4ErrCntY <= PIXEL_ERR_LOG_CNT)
          {
            printf("y(%4d,%4d) (m,p)(0x%08X,0x%08X) ", x, y, _u4ErrPos, y * u4DecWidth + x);
            printf("(0x%08X,0x%08X)\n", u4ErrVal, u4GolVal);
          }
        }
        fgYOK = FALSE;
      }
      u4Total += 4;
    }
  }
  
  if (!fgYOK)
    printf("Error count y: %d/%d\n", u4ErrCntY, u4Total);
    
  u4ErrCntC = 0;
#ifndef MB_GOLDEN
  for (y = 0; y < u4Height/2; y++)
  {
    for (x = 0; x < u4Width/2; x += 4)
    {
/*      
      u4ErrVal = mb_to_line_c(_u4OutBufC, u4DecWidth, x, y);
      u2UVal = *(UINT16 *)(GOLD_ADDR + 1920*1088 + y * u4DecWidth / 2 + x);
      u2VVal = *(UINT16 *)(GOLD_ADDR + 1920*1088 + 1920*1088/4 + y * u4DecWidth / 2 + x);
      u4GolVal = ((u2VVal << 16) & 0xFF000000) | ((u2UVal << 8) & 0x00FF0000) |
                 ((u2VVal << 8) & 0x0000FF00) | ((u2UVal) & 0x000000FF);
*/
      u4UVal = *(UINT32 *)(GOLD_ADDR + u4GoldYSz  + y * u4DecWidth / 2 + x);
      u4VVal = *(UINT32 *)(GOLD_ADDR + u4GoldYUSz + y * u4DecWidth / 2 + x);

#ifdef DDR3_TEST
      mb_to_line_c(&u4ErrVal, &u4ErrVal2, _u4OutBufC, u4DecWidthAlign, x, y);
#else
      mb_to_line_c(&u4ErrVal, &u4ErrVal2, _u4OutBufC, u4DecWidth, x, y);
#endif
      u4GolVal = ((u4VVal << 16) & 0xFF000000) | ((u4UVal << 8) & 0x00FF0000) |
                 ((u4VVal <<  8) & 0x0000FF00) | ((u4UVal) & 0x000000FF);

      if (u4ErrVal != u4GolVal)
      {
        u4ErrCntC++;
        if (_fgShowDiff)
        {
          if (u4ErrCntC == PIXEL_ERR_LOG_CNT + 1)
          {
            printf("!!! too many errors !!!\n");
          }
          else if (u4ErrCntC <= PIXEL_ERR_LOG_CNT)
          {
            printf("c(%4d,%4d) (m,u,v)(0x%08X,0x%08X,0x%08X) ", x, y, _u4ErrPos, u4GoldYSz + y * u4DecWidth / 2 + x, u4GoldYUSz + y * u4DecWidth / 2 + x);
            printf("(0x%08X,0x%08X)\n", u4ErrVal, u4GolVal);
          }
        }
        fgUOK = fgVOK = FALSE;
      }

      u4GolVal2 = ((u4VVal) & 0xFF000000)      | ((u4UVal >>  8) & 0x00FF0000) |
                  ((u4VVal >> 8) & 0x0000FF00) | ((u4UVal >> 16) & 0x000000FF);
                 
      if (u4ErrVal2 != u4GolVal2)
      {
        u4ErrCntC++;
        if (_fgShowDiff)
        {
          if (u4ErrCntC == PIXEL_ERR_LOG_CNT + 1)
          {
            printf("!!! too many errors !!!\n");
          }
          else if (u4ErrCntC <= PIXEL_ERR_LOG_CNT)
          {
            printf("c(%4d,%4d) (m,u,v)(0x%08X,0x%08X,0x%08X) ", x+2, y, _u4ErrPos + 4, u4GoldYSz + y * u4DecWidth / 2 + x+2, u4GoldYUSz + y * u4DecWidth / 2 + x+2);
            printf("(0x%08X,0x%08X)\n", u4ErrVal2, u4GolVal2);
          }
        }
        fgUOK = fgVOK = FALSE;
      }
    }
  }
#else
  for (y = 0; y < u4Height/2; y++)
  {
    for (x = 0; x < u4Width; x += 4)
    {
      _u4ErrPos = _u4OutBufC + y * u4DecWidth + x;
      u4ErrVal = *(UINT32 *)(DRAM_NONCACHE_OFFSET + _u4ErrPos);
      u4GolVal = *(UINT32 *)(GOLD_ADDR + u4GoldYSz + y * u4DecWidth + x);

      if (u4ErrVal != u4GolVal)
      {
        u4ErrCntC++;
        if (_fgShowDiff)
        {
          if (u4ErrCntC == PIXEL_ERR_LOG_CNT + 1)
          {
            printf("!!! too many errors !!!\n");
          }
          else if (u4ErrCntC <= PIXEL_ERR_LOG_CNT)
          {
            printf("c(%4d,%4d) (m,p)(0x%08X,0x%08X) ", x, y, _u4ErrPos, _u4OutBufC + y * u4DecWidth + x);
            printf("(0x%08X,0x%08X)\n", u4ErrVal, u4GolVal);
          }
        }
        fgUOK = fgVOK = FALSE;
      }
    }
  }
#endif

  if (!fgUOK || !fgVOK)
    printf("Error count c: %d/%d\n", u4ErrCntC, u4Total/2);

  fgRet = fgYOK && fgUOK && fgVOK;
  
  if (_fgFileFromIDE && !fgRet && ideRetryCnt++ < IDE_RETRY)
  {
    printf("%s #%d possible IDE error, retry golden comparison...\n", szDir, n);
    goto CMP_BEGIN;
  }
  
  _u4SumTotal += u4Total;
  _u4SumErrCnt += u4ErrCntY;
  
  //if (n == 186) fgRet = FALSE;

  return (fgRet);
#endif  // !CRC_MODE
}
#else  // GOLDEN_4
static BOOL fgCompareGold(UINT32 n, char *szDir, UINT32 u4DecWidth, UINT32 u4DecHeight, UINT32 u4Width, UINT32 u4Height, BOOL fgFirstTime)
{
  BOOL fgRet = TRUE;
  BOOL fgYOK = TRUE;
  BOOL fgUOK = TRUE;
  BOOL fgVOK = TRUE;
  int x, y;
  UINT8 u1ErrVal;
  UINT8 u1GolVal;
  UINT32 u4ErrCntY, u4ErrCntU, u4ErrCntV;
  UINT32 u4Total;
  UINT32 u4GoldYSz, u4GoldYUSz;
  int ideRetryCnt;

  u4GoldYSz = u4DecWidth * u4DecHeight;
  u4GoldYUSz = u4GoldYSz + u4GoldYSz / 4;

  ideRetryCnt = 0;
  
CMP_BEGIN:
  fgYOK = fgUOK = fgVOK = TRUE;
  
  if (fgFirstTime || ideRetryCnt > 0)
  {
    if (_fgFileFromIDE)
    {
      _DmxIdeGetFile(GOLD_ADDR, 6);
    }
    else
    {
      UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"%s/%s/g%07d.yuv\" 0x%08X /ny", GOLDEN_BASE_DIR, szDir, n, GOLD_ADDR));
    }
  }
  
  u4ErrCntY = u4Total = 0;
  for (y = 0; y < u4Height; y++)
  {
    for (x = 0; x < u4Width; x++)
    {
      u1ErrVal = mb_to_line_y(_u4OutBufY, u4DecWidth, x, y);
      u1GolVal = *(UINT8 *)(GOLD_ADDR + y * u4DecWidth + x);
      if (u1ErrVal != u1GolVal)
      {
        u4ErrCntY++;
        if (_fgShowDiff)
        {
          if (u4ErrCntY == PIXEL_ERR_LOG_CNT + 1)
          {
            printf("!!! too many errors !!!\n");
          }
          else if (u4ErrCntY <= PIXEL_ERR_LOG_CNT)
          {
            printf("y(%4d,%4d) (m,p)(0x%08X,0x%08X) ", x, y, _u4ErrPos, y * u4DecWidth + x);
            printf("(0x%02X,0x%02X)\n", u1ErrVal, u1GolVal);
          }
        }
        fgYOK = FALSE;
      }
      u4Total++;
    }
  }
  
  if (!fgYOK)
    printf("Error count y: %d/%d\n", u4ErrCntY, u4Total);
    
 #if 1  // enable to compare C
  u4ErrCntU = 0;
  for (y = 0; y < u4Height/2; y++)
  {
    for (x = 0; x < u4Width/2; x++)
    {
      u1ErrVal = mb_to_line_u(_u4OutBufC, u4DecWidth, x, y);
      u1GolVal = *(UINT8 *)(GOLD_ADDR + u4GoldYSz + y * u4DecWidth / 2 + x);
      if (u1ErrVal != u1GolVal)
      {
        u4ErrCntU++;
        if (_fgShowDiff)
        {
          if (u4ErrCntU == PIXEL_ERR_LOG_CNT + 1)
          {
            printf("!!! too many errors !!!\n");
          }
          else if (u4ErrCntU <= PIXEL_ERR_LOG_CNT)
          {
            printf("u(%4d,%4d) (m,p)(0x%08X,0x%08X) ", x, y, _u4ErrPos, u4GoldYSz + y * u4DecWidth / 2 + x);
            printf("(0x%02X,0x%02X)\n", u1ErrVal, u1GolVal);
          }
        }
        fgUOK = FALSE;
      }
    }
  }

  if (!fgUOK)
    printf("Error count u: %d/%d\n", u4ErrCnt, u4Total/4);

  u4ErrCntV = 0;
  for (y = 0; y < u4Height/2; y++)
  {
    for (x = 0; x < u4Width/2; x++)
    {
      u1ErrVal = mb_to_line_v(_u4OutBufC, u4DecWidth, x, y);
      u1GolVal = *(UINT8 *)(GOLD_ADDR + u4GoldYUSz + y * u4DecWidth / 2 + x);
      if (u1ErrVal != u1GolVal)
      {
        u4ErrCntV++;
        if (_fgShowDiff)
        {
          if (u4ErrCntV == PIXEL_ERR_LOG_CNT + 1)
          {
            printf("!!! too many errors !!!\n");
          }
          else if (u4ErrCntV <= PIXEL_ERR_LOG_CNT)
          {
            printf("v(%4d,%4d) (m,p)(0x%08X,0x%08X) ", x, y, _u4ErrPos, u4GoldYUSz + y * u4DecWidth / 2 + x);
            printf("(0x%02X,0x%02X)\n", u1ErrVal, u1GolVal);
          }
        }
        fgVOK = FALSE;
      }
    }
  }

  if (!fgVOK)
    printf("Error count v: %d/%d\n", u4ErrCntV, u4Total/4);
 #endif  // 0

  fgRet = fgYOK && fgUOK && fgVOK;
  
  if (_fgFileFromIDE && !fgRet && ideRetryCnt++ < IDE_RETRY)
  {
    printf("%s #%d possible IDE error, retry golden comparison...\n", szDir, n);
    goto CMP_BEGIN;
  }

  _u4SumTotal += u4Total;
  _u4SumErrCnt += u4ErrCntY;
  
  return (fgRet);
}
#endif // GOLDEN_4

static BOOL fgVDecIsVLDFetchOk(void)
{
    if ((u4VDecReadVLD(RO_VLD_FETCHOK) & VLD_FETCH_OK) == 0)
    {
        return (FALSE);
    }
    return (TRUE);
}

static BOOL fgVDecWaitVldFetchOk(void)
{
    UINT32 u4Cnt;
  
    if (!fgVDecIsVLDFetchOk())
    {
        u4Cnt = 0;
        while (!fgVDecIsVLDFetchOk())
        {
            u4Cnt++;
            if (u4Cnt >= WAIT_THRD)
            {
                return (FALSE);
            }
        }
    }

    return (TRUE);
}

static void INIT_BARREL_SHIFTER(void)
{
    INT32 u4Cnt = 0;

    vVDecWriteVLD(RW_VLD_VSTART, VFIFO_ADDR >> 6);
    vVDecWriteVLD(RW_VLD_VEND, (VFIFO_ADDR + VFIFO_SZ) >> 6);

    if (u4VDecReadVLD(RO_VLD_SRAMCTRL) & PROCESS_FLAG)
    {
        while((u4VDecReadVLD(RO_VLD_SRAMCTRL) & AA_FIT_TARGET_SCLK) == 0)
        {
            u4Cnt++;
            if(u4Cnt >= WAIT_THRD)
            {
                LOG(1, "WaitSramStable Fail\n");
                break;
            }
        }
    }

    RISCWrite(RV_VLD_BASE + 0x84, 0x1);

    vVDecWriteVLD(RW_VLD_RPTR, VFIFO_ADDR);
    vVDecWriteVLD(WO_VLD_WPTR, 0xffffffff);

    vVDecWriteVLD(WO_VLD_SRST, 0x100);
    vVDecWriteVLD(WO_VLD_SRST, 0x0);

    // initial fetch
    vVDecWriteVLD(RW_VLD_PROC, VLD_INIFET);

    VERIFY(fgVDecWaitVldFetchOk());

    // initial barrel shift
    vVDecWriteVLD(RW_VLD_PROC, VLD_INIBR);           
}

static UINT32 RV_VLD_SHIFT_BIT(UINT32 u4ShiftCnt)
{
  UINT32 u4Val;
  
  if (u4ShiftCnt == 0)
  {
    u4Val = RISCRead(RV_VLD_BASE);
  }
  else
  {
    while (u4ShiftCnt > 0)
    {
      u4Val = RISCRead(RV_VLD_BASE + 4);
      u4ShiftCnt--;
    }
  }
  
  return u4Val;
}

static void vVldSoftReset(void)
{
    UINT32 u4Cnt = 0;
    if (u4VDecReadVLD(RO_VLD_SRAMCTRL) & PROCESS_FLAG)
    {
        while((u4VDecReadVLD(RO_VLD_SRAMCTRL) & AA_FIT_TARGET_SCLK) == 0)
        {
            u4Cnt++;
            if(u4Cnt >= WAIT_THRD)
            {
                LOG(1, "WaitSramStable Fail\n");
                break;
            }
        }
    }

    vVDecWriteVLD(WO_VLD_SRST, 0x101);
    if (!BSP_IsFPGA())
    {
	RISCWrite(CKGEN_BASE + 0x224, 7);
    }
#ifdef POWER_SAVING_TEST
    RISCWrite(MISC_BASE + 0xC8, 0xEF);
    RISCWrite(MISC_BASE + 0xCC, 0x3B749100);
#endif
    if (!BSP_IsFPGA())
    {
	RISCWrite(MISC_BASE + 0x84, 0);
    }
    vVDecWriteVLD(WO_VLD_SRST, 0);
}

static void vMvInit(void)
{
  // MV global setting
  RISCWrite(AVC_MV_BASE + 131 * 4, MV_DIRECT_ADDR >> 4);
  RISCWrite(AVC_MV_BASE + 134 * 4, 0x0000);
}

static void vPpInit(void)
{
  // PP global setting
}

static void vMcInit(void)
{
  // MC global setting
  //RISCWrite(MC_BASE + 39*4, (MC_REF_OFFSET/512) + 0x2FD0);
  //RISCWrite(MC_BASE + 40*4, (MC_REF_OFFSET/256) + 0x7F80);

  RISCWrite(MC_BASE + 28*4, 0x0);
#ifdef DDR3_TEST
  RISCWrite(MC_BASE + 36*4, 0x2);
#else
  RISCWrite(MC_BASE + 36*4, 0x4);
#endif
  RISCWrite(MC_BASE +  6*4, 0x0);
  RISCWrite(MC_BASE + 38*4, 0x0);
  RISCWrite(MC_BASE + 114*4, 0x0);
  RISCWrite(MC_BASE + 9*4, 0x4);
  
  // requested by ChunChia Chen, 20091008
  //RISCWrite(MC_BASE + 453*4, (RISCRead(MC_BASE + 453*4) & 0xFFFFFFFE)); // no need, could use default after 5368
  //RISCWrite(MC_BASE + 488*4, 4); // no need, could use default after 5368

#ifdef DDR3_TEST
  RISCWrite(MC_BASE + 511*4, 1);
  RISCWrite(MC_BASE + 512*4, 1);
  
  RISCWrite(MC_BASE + 474*4, 0x2);
  
  RISCWrite(MC_BASE + 525*4, 0);
  
  RISCWrite(MC_BASE + 351*4, 0); // 0: enable pp wrapper
#else
  RISCWrite(MC_BASE + 351*4, 0); // 0: enable pp wrapper
#endif
  //RISCWrite(MC_BASE + 515*4, RISCRead(MC_BASE + 515*4) & 0xFFFFFFEF);
}

static void vVldInit(void)
{
#if 1 // 5368
  RISCWrite(VLD_TOP_BASE + 20*4, 0x80000000);
  RISCWrite(VLD_TOP_BASE + 10*4, VLD_PRED_OFFSET);
//  RISCWrite(VLD_TOP_BASE + 11*4, 0x40000000);
#else
  RISCWrite(RV_VLD_BASE + 38*4, 0x80000000);
  RISCWrite(RV_VLD_BASE + 39*4, /*0x40000000 | */VLD_PRED_OFFSET);
#endif
}

#if 0
static void vDumpMcReg(void)
{
    UINT32 i;
    
    for (i = 0; i <= 0xB00; i+=4)
        printf("MC %04X = 0x%08X\n", i, RISCRead(MC_BASE + i));
}
#endif


#define IMG_RESZ_START            0
#define IMG_RESZ_TYPE             1
#define IMG_RESZ_JPG_MODE         2
#define RZ_MEM_IF_MODE            3
#define RZ_SRC_BUF_LEN            4
#define RZ_JPEG_INT_SWITCH        5
#define RZ_TG_BUF_LEN             6
#define RZ_SRC_Y_OW_ADDR_BASE1    7
#define RZ_SRC_Y_OW_ADDR_BASE2    8
#define RZ_SRC_CB_OW_ADDR_BASE1   9
#define RZ_SRC_CB_OW_ADDR_BASE2   10
#define RZ_SRC_CR_OW_ADDR_BASE1   11
#define RZ_SRC_CR_OW_ADDR_BASE2   12
#define RZ_TG_Y_OW_ADDR_BASE      13
#define RZ_TG_C_OW_ADDR_BASE      14
#define RZ_SRC_SIZE_Y             16
#define RZ_SRC_SIZE_CB            17
#define RZ_SRC_SIZE_CR            18
#define RZ_TG_SIZE_Y              19
#define RZ_TG_SIZE_C              20
#define RZ_SRC_OFFSET_Y           21
#define RZ_SRC_OFFSET_C           22
#define RZ_SRC_OFFSET_CR          23
#define RZ_TG_OFFSET_Y            24
#define RZ_H8TAPS_SCL_Y           25
#define RZ_H8TAPS_SCL_CB          26
#define RZ_H8TAPS_SCL_CR          27
#define RZ_HEC_SCL_Y              28
#define RZ_HEC_SCL_CB             29
#define RZ_HEC_SCL_CR             30
#define RZ_V_SCL_Y                31
#define RZ_V_SCL_CB               32
#define RZ_V_SCL_CR               33
#define RZ_V4TAPS_SCL_Y           34
#define RZ_V4TAPS_SCL_CB          35
#define RZ_V4TAPS_SCL_CR          36
#define RZ_TMP_Y_OW_ADDR_BASE     37
#define RZ_PLD_Y_OW_ADDR_BASE     38
#define RZ_PLD_C_OW_ADDR_BASE     39
#define RZ_CPU_ASSIGN             40
#define RZ_SRC_CNT_Y              41
#define RZ_SRC_CNT_CB             42
#define RZ_SRC_CNT_CR             43
#define RZ_V_WR_OFFSET_Y          44
#define RZ_V_WR_OFFSET_CB         45
#define RZ_V_WR_OFFSET_CR         46
#define RZ_V_NEXT_C_Y             47
#define RZ_V_NEXT_C_CB            48
#define RZ_V_NEXT_C_CR            49
#define RZ_H8TAPS_OFT_Y           50
#define RZ_H8TAPS_OFT_CB          51
#define RZ_H8TAPS_OFT_CR          52
#define RZ_V4TAPS_OFT_Y           53
#define RZ_V4TAPS_OFT_CB          54
#define RZ_V4TAPS_OFT_CR          55
#define RZ_SPARE_REG1             56
#define RZ_SPARE_REG2             57
#define RZ_CHK_SUM                58 // read only
#define RZ_MONITOR_INT            59 // read only
#define RZ_MONITOR_FSM            60 // read only
#define RZ_MONITOR_STA            61 // read only
#define RZ_MONITOR_DATA           62 // read only
#define IMG_RESZ_DONE             63 // read only
#define IMG_RESZ_OSD_MODE         64
#define RZ_OSD_CTRL               65
#define RZ_OSD_A_TABLE            66
#define RZ_OSD_COLOR_TRANS        67
#define RZ_OSD_COLOR_TRANS1       68
#define RZ_OSD_COLOR_TRANS2       69
#define RZ_OSD_COLOR_TRANS3       70
#define RZ_OSD_CPT_ADDR           71
#define RZ_OSD_CPT_DATA           72
#define RZ_H_COEF0                73
#define RZ_H_COEF1                74
#define RZ_H_COEF2                75
#define RZ_H_COEF3                76
#define RZ_H_COEF4                77
#define RZ_H_COEF5                78
#define RZ_H_COEF6                79
#define RZ_H_COEF7                80
#define RZ_H_COEF8                81
#define RZ_H_COEF9                82
#define RZ_H_COEF10               83
#define RZ_H_COEF11               84
#define RZ_H_COEF12               85
#define RZ_H_COEF13               86
#define RZ_H_COEF14               87
#define RZ_H_COEF15               88
#define RZ_H_COEF16               89
#define RZ_H_COEF17               90
#define RZ_V_COEF0                91
#define RZ_V_COEF1                92
#define RZ_V_COEF2                93
#define RZ_V_COEF3                94
#define RZ_V_COEF4                95
#define RZ_V_COEF5                96
#define RZ_V_COEF6                97
#define RZ_V_COEF7                98
#define RZ_V_COEF8                99
#define RZ_DITHER                 100
#define RZ_CSC                    101
#define RZ_CSC_COEFF_11           102
#define RZ_CSC_COEFF_12           103
#define RZ_CSC_COEFF_13           104
#define RZ_CSC_COEFF_21           105
#define RZ_CSC_COEFF_22           106
#define RZ_CSC_COEFF_23           107
#define RZ_CSC_COEFF_31           108
#define RZ_CSC_COEFF_32           109
#define RZ_CSC_COEFF_33           110
#define RZ_LUMA_KEY               111                         
#define RZ_COLOR_SEL              112                         
#define RZ_JPG_V_Y                113
#define RZ_JPG_V_CB               114
#define RZ_JPG_V_CR               115
#define RZ_RPR                    116
#define RZ_DRAMQ_CFG              117
#define RZ_DRAMQ_STAD             118
#define RZ_DRAMQ_LEN              119
#define RZ_DRAMQ_RDPTR            120  
#define RZ_DRAMQ_EXPTR            121  // read only  
#define RZ_DUMMY                  122
#define RZ_CMDQDEBUG0             123
#define RZ_CMDQDEBUG1             124
#define RZ_CMDQDEBUG2             125
#define RZ_CMDQDEBUG3             126
#define RZ_ECO                    127

static UINT32 _u4Trap;

static void _VDEC_RvSetDramBusy(void) // 5368
{
    RISCWrite(IO_VIRT + 0x07200 + 0x18, RISCRead(IO_VIRT + 0x07200 + 0x18) | ((UINT32)1 << 25) | ((UINT32)1 <<31));

    RISCWrite(IO_VIRT + 0x07200 + 0x10, DRAM_BUSY_ADDR);
    RISCWrite(IO_VIRT + 0x07200 + 0x14, DRAM_BUSYSZ);

    RISCWrite(IO_VIRT + 0x7100 + 0x4, RISCRead(IO_VIRT + 0x7100 + 0x4) & 0xFFFFFFF);

    RISCWrite(IO_VIRT + 0x7100 + 0x14, 0xFFFFFF11);
}

extern void IMGRZ_HwReset(void);
static void vRprImgResz(
  UINT32 u4PrevPicDecWidth, UINT32 u4PrevPicDecHeight,
  UINT32 u4PrevPicDispWidth, UINT32 u4PrevPicDispHeight,
  UINT32 u4CurrPicDecWidth, UINT32 u4CurrPicDecHeight,
  UINT32 u4CurrPicDispWidth, UINT32 u4CurrPicDispHeight)
{
  UINT32 qw;
  UINT32 hfactor;
  UINT32 hoffset;
  UINT32 vfactor;
  UINT32 voffset;

  int  uInWidth, uInHeight;
  int  uOutWidth, uOutHeight;
  int  iUxR, iUyLB;
  int  Hprime, D;
  int  m, n;
  int  ax_initial, ax_increment;
  int  iUyL_inc;
  int  uTemp;
  
  //u4CurrPicDecWidth = u4PrevPicDecWidth;
  //u4CurrPicDecHeight = u4PrevPicDecHeight;
  //u4CurrPicDispWidth = u4PrevPicDispWidth;
  //u4CurrPicDispHeight = u4PrevPicDispHeight;

  //int_clr = 0;

  IMGRZ_HwReset();

  ASSERT((RISCRead(IMG_RESZ_BASE + IMG_RESZ_DONE * 4) & 1) == 0);

#ifdef DDR3_TEST
  RISCWrite(IMG_RESZ_BASE + RZ_MEM_IF_MODE * 4, RISCRead(IMG_RESZ_BASE + RZ_MEM_IF_MODE * 4) & 0xFFFFFF00 | 0x66);
#endif
  
  if (_eRprRacStt == RPR_RAC_SCP2_DTCED)
  {
    RISCWrite(IMG_RESZ_BASE + RZ_RPR * 4, 0x00000301);
    RISCWrite(IMG_RESZ_BASE + IMG_RESZ_JPG_MODE * 4, 0x8001E000);
    RISCWrite(IMG_RESZ_BASE + RZ_JPG_V_Y * 4, 0x00000020);
    RISCWrite(IMG_RESZ_BASE + RZ_JPG_V_CB * 4, 0x00000010);
  }
  else
  {
    RISCWrite(IMG_RESZ_BASE + RZ_RPR * 4, 1); // RPR_test, urcrpr = 0
  }

  qw = 0;
  RISCWrite(IMG_RESZ_BASE + IMG_RESZ_TYPE * 4, qw);

  qw = u4PrevPicDecWidth / 16;
#ifdef DDR3_TEST
  qw = (qw + 3) / 4 * 4;
#endif
  ASSERT(qw < ((UINT32)1 << 12));
  qw = (qw << 12) | qw;
  RISCWrite(IMG_RESZ_BASE + RZ_SRC_BUF_LEN * 4, qw);

//  qw = 0x04000000 | (u4CurrPicDecWidth / 16);
  qw = u4CurrPicDecWidth / 16;
#ifdef DDR3_TEST
  qw = (qw + 3) / 4 * 4;
#endif
  ASSERT(qw < ((UINT32)1 << 12));
  qw = 0x10000000 | (qw << 12) | qw;
  RISCWrite(IMG_RESZ_BASE + RZ_TG_BUF_LEN * 4, qw);
  
  if (_eRprRacStt == RPR_RAC_SCP2_DTCED)
  {
    RISCWrite(IMG_RESZ_BASE + RZ_SRC_Y_OW_ADDR_BASE1 * 4, _u4OutBufY/16);
    RISCWrite(IMG_RESZ_BASE + RZ_SRC_CB_OW_ADDR_BASE1 * 4, _u4OutBufC/16);
  }
  else
  {
    RISCWrite(IMG_RESZ_BASE + RZ_SRC_Y_OW_ADDR_BASE1 * 4, _u4FwdBufY/16);
    RISCWrite(IMG_RESZ_BASE + RZ_SRC_CB_OW_ADDR_BASE1 * 4, _u4FwdBufC/16);
  }
  
  RISCWrite(IMG_RESZ_BASE + RZ_TG_Y_OW_ADDR_BASE * 4, PPY_OUT_BUFR/16);
  RISCWrite(IMG_RESZ_BASE + RZ_TG_C_OW_ADDR_BASE * 4, PPC_OUT_BUFR/16);

  RISCWrite(IMG_RESZ_BASE + RZ_TMP_Y_OW_ADDR_BASE * 4, TMP_ADDR/16);
  
  qw = (u4PrevPicDispWidth << 16) | u4PrevPicDispHeight;
  RISCWrite(IMG_RESZ_BASE + RZ_SRC_SIZE_Y * 4, qw); //src_width_y, src_height_y    

  qw = ((u4PrevPicDispWidth >> 1) << 16) | (u4PrevPicDispHeight >> 1);
  RISCWrite(IMG_RESZ_BASE + RZ_SRC_SIZE_CB * 4, qw); //src_width_c, src_height_c      

//  qw = (u4CurrPicDecWidth) << 12 | u4CurrPicDecHeight;
  qw = (u4CurrPicDecWidth) << 16 | u4CurrPicDecHeight; // david
  RISCWrite(IMG_RESZ_BASE + RZ_TG_SIZE_Y * 4, qw); //tg_width_y, tg_height_y

  // horizontal bilinear
  RISCWrite(IMG_RESZ_BASE + RZ_H_COEF0 * 4, 0x0);
  RISCWrite(IMG_RESZ_BASE + RZ_H_COEF1 * 4, 0xf0000000);
  RISCWrite(IMG_RESZ_BASE + RZ_H_COEF2 * 4, 0xe0000000);
  RISCWrite(IMG_RESZ_BASE + RZ_H_COEF3 * 4, 0xd0000000);
  RISCWrite(IMG_RESZ_BASE + RZ_H_COEF4 * 4, 0xc0000000);
  RISCWrite(IMG_RESZ_BASE + RZ_H_COEF5 * 4, 0xb0000000);
  RISCWrite(IMG_RESZ_BASE + RZ_H_COEF6 * 4, 0xa0000000);
  RISCWrite(IMG_RESZ_BASE + RZ_H_COEF7 * 4, 0x90000000);
  RISCWrite(IMG_RESZ_BASE + RZ_H_COEF8 * 4, 0x80000000);
  RISCWrite(IMG_RESZ_BASE + RZ_H_COEF9 * 4, 0x70000000);
  RISCWrite(IMG_RESZ_BASE + RZ_H_COEF10 * 4, 0x60000000);
  RISCWrite(IMG_RESZ_BASE + RZ_H_COEF11 * 4, 0x50000000);
  RISCWrite(IMG_RESZ_BASE + RZ_H_COEF12 * 4, 0x40000000);
  RISCWrite(IMG_RESZ_BASE + RZ_H_COEF13 * 4, 0x30000000);
  RISCWrite(IMG_RESZ_BASE + RZ_H_COEF14 * 4, 0x20000000);
  RISCWrite(IMG_RESZ_BASE + RZ_H_COEF15 * 4, 0x10000000);
  RISCWrite(IMG_RESZ_BASE + RZ_H_COEF16 * 4, 0x00000008);
  RISCWrite(IMG_RESZ_BASE + RZ_H_COEF17 * 4, 0x00000000);

  // vertical bilinear 
  RISCWrite(IMG_RESZ_BASE + RZ_V_COEF0 * 4, 0xf0000000);
  RISCWrite(IMG_RESZ_BASE + RZ_V_COEF1 * 4, 0xd000e000);
  RISCWrite(IMG_RESZ_BASE + RZ_V_COEF2 * 4, 0xb000c000);
  RISCWrite(IMG_RESZ_BASE + RZ_V_COEF3 * 4, 0x9000a000);
  RISCWrite(IMG_RESZ_BASE + RZ_V_COEF4 * 4, 0x70008000);
  RISCWrite(IMG_RESZ_BASE + RZ_V_COEF5 * 4, 0x50006000);
  RISCWrite(IMG_RESZ_BASE + RZ_V_COEF6 * 4, 0x30004000);
  RISCWrite(IMG_RESZ_BASE + RZ_V_COEF7 * 4, 0x10002000);
  RISCWrite(IMG_RESZ_BASE + RZ_V_COEF8 * 4, 0x1);
  
  // -- scaling factor calculation --
  uInWidth   = u4PrevPicDispWidth;
  uInHeight  = u4PrevPicDispHeight;
  uOutWidth  = u4CurrPicDispWidth;
  uOutHeight = u4CurrPicDispHeight;
  
  m = 0;
  uTemp = uInWidth;
  while (uTemp > 0)
  {
    m=m+1;
    uTemp = uTemp>>1;
  }
  /* check for case when uInWidth is power of two */
  if (uInWidth == (1<<(m-1))) m=m-1;
  Hprime = 1<<m;
  D = (64*Hprime)/16;

  n = 0;
  uTemp = uInHeight;
  while (uTemp > 0)
  {
    n=n+1;
    uTemp = uTemp>>1;
  }
  /* check for case when uInHeight is power of two */
  if (uInHeight == (1<<(n-1))) n=n-1;

  /* iUxL and iUxR are independent of row, so compute once only */
  iUxR = ((((uInWidth - uOutWidth)<<1))<<(4+m));    /* numerator part */
  /* complete iUxR init by dividing by H with rounding to nearest integer, */
  /* half-integers away from 0 */
  if (iUxR >= 0)
    iUxR = (iUxR + (uOutWidth>>1))/uOutWidth;
  else
    iUxR = (iUxR - (uOutWidth>>1))/uOutWidth;

  /* initial x displacement and the x increment are independent of row */
  /* so compute once only */
  ax_initial = iUxR + (D>>1);
  ax_increment = (Hprime<<6) + (iUxR<<1);

  iUyLB = ((uInHeight - uOutHeight)<<(n+5)); /* numerator */
  /* complete iUyLB by dividing by V with rounding to nearest integer, */
  /* half-integers away from 0 */
  if (iUyLB >= 0)
    iUyLB = (iUyLB + (uOutHeight>>1))/uOutHeight;
  else
    iUyLB = (iUyLB - (uOutHeight>>1))/uOutHeight;
  iUyL_inc = (iUyLB<<1);

  hfactor = ax_increment<<(18-(m+6));
  hoffset = ax_initial<<(18-(m+6));
  vfactor = ((1<<(6+n)) + iUyL_inc) << (18-(n+6));
  voffset = (iUyLB + (1<<(1+n))) << (18-(n+6));

  RISCWrite(IMG_RESZ_BASE + RZ_H8TAPS_SCL_Y * 4,  hfactor); 
  RISCWrite(IMG_RESZ_BASE + RZ_H8TAPS_OFT_Y * 4,  hoffset); 
  RISCWrite(IMG_RESZ_BASE + RZ_V4TAPS_SCL_Y * 4,  vfactor);
  RISCWrite(IMG_RESZ_BASE + RZ_V4TAPS_OFT_Y * 4,  voffset);
  RISCWrite(IMG_RESZ_BASE + RZ_H8TAPS_SCL_CB * 4, hfactor); 
  RISCWrite(IMG_RESZ_BASE + RZ_H8TAPS_OFT_CB * 4, hoffset); 
  RISCWrite(IMG_RESZ_BASE + RZ_V4TAPS_SCL_CB * 4, vfactor);
  RISCWrite(IMG_RESZ_BASE + RZ_V4TAPS_OFT_CB * 4, voffset);

  //RISCWrite(IMG_RESZ_BASE + IMG_RESZ_START * 4, 0x8E); // soft reset
  //RISCWrite(IMG_RESZ_BASE + IMG_RESZ_START * 4, 0x82);    
  RISCWrite(IMG_RESZ_BASE + IMG_RESZ_START * 4, 0xC2);
  RISCWrite(IMG_RESZ_BASE + IMG_RESZ_START * 4, 0xC3); // activate                

  if (_eRprRacStt == RPR_RAC_SCP2_DTCED)  // since it will never finish without triggering vdec
    return;
    
  _u4Trap = 0;
  while ((RISCRead(IMG_RESZ_BASE + IMG_RESZ_DONE * 4) & 1) == 0)
    _u4Trap++;

//printf("RZ-Y: d.save.b V:\\MT5368\\RV\\y.raw 0x%08x--0x%08x\n", PPY_OUT_BUFR, PPY_OUT_BUFR + u4CurrPicDecWidth * u4CurrPicDecHeight - 1);
//printf("RZ-C: d.save.b V:\\MT5368\\RV\\c.raw 0x%08x--0x%08x\n", PPC_OUT_BUFR, PPC_OUT_BUFR + u4CurrPicDecWidth * u4CurrPicDecHeight / 2 - 1);
//d.save.b V:\MT5368\RV\y.raw 0x03294000--0x032a0fff
//d.save.b V:\MT5368\RV\c.raw 0x03492000--0x034987ff

  //int_clr=1; 
  //int_clr=0; 
}

static BOOL fgTestRvDec(RvPicAttr *prPic, BOOL fgFirstTime, UINT32 n)
{
  int i;
  UINT32 u4PpRvPara;
  UINT32 u4Reg;
  volatile UINT32 u4Wait;
#ifdef DDR3_TEST
  UINT32 u4MbW = prPic->u4Width / 16;
  u4MbW = (u4MbW + 3) / 4 * 4;
#endif
  //printf("Test RV picture decode\n");

  vVldSoftReset();
  
  vMvInit();
  RISCWrite(AVC_MV_BASE + 135 * 4, prPic->u4Iratio & 0x0000FFFF);
  
  vMcInit();

  vPpInit();
  
  // PP setting
  if (fgFirstTime)
  {
    if (prPic->ePtype == RV_INTRAPIC || prPic->ePtype == RV_FORCED_INTRAPIC)
    {
      if (_u4FwdBufY == 0)  // the first I pic
      {
        _u4OutBufY = PPY_OUT_BUF1;
        _u4OutBufC = PPC_OUT_BUF1;
        _u4FwdBufY = PPY_OUT_BUF1;
        _u4FwdBufC = PPC_OUT_BUF1;
        _u4BwdBufY = 0;
        _u4BwdBufC = 0;
      }
      else if (_u4BwdBufY == PPY_OUT_BUF1)
      {
        _u4OutBufY = PPY_OUT_BUF2;
        _u4OutBufC = PPC_OUT_BUF2;
        _u4FwdBufY = PPY_OUT_BUF1;
        _u4FwdBufC = PPC_OUT_BUF1;
        _u4BwdBufY = PPY_OUT_BUF2;
        _u4BwdBufC = PPC_OUT_BUF2;
        _fgBwdIsI = TRUE;
      }
      else if (_u4BwdBufY == PPY_OUT_BUF2)
      {
        _u4OutBufY = PPY_OUT_BUF1;
        _u4OutBufC = PPC_OUT_BUF1;
        _u4FwdBufY = PPY_OUT_BUF2;
        _u4FwdBufC = PPC_OUT_BUF2;
        _u4BwdBufY = PPY_OUT_BUF1;
        _u4BwdBufC = PPC_OUT_BUF1;
        _fgBwdIsI = TRUE;
      }
      else if (_u4BwdBufY == 0)  // should happen only when 2 consecutive I pictures at the beginning
      {
        _u4OutBufY = PPY_OUT_BUF2;
        _u4OutBufC = PPC_OUT_BUF2;
        _u4FwdBufY = PPY_OUT_BUF1;
        _u4FwdBufC = PPC_OUT_BUF1;
        _u4BwdBufY = PPY_OUT_BUF2;
        _u4BwdBufC = PPC_OUT_BUF2;
        _fgBwdIsI = TRUE;
      }
      else
      {
        ASSERT(0);
      }

#if defined(RPR_RACING) && 1  // megaa 20080116, for realvideo10_56 #395 case: P B I Psc
      if (_eRprRacStt == RPR_RAC_SCP2_DTCED)
      {
        printf("About to trigger image resizer in racing mode...\n");
        vRprImgResz(
          prPic->u4Width, prPic->u4Height,
          prPic->u4OrgWidth, prPic->u4OrgHeight,
          rTmpPic.u4Width, rTmpPic.u4Height,
          rTmpPic.u4OrgWidth, rTmpPic.u4OrgHeight
        );
        printf("Image resizer is now waiting for vdec...\n");
        _eRprRacStt = RPR_RAC_SCP1_PENDING;
      }
#endif

      // initialize the previous picture sizes
      _u4PrevDispWidth = prPic->u4OrgWidth;
      _u4PrevDispHeight = prPic->u4OrgHeight;
      _u4PrevDecWidth = prPic->u4Width;
      _u4PrevDecHeight = prPic->u4Height;
    }
    else if (prPic->ePtype == RV_INTERPIC)
    {
      _fgBwdIsI = FALSE;
      
      if (_u4BwdBufY == 0)  // the first P pic
      {
        _u4OutBufY = PPY_OUT_BUF2;
        _u4OutBufC = PPC_OUT_BUF2;
        _u4FwdBufY = PPY_OUT_BUF1;
        _u4FwdBufC = PPC_OUT_BUF1;
        _u4BwdBufY = PPY_OUT_BUF2;
        _u4BwdBufC = PPC_OUT_BUF2;
      }
      else if (_u4BwdBufY == PPY_OUT_BUF1)
      {
        _u4OutBufY = PPY_OUT_BUF2;
        _u4OutBufC = PPC_OUT_BUF2;
        _u4FwdBufY = PPY_OUT_BUF1;
        _u4FwdBufC = PPC_OUT_BUF1;
        _u4BwdBufY = PPY_OUT_BUF2;
        _u4BwdBufC = PPC_OUT_BUF2;
      }
      else if (_u4BwdBufY == PPY_OUT_BUF2)
      {
        _u4OutBufY = PPY_OUT_BUF1;
        _u4OutBufC = PPC_OUT_BUF1;
        _u4FwdBufY = PPY_OUT_BUF2;
        _u4FwdBufC = PPC_OUT_BUF2;
        _u4BwdBufY = PPY_OUT_BUF1;
        _u4BwdBufC = PPC_OUT_BUF1;
      }
      else
      {
        ASSERT(0);
      }
      
#ifdef RPR_RACING
      if (_eRprRacStt == RPR_RAC_SCP2_DTCED)
      {
        printf("About to trigger image resizer in racing mode...\n");
        vRprImgResz(
          prPic->u4Width, prPic->u4Height,
          prPic->u4OrgWidth, prPic->u4OrgHeight,
          rTmpPic.u4Width, rTmpPic.u4Height,
          rTmpPic.u4OrgWidth, rTmpPic.u4OrgHeight
        );
        printf("Image resizer is now waiting for vdec...\n");
        _eRprRacStt = RPR_RAC_SCP1_PENDING;
      }
      else  // to avoid the next if block
#endif
      if (prPic->u4OrgWidth != _u4PrevDispWidth || prPic->u4OrgHeight != _u4PrevDispHeight)  // RPR
      {
#ifdef RPR_RACING
        if (_eRprRacStt == RPR_RAC_NOTHING)
#endif
        {
          printf("About to trigger image resizer...\n");
          vRprImgResz(
            _u4PrevDecWidth, _u4PrevDecHeight,
            _u4PrevDispWidth, _u4PrevDispHeight,
            prPic->u4Width, prPic->u4Height,
            prPic->u4OrgWidth, prPic->u4OrgHeight
          );
          printf("Image has been resized!\n");
        }
#ifdef RPR_RACING
        //ASSERT(_eRprRacStt == RPR_RAC_SCP1_DONE);
        _eRprRacStt = RPR_RAC_NOTHING;
#endif
        _u4FwdBufY = PPY_OUT_BUFR;
        _u4FwdBufC = PPC_OUT_BUFR;
      }
      
      _u4PrevDispWidth = prPic->u4OrgWidth;
      _u4PrevDispHeight = prPic->u4OrgHeight;
      _u4PrevDecWidth = prPic->u4Width;
      _u4PrevDecHeight = prPic->u4Height;
    }
    else if (prPic->ePtype == RV_TRUEBPIC)
    {
      _u4OutBufY = PPY_OUT_BUF3;
      _u4OutBufC = PPC_OUT_BUF3;
	  RISCWrite(AVC_MV_BASE + 134 * 4, _fgBwdIsI);
    }
    else
    {
      ASSERT(0);
    }
  }
#if 1
  else if (_fgSetAvcMv134 && n == _u4SetAvcMv134OfFrameNo)
	  RISCWrite(AVC_MV_BASE + 134 * 4, 1); // for debug 1107
#endif

  //RISCWrite(MC_BASE + 39*4, PPY_OUT_BUF2 >> 9);
  //RISCWrite(MC_BASE + 40*4, PPC_OUT_BUF2 >> 8);
  RISCWrite(MC_BASE + 39*4, _u4OutBufY >> 9);
  RISCWrite(MC_BASE + 40*4, _u4OutBufC >> 8);
  RISCWrite(MC_BASE + 137*4, _u4OutBufY >> 9);
  RISCWrite(MC_BASE + 138*4, _u4OutBufC >> 8);
#if MC_PP_MODE == 1
  RISCWrite(MC_BASE + 136*4, 1);
#elif MC_PP_MODE == 0
  RISCWrite(MC_BASE + 136*4, (prPic->u4DFP == 0 ? 1 : 0));
#endif
#ifdef DDR3_TEST
  RISCWrite(MC_BASE + 139*4, u4MbW);
#else
  RISCWrite(MC_BASE + 139*4, prPic->u4Width / 16);
#endif
  RISCWrite(MC_BASE + 142*4, (prPic->u4DFP == 0 ? ((!prPic->fgRV9 && (prPic->ePtype != RV_INTRAPIC && prPic->ePtype != RV_FORCED_INTRAPIC)) ? 2 : 3) : 0));
  //RISCWrite(MC_BASE + 142*4, 2);
#if MC_PP_MODE == 1
  RISCWrite(MC_BASE + 148*4, 1);
#elif MC_PP_MODE == 0
  RISCWrite(MC_BASE + 148*4, (prPic->u4DFP == 0 ? 1 : 0));
#endif
  RISCWrite(MC_BASE + 152*4, prPic->u4Width / 16 - 1);
  RISCWrite(MC_BASE + 153*4, prPic->u4Height / 16 - 1);
  if (prPic->ePtype == RV_TRUEBPIC)
    u4PpRvPara = _u4RefQpMb0 << 16;
  else
    u4PpRvPara = prPic->u4Pquant << 16;
  u4PpRvPara |= (prPic->ePtype == RV_TRUEBPIC ? 1 : 0) << 11;
  u4PpRvPara |= (prPic->ePtype == RV_TRUEBPIC ? 1 : 0) << 10;
  u4PpRvPara |= prPic->u4DFP << 9;
  u4PpRvPara |= (prPic->u4Width * prPic->u4Height <= 176 * 144 ? 1 : 0) << 8;
  u4PpRvPara |= _u4Rv8TblNum << 4;
  u4PpRvPara |= (prPic->fgRV9 ? 1 : 0) << 1;
  u4PpRvPara |= (prPic->fgRV9 ? 0 : 1);
  RISCWrite(PP_BASE + 2*4, u4PpRvPara);  // !!! 0x110011

  // MC setting
  if (prPic->ePtype == RV_INTERPIC || prPic->ePtype == RV_TRUEBPIC)
  {
    RISCWrite(MC_BASE +  0*4, _u4FwdBufY >> 9);
    RISCWrite(MC_BASE +  1*4, _u4FwdBufC >> 8);
  }
  if (prPic->ePtype == RV_TRUEBPIC)
  {
    RISCWrite(MC_BASE +  2*4, _u4BwdBufY >> 9);
    RISCWrite(MC_BASE +  3*4, _u4BwdBufC >> 8);
  }
  RISCWrite(MC_BASE + 420*4, prPic->u4Iratio);       // B-picture I ratio
  RISCWrite(MC_BASE + 130*4, prPic->u4Width);
  RISCWrite(MC_BASE + 131*4, prPic->u4Height);

#if 1 // david: turn off mc_new_bandwidth_mode
  if (_fgSetMc474)
  {
	RISCWrite(MC_BASE + 474*4, RISCRead(MC_BASE + 474*4) | 0x100);
  }
  RISCWrite(MC_BASE + 460*4, 0x0000ffff);
#endif

#ifdef CRC_MODE
  RISCWrite(CRC_BASE + 1*4, 0x3);
#else
  RISCWrite(CRC_BASE + 1*4, 0);
#endif

  // VLD setting
  vVldInit();
#if 1 // david
	RISCWrite(VLD_BASE + 36*4, (prPic->u4Height << 16) | (prPic->u4Width / 16) | 0xFF00);
#else
	RISCWrite(VLD_BASE + 36*4, prPic->u4Width / 16);
#endif
#ifdef DDR3_TEST
#if 1 // 5368
  RISCWrite(MC_BASE + 608*4, u4MbW);
#else
  RISCWrite(VLD_BASE + 50*4, u4MbW);
#endif
#else
#if 1 // 5368
  RISCWrite(MC_BASE + 608*4, prPic->u4Width / 16);
#else
  RISCWrite(VLD_BASE + 50*4, prPic->u4Width / 16);
#endif
#endif

#if 1 // david: for IDE
  RISCWrite(VLD_BASE + 243*4, 0xffffffff);
  RISCWrite(VLD_BASE + 244*4, 0xff000000);
#endif


  INIT_BARREL_SHIFTER();
  
  if (_fg8253Test)
  {
    UINT32 dwResidue = _dwPrVRp & 0x0000000F;
    RV_VLD_SHIFT_BIT(dwResidue << 3);
    RISCWrite(RV_VLD_BASE + 66*4, 0x1); //byte cnt reset
    RISCWrite(RV_VLD_BASE + 66*4, 0x0); //byte cnt reset
  }
  
  RV_VLD_SHIFT_BIT(prPic->u4HdrSkip);
#if 1 // 5368
    u4Reg = _u4ErrConceal | _u4FastParsing | 0x3 | (prPic->fgRV9 ? 4 : 0);
    RISCWrite(RV_VLD_BASE + 33*4, u4Reg);  // RV8 or RV9
    RISCWrite(VLD_TOP_BASE + 21*4, _u4CycleCnt);
    u4Reg = prPic->u4OrgHeight << 16 | prPic->u4OrgWidth;
    RISCWrite(VLD_TOP_BASE + 28*4, u4Reg);
    u4Reg = (prPic->u4Height / 16 - 1) << 16 | (prPic->u4Width / 16 - 1);
    RISCWrite(VLD_TOP_BASE + 26*4, u4Reg);
    u4Reg = ((prPic->u4Width / 16) * (prPic->u4Height / 16));
    RISCWrite(VLD_TOP_BASE + 27*4, u4Reg);
    RISCWrite(RV_VLD_BASE + 35*4, prPic->u4MbaSize << 24 | prPic->u4PctszSize << 22);
#else
  RISCWrite(RV_VLD_BASE + 33*4, _u4CycleCnt | _u4ErrConceal | _u4FastParsing | 0x3 | (prPic->fgRV9 ? 4 : 0));  // RV8 or RV9
  RISCWrite(RV_VLD_BASE + 34*4, (prPic->u4Width / 16 - 1) << 24 | prPic->u4OrgHeight << 12 | prPic->u4OrgWidth);
  RISCWrite(RV_VLD_BASE + 35*4, prPic->u4MbaSize << 24 | prPic->u4PctszSize << 22 | ((prPic->u4Width / 16) * (prPic->u4Height / 16)) << 8 | (prPic->u4Height / 16 - 1));  //  0xbc39816
#endif
  RISCWrite(RV_VLD_BASE + 36*4, prPic->u4Rtype << 26 | prPic->u4Pctsz << 23 | prPic->u4Tr << 10 | prPic->u4DFP << 9 | prPic->u4Oquant << 7 | prPic->u4Pquant << 2 | (UINT32)prPic->ePtype);  // 0x4000044
  RISCWrite(RV_VLD_BASE + 37*4, prPic->u4Mba);
  RISCWrite(RV_VLD_BASE + 61*4, prPic->u4NumSlice);
  for (i = 0; i < prPic->u4NumSlice; i++)
  {
    //RISCWrite(RV_VLD_BASE + 62*4, i);
    //RISCWrite(RV_VLD_BASE + 63*4, prPic->au4SliceSize[i]);
    RISCWrite(RV_VLD_BASE + (68 + i)*4, prPic->au4SliceSize[i]);
  }

#ifdef LOG_FOR_MC_MEASURE
    MC_WRITE32(RW_MC_PARA_BEHAVIOR, RW_MC_PARA_BEHAVIOR_SETTING);
#endif

  _u4VldWindow = RISCRead(RV_VLD_BASE);

  RM_LOG(("To trigger decode...\n"));
  //vDumpMcReg();
//  printf("To trigger decode...\n");
  
  //vDumpReg();
#if 0
{
  UINT32 i;
  for (i = 0; i <= 0xb00; i+=4)
  {
      printf("MC(%d) = 0x%08X\n", i >> 2, RISCRead(MC_BASE + i));
  }
}
#endif

  //printf("RV_VLD 0x0084 = 0x%08X\n", RISCRead(RV_VLD_BASE + 0x0084));

#ifdef VDEC_L2_CACHE
    HalFlushInvalidateDCacheMultipleLine(MV_DIRECT_ADDR, MV_DIRECT_SZ);
    HalFlushInvalidateDCacheMultipleLine(VLD_PRED_OFFSET, VLD_PRED_OFFSET_SZ);
    HalFlushInvalidateDCacheMultipleLine(VFIFO_ADDR, VFIFO_SZ);
#else
  HalFlushInvalidateDCache();
#endif

  // trigger
  RISCWrite(RV_VLD_BASE + 42*4, 0x1);

  // wait for complete
  _fgDecTimeout = FALSE;
  u4Wait = 0;
  while (RISCRead(RV_VLD_BASE + 0xCC) == 0)
  {
    if (++u4Wait > DEC_WAIT_CNT)
    {
      _fgDecTimeout = TRUE;
      break;
    }
  }
  u4Wait = 0;
  while (RISCRead(RV_VLD_BASE + 0xD0) == 0)
  {
    if (++u4Wait > DEC_WAIT_CNT)
    {
      _fgDecTimeout = TRUE;
      break;
    }
  }

#ifdef POWER_SAVING_TEST
    RISCWrite(MISC_BASE + 0xC8, 0xFFFFFFFF);
    RISCWrite(MISC_BASE + 0xCC, 0xFFFFFFFF);
#endif

  if (!_fgDecTimeout)
  {
    UINT32 u4CycleCnt;
#if 1 // 5368
    u4CycleCnt = RISCRead(VLD_TOP_BASE + 40 * 4);
#else
    u4CycleCnt = RISCRead(RV_VLD_BASE + 50 * 4);
#endif
    u4CycleCnt /= ((prPic->u4Width / 16) * (prPic->u4Height / 16));
    if (u4CycleCnt > _u4MaxCycleCnt)
    {
      _u4MaxCycleCnt = u4CycleCnt;
      _u4MaxCycleCntPicNum = n;
    }
  }

#ifdef RPR_RACING
  if (_eRprRacStt == RPR_RAC_SCP1_PENDING)
  {
    u4Wait = 0;
    while ((RISCRead(IMG_RESZ_BASE + IMG_RESZ_DONE * 4) & 1) == 0)
    {
      if (++u4Wait > RPR_RAC_WAIT_CNT)
      {
        _fgDecTimeout = TRUE;
        break;
      }
    }
    if (!_fgDecTimeout)
      _eRprRacStt = RPR_RAC_SCP1_DONE;
  }
#endif

#ifdef LOG_FOR_MC_MEASURE
{
    CHAR ucPicType;

    switch (prPic->ePtype)
    {
    default:
        ucPicType = 'U';
        break;

    case RV_INTRAPIC:
    case RV_FORCED_INTRAPIC:
        ucPicType = 'I';
        break;

    case RV_INTERPIC:
        ucPicType = 'P';
        break;

    case RV_TRUEBPIC:
        ucPicType = 'B';
        break;
    }

    printf("VDEC_perf_measure:seq_name=%s,pic_idx=%d,pic_width=%d,pic_height=%d\n", _pcLoadedBitstream, n, prPic->u4OrgWidth, prPic->u4OrgHeight);
    printf("VDEC_perf_measure2:pic_type=%c,framepic=1,CYCLE_DRAM=%ld\n", ucPicType, MC_READ32(RO_MC_CYCLE_DRAM));
    printf("VDEC_perf_measure3:MC_DLE_NUM=%ld,MC_MBX =%ld,MC_MBY =%ld\n", MC_READ32(RO_MC_DLE_NUM), MC_READ32(RO_MC_MBX), MC_READ32(RO_MC_MBY));
    printf("VDEC_perf_measure4:CYC_SYS=%ld,INTRA_CNT=%ld,Y_BLK_CNT=%ld\n", MC_READ32(RO_MC_CYC_SYS), MC_READ32(RO_MC_INTRA_CNT), MC_READ32(RO_MC_Y_BLK_CNT));
    printf("VDEC_perf_measure5:C_BLK_CNT=%ld,WAIT_CNT=%ld,REQ_CNT=%ld\n", MC_READ32(RO_MC_C_BLK_CNT), MC_READ32(RO_MC_WAIT_CNT), MC_READ32(RO_MC_REQ_CNT));
    printf("VDEC_perf_measure6:PARA4_BEHAVIOR=%d,PARA8_BEHAVIOR=%d\n", ((MC_READ32(RW_MC_PARA_BEHAVIOR)&PARA4_BEHAVIOR)?1:0), ((MC_READ32(RW_MC_PARA_BEHAVIOR)&PARA8_BEHAVIOR)?1:0));
}
#endif

  // save ref_qp_mv0 for the next picture
  if (prPic->ePtype != RV_TRUEBPIC && !_fgDecTimeout)
    _u4RefQpMb0 = RISCRead(PP_BASE + 7 * 4);

  if (_fgDecTimeout)
  { 
//  	printf("Decode timeout!!!\n"); 
  	printf("[%d]Decode timeout!!!\n", n);
  }
  else
  { 
//  	printf("Decode complete!\n");
  	RM_LOG(("Decode complete!\n"));

	if (_fgShowMc)
	{
		// david MC log 
		printf("MC 401: 0x%08X\n", RISCRead(MC_BASE + 401 * 4));
		printf("MC 476: 0x%08X\n", RISCRead(MC_BASE + 476 * 4));
		printf("MC 477: 0x%08X\n", RISCRead(MC_BASE + 477 * 4));
		printf("MC 478: 0x%08X\n", RISCRead(MC_BASE + 478 * 4));
		printf("MC 522: 0x%08X\n", RISCRead(MC_BASE + 522 * 4));
	}
  }
  
  return (!_fgDecTimeout);
}

static void _VdecRvBarrelShifterTest(void)
{
  printf("Vdec RV test for barrel shifter\n\n");
  
  *(UINT32 *)0x00200000 = 0x12345678;
  *(UINT32 *)0x00200004 = 0x33333333;
  *(UINT32 *)0x00200008 = 0x11111111;
  *(UINT32 *)0x0020000c = 0x11111111;
  
  printf("DRAM addr 0: 0x%08X\n", *(UINT32 *)0x200000);
  printf("DRAM addr 4: 0x%08X\n", *(UINT32 *)0x200004);
  printf("DRAM addr 8: 0x%08X\n", *(UINT32 *)0x200008);
  printf("DRAM addr C: 0x%08X\n", *(UINT32 *)0x20000C);

  *(UINT32 *)0xF0030c84 = 0x00000007;  // RV ctrl: RV_dec + Packet format + RV9
  *(UINT32 *)0xF00020b0 = 0x00200000;  // RPAR: read pointer
  *(UINT32 *)0xF0002110 = 0xffffffff;  // RCRW: ???
  *(UINT32 *)0xF00020b4 = 0x00200000;  // DSTA: V FIFO start addr
  *(UINT32 *)0xF00020b8 = 0x00400000;  // DETA: V FIFO end addr
  *(UINT32 *)0xF000208c = 0x00100000;  // RCPR: set initial fetch flag
  
  while (!((*(UINT32 *)0xF00020e8) & 0x00000001));  // wait for FFSR bit 0 raise
  
  *(UINT32 *)0xF000208c = 0x00800000;  // RCPR: set initial filling of barrel shifter flag
  
#if defined(CC_MT5363) || defined(CC_MT5365)
    while((((*(UINT32 *)0xF00020ec & 0xf) * 4) + 
        (((*(UINT32 *)0xF00020ec) >> 24) & 0x3)) !=3);
#else
  while (!((*(UINT32 *)0xF00020ec) & 0x00000003));  // wait for VBAR bit 0, 1 raise
#endif

  printf("Barrel shifter initialize OK!\n");
  printf("REG 0: 0x%08X\n", *(UINT32 *)0xF0030C00);
  printf("REG 4: 0x%08X\n", *(UINT32 *)0xF0030C04);
  printf("REG 0: 0x%08X\n", *(UINT32 *)0xF0030C00);
  printf("REG 12: 0x%08X\n", *(UINT32 *)0xF0030C0C);
}

static UINT32 u4GetField(UINT32 u4Val, int b0, int b1)
{
    int i;
    UINT32 u4Mask = 0;
    
    for (i = b0; i <= b1; i++)
      u4Mask |= (1 << i);
    
    u4Val &= u4Mask;
    u4Val >>= b0;
    
    return u4Val;
}

static BOOL fgGetRvPic(RvPicAttr *prPic)
{
    UINT32 u4Parm;
    int i;
    
    if (_fg8253Test && !_fgFrameRdy)
      return FALSE;

    if (_fgDirectMode)
    {
      // get PRM from IDE
      _DmxIdeGetPrm(FRMINFO_ADDR);
      
      _u4FrmInfoPtr = 0;
    }

    u4Parm = *(UINT32 *)(FRMINFO_ADDR + _u4FrmInfoPtr);
    _u4FrmInfoPtr += 4;
    
    if (u4Parm == 0xFFFFFFFF)  // hit file end
      return FALSE;
    
    prPic->ePtype = (EnumRVPicCodType)u4GetField(u4Parm, 0, 1);
    prPic->u4OrgWidth = u4GetField(u4Parm, 2, 12);
    if (prPic->u4OrgWidth == 0)
    {
        prPic->u4OrgWidth = 2048;
    }
    prPic->u4OrgHeight = u4GetField(u4Parm, 13, 23);
    prPic->u4Width = (prPic->u4OrgWidth + 15) / 16 * 16;
    prPic->u4Height = (prPic->u4OrgHeight + 15) / 16 * 16;
    prPic->u4PctszSize = u4GetField(u4Parm, 24, 25);
    prPic->u4Pctsz = u4GetField(u4Parm, 26, 28);
    prPic->u4Oquant = u4GetField(u4Parm, 29, 30);
    prPic->u4DFP = u4GetField(u4Parm, 31, 31);

    u4Parm = *(UINT32 *)(FRMINFO_ADDR + _u4FrmInfoPtr);
    _u4FrmInfoPtr += 4;

    prPic->u4Tr = u4GetField(u4Parm, 0, 12);
    prPic->u4Mba = u4GetField(u4Parm, 13, 26);
    prPic->u4Rtype = u4GetField(u4Parm, 27, 27);
    prPic->fgRV9 = u4GetField(u4Parm, 28, 28);

    u4Parm = *(UINT32 *)(FRMINFO_ADDR + _u4FrmInfoPtr);
    _u4FrmInfoPtr += 4;

    prPic->u4NumSlice = u4GetField(u4Parm, 0, 7);
    prPic->u4Pquant = u4GetField(u4Parm, 8, 12);
    prPic->u4MbaSize = u4GetField(u4Parm, 13, 16);
    prPic->u4HdrSkip = u4GetField(u4Parm, 17, 24);

    u4Parm = *(UINT32 *)(FRMINFO_ADDR + _u4FrmInfoPtr);
    _u4FrmInfoPtr += 4;

    prPic->u4Iratio = u4Parm;

    prPic->u4BstLength = 0;
    if (_fg8253Test)
    {
      _u4FrmInfoPtr += 4 * prPic->u4NumSlice;
//      vGetRmSlice(&(prPic->u4NumSlice), &(prPic->au4SliceSize[0]), &(prPic->u4BstLength));
    }
    else
    {
      for (i = 0; i < prPic->u4NumSlice; i++)
      {
          u4Parm = *(UINT32 *)(FRMINFO_ADDR + _u4FrmInfoPtr);
          _u4FrmInfoPtr += 4;
          prPic->au4SliceSize[i] = u4Parm;
          prPic->u4BstLength += u4Parm;
      }
    }
    
    return TRUE;
}

static int _iCurEmuItem = 0;
static int _iTotEmuItem = 0;
static char *_aaRmEmuItem[1000];
/*
static char _aaRmEmuItem[][9] =
{
  "RV8_001",
  "RV8_003",
  "RV9_004",
  "RV9_005",
  "RV9_006",
  "RV9_007",
  "RV9_008",
  "RV9_009",
  "RV9_010",
  "RV10_001",
  "RV10_002",
  "RV10_003",
  "RV10_004",
  "RV10_005",
  "RV10_006",
  "RV10_008",
  "RV10_009",
  "RV10_010",
};
*/
static BOOL fgGetNextItem(char **ppcItem)
{
  if (_iCurEmuItem == _iTotEmuItem)
    return FALSE;
    
  *ppcItem = _aaRmEmuItem[_iCurEmuItem++];
  printf("Item to test: %s\n\n", *ppcItem);
  
  return TRUE;
}

static void vLoadExeList(char *szExelist)
{
  char *pcPtr, *pcStart;
  
  _iCurEmuItem = _iTotEmuItem = 0;
    
  UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"%s\" 0x%08X /ny", szExelist, EXELIST_ADDR));
  
  for (pcPtr = pcStart = (char *)EXELIST_ADDR; *pcPtr != ';'; pcPtr++)
  {
    if (*pcPtr == 0x0D && *(pcPtr + 1) == 0x0A)
    {
      _aaRmEmuItem[_iTotEmuItem++] = pcStart;
      *pcPtr = 0;
      pcStart = pcPtr + 2;
      pcPtr++;
    }
    else if (*pcPtr == 0x0A)
    {
      _aaRmEmuItem[_iTotEmuItem++] = pcStart;
      *pcPtr = 0;
      pcStart = pcPtr + 1;
    }
  }
  
//  printf("Execution list loaded.  Total %d items.\n\n", _iTotEmuItem);
  printf("Execution list loaded.  Total %d items.\n\n", _iTotEmuItem);
}

static void vShowExeList(void)
{
  int i;
  
  for (i = 0; i < _iTotEmuItem; i++)
//    printf("%s\n", _aaRmEmuItem[i]);
    printf("%s\n", _aaRmEmuItem[i]);
  
  _iCurEmuItem = 0;
  
//  printf("\n");
  printf("\n");
}

static INT32 _VdecT32(INT32 i4Argc, const CHAR ** szArgv)
{
    if (i4Argc > 1)
    {
        UNUSED(T32_HostExec(E_T32_EXEC, "%s", szArgv[1]));
    }

    return 0;
}

BOOL fgMpsState(void){return TRUE;}
void vMT8222Init(UINT32 u4Addr1, UINT32 u4Addr2){}
void vMT8222State(void){}
void vNormDecState(void){}

static INT32 _VdecRvTest(INT32 i4Argc, const CHAR ** szArgv)
{
  UINT32 u4Case;
  int n;
  int r, rep;
  int ideRetryCnt;
  UINT32 round;
  BOOL fgFirstTime;
  CHAR *pcItem;
  
#if 0  // not exist on 5365
	if (_fgDummyAgent)
	{
	  // test dummy agents, Sling requested
	  RISCWrite(0x20017004, 0x03051111);
	  RISCWrite(0x20017008, 0x03052222);
	  RISCWrite(0x2001700c, 0x03053333);
	  RISCWrite(0x20017010, 0x03054444);
	  RISCWrite(0x20017014, 0x03055555);
	  //enable all dummy agents
	  RISCWrite(0x20017000, 0x0000001f);
  }
  else
  {
    RISCWrite(0x20017000, 0);
  }
#endif

  if (i4Argc < 2)
  {
    printf("vdec.t case {0:Barrel Shifter 1:I decode 2:P decode 3:I decode 4:P decode}\n");
    printf("vdec.t case {100: Test whole file, vdec.t 100 filename n rep}\n");
    printf("vdec.t case {101: Test whole file by execution list}\n");
    printf("vdec.t case {220: Dump register of #n, vdec.t 220 n}\n");
    printf("vdec.t case {221: Set AVC_MV of #n, vdec.t 221 n}\n");
    printf("vdec.t case {222: Set mc_new_bandwidth_mode}\n");
    printf("vdec.t case {223: Print MC register}\n");
    printf("vdec.t case {224: Toggle register dump ON/OFF}\n");
    printf("vdec.t case {225: Set DRAM busy}\n");
    printf("vdec.t case {980: Toggle direct mode ON/OFF}\n");
    printf("vdec.t case {987: Toggle dummy DRAM agent ON/OFF}\n");
    printf("vdec.t case {988: Toggle golden comparison on PC/FPGA}\n");
    printf("vdec.t case {989: Toggle RPR racing mode ON/OFF}\n");
    printf("vdec.t case {990: Toggle bitstream & golden from ICE/IDE}\n");
    printf("vdec.t case {991: Toggle light log ON/OFF}\n");
    printf("vdec.t case {992: Load execution list, vdec.t 992 exe_list_path}\n");
    printf("vdec.t case {993: Toggle error concealment ON/OFF}\n");
    printf("vdec.t case {994: Change number of rounds, vdec.t 994 n}\n");
    printf("vdec.t case {995: Toggle golden comparison ON/OFF, vdec.t 995 n k}\n");
    printf("vdec.t case {996: Change number of frames to test, vdec.t 996 n}\n");
    printf("vdec.t case {997: Toggle stopping immediately ON/OFF}\n");
    printf("vdec.t case {998: Toggle showing difference ON/OFF}\n");
    printf("vdec.t case {999: Show all DRAM buffer addresses}\n");
    return -1;
  }
  
//  printf("Vdec RV test begin\n\n");
  printf("Vdec RV test begin\n\n");

#ifdef DDR3_TEST
  printf("DDR3 on\n");
#else
  printf("DDR3 off\n");
#endif

  round = 0;
  u4Case = StrToInt(szArgv[1]);
  
Main_Switch:
  switch (u4Case)
  {
  case 0:
    _VdecRvBarrelShifterTest();
    break;
  case 201:  //clock setting
  {
    UINT32 MC_CLK = VDEC_MC_MC_CLK;
    UINT32 SYS_CLK = VDEC_MC_SYS_CLK;

    if (i4Argc == 4)
    {
      MC_CLK = StrToInt(szArgv[2]);
      SYS_CLK = StrToInt(szArgv[3]);
    }
    RISCWrite(CKGEN_BASE + VDEC_CLK_SEL_MC, VDEC_MC_MC_CLK);
    RISCWrite(CKGEN_BASE + VDEC_CLK_SEL_SYS, VDEC_MC_SYS_CLK);
    printf("Set vdec clock MC_CLK: %d, SYS_CLK: %d\n\n", MC_CLK, SYS_CLK);
  }
    break;
  case 999:
    printf("VFIFO_ADDR   = 0x%08X\n", VFIFO_ADDR);
    printf("PPY_OUT_BUF1 = 0x%08X\n", PPY_OUT_BUF1);
    printf("PPC_OUT_BUF1 = 0x%08X\n", PPC_OUT_BUF1);
    printf("PPY_OUT_BUF2 = 0x%08X\n", PPY_OUT_BUF2);
    printf("PPC_OUT_BUF2 = 0x%08X\n", PPC_OUT_BUF2);
    printf("PPY_OUT_BUF3 = 0x%08X\n", PPY_OUT_BUF3);
    printf("PPC_OUT_BUF3 = 0x%08X\n", PPC_OUT_BUF3);
    printf("PPY_OUT_BUFR = 0x%08X\n", PPY_OUT_BUFR);
    printf("PPC_OUT_BUFR = 0x%08X\n", PPC_OUT_BUFR);
    printf("GOLD_ADDR    = 0x%08X\n", GOLD_ADDR);
    printf("FRMINFO_ADDR = 0x%08X\n", FRMINFO_ADDR);
    printf("TMP_ADDR     = 0x%08X\n", TMP_ADDR);
    printf("Show difference:  %s\n", _fgShowDiff ? "ON" : "OFF");
    printf("Stop immediately: %s\n", _fgStopImmed ? "ON" : "OFF");
    printf("Number of frames to test: %d\n", _u4NumFrmToTest);
    if (_fgGoldenCompare)
      printf("Golden comparison: ON\n");
    else if (_u4BeginCompare != 0xFFFFFFFF)
    {
      if (_u4BeginDecode == 0)
        printf("Golden comparison: OFF but ON @ #%d\n", _u4BeginCompare);
      else
        printf("Golden comparison: OFF but ON @ #%d with decoding from #%d\n", _u4BeginCompare, _u4BeginDecode);
    }
    else
      printf("Golden comparison: OFF\n");
    printf("Number of rounds: %d\n", _u4Round);
    printf("Error concealment: %s\n", _u4ErrConceal ? "ON" : "OFF");
    printf("Light log: %s\n", _fgLightLog ? "ON" : "OFF");
    printf("File from %s\n", _fgFileFromIDE ? "IDE" : "ICE");
    printf("RPR racing mode: %s\n", _fgRprRacing ? "ON" : "OFF");
    printf("Golden comparison on %s\n", _fgCmpOnPC ? "PC" : "FPGA");
    printf("Dummy DRAM agent: %s\n\n", _fgDummyAgent ? "ON" : "OFF");
    printf("Direct mode: %s\n\n", _fgDirectMode ? "ON" : "OFF");
    break;
  
  case 980:
    _fgDirectMode = !_fgDirectMode;
    printf("Toggle direct mode %s\n\n", _fgDirectMode ? "ON" : "OFF");
    break;
  
  case 981:
  case 982:
C981_982:
  {
    int iDelay;
    UINT32 _dwPicRdIdx;
    UINT32 _dwPicWrIdx;
    
    if (u4Case == 981)
    {
      _fgScan982 = FALSE;
      if (i4Argc == 3)
        pcItem = (char *)(szArgv[2]);
      else
        pcItem = "R:\\RM_Bitstream\\WCP\\RV30\\RV8_001.rmvb";
    }
    else
    {
      _fgScan982 = TRUE;
      if (!fgGetNextItem(&pcItem))
        break;
    }
    
    _fgFileFromIDE = TRUE;
    _DmxIdeReset();
    
    _fg8253Test = TRUE;
    _fgPlayEnd = FALSE;

    _eRprRacStt = RPR_RAC_NOTHING;
    
    _u4SumTotal = 0;
    _u4SumErrCnt = 0;
    
    _u4FrmInfoPtr = 0;

#ifdef MT8222_ON_MT5381
    _DmxIdeSendProp(pcItem, 0, 0, TRUE);
#else
    //vLoadFrmInfo("wcp\\rv30\\rv8_001");
#endif
    
    rPic.fgRV9 = FALSE;
    rPic.u4Oquant = 0;
    rPic.u4BstLength = 0;
    _u4FwdBufY = _u4BwdBufY = 0;
    n = 0;

AGAIN:
    vMT8222Init(TMP_ADDR, VFIFO_ADDR);
    while (_fg8253Test || _dwPicRdIdx != _dwPicWrIdx)
    {
      extern UINT32 _dwIPBMode;
//      if (!_fg8253Test && dwPicIdxNs() <= 2/* && _dwIPBMode == 2*/)  // tmp solution for FF to end or stop during FF
//        break;
//      if (_fgMpvTest)
//        fgIsMpvPicEmpty();
      
      fgMpsState();
      vMT8222State();
      
      // below is to emulate vMpvState()
#ifdef MT8222_ON_MT5381
//      if ((_u4MpvStateCnt++ % _u4MpvLoop) == 0)
//        vMpvState();
#else
      if (rPic.u4BstLength == 0)
        fgGetRvPic(&rPic);
      if (rPic.u4BstLength > 0 && rPic.u4BstLength <= dwFifoFullness(VFIFO_SZ, _dwPrVRp, _dwPrVWp))
      {
        RM_LOG(("Test %s frame #%d\n", "wcp\\rv30\\rv8_001", n));
//        printf("Test %s frame #%d\n", "wcp\\rv30\\rv8_001", n);
        
        ASSERT(fgTestRvDec(&rPic, TRUE, n));
        
        _dwPrVRp += _u4FrameLen;
        if (_dwPrVRp >= (UINT32)_pbVFifo + VFIFO_SZ)
        {
          RM_LOG(("VFIFO read pointer warp 0x%08X => 0x%08X\n", _dwPrVRp, _dwPrVRp - VFIFO_SZ));
//          printf("VFIFO read pointer warp 0x%08X => 0x%08X\n", _dwPrVRp, _dwPrVRp - VFIFO_SZ);
          _dwPrVRp -= VFIFO_SZ;
          for (iDelay = 0; iDelay < 10000000; iDelay++);
        }
      
        n++;
        rPic.u4BstLength = 0;
        _fgFrameRdy = FALSE;
      }
#endif
      
      for (iDelay = 0; iDelay < 1000; iDelay++);
    }
    if (_fg8253Test)
      goto AGAIN;
    
    _fg8253Test = FALSE;

    if (u4Case == 982)
      goto C981_982;
    else
      break;
  }
    
  case 987:
    _fgDummyAgent = !_fgDummyAgent;
    printf("Toggle dummy DRAM agent %s\n\n", _fgDummyAgent ? "ON" : "OFF");
    break;
    
  case 988:
    _fgCmpOnPC = !_fgCmpOnPC;
    printf("Toggle golden comparison on %s\n\n", _fgCmpOnPC ? "PC" : "FPGA");
    break;
    
  case 989:
    _fgRprRacing = !_fgRprRacing;
    printf("Toggle RPR racing mode %s\n\n", _fgRprRacing ? "ON" : "OFF");
    break;
  
  case 998:
    _fgShowDiff = !_fgShowDiff;
    printf("Toggle showing difference %s\n\n", _fgShowDiff ? "ON" : "OFF");
    break;
    
  case 997:
    _fgStopImmed = !_fgStopImmed;
    printf("Toggle stopping immediately %s\n\n", _fgStopImmed ? "ON" : "OFF");
    break;
  
  case 996:
    if (i4Argc == 3)
    {
      _u4NumFrmToTest = StrToInt(szArgv[2]);
      if (_u4NumFrmToTest == 0)
        _u4NumFrmToTest = 0xFFFFFFFF;
    }
    printf("Number of frames to test is %d\n\n", _u4NumFrmToTest);
    break;

  case 220:
	if (i4Argc >= 3)
	{
		_u4DumpRegFrameNo = StrToInt(szArgv[2]);
		if (_u4DumpRegFrameNo == 0)
			_u4DumpRegFrameNo = 0xFFFFFFFF;
	}
	printf("Dump register for frame #%d\n\n", _u4DumpRegFrameNo);
	break;

  case 221:
	_fgSetAvcMv134 = !_fgSetAvcMv134;
	if (_fgSetAvcMv134 && i4Argc >= 3)
	{
		_u4SetAvcMv134OfFrameNo = StrToInt(szArgv[2]);
		if (_u4SetAvcMv134OfFrameNo == 0)
			_u4SetAvcMv134OfFrameNo = 0xFFFFFFFF;
	}

	if (_fgSetAvcMv134)
		printf("Set AVC_MV 134 for frame #%d\n\n", _u4SetAvcMv134OfFrameNo);
	else
		printf("Set AVC_MV 134 OFF\n\n");
	break;

  case 222:
	_fgSetMc474 = !_fgSetMc474;
	if (_fgSetMc474)
		printf("mc_new_bandwidth_mode OFF\n");
	else
		printf("mc_new_bandwidth_mode ON\n");
	break;
	
  case 223:
	_fgShowMc = !_fgShowMc;
	if (_fgShowMc)
		printf("Show MC register\n");
	else
		printf("Not show MC register\n");
	break;

  case 224:
	_fgDumpReg = !_fgDumpReg;
	if (_fgDumpReg)
		printf("Dump register\n");
	else
		printf("Do not dump register\n");
	break;

  case 225:
    _VDEC_RvSetDramBusy();
    printf("Set DRAM busy\n");
	break;
	
  case 995:
    _fgGoldenCompare = !_fgGoldenCompare;
    _u4BeginCompare = 0xFFFFFFFF;
    _u4BeginDecode = 0;
    if (i4Argc >= 3 && !_fgGoldenCompare)
    {
      _u4BeginCompare = StrToInt(szArgv[2]);
      if (i4Argc == 4)
      {
        _u4BeginDecode = StrToInt(szArgv[3]);
        printf("Toggle golden comparison OFF, but ON @ #%d with decoding from #%d\n\n", _u4BeginCompare, _u4BeginDecode);
      }
      else
        printf("Toggle golden comparison OFF, but ON @ #%d\n\n", _u4BeginCompare);
    }
    else
    {
      printf("Toggle golden comparison %s\n\n", _fgGoldenCompare ? "ON" : "OFF");
    }
    break;
  
  case 994:
    if (i4Argc == 3)
    {
      _u4Round = StrToInt(szArgv[2]);
      if (_u4Round == 0)
        _u4Round = 1;
    }
    printf("Number of rounds is %d\n\n", _u4Round);
    break;
    
  case 993:
    if (_u4ErrConceal != 0)
      _u4ErrConceal = 0;
    else
      _u4ErrConceal = (2 << 24) | (1 << 27);
    printf("Toggle error concealment %s\n\n", _u4ErrConceal ? "ON" : "OFF");
    break;
  
  case 992:
    if (i4Argc == 3)
      vLoadExeList((char *)(szArgv[2]));
      
    vShowExeList();
    break;
  
  case 991:
    _fgLightLog = !_fgLightLog;
    printf("Toggle light log %s\n\n", _fgLightLog ? "ON" : "OFF");
    break;
  
  case 990:
    _fgFileFromIDE = !_fgFileFromIDE;
    printf("Toggle bitstream & golden from %s\n\n", _fgFileFromIDE ? "IDE" : "ICE");
    if (_fgFileFromIDE)
      _DmxIdeReset();
    break;
    
  case 100:
    pcItem = (char *)(szArgv[2]);
    _u4MaxCycleCnt = 0;
    _u4MaxCycleCntPicNum = 0xFFFFFFFF;
  case 101:
    if (u4Case == 101)
    {
      if (!fgGetNextItem(&pcItem))
      {
        u4Case = 9999;
        _iCurEmuItem = 0;
        break;
      }
      _u4MaxCycleCnt = 0;
      _u4MaxCycleCntPicNum = 0xFFFFFFFF;
      round = 0;
    }

#ifdef LOG_FOR_MC_MEASURE
    _pcLoadedBitstream = pcItem;
#endif

    if (i4Argc == 5)
    {
      r = StrToInt(szArgv[3]);
      rep = StrToInt(szArgv[4]);
    }
    else
    {
      r = 0xFFFFFFFF;
    }
#if 0
    // reg ISR      
    if (x_reg_isr(RVVLD_VECTOR, _RVVLDIsr, &pfnOldIsr) != OSR_OK)
    {
        ASSERT(0);
    }
#endif    
    _eRprRacStt = RPR_RAC_NOTHING;
    
    _u4SumTotal = 0;
    _u4SumErrCnt = 0;
    
    _u4FrmInfoPtr = 0;
    if (!_fgDirectMode)
      vLoadFrmInfo((char *)pcItem);

#ifdef CRC_MODE
    if (_u4BeginCompare != 0xFFFFFFFF)
        _u4CRCPtr = 36 * _u4BeginCompare;
    else
        _u4CRCPtr = 0;
    vLoadCRC((char *)pcItem);
#endif

    rPic.fgRV9 = FALSE;
    rPic.u4Oquant = 0;
    
    n = 0;
    fgGetRvPic(&rPic);  // get the first picture
    fgFirstTime = TRUE;
    _u4FwdBufY = _u4BwdBufY = 0;
    while (1)
    {
      if (_u4BeginDecode == 0 || (n >= _u4BeginDecode && (rPic.ePtype == RV_INTRAPIC || rPic.ePtype == RV_FORCED_INTRAPIC)))
      {
        if (_u4BeginDecode > 0)
          _u4BeginDecode = 0;

        RM_LOG(("Test %s frame #%d\n", (char *)pcItem, n));
//				printf("Test %s frame #%d\n", (char *)pcItem, n);
      
        if (_fgLightLog && n > 0 && (n % LOG_PROGRESS) == 0)
          printf("%s frame #%d in progress...\n", (char *)pcItem, n);
      
        if (fgFirstTime)
          vLoadRvPicBitstream(n, (char *)pcItem);
      }
        
      ideRetryCnt = 0;
      while (_u4BeginDecode == 0)
      {
        if (fgTestRvDec(&rPic, (ideRetryCnt == 0 ? fgFirstTime : FALSE), n))
          break;
        
        if (_fgFileFromIDE)  // if from IDE, retry
        {
          if (ideRetryCnt++ < IDE_RETRY)
          {
            printf("%s #%d possible IDE error, retry decoding...\n", (char *)pcItem, n);
            vLoadRvPicBitstream(n, (char *)pcItem);
          }
          else
          {
            printf("Reach IDE retry count!!!\n");
            break;
          }
        }
        else
        {
          break;
        }
      }
      
      if (n == _u4BeginCompare)
      {
        _fgGoldenCompare = TRUE;
        _u4BeginCompare = 0xFFFFFFFF;
        _u4BeginDecode = 0;
      }
      
      if (_fgGoldenCompare || _fgDecTimeout)
      {
        if (_fgDecTimeout || !fgCompareGold(n, (char *)pcItem, rPic.u4Width, rPic.u4Height, rPic.u4OrgWidth, rPic.u4OrgHeight, fgFirstTime))
        {
          vDumpVldCkSum();
          vDumpAvcMvCkSum();
          vDumpMcCkSum();
          vDumpPpCkSum();
          vDumpReg();

          vShowErrDumpHint(n, (char *)pcItem, rPic.u4Width, rPic.u4Height, rPic.u4BstLength);

          if (!_fgDecTimeout)
            printf("\n%s frame #%d compare failed!!!\n\n", (char *)pcItem, n);

          if (_fgStopImmed)
          {
            if (u4Case == 100)
              printf("%s round %d, %d errors in %d pixels of %d frames\n\n", (char *)pcItem, ++round, _u4SumErrCnt, _u4SumTotal, n + 1);
            else if (u4Case == 101)
              printf("%s, %d errors in %d pixels of %d frames\n\n", (char *)pcItem, _u4SumErrCnt, _u4SumTotal, n + 1);

            round = 0xFFFFFFFF;  // flag to indicate error exiting case
            ASSERT(0);
            break;
          }
        }
#if 1 // david: dump register for frame #_u4DumpRegFrameNo
        else if (n == _u4DumpRegFrameNo)
        {
          RM_LOG(("Compare OK!\n"));
          vDumpVldCkSum();
          vDumpAvcMvCkSum();
          vDumpMcCkSum();
          vDumpPpCkSum();
          vDumpReg();
          vShowErrDumpHint(n, (char *)pcItem, rPic.u4Width, rPic.u4Height, rPic.u4BstLength);
          ASSERT(0);
        }
#endif
        else
        {
          RM_LOG(("Compare OK!\n"));
        }
      }  // end of if (_fgGoldenCompare)

      if (n == r && --rep > 0)  // repeatedly test the rth picture
      {
        fgFirstTime = FALSE;
        continue;
      }
        
      n++;
      fgFirstTime = TRUE;
      
      if (!fgGetRvPic(&rPic))
        break;
        
      if (n == _u4NumFrmToTest)
        break;

#ifdef RPR_RACING
#if 1 // megaa 20080116, for realvideo10_56 #395 case: P B I Psc
      // if next is P or I, look for the next size-changed P in following several pictures
      if (_fgRprRacing &&
          (rPic.ePtype == RV_INTERPIC || rPic.ePtype == RV_INTRAPIC || rPic.ePtype == RV_FORCED_INTRAPIC) &&
          _eRprRacStt == RPR_RAC_NOTHING)
#else
      // if next is P, look for the next size-changed P in following several pictures
      if (_fgRprRacing && rPic.ePtype == RV_INTERPIC && _eRprRacStt == RPR_RAC_NOTHING)
#endif
      {
        UINT32 u4FrmInfoPtrBak = _u4FrmInfoPtr;
        int j;
        
        for (j = 1; j <= RPR_RAC_LOOK_CNT; j++)
        {
          if (!fgGetRvPic(&rTmpPic))
            break;
            
#if 1 // megaa 20080116, for realvideo10_56 #395 case: P B I Psc
          // if encounter an I first, stop searching
          if (rTmpPic.ePtype == RV_INTRAPIC || rTmpPic.ePtype == RV_FORCED_INTRAPIC)
            break;
#else
          // if encounter a size-changed I first, stop searching (to deal with RV8_002 #239 case)
          if (rTmpPic.ePtype == RV_INTRAPIC || rTmpPic.ePtype == RV_FORCED_INTRAPIC)
          {
            if (rTmpPic.u4OrgWidth != rPic.u4OrgWidth || rTmpPic.u4OrgHeight != rPic.u4OrgHeight)
              break;
          }
#endif

          if (rTmpPic.ePtype == RV_INTERPIC)
          {
            if (rTmpPic.u4OrgWidth != rPic.u4OrgWidth || rTmpPic.u4OrgHeight != rPic.u4OrgHeight)
            {
              _eRprRacStt = RPR_RAC_SCP2_DTCED;
              printf("Size-changed P2 detected @ #%d, enter RPR racing mode...\n", n + j);
            }
            break;
          }
        }
        
        _u4FrmInfoPtr = u4FrmInfoPtrBak;
      }
#endif  // RPR_RACING
    }  // end of while (1)
    
    if (round != 0xFFFFFFFF)  // to distinguish error exiting case
    {
      if (u4Case == 100)
      {
        printf("%s round %d, %d errors in %d pixels of %d frames\n", (char *)pcItem, ++round, _u4SumErrCnt, _u4SumTotal, n);
        printf("Max dec cycle per MB = %d @ #%d\n\n", _u4MaxCycleCnt, _u4MaxCycleCntPicNum);
      }
      else if (u4Case == 101)
      {
        printf("%s, %d errors in %d pixels of %d frames\n", (char *)pcItem, _u4SumErrCnt, _u4SumTotal, n);
        printf("Max dec cycle per MB = %d @ #%d\n\n", _u4MaxCycleCnt, _u4MaxCycleCntPicNum);
      }
    }
    
    break;
    
  case 886:
    printf("Enter IDE test mode for RX rate...\n\n");
    _DmxIdeReset();
    _DmxIdeTestRxRate(RMEMU_BASE_ADDR);
    printf("Leave IDE test mode for RX rate!\n\n");
    break;
    
  case 887:
    printf("Enter IDE test mode for TX rate...\n\n");
    _DmxIdeReset();
    _DmxIdeTestTxRate(RMEMU_BASE_ADDR);
    printf("Leave IDE test mode for TX rate!\n\n");
    break;
    
  case 888:
    printf("Enter IDE test mode for CMD receive...\n\n");
    _DmxIdeReset();
    while (1)
    {
      UINT8 u1Msg;
      UINT32 u4InputFileLen;
      
      u1Msg = *(volatile UINT8 *)0xF001f40c;
      if ((u1Msg & 0x80) > 0)
      {
        *(volatile UINT8 *)0xF001f40c = 0;  // ACK
        u1Msg &= 0x7F;
        switch (u1Msg)
        {
        case 0x01:
          printf("Begin to read 4 bytes from PC...\n");
          _DmxIdeRead(GOLD_ADDR, 4);
          printf("Read completed! Read data is 0x%08X\n", *(UINT32 *)GOLD_ADDR);
          break;
          
        case 0x02:
          printf("Begin to read file length from PC...\n");
          _DmxIdeGetNextCmd(&u1Msg);
          u4InputFileLen = u1Msg << 25;
          _DmxIdeGetNextCmd(&u1Msg);
          u4InputFileLen |= (u1Msg << 18);
          _DmxIdeGetNextCmd(&u1Msg);
          u4InputFileLen |= (u1Msg << 11);
          _DmxIdeGetNextCmd(&u1Msg);
          u4InputFileLen |= (u1Msg << 4);
          _DmxIdeGetNextCmd(&u1Msg);
          u4InputFileLen |= (u1Msg & 0x0F);
          printf("File length is %d\n", u4InputFileLen);
          printf("Begin to read file to 0x%08X...\n", GOLD_ADDR);
          _DmxIdeRead(GOLD_ADDR, u4InputFileLen);
          printf("File read done!\n");
          UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"r:/rm_emu/golden/rv8_007/golden_00077.y\" 0x%08X /ny", VFIFO_ADDR));
          {
            int j;
            for (j = 0; j < u4InputFileLen; j++)
              if (*(UINT8 *)(GOLD_ADDR + j) != *(UINT8 *)(VFIFO_ADDR + j))
              {
                printf("Compare failed @ %d!!!\n", j);
                break;
              }
            
            printf("Compare OK!\n");
          }
          break;
          
        case 0x03:
        case 0x04:
        case 0x05:
          printf("Message 0x%02X received\n", u1Msg);
          break;
        
        default:
          printf("Invalid message received\n");
          break;
        }
        
        *(UINT8 *)0xF001f40c = 0;
      }
      else
        u1Msg = 0x7F;
      
      if (u1Msg == 0x05)
        break;
    }
    printf("Leave IDE test mode for CMD receive!\n\n");
    break;
  
  case 889:
    _DmxIdeReset();
    if (((*(UINT8 *)0xF001f410) & 0x80) > 0)
    {
      printf("Error!!! Previous message exist!!!\n");
      break;
    }
    printf("Enter IDE test mode for CMD transmit...\n\n");
    if (i4Argc == 3)
    {
      UINT8 u1Msg;
      
      u1Msg = (UINT8)StrToInt(szArgv[2]);
      u1Msg &= 0x7F;
      printf("Transmitting message: 0x%02X\n", u1Msg);
      *(UINT8 *)0xF001f410 = u1Msg;
      u1Msg |= 0x80;
      *(UINT8 *)0xF001f410 = u1Msg;
      
      // wait for PC gets it and ACK
      while ((*(volatile UINT8 *)0xF001f410 & 0x80) > 0)
        ;
      
      printf("Message ACK!\n");
    }
    printf("Leave IDE test mode for CMD transmit!\n\n");
    break;
      
  case 1:
    // load bitstream into FIFO
    vLoadBitstream(1);

    rPic.fgRV9 = FALSE;
    rPic.ePtype = RV_INTRAPIC;
    rPic.u4OrgWidth = 640;
    rPic.u4OrgHeight = 360;
    rPic.u4Width = 640;
    rPic.u4Height = 368;
    rPic.u4PctszSize = 3;
    rPic.u4Pctsz = 0;
    rPic.u4Pquant = 0x11;
    rPic.u4Oquant = 0;
    rPic.u4DFP = 0;
    rPic.u4Tr = 0;
    rPic.u4MbaSize = 0xB;
    rPic.u4Mba = 0;
    rPic.u4Rtype = 1;
    rPic.u4Iratio = 0;
    rPic.u4HdrSkip = 40;
    rPic.u4NumSlice = 2;
    rPic.au4SliceSize[0] = 284;
    rPic.au4SliceSize[1] = 309;
    if (!fgTestRvDec(&rPic, TRUE, 0))
    {
          vDumpVldCkSum();
          vDumpAvcMvCkSum();
          vDumpMcCkSum();
          vDumpPpCkSum();
          vDumpReg();
    }
    break;
    
  case 2:
    // load bitstream into FIFO
    vLoadBitstream(2);
    
    rPic.fgRV9 = FALSE;
    rPic.ePtype = RV_INTERPIC;
    rPic.u4OrgWidth = 640;
    rPic.u4OrgHeight = 360;
    _u4PrevDispWidth = 640;
    _u4PrevDispHeight = 360;
    rPic.u4Width = 640;
    rPic.u4Height = 368;
    rPic.u4PctszSize = 3;
    rPic.u4Pctsz = 0;
    rPic.u4Pquant = 0x11;
    rPic.u4Oquant = 0;
    rPic.u4DFP = 0;
    rPic.u4Tr = 0x42;
    rPic.u4MbaSize = 0xB;
    rPic.u4Mba = 0;
    rPic.u4Rtype = 0;
    rPic.u4Iratio = 0;
    rPic.u4HdrSkip = 40;
    rPic.u4NumSlice = 2;
    rPic.au4SliceSize[0] = 60;
    rPic.au4SliceSize[1] = 65;
    //fgTestRvDec(&rPic, TRUE, 0);
    if (!fgTestRvDec(&rPic, TRUE, 0))
    {
          vDumpVldCkSum();
          vDumpAvcMvCkSum();
          vDumpMcCkSum();
          vDumpPpCkSum();
          vDumpReg();
    }
    break;
    
  case 3:
    // load bitstream into FIFO
    vLoadBitstream(3);
    
    rPic.fgRV9 = FALSE;
    rPic.ePtype = RV_INTRAPIC;
    rPic.u4OrgWidth = 176;
    rPic.u4OrgHeight = 144;
    rPic.u4Width = 176;
    rPic.u4Height = 144;
    rPic.u4PctszSize = 1;
    rPic.u4Pctsz = 0;
    rPic.u4Pquant = 0x1A;
    rPic.u4Oquant = 0;
    rPic.u4DFP = 0;
    rPic.u4Tr = 0;
    rPic.u4MbaSize = 7;
    rPic.u4Mba = 0;
    rPic.u4Rtype = 1;
    rPic.u4Iratio = 0;
    rPic.u4HdrSkip = 34;
    rPic.u4NumSlice = 1;
    rPic.au4SliceSize[0] = 70;
    fgTestRvDec(&rPic, TRUE, 0);
    break;

  case 4:
    // load bitstream into FIFO
    vLoadBitstream(4);
    
    rPic.fgRV9 = FALSE;
    rPic.ePtype = RV_INTERPIC;
    rPic.u4OrgWidth = 176;
    rPic.u4OrgHeight = 144;
    rPic.u4Width = 176;
    rPic.u4Height = 144;
    rPic.u4PctszSize = 1;
    rPic.u4Pctsz = 0;
    rPic.u4Pquant = 0x1A;
    rPic.u4Oquant = 0;
    rPic.u4DFP = 0;
    rPic.u4Tr = 0x85;
    rPic.u4MbaSize = 7;
    rPic.u4Mba = 0;
    rPic.u4Rtype = 0;
    rPic.u4Iratio = 0;
    rPic.u4HdrSkip = 34;
    rPic.u4NumSlice = 2;
    rPic.au4SliceSize[0] = 593;
    rPic.au4SliceSize[1] = 173;
    fgTestRvDec(&rPic, TRUE, 0);
    break;

  case 5:
    // load bitstream into FIFO
    vLoadBitstream(5);
    
    rPic.fgRV9 = FALSE;
    rPic.ePtype = RV_INTRAPIC;
    rPic.u4OrgWidth = 176;
    rPic.u4OrgHeight = 144;
    rPic.u4Width = 176;
    rPic.u4Height = 144;
    rPic.u4PctszSize = 1;
    rPic.u4Pctsz = 0;
    rPic.u4Pquant = 0x16;
    rPic.u4Oquant = 0;
    rPic.u4DFP = 0;
    rPic.u4Tr = 0xA17;
    rPic.u4MbaSize = 7;
    rPic.u4Mba = 0;
    rPic.u4Rtype = 1;
    rPic.u4Iratio = 0;
    rPic.u4HdrSkip = 0x22;
    rPic.u4NumSlice = 3;
    rPic.au4SliceSize[0] = 598;
    rPic.au4SliceSize[1] = 594;
    rPic.au4SliceSize[2] = 102;
    fgTestRvDec(&rPic, TRUE, 0);
    break;

  case 6:
    // load bitstream into FIFO
    vLoadBitstream(6);
    
    rPic.fgRV9 = FALSE;
    rPic.ePtype = RV_INTERPIC;
    rPic.u4OrgWidth = 176;
    rPic.u4OrgHeight = 144;
    rPic.u4Width = 176;
    rPic.u4Height = 144;
    rPic.u4PctszSize = 1;
    rPic.u4Pctsz = 0;
    rPic.u4Pquant = 0x14;
    rPic.u4Oquant = 0;
    rPic.u4DFP = 0;
    rPic.u4Tr = 0xABE;
    rPic.u4MbaSize = 7;
    rPic.u4Mba = 0;
    rPic.u4Rtype = 0;
    rPic.u4Iratio = 0;
    rPic.u4HdrSkip = 0x22;
    rPic.u4NumSlice = 2;
    rPic.au4SliceSize[0] = 596;
    rPic.au4SliceSize[1] = 559;
    fgTestRvDec(&rPic, TRUE, 0);
    break;

  case 7:
    // load bitstream into FIFO
    vLoadBitstream(7);
    
    rPic.fgRV9 = FALSE;
    rPic.ePtype = RV_INTRAPIC;
    rPic.u4OrgWidth = 240;
    rPic.u4OrgHeight = 180;
    rPic.u4Width = 240;
    rPic.u4Height = 192;
    rPic.u4PctszSize = 1;
    rPic.u4Pctsz = 0;
    rPic.u4Pquant = 0xC;
    rPic.u4Oquant = 0;
    rPic.u4DFP = 0;
    rPic.u4Tr = 0x1;
    rPic.u4MbaSize = 9;
    rPic.u4Mba = 0;
    rPic.u4Rtype = 1;
    rPic.u4Iratio = 0;
    rPic.u4HdrSkip = 0x24;
    rPic.u4NumSlice = 8;
    rPic.au4SliceSize[0] = 1351;
    rPic.au4SliceSize[1] = 1332;
    rPic.au4SliceSize[2] = 1367;
    rPic.au4SliceSize[3] = 1288;
    rPic.au4SliceSize[4] = 1386;
    rPic.au4SliceSize[5] = 1385;
    rPic.au4SliceSize[6] = 1373;
    rPic.au4SliceSize[7] = 651;
    //rPic.au4SliceSize[8] = 1387;
    fgTestRvDec(&rPic, TRUE, 0);
    break;
  }
  
  if (round != 0xFFFFFFFF)
  {
    if (u4Case == 100 && round < _u4Round)
      goto Main_Switch;
  }
  
  if (u4Case == 101)
    goto Main_Switch;

  _iCurEmuItem = 0;
  
  printf("Vdec RV test end\n");
  
  return 0;
}

void _assert(BOOL fgCond)
{
  ASSERT(fgCond);
}

//david end

/******************************************************************************
* Variable        : cli "vdec" command table
******************************************************************************/
CLIMOD_DEBUG_FUNCTIONS(VDEC)

static CLI_EXEC_T _arVdecCmdTbl[] =
{
    {"init",                "i",      _VdecCmdInit,            NULL,    "Vdec init",            CLI_SUPERVISOR},
    {"play",               "p",     _VdecCmdPlay,            NULL,    "Vdec play",            CLI_SUPERVISOR},
    {"stop",              "s",      _VdecCmdStop,          NULL,   "Vdec stop",    CLI_SUPERVISOR},    
    {"setRRmode",      "srr",    _VdecCmdSetRRMode,    NULL,    "Vdec set RR mode",    CLI_SUPERVISOR},
    {"setECMode",       "secm",_VdecCmdECMode,        NULL,    "Vdec set EC mode",    CLI_SUPERVISOR},
    {"setSyncStc",      "ss",    _VdecCmdSyncStc,    NULL,   "Vdec set sync stc",    CLI_SUPERVISOR},
    {"query",              "q",     _VdecCmdQuery,          NULL,   "Vdec query info",    CLI_SUPERVISOR},
    {"rstTimeProfile",    "r",      _VdecCmdRstProfile,  NULL,   "Vdec rst time profile",    CLI_SUPERVISOR},
    {"setDramUsage",   "d",      _VdecCmdDramUsage,  NULL,   "Vdec dram usage",    CLI_SUPERVISOR},    
    {"capture",   "c",      _VdecCmdCapture,  NULL,   "Vdec capture",    CLI_SUPERVISOR},  
    {"setDramBWSimu","ds",   _VdecCmdDramBWSimu,    NULL,    "Vdec set dram bandwidth simulation",    CLI_SUPERVISOR},    
#ifdef CC_EMULATION
    {"sendPattern",    "sp",    _VdecCmdSendPattern,    NULL,    "Vdec send pattern",    CLI_SUPERVISOR},
    {"saveDecOut",    "so",    _VdecCmdSaveDecOut,    NULL,    "Vdec save dec out data",    CLI_SUPERVISOR},
    {"saveSimPat",     "ssp",   _VdecCmdSaveSimPat,    NULL,    "Vdec save sim pat",    CLI_SUPERVISOR},
    {"setTestMode",   "stm",   _VdecCmdSetTestMode,    NULL,    "Vdec set test mode",    CLI_SUPERVISOR},
    {"setRepeatFrm",   "srf",    _VdecCmdSetRepeatFrm,    NULL,    "Vdec set repeat frame",    CLI_SUPERVISOR},
#endif
    {"test", "t", _VdecRvTest, NULL, "Vdec test RV barrel shifter", CLI_SUPERVISOR},
    {"t32", NULL, _VdecT32, NULL, "Run T32 script", CLI_SUPERVISOR},
    CLIMOD_DEBUG_CLIENTRY(VDEC),
    {NULL, NULL, NULL, NULL, NULL, CLI_SUPERVISOR}
};

CLI_MAIN_COMMAND_ITEM(Vdec)
{
    "vdec",
    NULL,
    NULL,
    _arVdecCmdTbl,
    "Vdec command",
    CLI_SUPERVISOR
};

/******************************************************************************
* Variable        : cli "vdec" enums
******************************************************************************/
static ENUM_VDEC_FMT_T _eFmt; // Record the current file format
static UINT32 _u4YRREn = 0, _u4CRREn = 0, _u4YRRRatio = 0, _u4CRRRatio = 0;
static UINT8 _u1DramMoniter = 0;
static HANDLE_T _hVdecCapSema;
static UINT8* pu1CapBuf = NULL;
#ifdef CC_EMULATION
static BOOL _bIsFirstFrm = TRUE;
#endif

typedef enum
{
    VDEC_COLORMODE_AYUV_CLUT2     = 0,
    VDEC_COLORMODE_AYUV_CLUT4     = 1,
    VDEC_COLORMODE_AYUV_CLUT8     = 2,
    VDEC_COLORMODE_UYVY_16        = 3,
    VDEC_COLORMODE_YUYV_16        = 4,
    VDEC_COLORMODE_AYUV_D8888     = 5,
    VDEC_COLORMODE_ARGB_CLUT2     = 6,
    VDEC_COLORMODE_ARGB_CLUT4     = 7,
    VDEC_COLORMODE_ARGB_CLUT8     = 8,
    VDEC_COLORMODE_RGB_D565       = 9,
    VDEC_COLORMODE_ARGB_D1555     = 10,
    VDEC_COLORMODE_ARGB_D4444     = 11,
    VDEC_COLORMODE_ARGB_D8888     = 12,
    VDEC_COLORMODE_YUV_420_BLK    = 13,
    VDEC_COLORMODE_YUV_420_RS     = 14,
    VDEC_COLORMODE_YUV_422_BLK    = 15,
    VDEC_COLORMODE_YUV_422_RS     = 16,
    VDEC_COLORMODE_YUV_444_BLK    = 17,
    VDEC_COLORMODE_YUV_444_RS     = 18
} VDEC_COLORMODE_T;

typedef struct _VID_DEC_THUMBNAIL_INFO_T
{
    UCHAR*                    pc_canvas_buffer;
    VDEC_COLORMODE_T       e_canvas_colormode;
    UINT32                    ui4_canvas_width;
    UINT32                    ui4_canvas_height;

    UINT32                    ui4_thumbnail_x;
    UINT32                    ui4_thumbnail_y;
    UINT32                    ui4_thumbnail_width;
    UINT32                    ui4_thumbnail_height;
}   VDEC_THUMBNAIL_INFO_T;


#ifdef CC_EMULATION
static char _acFileDir[MAX_FILE_NUM][MAX_FILENAME_LEN];
static char _acGoldPrefix[MAX_FILE_NUM][MAX_FILENAME_LEN];
static UINT32 _u4CurFileIdx = 0, _u4CurFrmIdx = 0;
static INT32 _i4SSimPatFrmNo = -1;
static UINT8* _pu1GoldenData = NULL;
#ifdef RR_TEST
static UINT8* _pu1GoldenDataC = NULL;
#endif
static BOOL _bSaveOutput = FALSE, _bCmpEn = FALSE, _bIsPatSending = FALSE;
static HANDLE_T _hVdecSema;
static UINT8 _u1TestSet = 0;    //0:other, 1:Allegro, 2:Allegro2
static char* _pcFileInfo = NULL;
static INT32 _i4LoopCnt = 0;
//emulation, compare switch, repeat frame
static BOOL _bCmpFrmEn[1000];
static BOOL _bRptFrmEn;
static UINT32 _u4RptFrmNo;

UINT8 gu1VdecLogSimPat = 0;

static char _acTestBitstream[MAX_FILE_NUM][MAX_FILENAME_LEN];
static BOOL _afgLoadCompData[MAX_FILE_NUM];
static BOOL _fgLoadFail = FALSE;
static UINT32 _au4StartCompareFrameNum[MAX_FILE_NUM];
static UINT32 _au4PicWidth[MAX_FILE_NUM];
static UINT32 _au4PicHeight[MAX_FILE_NUM];
static UINT32 _u4FileCount = 0, _u4DecFrmIdx = 0;
static UINT32 _u4DecCycles = 0;
static char _acFilePrefix[MAX_FILE_NUM][MAX_FILENAME_LEN];

//wmv
static UINT32 bitstream_sz[MAX_FILE_NUM];
static CHAR* _pcStartCodeFile=NULL;
static char golden_path[MAX_FILE_NUM][1024];
int _vlcreg[250];
int _mcreg[250];
UINT32 m_dwBitsFSz;
//end wmv

char _acFileName[MAX_FILENAME_LEN];
#endif

/******************************************************************************
* Flag(s)        :
******************************************************************************/

/******************************************************************************
* Implementation        : helper functions for commands parsing
******************************************************************************/

static INT32 _VdecCmdInit(INT32 i4Argc, const CHAR ** szArgv)
{
    #ifdef MEMRW_TEST
    HANDLE_T hThread;
    INT32 i4Ret;
    #endif
    
    UNUSED(i4Argc);
    UNUSED(szArgv);

    #ifdef CC_EMULATION
        if(_pcFileInfo == NULL)
        {
            _pcFileInfo = (char*)BSP_AllocAlignedDmaMemory(FILE_INFO_SIZE, 1024);
        }
        if(_pu1GoldenData == NULL)
        {
            _pu1GoldenData = (UINT8*)BSP_AllocAlignedDmaMemory(GOLDEN_SIZE, 1024);
            #ifdef RR_TEST
            _pu1GoldenDataC = (UINT8*)BSP_AllocAlignedDmaMemory(GOLDEN_SIZE, 1024);
            #endif
        }
        if(_pcStartCodeFile==NULL)
        {
            _pcStartCodeFile = (char*)BSP_AllocAlignedDmaMemory(FILE_INFO_SIZE, 1024);
        }
        #ifdef IDE_READ_SUPPORT
        _DmxIdeReset();
        #endif
        VERIFY(x_sema_create(&_hVdecSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
    #endif

    VERIFY(x_sema_create(&_hVdecCapSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);

    VDEC_Init();

    #ifdef MEMRW_TEST
        #if MEMORY_RW_TEST
        // Create Task
        i4Ret = x_thread_create(&hThread,
                                "memrw_thread",
                                MLVDO_STACK_SIZE,
                                MLVDO_THREAD_PRIORITY,
                                memrw,
                                0,
                                NULL);

        VERIFY(i4Ret == OSR_OK);
        #endif
    #endif
    
    return 0;
}


static INT32 _VdecCmdPlay(INT32 i4Argc, const CHAR ** szArgv)
{
    ASSERT(szArgv != NULL);
    if (i4Argc > 2)
    {
        _eFmt = (ENUM_VDEC_FMT_T)StrToInt(szArgv[2]);   
        if(_eFmt >= VDEC_FMT_MAX)
        {
            printf("vdec.p esid {0} fmt {0:MPV, 1:MPEG4, 2:H.264, 3:WMV, 4:H.264Verification}");
            return -1;
        }
    }
    else
    {
        printf("vdec.p esid {0} fmt {0:MPV, 1:MPEG4, 2:H.264, 3:WMV, 4:H.264Verification}");
        return 0;
    }

    #ifdef CC_EMULATION
        switch(_eFmt)
        {
            case VDEC_FMT_H264: case VDEC_FMT_H264VER:
                VDEC_RegEventNotifyFunc(_VdecNotify);
                break;
            case VDEC_FMT_MP4:
                VDEC_RegEventNotifyFunc(_VdecMPEG4Notify);
                break;
            case VDEC_FMT_WMV:
                VDEC_RegEventNotifyFunc(_VdecWMVNotify);
                break;
            default: //VDEC_FMT_MPV
                break;
        }
    #endif
    
    VDEC_Play(0, _eFmt);
    return 0;
}

static INT32 _VdecCmdStop(INT32 i4Argc, const CHAR ** szArgv)
{
    ASSERT(szArgv != NULL);
    if (i4Argc > 1)
    {
        VDEC_Stop(StrToInt(szArgv[1]));
    }
    else
    {
        printf("vdec.s esid {0}");
    }
    return 0;
}

static INT32 _VdecCmdSetRRMode(INT32 i4Argc, const CHAR ** szArgv)
{
    UINT32 u4Param1=0, u4Param2=0, u4Param3=0;
    
    ASSERT(szArgv != NULL);    
    if(i4Argc < 4)
    {
        printf("vdec.srr [enable 1, enable without wrapper 2][Y:0/C:1/MaxHband:2][Ratio (1/2):0, (3/4):1, bypass:2]\n");
        if(i4Argc > 2)
        {
            u4Param2 = StrToInt(szArgv[2]);
        }
        VDEC_GetParam(ES0, RRMODE, &u4Param1, &u4Param2, &u4Param3);
        printf("vdec.srr %d %d %d\n", u4Param1, u4Param2, u4Param3);
        return 0;
    }
    VDEC_SetParam(ES0, RRMODE, StrToInt(szArgv[1]), StrToInt(szArgv[2]), StrToInt(szArgv[3]));

    if(StrToInt(szArgv[2])==0)            //Y
    {
        _u4YRREn = StrToInt(szArgv[1]);
        if(_u4YRREn > 0)
        {
            _u4YRRRatio = StrToInt(szArgv[3]);
        }
    }
    else if(StrToInt(szArgv[2])==1)        //C
    {
        _u4CRREn = StrToInt(szArgv[1]);
        if(_u4CRREn > 0)
        {
            _u4CRRRatio = StrToInt(szArgv[3]);
        }        
    }

    UNUSED(_u4YRRRatio);
    UNUSED(_u4CRRRatio);
    return 0;
}


//u4Param1: Enable error re-concealment, u4Param2: Mode, u4Param3: Dedicate error
static INT32 _VdecCmdECMode(INT32 i4Argc, const CHAR ** szArgv)
{
    UINT32 u4Param1=0, u4Param2=0, u4Param3=0, u4Param4=0, u4Param5=0, u4Param6=0;
    
    ASSERT(szArgv != NULL);
    if(i4Argc < 4)
    {
        printf("vdec.secm [ERC Enable][ERC mode][ERC errDed][DedType]\n");
        VDEC_GetParam(ES0, ECC_MODE, &u4Param1, &u4Param2, &u4Param3);
        VDEC_GetParam(ES0, ECC_TYPE, &u4Param4, &u4Param5, &u4Param6);
        printf("vdec.secm %d %d %d 0x%x\n", u4Param1, u4Param2, u4Param3, u4Param4);        
        return 0;
    }

    VDEC_SetParam(ES0, ECC_MODE, StrToInt(szArgv[1]), StrToInt(szArgv[2]), StrToInt(szArgv[3]));
    VDEC_SetParam(ES0, ECC_TYPE, StrToInt(szArgv[4]), 0, 0);
    return 0;
}


static INT32 _VdecCmdSyncStc (INT32 i4Argc, const CHAR ** szArgv)
{
    UCHAR ucEsId;
    UCHAR ucMode;
    UCHAR ucStcId;

    if ((i4Argc < 4) || (szArgv == NULL) || (szArgv[1] == NULL) || (szArgv[2] == NULL) || (szArgv[3] == NULL))
    {
        printf("vdec.ss [ucEsId] [ucMode, 0:none, 1:slave, 2:master] [ucStcId]\n");
        return 0;
    }

    ucEsId = (UCHAR)StrToInt(szArgv[1]);
    ucMode = (UCHAR)StrToInt(szArgv[2]); 
    ucStcId = (UCHAR)StrToInt(szArgv[3]);
    
    VDEC_SyncStc(ucEsId, ucMode, ucStcId);

    return 0;
}


static INT32 _VdecCmdQuery (INT32 i4Argc, const CHAR ** szArgv)
{
    UINT64 au8DramUsage[5], u4DramClockCycle = 1;
    INT32 idx;
    VDEC_HDR_INFO_T rHdrInfo;
    
    VDEC_QueryInfo(ES0, &rHdrInfo);

    if(!rHdrInfo.fgHdrInvalid)
    {
        printf("Video decoder: ");
        switch(rHdrInfo.eCodecType)
        {
            case VDEC_FMT_H264:
            case VDEC_FMT_H264VER:
                printf("H.264, ");
                if(rHdrInfo.u4Profile == 66)
                {
                    printf("BaseLine ");
                }
                else if(rHdrInfo.u4Profile == 77)
                {
                    printf("Main ");
                }
                else if(rHdrInfo.u4Profile == 88)
                {
                    printf("Extended ");
                }
                else if(rHdrInfo.u4Profile == 100)
                {
                    printf("High ");
                }
                printf("Profile, Level %d.%d, ", rHdrInfo.u4Level/10, rHdrInfo.u4Level%10);
                if(rHdrInfo.u4IsCABAC > 0)
                {
                    printf("CABAC\n");
                }
                else
                {
                    printf("CAVLC\n");
                }                    
                break;
            case VDEC_FMT_WMV:
                printf("WMV\n");
                break;
            case VDEC_FMT_MPV:
                printf("MPEG2\n");
                break;
            case VDEC_FMT_MP4:
                printf("MPEG4\n");
                break;
            default:
                printf("UnKnown\n");
                break;
        }

        printf("State: ");
        switch(rHdrInfo.u1CurState)
        {
            case VDEC_ST_STOP:
                printf("Stop\n");
                break;
            case VDEC_ST_PLAY:
            case VDEC_ST_PLAY_I_FRAME:
                printf("Play\n");
                break;
            case VDEC_ST_IDLE:
                printf("Idle\n");
                break;
            default:
                printf("UnKnown\n");
                break;
        }
        
        printf("Format: %dx%d", rHdrInfo.u2Width, rHdrInfo.u2Height);
        if(rHdrInfo.fgProgressiveSeq)
        {
            printf("p ");
        }
        else
        {
            printf("i ");
        }
        printf("%d.%dfps\n", (rHdrInfo.u2FrmRate/100), (rHdrInfo.u2FrmRate%100));

        //es fifo related
        printf("EsFifoCnt: (Cur,Max) = (%d, %d)\n", rHdrInfo.u4CurEsCnt, rHdrInfo.u4MaxEsCnt);
        //fbm related
        printf("FbNum: %d\n", rHdrInfo.ucFbNum);
        
        rHdrInfo.au4DecTime[0][3] = (rHdrInfo.au4DecTime[0][3] == 0)?1:rHdrInfo.au4DecTime[0][3];
        rHdrInfo.au4DecTime[1][3] = (rHdrInfo.au4DecTime[1][3] == 0)?1:rHdrInfo.au4DecTime[1][3];
        rHdrInfo.au4DecTime[2][3] = (rHdrInfo.au4DecTime[2][3] == 0)?1:rHdrInfo.au4DecTime[2][3];
        //u4DramClockCycle = (BSP_GetDomainClock(CAL_SRC_DMPLL) >> 1) ; //dram cycle /sec
        if(_u1DramMoniter == 0)
        {
            for(idx = 2; idx >=0; idx--)
            {
                au8DramUsage[idx] = 0;
            }
            printf("Time Profile:\n\tPicType(MinHwDec, AvgHwDec, MaxHwDec, SkipFrm/TotalFrm, MaxSwDec, MaxDramCycle(-))");
        }
        else if(_u1DramMoniter == 1) //total percentage
        {
            #ifdef VDEC_TIME_PROFILE
            for(idx = 2; idx >=0; idx--)
            {
                if((UINT64)rHdrInfo.au4DecTime[idx][0] > 0)
                {
                    au8DramUsage[idx] = (((UINT64)rHdrInfo.au4DecTime[idx][8] * 100000) / (UINT64)rHdrInfo.au4DecTime[idx][0])/(UINT64)u4DramClockCycle;
                }
                else
                {
                    au8DramUsage[idx] = 0;
                }
            }
            #else
            UNUSED(u4DramClockCycle);
            #endif
            
            printf("Time Profile:\n\tPicType(MinHwDec, AvgHwDec, MaxHwDec, SkipFrm/TotalFrm, MaxSwDec, MaxDramCycle(Total %))");
        }
        else if(_u1DramMoniter == 2) //MPEG Mbyte
        {
            #ifdef VDEC_TIME_PROFILE
            for(idx = 2; idx >=0; idx--)
            {
                au8DramUsage[idx] = (((UINT64)rHdrInfo.au4DecTime[idx][8] * 16) * (UINT64)rHdrInfo.u2FrmRate)/100;
            }
            #endif
            printf("Time Profile:\n\tPicType(MinHwDec, AvgHwDec, MaxHwDec, SkipFrm/TotalFrm, MaxSwDec, MaxDramCycle(Mpeg Byte))");
        }
        printf("\n\tI(0.%03us, 0.%03us/%d, 0.%03us/%d, %d/%d, 0.%06us, %d)",
            rHdrInfo.au4DecTime[2][1], rHdrInfo.au4DecTime[2][2]/rHdrInfo.au4DecTime[2][3], (rHdrInfo.au4DecTime[2][6]/rHdrInfo.au4DecTime[2][3])*10000,
            rHdrInfo.au4DecTime[2][0], rHdrInfo.au4DecTime[2][7], 
            rHdrInfo.au4DecTime[2][4], rHdrInfo.au4DecTime[2][3], rHdrInfo.au4DecTime[2][5],
            au8DramUsage[2]);
        printf("\n\tP(0.%03us, 0.%03us/%d, 0.%03us/%d, %d/%d, 0.%06us, %d)",
            rHdrInfo.au4DecTime[0][1], rHdrInfo.au4DecTime[0][2]/rHdrInfo.au4DecTime[0][3], (rHdrInfo.au4DecTime[0][6]/rHdrInfo.au4DecTime[0][3])*10000,
            rHdrInfo.au4DecTime[0][0], rHdrInfo.au4DecTime[0][7], 
            rHdrInfo.au4DecTime[0][4], rHdrInfo.au4DecTime[0][3], rHdrInfo.au4DecTime[2][5],
            au8DramUsage[0]);
        printf("\n\tB(0.%03us, 0.%03us/%d, 0.%03us/%d, %d/%d, 0.%06us, %d)\n", 
            rHdrInfo.au4DecTime[1][1], rHdrInfo.au4DecTime[1][2]/rHdrInfo.au4DecTime[1][3], (rHdrInfo.au4DecTime[1][6]/rHdrInfo.au4DecTime[1][3])*10000,
            rHdrInfo.au4DecTime[1][0], rHdrInfo.au4DecTime[1][7], 
            rHdrInfo.au4DecTime[1][4], rHdrInfo.au4DecTime[1][3], rHdrInfo.au4DecTime[1][5],
            au8DramUsage[1]);
    }
    return 0;
}


static INT32 _VdecCmdRstProfile(INT32 i4Argc, const CHAR ** szArgv)
{
    VDEC_SetParam(ES0, TIME_PROFILE, 0, 0, 0);
    return 0;
}


static INT32 _VdecCmdDramUsage(INT32 i4Argc, const CHAR ** szArgv)
{
    if ((i4Argc < 2) || (szArgv == NULL) || (szArgv[1] == NULL))
    {
        printf("vdec.d [DramUsage Mode, 0: none, 1: total, 2: mpeg]\n");
        return 0;
    }
    _u1DramMoniter = StrToInt(szArgv[1]);
    VDEC_SetParam(ES0, TIME_PROFILE2, _u1DramMoniter, 0, 0);
    return 0;
}

void _VdecCapNotify(UINT32 u4CompId, UINT32 u4Cond, UINT32 u4Data1, UINT32 u4Data2)
{
    VDP_SetFreeze(0, 0);
    
    if(u4Cond == (UINT32)VDEC_COND_THUMBNAIL_DONE)
    {
        VERIFY(x_sema_unlock(_hVdecCapSema) == OSR_OK);
    }
}

static INT32  _VdecCmdCapture(INT32 i4Argc, const CHAR ** szArgv)
{
    VDEC_THUMBNAIL_INFO_T rCapInfo;
    VDP_CAPTURE_INTO_T rVdpCapInfo;
    //BOOL bIsOnePass = FALSE;
    HAL_TIME_T rTimeS, rTimeE, rTimeDt; 
    UINT32 u4ColorMode = 0;
    
    if ((i4Argc < 2) || (szArgv == NULL) || (szArgv[1] == NULL))
    {
        printf("vdec.c [Color Mode, 0: AYUV_8888, 1: ARGB_8888, 2: ARGB_4444]\n");
        return 0;
    }

    if(pu1CapBuf == NULL)
    {
        pu1CapBuf = (UINT8*)BSP_AllocAlignedDmaMemory(200*200*4, 1024);
    }
    rCapInfo.pc_canvas_buffer = pu1CapBuf;

    u4ColorMode = StrToInt(szArgv[1]);
    if(u4ColorMode == 0)
    {    
        rCapInfo.e_canvas_colormode = VDEC_COLORMODE_AYUV_D8888;
    }
    else if(u4ColorMode == 1)
    {
        rCapInfo.e_canvas_colormode = VDEC_COLORMODE_ARGB_D8888;
    }
    else if(u4ColorMode == 2)
    {
        rCapInfo.e_canvas_colormode = VDEC_COLORMODE_ARGB_D4444;
    }
    else
    {
        printf("vdec.c [Color Mode, 0: AYUV_8888, 1: ARGB_8888, 2: ARGB_4444]\n");    
        return 0;
    }
    rCapInfo.ui4_canvas_width = 200;
    rCapInfo.ui4_canvas_height = 200;
    rCapInfo.ui4_thumbnail_width = 200;
    rCapInfo.ui4_thumbnail_height = 200;
    rCapInfo.ui4_thumbnail_x = 0;
    rCapInfo.ui4_thumbnail_y = 0;
    
    VDEC_RegEventNotifyFunc(_VdecCapNotify);
    VDP_SetFreeze(0, 1);    //use vdpID 0 arbitrary
    VDP_GetLockFrameBufferAddr(0, &rVdpCapInfo);
    if((rVdpCapInfo.u4AddrY == 0) || (rVdpCapInfo.u4AddrC== 0) || (rVdpCapInfo.u4RRMode > 1))
    {
        LOG(1, "Video Capture Fail!");
        return 0;
    }

    //bIsOnePass = (rCapInfo.e_canvas_colormode == VDEC_COLORMODE_AYUV_D8888) ? TRUE : FALSE;

    HAL_GetTime(&rTimeS);

    //VDEC_GetVideoThumbnail(ES0, 0, bIsOnePass, (void*)&rCapInfo, (void*)&rVdpCapInfo);
    
    VERIFY(x_sema_lock(_hVdecCapSema, X_SEMA_OPTION_WAIT) == OSR_OK);    //wait capture done

    HAL_GetTime(&rTimeE);
    HAL_GetDeltaTime(&rTimeDt, &rTimeS, &rTimeE);

    LOG(1, "Video Capture Frm 0x%x--0x%x, %u.%6u sec\n", (UINT32)rCapInfo.pc_canvas_buffer, 
        (UINT32)rCapInfo.pc_canvas_buffer + (200*200*4), rTimeDt.u4Seconds, rTimeDt.u4Micros);
    
    return 0;
}



#ifdef CC_EMULATION
void _VdecNotify(UINT32 u4Arg1, UINT32 u4Arg2, UINT32 u4Arg3, UINT32 u4Arg4) //for h264
{
    VDEC_DISP_PIC_INFO_T* prDispInfo;
    static UINT8 u1PicStruct = 0;
    static UINT32 u4YAddr = 0;
    BOOL bIsCmpPass = TRUE;

    #ifdef IDE_READ_SUPPORT
    UINT32 u4IDERetryCnt = 0;
    #endif      
    
    prDispInfo = (VDEC_DISP_PIC_INFO_T*)u4Arg2;
    ASSERT(prDispInfo!=NULL);

    if(!_bSaveOutput)    //normal comparing verification
    {
        if(u4Arg1 == (UINT32)VDEC_EVT_DISP_READY)
        {
            #ifndef RR_TEST
            if(((UINT32)prDispInfo->pvYAddr != u4YAddr) && (!_bIsFirstFrm))
            {
                _u4CurFrmIdx++;
            }
            _bIsFirstFrm = FALSE;
            u4YAddr = (UINT32)prDispInfo->pvYAddr;

            if(_bCmpEn && _bCmpFrmEn[_u4CurFrmIdx])
            {  
                //compare decoded golden data: Y
                if((_u4YRREn>0) || (_u4CRREn>0))
                {
                    sprintf(_acFileName, "%s/%s.26l_%05d.bitY", 
                        _acFileDir[_u4CurFileIdx], _acGoldPrefix[_u4CurFileIdx], _u4CurFrmIdx);
                }
                else
                {
                    sprintf(_acFileName, "%s/%s_%d_Y.out", 
                        _acFileDir[_u4CurFileIdx], _acGoldPrefix[_u4CurFileIdx], _u4CurFrmIdx);
                }
                #ifdef IDE_WRITE_SUPPORT
                    bIsCmpPass = _IDEComp(_acFileName, (UINT32)prDispInfo->pvYAddr, prDispInfo->u4YSize);
                #else
                    #ifdef  IDE_READ_SUPPORT
                    printf("//%s_%d_Y.out\n", _acGoldPrefix[_u4CurFileIdx], _u4CurFrmIdx);
                    #endif                
                    if(_LoadFile(_acFileName, (UINT32)_pu1GoldenData))
                    {
                        bIsCmpPass = _VdecVerifyDecData((UINT32*)_pu1GoldenData, *prDispInfo, TRUE);
                    }
                #endif
                   
                //compare decoded golden data: C
                if(prDispInfo->u4ChromaFormatIdc > 0)
                {
                    if((_u4YRREn>0) || (_u4CRREn>0))
                    {
                        sprintf(_acFileName, "%s/%s.26l_%05d.bitC",
                                   _acFileDir[_u4CurFileIdx], _acGoldPrefix[_u4CurFileIdx], _u4CurFrmIdx);
                    }
                    else
                    {
                        sprintf(_acFileName, "%s/%s_%d_CbCr.out",
                                   _acFileDir[_u4CurFileIdx], _acGoldPrefix[_u4CurFileIdx], _u4CurFrmIdx);
                    }

                    #ifdef IDE_WRITE_SUPPORT
                        bIsCmpPass &= _IDEComp(_acFileName, (UINT32)prDispInfo->pvCAddr, prDispInfo->u4CSize);
                    #else
                        #ifdef  IDE_READ_SUPPORT
                        printf("//%s_%d_CbCr.out\n", _acGoldPrefix[_u4CurFileIdx], _u4CurFrmIdx);
                        #endif                    
                        if(_LoadFile(_acFileName, (UINT32)_pu1GoldenData))
                        {
                            bIsCmpPass &= _VdecVerifyDecData((UINT32*)_pu1GoldenData, *prDispInfo, FALSE);
                        }
                     #endif
                }

                #ifdef MEMRW_TEST
                x_thread_delay(500);
                #endif
                
                if(!bIsCmpPass)
                {
                    //_VDEC_DumpReg();
                    #ifdef CC_DRIVER_DEMO
                    printf("//d.save.binary \"%s/Err%d_Y_decDump.out\" 0x%08x--0x%08x\n",
                    _acFileDir[_u4CurFileIdx], _u4CurFrmIdx, 
                    (UINT32)prDispInfo->pvYAddr, (UINT32)prDispInfo->pvYAddr + prDispInfo->u4YSize);
                    UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"%s/Err%d_Y_decDump.out\" 0x%08x--0x%08x", 
                    _acFileDir[_u4CurFileIdx], _u4CurFrmIdx,
                    (UINT32)prDispInfo->pvYAddr, (UINT32)prDispInfo->pvYAddr + prDispInfo->u4YSize));
                    
                    printf("//d.save.binary \"%s/Err%d_CbCr_decDump.out\" 0x%08x--0x%08x\n",
                    _acFileDir[_u4CurFileIdx], _u4CurFrmIdx, 
                    (UINT32)prDispInfo->pvCAddr, (UINT32)prDispInfo->pvCAddr + prDispInfo->u4CSize);
                    UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"%s/Err%d_CbCr_decDump.out\" 0x%08x--0x%08x", 
                    _acFileDir[_u4CurFileIdx], _u4CurFrmIdx,
                    (UINT32)prDispInfo->pvCAddr, (UINT32)prDispInfo->pvCAddr + prDispInfo->u4CSize));
                    #endif
                }
            }
            #else
            _VdecVerifyDecData((UINT32*)_pu1GoldenData, *prDispInfo, TRUE);    
            _VdecVerifyDecData((UINT32*)_pu1GoldenDataC, *prDispInfo, FALSE);    
            #endif
            
            u1PicStruct |= prDispInfo->u1PicStruct;
            if(u1PicStruct == FRAME)            //wait for both top and bottom field
            {
                #ifndef RR_TEST
                //if(!((_bRptFrmEn) && (_u4RptFrmNo==_i4CurFrmIdx)))
                //{
                //    _i4CurFrmIdx++;
                //}
                #endif
                u1PicStruct = 0;        
            }

            if((_bRptFrmEn) && (_u4RptFrmNo==_u4CurFrmIdx))
            {
                VDEC_SetParam(ES0, REPEAT_FRM, 1, 0, 0);
            }
            else
            {
                VDEC_SetParam(ES0, REPEAT_FRM, 0, 0, 0);    //repeat frame disable
            }                    
        }
        else if(u4Arg1 == (UINT32)VDEC_EVT_DEC_BS_END)
        {
            VERIFY(x_sema_unlock(_hVdecSema) == OSR_OK);
        }
    }
    else if (_bSaveOutput)                    //save to PC
    {
        UINT32 u4StartAddr;
        UINT32 u4EndAddr;
        
        //Y
        if(u4Arg3 == 0)
        {
            _Vdec1389B2R(_pu1GoldenData, *prDispInfo, TRUE);
            u4StartAddr = (UINT32)_pu1GoldenData;
            u4EndAddr = (UINT32)_pu1GoldenData + prDispInfo->u4YSize - 1; 
        }
        else
        {
            u4StartAddr = (UINT32)prDispInfo->pvYAddr;        
            if(u4Arg4 == 0)    // 1/2 RR mode
            {
                u4EndAddr = (UINT32)prDispInfo->pvYAddr + (prDispInfo->u4YSize >> 1) - 1;
            }
            else
            {
                u4EndAddr = (UINT32)prDispInfo->pvYAddr + ((prDispInfo->u4YSize * 3) >> 2) - 1;
            }
        }
        #ifdef CC_DRIVER_DEMO
        LOG(6, "//d.save.binary \"%s/%s_%d_decY.out\" 0x%x--0x%x\n",
        _acFileDir[_u4CurFileIdx], _acGoldPrefix[_u4CurFileIdx], _u4CurFrmIdx, 
        u4StartAddr, u4EndAddr);
        UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"%s/%s_%d_decY.out\" 0x%x--0x%x", 
        _acFileDir[_u4CurFileIdx], _acGoldPrefix[_u4CurFileIdx], _u4CurFrmIdx, 
        u4StartAddr, u4EndAddr));
        #endif

        //C
        if(u4Arg3 == 0)
        {
            _Vdec1389B2R(_pu1GoldenData, *prDispInfo, FALSE);
            u4StartAddr = (UINT32)_pu1GoldenData;
            u4EndAddr = (UINT32)_pu1GoldenData + prDispInfo->u4CSize - 1; 
        }
        else
        {
            u4StartAddr = (UINT32)prDispInfo->pvCAddr;        
            if(u4Arg4 == 0)    // 1/2 RR mode
            {
                u4EndAddr = (UINT32)prDispInfo->pvCAddr + (prDispInfo->u4CSize >> 1) - 1;
            }
            else
            {
                u4EndAddr = (UINT32)prDispInfo->pvCAddr + ((prDispInfo->u4CSize * 3) >> 2) - 1;
            }
        }
        #ifdef CC_DRIVER_DEMO
        LOG(6, "//d.save.binary \"%s/%s_%d_decC.out\" 0x%x--0x%x\n",
        _acFileDir[_u4CurFileIdx], _acGoldPrefix[_u4CurFileIdx], _u4CurFrmIdx, 
        u4StartAddr, u4EndAddr);
        UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"%s/%s_%d_decC.out\" 0x%x--0x%x", 
        _acFileDir[_u4CurFileIdx], _acGoldPrefix[_u4CurFileIdx], _u4CurFrmIdx, 
        u4StartAddr, u4EndAddr));
        #endif

        u1PicStruct |= prDispInfo->u1PicStruct;
        if(u1PicStruct == FRAME)    //wait for both top and bottom field
        {
            _u4CurFrmIdx++;
            u1PicStruct = 0;
        }            
    }

    if(_i4SSimPatFrmNo == _u4CurFrmIdx)
    {
        gu1VdecLogSimPat = 1;
        VDEC_SetParam(ES0, LOG_SIMU_PAT, 1, 0, 0);
    }
    else
    {
        gu1VdecLogSimPat = 0;
        VDEC_SetParam(ES0, LOG_SIMU_PAT, 0, 0, 0);
    }
}


void _VdecMPEG4Notify(UINT32 u4Arg1, UINT32 u4Arg2, UINT32 u4Arg3, UINT32 u4Arg4)
{
    char acPath[MAX_FILENAME_LEN];
    VDEC_DISP_PIC_INFO_T* prDispInfo;
    static UINT8 u1PicStruct = 0;
    UINT32* pu4YRetVal, *pu4CbCrRetVal;

    x_memset(acPath, 0, MAX_FILENAME_LEN);
    
    if(u4Arg1 == (UINT32)VDEC_EVT_DISP_READY)
    {

        prDispInfo = (VDEC_DISP_PIC_INFO_T*)u4Arg2;
        ASSERT(prDispInfo!=NULL);
        /*pu4YRetVal = (UINT32*)u4Arg3;
        ASSERT(pu4YRetVal!=NULL);
        pu4CbCrRetVal = (UINT32*)u4Arg4;
        ASSERT(pu4CbCrRetVal!=NULL);

        *pu4YRetVal = 1;
        *pu4CbCrRetVal = 1;*/

        if (_u4CurFrmIdx >= _au4StartCompareFrameNum[_u4CurFileIdx]
            /*&& _u4CurFrmIdx <= _au4FrameNumber[_u4CurFileIdx]*/)
        {
            x_memset(acPath, 0, MAX_FILENAME_LEN);
            sprintf(acPath, "%s_%d_Y.out",
                _acGoldPrefix[_u4CurFileIdx], _u4CurFrmIdx);

            //compare decoded golden data: Y
            /*UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"%s\\%s_%d_Y.out\" 0x%08x /ny", 
                _acFileDir[_u4CurFileIdx], _acGoldPrefix[_u4CurFileIdx], _u4CurFrmIdx, (UINT32)_pu1GoldenData));*/
            printf("d.load.binary %s 0x%08x--0x%08x /ny\n", acPath, (UINT32)_pu1GoldenData, 
                (UINT32)(_pu1GoldenData + GOLDEN_SIZE));

            if (_LoadFile(acPath, (UINT32)_pu1GoldenData))
            {
                //*pu4YRetVal = _VdecVerifyDecData(_pu1GoldenData, *prDispInfo, TRUE);
                _VdecVerifyDecData(_pu1GoldenData, *prDispInfo, TRUE);
            }
            else
            {
                if (!_fgLoadFail)
                {
                      #ifdef CC_DRIVER_DEMO
                    UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"%s%s_%d_Y_Fail_Load.out\" 0x%08x--0x%08x", 
                        "c:\\", _acFilePrefix[_u4CurFileIdx], _u4CurFrmIdx, (UINT32)_pu1GoldenData, (UINT32)(_pu1GoldenData + 1)));
                                #else
                                printf("CC_DRIVER_DEMO !defined, File save fail~");
                                #endif
                    _fgLoadFail = TRUE;
                }
            }
            
            sprintf(acPath, "%s_%d_CbCr.out",
                _acGoldPrefix[_u4CurFileIdx], _u4CurFrmIdx);
            //compare decoded golden data: C
            /*UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"%s\\%s_%d_CbCr.out\" 0x%08x /ny", 
                _acFileDir[_u4CurFileIdx], _acGoldPrefix[_u4CurFileIdx], _u4CurFrmIdx, (UINT32)_pu1GoldenData));*/
            printf("d.load.binary %s to 0x%08x--0x%08x /ny \n", acPath, (UINT32)_pu1GoldenData, 
                (UINT32)(_pu1GoldenData + GOLDEN_SIZE));

            if (_LoadFile(acPath, (UINT32)_pu1GoldenData))
            {
                //*pu4CbCrRetVal = _VdecVerifyDecData(_pu1GoldenData, *prDispInfo, FALSE);
                _VdecVerifyDecData(_pu1GoldenData, *prDispInfo, FALSE);
            }
            else
            {
                if (!_fgLoadFail)
                {
                      #ifdef CC_DRIVER_DEMO
                    UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"%s%s_%d_CbCr_Fail_Load.out\" 0x%08x--0x%08x", 
                                        "c:\\", _acFilePrefix[_u4CurFileIdx], _u4CurFrmIdx, (UINT32)_pu1GoldenData, (UINT32)(_pu1GoldenData + 1)));
                                #else
                                printf("CC_DRIVER_DEMO !defined, File save fail~");
                                #endif
                    _fgLoadFail = TRUE;
                }
            }

        }


        /*UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"%s%s_%d_Y.out\" 0x%08x--0x%08x", 
        "c:\\", _acGoldPrefix[_u4CurFileIdx], _u4CurFrmIdx, (UINT32)prDispInfo->pvYAddr, (UINT32)(prDispInfo->pvYAddr + prDispInfo->u4YSize)));
        printf("prDispInfo->u4YSize = %ld\n", prDispInfo->u4YSize);*/
        
        u1PicStruct |= prDispInfo->u1PicStruct;
        //if(prDispInfo->u1PicStruct == FRAME)
        {
            _u4CurFrmIdx++;
            //_u4DecFrmIdx++;
            u1PicStruct = 0;
        }
    }
    else if(u4Arg1 == (UINT32)VDEC_EVT_DEC_BS_END)
    {
        VERIFY(x_sema_unlock(_hVdecSema) == OSR_OK);
        VDEC_Stop(0);
        VDEC_Init();
        VDEC_Play(0, VDEC_FMT_MP4);
    }
}


void _VdecWMVNotify(UINT32 u4Arg1, UINT32 u4Arg2, UINT32 u4Arg3, UINT32 u4Arg4)
{
    //char path[1024]="D:\\My_data\\project\\VC1\\test_bitstream";
    //UINT32 dwSz, dwRdSz, dwPtr;
    UINT32 dwWidth, dwHeight;
    VDEC_DISP_PIC_INFO_T* prDispInfo;
    UINT8* golden_dataY = 0x7300000;
    UINT8* golden_dataC = golden_dataY + (2084+64)*(1088+64);
    //static UINT8 golden_dataY[2084*1088];
    //static UINT8 golden_dataC[2084*1088/2];
    //static UINT32 dwCount = 0;
    //UINT8* tmp_data;
    UINT8* hw_output;
    UINT32 i=0, u4hw_idx = 0, u4ResisualW = 0, u4ResisualWs = 0, u4FullBlkSz = 0, u4TotlBlkSz = 0;
    //UINT32 u4FullFrmSz = 0, u4ResisualHs = 0, u4CmpSz = 0;
    UINT8* pu1DecData;
    //UINT32 u4CvWByte, u4CvHByte, u4PixIdx;
    //UINT32 u4BlkH, u4PicH, u4X, u4Y;    //Y: 16x32, CbCr: 16x16
    UINT32 u4X, u4Y;
    UINT8* pu1GoldenAddr;
    UINT8* pu1HWAddr;
    INT8 fgY;
    UINT32 CurFrame;

    prDispInfo = (VDEC_DISP_PIC_INFO_T*)(u4Arg2);
    LOG(7,"entering my_save_pic\n");
    CurFrame = prDispInfo->u4CurrentPicNo-1;
    LOG(7,"frame idx : %d %d\n",CurFrame,_u4CurFrmIdx);

    //Printf("src decode resolution : %dx%d\n",rWMVDecInfo.rSeqHdrR.u4PicWidthDec,rWMVDecInfo.rSeqHdrR.u4PicHeightDec);
    //Printf("src cmp resolution : %dx%d\n",rWMVDecInfo.rSeqHdrR.u4PicWidthCmp,rWMVDecInfo.rSeqHdrR.u4PicHeightCmp);
    //Printf("src resolution : %dx%d\n",rWMVDecInfo.rSeqHdrR.u4PicWidthSrc,rWMVDecInfo.rSeqHdrR.u4PicHeightSrc);
    //Printf("rWMVDecInfo.rOther.rPicInfo src resolution : %dx%d\n",rWMVDecInfo.rOther.rPicInfo.dwW,rWMVDecInfo.rOther.rPicInfo.dwH);

    if(u4Arg1 == VDEC_EVT_DEC_BS_END) {
        VERIFY(x_sema_unlock(_hVdecSema) == OSR_OK);
    }

    else if(u4Arg1 == VDEC_EVT_DISP_READY) {
    for(fgY =1 ; fgY >=0 ; fgY--) {
        if(fgY) 
        {
        
        dwWidth = prDispInfo->u4W;
        dwHeight = prDispInfo->u4H;
        hw_output = prDispInfo->pvYAddr;
        
        if(dwWidth%16) 
        {
            dwWidth = ((dwWidth / 16) + 1) * 16; //ming modify@2006/4/11
        }
        if(dwHeight % 32) 
        {
            dwHeight = ((dwHeight / 32) + 1) * 32;

        }

        //dwSz = dwWidth * dwHeight;
        LOG(7,"hw y address : %x\n",hw_output);
        LOG(7,"Y %d size :%d, %dx%d\n", CurFrame/*_u4CurFrmIdx*/, prDispInfo->u4YSize, prDispInfo->u4W, prDispInfo->u4H);
    #ifdef DO_COMPARE_FRAME
        if((CurFrame >= START_COMPARE_FRAME) && (CurFrame<=END_COMPARE_FRAME))
        {
        LOG(0,"d.load.binary \"%s/%s_%d_Y.out\" 0x%08x /ny\n", golden_path[_u4CurFileIdx],_acGoldPrefix[_u4CurFileIdx],CurFrame/*_u4CurFrmIdx*/,golden_dataY);
        //if(!rWMVDecInfo.rOther.u4repeatframe)

            sprintf(_acFileName, "%s/%s_%d_Y.out", golden_path[_u4CurFileIdx],_acGoldPrefix[_u4CurFileIdx],CurFrame/*_u4CurFrmIdx*/);
            _LoadFile(_acFileName, golden_dataY);
            //UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"%s/%s_%d_Y.out\" 0x%08x /ny", golden_path[_u4CurFileIdx],_acGoldPrefix[_u4CurFileIdx],CurFrame/*_u4CurFrmIdx*/,golden_dataY));
        }
    #endif
        
    #ifdef DO_COMPARE_FRAME
        if((CurFrame >= START_COMPARE_FRAME) && (CurFrame<=END_COMPARE_FRAME))
        {
      for(u4Y = 0; u4Y < prDispInfo->u4H_Cmp/*rWMVDecInfo.rOther.rPicInfo.dwH*/; u4Y++)
     {
          for(u4X = 0; u4X < prDispInfo->u4W_Cmp/*rWMVDecInfo.rOther.rPicInfo.dwW*/; u4X++)
         {
            pu1GoldenAddr = (UINT8*)u4CalculatePixelAddress_Y((UINT32)golden_dataY, u4X, u4Y, dwWidth/*720*/, TRUE, 3);
            pu1HWAddr = (UINT8*)u4CalculatePixelAddress_Y((UINT32)hw_output, u4X, u4Y, dwWidth, TRUE, 4);
            if(*pu1GoldenAddr != *pu1HWAddr)
            {
                    LOG(9,"=== %s dif in Y(x%d,y%d), golden(@0x%x=0x%x),hw (@0x%x=0x%x)\n",
                            _acGoldPrefix[_u4CurFileIdx], u4X, u4Y, (UINT32)pu1GoldenAddr, *pu1GoldenAddr, 
                            (UINT32)pu1HWAddr, *pu1HWAddr);
                    //print_checksum(1);
                    Dump_Mem(prDispInfo);
                    //x_mem_free(golden_dataY);
                    //x_mem_free(golden_dataC);
                    return FALSE;
                    }
         }
     }

        }
        #endif

        printf("Cmp Y size %d\n", u4X * u4Y);
    }
    
    else 
    {
        //hw_output = rWMVDecInfo.rOther.rVSyncPrm.dwPtrC;
        //dwWidth = rWMVDecInfo.rOther.rPicInfo.dwW; /// 2;
        //dwHeight = rWMVDecInfo.rOther.rPicInfo.dwH / 2;

        dwWidth = prDispInfo->u4W;
        dwHeight = prDispInfo->u4H/2;
        hw_output = prDispInfo->pvCAddr;

        if(dwWidth%16)
        {
            dwWidth = ((dwWidth / 16) + 1) * 16; //ming modify@2006/4/11
            //u4ResisualWs = 16 - (dwWidth - rWMVDecInfo.rOther.rPicInfo.dwW);
            //u4FullBlkSz = (dwWidth - 16) * 16;
        }
                
        if(dwHeight % 16)
        {
            dwHeight = ((dwHeight / 16) + 1) * 16;
            //u4ResisualHs = 16 - (dwHeight - (rWMVDecInfo.rOther.rPicInfo.dwH / 2));
            //u4FullFrmSz = dwWidth * (dwHeight - 16);            
        }

        LOG(7,"hw Cbcr address : %x\n",hw_output);
        LOG(7,"CbCr %d size :%d\n", CurFrame/*_u4CurFrmIdx*/, prDispInfo->u4CSize);        
    #ifdef DO_COMPARE_FRAME
        if(CurFrame >= (START_COMPARE_FRAME) && (CurFrame<=END_COMPARE_FRAME))
        {
        LOG(0,"d.load.binary \"%s/%s_%d_CbCr.out\" 0x%08x /ny\n", golden_path[_u4CurFileIdx],_acGoldPrefix[_u4CurFileIdx],CurFrame/*_u4CurFrmIdx*/,golden_dataC);
        //if(!rWMVDecInfo.rOther.u4repeatframe)
            sprintf(_acFileName, "%s/%s_%d_CbCr.out", golden_path[_u4CurFileIdx],_acGoldPrefix[_u4CurFileIdx],CurFrame/*_u4CurFrmIdx*/);
            _LoadFile(_acFileName, golden_dataC);
            //UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"%s/%s_%d_CbCr.out\" 0x%08x /ny", golden_path[_u4CurFileIdx],_acGoldPrefix[_u4CurFileIdx],CurFrame/*_u4CurFrmIdx*/,golden_dataC));
        }
    #endif
       
    #ifdef DO_COMPARE_FRAME
        if((CurFrame >= START_COMPARE_FRAME) && (CurFrame<=END_COMPARE_FRAME))
        {
       for(u4Y = 0; u4Y < (prDispInfo->u4H_Cmp>>1)/*(rWMVDecInfo.rOther.rPicInfo.dwH>>1)*/; u4Y++)
      {
         for(u4X = 0; u4X < prDispInfo->u4W_Cmp/*rWMVDecInfo.rOther.rPicInfo.dwW*/; u4X++)
         {
            pu1GoldenAddr = (UINT8*)u4CalculatePixelAddress_C((UINT32)golden_dataC, u4X, u4Y, dwWidth/*720*/, TRUE, TRUE, 3);
            pu1HWAddr = (UINT8*)u4CalculatePixelAddress_C((UINT32)hw_output, u4X, u4Y, dwWidth, TRUE, TRUE, 4);
            if(*pu1GoldenAddr != *pu1HWAddr)
            {
                    LOG(7,"=== %s dif in C(x%d,y%d), golden(@0x%x=0x%x), hw(@0x%x=0x%x)\n",
                            _acGoldPrefix[_u4CurFileIdx], u4X, u4Y, (UINT32)pu1GoldenAddr, *pu1GoldenAddr, 
                            (UINT32)pu1HWAddr, *pu1HWAddr);
                    //print_checksum(1);
                    Dump_Mem(prDispInfo);
                    //x_mem_free(golden_dataY);
                    //x_mem_free(golden_dataC);
                    return FALSE;                
                    }
         }
     }
        }
    #endif
    
        printf("Cmp CbCr size %d\n", u4X * u4Y);
    }
    }
        _u4CurFrmIdx++;
        }
    

    return;
}


static BOOL _LoadFile(const char* pcFileName, UINT32 u4Addr)
{
    #ifdef  IDE_READ_SUPPORT
    _DmxIdeSendName(pcFileName);
    if(!_DmxIdeGetFile(u4Addr, 0))
    {
        printf("--File load failed\n");
        return FALSE;
    }
    #else
    LOG(1, "//d.load.binary \"%s\" 0x%08x /ny\n", pcFileName, u4Addr);
        #ifdef CC_DRIVER_DEMO    
        UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"%s\" 0x%08x /ny", pcFileName, u4Addr));
        #else
        printf("CC_DRIVER_DEMO !defined, File load fail~");
        #endif
    #endif

    return TRUE;
}

#ifdef IDE_WRITE_SUPPORT
static BOOL _IDEComp(const char* pcFileName, UINT32 u4Addr, UINT32 u4Len)
{
    _DmxIdeSendName(pcFileName);
    _DmxIdeSendDataLen(u4Len);
    _DmxIdeSendData(u4Addr, u4Len);

    return _DmxIdeGetCmpResult();
}
#endif

static BOOL _VdecVerifyDecData(UINT32* pu4GoldData, VDEC_DISP_PIC_INFO_T rDispInfo, BOOL bIsYcomp)
{
    UINT32* pu4DecData;
    UINT32 u4DecSize, u4CmpWord;
    UINT32 u4Residual, u4Pic32XSize;
    UINT32 u4BlkH, u4BlkFrmSize, u4PicH;    //Y: 16x32, CbCr: 16x16
    
    u4BlkH = (bIsYcomp) ? 32 : 16;
    u4BlkFrmSize = u4BlkH << 2;        //size in word
    u4PicH = (bIsYcomp) ? rDispInfo.u4H : (rDispInfo.u4H>>1);
        
    pu4DecData = (UINT32*)((bIsYcomp) ? rDispInfo.pvYAddr : rDispInfo.pvCAddr);
    u4Residual = u4PicH % u4BlkH; // 16 or 0
    u4Pic32XSize = rDispInfo.u4W * (u4PicH - u4Residual);
    
    if(bIsYcomp && (_u4YRREn > 0))
    {
        if(_u4YRRRatio > 0)
        {
            u4DecSize = ((rDispInfo.u4W * u4PicH) * 3) >> 2;
            u4Pic32XSize = (u4Pic32XSize * 3) >> 2;
        }
        else
        {
            u4DecSize = (rDispInfo.u4W * u4PicH) >> 1;
            u4Pic32XSize = u4Pic32XSize >> 1;
        }
    }
    else if((!bIsYcomp) && (_u4CRREn > 0))
    {
        if(_u4CRRRatio > 0)
        {
            u4DecSize = ((rDispInfo.u4W * u4PicH) * 3) >> 2;
            u4Pic32XSize = (u4Pic32XSize * 3) >> 2;
        }
        else
        {
            u4DecSize = (rDispInfo.u4W * u4PicH) >> 1;
            u4Pic32XSize = u4Pic32XSize >> 1;
        }
    }
    else
    {
        u4DecSize = (bIsYcomp) ? rDispInfo.u4YSize: rDispInfo.u4CSize;
    }
    u4DecSize >>= 2;    //size in word
    u4Pic32XSize >>= 2;

    if((_u4YRREn > 0) || (_u4CRREn > 0))
    {        
        for(u4CmpWord = 0; u4CmpWord < u4Pic32XSize; u4CmpWord++)
        {
            //compare data
            if((pu4GoldData[u4CmpWord] != pu4DecData[u4CmpWord]))
            {
                printf("//Error! %c Data Mismatch at [0x%.8x] = 0x%.2x, Golden = 0x%.2x !!! \n", 
                    (bIsYcomp)?'Y':'C', u4CmpWord, pu4DecData[u4CmpWord], pu4GoldData[u4CmpWord]);
                #ifndef STRESS_TESTING
                while(1);
                #else
                return FALSE;
                #endif
            }
        }
    }
    else
    {
        for(u4CmpWord = 0; u4CmpWord < u4DecSize; u4CmpWord++)
        {
            if((rDispInfo.u1PicStruct == BOTTOM_FIELD) && ((u4CmpWord%4) == 0) && (!((u4CmpWord>>2)%2)))
            {
                // Skip Top lines, 16x16 Block mode
                u4CmpWord += 4;    // 4*4 byte 
            }
            //compare data
            if((pu4GoldData[u4CmpWord] != pu4DecData[u4CmpWord]))
            {
                printf("//Error! %c Data Mismatch at [0x%.8x] = 0x%.2x, Golden = 0x%.2x !!! \n", 
                    (bIsYcomp)?'Y':'C', u4CmpWord, pu4DecData[u4CmpWord], pu4GoldData[u4CmpWord]);
                #ifndef STRESS_TESTING
                while(1);
                #else
                return FALSE;
                #endif
            }
            //ignore partial data of the last row if u4Residual > 0
            if(u4Residual && (u4CmpWord > u4Pic32XSize) && 
                (((u4CmpWord - u4Pic32XSize) % u4BlkFrmSize) == ((u4Residual << 2)-1)) && 
                (rDispInfo.u1PicStruct == FRAME))
            {
                u4CmpWord += ((u4BlkH - u4Residual) << 2);
            }    
            /*else if(u4Residual && (u4CmpWord > u4Pic32XSize) && 
                (u4CmpWord % u4BlkFldSize == (u4BlkFrmSize - 1)) && 
                (rDispInfo.u1PicStruct != FRAME))
            {
                u4CmpWord += u4BlkFrmSize;
            }*/
            // odd lines for top, even lines for bottom
            if((rDispInfo.u1PicStruct == TOP_FIELD) && ((u4CmpWord%4) == 3) && (!((u4CmpWord>>2)%2)))
            {      
                // Skip bottom lines, 16x16 Block mode
                u4CmpWord += 4;    // 4*4 = 16 byte
            }
        }
    }

    return TRUE;
}


static void _Vdec1389B2R(UINT8* pu1RasterData, VDEC_DISP_PIC_INFO_T rDispInfo, BOOL bIsYcomp)
{
    UINT8* pu1DecData;
    UINT32 u4CvWByte, u4CvHByte, u4PixIdx;
    UINT32 u4BlkH, u4PicH;    //Y: 16x32, CbCr: 16x16
    
    u4BlkH = (bIsYcomp) ? 32 : 16;
    u4PicH = (bIsYcomp) ? rDispInfo.u4H : (rDispInfo.u4H>>1);
        
    pu1DecData = (UINT8*)((bIsYcomp) ? rDispInfo.pvYAddr : rDispInfo.pvCAddr);

    for(u4CvHByte = 0; u4CvHByte < u4PicH; u4CvHByte += u4BlkH)
    {
        for(u4CvWByte = 0; u4CvWByte < rDispInfo.u4W; u4CvWByte += 16)
        {
            for(u4PixIdx = 0; u4PixIdx < u4BlkH; u4PixIdx++)
            {
                x_memcpy(pu1RasterData, pu1DecData, 16);
                pu1DecData += 16;
                pu1RasterData += rDispInfo.u4W;
            }
            pu1RasterData -= (u4BlkH * rDispInfo.u4W);
            pu1RasterData += 16;
        }
        pu1RasterData += ((u4BlkH-1) * rDispInfo.u4W);
    }
}

static INT32 _VdecCmdSendPattern(INT32 i4Argc, const CHAR ** szArgv)
{
    HANDLE_T hThread;
    ASSERT(szArgv != NULL);    
    
    if(!_bIsPatSending)
    {
        switch (_eFmt)
        {
            case VDEC_FMT_H264: case VDEC_FMT_H264VER:
                if(i4Argc < 3)
                {
                    printf("vdec.sp FileInfoDir LoopCnt {Location of PatternFiles.txt, loop times(0=stress test)}");
                    return 0;
                }
                //prepare memory
                x_memset((void*)_pcFileInfo, 0, FILE_INFO_SIZE);

                sprintf(_acFileName, "%s/PatternFiles.txt", szArgv[1]);
                #ifdef  IDE_READ_SUPPORT
                printf("//PatternFiles.txt\n");
                  #endif    
                _LoadFile(_acFileName, (UINT32)_pcFileInfo);
                
                _i4LoopCnt = StrToInt(szArgv[2]);

                VERIFY(x_thread_create(&hThread, "PseudoDMX", 4096,
                255,  _VdecSendH264Pattern, 0, NULL) == OSR_OK);
                break;
            case VDEC_FMT_MP4:
                VERIFY(x_thread_create(&hThread, "PseudoDMX", 4096,
                255,  _VdecSendMPEG4Pattern, 0, NULL) == OSR_OK);
                break;    
            case VDEC_FMT_WMV:
                _i4LoopCnt = StrToInt(szArgv[1]);
                VERIFY(x_thread_create(&hThread, "PseudoDMX", 4096,
                255,  _VdecSendWMVPattern, 0, NULL) == OSR_OK);
                break;
            default:
                printf("Unknown format type\n");
                break;
        }
    }

    return 0;
}


static void _VdecSendH264Pattern(void* pvArg)
{
    char* pcToken;
    UINT32 u4FileNum = 0,u4Idx = 0;
    VDEC_PES_INFO_T rPesInfo;

    _bIsPatSending = TRUE;
    //parsing file name
    pcToken = strtok(_pcFileInfo, "\r\n");
    while( pcToken != NULL )
    {
        if(pcToken[0] == '#')    //skip the test case
        {
            pcToken = strtok(NULL, "\r\n");
        }
        else
        {
            strcpy(_acFileDir[u4Idx], pcToken);
            pcToken = strtok(NULL, "\r\n");
            if(pcToken != NULL)
            {
                strcpy(_acGoldPrefix[u4Idx++], pcToken);
                pcToken = strtok(NULL, "\r\n");
            }
        }
    }    
    u4FileNum = u4Idx;
    LOG(6, "///FileNum %d\n", u4FileNum);
    //loop all files
    x_memset((void*)&rPesInfo, 0, sizeof(VDEC_PES_INFO_T));
    rPesInfo.u4FifoStart = VFIFO_S;    //should get from dmx fbm
    rPesInfo.u4FifoEnd = VFIFO_E;
    do
    {
        for(_u4CurFileIdx = 0; _u4CurFileIdx < u4FileNum; _u4CurFileIdx++)
        {            
            //reset frame idx
            _u4CurFrmIdx = 0;
            _bIsFirstFrm = TRUE;
            rPesInfo.ucMpvId = VLD0;
            #ifdef RR_TEST
            if(_u4YRREn)
            {
                sprintf(_acFileName, "%s/%s.26l_%05d.bitY", 
                _acFileDir[_u4CurFileIdx], _acGoldPrefix[_u4CurFileIdx], _u4CurFrmIdx);
                _LoadFile(_acFileName, (UINT32)_pu1GoldenData);
                sprintf(_acFileName, "%s/%s.26l_%05d.bitC", 
                    _acFileDir[_u4CurFileIdx], _acGoldPrefix[_u4CurFileIdx], _u4CurFrmIdx);
                _LoadFile(_acFileName, (UINT32)_pu1GoldenDataC);
            }
            else
            {
                sprintf(_acFileName, "%s/%s_0_Y.out", 
                _acFileDir[_u4CurFileIdx], _acGoldPrefix[_u4CurFileIdx], _u4CurFrmIdx);
                _LoadFile(_acFileName, (UINT32)_pu1GoldenData);
                sprintf(_acFileName, "%s/%s_0_CbCr.out", 
                    _acFileDir[_u4CurFileIdx], _acGoldPrefix[_u4CurFileIdx], _u4CurFrmIdx);
                _LoadFile(_acFileName, (UINT32)_pu1GoldenDataC);
            }
            #endif
            
            //load BsInfo.txt
            if(_u1TestSet == 1)        //SarnOff
            {
                    sprintf(_acFileName, "R:/264TS/Sarnoff/BsInfo/%s.264.info", _acGoldPrefix[_u4CurFileIdx]);
            }
            else if(_u1TestSet == 2)    //Allegro2
            {
                    sprintf(_acFileName, "D:/Project/539xVDEC/264_For_DTV/BsInfo2/%s/BsInfo.txt", _acGoldPrefix[_u4CurFileIdx]);
            }
            else
            {                
                    sprintf(_acFileName, "%s/BsInfo.txt", _acFileDir[_u4CurFileIdx]);
            }
            #ifdef  IDE_READ_SUPPORT
            printf("//BsInfo.txt\n");
            #endif    
            _LoadFile(_acFileName, (UINT32)_pcFileInfo);

            //parsing bitstream file name
            pcToken = strtok(_pcFileInfo, "\r\n");
            if(pcToken != NULL)
            {
                sprintf(_acFileName, "%s/%s", _acFileDir[_u4CurFileIdx], pcToken);
                #ifdef  IDE_READ_SUPPORT
                printf("//%s\n", pcToken);
                #endif    
                _LoadFile(_acFileName, rPesInfo.u4FifoStart);
            }
            //parsing bitstream info, offset/type of nalu, and send pes packet    
            pcToken = strtok(NULL, "\r\n");
            while( pcToken != NULL )
            {
                rPesInfo.ucPicType = StrToInt(pcToken);
                pcToken = strtok(NULL, "\r\n");
                if(pcToken != NULL)
                {
                    rPesInfo.u4VldReadPtr = (rPesInfo.u4FifoStart + StrToInt(pcToken)) + 1; // +1 eliminate ucPicType
                    VDEC_SendEs((void*)&rPesInfo);
                    #ifdef RR_TEST
                    if(_u4YRREn)
                    {
                        while(rPesInfo.ucPicType == 0x65)
                        {
                            VDEC_SendEs((void*)&rPesInfo);
                        }
                    }
                    else
                    {
                        while(rPesInfo.ucPicType == 0x25)
                        {
                            VDEC_SendEs((void*)&rPesInfo);
                        }
                    }
                    #endif
                    pcToken = strtok(NULL, "\r\n");
                }
            }
            rPesInfo.ucPicType = 0;
            VDEC_SendEs((void*)&rPesInfo);
            VDEC_SendEs((void*)&rPesInfo);  //for remain 1 Q entry in sendES
            rPesInfo.ucMpvId = VDEC_MAX_VLD;
            VDEC_SendEs((void*)&rPesInfo);
            LOG(6, "//File %d Finish ES Sending\n", _u4CurFileIdx);
            VERIFY(x_sema_lock(_hVdecSema, X_SEMA_OPTION_WAIT) == OSR_OK);    //wait decode done
            LOG(6, "//File %d Finish ES Decoding\n", _u4CurFileIdx);
            //re-play, re-init
            VDEC_Play(0, VDEC_FMT_H264VER);
            VDEC_SetParam(ES0, OUTPUT_DISP_ORDER, _bCmpEn, 0, 0);
            VDEC_SetParam(ES0, RRMODE, _u4YRREn, 0, _u4YRRRatio);
            VDEC_SetParam(ES0, RRMODE, _u4CRREn, 1, _u4CRRRatio);
        }
        
        if(_i4LoopCnt > 0)
        {
            _i4LoopCnt--;
        }
        else
        {
            _i4LoopCnt = -1;    //forever loop
        }
        if(_i4LoopCnt == 0)
        {
            break;
        }
    }while(1);

    _bIsPatSending = FALSE;
    printf("//Finish All ES Decoding\n");

}


static void _VdecSendMPEG4Pattern(void* pvArg)
{
    INT32 i = 0, i4Temp = 0;
    char acPath[MAX_FILENAME_LEN];
    char* pcToken;
    UINT32 u4FileNum = 0,u4Idx = 0;
    INT32 i4LoopCnt = 0;
    HANDLE_T hThread;
    INT32 i4Pos = 0; 
    VDEC_PES_INFO_T rPesInfo;

    printf("Enter _VdecCmdSendPattern");

    x_memset(acPath, 0, MAX_FILENAME_LEN);
    sprintf(acPath, "c:\\BitstreamList.txt");
    x_memset(_pcFileInfo, 0, FILE_INFO_SIZE);    

    printf("Start  to load %s\n", acPath);
    _LoadFile(acPath, (UINT32)_pcFileInfo);
    //UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"%s\" 0x%08x /ny", 
    //            acPath, (UINT32)_pcFileInfo));
    //_LoadFile(acPath, pcFileInfo);
    printf("d.load.binary c:\\BitstreamList.txt 0x%08x /ny", _pcFileInfo);

                
    pcToken = strtok(_pcFileInfo, ",\n");
    if(pcToken != NULL)
    {
        _u4FileCount = StrToInt(pcToken);
        printf("Total File Count = %ld\n", _u4FileCount);
    }

    pcToken = strtok(NULL, ",");
    while(i < _u4FileCount)
    {
        strcpy(_acFileDir[i], pcToken + 1);
        printf("File : %s\n", _acFileDir[i]);
        pcToken = strtok(NULL, ",");
        strcpy(_acTestBitstream[i], pcToken);
        pcToken = strtok(NULL, ",");
        strcpy(_acGoldPrefix[i], pcToken);
        pcToken = strtok(NULL, ",");
        strcpy(_acFilePrefix[i], pcToken);
        pcToken = strtok(NULL, ",");
        _afgLoadCompData[i] = StrToInt(pcToken);
        pcToken = strtok(NULL, ",");
        _au4StartCompareFrameNum[i] = StrToInt(pcToken);
        pcToken = strtok(NULL, ",");
        _au4PicWidth[i] = StrToInt(pcToken);
        pcToken = strtok(NULL, ",\n");
        _au4PicHeight[i++] = StrToInt(pcToken);
        pcToken = strtok(NULL, ",");
    } 

    while (1)
    {
        for(_u4CurFileIdx = 0; _u4CurFileIdx < _u4FileCount; _u4CurFileIdx++)
        {
            //loop all files
            x_memset((void*)&rPesInfo, 0, sizeof(VDEC_PES_INFO_T));
            x_memset(_pcFileInfo, 0, FILE_INFO_SIZE);
            
            rPesInfo.u4VldReadPtr = VFIFO_S;
            rPesInfo.u4FifoStart = VFIFO_S;    //should get from dmx fbm
            rPesInfo.u4FifoEnd = VFIFO_E;

            //reset frame idx
            _u4CurFrmIdx = 0; 

            _fgLoadFail = FALSE;
            
            x_memset(acPath, 0, MAX_FILENAME_LEN);
            sprintf(acPath, "%sBsInfo.txt", _acFileDir[_u4CurFileIdx]);
            
            printf("Start  to load %s\n", acPath);

                    sprintf(_acFileName, "%sBsInfo.txt", _acFileDir[_u4CurFileIdx]);
                    _LoadFile(_acFileName, (UINT32)_pcFileInfo);
            //UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"%sBsInfo.txt\" 0x%08x /ny", 
            //            _acFileDir[_u4CurFileIdx], (UINT32)_pcFileInfo));        
            //_LoadFile(acPath, pcFileInfo);
            printf("d.load.binary \"%sBsInfo.txt\" 0x%08x /ny \n", _acFileDir[_u4CurFileIdx], (UINT32)_pcFileInfo);


            x_memset(acPath, 0, MAX_FILENAME_LEN);
            sprintf(acPath, "%s.m4v", _acTestBitstream[_u4CurFileIdx]);

            if (_afgLoadCompData[_u4CurFileIdx])
            {
                /*UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"%s.m4v\" 0x27a4000 /ny", 
                            _acFileDir[_u4CurFileIdx]));*/
                _LoadFile(acPath, rPesInfo.u4FifoStart);
                printf("d.load.binary \"%s.m4v\" 0x27a4000 /ny \n", _acTestBitstream[_u4CurFileIdx]);
            }
            else
            {
                printf("We don't need to load file\n");
            }

            //parsing bitstream info, offset/type of nalu, and send pes packet     
            pcToken = strtok(_pcFileInfo, ",");
            while( pcToken != NULL )
            {
                if(pcToken != NULL)
                {
                    i4Pos = StrToInt(pcToken);
                    if (i4Pos == 0 && _u4CurFrmIdx != 0)
                    {
                        break;
                    }
                    rPesInfo.u4VldReadPtr = (rPesInfo.u4FifoStart + i4Pos);
                    //rPesInfo.u4Width = _au4PicWidth[_u4CurFileIdx];
                    //rPesInfo.u4Height = _au4PicHeight[_u4CurFileIdx];
                    VDEC_SendEs((void*)&rPesInfo);
                }
                pcToken = strtok(NULL, ",");
            }

               /*while(1)
            {
                if (_u4DecFrmIdx == 0)
                    i4Pos = 0;
                else
                    i4Pos = 47;

                _u4DecFrmIdx++;
                printf("_VdecCmdSendPattern File Position = %ld\n", i4Pos);
                rPesInfo.u4VldReadPtr = (rPesInfo.u4FifoStart + i4Pos);
                printf("_VdecCmdSendPattern rPesInfo.u4VldReadPtr = %08x\n", rPesInfo.u4VldReadPtr);
                VDEC_SendEs((void*)&rPesInfo);
            }*/

            rPesInfo.u4VldReadPtr = 0x00000000;
            VDEC_SendEs((void*)&rPesInfo);
            VERIFY(x_sema_lock(_hVdecSema, X_SEMA_OPTION_WAIT) == OSR_OK);    //wait decode done

        }
        _u4DecCycles++;
        #ifdef CC_DRIVER_DEMO
        UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary c:\\decodingcycles%ld.txt 0x%08x--0x%08x", 
        _u4DecCycles, (UINT32)_pu1GoldenData, (UINT32)(_pu1GoldenData + 1)));
            #else
            printf("CC_DRIVER_DEMO !defined, File save fail~");
            #endif
    }
}

BOOL _VdecWMVIsX8Intra(UINT32 u4addr,UINT32 u4addr2)
{
    return FALSE;
}
void _VdecVParserWMV(VDEC_PES_INFO_T* pVDec_Pes_Info)
{
    VDEC_PES_INFO_T* rPesInfo = pVDec_Pes_Info;
    
    UINT32 u4RCVVersion,u4CodecVersion;
    UINT32 u4RcvNumFrames,u4SetPos;
    UINT32 u4PicHeightSrc,u4PicWidthSrc;
    UINT32 u4HeaderLen,u4SeqHdrData1,u4SeqHdrDataLen;
    //BOOL bPreviousSkip = FALSE;
    BOOL bSkipFrame = FALSE;
    UINT32 hdrext;
    UINT8 u1flag=0;
    UINT32 u4FrameIndex = 0;

    u4SetPos = 0;

    //rPesInfo->fgSkipFrame = FALSE;

    if( rPesInfo->ucPicType == 1 )  { //.vc1
        rPesInfo->ucPicType = 7;//WMVA;
    }
    else if( rPesInfo->ucPicType == 2 ) //.rcv
    {
        x_memcpy(&u4RcvNumFrames,((rPesInfo->u4FifoStart)+u4SetPos),4);
        u4SetPos +=4;
        u4RCVVersion = (u4RcvNumFrames >> 30) & 0x1;
        u4CodecVersion = u4RcvNumFrames >> 24;
        u4RcvNumFrames &= 0xffffff;
        hdrext = (u4CodecVersion >> 7) & 0x1;
        if (u4RCVVersion == 0) {
            u4CodecVersion &= 0x7f;
        }
        else {
            u4CodecVersion &= 0x3f;
        }
        if((u4CodecVersion==0)){ //wmv7
            rPesInfo->ucPicType = 4;//WMV1;
        }
        else if(u4CodecVersion == 2) {// WMV8 
            rPesInfo->ucPicType = 5;//WMV2;
        }
        else if(u4CodecVersion == 5){ // Simple & Main Profile
            rPesInfo->ucPicType = 6;//WMV3;
        }
        else if(u4CodecVersion == 6){ // Advanced Profile
            rPesInfo->ucPicType = 7;//WMVA;
        }
        else if(u4CodecVersion == 8){ // Advanced Profile
            rPesInfo->ucPicType = 7;//WMVA;
        }
        else {
            rPesInfo->ucPicType = 8;//UNKNOW_TYPE;
            LOG(1,"UNKNOW TYPE\n");           
        }
    }
    if((rPesInfo->ucPicType)!= 7) //for others , no start code
    {
        if(hdrext != 0) 
        {
            x_memcpy(&u4SeqHdrDataLen,((rPesInfo->u4FifoStart)+u4SetPos),4);
            u4SetPos += 4; // parsing SeqHdrDataLen
            x_memcpy(&u4SeqHdrData1,((rPesInfo->u4FifoStart)+u4SetPos),4);
            u4SetPos += 4; // parsing SeqHdrData1
            u4SeqHdrData1 = ((u4SeqHdrData1&0x000000FF)<<24) | ((u4SeqHdrData1&0x0000FF00)<<8) | ((u4SeqHdrData1&0x00FF0000)>>8) | ((u4SeqHdrData1&0xFF000000)>>24);
              if(u4SeqHdrDataLen == 5) {
                u4SetPos +=1;
              }
              else {
              }
        }
        x_memcpy(&u4PicHeightSrc,((rPesInfo->u4FifoStart)+u4SetPos),4);
        u4SetPos += 4;
        x_memcpy(&u4PicWidthSrc,((rPesInfo->u4FifoStart)+u4SetPos),4);
        u4SetPos += 4;        
        LOG(0,"Source Resolution = %d x %d\n",u4PicWidthSrc, u4PicHeightSrc);
           if (u4RCVVersion == 1) {    
             u4SetPos +=16;
           }
        x_memcpy(&u4HeaderLen,((rPesInfo->u4FifoStart + u4SetPos)),4);
        u4HeaderLen += 4;
        u4SetPos +=4;
        u1flag++;
        if(u4RCVVersion == 0) {
              u4HeaderLen &= 0x0fffffff;
              if(u4HeaderLen == 5) // it indicates Picture Length == 1, means a skipped frame
              {
                //bPreviousSkip = TRUE;
                bSkipFrame = TRUE;
                LOG(0,"Frame %d : Skip Frame \n",u4FrameIndex);
              }
            else
            {
                //bPreviousSkip = FALSE;
                bSkipFrame = FALSE;
            }
        }
        else if(u4RCVVersion == 1)
        {
              u4HeaderLen &= 0x0fffffff;
              if(u4HeaderLen == 5) // it indicates Picture Length == 1, means a skipped frame
              {
                //bPreviousSkip = TRUE;
                bSkipFrame = TRUE;
                LOG(0,"Frame %d : Skip Frame \n",u4FrameIndex);
              }
            else
            {
                //bPreviousSkip = FALSE;
                bSkipFrame = FALSE;
            }
            u4SetPos +=4;
            u4HeaderLen += 4;
            u1flag++;
        }
        else
        {
              ASSERT(FALSE);
        }
        rPesInfo->fgGop = bSkipFrame;
        rPesInfo->u4VldReadPtr = u4SetPos;
        VDEC_SetParam(0,0,rPesInfo->ucPicType,u4PicWidthSrc,u4PicHeightSrc);
        VDEC_SetParam(0,1,u4SeqHdrDataLen,u4SeqHdrData1,bSkipFrame);
        if(rPesInfo->ucPicType==5 && _VdecWMVIsX8Intra(rPesInfo->u4VldReadPtr,rPesInfo->u4FifoStart))
        {
            return;
            
        }
        else
        {
            VDEC_SendEs((void*)rPesInfo);
            u4FrameIndex++;
        }
        do {
            if(u1flag ==1)
                u4SetPos -= 4;
            else if(u1flag ==2)
                u4SetPos -= 8;
            u1flag=0;
            u4SetPos += u4HeaderLen;
            rPesInfo->u4VldReadPtr = u4SetPos;
            x_memcpy(&u4HeaderLen,((rPesInfo->u4VldReadPtr + rPesInfo->u4FifoStart)),4);
            u4HeaderLen += 4;
            u4SetPos +=4;
            u1flag++;
            if(u4RCVVersion == 0) {
                  u4HeaderLen &= 0x0fffffff;
                  if(u4HeaderLen == 5) // it indicates Picture Length == 1, means a skipped frame
                  {
                    //bPreviousSkip = TRUE;
                    bSkipFrame = TRUE;
                    LOG(0,"Frame %d : Skip Frame \n",u4FrameIndex);
                  }
                else
                {
                    //bPreviousSkip = FALSE;
                    bSkipFrame = FALSE;
                }
            }
            else if(u4RCVVersion == 1)
            {
                  u4HeaderLen &= 0x0fffffff;
                  if(u4HeaderLen == 5) // it indicates Picture Length == 1, means a skipped frame
                  {
                    //bPreviousSkip = TRUE;
                    bSkipFrame = TRUE;
                    LOG(0,"Frame %d : Skip Frame \n",u4FrameIndex);
                  }
                else
                {
                    //bPreviousSkip = FALSE;
                    bSkipFrame = FALSE;
                }
                u4SetPos += 4;
                u4HeaderLen += 4;
                u1flag++;
            }
            else
            {
                  ASSERT(FALSE);
            }
            rPesInfo->u4VldReadPtr = u4SetPos;
            rPesInfo->fgGop = bSkipFrame;
            //VDEC_SetParam(0,1,u4SeqHdrDataLen,u4SeqHdrData1,bSkipFrame);
            if((rPesInfo->u4VldReadPtr +0x4)< bitstream_sz[_u4CurFileIdx]) {
                if(rPesInfo->ucPicType==5 && _VdecWMVIsX8Intra(rPesInfo->u4VldReadPtr,rPesInfo->u4FifoStart))
                {
                    return;
                }
                else
                {
                    VDEC_SendEs((void*)rPesInfo);
                    u4FrameIndex++;
                }
                //VDEC_SendEs((void*)rPesInfo);
            } else {
                break;
            }
        }while((rPesInfo->u4VldReadPtr +0x4)< bitstream_sz[_u4CurFileIdx]);

    }
    else //start code version
    {
        CHAR* pcToken;
        VDEC_SetParam(0,0,rPesInfo->ucPicType,0,0);
        x_memset((void*)_pcStartCodeFile, 0, FILE_INFO_SIZE);
        #ifdef CC_DRIVER_DEMO
        LOG(0,"d.load.binary \"%s\\StartCode_%s.txt\" 0x%08x /ny\n","D:\\WMV\\vc1_start_code"/*golden_path[_u4CurFileIdx]*/,_acGoldPrefix[_u4CurFileIdx], _pcStartCodeFile);
        UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"%s\\StartCode_%s.txt\" 0x%08x /ny","D:\\WMV\\vc1_start_code"/*golden_path[_u4CurFileIdx]*/,_acGoldPrefix[_u4CurFileIdx], _pcStartCodeFile));

        pcToken = strtok(_pcStartCodeFile, "\r\n");
        while( pcToken != NULL) {
            rPesInfo->u4VldReadPtr = StrToInt(pcToken);
            VDEC_SendEs((void*)rPesInfo);
            pcToken = strtok(NULL, "\r\n");
        } 
        #endif
    }

}

void _VdecSendWMVPattern(void* pvArg)
{
    VDEC_PES_INFO_T rPesInfo;
    CHAR* pcToken;
    UINT32 iLen;
    UINT16 u2file_num=0;
    UINT16 index=0;    
    char acPath[MAX_FILENAME_LEN];            

    char pattern_path[512]= "c:\\PatternFiles.txt";
    char golden_path_arg[512] ="c:\\Golden.txt";


    //char* start_code_file = x_mem_alloc(FILE_INFO_SIZE);
    //start_code_file = (CHAR*)((UINT32)start_code_file | 0x30000000);

    x_memset((void*)_pcFileInfo, 0, FILE_INFO_SIZE);
    #ifdef CC_DRIVER_DEMO
    LOG(0,"d.load.binary1 \"%s\" 0x%08x /ny\n",pattern_path, _pcFileInfo);
    UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"%s\" 0x%08x /ny", pattern_path, _pcFileInfo));

    pcToken = strtok(_pcFileInfo, "\r\n");
    while( pcToken != NULL) { 
        strcpy(_acFileDir[u2file_num], pcToken);
        pcToken = strtok(NULL, "\r\n");
        bitstream_sz[u2file_num++] = StrToInt(pcToken);
        //Printf("bitstream_sz : %d \n",bitstream_sz[u2file_num-1]);
        pcToken = strtok(NULL, "\r\n");
    }

    index=0;
    x_memset((void*)_pcFileInfo, 0, FILE_INFO_SIZE);

    LOG(0,"d.load.binary2 \"%s\" 0x%08x /ny\n",golden_path_arg, _pcFileInfo);
    UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"%s\" 0x%08x /ny", golden_path_arg, _pcFileInfo));

    pcToken = strtok(_pcFileInfo, "\r\n");
    while( pcToken != NULL) {
        strcpy(golden_path[index], pcToken);
        pcToken = strtok(NULL, "\r\n");
        strcpy(_acGoldPrefix[index++], pcToken);
        pcToken = strtok(NULL, "\r\n");
    } 

    do
    {
        for(_u4CurFileIdx =0 ; _u4CurFileIdx < u2file_num ; _u4CurFileIdx++)
        {
            _u4CurFrmIdx =0;
            x_memset((void*)&rPesInfo, 0, sizeof(VDEC_PES_INFO_T));
              if(_acFileDir[_u4CurFileIdx][0] == '#')
              {
                  continue;    //skip the pattern
              }

            //get input_bitstream to vFIFO start point
            LOG(0,"d.load.binary \"%s\" 0x%08x /ny\n",_acFileDir[_u4CurFileIdx], VFIFO_S);
            //UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"%s\" 0x%08x /ny",_acFileDir[_u4CurFileIdx], VFIFO_S));
            x_memset(acPath, 0, MAX_FILENAME_LEN);
            sprintf(acPath, "%s", _acFileDir[_u4CurFileIdx]);
            _LoadFile(acPath,VFIFO_S);

            iLen = x_strlen(_acFileDir[_u4CurFileIdx]);


            if( (_acFileDir[_u4CurFileIdx][iLen-4] == '.') && (_acFileDir[_u4CurFileIdx][iLen-3] == 'v') &&
                (_acFileDir[_u4CurFileIdx][iLen-2] == 'c') && (_acFileDir[_u4CurFileIdx][iLen-1] == '1') )
            {
                //rPesInfo.WMV_TYPE = 1;
                rPesInfo.ucPicType = 1;
                //rWMVDecInfo.rOther.u4CodecFOURCC = FOURCC_WVC1_WMV;
            }
            else if( (_acFileDir[_u4CurFileIdx][iLen-4] == '.') && (_acFileDir[_u4CurFileIdx][iLen-3] == 'r') &&
                (_acFileDir[_u4CurFileIdx][iLen-2] == 'c') && (_acFileDir[_u4CurFileIdx][iLen-1] == 'v') )
            {
                //rPesInfo.WMV_TYPE = 2;
                rPesInfo.ucPicType = 2;
            }
            //vCodecVersion();
            //m_dwBitsFSz = bitstream_sz[_u4CurFileIdx];
            //Printf("before passing parameter size : %d\n",m_dwBitsFSz);
            //rWMVDecInfo.rOther.u4FileLen = m_dwBitsFSz;
            rPesInfo.u4VldReadPtr = VFIFO_S;
            rPesInfo.u4FifoStart = VFIFO_S;    //should get from dmx fbm
            rPesInfo.u4FifoEnd = VFIFO_E;
            _VdecVParserWMV(&rPesInfo);
            rPesInfo.ucPicType = 255;
            VDEC_SendEs((void*)&rPesInfo);
            VERIFY(x_sema_lock(_hVdecSema, X_SEMA_OPTION_WAIT) == OSR_OK);    //wait decode done
            //re-play, re-init
            VDEC_Play(0, VDEC_FMT_WMV);
        }
        if(_i4LoopCnt > 0)
        {
            _i4LoopCnt--;
        }
        else
        {
            _i4LoopCnt = -1;    //forever loop
        }
        if(_i4LoopCnt == 0)
        {
            break;
        }
    }while(1);
    #endif
}

static INT32 _VdecCmdSaveDecOut(INT32 i4Argc, const CHAR ** szArgv)
{
    ASSERT(szArgv != NULL);    
    if(i4Argc < 2)
    {
        printf("vdec.so enable(1)");
        return 0;
    }
    _bSaveOutput = (BOOL)StrToInt(szArgv[1]);
    return 0;    
}

void Dump_Mem(VDEC_DISP_PIC_INFO_T* prDispInfo)
{
    #ifndef DUMP_MEM
    return;
    #endif 
    
    static int i = 0;
    UINT8* pu1HWAddr;
    UINT8* hw_output;

    UINT32 dwWidth = prDispInfo->u4W;
    UINT32 dwHeight = prDispInfo->u4H;
    //UINT32 m_dwCurrPicNo = _u4CurFrmIdx;

    //UINT32 dwWidth = rWMVDecInfo.rSeqHdrR.u4PicWidthCmp;
    //UINT32 dwHeight = rWMVDecInfo.rSeqHdrR.u4PicHeightCmp;
    
    hw_output = prDispInfo->pvYAddr;
        
    if(dwWidth%16)
        dwWidth = ((dwWidth / 16) + 1) * 16; //ming modify@2006/4/11
    if(dwHeight % 32) 
        dwHeight = ((dwHeight / 32) + 1) * 32;
        

    pu1HWAddr = (UINT8*)u4CalculatePixelAddress_Y((UINT32)hw_output, 0, 0, dwWidth, TRUE, 4);

      #ifdef CC_DRIVER_DEMO
    UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"d:\\dump\\tmp\\%s_Y_%d.raw\" 0x%08x--0x%08x",_acGoldPrefix[_u4CurFileIdx],_u4CurFrmIdx,pu1HWAddr, pu1HWAddr+dwWidth*dwHeight-1));
      #else
      printf("CC_DRIVER_DEMO !defined, File save fail~");
      #endif
    //UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"d:\\dump\\tmp\\%s_Y0_%d.raw\" 0x%08x--0x%08x",_acGoldPrefix[_u4CurFileIdx],_u4CurFrmIdx,rWMVDecInfo.rDram.pu1Pic0Y, rWMVDecInfo.rDram.pu1Pic0Y+dwWidth*dwHeight-1));
    //UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"d:\\dump\\tmp\\%s_Y1_%d.raw\" 0x%08x--0x%08x",_acGoldPrefix[_u4CurFileIdx],_u4CurFrmIdx,rWMVDecInfo.rDram.pu1Pic1Y, rWMVDecInfo.rDram.pu1Pic1Y+dwWidth*dwHeight-1));
    //UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"d:\\dump\\tmp\\%s_Y2_%d.raw\" 0x%08x--0x%08x",_acGoldPrefix[_u4CurFileIdx],_u4CurFrmIdx,rWMVDecInfo.rDram.pu1Pic2Y, rWMVDecInfo.rDram.pu1Pic2Y+dwWidth*dwHeight-1));

    hw_output = prDispInfo->pvCAddr;
    pu1HWAddr = (UINT8*)u4CalculatePixelAddress_C((UINT32)hw_output, 0, 0, dwWidth, TRUE, TRUE, 4);

      #ifdef CC_DRIVER_DEMO
    UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"d:\\dump\\tmp\\%s_C_%d.raw\" 0x%08x--0x%08x",_acGoldPrefix[_u4CurFileIdx],_u4CurFrmIdx,pu1HWAddr, (pu1HWAddr+dwWidth*dwHeight/2)-1));
      #else
      printf("CC_DRIVER_DEMO !defined, File save fail~");
      #endif
    //UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"d:\\dump\\tmp\\%s_C0_%d.raw\" 0x%08x--0x%08x",_acGoldPrefix[_u4CurFileIdx],rWMVDecInfo.rOther.u4CurrPicNo,rWMVDecInfo.rDram.pu1Pic0C, (rWMVDecInfo.rDram.pu1Pic0C+dwWidth*dwHeight/2)-1));
    //UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"d:\\dump\\tmp\\%s_C1_%d.raw\" 0x%08x--0x%08x",_acGoldPrefix[_u4CurFileIdx],rWMVDecInfo.rOther.u4CurrPicNo,rWMVDecInfo.rDram.pu1Pic1C, (rWMVDecInfo.rDram.pu1Pic1C+dwWidth*dwHeight/2)-1));
    //UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"d:\\dump\\tmp\\%s_C2_%d.raw\" 0x%08x--0x%08x",_acGoldPrefix[_u4CurFileIdx],rWMVDecInfo.rOther.u4CurrPicNo,rWMVDecInfo.rDram.pu1Pic2C, (rWMVDecInfo.rDram.pu1Pic2C+dwWidth*dwHeight/2)-1));


    //UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"d:\\dump\\tmp\\%s_BP1_%d.dat\" 0x%08x--0x%08x",_u4CurFileIdx[_u4CurFileIdx],rWMVDecInfo.rOther.u4CurrPicNo,Bp_1, Bp_2));
    //UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"d:\\dump\\tmp\\%s_BP2_%d.dat\" 0x%08x--0x%08x",_u4CurFileIdx[_u4CurFileIdx],rWMVDecInfo.rOther.u4CurrPicNo,Bp_2, Bp_3));
    //UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"d:\\dump\\tmp\\%s_BP3_%d.dat\" 0x%08x--0x%08x",_u4CurFileIdx[_u4CurFileIdx],rWMVDecInfo.rOther.u4CurrPicNo,Bp_3, Bp_4));
    //UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"d:\\dump\\tmp\\%s_BP4_%d.dat\" 0x%08x--0x%08x",_u4CurFileIdx[_u4CurFileIdx],rWMVDecInfo.rOther.u4CurrPicNo,Bp_4, Mv_3));
    //UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"d:\\dump\\tmp\\%s_MV1_%d.dat\" 0x%08x--0x%08x",_fiInName,rWMVDecInfo.rOther.u4CurrPicNo,Mv_1, Mv_2));
    //UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"d:\\dump\\tmp\\%s_MV12_%d.dat\" 0x%08x--0x%08x",_fiInName,rWMVDecInfo.rOther.u4CurrPicNo,Mv_1_2, DCAC_2));

    
    //UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"d:\\dump\\tmp\\%s_stream_%d_%d.dat\" 0x%08x--0x%08x",_acGoldPrefix[_u4CurFileIdx],rWMVDecInfo.rSeqHdrR.u4PicWidthDec, rWMVDecInfo.rSeqHdrR.u4PicHeightDec, V_FIFO_SA, V_FIFO_SA+rWMVDecInfo.rOther.u4FileLen));

    i++;
}

/*
void print_checksum(BOOL fgRead) {
    UINT32 u4Idx;
    
        //Printf("===Dump VLD mem ===cc\n");    
    //UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"d:\\dump\\tmp\\%s_VLD_90_%d.raw\" 0x%08x--0x%08x",_fiInName,rWMVDecInfo.rOther.u4CurrPicNo,RW_VLD_PICSZ, RW_VLD_MV3_ADDR));

        Printf("===WMV MC status===\n");    
        Printf("MC status, 37(0x94)=0x%x\n",ReadREG(RO_MC_STATUS));
        Printf("MC QIU status, 245(0x3d4)=0x%x\n",ReadREG(RO_MC_QIU_STATUS));
        Printf("MC HIU status, 246(0x3d8)=0x%x\n",ReadREG(RO_MC_HIU_STATUS));
    Printf("MC CBP, 27(0x6C)=0x%x\n", ReadREG(RO_MC_CBP));
    
        Printf("===WMV VLD status===\n");
        Printf("Decode complete, 240 : %d\n",ReadREG(RO_VLD_PIC_DEC_END));
    for(u4Idx = (VLD_REG_OFST + (0x208)); u4Idx <= (VLD_REG_OFST + (0x258)); u4Idx+=4)
    {
        Printf("VLD status, %d(0x%3x)=0x%x\n", ((u4Idx-VLD_REG_OFST)>>2), u4Idx, ReadREG(u4Idx));
    }
    Printf("===WMV CheckSum===\n");
        Printf("_DCAC_1_CheckSum = 0x%x\n",ReadREG(VLD_REG_OFST+(0x3C4)));
          Printf("_DCAC_2_CheckSum = 0x%x\n",ReadREG(VLD_REG_OFST+(0x3C8)));
          //Printf("_DCAC_237_CheckSum = 0x%x\n",VLD_READ32((0x3B4)));
          //Printf("_DCAC_238_CheckSum = 0x%x\n",VLD_READ32((0x3B8)));
          //Printf("_VLD_189_CheckSum = %x\n",ReadREG(VLD_REG_OFST + (0x2F4)));
          Printf("MV1 = 0x%x\n",ReadREG(VLD_REG_OFST+(0x3D8)));
          Printf("MV2 = 0x%x\n",ReadREG(VLD_REG_OFST+(0x3DC)));
          Printf("MV3 = 0x%x\n",ReadREG(VLD_REG_OFST+(0x3E0)));
          
          Printf("DP1 = 0x%x\n",ReadREG(VLD_REG_OFST+(0x3EC)));

          Printf("VOP_REG = 0x%x\n",ReadREG(VLD_REG_OFST+(0x350)));
          Printf("DCSA2_REG = 0x%x\n",ReadREG(VLD_REG_OFST+(0x354)));
          Printf("MV3SA_REG = 0x%x\n",ReadREG(VLD_REG_OFST+(0x358)));
          Printf("DCFSM_REG = 0x%x\n",ReadREG(VLD_REG_OFST+(0x37C)));
          Printf("DCBLK_REG = 0x%x\n",ReadREG(VLD_REG_OFST+(0x380)));
          Printf("DCDRAM_REG = 0x%x\n",ReadREG(VLD_REG_OFST+(0x384)));

          Printf("MVST_REG = 0x%x\n",ReadREG(VLD_REG_OFST+(0x388)));
          Printf("MVDRAM_REG = 0x%x\n",ReadREG(VLD_REG_OFST+(0x38C)));
          Printf("MBST_REG = 0x%x\n",ReadREG(VLD_REG_OFST+(0x390)));
          

          Printf("IDCT Input Y0 Y1 Checksum Register = 0x%x\n",ReadREG(MC_REG_OFST+(0x5E8)));
          Printf("IDCT Input Y2 Y3 Checksum Register = 0x%x\n",ReadREG(MC_REG_OFST+(0x5EC)));
          Printf("IDCT Input Cb Cr Checksum Register = 0x%x\n",ReadREG(MC_REG_OFST+(0x5F0)));
          Printf("MC Output Luma Checksum Register = 0x%x\n",ReadREG(MC_REG_OFST+(0x5F4)));
          Printf("MC Output Chroma Checksum Register = 0x%x\n",ReadREG(MC_REG_OFST+(0x5F8)));
          Printf("Motion Vector Checksum Register = 0x%x\n",ReadREG(MC_REG_OFST+(0x5FC)));
          Printf("MC Parameter 1 Checksum Register = 0x%x\n",ReadREG(MC_REG_OFST+(0x600)));
          Printf("MC Parameter 2 Checksum Register = 0x%x\n",ReadREG(MC_REG_OFST+(0x604)));

          Printf("WMV parameter 2: %x\n",ReadREG(RW_VLD_WMV_PARA2));
         
        if(fgRead)
        {
        for(u4Idx = 0; u4Idx <= 231; u4Idx++)
        {
            _mcreg[u4Idx] = ReadREG(MC_REG_OFST + u4Idx*4);
        }
        
              //VLD from 34 ~ 147,  192~250, except 40, 41 (read will lead some impact of HW)
        for(u4Idx = 34; u4Idx <= 39; u4Idx++)
        {
            _vlcreg[u4Idx] = ReadREG(VLD_REG_OFST + u4Idx*4);
        }
        for(u4Idx = 42; u4Idx <= 147; u4Idx++)
        {
            _vlcreg[u4Idx] = ReadREG(VLD_REG_OFST + u4Idx*4);
        }
        for(u4Idx = 192; u4Idx <= 250; u4Idx++)
        {
            _vlcreg[u4Idx] = ReadREG(VLD_REG_OFST + u4Idx*4);
        }
         }
          
        Printf("===WMV MC ALL===\n");
        for(u4Idx = (MC_REG_OFST + (0)); u4Idx <= (MC_REG_OFST + (0x39C)); u4Idx+=4)
    {
        Printf("MC status, %d(0x%3x)=0x%x\n", ((u4Idx-MC_REG_OFST)>>2), u4Idx, _mcreg[(u4Idx - MC_REG_OFST)>>2]);
    }
        
          //VLD from 34 ~ 147,  192~250, except 40, 41 (read will lead some impact of HW)
        Printf("===WMV VLD Other===\n");
    for(u4Idx = (VLD_REG_OFST + (0x88)); u4Idx <= (VLD_REG_OFST + (0x9C)); u4Idx+=4)
    {
        Printf("VLD status, %d(0x%3x)=0x%x\n", ((u4Idx-VLD_REG_OFST)>>2), u4Idx, _vlcreg[(u4Idx - VLD_REG_OFST)>>2]);
    }
    for(u4Idx = (VLD_REG_OFST + (0xA8)); u4Idx <= (VLD_REG_OFST + (0x24C)); u4Idx+=4)
    {
        Printf("VLD status, %d(0x%3x)=0x%x\n", ((u4Idx-VLD_REG_OFST)>>2), u4Idx, _vlcreg[(u4Idx - VLD_REG_OFST)>>2]);
    }
    for(u4Idx = (VLD_REG_OFST + (0x300)); u4Idx <= (VLD_REG_OFST + (0x3e8)); u4Idx+=4)
    {
        Printf("VLD status, %d(0x%3x)=0x%x\n", ((u4Idx-VLD_REG_OFST)>>2), u4Idx, _vlcreg[(u4Idx - VLD_REG_OFST)>>2]);
    }
    
    
}
*/


UINT32 u4CalculatePixelAddress_Y(
  UINT32 u4YBase,                           ///< [IN] frame buffer Y component address
  UINT32 u4XPos,                             ///< [IN] x position of pixel in frame buffer
  UINT32 u4YPos,                             ///< [IN] y position of pixel in frame buffer
  UINT32 u4FrameWidth,                 ///< [IN] Frame buffer width
  BOOL fgBlock,                               ///< [IN] MTK block / raster scan
  UCHAR bBlockW
)
{
  UINT32 u4YAdr;
  UINT32 u4X = u4XPos;
  UINT32 u4Y = u4YPos;
  UINT32 u4YBlockNum;
  UINT32 u4YOffsetInBlock;
  UINT32 u4XBlockNum;
  UINT32 u4XOffsetInBlock;



  if(bBlockW == 4)  // in MT8520, one block width is (16x32)
  {
    u4XOffsetInBlock = u4X & 0xF;
  }
  else if(bBlockW == 3)  // in MT8108 and MT1389S, one block width is (8x32)
  {
    u4XOffsetInBlock = u4X & 0x7;
  }
  else if(bBlockW == 2)  // in MT8105, one block width is (4x32)
  {
    u4XOffsetInBlock = u4X & 0x3;
  }


  // Y arrangement is the same in 420 and 422 YCbCr Mode.
  u4YBlockNum = u4Y >> 5;
  u4YOffsetInBlock = u4Y & 0x1F;
  u4XBlockNum = u4X >> bBlockW;
  if (fgBlock)
    u4YAdr = u4YBase + ((u4FrameWidth * u4YBlockNum) << 5) + ((u4YOffsetInBlock + (u4XBlockNum << 5) ) << bBlockW) + u4XOffsetInBlock;
  else
    u4YAdr =  u4YBase + (u4Y * u4FrameWidth) + u4X;

  return u4YAdr;
}

UINT32 u4CalculatePixelAddress_C(
  UINT32 u4CBase,                           ///< [IN] frame buffer CbCr component address
  UINT32 u4XPos,                             ///< [IN] x position of pixel in frame buffer
  UINT32 u4YPos,                             ///< [IN] y position of pixel in frame buffer
  UINT32 u4FrameWidth,                 ///< [IN] Frame buffer width
  BOOL fgBlock,                               ///< [IN] MTK block / raster scan
  BOOL fg420,                                   ///< [IN] 420 / 422
  UCHAR bBlockW
)
{
  UINT32 u4CAdr;
  UINT32 u4X = u4XPos;
  UINT32 u4Y = u4YPos;
  UINT32 u4YBlockNum;
  UINT32 u4YOffsetInBlock;
  UINT32 u4XBlockNum;
  UINT32 u4XOffsetInBlock;
  

  u4YBlockNum = u4Y >> 5;
  u4YOffsetInBlock = u4Y & 0x1F;
  u4XBlockNum = u4X >> bBlockW;

  // Map C (chrominance)
  u4X &= 0xFFFE;
  if(bBlockW == 4)
  {
    u4XOffsetInBlock = u4X & 0xF;
  }
  else if(bBlockW == 3)
  {
    u4XOffsetInBlock = u4X & 0x7;
  }
  else if(bBlockW == 2)
  {
     u4XOffsetInBlock = u4X & 0x3;
   }
  
  u4FrameWidth = ((u4FrameWidth + 1) & 0xFFFE);
  if (fg420)
  {
      u4Y = u4Y >> 1;
      u4YOffsetInBlock = u4Y & 0xF;
      if (fgBlock)
        u4CAdr = u4CBase + ((u4FrameWidth * u4YBlockNum) << 4) + ((u4YOffsetInBlock + (u4XBlockNum << 4) ) << bBlockW) + u4XOffsetInBlock;
      else
        u4CAdr = u4CBase + (u4Y * u4FrameWidth) + (u4X);
   }
   else
   {
      // This code should be emended.
      if (fgBlock)
        u4CAdr = u4CBase + ((u4FrameWidth * u4YBlockNum) << 5) + ((u4YOffsetInBlock + (u4XBlockNum << 4) ) << bBlockW) + u4XOffsetInBlock;
      else
        u4CAdr = u4CBase + (u4Y * u4FrameWidth) + (u4X);
    }

  return u4CAdr;
}

/*
BOOL WMV_save_pic(CHAR* golden_path,CHAR* golden_prefix)
{
  
  if((rWMVDecInfo.rPicLayer.u1FrameCodingMode != INTERLACEFIELD)||(rWMVDecInfo.rPicLayer.i4CurrentTemporalField == 1))
  {
    if(!my_save_pic(golden_path,golden_prefix, TRUE))
    {
        return FALSE;
    }
    if(!my_save_pic(golden_path,golden_prefix, FALSE))
    {
        return FALSE;
    }
  }
  return TRUE;
}*/

static INT32 _VdecCmdSaveSimPat(INT32 i4Argc, const CHAR ** szArgv)
{
    ASSERT(szArgv != NULL);    
    if(i4Argc < 3)
    {
        printf("vdec.ssp [enable 1][Frame number(start from 0)]");
        return 0;
    }
    if(StrToInt(szArgv[1]) == 0)
    {
        _i4SSimPatFrmNo = -1;
    }
    else
    {
        _i4SSimPatFrmNo = (INT32)StrToInt(szArgv[2]);
        if(_i4SSimPatFrmNo==0)
        {
            gu1VdecLogSimPat = 1;
            VDEC_SetParam(ES0, LOG_SIMU_PAT, 1, 0, 0);
        }
    }
    return 0;    
}

static INT32 _VdecCmdSetTestMode(INT32 i4Argc, const CHAR ** szArgv)
{
    UINT32 u4FrmIdx;
    
    ASSERT(szArgv != NULL);
    if(i4Argc < 3)
    {
        printf("vdec.stm [TestSet 0:other, 1:Allegro, 2:Allegro2][Comparison Enable]([FrameNo1][FrameNo2]...)");
        return 0;
    }
    VDEC_SetParam(ES0, OUTPUT_DISP_ORDER, StrToInt(szArgv[2]), 0, 0);

    _u1TestSet = (UINT8)StrToInt(szArgv[1]);
    if(StrToInt(szArgv[2]) > 0)
    {
        _bCmpEn = TRUE;
        if(i4Argc > 3)
        {
            x_memset((void*)_bCmpFrmEn, 0, sizeof(_bCmpFrmEn));    
            for(u4FrmIdx = 3; u4FrmIdx < i4Argc; u4FrmIdx++)
            {
                _bCmpFrmEn[StrToInt(szArgv[u4FrmIdx])] = TRUE;
            }
        }
        else
        {
            for(u4FrmIdx = 0; u4FrmIdx < 1000; u4FrmIdx++)
            {
                _bCmpFrmEn[u4FrmIdx] = TRUE;
            }            
        }
    }
    else
    {
        x_memset((void*)_bCmpFrmEn, 0, sizeof(_bCmpFrmEn));
        _bCmpEn = FALSE;
    }

    return 0;    
}

static INT32 _VdecCmdSetRepeatFrm(INT32 i4Argc, const CHAR ** szArgv)
{
    ASSERT(szArgv != NULL);
    if(i4Argc < 3)
    {
        printf("vdec.srf [Repeat Frame Enable][Repeat FrameNo]\n");
        printf("vdec.srf %d %d\n", _bRptFrmEn, _u4RptFrmNo);
        return 0;
    }
    if(StrToInt(szArgv[1]) > 0)
    {
        _bRptFrmEn = TRUE;
        _u4RptFrmNo = (UINT32)StrToInt(szArgv[2]);
        if(_u4RptFrmNo == 0)
        {
            VDEC_SetParam(ES0, REPEAT_FRM, 1, 0, 0);
        }
    }
    else
    {
        _bRptFrmEn = FALSE;
        _u4RptFrmNo = 0;
    }

    return 0;
}
#endif

static INT32 _VdecCmdDramBWSimu(INT32 i4Argc, const CHAR ** szArgv)
{
    UINT32 u4Param1=0, u4Param2=0, u4Param3=0;
    
    ASSERT(szArgv != NULL);
    if(i4Argc < 4)
    {
        printf("vdec.ds [MC request max len (1-100), Disable req connect(1:Disable, 0:En), FIFO(0:16Bank, 1:8bank, 2:4bank)]\n");
        VDEC_GetParam(ES0, DRAM_BANDWITH_SIMU, &u4Param1, &u4Param2, &u4Param3);
        printf("vdec.ds %d %d %d\n", u4Param1, u4Param2, u4Param3);        
        return 0;
    }

    VDEC_SetParam(ES0, DRAM_BANDWITH_SIMU, StrToInt(szArgv[1]), StrToInt(szArgv[2]), StrToInt(szArgv[3]));
    return 0;
}

BOOL _fgAbtCurItem = FALSE;  // just for compilation of WMV emulation code
