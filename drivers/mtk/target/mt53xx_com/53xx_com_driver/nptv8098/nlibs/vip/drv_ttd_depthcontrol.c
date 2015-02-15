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
 * $RCSfile: drv_ttd_depthcontrol.c $
 * $Revision: #1 $
 *
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 * Header Files
 *----------------------------------------------------------------------------*/
#include "general.h"
#include "x_assert.h"
#if defined(CC_MT5398) 
#include "hw_tddc.h"
#endif

#include "hw_ycproc.h"
#include "hw_ttd.h"
#include "hw_sw.h"
#include "hw_di.h"
#include "hw_scpos.h"
#include "hw_tdnavi.h"
#include "nptv_debug.h"
#include "drv_ttd.h"
#include "drv_display.h"
#include "drv_video.h"
#include "drv_tdtv_drvif.h"
#include "drv_scaler_drvif.h"
#include "drv_meter.h"
#include "drv_di.h"
#include "drv_scaler.h"
#include "panel.h"
#include "fbm_drvif.h"

/*---------------------------------------------------------------------*
 * definition
 *---------------------------------------------------------------------*/
#define Diff(a, b)  (((a)>(b))?((a)-(b)):((b)-(a)))

/*----------------------------------------------------------------------*
 * Global/Static Variables
 *----------------------------------------------------------------------*/
// LSV variables
UINT8 u1Lsvinit = SV_FALSE;
INT32 iSclRatio = -64;

INT32 m_width,m_height;
INT32 m_mbSize = 4;
INT32 m_lsv_width;
INT32 m_lsv_height;
INT32 pred_de_dv[10];
INT32 * dv_meter_LH = NULL;
INT32 * DE_buf_L = NULL;
INT32 * pre_depth_buf_L = NULL;
INT32 shift_avg_H=0;    
INT32 cur_convg=0;
INT32 stab_cnt=0,stab_flag=0;
INT32 pre_shift_avg_H=0;  
INT32 SR_H=160, SR_V=0;
INT32 m_depth_width=32,m_depth_height=8;//W((384-256)/4)*2+1 H((15-3*2)/2)/2+1
INT32 step_v=1,step_h=2;
INT32 str_h;
INT32 cand_num;
INT32 var_cur;
UINT8 lsv_auto_gain=32;
UINT8 lsv_local_gain=0;
UINT8 lsv_focal_plen=128;
UINT8 d2d_convg =CONVERGENCE_DEPTH_3D; 
UINT8 d2d_ratio =DEPTH_RANGE_RATIO_3D;
UINT8 d2d_gain =DISPARITY_GAIN_3D;

INT32 var_en_th, var_pnlty_sel;
INT32 rand_dvpnlty_sel;
INT32 var_cor_th;
INT32 tmpr_dvpnlty_sel;
INT32 me_mvpnlty_sel;
INT32 ord_sel, obme_num;
INT32 zero_sel;
INT32 iIIR;
UINT32 u4DebugCnt;
UINT32 u4CalLoop;
UINT32 u4WidthDec, u4HeightDec;
UINT32 u4LumaSumL, u4LumaSumR, u4LumaSumDiffThld;

const INT32  seed_1[9]={9,3,8,1,5,0,7,2,6};
const INT32  seed_2[9]={9,5,6,1,3,2,7,0,8};
const INT32  seed_3[9]={9,3,2,7,5,6,1,8,0};
const INT32  seed_4[9]={9,5,0,7,3,8,1,6,2};	
const INT32 LutT[9] ={1,-1,5,-5,11,-11,19,-19,0}; 

/*----------------------------------------------------------------------*
 * External Variables & Functions
 *----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*
 * Local Functions
 *-----------------------------------------------------------------------*/

static INT32 ClipABC(INT32 p,INT32 max_val, INT32 min_val)
{
    if ( p > max_val )		return max_val;
    else if ( p < min_val )   return min_val;
    else				    return p;
}

void vDrvLSVModeChange(INT32 iWidth, INT32 iHeight, INT32 iStep_h)
{   
    VDP_SCALER_PRM_T* scalerInfo = getScalerInfo(VDP_1); 
    INT32 i4SclFactor;
    
    if ((m_lsv_width != iWidth) || (m_lsv_height != iHeight)) 
    {  
        m_lsv_width = iWidth; 
        m_lsv_height = iHeight; 
        step_h = iStep_h;   
        str_h = (m_lsv_width-m_depth_width*step_h*m_mbSize)/2;        
        SR_H = ClipABC(str_h-step_h*4, 160, 0);            

        x_memset((void *)DE_buf_L, 0, sizeof(INT32)*m_depth_width*m_depth_height);
        x_memset((void *)pre_depth_buf_L, 0, sizeof(INT32)*m_depth_width*m_depth_height);
        x_memset((void *)dv_meter_LH, 0, sizeof(INT32)*(SR_H*2+1));

        vIO32WriteFldMulti(TTD_FW_REG_05, P_Fld(SR_H, TTD_SR_H) | P_Fld(step_h, TTD_STEP_H));    

        i4SclFactor = (INT32)((scalerInfo->u4HDeltaPhaseIntg<<10)+(scalerInfo->u4HDeltaPhaseFrac>>10));
        iSclRatio = i4SclFactor ? ((-65536)/i4SclFactor) : (-64);
        
        LOG(2, "LSV Mode Change ==> iWidth %d   iHeight %d   iSr_h %d   iStep_h %d \n", 
                m_lsv_width, m_lsv_height, SR_H, step_h);  
        LOG(2, "LSV Mode Change ==> str_h %d   depth_width %d   step_h %d   mbSize %d   scaler ratio %d\n", 
                str_h, m_depth_width, step_h, m_mbSize, iSclRatio);       
    }
}



