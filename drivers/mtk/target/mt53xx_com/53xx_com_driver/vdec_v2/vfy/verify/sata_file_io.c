#include "vdec_verify_mpv_prov.h"
#include "x_typedef.h"

#if defined(SATA_HDD_READ_SUPPORT) && !defined(SATA_HDD_FS_SUPPORT)

#include "sata_file_io.h"
#include "x_os.h"

#define USE_FILE_LIST_CACHE

const static UINT32 FILE_TABLE_ENTRY_SIZE = 256;

static CHAR *_pbBuf[2];
static CHAR _cPreDir[2][FILE_TABLE_ENTRY_SIZE] ;
static UINT32 _u4PrePos[2] = {0,0}, _u4TotalFileCnt[2] = {0,0};
static CHAR _bDWBuf[2][5];
#define SWAP_DW_BUF(a)  {a[4] = a[0];	a[0] = a[3];	a[3] = a[4]; a[4] = a[1];	a[1] = a[2];	a[2] = a[4];}
#ifdef USE_FILE_LIST_CACHE
static CHAR *_pbFileListCacheBuf[2];
#endif
static __align(16) CHAR _bFileNumBuf[2][1024];
//static BOOL _fgTest = TRUE;
UINT32 _u4HDDRefCnt[2] = {0,0};

#include "vdec_verify_file_common.h"
//extern void vVDecOutputDebugString(const CHAR * format, ...);

BOOL fgInitHDDFileAccess(UINT32 u4InstID)
{
	if(0 != _u4HDDRefCnt[u4InstID])
	{
		vVDecOutputDebugString("already InitHDD!!\n");
		return FALSE;
	}
	_u4HDDRefCnt[u4InstID]++;

	//init HDD.
	//i4IdeHalTestInit(0, TRUE, FALSE); // BDP_0871
	
	//get total #file.
	i4IdeHalTestAtaRead(0, 0x100, 0x1, (UINT32)_bFileNumBuf[u4InstID], TRUE, FALSE);
	x_memcpy(_bDWBuf[u4InstID], _bFileNumBuf[u4InstID], 4);
	SWAP_DW_BUF(_bDWBuf[u4InstID]);
	x_memcpy(&_u4TotalFileCnt[u4InstID], _bDWBuf[u4InstID], 4);	
	
	//init local use variables.
	_u4PrePos[u4InstID] = 0; 
	x_memset(_cPreDir[u4InstID], 0x0, FILE_TABLE_ENTRY_SIZE);
	_pbBuf[u4InstID] = NULL;
	_pbBuf[u4InstID] = (CHAR *)x_alloc_aligned_nc_mem(0x100*512,1024);
#ifdef USE_FILE_LIST_CACHE
	_pbFileListCacheBuf[u4InstID] = NULL;
	_pbFileListCacheBuf[u4InstID] = (CHAR *)x_alloc_aligned_nc_mem(0x500000,1024);
	if(NULL == _pbFileListCacheBuf[u4InstID] || NULL == _pbBuf[u4InstID])
		return FALSE;
	else
		return TRUE;
#endif
	if(NULL == _pbBuf[u4InstID])
		return FALSE;
	else
		return TRUE;
	}

BOOL fgUnInitHDDFileAccess(UINT32 u4InstID)
{

	_u4HDDRefCnt[u4InstID]--;
	if(0 != _u4HDDRefCnt[u4InstID])
	{
		vVDecOutputDebugString("UnInitHDD, ref:%d\n", _u4HDDRefCnt[u4InstID]);
		return TRUE;
	}
	else
	{
		_u4HDDRefCnt[u4InstID]++; //FIXME: workaround for HDD can not invoke init twice if program not terminate.
		return TRUE;
	}
	
	//free local use variables.	
	 x_free_aligned_nc_mem(_pbBuf[u4InstID]);
	_pbBuf[u4InstID] = NULL;
#ifdef USE_FILE_LIST_CACHE
	x_free_aligned_nc_mem(_pbFileListCacheBuf[u4InstID]);
	_pbFileListCacheBuf[u4InstID] = NULL;
#endif
	return TRUE;
}

UINT32 u4GetDir(CHAR *ptDest, CHAR *ptSrc)
{
	UINT32 u4Pos = 0, u4CurPos = 0;
	CHAR *ptOrgDest = ptDest;
	
	while('\0' != *ptSrc)
	{
		*ptDest = *ptSrc;
		if('\\' == *ptSrc)
		{
			u4Pos = u4CurPos;
		}
		u4CurPos++;
		ptDest++;
		ptSrc++;
	}
	*(ptOrgDest+u4Pos) = '\0';
	
	return u4Pos;
}

