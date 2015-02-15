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

/*
 * remove memory debugger for lint free
 */
#define __MEMDBG_H

#include "x_hal_5381.h"
#include "x_bim.h"
#include "jpg_common.h"
#include "jpg_if.h"
#include "jpg_hw.h"
#include "jpg_debug.h"
#include "x_hal_926.h"
#include "x_bim.h"

#ifdef JPEG_AUTO_TEST
extern INT32 _JpgVerifyCmdGetParam(INT32 i4ParamType, INT32 Param);
#endif

#define D_VLD_POLL_DELAY			1
#define D_VLD_POLL_TIMEOUT			300


/* vld register clear value, by default, zero */
#define VLD_CLEAR					0

/* barrel shifter base, and bit shift address */
#define VLD_BARREL_SHIFTER_BITS(BITS) ((BITS) << 2)


#if 1	//CC_5368_JPG

/* vld register address/value definition*/
#define VLDREG_RISC_PROCESS		0x8C
	#define VLD_INIT_FETCH_DRAM		(1 << 20)
	#define VLD_INIT_FILL_BS		(1 << 23)

//#define MCREG_SWAP_MODE				0x0090
	//#define MC_SWAP_RASTER			3


#define VLDREG_LOAD_INTRA_Q_MATRIX		0x0A0
#define VLDREG_LOAD_NONINTRA_Q_MATRIX	0x0A4	

#define VLDREG_READ_POINTER		0x0B0
#define VLDREG_START_ADDRESS		0x0B4
#define VLDREG_END_ADDRESS		0x0B8

#define VLDREG_PWRSAVE		0xC4
	#define VLD_PDN_SCLK    						1
	#define VLD_PDN_DCLK       					(1 << 1)
	#define VLD_PDN_IOMMU_DCLK    		(1 << 4)

#ifdef JPEG_VLD_V4
#define RW_VLD_PDNFINE          0xC8
     #define RW_PDN_BASE_CLK_OFF         ((unsigned)0x1 << 0)
     #define RW_PDN_PROG_CLK_OFF         ((unsigned)0x1 << 1)
#endif

#define VLDREG_LATCH2				0x0D0
	#define LATCH2_SET					0x1
	#define LATCH2_RCWP				((unsigned)0x1 << 18)

#define VLDREG_FETCH_FINISH		0xE8
    #define VLD_FETCH_OK            ((unsigned)0x1 << 0)
    #define VLD_DRAM_REQ_FIN        ((unsigned)0x1 << 2)
    #define VLD_INIT_VALID_FLG       ((unsigned)0x1 << 3)
    
#define VLDREG_INPUT_WINDOW     (0xF0)

#define VLDREG_VID_BUF_CTL_SIGNAL   (0x0F4)
#ifdef JPEG_VLD_V4
	#define SBITS_PROC_RDY    1
	#define BBITS_PROC_RDY       (1 << 1)
#elif defined(JPEG_VLD_V3)
	#define VLD_SRAM_AA_FIT_TAR_SCLK    1
	#define VLD_SRAM_AA_FIT_TAR_D       (1 << 1)
#else
	#define VLD_SRAM_AA_FIT_TAR_SCLK    (1 << 16)
	#define VLD_SRAM_AA_FIT_TAR_D       (1 << 17)
#endif	

#define VLDREG_WATCH_WP			0x0F8
#define VLDREG_WATCH_RP			0x0FC
#define VLDREG_VIDEO_RPTR       (0xFC)

#define VLDREG_WAIT_THRES       (0x100)

#define VLDREG_BS_PARSED_FLAG		0x104
	#define VLD_USERDEF_QMATRIX	0x300

#define VLDREG_SOFT_RESET			0x0108
    #define VLD_SW_RESET_CLEAR       0x0
    #define VLD_SW_RESET_RESET       0x1
    #ifdef JPEG_VLD_V4
    #define VLD_LARB_RESET       ((unsigned)0x1 << 4)
    #define VLD_ASYNC_FIFO_RST  ((unsigned)0x1 << 8)
    #endif
    
#define VLDREG_RISC_CTRL_WP		0x0110
    #define VLD_CTRL_PROG_EN    0x1
	#define VLD_RISC_CTRL_WP_EN	0x2
	#define VLD_WP_LATCH_EN		0x4


#define VLDREG_MAXCODE			0x0118

#define VLDREG_DRAM_HUFF			0x011C
	#define VLD_AC_HUFF_FLAG		0x80000000
	
/* q table for each component */
#define VLDREG_Q_TBL_LIST		0x0120

/* huffman related registers */
#define VLDREG_LA_HUFF_INDEX	0x0124
#define VLDREG_LA_HUFF_DATA		0x0128

#define VLDREG_JPGMISC		0x012c

#define VLDREG_DC_TBL_LIST		0x0130
#define VLDREG_AC_TBL_LIST		0x0134

/* component info */
#define VLDREG_BLK_PARAM_LIST	0x0138
	#define VLD_DCNEED_MASK			0x000003FF
	#define VLD_DCNEED_SHIFT		0
	#define VLD_ACNEED_MASK			0x000003FF
	#define VLD_ACNEED_SHIFT		10
	#define VLD_MCUBLK_MASK			0x0000000F
	#define VLD_MCUBLK_SHIFT		24

/* block membership (blongs to which component) */
#define VLDREG_MCU_MEMBERSHIP	0x013C

#define VLDREG_START_DECODE			0x140
	#define VLD_DECODE_MCU			1
	
#define VLDREG_JPEG_SWITCH		0x0144
	#define VLD_MPEG_MODE		0
	#define VLD_JPEG_MODE		1	
	#define VLD_INTMASK         4
	#define VLD_ERRINT_OFF		6		

#define VLDREG_FAST_START_DECODE	0x0148
	#define VLD_CLEAN_INTERNAL_BUFFER		0
	#define VLD_DONT_CLEAN_INTERNAL_BUFFER	1

#define VLDREG_RESTART				0x014C
	#define VLD_RESTART				1
	#define VLD_RESTART_ALL			0x101
	#define VLD_RESTART_WORLD		0x10101
	#define VLD_JSTOR               24
	#define VLD_JSTOP_RDY           25
	
#define VLDREG_PROGRESSIVE_MODE		0x0150
	#define VLD_PROG_NONE		    0
	#define VLD_PROG_DC_REFINE		1
	#define VLD_PROG_AC_FIRST		2
	#define VLD_PROG_AC_REFINE		3
	#define VLD_PROG_DC_FIRST		4
	#define VLD_PROG_MASK			0x7
	#define VLD_PROG_SHIFT			0
	/* progressive mode write out (write to idct if last scan) */
	#define VLD_PROG_COEF_WRITE		0
	#define VLD_PROG_IDCT_WRITE		1
	#define VLD_PROG_WRITE_MASK		1
	#define VLD_PROG_WRITE_SHIFT	3
	/* select baseline or progressive mode */
	#define VLD_DECODE_BASELINE		0
	#define VLD_DECODE_PROGRESSIVE	1
	#define VLD_DECODE_MODE_MASK	1
	#define VLD_DECODE_MODE_SHIFT	8

#define VLDREG_PROGRESSIVE_NONZERO_ONLY		0x0154
	#define VLD_PROG_BUFF_ALL		0
	#define VLD_PROG_BUFF_NZ_ONLY	1

#define VLDREG_PROGRESSIVE_ALAH		0x0158
	#define VLD_PROG_AL_MASK		0xf
	#define VLD_PROG_AL_SHIFT		0
	#define VLD_PROG_AH_MASK		0xf
	#define VLD_PROG_AH_SHIFT		8

#define VLDREG_PROGRESSIVE_SESS		0x015C
	#define VLD_PROG_SE_MASK		0x3f
	#define VLD_PROG_SE_SHIFT		0
	#define VLD_PROG_SS_MASK		0x3f
	#define VLD_PROG_SS_SHIFT		8

#define VLDREG_NONE_ZERO_HISTORY	0x0160

#define VLDREG_CURR_MCU				0x0164
#define VLDREG_EOB_RUN_WR			0x0168
#define VLDREG_EOB_RUN_RD			0x01A0

#define VLDREG_READ_START_COMP(X)	(0x016C + (4 * (X)))

#define VLDREG_WRITE_START_COMP(X)	(0x0178 + (4 * (X)))

#define VLDREG_X_ADDR_LIST			0x0184
#define VLDREG_Y_ADDR_LIST			0x0188

#define VLDREG_COMP_PITCH1			0x018C
#define VLDREG_COMP_PITCH2			0x0190
#define VLDREG_COMP_PITCH3			0x0194
	#define VLD_PITCH_LO_MASK		0x3FFF
	#define VLD_PITCH_LO_SHIFT		0
	#define VLD_PITCH_HI_MASK		0x3FFF
	#define VLD_PITCH_HI_SHIFT		14

#define VLDREG_MCU_STATUS			0x198
	#define VLD_MCU_DECODE_FINISH	1

#define VLDREG_CUR_MCU_NUM  0x1BC
  #define VLD_MCU_ROW       0
	#define VLD_MCU_ROW_MASK 0xFFFF
	#define VLD_MCU_ROW_X     16
	#define VLD_MCU_ROW_X_MASK 0xFFFF
	
#ifdef JPEG_VLD_V4
#define VLDREG_JPG_MCU_COEF_SIZE 0x1C0
#define VLDREG_JPG_BST_OUT_MODE  0x1DC  
	#ifdef JPEG_VLD_V5
	#define JPG_BST_FORMAT_SEL_422            (0x1<<13)
	#define JPG_BST_FORMAT_SEL_422V           (0x4<<13)
	#define JPG_BST_FORMAT_SEL_422VX2           (0x5<<13)		
	#else
    #define JPG_BST_FORMAT_SEL_422           (0x1<<17)
    #endif
    #define JPG_ROW_BST_OUT_ON                  (0x1<<16)
    #define JPG_SWAP_CBCR_SEL                     (0x1<<22)
    #define JPG_VIDEO_FORMAT_PADDING      (0x1<<21)
    #define JPG_VIDEO_FORMAT_OUTPUT        (0x1<<20)    
#endif

#define VLDREG_JPEG_FSM				0x0208
	#define VLD_JPEG_IDLE			0

#define VLDREG_JPEG_BS_DDRAM_IN_CHKSUM 0x224
#define VLDREG_JPEG_INPUT_CHKSUM           0x228
#define VLDREG_JPEG_HUFFDEC_CHKSUM       0x22c
#define VLDREG_JPEG_VLD_OUT_CHKSUM       0x230
#define VLDREG_JPEG_IDCT_OUT_CHKSUM      0x234
#define VLDREG_JPEG_WINTF_CHKSUM           0x238
#define VLDREG_JPEG_COEFF_DRAM_IN_CHKSUM    0x23c
#define VLDREG_JPEG_NZ_IN_CHKSUM            0x240
#define VLDREG_JPEG_NZ_OUT_CHKSUM         0x244

#ifdef JPEG_VLD_V4
#define  VLDREG_IQ_TABLE_INDEX                           0x260
#define  VLDREG_IQ_TABLE_DATA_IN                       0x264
#define	 VLDREG_JPEG_WINTF_CRC           				0x278
#endif

#define VLDREG_DEC_MB_ROW           (0x2A8)
    #define VLD_DEC_FIRST_ROW       0x1
    #define VLD_DEC_NEXT_ROW        0x100

#define VLDREG_MB_ROW_DEC_SWITCH        (0x2AC)
    #define VLD_ROW_DEC_WR_ADDR                 0x100
    #define VLD_ROW_DEC_WR_BANK1_ADDR           0x200
    #define VLD_ROW_DEC_MCU_LEVEL               0x0
    #define VLD_ROW_DEC_MCU_ROW_LEVEL           0x1
    #define VLD_ROW_DEC_PIC_LEVEL               0x3
#define VLDREG_ROW_DEC_COMP0_ADDR       (0x2B0)
#define VLDREG_ROW_DEC_COMP1_ADDR       (0x2B4)
#define VLDREG_ROW_DEC_COMP2_ADDR       (0x2B8)

#ifdef JPEG_VLD_V4
#define VLDREG_JPG_PARTIALMODE  (0x2C0)
	#define JPG_PARTIAL_MODE					(0x1 << 16)
	#define JPG_VERT_START_MB_MASK	0x1fff	
	#define JPG_REV_MCU_ROW_STA					(0x1 << 20)
	#define JPG_REV_MCU_ROW_BUSY				(0x1 << 21)
#endif

//JPEG_VLD_V4 don't contains 0x2bc 0x2c0 0x2c4
#ifdef JPEG_VLD_V4
#define VLDREG_JPG_PIC_PIX_WIDTH  (0x2C4)
#else
#define VLDREG_ROW_DEC_COMP0_ADDR_OFST  (0x2BC)
#define VLDREG_ROW_DEC_COMP1_ADDR_OFST  (0x2C0)
#define VLDREG_ROW_DEC_COMP2_ADDR_OFST  (0x2C4)
#endif

#define VLDREG_JPEG_PIC_SIZE            (0x2C8)
    #define VLD_ROW_DEC_WIDTH_SHIFT     16
    #define VLD_ROW_DEC_HEIGHT_SHIFT    0
    
#define VLDREG_ROW_PIC_DEC_FINISH       (0x2CC)
    #define VLD_ROW_DEC_PIC_FINISH      (0x1 << 8)
    #define VLD_ROW_DEC_MCU_ROW_FINISH  (0x1 << 0)

#define VLDREG_RESTART_INTERVAL     (0x2D0)
    #define VLD_RST_ENABLE          16


#ifdef JPEG_VLD_V4
#define VLDREG_JPEG_LOAD_INTER_DATA_TRIGGER  0x2D4
#define VLDREG_JPEG_LOAD_INTER_DATA1         0x2D8
#define VLDREG_JPEG_LOAD_INTER_DATA2         0x2DC
#define VLDREG_JPEG_LOAD_INTER_DATA3         0x2E0
#define VLDREG_JPEG_LOAD_INTER_DATA4         0x2E4
#define VLDREG_JPEG_LOAD_INTER_DATA5         0x2E8
#define VLDREG_JPEG_SRAM_CLEAR               0x2EC
#define VLDREG_JPEG_COEF_PITCH_0             0x2EC
#define VLDREG_JPEG_COEF_PITCH_1             0x2F0
#define VLDREG_JPEG_COEF_PITCH_2             0x2F4
#else
#define VLDREG_JPEG_SRAM_CLEAR      (0x2EC)
#endif

#define VLDREG_JNEWBASE		        (0x3C4)
	#ifdef JPEG_VLD_V4	
    #define NEW_BASELINE_ENABLE             0x701
    #define NEW_BASELINE_DISABLE            0x701  
    #define NEW_PROGRESSIVE_MULTI_COLLECT   0x701
    #define NEW_PROGRESSIVE_ENHANCE         0x700
	#else
    #define VLD_NEW_JENGINE           0x7FF
    #define VLD_OLD_JENGINE           0x707
	#endif

#ifdef JPEG_VLD_V4
#define VLDREG_JPG_HSRAM_INIT               0x3C8
    #define SRAM_CLOCK_DISABLE                   0x0
    #define SRAM_CLOCK_ENABLE                    0x1
    #define FILL_ENABLE                          (0x1 << 1)
    #define FILL_DISABLE                         (0x0 << 1)
#endif
	
#define VLDREG_JED_EN    		    (0x3CC)
#define VLDREG_JEC_EN               (0x3D0)
    #define VLD_EC_BEC_LEN_EN                        0 // baseline error code length
    #define VLD_EC_GT64_EN                           1
    #define VLD_EC_RSTMAK_EN                         4
#define VLDREG_JFILLH                   (0x3D4)
    #define VLD_HUFF_DC_AC_SELECTION        12
    #define VLD_HUFF_TABLE_IDENTIFIER       8
    #define VLD_HUFF_TABLE_ADDR             0
#define VLDREG_JHVAL                    (0x3D8)
#define VLDREG_JERR_STATUS		    (0x3DC)
	#ifdef JPEG_VLD_V4	
	#define VLD_ERR_PROGRSSIVE_SE  9
	#define VLD_ERR_RSTMKR_EARLY  5
    #define VLD_ERR_RSTMKR	            4			
    #define VLD_ERR_GT64	            1    
    #define VLD_ERR_ACLEN	            0
	#else
    #define VLD_ERR_RSTMKR	            4			
    #define VLD_ERR_DCRR	            3
    #define VLD_ERR_ACSR	            2    
    #define VLD_ERR_GT64	            1    
    #define VLD_ERR_ACLEN	            0
	#endif
