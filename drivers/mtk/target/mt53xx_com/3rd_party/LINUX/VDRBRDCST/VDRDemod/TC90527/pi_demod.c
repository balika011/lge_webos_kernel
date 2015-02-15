
#include "pi_demod.h"

#include "i2c_api.h"
#include "tuner_interface_if.h"
#include "x_typedef.h"
#include "tunerDebug.h"
#include "x_os.h"
#include "tuner_if.h"
#include "pi_isdbt_api.h"
#define TC90527_I2C_ADDR 0x30
#define I2C_TIMING        270


static DEMOD_I2C_DATA_T TC90527InitLowIFTbl[]=
{  
	//For TC90527,16M crystal,3.25M LOW IF,IF AGC only ,serial TS,
	//Register address,register value.
	{0x03,0x00},
	{0x04,0x02},
	{0x11,0x2D},// set CryStal as  16MHZ ,refer to page105 of demod datasheet
	{0x12,0x02},
	{0x1C,0x15},
	{0x1D,0x01},
	{0x1E,0x04},
	{0x1F,0x6A},
	{0x2B,0x20},
	{0x47,0x10},
	{0x50,0x00},
	{0xF1,0x80},
	{0xF8,0x42},
	{0xF9,0x02},
	{0x30,0x28},//极性设为与tuner 一致
	{0x31,0x09},//change IF to 3.25Mhz,if IF is 4MHz(reference to demod),don't need add this register setting
	{0x32,0x3f},//change IF to 3.25Mhz,if IF is 4MHz(reference to demod),don't need add this register setting
	{0x01,0x40},

};

#if 0 //25.4MHz crystal,3.25MHz LOW IF
static DEMOD_I2C_DATA_T TC90527InitLowIFTbl[]=
{  
	//For TC90527, 25.4M crystal,3.25M LOW IF,IF AGC only ,serial TS,
	//Register address,register value.
	{0x03,0x00},
	{0x04,0x02},
	{0x11,0x18},// set CryStal as  25.4MHZ ,refer to page105 of demod datasheet
	{0x12,0x02},
	{0x13,0x2c},
	{0x14,0x09},
	{0x15,0x40},
	{0x34,0x2f},
	{0x38,0x00},
	{0x39,0x96},
	{0x1C,0x15},
	{0x1D,0x01},
	{0x1E,0x04},
	{0x1F,0x6A},
	{0x2B,0x20},
	{0x47,0x10},
	{0x50,0x00},
	{0xF1,0x80},
	{0xF8,0x42},
	{0xF9,0x02},
	{0x30,0x28},//极性设为与tuner 一致
	{0x31,0x09},//change IF to 3.25Mhz
	{0x32,0x3f},//change IF to 3.25Mhz
	{0x01,0x40},

};
#endif


//----------------------------------------------------------------------------- 
/** _GetLogarithm
 *  Get the logarithm of the input value, the base of the logarithm is 10.
 *  Call this function by ISDBT_GetCNR, for CNR calculation
 *  @param   u4x  the input value.
 *  @retval  : the logarithm of the param * 100.  
 */
//-----------------------------------------------------------------------------
static UINT32 _GetLogarithm(UINT32 u4x);

//----------------------------------------------------------------------------- 
/** ISDBT_Init
 *  Initialization of ISDB-T Demod.
 *  @param   pSBTVD_DEMOD_CTX_T  Pointer of Demod context.
 *  @retval  TRUE : success.
 *  @retval  FALSE : fail.   
 */
//-----------------------------------------------------------------------------

BOOL ISDBT_Init(SBTVD_DEMOD_CTX_T * pSBTVD_DEMOD_CTX_T)
{
    UINT8 u1InitTblLen;
    UINT8 index;	       
    mcDBG_MSG_INFO(("%s, Low IF mode \n", __FUNCTION__));
    u1InitTblLen=sizeof(TC90527InitLowIFTbl)/sizeof(TC90527InitLowIFTbl[0]);
    for(index=0;index<u1InitTblLen;index++)
    {
            mcDBG_MSG_L1(("Write [0x%02X]=0x%02X\n",TC90527InitLowIFTbl[index].u1RegAddr,TC90527InitLowIFTbl[index].u1RegVal));
        if(ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,TC90527InitLowIFTbl[index].u1RegAddr,&(TC90527InitLowIFTbl[index].u1RegVal),1))
        {
            mcDBG_MSG_ERR(("ISDBT_Init I2C Fail\n"));
            return FALSE;
        }

    }   
	
    //Inital channel information data
    u1InitTblLen=sizeof(pSBTVD_DEMOD_CTX_T->ch_info)/sizeof(pSBTVD_DEMOD_CTX_T->ch_info[0]);
    mcDBG_MSG_L1(("Length of table is %d\n",u1InitTblLen));
    
    for(index=0;index<u1InitTblLen;index++)
    {
        pSBTVD_DEMOD_CTX_T->ch_info[index].fgValid=FALSE;
    }
	
    return TRUE;
}

//----------------------------------------------------------------------------- 
/** ISDBT_Reset
 *  Demodulation reset that resets the demodulation synchronization sequence.
 *  @retval  TRUE : success.
 *  @retval  FALSE : fail.    
 */
//-----------------------------------------------------------------------------

