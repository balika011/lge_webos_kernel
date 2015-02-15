///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <EDID.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2013/09/18
//   @fileversion: ITE_IT66332_1.01
//******************************************/
#define _EDID_DEFAULT_TAB_
#include "../IT66332_Customer/IT66332_IO.h"
#include "../IT66332_Customer/IT66332.h"
#include "HDMI_Switch.h"
#include "EDID.h"

#define DDC_RETRY_CNT	(10)
EDID_STR stEdidStr = {0x00, 0x00, 0xffff, 0x00, 0x00};
PARSE3D_STR	st3DParse = {0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00};
//st3DParse.uc3DInfor = 0;

//****************************************************************************
#if USING_DEFAULT_EDID
iTE_u1 edid_CheckDefaultVSDB(iTE_u8 *pucEdid)
{
	EDID_VSDB_STR	stEdidVsdb;
	EDID_PARSE	stEdidPar;
	iTE_u1 bCEAv23 = iTE_FALSE;
	iTE_u16	usReadEnd;


	usReadEnd = 0x80;

	if(bCEAv23==iTE_FALSE){
		if(pucEdid[0]==0x02 && pucEdid[1]==0x03){
			bCEAv23 = iTE_TRUE;
			stEdidPar.ucRdPtr = 4;
			stEdidPar.eTag = EDID_MODE_NONE;
			stEdidPar.pucRdEdid = pucEdid;
			stEdidPar.cLength = 0;
			usReadEnd = pucEdid[2];
		}else{
			return iTE_FALSE;
		}
	}else{
		stEdidPar.ucRdPtr = 0;
	}

	for(;stEdidPar.ucRdPtr < usReadEnd;){
		switch(stEdidPar.eTag){
			case 	EDID_MODE_NONE:
					stEdidPar.cLength = pucEdid[stEdidPar.ucRdPtr] & 0x1F;
					stEdidPar.eTag = (EDID_MODE)((pucEdid[stEdidPar.ucRdPtr] & 0xE0) >> 5);
					switch(stEdidPar.eTag){
						case	EDID_MODE_VSDB:
								stEdidVsdb.eVsdbState = EDID_VSDB_NONE;
								stEdidPar.ucRdPtr++;
							break;
						case	EDID_MODE_VDB:
								st3DParse.ucVicCnt = stEdidPar.cLength;
								st3DParse.ucVicStart = 0x80 + stEdidPar.ucRdPtr + 1;
								iTE_EDID_Msg(("st3DParse.ucVicCnt = 0x%x\n", st3DParse.ucVicCnt));
						default:
								stEdidPar.eTag = EDID_MODE_NONE;
								stEdidPar.ucRdPtr += stEdidPar.cLength+1;
							break;
					}

				break;
			case	EDID_MODE_VSDB:
					if(stEdidVsdb.eVsdbState==EDID_VSDB_PA){
						stEdidStr.ucVSDBAdr = 0x80 + stEdidPar.ucRdPtr;
					}
					if(stEdidPar.cLength>0){
						edid_ParseVSDB(&stEdidPar, &stEdidVsdb);
					}else{
						stEdidPar.eTag = EDID_MODE_NONE;
					}
					if(stEdidVsdb.eVsdbState==EDID_VSDB_3D){
						if(stEdidPar.cLength > 0){
						st3DParse.uc3DEdidStart = stEdidPar.ucRdPtr;
						st3DParse.uc3DBlock = 1;
						}
					}
				break;
#if 0
			case	EDID_MODE_ADB:
				edid_ParseADB(&stEdidStr, &stEdidAdb);
				if(stEdidAdb.eAdbState == EDID_ADB_DONE)
					eEdidMode = EDID_MODE_NONE;
				break;
			case	EDID_MODE_VDB:
				stEdidVdb.ucLen = stEdidStr.cLength;
				edid_ParseVDB(&stEdidStr, &stEdidVdb);
				if(stEdidVdb.eVdbState == EDID_VDB_DONE)
					eEdidMode = EDID_MODE_NONE;
				break;
			case	EDID_MODE_VSDB:
				edid_ParseVSDB(&stEdidStr, &stEdidVsdb);
				if(stEdidVsdb.eVsdbState == EDID_VSDB_DONE)
					eEdidMode = EDID_MODE_NONE;
				break;
			case	EDID_MODE_SPK:
				edid_ParseSPK(&stEdidStr, &stEdidSpk);
				if(stEdidSpk.eSpkState == EDID_SPK_DONE)
					eEdidMode = EDID_MODE_NONE;
				break;
			case	EDID_MODE_DTC:
				edid_ParseDTC(&stEdidStr, &stEdidDtc);
				if(stEdidDtc.eDtcState == EDID_DTC_DONE);
					eEdidMode = EDID_MODE_NONE;
				break;
			case	EDID_MODE_EXT:
				edid_ParseEXT(&stEdidStr, &stEdidExt);
					eEdidMode = EDID_MODE_NONE;
				break;
#endif
		}
	}


	if(stEdidStr.ucVSDBAdr)
		return iTE_TRUE;
	else
		return iTE_FALSE;

}
//****************************************************************************
iTE_u8 edid_DefaultCheckSumGet(iTE_u8 *ucpStart, iTE_u8 ucLength)
{
	iTE_u8 ucTemp = 0;

	while(ucLength--){
		ucTemp += *(ucpStart++);
	};
	return ucTemp;
}
//****************************************************************************
void edid_LoadDefaultTab(void)
{
	iTE_u8 ucTemp;

	edid_bw(0x00, 0x80, &DEFAULT_EDID_TAB[0x00]);
	stEdidStr.ucExtBlkCnt = DEFAULT_EDID_TAB[0x7E];
	if(edid_CheckDefaultVSDB(&DEFAULT_EDID_TAB[0x80]) == iTE_FALSE){
		return;
	}
	edid_bw(0x80, 0x80, &DEFAULT_EDID_TAB[0x80]);

	stEdidStr.ucChkSumBlk0 = DEFAULT_EDID_TAB[0x7f];
	stEdidStr.ucChkSumBlk1 = DEFAULT_EDID_TAB[0xff];

	ucTemp = edid_DefaultCheckSumGet(&DEFAULT_EDID_TAB[0], 0x80);
	if(ucTemp)
		stEdidStr.ucChkSumBlk0 -= ucTemp;

	ucTemp = edid_DefaultCheckSumGet(&DEFAULT_EDID_TAB[0x80], 0x80);
	if(ucTemp)
		stEdidStr.ucChkSumBlk1 -= ucTemp;

	hdmi_sw_w(0x47, stEdidStr.ucVSDBAdr);
	hdmi_sw_w(0x48, EDID_PHYSICAL_ADDR_A >> 8);
	hdmi_sw_w(0x49, EDID_PHYSICAL_ADDR_A & 0xff);

	hdmi_sw_w(0x4c, EDID_PHYSICAL_ADDR_B >> 8);
	hdmi_sw_w(0x4d, EDID_PHYSICAL_ADDR_B & 0xff);

	hdmi_sw_w(0x50, EDID_PHYSICAL_ADDR_C >> 8);
	hdmi_sw_w(0x51, EDID_PHYSICAL_ADDR_C & 0xff);

	hdmi_sw_w(0x4a, stEdidStr.ucChkSumBlk0);
	hdmi_sw_w(0x4e, stEdidStr.ucChkSumBlk0);
	hdmi_sw_w(0x52, stEdidStr.ucChkSumBlk0);

	stEdidStr.ucChkSumBlk1 +=  (stEdidStr.usPhyAdr >> 8);
	stEdidStr.ucChkSumBlk1 +=  (stEdidStr.usPhyAdr & 0xff);

	hdmi_sw_w(0x4b, stEdidStr.ucChkSumBlk1 - (EDID_PHYSICAL_ADDR_A >> 8) - (EDID_PHYSICAL_ADDR_A & 0xff));
	hdmi_sw_w(0x4f, stEdidStr.ucChkSumBlk1 - (EDID_PHYSICAL_ADDR_B >> 8) - (EDID_PHYSICAL_ADDR_B & 0xff));
	hdmi_sw_w(0x53, stEdidStr.ucChkSumBlk1 - (EDID_PHYSICAL_ADDR_C >> 8) - (EDID_PHYSICAL_ADDR_C & 0xff));

}

