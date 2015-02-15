/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * $RCSfile: iris_d_custom.c,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 *
 * Description:
 *         This header file contains specific implementation.
 *---------------------------------------------------------------------------*/


//-----------------------------------------------------------------------------
//                    include files
// ----------------------------------------------------------------------------
#include "d_ATV_cust.h"
#include "sv_const.h"
#ifdef CC_DRV_SUPPORT_SMICRO
#include "SMicro/SMicro_if.h"
#endif
#include "nptv_debug.h"

//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------

#define SIZE_CHK(size, target_size)                                   \
          do {                                              \
            if (size != sizeof(target_size)) { \
                LOG(2, "Invalid parameter\n");\
                return -1; }     \
          } while (0)

#define PARAM_CHK(ptr, type, limit)  \
          do {                                              \
            if ((ptr == NULL) || ((*(type *)(ptr)) >= limit))\
            {\
                    LOG(2, "Invalid parameter\n");\
                    return -1;\
            } } while (0)

#define ONE_BYTE_CMD(MODULE, CMD, VALUE, FCN)  \
        do {  SMICRO_MSG rMsg;\
        rMsg.u4ModuleId = MODULE;\
        rMsg.u4CommandId = CMD;\
        rMsg.aPrm[0] = (UINT8)(VALUE);\
        rMsg.pCallbackFcn = (void*)FCN;\
        vSMicroSendEvent(rMsg);  } while (0)

//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Global variables
//---------------------------------------------------------------------------
extern UINT32 _u4gATVModelType;

CHAR szTagStr[DRV_CUSTOM_TAG_TYPE_MAX][20]
={
	"load",
	"firm", 
	"pqda", 
	"pnel", 
	"aqda",
	"ueep",
	"uimg",
	"nvrm",
	"edid"
  };
        
