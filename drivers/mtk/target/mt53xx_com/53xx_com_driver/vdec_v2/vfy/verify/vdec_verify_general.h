#ifndef _VDEC_VERIFY_GENERAL_H_
#define _VDEC_VERIFY_GENERAL_H_

#include "x_typedef.h"
#include "vdec_verify_keydef.h"
#include "vdec_verify_mm_map.h"
#include "drv_config.h"
#include "chip_ver.h"
#include "x_printf.h"

/***************************************************/
/***************************************************/

/*Add common verify MACRO*/

/***************************************************/
/***************************************************/

//#define IRQ_DISABLE
//#define VDEC_MVC_SUPPORT 0
#define VDEC_DDR3_SUPPORT 0
#define VDEC_SUPPORT_YC_SEPARATE 0

//Vdec Codec Config
//#define VDEC_VIDEOCODEC_RM                            //Define Vdec Verification Flow for RM Decoder

//Vdec Codec - Feature Config 
#ifdef VDEC_VIDEOCODEC_RM
//#define DOWN_SCALE_SUPPORT                      // DownScale  //Must Disable DDR3 Setting
//#define MT8550_DDR3_VDEC_ENABLE             //DDR3     //Disable when use No-wrap, No-NBM FPGA bitfile    //!!!! Enable DDR3, Must Enable ADDRSWAP !!!!
#ifdef  MT8550_DDR3_VDEC_ENABLE
#define MT8550_ADDRSWAP_ENABLE              //Address Swap      //Disable when use No-wrap, No-NBM FPGA bitfile
#endif
//#define RM_RINGVIFO_FLOW                           //Use Ring VFIFO for RM Only

//#define RM_ATSPEED_TEST_ENABLE        //Load All Bitstream to VFIFO, Trigger Decode ASAP
//#define RM_DUMP_CHECKSUM

#define  RM_SUPPORT_NEW_TABLE_MODE //if do not test RM new table mode, Hide it
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8555)
#define RM_CRCCHECKFLOW_SUPPORT   //if CRCcheck ,open it
#define RM_CRCCHECK_ENABLE              //if CRCcheck ,open it
#endif
//#define RM_CRCCHECK_RANDOMCHECK     //Pixel by pixel Check

//#define RM_CRCCHECK_ENABLE_ONEPATH    //Only for Test Flow, Should be Disabled
//#define RM_CRCCHECK_HWOUT_GENERATE
//#define RM_CRCCHECK_TIMER                     //Check Process Time
//#define RM_DISABLE_HWCRCCHECK              //Only for MT8550,8550 don't support crc
#endif //VDEC_VIDEOCODEC_RM

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8555)
#define WMV_CRCCHECK_ENABLE 
#endif

#ifdef  WMV_CRCCHECK_ENABLE
#define WMV_CRC_COMPOSITE_CHECK_ENABLE
#endif

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8555)
#define MPEG4_CRCCHECK_ENABLE 
#endif

#define HEVC_CRC_COMPARE 1
#define MT5890_E3_SUPPORT

#define    PROFILE_TIME_TEST    1
#define    DUAL_VDEC_TEST    0
#define    STILL_IMAGE_TEST 0
//#define     HEVC_VP_MODE
#define     QUALITY_CHCEK   0

#define PROFILE_TIME_TEST 0

#define DRAM_BUSY_TEST 0
//FILE IO Config
//#define VDEC_EMUVER_FILEIO    //Enable This Define When System Don't Support FileIO Function

//#define INTERGRATION_WITH_DEMUX

#define SATA_HDD_READ_SUPPORT
#define SATA_HDD_FS_SUPPORT

#ifndef SATA_HDD_READ_SUPPORT
#ifdef SATA_HDD_FS_SUPPORT
 #undef SATA_HDD_FS_SUPPORT
#endif
#endif

//#define VDEC_UFO_SUPPORT

//#define BARREL2_SUPPORT  
//#define BARREL2_THREAD_SUPPORT

#ifdef BARREL2_THREAD_SUPPORT
#undef BARREL2_SUPPORT
#endif

#define ADDR_SWAP_MODE ADDRSWAP_OFF
#define VDEC_PP_ENABLE  FALSE
#define VDEC_UFO_ENABLE FALSE  //just for mpeg

//#define MEPG2_ERROR_BITSTREAM    //for error bitstream, even if mismatch @ some frame, will decode continue
#define DUAL_VDEC_TEST 0

#define BANDWIDTH_MEASURE FALSE  // speed and bandwidth for mc for python xiaolei.li
#define PARA8_BEHAVIOR_TEST FALSE //MC_PARA8_TEST for remy xiaolei.li
//#define	VPMODE
#define FW_WRITE_QUANTIZATION


#define DIRECT_DEC
#define SW_RESET
#define MEM_PAUSE_SUPPORT
//#define DEBUG_LOG
//#define REDEC
//#define EXT_COMPARE
//#define GEN_HW_CHKSUM
//#define COMP_HW_CHKSUM
//#define DOWN_SCALE_SUPPORT
//#define FGT_SUPPORT
//#define HW_FINDSTARTCODE_SUPPORT

//#define LETTERBOX_SUPPORT
#ifdef LETTERBOX_SUPPORT
#define LETTERBOX_DETECTION_ONLY
#endif

