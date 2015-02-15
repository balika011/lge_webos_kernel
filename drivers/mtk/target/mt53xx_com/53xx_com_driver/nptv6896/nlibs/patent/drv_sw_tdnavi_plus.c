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

/*----------------------------------------------------------------------------*
 * Global/Static variables
 *----------------------------------------------------------------------------*/
#if defined(CC_SUPPORT_SW_NAVI_PLUS)


#include "x_os.h"
#include "drv_di.h"

EXTERN void HalFlushInvalidateDCache(void);

#define SW_NAVI_DEBUG
#define NAVI_DATA_BUF_WIDTH 1920
#define NAVI_LINE_WIDTH     32
#define NAVI_LINE_HEIGHT    16

typedef enum
{
    NP_WAIT,
    NP_COPY,
    NP_SUBSAMPLE,
    NP_CHK,
}NAVI_PLUS_STATE;

static NAVI_PLUS_STATE eNPState = NP_WAIT;
static TD_NAVI_DETECT SwNaviPlusDetected=TD_NAVI_DETECT_2D;


static UINT8 u1RawDataBuf[NAVI_DATA_BUF_WIDTH*NAVI_LINE_HEIGHT+1]; // Add 1 for reducing calculation

static UINT32 blk_buf[4][NAVI_LINE_WIDTH/2*NAVI_LINE_HEIGHT/2];
static UINT32 blk_diff[4][NAVI_LINE_WIDTH/2*NAVI_LINE_HEIGHT/2];
static UINT32 blk_var[4][NAVI_LINE_WIDTH/2*NAVI_LINE_HEIGHT/2];


static int	gmv_s0 = 0;
static int	gmv_s1 = 0;
static int	gmv_t0 = 0;
static int	gmv_t1 = 0;

static int	gmv2_s0 = 0;
static int	gmv2_s1 = 0;
static int	gmv2_t0 = 0;
static int	gmv2_t1 = 0;

static int blk_apl = 128;

char det_type[10];
int confidence;
static int similar_thr;


static int direct_cnt_s0[3];
static int direct_cnt_s1[3];
static int direct_cnt_t0[3];
static int direct_cnt_t1[3];

static int w,h;
int bw,bh;
int blk_grp;
int temp_int;
static int like_sbs;
static int like_tab;
static int invalid_blk_like_sbs;
static int invalid_blk_like_tab;
static int sbs_neighbor_diff_cnt;
static int tab_neighbor_diff_cnt;
static int sbs_vld_blk;
static int tab_vld_blk;
static int sbs_diff_sum;
static int tab_diff_sum;
int valid_sbs_ratio;
int valid_tab_ratio;
static int curr_pos;
static int last_pos;
bool is_similar;
static int temp_a[5];
static int temp_b[5];
static int valid_blk_cnt;
static int min_vld_blk;
static int wt_sbs_vld_blk;
static int wt_tab_vld_blk;

static int gmv_s0_in, gmv_s1_in, gmv_t0_in, gmv_t1_in;
static int gmv2_s0_in, gmv2_s1_in, gmv2_t0_in, gmv2_t1_in;
static int s0_cnt_all, s1_cnt_all, t0_cnt_all, t1_cnt_all;
static int gmv_s0_tmp, gmv_s1_tmp, gmv_t0_tmp, gmv_t1_tmp;
static int gmv2_s0_tmp, gmv2_s1_tmp, gmv2_t0_tmp, gmv2_t1_tmp;


int  c_blk_size_x        = -1;
int  c_blk_size_y        = -1;
int  c_pxl_diff_thr      = 5;
int  c_var_thr           = 0;
int  c_valid_blk_thr     = 10*16/16;
int  c_d_th                    = 218;
int  c_d_th_in                = 218; 
int  c_d_th_out              = 160; 
bool c_adap_thr_en       = 1;
int  c_adap_thr_shift    = 3;  
int  c_similar_thr       = 5;
int  c_ambig_thr         = 9;  
int  c_decision_type     = 1;  
int  c_neighbor_diff_thr = 5;  
int  c_neighbor_cnt_thr  = 8;
int  c_ord_cnt_th        = 3;  //max 4
int  c_fw_ratio_gain_th  = 30;
int  c_fw_hw_ratio_weight= 3; //0~4
int  c_line_num          = 16;
int  c_hor_blk           = -1; 
int  c_gmv_blk_th        = 30*16/16;
int  c_gmv_en            = 1;
int  c_gmv2_en           = 1;
int  c_decision_type_ex  = 7; 
int  c_gmv_sbs_max       = 4; 
int  c_gmv_tab_max       = 8; 


int  sta_sbs_cnt         = -1;
int  sta_tab_cnt         = -1;
int  sta_sbs_vld_cnt     = -1;
int  sta_tab_vld_cnt     = -1;
int  sta_blk_apl         = -1;
int  sta_total_blk       = -1;
int  sta_invld_sbs       = -1;
int  sta_invld_tab       = -1;
int  sta_sbs_neighbor_cnt= -1;
int  sta_tab_neighbor_cnt= -1;


int  sta_gmv_s0          = -1;
int  sta_gmv_s1          = -1;
int  sta_gmv_t0          = -1;
int  sta_gmv_t1          = -1;
int  sta_gmv2_s0         = -1;
int  sta_gmv2_s1         = -1;
int  sta_gmv2_t0         = -1;
int  sta_gmv2_t1         = -1;


TD_NAVI_DETECT final_sta_det_type=TD_NAVI_DETECT_2D;


//======================================================================================
//SBS TNB Detection Performance improvement
//======================================================================================

