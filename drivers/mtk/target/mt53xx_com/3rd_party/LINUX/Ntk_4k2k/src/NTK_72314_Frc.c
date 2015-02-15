/*-----------------------------------------------------------------------------*/
/* Include Header Files                                                        */
/*-----------------------------------------------------------------------------*/ 
//! Standard lib
#include "x_os.h"
//#include <stdio.h>
#include <linux/string.h>
//#include <stdlib.h>
//#include <stdint.h>
//#include <malloc.h>
#include <linux/unistd.h>

#include <stdarg.h>
//#include <linux/signal.h>
//#include <pthread.h>

//#include <linux/semaphore.h>
#include <linux/errno.h>
//#include <linux/time.h>
#include <linux/types.h>
#include <linux/ipc.h>
#include <linux/msg.h>
#include <linux/prctl.h>
#include <linux/ioctl.h>
//#include <assert.h>
//#include <linux/syscall.h>
//! common


//vivienwei for Multi Process
//#include <fcntl.h>           /* For O_* constants */
//#include <sys/stat.h>        /* For mode constants */

//! driver
//! Platform

#include "Def_types.h"
#include "hal_frc_iic.h"
#include "frc_protocol.h"

#include "NTK_72314_Frc.h"
#include "drv_tdtv_drvif.h"



#define DRV_OS_TMO_NOW      0
#define DRV_OS_TMO_NEVER    0xffffffff

#define PLAT_FRC_REFIX     "#PLAT-FRC# "

#define PLAT_FRC_ERR(x...)	printk(PLAT_FRC_REFIX x)
#define PLAT_FRC_WRN(x...)	printk(PLAT_FRC_REFIX x)

#define SUPPORT_SEMA 0

/*-----------------------------------------------------------------------------*/
/* Local Function Protype                                                      										*/
/*-----------------------------------------------------------------------------*/
static FRC_Result _app_frc_314_WriteMailbox(ST_PLAT_FRC_314_FUNCTION_TABLE_t* pstTable, u32 u32Value, bool bSkipDetectDeadlock);
 
 FRC_Result App_FRC_Set_314_FHD_4K2K(bool bOnOff);
