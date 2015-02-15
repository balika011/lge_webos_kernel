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
/*!
 * @file ir_regs.h
 *
 *
 *
 * @par Project
 *    MT8530
 *
 * @par Description
 *    IR RX Register Definition
 *
 * @par Author_Name
 *    Zhifeng Zhao
 *
 * @par Last_Changed
 * $Author: p4admin $
 * $Modtime: $
 * $Revision: #1 $
 *
 */
#ifndef __IR_REGS_H__
#define __IR_REGS_H__



#include "x_hal_ic.h"
#include "drv_config.h"
#include "chip_ver.h"
#include "x_typedef.h"

/**************************************************
    IR protocol & user define
    ************************************************/

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8563)
#define CONFIG_ARM2_EJECT     1
#define IRRX_USE_27M          0
#endif

#define IRRX_RC_NEC  (0x00)
#define IRRX_RC_RC6  (0x01)
#define IRRX_RC_RC5  (0x02)
#define IRRX_RC_PAN  (0X03)
#define IRRX_RC_SIRC (0x04)
#define IRRX_RC_JVC  (0x05) 
#define IRRX_RC_RCA  (0x06) 
#define IRRX_RC_SAS  (0x07) 


#define IRRX_RC_PROTOCOL  IRRX_RC_NEC



#if (IRRX_RC_PROTOCOL == IRRX_RC_NEC )
#define IRRX_POLL_SPACE_1ST    (70)
#define IRRX_POLL_SPACE_2ST    (115)
#elif (IRRX_RC_PROTOCOL == IRRX_RC_JVC )
#define IRRX_POLL_SPACE_1ST    (70)
#define IRRX_POLL_SPACE_2ST    (70)
#elif (IRRX_RC_PROTOCOL == IRRX_RC_PAN )
#define IRRX_POLL_SPACE_1ST    (165)
#define IRRX_POLL_SPACE_2ST    (165)
#elif (IRRX_RC_PROTOCOL == IRRX_RC_SIRC )
#define IRRX_POLL_SPACE_1ST    (150)
#define IRRX_POLL_SPACE_2ST    (50)
#elif (IRRX_RC_PROTOCOL == IRRX_RC_RCA)
#define IRRX_POLL_SPACE_1ST    (70)
#define IRRX_POLL_SPACE_2ST    (70)
#elif (IRRX_RC_PROTOCOL == IRRX_RC_SAS)
#define IRRX_POLL_SPACE_1ST    (125)
#define IRRX_POLL_SPACE_2ST    (125)
#else
#define IRRX_POLL_SPACE_1ST    (112)
#define IRRX_POLL_SPACE_2ST    (112)
#endif


#if (IRRX_RC_PROTOCOL == IRRX_RC_RC6 ) 
extern UINT32 _IRRX_XferRC6ToCrystal(UINT32 u4Info, const UINT8 * pu1Data, BOOL u1NeedRelease);
extern INT32 _IRRX_XferCrystalToRC6(UINT32 u4CrystalKey, UINT32* pu4RC6IrM, UINT32* pu4RC6IrL);

#elif (IRRX_RC_PROTOCOL == IRRX_RC_SIRC ) 

extern UINT32 _IRRX_XferSIRCToCrystal(UINT32 u4Info, const UINT8 * pu1Data, BOOL u1NeedRelease);
extern INT32 _IRRX_XferCrystalToSIRC(UINT32 u4CrystalKey, UINT32* pu4SIRCIrM, UINT32* pu4SIRCIrL);

#elif (IRRX_RC_PROTOCOL == IRRX_RC_JVC)
extern UINT32 _IRRX_XferJVCToCrystal(UINT32 u4Info, const UINT8 * pu1Data, BOOL u1NeedRelease);
extern INT32 _IRRX_XferCrystalToJVC(UINT32 u4CrystalKey, UINT32* pu4MtkIrM, UINT32* pu4MtkIrL);

#elif (IRRX_RC_PROTOCOL == IRRX_RC_PAN)