/* =========================================================================================== */
void AdaptiveMW(INT32 *avg_idx_H, UINT8 SR_H, UINT8 SR_V)
{
	INT32 sum_cnt_H=0;
	INT32 sum_total_H=0;
	INT32 i;
    UINT8 fg_weight = IO32ReadFldAlign(TTD_FW_REG_07, TTD_FG_WEIGHT); 
    UINT8 bg_weight = IO32ReadFldAlign(TTD_FW_REG_07, TTD_BG_WEIGHT); 
    UINT8 amw_focal_plen = IO32ReadFldAlign(TTD_FW_REG_07, AMW_FOCAL_PLEN); 
	
	for(i=-SR_H;i<=SR_H; i++)
	{
		if(i>=(amw_focal_plen-128))
		{
			sum_cnt_H+=dv_meter_LH[SR_H+i]*fg_weight;
			sum_total_H+=dv_meter_LH[SR_H+i]*i*fg_weight;
		}
		else
		{
			sum_cnt_H+=dv_meter_LH[SR_H+i]*bg_weight;
			sum_total_H+=dv_meter_LH[SR_H+i]*i*bg_weight;
		}
	}
	*avg_idx_H=sum_total_H*2/sum_cnt_H;

	x_memset((void *)dv_meter_LH, 0, sizeof(INT32)*(SR_H*2+1));
}

/* =========================================================================================== */
void prepare_dv(INT32 blk_x, INT32 blk_y, INT32* mv_frm_buf_bi_sp_sram, INT32* mv_frm_buf_bi_tp_sram,
                UINT8 SR_H, UINT8 SR_V, UINT8 fcount,UINT8 foreback)
{
	INT32 v_bound=m_depth_height;
	INT32 h_bound=m_depth_width;

	INT32* p_mv_frm_buf_bi_tp = mv_frm_buf_bi_tp_sram;
	INT32* p_mv_frm_buf_bi_sp = mv_frm_buf_bi_sp_sram;
	INT32* p_mv_frm_buf_bi;

	INT32 idx,idy;
	INT32 i,j,k = 0;
    
	// Generate random MV
	INT32 rand_cnt = fcount;
	INT32 rand_factor = ((blk_x+blk_y+rand_cnt)&0x07);
	INT32 rand_mv;
    
	if(foreback==0)
	{
		for(j=0;j<3;j++)
		{
			for(i=0;i<3;i++)
			{	
				if (blk_y==0)
                {   //1st line
					if ((j==1)&&(i==0))		        
                        p_mv_frm_buf_bi = p_mv_frm_buf_bi_sp;
					else							
                        p_mv_frm_buf_bi = p_mv_frm_buf_bi_tp;
				}
				else if ((blk_y%2)==0)
				{	//forward 
					if ((j==0) || ((j==1)&&(i==0)))		
                        p_mv_frm_buf_bi = p_mv_frm_buf_bi_sp;
					else							
                        p_mv_frm_buf_bi = p_mv_frm_buf_bi_tp;
				}
				else				
				{	//backward				
					if ((j==0) || ((j==1)&&(i==2)))	
                        p_mv_frm_buf_bi = p_mv_frm_buf_bi_sp;
					else							
                        p_mv_frm_buf_bi = p_mv_frm_buf_bi_tp;
				}
			
				idx = ClipABC((blk_x + i - 1), h_bound-1, 0);
				idy = ClipABC((blk_y + j - 1), v_bound-1, 0);
				pred_de_dv[k++] = p_mv_frm_buf_bi[idx + idy*h_bound];
			}	
		}
	}
	else
	{
		for(j=0;j<3;j++)
		{
			for(i=0;i<3;i++)
			{	
				if (blk_y==m_depth_height-1)
				{
					if ((j==1)&&(i==0))		
                        p_mv_frm_buf_bi = p_mv_frm_buf_bi_sp;
					else							
                        p_mv_frm_buf_bi = p_mv_frm_buf_bi_tp;
				}
				else if ((blk_y%2)==0)
				{	//backward 
					if ((j==2) || ((j==1)&&(i==2)))	
                        p_mv_frm_buf_bi = p_mv_frm_buf_bi_sp;
					else							
                        p_mv_frm_buf_bi = p_mv_frm_buf_bi_tp;
				}
				else				
				{	//foreward				
					if ((j==2) || ((j==1)&&(i==0)))	
                        p_mv_frm_buf_bi = p_mv_frm_buf_bi_sp;
					else							
                        p_mv_frm_buf_bi = p_mv_frm_buf_bi_tp;
				}
			
				idx = ClipABC((blk_x + i - 1), h_bound-1, 0);
				idy = ClipABC((blk_y + j - 1), v_bound-1, 0);
				pred_de_dv[k++] = p_mv_frm_buf_bi[idx + idy*h_bound];
			}	
		}
	}

    if( blk_x==0 &&blk_y==0)					
        rand_mv = pred_de_dv[4];   //tmpr
    else if ((blk_x==0 &&blk_y!=0&&(blk_y%2)==0) || (blk_x==m_depth_width-1&&blk_y!=0&&(blk_y%2)==1))	
        rand_mv = pred_de_dv[1];    //up
    else if ((blk_y%2)==0)
        rand_mv = pred_de_dv[3];   //left
    else														
        rand_mv = pred_de_dv[5];   //right
        
	pred_de_dv[9] = ClipABC( rand_mv + LutT[rand_factor], SR_H, -SR_H);

}
/* =========================================================================================== */
INT32 ORD_Quad(PEL *pCurFull, PEL *pRefFull, INT32 cur_x, INT32 cur_y, INT32 ref_x, INT32 ref_y, UINT8 step_v, UINT8 step_h)
{
    INT32 cur_avg, ref_avg, i_str, i_end;    
	INT32 ord_cost = 0;
	INT32 cur_acc=0,ref_acc=0;
	INT32 i, pos1_0, pos2_0, pos1, pos2;
	UINT8 ord_cur,ord_ref;
	INT32 ord_pnlty = 0;

    pos1 = (cur_y+step_v)*m_lsv_width+cur_x;
    pos2 = (ref_y+step_v)*m_lsv_width+ref_x;
    
    pos1_0 = (cur_y)*m_lsv_width+cur_x;
    pos2_0 = (ref_y)*m_lsv_width+ref_x;
    
	cur_acc+=pCurFull[pos1+0*step_h];
	cur_acc+=pCurFull[pos1+1*step_h];
	cur_acc+=pCurFull[pos1+2*step_h];
	cur_acc+=pCurFull[pos1+3*step_h];

	cur_acc+=pCurFull[pos1_0+0*step_h];
	cur_acc+=pCurFull[pos1_0+1*step_h];
	cur_acc+=pCurFull[pos1_0+2*step_h];
	cur_acc+=pCurFull[pos1_0+3*step_h];

	ref_acc+=pRefFull[pos2+0*step_h];
	ref_acc+=pRefFull[pos2+1*step_h];
	ref_acc+=pRefFull[pos2+2*step_h];
	ref_acc+=pRefFull[pos2+3*step_h];

	ref_acc+=pRefFull[pos2_0+0*step_h];
	ref_acc+=pRefFull[pos2_0+1*step_h];
	ref_acc+=pRefFull[pos2_0+2*step_h];
	ref_acc+=pRefFull[pos2_0+3*step_h];
	
	cur_avg=(cur_acc+4)>>3;	
	ref_avg=(ref_acc+4)>>3;

	i_str = -obme_num;
	i_end = m_mbSize+obme_num;

    pos1 += i_str*step_h;
    pos2 += i_str*step_h;
    
	for(i=i_str; i<i_end; i++)
    {
		ord_cur=(cur_avg>pCurFull[pos1])?1:0;
		ord_ref=(ref_avg>pRefFull[pos2])?1:0;
        
        pos1 += step_h;
        pos2 += step_h;

		if(ord_cur!=ord_ref)	ord_cost++;
        
		ord_cur=(cur_avg>pCurFull[pos1_0])?1:0;
		ord_ref=(ref_avg>pRefFull[pos2_0])?1:0;
        
        pos1_0 += step_h;
        pos2_0 += step_h;

		if(ord_cur!=ord_ref)	ord_cost++;
	}

	if     (ord_sel == 0)		ord_pnlty = 0;
	else if(ord_sel == 1)		ord_pnlty = MIN(ord_cost, 36)*1;
	else if(ord_sel == 2)		ord_pnlty = MIN(ord_cost, 36)*2;
	else if(ord_sel == 3)		ord_pnlty = MIN(ord_cost, 36)*3;
	else if(ord_sel == 4)		ord_pnlty = MIN(ord_cost, 36)*4;
	else if(ord_sel == 5)		ord_pnlty = MIN(ord_cost, 36)*6;
	else if(ord_sel == 6)		ord_pnlty = MIN(ord_cost, 36)*8;

	return ord_pnlty;
}

