
#ifndef _HW_LBOX_H_
#define _HW_LBOX_H_

#include "hw_vdoin.h"


#define LBOX_IO_PREPROC_BASE   (VDOIN_BASE+0x3000)


// LETTERBOX
#define LBOX_00 (LBOX_IO_PREPROC_BASE + 0x500)
    #define LBOX_START Fld(1,31,AC_MSKB3)//[31:31]
    #define LBOX_STATE Fld(1,30,AC_MSKB3)//[30:30]
    #define FORCE_FCNT Fld(1,29,AC_MSKB3)//[29:29]
	#define LBOX_LOW_BOUND Fld(11,12,AC_MSKW21)//[22:12]
	#define LBOX_UP_BOUND Fld(11,0,AC_MSKW10)//[10:0]

    #define LBOX_LOW_BOUND_4K Fld(12,12,AC_MSKW21)//[23:12]   //Gazelle
    #define LBOX_UP_BOUND_4K Fld(12,0,AC_MSKW10)//[11:0]	  //Gazelle

#define LBOX_01 (LBOX_IO_PREPROC_BASE + 0x504)
    #define Y_THRES Fld(8,24,AC_FULLB3)//[31:24]
    #define LBOX_RIGHT_BOUND Fld(11,12,AC_MSKW21)//[22:12]    //oryx_only
    #define LBOX_LEFT_BOUND Fld(11,0,AC_MSKW10)//[10:0]       //oryx_only
#define LBOX_02 (LBOX_IO_PREPROC_BASE + 0x508)
	#define LBOX_MIDBOUND_LOW Fld(11,12,AC_MSKW21)//[22:12]
	#define LBOX_MIDBOUND_UP Fld(11,0,AC_MSKW10)//[10:0]

	#define LBOX_MIDBOUND_LOW_4K Fld(12,12,AC_MSKW21)//[23:12]	 //gazelle
	#define LBOX_MIDBOUND_UP_4K Fld(12,0,AC_MSKW10)//[11:0] 	 //gazelle

#define LBOX_04 (0x0560)                                         //gazelle
    #define LBOX_RIGHT_BOUND_4K Fld(13,13,AC_MSKDW)//[25:13]        //Gazelle
    #define LBOX_LEFT_BOUND_4K Fld(13,0,AC_MSKW10)//[12:0]          //Gazelle


#define LBOX_03 (LBOX_IO_PREPROC_BASE + 0x50C)
    #define FCNT_BOUND Fld(10,0,AC_MSKW10)//[9:0]
#define SIDE_LBOX_00 (LBOX_IO_PREPROC_BASE + 0x510)
    #define C_START Fld(1,31,AC_MSKB3)//[31:31]
    #define C_UP_BOUND Fld(11,12,AC_MSKW21)//[22:12]
    #define C_LOW_BOUND Fld(11,0,AC_MSKW10)//[10:0]

	#define C_UP_BOUND_4K	Fld(12,12,AC_MSKW21)//[23:12]	 //Gazelle
	#define C_LOW_BOUND_4K Fld(12,0,AC_MSKW10)//[11:0]    //Gazelle
	
#define SIDE_LBOX_01 (LBOX_IO_PREPROC_BASE + 0x514)
    #define C_Y_THRES Fld(8,24,AC_FULLB3)//[31:24]
    #define C_LEFT_BOUND Fld(11,12,AC_MSKW21)//[22:12]   //Oryx
    #define C_RIGHT_BOUND Fld(11,0,AC_MSKW10)//[10:0]    //Oryx
#define SIDE_LBOX_02 (LBOX_IO_PREPROC_BASE + 0x518)
    #define C_TONE1_U_RANGE Fld(8,24,AC_FULLB3)//[31:24]
    #define C_TONE1_U Fld(8,16,AC_FULLB2)//[23:16]
    #define C_TONE1_V_RANGE Fld(8,8,AC_FULLB1)//[15:8]
    #define C_TONE1_V Fld(8,0,AC_FULLB0)//[7:0]
#define SIDE_LBOX_03 (LBOX_IO_PREPROC_BASE + 0x51C)
    #define C_TONE2_U_RANGE Fld(8,24,AC_FULLB3)//[31:24]
    #define C_TONE2_U Fld(8,16,AC_FULLB2)//[23:16]
    #define C_TONE2_V_RANGE Fld(8,8,AC_FULLB1)//[15:8]
    #define C_TONE2_V Fld(8,0,AC_FULLB0)//[7:0]
#define SIDE_LBOX_04 (LBOX_IO_PREPROC_BASE + 0x520)
    #define C_TONE3_U_RANGE Fld(8,24,AC_FULLB3)//[31:24]
    #define C_TONE3_U Fld(8,16,AC_FULLB2)//[23:16]
    #define C_TONE3_V_RANGE Fld(8,8,AC_FULLB1)//[15:8]
    #define C_TONE3_V Fld(8,0,AC_FULLB0)//[7:0]
