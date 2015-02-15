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
/*-----------------------------------------------------------------------------
 * Copyright (c) 2008, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: pi_tuner_ra217.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file pi_tuner_ra217.c
 *  Tuner control for SS RA217
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
//#include "demod_tuner_interface.h"
//#include "pd_i2capi.h"
#include "tuner_interface_if.h"
#include "fe_tuner_common_if.h"
#include "i2c_api.h"
#include "x_typedef.h"
#include "tunerDebug.h"
#include "x_os.h"
#include "eq_script_ra217.h"

#include "ra217_tuner.h" 
#include "ra217_tuner_i2c_MTK.h"
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
//Frequency boundary .Get from tuner spec
#define C_RA217_FREQ_DBOUND_UPPER   880*1000*1000     
#define C_RA217_FREQ_DBOUND_LOWER    42*1000*1000 
#define C_RA217_FREQ_DBOUND_UPPER_Ana     880*1000*1000
#define C_RA217_FREQ_DBOUND_LOWER_Ana    42*1000*1000


#define RA217_ANA_LO_STEP             ((UINT16) 100)  /* 10Hz */

#define C_RA217_LO_ADDRESS           0xC0 // In 8bit form
#define C_RA217_I2C_GATEWAY_ADDRESS  0xD8  // Demod address connected to silicon tuner.
#define C_RA217_I2C_GATEWAY_SUB          0x09  // Gateway sub address 
#define C_RA217_IF_FREQUENCY        ((UINT16)  4000)  /* kHz */
#define C_RA217_IF_FREQUENCY_ANA      ((UINT16)  5750)  /* kHz */
#define C_RA217_LO_DIVIDER_STEP       ((UINT32) 1000)  /* Hz */
#define C_RA217_LO_DIVIDER_STEP_ANA   ((UINT16)  1000)  /* Hz */
#define C_RA217_STR                        "RA217_Silicon_tuner"



//-----------------------------------------------------------------------------
//static  variable
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// pi  constant definition
//-----------------------------------------------------------------------------

static UINT8* pRA217EqNormal[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};
static UINT8* pRA217EqWeak[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};


//-----------------------------------------------------------------------------
//  Rb211 specific tuner information
//-----------------------------------------------------------------------------
typedef struct ra217_tuner_ctx
{
    ra217_tuner_t tuner;
    ra217_tuner_i2c_t i2c;
    BOOL fgCable;// Analog signal input, 0:Air input, 1:Cable input
} ra217_tuner_ctx;

static ra217_tuner_ctx g_ra217_tuner_ctx;


//Tuner releated ATD register
static ATD_TUNER_REL_REG_T arrAtdRelReg[]={
#if !(defined(CC_MT5365) || defined(CC_MT5395))
    {0x7D4,0x04},//PRA RF BIAS
    {0x7D5,0x00},//PRA IF BIAS 1
    {0x7D6,0x00},//PRA IF BIAS 2
    {0x7D7,0xEC},//PRA RF SCLOPE
    {0x7D8,0xF1},//PRA IF SLOPE 1
    {0x7D9,0xF1},//PRA IF SLOPE 2
    {0x7DA,0x7F},//PRA RF AGC MAXIMUM
    {0x7DB,0x7F},//PRA IF AGC MINIMUM
    {0x7DC,0x80},//PAR RF AGC MINIMUM
    {0x7DD,0x80},//PRA IF AGC MINIMUM 1
    {0x7DE,0x80},//PRA IF AGC MINIMUM 2
    {0x7DF,0X51},  //IFAGC PORIRITY
    {0x790,0x08},//DAGC DC REMOVER CONFIGURATION
    {0x794,0x30}, //DAGC Target H
    {0x7a4,0x30}, //DAGC Target L
    {0x7B2,0x03},
	  {0x7B3,0x60},
    {0x7B7,0x10},//PRA STATIC TARGET LEVEL HIGH(MSB),degrade target level for lowif tuner(legacy)
    {0x7B9,0x10},//PRA STATIC TARGET LEVEL LOW(MSB)
    {0x13b,0x80},
	{0x1b4,0x17},
	{0x782,0xA0},
    {0x00,0x00}//end flag
#else
    {0x2D4,0x04},//PRA RF BIAS
    {0x2D5,0x00},//PRA IF BIAS 1
    {0x2D6,0x00},//PRA IF BIAS 2
    {0x2D7,0xEC},//PRA RF SCLOPE
    {0x2D8,0xF1},//PRA IF SLOPE 1
    {0x2D9,0xF1},//PRA IF SLOPE 2
    {0x2DA,0x7F},//PRA RF AGC MAXIMUM
    {0x2DB,0x7F},//PRA IF AGC MINIMUM
    {0x2DC,0x80},//PAR RF AGC MINIMUM
    {0x2DD,0x80},//PRA IF AGC MINIMUM 1
    {0x2DE,0x80},//PRA IF AGC MINIMUM 2
    {0x2DF,0x51},//IFAGC/RFAGC POR
    {0x290,0x08},//DAGC DC REMOVER CONFIGURATION
    {0x294,0x30}, //DAGC Target H
    {0x2a4,0x30}, //DAGC Target L
    {0x2B2,0x03},
	{0x2B3,0x60},
    {0x2B7,0x10},//PRA STATIC TARGET LEVEL HIGH(MSB),degrade target level for lowif tuner(legacy)
    {0x2B9,0x10},//PRA STATIC TARGET LEVEL LOW(MSB)
	{0x13b,0x80},
	{0x1b4,0x17},
	{0x282,0xA0},
    {0x00,0x00}//end flag
#endif
};