/* =========================================================================================== */
void DVEC_sort(INT32* dvecand)
{
	INT32 sort_tmp;
	INT32 i,j;
    
	for(i = 0; i < 4; i++)
	{
    	for(j = i+1; j < 5; j++)
        {
    		if(dvecand[i] > dvecand[j])
    		{
    			sort_tmp = dvecand[i];
    			dvecand[i] = dvecand[j];
    			dvecand[j] = sort_tmp;
    		}
    	}
	}
}
/* =========================================================================================== */
INT32  smth_pnlty(INT32 dv_curr, INT32 *dv_in, UINT8 is_even_line)
{
	//  0  1  2  3  4
	//  5  6  7  8  9 
	// 10 11 12 13 14
	// 15 16 17 18 19
	// 20 21 22 23 24
	INT32 dvecand[5];
	INT32 sad_pnlty;
	INT32 mv_pred_dif, dv_med;    

    dvecand[0] = dv_in[1];   
    dvecand[1] = dv_in[3];   
    dvecand[2] = dv_in[5];   
    dvecand[3] = dv_in[7];   
    dvecand[4] = (is_even_line) ? dv_in[0] : dv_in[2];   

	//candidate median
	DVEC_sort(dvecand);
	dv_med = dvecand[2];
    
    mv_pred_dif=MIN(ABS(dv_curr - dv_med), 8);

	if     (me_mvpnlty_sel == 0)		sad_pnlty = 0;
	else if(me_mvpnlty_sel == 1)		sad_pnlty = mv_pred_dif* 1/8;
	else if(me_mvpnlty_sel == 2)		sad_pnlty = mv_pred_dif* 2/8;
	else if(me_mvpnlty_sel == 3)		sad_pnlty = mv_pred_dif* 3/8;
	else if(me_mvpnlty_sel == 4)		sad_pnlty = mv_pred_dif* 4/8;
	else if(me_mvpnlty_sel == 5)		sad_pnlty = mv_pred_dif* 6/8;
	else if(me_mvpnlty_sel == 6)		sad_pnlty = mv_pred_dif* 8/8;
	else if(me_mvpnlty_sel == 7)		sad_pnlty = mv_pred_dif*12/8;
    else                                sad_pnlty = 0;
    
	return sad_pnlty;
}
/* =========================================================================================== */
INT32  tmpr_pnlty(INT32 i)
{
	INT32 tmpr_pnlty;
	INT32 tmpr_cost;

	if(	i==2||i==4)		tmpr_cost=2;
	else				tmpr_cost=0;

    tmpr_pnlty = tmpr_cost*tmpr_dvpnlty_sel;

	return tmpr_pnlty;
}


