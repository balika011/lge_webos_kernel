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
 *----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2005, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: drv_mjc_int.c $
 * $Revision: #1 $
 *
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 * Header Files
 *----------------------------------------------------------------------------*/

#include "x_assert.h"
#include "hw_tdnavi.h"

#include "drv_tdtv_feature.h"
#include "drv_tdnavi.h"
#include "vdo_misc.h"
#include "sv_const.h"
#include "vdp_if.h"
#include "mute_if.h"

static volatile UINT32 u4Dummy;
#define WAITING_HOLD_TIME 0
#define WAIT_FOR_STA_RDY() //for(u4Dummy=0;u4Dummy<WAITING_HOLD_TIME;++u4Dummy)

EXTERN UINT32 blk_buf[4][NAVI_LINE_WIDTH/2*NAVI_LINE_HEIGHT/2];
EXTERN UINT32 blk_var[4][NAVI_LINE_WIDTH/2*NAVI_LINE_HEIGHT/2];


/*----------------------------------------------------------------------------*
 * NaviV1 Global/Static variables
 *----------------------------------------------------------------------------*/

void vDrvTDTVTDNAVIStressTest(UINT32 count)
{
    UINT32 u4Index = 0;
    static UINT32 a4BlkPixelDiff[480];
    static UINT32 a4BlkYValue[480];
    UINT32 u4TempPixel =0;
    UINT32 u4TempYValue =0;
    UINT32 u4Compare = 0;

#if defined(CC_SUPPORT_SW_NAVI_PLUS)
    UINT32 i,j;

    Printf("==APL==\n");

    for(j=0;j<NAVI_LINE_HEIGHT;j++)
    {
        for(i=0;i<NAVI_LINE_WIDTH;i++)    
        {
            if(i<NAVI_LINE_WIDTH/2)
            {
                if(j<NAVI_LINE_HEIGHT/2)
                    Printf("%2x ",blk_buf[0][j*NAVI_LINE_WIDTH/2+i]);
                else
                    Printf("%2x ",blk_buf[2][(j-NAVI_LINE_HEIGHT/2)*NAVI_LINE_WIDTH/2+i]);
            }
            else
            {
                if(j<NAVI_LINE_HEIGHT/2)
                    Printf("%2x ",blk_buf[1][j*NAVI_LINE_WIDTH/2+i-NAVI_LINE_WIDTH/2]);
                else
                    Printf("%2x ",blk_buf[3][(j-NAVI_LINE_HEIGHT/2)*NAVI_LINE_WIDTH/2+i-NAVI_LINE_WIDTH/2]);
            }
        }  
        Printf("\n");
    }
    
    Printf("==VAR==\n");
    
    for(j=0;j<NAVI_LINE_HEIGHT;j++)
    {
        for(i=0;i<NAVI_LINE_WIDTH;i++)    
        {
            if(i<NAVI_LINE_WIDTH/2)
            {
                if(j<NAVI_LINE_HEIGHT/2)
                    Printf("%2x ",blk_var[0][j*NAVI_LINE_WIDTH/2+i]);
                else
                    Printf("%2x ",blk_var[2][(j-NAVI_LINE_HEIGHT/2)*NAVI_LINE_WIDTH/2+i]);
            }
            else
            {
                if(j<NAVI_LINE_HEIGHT/2)
                    Printf("%2x ",blk_var[1][j*NAVI_LINE_WIDTH/2+i-NAVI_LINE_WIDTH/2]);
                else
                    Printf("%2x ",blk_var[3][(j-NAVI_LINE_HEIGHT/2)*NAVI_LINE_WIDTH/2+i-NAVI_LINE_WIDTH/2]);
            }
        }  
        Printf("\n");
    }

#endif

    if(count <= 0)
        return;

    u4Compare = 0;

    Printf("\n");    
    while(count--)
    {
        vRegWriteFldAlign(TDNVAI_0B,1,FW_READ_EN);
#if defined(CC_SUPPORT_SW_NAVI_PLUS)
        x_sema_lock(_hTDNAVIVCheck,X_SEMA_OPTION_WAIT);        
        x_sema_lock(_hTDNAVIVCheck,X_SEMA_OPTION_WAIT);  
#else        
        x_thread_delay(16);
#endif                
        //compare the BLK pixle differ
        for(u4Index = 0; u4Index < 480; u4Index++)
        {
            vRegWriteFldAlign(TDNVAI_0B,u4Index,FW_ADDRESS);

            WAIT_FOR_STA_RDY();
                
            if(u4Compare == 0)
            {
                a4BlkPixelDiff[u4Index] = RegReadFldAlign(TDNVAI_STA1,TDNVAI_FPGA_02_BLK_PIXEL_DIFF);

                Printf("%3d ",a4BlkPixelDiff[u4Index] );
                if((u4Index+1)%30 == 0)
                    Printf("\n");
            }
            else
            {
                u4TempPixel = RegReadFldAlign(TDNVAI_STA1,TDNVAI_FPGA_02_BLK_PIXEL_DIFF);
                if(a4BlkPixelDiff[u4Index] != u4TempPixel)
                {                            
                    Printf("\n===cnt(%d) index(%d-%d) Pxl Diff value(%4d-%4d),navi pixel differ  stress test Not Passed!==!\n",count,u4Index/30,u4Index%30,a4BlkPixelDiff[u4Index],u4TempPixel);
                }             
            }   
        }
        
        if(u4Compare == 0)
            Printf("------------------------------------------------\n");   

        //compare the BLK pixle Y value
        for(u4Index = 0; u4Index < 480; u4Index++)
        {
            vRegWriteFldAlign(TDNVAI_0B,u4Index,FW_ADDRESS);
            WAIT_FOR_STA_RDY();

            if(u4Compare == 0)
            {
                a4BlkYValue[u4Index] = RegReadFldAlign(TDNVAI_STA1,TDNVAI_FPGA_02_BLK_Y);
                Printf("%3d ",a4BlkYValue[u4Index] );
                if((u4Index+1)%30 == 0)
                    Printf("\n");
            }
            else
            {                    
                u4TempYValue = RegReadFldAlign(TDNVAI_STA1,TDNVAI_FPGA_02_BLK_Y);
                if(a4BlkYValue[u4Index] != u4TempYValue)
                {
                    Printf("\n===cnt(%d) index(%d-%d) value(%4d:%4d),navi  Y value  stress test Not Passed!==!\n",count,u4Index/30,u4Index%30,a4BlkYValue[u4Index],u4TempYValue);
                }             
            }             
        }      

        if(0==count%4)
        {
            printf("/\b");
        }
        else if(1==count%4)
        {
            printf("-\b");
        }
        else if(2==count%4)
        {
            printf("\\\b");
        }
        else
        {
            printf("-\b");
        }
        vRegWriteFldAlign(TDNVAI_0B,0,FW_READ_EN);
        x_thread_delay(16);
        u4Compare=1;
    }    
}