BOOL ISDBT_Reset()
{
    UINT8 u1Data=0x40;//Demodulation reset
	
    if(ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x01,&u1Data,1))
    {
        mcDBG_MSG_ERR(("I2C Fail\n"));
        return FALSE;
    }
	
    return TRUE;
}

//----------------------------------------------------------------------------- 
/** ISDBT_ScanInit
 *  Channel scan init, fast chnnel search by SP.
 *  @retval  TRUE : success.
 *  @retval  FALSE : fail.    
 */
//-----------------------------------------------------------------------------

BOOL ISDBT_ScanInit()
{
    UINT8 u1Data;
    UINT8 u1Ret=0;

    //bit7, mdetsel=0:    Mode Search ON;
    //bit6, mlocksel=1:   search completion flag ;
    //bit5, mlockcmd=0: SP detection completion;
    //bit4, mdsmd=1:     12 types search;   
    u1Data=0x50;	 
    u1Ret|=ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x47,&u1Data,1);

    //Amplitude decision threshold setting of correlation detection, Min.
    u1Data=0x00;
    u1Ret|=ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x3D,&u1Data,1);	
   
    return (u1Ret?FALSE:TRUE);
}

//----------------------------------------------------------------------------- 
/** ISDBT_TuningInit
 *  Channel change init, fast chnnel search by SP.
 *  @retval  TRUE : success.
 *  @retval  FALSE : fail.    
 */
//-----------------------------------------------------------------------------

BOOL ISDBT_TuningInit()
{
    UINT8 u1Data;
    UINT8 u1Ret=0;

    //bit7, mdetsel=1:    Mode Search OFF;
    //bit6, mlocksel=0:   synchronization completion flag ;
    //bit5, mlockcmd=0: SP detection completion;
    //bit4, mdsmd=1:     12 types search;
    u1Data=0x90;	 
    u1Ret|=ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x47,&u1Data,1);

    //Amplitude decision threshold setting of correlation detection, Min.	
    u1Data=0x00;
    u1Ret|=ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x3D,&u1Data,1);	
   
    return (u1Ret?FALSE:TRUE);
}

//----------------------------------------------------------------------------- 
/** ISDBT_Connect
 *  Wake up the ISDB-T Demodulation.
 *  @retval  TRUE : success.
 *  @retval  FALSE : fail.    
 */
//-----------------------------------------------------------------------------

BOOL ISDBT_Connect()
{
    UINT8 u1Data;
    UINT8 u1Ret=0;

    //IF_AGC  enable
    u1Data=0x15;
    u1Ret |= ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x1C,&u1Data,1);

    u1Data=0x01;
    u1Ret |= ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x1D,&u1Data,1);

    //Enable part of serial TS output
    u1Data=0x04;
    u1Ret |= ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x1E,&u1Data,1);

    u1Data=0x6a;
    u1Ret |= ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x1F,&u1Data,1);
	
    //Turn on crystal 
    u1Data=0x02;
    u1Ret |= ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x04,&u1Data,1);

    //Wake up	
    u1Data=0x00;
    u1Ret |= ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x03,&u1Data,1);
	//sw reset demod
     u1Data=0x40;
    u1Ret |= ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x01,&u1Data,1);
    //Reset demod PLL clock
    //u1Data=0x21;//Wake up
   // u1Ret |= ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x11,&u1Data,1);
	
    return (u1Ret?FALSE:TRUE);
}

//----------------------------------------------------------------------------- 
/** ISDBT_Sleep
 *  Sleep the ISDB-T Demodulation.
 *  @retval  TRUE : success.
 *  @retval  FALSE : fail.    
 */
//-----------------------------------------------------------------------------

BOOL ISDBT_Sleep()
{//return TRUE; 

    UINT8 u1Ret=0;
    UINT8 u1Data;

    //Fully Sleep, 2010/02/24
    u1Data=0x10;  
    u1Ret|=ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x03,&u1Data,1);
   
    //IF_AGC Hi-Z
    u1Data=0x55;
    u1Ret|=ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x1C,&u1Data,1);
	
	//disable TS CLOCK Valid sync 
	u1Data=0x55;
	u1Ret|=ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x1D,&u1Data,1);


    //disable part of serial TS output
   u1Data=0x55;
    u1Ret|=ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x1E,&u1Data,1);
	
    u1Data=0x54;
    u1Ret|=ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x1F,&u1Data,1);

    //Shut down crystal 
    //u1Data=0x01;
    //u1Ret|=ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x04,&u1Data,1);
	
    return (u1Ret?FALSE:TRUE);
    
}

//----------------------------------------------------------------------------- 
/** ISDBT_GetLockSts
 *  Get the ISDB-T Demod Lock Status.
 *  @retval  1 : Lock.
 *  @retval  other : Unlock.    
 */
//-----------------------------------------------------------------------------

INT8 ISDBT_GetLockSts()
{
    UINT8 u1Data=0;
    UINT8 u1Ret=0;

    u1Ret|=ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0x80,&u1Data,1);
	
    mcDBG_MSG_L4(("[0x80]=0x%02X\n",u1Data));

    return (u1Ret?-1:((u1Data & 0x28)==0));//,tmunvld=0,sulock=0
}