INT32  zero_pnlty(INT32 dv_cur)
{
	INT32 mv_pred_dif = MIN(ABS(dv_cur), 1);
	INT32 sad_pnlty;

    sad_pnlty = mv_pred_dif*zero_sel;

	return sad_pnlty;
}


INT32  rand_pnlty(INT32 i)
{
	INT32 rand_pnlty;
	INT32 rand_cost;

	if(	i==0)		rand_cost=4;
	else			rand_cost=0;

    rand_pnlty = rand_cost*rand_dvpnlty_sel;

	return rand_pnlty;
}


void DepthBlend(INT32 *pre_depth_buf_R, INT32 *filter_buf_R)
{
    x_memcpy((void *)pre_depth_buf_R, (void *)filter_buf_R, sizeof(INT32)*m_depth_height*m_depth_width);
}


INT32 blk_var_calc(PEL *pCurFull, UINT8 step_v, UINT8 step_h, INT32 str_h)
{
	INT32 i,j;
	INT32 abs_df1, abs_df2;
	
	var_cur=0;
	for(i=str_h;i<m_depth_width*step_h*m_mbSize;i+=4)
	{
		for(j=0;j<m_depth_height*step_v*2;j+=2)
		{
			abs_df1 = ABS(pCurFull[(i+step_h)+(j  )*m_lsv_width] - pCurFull[(i	     )+(j+1)*m_lsv_width]);
			abs_df2 = ABS(pCurFull[(i		)+(j+1)*m_lsv_width] - pCurFull[(i+step_h)+(j  )*m_lsv_width]);
			
			if(abs_df1>var_cor_th)		var_cur ++;
			if(abs_df2>var_cor_th)		var_cur ++;            
		}
	}
	return var_cur;
}   


INT32 var_pnlty(INT32 dv_cur)
{
	INT32 var_pnlty;
	INT32 var_cost = 0;

	if(var_cur<var_en_th)	
	{
        var_cost=ABS(dv_cur);
	}
    
    var_pnlty = var_cost*var_pnlty_sel;

	return var_pnlty;
}


void DVSearchLPS(PEL *pCurFull, PEL *pRefFull, INT32* cur_depth_buf, INT32* pre_depth_buf_cur,
						 UINT8 SR_H, UINT8 SR_V, UINT8 fcount, UINT8 step_v, UINT8 step_h, INT32 str_h)
{
	INT32 n, dx; //n = Search Loop For SRW ,  dx = best candidate
	INT32 ref_x, ref_y;
	INT32 smth_cost, ord_cost, rand_cost, tmpr_cost, zero_cost, var_cost;
	INT32 minimum, SAD_cand;
	INT32 V_cand_step=2*step_v; 
	INT32 H_cand_step=step_h*m_mbSize;
	INT32 i, cur_x, cur_y;
	UINT8 is_even_line;
			
	//  0  1  2  3  4
	//  5  6  7  8  9 
	// 10 11 12 13 14
	// 15 16 17 18 19
	// 20 21 22 23 24

	// Full search disparity estimation										
	for (cur_y=0; cur_y<m_depth_height; cur_y++)
	{
		is_even_line = ((cur_y%2)==0) ? SV_TRUE : SV_FALSE;
        
		if(is_even_line)
		{
			for (cur_x=0; cur_x<m_depth_width; cur_x++)
			{
				minimum = 65535;	dx=-256;
				prepare_dv(cur_x, cur_y, cur_depth_buf, pre_depth_buf_cur, SR_H, SR_V, fcount, 0);

				for (i=0; i < cand_num; i++)
				{
					n=pred_de_dv[seed_1[i]];

					ref_y = cur_y*V_cand_step;		        // Vertical   co-ordinate for reference block in previous frame
					ref_x = str_h+cur_x*H_cand_step+n;		// Horizontal co-ordinate for reference block in previous frame
				
					smth_cost = smth_pnlty(n, pred_de_dv, is_even_line);
					tmpr_cost = tmpr_pnlty(i);
					rand_cost =	rand_pnlty(i);
					zero_cost =	zero_pnlty(n);
					 var_cost =  var_pnlty(n);
					ord_cost  = ORD_Quad(pCurFull, pRefFull, str_h+cur_x*H_cand_step, cur_y*V_cand_step, 
                                        ref_x, ref_y, step_v, step_h);
                    
					SAD_cand =  smth_cost + ord_cost + zero_cost + rand_cost + tmpr_cost + var_cost;

					// Find the vector where the SAD is minimum and store it
					if(SAD_cand < minimum)
					{
						minimum = SAD_cand;
						dx = n;
					}
				}
                
				//write histogram
				cur_depth_buf[cur_x+cur_y*m_depth_width] = dx;
                
				if(dx!=-256)
				{
					dv_meter_LH[dx+SR_H]++;
				}
			}//LOOP BLOCK X :cur_x 0->width
		}//LOOP Even Line 
		else
		{
			for (cur_x=(m_depth_width-1); cur_x>=0; cur_x--)
			{
				minimum = 65535;	dx=-256;
				prepare_dv(cur_x, cur_y,cur_depth_buf, pre_depth_buf_cur, SR_H, SR_V, fcount, 0);

				for (i=0; i < cand_num; i++)
				{
					n=pred_de_dv[seed_2[i]];
					ref_y = cur_y*V_cand_step;		        // Vertical co-ordinate for reference block in previous frame
					ref_x = str_h+cur_x*H_cand_step+n;		// Horizontal co-ordinate for reference block in previous frame
				
					smth_cost = smth_pnlty(n, pred_de_dv, is_even_line);
					tmpr_cost = tmpr_pnlty(i);
					rand_cost =	rand_pnlty(i);
					zero_cost =	zero_pnlty(n);
					 var_cost =  var_pnlty(n);
					ord_cost  = ORD_Quad(pCurFull, pRefFull, str_h+cur_x*H_cand_step, cur_y*V_cand_step, 
                                        ref_x, ref_y, step_v, step_h);
                    
					SAD_cand =  smth_cost + ord_cost + zero_cost + rand_cost + tmpr_cost + var_cost;

					// Find the vector where the SAD is minimum and store it
					if(SAD_cand < minimum)
					{
						minimum = SAD_cand;
						dx = n;
					}
				}
                
				//write histogram
				cur_depth_buf[cur_x+cur_y*m_depth_width] = dx;
                
				if(dx!=-256)
				{
					dv_meter_LH[dx+SR_H]++;
				}
			}//LOOP BLOCK X :cur_x 0->width
		}
	}//LOOP BLOCK Y :cur_y
}


