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
 *---------------------------------------------------------------------------*/
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "tzst.h"
#include "x_ckgen.h"
#include "x_bim.h"
#include "x_chipreg.h"
#include "x_rand.h"

unsigned long sec_fbm_size = 0;
unsigned long sec_mjc_size = 0;
unsigned long sec_drm_addr = 0;
unsigned long sec_drm_size = 0;


extern UINT32 PA_LOAD_BASE;
extern BOOL TZ_TVP_Protection(UINT32 u4ProtFlag);

BOOL TZ_FBM_Initialization(void *prKernParam  , UINT32 u4Size)
{
    static int isInit = 0;

    FBM_POOL_T* _arPool = (FBM_POOL_T*)prKernParam;
    FBM_POOL_TYPE_T aeSecure_pool[] =
    {
        FBM_POOL_TYPE_DRM,
#ifndef CC_SVP_SUPPORT
        FBM_POOL_TYPE_DMX,
		FBM_POOL_TYPE_DMX2, 	   
		FBM_POOL_TYPE_DMX3,
		FBM_POOL_TYPE_DMX4,
#endif
        FBM_POOL_TYPE_MPEG,
        FBM_POOL_TYPE_SCPOS_MAIN,
        FBM_POOL_TYPE_SCPOS_PIP,
        FBM_POOL_TYPE_MDDI_1,
        FBM_POOL_TYPE_MDDI_PIP,
        FBM_POOL_TYPE_NR_1,
        FBM_POOL_TYPE_B2R,
        FBM_POOL_TYPE_OD,
        FBM_POOL_TYPE_MPEG2,
        FBM_POOL_TYPE_MPEG3,
        FBM_POOL_TYPE_MPEG4,
        FBM_POOL_TYPE_TVE,
        FBM_POOL_TYPE_MJC_1,
		#if defined(CC_MT5890)
        FBM_POOL_TYPE_MPEG_C,
        FBM_POOL_TYPE_MPEG2_C,
        FBM_POOL_TYPE_MPEG3_C,
        FBM_POOL_TYPE_MPEG4_C,
        FBM_POOL_TYPE_W2D_R2R,
        FBM_POOL_TYPE_W2D_R2R_Y,
		//FBM_POOL_TYPE_VSS,
		#endif
    };
    int i=0;
    unsigned int minaddr = PA_LOAD_BASE;
	unsigned int maxaddr = PA_LOAD_BASE + TZ_DRAM_SC_SIZE;

    if(isInit == 1)
        return TRUE;

    for(i=0;i<sizeof(aeSecure_pool)/sizeof(FBM_POOL_TYPE_T);i++)
    {
        if(minaddr > _arPool[(UINT8)aeSecure_pool[i]].u4Addr && _arPool[(UINT8)aeSecure_pool[i]].u4Size!=0 && _arPool[(UINT8)aeSecure_pool[i]].u4Addr < PA_LOAD_BASE)
            minaddr = _arPool[(UINT8)aeSecure_pool[i]].u4Addr;
        if((maxaddr < _arPool[(UINT8)aeSecure_pool[i]].u4Addr + _arPool[(UINT8)aeSecure_pool[i]].u4Size) && (_arPool[(UINT8)aeSecure_pool[i]].u4Size!=0) && _arPool[(UINT8)aeSecure_pool[i]].u4Addr > (PA_LOAD_BASE + TZ_DRAM_SC_SIZE))
            maxaddr = _arPool[(UINT8)aeSecure_pool[i]].u4Addr + _arPool[(UINT8)aeSecure_pool[i]].u4Size;
    }
    sec_fbm_size = PA_LOAD_BASE - minaddr;
	sec_mjc_size = maxaddr - PA_LOAD_BASE - TZ_DRAM_SC_SIZE;

    sec_drm_addr = _arPool[FBM_POOL_TYPE_SECURE_FEEDER].u4Addr;
    sec_drm_size = _arPool[FBM_POOL_TYPE_SECURE_FEEDER].u4Size;

    isInit = 1;

    return TRUE;
}