//---------------------------------------------------------------------------
// Internal functions
//---------------------------------------------------------------------------
extern vGetTagVersion(CHAR* ucTagName, CHAR * szVerStr);
//---------------------------------------------------------------------------
// Global functions
//---------------------------------------------------------------------------
INT32 d_ATV_cust_spec_set (
    DRV_CUSTOM_CUST_SPEC_TYPE_T    e_cust_spec_type,
    VOID*                          pv_set_info,
    SIZE_T                         z_size,
    BOOL                           b_store
)
{
    INT32 i4_ret = 0;

    switch (e_cust_spec_type)
    {
        #ifdef CC_DRV_SUPPORT_SMICRO
        case DRV_CUSTOM_CUST_SPEC_TYPE_DEMO_MODE:
        {
            SIZE_CHK(z_size, UINT8); 
            PARAM_CHK(pv_set_info, UINT8, NUM_OF_D_DEMO_MODE);
            ONE_BYTE_CMD(MODULEID_PANEL_PARAM, CMD17_SET_DEMO_MODE, (*(UINT8 *)pv_set_info), NULL);
            break;
        }
        case DRV_CUSTOM_CUST_SPEC_TYPE_PATH:
        {
            SIZE_CHK(z_size, UINT8); 
            PARAM_CHK(pv_set_info, UINT8, NUM_OF_D_PATH);
            ONE_BYTE_CMD(MODULEID_PANEL_PARAM, CMD17_SET_PATH, (*(UINT8 *)pv_set_info), NULL);
            break;
        }
        case DRV_CUSTOM_CUST_SPEC_TYPE_MOTIONFLOW_MODE:
        {
            SIZE_CHK(z_size, UINT8); 
            PARAM_CHK(pv_set_info, UINT8, NUM_OF_D_MOTIONFLOW_MODE);
            ONE_BYTE_CMD(MODULEID_PANEL_PARAM, CMD17_SET_MOTIONFLOW_MODE, (*(UINT8 *)pv_set_info), NULL);
            break;
        }
        case DRV_CUSTOM_CUST_SPEC_TYPE_PICTURE_MODE:
        {
            SIZE_CHK(z_size, UINT8); 
            PARAM_CHK(pv_set_info, UINT8, NUM_OF_D_PICTURE_MODE);
            ONE_BYTE_CMD(MODULEID_PANEL_PARAM, CMD17_SET_PICTURE_MODE, (*(UINT8 *)pv_set_info), NULL);
            break;
        }
        case DRV_CUSTOM_CUST_SPEC_TYPE_CINE_MOTION_MODE:
        {
            SIZE_CHK(z_size, UINT8); 
            PARAM_CHK(pv_set_info, UINT8, NUM_OF_D_CINE_MOTION);
            ONE_BYTE_CMD(MODULEID_PANEL_PARAM, CMD17_SET_CINEMA_MOTION_MODE, (*(UINT8 *)pv_set_info), NULL);
            break;
        }
        case DRV_CUSTOM_CUST_SPEC_TYPE_GRAPHIC_AREA:
        {
            SIZE_CHK(z_size, UINT8); 
            PARAM_CHK(pv_set_info, UINT8, 2);
            ONE_BYTE_CMD(MODULEID_PANEL_PARAM, CMD17_SET_GRAPHIC_AREA, (*(UINT8 *)pv_set_info), NULL);
            break;
        }
        case DRV_CUSTOM_CUST_SPEC_TYPE_DCC:
        {
            SIZE_CHK(z_size, UINT8); 
            PARAM_CHK(pv_set_info, UINT8, 2);
            ONE_BYTE_CMD(MODULEID_PANEL_PARAM, CMD17_SET_DCC, (*(UINT8 *)pv_set_info), NULL);
            break;
        }
        case DRV_CUSTOM_CUST_SPEC_TYPE_MASK_AREA:
        {
            SMICRO_MSG rMsg;
            D_SMicroMaskArea *pPtr;
            if (pv_set_info == NULL)
            {
                LOG(2, "Invalid parameter\n");
                return -1;              
            }
           
            SIZE_CHK(z_size, D_SMicroMaskArea); 
            pPtr = (D_SMicroMaskArea *)pv_set_info;
            rMsg.u4ModuleId = MODULEID_PANEL_PARAM;
            rMsg.u4CommandId = CMD17_SET_MASK_AREA;
            rMsg.aPrm[0] = pPtr->u1Index;
            rMsg.aPrm[1] = (UINT8)((pPtr->u2Width) & 0xff);
            rMsg.aPrm[2] = (UINT8)(((pPtr->u2Width)>>8) & 0xff);
            rMsg.aPrm[3] = (UINT8)((pPtr->u2Height) & 0xff);
            rMsg.aPrm[4] = (UINT8)(((pPtr->u2Height)>>8) & 0xff);
            rMsg.aPrm[5] = (UINT8)((pPtr->u2X) & 0xff);
            rMsg.aPrm[6] = (UINT8)(((pPtr->u2X)>>8) & 0xff);        
            rMsg.aPrm[7] = (UINT8)((pPtr->u2Y) & 0xff);
            rMsg.aPrm[8] = (UINT8)(((pPtr->u2Y)>>8) & 0xff);   
            rMsg.pCallbackFcn = NULL;
            vSMicroSendEvent(rMsg);  

            break;
        }        
        case DRV_CUSTOM_CUST_SPEC_TYPE_INPUT_MODE:
        {
            SIZE_CHK(z_size, UINT8); 
            PARAM_CHK(pv_set_info, UINT8, NUM_OF_D_INPUT_MODE);
            ONE_BYTE_CMD(MODULEID_PANEL_PARAM, CMD17_SET_INPUT, (*(UINT8 *)pv_set_info), NULL);
            break;
        }
        #endif
        case DRV_CUSTOM_CUST_SPEC_TYPE_LED_MODE:
        {
            D_LED_DISPLAY *prLEDInfo = (D_LED_DISPLAY *)pv_set_info;

            if ((pv_set_info == NULL) || (z_size != sizeof(D_LED_DISPLAY)))
            {
                LOG(2, "Invalid parameter\n");
                return -1;
            }
            
            LOG(7, "[d_ATV_cust_spec_set] Set led %d as %d\n", 
                prLEDInfo->eLedType, prLEDInfo->eLedState);
            ATVLEDCtrlFunc(prLEDInfo->eLedType, prLEDInfo->eLedState);
            break;
        }
        case DRV_CUSTOM_CUST_SPEC_TYPE_MMP_MODE:
        {
            SIZE_CHK(z_size, UINT8); 
            PARAM_CHK(pv_set_info, UINT8, NUM_OF_MMP_MODE);            
            break;
        }
        case DRV_CUSTOM_CUST_SPEC_TYPE_MMP_OSD_STATE:
        {
            SIZE_CHK(z_size, UINT8); 
            PARAM_CHK(pv_set_info, UINT8, NUM_OF_MMP_OSD_STATE);            
            break;
        }
        case DRV_CUSTOM_CUST_SPEC_TYPE_OSD_TYPE:
        {
            SIZE_CHK(z_size, UINT8); 
            PARAM_CHK(pv_set_info, UINT8, NUM_OF_OSD_TYPE);
            break;
        }       
        case DRV_CUSTOM_CUST_SPEC_TYPE_FM_RADIO_STATE:
        {
            SIZE_CHK(z_size, UINT8); 
            PARAM_CHK(pv_set_info, UINT8, NUM_OF_FM_RADIO_STATE);
            break;
        }  
        default:
            i4_ret = -1;
            break;
    }

    return i4_ret;
}


INT32 d_ATV_cust_spec_get (
    DRV_CUSTOM_CUST_SPEC_TYPE_T    e_cust_spec_type,
    VOID*                          pv_get_info,
    SIZE_T*                        pz_size
)
{
    switch (e_cust_spec_type)
    {
		 case DRV_CUSTOM_CUST_SPEC_TYPE_PCB_MODE:
		 {
			if ((pv_get_info == NULL) || (*(pz_size) != sizeof(UINT32)))
			{
				LOG(2, "[DRV_CUSTOM_CUST_SPEC_TYPE_PCB_MODE] Invalid parameter\n");
				return -1;
			}
			*((UINT32 *)(pv_get_info)) = _u4gATVModelType;
			break;
		 }
    
        case DRV_CUSTOM_CUST_SPEC_GET_TAG_VERSION:
        {
        	DRV_CUSTOM_TAG_VERSION_TYPE_T *prTageVer=NULL;
        	
        	prTageVer = (DRV_CUSTOM_TAG_VERSION_TYPE_T *)pv_get_info;        	
            if(prTageVer->ui1_tag_type >=DRV_CUSTOM_TAG_TYPE_MAX)
            {
            	 return RMR_DRV_INV_GET_INFO;
            }
            vGetTagVersion(szTagStr[prTageVer->ui1_tag_type], prTageVer->szVerStr);
            break;
        }    	
        default:
            break;
    }

    return RMR_OK;
}




