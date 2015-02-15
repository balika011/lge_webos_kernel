#include "vdec_verify_fgt_table.h"
#include "vdec_verify_mpv_prov.h"
#include "vdec_verify_fgt.h"
#include "vdec_verify_file_common.h"

// *********************************************************************
// Function : void vAllocFGTTable(UINT32 u4InstID)
// Description : Allocate FGT Table
// Parameter : None
// Return    : void
// *********************************************************************
void vAllocFGTTable(UINT32 u4InstID)
{
#ifndef INTERGRATION_WITH_DEMUX
  // setup SEED table
  _tFBufFileInfo[u4InstID].fgGetFileInfo = TRUE;  
  _tFBufFileInfo[u4InstID].pucTargetAddr = (UCHAR *)_pucFGSeedbase[u4InstID];
  _tFBufFileInfo[u4InstID].u4TargetSz = FGT_SEED_SZ;  
  _tFBufFileInfo[u4InstID].u4FileLength = 0; 
  fgOpenPCFile(u4InstID, "Z:\\ChkFolder\\SEED_table.bin2","r+b", &_tFBufFileInfo[u4InstID]); 

  // setup database
  _tFBufFileInfo[u4InstID].fgGetFileInfo = TRUE;  
  _tFBufFileInfo[u4InstID].pucTargetAddr = (UCHAR *)_pucFGDatabase[u4InstID];
  _tFBufFileInfo[u4InstID].u4TargetSz = FGT_DATABASE_SZ;  
  _tFBufFileInfo[u4InstID].u4FileLength = 0; 
  fgOpenPCFile(u4InstID, "Z:\\ChkFolder\\fgt_database.bin2","r+b", &_tFBufFileInfo[u4InstID]); 
#endif
  _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSEI->pucCompModelValue = _pucFGSEISa[u4InstID]; 
}