/*-----------------------------------------------------------------------------*/
/* Local Global Variables                                                      										*/
/*-----------------------------------------------------------------------------*/
static ST_PLAT_FRC_314_FUNCTION_TABLE_t FRC_314_FunctionList[]=
{

	// OSD
	{EN_PLAT_FRC_FUNC_OSD_INPUT_FORMAT,                             EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_INPUT_FORMAT, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OSD_OUTPUT_FORMAT,                          EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_OUTPUT_FORMAT, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OSD_INPUT_OUTPUT_FORMAT,                EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_INPUT_OUTPUT_FORMAT,0,  0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OSD_PIXEL_FORMAT_8BIT,                      EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_PIXEL_FORMAT_8BIT,  0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OSD_PIXEL_FORMAT_444,                       EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_PIXEL_FORMAT_444, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OSD_PIXEL_FORMAT_RGB,                      EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_PIXEL_FORMAT_RGB, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OSD_PIXEL_FORMAT_PERALPHA,              EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_PIXEL_FORMAT_PERALPHA, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OSD_INPUT_WIDTH,                                EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_INPUT_WIDTH, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OSD_INPUT_HEIGHT,                               EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_INPUT_HEIGHT, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OSD_OUTPUT_WIDTH,                             EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_OUTPUT_WIDTH, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OSD_OUTPUT_HEIGHT,                            EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_OUTPUT_HEIGHT, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OSD_X_POSITION,                                  EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_X_POSITION, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OSD_Y_POSITION,                                  EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_Y_POSITION, 0, 0xFFFFFFFF},

	//colorkey
	{EN_PLAT_FRC_FUNC_OSD_COLORKEY_R,                                  EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_COLORKEY_R, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OSD_COLORKEY_G,                                  EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_COLORKEY_G, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OSD_COLORKEY_B,                                  EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_COLORKEY_B, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OSD_COLORKEYALPHA,                                  EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_COLORKEYALPHA, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OSD_COLORKEY_EN,                                  EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_COLORKEY_EN, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OSD_NONCOLORKEYALPHA,                                  EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_NONCOLORKEYALPHA, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OSD_NONCOLORKEY_EN,                                  EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_NONCOLORKEY_EN, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OSD_PLANEALPHA,                                  EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_PLANEALPHA, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OSD_PLANEALPHA_EN,                                  EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_PLANEALPHA_EN, 0, 0xFFFFFFFF},
	
	{EN_PLAT_FRC_FUNC_OSD_ON_OFF,                                         EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_ONOFF, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OSD_FREEZE_EN,                                    EN_CMD_MAIN_OSD, EN_CMD_SUB_OSD_FREEZE_EN, 0, 0xFFFFFFFF},

	// FHD & 4K2K
	{EN_PLAT_FRC_FUNC_FHDTO4K2K,                                    EN_CMD_MAIN_FORMAT, EN_CMD_SUB_FORMAT_FHD2UHD, 0, 0xFFFFFFFF},

	{EN_PLAT_FRC_FUNC_OSD_PORT,                                    EN_CMD_MAIN_LVDS, EN_CMD_SUB_LVDS_SETOSDPORT, 0, 0xFFFFFFFF},

	//video mute
	{EN_PLAT_FRC_FUNC_VIDEO_MUTE_N,                                    EN_CMD_MAIN_GENERAL, EN_CMD_SUB_GENERAL_ALL_MUTE, 0, 0xFFFFFFFF},

	//three d change
	{EN_PLAT_FRC_FUNC_INPUT_FORMAT,                                    EN_CMD_MAIN_FORMAT, EN_CMD_SUB_FORMAT_INPUT_FORMAT, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_OUTPUT_FORMAT,                                    EN_CMD_MAIN_FORMAT, EN_CMD_SUB_FORMAT_OUTPUT_FORMAT, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_SET_DISPARITY_LEVEL,                                    EN_CMD_MAIN_MEMC, EN_CMD_SUB_MEMC_DISPARTY_LEVEL, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_SET_DEPTH_LEVEL,                                    EN_CMD_MAIN_MEMC, EN_CMD_SUB_MEMC_DEPTH_LEVEL, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_3D_LR_TOGGLE,                                    EN_CMD_MAIN_MEMC, EN_CMD_SUB_MEMC_LR_SWAP, 0, 0xFFFFFFFF},

	//PC mode
	{EN_PLAT_FRC_FUNC_PCMODE,                                    EN_CMD_MAIN_FORMAT, EN_CMD_SUB_FORMAT_OUTPUT_MODE, 0, 0xFFFFFFFF},

	//FRC mode
	{EN_PLAT_FRC_FUNC_FRCMODE,                                    EN_CMD_MAIN_MEMC, EN_CMD_SUB_MEMC_MODE, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_SET_DEJUDDERLEVEL,                 EN_CMD_MAIN_MEMC, EN_CMD_SUB_MEMC_DEJUDDER_LEVEL, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_SET_DEBLURLEVEL,                 EN_CMD_MAIN_MEMC, EN_CMD_SUB_MEMC_DEBLUR_LEVEL, 0, 0xFFFFFFFF},

	//FRC DEMOMODE
	{EN_PLAT_FRC_FUNC_SET_FRC_DEMOMODE,                 EN_CMD_MAIN_WINDOW, EN_CMD_SUB_WINDOW_DEMO_MODE, 0, 0xFFFFFFFF},

	//White Balance
	{EN_PLAT_FRC_FUNC_SET_WB_ONOFF,                                    EN_CMD_MAIN_WB, EN_CMD_SUB_WB_EN, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_SET_WB_R_GAIN,                                    EN_CMD_MAIN_WB, EN_CMD_SUB_WB_R_GAIN, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_SET_WB_G_GAIN,                                    EN_CMD_MAIN_WB, EN_CMD_SUB_WB_G_GAIN, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_SET_WB_B_GAIN,                                    EN_CMD_MAIN_WB, EN_CMD_SUB_WB_B_GAIN, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_SET_WB_R_OFFSET,                                    EN_CMD_MAIN_WB, EN_CMD_SUB_WB_R_OFFSET, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_SET_WB_G_OFFSET,                                    EN_CMD_MAIN_WB, EN_CMD_SUB_WB_G_OFFSET, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_SET_WB_B_OFFSET,                                    EN_CMD_MAIN_WB, EN_CMD_SUB_WB_B_OFFSET, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_SET_WB_R_LIMIT,                                    EN_CMD_MAIN_WB, EN_CMD_SUB_WB_R_LIMIT, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_SET_WB_G_LIMIT,                                    EN_CMD_MAIN_WB, EN_CMD_SUB_WB_G_LIMIT, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_SET_WB_B_LIMIT,                                    EN_CMD_MAIN_WB, EN_CMD_SUB_WB_B_LIMIT, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_SET_WB_R_GRADATION,                                    EN_CMD_MAIN_WB, EN_CMD_SUB_WB_GRADATION_R, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_SET_WB_G_GRADATION,                                    EN_CMD_MAIN_WB, EN_CMD_SUB_WB_GRADATION_G, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_SET_WB_B_GRADATION,                                    EN_CMD_MAIN_WB, EN_CMD_SUB_WB_GRADATION_B, 0, 0xFFFFFFFF},

	//Picture Contrast
	{EN_PLAT_FRC_FUNC_SET_PQ_CONTRAST,                                    EN_CMD_MAIN_PQ, EN_CMD_SUB_PQ_AUTO_CONTRAST, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_SET_PQ_BRIGHTNESS,                                    EN_CMD_MAIN_PQ, EN_CMD_SUB_PQ_BRIGHTNESS, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_SET_PQ_SHARPNESS,                                    EN_CMD_MAIN_PQ, EN_CMD_SUB_PQ_SHARPNESS, 0, 0xFFFFFFFF},
	
	//Super Resolution
	{EN_PLAT_FRC_FUNC_SET_PQ_SR_ONOFF,                                    EN_CMD_MAIN_PQ, EN_CMD_SUB_PQ_SR_ONOFF, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_SET_PQ_SR_DEMO_ONOFF,                         EN_CMD_MAIN_PQ, EN_CMD_SUB_PQ_SR_DEMO_ONOFF, 0, 0xFFFFFFFF},
	{EN_PLAT_FRC_FUNC_SET_PQ_SR_LEVEL,                                      EN_CMD_MAIN_PQ, EN_CMD_SUB_PQ_SR_LEVEL, 0, 0xFFFFFFFF},

    //3D For Skyworth
    {EN_PLAT_FRC_FUNC_SET_SKYWORTH_3D,                  EN_CMD_MAIN_FORMAT,EN_CMD_SUB_FORMAT_SKYWORTH_3D_FORMAT,0,0xffffffff},
};