#else

/* vld register address/value definition*/
#define VLDREG_RISC_PROCESS		0x8C
	#define VLD_INIT_FETCH_DRAM		(1 << 20)
	#define VLD_INIT_FILL_BS		(1 << 23)

#define VLDREG_PWRSAVE		0xC4

#define VLDREG_FETCH_FINISH		0xE8
	#define VLD_INIT_FETCH_FIN		1

#define VLDREG_BS_PARSED_FLAG		0x104
	#define VLD_USERDEF_QMATRIX	0x300

#define VLDREG_INPUT_WINDOW     (0xF0)

#define VLDREG_VID_BUF_CTL_SIGNAL   (0x0F4)
#ifdef JPEG_VLD_V3
	#define VLD_SRAM_AA_FIT_TAR_SCLK    1
	#define VLD_SRAM_AA_FIT_TAR_D       (1 << 1)
#else
	#define VLD_SRAM_AA_FIT_TAR_SCLK    (1 << 16)
	#define VLD_SRAM_AA_FIT_TAR_D       (1 << 17)
#endif	

#define VLDREG_VIDEO_RPTR       (0xFC)

#define VLDREG_WAIT_THRES       (0x100)

#define VLDREG_MCU_RESTART		0x014C
	#define VLD_MCU_RESTART			0x1
	#define VLD_RES_DC_PRED			0x101
	#define VLD_RES_MKR_FND			0x10101

/* huffman related registers */
#define VLDREG_LA_HUFF_INDEX	0x0124
#define VLDREG_LA_HUFF_DATA		0x0128
#define VLDREG_MAXCODE			0x0118

/* component info */
#define VLDREG_BLK_PARAM_LIST	0x0138
	#define VLD_DCNEED_MASK			0x000003FF
	#define VLD_DCNEED_SHIFT		0
	#define VLD_ACNEED_MASK			0x000003FF
	#define VLD_ACNEED_SHIFT		10
	#define VLD_MCUBLK_MASK			0x0000000F
	#define VLD_MCUBLK_SHIFT		24

/* block membership (blongs to which component) */
#define VLDREG_MCU_MEMBERSHIP	0x013C

#define VLDREG_DC_TBL_LIST		0x0130
#define VLDREG_AC_TBL_LIST		0x0134

/* q table for each component */
#define VLDREG_Q_TBL_LIST		0x0120

#define VLDREG_JPEG_SWITCH		0x0144
	#define VLD_MPEG_MODE		0
	#define VLD_JPEG_MODE		1	
	#define VLD_INTMASK         4
	#define VLD_ERRINT_OFF		6		

#define VLDREG_FAST_START_DECODE	0x0148
	#define VLD_CLEAN_INTERNAL_BUFFER		0
	#define VLD_DONT_CLEAN_INTERNAL_BUFFER	1

#define VLDREG_PROGRESSIVE_MODE		0x0150
	#define VLD_PROG_NONE		    0
	#define VLD_PROG_DC_REFINE		1
	#define VLD_PROG_AC_FIRST		2
	#define VLD_PROG_AC_REFINE		3
	#define VLD_PROG_DC_FIRST		4
	#define VLD_PROG_MASK			0x7
	#define VLD_PROG_SHIFT			0
	/* progressive mode write out (write to idct if last scan) */
	#define VLD_PROG_COEF_WRITE		0
	#define VLD_PROG_IDCT_WRITE		1
	#define VLD_PROG_WRITE_MASK		1
	#define VLD_PROG_WRITE_SHIFT	3
	/* select baseline or progressive mode */
	#define VLD_DECODE_BASELINE		0
	#define VLD_DECODE_PROGRESSIVE	1
	#define VLD_DECODE_MODE_MASK	1
	#define VLD_DECODE_MODE_SHIFT	8

#define VLDREG_PROGRESSIVE_NONZERO_ONLY		0x0154
	#define VLD_PROG_BUFF_ALL		0
	#define VLD_PROG_BUFF_NZ_ONLY	1

#define VLDREG_PROGRESSIVE_ALAH		0x0158
	#define VLD_PROG_AL_MASK		0xf
	#define VLD_PROG_AL_SHIFT		0
	#define VLD_PROG_AH_MASK		0xf
	#define VLD_PROG_AH_SHIFT		8

#define VLDREG_PROGRESSIVE_SESS		0x015C
	#define VLD_PROG_SE_MASK		0x3f
	#define VLD_PROG_SE_SHIFT		0
	#define VLD_PROG_SS_MASK		0x3f
	#define VLD_PROG_SS_SHIFT		8

#define VLDREG_NONE_ZERO_HISTORY	0x0160

#define VLDREG_CURR_MCU				0x0164
#define VLDREG_EOB_RUN_WR			0x0168
#define VLDREG_EOB_RUN_RD			0x01A0

#define VLDREG_READ_START_COMP(X)	(0x016C + (4 * (X)))

#define VLDREG_WRITE_START_COMP(X)	(0x0178 + (4 * (X)))

#define VLDREG_X_ADDR_LIST			0x0184
#define VLDREG_Y_ADDR_LIST			0x0188

#define VLDREG_COMP_PITCH1			0x018C
#define VLDREG_COMP_PITCH2			0x0190
#define VLDREG_COMP_PITCH3			0x0194
	#define VLD_PITCH_LO_MASK		0x3FFF
	#define VLD_PITCH_LO_SHIFT		0
	#define VLD_PITCH_HI_MASK		0x3FFF
	#define VLD_PITCH_HI_SHIFT		14

#define VLDREG_START_DECODE			0x140
	#define VLD_DECODE_MCU			1

#define VLDREG_MCU_STATUS			0x198
	#define VLD_MCU_DECODE_FINISH	1

#define VLDREG_RISC_CTRL_WP		0x0110
    #define VLD_CTRL_PROG_EN    0x1
	#define VLD_RISC_CTRL_WP_EN	0x2
	#define VLD_WP_LATCH_EN		0x4
#define VLDREG_READ_POINTER		0x0B0
#define VLDREG_START_ADDRESS		0x0B4
#define VLDREG_END_ADDRESS		0x0B8

#define VLDREG_LATCH2				0x0D0
	#define LATCH2_SET				0x1
	
#define VLDREG_WATCH_RP			0x0FC
#define VLDREG_WATCH_WP			0x0F8

#define VLDREG_LOAD_INTRA_Q_MATRIX		0x0A0
#define VLDREG_LOAD_NONINTRA_Q_MATRIX	0x0A4

#define VLDREG_SOFT_RESET			0x0108
	#define VLD_RESET				1

#define VLDREG_DRAM_HUFF			0x011C
	#define VLD_AC_HUFF_FLAG		0x80000000

#define VLDREG_RESTART				0x014C
	#define VLD_RESTART				1
	#define VLD_RESTART_ALL			0x101
	#define VLD_RESTART_WORLD		0x10101
	#define VLD_JSTOR               24
	#define VLD_JSTOP_RDY           25

#define VLDREG_JPEG_FSM				0x0208
	#define VLD_JPEG_IDLE			0

#define VLDREG_DEC_MB_ROW           (0x2A8)
    #define VLD_DEC_FIRST_ROW       0x1
    #define VLD_DEC_NEXT_ROW        0x100

#define VLDREG_MB_ROW_DEC_SWITCH        (0x2AC)
    #define VLD_ROW_DEC_WR_ADDR                 0x100
    #define VLD_ROW_DEC_WR_BANK1_ADDR           0x200
    #define VLD_ROW_DEC_MCU_LEVEL               0x0
    #define VLD_ROW_DEC_MCU_ROW_LEVEL           0x1
    #define VLD_ROW_DEC_PIC_LEVEL               0x3
#define VLDREG_ROW_DEC_COMP0_ADDR       (0x2B0)
#define VLDREG_ROW_DEC_COMP1_ADDR       (0x2B4)
#define VLDREG_ROW_DEC_COMP2_ADDR       (0x2B8)
#define VLDREG_ROW_DEC_COMP0_ADDR_OFST  (0x2BC)
#define VLDREG_ROW_DEC_COMP1_ADDR_OFST  (0x2C0)
#define VLDREG_ROW_DEC_COMP2_ADDR_OFST  (0x2C4)

#define VLDREG_JPEG_PIC_SIZE            (0x2C8)
    #define VLD_ROW_DEC_WIDTH_SHIFT     16
    #define VLD_ROW_DEC_HEIGHT_SHIFT    0
    
#define VLDREG_ROW_PIC_DEC_FINISH       (0x2CC)
    #define VLD_ROW_DEC_PIC_FINISH      (0x1 << 8)
    #define VLD_ROW_DEC_MCU_ROW_FINISH  (0x1 << 0)

#define VLDREG_RESTART_INTERVAL     (0x2D0)
    #define VLD_RST_ENABLE          16

#define VLDREG_JPEG_SRAM_CLEAR      (0x2EC)

#define MCREG_SWAP_MODE				0x0090
	#define MC_SWAP_RASTER			3
#define MCREG_DRAM_TYPE				0x01C8
	#define MC_DRAM_RASTER			3

#define MCREG_STATUS				0x0894

#define VLDREG_JNEWBASE		        (0x3C4)
    #define VLD_NEW_JENGINE           0x7FF
    #define VLD_OLD_JENGINE           0x707

#define VLDREG_JFILLH                   (0x3D4)
    #define VLD_HUFF_DC_AC_SELECTION        12
    #define VLD_HUFF_TABLE_IDENTIFIER       8
    #define VLD_HUFF_TABLE_ADDR             0
#define VLDREG_JHVAL                    (0x3D8)

#define VLDREG_JED_EN    		    (0x3CC)
#define VLDREG_JEC_EN               (0x3D0)
#define VLDREG_JERR_STATUS		    (0x3DC)
    #define VLD_ERR_RSTMKR	            4			
    #define VLD_ERR_DCRR	            3
    #define VLD_ERR_ACSR	            2    
    #define VLD_ERR_GT64	            1    
    #define VLD_ERR_ACLEN	            0

#endif	//CC_5368_JPG



#ifdef JPEG_VLD_V4
#define JPEG_MMU_ADDR	JPEG_MMU_BASE
/// Read 32 bits data from GFX MMU HW registers.
#define JPEG_MMU_READ32(offset)           IO_READ32(JPEG_MMU_ADDR, (offset))

/// Write 32 bits data into GFX MMU HW registers.
#define JPEG_MMU_WRITE32(offset, value)   IO_WRITE32(JPEG_MMU_ADDR, (offset), (value))

#define JPEG_REG_RW_IOMMU_CFG0		0x0
#define JPEG_REG_RW_IOMMU_CFG1		0x4
#define JPEG_REG_RW_IOMMU_CFG2		0x8
#define JPEG_REG_RW_IOMMU_CFG3		0xc
#define JPEG_REG_RW_IOMMU_CFG4		0x10
#define JPEG_REG_RW_IOMMU_CFG9		0x24
#define JPEG_REG_RW_IOMMU_CFGA		0x28
#define JPEG_REG_RW_IOMMU_CFGB		0x2c
#define JPEG_REG_RW_IOMMU_CFGC		0x30
#define JPEG_REG_RW_IOMMU_CFGD		0x34
#endif



#define BIG_LOOP 5000

#ifdef JPEG_VLD_V3
#ifdef VLD0_BASE
#undef VLD0_BASE
#define VLD0_BASE   JPG_VLD_BASE
#endif
#endif

#if !defined(D_JPEG_REG_DEBUG)
	#ifdef JPEG_VLD_V4		//no MC related registers
	#define VLD0_READ32(offset)				IO_READ32	(VLD0_BASE,	(offset))
	#define MC0_READ32(offset)				//IO_READ32	(MC0_BASE,	(offset))
	#define VLD0_WRITE32(offset, value)		IO_WRITE32	(VLD0_BASE,	(offset), (value))
	#define MC0_WRITE32(offset, value)		//IO_WRITE32	(MC0_BASE,	(offset), (value))
	#else
	#define VLD0_READ32(offset)				IO_READ32	(VLD0_BASE,	(offset))
	#define MC0_READ32(offset)				IO_READ32	(MC0_BASE,	(offset))
	#define VLD0_WRITE32(offset, value)		IO_WRITE32	(VLD0_BASE,	(offset), (value))
	#define MC0_WRITE32(offset, value)		IO_WRITE32	(MC0_BASE,	(offset), (value))
	#endif
#else
	#ifdef JPEG_VLD_V4		//no MC related registers
	/*
	static __inline UINT32 VLD0_READ32(UINT32 u4Offset)
	{
		UINT32 u4Reg;
		u4Reg = IO_READ32(VLD0_BASE, (u4Offset));
		Printf("  - 0x%08x = VLD0_READ32(0x%03x)\n", u4Reg, (u4Offset));
//		Printf("r 0x%08x 1\n", (VLD0_BASE + (u4Offset - 0x400)));
		return u4Reg;
	}

	static __inline void VLD0_WRITE32(UINT32 u4Offset, UINT32 u4Value)
	{
		Printf("  - VLD0_WRITE32(0x%03x, 0x%08x)\n", (u4Offset), u4Value);
		IO_WRITE32(VLD0_BASE, (u4Offset), u4Value);
//		Printf("w 0x%08x 0x%08x\n", (VLD0_BASE + (u4Offset - 0x400)), u4Value);
	}
*/
    static __inline UINT32 VLD0_READ32(UINT32 u4Offset)
    {
        UINT32 u4Reg;
        u4Reg = IO_READ32(VLD0_BASE, (u4Offset));
        //Printf("  - 0x%08x = VLD0_READ32(4*%d)\n", u4Reg, (u4Offset/4));
//      Printf("r 0x%08x 1\n", (VLD0_BASE + (u4Offset - 0x400)));
        return u4Reg;
    }

    static __inline void VLD0_WRITE32(UINT32 u4Offset, UINT32 u4Value)
    {
        //Printf("  - VLD0_WRITE32(4*%d, 0x%x)\n", (u4Offset/4), u4Value);
        IO_WRITE32(VLD0_BASE, (u4Offset), u4Value);
//      Printf("w 0x%08x 0x%08x\n", (VLD0_BASE + (u4Offset - 0x400)), u4Value);
    }

	#else
	static __inline UINT32 VLD0_READ32(UINT32 u4Offset)
	{
		UINT32 u4Reg;
		u4Reg = IO_READ32(VLD0_BASE, (u4Offset));
		Printf("  - 0x%08x = VLD0_READ32(0x%03x)\n", u4Reg, (u4Offset));
//		Printf("r 0x%08x 1\n", (VLD0_BASE + (u4Offset - 0x400)));
		return u4Reg;
	}
	/*
	static __inline UINT32 MC0_READ32(UINT32 u4Offset)
	{
		UINT32 u4Reg;
		u4Reg = IO_READ32(MC0_BASE,	u4Offset);
		Printf("  - 0x%08x = MC0_READ32(0x%08x)\n", u4Reg, u4Offset);
		return u4Reg;
	}*/
	
	static __inline void VLD0_WRITE32(UINT32 u4Offset, UINT32 u4Value)
	{
		Printf("  - VLD0_WRITE32(0x%03x, 0x%08x)\n", (u4Offset), u4Value);
		IO_WRITE32(VLD0_BASE, (u4Offset), u4Value);
//		Printf("w 0x%08x 0x%08x\n", (VLD0_BASE + (u4Offset - 0x400)), u4Value);
	}
	/*
	static __inline void MC0_WRITE32(UINT32 u4Offset, UINT32 u4Value)
	{
		Printf("  - MC0_WRITE32(0x%8x, 0x%08x)\n", u4Offset, u4Value);
		IO_WRITE32(MC0_BASE, u4Offset, u4Value);
	}*/
	#endif
#endif

#if defined(D_JPEG_HWFUNC_DEBUG)
	#define TRACE(MSG) Printf(MSG)
#else //D_JPEG_HWFUNC_DEBUG
	#define TRACE(MSG)
#endif //D_JPEG_HWFUNC_DEBUG