//****************************************************************************
#if (USING_DEFAULT_3D_VIC == 0)
void edid_Parse3DDefaultTab(PARSE3D_STR *pstParse3D)
{
	iTE_u8	ucTemp;
	iTE_u8	uc3DMulti;
	iTE_u8	uc3DEdidEnd = 0xFF;
	iTE_u8	ucRdPtr = pstParse3D->uc3DEdidStart + (pstParse3D->uc3DBlock * 0x80);

	PARSE3D_STA	e3DEdidState = PARSE3D_START;

	iTE_Msg(("***edid_Parse3DDefaultTab*** \n"));
	if(ucRdPtr ==0)
		return;

		for(;ucRdPtr <= uc3DEdidEnd;){
			switch(e3DEdidState){
					case	PARSE3D_START:
							uc3DMulti = DEFAULT_EDID_TAB[ucRdPtr++];
							if(uc3DMulti & 0x80){
								uc3DMulti &= 0x60;
								e3DEdidState = PARSE3D_LEN;
							}else{
								return;
}
						break;
					case	PARSE3D_LEN:
							uc3DEdidEnd = (DEFAULT_EDID_TAB[ucRdPtr] >> 5) + (DEFAULT_EDID_TAB[ucRdPtr] & 0x1F) +ucRdPtr;
							ucRdPtr += (DEFAULT_EDID_TAB[ucRdPtr] >> 5) + 1;
							e3DEdidState = PARSE3D_STRUCT_H;
						break;
					case	PARSE3D_STRUCT_H:
							switch(uc3DMulti){
								case	0x20:
								case	0x40:
										if(DEFAULT_EDID_TAB[ucRdPtr++] & 0x01){
											uc3DMulti |= 0x04;
										}
										e3DEdidState = PARSE3D_STRUCT_L;
									break;
								default:
										e3DEdidState = PARSE3D_VIC;
									break;
							}
						break;
					case	PARSE3D_STRUCT_L:
							ucTemp = DEFAULT_EDID_TAB[ucRdPtr++];
							if(ucTemp & 0x40)
								uc3DMulti |= 0x02;
							if(ucTemp & 0x01)
								uc3DMulti |= 0x01;
							if((uc3DMulti & 0x60) == 0x20){
								e3DEdidState = PARSE3D_VIC;
								uc3DMulti &= 7;
#if 1
								for(ucTemp=0; ucTemp<16; ucTemp++){
									pstParse3D->uc3DInfor[ucTemp] = uc3DMulti;
									iTE_EDID_Msg(("Vic[%d]=0x%x\n", ucTemp, uc3DMulti));
								}
#else
								ucTemp = 15;
								do{
									pstParse3D->uc3DInfor[ucTemp] = uc3DMulti;
									}while(ucTemp--);
#endif
							}else{
								e3DEdidState = PARSE3D_MASK_H;
								uc3DMulti &= 7;
							}
						break;
					case	PARSE3D_MASK_H:
							for(ucTemp=0; ucTemp<8; ucTemp++){
								if(DEFAULT_EDID_TAB[ucRdPtr] & (1<<ucTemp)){
									pstParse3D->uc3DInfor[ucTemp+8] = uc3DMulti;
									iTE_EDID_Msg(("Vic[%d]=0x%x\n", ucTemp+8, uc3DMulti));
								}else{
									pstParse3D->uc3DInfor[ucTemp+8] = 0;
								}
							}
							ucRdPtr++;
							e3DEdidState = PARSE3D_MASK_L;
						break;
					case	PARSE3D_MASK_L:
							for(ucTemp=0; ucTemp<8; ucTemp++){
								if(DEFAULT_EDID_TAB[ucRdPtr] & (1<<ucTemp)){
									pstParse3D->uc3DInfor[ucTemp] = uc3DMulti;
									iTE_EDID_Msg(("Vic[%d]=0x%x\n", ucTemp, uc3DMulti));
								}else{
									pstParse3D->uc3DInfor[ucTemp] = 0;
								}
							}
							ucRdPtr++;
							e3DEdidState = PARSE3D_VIC;
						break;
					case	PARSE3D_VIC:
							ucTemp = DEFAULT_EDID_TAB[ucRdPtr]>>4;
							if(pstParse3D->ucVicCnt > ucTemp){
								pstParse3D->uc3DInfor[ucTemp] |= STRUCTURE_3D[DEFAULT_EDID_TAB[ucRdPtr] & 0xF];
								iTE_EDID_Msg(("Vic[%d]=0x%x\n", ucTemp, STRUCTURE_3D[DEFAULT_EDID_TAB[ucRdPtr] & 0xF]));
							}
							if(DEFAULT_EDID_TAB[ucRdPtr] & 0x8 )
							ucRdPtr+=2;
							else
								ucRdPtr+=1;
						break;
					default:
						break;
				}
		}
}
#endif
//****************************************************************************
#else
void edid_DdcBusReset(void)
{
	hdmi_sw_set(0x07, 0x20, 0x20);
	hdmi_sw_set(0x07, 0x20, 0x00);
	hdmi_sw_set(0x54, 0x01, 0x01);	// Enable PC DDC Mode
	hdmi_sw_w(0x5a, 0x0F);			// Abort DDC Command
	hdmi_sw_set(0x1e, 0x08, 0x00);	// Enable DDC Bus Hang Interrupt		 //????
}