static ST_PLAT_FRC_INPUT_FORMAT_TABLE_t FRC_314_InputFormatList[]=
{
	{EN_INPUT_2D, 		        	0x00},
	{EN_INPUT_3D_LR, 	                0x01},
	{EN_INPUT_3D_TB, 		        0x02},
	{EN_INPUT_3D_SBS, 	                0x03},
	{EN_INPUT_3D_FP, 		        0x04},
	{EN_INPUT_3D_LINE_BY_LINE, 	0x05},
};

static ST_PLAT_FRC_OUTPUT_FORMAT_TABLE_t FRC_314_OutputFormatList[]=
{
	{EN_OUTPUT_2D, 			                 0x00},
	{EN_OUTPUT_3D_SG, 	                         0x01},
	{EN_OUTPUT_3D_SSG_ME, 		                 0x02},
	{EN_OUTPUT_3D_SSG_DDR, 	                 0x03},
	{EN_OUTPUT_3D_PR1X_ME, 		 0x04},
	{EN_OUTPUT_3D_PR1X_DDR, 	                 0x05},
	{EN_OUTPUT_3D_PR2X_DDR, 	                 0x06},
	{EN_OUTPUT_3D_AUOPR_DDR, 	                 0x07},
	{EN_OUTPUT_2D2X, 	                 0x08},
};