#if defined(D_JPEG_REG_DUMP)
	void _jpg_DumpReg(void);
	#define DUMP_REG() _jpg_DumpReg()
#else
	#define DUMP_REG() \
	/* a safe do while (0)  */ \
	LINT_SUPPRESS_NEXT_STATEMENT(717) \
	do {} while (0)
#endif //D_JPEG_REG_DUMP


/***********************************************************
* static variable
***********************************************************/
static INT32 _i4JpgCurDecMode5391 = (INT32)E_JPG_DEC_MODE_BASELINE_MCU;
static BOOL _fgJpgHwDetectError = FALSE;

/***********************************************************
* local function prototypes
***********************************************************/
void vHwJpgPrintRegister(void)
{
	UINT32 u4Base = 0xB0;
	UINT32 u4Temp = 0;
	printf("Enter %s!\n", __FUNCTION__);
	for (u4Temp = 0; u4Temp < 252; u4Temp ++)
	{
		if ((u4Temp % 4) == 0)
		{
			printf("[0x%8x]",0xF0014000+u4Base + u4Temp * 4);
		}
		printf("0x%8x	 ",IO_READ32(VLD0_BASE,(u4Base + u4Temp * 4)));
		if ((u4Temp % 4) == 3)
		{
			printf("\n");
		}
	}
#ifdef JDEC_IO_MMU_TEST
	printf("[0xFD05F0C0]0x%x , 0x%x, 0x%x, 0x%x \n", IOMMU_READ32(0x0, IOMMU_JPG),IOMMU_READ32(0x4, IOMMU_JPG),IOMMU_READ32(0x8, IOMMU_JPG),IOMMU_READ32(0xC, IOMMU_JPG));
	printf("[0xFD05F0D0]0x%x , 0x%x, 0x%x, 0x%x \n", IOMMU_READ32(0x10, IOMMU_JPG),IOMMU_READ32(0x14, IOMMU_JPG),IOMMU_READ32(0x18, IOMMU_JPG),IOMMU_READ32(0x1C, IOMMU_JPG));
	printf("[0xFD05F0E0]0x%x , 0x%x, 0x%x, 0x%x \n", IOMMU_READ32(0x20, IOMMU_JPG),IOMMU_READ32(0x24, IOMMU_JPG),IOMMU_READ32(0x28, IOMMU_JPG),IOMMU_READ32(0x2C, IOMMU_JPG));
	printf("[0xFD05F0F0]0x%x , 0x%x, 0x%x, 0x%x \n", IOMMU_READ32(0x30, IOMMU_JPG),IOMMU_READ32(0x34, IOMMU_JPG),IOMMU_READ32(0x38, IOMMU_JPG),IOMMU_READ32(0x3C, IOMMU_JPG));
#endif

	}

static INT32 _PollWaitFetchFinish(void);
static HANDLE_T _hIsrSeam=NULL_HANDLE;

/***********************************************************
* implementation
***********************************************************/
static INT32 _PollWaitFetchFinish(void)
{
	UINT32 u4TimeAccu = 0;
	//UINT32 u4TimeAccu2 = 0;

	TRACE(("_PollWaitFetchFinish()\n"));
	
	#ifdef JPEG_VLD_V4
		//while (! (VLD0_READ32(VLDREG_FETCH_FINISH) & VLD_FETCH_OK));

		while (! (VLD0_READ32(VLDREG_FETCH_FINISH) & VLD_FETCH_OK))
		{
			x_thread_delay(D_VLD_POLL_DELAY);
			u4TimeAccu += D_VLD_POLL_DELAY;
			if (u4TimeAccu >= D_VLD_POLL_TIMEOUT)
			{
				return -(INT32)E_HWJPG_ERR_TIMEOUT;
			}
		}
		
	#else
		while (! (VLD0_READ32(VLDREG_FETCH_FINISH) & VLD_FETCH_OK));
		while ((! (VLD0_READ32(VLDREG_VID_BUF_CTL_SIGNAL) & VLD_SRAM_AA_FIT_TAR_SCLK)) && 
		       (! (VLD0_READ32(VLDREG_VID_BUF_CTL_SIGNAL) & VLD_SRAM_AA_FIT_TAR_D)));	
	#endif
/*
	while (! (VLD0_READ32(VLDREG_FETCH_FINISH) & VLD_FETCH_OK))
	{
		x_thread_delay(D_VLD_POLL_DELAY);
		u4TimeAccu += D_VLD_POLL_DELAY;
		if (u4TimeAccu >= D_VLD_POLL_TIMEOUT)
		{
			return -(INT32)E_HWJPG_ERR_TIMEOUT;
		}
	}

    // temp check -- to be deleted
	while ((! (VLD0_READ32(VLDREG_VID_BUF_CTL_SIGNAL) & VLD_SRAM_AA_FIT_TAR_SCLK)) && 
	       (! (VLD0_READ32(VLDREG_VID_BUF_CTL_SIGNAL) & VLD_SRAM_AA_FIT_TAR_D)))
	{
		x_thread_delay(D_VLD_POLL_DELAY);
		u4TimeAccu2 += D_VLD_POLL_DELAY;
		if (u4TimeAccu2 >= D_VLD_POLL_TIMEOUT)
		{
			return -(INT32)E_HWJPG_ERR_TIMEOUT;
		}
	}
*/	
	return (INT32)E_HWJPG_OK;
}

UINT8 VLDJPG_BarrelShifterPeekByte(UINT8 offset)
{
    UINT8 b = 0;
    ASSERT(offset <= 3);
    b = (UINT8)((VLD0_READ32(VLDREG_INPUT_WINDOW) >> (24-offset*8)) & 0xFF);
	//Printf("VLD peek window=0x%x offset=0x%x, return 0x%x\n", VLD0_READ32(VLDREG_INPUT_WINDOW), offset, b);
    return b;
}

INT32 VLDJPG_BarrelShifterSkipBytes(UINT32 u4Bytes)
{
	UINT32 u4Dummy = 0;

	TRACE(("VLDJPG_BarrelShifterSkipBytes(%u)\n", u4Bytes));

	if (u4Bytes)
	{
		while (u4Bytes >= 4)
		{
			u4Dummy = VLD0_READ32(VLD_BARREL_SHIFTER_BITS(32));
			u4Bytes -= 4;
		}

		if (u4Bytes)
		{
			u4Dummy = VLD0_READ32(VLD_BARREL_SHIFTER_BITS(u4Bytes << 3));
		}
	}

	UNUSED(u4Dummy);
	return (INT32)E_HWJPG_OK;
}

UINT8 VLDJPG_BarrelShifterReadBytes(UINT32 offset)
{
	UINT32 reg = VLD0_READ32(VLD_BARREL_SHIFTER_BITS(8));    
	//Printf("VLD window=0x%x read=0x%x offset=%d\n",VLD0_READ32(VLDREG_INPUT_WINDOW), reg, offset);
	return (UINT8)(reg&0xFF);
}


INT32 VLDJPG_SetRingBuffer(void *pvStartAddr, UINT32 u4ByteSize)
{
	UINT32 u4StartAddr;
	u4StartAddr = PHYSICAL((UINT32)pvStartAddr);
	if ((u4StartAddr & 0x3f) != 0)
	{
		return -(INT32)E_HWJPG_ERR_PARAM;
	}
	if ((u4ByteSize & 0x3f) != 0)
	{
		return -(INT32)E_HWJPG_ERR_PARAM;
	}

	VLD0_WRITE32(VLDREG_START_ADDRESS, (UINT32)(u4StartAddr >> 6));
	VLD0_WRITE32(VLDREG_END_ADDRESS, (UINT32)((u4StartAddr + u4ByteSize) >> 6));

	return (INT32)E_JPG_OK;
}

void VLDJPG_SetPowerOn(void)
{
	#ifdef JPEG_VLD_V4
	VLD0_WRITE32(VLDREG_PWRSAVE, (UINT32)~(VLD_PDN_SCLK+VLD_PDN_DCLK+VLD_PDN_IOMMU_DCLK));
	#else
	VLD0_WRITE32(VLDREG_PWRSAVE, (UINT32)~(VLD_PDN_SCLK+VLD_PDN_DCLK));
	#endif
}

void VLDJPG_SetPowerOff(void)
{
	#ifdef JPEG_VLD_V4
	VLD0_WRITE32(VLDREG_PWRSAVE, (UINT32)(VLD_PDN_SCLK+VLD_PDN_DCLK+VLD_PDN_IOMMU_DCLK));	
	#else
	VLD0_WRITE32(VLDREG_PWRSAVE, (UINT32)(VLD_PDN_SCLK+VLD_PDN_DCLK));
	#endif
}

void VLDJPG_SetLinearBuffer(void)
{
#ifdef JPEG_AUTO_TEST
        UINT32 u4StartAddr,pTestBuffer;
        pTestBuffer = _JpgVerifyCmdGetParam(3,0);
        u4StartAddr = PHYSICAL(pTestBuffer);
        VLD0_WRITE32(VLDREG_START_ADDRESS, (UINT32)(u4StartAddr >> 6));
        VLD0_WRITE32(VLDREG_END_ADDRESS, (UINT32)((u4StartAddr + 1024*1024*10)>> 6));
        VLD0_WRITE32(VLDREG_RISC_CTRL_WP,(UINT32)~0);
#else
        VLD0_WRITE32(VLDREG_START_ADDRESS, (UINT32)0);
        VLD0_WRITE32(VLDREG_END_ADDRESS, (UINT32)~0);
#endif
}

INT32 VLDJPG_SetBarrelShifter(UINT8 *pu1MemAddr)
{
	UINT32 u4NonCacheAddr;
	UINT32 u4NonAlignByteCount;

	TRACE(("VLDJPG_SetBarrelShifter(0x%08x)\n", pu1MemAddr));
    
	u4NonCacheAddr = PHYSICAL((UINT32)pu1MemAddr);

	#ifdef JPEG_VLD_V4
	if((VLD0_READ32(VLDREG_FETCH_FINISH) & VLD_INIT_VALID_FLG))
    	while (!(VLD0_READ32(VLDREG_VID_BUF_CTL_SIGNAL) & 1));
	#endif

	VLD0_WRITE32(VLDREG_READ_POINTER, u4NonCacheAddr);

//Reset async fifo
	#ifdef JPEG_VLD_V4
    VLD0_WRITE32(VLDREG_SOFT_RESET,VLD_ASYNC_FIFO_RST);
    VLD0_WRITE32(VLDREG_SOFT_RESET,VLD_SW_RESET_CLEAR);
	#endif
	
	VLD0_WRITE32(VLDREG_RISC_PROCESS, (UINT32)VLD_INIT_FETCH_DRAM);
	
	if ((INT32)E_HWJPG_OK != _PollWaitFetchFinish())
	{
		return -(INT32)E_HWJPG_ERR_SET_BS;
	}
	VLD0_WRITE32(VLDREG_RISC_PROCESS, (UINT32)VLD_INIT_FILL_BS);

	/* shift barrel shifter to non-128bit align address */
	u4NonAlignByteCount = (UINT32)pu1MemAddr & ALIGN_MASK(128);

	return VLDJPG_BarrelShifterSkipBytes(u4NonAlignByteCount);
}


void VLDJPG_SetRiscCtrlWP(UINT32 u4VldWptr)
{
	UINT32 u4RegVal;
//#ifdef CC_MT5387
#ifdef JPEG_VLD_V2
    u4RegVal = (u4VldWptr << 3) | (VLD_CTRL_PROG_EN);
#else
	u4RegVal = (u4VldWptr << 4) | (VLD_RISC_CTRL_WP_EN | VLD_WP_LATCH_EN);
#endif

#ifdef JPEG_VLD_V3
    VLD0_WRITE32(VLDREG_RISC_CTRL_WP, u4VldWptr);
    UNUSED(u4RegVal);
#else
	VLD0_WRITE32(VLDREG_RISC_CTRL_WP, u4RegVal);
#endif
}

INT32 VLDJPG_ReadRiscCtrlWP(void)
{
	INT32 u4RegVal;

	u4RegVal = VLD0_READ32(VLDREG_RISC_CTRL_WP);

	return u4RegVal;

}
#ifndef JDEC_QTABLE_RISC_MODE
BOOL VLDJPGFillQTBLRdy(void)
{
    if(VLD0_READ32(VLDREG_JPGMISC) & 0x1)
        return TRUE;

    return FALSE;
}
#endif
INT32 VLDJPG_LoadTblQ(UINT8 u1Qn, UINT8 *pu1Qtbl)
{
    #ifdef  JDEC_QTABLE_RISC_MODE
    INT32 i4Index;
    UINT32 *rQtable0Addr;
    UINT32 *rQtable1Addr;
    #endif
    
    UINT32 u4Dummy;

	TRACE(("VLDJPG_LoadTblQ(%u, 0x%08x)\n", u1Qn, pu1Qtbl));

	if (u1Qn > 1)
	{
		// vld hardware currently supports only 2 quantization table
		// 5351A1 (351AB) 2005/1/16
		return -(INT32)E_HWJPG_ERR_QTBL_INDEX;
	}
#ifndef  JDEC_QTABLE_RISC_MODE
	VLDJPG_SetLinearBuffer();
#endif

    #ifdef JPEG_VLD_TO_PARSE_HEADER
    #else   /*JPEG_VLD_TO_PARSE_HEADER*/
    if((INT32)E_HWJPG_OK != VLDJPG_SetBarrelShifter(pu1Qtbl))
    {
        return -(INT32)E_HWJPG_ERR_LOAD_Q;
    }
    #endif  /*JPEG_VLD_TO_PARSE_HEADER*/


	if (u1Qn== 0)
	{

		#ifdef  JDEC_QTABLE_RISC_MODE
		       rQtable0Addr = (UINT32 *)(pu1Qtbl);

		       VLD0_WRITE32(VLDREG_IQ_TABLE_INDEX,0);
		     	for (i4Index = 0; i4Index < 16; i4Index++)
		        {  
		            VLD0_WRITE32(VLDREG_IQ_TABLE_DATA_IN,(UINT32)*(rQtable0Addr + i4Index));
		        }
		#else	
				u4Dummy = VLD0_READ32(VLDREG_LOAD_INTRA_Q_MATRIX);
		#endif
	}
	else
	{
		#ifdef JDEC_QTABLE_RISC_MODE
		       rQtable1Addr = (UINT32 *)(pu1Qtbl);

		       VLD0_WRITE32(VLDREG_IQ_TABLE_INDEX,0x40);
		     	for (i4Index = 0; i4Index < 16; i4Index++)
		        {  
		            VLD0_WRITE32(VLDREG_IQ_TABLE_DATA_IN,(UINT32)*(rQtable1Addr + i4Index));
		        }
		#else	
				u4Dummy = VLD0_READ32(VLDREG_LOAD_NONINTRA_Q_MATRIX);
		#endif
	}

	#ifdef JPEG_VLD_V4
        #ifndef JDEC_QTABLE_RISC_MODE
		while(TRUE)
		{
			if(VLDJPGFillQTBLRdy())
				break;
		}
        #endif
	#endif


	UNUSED(u4Dummy);

	return (INT32)E_HWJPG_OK;
}


void VLDJPG_SetUserDefQMatrix(void)
{
	VLD0_WRITE32(VLDREG_BS_PARSED_FLAG, VLD_USERDEF_QMATRIX);
}

#ifndef JPEG_VLD_V4
INT32 VLDJPG_ResetDcPrediction(void)
{
	TRACE(("VLDJPG_ResetDcPrediction()\n"));
	VLD0_WRITE32(VLDREG_RESTART, VLD_CLEAR);
	VLD0_WRITE32(VLDREG_RESTART, VLD_RESTART_ALL);
	VLD0_WRITE32(VLDREG_RESTART, VLD_CLEAR);
	return (INT32)E_HWJPG_OK;
}
#endif