#ifdef USE_FILE_LIST_CACHE
BOOL fgGetDirStartPosition(UINT32 u4InstID, CHAR *ptDir, UINT32 *pu4Pos)
{
	//static UINT32 u4Buf[0x100*512];
	CHAR cDir[256];
	UINT32 u4DirLen = 0;
	UINT32 i, j;
	CHAR *bDestBuf;
	
	u4DirLen = u4GetDir(cDir, ptDir);
	
	if(0 == x_strcasecmp(cDir, _cPreDir[u4InstID]))
	{
		*pu4Pos = _u4PrePos[u4InstID];
		return TRUE;
	}
	else
	{
		for(i = 0; i<0x100; i++)
		{
			i4IdeHalTestAtaRead(0, 0x70000+(i*0x100), 0x100, (UINT32)_pbBuf[u4InstID], TRUE, FALSE);
			for(j = 0; j<0x100*2; j++)
			{
				if(0 == x_strcasecmp(cDir, _pbBuf[u4InstID]+j*256))
				{
					x_memcpy(_cPreDir[u4InstID], cDir, u4DirLen+1);
					x_memcpy(_bDWBuf[u4InstID], _pbBuf[u4InstID]+j*256+252, 4);
					SWAP_DW_BUF(_bDWBuf[u4InstID]);
					x_memcpy(pu4Pos, _bDWBuf[u4InstID], 4);
					_u4PrePos[u4InstID] = (*pu4Pos); //sector number from 0x101.

					//get next directory start LBA.
					if( (0x100*2-1) == j) //if the dir entry is the last one of 0x100 sectors, need to read one more sector.
					{
						i4IdeHalTestAtaRead(0, 0x70000+((i+1)*0x100), 0x1, (UINT32)_pbBuf[u4InstID], TRUE, FALSE);
						x_memcpy(_bDWBuf[u4InstID], _pbBuf[u4InstID]+252, 4);
					}
					else
					{
						x_memcpy(_bDWBuf[u4InstID], _pbBuf[u4InstID]+j*256+252+256, 4);
					}
					SWAP_DW_BUF(_bDWBuf[u4InstID]);
					x_memcpy(&u4DirLen, _bDWBuf[u4InstID], 4);

					if(0 == u4DirLen) //last entry of dir list, read upto 0x70000.
					{
						u4DirLen = 0x70000- 0x101 - _u4PrePos[u4InstID];
                                }
                                if((u4DirLen - _u4PrePos[u4InstID]) < 0x20)
					{
					  u4DirLen += 2000;
                                }
					//copy all this dir list until next dir start.
					for(i = 0; i<(u4DirLen-_u4PrePos[u4InstID])/0x100+1; i++)
					{
						bDestBuf = _pbFileListCacheBuf[u4InstID]+(i*0x100*512);
						i4IdeHalTestAtaRead(0, 0x101+_u4PrePos[u4InstID]+i*0x100, 0x100, (UINT32) bDestBuf, TRUE, FALSE);
					}						
					_u4PrePos[u4InstID] = (*pu4Pos) = ((u4DirLen-_u4PrePos[u4InstID])*2); //total number of files in this directory.					
					return TRUE;
				}
			}
		}		
	}	
	return FALSE;
}