static ST_PLAT_FRC_FRC_MODE_TABLE_t FRC_314_FRCModeList[]=
{
	{EN_FRC_OFF,			0x00},
	{EN_FRC_STANDARD,		0x01},
	{EN_FRC_SMOOTH,		0x02},
	{EN_FRC_CLEAR,			0x03},
	{EN_FRC_CUSTOM,		0x04},
	{EN_FRC_MAX,			0xff},	
};

static void *g_platfrc_314_sem = NULL;

static bool bBypassFRCFlag = FALSE;

bool bWaitCmdDone=1;


static inline u32 _OS_ClockGetInMS( void )
{
#if SUPPORT_SEMA
	struct timespec stTSNow;
	clock_gettime( CLOCK_MONOTONIC, &stTSNow );
	return ( u32 )( stTSNow.tv_nsec / 1000000 + stTSNow.tv_sec * 1000 );
#else
    return 0;
#endif
}
/*------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------*/
/* Interface Functions                                                        									       */
/*-----------------------------------------------------------------------------*/
/* semaphore */
static void * DRV_OS_SemaphoreCreate( u32 u32Value, const c8 *pc8Name )
{
#if SUPPORT_SEMA
	sem_t *pstSem = NULL;

	pstSem = ( sem_t * )malloc( sizeof( sem_t ) );

	if( pstSem != NULL )
	{
		sem_init( pstSem, 0, u32Value );
		return pstSem;
	}
#endif

	return NULL;
}


static bool DRV_OS_SemaphoreDestroy( void *pSemaphore )
{
#if SUPPORT_SEMA
	sem_t *pstSem = ( sem_t * )pSemaphore;

	if( pstSem != NULL )
	{
		sem_destroy( pstSem );
		free( pstSem );
		return TRUE;
	}
#endif
	return TRUE;//unused
}
static bool DRV_OS_SemaphoreWait( void *pSemaphore, u32 u32TimeoutInMS )
{
#if SUPPORT_SEMA
	sem_t          *pstSem = ( sem_t * )pSemaphore;
	int             nRet = 0;
	u32             u32TSOut = 0;

	if( pstSem != NULL )
	{
		if( u32TimeoutInMS == DRV_OS_TMO_NOW )
		{
			nRet = sem_trywait( pstSem );
		}
		else if( u32TimeoutInMS != DRV_OS_TMO_NEVER )
		{
			u32TSOut = _OS_ClockGetInMS() + u32TimeoutInMS;

			do
			{
				nRet = sem_trywait( pstSem );

				if( nRet == 0 )
				{
					break;
				}

				if( _OS_ClockGetInMS() >= u32TSOut )
				{
					break;
				}

				//! delay 1ms
				//usleep( 1000 );
				x_thread_delay(1);
			}
			while ( TRUE );
		}
		else
		{
			nRet = sem_wait( pstSem );
		}

		if ( nRet == 0 )
		{
			return TRUE;
		}
	}
#endif
	return TRUE;//unused
}

static bool DRV_OS_SemaphoreSignal( void *pSemaphore )
{
#if SUPPORT_SEMA
	sem_t *pstSem = ( sem_t * )pSemaphore;

	if( pstSem != NULL )
	{
		if( sem_post( pstSem ) == 0 )
		{
			return TRUE;
		}
	}
#endif
	return TRUE;//unused
}