//-----------------------------------------------------------------------------
/** RA217_TunerInit
 *  Tuner initialzation for RA217.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval void
 */
//-----------------------------------------------------------------------------
VOID RA217_TunerInit(
    ITUNER_CTX_T *pTCtx
    )
{
    ITUNER_CTX_T *psTunerCtx = pTCtx;
    //SPECIFIC_MEMBER_EU_CTX * pEUCtx= &(pTCtx->specific_member.eu_ctx);
    SPECIFIC_MEMBER_US_CTX *psTunerSpecificCtx=&(psTunerCtx->specific_member.us_ctx);
    // Rb211 specific
    ra217_result_t ra217_result = RA217_RESULT_OK;
    ra217_tuner_t *pTuner = &g_ra217_tuner_ctx.tuner;
    ra217_tuner_i2c_t *pI2c = &g_ra217_tuner_ctx.i2c;
	 
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));
    psTunerCtx->I2cAddress = C_RA217_LO_ADDRESS;
    psTunerCtx->u2IF_Freq = C_RA217_IF_FREQUENCY;
    psTunerCtx->u2IF_Freq_A = C_RA217_IF_FREQUENCY_ANA;//analog if frequency for ATD
    //ATD patch selection
    psTunerCtx->u1AtdPatchSwitch =0x09;
    
    psTunerCtx->fgRFAGC = FALSE;//indicate if tuner need extern RF_AGC
    psTunerCtx->fgRFTuner =FALSE;//not sawless tuner,it include BPF internal
    //psTunerCtx->u1EqIndex=0;//EQ index select
    //Tuner frequency range
    //pEUCtx->m_s4FreqBoundUpper = C_RA217_FREQ_DBOUND_UPPER;
    //pEUCtx->m_s4FreqBoundLower = C_RA217_FREQ_DBOUND_LOWER;
    //pEUCtx->m_s4AnaFreqBoundUpper = C_RA217_FREQ_DBOUND_UPPER_Ana;
    //pEUCtx->m_s4AnaFreqBoundLower = C_RA217_FREQ_DBOUND_LOWER_Ana;

    
    // AGC para
     psTunerCtx->sAtdAgcPara.AgcRfBias = 0x2F;     //CJ 091021//0x19;
     psTunerCtx->sAtdAgcPara.AgcRfMin  = 0x80;
     psTunerCtx->sAtdAgcPara.AgcIfMin  = 0xD5;      //CJ 091021//0xE8;//Yueqin's test result 090925 0xEF;According to YueQin's advice 090921
     psTunerCtx->sAtdAgcPara.AgcDrSel  = 0x0B;

    // Rb211 driver initialization
    g_ra217_tuner_ctx.fgCable = 0;

    // This silicon tuner is connected via demodulator. (I2C Gateway)
    ra217_tuner_i2c_MTK_CreateI2cGw(pI2c, C_RA217_I2C_GATEWAY_ADDRESS, C_RA217_I2C_GATEWAY_SUB);

    /* Xtal = 16MHz, Sub Address = 0xC0, has just one IF and AGC path, use IF&AGC 2 */
    ra217_tuner_Create(pTuner, 16, C_RA217_LO_ADDRESS, pI2c, RA217_TUNER_CONFIG_NVMSEL_AUTO | RA217_TUNER_CONFIG_IFAGCSEL_ALL2);

    ra217_result = ra217_tuner_Initialize(pTuner);
    if(ra217_result != RA217_RESULT_OK){
        mcDBG_MSG_L2(("ERROR: ra217_tuner_Initialize is failed. (%d)\n", ra217_result));
    }

    // init EQ script
    pRA217EqNormal[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN]  = EQRA217MN;
    pRA217EqWeak[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN]  = EQRA217Weak;

    psTunerSpecificCtx->u2LO_Step=RA217_ANA_LO_STEP;

    mcDBG_MSG_L2(("Init " C_RA217_STR "(Cust tuner)\n"));
}


