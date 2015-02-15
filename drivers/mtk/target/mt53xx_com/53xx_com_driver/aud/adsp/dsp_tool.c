#include "dsp_common.h"
#include "dsp_reg.h"

#include "aud_debug.h"

void CheckSumIt(UINT16 u2StartPc, UINT32 u4CodeSA, UINT32 u4CodeSZ);


UINT32 dwCRC32(UINT32 pdwBuffer, UINT32 dwCount, UINT32 dwCRC);

UINT16 encrypt(UINT16 u2pc)
{
    UINT16 i, bit[8], out;
    const UINT16 msb[16] = {2,4,1,0,3,6,2,7,1,3,5,0,0,2,1,4};
    const UINT16 lsb[16] = {2,3,7,4,1,0,5,0,6,0,2,2,1,3,1,4};

    out = 0;
    for (i = 0; i < 8 ; i ++)
        bit[i] = ( u2pc >> ( i + 1 ) ) & 1;
    if(u2pc % 2) // pc + 1 msb
    {
        for ( i = 0; i < 16 ; i ++)
            out = ( out << 1 ) + bit[msb[i]]; 
        return 
            out;
    }
    else    // pc lsb
    {
        for ( i = 0; i < 16 ; i ++)
            out = ( out << 1 ) + bit[lsb[i]];
        return 
            out;
    }

}

void CheckSumIt(UINT16 u2StartPc, UINT32 u4CodeSA, UINT32 u4CodeSZ)
{
    UINT32 u4FlashOfst;
    UINT32 u4PraOffset;
    UINT32 u4PraSize;
    UINT16 u2PC;
    UINT16 u2Idx;
    UINT16 u2Tmp;
    UINT16* pu2OpCode;
    UINT32 u4ChkSum;

    if (u2StartPc ==0xff)
    {
        return;
    }

    u2PC = u2StartPc;
    u4FlashOfst = u4GetDspImgAddr ();
    u4PraOffset = u4GetFlashData (u4FlashOfst + (u4CodeSA << 2));
    u4PraSize = u4GetFlashData (u4FlashOfst + (u4CodeSZ << 2));
    u4PraOffset = u4FlashOfst + (u4PraOffset * 4);    
    pu2OpCode = (UINT16*)(u4PraOffset);

    //LOG(0, "u2Pc=0x%x, u4CodeSA_SZ=%d,%d, u4PraOffset=0x%x, u4PraSize=%d \n", u2StartPc, u4CodeSA, u4CodeSZ, u4PraOffset-u4FlashOfst, u4PraSize);

    if (u4PraSize ==0)
    {
        return;
    }

    u4ChkSum = 0;
    for(u2Idx = 0; u2Idx < ((u4PraSize-1)*2) ; u2Idx += 2) 
    {
        u2Tmp = pu2OpCode[u2Idx];
        u2Tmp ^= 0x3d5a;
        pu2OpCode[u2Idx] = (u2Tmp ^ 0xca53 ^ encrypt(u2PC) );

        u2Tmp = pu2OpCode[u2Idx+1];
        u2Tmp ^= 0x4c7b;        
        pu2OpCode[u2Idx+1] = (u2Tmp ^ 0x5ab7 ^ encrypt(u2PC+1) );
        
        u2PC += 2;
    }
    u4ChkSum = dwCRC32( (UINT32)(pu2OpCode), (u2Idx/2), 0xFFFFFFFF);
    pu2OpCode[u2Idx] = (u4ChkSum & 0xFFFF);
    pu2OpCode[u2Idx+1] = (u4ChkSum >> 16);

    //LOG(0,"u4ChkSum =0x%x\n", u4ChkSum);
}


void CheckSumItDynam(UINT16 u2StartPc, UINT32 u4CodeSA, UINT32 u4CodeSZ, UINT32 u4ImgAddr,UINT32 u4ImageSz)
{
    UINT32 u4FlashOfst;
    UINT32 u4PraOffset;
    UINT32 u4PraSize;
    UINT16 u2PC;
    UINT16 u2Idx;
    UINT16 u2Tmp;
    UINT16* pu2OpCode;
    UINT32 u4ChkSum;

    if (u2StartPc ==0xff)
    {
        return;
    }

    u2PC = u2StartPc;
    u4FlashOfst = u4GetDspImgAddr_newscram (u4ImgAddr, u4ImageSz);
    u4PraOffset = u4GetFlashData (u4FlashOfst + (u4CodeSA << 2));
    u4PraSize = u4GetFlashData (u4FlashOfst + (u4CodeSZ << 2));
    u4PraOffset = u4FlashOfst + (u4PraOffset * 4);    
    pu2OpCode = (UINT16*)(u4PraOffset);

    //LOG(0, "u2Pc=0x%x, u4CodeSA_SZ=%d,%d, u4PraOffset=0x%x, u4PraSize=%d \n", u2StartPc, u4CodeSA, u4CodeSZ, u4PraOffset-u4FlashOfst, u4PraSize);

    if (u4PraSize ==0)
    {
        return;
    }

    u4ChkSum = 0;
    for(u2Idx = 0; u2Idx < ((u4PraSize-1)*2) ; u2Idx += 2) 
    {
        u2Tmp = pu2OpCode[u2Idx];
        u2Tmp ^= 0x3d5a;
        pu2OpCode[u2Idx] = (u2Tmp ^ 0xca53 ^ encrypt(u2PC) );

        u2Tmp = pu2OpCode[u2Idx+1];
        u2Tmp ^= 0x4c7b;        
        pu2OpCode[u2Idx+1] = (u2Tmp ^ 0x5ab7 ^ encrypt(u2PC+1) );
        
        u2PC += 2;
    }
    u4ChkSum = dwCRC32( (UINT32)(pu2OpCode), (u2Idx/2), 0xFFFFFFFF);
    pu2OpCode[u2Idx] = (u4ChkSum & 0xFFFF);
    pu2OpCode[u2Idx+1] = (u4ChkSum >> 16);

    //LOG(0,"u4ChkSum =0x%x\n", u4ChkSum);
}


static UINT32 CRCTable[256];    // Table constructed for fast lookup.
#define CRC32_POLYNOMIAL     0xEDB88320L
BOOL fgBuildCRCTable(void)
{
  UINT16 i, j;
  UINT32 dwCRC;

  for (i = 0; i <= 255; i++)
  {
    dwCRC = i;
    for (j = 8; j > 0; j--)
    {
      if (dwCRC & 1)
        dwCRC = (dwCRC >> 1) ^ CRC32_POLYNOMIAL;
      else
        dwCRC >>= 1;
    }
    CRCTable[i] = dwCRC;
  }
  return 0;
}

UINT32 dwCRC32(UINT32 u4Addr, UINT32 dwCount, UINT32 dwCRC)
{
  UINT32 dwTmp, dwTmp1, dwTmp2;
  UINT32 i;
  INT32 index;  
  static BOOL fgFirstTime = TRUE;
  UINT32*  pdwBuffer;

  pdwBuffer =  (UINT32*)u4Addr;

  if (fgFirstTime)
  {
    fgBuildCRCTable();
    fgFirstTime = FALSE;
  }

  for (index=dwCount-1; index>=0; index--)
  {
    for(i=0;i<4;i++)
    {
      dwTmp=((pdwBuffer[index]<<(i*8)) & 0xFF000000)>>24;
      dwTmp1 = (dwCRC >> 8) & 0x00FFFFFF;
      dwTmp2 = CRCTable[(dwCRC ^ dwTmp) & 0xFF];
      dwCRC = dwTmp1 ^ dwTmp2;
    }
  }

  return dwCRC;
}