FRC_Result App_FRC_314_Init(void)
{
	g_platfrc_314_sem = DRV_OS_SemaphoreCreate ( 1, (const c8 *)"plat_frc_314" );
		
	return API_FRC_Init();
}


FRC_Result App_FRC_314_DeInit(void)
{
	FRC_Result ret=FRC_OK;
	
	if(DRV_OS_SemaphoreDestroy(g_platfrc_314_sem))
	{
		g_platfrc_314_sem=NULL;
	}
	else
	{
		ret=FRC_NOT_OK;
	}
	return ret;
}

FRC_Result App_SET_FHD_TO_4K2K(void)
{
	
	if( API_FRC_IsInit() != TRUE)
		return FRC_NOT_OK;
#if 0 // [JW test]
	// VIDEO  MUTE Off
	if (App_FRC_Set_314_VideoMute(0x01) != FRC_OK)
	{
		printk("%s, PLAT_FRC_Set_314_VideoMute Failed!\n", __FUNCTION__);
		return FRC_NOT_OK;
	}
#endif
#if 0
	if(u4DrvTDTV3DModeQuery() != E_TDTV_UI_3D_MODE_OFF)
	{
		PLAT_FRC_ERR("Before entry 4K2K mode, we need confirm NPTV is not 3D status!!!!n");
		return FRC_NOT_OK;
	}
#endif	 

	//FHD->4K2K
	if (App_FRC_Set_314_FHD_4K2K(0x01) != FRC_OK)
	{
		printk("%s, PLAT_FRC_Set_314_FHD_4K2K Failed!\n", __FUNCTION__);
		return FRC_NOT_OK;
	}
#if 0 // [JW test]
	//delay
	//usleep(800*1000);
	x_thread_delay(800);

	//VIDEO MUTE ON
	if (App_FRC_Set_314_VideoMute(0x00) != FRC_OK)
	{
		printk("%s, PLAT_FRC_Set_314_VideoMute Failed!\n", __FUNCTION__);
		return FRC_NOT_OK;
	}
#endif

	return FRC_OK;
}

FRC_Result App_SET_4K2K_TO_FHD(void)
{
	if( API_FRC_IsInit() != TRUE)
		return FRC_NOT_OK;	

#if 0 // [JW test]
	// VIDEO  MUTE On
	if (App_FRC_Set_314_VideoMute(0x01) != FRC_OK)
	{
		printk("%s, PLAT_FRC_Set_314_VideoMute Failed!\n", __FUNCTION__);
		return FRC_NOT_OK;
	}
#endif	

	//k2K->FHD
	if (App_FRC_Set_314_FHD_4K2K(0x00) != FRC_OK)
	{
		printk("%s, PLAT_FRC_Set_314_FHD_4K2K Failed!\n", __FUNCTION__);
		return FRC_NOT_OK;
	}
	
#if 0 // [JW test]
	//delay
	//usleep(800*1000);
	x_thread_delay(800);
	//VIDEO MUTE Off
	if (App_FRC_Set_314_VideoMute(0x00) != FRC_OK)
	{
		printk("%s, PLAT_FRC_Set_314_VideoMute Failed!\n", __FUNCTION__);
		return FRC_NOT_OK;
	}
#endif

	return FRC_OK;
}



FRC_Result App_FRC_Set_314_FHD_4K2K(bool bOnOff)
{
	if(API_FRC_IsInit() != TRUE)
	{
		PLAT_FRC_ERR("HAL FRC IS NOT Initialized\n");
		return FRC_NOT_OK;
	}
	
	if(bOnOff)
	{
		if(_app_frc_314_WriteMailbox(&FRC_314_FunctionList[EN_PLAT_FRC_FUNC_FHDTO4K2K], 0x01, FALSE) != FRC_OK)
		return FRC_NOT_OK;
	}
	else
	{
		if(_app_frc_314_WriteMailbox(&FRC_314_FunctionList[EN_PLAT_FRC_FUNC_FHDTO4K2K], 0x00, FALSE) != FRC_OK)
		return FRC_NOT_OK;
	}
	
	return FRC_OK;
}


