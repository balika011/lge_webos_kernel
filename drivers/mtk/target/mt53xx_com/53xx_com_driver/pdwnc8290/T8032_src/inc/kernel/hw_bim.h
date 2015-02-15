/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2003
*
*****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   $Workfile: hw_bim.h $ 
 *
 * Project:
 * --------
 *   MT8226
 *
 * Description:
 * ------------
 *   BIM register definition
 *   
 * Author:
 * -------
 *   Daniel Hsiao
 *
 * Last changed:
 * ------------- 
 * $Author: p4admin $ 
 *
 * $Modtime: 04/05/27 10:47p $    
 *
 * $Revision: #1 $
****************************************************************************/

#ifndef _HW_BIM_H_
#define _HW_BIM_H_


//-------------------------------------------------
#define T8032_NINT 0xf002
	#define PDWNC_INT 0x01
	#define ARM_INT 0x02
	#define CEC_INT 0x04
	#define RS232_INT 0x08

#define T8032_INT_CLR 0xf003
	#define PDWNC_CLR 0x01
	#define ARM_CLR 0x02
	#define CEC_CLR 0x04
	#define RS232_CLR 0x08
	#define INT_CLR_ALL 0x0f
	
#define T8032_NINT_E 0xf004
	#define PDWNC_INT_EN 0x01
	#define ARM_INT_EN 0x02
	#define CEC_INT_EN 0x04
	#define RS232_INT_EN 0x08
	#define INT_EN_ALL 0x0f
#define PDWNC_INT_STATUS 0x0040
#define PDWNC_INT_ENABLE 0x004c
#define PDWNC_INT_CLEAR 0x0048
//#define T8032_ADDR_FromARM 0x01E0  //share register8
//#define T8032_DATA_FromARM 0x01E4 //share register9
//#define T8032_DATA_ToARM 0x0180//0x01F0 share register0
//#define PDWNC_SHREG7 0x019C //share register7
//#define T8032_Interrupt_ToARM 0x01F4
//#define wReadAddrFromARM() u4RegRead4B(T8032_ADDR_FromARM)
//#define dReadDataFromARM() u4RegRead4B(T8032_DATA_FromARM)
//#define vWriteDataToARM(val32) vRegWrite4B(T8032_DATA_ToARM,(val32))
//#define vWriteDataToARM(val32) vRegWrite2B(T8032_DATA_ToARM,(UINT16)val32)
//#define dReadDataToARM() u4RegRead4B(T8032_DATA_ToARM)
//#define vTriggerARMInterrupt() vRegWrite1B(T8032_Interrupt_ToARM,1)

#define MEnableT8032INT() vRegSetBit(T8032_NINT_E, 1)
#define MDisableT8032INT() vRegClrBit(T8032_NINT_E, 1)
#define bReadSysInt()		u1RegRead1B(T8032_NINT)
#define vEnableSysInt(IntBit)	vRegSetBit(T8032_NINT_E,(IntBit))
#define vDisableSysInt(IntBit) vRegClrBit(T8032_NINT_E,(IntBit))
#define vClearSysInt(IntBit)	vRegSetBit(T8032_INT_CLR,(IntBit))
#define MClearT8032INT() vRegClrBit(T8032_INT_CLR, 1)



/*
#define vTriggerARMInterrupt() \
    do \
    { \
        if (fgTrigARMIntEn) \
        { \
            vRegWrite4B(T8032_Interrupt_ToARM,1); \
        } \
    } while (0)
*/

//#define Print8bitsHex(num)  
/*


#define Print8bitsHex(num)  \
   do\
        {\
    vIO32Write1B(PDWNC_DATAW, '0');\
    vIO32Write1B(PDWNC_DATAW, 'x');\
    if( (num >> 4) > 9)\
        vIO32Write1B(PDWNC_DATAW, ( (num >> 4)-0xa) + 'a');\
    else\
        vIO32Write1B(PDWNC_DATAW,  (num >> 4) + '0');\
    if((num & 0x0f) > 9)\
        vIO32Write1B(PDWNC_DATAW, ((num & 0x0f)-0xa) + 'a');\
    else\
        vIO32Write1B(PDWNC_DATAW, (num & 0x0f) + '0');\
    vIO32Write1B(PDWNC_DATAW, 0x20);\
        }while(0)




*/


#endif