INT32 VLDJPG_CalcDerivedHuffTable(const UINT8 *au1Bits, const UINT8 *au1HuffVal, DERIVED_HUFF_TABLE_T *prDerivedHuffTbl)
{
	//int numsymbols;
	INT32	i4BitLen, i4Symbol, i4LookBits, i4Ctr, i4Size, i4Loop1, i4Loop2;
	INT16	i2Bits;

	INT8	ai1HuffSize[257];
	UINT16	au2HuffCode[257];
	UINT16	u2Code;

	ASSERT(au1Bits != NULL);
	ASSERT(au1HuffVal != NULL);
	ASSERT(prDerivedHuffTbl != NULL);
	TRACE(("VLDJPG_CalcDerivedHuffTable(bits(0x%08x),val(0x%08x),dhufftbl(0x%08x))\n",au1Bits,au1HuffVal,prDerivedHuffTbl));

	/* Note that huffsize[] and huffcode[] are filled in code-length order,
	 * paralleling the order of the symbols themselves in htbl->huffval[].
	 */

	//x_memset(ai1HuffSize, 0, sizeof(ai1HuffSize));
	//x_memset(au2HuffCode, 0, sizeof(au2HuffCode));

	for (i4Loop1 = 0; i4Loop1 < 257; i4Loop1++)
	{
		ai1HuffSize[i4Loop1]=0;
		au2HuffCode[i4Loop1]=0;
	}	
	/* Figure C.1: make table of Huffman code length for each symbol */

	i4Symbol = 0;
	for (i4BitLen = 1; i4BitLen <= 16; i4BitLen++)
	{
		i2Bits = (INT16)au1Bits[i4BitLen];

		if ((i4Symbol + i2Bits) > 256) /* protect against table overrun */
		{
			// Error handler:
			// set to nearest value
			i2Bits = 255 - i4Symbol;	/* p + i = 255 */
		}

		while (i2Bits--)  // safe while
		{
			ai1HuffSize[i4Symbol++] = (INT8) i4BitLen;
		}
	}

	ai1HuffSize[i4Symbol] = 0;
	//numsymbols = p;

	/* Figure C.2: generate the codes themselves */
	/* We also validate that the counts represent a legal Huffman code tree. */

	i4Symbol	= 0;
	u2Code		= 0;
	i4Size		= ai1HuffSize[0];


	for (i4Loop1 = 0; i4Loop1 < BIG_LOOP; i4Loop1++)
	{
	    if (i4Symbol > 256)
	    {
	        i4Symbol = 256;
	    }
		if (ai1HuffSize[i4Symbol])
		{
			for (i4Loop2 = 0; i4Loop2 < BIG_LOOP; i4Loop2++)
			{
				if (((INT32) ai1HuffSize[i4Symbol]) == i4Size)
				{
					au2HuffCode[i4Symbol++] = u2Code;
					u2Code++;
				}
				else
				{
					break;
				}
			}
			/* code is now 1 more than the last code used for codelength si; but
			* it must still fit in si bits, since no code is allowed to be all ones.
			*/

			if (((INT32) u2Code) >= (((INT32) 1) << i4Size))
			{
				// Error handler:
				// set to 0
				u2Code = 0;
			}

			u2Code <<= 1;
			i4Size++;
		}// huffsize[9]
		else
		{
			break;
		}
	}// BIG_LOOP

	/* Figure F.15: generate decoding tables for bit-sequential decoding */

	i4Symbol = 0;
	for (i4BitLen = 1; i4BitLen <= 16; i4BitLen++)
	{
		if (au1Bits[i4BitLen])
		{
			/* valoffset[l] = huffval[] index of 1st symbol of code length l,
			 * minus the minimum code of length l
			 */
			prDerivedHuffTbl->ai4Valoffset[i4BitLen] =
				(INT32)i4Symbol - (INT32)au2HuffCode[i4Symbol];
			i4Symbol += au1Bits[i4BitLen];
			if (!i4Symbol)
			{
			    i4Symbol = 1;
			    LOG(0, "Huffmantable warning");
			}
			prDerivedHuffTbl->ai4Maxcode[i4BitLen] =
				(INT32)au2HuffCode[i4Symbol - 1];		/* maximum code of length l */
		}
		else
		{
			prDerivedHuffTbl->ai4Valoffset[i4BitLen] = 0;
				/* clear to zero if no codes of this length */
			prDerivedHuffTbl->ai4Maxcode[i4BitLen] = -1;
				/* -1 if no codes of this length */
		}
	}
	prDerivedHuffTbl->ai4Maxcode[17] = (INT32)0xFFFFF;
		/* ensures jpeg_huff_decode terminates */

	/* Compute lookahead tables to speed up decoding.
	 * First we set all the table entries to 0, indicating "too long";
	 * then we iterate through the Huffman codes that are short enough and
	 * fill in all the entries that correspond to bit sequences starting
	 * with that code.
	 */

	for (i4Loop1 = 0; i4Loop1 < (1 << D_HUFF_LOOKAHEAD_BITS); i4Loop1++)
	{
		prDerivedHuffTbl->ai4LookNbits[i4Loop1] = 0;
	}

	i4Symbol = 0;
	for (i4BitLen = 1; i4BitLen <= D_HUFF_LOOKAHEAD_BITS; i4BitLen++)
	{
		for (i2Bits = 1; i2Bits <= (INT16) au1Bits[i4BitLen]; i2Bits++, i4Symbol++)
		{
			/* i4BitLen = current code's length, i4Symbol = its index in huffcode[] & huffval[]. */
			/* Generate left-justified code followed by all possible bit sequences */
			i4LookBits = (INT32)au2HuffCode[i4Symbol] << (D_HUFF_LOOKAHEAD_BITS - i4BitLen);
			for (i4Ctr = 1 << (D_HUFF_LOOKAHEAD_BITS - i4BitLen); i4Ctr > 0; i4Ctr--)
			{
				prDerivedHuffTbl->ai4LookNbits[i4LookBits] = i4BitLen;
				prDerivedHuffTbl->au1LookSym[i4LookBits] = au1HuffVal[i4Symbol];
				i4LookBits++;
			}
		}
	}

	return (INT32)E_HWJPG_OK;
}
#ifndef JPEG_VLD_V4
// load ac look ahead table
INT32 VLDJPG_LoadAcLookAheadTable(UINT8 u1HuffTblNo, const DERIVED_HUFF_TABLE_T *prDerivedHuffTbl)
{
	INT32 i4LookAhead;

	ASSERT(prDerivedHuffTbl != NULL);
	
	TRACE(("VLDJPG_LoadAcLookAheadTable(%u, 0x%08x)\n", u1HuffTblNo, prDerivedHuffTbl));

	VLD0_WRITE32(VLDREG_LA_HUFF_INDEX, (UINT32) u1HuffTblNo);

	for (i4LookAhead = 0; i4LookAhead < 64; i4LookAhead++)
	{
		VLD0_WRITE32(VLDREG_LA_HUFF_DATA,
			(((UINT32)prDerivedHuffTbl->ai4LookNbits[i4LookAhead] << 8) |
			(UINT32)prDerivedHuffTbl->au1LookSym[i4LookAhead]));
	}

	VERIFY(VLDJPG_ResetDcPrediction() == (INT32)E_HWJPG_OK);

	return (INT32)E_HWJPG_OK;
}
#endif

void VLDJPG_SetMaxcodeSequence(INT32 u4Value)
{
	VLD0_WRITE32(VLDREG_LA_HUFF_INDEX, u4Value);
}

// maxcode and valoffset
INT32 VLDJPG_LoadMaxcode(const INT32 *ai4Value)
{
	// decoder ap should load maxcode and valoffset follow this procedure

	INT32 i4Index;

	ASSERT(ai4Value != NULL);
	
	TRACE(("VLDJPG_LoadMaxcode(1..16)\n"));

	for (i4Index = 1; i4Index <= 16; i4Index++)
	{
		VLD0_WRITE32(VLDREG_MAXCODE, (UINT32)(ai4Value[i4Index]));
	}
	return (INT32)E_HWJPG_OK;
}

void VLDJPG_TransHuffTblToHwFormat(UINT8 *pu1DstAddr, const UINT8 *au1Huffval)
{
	UINT8 *pu1Huff;
	INT32 i4Index;

	ASSERT(au1Huffval != NULL);
	ASSERT(pu1DstAddr != NULL);

    TRACE(("VLDJPG_TransHuffTblToHwFormat(Dst:0x%08x, Huff:0x%08x)\n", pu1DstAddr, au1Huffval));

	pu1Huff = pu1DstAddr;

	for (i4Index = 0; i4Index < 256; (i4Index += 4), (pu1Huff += 4))
	{
		// every 128 bits, write only 32 bits data, rest 96 bits fill zero
		//x_memcpy(pu1Huff, &au1Huffval[i4Index], 4);
		pu1Huff[0] = au1Huffval[i4Index + 0];
		pu1Huff[1] = au1Huffval[i4Index + 1];
		pu1Huff[2] = au1Huffval[i4Index + 2];
		pu1Huff[3] = au1Huffval[i4Index + 3];
		//x_memset(pu1Huff + 4, 0, 12);
	}
	//HalFlushDCache();
#if defined(CC_MT5365) || defined(CC_MT5395) ||defined(JPEG_IS_POST_MT5368)
    HalFlushInvalidateDCacheMultipleLine((UINT32)(pu1DstAddr), (UINT32)(256));
#else
    HalFlushInvalidateDCache();
#endif
}

// huff value address
#ifndef JPEG_VLD_V3
INT32 VLDJOG_SetHuffAddr(UINT8 *pu1DcHuff, UINT8 *pu1AcHuff)
{
	ASSERT(pu1DcHuff != NULL);
	ASSERT(pu1AcHuff != NULL);
	
	TRACE(("VLDJOG_SetHuffAddr(dc(0x%08x),ac(0x%08x))\n", pu1DcHuff, pu1AcHuff));
    
      if (((UINT32)pu1DcHuff & 15) || ((UINT32)pu1AcHuff & 15))
      {
            return -(INT32)E_HWJPG_ERR_HUFF_ADDR;
      }
      
	//dump huffman value
	#if 0
	/*{
		INT32 i4Index;

		LOG(6, "Dc huffval - \n");
		for (i4Index = 0; i4Index < 4096; i4Index++)
		{
			LOG(6, (const CHAR *)((i4Index != 0 && (i4Index % 16 == 0)) ? "\n" : " "));
			LOG(6, "%2x ", (UINT32)*((UINT8*)((UINT32)pu1DcHuff + i4Index)));
		}
		LOG(6, "\n\nAc huffval - \n");
		for (i4Index = 0; i4Index < 4096; i4Index++)
		{
			LOG(6, (const CHAR *)((i4Index != 0 && (i4Index % 16 == 0)) ? "\n" : " "));
			LOG(6, "%2x ", (UINT32)*((UINT8*)((UINT32)pu1AcHuff + i4Index)));
		}
		LOG(6, "\n\n");
	}*/
	#endif
	//Christie 07/0307, huffman table limitation, 64M bound
      if(((UINT32)pu1DcHuff > (UINT32)0x4000000) ||((UINT32)pu1AcHuff > (UINT32)0x4000000)) //64M
      {
          Printf("Huffman Tbl Addr out of 64M\n");
      }

//	VLD0_WRITE32(VLDREG_DRAM_HUFF, (UINT32)pu1DcHuff >> 2);
//	VLD0_WRITE32(VLDREG_DRAM_HUFF, ((UINT32)pu1AcHuff >> 2) | VLD_AC_HUFF_FLAG);
	VLD0_WRITE32(VLDREG_DRAM_HUFF, (UINT32)pu1DcHuff);
	VLD0_WRITE32(VLDREG_DRAM_HUFF, ((UINT32)pu1AcHuff) | VLD_AC_HUFF_FLAG);

	return (INT32)E_HWJPG_OK;
}
#endif

INT32 VLDJPG_SetHuffTblVal(UINT8 *au1Huffval, BOOL fgDCTbl)
{
    UINT32 u4Val=0, u4TblNum=0, u4I;
    UINT8 *pu1Huffval;

#ifdef __KERNEL__
    au1Huffval=(UINT8*)(VIRTUAL((UINT32)au1Huffval));
#endif

    for (u4TblNum = 0; u4TblNum < D_MAX_JPEG_HUFF_TBL; u4TblNum++)
    {
        u4Val = u4TblNum << VLD_HUFF_TABLE_IDENTIFIER;
        if (!fgDCTbl)
        {
            u4Val |= (1 << VLD_HUFF_DC_AC_SELECTION);
        }
    
        VLD0_WRITE32(VLDREG_JFILLH, u4Val);

        pu1Huffval = au1Huffval + (u4TblNum << 8);

        for (u4I = 0; u4I < 256; u4I += 4)
        {
            u4Val = ((pu1Huffval[u4I + 3] << 24) | (pu1Huffval[u4I + 2] << 16) |
                    (pu1Huffval[u4I + 1] << 8) | pu1Huffval[u4I]);
            VLD0_WRITE32(VLDREG_JHVAL, u4Val);
        }
    }
    
    return (INT32)E_HWJPG_OK;
}	

// set dc needed flag (1bit) for each block (maximum 10)
INT32 VLDJPG_SetDcNeededList(UINT32 u4DcNeededList)
{
	UINT32 u4Reg;

	TRACE(("VLDJPG_SetDcNeededList(0x%08x)\n", u4DcNeededList));

	u4Reg = VLD0_READ32(VLDREG_BLK_PARAM_LIST);
	u4Reg = (u4Reg & ~(VLD_DCNEED_MASK << VLD_DCNEED_SHIFT)) |
		((u4DcNeededList & VLD_DCNEED_MASK) << VLD_DCNEED_SHIFT);
	VLD0_WRITE32(VLDREG_BLK_PARAM_LIST, u4Reg);

	return (INT32)E_HWJPG_OK;
}

// set ac needed flag (1bit) for each block (maximum 10)
INT32 VLDJPG_SetAcNeededList(UINT32 u4AcNeededList)
{
	UINT32 u4Reg;

	TRACE(("VLDJPG_SetAcNeededList(0x%08x)\n", u4AcNeededList));

	u4Reg = VLD0_READ32(VLDREG_BLK_PARAM_LIST);
	u4Reg = (u4Reg & ~(VLD_ACNEED_MASK << VLD_ACNEED_SHIFT)) | ((u4AcNeededList & VLD_ACNEED_MASK) << VLD_ACNEED_SHIFT);
	VLD0_WRITE32(VLDREG_BLK_PARAM_LIST, u4Reg);

	return (INT32)E_HWJPG_OK;
}

// set block count in an MCU
INT32 VLDJPG_SetBlockCount(UINT32 u4BlockCount)
{
	UINT32 u4Reg;

	TRACE(("VLDJPG_SetBlockCount(%u)\n", u4BlockCount));

	u4Reg = VLD0_READ32(VLDREG_BLK_PARAM_LIST);
	u4Reg = (u4Reg & ~(VLD_MCUBLK_MASK << VLD_MCUBLK_SHIFT)) | ((u4BlockCount & VLD_MCUBLK_MASK) << VLD_MCUBLK_SHIFT);
	VLD0_WRITE32(VLDREG_BLK_PARAM_LIST, u4Reg);

	return (INT32)E_HWJPG_OK;
}

// set membership register
INT32 VLDJPG_SetMemberShip(UINT32 u4Membership)
{
	// per block has 2 bits
	// member = 0;
	// for each block (index blkno, blongs to blk_comp[blkno])
	// 	member |= blk_comp[blkno]

	TRACE(("VLDJPG_SetMemberShip(0x%08x)\n", u4Membership));

	VLD0_WRITE32(VLDREG_MCU_MEMBERSHIP, u4Membership);
	return (INT32)E_HWJPG_OK;
}

// set dc & ac table for each block
// maximum 10 blocks (4,4,2) for mtk vld hardware
// each block can be one of table 0 1 2 3
INT32 VLDJPG_SetDcTableList(UINT32 u4DcTableList)
{
	TRACE(("VLDJPG_SetDcTableList(0x%08x)\n", u4DcTableList));

	VLD0_WRITE32(VLDREG_DC_TBL_LIST, u4DcTableList);
	return (INT32)E_HWJPG_OK;
}
INT32 VLDJPG_SetAcTableList(UINT32 u4AcTableList)
{
	TRACE(("VLDJPG_SetAcTableList(0x%08x)\n", u4AcTableList));

	VLD0_WRITE32(VLDREG_AC_TBL_LIST, u4AcTableList);
	return (INT32)E_HWJPG_OK;
}

// set q table for each component
INT32 VLDJPG_SetQuanTableList(UINT32 u4QuanTableList)
{
	TRACE(("VLDJPG_SetQuanTableList(0x%08x)\n", u4QuanTableList));

	VLD0_WRITE32(VLDREG_Q_TBL_LIST, u4QuanTableList);
	return (INT32)E_HWJPG_OK;
}