//----------------------------------------------------------------------------- 
/** ISDBT_GetTuningSts
 *  Get the ISDB-T Demod channel change Status.
 *  @retval  1 : Lock.
 *  @retval  other : Unlock.    
 */
//-----------------------------------------------------------------------------

INT8 ISDBT_GetTuningSts()
{
    UINT8 u1Rlock,u1Seqen;
    UINT8 u1Ret=0;
    UINT8 u1LockSts=FALSE;

    //RLCOK status	
    u1Ret|=ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0x96,&u1Rlock,1);
    //seqen status(if synchronization sequence completed)	
    u1Ret|=ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0xB0,&u1Seqen,1);
	
    if(((u1Rlock & 0xE0)!=0xE0) && ((u1Seqen & 0x0F)>=9))
    {
        u1LockSts=TRUE;
    }

    return (u1Ret?-1:u1LockSts);  //RLOCK=1 and seqen>=9
}

//----------------------------------------------------------------------------- 
/** ISDBT_TimeOut
 *  Check if the retry count is greater than the setting when channel scan
 *  @retval  1 : Time out.
 *  @retval  other : Not time out.    
 */
//-----------------------------------------------------------------------------

INT8 ISDBT_TimeOut()
{
    UINT8 u1Data;
    UINT8 u1Ret=0;
	
    u1Ret|=ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0x80,&u1Data,1);
	
    mcDBG_MSG_L4(("[0x80]=0x%02X\n",u1Data));
	
    return (u1Ret?-1:((u1Data & 0x80)==0x80)); //retryov=1 timeout
}

//----------------------------------------------------------------------------- 
/** ISDBT_GetSignalLevel
 *  Get the signal level, computed by the IF AGC value
 *  @retval  value [0~100]
 */
//-----------------------------------------------------------------------------

INT8 ISDBT_GetSignalLevel()
{  
	  
    UINT8 u1If_max,u1If_min;
    UINT8 u1If_gain;
    UINT8 u1Ret=0;
   
	
    //Get IF_AGC range
    //Get IF_Max
    u1Ret |= ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0x2E,&u1If_max,1);
    //Get IF_Min
    u1Ret |= ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0x20,&u1If_min,1);
	
    //Get IF gain data 
    u1Ret |= ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0x82,&u1If_gain,1);   
   
    if(u1Ret)
    {
        return -1; 
    }   
	
    mcDBG_MSG_L2(("[0x82]=0x%02X\n", u1If_gain));
	
    if(u1If_gain>u1If_max)
    {
        return 100;
    }

    if(u1If_gain<u1If_min)
    {
        return 0;
    }
    
    return ((u1If_max-u1If_gain)*100/(u1If_max-u1If_min));

}

//----------------------------------------------------------------------------- 
/** ISDBT_GetSigCnr
 *  Get signal Carrier to Noise Ratio (CNR), Units: 0.01.
 *  @param   psDemodCtx  Pointer of Demod context.
 *  @retval  CNR
 *   if return value is 0, the Demod unsync or I2c Failed
 */
//-----------------------------------------------------------------------------
UINT32 ISDBT_GetSigCnr()
{ 

    UINT32 u4CNR=0;  
    UINT32 u4CNData;
    INT32 i4Temp=0;
    UINT32 u4P=0;
    UINT8 u1Data[3];
    UINT8 fgRet = FALSE;

    fgRet |= ICtrlBus_I2cDemodRead(I2C_TIMING, TC90527_I2C_ADDR, 0x8B, u1Data, 3);
    if(fgRet)
    {
        mcDBG_MSG_INFO(("I2c Failed\n"));
        return 0;
    }    
    u4CNData = ((UINT32)u1Data[0] << 16) | ((UINT32)u1Data[1] << 8) |((UINT32)u1Data[2]);
    if(u4CNData == 0)
    {
        mcDBG_MSG_INFO(("cndata is 0\n"));
        return 0;
    }

    u4P = _GetLogarithm(5505024*100/u4CNData);  // P*10
    i4Temp = 24*u4P-16000;
    i4Temp = i4Temp/10;                 // P3
    i4Temp = i4Temp*u4P+398000;
    i4Temp = i4Temp/10;                 // P2
    i4Temp = i4Temp*u4P+5491000;
    i4Temp = i4Temp/1000;             // P1
    i4Temp = i4Temp*u4P+309650;
    i4Temp = i4Temp/1000;             // P0  
    if(i4Temp < 0)
    {
        i4Temp = 0;
    }
    u4CNR = (UINT32)i4Temp;   // Units: 0.01
    mcDBG_MSG_L2(("%s, CNData = %ld, CNR = %ld\n", __FUNCTION__, u4CNData, u4CNR)); 

    return u4CNR;
    
}

//----------------------------------------------------------------------------- 
/** ISDBT_SetFreq
 *  Set the tuner freq to the specified freq
 *  @param  param, Pointer of tuner param context.
 *  @retval  1: Success
 *  @retval  0: unlock
 *  @retval  -1: I2c fail
 */
//-----------------------------------------------------------------------------

