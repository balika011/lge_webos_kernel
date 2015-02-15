///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <EDID.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2013/09/18
//   @fileversion: ITE_IT66332_1.01
//******************************************/
#ifndef _EDID_H_
#define _EDID_H_


#ifdef _EDID_DEFAULT_TAB_
#if USING_DEFAULT_EDID
static iTE_u8 _CODE STRUCTURE_3D[16]={1,0,0,0,0,0,2,0,4,0,0,0,0,0,0,0};
#endif
#endif

typedef struct{
	iTE_u8	ucExtBlkCnt;
	iTE_u8	ucVSDBAdr;
	iTE_u16	usPhyAdr;
	iTE_u8	ucChkSumBlk0;
	iTE_u8	ucChkSumBlk1;
}EDID_STR, *pEDID_STR;

typedef enum _EDID_CHKSTA{
	EDID_NONE,
	EDID_CEAV23,
	EDID_VSDB,
	EDID_HDMIID,
	EDID_PHYADR,
	EDID_MAXTMDS,
	EDID_LANTECY,
	EDID_ILANTECY,
	EDID_HDMIVDO,
	EDID_3DLEN,
	EDID_3DMULTI,
	EDID_3DSTRUCT,
	EDID_3DMASK,
	EDID_DONE
}EDID_CHKSTA;

typedef enum _PARSE3D_STA{
	PARSE3D_START,
	PARSE3D_LEN,
	PARSE3D_STRUCT_H,
	PARSE3D_STRUCT_L,
	PARSE3D_MASK_H,
	PARSE3D_MASK_L,
	PARSE3D_VIC,
	PARSE3D_DONE
}PARSE3D_STA;

typedef struct _PARSE3D_STR{
	iTE_u8	uc3DEdidStart;
	iTE_u8	uc3DBlock;
	iTE_u8	*uc3DInfor;
	iTE_u8	ucVicStart;
	iTE_u8	ucVicCnt;
	iTE_u8	uc3DTempCnt;
	iTE_u8	ucDtdCnt;
}PARSE3D_STR;

typedef enum _EDID_MODE{
	EDID_MODE_NONE = 0,
	EDID_MODE_ADB,
	EDID_MODE_VDB,
	EDID_MODE_VSDB,
	EDID_MODE_SPK,
	EDID_MODE_DTC,
	EDID_MODE_REV,
	EDID_MODE_EXT
}EDID_MODE;
typedef enum _EDID_ADB_STATE{
	EDID_ADB_DONE
}EDID_ADB_STATE;
typedef enum _EDID_VDB_STATE{
	EDID_VDB_DONE
}EDID_VDB_STATE;
typedef enum _EDID_SPK_STATE{
	EDID_SPK_DONE
}EDID_SPK_STATE;
typedef enum _EDID_DTC_STATE{
	EDID_DTC_DONE
}EDID_DTC_STATE;
typedef enum _EDID_EXT_STATE{
	EDID_EXT_DONE
}EDID_EXT_STATE;
typedef enum _EDID_VSDB_STATE{
	EDID_VSDB_NONE,
	EDID_VSDB_ID,
	EDID_VSDB_PA,
	EDID_VSDB_LATENCY,
	EDID_VSDB_3D,
	EDID_VSDB_DONE
}EDID_VSDB_STATE;
typedef struct _EDID_ADB{
	EDID_ADB_STATE	eAdbState;
}EDID_ADB;
typedef struct _EDID_VDB_STR{
	EDID_VDB_STATE	eVdbState;
}EDID_VDB_STR;
typedef struct _EDID_VSDB_STR{
	EDID_VSDB_STATE	eVsdbState;
}EDID_VSDB_STR;
typedef struct _EDID_SPK{
	EDID_SPK_STATE	eSpkState;
}EDID_SPK;
typedef struct _EDID_DTC{
	EDID_DTC_STATE	eDtcState;
}EDID_DTC;
typedef struct _EDID_EXT{
	EDID_EXT_STATE	eExtState;
}EDID_EXT;

typedef struct _EDID_PARSE{
	iTE_u8 	ucRdPtr;
	iTE_s8	cLength;
	EDID_MODE	eTag;
	iTE_u8	*pucRdEdid;
}EDID_PARSE;

#if USING_DEFAULT_EDID
iTE_u1 edid_CheckDefaultVSDB(iTE_u8 *pucEdid);
void edid_LoadDefaultTab(void);
iTE_u8 edid_DefaultCheckSumGet(iTE_u8 *ucpStart, iTE_u8 ucLength);
void edid_Parse3DDefaultTab(PARSE3D_STR *pstParse3D);
#else
iTE_u8 edid_ReadBlock(iTE_u8 block);
iTE_u1 edid_CheckVSDB(iTE_u8 Block);
void edid_RxPhyAdrSet(void);
#if((Dongle==iTE_FALSE ) || (DongleEDID==INT))
void edid_Parse3DIntMem(PARSE3D_STR *pstParse3D);
#else
void edid_Parse3DExtMem(PARSE3D_STR *pstParse3D);
#endif
#endif
void edid_ParseVSDB(EDID_PARSE *pstEdidPar, EDID_VSDB_STR *pstEdidVsdb);
void edid_Get(iTE_u1 bTxHPD);
void edid_Parse3DVic(PARSE3D_STR *pstParse3D);

SYS_STATUS edid_br(iTE_u8 Offset, iTE_u8 Count, iTE_u8 *pData);
SYS_STATUS edid_bw(iTE_u8 Offset, iTE_u8 Count, iTE_u8 *pData);
SYS_STATUS edid_w(iTE_u8 Offset, iTE_u8 Data);
extern EDID_STR stEdidStr;
extern PARSE3D_STR	st3DParse;
#endif//_EDID_H_
