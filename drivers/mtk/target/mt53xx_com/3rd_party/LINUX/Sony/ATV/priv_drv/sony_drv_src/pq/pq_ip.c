
/******************************************************************************
*[File]             pq_ip.c
*[Version]          v0.1
*[Revision Date]    2009-07-17
*[Author]           Ooi
*[Description]
*    Sony Intelligent-Picture implementation.
*[Copyright]
*    Copyright (C) 2009 Sony All Rights Reserved.
******************************************************************************/

/*-----------------------------------------------------------------------------
	include files
-----------------------------------------------------------------------------*/
#include "../../sony_priv_inc/pq_gamma.h"
#include "../../sony_priv_inc/pq_ip.h"
#include "../../../../../../../../../custom/Sony/2K12/res/app_util/config/acfg_atv_eep.h"
#include "eeprom_if.h"

#include "general.h"
#include "video_def.h"
#include "drv_nr.h"
#include "source_select.h"
#include "source_table.h"
#include "hw_nr.h"
#include "../../sony_drv_inc/pq_ip_if.h"
#include "../../sony_drv_inc/Sony_drv_cb.h"

/*-----------------------------------------------------------------------------
	Enum, typedef 
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Defines
-----------------------------------------------------------------------------*/
#define NVM_TOTAL_NOISE_TH		7
#define IP_TH_ADJUST	3

/* signal strength (dB) */
//#define SIG_STRENGTH_TH_1	75
//#define SIG_STRENGTH_TH_2	71
//#define SIG_STRENGTH_TH_3	67
//#define SIG_STRENGTH_TH_4	63
//#define SIG_STRENGTH_TH_5	59	// CVBS noisy threshold
//#define SIG_STRENGTH_TH_6	55
//#define SIG_STRENGTH_TH_7	51


#define NVM_SONY_PQ_HEADER_OFFSET		EEP_OFFSET_EMCS_PQ_START
#define NVM_SONY_PQ_IP_NOISE_TH_OFFSET	(NVM_SONY_PQ_HEADER_OFFSET+0x04a9)

#define IP_CHANCHG_DELAY_TIME	650

/*-----------------------------------------------------------------------------
	N V M   Defines
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Structures
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Global variable declarations
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Local variable declarations
-----------------------------------------------------------------------------*/
static UINT8 noise_th_data[NVM_TOTAL_NOISE_TH];
static HANDLE_T hIpChanChg = NULL_HANDLE;
static BOOL ip_timer_running ;
static IP_LVL_T prevLVL, curLVL;
/*-----------------------------------------------------------------------------
	Extern function prototype
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Private function prototype
-----------------------------------------------------------------------------*/
static void sony_IpClrFlg (HANDLE_T  pt_tm_handle, VOID *pv_tag);

/*-----------------------------------------------------------------------------
	Function definition
-----------------------------------------------------------------------------*/

//----------------------------------------------------------------------
//  Brief   : Check if source is CVBS and noisy.
//  @param  : none
//  @return : 0:not CVBS/not noisy, 1:CVBS & noisy
// 
//----------------------------------------------------------------------
void DRVCUST_IP_Init(void)
{
	/* Initialise noise TH value */
	UINT64 u8Offset = NVM_SONY_PQ_IP_NOISE_TH_OFFSET;
	
	EEPROM_Read(u8Offset, (UINT32)noise_th_data, (UINT32)NVM_TOTAL_NOISE_TH);
	ip_timer_running  = FALSE;
	curLVL = IP_RESET;
	prevLVL = IP_RESET;
	
	VERIFY(x_timer_create(&hIpChanChg) == OSR_OK);
}


//----------------------------------------------------------------------
//  Brief   : Check if source is CVBS and noisy.
//  @param  : none
//  @return : 0:not CVBS/not noisy, 1:CVBS & noisy
// 
//----------------------------------------------------------------------
BOOL DRVCUST_IsSourceNoisy(void)
{
	UINT8 lvl;
	vIPGetNoise(&lvl);
	return (lvl <= noise_th_data[4]); 
}