void DVSearchLPS_Back(PEL *pCurFull, PEL *pRefFull, INT32* cur_depth_buf, INT32* pre_depth_buf_cur,
						 UINT8 SR_H, UINT8 SR_V, UINT8 fcount, UINT8 step_v, UINT8 step_h, INT32 str_h)
{
	INT32 n, dx; //n = Search Loop For SRW ,  dx = best candidate
	INT32 ref_x, ref_y;
	INT32 smth_cost,ord_cost,rand_cost,tmpr_cost,zero_cost,var_cost;
	INT32 minimum, SAD_cand;
	INT32 V_cand_step=2*step_v; 
	INT32 H_cand_step=step_h*m_mbSize;
	INT32 cur_x,cur_y;
	UINT8 is_even_line;
	INT32 i;
						
	//  0  1  2  3  4
	//  5  6  7  8  9 
	// 10 11 12 13 14
	// 15 16 17 18 19
	// 20 21 22 23 24

	// Full search disparity estimation										
	for(cur_y=(m_depth_height-1); cur_y>=0; cur_y--)
	{
		is_even_line=((cur_y%2)==1) ? SV_TRUE : SV_FALSE;
        
		if(is_even_line)
		{
			for(cur_x=0; cur_x < m_depth_width; cur_x++)
			{
				minimum = 65535;	dx=-256;
				prepare_dv(cur_x, cur_y, cur_depth_buf, pre_depth_buf_cur, SR_H, SR_V, fcount, 1);
				
				for (i=0; i < cand_num; i++)
				{
					n=pred_de_dv[seed_3[i]];

					ref_y = cur_y*V_cand_step;		        // Vertical   co-ordinate for reference block in previous frame
					ref_x = str_h+cur_x*H_cand_step+ n;		// Horizontal co-ordinate for reference block in previous frame
				
					smth_cost = smth_pnlty(n, pred_de_dv, is_even_line);
					tmpr_cost = tmpr_pnlty(i);
					rand_cost =	rand_pnlty(i);
					zero_cost =	zero_pnlty(n);
					 var_cost =  var_pnlty(n);
					ord_cost  = ORD_Quad(pCurFull, pRefFull, str_h+cur_x*H_cand_step, cur_y*V_cand_step, 
                                        ref_x, ref_y, step_v, step_h);
					
					SAD_cand =  smth_cost + ord_cost + zero_cost + rand_cost + tmpr_cost + var_cost;

					// Find the vector where the SAD is minimum and store it
					if(SAD_cand < minimum)
                    {
						minimum = SAD_cand;
						dx = n;
					}
				}
                
				//write histogram
				cur_depth_buf[cur_x+cur_y*m_depth_width] = dx;
                
				if(dx!=-256)
                {
					dv_meter_LH[dx+SR_H]++;
				}
			}//LOOP cur_x 
		}//LOOP Even Line 
		else
		{
			for(cur_x=m_depth_width-1; cur_x>=0; cur_x--)
			{
				minimum = 65535;	dx=-256;
				prepare_dv(cur_x, cur_y, cur_depth_buf, pre_depth_buf_cur, SR_H, SR_V, fcount, 1);
				
				for (i=0; i < cand_num; i++)
				{
					n=pred_de_dv[seed_4[i]];
                    
					ref_y = cur_y*V_cand_step;		        // Vertical co-ordinate for reference block in previous frame
					ref_x = str_h+cur_x*H_cand_step+n;		// Horizontal co-ordinate for reference block in previous frame
				
					smth_cost = smth_pnlty(n, pred_de_dv, is_even_line);
					tmpr_cost = tmpr_pnlty(i);
					rand_cost =	rand_pnlty(i);
					zero_cost =	zero_pnlty(n);
					 var_cost =  var_pnlty(n);
					ord_cost  = ORD_Quad(pCurFull, pRefFull, str_h+cur_x*H_cand_step, cur_y*V_cand_step, 
                                        ref_x, ref_y, step_v, step_h);
					
					SAD_cand =  smth_cost + ord_cost + zero_cost + rand_cost + tmpr_cost + var_cost;

					// Find the vector where the SAD is minimum and store it
					if(SAD_cand < minimum)
                    {
						minimum = SAD_cand;
						dx = n;
					}
				}
                
				//write histogram
				cur_depth_buf[cur_x+cur_y*m_depth_width] = dx;
                
				if(dx!=-256)
                {
					dv_meter_LH[dx+SR_H]++;
				}
			}//LOOP cur_x 
		}
	}//LOOP BLOCK Y :cur_y
}