/**************************FOR 314 3D SETTING***********************************************/
FRC_Result App_FRC_Set_314_VideoMute(bool bOnOff)
{
	if(API_FRC_IsInit() != TRUE)
	{
		PLAT_FRC_ERR("HAL FRC IS NOT Initialized\n");
		if(API_FRC_Init()!= FRC_OK)
		{
			PLAT_FRC_ERR("HAL FRC IS NOT Initialized again!\n");
			return FRC_NOT_OK;
		}
			
	}
	
	if(bOnOff)
	{
		if(_app_frc_314_WriteMailbox(&FRC_314_FunctionList[EN_PLAT_FRC_FUNC_VIDEO_MUTE_N], 0x01, FALSE) != FRC_OK)
		{
			PLAT_FRC_ERR("%s _plat_frc_314_WriteMailbox freeze failed\n", __FUNCTION__);
			return FRC_NOT_OK;
		}
	}
	else
	{
		if(_app_frc_314_WriteMailbox(&FRC_314_FunctionList[EN_PLAT_FRC_FUNC_VIDEO_MUTE_N], 0x00, FALSE) != FRC_OK)
		{
			PLAT_FRC_ERR("%s _plat_frc_314_WriteMailbox unfreeze failed\n", __FUNCTION__);
			return FRC_NOT_OK;
		}
	}
	
	return FRC_OK;	
}

FRC_Result App_FRC_Set_314_3DInputFormat(EN_INPUT_314_FORMAT e3DInputFormat)
{
	if(API_FRC_IsInit() != TRUE)
	{
		PLAT_FRC_ERR("HAL FRC IS NOT Initialized\n");
		return FRC_NOT_OK;
	}

	if(_app_frc_314_WriteMailbox(&FRC_314_FunctionList[EN_PLAT_FRC_FUNC_INPUT_FORMAT], FRC_314_InputFormatList[e3DInputFormat].u32Command, FALSE) != FRC_OK)
		return FRC_NOT_OK;
	return FRC_OK;
}

FRC_Result App_FRC_Set_314_3DOutputFormat(EN_OUTPUT_314_FORMAT e3DOutputFormat)
{
	if(API_FRC_IsInit() != TRUE)
	{
		PLAT_FRC_ERR("HAL FRC IS NOT Initialized\n");
		return FRC_NOT_OK;
	}
	
	if(_app_frc_314_WriteMailbox(&FRC_314_FunctionList[EN_PLAT_FRC_FUNC_OUTPUT_FORMAT], FRC_314_OutputFormatList[e3DOutputFormat].u32Command, FALSE) != FRC_OK)
		return FRC_NOT_OK;
	return FRC_OK;
}

FRC_Result APP_FRC_Set_314_Skyworth_Enter3D(void)
{
	if(API_FRC_IsInit() != TRUE)
	{
		PLAT_FRC_ERR("HAL FRC IS NOT Initialized\n");
		if(API_FRC_Init()!= FRC_OK)
			return FRC_NOT_OK;
	}
#if 1
	if(u4DrvTDTV3DModeQuery() == E_TDTV_UI_3D_MODE_OFF)
	{
		PLAT_FRC_ERR("Before entry 3D, we need confirm NPTV is 3D status!!!!n");
		return FRC_NOT_OK;
	}
#endif		
	if(_app_frc_314_WriteMailbox(&FRC_314_FunctionList[EN_PLAT_FRC_FUNC_SET_SKYWORTH_3D], 0x01, FALSE) != FRC_OK)
	{
			PLAT_FRC_ERR("HAL FRC IS NOT Initialized\n");
			return FRC_NOT_OK;
	}
	return FRC_OK;

}