INT8 ISDBT_SetFreq(PARAM_SETFREQ_T  * param)
{	

    BOOL fgPllSts;
    UINT8 ii=0;

	
    if(ITuner_SetFreq(ITunerGetCtx(),param))
    {  //I2C fail
        return -1;
    }
    else
    {
        while(1)
        {
            if(ITuner_OP(ITunerGetCtx(),itGetStatus,0,&fgPllSts)==ITUNER_OK)
            {  // Check tuner PLL if lock
                if(fgPllSts)
                {
                    mcDBG_MSG_L1(("PLL lock the %dth time\n",ii))
                    return 1;
                }

                ii++;
                if(ii>10)
                {
                    mcDBG_MSG_L1(("PLL unlock at the %dth time\n",ii))
                    return 0;
                }
                x_thread_delay(10);
            }
            else
            {  //Not support itGetStatus
                return 1;
            }
	}
    }
 }

//----------------------------------------------------------------------------- 
/** ISDBT_GetBer
 *  Get the signal BER/PER measurement
 *  @param   pSBTVD_DEMOD_CTX_T  Pointer of Demod context.
 *  @param  fgBer, 0:PER measurement, 1:BER measurement.
 *  @retval  TRUE: Success
 *  @retval  FALSE: Fail
 */
//-----------------------------------------------------------------------------

BOOL ISDBT_GetBer(SBTVD_DEMOD_CTX_T * pSBTVD_DEMOD_CTX_T, BOOL fgBer)
{
    UINT8 u1LockSts;
    UINT8 u1Data[15], u1Seg[4];
    UINT8 u1NSegA, u1NSegB, u1NSegC;   //Segment numbers on the Layer A, B and C
    UINT32 u4ErrPkt, u4TotPkt, u4Val;
    UINT8 u1Ret=0;
    UINT8 u1Cor=0; 
  
    if(fgBer)//For BER
    {
        u1Cor=0x01;//Cor=1 For BER measurement 
    }
    u1Ret|=ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x77,&u1Cor,1);

    //Demod Lock Status
    u1Ret|=ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0x80,&u1LockSts,1);
    if(!u1Ret)
    {
        if((u1LockSts & 0x28)) // Demod Unlock
        {
            pSBTVD_DEMOD_CTX_T->u4Ber = 0xFFFFFFFF;
            pSBTVD_DEMOD_CTX_T->u4Per = 0xFFFFFFFF;
            mcDBG_MSG_L2(("Demod Unlock, both BER and PER are max value\n"));	
            return TRUE;
        }
    }

    // Read the packet statusdata  
    u1Ret|=ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0x9D,u1Data,15);
	
    // Read the layer segment status   
    u1Ret|=ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0xB4,u1Seg,4);
    u1NSegA = (UINT8)((u1Seg[0] >> 3) & 0x0F);
    u1NSegB = (UINT8)(((u1Seg[1] & 0x03) << 2)|((u1Seg[2] >> 6) & 0x03));
    u1NSegC = (UINT8)((u1Seg[3] >> 1) & 0x0F);

    // Compute the BER or PER
    u4ErrPkt=0;
    u4TotPkt=0;

    if(u1NSegA < 0x0F)
    {  // layer A segment
        u4ErrPkt+=(UINT32)(u1Data[0]<<16) | (UINT32)(u1Data[1]<<8) | (UINT32)(u1Data[2]);
        u4TotPkt+=(UINT32)(u1Data[9]<<8) | (UINT32)(u1Data[10]);
        mcDBG_MSG_L2(("Number of segments used on Layer A  = %d\n", u1NSegA));
    }
    if(u1NSegB < 0x0F)
    {  // layer B segment
        u4ErrPkt+=(UINT32)(u1Data[3]<<16) | (UINT32)(u1Data[4]<<8) | (UINT32)(u1Data[5]);
        u4TotPkt+=(UINT32)(u1Data[11]<<8) | (UINT32)(u1Data[12]);
        mcDBG_MSG_L2(("Number of segments used on Layer B  = %d\n", u1NSegB));
    }
    if(u1NSegC < 0x0F)
    {  // layer C segment
        u4ErrPkt+=(UINT32)(u1Data[6]<<16) | (UINT32)(u1Data[7]<<8) | (UINT32)(u1Data[8]);
        u4TotPkt+=(UINT32)(u1Data[13]<<8) | (UINT32)(u1Data[14]);
        mcDBG_MSG_L2(("Number of segments used on Layer C = %d\n", u1NSegC));
    }
  
    mcDBG_MSG_L2(("u4ErrPkt=%d,u4TotPkt=%d\n",u4ErrPkt,u4TotPkt));
  
    if(u4TotPkt==0)
    {
        pSBTVD_DEMOD_CTX_T->u4Ber = 0xFFFFFFFF;
        pSBTVD_DEMOD_CTX_T->u4Per = 0xFFFFFFFF;		
        mcDBG_MSG_L2(("u4TotPkt is 0, both BER and PER are max value\n"));
        return TRUE;
    }
	
    if(fgBer)
    {  // BER
        u4Val=(100000*u4ErrPkt)/(u4TotPkt * 204 * 8);
        pSBTVD_DEMOD_CTX_T->u4Ber=u4Val;
    }
    else
    {  //PER
        u4Val=(1000*u4ErrPkt)/u4TotPkt;
        u4Val*=100;
        pSBTVD_DEMOD_CTX_T->u4Per=u4Val;
    }
  
    return (u1Ret?FALSE:TRUE);
}