extern UINT32 _IRRX_XferPANToCrystal(UINT32 u4Info, const UINT8 * pu1Data, BOOL u1NeedRelease)
extern INT32 _IRRX_XferCrystalToPAN(UINT32 u4CrystalKey, UINT32* pu4MtkIrM, UINT32* pu4MtkIrL);

#elif(IRRX_RC_PROTOCOL == IRRX_RC_RC5 )  

extern UINT32 _IRRX_XferRC5ToCrystal(UINT32 u4Info, const UINT8 * pu1Data, BOOL u1NeedRelease);	

extern INT32 _IRRX_XferCrystalToRC5(UINT32 u4CrystalKey, UINT32* pu4RC5IrM, UINT32* pu4RC5IrL);

#elif (IRRX_RC_PROTOCOL == IRRX_RC_RCA)
extern UINT32 _IRRX_XferRCAToCrystal(UINT32 u4Info, const UINT8 * pu1Data, BOOL u1NeedRelease);
extern INT32 _IRRX_XferCrystalToRCA(UINT32 u4CrystalKey, UINT32* pu4MtkIrM, UINT32* pu4MtkIrL);
#elif (IRRX_RC_PROTOCOL == IRRX_RC_SAS)

extern UINT32 _IRRX_XferSASToCrystal(UINT32 u4Info, const UINT8 * pu1Data, BOOL u1NeedRelease);
extern INT32 _IRRX_XferCrystalToSAS(UINT32 u4CrystalKey, UINT32* pu4MtkIrM, UINT32* pu4MtkIrL);

#elif (IRRX_RC_PROTOCOL == IRRX_RC_NEC ) 

extern UINT32 _IRRX_XferMtkToCrystal(UINT32 u4Info, const UINT8 * pu1Data, BOOL u1NeedRelease);
extern INT32 _IRRX_XferCrystalToMtk(UINT32 u4CrystalKey, UINT32* pu4MtkIrM, UINT32* pu4MtkIrL);
#endif




#if(IRRX_RC_PROTOCOL == IRRX_RC_RC6)

#if (IRRX_USE_27M)
#define IRRX_SAPERIOD_RC6 (UINT32)(0x2e)
#else
#define IRRX_SAPERIOD_RC6 (UINT32)(0xA6)
#endif
#define IRRX_RC6_CONFIG   (IRRX_CH_END_15 | IRRX_CH_IGSYN | IRRX_CH_HWIR | IRRX_CH_ORDINV | IRRX_CH_RC5)
#define IRRX_RC6_BITCNT   (UINT32)(0x1e)
#define IRRX_RC6_LEADER   (UINT32)(0x8)
#define IRRX_RC6_TOGGLE0  (UINT32)(0x1)
#define IRRX_RC6_TOGGLE1  (UINT32)(0x2)
#define IRRX_RC6_CUSTOM (UINT32)(0x32)

#define IRRX_RC6_GET_LEADER(bdata0) ((bdata0>>4))
#define IRRX_RC6_GET_TOGGLE(bdata0) ((bdata0 & 0xc)>>2)
#define IRRX_RC6_GET_CUSTOM(bdata0,bdata1) (((bdata0 & 0x3) << 6) |bdata1 >> 2)
#define IRRX_RC6_GET_KEYCODE(bdata1,bdata2)  \
                (((bdata2>>2) | ((bdata1 & 0x3)<<6)) & 0xff)

#define IRRX_RC6_MAX_MAP_ENTRY   (0xFF)

#elif(IRRX_RC_PROTOCOL == IRRX_RC_SIRC)

#if (IRRX_USE_27M)
#define IRRX_SAPERIOD_SIRC (UINT32)(0x30)  
#else
#define IRRX_SAPERIOD_SIRC (UINT32)(0xAB)
#endif
#define IRRX_SIRC_CONFIG   (IRRX_CH_END_15 | IRRX_CH_ORDINV | IRRX_CH_HWIR)

#define IRRX_SIRC_BITCNT12 (UINT32)(0xc)
#define IRRX_SIRC_12B_DEVICE    ((UINT8)(0x01))