// switch VLD to jpeg decoding mode only when start jpeg decoding,
// after jpeg operation, switch back to mpeg
INT32 VLDJPG_Switch2Jpeg(void)
{
	TRACE(("VLDJPG_Switch2Jpeg()\n"));
	VLD0_WRITE32(VLDREG_JPEG_SWITCH, VLD_JPEG_MODE);
	return (INT32)E_HWJPG_OK;
}
INT32 VLDJPG_Switch2Mpeg(void)
{
	TRACE(("VLDJPG_Switch2Mpeg()\n"));
	VLD0_WRITE32(VLDREG_JPEG_SWITCH, VLD_MPEG_MODE);
	return (INT32)E_HWJPG_OK;
}

// clean buffer only when decode 1st mcu
// after 1st mcu, we can keep buffer for fast decode
INT32 VLDJPG_Switch2CleanBuffer(void)
{
	#ifndef JPEG_VLD_V4
	TRACE(("VLDJPG_Switch2CleanBuffer()\n"));
	VLD0_WRITE32(VLDREG_FAST_START_DECODE, VLD_CLEAN_INTERNAL_BUFFER);
	#endif
	
	return (INT32)E_HWJPG_OK;
}
INT32 VLDJPG_Switch2KeepBuffer(void)
{
	#ifndef JPEG_VLD_V4
	TRACE(("VLDJPG_Switch2KeepBuffer()\n"));
	VLD0_WRITE32(VLDREG_FAST_START_DECODE, VLD_DONT_CLEAN_INTERNAL_BUFFER);
	#endif
	
	return (INT32)E_HWJPG_OK;
}

// set decoding mode
INT32 VLDJPG_Switch2Baseline(void)
{
	TRACE(("VLDJPG_Switch2Baseline()\n"));
	VLD0_WRITE32(VLDREG_PROGRESSIVE_MODE, VLD_DECODE_BASELINE << VLD_DECODE_MODE_SHIFT);
	return (INT32)E_HWJPG_OK;
}
INT32 VLDJPG_Switch2Progressive(void)
{
	UINT32 u4Reg;
	TRACE(("VLDJPG_Switch2Progressive()\n"));
	u4Reg = VLD0_READ32(VLDREG_PROGRESSIVE_MODE);
	u4Reg = (u4Reg & (~(VLD_DECODE_MODE_MASK << VLD_DECODE_MODE_SHIFT))) | (VLD_DECODE_PROGRESSIVE << VLD_DECODE_MODE_SHIFT);
	VLD0_WRITE32(VLDREG_PROGRESSIVE_MODE, u4Reg);
	return (INT32)E_HWJPG_OK;
}


// set one of four progressive mode:
// ac_first, ac_refine, dc_first, dc_refine
INT32 VLDJPG_SetProgressiveMode(UINT8 u1ProgMode)
{
	UINT32 u4Reg;
	TRACE(("VLDJPG_SetProgressiveMode(%u)\n", u1ProgMode));
	u4Reg = VLD0_READ32(VLDREG_PROGRESSIVE_MODE);
	u4Reg = (u4Reg & (~(VLD_PROG_MASK << VLD_PROG_SHIFT))) | ((UINT32)u1ProgMode << VLD_PROG_SHIFT);
	VLD0_WRITE32(VLDREG_PROGRESSIVE_MODE, u4Reg);
	return (INT32)E_HWJPG_OK;
}


// indicate this is the last scan of a progressive image
INT32 VLDJPG_SetProgressiveLastScan(BOOL fgLastScan)
{
	UINT32 u4Reg;

	TRACE(("VLDJPG_SetProgressiveLastScan(%d)\n", fgLastScan));

	u4Reg = VLD0_READ32(VLDREG_PROGRESSIVE_MODE);
	u4Reg = (u4Reg & (~(VLD_PROG_WRITE_MASK << VLD_PROG_WRITE_SHIFT))) |
		((UINT32)fgLastScan << VLD_PROG_WRITE_SHIFT);
	VLD0_WRITE32(VLDREG_PROGRESSIVE_MODE, u4Reg);
	return (INT32)E_HWJPG_OK;
}

// TRUE to write only none zero history buffer (1 bit per pixel)
// FALSE to write to IDCT output buffer
INT32 VLDJPG_SetProgressiveNzOnly(BOOL fgWriteNzOnly)
{
	TRACE(("VLDJPG_SetProgressiveNzOnly(%d)\n", fgWriteNzOnly));
	VLD0_WRITE32(VLDREG_PROGRESSIVE_NONZERO_ONLY, (UINT32)fgWriteNzOnly);
	return (INT32)E_HWJPG_OK;
}

// specify Ah and Al in progressive decoding process
INT32 VLDJPG_SetProgressiveAhAl(UINT8 u1AhAl)
{
	TRACE(("VLDJPG_SetProgressiveAhAl(0x%x)\n", u1AhAl));
	VLD0_WRITE32(VLDREG_PROGRESSIVE_ALAH, ((u1AhAl & 0xf) << VLD_PROG_AL_SHIFT) | ((u1AhAl >> 4) << VLD_PROG_AH_SHIFT));
	return (INT32)E_HWJPG_OK;
}

INT32 VLDJPG_SetProgressiveSeSs(UINT8 u1Se, UINT8 u1Ss)
{
	TRACE(("VLDJPG_SetProgressiveSeSs(0x%x,0x%x)\n", u1Se, u1Ss));
	VLD0_WRITE32(VLDREG_PROGRESSIVE_SESS,
		((u1Se & VLD_PROG_SE_MASK) << VLD_PROG_SE_SHIFT) |
		((u1Ss & VLD_PROG_SS_MASK) << VLD_PROG_SS_SHIFT));
	return (INT32)E_HWJPG_OK;
}

// set none zero history buffer for progressive decoding process
INT32 VLDJPG_SetProgressiveNoneZeroHistory(UINT8 *pu1NoneZeroHistory)
{
	UINT32 u4NzHist;

	TRACE(("VLDJPG_SetProgressiveNoneZeroHistory(0x%08x)\n", pu1NoneZeroHistory));

	u4NzHist = (UINT32)pu1NoneZeroHistory;
	VLD0_WRITE32(VLDREG_NONE_ZERO_HISTORY, u4NzHist);
	return (INT32)E_HWJPG_OK;
}

INT32 VLDJPG_SetCurrMCUNum(UINT32 u4CurrMCUNum)
{
	TRACE(("VLDJPG_SetCurrMCUNum(%u)\n", u4CurrMCUNum));

	VLD0_WRITE32(VLDREG_CURR_MCU, u4CurrMCUNum);
	return (INT32)E_HWJPG_OK;
}

INT32 VLDJPG_SetCurrMcuRowNum5391(UINT32 u4CurrMcuRowNum)
{
	TRACE(("---- VLDJPG_SetCurrMcuRowNum5391(%u) ----\n", u4CurrMcuRowNum));

//	VLD0_WRITE32(VLDREG_CURR_MCU, u4CurrMcuRowNum);
	return (INT32)E_HWJPG_OK;
}

INT32 VLDJPG_SetEndOfBandRun(UINT32 u4EndOfBandRun)
{
	TRACE(("VLDJPG_SetEndOfBandRun(%u)\n", u4EndOfBandRun));

	VLD0_WRITE32(VLDREG_EOB_RUN_WR, u4EndOfBandRun);
	return (INT32)E_HWJPG_OK;
}

INT32 VLDJPG_GetEndOfBandRun(UINT32 *pu4EndOfBandRun)
{
	if (pu4EndOfBandRun == NULL)
	{
		return -(INT32)E_HWJPG_ERR_PARAM;
	}
	*pu4EndOfBandRun = VLD0_READ32(VLDREG_EOB_RUN_RD);

	TRACE(("VLDJPG_GetEndOfBandRun(%u)\n", *pu4EndOfBandRun));

	return (INT32)E_HWJPG_OK;
}

// read address of components
INT32 VLDJPG_SetCompReadAddr(UINT32 u4Comp, void *avCompReadAddr)
{
	ASSERT(u4Comp < D_MAX_JPEG_HW_COMP);

	TRACE(("VLDJPG_SetCompReadAddr(%u, 0x%08x)\n", u4Comp, avCompReadAddr));

	VLD0_WRITE32(VLDREG_READ_START_COMP(u4Comp), (UINT32)avCompReadAddr >> 2);
	return (INT32)E_HWJPG_OK;
}

// write address of components
INT32 VLDJPG_SetCompWriteAddr(UINT32 u4Comp, void *avCompWriteAddr)
{
	ASSERT(u4Comp < D_MAX_JPEG_HW_COMP);

	TRACE(("VLDJPG_SetCompWriteAddr(%u, 0x%08x)\n", u4Comp, avCompWriteAddr));

	VLD0_WRITE32(VLDREG_WRITE_START_COMP(u4Comp), (UINT32)avCompWriteAddr >> 2);
	return (INT32)E_HWJPG_OK;
}

// write x address list and y address list for each block
// each block has 3 bits
INT32 VLDJPG_SetBlockAddrList(const UINT32 *au4XAddrList, const UINT32 *au4YAddrList)
{
	INT32 i4Blk;
	UINT32 u4XAddrList = 0, u4YAddrList = 0;
	
	ASSERT(au4XAddrList != NULL);
	ASSERT(au4YAddrList != NULL);

	for (i4Blk = 0; i4Blk < 10; i4Blk++)
	{
		u4XAddrList |= (au4XAddrList[i4Blk] & 0x7) << (i4Blk * 3);
		u4YAddrList |= (au4YAddrList[i4Blk] & 0x7) << (i4Blk * 3);
	}

	TRACE(("VLDJPG_SetBlockAddrList(0x%08x, 0x%08x)\n", u4XAddrList, u4YAddrList));
	VLD0_WRITE32(VLDREG_X_ADDR_LIST, u4XAddrList);
	VLD0_WRITE32(VLDREG_Y_ADDR_LIST, u4YAddrList);

	return (INT32)E_HWJPG_OK;
}

// write idct pitch of each component
INT32 VLDJPG_SetCompIdctPitch(const UINT32 *au4CompPitch)
{
	UINT32 u4Reg;

	ASSERT(au4CompPitch != NULL);

if ((VLDJPG_GetDecMode5391() == E_JPG_DEC_MODE_BASELINE_MCU_ROW_RZ) ||
    (VLDJPG_GetDecMode5391() == E_JPG_DEC_MODE_BASELINE_PIC_RZ)||
    (VLDJPG_GetDecMode5391() == E_JPG_DEC_MODE_BASELINE_PIC_VIDEO_OUTPUT))
{
	TRACE(("VLDJPG_SetCompIdctPitch(%u, %u, %u)\n", 
	    au4CompPitch[0], au4CompPitch[0], au4CompPitch[0]));

	VLD0_WRITE32(VLDREG_COMP_PITCH1,
		(((au4CompPitch[0] >> 2) & VLD_PITCH_LO_MASK) << VLD_PITCH_LO_SHIFT) |
		(((au4CompPitch[0] >> 2) & VLD_PITCH_HI_MASK) << VLD_PITCH_HI_SHIFT));

	u4Reg = VLD0_READ32(VLDREG_COMP_PITCH2);
	u4Reg = (u4Reg & ~(VLD_PITCH_LO_MASK << VLD_PITCH_LO_SHIFT)) |
		(((au4CompPitch[0] >> 2) & VLD_PITCH_LO_MASK) << VLD_PITCH_LO_SHIFT);
	VLD0_WRITE32(VLDREG_COMP_PITCH2, u4Reg);
}
else
{
	TRACE(("VLDJPG_SetCompIdctPitch(%u, %u, %u)\n", au4CompPitch[0], au4CompPitch[1], au4CompPitch[2]));

	VLD0_WRITE32(VLDREG_COMP_PITCH1,
		(((au4CompPitch[0] >> 2) & VLD_PITCH_LO_MASK) << VLD_PITCH_LO_SHIFT) |
		(((au4CompPitch[1] >> 2) & VLD_PITCH_HI_MASK) << VLD_PITCH_HI_SHIFT));

	u4Reg = VLD0_READ32(VLDREG_COMP_PITCH2);
	u4Reg = (u4Reg & ~(VLD_PITCH_LO_MASK << VLD_PITCH_LO_SHIFT)) |
		(((au4CompPitch[2] >> 2) & VLD_PITCH_LO_MASK) << VLD_PITCH_LO_SHIFT);
	VLD0_WRITE32(VLDREG_COMP_PITCH2, u4Reg);
}
	return (INT32)E_HWJPG_OK;
}

// write coef pitch of each component
INT32 VLDJPG_SetCompCoefPitch(const UINT32 *au4CoefPitch)
{
	UINT32 u4Reg;

	ASSERT(au4CoefPitch != NULL);

if ((VLDJPG_GetDecMode5391() == E_JPG_DEC_MODE_BASELINE_MCU_ROW_RZ) ||
    (VLDJPG_GetDecMode5391() == E_JPG_DEC_MODE_BASELINE_PIC_RZ)||
    (VLDJPG_GetDecMode5391() == E_JPG_DEC_MODE_BASELINE_PIC_VIDEO_OUTPUT))
{
	TRACE(("VLDJPG_SetCompCoefPitch(%u, %u, %u)\n", 
	    au4CoefPitch[0], au4CoefPitch[0], au4CoefPitch[0]));

	u4Reg = VLD0_READ32(VLDREG_COMP_PITCH2);
	u4Reg = (u4Reg & (~(VLD_PITCH_HI_MASK << VLD_PITCH_HI_SHIFT))) |
		(((au4CoefPitch[0] >> 2) & VLD_PITCH_HI_MASK) << VLD_PITCH_HI_SHIFT);
	VLD0_WRITE32(VLDREG_COMP_PITCH2, u4Reg);

	VLD0_WRITE32(VLDREG_COMP_PITCH3,
		(((au4CoefPitch[0] >> 2) & VLD_PITCH_LO_MASK) << VLD_PITCH_LO_SHIFT) |
		(((au4CoefPitch[0] >> 2) & VLD_PITCH_HI_MASK) << VLD_PITCH_HI_SHIFT));
}
else
{
	TRACE(("VLDJPG_SetCompCoefPitch(%u, %u, %u)\n", au4CoefPitch[0], au4CoefPitch[1], au4CoefPitch[2]));

	u4Reg = VLD0_READ32(VLDREG_COMP_PITCH2);
	u4Reg = (u4Reg & (~(VLD_PITCH_HI_MASK << VLD_PITCH_HI_SHIFT))) |
		(((au4CoefPitch[0] >> 2) & VLD_PITCH_HI_MASK) << VLD_PITCH_HI_SHIFT);
	VLD0_WRITE32(VLDREG_COMP_PITCH2, u4Reg);

	VLD0_WRITE32(VLDREG_COMP_PITCH3,
		(((au4CoefPitch[1] >> 2) & VLD_PITCH_LO_MASK) << VLD_PITCH_LO_SHIFT) |
		(((au4CoefPitch[2] >> 2) & VLD_PITCH_HI_MASK) << VLD_PITCH_HI_SHIFT));
}
	return (INT32)E_HWJPG_OK;
}

#define D_JPEG_HW_WAIT	0x200000
#define D_JPEG_HW_WAITPIC	(0x200000 * 10)
#define LED_ADDR		0x11000000      // hardcoded, fixme
// start decode process (mcu mode)
INT32 VLDJPG_DecodeMcu(void)
{
	UINT32 u4Wait;

	TRACE(("VLDJPG_DecodeMcu()\n"));

	DUMP_REG();
	VLD0_WRITE32(VLDREG_START_DECODE, VLD_DECODE_MCU);
	/*
	while((IO_READ32(VLD0_BASE, VLDREG_MCU_STATUS) & 1) != 0)
	{
		SetLed((u2Loop++ & 0xff));
	}
	*/
	VLD0_WRITE32(VLDREG_START_DECODE, VLD_CLEAR);
	for (u4Wait = 1;
		(u4Wait < D_JPEG_HW_WAIT) &&
			((VLD0_READ32(VLDREG_MCU_STATUS) & 1) == 0);
		u4Wait++)
	{
		//HAL_WRITE8(LED_ADDR, u4Wait >> 8);
	}
	DUMP_REG();
	if (u4Wait != D_JPEG_HW_WAIT)
	{
		//BIM_WRITE32(REG_SEG7, (UINT16)_u2McuCnt++);
		return (INT32)E_HWJPG_OK;
	}

	return -(INT32)E_JPG_ERR_FATAL;
}