void vDrvTTDLSVUpdateParam(void)
{    
    static UINT32 u4HomoMo[4] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
    static UINT8 u1MoCnt = 0;

    u4HomoMo[u1MoCnt] = u4DrvDIGetFusionMotionLevel();
    u1MoCnt = ((u1MoCnt+1)&0x3);   
    
    m_depth_width = IO32ReadFldAlign(TTD_FW_REG_04, TTD_DE_ARRAY_W); 
    m_depth_height = IO32ReadFldAlign(TTD_FW_REG_04, TTD_DE_ARRAY_H);    

    SR_H = IO32ReadFldAlign(TTD_FW_REG_05, TTD_SR_H); 
    step_h = IO32ReadFldAlign(TTD_FW_REG_05, TTD_STEP_H); 
    m_mbSize = IO32ReadFldAlign(TTD_FW_REG_06, TTD_MB_SIZE); 
    cand_num = IO32ReadFldAlign(TTD_FW_REG_06, TTD_CAND_NUM); 
    lsv_focal_plen =  IO32ReadFldAlign(TTD_FW_REG_08, LSV_CONVG_PLEN);      
    lsv_auto_gain = IO32ReadFldAlign(SW_TTD_REG_00, SW_LSV_CONVG_GAIN);
    lsv_local_gain = IO32ReadFldAlign(SW_TTD_REG_00, SW_LSV_ENH_GAIN);
    d2d_convg =  IO32ReadFldAlign(TTD_FW_REG_10, D2D_CONVG); 
    d2d_ratio =  IO32ReadFldAlign(TTD_FW_REG_10, D2D_RATIO); 
    d2d_gain  =  IO32ReadFldAlign(TTD_FW_REG_10, D2D_GAIN); 
    var_en_th = IO32ReadFldAlign(TTD_FW_REG_06, TTD_VAR_EN_TH); 
    var_pnlty_sel = IO32ReadFldAlign(TTD_FW_REG_02, TTD_VAR_SEL);
    var_cor_th = IO32ReadFldAlign(TTD_FW_REG_02, TTD_VAR_COR_TH); 
    me_mvpnlty_sel = IO32ReadFldAlign(TTD_FW_REG_02, TTD_SMTH_SEL);
    ord_sel = IO32ReadFldAlign(TTD_FW_REG_02, TTD_ORD_QUAD_SEL); 
    obme_num= IO32ReadFldAlign(TTD_FW_REG_05, TTD_OBME_NUM); 
    zero_sel = IO32ReadFldAlign(TTD_FW_REG_02, TTD_ZERO_SEL);
    u4DebugCnt = IO32ReadFldAlign(TTD_FW_REG_16, TTD_DEBUG_CNT);
    u4CalLoop = IO32ReadFldAlign(SW_TTD_REG_00, SW_LSV_DELAY_TIMES);
    u4WidthDec = IO32ReadFldAlign(SW_TTD_REG_00, SW_LSV_WIDTH_DEC);    
    u4HeightDec = IO32ReadFldAlign(SW_TTD_REG_00, SW_LSV_HEIGHT_DEC);

    m_depth_width = (m_depth_width > (u4WidthDec*2)) ? (m_depth_width - (u4WidthDec*2)) : 2;
    m_depth_height = (m_depth_height>u4HeightDec) ? (m_depth_height-u4HeightDec) : 1; 
    
    if ((u4HomoMo[0] == 0) && (u4HomoMo[1] == 0) 
        && (u4HomoMo[2] == 0) && (u4HomoMo[3] == 0))
    {        
        rand_dvpnlty_sel = IO32ReadFldAlign(TTD_FW_REG_16, TTD_RAND_SEL_FREEZE);        
        tmpr_dvpnlty_sel = IO32ReadFldAlign(TTD_FW_REG_16, TTD_TMPR_SEL_FREEZE);
        iIIR = IO32ReadFldAlign(TTD_FW_REG_16, TTD_IIR_FREEZE);
    }
    else
    {
        rand_dvpnlty_sel = IO32ReadFldAlign(TTD_FW_REG_02, TTD_RAND_SEL);        
        tmpr_dvpnlty_sel = IO32ReadFldAlign(TTD_FW_REG_02, TTD_TMPR_SEL);        
        iIIR = IO32ReadFldAlign(TTD_FW_REG_16, TTD_IIR_NORMAL);        
    }
}


void vDrvTTDLumaSum(UINT32 size, PEL *SRYL, PEL *SRYR)
{      
    UINT32 i;

    if (u1DrvTTDGet3DDepthCtrl())
    {
        u4LumaSumL = 0;
        u4LumaSumR = 0;
        u4LumaSumDiffThld = size*0x10;
        
        for (i=0; i < size; i++)
        {           
            u4LumaSumL += SRYL[i];
            u4LumaSumR += SRYR[i];
        }
    }
}