UINT16 ISDBT_GetSQI(SBTVD_DEMOD_CTX_T * pSBTVD_DEMOD_CTX_T)
{
	UINT32 u4CNR=0;
    UINT16 u2_ret=0;
	UINT8 fgLock=0;
 	u4CNR=ISDBT_GetSigCnr();
 	fgLock=ISDBT_GetLockSts();
   if(!fgLock)
  		{
  		u2_ret=0;
		}
   else
   	{
   	    if(u4CNR > 3000)
	    {
	        u2_ret = 100;   
	    }
    else if(u4CNR < 500)
	    {
	        u2_ret = 0;
	    }
    else
	    {
	        u2_ret = (UINT16)((u4CNR-500)/25);
	    }
   	}
  return u2_ret;
}
//#ifdef ENABLE_DEMOD_PRESET

//----------------------------------------------------------------------------- 
/** GetChIndex
 *  Get the channel number, computed by the RF in
 *  @param   u4Freq  RF Freq to set the tuner.
 *  @retval  Channel Number
 */
//-----------------------------------------------------------------------------

static UINT8 GetChIndex(UINT32 u4Freq)
{
    UINT32 u4FreqMHz=u4Freq/1000;
   
    // mcDBG_MSG_L1(("u4Freq=%d KHz,u4FreqMHz=%d MHz\n",u4Freq,u4FreqMHz));
   
    if(u4FreqMHz>213)
    {
        return (UINT8)((u4FreqMHz-473)/6+7);
    }
   
    return (UINT8)(u4FreqMHz-177)/6;
}

//----------------------------------------------------------------------------- 
/** StoreTMCCData
 *  Get and save the TMCC status data from the demod
 *  @param   pSBTVD_DEMOD_CTX_T  Pointer of Demod context.
 *  @retval  TRUE: Success
 *  @retval  FALSE: Fail
 */
//-----------------------------------------------------------------------------

BOOL StoreTMCCData(SBTVD_DEMOD_CTX_T * pSBTVD_DEMOD_CTX_T)
{
    UINT8 u2ChIndex;
    UINT8 u1Ret=0;

    // Get the current RF freq
    u2ChIndex=GetChIndex(ITunerGetCtx()->u4RF_Freq);
    mcDBG_MSG_L2(("Store TMCC data %d\n", u2ChIndex ));
	
    //Read TMCC data 0xB0,0xB2~0xB8
    u1Ret |= ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0xB0,pSBTVD_DEMOD_CTX_T->ch_info[u2ChIndex].u1TMCCData,1);
    mcDBG_MSG_L1(("[B0]=0x%02X\n",pSBTVD_DEMOD_CTX_T->ch_info[u2ChIndex].u1TMCCData[0]));
	
    u1Ret |= ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0xB2,pSBTVD_DEMOD_CTX_T->ch_info[u2ChIndex].u1TMCCData+1,7);

    //Read Eqqdt 0x90 [2:0]
    u1Ret |= ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0x90,&(pSBTVD_DEMOD_CTX_T->ch_info[u2ChIndex].u1Eqqdt),1);
  
    if(u1Ret)
    {  //I2C fail
        pSBTVD_DEMOD_CTX_T->ch_info[u2ChIndex].fgValid=FALSE;
        return FALSE;
    }
    else
    {
        pSBTVD_DEMOD_CTX_T->ch_info[u2ChIndex].fgValid=TRUE;
        return TRUE;
    }
}

//----------------------------------------------------------------------------- 
/** LoadTMCCData
 *  Write the TMCC status data to the demod
 *  @param   pSBTVD_DEMOD_CTX_T  Pointer of Demod context.
 *  @retval  TRUE: Success
 *  @retval  FALSE: Fail
 */
//-----------------------------------------------------------------------------

BOOL LoadTMCCData(SBTVD_DEMOD_CTX_T * pSBTVD_DEMOD_CTX_T)
{
    UINT8 u2ChIndex;
    UINT8 u1Ret=0;
    UINT8 u1Data;

    // Get the current RF freq
    u2ChIndex=GetChIndex(ITunerGetCtx()->u4RF_Freq);
    mcDBG_MSG_L2(("Load TMCC data %d\n",u2ChIndex ));
  
    //u1Ret|=ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0x47,&u1Data,1);
    //u1Data |=0x80;//mdetsel=1 Mode search off
    //u1Ret|=ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x47,&u1Data,1);
  
    //Write TMCC data 0xB0,0xB2~0xB8
    u1Ret|=ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0xB0,pSBTVD_DEMOD_CTX_T->ch_info[u2ChIndex].u1TMCCData,1);
    u1Ret|=ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0xB2,pSBTVD_DEMOD_CTX_T->ch_info[u2ChIndex].u1TMCCData+1,7);

    //Write Eqqdt 0x90 [2:0] to  eqqth 0x4F[2:0] 
    u1Ret|=ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0x4F,&u1Data,1);
    u1Data &=0x1F;
    u1Data|=(pSBTVD_DEMOD_CTX_T->ch_info[u2ChIndex].u1Eqqdt & 0xE0);
    u1Ret|=ICtrlBus_I2cDemodWrite(I2C_TIMING,TC90527_I2C_ADDR,0x4F,&u1Data,1);

    return (u1Ret?FALSE:TRUE);
}