void edid_DdcRead(iTE_u8 ucReadStart, iTE_u8 ucReadCnt, iTE_u8 ucBlockNo)
{
	hdmi_sw_w(0x5a, 0x09);                  // DDC FIFO Clear
	hdmi_sw_w(0x56, 0xA0);                  // EDID Address
	hdmi_sw_w(0x57, ucReadStart);     // EDID Offset
	hdmi_sw_w(0x58, ucReadCnt);        // Read ByteNum
	hdmi_sw_w(0x59, ucBlockNo/2);     // EDID Segment
	hdmi_sw_w(0x5a, 0x03);                  // EDID Read Fire
}
void edid_DdcDisable(void)
{
	hdmi_sw_w(0x5a, 0x09);          		// DDC FIFO Clear
	hdmi_sw_set(0x54, 0x01, 0x00);  	// Disable PC DDC Mode
}
iTE_u8 edid_ReadBlock(iTE_u8 block)
{
	iTE_u8	ReadCnt, CheckSum,WriteStart;
	iTE_u16 ReadStart, ReadEnd;
	iTE_u8	ucTemp, RdEdid[EDID_READ_CNT];

	iTE_EDID_Msg(("edid_ReadBlock %d \n", (int)block));

	if(block)
		WriteStart = 0x80;
	else
		WriteStart = 0x00;

	ReadStart = (block%2) * 0x80;
	ReadEnd = ReadStart + 0x80;
	CheckSum = 0;

	// Enable DDC Bus Reset
	edid_DdcBusReset();

	ucTemp = 0;
	while(ReadStart < ReadEnd){
		if(ReadStart + EDID_READ_CNT <= ReadEnd)
			ReadCnt = EDID_READ_CNT;
		else
			ReadCnt = ReadEnd - ReadStart;

		edid_DdcRead(ReadStart, ReadCnt, block);

		if( ddcwait()==iTE_FALSE ) {
			iTE_EDID_Msg(("ERROR: DDC EDID Read Fail, ReadStart=0x%x [%d/%d]!!!\n",(int)ReadStart,(int)ucTemp+1, (int)DDC_RETRY_CNT));

			if(ucTemp++ < DDC_RETRY_CNT){
			           continue;
			}else{
				edid_DdcDisable();
				return 0;
			}
			ucTemp = 0;
		}
			hdmi_sw_br(0x5b, ReadCnt, RdEdid);
//		for(ucTemp=0; ucTemp<ReadCnt; ucTemp++)
//			iTE_EDID_Msg(("0x%x ", RdEdid[ucTemp]));
#if 1	//start @20130621 added for check EDID Block 0
{
	iTE_u8 _CODE ucBlock0Header[8]={0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};

		if(block==0){
			for(ucTemp=0; ReadStart+ucTemp < 8; ucTemp++){
				if(RdEdid[ucTemp] != ucBlock0Header[ReadStart+ucTemp]){
					iTE_EDID_Msg(("\nucTemp=0x%x, ReadStart=0x%x, RdEdid=0x%x, ucBlock0Header=0x%x\n",(int)ucTemp, (int)ReadStart, (int)RdEdid[ucTemp], (int)ucBlock0Header[ReadStart+ucTemp]));
					edid_DdcDisable();
					return 0;
				}
			}
		}
}
#endif	//end @20130621
		for(ucTemp=0; ucTemp<ReadCnt; ucTemp++){
				CheckSum += RdEdid[ucTemp];
		}

		edid_bw(WriteStart, ReadCnt, RdEdid);
		WriteStart += ReadCnt;
		ReadStart += ReadCnt;
	};

	edid_DdcDisable();

	if(block == 0){
		stEdidStr.ucExtBlkCnt = RdEdid[ucTemp-2];
		iTE_EDID_Msg(("ExtBlkCnt=%d\n",(int)stEdidStr.ucExtBlkCnt));
	}

	if(CheckSum){
		ucTemp = RdEdid[ucTemp-1] - CheckSum;
		edid_w(WriteStart-1, ucTemp);
		return ucTemp;
	}

		return RdEdid[ucTemp-1];

}