#define IRRX_SIRC_BITCNT15 (UINT32)(0xf)
#define IRRX_SIRC_15B_DEVICE    ((UINT8)(0x01))

#define IRRX_SIRC_BITCNT20 (UINT32)(0x14)
#define IRRX_SIRC_20B_DEVICE    ((UINT8)(0x1A))
#define IRRX_SIRC_20B_EXTENDED  ((UINT8)(0xE2))

#define IRRX_SIRC_MAX_MAP_ENTRY   (0x80)

#elif(IRRX_RC_PROTOCOL == IRRX_RC_NEC)

//NEC config
#define MTK_IRRX_CONFIG         (IRRX_CH_END_15 + IRRX_CH_IGSYN + IRRX_CH_HWIR)
#if (IRRX_USE_27M)
//NEC sampling period 0x32 = 560us/9.5us
#define MTK_IRRX_SAPERIOD       (0x0032)
#else
#define MTK_IRRX_SAPERIOD       (0x00B4)
#endif
#define MTK_NEC_MAX_MAP_ENTRY   (0xFF)

#define MTK_IRRX_1st_Plus_REPEAT  (3)
#define MTK_IRRX_BITCNT_NORMAL    (33)
#define MTK_IRRX_BITCNT_REPEAT    (1)
#define MTK_IRRX_BIT8_VERIFY      (0xff)

#define MTK_IRRX_GRPID_DVD    (0xff00)

#elif(IRRX_RC_PROTOCOL == IRRX_RC_JVC)

#if (IRRX_USE_27M)
#define IRRX_SAPERIOD_JVC (UINT32)(0x30)
#else
#define IRRX_SAPERIOD_JVC (UINT32)(0xB0)
#endif

#define IRRX_JVC_CONFIG            (IRRX_CH_END_15 + IRRX_CH_HWIR)

#define IRRX_JVC_BITCNT_NORMAL     (UINT32)(0x11)
#define IRRX_JVC_BITCNT_REPEAT     (UINT32)(0x10)
#define IRRX_JVC_1ST_PULSE_NORMAL  (UINT32)(0x8)
#define IRRX_JVC_1ST_PULSE_REPEAT  (UINT32)(0x2)
#define IRRX_JVC_CUSTOM            (UINT32)(0xEF)

#define IRRX_JVC_MAX_MAP_ENTRY   (0xFF)

#elif(IRRX_RC_PROTOCOL == IRRX_RC_PAN)

#if (IRRX_USE_27M)
#define IRRX_SAPERIOD_PAN (UINT32)(0x26)
#else
#define IRRX_SAPERIOD_PAN (UINT32)(0x8A)
#endif

#define IRRX_PAN_CONFIG            (IRRX_CH_END_15 + IRRX_CH_IGSYN +  +IRRX_CH_HWIR)

#define IRRX_PAN_BITCNT_NORMAL     (UINT32)(0x31)
#define IRRX_PAN_BITCNT_REPEAT     (UINT32)(0x10)
#define IRRX_PAN_1ST_PULSE_NORMAL  (UINT32)(0x8)
#define IRRX_PAN_1ST_PULSE_REPEAT  (UINT32)(0x2)
#define IRRX_PAN_CUSTOM            (UINT32)(0x2002)

#define IRRX_PAN_MAX_MAP_ENTRY   (0xFF)

#elif(IRRX_RC_PROTOCOL == IRRX_RC_RC5)

#if (IRRX_USE_27M)
#define IRRX_SAPERIOD_RC5 (UINT32)(0x60)
#else
#define IRRX_SAPERIOD_RC5 (UINT32)(0xB0)
#endif
#define IRRX_RC5_CONFIG   (IRRX_CH_IGSYN | IRRX_CH_HWIR | IRRX_CH_ORDINV | IRRX_CH_RC5)

#define IRRX_RC5_BITCNT   (UINT32)(0x10)
#define IRRX_RC5_CUSTOM   (UINT32)(0x00)