static UINT32 u4DrvNaviMatch(void)
{
    UINT32 u43DMode = u4DrvTDTV3DModeQuery();
    UINT32 u4SbsCnt, u4TabCnt, u4SbsVld, u4TabVld;
    UINT32 u4SbsRatio, u4TabRatio, u4Ret;

    u4SbsCnt = IO32ReadFldAlign(TDNVAI_STA1, STA_SBS_CNT);
    u4TabCnt = IO32ReadFldAlign(TDNVAI_STA1, STA_TAB_CNT);
    u4SbsVld = IO32ReadFldAlign(TDNVAI_STA0, STA_SBS_VLD_BLK);
    u4TabVld = IO32ReadFldAlign(TDNVAI_STA0, STA_TAB_VLD_BLK);
    u4SbsRatio = 0;
    u4TabRatio = 0;
    u4Ret = SV_TRUE;

    if ((u43DMode == E_TDTV_UI_3D_MODE_SBS) || u43DMode == E_TDTV_UI_3D_MODE_SS)
    {
        if (u4SbsVld > 120)
        {
            u4SbsRatio = (100 * u4SbsCnt) / u4SbsVld;
            u4Ret = (u4SbsRatio > 50) ? SV_TRUE : SV_FALSE;
        }
        else
        {
            u4Ret = SV_TRUE;
        }
    }
    else if (u43DMode == E_TDTV_UI_3D_MODE_TB)
    {
        if (u4TabVld > 120)
        {
            u4TabRatio = (100 * u4TabCnt) / u4TabVld;
            u4Ret = (u4TabRatio > 50) ? SV_TRUE : SV_FALSE;
        }
        else
        {
            u4Ret = SV_TRUE;
        }
    }

    if (IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_NAVI_LOG))
    {
        LOG(0, "NaviMatch = %d\n", u4Ret);
        LOG(0, "u4SbsCnt = %d, u4SbsVld = %d, u4SbsRatio = %d\n", u4SbsCnt, u4SbsVld, u4SbsRatio);
        LOG(0, "u4TabCnt = %d, u4TabVld = %d, u4TabRatio = %d\n", u4TabCnt, u4TabVld, u4TabRatio);
    }
    
    return u4Ret;
}

void vDrvTTDLSVCalculate(PEL *SRYL, PEL *SRYR)
{      
    static UINT32 u4DiffPat = 0;    
    static UINT8 bCount = 0;  
    static UINT8 u1LsvEnable = SV_FALSE;

    if (u1DrvTTDGet3DDepthCtrl())
    {           
        vDrvTTDLSVUpdateParam();
        
        blk_var_calc(SRYL, step_v,step_h,str_h);
        
        if(bCount%2==0)
            DVSearchLPS(SRYL, SRYR, DE_buf_L, pre_depth_buf_L, SR_H, SR_V,bCount,step_v,step_h,str_h);
        else
            DVSearchLPS_Back(SRYL, SRYR, DE_buf_L, pre_depth_buf_L, SR_H, SR_V,bCount,step_v,step_h,str_h);

        //global disparity analysis
    	AdaptiveMW(&shift_avg_H, SR_H, SR_V); 
        shift_avg_H = shift_avg_H/2;    // TBD

        //compensate upscaler effect
        shift_avg_H = (shift_avg_H*iSclRatio)/64;      
        
    	//disparity buffer store
        DepthBlend(pre_depth_buf_L, DE_buf_L);
    	
    	//analysis global disparity stability
    	if(pre_shift_avg_H==shift_avg_H)
    	{
    	 	if(stab_cnt>=8)		
            {
                stab_flag=1;
    		}
    		else
            {
                stab_cnt++;
    		}
    	}
    	else
    	{
    		stab_cnt=0;
    		stab_flag=0;
    	}
        
    	//current global disparity decision
        if (u1LsvEnable == SV_TRUE)
        {
        	if (cur_convg!=shift_avg_H)
        	{
        		if (stab_flag==0)	
            	{                    
                    cur_convg = cur_convg+(shift_avg_H-cur_convg)/iIIR;        
            	}
        		else 
        		{
        			if (shift_avg_H>cur_convg)		
                    {
                        cur_convg++;
        			}
        			else if(shift_avg_H<cur_convg)	
                    {
        			    cur_convg--;
        			}
        		}
        	}
        }
        else
        {
            cur_convg = shift_avg_H;
            u1LsvEnable = SV_TRUE;
            LOG(3, "LSV enable : cur_convg %d  shift_avg_H %d \n", cur_convg/2, shift_avg_H/2); 
        }

        // Disable 3DC when LR are different images 
        if ((Diff(u4LumaSumL, u4LumaSumR) > u4LumaSumDiffThld)
            || (u4DrvNaviMatch() == SV_FALSE))
        {
            u4DiffPat = (u4DiffPat < 0x8) ? (u4DiffPat+1) : 0x8;
        }
        else
        {
            u4DiffPat = 0;
        }

        if (u4DiffPat >= 8) 
        {            
            cur_convg = 0;
            shift_avg_H = 0;                    
            LOG(3, "Disable 3DC : Sum %08x %08x   Diff %08x Thld %08x\n", 
                u4LumaSumL, u4LumaSumR, Diff(u4LumaSumL, u4LumaSumR), u4LumaSumDiffThld);       
        }
        
    	pre_shift_avg_H=shift_avg_H;	
    }
    else
    {         
        cur_convg = 0;
        shift_avg_H = 0;
        lsv_auto_gain = 32;
        lsv_local_gain = 0;
        u1LsvEnable = SV_FALSE;
    }

    if (bCount++ >= u4DebugCnt)
    {
        LOG(3, "cur_convg %d  shift_avg_H %d \n", cur_convg/2, shift_avg_H/2); 
        LOG(5, "IIR %d   Rand %d   Tmpr %d \n", iIIR, rand_dvpnlty_sel, tmpr_dvpnlty_sel); 
        bCount = 0;
    }   

    vIO32WriteFldMulti(TTD_FW_REG_01, P_Fld(pre_shift_avg_H, TTD_PRE_SHIFT_AVG_H)
                                    | P_Fld(shift_avg_H, TTD_SHIFT_AVG_H));                                        
    vIO32WriteFldAlign(TTD_FW_REG_07, cur_convg, TTD_CUR_CONVG);      
    vIO32WriteFldAlign(TTD_FW_REG_05, stab_cnt, TTD_STAB_CNT);  
}