static inline bool decide_similar_fw(int* blk_a, int* blk_b, int similar_thr, int gmv2, int c_decision_type,int c_decision_type_ex)
{
    int th = 2;
    int ord_a[4];
    int ord_b[4];
    int i;
    int ord_cnt = 0;

    bool condition0;
    bool condition1;
    bool condition2;
    bool condition3;
    bool condition4;
    bool condition5;

    bool condition20;
    bool condition21;                 
    bool condition22;

    bool condition23;                                                        
    bool condition24;                  
    bool condition25;
     
    if((blk_a[2]-blk_a[1])>th)      ord_a[0] = 0;
    else if((blk_a[1]-blk_a[2])>th) ord_a[0] = 1;
    else                            ord_a[0] = 2;
    if((blk_a[2]-blk_a[3])>th)      ord_a[1] = 0;
    else if((blk_a[3]-blk_a[2])>th) ord_a[1] = 1;
    else                            ord_a[1] = 2;
    if((blk_a[3]-blk_a[1])>th)      ord_a[2] = 0;
    else if((blk_a[1]-blk_a[3])>th) ord_a[2] = 1;
    else                            ord_a[2] = 2;
    if((blk_a[3]-blk_a[2]-blk_a[2]+blk_a[1])>th)        ord_a[3] = 0;
    else if((blk_a[2]-blk_a[3]-blk_a[1]+blk_a[2])>th)   ord_a[3] = 1;
    else                                                ord_a[3] = 2;
    if((blk_b[2]-blk_b[1])>th)      ord_b[0] = 0;
    else if((blk_b[1]-blk_b[2])>th) ord_b[0] = 1;
    else                            ord_b[0] = 2;
    if((blk_b[2]-blk_b[3])>th)      ord_b[1] = 0;
    else if((blk_b[3]-blk_b[2])>th) ord_b[1] = 1;
    else                            ord_b[1] = 2;
    if((blk_b[3]-blk_b[1])>th)      ord_b[2] = 0;
    else if((blk_b[1]-blk_b[3])>th) ord_b[2] = 1;
    else                            ord_b[2] = 2;
    if((blk_b[3]-blk_b[2]-blk_b[2]+blk_b[1])>th)        ord_b[3] = 0;
    else if((blk_b[2]-blk_b[3]-blk_b[1]+blk_b[2])>th)   ord_b[3] = 1;
    else                                                ord_b[3] = 2;
    
    for(i=0;i<4;i++)
    {
        if(ord_a[i]==ord_b[i])  ord_cnt ++;
    }
    
      condition0 = abs(blk_a[2] - blk_b[2])<=similar_thr;
      condition1 = abs(blk_a[2] - (blk_b[1]+blk_b[2])/2)<=similar_thr ||
                      abs(blk_a[2] - (blk_b[2]+blk_b[3])/2)<=similar_thr ||
                      abs(blk_b[2] - (blk_a[1]+blk_a[2])/2)<=similar_thr ||
                      abs(blk_b[2] - (blk_a[2]+blk_a[3])/2)<=similar_thr ;
      condition2 = abs(blk_a[2] - blk_b[1])<=similar_thr ||
                      abs(blk_a[2] - blk_b[3])<=similar_thr ||
                      abs(blk_b[2] - blk_a[1])<=similar_thr ||
                      abs(blk_b[2] - blk_a[3])<=similar_thr ;
    //new condition
    //ORD1
      condition3 = (!(c_decision_type_ex&0x1))||
                      (((((blk_b[1]*1+blk_b[2]*2+blk_b[3]*1+2)/4-blk_b[1])*((blk_a[1]*1+blk_a[2]*2+blk_a[3]*1+2)/4-blk_a[1])>=0)||
                       (((blk_b[1]*1+blk_b[2]*2+blk_b[3]*1+2)/4-blk_b[3])*((blk_a[1]*1+blk_a[2]*2+blk_a[3]*1+2)/4-blk_a[3])>=0))&&
                       ((blk_b[3]-blk_b[1])*(blk_a[3]-blk_a[1])>=0));
    //ORD2
      condition4 = (c_decision_type_ex&0x2)&&
                      (ord_cnt>=c_ord_cnt_th);
    //Sample miss
      condition5 = (c_decision_type_ex&0x4)&&
                      ((((blk_b[1]*2+blk_b[2]*2+2)/4-blk_a[2])*(blk_b[2]-blk_a[2]) <0) ||
                       (((blk_b[3]*2+blk_b[2]*2+2)/4-blk_a[2])*(blk_b[2]-blk_a[2]) <0) ||
                       (((blk_a[1]*2+blk_a[2]*2+2)/4-blk_b[2])*(blk_a[2]-blk_b[2]) <0) ||
                       (((blk_a[3]*2+blk_a[2]*2+2)/4-blk_b[2])*(blk_a[2]-blk_b[2]) <0));

    condition1 = condition1 | condition5;
    condition0 = condition0&condition3;
    condition1 = condition1&condition3;
    condition1 = condition1 | condition4;

    condition20 = abs(blk_a[2] - blk_b[2+gmv2])<=similar_thr;
    condition21 = abs(blk_a[2] - (blk_b[1+gmv2]+blk_b[2+gmv2])/2)<=similar_thr ||
                   abs(blk_a[2] - (blk_b[2+gmv2]+blk_b[3+gmv2])/2)<=similar_thr ||
                   abs(blk_b[2+gmv2] - (blk_a[1]+blk_a[2])/2)<=similar_thr ||
                   abs(blk_b[2+gmv2] - (blk_a[2]+blk_a[3])/2)<=similar_thr ;
    condition22 = abs(blk_a[2] - blk_b[1+gmv2])<=similar_thr ||
                   abs(blk_a[2] - blk_b[3+gmv2])<=similar_thr ||
                   abs(blk_b[2+gmv2] - blk_a[1])<=similar_thr ||
                   abs(blk_b[2+gmv2] - blk_a[3])<=similar_thr ;

    if((blk_a[2]-blk_a[1])>th)      ord_a[0] = 0;
    else if((blk_a[1]-blk_a[2])>th) ord_a[0] = 1;
    else                            ord_a[0] = 2;
    if((blk_a[2]-blk_a[3])>th)      ord_a[1] = 0;
    else if((blk_a[3]-blk_a[2])>th) ord_a[1] = 1;
    else                            ord_a[1] = 2;
    if((blk_a[3]-blk_a[1])>th)      ord_a[2] = 0;
    else if((blk_a[1]-blk_a[3])>th) ord_a[2] = 1;
    else                            ord_a[2] = 2;
    if((blk_a[3]-blk_a[2]-blk_a[2]+blk_a[1])>th)        ord_a[3] = 0;
    else if((blk_a[2]-blk_a[3]-blk_a[1]+blk_a[2])>th)   ord_a[3] = 1;
    else                                                ord_a[3] = 2;
    if((blk_b[2+gmv2]-blk_b[1+gmv2])>th)        ord_b[0] = 0;
    else if((blk_b[1+gmv2]-blk_b[2+gmv2])>th)   ord_b[0] = 1;
    else                            ord_b[0] = 2;
    if((blk_b[2+gmv2]-blk_b[3+gmv2])>th)        ord_b[1] = 0;
    else if((blk_b[3+gmv2]-blk_b[2+gmv2])>th)   ord_b[1] = 1;
    else                            ord_b[1] = 2;
    if((blk_b[3+gmv2]-blk_b[1+gmv2])>th)        ord_b[2] = 0;
    else if((blk_b[1+gmv2]-blk_b[3+gmv2])>th)   ord_b[2] = 1;
    else                            ord_b[2] = 2;
    if((blk_b[3+gmv2]-blk_b[2+gmv2]-blk_b[2+gmv2]+blk_b[1+gmv2])>th)        ord_b[3] = 0;
    else if((blk_b[2+gmv2]-blk_b[3+gmv2]-blk_b[1+gmv2]+blk_b[2+gmv2])>th)   ord_b[3] = 1;
    else                                                ord_b[3] = 2;
    ord_cnt = 0;
    
    for(i=0;i<4;i++)
    {
        if(ord_a[i]==ord_b[i])  ord_cnt ++;
    }

    //new condition2
      condition23 = (!(c_decision_type_ex&0x1))||
                       (((((blk_b[1+gmv2]*1+blk_b[2+gmv2]*2+blk_b[3+gmv2]*1+2)/4-blk_b[1+gmv2])*((blk_a[1]*1+blk_a[2]*2+blk_a[3]*1+2)/4-blk_a[1])>=0)||
                        (((blk_b[1+gmv2]*1+blk_b[2+gmv2]*2+blk_b[3+gmv2]*1+2)/4-blk_b[3+gmv2])*((blk_a[1]*1+blk_a[2]*2+blk_a[3]*1+2)/4-blk_a[3])>=0))&&
                        ((blk_b[3+gmv2]-blk_b[1+gmv2])*(blk_a[3]-blk_a[1])>=0));
      condition24 = (c_decision_type_ex&0x2)&&
                      (ord_cnt>=c_ord_cnt_th);
      condition25 = (c_decision_type_ex&0x4)&&
                       ((((blk_b[1+gmv2]*2+blk_b[2+gmv2]*2+2)/4-blk_a[2])*(blk_b[2+gmv2]-blk_a[2]) <0) ||
                        (((blk_b[3+gmv2]*2+blk_b[2+gmv2]*2+2)/4-blk_a[2])*(blk_b[2+gmv2]-blk_a[2]) <0) ||
                        (((blk_a[1]*2+blk_a[2]*2+2)/4-blk_b[2+gmv2])*(blk_a[2]-blk_b[2+gmv2]) <0) ||
                        (((blk_a[3]*2+blk_a[2]*2+2)/4-blk_b[2+gmv2])*(blk_a[2]-blk_b[2+gmv2]) <0));

    condition21 = condition21 | condition25;
    condition20 = condition20&condition23;
    condition21 = condition21&condition23;
    condition21 = condition21 | condition24;

    condition0 = condition0 | condition20;
    condition1 = condition1 | condition21;
    condition2 = condition2 | condition22;

    // 0 1 2 3 4
    if(  condition0 ||
        (condition1 && (c_decision_type&0x1)) 
      ||(condition2 && (c_decision_type&0x2)) 
        )
        return true;
    else
        return false;
}