//----------------------------------------------------------------------------- 
/** vCleanTMCCData
 *  Clean the TMCC Data, Set the fgValid to FALSE
 *  @param   pSBTVD_DEMOD_CTX_T  Pointer of Demod context.
 *  @retval  void
 */
//-----------------------------------------------------------------------------

VOID vCleanTMCCData(SBTVD_DEMOD_CTX_T * pSBTVD_DEMOD_CTX_T)
{
    UINT8 u2ChIndex;

    // Get the current RF freq
    u2ChIndex=GetChIndex(ITunerGetCtx()->u4RF_Freq);
    mcDBG_MSG_L2(("Clean TMCC data %d\n",u2ChIndex ));
    pSBTVD_DEMOD_CTX_T->ch_info[u2ChIndex].fgValid=FALSE;
}

//----------------------------------------------------------------------------- 
/** fgTMCCDataAval
 *  Get  the TMCC status data if is valid
 *  @param   pSBTVD_DEMOD_CTX_T  Pointer of Demod context.
 *  @retval  TRUE: Valid
 *  @retval  FALSE: Unvalid
 */
//-----------------------------------------------------------------------------

BOOL fgTMCCDataAval(SBTVD_DEMOD_CTX_T * pSBTVD_DEMOD_CTX_T)
{
    UINT8 u2ChIndex;

    // Get the current RF freq
    u2ChIndex=GetChIndex(ITunerGetCtx()->u4RF_Freq);
    mcDBG_MSG_L2(("TMCC data %d is %svalid\n",u2ChIndex,((pSBTVD_DEMOD_CTX_T->ch_info[u2ChIndex].fgValid)?"":"in")));
	
    return (pSBTVD_DEMOD_CTX_T->ch_info[u2ChIndex].fgValid);
}

//----------------------------------------------------------------------------- 
/** vShowTMCCData
 *  Show  the TMCC status data
 *  @param   pSBTVD_DEMOD_CTX_T  Pointer of Demod context.
 *  @retval  void
 */
//-----------------------------------------------------------------------------