#define IRRX_RC5_GET_TOGGLE(bdata0) (((~(bdata0)) & 0x80) >> 7)
#define IRRX_RC5_GET_CUSTOM(bdata0) (((~(bdata0)) & 0x7C) >> 2)
#define IRRX_RC5_GET_KEYCODE(bdata0,bdata1)  \
                ((((~(bdata0)) & 0x03) << 4) | (((~(bdata1)) & 0xF0) >> 4))

#define IRRX_RC5_MAX_MAP_ENTRY   (0x40)


#elif(IRRX_RC_PROTOCOL == IRRX_RC_RCA)

#if (IRRX_USE_27M)
#define IRRX_SAPERIOD_RCA (UINT32)(0x26)
#else
#define IRRX_SAPERIOD_RCA (UINT32)(0x96)
#endif

#define IRRX_RCA_CONFIG            (IRRX_CH_END_15 + IRRX_CH_IGSYN +  +IRRX_CH_HWIR)

#define IRRX_RCA_BITCNT_NORMAL     (UINT32)(0x19)
#define IRRX_RCA_BITCNT_REPEAT     (UINT32)(0x10)
#define IRRX_RCA_1ST_PULSE_NORMAL  (UINT32)(0x9)
#define IRRX_RCA_1ST_PULSE_REPEAT  (UINT32)(0x2)
#define IRRX_RCA_CUSTOM            (UINT32)(0x3)

#define IRRX_RCA_MAX_MAP_ENTRY   (0xFF)

#elif(IRRX_RC_PROTOCOL == IRRX_RC_SAS)

//SAS config
#define IRRX_SAS_CONFIG         (IRRX_CH_END_15 + IRRX_CH_IGSYN + IRRX_CH_HWIR)
#if (IRRX_USE_27M)
//SAS sampling period 0x32 = 560us/9.5us
#define IRRX_SAPERIOD_SAS       (0x0032)
#else
#define IRRX_SAPERIOD_SAS       (0x00E1)
#endif
#define IRRX_MAX_MAP_ENTRY_SAS   (0xFF)
#define IRRX_SAS_1ST_PULSE_NORMAL  (UINT32)(0x7)

#define IRRX_BITCNT_NORMAL_SAS    (38)
#define IRRX_BIT8_VERIFY_SAS      (0xff)

#define IRRX_CUSTOM_SAS            (UINT32)(0x2000)
#define IRRX_GRPID_BD_SAS    		      (UINT32)(0x7)

#else

#error "Error: please set IR protocol!"

#endif

//set deglitch with the max number.
#define MTK_IRRX_THRESHOLD      (0x0201)    

/* if there is no key in 400ms, poll function will get a 0xffffffff key. */
#define MTK_IRRX_TIMESLICE      (400)   
#define MTK_IRRX_PRIORITY       (100)
#define MTK_IRRX_ITEMCNT        (16)

#define MAX_IRRX_DATA        (4)

/**************************************************
    IR clock select
    ************************************************/

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8563)  

#define IRRX_UP_CFG              0x05C
#define FAST_CK_EN             (0x1<<3)//1: 27M; 0:3M
#define PD_CLK_SEL             0x00000007    // for puse width detection clock
#endif


#define IRRX_CLKPDN              0x040
  #define IRRXPD                 (0x01<<1) //IR receiver module clock stop
  
#define IRRX_IRCKSEL             0x044
  #define IRCLKSEL_MASK          0xf
  
  #define CLK_SEL_IR_DIV_1_1		 0x0		 
  #define CLK_SEL_IR_DIV_1_2	     0x1		
  #define CLK_SEL_IR_DIV_1_4	     0x2		
  #define CLK_SEL_IR_DIV_1_8	     0x3		
  #define CLK_SEL_IR_DIV_1_16	     0x4		
  #define CLK_SEL_IR_DIV_1_32	     0x5		
  #define CLK_SEL_IR_DIV_1_64	     0x6		
  #define CLK_SEL_IR_DIV_1_128       0x7		
  #define CLK_SEL_IR_DIV_1_256       0x8

/**************************************************
  IR interrupt , level 2 (level 1 is VECTOR_PWDNC)
  ************************************************/
