/****************************************************************************
*
* File:        eep_ctrl.c
*
* Description: EEP Mapping control
*  
* Author:      kl-lees
*
* History:     10 July 2009 - Creation
*
*****************************************************************************/

/****************************************************************************/
/*     			      HEADER FILES				    						*/
/****************************************************************************/
#include "x_os.h"
#include "x_assert.h"
#include "x_dbg.h"
#include "sif_if.h"
#include "../../../../../../../../../custom/Sony/2K12/res/app_util/config/acfg_atv_eep.h"
#include "eeprom_if.h"


/****************************************************************************/
/*     			      TYPE DEFINITIONS			    						*/
/****************************************************************************/
typedef struct _EEP_MAP_GRP{
	CHAR* c_group;
	const UINT16 ui2_start_area;
	const UINT16 ui2_end_area;
} EEP_MAP_GRP;


/****************************************************************************
*			      VARIABLES                                     			*
*****************************************************************************/
const EEP_MAP_GRP tbl_eep_map[] = {
    /*  c_group                 ui2_start_area                          ui2_end_area */        
    {   "SIG_BSTR_AREA",        EEP_OFFSET_EMCS_SIG_BSTR_START,         EEP_OFFSET_EMCS_SIG_BSTR_END        }, /* System - Sig_Bstr */
    {   "SELF_DIAG_AREA",       EEP_OFFSET_EMCS_SELF_DIAG_START,        EEP_OFFSET_EMCS_SELF_DIAG_END       }, /* System - Self_Diagnosis */
    {   "HOTEL_AREA",           EEP_OFFSET_EMCS_HOTEL_DRV_START,        EEP_OFFSET_EMCS_HOTEL_DRV_END       }, /* Interface - Hotel */
    {   "BL_AREA",              EEP_OFFSET_EMCS_BL_CTRL_AREA_START,     EEP_OFFSET_EMCS_BL_CTRL_AREA_END    }, /* Driver - Backklight */
    {   "RGB_AREA",             EEP_OFFSET_EMCS_RGB_AREA_START,         EEP_OFFSET_EMCS_RGB_AREA_END        }, /* Driver - RGB */
    {   "PQ_AREA",              EEP_OFFSET_EMCS_PQ_START,               EEP_OFFSET_EMCS_PQ_END              }, /* PQ */
    {   "AGING_AREA",           EEP_OFFSET_EMCS_AGING_START,            EEP_OFFSET_EMCS_AGING_END           }, /* aging */
    {   "WB_PBM_AREA",          EEP_OFFSET_EMCS_WB_PBM_START,           EEP_OFFSET_EMCS_WB_PBM_END          }  /* WB PBM */
};
			
            	 
/****************************************************************************
*			  	FUNCTION PROTOTYPES				    						*	
*****************************************************************************/


/****************************************************************************
*				FUNCTION DEFINITIONS			    						*
*****************************************************************************/

/*****************************************************************************
*	FUNCTION: void EEP_map_check(void)
*	DESCRIPTION: checking on NVM mapping
*	PRECOND.: None
*	POSTCOND.: None
*****************************************************************************/
VOID EEP_map_check (void)
{
    UINT16 ui2_eep_tbl_cnt;
   
    /* Check for valid NVM area for DRIVER Layer */
    if ( tbl_eep_map[0].ui2_start_area < EEP_OFFSET_EMCS_DRIVER_AREA_START )
    {
        printf( "[EEP_CHECK] Overlap Mapping from Application Layer!! \n" );                 
    }  
    
    /* Check for overlap eep mapping */
    for ( ui2_eep_tbl_cnt = 0; ui2_eep_tbl_cnt < (sizeof(tbl_eep_map)/sizeof(tbl_eep_map[0])-1); ui2_eep_tbl_cnt++ )
    {        
        if  ( tbl_eep_map[ui2_eep_tbl_cnt].ui2_end_area <= tbl_eep_map[ui2_eep_tbl_cnt+1].ui2_start_area ) 
        {
            /* Do Nothing */
        } 
        else 
        {
            printf( "[EEP_CHECK] Overlap Mapping at %s \n", tbl_eep_map[ui2_eep_tbl_cnt].c_group );
        }
    }        

    /* Check for NVM overflow */    
    if ( ui2_eep_tbl_cnt == (sizeof(tbl_eep_map)/sizeof(tbl_eep_map[0])-1) )
    {
        if ( tbl_eep_map[ui2_eep_tbl_cnt].ui2_end_area > EEP_OFFSET_EMCS_END )
        {
            printf( "[EEP_CHECK] NVM OVERFLOW!!! \n" );
        }              
    }                  
}


/*****************************************************************************
*   End of file
*****************************************************************************/