// start decode process (mcu-ROW mode)
INT32 VLDJPG_DecodeFirstMcuRow5391(void)
{
	UINT32 u4Wait;

	TRACE(("VLDJPG_DecodeFirstMcuRow5391()\n"));

	DUMP_REG();
	VLD0_WRITE32(VLDREG_DEC_MB_ROW, VLD_DEC_FIRST_ROW);
	/*
	while((IO_READ32(VLD0_BASE, VLDREG_MCU_STATUS) & 1) != 0)
	{
		SetLed((u2Loop++ & 0xff));
	}
	*/
	VLD0_WRITE32(VLDREG_DEC_MB_ROW, VLD_CLEAR);
	for (u4Wait = 1;
		(u4Wait < D_JPEG_HW_WAIT) &&
			((VLD0_READ32(VLDREG_ROW_PIC_DEC_FINISH) & VLD_ROW_DEC_MCU_ROW_FINISH) == 0);
		u4Wait++)
	{
		//HAL_WRITE8(LED_ADDR, u4Wait >> 8);
	}
	DUMP_REG();
	if (u4Wait != D_JPEG_HW_WAIT)
	{
		//BIM_WRITE32(REG_SEG7, (UINT16)_u2McuCnt++);
		return (INT32)E_HWJPG_OK;
	}

	return -(INT32)E_JPG_ERR_FATAL;
}

// start decode process (mcu-ROW mode)
INT32 VLDJPG_DecodeNextMcuRow5391(void)
{
	UINT32 u4Wait;

	TRACE(("VLDJPG_DecodeNextMcuRow5391()\n"));

	DUMP_REG();
	VLD0_WRITE32(VLDREG_DEC_MB_ROW, VLD_DEC_NEXT_ROW);
	/*
	while((IO_READ32(VLD0_BASE, VLDREG_MCU_STATUS) & 1) != 0)
	{
		SetLed((u2Loop++ & 0xff));
	}
	*/
	VLD0_WRITE32(VLDREG_DEC_MB_ROW, VLD_CLEAR);
	for (u4Wait = 1;
		(u4Wait < D_JPEG_HW_WAIT) &&
			((VLD0_READ32(VLDREG_ROW_PIC_DEC_FINISH) & VLD_ROW_DEC_MCU_ROW_FINISH) == 0);
		u4Wait++)
	{
		//HAL_WRITE8(LED_ADDR, u4Wait >> 8);
	}
	DUMP_REG();
	if (u4Wait != D_JPEG_HW_WAIT)
	{
		//BIM_WRITE32(REG_SEG7, (UINT16)_u2McuCnt++);
		return (INT32)E_HWJPG_OK;
	}

	return -(INT32)E_JPG_ERR_FATAL;
}

// start decode process (pic mode)
INT32 VLDJPG_DecodePic5391(void)
{
	UINT32 u4Wait;

	TRACE(("VLDJPG_DecodePic5391()\n"));

	DUMP_REG();
	
	#ifdef MT5368_VFIFO_RWPTR_TEST
	VLDJPG_SetRiscCtrlWP(VLD0_READ32(VLDREG_WATCH_RP)+0x2000);
	#endif
	
	VLD0_WRITE32(VLDREG_DEC_MB_ROW, VLD_DEC_FIRST_ROW);
	/*
	while((IO_READ32(VLD0_BASE, VLDREG_MCU_STATUS) & 1) != 0)
	{
		SetLed((u2Loop++ & 0xff));
	}
	*/
	VLD0_WRITE32(VLDREG_DEC_MB_ROW, VLD_CLEAR);

	for (u4Wait = 1;
		(u4Wait < D_JPEG_HW_WAITPIC) &&
			((VLD0_READ32(VLDREG_ROW_PIC_DEC_FINISH) & VLD_ROW_DEC_PIC_FINISH) == 0);
		u4Wait++)
	{
		//HAL_WRITE8(LED_ADDR, u4Wait >> 8);
		#ifdef MT5368_VFIFO_RWPTR_TEST
		if(VLD0_READ32(VLDREG_WAIT_THRES) == VLD0_READ32(VLDREG_WATCH_WP))
		{
			x_thread_delay(10);			
			VLDJPG_SetRiscCtrlWP(VLD0_READ32(VLDREG_RISC_CTRL_WP)+0x800);
			printf("111 extend w ptr =[%x]\n", VLD0_READ32(VLDREG_RISC_CTRL_WP));
		}		
		#endif
	}
	
	DUMP_REG();
	if (u4Wait != D_JPEG_HW_WAITPIC)
	{
		//BIM_WRITE32(REG_SEG7, (UINT16)_u2McuCnt++);
		return (INT32)E_HWJPG_OK;
	}

	return -(INT32)E_JPG_ERR_FATAL;
}

// start decode process (pic mode, not polling in this api)
void VLDJPG_DecodePic(void)
{
	TRACE(("VLDJPG_DecodePic()\n"));

	DUMP_REG();
	VLD0_WRITE32(VLDREG_DEC_MB_ROW, VLD_DEC_FIRST_ROW);

	VLD0_WRITE32(VLDREG_DEC_MB_ROW, VLD_CLEAR);
}

// check decode finish status
// return 0 if finish
// return err if unfinish
INT32 VLDJPG_CheckDecodeFinish(void)
{
    if ((VLD0_READ32(VLDREG_ROW_PIC_DEC_FINISH) & VLD_ROW_DEC_PIC_FINISH))
    {
	return (INT32)E_HWJPG_OK;
    }
    else
    {
        return (INT32)E_HWJPG_BUSY;
    }
}

INT32 VLDJPG_SoftwareReset(void)
{
	TRACE(("VLDJPG_SoftwareReset()\n"));

	#ifdef JPEG_VLD_V4
	VLD0_WRITE32(VLDREG_SOFT_RESET, (VLD_SW_RESET_RESET |VLD_LARB_RESET |VLD_ASYNC_FIFO_RST));
	//VLD0_WRITE32(VLDREG_SOFT_RESET, (VLD_ASYNC_FIFO_RST));
	#else
	VLD0_WRITE32(VLDREG_SOFT_RESET, VLD_SW_RESET_RESET);
	#endif
	
	VLD0_WRITE32(VLDREG_SOFT_RESET, VLD_SW_RESET_CLEAR);
	return (INT32)E_HWJPG_OK;
}

INT32 VLDJPG_SramClear(void)
{
	TRACE(("VLDJPG_SramClear()\n"));
	VLD0_WRITE32(VLDREG_JPEG_SRAM_CLEAR, 0x0);
	VLD0_WRITE32(VLDREG_JPEG_SRAM_CLEAR, 0x100);
	VLD0_WRITE32(VLDREG_JPEG_SRAM_CLEAR, 0x0);
	return (INT32)E_HWJPG_OK;
}

#ifdef JPEG_VLD_V2
INT32 VLDJPG_SetRiscWaitThreshold(void)
{
	TRACE(("VLDJPG_SetRiscWaitThreshold()\n"));

	VLD0_WRITE32(VLDREG_WAIT_THRES, 0x4000);
	return (INT32)E_HWJPG_OK;
}
#endif

INT32 VLDJPG_Restart()
{
	TRACE(("VLDJPG_Restart()\n"));

	VLD0_WRITE32(VLDREG_RESTART, VLD_RESTART);
	VLD0_WRITE32(VLDREG_RESTART, VLD_CLEAR);
	return (INT32)E_HWJPG_OK;
}

INT32 VLDJPG_AdvanceRestartMarker(UINT8 *pu1Marker)
{
	UINT8 u1RestartMarker;

	TRACE(("VLDJPG_AdvanceRestartMarker()\n"));

	u1RestartMarker = (UINT8)((VLD0_READ32(VLDREG_MCU_STATUS) >> 8) & 0xff);
	if (pu1Marker)
	{
		*pu1Marker = u1RestartMarker;
	}
	return (INT32)E_HWJPG_OK;
}

INT32 VLDJPG_InitScan(void)
{
	TRACE(("VLDJPG_InitScan()\n"));

	VLD0_WRITE32(VLDREG_RESTART, VLD_CLEAR);
	VLD0_WRITE32(VLDREG_RESTART, VLD_RESTART_WORLD);
	VLD0_WRITE32(VLDREG_RESTART, VLD_CLEAR);
	return (INT32)E_HWJPG_OK;
}

INT32 VLDJPG_CheckVideoInputWindow(void)
{
	TRACE(("VLDJPG_CheckVideoInputWindow()\n"));
    
	VLD0_READ32(VLDREG_INPUT_WINDOW);
	return (INT32)E_HWJPG_OK;
}

INT32 VLDJPG_CheckVideoReadPointer(void)
{
	TRACE(("VLDJPG_CheckVideoReadPointer()\n"));

	VLD0_READ32(VLDREG_VIDEO_RPTR);
	return (INT32)E_HWJPG_OK;
}

INT32 VLDJPG_WritePointer(void)
{
	TRACE(("VLDJPG_WritePointer()\n"));
	
	return (INT32)VLD0_READ32(VLDREG_WATCH_WP);
}
#ifdef JPEG_VLD_TO_PARSE_HEADER
INT32 VLDJPG_ReadPointer(void)
{
	TRACE(("VLDJPG_ReadPointer()\n"));
	
	return (INT32)VLD0_READ32(VLDREG_VIDEO_RPTR);
}
#endif
INT32 VLDJPG_SetRiscControlWptr(void)
{
	VLD0_WRITE32(VLDREG_LATCH2, LATCH2_RCWP);
	return (INT32)E_HWJPG_OK;
}

//==========================================================

// allocate an vld jpeg hardware instance
JPEGHANDLE VLDJPG_AllocInstance(void)
{
	JPEGHANDLE hInstance;
	JPEG_BASE_CLASS_T *prRootObject;
	JPEG_VLDJPG_DATA_T *prVldObject;
	#ifndef __KERNEL__
	static UINT8 au1DCTbl[(1024*4) + 15];
	static UINT8 au1ACTbl[(1024*4) + 15];
	#endif

	// allocate vld information instance
	hInstance = (JPEGHANDLE)x_mem_alloc(sizeof(JPEG_VLDJPG_DATA_T));
	if (hInstance == NULL)
	{
		x_mem_free(hInstance);
		return hInstance;
	}
	x_memset(hInstance, 0, sizeof(JPEG_VLDJPG_DATA_T));
	// init its root object
	prRootObject = (JPEG_BASE_CLASS_T *)hInstance;
	prRootObject->eJpegClass = E_JC_VLD_DECODER;
	prRootObject->zObjectSize = sizeof(JPEG_VLDJPG_DATA_T);

	// init huffman table buffer
	prVldObject = (JPEG_VLDJPG_DATA_T *)hInstance;
	
	#ifdef __KERNEL__		//for linux system	
	prVldObject->pu1HwDcHuffTbl = (UINT8 *)BSP_AllocAlignedDmaMemory(1024*4, 16);
	prVldObject->pu1HwAcHuffTbl = (UINT8 *)BSP_AllocAlignedDmaMemory(1024*4, 16);
	if((prVldObject->pu1HwDcHuffTbl == NULL) || (prVldObject->pu1HwAcHuffTbl == NULL))
	{
		x_mem_free(hInstance);
		return NULL;
	}
	//x_memset(VIRTUAL(prVldObject->pu1HwDcHuffTbl), 0, (1024*4));
	//x_memset(VIRTUAL(prVldObject->pu1HwAcHuffTbl), 0, (1024*4));
	#else					//for nucleus system
	prVldObject->pu1HwDcHuffTbl = (UINT8*)PHYSICAL(((UINT32)au1DCTbl + 15) & (~15));
	prVldObject->pu1HwAcHuffTbl = (UINT8*)PHYSICAL(((UINT32)au1ACTbl + 15) & (~15));	
	//x_memset(au1DCTbl, 0, (1024*4) + 15);
	//x_memset(au1ACTbl, 0, (1024*4) + 15);	
#endif
	
	return hInstance;
}

// release an vld jpeg hardware instance
void VLDJPG_FreeInstance(JPEGHANDLE hInstance)
{
	#ifdef __KERNEL__		//for linux system	
	JPEG_VLDJPG_DATA_T *prVldObject;
	
	prVldObject = (JPEG_VLDJPG_DATA_T *)hInstance;
	if(prVldObject == NULL)
	{
		return;
	}
	if(prVldObject->pu1HwDcHuffTbl)
	{
		VERIFY(BSP_FreeAlignedDmaMemory((UINT32)prVldObject->pu1HwDcHuffTbl));
	}
	if(prVldObject->pu1HwAcHuffTbl)
	{
		VERIFY(BSP_FreeAlignedDmaMemory((UINT32)prVldObject->pu1HwAcHuffTbl));
	}
	#endif
	
	x_mem_free(hInstance);
}

// start hardware decoder, init data structure (allocate vld0)
INT32 VLDJPG_New(const JPEGHANDLE hInstance)
{
  UINT32 u4SysClock=5;
#ifdef JPEG_VLD_V2 
    u4SysClock=5;//200
#elif defined(JPG_CLK_FROM_JPGDEC_CKCFG)
    #if defined(CC_MT5882)    
    u4SysClock = 7; // 288
    #elif defined(CC_MT5890)
    if(IS_IC_5861())
    {        
        u4SysClock = 7; // 288 gazelle
    }
    else
    {
        u4SysClock = 2; //216 oryx
    }
    #else
    u4SysClock=2; // 216
    #endif
#elif defined(JPG_CLK_FROM_GRAPHIC)
  u4SysClock=1; // 240
#else
  u4SysClock=4;//162
#endif
#ifdef JPEG_AUTO_TEST
  u4SysClock=_JpgVerifyCmdGetParam(9,0);
#endif
  u4SysClock=u4SysClock&0x0f;
  LOG(5, "jpg clk:%d\n", u4SysClock);
    // set mpg & jpg clock
  //IO_WRITE32(CKGEN_BASE, 0x224, u4SysClock);         //162 mhz for baseline-->1
#if defined(JPG_CLK_FROM_JPGDEC_CKCFG)  //joshua
  #ifdef JPEG_VLD_V5
  IO_WRITE32(CKGEN_BASE, 0x394, u4SysClock);
  #else
  IO_WRITE32(CKGEN_BASE, 0x310, u4SysClock);        //from jpgdec_clk    
  #endif
#elif defined(JPG_CLK_FROM_GRAPHIC)  //joshua
  IO_WRITE32(CKGEN_BASE, 0x230, u4SysClock);        //from gra_clk    
#else
  IO_WRITE32(CKGEN_BASE, 0x224, u4SysClock);         //from sys_clk    
#endif

//#ifndef CC_MT5392B
	VLDJPG_SetPowerOn();
//#endif
	VERIFY(VLDJPG_SoftwareReset() == (INT32)E_HWJPG_OK);
    VLDJPG_SramClear();
    
    #if defined (CC_MT5890) 
    VLD0_WRITE32(VLDREG_JPGMISC, VLD0_READ32(VLDREG_JPGMISC) | (1 << 8));
    LOG(7, "IMGRZ Hw Id:0x%x\n", VLD0_READ32(VLDREG_JPGMISC));
    #endif
    // ????
//    VLD0_WRITE32((0x12C + 0x400), 0xffffffff);

	// ????
//	MC0_WRITE32(0x98, 1);
//	VLD0_WRITE32(VLDREG_LATCH2, LATCH2_SET);
//#ifdef CC_MT5387
#ifdef JPEG_VLD_V2
    VLD0_WRITE32(VLDREG_LATCH2, 0x01000001);
    VLDJPG_SetRiscWaitThreshold();
#else
    //LOG(3,"--- 536x test ---\n");
    #ifndef JPEG_VLD_V4
	VLD0_WRITE32(VLDREG_LATCH2, 0x010E0103);
    #endif
#endif	


	UNUSED(hInstance);
	return VLDJPG_Switch2Jpeg();
}