#define IRRX_INTSTA             0x140
  #define IR_INT                (0x01<<13)

#define IRRX_INTEN              0x144
  #define IR_INTEN              (0x01<<13)

#define IRRX_INTCLR             0x148
  #define IR_INTCLR             (0x01<<13)

/**************************************************
    IRRX register define
    ************************************************/
#define IRRX_COUNT_HIGH_REG        0x200  
  #define IRRX_CH_BITCNT_MASK         0x0000003f
  #define IRRX_CH_BITCNT_BITSFT       0
  #define IRRX_CH_1ST_PULSE_MASK      0x0000ff00
  #define IRRX_CH_1ST_PULSE_BITSFT    8
  #define IRRX_CH_2ND_PULSE_MASK      0x00ff0000
  #define IRRX_CH_2ND_PULSE_BITSFT    16
  #define IRRX_CH_3RD_PULSE_MASK      0xff000000
  #define IRRX_CH_3RD_PULSE_BITSFT    24
  
#define IRRX_COUNT_MID_REG         0x204
#define IRRX_COUNT_LOW_REG         0x208

#define IRRX_CONFIG_HIGH_REG     0x20c
  #define IRRX_CH_DISPD        ((INT32)(1 << 15)) 
  #define IRRX_CH_IGB0         ((INT32)(1 << 14))
  #define IRRX_CH_END_7        ((INT32)(0x00 << 8))


#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8563)  
#define IRRX_CH_END_15       ((INT32)(0x0f << 16))  //2420c[22:16]
#define IRRX_CH_END_23		 ((INT32)(0x17 << 16))
#define IRRX_CH_END_31		 ((INT32)(0x1f << 16))
#define IRRX_CH_END_39		 ((INT32)(0x27 << 16))
#define IRRX_CH_END_47		 ((INT32)(0x2f << 16))
#define IRRX_CH_END_55		 ((INT32)(0x37 << 16))
#define IRRX_CH_END_63		 ((INT32)(0x3f << 16))
#define IRRX_ENABLE_PULSE_WIDTH  ((INT32)(0x1 << 13))
#endif

  
  #define IRRX_CH_DISCH        ((INT32)(1 << 7))
  #define IRRX_CH_DISCL        ((INT32)(1 << 6))
  #define IRRX_CH_IGSYN        ((INT32)(1 << 5))
  #define IRRX_CH_ORDINV       ((INT32)(1 << 4))
  #define IRRX_CH_RC5_1ST      ((INT32)(1 << 3))
  #define IRRX_CH_RC5          ((INT32)(1 << 2))
  #define IRRX_CH_IRI          ((INT32)(1 << 1))
  #define IRRX_CH_HWIR         ((INT32)(1 << 0))

#define IRRX_CONFIG_LOW_REG       (0x210)
#define IRRX_THRESHOLD_REG        (0x214)
    #define IRRX_GD_DEL_MASK      (0x0300)
	#define IRRX_ICLR             (0x80)
    #define IRRX_THRESHOLD_MASK   (0x7f)

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8563)

#define IRRX_PUSLE_WIDTH_PERIOD_MASK  0x001fff00    [20:8]

#endif




#define IRRX_IRCLR                (0x218)
    #define IRCLR                 (0x1)

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8563)
#define IRRX_WAKEN             0x080
  #define IR_WAKEN             (0x1<<8)
  
#define IRRX_PDSTAT            0x088
  #define IR_IR_WAK            (0x1<<8)

#define IRRX_PDSTCLR            0x08C

#define IRRX_IREXP_EN           ((UINT32)(0x240))
    #define PD_IREXPEN_IR0        (((UINT32)(1))<<10)
	#define PD_IREXPEN_IR1        (((UINT32)(1))<<11)
	#define PD_IRPDWN_EN          (((UINT32)(1))<<9)
	#define WU_BCEPEN             (((UINT32)(1))<<8)
	#define WU_IREXPEN_IR0        (((UINT32)(1))<<0)
	#define WU_IREXPEN_IR1        (((UINT32)(1))<<1)
	#define WU_IREXPEN_IR2        (((UINT32)(1))<<2)
	#define WU_IREXPEN_IR3        (((UINT32)(1))<<3)
	#define WU_IREXPEN_IR4        (((UINT32)(1))<<4)
	#define WU_IREXPEN_IR5        (((UINT32)(1))<<5)
	#define WU_IREXPEN_IR6        (((UINT32)(1))<<6)
	#define WU_IREXPEN_IR7        (((UINT32)(1))<<7)