#define SIDE_LBOX_05 (LBOX_IO_PREPROC_BASE + 0x524)
    #define C_DETECT_MODE Fld(2,30,AC_MSKB3)//[31:30]
    #define C_RIGHT_TONE_SEPERATE Fld(11,16,AC_MSKW32)//[26:16]
    #define C_LEFT_TONE_SEPERATE Fld(11,0,AC_MSKW10)//[10:0]
#define SIDE_LBOX_06 (LBOX_IO_PREPROC_BASE + 0x528)
    #define C_U_THRES Fld(8,16,AC_FULLB2)//[23:16]
    #define C_V_THRES Fld(8,8,AC_FULLB1)//[15:8]
#define LBOX_STA_00 (LBOX_IO_PREPROC_BASE + 0x52C)
    #define STA_LBOX_Y_IS_SMALL Fld(1,25,AC_MSKB3)//[25:25]
    #define STA_LOX_STATE_CHANGE Fld(1,24,AC_MSKB3)//[24:24]
	#define STA_LBOX_Y_UP_LINE Fld(11,12,AC_MSKW21)//[22:12]
	#define STA_LBOX_Y_LOW_LINE Fld(11,0,AC_MSKW10)//[10:0]

	#define STA_LBOX_Y_UP_LINE_4K Fld(12,12,AC_MSKW21)//[23:12]   //Gazelle
	#define STA_LBOX_Y_LOW_LINE_4K Fld(12,0,AC_MSKW10)//[11:0]	  //Gazelle

#define SIDE_LBOX_STA_00 (LBOX_IO_PREPROC_BASE + 0x530)
    #define STA_SUM_BLACK_LEFT_POS_DIFF Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_SUM_BLACK_RIGHT_POS_DIFF Fld(16,0,AC_FULLW10)//[15:0]
#define SIDE_LBOX_STA_01 (LBOX_IO_PREPROC_BASE + 0x534)
    #define STA_TOTAL_Y_IS_SMALL Fld(1,31,AC_MSKB3)//[31:31]
	#define STA_BLACK_LEFT_POS_MAX Fld(11,16,AC_MSKW32)//[26:16]
	#define STA_BLACK_RIGHT_POS_MAX Fld(11,0,AC_MSKW10)//[10:0]
	#define STA_BLACK_LEFT_POS_MAX_4K Fld(13,16,AC_MSKW32)//[28:16]   //gazelle
	#define STA_BLACK_RIGHT_POS_MAX_4K Fld(13,0,AC_MSKW10)//[12:0]	  //gazelle

#define SIDE_LBOX_STA_02 (LBOX_IO_PREPROC_BASE + 0x538)
	#define STA_BLACK_LEFT_POS_MIN_4K Fld(13,16,AC_MSKW32)//[28:16]    //gazelle
	#define STA_BLACK_RIGHT_POS_MIN_4K Fld(13,0,AC_MSKW10)//[12:0]	   //gazelle
	#define STA_BLACK_LEFT_POS_MIN Fld(11,16,AC_MSKW32)//[26:16]
    #define STA_BLACK_RIGHT_POS_MIN Fld(11,0,AC_MSKW10)//[10:0]
    

#define SIDE_LBOX_STA_03 (LBOX_IO_PREPROC_BASE + 0x53C)
    #define STA_U_IN_TONE1_LEFT Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_V_IN_TONE1_LEFT Fld(16,0,AC_FULLW10)//[15:0]
#define SIDE_LBOX_STA_04 (LBOX_IO_PREPROC_BASE + 0x540)
    #define STA_U_IN_TONE1_RIGHT Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_V_IN_TONE1_RIGHT Fld(16,0,AC_FULLW10)//[15:0]
#define SIDE_LBOX_STA_05 (LBOX_IO_PREPROC_BASE + 0x544)
    #define STA_U_IN_TONE2_LEFT Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_V_IN_TONE2_LEFT Fld(16,0,AC_FULLW10)//[15:0]
#define SIDE_LBOX_STA_06 (LBOX_IO_PREPROC_BASE + 0x548)
    #define STA_U_IN_TONE2_RIGHT Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_V_IN_TONE2_RIGHT Fld(16,0,AC_FULLW10)//[15:0]
#define SIDE_LBOX_STA_07 (LBOX_IO_PREPROC_BASE + 0x54C)
    #define STA_U_IN_TONE3_LEFT Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_V_IN_TONE3_LEFT Fld(16,0,AC_FULLW10)//[15:0]
#define SIDE_LBOX_STA_08 (LBOX_IO_PREPROC_BASE + 0x550)
    #define STA_U_IN_TONE3_RIGHT Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_V_IN_TONE3_RIGHT Fld(16,0,AC_FULLW10)//[15:0]

#define SIDE_LBOX_07 (0x0554)                            //gazelle
    #define C_LEFT_BOUND_4K Fld(13,13,AC_MSKDW)//[25:13]    //gazelle
    #define C_RIGHT_BOUND_4K Fld(13,0,AC_MSKW10)//[12:0]    //gazelle

#endif