#if defined(D_JPEG_REG_DUMP)
void _jpg_DumpReg(void)
{
	UINT32  u4Idx;
    for (u4Idx = 0xB0/16; u4Idx < 0x3F4/16; u4Idx++)
    {
        //x_thread_delay(5);
        Printf("0x%08x:\t 0x%08x 0x%08x 0x%08x 0x%08x\n",
               (VLD0_BASE +(u4Idx*16 + 0)),
               VLD0_READ32((u4Idx*16 + 0)),
               VLD0_READ32((u4Idx*16 + 4)),
               VLD0_READ32((u4Idx*16 + 8)),
               VLD0_READ32((u4Idx*16 + 12)));
    }
    Printf("\n");
    
}
#endif //D_JPEG_REG_DUMP


//////////////////////////////////////////////////////////////
//
// Picture Mode (MCU Row Mode) Related Function
//
//////////////////////////////////////////////////////////////
// read address of components
INT32 VLDJPG_SetCompReadAddr5391(UINT32 u4Comp0Addr, UINT32 u4Comp1Addr, UINT32 u4Comp2Addr)
{
	TRACE(("VLDJPG_SetCompReadAddr5391(%u, 0x%08x)\n", 0, u4Comp0Addr));
	TRACE(("VLDJPG_SetCompReadAddr5391(%u, 0x%08x)\n", 1, u4Comp1Addr));
	TRACE(("VLDJPG_SetCompReadAddr5391(%u, 0x%08x)\n", 2, u4Comp2Addr));

	VLD0_WRITE32(VLDREG_READ_START_COMP(0), (u4Comp0Addr >> 2));
	VLD0_WRITE32(VLDREG_READ_START_COMP(1), (u4Comp1Addr >> 2));
	VLD0_WRITE32(VLDREG_READ_START_COMP(2), (u4Comp2Addr >> 2));
	return (INT32)E_HWJPG_OK;
}

// write address of components
INT32 VLDJPG_SetCompWriteAddr5391(UINT32 u4Comp0Addr, UINT32 u4Comp1Addr, UINT32 u4Comp2Addr)
{
	TRACE(("VLDJPG_SetCompWriteAddr5391(%u, 0x%08x)\n", 0, u4Comp0Addr));
	TRACE(("VLDJPG_SetCompWriteAddr5391(%u, 0x%08x)\n", 1, u4Comp1Addr));
	TRACE(("VLDJPG_SetCompWriteAddr5391(%u, 0x%08x)\n", 2, u4Comp2Addr));

	VLD0_WRITE32(VLDREG_WRITE_START_COMP(0), (u4Comp0Addr >> 2));
	VLD0_WRITE32(VLDREG_WRITE_START_COMP(1), (u4Comp1Addr >> 2));
	VLD0_WRITE32(VLDREG_WRITE_START_COMP(2), (u4Comp2Addr >> 2));
	return (INT32)E_HWJPG_OK;
}

void VLDJPG_SetOutputBank0BufAddr5391(UINT32 u4Comp0Addr, UINT32 u4Comp1Addr, UINT32 u4Comp2Addr)
{
    UINT32 u4Value;
#ifdef __MODEL_slt__
    u4Comp0Addr = PHYSICAL(u4Comp0Addr);
    u4Comp1Addr = PHYSICAL(u4Comp1Addr);
    u4Comp2Addr = PHYSICAL(u4Comp2Addr);
#endif

    TRACE(("VLDJPG_SetOutputBank0BufAddr5391(%u, 0x%08x)\n", 0, u4Comp0Addr));
    TRACE(("VLDJPG_SetOutputBank0BufAddr5391(%u, 0x%08x)\n", 1, u4Comp1Addr));
    TRACE(("VLDJPG_SetOutputBank0BufAddr5391(%u, 0x%08x)\n", 2, u4Comp2Addr));

    u4Value = VLD0_READ32(VLDREG_MB_ROW_DEC_SWITCH);
    u4Value |= VLD_ROW_DEC_WR_ADDR;
    u4Value &= ~(VLD_ROW_DEC_WR_BANK1_ADDR);

    VLD0_WRITE32(VLDREG_MB_ROW_DEC_SWITCH, u4Value);
    VLD0_WRITE32(VLDREG_ROW_DEC_COMP0_ADDR, (u4Comp0Addr >> 2));
    VLD0_WRITE32(VLDREG_ROW_DEC_COMP1_ADDR, (u4Comp1Addr >> 2));
    VLD0_WRITE32(VLDREG_ROW_DEC_COMP2_ADDR, (u4Comp2Addr >> 2));
}

void VLDJPG_SetOutputBank1BufAddr5391(UINT32 u4Comp0Addr, UINT32 u4Comp1Addr, UINT32 u4Comp2Addr)
{
    UINT32 u4Value;
    
#ifdef __MODEL_slt__
        u4Comp0Addr = PHYSICAL(u4Comp0Addr);
        u4Comp1Addr = PHYSICAL(u4Comp1Addr);
        u4Comp2Addr = PHYSICAL(u4Comp2Addr);
#endif

    TRACE(("VLDJPG_SetOutputBank1BufAddr5391(%u, 0x%08x)\n", 0, u4Comp0Addr));
    TRACE(("VLDJPG_SetOutputBank1BufAddr5391(%u, 0x%08x)\n", 1, u4Comp1Addr));
    TRACE(("VLDJPG_SetOutputBank1BufAddr5391(%u, 0x%08x)\n", 2, u4Comp2Addr));

    u4Value = VLD0_READ32(VLDREG_MB_ROW_DEC_SWITCH);
    u4Value |= VLD_ROW_DEC_WR_ADDR;
    u4Value |= VLD_ROW_DEC_WR_BANK1_ADDR;

    VLD0_WRITE32(VLDREG_MB_ROW_DEC_SWITCH, u4Value);
    VLD0_WRITE32(VLDREG_ROW_DEC_COMP0_ADDR, (u4Comp0Addr >> 2));
    VLD0_WRITE32(VLDREG_ROW_DEC_COMP1_ADDR, (u4Comp1Addr >> 2));
    VLD0_WRITE32(VLDREG_ROW_DEC_COMP2_ADDR, (u4Comp2Addr >> 2));
}

void VLDJPG_SetMCUWidth5391(UINT8 compNum, UINT32 u4Comp0MCUWidth, UINT32 u4Comp1MCUWidth, UINT32 u4Comp2MCUWidth)
{
    UINT32 u4Value=0;

	TRACE(("VLDJPG_SetMCUWidth5391(%u)\n", 0, compNum));
    TRACE(("VLDJPG_SetMCUWidth5391(%u, %u)\n", 0, u4Comp0MCUWidth));
    TRACE(("VLDJPG_SetMCUWidth5391(%u, %u)\n", 1, u4Comp1MCUWidth));
    TRACE(("VLDJPG_SetMCUWidth5391(%u, %u)\n", 2, u4Comp2MCUWidth));
    
    //set each component MCU width   
    #ifdef JPEG_VLD_V4
    
    if(compNum == 1)
    {
        u4Value = (1 << 8) + (1 << 4) + 1;
    }
    else
    {
        u4Value += (u4Comp0MCUWidth>>3);
        u4Value += ((u4Comp1MCUWidth>>3) << 4);
        u4Value += ((u4Comp2MCUWidth>>3) << 8);
    }
    VLD0_WRITE32(VLDREG_JPG_MCU_COEF_SIZE,u4Value);
    
    #else
    VLD0_WRITE32(VLDREG_ROW_DEC_COMP0_ADDR_OFST, (u4Comp0MCUWidth >> 2));
    VLD0_WRITE32(VLDREG_ROW_DEC_COMP1_ADDR_OFST, (u4Comp1MCUWidth >> 2));
    VLD0_WRITE32(VLDREG_ROW_DEC_COMP2_ADDR_OFST, (u4Comp2MCUWidth >> 2));
    
    u4Value = VLD0_READ32(VLDREG_MB_ROW_DEC_SWITCH);
    u4Value &= ~(VLD_ROW_DEC_WR_ADDR | VLD_ROW_DEC_WR_BANK1_ADDR);
    VLD0_WRITE32(VLDREG_MB_ROW_DEC_SWITCH, u4Value);
	#endif


}

void VLDJPG_SetPicSize5391(UINT32 u4MCUsPerRow, UINT32 u4TotalRows)
{
    UINT32 u4Value = 0;

    TRACE(("VLDJPG_SetPicSize5391(%u, %u)\n", u4MCUsPerRow, u4TotalRows));

    u4Value = ((u4MCUsPerRow << VLD_ROW_DEC_WIDTH_SHIFT) | (u4TotalRows));
    VLD0_WRITE32(VLDREG_JPEG_PIC_SIZE, u4Value);
}

void VLDJPG_SetRestartInterval5391(UINT32 u4RestartInterval)
{
    TRACE(("VLDJPG_SetRestartInterval5391(%u)\n", u4RestartInterval));

#ifdef JPEG_VLD_V3
    VLD0_WRITE32(VLDREG_RESTART_INTERVAL, ((u4RestartInterval-1) & 0xFFFF) | (1 << VLD_RST_ENABLE));
#else
    VLD0_WRITE32(VLDREG_RESTART_INTERVAL, ((u4RestartInterval-1) & 0xFFFF));
#endif
}

void JPG_SetDecMode5391(INT32 i4DecMode)
{
    _i4JpgCurDecMode5391 = i4DecMode;
}

void VLDJPG_SetDecMode5391(INT32 i4DecMode)
{
    UINT32 u4Value;
    
    u4Value = VLD0_READ32(VLDREG_MB_ROW_DEC_SWITCH);
    u4Value &= ~(VLD_ROW_DEC_MCU_LEVEL | VLD_ROW_DEC_MCU_ROW_LEVEL | VLD_ROW_DEC_PIC_LEVEL);

    switch((ENUM_JPEG_DEC_MODE_T)i4DecMode)
    {
        case E_JPG_DEC_MODE_BASELINE_MCU:
        case E_JPG_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT:
        case E_JPG_DEC_MODE_PROGRESSIVE_MCU_ENHANCE:
            TRACE(("VLDJPG_SetDecMode5391( BASELINE_MCU )\n"));
            u4Value |= VLD_ROW_DEC_MCU_LEVEL;
            VLD0_WRITE32(VLDREG_MB_ROW_DEC_SWITCH, u4Value);
            break;
        case E_JPG_DEC_MODE_BASELINE_MCU_ROW:
        case E_JPG_DEC_MODE_BASELINE_MCU_ROW_RZ:
            TRACE(("VLDJPG_SetDecMode5391( BASELINE_MCU_ROW )\n"));
            u4Value |= VLD_ROW_DEC_MCU_ROW_LEVEL;
            VLD0_WRITE32(VLDREG_MB_ROW_DEC_SWITCH, u4Value);
            break;
        case E_JPG_DEC_MODE_BASELINE_PIC:
        case E_JPG_DEC_MODE_BASELINE_PIC_RZ:
        case E_JPG_DEC_MODE_BASELINE_PIC_VIDEO_OUTPUT:
            TRACE(("VLDJPG_SetDecMode5391( BASELINE_PIC )\n"));
            u4Value |= VLD_ROW_DEC_PIC_LEVEL;
            VLD0_WRITE32(VLDREG_MB_ROW_DEC_SWITCH, u4Value);
            break;
        default:
            Printf("Error: No support i4DecMode = %ld\n", i4DecMode);
            ASSERT(0);
            return;
    }
}

INT32 VLDJPG_GetDecMode5391(void)
{
    return _i4JpgCurDecMode5391;
}

void VLDJPG_UseNewEngine(void)
{
#ifdef JPEG_VLD_V4
	VLD0_WRITE32(VLDREG_JNEWBASE, NEW_BASELINE_ENABLE);
#else
	#ifdef JPEG_VLD_V3
	    VLD0_WRITE32(VLDREG_JNEWBASE, VLD_NEW_JENGINE);
	#endif
#endif
}

void VLDJPG_HwIsr(UINT16 u2Vector)
{
#ifdef JPEG_VLD_V4
	    UINT32 u4Value;

	    ASSERT(u2Vector == (UINT16)VECTOR_JPGDEC);
	    if (BIM_ClearIrq((UINT32)VECTOR_JPGDEC))
	    {
	        LOG(9, "JPG IRQ comming, Finish status[0x%x],ErrStatus[0x%x]\n",
				VLD0_READ32(VLDREG_ROW_PIC_DEC_FINISH),
				VLD0_READ32(VLDREG_JERR_STATUS));
	        if (VLD0_READ32(VLDREG_ROW_PIC_DEC_FINISH) == 0)
	        {
	            u4Value = VLD0_READ32(VLDREG_JERR_STATUS);
	            if (u4Value & (UINT32)(1 << VLD_ERR_ACLEN))
	            {
	                LOG(3, "*E* VLD_ERR_ACLEN\n");
	            }
	            if (u4Value & (UINT32)(1 << VLD_ERR_GT64))
	            {
	                LOG(3, "*E* VLD_ERR_GT64\n");
	            }
	            if (u4Value & (UINT32)(1 << VLD_ERR_RSTMKR))
	            {
	                LOG(3, "*E* VLD_ERR_ACSR\n");
	            }
	            if (u4Value & (UINT32)(1 << VLD_ERR_RSTMKR_EARLY))
	            {
	                LOG(3, "*E* VLD_ERR_RSTMKR\n");
	            }
	            if (u4Value & (UINT32)(1 << VLD_ERR_PROGRSSIVE_SE))
	            {
	                LOG(3, "*E* VLD_ERR_RSTMKR\n");
	            }
	            _fgJpgHwDetectError = TRUE;
	        }
	    }
#else
	#ifdef JPEG_VLD_V3
	    UINT32 u4Value;

	    ASSERT(u2Vector == (UINT16)VECTOR_JPGDEC);
	    if (BIM_ClearIrq((UINT32)VECTOR_JPGDEC))
	    {
	         LOG(9, "JPG IRQ comming, Finish status[0x%x],ErrStatus[0x%x]\n",
				VLD0_READ32(VLDREG_ROW_PIC_DEC_FINISH),
				VLD0_READ32(VLDREG_JERR_STATUS));
	        if (VLD0_READ32(VLDREG_ROW_PIC_DEC_FINISH) == 0)
	        {
	            u4Value = VLD0_READ32(VLDREG_JERR_STATUS);
	            if (u4Value & (UINT32)(1 << VLD_ERR_ACLEN))
	            {
	                LOG(3, "*E* VLD_ERR_ACLEN\n");
	            }
	            if (u4Value & (UINT32)(1 << VLD_ERR_GT64))
	            {
	                LOG(3, "*E* VLD_ERR_GT64\n");
	            }
	            if (u4Value & (UINT32)(1 << VLD_ERR_ACSR))
	            {
	                LOG(3, "*E* VLD_ERR_ACSR\n");
	            }
	            if (u4Value & (UINT32)(1 << VLD_ERR_DCRR))
	            {
	                LOG(3, "*E* VLD_ERR_RSTMKR\n");
	            }
	            if (u4Value & (UINT32)(1 << VLD_ERR_RSTMKR))
	            {
	                LOG(3, "*E* VLD_ERR_RSTMKR\n");
	            }
	            _fgJpgHwDetectError = TRUE;
	        }
	    }
	#endif   
#endif
	 if(_hIsrSeam)
	 {
	 	LOG(5, "@@@ _hIsrSeam unlock\n");
		 VERIFY (x_sema_unlock(_hIsrSeam) == OSR_OK);
	 }

    UNUSED(u2Vector);
}

void VLDJPG_SetErrorConcealment(void)
{
#ifdef JPEG_VLD_V4
	VLD0_WRITE32(VLDREG_JEC_EN, (1 << VLD_EC_GT64_EN) | (1 << VLD_EC_RSTMAK_EN)|(1<<VLD_EC_BEC_LEN_EN));
#else
	#ifdef JPEG_VLD_V3
	    VLD0_WRITE32(VLDREG_JEC_EN, (1 << VLD_EC_GT64_EN) | (1 << VLD_EC_RSTMAK_EN));
	#endif
#endif
}