#define IRRX_ENEXP_IRM          ((UINT32)(0x244))
#define IRRX_ENEXP_IRL          ((UINT32)(0x248))

#define IRRX_EXP_BCNT           ((UINT32)(0x24C))

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8563)
 
#define  IRRX_EXP_CHK_CNT_MASK     0x1fb0     [12:6]  
#define  IRRX_PULSE_DATA0         (0x3B0)
#define  IRRX_PULSE_DATA1         (0X3B4)
#define  IRRX_PULSE_DATA2         (0X3B8)
#define  IRRX_PULSE_DATA3         (0X3BC)
#define  IRRX_PULSE_DATA4         (0X3C0)
#define  IRRX_PULSE_DATA5         (0X3C4)
#define  IRRX_PULSE_DATA6         (0X3C8)
#define  IRRX_PULSE_DATA7         (0X3CC)
#define  IRRX_PULSE_DATA8         (0X3D0)
#define  IRRX_PULSE_DATA9         (0X3D4)
#define  IRRX_PULSE_DATA10        (0X3D8)
#define  IRRX_PULSE_DATA11        (0X3DC)
#define  IRRX_PULSE_DATA12        (0X3E0)
#define  IRRX_PULSE_DATA13        (0X3E4)
#define  IRRX_PULSE_DATA14        (0X3E8)
#define  IRRX_PULSE_DATA15        (0X3EC)
#define  IRRX_PULSE_DATA16        (0X3F0)
#endif

#define IRRX_PDWNCNT            ((UINT32)(0x250))
#define IRRX_EXP_IRM0          ((UINT32)(0x280))
#define IRRX_EXP_IRL0          ((UINT32)(0x284))
#define IRRX_EXP_IRM1          ((UINT32)(0x288))
#define IRRX_EXP_IRL1          ((UINT32)(0x28C))
#define IRRX_EXP_IRM2          ((UINT32)(0x290))
#define IRRX_EXP_IRL2          ((UINT32)(0x294))
#define IRRX_EXP_IRM3          ((UINT32)(0x298))
#define IRRX_EXP_IRL3          ((UINT32)(0x29C))
#define IRRX_EXP_IRM4          ((UINT32)(0x2A0))
#define IRRX_EXP_IRL4          ((UINT32)(0x2A4))
#define IRRX_EXP_IRM5          ((UINT32)(0x2A8))
#define IRRX_EXP_IRL5          ((UINT32)(0x2AC))
#define IRRX_EXP_IRM6          ((UINT32)(0x2B0))
#define IRRX_EXP_IRL6          ((UINT32)(0x2B4))
#define IRRX_EXP_IRM7          ((UINT32)(0x2B8))
#define IRRX_EXP_IRL7          ((UINT32)(0x2BC))
#define IRRX_EXPD_IRM0          ((UINT32)(0x2C0))
#define IRRX_EXPD_IRL0          ((UINT32)(0x2C4))
#define IRRX_EXPD_IRM1          ((UINT32)(0x2C8))
#define IRRX_EXPD_IRL1          ((UINT32)(0x2CC))

#endif	
/**************************************************
    IR device register read/write macros
    ************************************************/
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8563)
#define IR_REGISTER_BASE         PDWNC_BASE
#define IR_BASE                  PDWNC_BASE
#endif

#define IR_REG_LENGTH            0

#define IR_WRITE32(i4Addr, u4Val)  IO_WRITE32(IR_REGISTER_BASE, i4Addr, u4Val)   
#define IR_READ32(i4Addr)          IO_READ32(IR_REGISTER_BASE, i4Addr)

#endif /* __IRRX_VRF_HW_H__ */

 