//----------------------------------------------------------------------------- 
/**  RA217_TunerGetStatus
 *  Set Tuner PLL staus for RA217/RA217
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval TRUE        PLL lock
 *  @retval FALSE       PLL unlock
 */
//-----------------------------------------------------------------------------
static BOOL RA217_TunerGetStatus(ITUNER_CTX_T *pTCtx)
{
    //ITUNER_CTX_T *psTunerCtx = pTCtx;

    ra217_result_t ra217_result = RA217_RESULT_OK;
    ra217_tuner_t *pTuner = &g_ra217_tuner_ctx.tuner;
    UINT8 data = 0;
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

    /* Get DLD */
    ra217_result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x1D, &data, 1);
    if(ra217_result != RA217_RESULT_OK){
        return FALSE;
    }

    if(data & 0x02){
        return TRUE;
    }else{
        return FALSE;
    }
}

//----------------------------------------------------------------------------- 
/** RA217_TunerSetFreq
 *  Set Tuner PLL forRA217/RA217 to tune RF frequency.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @param 	param    Pointer of structure of parameters
 *  @retval  0 success
                    1  fail   out of range 
                   -1 fail  I2C error 
 */
//-----------------------------------------------------------------------------
INT16 RA217_TunerSetFreq(
    ITUNER_CTX_T *pTCtx,
    PARAM_SETFREQ_T* param
    )
{
    ra217_result_t ra217_result = RA217_RESULT_OK;
    ra217_tuner_t *pTuner = &g_ra217_tuner_ctx.tuner;

    UINT32 Freq = param->Freq; // transfer to video freq(KHz)
    UINT8 Mode = param->Modulation;

    mcDBG_MSG_L2((" %s \n",__FUNCTION__));
    pTCtx->u4RF_Freq = Freq;//Update current frequency to context

    mcDBG_MSG_L2((C_RA217_STR " Set Freq %d KHz\n",Freq));
    if (Freq < (C_RA217_FREQ_DBOUND_LOWER/1000) || 
        Freq > (C_RA217_FREQ_DBOUND_UPPER/1000))
    {
		mcDBG_MSG_ERR(("Out of range for LO!\n"));
		return 1;
    }
	
    if(Mode == MOD_ISDBT){
        mcDBG_MSG_L2(("Rb211Tuner ISDB-T SetFreq (%d kHz)\n", Freq));
        ra217_result = ra217_tuner_GPOCtrl(pTuner, 1, 1); /* GPO 1: Terr/Cable Ctrl  0: Cable, 1: Terr */
        if(ra217_result != RA217_RESULT_OK){
            return -1;
        }

	//for CXD2831 freq setting
	Freq = Freq - (UINT32)143;

        ra217_result = ra217_tuner_DigitalTune(pTuner, Freq, RA217_DTV_SYSTEM_ISDBT);
        if(ra217_result != RA217_RESULT_OK){
            return -1;
        }
    }else{
        mcDBG_MSG_L2(("Rb211Tuner Analog SetFreq (%d kHz, fgCable = %d)\n", Freq, g_ra217_tuner_ctx.fgCable));
        // Analog mode
        // Assume System-M

        // Switch input by g_ra217_tuner_ctx.fgCable flag
        ra217_result = ra217_tuner_GPOCtrl(pTuner, 1, g_ra217_tuner_ctx.fgCable ? 0 : 1); /* GPO 1: Terr/Cable Ctrl  0: Cable, 1: Terr */
        if(ra217_result != RA217_RESULT_OK){
            return -1;
        }
        // NOTE: The last argument of this function (Audio system) is not used except for RA217_ATV_AUDIO_M_DUAL_FM_STEREO (Korea).
        ra217_result = ra217_tuner_AnalogTune(pTuner, Freq, RA217_ATV_VIDEO_M, RA217_ATV_AUDIO_N_BTSC);
        if(ra217_result != RA217_RESULT_OK){
            return -1;
        }        
    }

    return 0;
}
//----------------------------------------------------------------------------- 
/** RA217_TunerGetVer
 *  Get Tuner type version
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval tuner model name
 */