void VLDJPG_SetErrorDetection(void)
{

#ifdef JPEG_VLD_V4
    x_os_isr_fct pfnOldIsr;
    UINT32 u4Value;
    static BOOL fgIRQ = FALSE;

    if (!fgIRQ)
    {
        VERIFY(x_reg_isr((UINT16)VECTOR_JPGDEC, VLDJPG_HwIsr, &pfnOldIsr) == OSR_OK);
        fgIRQ = TRUE;
    }
    
    u4Value = VLD0_READ32(VLDREG_JPEG_SWITCH);
    VLD0_WRITE32(VLDREG_JPEG_SWITCH, u4Value | (1 << VLD_INTMASK));

    //VLD0_WRITE32(VLDREG_JED_EN, (1 << VLD_ERR_DCRR) | (1 << VLD_ERR_ACSR) | (1 << VLD_ERR_ACLEN));
#else

	#ifdef JPEG_VLD_V3
	    x_os_isr_fct pfnOldIsr;
	    UINT32 u4Value;
	    static BOOL fgIRQ = FALSE;

	    if (!fgIRQ)
	    {
	        VERIFY(x_reg_isr((UINT16)VECTOR_JPGDEC, VLDJPG_HwIsr, &pfnOldIsr) == OSR_OK);
	        fgIRQ = TRUE;
	    }
	    
	    u4Value = VLD0_READ32(VLDREG_JPEG_SWITCH);
	    VLD0_WRITE32(VLDREG_JPEG_SWITCH, u4Value | (1 << VLD_INTMASK));

	    VLD0_WRITE32(VLDREG_JED_EN, (1 << VLD_ERR_DCRR) | (1 << VLD_ERR_ACSR) | (1 << VLD_ERR_ACLEN));
	#endif
#endif
}

INT32 VLDJPG_DetectError(void)
{
    if (_fgJpgHwDetectError)
    {
        _fgJpgHwDetectError = FALSE;
        return -(INT32)E_JPG_ERR_FATAL;
    }

    return (INT32)E_HWJPG_OK;
}

VOID VLDJPG_SetIsrSem(HANDLE_T hSeam)
{
     _hIsrSeam=hSeam;
}

BOOL VLDJPG_HwIsBusy(void)
{
    if((1<<9) == ((1<<9) & VLD0_READ32(VLDREG_JPEG_SWITCH)))
    {
        return TRUE;
    }
    return FALSE;
}

BOOL VLDJPG_CheckHwStop(void)
{
    UINT32 u4Value;

    u4Value = VLD0_READ32(VLDREG_WATCH_RP);
    x_thread_delay(1);
    if (u4Value == VLD0_READ32(VLDREG_WATCH_RP))        //wr ptr stop moving
    {
        return TRUE;
    }

    return FALSE;
}

INT32 VLDJPG_StopHw(void)
{    
#if defined(JPEG_VLD_V3)||defined(JPEG_VLD_V4)    
    UINT32 u4Value;
    UINT32 u2Cnt = 0;

    u4Value = VLD0_READ32(VLDREG_RESTART);
    VLD0_WRITE32(VLDREG_RESTART, u4Value | (1 << VLD_JSTOR));

    while (!(VLD0_READ32(VLDREG_RESTART) & (1 << VLD_JSTOP_RDY)))
    {
        u2Cnt++;
        x_thread_delay(1);
        if (u2Cnt > (UINT16)3000)
        {
            LOG(3, "*E* decoder HW stop timeout\n");
            break;
        }
    }
#else           
    while (VLD0_READ32(VLDREG_WATCH_RP) != VLD0_READ32(VLDREG_WATCH_WP))
    {               
        if (VLDJPG_CheckHwStop())
        {
            break;
        }
    }
#endif
    return (INT32)E_HWJPG_OK;
}


#ifdef JPEG_VLD_V4

INT32 VLDJPG_SetVideoOutMode(BOOL fgPadding16Byte, BOOL fgVideoOutput,UINT32 u4Comp0MCUWidth, UINT32 u4Comp1MCUWidth, UINT32 u4Comp0MCUHeight, UINT32 u4Comp1MCUHeight)
{
    UINT32 u4Reg;
    u4Reg = VLD0_READ32(VLDREG_JPG_BST_OUT_MODE);
    u4Reg |= JPG_ROW_BST_OUT_ON;
    if(fgPadding16Byte)
        u4Reg |= JPG_VIDEO_FORMAT_PADDING;
    if(fgVideoOutput)
        u4Reg |= JPG_VIDEO_FORMAT_OUTPUT;
    
	if ((u4Comp0MCUWidth == 2)&&(u4Comp1MCUWidth == 1))
	{
		if((u4Comp0MCUHeight == 2)&&(u4Comp1MCUHeight == 1)) //420 format
			u4Reg &= ~JPG_BST_FORMAT_SEL_422;
		else if ((u4Comp0MCUHeight == 1)&&(u4Comp1MCUHeight == 1))  //  422H format
			u4Reg |= JPG_BST_FORMAT_SEL_422;
		else
			return -(INT32)E_JPG_ERR_FATAL;
	}
	#ifdef JPEG_VLD_V5
	else if ((u4Comp0MCUWidth == 2)&&(u4Comp1MCUWidth == 2))
	{
		if((u4Comp0MCUHeight == 2)&&(u4Comp1MCUHeight == 1)) //422vx2 format
			u4Reg |= JPG_BST_FORMAT_SEL_422VX2;
		else
			return -(INT32)E_JPG_ERR_FATAL;
	}	
	else if((u4Comp0MCUWidth == 1)&&(u4Comp1MCUWidth == 1))
	{
		if((u4Comp0MCUHeight == 2)&&(u4Comp1MCUHeight == 1)) //422V format
		{
			u4Reg |= JPG_BST_FORMAT_SEL_422V;
		}
		else
			return -(INT32)E_JPG_ERR_FATAL;
	}
	#endif
	else
	{
		return -(INT32)E_JPG_ERR_FATAL;
	}
	
    VLD0_WRITE32(VLDREG_JPG_BST_OUT_MODE, u4Reg);

  return (INT32)E_HWJPG_OK;

	
}

INT32 VLDJPG_SetPicturePixelSize(UINT32 u4PicPixelSize)
{
    VLD0_WRITE32(VLDREG_JPG_PIC_PIX_WIDTH, u4PicPixelSize);
    return (INT32)E_HWJPG_OK;
}

#ifdef MT5368_JPG_PARTIAL_OUTPUT
INT32 VLDJPG_SetPartialMode_StartRow(UINT32 u4StartRow)
{
	UINT32 start_row_num=0;
	start_row_num = u4StartRow&JPG_VERT_START_MB_MASK;

	if(start_row_num%2==1)
	{
		VLD0_WRITE32(VLDREG_JPG_PARTIALMODE, JPG_PARTIAL_MODE|JPG_REV_MCU_ROW_STA|JPG_REV_MCU_ROW_BUSY|start_row_num);
	}
	else
	{
    	VLD0_WRITE32(VLDREG_JPG_PARTIALMODE, JPG_PARTIAL_MODE|start_row_num);
	}
    return (INT32)E_HWJPG_OK;
}
#endif
extern void HalSet4KPageTable(UINT32 u4PhyAddr, UINT32 u4VirtAdr, UINT32* pu4PageTableAddress);
extern void HalSet1MSectionTable(UINT32 u4PhyAddr, UINT32 u4VirtAdr);
extern UINT32 HalGetMMUTableAddress(void);
#define JPEG_READ_AGENT 0
#define JPEG_WRITE_AGENT 1
static BOOL fgJpgIOMMUISRInited = FALSE;


void JPEG_IOMMU_IrqHandle(UINT16 u2Vector)
{
    if (fgJpgIOMMUISRInited == 0)
    {
        //return;
        Printf("RZ IOMMU fgIOMMUISRInited=FALSE \n");
        ASSERT(0);
    }

    if (u2Vector != VECTOR_MMU_JPG)
    {
      	 Printf("RZ IOMMU u2Vector false \n");
        ASSERT(0);
    }

    Printf("RZ IOMMU page fault occurs\n");
    //clear IOMMU status bit
    JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFG4, JPEG_MMU_READ32(JPEG_REG_RW_IOMMU_CFG4) | 0x5);    
    JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFG4, JPEG_MMU_READ32(JPEG_REG_RW_IOMMU_CFG4) & ~(0x5));   
    //clear BIM status bit
    VERIFY(BIM_ClearIrq(u2Vector));
}

void  vJpgIOMMU_OverRead_Protection(UINT32 ui4_Agent,
                                                                     UINT32 ui4_StartAddr0,
                                                                     UINT32 ui4_Buff_sz0,
                                                                     UINT32 ui4_StartAddr1,
                                                                     UINT32 ui4_Buff_sz1)
{

	//method 0
	/*
	IMGRZ_MMU_WRITE32(RZ_REG_RW_IOMMU_CFG9, ((ui4_StartAddr0+ui4_Buff_sz0)&0xfffff000) | 0x1); 
	IMGRZ_MMU_WRITE32(RZ_REG_RW_IOMMU_CFGA, ((ui4_StartAddr1+ui4_Buff_sz1)&0xfffff000) | 0x3); 
	IMGRZ_MMU_WRITE32(RZ_REG_RW_IOMMU_CFGB, 0x21); 
	*/
	//method 1
	
	JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFG9, ((ui4_StartAddr0+ui4_Buff_sz0)&0xfffff000) | 0x1); 
	JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFGC, (ui4_StartAddr0&0xfffff000)); 
	
	JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFGA, ((ui4_StartAddr1+ui4_Buff_sz1)&0xfffff000) | 0x3); 
	JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFGD, (ui4_StartAddr1&0xfffff000)); 	
	JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFGB, 0x10021); 
	
}

#ifdef JDEC_HW_IO_MMU
void VLDJPG_SetMMU(void)
{
     UINT32 u4RegVal = 0;
     UINT32 i = 0;
     x_os_isr_fct pfnOldIsr;

#define USING_1M_PAGE
#ifdef USING_1M_PAGE
	//==================================================================
	//test 1M page	
	for(i=0;i<16;i++)
	{
    	HalSet1MSectionTable(0x3000000+i*0x100000, 0x2000000+i*0x100000);
    	HalSet1MSectionTable(0x2000000+i*0x100000, 0x3000000+i*0x100000);
    	HalSet1MSectionTable(0x5000000+i*0x100000, 0x4000000+i*0x100000);
    	HalSet1MSectionTable(0x4000000+i*0x100000, 0x5000000+i*0x100000);    	
	}
    Printf("[JPG_VFY] Using 1M page\n");    
#else
	//==================================================================
	//test 4K page
    UINT32 * u4PageTableAdd;
    u4PageTableAdd = (UINT32 *)BSP_AllocAlignedDmaMemory(0x4000, 0x10000);

    for (i = 0; i < 0x1000; i++)
    {
        HalSet4KPageTable(
            PHYSICAL((UINT32)0x3600000+ (0xFFF - i)*0x1000),
            ((UINT32)0x3600000 + i * 0x1000),
            u4PageTableAdd
        );
    }
    Printf("[JPG_VFY] Using 4K page\n");
#endif	

	/*
	//page fault case
    x_memset((void*)0xa000000, 0, 0x400);
	HalSet4KPageTable(0x7000000, 0x7000000, (UINT32*)0xa000000); //phy=184M, virt=168M
	HalSet4KPageTable(0x7001000, 0x7001000, (UINT32*)0xa000000); //phy=184M, virt=168M
	HalSet4KPageTable(0x7002000, 0x7002000, (UINT32*)0xa000000); //phy=184M, virt=168M
	HalSet4KPageTable(0x7003000, 0x7003000, (UINT32*)0xa000000); //phy=184M, virt=168M
	HalSet4KPageTable(0x7004000, 0x7004000, (UINT32*)0xa000000); //phy=184M, virt=168M
	
	HalSet4KPageTable(0x8005000, 0x7005000, (UINT32*)0xa000000); //phy=184M, virt=168M
	HalSet4KPageTable(0x8006000, 0x7006000, (UINT32*)0xa000000); //phy=184M, virt=168M
	HalSet4KPageTable(0x8007000, 0x7007000, (UINT32*)0xa000000); //phy=184M, virt=168M	

	HalSet4KPageTable(0x7005000, 0x8005000, (UINT32*)0xa000000); //phy=184M, virt=168M
	HalSet4KPageTable(0x7006000, 0x8006000, (UINT32*)0xa000000); //phy=184M, virt=168M
	HalSet4KPageTable(0x7007000, 0x8007000, (UINT32*)0xa000000); //phy=184M, virt=168M		
   */
   
	//==================================================================
	
	JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFG4, 0x0020010A);

	//enable mmu
	u4RegVal = 0xFE|(0x1<<0);
	JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFG0, u4RegVal);     
	u4RegVal = JPEG_MMU_READ32(JPEG_REG_RW_IOMMU_CFG0);   
	printf("[JPG_VFY] MMU CFG0 is %x . \n",u4RegVal);

	JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFG1, HalGetMMUTableAddress());
	u4RegVal = JPEG_MMU_READ32(JPEG_REG_RW_IOMMU_CFG1);   
	printf("[JPG_VFY] MMU CFG1 is %x . \n",u4RegVal);

	JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFG2, 0x0b190715);
	u4RegVal =JPEG_MMU_READ32(JPEG_REG_RW_IOMMU_CFG2);   
	printf("[JPG_VFY] MMU CFG2 is %x . \n",u4RegVal);

	JPEG_MMU_WRITE32(JPEG_REG_RW_IOMMU_CFG3, 0x0000001d);
	u4RegVal = JPEG_MMU_READ32(JPEG_REG_RW_IOMMU_CFG3);   
	printf("[JPG_VFY] MMU CFG3 is %x . \n",u4RegVal);	 

   if (fgJpgIOMMUISRInited == 0)
    {
        fgJpgIOMMUISRInited = 1;
        //i4GCPU_HAL_ClrIntr(GCPU_INT_MASK);
        BIM_ClearIrq(VECTOR_MMU_JPG);
        
        if (x_reg_isr(VECTOR_MMU_JPG, JPEG_IOMMU_IrqHandle, &pfnOldIsr) != OSR_OK)
        {
        	  Printf("JPG IOMMU x_reg_isr create error \n");	
            ASSERT(0);
        }
    }

	//vJpgIOMMU_OverRead_Protection(JPEG_WRITE_AGENT, 0xa7fc000, 253008, 0xaffc000, 0x1000);

}
#endif	//JDEC_HW_IO_MMU

UINT32 VLDJPG_ReadErrorType(void)
{
    return VLD0_READ32(VLDREG_JERR_STATUS);
}

UINT32 VLDJPG_ReadCKsum(void)
{
    return VLD0_READ32(VLDREG_JPEG_WINTF_CHKSUM);
}

UINT32 VLDJPG_ReadInputCKsum(void)
{
    return VLD0_READ32(VLDREG_JPEG_INPUT_CHKSUM);
}

UINT32 VLDJPG_BsInputCKsum(void)
{
    return VLD0_READ32(VLDREG_JPEG_BS_DDRAM_IN_CHKSUM);
}

UINT32 VLDJPG_HuffCKsum(void)
{
    return VLD0_READ32(VLDREG_JPEG_HUFFDEC_CHKSUM);
}

UINT32 VLDJPG_VldOutCKsum(void)
{
    return VLD0_READ32(VLDREG_JPEG_VLD_OUT_CHKSUM);
}

UINT32 VLDJPG_IdctOutCKsum(void)
{
    return VLD0_READ32(VLDREG_JPEG_IDCT_OUT_CHKSUM);
}

UINT32 VLDJPG_CoefCKsum(void)
{
    return VLD0_READ32(VLDREG_JPEG_COEFF_DRAM_IN_CHKSUM);
}

UINT32 VLDJPG_NzInCKsum(void)
{
    return VLD0_READ32(VLDREG_JPEG_NZ_IN_CHKSUM);
}

UINT32 VLDJPG_NzOutCKsum(void)
{
    return VLD0_READ32(VLDREG_JPEG_NZ_OUT_CHKSUM);
}

UINT32 VLDJPG_ReadCRC(void)
{
    return VLD0_READ32(VLDREG_JPEG_WINTF_CRC);
}

UINT32 VLDJPG_ReadCurMcuRow(void)
{
    UINT32 u4CurMcuRow;
	u4CurMcuRow= (VLD0_READ32(VLDREG_CUR_MCU_NUM) << VLD_MCU_ROW ) & VLD_MCU_ROW_MASK;
    return u4CurMcuRow;
}
#endif