//----------------------------------------------------------------------
//  Brief   : Get IP level (8 lvl -> RF/CVBS noisy)
//  @param  : none
//  @return : none
// 
//----------------------------------------------------------------------
IP_LVL_T DRVCUST_IP_Get8Level(void)
{
    UINT8 bRFNMLevel;
    BOOL chanchg;
#if 0
	static UINT8 bCnt=0;

    bCnt+=0x4;
    if(bCnt>=0xF8)
    {
       Printf("8LVL detection noise measure value : %d\n", bRFNMLevel);
    }
#endif
	vIPIsChannelChange(&chanchg);
	
	if (chanchg)
	{
		if(ip_timer_running == FALSE)
		{
			ip_timer_running = TRUE;
			VERIFY(x_timer_start(hIpChanChg, IP_CHANCHG_DELAY_TIME, X_TIMER_FLAG_ONCE, sony_IpClrFlg, NULL) == OSR_OK);
		}
	}


	if (ip_timer_running == FALSE)
	{	
		vIPGetNoise(&bRFNMLevel);
		
		if(bRFNMLevel > (noise_th_data[0]+1))
		{
			curLVL = IP_LVL1; /* good signal */
		}
		else if(bRFNMLevel > (noise_th_data[1]+1) && bRFNMLevel < (noise_th_data[0]-1))
		{
			curLVL = IP_LVL2;
		}
		else if(bRFNMLevel > (noise_th_data[2]+1) && bRFNMLevel < (noise_th_data[1]-1))
		{
			curLVL = IP_LVL3;
		}
		else if(bRFNMLevel > (noise_th_data[3]+1) && bRFNMLevel < (noise_th_data[2]-1))
		{
			curLVL = IP_LVL4;
		}
		else if(bRFNMLevel > (noise_th_data[4]+1) && bRFNMLevel < (noise_th_data[3]-1))
		{
			curLVL = IP_LVL5;
		}
		else if(bRFNMLevel > (noise_th_data[5]+1) && bRFNMLevel < (noise_th_data[4]-1))
		{
			curLVL = IP_LVL6;	
		}
		else if(bRFNMLevel > (noise_th_data[6]+1) && bRFNMLevel < (noise_th_data[5]-1))
		{
			curLVL = IP_LVL7;
		}
		else if(bRFNMLevel < (noise_th_data[6]-1))
		{
			curLVL = IP_LVL8; /* weak signal */
		}		
		else
		{
			/* in dead zone */
			if(bRFNMLevel >= noise_th_data[0]-1 && bRFNMLevel <= noise_th_data[0]+1)	/*74 ~ 76 */
			{
				curLVL = IP_LVL2;
			}
			else if (bRFNMLevel >= noise_th_data[1]-1 && bRFNMLevel <= noise_th_data[1]+1) /* 70 ~ 72 */
			{
				curLVL = IP_LVL3;
			}
			else if (bRFNMLevel >= noise_th_data[2]-1 && bRFNMLevel <= noise_th_data[2]+1) /* 66 ~ 68 */
			{
				curLVL = IP_LVL4;
			}
			else if (bRFNMLevel >= noise_th_data[3]-1 && bRFNMLevel <= noise_th_data[3]+1) /* 62 ~ 64 */
			{
				curLVL = IP_LVL5;
			}
			else if (bRFNMLevel >= noise_th_data[4]-1 && bRFNMLevel <= noise_th_data[4]+1) /* 58 ~ 60 */
			{
				curLVL = IP_LVL6;
			}
			else if (bRFNMLevel >= noise_th_data[5]-1 && bRFNMLevel <= noise_th_data[5]+1) /* 54 ~ 56 */
			{
				curLVL = IP_LVL7;
			}
			else if (bRFNMLevel >= noise_th_data[6]-1 && bRFNMLevel <= noise_th_data[6]+1) /* 50 ~ 52 */
			{
				curLVL = IP_LVL8;
			}

			if (curLVL > prevLVL)
			{
				curLVL = (IP_LVL_T)((UINT8)curLVL - 1);
			}

		}
		
	}
	else
	{
		curLVL = IP_LVL1;	//always return LVL1 during tuning period
	}	
	
	if (curLVL != prevLVL)
	{
		prevLVL = curLVL;	
	}
	
	return curLVL;
}