VOID vShowTMCCData(SBTVD_DEMOD_CTX_T * pSBTVD_DEMOD_CTX_T)
{
    UINT8 u2ChIndex;
    UINT8 *pTMCCData;
    UINT8 u1Data;
    UINT8 u1Mode=4;
    UCHAR ucEnableBanner;
	UCHAR u1DbgPfxMsgEnable=FALSE;
    // Get the current RF freq
    u2ChIndex=GetChIndex(ITunerGetCtx()->u4RF_Freq);
    // Get the pointer to the TMCC data array
    pTMCCData=pSBTVD_DEMOD_CTX_T->ch_info[u2ChIndex].u1TMCCData;
   
    if(pSBTVD_DEMOD_CTX_T->ch_info[u2ChIndex].fgValid)
    {        
        mcDBG_MSG_L2(("Show TMCC data %d\n",u2ChIndex ));
        ucEnableBanner=u1DbgPfxMsgEnable;
        u1DbgPfxMsgEnable=FALSE;

        //Addr: 0xB0   
        u1Data=pTMCCData[0]; 
        //ffsize undefined, FFT size
        if((u1Data & 0xC0)!=0xC0)
        {
            u1Mode=((u1Data & 0xC0)>>6)+1;
            mcDBG_MSG_L1(("Mode %d, ",u1Mode));
        }
        // gdleng, Guard ratio
        mcDBG_MSG_L1(("GI=1/%d, ",32>>((u1Data & 0x30)>>4)));

        // Addr: 0xB2
        u1Data=pTMCCData[1];
        // sysid, TMCC system identification
        if((u1Data & 0x80)!=0x80)
        {
            mcDBG_MSG_L1(("TMCC Sys: %s ,",((u1Data & 0x40)?"Audio":"TV"))); 
            if(u1Data & 0x40)
            {  //Audio system
                mcDBG_MSG_L1((" %s\n",((u1Data & 0x01)?"3 segments":"1 segment")));
            }
            else
            {  //TV System
                mcDBG_MSG_L1((" %s Partial reception\n",((u1Data & 0x01)?"":"No")));
            }
        } 

        // Addr: 0xB3
        u1Data=pTMCCData[2];
        mcDBG_MSG_L1(("Layer A:"));
        // Layer A, Carrier modulation scheme
        switch((u1Data & 0xE0)>>5)
        {
        case 0:
            mcDBG_MSG_L1(("DQPSK,"));
            break;
        case 1:
            mcDBG_MSG_L1(("QPSK,"));
            break;
        case 2:
            mcDBG_MSG_L1(("16QAM,"));
            break;
        case 3:
            mcDBG_MSG_L1(("64QAM,"));
            break;
        default:
            break;
        }
        // Layer A, Convolution coding rate	 
        u1Data>>=2;
        u1Data&=0x07;
        if(u1Data<5)
        {
            switch(u1Data)
            {
            case 0:
                mcDBG_MSG_L1(("CR: 1/2,"));
                break;
            case 1:
                mcDBG_MSG_L1(("CR: 2/3,"));
                break;
            case 2:
            case 3:
            case 4:
                mcDBG_MSG_L1(("CR: %d/%d,",u1Data*2-1,u1Data*2));
                break;
            default:
                break;
            }
        }
        // Layer A, Time interleave scheme	 
        u1Data = ((u1Data & 0x03)<<1)|((pTMCCData[3] & 0x80)>>7);	 
        if(u1Data==0)
        {
            mcDBG_MSG_L1(("I==0,"));
        }
        else 
        {
            if(u1Data<5)
            {
                if(u1Mode!=4)
                {
                    mcDBG_MSG_L1(("I=%d,",0x20>>(3+u1Mode-u1Data))); 	  
                } 
            }
        }
        // Layer A, Number of segments used 
        u1Data=pTMCCData[3];   // Addr: 0xB4
        mcDBG_MSG_L1((" %d segments\n",(u1Data & 0x78)>>3));

        // Layer B, Carrier modulation scheme	  
        mcDBG_MSG_L1(("Layer B:"));
        switch(u1Data & 0x03)
        {
        case 0:
            mcDBG_MSG_L1(("DQPSK,"));
            break;
        case 1:
            mcDBG_MSG_L1(("QPSK,"));
            break;
        case 2:
            mcDBG_MSG_L1(("16QAM,"));
            break;
        case 3:
            mcDBG_MSG_L1(("64QAM,"));
            break;
        default:
            break;
        }
        // Layer B, Convolutional coding rate
        u1Data=pTMCCData[4]>>5 & 0x07;  //Addr: 0xB5
        if(u1Data<5)
        {
            switch(u1Data)
            {
            case 0:
                mcDBG_MSG_L1(("CR: 1/2,"));
                break;
            case 1:
                mcDBG_MSG_L1(("CR: 2/3,"));
                break;
            case 2:
            case 3:
            case 4:
                mcDBG_MSG_L1(("CR: %d/%d,",u1Data*2-1,u1Data*2));
                break;
            default:
                break;
            }
        }
        // Layer B, Time interleave scheme
        u1Data=(pTMCCData[4]>>2) & 0x07; 
        if(u1Data==0)
        {
            mcDBG_MSG_L1(("I==0,"));
        }
        else 
        {
            if(u1Data<5)
            {
                if(u1Mode!=4)
                {
                    mcDBG_MSG_L1(("I=%d,",0x20>>(3+u1Mode-u1Data))); 	  
                } 
            }
        }
        // Layer B, Number of segments used 
        u1Data=((pTMCCData[4] & 0x03)<<2) | ((pTMCCData[5]>>6) & 0x03); //0xB5
        mcDBG_MSG_L1((" %d segments\n",u1Data));

        // Layer C, Carrier modulation scheme	
        mcDBG_MSG_L1(("Layer C:"));
        u1Data=pTMCCData[5]>>3;        //Addr:0xB6
        switch(u1Data & 0x03)
        {
        case 0:
            mcDBG_MSG_L1(("DQPSK,"));
            break;
        case 1:
            mcDBG_MSG_L1(("QPSK,"));
            break;
        case 2:
            mcDBG_MSG_L1(("16QAM,"));
            break;
        case 3:
            mcDBG_MSG_L1(("64QAM,"));
            break;
        default:
            break;
        }
        // Layer C, Convolutional coding rate	 
        u1Data=pTMCCData[5] & 0x07;      //Addr:0xB6
        if(u1Data<5)
        {
            switch(u1Data)
            {
            case 0:
                mcDBG_MSG_L1(("CR: 1/2,"));
                break;
            case 1:
                mcDBG_MSG_L1(("CR: 2/3,"));
                break;
            case 2:
            case 3:
            case 4:
                mcDBG_MSG_L1(("CR: %d/%d,",u1Data*2-1,u1Data*2));
                break;
            default:
                break;
            }
        }
        // Layer C, Time interleave scheme
        u1Data=(pTMCCData[6]>>5) & 0x07;//0xB7	 
        if(u1Data==0)
        {
            mcDBG_MSG_L1(("I==0,"));
        }
        else 
        {
            if(u1Data<5)
            {
                if(u1Mode!=4)
                {
                    mcDBG_MSG_L1(("I=%d,",0x20>>(3+u1Mode-u1Data))); 	  
                } 
            }
	  }
        // Layer C, Number of segments used 
        u1Data=(pTMCCData[6]>>1) & 0x0F;//0xB7
        if(u1Data!=0x0F)
        {
            mcDBG_MSG_L1((" %d segments\n",u1Data));
        }
		
        mcDBG_MSG_L1(("Eqqdt=0x%02X\n",(pSBTVD_DEMOD_CTX_T->ch_info[u2ChIndex].u1Eqqdt & 0xE0)>>5));
        u1DbgPfxMsgEnable=ucEnableBanner;
    }
}
//#endif

//----------------------------------------------------------------------------- 
/** ISDBT_GetEWSInfo
 *  Get the EWS(Emergency Warning System) flag
 *  @param   pEwsInfo Pointer of EWS information context.
 *  @retval  TRUE: Success
 *  @retval  FALSE: Fail
 */
//-----------------------------------------------------------------------------