static inline int similar_direct_fw(int* blk_a, int* blk_b)
{
	int diff0, diff1, diff2;

	diff0 = (abs(blk_a[1]-blk_b[0]) + abs(blk_a[2]-blk_b[1])*2 + abs(blk_a[3]-blk_b[2]) + 2)/4;
	diff1 = (abs(blk_a[1]-blk_b[1]) + abs(blk_a[2]-blk_b[2])*2 + abs(blk_a[3]-blk_b[3]) + 2)/4;
	diff2 = (abs(blk_a[1]-blk_b[2]) + abs(blk_a[2]-blk_b[3])*2 + abs(blk_a[3]-blk_b[4]) + 2)/4;

	if((diff1<=diff0&&diff1<=diff2)||
	    ((((blk_b[1]*1+blk_b[2]*2+blk_b[3]*1+2)/4-blk_b[1])*((blk_a[1]*1+blk_a[2]*2+blk_a[3]*1+2)/4-blk_a[1])>0)&&
            (((blk_b[1]*1+blk_b[2]*2+blk_b[3]*1+2)/4-blk_b[3])*((blk_a[1]*1+blk_a[2]*2+blk_a[3]*1+2)/4-blk_a[3])>0)&&
            ((blk_b[3]-blk_b[1])*(blk_a[3]-blk_a[1])>0)&&
            ((blk_b[3]-blk_b[2]-blk_b[2]+blk_b[1])*(blk_a[3]-blk_a[2]-blk_a[2]+blk_a[1])>0)))
		return 0;
	else if(diff0<=diff1&&diff0<=diff2)
		return -1;
	else if(diff2<=diff1&&diff2<=diff0)
		return 1;
	else
		return 0;
}

static inline int clipping(int in, int max, int min)
{
	if(in>max)
		return max;
	else if(in<min)
		return min;
	else
		return in;
}

TD_NAVI_DETECT eDrvNaviPlusDetectedFmtQuery(void)
{
    //return final_sta_det_type=TD_NAVI_DETECT_2D;
    LOG(7,"-----final_sta_det_type(%d)------------------------\n",final_sta_det_type);    
    return final_sta_det_type;
}

void vDrvTDNaviPlusReset(void)
{
	int i;

    for (i=0;i<4;i++)
	{
		memset(blk_buf[i],0,sizeof(int)*(NAVI_LINE_WIDTH/2)*(NAVI_LINE_HEIGHT/2));
		memset(blk_var[i],0,sizeof(int)*(NAVI_LINE_WIDTH/2)*(NAVI_LINE_HEIGHT/2));
	}

	for(i=0;i<3;i++) {
		direct_cnt_s0[i] = 0;
		direct_cnt_s1[i] = 0;
		direct_cnt_t0[i] = 0;
		direct_cnt_t1[i] = 0;
	}	
	
	like_sbs=0;
	like_tab=0;
	invalid_blk_like_sbs=0;
	invalid_blk_like_tab=0;
	sbs_vld_blk=0;
	tab_vld_blk=0;
	sbs_diff_sum=0;
	tab_diff_sum=0;
	sbs_neighbor_diff_cnt=0;
	tab_neighbor_diff_cnt=0;
}