INT32 _i4FindFileInfo(UINT32 u4InstID, char *strFileName, UINT32 *pu4Lba, UINT32 *pu4TotalFileLength)
{
  
	UINT32 u4Pos = 0;
	UINT32 i;
	//UINT32 u4Buf[0x100*512];
		
	if(FALSE == fgGetDirStartPosition(u4InstID, strFileName, &u4Pos))
	{
		return FALSE;
	}
	//i4IdeHalTestAtaRead(0, 0x100, 0x1, (UINT32)_pbBuf[_u4VDecID], TRUE, FALSE);
	//x_memcpy(&u4FileCnt, _pbBuf[_u4VDecID], 4);
	
	//for(i = 1; i<0x800; i++) //start from 0x101. max table is 0x80000
	i=0;
	while((i*256 < 0x500000) && (i<_u4TotalFileCnt[u4InstID]) && (i<u4Pos))
	{
			if(0 == x_strcasecmp(strFileName, _pbFileListCacheBuf[u4InstID]+i*256))
			{
				x_memcpy(_bDWBuf[u4InstID], _pbFileListCacheBuf[u4InstID]+i*256+248, 4);
				SWAP_DW_BUF(_bDWBuf[u4InstID]);
				x_memcpy(pu4Lba, _bDWBuf[u4InstID], 4);
				x_memcpy(_bDWBuf[u4InstID], _pbFileListCacheBuf[u4InstID]+i*256+252, 4);
				SWAP_DW_BUF(_bDWBuf[u4InstID]);
				x_memcpy(pu4TotalFileLength, _bDWBuf[u4InstID], 4);
				return TRUE;
			}
			i++;
	}		
	/*
  for(i=0;i<u4FileCnt;i++)
  {
    x_strcasecmp(strFileName, pucFileListPtr);
    return 0;
  }
  */
  return FALSE;
  
}
#else
BOOL fgGetDirStartPosition(UINT32 u4InstID, CHAR *ptDir, UINT32 *pu4Pos)
{
	//static UINT32 u4Buf[0x100*512];
	CHAR cDir[256];
	INT32 u4DirLen = 0;
	INT32 i, j;
	
	u4DirLen = u4GetDir(cDir, ptDir);
	
	if(0 == x_strcasecmp(cDir, _cPreDir[u4InstID]))
	{
		*pu4Pos = _u4PrePos[u4InstID];
		return TRUE;
	}
	else
	{
		for(i = 0; i<0x100; i++)
		{
			i4IdeHalTestAtaRead(0, 0x70000+(i*0x100), 0x100, (UINT32)_pbBuf[u4InstID], TRUE, FALSE);
			for(j = 0; j<0x100*2; j++)
			{
				if(0 == x_strcasecmp(cDir, _pbBuf[u4InstID]+j*256))
				{
					x_memcpy(_cPreDir[u4InstID], cDir, u4DirLen+1);
					x_memcpy(_bDWBuf[u4InstID], _pbBuf[u4InstID]+j*256+252, 4);
					SWAP_DW_BUF(_bDWBuf[u4InstID]);
					x_memcpy(pu4Pos, _bDWBuf[u4InstID], 4);
					_u4PrePos[u4InstID] = (*pu4Pos); //sector number from 0x101.
					return TRUE;
				}
			}
		}		
	}	
	return FALSE;
}