BOOL ISDBT_GetEWSInfo(EWS_INFO* pEwsInfo)
{
    UINT8 u1Ret=0;
    UINT8 u1Data;//Emergence flag

    u1Ret |= ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0x81,&u1Data,1);
  
    if((u1Data & 0x80)==0x80)//emerg=1 
    {  // Start controlled
        pEwsInfo->val=1; 
    }
    else
    {  // Start not controlled
        pEwsInfo->val=0;
    }
  
    return ((u1Ret)?FALSE:TRUE);
 }

 //----------------------------------------------------------------------------- 
 /** ISDBT_GetTSFMT
  *  Get the TS format
  *  @param pTsfmt Pointer of TS format context.
  *  @retval  TRUE: Success
  *  @retval  FALSE: Fail
  */
 //-----------------------------------------------------------------------------

BOOL ISDBT_GetTSFMT(TS_FMT_T * pTsfmt)
{
    UINT8 u1Data,u1Data2;
    UINT8 u1Ret=0;
    UINT32 u4TSClkInKHz;

    //Serial TS data output control   
    u1Ret |= ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0x1E,&u1Data,1);
    //0x1E [3:2], srdtoen 0:Enable 1:Hign-Z 2:Fixed to '0' 3:Fixed to '1'
    if((u1Data & 0x0C) ==0)
    {  //TS data output enable
        pTsfmt->fgTristate=BOOL_TRUE;  
    }
    else if((u1Data & 0x0C) ==0x04)     //Hign-Z
    {  //TS data output disable
        pTsfmt->fgTristate=BOOL_FALSE;
    }
    else
    {
        pTsfmt->fgTristate=BOOL_UNKNOWN;
    }
	
    //Serial or parallel 0x0F [1:0] 0:Serial TS 1:2TS output mode 2:Parallel TS 3:Flag output mode
    u1Ret|=ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0x0F,&u1Data,1); 
    if((u1Data & 0x03)==0x00)
    {  //Serial
        pTsfmt->SerOrPar=SP_SERIAL;
    }
    else if((u1Data & 0x03)==0x02)
    {  //parallel
        pTsfmt->SerOrPar=SP_PARALLEL;
    }
    else
    {
        pTsfmt->SerOrPar=SP_UNKNOWN;
    }
   
    //Clock polarity 0x75 [1] 0:rising edge  1: inverse
    u1Ret|=ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0x75,&u1Data,1);
    if((u1Data & 0x02)==0x00)//rising edge match
    {
        pTsfmt->ClockPolarity=POL_HIGH_ACTIVE;
    }
    else
    {
        pTsfmt->ClockPolarity=POL_LOW_ACTIVE;
    }
   
    //TS clock rate
    u1Ret|=ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0x12,&u1Data,1); //iexdiv
    if((u1Data & 0x1F)==0x0B)
    {  //25.4MHz XTAL
        u4TSClkInKHz=25400;
    }
    else if((u1Data & 0x1F)==0x08)
    {  //16MHz XTAL
        u4TSClkInKHz=16000;
    }
    else if((u1Data & 0x1F)==0x02)
    {  //4MHz XTAL
        u4TSClkInKHz=4000;
    }
    else 
    {  //Default 25.4MHz
        u4TSClkInKHz=25400;
    }
    u1Ret|=ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0x11,&u1Data2,1); //ilpdiv
    u4TSClkInKHz*=(u1Data2/u1Data);	 
    u4TSClkInKHz/=2;	 
    pTsfmt->u4TsClockInKHz=u4TSClkInKHz;
	 
    //If gated clock
    u1Ret|=ICtrlBus_I2cDemodRead(I2C_TIMING,TC90527_I2C_ADDR,0x71,&u1Data2,1); //palonff
    if((u1Data2&0x20)==0x20)//Gated
    {
        pTsfmt->fgGatedClock=BOOL_TRUE;
    }
    else
    {
        pTsfmt->fgGatedClock=BOOL_FALSE;
    }
	
    //Externel demod
    pTsfmt->fgExternalDemod=BOOL_TRUE;
	 
    return ((u1Ret)?FALSE:TRUE);
}

//----------------------------------------------------------------------------- 
/** _GetLogarithm
 *  Get the logarithm of the input value, the base of the logarithm is 10.
 *  Call this function by ISDBT_GetCNR, for CNR calculation
 *  @param   u4x  the input value.
 *  @retval  : the logarithm of the param * 100.  
 */
//-----------------------------------------------------------------------------
static UINT32 _GetLogarithm(UINT32 u4x)
{
    UINT32 u4logx;
    UINT32 u4Num, u4Temp;
	
    // Log Map, log3,log8, log(5*i)	
    static UINT32 u4logMap[20] = 
        {48,   90,  100, 118, 130, 140, 148, 154, 160, 165,
          170,174, 178, 181, 185, 188, 190, 193, 195, 198};

    u4Num = 0;

    if(u4x < 1)
    {
        return 0;
    }
    u4Temp = u4x;
    while(u4Temp >= 100)
    {
       u4Temp= u4Temp/10;       
       u4Num++;
    }
    u4Temp= u4Temp*2/10;
    u4logx = u4Num*100 + u4logMap[u4Temp];  //log Value * 100

    if(u4logx < 200)
    {
        return 0;
    }
	
    u4logx -= 200;
	
    return u4logx;
}

 