void vDrvTDNaviPlusDet(void)
{
    int i;

    int sta_hw_sbs_cnt      = vDrvTdNaviGetSbsCnt();
    int sta_hw_tab_cnt      = vDrvTdNaviGetTabCnt();
    int sta_hw_sbs_vld_cnt  = vDrvTdNaviGetSbsVldBlk();
    int sta_hw_tab_vld_cnt  = vDrvTdNaviGetTabVldBlk();
    int hw_valid_sbs_ratio ;
    int hw_valid_tab_ratio ;

    //--------------------------------------------------------------
    if(c_adap_thr_en) // adaptive thr by APL
    {
        similar_thr = blk_apl >> c_adap_thr_shift;
        if(similar_thr > c_similar_thr) similar_thr = c_similar_thr;
    }
    else
        similar_thr = c_similar_thr;

    // 0 1
    // 2 3
    // check SBS/TAB
    gmv_s0_in = c_gmv_en==1 ? gmv_s0 : 0;
    gmv_s1_in = c_gmv_en==1 ? gmv_s1 : 0;
    gmv_t0_in = c_gmv_en==1 ? gmv_t0 : 0;
    gmv_t1_in = c_gmv_en==1 ? gmv_t1 : 0;
    gmv2_s0_in = c_gmv2_en==1 ? gmv2_s0 : gmv_s0_in;
    gmv2_s1_in = c_gmv2_en==1 ? gmv2_s1 : gmv_s1_in;
    gmv2_t0_in = c_gmv2_en==1 ? gmv2_t0 : gmv_t0_in;
    gmv2_t1_in = c_gmv2_en==1 ? gmv2_t1 : gmv_t1_in;

    for(h=0;h<(NAVI_LINE_HEIGHT/2);h++)
    {   
        for(w=0;w<(NAVI_LINE_WIDTH/2);w++)
        {
            curr_pos = (h*(NAVI_LINE_WIDTH/2)+w);
            blk_diff[0][curr_pos] = 255;
            blk_diff[1][curr_pos] = 255;
            blk_diff[2][curr_pos] = 255;
            blk_diff[3][curr_pos] = 255;

            // Compare Up-left and Up-right (SBS up part)
            for(i=-2;i<3;i++)
            {
                temp_a[i+2]= (w+i<0 || w+i>(NAVI_LINE_WIDTH/2)-1)? blk_buf[0][curr_pos]:blk_buf[0][curr_pos+i];
                temp_b[i+2]= (w+i+gmv_s0_in<0) ?       blk_buf[1][curr_pos-w] :
                             (w+i+gmv_s0_in>(NAVI_LINE_WIDTH/2)-1) ? blk_buf[1][curr_pos-w+(NAVI_LINE_WIDTH/2)-1] : blk_buf[1][curr_pos+i+gmv_s0_in];                
            }
            is_similar = decide_similar_fw(temp_a, temp_b, similar_thr, gmv2_s0_in-gmv_s0_in, c_decision_type, c_decision_type_ex);

            if(blk_var[0][curr_pos]>c_var_thr || blk_var[1][curr_pos]>c_var_thr)
            {
                sbs_vld_blk++; 
                if(is_similar)	{like_sbs++; blk_diff[0][curr_pos] = 0;}
                direct_cnt_s0[similar_direct_fw(temp_a, temp_b)+1]++;
            }
            else
            {
                blk_diff[0][curr_pos] = 128; // invalid
                if(is_similar)
                    invalid_blk_like_sbs++;
            }

            last_pos = curr_pos - 1;
            if(last_pos>=0)
            {
                if(abs(blk_buf[0][curr_pos]-blk_buf[0][last_pos]) > c_neighbor_diff_thr)
                    sbs_neighbor_diff_cnt++;
                if(abs(blk_buf[1][curr_pos]-blk_buf[1][last_pos]) > c_neighbor_diff_thr)
                    sbs_neighbor_diff_cnt++;
            }

            // Compare Down-left and Down-right (SBS down part)
            for(i=-2;i<3;i++)
            {
                temp_a[i+2]= (w+i<0 || w+i>(NAVI_LINE_WIDTH/2)-1)? blk_buf[2][curr_pos]:blk_buf[2][curr_pos+i];
                temp_b[i+2]= (w+i+gmv_s1_in<0) ?       blk_buf[3][curr_pos-w] :
                             (w+i+gmv_s1_in>(NAVI_LINE_WIDTH/2)-1) ? blk_buf[3][curr_pos-w+(NAVI_LINE_WIDTH/2)-1] : blk_buf[3][curr_pos+i+gmv_s1_in];
                
            }
            is_similar = decide_similar_fw(temp_a, temp_b, similar_thr, gmv2_s1_in-gmv_s1_in, c_decision_type, c_decision_type_ex);

            if(blk_var[2][curr_pos]>c_var_thr || blk_var[3][curr_pos]>c_var_thr)
            {
                sbs_vld_blk++;
                if(is_similar)	{like_sbs++; blk_diff[1][curr_pos] = 0;}
                direct_cnt_s1[similar_direct_fw(temp_a, temp_b)+1]++;
            }
            else
            {
                blk_diff[1][curr_pos] = 128; // invalid
                if(is_similar)	
                    invalid_blk_like_sbs++;
            }

            last_pos = curr_pos - 1;
            if(last_pos>=0)
            {
                if(abs(blk_buf[2][curr_pos]-blk_buf[2][last_pos]) > c_neighbor_diff_thr)
                    sbs_neighbor_diff_cnt++;
                if(abs(blk_buf[3][curr_pos]-blk_buf[3][last_pos]) > c_neighbor_diff_thr)
                    sbs_neighbor_diff_cnt++;
            }

            // Compare Up-left and Down-Left (TAB left part)
            for(i=-2;i<3;i++)
            {
                temp_a[i+2]= (w+i<0 || w+i>(NAVI_LINE_WIDTH/2)-1)? blk_buf[0][curr_pos]:blk_buf[0][curr_pos+i];
                temp_b[i+2]= (w+i+gmv_t0_in<0) ?       blk_buf[2][curr_pos-w] :
                             (w+i+gmv_t0_in>(NAVI_LINE_WIDTH/2)-1) ? blk_buf[2][curr_pos-w+(NAVI_LINE_WIDTH/2)-1] : blk_buf[2][curr_pos+i+gmv_t0_in];
            }
            is_similar = decide_similar_fw(temp_a, temp_b, similar_thr, gmv2_t0_in-gmv_t0_in, c_decision_type, c_decision_type_ex);

            if(blk_var[0][curr_pos]>c_var_thr || blk_var[2][curr_pos]>c_var_thr)
            {
                tab_vld_blk++;
                if(is_similar)	{like_tab++; blk_diff[2][curr_pos] = 0;}
                direct_cnt_t0[similar_direct_fw(temp_a, temp_b)+1]++;
            }
            else
            {
                blk_diff[2][curr_pos] = 128; // invalid
                if(is_similar)	
                    invalid_blk_like_tab++;
            }

            last_pos = curr_pos - (NAVI_LINE_WIDTH/2);
            if(last_pos>=0)
            {
                if(abs(blk_buf[0][curr_pos]-blk_buf[0][last_pos]) > c_neighbor_diff_thr)
                    tab_neighbor_diff_cnt++;
                if(abs(blk_buf[2][curr_pos]-blk_buf[2][last_pos]) > c_neighbor_diff_thr)
                    tab_neighbor_diff_cnt++;
            }

            // Compare Up-right and Down-right (TAB right part)
            for(i=-2;i<3;i++)
            {
                temp_a[i+2]= (w+i<0 || w+i>(NAVI_LINE_WIDTH/2)-1)? blk_buf[1][curr_pos]:blk_buf[1][curr_pos+i];
                temp_b[i+2]= (w+i+gmv_t1_in<0) ?       blk_buf[3][curr_pos-w] :
                             (w+i+gmv_t1_in>(NAVI_LINE_WIDTH/2)-1) ? blk_buf[3][curr_pos-w+(NAVI_LINE_WIDTH/2)-1] : blk_buf[3][curr_pos+i+gmv_t1_in];
            }
            is_similar = decide_similar_fw(temp_a, temp_b, similar_thr, gmv2_t1_in-gmv_t1_in, c_decision_type, c_decision_type_ex);

            if(blk_var[1][curr_pos]>c_var_thr || blk_var[3][curr_pos]>c_var_thr)
            {
                tab_vld_blk++;
                if(is_similar)	{like_tab++; blk_diff[3][curr_pos] = 0;}
                direct_cnt_t1[similar_direct_fw(temp_a, temp_b)+1]++;
            }
            else
            {
                blk_diff[3][curr_pos] = 128; // invalid
                if(is_similar)	
                    invalid_blk_like_tab++;
            }

            last_pos = curr_pos - (NAVI_LINE_WIDTH/2);
            if(last_pos>=0)
            {
                if(abs(blk_buf[1][curr_pos]-blk_buf[1][last_pos]) > c_neighbor_diff_thr)
                    tab_neighbor_diff_cnt++;
                if(abs(blk_buf[3][curr_pos]-blk_buf[3][last_pos]) > c_neighbor_diff_thr)
                    tab_neighbor_diff_cnt++;
            }
        }
    }
    //global disparity 
    s0_cnt_all = direct_cnt_s0[0] + direct_cnt_s0[1] + direct_cnt_s0[2];
    s1_cnt_all = direct_cnt_s1[0] + direct_cnt_s1[1] + direct_cnt_s1[2];
    t0_cnt_all = direct_cnt_t0[0] + direct_cnt_t0[1] + direct_cnt_t0[2];
    t1_cnt_all = direct_cnt_t1[0] + direct_cnt_t1[1] + direct_cnt_t1[2];

    gmv_s0_tmp = direct_cnt_s0[2] >= (s0_cnt_all >> 1) + direct_cnt_s0[0] && s0_cnt_all > c_gmv_blk_th ? gmv_s0_in + 1 :
        direct_cnt_s0[0] >= (s0_cnt_all >> 1) + direct_cnt_s0[2] && s0_cnt_all > c_gmv_blk_th ? gmv_s0_in - 1 : gmv_s0_in;
    gmv_s1_tmp = direct_cnt_s1[2] >= (s1_cnt_all >> 1) + direct_cnt_s1[0] && s1_cnt_all > c_gmv_blk_th ? gmv_s1_in + 1 :
        direct_cnt_s1[0] >= (s1_cnt_all >> 1) + direct_cnt_s1[2] && s1_cnt_all > c_gmv_blk_th ? gmv_s1_in - 1 : gmv_s1_in;
    gmv_t0_tmp = direct_cnt_t0[2] >= (t0_cnt_all >> 1) + direct_cnt_t0[0] && t0_cnt_all > c_gmv_blk_th ? gmv_t0_in + 1 :
        direct_cnt_t0[0] >= (t0_cnt_all >> 1) + direct_cnt_t0[2] && t0_cnt_all > c_gmv_blk_th ? gmv_t0_in - 1 : gmv_t0_in;
    gmv_t1_tmp = direct_cnt_t1[2] >= (t1_cnt_all >> 1) + direct_cnt_t1[0] && t1_cnt_all > c_gmv_blk_th ? gmv_t1_in + 1 :
        direct_cnt_t1[0] >= (t1_cnt_all >> 1) + direct_cnt_t1[2] && t1_cnt_all > c_gmv_blk_th ? gmv_t1_in - 1 : gmv_t1_in;

    gmv2_s0_tmp = gmv_s0_tmp!=gmv_s0 ? gmv_s0 :
        direct_cnt_s0[2]>direct_cnt_s0[0] && s0_cnt_all>c_gmv_blk_th ? gmv_s0 + 1 :
        direct_cnt_s0[0]>direct_cnt_s0[2] && s0_cnt_all>c_gmv_blk_th ? gmv_s0 - 1 : gmv_s0;
    gmv2_s1_tmp = gmv_s1_tmp!=gmv_s1 ? gmv_s1 :
        direct_cnt_s1[2]>direct_cnt_s1[0] && s1_cnt_all>c_gmv_blk_th ? gmv_s1 + 1 :
        direct_cnt_s1[0]>direct_cnt_s1[2] && s1_cnt_all>c_gmv_blk_th ? gmv_s1 - 1 : gmv_s1;
    gmv2_t0_tmp = gmv_t0_tmp!=gmv_t0 ? gmv_t0 :
        direct_cnt_t0[2]>direct_cnt_t0[0] && t0_cnt_all>c_gmv_blk_th ? gmv_t0 + 1 :
        direct_cnt_t0[0]>direct_cnt_t0[2] && t0_cnt_all>c_gmv_blk_th ? gmv_t0 - 1 : gmv_t0;
    gmv2_t1_tmp = gmv_t1_tmp!=gmv_t1 ? gmv_t1 :
        direct_cnt_t1[2]>direct_cnt_t1[0] && t1_cnt_all>c_gmv_blk_th ? gmv_t1 + 1 :
        direct_cnt_t1[0]>direct_cnt_t1[2] && t1_cnt_all>c_gmv_blk_th ? gmv_t1 - 1 : gmv_t1;

    //fallback
    if(gmv_s0!=0 && direct_cnt_s0[0] > direct_cnt_s0[1]*5/4 && direct_cnt_s0[2] > direct_cnt_s0[1]*5/4 &&
            ((gmv_s0>0&&direct_cnt_s0[0]>direct_cnt_s0[2])||(gmv_s0<0&&direct_cnt_s0[0]<direct_cnt_s0[2]))) {
        gmv_s0_tmp = 0;
        gmv2_s0_tmp = 0;
    }
    if(gmv_s1!=0 && direct_cnt_s1[0] > direct_cnt_s1[1]*5/4 && direct_cnt_s1[2] > direct_cnt_s1[1]*5/4 &&
            ((gmv_s1>0&&direct_cnt_s1[0]>direct_cnt_s1[2])||(gmv_s1<0&&direct_cnt_s1[0]<direct_cnt_s1[2])))	{
        gmv_s1_tmp = 0;
        gmv2_s1_tmp = 0;
    }
    if(gmv_t0!=0 && direct_cnt_t0[0] > direct_cnt_t0[1]*5/4 && direct_cnt_t0[2] > direct_cnt_t0[1]*5/4 &&
            ((gmv_t0>0&&direct_cnt_t0[0]>direct_cnt_t0[2])||(gmv_t0<0&&direct_cnt_t0[0]<direct_cnt_t0[2]))) {
        gmv_t0_tmp = 0;
        gmv2_t0_tmp = 0;
    }
    if(gmv_t1!=0 && direct_cnt_t1[0] > direct_cnt_t1[1]*5/4 && direct_cnt_t1[2] > direct_cnt_t1[1]*5/4 &&
            ((gmv_t1>0&&direct_cnt_t1[0]>direct_cnt_t1[2])||(gmv_t1<0&&direct_cnt_t1[0]<direct_cnt_t1[2]))) {
        gmv_t1_tmp = 0;
        gmv2_t1_tmp = 0;
    }

    //fallback
    if((gmv_s0_tmp==gmv_s0&&gmv_s1_tmp==gmv_s1&&abs(gmv_s0-gmv_s1)>1&&gmv_s0!=0&&gmv_s1!=0)||(gmv_s0_tmp*gmv_s1_tmp<0)||abs(gmv_s0_tmp-gmv_s1_tmp)>2) {
        gmv_s0_tmp = 0;
        gmv_s1_tmp = 0;
        gmv2_s0_tmp = 0;
        gmv2_s1_tmp = 0;
    }
    if((gmv_t0_tmp==gmv_t0&&gmv_t1_tmp==gmv_t1&&abs(gmv_t0-gmv_t1)>1&&gmv_t0!=0&&gmv_t1!=0)||(gmv_t0_tmp*gmv_t1_tmp<0)||abs(gmv_t0_tmp-gmv_t1_tmp)>2) {
        gmv_t0_tmp = 0;
        gmv_t1_tmp = 0;
        gmv2_t0_tmp = 0;
        gmv2_t1_tmp = 0;
    }

    if(gmv_s0_tmp!=0&&gmv_s0_tmp==gmv_s0&&like_sbs<((sbs_vld_blk * c_d_th) >> 8))
    {
        gmv2_s0_tmp = 0;
        gmv_s0_tmp = 0;
    }
    if(gmv_s1_tmp!=0&&gmv_s1_tmp==gmv_s1&&like_sbs<((sbs_vld_blk * c_d_th) >> 8))
    {
        gmv2_s1_tmp = 0;
        gmv_s1_tmp = 0;
    }
    if(gmv_t0_tmp!=0&&gmv_t0_tmp==gmv_t0&&like_tab<((tab_vld_blk * c_d_th) >> 8))
    {
        gmv2_t0_tmp = 0;
        gmv_t0_tmp = 0;
    }
    if(gmv_t1_tmp!=0&&gmv_t1_tmp==gmv_t1&&like_tab<((tab_vld_blk * c_d_th) >> 8))
    {
        gmv_t1_tmp = 0;
        gmv2_t1_tmp = 0;
    }

    if(like_sbs>like_tab&&like_sbs>(sbs_vld_blk * c_d_th) >> 8) {
        gmv_s0 = gmv_s0_tmp;
        gmv_s1 = gmv_s1_tmp;
        gmv2_s0 = gmv2_s0_tmp;
        gmv2_s1 = gmv2_s1_tmp;
        gmv_t0 = 0;
        gmv_t1 = 0;
        gmv2_t0 = 0;
        gmv2_t1 = 0;
    }
    else if(like_sbs<like_tab&&like_tab>(tab_vld_blk * c_d_th) >> 8) {
        gmv_s0 = 0;
        gmv_s1 = 0;
        gmv2_s0 = 0;
        gmv2_s1 = 0;
        gmv_t0 = gmv_t0_tmp;
        gmv_t1 = gmv_t1_tmp;
        gmv2_t0 = gmv2_t0_tmp;
        gmv2_t1 = gmv2_t1_tmp;
    }
    else {
        gmv_s0 = gmv_s0_tmp;
        gmv_s1 = gmv_s1_tmp;
        gmv2_s0 = gmv2_s0_tmp;
        gmv2_s1 = gmv2_s1_tmp;
        gmv_t0 = gmv_t0_tmp;
        gmv_t1 = gmv_t1_tmp;
        gmv2_t0 = gmv2_t0_tmp;
        gmv2_t1 = gmv2_t1_tmp;
    }


    gmv_s0 = clipping(gmv_s0, c_gmv_sbs_max, -1*c_gmv_sbs_max);
    gmv_s1 = clipping(gmv_s1, c_gmv_sbs_max, -1*c_gmv_sbs_max);
    gmv_t0 = clipping(gmv_t0, c_gmv_tab_max, -1*c_gmv_tab_max);
    gmv_t1 = clipping(gmv_t1, c_gmv_tab_max, -1*c_gmv_tab_max);
    gmv2_s0 = clipping(gmv2_s0, c_gmv_sbs_max, -1*c_gmv_sbs_max);
    gmv2_s1 = clipping(gmv2_s1, c_gmv_sbs_max, -1*c_gmv_sbs_max);
    gmv2_t0 = clipping(gmv2_t0, c_gmv_tab_max, -1*c_gmv_tab_max);
    gmv2_t1 = clipping(gmv2_t1, c_gmv_tab_max, -1*c_gmv_tab_max);

    // block APL
    blk_apl=0;
    valid_blk_cnt=0;
    for(h=0;h<(NAVI_LINE_HEIGHT/2);h++)
        for(w=0;w<(NAVI_LINE_WIDTH/2);w++)
            for(i=0;i<4;i++)
            {
                if(blk_var[i][h*(NAVI_LINE_WIDTH/2) + w]>c_var_thr) //valid
                {
                    blk_apl += blk_buf[i][h*(NAVI_LINE_WIDTH/2) + w];
                    valid_blk_cnt++;
                }
            }

    if(valid_blk_cnt!=0) blk_apl /= valid_blk_cnt;


    //---------------------------------------------------------------------------------------
    // make decision 1
    //if     (like_sbs <  sbs_vld_blk/2 && like_tab <  tab_vld_blk/2 ) strcpy(det_type,"2D");
    //else if(like_sbs >= sbs_vld_blk/2 && like_tab <  tab_vld_blk/2 ) strcpy(det_type,"SBS");
    //else if(like_sbs <  sbs_vld_blk/2 && like_tab >= tab_vld_blk/2 ) strcpy(det_type,"TAB"); 
    //else                                                             strcpy(det_type,"UNKNOWN"); 

    //---------------------------------------------------------------------------------------
    // make decision 2
    wt_sbs_vld_blk = (sbs_vld_blk * c_d_th) >> 8;
    wt_tab_vld_blk = (tab_vld_blk * c_d_th) >> 8;

    //clipping to 6 bits
    sbs_neighbor_diff_cnt = (sbs_neighbor_diff_cnt>=0x3F)? 0x3F: sbs_neighbor_diff_cnt; //6bits
    tab_neighbor_diff_cnt = (tab_neighbor_diff_cnt>=0x3F)? 0x3F: tab_neighbor_diff_cnt; //6bits

    min_vld_blk = (sbs_vld_blk<=tab_vld_blk)? sbs_vld_blk : tab_vld_blk;

    if(like_sbs <  wt_sbs_vld_blk && like_tab <  wt_tab_vld_blk ) 
    {
        SwNaviPlusDetected = TD_NAVI_DETECT_2D;
	 c_d_th=c_d_th_in;
	 //c_similar_thr=5;
     }
    else if(like_sbs >= wt_sbs_vld_blk && like_tab <  wt_tab_vld_blk ) 
     {
          SwNaviPlusDetected = TD_NAVI_DETECT_SBS;
         c_d_th=c_d_th_out;
	    //c_similar_thr=10;
      }
    else if(like_sbs <  wt_sbs_vld_blk && like_tab >= wt_tab_vld_blk ) 
    {
        SwNaviPlusDetected = TD_NAVI_DETECT_TAB;
        c_d_th=c_d_th_out;
	 //c_similar_thr=10;
     }
    else //UNKNOWN
        SwNaviPlusDetected = TD_NAVI_DETECT_UNKNOWN;

    //-------------------------------------------
    // status assignemnt
    //-------------------------------------------
    sta_sbs_cnt = like_sbs;
    sta_tab_cnt = like_tab;
    sta_sbs_vld_cnt = sbs_vld_blk;
    sta_tab_vld_cnt = tab_vld_blk;
    sta_blk_apl = blk_apl;
    sta_total_blk = (NAVI_LINE_WIDTH/2)*(NAVI_LINE_HEIGHT/2)*2;
    sta_invld_sbs = invalid_blk_like_sbs;
    sta_invld_tab = invalid_blk_like_tab;
    sta_sbs_neighbor_cnt = sbs_neighbor_diff_cnt; //6bits
    sta_tab_neighbor_cnt = tab_neighbor_diff_cnt; //6bits

    sta_gmv_s0 = gmv_s0;
    sta_gmv_s1 = gmv_s1;
    sta_gmv_t0 = gmv_t0;
    sta_gmv_t1 = gmv_t1;
    sta_gmv2_s0 = gmv2_s0;
    sta_gmv2_s1 = gmv2_s1;
    sta_gmv2_t0 = gmv2_t0;
    sta_gmv2_t1 = gmv2_t1;
    //-------------------------------------------

#ifdef TDNAVITODO		            
    valid_sbs_ratio = (sbs_vld_blk==0)? 100 : like_sbs*100 / sbs_vld_blk;
    valid_tab_ratio = (tab_vld_blk==0)? 100 : like_tab*100 / tab_vld_blk;

    if     (strcmp(det_type,"SBS")==0) confidence = valid_sbs_ratio - valid_tab_ratio;
    else if(strcmp(det_type,"TAB")==0) confidence = valid_tab_ratio - valid_sbs_ratio;
    else if(strcmp(det_type,"2D" )==0) confidence = 100 - (valid_sbs_ratio + valid_tab_ratio)/2;
    else                               confidence = (valid_sbs_ratio + valid_tab_ratio)/2;
#endif

    //final decision
    valid_sbs_ratio = (sta_sbs_vld_cnt==0) ? 0 : sta_sbs_cnt*100 / sta_sbs_vld_cnt;
    valid_tab_ratio = (sta_tab_vld_cnt==0) ? 0 : sta_tab_cnt*100 / sta_tab_vld_cnt;

    hw_valid_sbs_ratio = (sta_hw_sbs_vld_cnt==0) ? 0 : sta_hw_sbs_cnt*100 / sta_hw_sbs_vld_cnt;
    hw_valid_tab_ratio = (sta_hw_tab_vld_cnt==0) ? 0 : sta_hw_tab_cnt*100 / sta_hw_tab_vld_cnt;

    if((SwNaviPlusDetected==TD_NAVI_DETECT_UNKNOWN)||
         ((valid_sbs_ratio-hw_valid_sbs_ratio)<c_fw_ratio_gain_th&&
          (valid_tab_ratio-hw_valid_tab_ratio)<c_fw_ratio_gain_th&&
          (hw_valid_sbs_ratio*c_fw_hw_ratio_weight+valid_sbs_ratio*(4-c_fw_hw_ratio_weight))/4<c_d_th*100/256&&
          (hw_valid_tab_ratio*c_fw_hw_ratio_weight+valid_tab_ratio*(4-c_fw_hw_ratio_weight))/4<c_d_th*100/256))
        final_sta_det_type = TD_NAVI_DETECT_2D;  //status accodring to HW

    else
        final_sta_det_type = SwNaviPlusDetected; //status accodring to FW    
}

