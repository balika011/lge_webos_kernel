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


/*****************************************************************************
*  VDec Driver: Patch for Disc identify
*****************************************************************************/
#ifndef _D_DRV_VDEC_H_
#define  _D_DRV_VDEC_H_


#if  1 //mtk40088 add for special disc library

typedef struct{
   UINT8 au1DiscId[32];
}DRV_DISC_INFO_T;

typedef enum{
   /** define the new error disc type here**/
   NONE_ERROR
}DRV_DISC_SPECIAL_ERROR_E;


typedef struct{
   UINT8 au1DiscId[32];
   UINT64 u8Start;
   UINT64 u8End;

   DRV_DISC_SPECIAL_ERROR_E eErrorType;
}DRV_DISC_SPECIAL_LIST_T;


void vSetVdecCurrentDiscInfo(DRV_DISC_INFO_T rDiscInfo);

#endif



typedef struct _VDEC_DISC_INFO_T_
{
    CHAR ucDiscID[16];  //disc ID
    UINT32 u4PlaylistId;
    UINT16 u2PlayItemId;
}VDEC_DISC_INFO_T;

extern void d_set_vdec_disc_info(VDEC_DISC_INFO_T *prMwDiscInfo);
extern void d_update_vdec_disc_info(UINT32 *prVDecPatchInfo);
extern void d_init_vdec_disc_info(void);

#if 1//def DRV_NEW_CHIP_BOUNDING
extern void d_get_divx_support_info(BOOL *fgIsDivXSupport, BOOL *fgIsDivXHT, BOOL *fgIsDivXUltra, BOOL *fgIsDivXHD, BOOL *fgIsDivXPlus);
#else
extern void d_get_divx_support_info(BOOL *fgIsDivXSupport);
#endif

#endif