//****************************************************************************
#if 0
void edid_ParseADB(EDID_PARSE *pstEdidStr, EDID_ADB *pstEdidAdb)
{
	pstEdidAdb->eAdbState = EDID_ADB_DONE;
	pstEdidStr->ucRdPtr += pstEdidStr->ucLength;
}
void edid_ParseVDB(EDID_PARSE *pstEdidStr, EDID_VDB *pstEdidVdb)
{
	pstEdidVdb->eAdbState = EDID_VDB_DONE;
	pstEdidStr->ucRdPtr += pstEdidStr->ucLength;
}
void edid_ParseSPK(EDID_PARSE *pstEdidStr, EDID_SPK *pstEdidSpk)
{
	pstEdidSpk->eAdbState = EDID_SPK_DONE;
	pstEdidStr->ucRdPtr += pstEdidStr->ucLength;
}
void edid_ParseDTC(EDID_PARSE *pstEdidStr, EDID_DTC *pstEdidDtc)
{
	pstEdidDtc->eAdbState = EDID_DTC_DONE;
	pstEdidStr->ucRdPtr += pstEdidStr->ucLength;
}
void edid_ParseEXT(EDID_PARSE *pstEdidStr, EDID_EXT *pstEdidExt)
{
	pstEdidExt->eAdbState = EDID_EXT_DONE;
	pstEdidStr->ucRdPtr += pstEdidStr->ucLength;
}
#endif
//****************************************************************************
iTE_u1 edid_CheckVSDB(iTE_u8 Block)
{
	EDID_VSDB_STR	stEdidVsdb;
//	EDID_VDB_STR	stEdidVdb;

	iTE_u8	RdEdid[EDID_READ_CNT],ucTemp;
	EDID_PARSE	stEdidPar;
	iTE_u1 bCEAv23 = iTE_FALSE;
	iTE_u16	usReadStart;
	iTE_u16	usReadEnd;

	usReadStart = (Block%2)*EDID_BLOCK_SIZE;
	usReadEnd = usReadStart + 0x80;

	// Enable DDC Bus Reset
	edid_DdcBusReset();

	ucTemp=0;
	while(usReadStart < usReadEnd){
		edid_DdcRead(usReadStart, EDID_READ_CNT, Block);

		if( ddcwait()==iTE_FALSE ) {
			iTE_EDID_Msg(("ERROR: DDC EDID Read Fail, ReadStart=0x%x [%d/%d]!!!\n",(int)usReadStart,(int)ucTemp+1, (int)DDC_RETRY_CNT));
			if(ucTemp++ < DDC_RETRY_CNT){
			           continue;
			}
			ucTemp = 0;
		}

		hdmi_sw_br(0x5b, EDID_READ_CNT, RdEdid);
		if(bCEAv23==iTE_FALSE){
			if(RdEdid[0] == 0x02 && RdEdid[1] == 0x03){
				bCEAv23 = iTE_TRUE;
				stEdidPar.ucRdPtr = 4;
				stEdidPar.eTag = EDID_MODE_NONE;
				stEdidPar.pucRdEdid = RdEdid;
				stEdidPar.cLength = 0;
				usReadEnd = RdEdid[2]+usReadStart;
			}else{
				edid_DdcDisable();
				return iTE_FALSE;
				}
		}else{
			stEdidPar.ucRdPtr = 0;
			}

		for(;stEdidPar.ucRdPtr <= (EDID_READ_CNT - EDID_READ_MIN);){
			switch(stEdidPar.eTag){
				case 	EDID_MODE_NONE:
						stEdidPar.cLength = RdEdid[stEdidPar.ucRdPtr] & 0x1F;
						stEdidPar.eTag = (EDID_MODE)(RdEdid[stEdidPar.ucRdPtr] >> 5);
						switch(stEdidPar.eTag){
							case	EDID_MODE_VSDB:
									stEdidVsdb.eVsdbState = EDID_VSDB_NONE;
									stEdidPar.ucRdPtr++;
								break;
							case	EDID_MODE_VDB:
									st3DParse.ucVicCnt = stEdidPar.cLength;
									st3DParse.ucVicStart = usReadStart + stEdidPar.ucRdPtr + 1;
							default:
									stEdidPar.eTag = EDID_MODE_NONE;
									stEdidPar.ucRdPtr += stEdidPar.cLength+1;
								break;
		}
			break;
				case	EDID_MODE_VSDB:
						if(stEdidVsdb.eVsdbState==EDID_VSDB_PA){
							stEdidStr.ucVSDBAdr = usReadStart + stEdidPar.ucRdPtr;
							stEdidStr.ucVSDBAdr |= 0x80;
}
						if(stEdidPar.cLength>0){
							edid_ParseVSDB(&stEdidPar, &stEdidVsdb);
	}else{
							stEdidPar.eTag = EDID_MODE_NONE;
						}
						if(stEdidVsdb.eVsdbState==EDID_VSDB_3D){
							iTE_EDID_Msg(("EDID_VSDB_3D\n"));
							if(stEdidPar.cLength > 0){
								st3DParse.uc3DEdidStart = usReadStart + stEdidPar.ucRdPtr;
								st3DParse.uc3DBlock = Block;
								iTE_EDID_Msg(("EDID_VSDB_3D start=0x%x, block=0x%x\n", (int)stEdidPar.ucRdPtr, (int)Block));
							}
						}
					break;
#if 0
				case	EDID_MODE_ADB:
					edid_ParseADB(&stEdidStr, &stEdidAdb);
					if(stEdidAdb.eAdbState == EDID_ADB_DONE)
						eEdidMode = EDID_MODE_NONE;
					break;
				case	EDID_MODE_VDB:
					stEdidVdb.ucLen = stEdidStr.cLength;
					edid_ParseVDB(&stEdidStr, &stEdidVdb);
					if(stEdidVdb.eVdbState == EDID_VDB_DONE)
						eEdidMode = EDID_MODE_NONE;
					break;
				case	EDID_MODE_VSDB:
					edid_ParseVSDB(&stEdidStr, &stEdidVsdb);
					if(stEdidVsdb.eVsdbState == EDID_VSDB_DONE)
						eEdidMode = EDID_MODE_NONE;
					break;
				case	EDID_MODE_SPK:
					edid_ParseSPK(&stEdidStr, &stEdidSpk);
					if(stEdidSpk.eSpkState == EDID_SPK_DONE)
						eEdidMode = EDID_MODE_NONE;
					break;
				case	EDID_MODE_DTC:
					edid_ParseDTC(&stEdidStr, &stEdidDtc);
					if(stEdidDtc.eDtcState == EDID_DTC_DONE);
						eEdidMode = EDID_MODE_NONE;
					break;
				case	EDID_MODE_EXT:
					edid_ParseEXT(&stEdidStr, &stEdidExt);
						eEdidMode = EDID_MODE_NONE;
					break;
#endif
				default:
					break;
			}
		}

		usReadStart += stEdidPar.ucRdPtr;
	}
	edid_DdcDisable();

	if(stEdidStr.ucVSDBAdr)
		return iTE_TRUE;
	else
		return iTE_FALSE;
}
//****************************************************************************
void edid_RxPhyAdrSet(void)
{
	iTE_u8 ucTemp;
	iTE_u16 usRxPhyAdr0, usRxPhyAdr1, usRxPhyAdr2;

	for(ucTemp=0; ucTemp<16; ucTemp+=4){
		if(stEdidStr.usPhyAdr & (0x0f << ucTemp)){
			break;
		}
	}

	iTE_EDID_Msg(("RxPhyAdr = 0x%x \n", stEdidStr.usPhyAdr));
	if(ucTemp){
		ucTemp = ucTemp-4;
		usRxPhyAdr0 = stEdidStr.usPhyAdr | (0x0001 << ucTemp);
		usRxPhyAdr1 = stEdidStr.usPhyAdr | (0x0002 << ucTemp);
		usRxPhyAdr2 = stEdidStr.usPhyAdr | (0x0003 << ucTemp);
		if(ucTemp%8){
			ucTemp = 0x10;
		}else{
			ucTemp = 0x01;
		}
	}else{
		usRxPhyAdr0 = 0xffff;
		usRxPhyAdr1 = 0xffff;
		usRxPhyAdr2 = 0xffff;
		ucTemp = 0xff;
	}

	hdmi_sw_w(0x47, stEdidStr.ucVSDBAdr);
	hdmi_sw_w(0x48, usRxPhyAdr0 >> 8);
	hdmi_sw_w(0x49, usRxPhyAdr0 & 0xff);

	hdmi_sw_w(0x4c, usRxPhyAdr1 >> 8);
	hdmi_sw_w(0x4d, usRxPhyAdr1 & 0xff);

	hdmi_sw_w(0x50, usRxPhyAdr2 >> 8);
	hdmi_sw_w(0x51, usRxPhyAdr2 & 0xff);

	hdmi_sw_w(0x4a, stEdidStr.ucChkSumBlk0);
	hdmi_sw_w(0x4e, stEdidStr.ucChkSumBlk0);
	hdmi_sw_w(0x52, stEdidStr.ucChkSumBlk0);
#if 0
	if(ucTemp == 0xff){
		stEdidStr.ucChkSumBlk1 +=  (stEdidStr.usPhyAdr >> 8);
		stEdidStr.ucChkSumBlk1 +=  (stEdidStr.usPhyAdr & 0xff);
		stEdidStr.ucChkSumBlk1 += 2;
		hdmi_sw_w(0x4b, stEdidStr.ucChkSumBlk1);
		hdmi_sw_w(0x4f, stEdidStr.ucChkSumBlk1);
		hdmi_sw_w(0x53, stEdidStr.ucChkSumBlk1);
	}else{
		hdmi_sw_w(0x4b, stEdidStr.ucChkSumBlk1 - ucTemp);
		hdmi_sw_w(0x4f, stEdidStr.ucChkSumBlk1 - ucTemp*2);
		hdmi_sw_w(0x53, stEdidStr.ucChkSumBlk1 - ucTemp*3);
	}
#else
	if(ucTemp == 0xff){
		stEdidStr.ucChkSumBlk1 +=  (stEdidStr.usPhyAdr >> 8);
		stEdidStr.ucChkSumBlk1 +=  (stEdidStr.usPhyAdr & 0xff);
		stEdidStr.ucChkSumBlk1 += 2;
		ucTemp = 0;
	}

	hdmi_sw_w(0x4b, stEdidStr.ucChkSumBlk1 - ucTemp);
	hdmi_sw_w(0x4f, stEdidStr.ucChkSumBlk1 - ucTemp*2);
	hdmi_sw_w(0x53, stEdidStr.ucChkSumBlk1 - ucTemp*3);

#endif
}
//****************************************************************************
#if (USING_DEFAULT_3D_VIC == 0)
#if((Dongle==iTE_FALSE ) || (DongleEDID==INT))
void edid_Parse3DIntMem(PARSE3D_STR *pstParse3D)
{
	iTE_u8	Struct3d[0x10] = {1,0,0,0,0,0,2,0,4,0,0,0,0,0,0,0};
	iTE_u8	RdEdid[EDID_READ_CNT];
	iTE_u8	ucTemp;
	iTE_u8	uc3DMulti = 0;
	iTE_u8	uc3DEdidEnd = 0xFF;
	iTE_u8	uc3DEdidStart =  pstParse3D->uc3DEdidStart;
	iTE_u8	ucRdPtr = 0;
	PARSE3D_STA	e3DEdidState = PARSE3D_START;

	iTE_Msg(("***edid_Parse3DIntMem*** \n"));
	if(uc3DEdidStart == 0)
		return;

	if(uc3DEdidStart < 0x80)
		uc3DEdidStart |= 0x80;

//	// Enable DDC Bus Reset
//	edid_DdcBusReset();

	ucTemp=0;
	while(uc3DEdidStart < uc3DEdidEnd){
#if 1
		edid_br(uc3DEdidStart, EDID_READ_CNT,RdEdid);
		iTE_Msg(("Start=0x%x, count=0x%x\n", (int)uc3DEdidStart, (int)EDID_READ_CNT));
#else
		edid_DdcRead(uc3DEdidStart, EDID_READ_CNT, pstParse3D->uc3DBlock);
		iTE_Msg(("Start=0x%x, count=0x%x, block=0x%x\n", (int)uc3DEdidStart, (int)EDID_READ_CNT, (int)pstParse3D->uc3DBlock));

		if( ddcwait()==iTE_FALSE ) {
			iTE_EDID_Msg(("ERROR: DDC EDID Read Fail, ReadStart=0x%x [%d/%d]!!!\n",(int)uc3DEdidStart,(int)ucTemp+1, (int)DDC_RETRY_CNT));
			if(ucTemp++ < DDC_RETRY_CNT){
			           return;
			}
			ucTemp = 0;
		}
		hdmi_sw_br(0x5b, EDID_READ_CNT, RdEdid);
#endif
		ucRdPtr = 0;

		for(;ucRdPtr < EDID_READ_CNT;){
			if(uc3DEdidEnd >= ucRdPtr + uc3DEdidStart){
				switch(e3DEdidState){
					case	PARSE3D_START:
							uc3DMulti = RdEdid[ucRdPtr++];
							if(uc3DMulti & 0x80){
								uc3DMulti &= 0x60;
								e3DEdidState = PARSE3D_LEN;
							}else{
								return;
							}
						break;
					case	PARSE3D_LEN:
							uc3DEdidEnd = (RdEdid[ucRdPtr] >> 5) + (RdEdid[ucRdPtr] & 0x1F) + uc3DEdidStart + ucRdPtr;
							ucRdPtr += (RdEdid[ucRdPtr] >> 5) + 1;
							e3DEdidState = PARSE3D_STRUCT_H;
						break;
					case	PARSE3D_STRUCT_H:
							switch(uc3DMulti){
								case	0x20:
								case	0x40:
										if(RdEdid[ucRdPtr++] & 0x01){
											uc3DMulti |= 0x04;
										}
										e3DEdidState = PARSE3D_STRUCT_L;
									break;
								default:
										e3DEdidState = PARSE3D_VIC;
									break;
							}
						break;
					case	PARSE3D_STRUCT_L:
							ucTemp = RdEdid[ucRdPtr++];
							if(ucTemp & 0x40)
								uc3DMulti |= 0x02;
							if(ucTemp & 0x01)
								uc3DMulti |= 0x01;
							if((uc3DMulti & 0x60) == 0x20){
								e3DEdidState = PARSE3D_VIC;
								uc3DMulti &= 7;

#if 1
								for(ucTemp=0; ucTemp<16; ucTemp++)
									pstParse3D->uc3DInfor[ucTemp] = uc3DMulti;
#else
								ucTemp = 16;
								do{
									pstParse3D->uc3DInfor[--ucTemp] = uc3DMulti;
								}while(ucTemp);
#endif

							}else{
								e3DEdidState = PARSE3D_MASK_H;
								uc3DMulti &= 7;
							}
						break;
					case	PARSE3D_MASK_H:
#if 1
							for(ucTemp=0; ucTemp<8; ucTemp++){
								if(RdEdid[ucRdPtr] & (1<<ucTemp)){
									pstParse3D->uc3DInfor[ucTemp+8] = uc3DMulti;
									iTE_EDID_Msg(("Vic[%d]=0x%x\n", ucTemp+8, uc3DMulti));
								}else{
									pstParse3D->uc3DInfor[ucTemp+8] = 0;
								}
							}
#else
							ucTemp = 7;
							do{
								if(RdEdid[ucRdPtr] & (1<<ucTemp))
									pstParse3D->uc3DInfor[ucTemp+8] = uc3DMulti;
							}while(ucTemp--);
#endif
							ucRdPtr++;
							e3DEdidState = PARSE3D_MASK_L;
						break;
					case	PARSE3D_MASK_L:
#if 1
							for(ucTemp=0; ucTemp<8; ucTemp++){
								if(RdEdid[ucRdPtr] & (1<<ucTemp)){
									pstParse3D->uc3DInfor[ucTemp] = uc3DMulti;
									iTE_EDID_Msg(("Vic[%d]=0x%x\n", ucTemp, uc3DMulti));
								}else{
									pstParse3D->uc3DInfor[ucTemp] = 0;
								}
							}
#else
							ucTemp = 7;
							do{
								if(RdEdid[ucRdPtr] & (1<<ucTemp))
									pstParse3D->uc3DInfor[ucTemp] = uc3DMulti;
							}while(ucTemp--);
#endif
							ucRdPtr++;
							e3DEdidState = PARSE3D_VIC;
						break;
					case	PARSE3D_VIC:
							ucTemp = RdEdid[ucRdPtr]>>4;
							if(pstParse3D->ucVicCnt > ucTemp){
								pstParse3D->uc3DInfor[ucTemp] |= Struct3d[RdEdid[ucRdPtr] & 0xF];
								iTE_EDID_Msg(("Vic[%d]=0x%x\n", ucTemp, Struct3d[RdEdid[ucRdPtr] & 0xF]));
							}
							if(RdEdid[ucRdPtr] & 0x8 )
							ucRdPtr+=2;
							else
								ucRdPtr+=1;
						break;
						default:
							break;
				}
			}else{
				iTE_EDID_Msg(("uc3DEdidEnd=0x%x, ucRdPtr=0x%x, uc3DEdidStart=0x%x\n",uc3DEdidEnd,ucRdPtr ,uc3DEdidStart));
				return;
			}
		}
		uc3DEdidStart += ucRdPtr;
	};
}
#else
void edid_Parse3DExtMem(PARSE3D_STR *pstParse3D)
{
	iTE_u8	Struct3d[0x10] = {1,0,0,0,0,0,2,0,4,0,0,0,0,0,0,0};
	iTE_u8	RdEdid[EDID_READ_CNT];
	iTE_u8	ucTemp;
	iTE_u8	uc3DMulti = 0;
	iTE_u8	uc3DEdidEnd = 0xFF;
	iTE_u8	uc3DEdidStart =  pstParse3D->uc3DEdidStart;
	iTE_u8	ucRdPtr = 0;
	PARSE3D_STA	e3DEdidState = PARSE3D_START;

	iTE_Msg(("***edid_Parse3DExtMem*** \n"));
	if(uc3DEdidStart == 0)
		return;

	// Enable DDC Bus Reset
	edid_DdcBusReset();

	ucTemp=0;
	while(uc3DEdidStart < uc3DEdidEnd){
		edid_DdcRead(uc3DEdidStart, EDID_READ_CNT, pstParse3D->uc3DBlock);
		iTE_Msg(("Start=0x%x, count=0x%x, block=0x%x\n", (int)uc3DEdidStart, (int)EDID_READ_CNT, (int)pstParse3D->uc3DBlock));

		if( ddcwait()==iTE_FALSE ) {
			iTE_EDID_Msg(("ERROR: DDC EDID Read Fail, ReadStart=0x%x [%d/%d]!!!\n",(int)uc3DEdidStart,(int)ucTemp+1, (int)DDC_RETRY_CNT));
			if(ucTemp++ < DDC_RETRY_CNT){
			           return;
			}
			ucTemp = 0;
		}
		hdmi_sw_br(0x5b, EDID_READ_CNT, RdEdid);

		ucRdPtr = 0;

		for(;ucRdPtr < EDID_READ_CNT;){
			if(uc3DEdidEnd >= ucRdPtr + uc3DEdidStart){
				switch(e3DEdidState){
					case	PARSE3D_START:
							uc3DMulti = RdEdid[ucRdPtr++];
							if(uc3DMulti & 0x80){
								uc3DMulti &= 0x60;
								e3DEdidState = PARSE3D_LEN;
							}else{
								return;
							}
						break;
					case	PARSE3D_LEN:
							uc3DEdidEnd = (RdEdid[ucRdPtr] >> 5) + (RdEdid[ucRdPtr] & 0x1F) + uc3DEdidStart + ucRdPtr;
							ucRdPtr += (RdEdid[ucRdPtr] >> 5) + 1;
							e3DEdidState = PARSE3D_STRUCT_H;
						break;
					case	PARSE3D_STRUCT_H:
							switch(uc3DMulti){
								case	0x20:
								case	0x40:
										if(RdEdid[ucRdPtr++] & 0x01){
											uc3DMulti |= 0x04;
										}
										e3DEdidState = PARSE3D_STRUCT_L;
									break;
								default:
										e3DEdidState = PARSE3D_VIC;
									break;
							}
						break;
					case	PARSE3D_STRUCT_L:
							ucTemp = RdEdid[ucRdPtr++];
							if(ucTemp & 0x40)
								uc3DMulti |= 0x02;
							if(ucTemp & 0x01)
								uc3DMulti |= 0x01;
							if((uc3DMulti & 0x60) == 0x20){
								e3DEdidState = PARSE3D_VIC;
								uc3DMulti &= 7;

#if 1
								for(ucTemp=0; ucTemp<16; ucTemp++)
									pstParse3D->uc3DInfor[ucTemp] = uc3DMulti;
#else
								ucTemp = 16;
								do{
									pstParse3D->uc3DInfor[--ucTemp] = uc3DMulti;
								}while(ucTemp);
#endif

							}else{
								e3DEdidState = PARSE3D_MASK_H;
								uc3DMulti &= 7;
}
						break;
					case	PARSE3D_MASK_H:
#if 1
							for(ucTemp=0; ucTemp<8; ucTemp++){
								if(RdEdid[ucRdPtr] & (1<<ucTemp)){
									pstParse3D->uc3DInfor[ucTemp+8] = uc3DMulti;
									iTE_EDID_Msg(("Vic[%d]=0x%x\n", ucTemp+8, uc3DMulti));
								}else{
									pstParse3D->uc3DInfor[ucTemp+8] = 0;
								}
							}
#else
							ucTemp = 7;
							do{
								if(RdEdid[ucRdPtr] & (1<<ucTemp))
									pstParse3D->uc3DInfor[ucTemp+8] = uc3DMulti;
							}while(ucTemp--);
#endif
							ucRdPtr++;
							e3DEdidState = PARSE3D_MASK_L;
						break;
					case	PARSE3D_MASK_L:
#if 1
							for(ucTemp=0; ucTemp<8; ucTemp++){
								if(RdEdid[ucRdPtr] & (1<<ucTemp)){
									pstParse3D->uc3DInfor[ucTemp] = uc3DMulti;
									iTE_EDID_Msg(("Vic[%d]=0x%x\n", ucTemp, uc3DMulti));
								}else{
									pstParse3D->uc3DInfor[ucTemp] = 0;
								}
							}
#else
							ucTemp = 7;
							do{
								if(RdEdid[ucRdPtr] & (1<<ucTemp))
									pstParse3D->uc3DInfor[ucTemp] = uc3DMulti;
							}while(ucTemp--);
#endif
							ucRdPtr++;
							e3DEdidState = PARSE3D_VIC;
						break;
					case	PARSE3D_VIC:
							ucTemp = RdEdid[ucRdPtr]>>4;
							if(pstParse3D->ucVicCnt > ucTemp){
								pstParse3D->uc3DInfor[ucTemp] |= Struct3d[RdEdid[ucRdPtr] & 0xF];
								iTE_EDID_Msg(("Vic[%d]=0x%x\n", ucTemp, Struct3d[RdEdid[ucRdPtr] & 0xF]));
							}
							if(RdEdid[ucRdPtr] & 0x8 )
							ucRdPtr+=2;
							else
								ucRdPtr+=1;
						break;
						default:
							break;
				}
			}else{
				iTE_EDID_Msg(("uc3DEdidEnd=0x%x, ucRdPtr=0x%x, uc3DEdidStart=0x%x\n",uc3DEdidEnd,ucRdPtr ,uc3DEdidStart));
				return;
			}
		}
		uc3DEdidStart += ucRdPtr;
	};
}
#endif
#endif
#endif
//****************************************************************************
//****************************************************************************
void edid_ParseVSDB(EDID_PARSE *pstEdidPar, EDID_VSDB_STR *pstEdidVsdb)
{
	iTE_u8	ucTemp;
	iTE_u8	ucRetValue = 0;
	switch(pstEdidVsdb->eVsdbState){
		case	EDID_VSDB_NONE:
		case	EDID_VSDB_ID:
				if(pstEdidPar->pucRdEdid[pstEdidPar->ucRdPtr]==0x03
					&& pstEdidPar->pucRdEdid[pstEdidPar->ucRdPtr+1]==0x0c
					&& pstEdidPar->pucRdEdid[pstEdidPar->ucRdPtr+2]==0x00){
					pstEdidVsdb->eVsdbState = EDID_VSDB_PA;
//					pstEdidPar->ucRdPtr += 3;
//					pstEdidPar->cLength -= 3;
					ucRetValue = 3;
				}else{
//					pstEdidPar->ucRdPtr += pstEdidPar->cLength;
					pstEdidPar->eTag = EDID_MODE_NONE;
					ucRetValue = pstEdidPar->cLength;
				}
			break;
		case	EDID_VSDB_PA:
					stEdidStr.usPhyAdr  = (iTE_u16)pstEdidPar->pucRdEdid[pstEdidPar->ucRdPtr]<<8 ;
					stEdidStr.usPhyAdr |=  pstEdidPar->pucRdEdid[pstEdidPar->ucRdPtr+1] ;
					iTE_EDID_Msg(("EDID PA=0x%x\n", (int)stEdidStr.usPhyAdr));
//					pstEdidPar->ucRdPtr+=4;
//					pstEdidPar->cLength -=4;
					pstEdidVsdb->eVsdbState = EDID_VSDB_LATENCY;
					ucRetValue = 4;
			break;
		case	EDID_VSDB_LATENCY:
					ucTemp = pstEdidPar->pucRdEdid[pstEdidPar->ucRdPtr];
					if(ucTemp & 0x80){
//						pstEdidPar->ucRdPtr += 2;
//						pstEdidPar->cLength -= 2;
						ucRetValue += 2;
					}
					if(ucTemp & 0x40){
//						pstEdidPar->ucRdPtr += 2;
//						pstEdidPar->cLength -= 2;
						ucRetValue += 2;
					}
					if(ucTemp & 0x20){
//						pstEdidPar->ucRdPtr++;
//						pstEdidPar->cLength --;
						pstEdidVsdb->eVsdbState = EDID_VSDB_3D;
						ucRetValue ++;
						iTE_EDID_Msg(("EDID_VSDB_LATENCY EDID_VSDB_3D\n"));
					}else{
						pstEdidVsdb->eVsdbState = EDID_VSDB_DONE;
					}
				break;
		case	EDID_VSDB_3D:
		case	EDID_VSDB_DONE:
//					pstEdidPar->ucRdPtr += pstEdidPar->cLength;
					pstEdidPar->eTag = EDID_MODE_NONE;
//					pstEdidPar->cLength = 0;
					ucRetValue = pstEdidPar->cLength;
			break;

	}
	pstEdidPar->ucRdPtr += ucRetValue;
	pstEdidPar->cLength -= ucRetValue;
}