static inline void vDrvTDNaviPlusSampling(void)
{

    UINT32 u4Width = u2DrvDIGetVdpWidth(SV_VP_MAIN);

    UINT32 blk_size_x = u4Width/NAVI_LINE_WIDTH;   

    UINT32 u4PosBlk_h;
    UINT32 u4Mod;

    UINT32 u4PosRaw_h;

    UINT32 u4BlkIdx;
    UINT32 u4RawIdx;
    
    INT32 tmp;
    UINT32 BlkTmp,VarTmp;
    
    for(h=0, u4PosRaw_h = 0, u4PosBlk_h = 0
        ;h<(NAVI_LINE_HEIGHT/2);h++)  
    {    
        // top-left
        u4Mod = 0;        
        u4RawIdx = u4PosRaw_h;
        u4BlkIdx = u4PosBlk_h;
        
        for(w=0,BlkTmp=0,VarTmp=0;w< u4Width/2 ;w++)
        {
            tmp = u1RawDataBuf[u4RawIdx]-u1RawDataBuf[u4RawIdx+1];        
            if(abs(tmp)>c_pxl_diff_thr)
                VarTmp++;
            BlkTmp += u1RawDataBuf[u4RawIdx];
            
            if(u4Mod < blk_size_x -1)
            {
                u4Mod++;
            }
            else
            {            
                blk_buf[0][u4BlkIdx] = BlkTmp;
                blk_var[0][u4BlkIdx] = VarTmp;
                
                u4Mod=0;                
                BlkTmp=0;
                VarTmp=0;
                
                u4BlkIdx++;
            }
            u4RawIdx ++;
            
        }

        // top-right

        u4Mod = 0;              
        u4RawIdx = u4PosRaw_h + (u4Width/2);   
        u4BlkIdx = u4PosBlk_h;        
        
        for(w=(u4Width/2),BlkTmp=0,VarTmp=0;w< u4Width;w++)
        {
            tmp = u1RawDataBuf[u4RawIdx]-u1RawDataBuf[u4RawIdx+1];        
            if(abs(tmp)>c_pxl_diff_thr)
                VarTmp++;
            BlkTmp += u1RawDataBuf[u4RawIdx];

            if(u4Mod < blk_size_x -1)
            {
                u4Mod++;
            }
            else
            {            
                blk_buf[1][u4BlkIdx] = BlkTmp;
                blk_var[1][u4BlkIdx] = VarTmp;
                
                u4Mod=0;                
                BlkTmp=0;
                VarTmp=0;                
                
                u4BlkIdx++;
            }
            u4RawIdx ++;            
        }
        
        u4PosRaw_h += u4Width;
        u4PosBlk_h += NAVI_LINE_WIDTH/2;                     
    }

    for(h=(NAVI_LINE_HEIGHT/2),u4PosRaw_h = h*u4Width , u4PosBlk_h = 0,BlkTmp=0,VarTmp=0
        ;h<NAVI_LINE_HEIGHT;h++)
    {    
        // Bottom-left
        u4Mod = 0;        
        u4BlkIdx = u4PosBlk_h;        
        u4RawIdx = u4PosRaw_h;
            
        for(w=0;w< u4Width/2 ;w++)
        { 
            tmp = u1RawDataBuf[u4RawIdx]-u1RawDataBuf[u4RawIdx+1];        
            if(abs(tmp)>c_pxl_diff_thr)
                VarTmp++;
            BlkTmp += u1RawDataBuf[u4RawIdx];

            if(u4Mod < blk_size_x -1)
            {
                u4Mod++;
            }
            else
            {            
                blk_buf[2][u4BlkIdx] = BlkTmp;
                blk_var[2][u4BlkIdx] = VarTmp;
                
                u4Mod=0;                
                BlkTmp=0;
                VarTmp=0;                
                
                u4BlkIdx++;
            }
            u4RawIdx ++;
            
        }


        // Bottom-right

        u4Mod = 0;              
        u4BlkIdx = u4PosBlk_h;        
        u4RawIdx = u4PosRaw_h + (u4Width/2);   
        
        for(w=(u4Width/2);w< u4Width;w++)
        {
            tmp = u1RawDataBuf[u4RawIdx]-u1RawDataBuf[u4RawIdx+1];        
            if(abs(tmp)>c_pxl_diff_thr)
                VarTmp++;
            BlkTmp += u1RawDataBuf[u4RawIdx];

            if(u4Mod < blk_size_x -1)
            {
                u4Mod++;
            }
            else
            {            
                blk_buf[3][u4BlkIdx] = BlkTmp;
                blk_var[3][u4BlkIdx] = VarTmp;
                
                u4Mod=0;                
                BlkTmp=0;
                VarTmp=0;                
                
                u4BlkIdx++;
            }
            u4RawIdx ++;            
        }
        
        u4PosRaw_h += u4Width;
        u4PosBlk_h += NAVI_LINE_WIDTH/2;                    
    }

    for(h=0;h<(NAVI_LINE_HEIGHT/2);h++)
    {
        for(w=0;w<(NAVI_LINE_WIDTH/2);w++)
        {
            blk_buf[0][h*(NAVI_LINE_WIDTH/2)+w] /= blk_size_x;
            blk_buf[1][h*(NAVI_LINE_WIDTH/2)+w] /= blk_size_x;
            blk_buf[2][h*(NAVI_LINE_WIDTH/2)+w] /= blk_size_x;
            blk_buf[3][h*(NAVI_LINE_WIDTH/2)+w] /= blk_size_x;
        }
    }
}