FRC_Result APP_FRC_Set_314_Skyworth_Exit3D(void)
{
	if(API_FRC_IsInit() != TRUE)
	{
		PLAT_FRC_ERR("HAL FRC IS NOT Initialized\n");
		if(API_FRC_Init()!= FRC_OK)
			return FRC_NOT_OK;
	}
	if(_app_frc_314_WriteMailbox(&FRC_314_FunctionList[EN_PLAT_FRC_FUNC_SET_SKYWORTH_3D], 0x00, FALSE) != FRC_OK)
	{
		PLAT_FRC_ERR("HAL FRC IS NOT Initialized\n");
		return FRC_NOT_OK;
	}
			
	return FRC_OK;

}

FRC_Result APP_FRC_Set_314_FRCMode(EN_FRC_MODE enFRCMode)
{
	if(API_FRC_IsInit() != TRUE)
	{
		PLAT_FRC_ERR("HAL FRC IS NOT Initialized\n");
		return FRC_NOT_OK;
	}

	if(_app_frc_314_WriteMailbox(&FRC_314_FunctionList[EN_PLAT_FRC_FUNC_FRCMODE], FRC_314_FRCModeList[enFRCMode].u32Command, FALSE) != FRC_OK)
		return FRC_NOT_OK;
	
	return FRC_OK;
}

FRC_Result APP_FRC_Set_314_FRC_Demo_OnOff(bool bOnOff)
{
	if(API_FRC_IsInit() != TRUE)
	{
		PLAT_FRC_ERR("HAL FRC IS NOT Initialized\n");
		return FRC_NOT_OK;
	}

	if (bOnOff == FALSE)
	{
		if(_app_frc_314_WriteMailbox(&FRC_314_FunctionList[EN_PLAT_FRC_FUNC_SET_FRC_DEMOMODE], 0x00, FALSE) != FRC_OK)
			return FRC_NOT_OK;
	}
	else
	{
		if(_app_frc_314_WriteMailbox(&FRC_314_FunctionList[EN_PLAT_FRC_FUNC_SET_FRC_DEMOMODE], 0x01, FALSE) != FRC_OK)
			return FRC_NOT_OK;
	}
	
	return FRC_OK;
}

FRC_Result APP_FRC_Set_314_SR_OnOff(bool bOnOff)
{
	if(API_FRC_IsInit() != TRUE)
	{
		PLAT_FRC_ERR("HAL FRC IS NOT Initialized\n");
		return FRC_NOT_OK;
	}
	
	if(bOnOff)
	{
		if(_app_frc_314_WriteMailbox(&FRC_314_FunctionList[EN_PLAT_FRC_FUNC_SET_PQ_SR_ONOFF], 0x01, FALSE) != FRC_OK)
		{
			PLAT_FRC_ERR("%s _plat_frc_314_WriteMailbox freeze failed\n", __FUNCTION__);
			return FRC_NOT_OK;
		}
	}
	else
	{
		if(_app_frc_314_WriteMailbox(&FRC_314_FunctionList[EN_PLAT_FRC_FUNC_SET_PQ_SR_ONOFF], 0x00, FALSE) != FRC_OK)
		{
			PLAT_FRC_ERR("%s _plat_frc_314_WriteMailbox unfreeze failed\n", __FUNCTION__);
			return FRC_NOT_OK;
		}
	}
	
	return FRC_OK;
}

FRC_Result APP_FRC_Set_314_SR_Demo_OnOff(bool bOnOff)
{
	if(API_FRC_IsInit() != TRUE)
	{
		PLAT_FRC_ERR("HAL FRC IS NOT Initialized\n");
		return FRC_NOT_OK;
	}
	
	if(bOnOff)
	{
		if(_app_frc_314_WriteMailbox(&FRC_314_FunctionList[EN_PLAT_FRC_FUNC_SET_PQ_SR_DEMO_ONOFF], 0x01, FALSE) != FRC_OK)
		{
			PLAT_FRC_ERR("%s _plat_frc_314_WriteMailbox freeze failed\n", __FUNCTION__);
			return FRC_NOT_OK;
		}
	}
	else
	{
		if(_app_frc_314_WriteMailbox(&FRC_314_FunctionList[EN_PLAT_FRC_FUNC_SET_PQ_SR_DEMO_ONOFF], 0x00, FALSE) != FRC_OK)
		{
			PLAT_FRC_ERR("%s _plat_frc_314_WriteMailbox unfreeze failed\n", __FUNCTION__);
			return FRC_NOT_OK;
		}
	}
	
	return FRC_OK;
}