/*-----------------------------------------------------------------------*
 * LSV interface
 *-----------------------------------------------------------------------*/

void vDrvTTDLSVInit(void)
{   
    if (u1Lsvinit != SV_FALSE)
    {
        LOG(0, "Lsv already init\n");
        return;
    }
    
    m_width = 1920;          // 1920
    m_height = 1080;        // 1080
    m_depth_width  = 32;     // 16
    m_depth_height = 8;    // 8

    m_lsv_width  = LSV_DES_WIDTH*2; // dram width
    m_lsv_height = LSV_DES_HEIGHT;  // dram height 
    str_h = (m_lsv_width-m_depth_width*step_h*m_mbSize)/2;
        
    DE_buf_L = (INT32 *)x_mem_alloc(sizeof(INT32)*(m_depth_width*m_depth_height));  
    pre_depth_buf_L = (INT32 *)x_mem_alloc(sizeof(INT32)*(m_depth_width*m_depth_height));    
    dv_meter_LH = (INT32 *)x_mem_alloc(sizeof(INT32)*(SR_H*2+1));  

    if ((DE_buf_L == NULL) || (pre_depth_buf_L == NULL) || (dv_meter_LH == NULL))
    {        
        LOG(0, "Lsv init fail : allocate memory fail\n");
        return;
    }

    x_memset((void *)DE_buf_L, 0, sizeof(INT32)*m_depth_width*m_depth_height);
    x_memset((void *)pre_depth_buf_L, 0, sizeof(INT32)*m_depth_width*m_depth_height);
    x_memset((void *)dv_meter_LH, 0, sizeof(INT32)*(SR_H*2+1));

    vIO32WriteFldMulti(TTD_FW_REG_02, P_Fld(3, TTD_ORD_QUAD_SEL)
                                    | P_Fld(2, TTD_SMTH_SEL)
                                    | P_Fld(1, TTD_TMPR_SEL)
                                    | P_Fld(3, TTD_ZERO_SEL)
                                    | P_Fld(3, TTD_VAR_SEL)
                                    | P_Fld(1, TTD_RAND_SEL)
                                    | P_Fld(4, TTD_VAR_COR_TH)); 
    
	vIO32WriteFldMulti(TTD_FW_REG_04, P_Fld(32, TTD_DE_ARRAY_W)
                                    | P_Fld(8, TTD_DE_ARRAY_H)); 
    
	vIO32WriteFldMulti(TTD_FW_REG_05, P_Fld(32, TTD_Y_SUBSAMPLE)
                                    | P_Fld(80, TTD_SR_H)
                                    | P_Fld(4, TTD_OBME_NUM)
                                    | P_Fld(4, TTD_STEP_H)); 
    
	vIO32WriteFldMulti(TTD_FW_REG_06, P_Fld(4, TTD_MB_SIZE)
                                    | P_Fld(4, TTD_CAND_NUM)
                                    | P_Fld(64,TTD_VAR_EN_TH)); 

	vIO32WriteFldMulti(TTD_FW_REG_07, P_Fld(2, TTD_FG_WEIGHT)
									| P_Fld(1, TTD_BG_WEIGHT)
									| P_Fld(128, AMW_FOCAL_PLEN)); 

	vIO32WriteFldMulti(TTD_FW_REG_08, P_Fld(128, LSV_CONVG_PLEN)
                                    | P_Fld(128, LSV_CID_APL_AVG)); 
    
    vIO32WriteFldMulti(TTD_FW_REG_16, P_Fld(4, TTD_IIR_NORMAL)
                                    | P_Fld(8, TTD_IIR_FREEZE)
                                    | P_Fld(0, TTD_TMPR_SEL_FREEZE)             
                                    | P_Fld(3, TTD_RAND_SEL)
                                    | P_Fld(8, TTD_DEBUG_CNT)); 
    
    vIO32WriteFldMulti(SW_TTD_REG_00, P_Fld(32, SW_LSV_CONVG_GAIN)                                    
                                    | P_Fld(0, SW_LSV_ENH_GAIN));     

#ifdef DEF_3DC_LITE_VERSION
    vIO32WriteFldMulti(SW_TTD_REG_00, P_Fld(2, SW_LSV_WIDTH_DEC)
                                       | P_Fld(2, SW_LSV_HEIGHT_DEC)
                                       | P_Fld(1, SW_LSV_DELAY_TIMES));    
#else
    vIO32WriteFldMulti(SW_TTD_REG_00, P_Fld(0, SW_LSV_WIDTH_DEC)
                                       | P_Fld(0, SW_LSV_HEIGHT_DEC)
                                       | P_Fld(0, SW_LSV_DELAY_TIMES));    
#endif

	vIO32WriteFldMulti(TTD_FW_REG_10, P_Fld(CONVERGENCE_DEPTH_3D , D2D_CONVG)
                                    | P_Fld(DEPTH_RANGE_RATIO_3D  , D2D_RATIO)
                                    | P_Fld(DISPARITY_GAIN_3D  , D2D_GAIN)); 

    // CID inverse 
    vIO32WriteFldMulti(TTD_FW_REG_03, P_Fld(0 , TTD_HIST_THLD)
                                    | P_Fld(80, TTD_AVG_THLD)
                                    | P_Fld(61, TTD_DIVER_THLD_L)
                                    | P_Fld(146, TTD_DIVER_THLD_H));    

    u1Lsvinit = SV_TRUE; 
}





