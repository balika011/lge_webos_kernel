/**********************************************************************/
/***************           MTK CONFIDENTIAL            ****************/
/***************                                       ****************/
/***************   Description : MT8118 MTKPrintf      ****************/
/***************                 Procedure             ****************/
/***************                                       ****************/
/***************       Company : MediaTek Inc.         ****************/
/***************    Programmer : Ted Hu                ****************/
/**********************************************************************/
//#define _IRQ_FIQ_PROC_
#include "vdec_verify_irq_fiq_proc.h"
#include "vdec_verify_mpv_prov.h"

extern UINT32 gu4MCCycle[2];
extern UINT32 gu4VLDCycle[2];
extern UINT32 u4VDecReadMC(UINT32 u4VDecID, UINT32 u4Addr);
extern UINT32 u4VDecReadVLDTOP(UINT32 u4VDecID, UINT32 u4Addr);
// *********************************************************************
// Function    : void vVldIrq0(void)
// Description : Clear picture info in frame buffer
// Parameter   : None
// Return      : None
// *********************************************************************
void vVldIrq0(void)
{
  #if (PROFILE_TIME_TEST && CONFIG_DRV_VERIFY_SUPPORT)
  HAL_GetTime(&rTimeEndDec[0]);
  #endif
//  x_sema_unlock(_ahVDecCompleteSema[0]);
  gu4MCCycle[0] = u4VDecReadMC(0,(478 << 2));
  gu4VLDCycle[0] = u4VDecReadVLDTOP(0,(40 << 2));
  _fgVDecComplete[0] = TRUE;
  VERIFY(x_sema_unlock(hVdecDecDone[0]) == OSR_OK);
}

void vLaeIrq0(void)
{
//  #if (PROFILE_TIME_TEST && CONFIG_DRV_VERIFY_SUPPORT)
//  HAL_GetTime(&rTimeEndDec[0]);
//  #endif
    _fgLaeComplete[0] = TRUE;
}

// *********************************************************************
// Function    : void vVldIrq1(void)
// Description : Clear picture info in frame buffer
// Parameter   : None
// Return      : None
// *********************************************************************
void vVldIrq1(void)
{
  #if (PROFILE_TIME_TEST && CONFIG_DRV_VERIFY_SUPPORT)
  HAL_GetTime(&rTimeEndDec[1]);
  #endif
  gu4MCCycle[1] = u4VDecReadMC(1,(478 << 2));
  gu4VLDCycle[1] = u4VDecReadVLDTOP(1,(40 << 2));
  _fgVDecComplete[1] = TRUE;
  VERIFY(x_sema_unlock(hVdecDecDone[1]) == OSR_OK);
}


// *********************************************************************
// Function    : void vVDec0IrqProc(UINT16 u2Vector)
// Description : Irq Service routine.
// Parameter   : None
// Return      : None
// *********************************************************************
void vVDec0IrqProc(UINT16 u2Vector)
{
//#ifndef CONFIG_TV_DRV_VFY
#ifndef IRQ_DISABLE    
  BIM_ClearIrq(VECTOR_VDFUL);
  vVldIrq0();
#endif
//#endif // CONFIG_TV_DRV_VFY
}

void vVDecLae0IrqProc(UINT16 u2Vector)
{
    #ifndef IRQ_DISABLE    
    BIM_ClearIrq(VECTOR_LAE0);
    vLaeIrq0();
    #endif
}

// *********************************************************************
// Function    : void vVDec1IrqProc(void)
// Description : Irq Service routine.
// Parameter   : None
// Return      : None
// *********************************************************************
void vVDec1IrqProc(UINT16 u2Vector)
{
//#ifndef CONFIG_TV_DRV_VFY
#ifndef IRQ_DISABLE    
  BIM_ClearIrq(VECTOR_VDLIT);
  vVldIrq1();
#endif
//#endif // CONFIG_TV_DRV_VFY
}

// *********************************************************************
// Function    : void vVldIrq1(void)
// Description : Clear picture info in frame buffer
// Parameter   : None
// Return      : None
// *********************************************************************
void vVldIrqIOMMU(void)
{
  UTIL_Printf("[IOMMU]Receive IOMMU irq \n");
}

// *********************************************************************
// Function    : void vVDecIOMMUIrqProc(void)
// Description : Irq Service routine.
// Parameter   : None
// Return      : None
// *********************************************************************
void vVDecIOMMUIrqProc(UINT16 u2Vector)
{
//#ifndef CONFIG_TV_DRV_VFY
#ifndef IRQ_DISABLE    
  BIM_ClearIrq(VECTOR_MMU_VDEC);
  vVldIrqIOMMU();
#endif
//#endif // CONFIG_TV_DRV_VFY

}