INT32 _i4FindFileInfo(UINT32 u4InstID, char *strFileName, UINT32 *pu4Lba, UINT32 *pu4TotalFileLength)
{
  
	UINT32 u4Pos = 0;
	UINT32 i, j;
	//UINT32 u4Buf[0x100*512];
	CHAR cDir[5];
		
	if(FALSE == fgGetDirStartPosition(u4InstID, strFileName, &u4Pos))
	{
		return FALSE;
	}
	//i4IdeHalTestAtaRead(0, 0x100, 0x1, (UINT32)_pbBuf[_u4VDecID], TRUE, FALSE);
	//x_memcpy(&u4FileCnt, _pbBuf[_u4VDecID], 4);
	
	//for(i = 1; i<0x800; i++) //start from 0x101. max table is 0x80000
	i=1;
	while(0x1+(i*0x100)+u4Pos < 0x70000)
	{
			i4IdeHalTestAtaRead(0, 0x1+(i*0x100)+u4Pos, 0x100, (UINT32)_pbBuf[u4InstID], TRUE, FALSE);
			for(j = 0; j<0x100*2; j++) //512BytePerSector = 256 * 2
			{
				if(0 == x_strcasecmp(strFileName, _pbBuf[u4InstID]+j*256))
				{
					x_memcpy(_bDWBuf[u4InstID], _pbBuf[u4InstID]+j*256+248, 4);
					SWAP_DW_BUF(_bDWBuf[u4InstID]);
					x_memcpy(pu4Lba, _bDWBuf[u4InstID], 4);
					x_memcpy(_bDWBuf[u4InstID], _pbBuf[u4InstID]+j*256+252, 4);
					SWAP_DW_BUF(_bDWBuf[u4InstID]);
					x_memcpy(pu4TotalFileLength, _bDWBuf[u4InstID], 4);
					return TRUE;
				}
			}
			i++;
	}		
	/*
  for(i=0;i<u4FileCnt;i++)
  {
    x_strcasecmp(strFileName, pucFileListPtr);
    return 0;
  }
  */
  return FALSE;
  
}
#endif
INT32 i4ReadFileFormHDD(
    UINT32 u4InstID,
    CHAR *strFileName,
    UINT32 u4TarAddr,
    UINT32 u4Offset,
    UINT32 u4Length,
    UINT32 *u4RealReadSize,
    UINT32 *pu4TotalFileLength,
    INT32* pi4FileId
)
{
  UINT32 u4Lba,u4ReadLength,u4DesAddr;

  // file offset must be multiple of 512
  if((u4Offset&0x1FF) != 0)
  {
    return FILE_OFFSET_ERROR;
  }
  // find Lba and file length
  if(FALSE == _i4FindFileInfo(u4InstID, strFileName, &u4Lba, pu4TotalFileLength))
  	return NOT_FIND_FILE_IN_LIST;

  //read data from HDD
  if(u4Offset > *pu4TotalFileLength)
  {
    return FILE_OFFSET_LARGE_THAN_FILE_SZ;
  }
  
  u4Lba = u4Lba + (u4Offset >> 9);
  if((*pu4TotalFileLength - u4Offset) < u4Length)
  {
    *u4RealReadSize = (*pu4TotalFileLength - u4Offset);
    u4ReadLength = (*pu4TotalFileLength - u4Offset);
    u4DesAddr = u4TarAddr;
    do
    {
      if((((u4ReadLength & 0x1FF) != 0) + (u4ReadLength >> 9)) > 0x100)
      {
        i4IdeHalTestAtaRead(0, u4Lba, 0x100, u4DesAddr, TRUE, FALSE);
        u4DesAddr += 0x20000;
        u4ReadLength -= 0x20000;
        u4Lba += 0x100;
      }
      else
      {
        i4IdeHalTestAtaRead(0, u4Lba, ((u4ReadLength & 0x1FF) != 0) + (u4ReadLength >> 9), u4DesAddr, TRUE, FALSE);
        u4ReadLength = 0;
      }
    }while(u4ReadLength!=0);
  }
  else
  {
    *u4RealReadSize = u4Length;
    u4ReadLength = u4Length;
    u4DesAddr = u4TarAddr;
    do
    {
      if((((u4ReadLength & 0x1FF) != 0) + (u4ReadLength >> 9)) > 0x100)
      {
        i4IdeHalTestAtaRead(0, u4Lba, 0x100, u4DesAddr, TRUE, FALSE);
        u4DesAddr += 0x20000;
        u4ReadLength -= 0x20000;
        u4Lba += 0x100;
      }
      else
      {
        i4IdeHalTestAtaRead(0, u4Lba, ((u4ReadLength & 0x1FF) != 0) + (u4ReadLength >> 9), u4DesAddr, TRUE, FALSE);
        u4ReadLength = 0;
      }
    }while(u4ReadLength!=0);
  }
  
  return 0;
}


/*
BOOL fgCreateFileTableInDir(CHAR *ptDir, UINT32 *pu4Pos)
{
	static CHAR cPreDir[256] ='\0';
	static UINT32 u4PrePos = 0;
	static UINT32 u4Buf[0x100*512];
	INT32 i;
	
	INT32 dwDirLen = strlen(ptDir);
	
	if(0 == x_strcasecmp(ptDir, cPreDir))
	{
		*pu4Pos = u4PrePos;
		return TRUE;
	}
	else
	{
		for(i = 0; i<0x100; i++)
		{
			i4IdeHalTestAtaRead(0, 0x70000+(i*0x100), 0x100, (UINT32)u4Buf, TRUE, FALSE);
			for(j = 0; j<0x100; j++)
			{
				if(0 == x_strcasecmp(ptDir, u4Buf+j*256))
				{
					x_memcpy(cPreDir, ptDir, 256);
					x_memcpy(pu4Pos, u4Buf+j*256+252, 4);
					u4PrePos = (*pu4Pos);
					return TRUE;
				}
			}
		}		
	}	
	return FALSE;
}
void vReadFileListFormHDD(UINT32 u4TarAddr, UINT32 *u4FileCnt)
{
  // read file count
  i4IdeHalTestAtaRead(0, 0x100, 1, u4TarAddr, TRUE, FALSE);
  u4FileCnt = (UINT32*)u4TarAddr;
  i4IdeHalTestAtaRead(0, 0x101, (*u4FileCnt)/2, u4TarAddr, TRUE, FALSE);
}
*/
#endif