void vDrvTDNaviPlusProc(void)
{
    UINT32 i;
    UINT32 u4Width = u2DrvDIGetVdpWidth(SV_VP_MAIN);
    UINT32 u4Height = u2DrvDIGetVdpHeight(SV_VP_MAIN);

    UINT32 u4LinePos = (u4Height/ NAVI_LINE_HEIGHT)/2;

    #ifdef SW_NAVI_DEBUG
    HAL_TIME_T TimeStart, TimeNow, TimeDelta;
    #endif

    if(bDrvVideoIsSrcInterlace(SV_VP_MAIN))
    {
        u4Height=u4Height>>1;
    }
            
    if((bDrvVideoSignalStatus(SV_VP_MAIN)!=SV_VDO_STABLE)
       ||(u4DrvTDTVGetTag3D(SV_VP_MAIN)!=TDTV_TAG3D_2D)
       ||(!fgDrvDIIsMIBDramAvaliable(SV_VP_MAIN))
       ||(eDrvTDTVTDNavimModeQuery()==E_TDTV_UI_NAVI_MANAUL)
       ||(fgDrvTDTVIsFix2DDisplay(SV_VP_MAIN)==SV_TRUE)
       ||((u4Width < 480)||(u4Height < 160)||(u4Width > 1920))
        )
    {
        SwNaviPlusDetected = TD_NAVI_DETECT_2D;
        final_sta_det_type = TD_NAVI_DETECT_2D;
        
        eNPState=NP_WAIT;
    }
    else if(NP_WAIT==eNPState)
    {
        //CLEAR BUFFER
        vDrvTDNaviPlusReset();

        eNPState = NP_COPY;
    }
    else if(NP_COPY==eNPState)
    {
        #ifdef SW_NAVI_DEBUG
        HAL_GetTime(&TimeStart);                                        
        #endif    
        //DUMP DATA FROM DRAM
        for(i=0;i<NAVI_LINE_HEIGHT/2;++i)
        {    
            vDrvDIFlushDramForTDNAVI(SV_VP_MAIN, 0, u4LinePos, u4Width, 1);        
            vDrvDIDumpDramForTDNAVI(SV_VP_MAIN , 0, u4LinePos, u4Width, 1
                ,(UINT32 *)(u1RawDataBuf+u4Width*i));
            
            vDrvDIFlushDramForTDNAVI(SV_VP_MAIN, 0, u4LinePos + u4Height/2, u4Width, 1);        
            vDrvDIDumpDramForTDNAVI(SV_VP_MAIN , 0, u4LinePos + u4Height/2, u4Width, 1
                ,(UINT32 *)(u1RawDataBuf+u4Width*(i+NAVI_LINE_HEIGHT/2)));

            u4LinePos+=(u4Height/ NAVI_LINE_HEIGHT);
        }

        #ifdef SW_NAVI_DEBUG
        HAL_GetTime(&TimeNow);
        HAL_GetDeltaTime(&TimeDelta, &TimeStart, &TimeNow);
        LOG(3, "[NAVI Plus]Dump time  %d.%06d sec.\n", TimeDelta.u4Seconds, TimeDelta.u4Micros);
        #endif 

        eNPState=NP_SUBSAMPLE;
    }
    else if(NP_SUBSAMPLE==eNPState)
    {
        //BLOCK SAMPLING
        #ifdef SW_NAVI_DEBUG
        HAL_GetTime(&TimeStart);                                        
        #endif  
        
        vDrvTDNaviPlusSampling();
        
        #ifdef SW_NAVI_DEBUG
        HAL_GetTime(&TimeNow);
        HAL_GetDeltaTime(&TimeDelta, &TimeStart, &TimeNow);
        LOG(3, "[NAVI Plus]Sub Sampling time  %d.%06d sec.\n", TimeDelta.u4Seconds, TimeDelta.u4Micros);
        #endif

        eNPState=NP_CHK;       
    }    
    else if(NP_CHK==eNPState)
    {
        //START DETECTION
        #ifdef SW_NAVI_DEBUG
        HAL_GetTime(&TimeStart);                                        
        #endif  
        
        vDrvTDNaviPlusDet();    

        //CLEAR BUFFER
        vDrvTDNaviPlusReset();

        #ifdef SW_NAVI_DEBUG
        HAL_GetTime(&TimeNow);
        HAL_GetDeltaTime(&TimeDelta, &TimeStart, &TimeNow);
        LOG(3, "[NAVI Plus]Detection time  %d.%06d sec.\n", TimeDelta.u4Seconds, TimeDelta.u4Micros);
        #endif

        eNPState=NP_COPY;               
    }

    LOG(3, "[NAVI Plus]SwNaviPlusDetected = %d\n", SwNaviPlusDetected);
    
}

#endif