FRC_Result APP_FRC_Get_314_FWVersion(u32 *u32Version)
{
	FRC_Result FRCRet = FRC_OK;
	u32 u32Value;
	
	if(API_FRC_IsInit() != TRUE)
	{
		PLAT_FRC_ERR("FRC IS NOT Initialized\n");
		return FRC_NOT_OK;
	}
	
	if(u32Version == NULL)
		return FRC_PARAMETER_ERROR;

	// Get FRC Semaphore
	if(FALSE == DRV_OS_SemaphoreWait(g_platfrc_314_sem, DRV_OS_TMO_NEVER))
	{
		PLAT_FRC_ERR( "Mailbox - semaphore waiting \n");
		printk( "Mailbox - semaphore waiting \n");
		FRCRet = FRC_NOT_OK;
		goto END_FUN;
	}

	if(HAL_FRC_ReadReg(FRC_REG_GET_VERSION, &u32Value) != TRUE)
	{
		PLAT_FRC_ERR("Get Version fail\n");
		FRCRet = FRC_NOT_OK;
		goto END_FUN;
	}

	*u32Version = u32Value;
	
END_FUN:
	// Release FRC Semaphore
	DRV_OS_SemaphoreSignal(g_platfrc_314_sem);
	return FRCRet;	
}

static FRC_Result _app_frc_314_WriteMailbox(ST_PLAT_FRC_314_FUNCTION_TABLE_t* pstTable, u32 u32Value, bool bSkipDetectDeadlock)
{
	FRC_Result FRCRet = FRC_OK;
	//u32 u32Cmd=0, u32Temp;
	
	if(bBypassFRCFlag)
	{
		PLAT_FRC_ERR("bBypassFRCFlag is true!\n");
		printk("bBypassFRCFlag is true!\n");
		FRCRet = FRC_OK;
		goto END_FUN;
	}

	//printf("\n####LS Send Mailbox u8Priority=0x%08x;u8MainType=0x%08x, u8SubType=0x%08x, u32Value=%d\n", pstTable->u8Priority,pstTable->u8MainType, pstTable->u8SubType, u32Value );
	if(API_FRC_IsInit() != TRUE)
	//if(HAL_FRC_Init() != TRUE)
	{
		PLAT_FRC_ERR("HAL FRC IS NOT Initialized\n");
		printk("HAL FRC IS NOT Initialized\n");
		FRCRet = FRC_NOT_OK;
		goto END_FUN;
	}

	// Get FRC Semaphore
	if(FALSE == DRV_OS_SemaphoreWait(g_platfrc_314_sem, DRV_OS_TMO_NEVER))
	{
		PLAT_FRC_ERR( "Mailbox - semaphore waiting \n");
		//printf( "Mailbox - semaphore waiting \n");
		FRCRet = FRC_NOT_OK;
		goto END_FUN;
	}


 	API_FRC_WriteCmd(pstTable->u8MainType,pstTable->u8SubType,u32Value,bWaitCmdDone);
	
	pstTable->u32Default = u32Value;		//save mailbox setting
	
END_FUN:
	// Release FRC Semaphore
	DRV_OS_SemaphoreSignal(g_platfrc_314_sem);
	//printf("%s end, frcret: %d\n", __FUNCTION__, FRCRet);
	return FRCRet;
}
 
FRC_Result APP_FRC_Update_CodebySoc(char *Path)
{
#if !defined(CC_MTK_LOADER) 
	return API_FRC_Update_OnePackage(Path);
#else
	return FRC_OK;
#endif
}