//****************************************************************************
void edid_Get(iTE_u1 bChkTxHPD)
{
	iTE_u8	ucTemp;

//	iTE_EDID_Msg(("\n********************reg0x20=0x%x\n",hdmi_sw_r(0x20)));
	stEdidStr.ucVSDBAdr = 0;
	stEdidStr.ucExtBlkCnt = 0;
#if USING_DEFAULT_EDID
	edid_LoadDefaultTab();
#else
	if((bChkTxHPD==iTE_FALSE) ||(hdmi_sw_r(0x20)&0x02)){
		stEdidStr.ucChkSumBlk0 = edid_ReadBlock(0);
		for(ucTemp=0; ucTemp<stEdidStr.ucExtBlkCnt;){
			if(edid_CheckVSDB(++ucTemp)){
			iTE_EDID_Msg(("	edid_Get = 0x%x \n", (int)ucTemp));
				stEdidStr.ucChkSumBlk1 = edid_ReadBlock(ucTemp);
				edid_RxPhyAdrSet();
			}
		}
	}
#endif

}

//****************************************************************************
#if (USING_DEFAULT_3D_VIC == 0)
void edid_Parse3DVic(PARSE3D_STR *pstParse3D)
{
	iTE_Msg(("***edid_Parse3DVic*** \n"));
	if(pstParse3D->uc3DInfor == 0x00){
		return;
	}
#if	USING_DEFAULT_EDID
	edid_Parse3DDefaultTab(pstParse3D);
#else
	#if((Dongle==iTE_FALSE ) || (DongleEDID==INT))
		edid_Parse3DIntMem(pstParse3D);
	#else
		edid_Parse3DExtMem(pstParse3D);
	#endif
#endif
}
#endif
//****************************************************************************

//****************************************************************************
SYS_STATUS edid_br(iTE_u8 Offset, iTE_u8 Count, iTE_u8 *pData)
{
	iTE_i2c_read_byte(EDIDAddr, Offset, Count, pData, HDMI_Sw_I2C_Dev);
	return SYS_SUCCESS;
}
//****************************************************************************
SYS_STATUS edid_bw(iTE_u8 Offset, iTE_u8 Count, iTE_u8 *pData)
{
	iTE_i2c_write_byte(EDIDAddr, Offset, Count, pData, EDID_I2C_Dev);
	return SYS_SUCCESS;
}
//****************************************************************************
SYS_STATUS edid_w(iTE_u8 Offset, iTE_u8 Data)
{
	iTE_i2c_write_byte(EDIDAddr, Offset, 1, &Data, EDID_I2C_Dev);

	return SYS_SUCCESS;
}