//-----------------------------------------------------------------------------
static CHAR *RA217_TunerGetVer(VOID)
{
    return (C_RA217_STR);
}
//----------------------------------------------------------------------------- 
/** RA217_SetInMode
 *  Set Tuner for Cable or Air input mode
 *  @param  pTunerCtx       Pointer of pi tuner driver context.
 *  @param  bMode       Pointer of pi tuner driver context.
 *  @retval void
 */
//-----------------------------------------------------------------------------
static void  RA217_SetInMode(ITUNER_CTX_T * pTunerCtx, BOOL bMode)
{
    g_ra217_tuner_ctx.fgCable= bMode;
    mcDBG_MSG_L2(("%s, mode = %d (0: Air, 1:Cable)\n", __FUNCTION__, g_ra217_tuner_ctx.fgCable));  
}

//----------------------------------------------------------------------------- 
/** RA217_TunerOP
 * Set/Get function
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @param  eOperation       To Get/Set operation id
 *  @retval  ITUNER_OK    Operation Success
 *  @retval  ITUNER_NOT_OK    Operation fail
 *  @retval  ITUNER_NOT_SUPPORTED Operation not supported in  driver
 */
//-----------------------------------------------------------------------------
INT16 RA217_TunerOP(ITUNER_CTX_T * pTCtx, ITUNEROP_T eOperation, UINT8 SetVal,  VOID * pInOutVal){
         
    switch(eOperation){
    case itGetStatus:
        {
            BOOL * Retval=(BOOL *)pInOutVal;
            *Retval = RA217_TunerGetStatus(pTCtx);
        }
        break;
    case itGetVer:
        {
        CHAR ** RetStr=(CHAR **)pInOutVal;
        *RetStr=RA217_TunerGetVer();
        }
        break;
        
    case itSetInCableMode: 
        RA217_SetInMode(pTCtx,SetVal);
        break; 
        
    case itGetEqScriptNormal: 
        *(UINT8**)pInOutVal = pRA217EqNormal[SetVal];
        break;
        
    case itGetEqScriptWeak: 
        *(UINT8**)pInOutVal = pRA217EqWeak[SetVal];
        break;
                
    case itGetEqScriptStrongChroma: 
        *(UINT8**)pInOutVal = NULL;
        break;
        
    case itGetTunerRelReg:
        {
        *(ATD_TUNER_REL_REG_T **)pInOutVal=arrAtdRelReg;
        }
        break;


    default:
        return ITUNER_NOT_SUPPORTED;
    }

    return ITUNER_OK;
}
void ra217_tuner_trace_log_enter(const char* funcname, const char* filename, unsigned int linenum)
{
    mcDBG_MSG_L4(("RA217_ENTER : %s (%d)\n", funcname, linenum));
}
void ra217_tuner_trace_log_return(ra217_result_t result, const char* filename, unsigned int linenum)
{
    if(result != RA217_RESULT_OK){
        mcDBG_MSG_L4(("RA217_ERROR_RETURN(%d) : (line %d)\n", result, linenum));
    }else{
        mcDBG_MSG_L4(("RA217_RETURN : (line %d))\n", linenum));
    }
}

void ra217_tuner_trace_i2c_log_enter(const char* funcname, const char* filename, unsigned int linenum)
{
    ra217_tuner_trace_log_enter(funcname, filename, linenum);
}
void ra217_tuner_trace_i2c_log_return(ra217_result_t result, const char* filename, unsigned int linenum)
{
    ra217_tuner_trace_log_return(result, filename, linenum);
}