//----------------------------------------------------------------------
//  Brief   : clear flag when timer time out
//  @param  : none
//  @return : none
// 
//----------------------------------------------------------------------
static void sony_IpClrFlg (HANDLE_T  pt_tm_handle, VOID *pv_tag)
{
	ip_timer_running = FALSE;
	x_timer_stop(hIpChanChg);
}

/* Ooi START - Return current IP level for signal indicator 
 * @ get IP noise level for OSD - OSD USED ONLY!!
 * @ return noise level        
 */ 
UINT8 sony_IPGetIndicatorNoiseLevel(void)
{
	enum
	{
		LVL0,	//weak signal
		LVL1,
		LVL2,
		LVL3,
		LVL4,
		LVL5,
		LVL6,
		LVL7,
		LVL8	//good signal		
	};
	UINT8 curLvl = LVL0;
	BOOL signal_stable;
	BOOL is_cvbs;

	vIPGetSignalStatus(&signal_stable);
	vIPIsCVBS(&is_cvbs);
	
	if(signal_stable == TRUE)
	{
		curLvl = DRVCUST_NoiseLevel();

		if(is_cvbs == TRUE && DRVCUST_IsSourceNoisy()==FALSE)
		{
			switch (curLvl)
			{
				case NM_LEVEL1_0:
				case NM_LEVEL1_1:
				case NM_LEVEL1_2:
				case NM_LEVEL1_3:
					curLvl = LVL8;
					break;

				case NM_LEVEL2:
					curLvl = LVL7;
					break;

				case NM_LEVEL3:
					curLvl = LVL6;
					break;	

				case NM_LEVEL4:
					curLvl = LVL5;
					break;	

				case NM_LEVEL5:
					curLvl = LVL4;
					break;	

				case NM_LEVEL6:
					curLvl = LVL3;
					break;	

				case NM_LEVEL7:
					curLvl = LVL2;
					break;	

				case NM_LEVEL8:
					curLvl = LVL1;
					break;	

				default:
					curLvl = LVL0;
					break;
			}
		}
		else
		{
			switch (curLvl)
			{
				case IP_LVL1:
					curLvl = LVL8;
					break;

				case IP_LVL2:
					curLvl = LVL7;
					break;

				case IP_LVL3:
					curLvl = LVL6;
					break;	

				case IP_LVL4:
					curLvl = LVL5;
					break;	

				case IP_LVL5:
					curLvl = LVL4;
					break;	

				case IP_LVL6:
					curLvl = LVL3;
					break;	

				case IP_LVL7:
					curLvl = LVL2;
					break;	

				case IP_LVL8:
					curLvl = LVL1;
					break;	

				default:
					curLvl = LVL0;
					break;
			}
		}
	}
	
	return curLvl;
}
/* Ooi END */




/* Write Sreg
 * @ sony_IpSetTh
 * @ return void      
 */ 
void sony_IpSetTh (CUST_IP_NOISE_TH_INFO_T* p_param)
{
	UINT64 u8Offset = NVM_SONY_PQ_IP_NOISE_TH_OFFSET;
	
	u8Offset = u8Offset + p_param->lvl - IP_TH_ADJUST;
	noise_th_data[p_param->lvl-IP_TH_ADJUST] = p_param->data;
	
	EEPROM_Write((UINT64)0x3aa9, (UINT32)noise_th_data, (UINT32)NVM_TOTAL_NOISE_TH);
}


/* Get Sreg
 * @ sony_IpGetTh
 * @ return void      
 */ 
void sony_IpGetTh (CUST_IP_NOISE_TH_INFO_T* p_param)
{
	p_param->data = noise_th_data[p_param->lvl - IP_TH_ADJUST];
}