#ifdef DOWN_SCALE_SUPPORT
#define VERIFICATION_DOWN_SCALE
#endif
//#define VERIFICATION_FGT
//#define VDEC_DEBUG_USAGE

#ifdef VDEC_DEBUG_USAGE
//#define NO_COMPARE
#endif

//#define MVC_DEBUG
#define AVC_SEARCH_START_CODE_WR 0//search start code work around
#define AVC_CHKSUM_PRNT 0

#define VDEC_VER_COMPARE_CRC 1
#define VDEC_SUPPORT_MERGE_CRC 0 //Yi Feng added
#define VDEC_DUMP_CRC //longfei wang added for dump CRC to PC

#if VDEC_VER_COMPARE_CRC
#define AVC_NEW_CRC_COMPARE 1
#endif
#define CONFIG_DRV_MEM

//use cli to enable crc mode
//	#define HEVC_CRC_COMPARE 0

//#define AVC_HIGH_RESOLUTION
//#define MVC_VFY_SUPPORT
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890)
//#define AVC_UFO_MODE
//#define AVC_COMPACT_MODE
#define Y_C_SEPERATE
#endif

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561) //14/9/2010 mtk40343
#define fgVDEC_H264_REDUCE_MV_BUFF(fgDirBuffReduce)  (fgDirBuffReduce == 1)
#define VDEC_H264_REDUCE_MV_BUFF 1

#else
#define VDEC_H264_REDUCE_MV_BUFF 0
#endif

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
#ifndef DOWN_SCALE_SUPPORT
#define GOLDEN_128BIT_COMP
#endif
//#define VDEC_MPEG4_SUPPORT_RESYNC_MARKER
#define WMV_EC_IMPROVE_SUPPORT 1
#endif
#define MPV_DUMP_H264_DEC_REG

#define VDEC_VP6_ERR_TEST 0
#define VDEC_VP8_ERR_TEST 0

#define VDEC_WMV_EMU 0
#define VDEC_AVS_EMU 0
#define VDEC_MPEG_EMU 0
#define VDEC_VP8_EMU 0
#define VDEC_VP9_EMU 0
#define VDEC_VP6_EMU 0
//#define VDEC_AVS_ERR_TEST 0
#define VDEC_DRAM_BUSY_TEST 0
//#if VDEC_AVS_EMU
#define AVS_VLD_PRED_SZ ( 1920 * 4 )
#define AVS_VLD_MV_SZ ((1920 * 1080) / 8) //DE suggest use x2 Original MV buffer ((1920 * 1080) / 16)
//#endif
#define PARA8_BEHAVIOR_TEST 0
#define VDEC_VP8_COPY_SEGID_SUPPORT 0

#if (CONFIG_DRV_FPGA_BOARD)
#define IRQ_DISABLE
//#define FPGA_FOR_MPEG2
//#define FPGA_FOR_H264
//#define IDE_READ_SUPPORT
//#define IDE_WRITE_SUPPORT
//#define PCFILE_WRITE

#define RING_VFIFO_SUPPORT
#define DYNAMIC_MEMORY_ALLOCATE
#else
#define RING_VFIFO_SUPPORT
#define DYNAMIC_MEMORY_ALLOCATE
//#define ReDecBitstream
#endif


#define SUPPORT_4K2K_DUAL_CORE

#define VDEC_NORMAL_ST  0
#define VDEC_LAE_ST     1
#define VDEC_CORE0_ST   2
#define VDEC_CORE1_ST   3

///////////////////////////////////
typedef enum
{
    VDEC_UNKNOWN = 0xFF,
    VDEC_MPEG       = 0x0,                 ///< MPEG Deocde Request
    VDEC_MPEG1      = 0x1,               ///< MPEG1 Deocde Request
    VDEC_MPEG2      = 0x2,               ///< MPEG2 Deocde Request
    VDEC_DIVX3       = 0x3,              ///< MPEG3 Deocde Request
    VDEC_MPEG4      = 0x4,              ///< MPEG4 Deocde Request
    VDEC_WMV         = 0x10,            /// < WMV Decode Request
    VDEC_WMV1       = 0x11,             ///< WMV7 Deocde Request
    VDEC_WMV2       = 0x12,            ///< WMV8 Deocde Request
    VDEC_WMV3       = 0x13,            ///< WMV9 Deocde Request
    VDEC_VC1        = 0x111,          ///< VC1 Deocde Request
    VDEC_H265        = 0x265,           ///< H265 Deocde Request
    VDEC_H264        = 0x264,           ///< H264 Deocde Request
    VDEC_H263        = 0x263,           ///< H263 Deocde Request
    VDEC_RM          = 0x300,             ///< RM Decode Request
    VDEC_VP6         = 0x600,             ///< VP6 Decode Request
    VDEC_AVS         = 0x700,             ///< AVS Decode Request
    VDEC_VP8         = 0x800,             ///< VP8 Decode Request
    VDEC_JPG         = 0x900,              ///< JPG Decode Request
    VDEC_WEBP        = 0xA00,
    VDEC_VP9        = 0xB00               ///< VP9 Decode Request
}VDEC_CODEC_T;
//xiaolei.li's
//#define SEMI_RING_FIFO //for large bitstream

//#define LOOP_ONE_BITSTREAM // for mustang wmv random error
#endif
