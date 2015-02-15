/*
  NOTE: This is glue layer code of RE227 driver but not perfect
  because Sony do not have much information about MTK software.
  Please modify it to fit MTK's system.
*/

#include "sonyrf_ascot2e_re227.h"
#include "sonyrf_i2c_MTK.h"

#ifndef SONYRF_DUMMY_DEBUG // For checking driver in Sony dummy environment. Please ignore it.
#include "tuner_interface_if.h"
#include "fe_tuner_common_if.h"
#include "i2c_api.h"
#include "x_typedef.h"
#include "tunerDebug.h"
#include "x_os.h"
#include "eq_script_re227.h"
#include "x_hal_5381.h"
#else // SONYRF_DUMMY_DEBUG
#include "MTKdef.h"
#endif

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define C_RE227_LO_ADDRESS           0xC0 // In 8bit form

#ifdef CC_SONYDEMOD_CXD2817
#define C_RE227_IF_FREQUENCY_DVBT_6M         ((UINT16)  3900)  /* kHz */  //JUST for 5.81M
#define C_RE227_IF_FREQUENCY_DVBT_7M         ((UINT16)  4350)  /* kHz */  //JUST for 6.8M
#define C_RE227_IF_FREQUENCY_DVBT_8M         ((UINT16)  4850)  /* kHz */  //JUST for 7.65M
#define C_RE227_IF_FREQUENCY_DVBT2_6M        ((UINT16)  4000)  /* kHz */  //JUST for 6.8M
#define C_RE227_IF_FREQUENCY_DVBT2_7M        ((UINT16)  4200)  /* kHz */  //JUST for 6.8M
#define C_RE227_IF_FREQUENCY_DVBT2_8M        ((UINT16)  4700)  /* kHz */  //JUST for 7.65M
#define C_RE227_IF_FREQUENCY_DVBC        	 ((UINT16)  4950)  /* kHz */  //JUST for 7.95M
#define C_RE227_IF_FREQUENCY_DTMB            ((UINT16)  5100)  /* kHz */  //JUST for 7.95M
#else // CC_SONYDEMOD_CXD2817
#define C_RE227_IF_FREQUENCY_DVBT_6M         ((UINT16)  3600)  /* kHz */  //JUST for 5.81M
#define C_RE227_IF_FREQUENCY_DVBT_7M         ((UINT16)  4200)  /* kHz */  //JUST for 6.8M
#define C_RE227_IF_FREQUENCY_DVBT_8M         ((UINT16)  4800)  /* kHz */  //JUST for 7.65M
#define C_RE227_IF_FREQUENCY_DVBT2_6M        ((UINT16)  3600)  /* kHz */  //JUST for 6.8M
#define C_RE227_IF_FREQUENCY_DVBT2_7M        ((UINT16)  4200)  /* kHz */  //JUST for 6.8M
#define C_RE227_IF_FREQUENCY_DVBT2_8M        ((UINT16)  4800)  /* kHz */  //JUST for 7.65M
#define C_RE227_IF_FREQUENCY_DVBC        	 ((UINT16)  4900)  /* kHz */  //JUST for 7.95M
#define C_RE227_IF_FREQUENCY_DTMB            ((UINT16)  5100)  /* kHz */  //JUST for 7.95M
#endif // CC_SONYDEMOD_CXD2817

#define C_RE227_IF_FREQUENCY_PAL_BG          ((UINT16)  7300)  /* kHz */  //JUST for 7.95M
#define C_RE227_IF_FREQUENCY_PAL_DK          ((UINT16)  7850)  /* kHz */  //JUST for 7.95M
#define C_RE227_IF_FREQUENCY_PAL_I           ((UINT16)  7850)  /* kHz */  //JUST for 7.95M
#define C_RE227_IF_FREQUENCY_SEMCAL_L        ((UINT16)  7850)  /* kHz */  //JUST for 7.95M
#define C_RE227_IF_FREQUENCY_SEMCAL_L1       ((UINT16)  2200)  /* kHz */  //JUST for 7.95M
#define C_RE227_IF_FREQUENCY_NTSC			 ((UINT16)  5750)  /* kHz */  //JUST for 7.95M

#define C_RE227_SAW_BW              SAW_BW_8M 

//Frequency boundary .Get from tuner spec
#define C_RE227_FREQ_DBOUND_UPPER      870*1000*1000
#define C_RE227_FREQ_DBOUND_LOWER        47*1000*1000
#define C_RE227_FREQ_DBOUND_UPPER_Ana  875*1000*1000
#define C_RE227_FREQ_DBOUND_LOWER_Ana    42*1000*1000

#define C_RE227_CHANNEL_SCAN_JUMP_SMALL_STEP ((UINT16) 250)
#define C_RE227_CHANNEL_SCAN_JUMP_MIDDLE_STEP_UP ((UINT16) 1500)
#define C_RE227_CHANNEL_SCAN_JUMP_MIDDLE_STEP_DOWN ((UINT16) 500)

#define C_SONY_RE227_STR		"SONY_RE227 ES2 2010-01-01\n 2013-01-14\n v1.3"

#ifndef SONYRF_DUMMY_DEBUG

// DVB-T+DVB-C D-only Tuner
#define ccTUNER_LOCK_CONF_THRESHOLD     0
#define cALC_ADC_COMM_VOLT              1.2
#define cALC_ADC_INPUT_SWING            2  //Volt
#define cALC_ADC_BIAS                   (cALC_ADC_COMM_VOLT - cALC_ADC_INPUT_SWING/2)

#define C_RE227_AGC_IF_SLP            -0.1407
#define C_RE227_AGC_IF_INT           0.4375    
#define C_RE227_AGC_IF_MAX           0.998  // 0.5 will overflow  //0v
#define C_RE227_AGC_IF_MIN          -0.1094  
#define C_RE227_AGC_IF_SLP_SGN  (INT8)((C_RE227_AGC_IF_SLP>0)?1:-1) 
#define C_RE227_AGC_IF_LVL_MAX      0.4774    //-90dBm    127/256
#define C_RE227_AGC_IF_LVL_MIN      0.08203125   //-60dBm  21/256


#define C_RE227_MAX_IF_GAIN_SET       -0.12   
#define C_RE227_MIN_IF_GAIN_SET       -0.38   
#define C_RE227_MAX_RF_GAIN_SET        cALC_ADC_BIAS *2   // Volt
#define C_RE227_MIN_RF_GAIN_SET        cALC_ADC_BIAS *2   // Volt
#define C_RE227_MAX_IF_GAIN_POW       -100    // dBm
#define C_RE227_MIN_IF_GAIN_POW		-50     // dBm
#define C_RE227_MAX_RF_GAIN_POW		 C_RE227_MIN_IF_GAIN_POW
#define C_RE227_MIN_RF_GAIN_POW		-20     // dBm
#define C_RE227_POWER_CALIBRATE        26     // dBm
//#define C_RE227_SIGNAL_LVL_TH          6  //liuqu,20080926,no use for nimtool

#define C_RE227_MAX_IF_GAIN_SET       -0.12   
#define C_RE227_MIN_IF_GAIN_SET       -0.38   
#define C_RE227_MAX_RF_GAIN_SET        cALC_ADC_BIAS *2   // Volt
#define C_RE227_MIN_RF_GAIN_SET        cALC_ADC_BIAS *2   // Volt
#define C_RE227_MAX_IF_GAIN_POW       -100    // dBm
#define C_RE227_MIN_IF_GAIN_POW		-50     // dBm
#define C_RE227_MAX_RF_GAIN_POW		 C_RE227_MIN_IF_GAIN_POW
#define C_RE227_MIN_RF_GAIN_POW		-20     // dBm
#define C_RE227_POWER_CALIBRATE        26     // dBm

#define C_RE227_AGC_COEF			((UINT8)   0xBD)
//#define C_RE227_DEMOD_INPUT_POWER	cIF_TARGET_LEVEL_0CCC_0D36

#define C_UINT8_RE227_AGC_IF_SLP         (INT8)(C_RE227_AGC_IF_SLP *64 )  // MT5135(S(1, -6)), -0.125*64 = -8 (0xF8), -8/64 = -1/8
#define C_UINT8_RE227_AGC_IF_INT         (INT8)(C_RE227_AGC_IF_INT *16 ) // MT5135(S(3, -4))  0.5*16 = 8, 8/16 =0.5
#define C_UINT8_RE227_AGC_IF_MAX         (INT8)(C_RE227_AGC_IF_MAX *128)    //0.499*256 = 0x7F
#define C_UINT8_RE227_AGC_IF_MIN         (INT8)(C_RE227_AGC_IF_MIN *128)    //0
#define C_UINT8_RE227_AGC_IF_LVL_MAX        (INT8)(C_RE227_AGC_IF_LVL_MAX *256)
#define C_UINT8_RE227_AGC_IF_LVL_MIN         (INT8)(C_RE227_AGC_IF_LVL_MIN *256)

#define C_UINT8_RE227_MAX_IF_GAIN_SET	(INT8)(C_RE227_MAX_IF_GAIN_SET *256)
#define C_UINT8_RE227_MIN_IF_GAIN_SET	(INT8)(C_RE227_MIN_IF_GAIN_SET *256)
#define C_UINT8_RE227_MAX_RF_GAIN_SET	(INT8)((C_RE227_MAX_RF_GAIN_SET /2 -cALC_ADC_BIAS) /cALC_ADC_INPUT_SWING *256)
#define C_UINT8_RE227_MIN_RF_GAIN_SET	(INT8)((C_RE227_MIN_RF_GAIN_SET /2 -cALC_ADC_BIAS) /cALC_ADC_INPUT_SWING *256)
#define C_UINT8_RE227_IF_GAIN_RANGE      (INT8)(C_RE227_MIN_IF_GAIN_POW - C_RE227_MAX_IF_GAIN_POW)
#define C_UINT8_RE227_RF_GAIN_RANGE      (INT8)(C_RE227_MIN_RF_GAIN_POW - C_RE227_MAX_RF_GAIN_POW)

static UINT8* pRE227EqNormal[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};
static UINT8* pRE227EqWeak[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};
enum
   {
	   SAW_BW_6M = 0,
	   SAW_BW_7M,
	   SAW_BW_8M,
	   SAW_BW_5M
   };
#define PRA_TARGET_H_INDEX 0
#define PRA_TARGET_L_INDEX 1
#define SLD_DAGC_00_INDEX 2
#define DAGC_TARGET_LEVEL_H_INDEX 4  
#define DAGC_TARGET_LEVEL_L_INDEX 5
#define PRA_LOOP_CONF_INDEX  6

#define AD_R_setting 0      // 1:have IF attenuator, 0:No IF attenuator

//Tuner releated ATD register
static ATD_TUNER_REL_REG_T arrAtdRelReg[]={
#if !(defined(CC_MT5365) || defined(CC_MT5395))
	 {0x7b7,0x10}, //PRA target H
	 {0x7b9,0x10}, //PRA target L
	 {0x790,0x08}, //DAGC setting
	 {0x7df,0x51}, //AGC polarity inverse.RF_AGC	disable.IF_AGC polarity inverse
	 {0x794,0x2c}, //DAGC Target level H
	 {0x7a4,0x2c}, //DAGC Target level L
	 {0x7ca,0x88}, //PRA speed
	 {0x7d4,0x04},//RF_BIAS  /*wenming 11-12-07: new PRA parameter*/
	 {0x7d5,0x04},//IF_BIAS1
	 {0x7d6,0x04},//IF_BIAS2
	 {0x7d7,0xec},//RF_SLOPE
	 {0x7d8,0xf4},//IF_SLOPE1
	 {0x7d9,0xf4},//IF_SLOPE2
	 {0x7da,0x7f},//RF Max
	 {0x7db,0x7f},//IF Max
	 {0x7dc,0x80},//RF Min
	 {0x7dd,0x80},//IF Min 1
	 {0x7de,0x80},//IF Min 2
	 {0x00,0x00}//end flag
#else
	{0x2B7,0x10},//PRA STATIC TARGET LEVEL HIGH(MSB),degrade target level for lowif tuner(legacy)
	{0x2B9,0x10},//PRA STATIC TARGET LEVEL LOW(MSB)
	{0x290,0x08},//DAGC DC REMOVER CONFIGURATION
	{0x2df,0x51},
	{0x294,0x2c}, //DAGC Target H
    {0x2a4,0x2c}, //DAGC Target L
    {0x2ca,0x88}, //PRA speed
    {0x2D4,0x04},//PRA RF BIAS
    {0x2D5,0x04},//PRA IF BIAS 1
    {0x2D6,0x04},//PRA IF BIAS 2
    {0x2D7,0xEC},//PRA RF SCLOPE
    {0x2D8,0xF4},//PRA IF SLOPE 1
    {0x2D9,0xF4},//PRA IF SLOPE 2
    {0x2DA,0x7F},//PRA RF AGC MAXIMUM
    {0x2DB,0x7F},//PRA IF AGC MINIMUM
    {0x2DC,0x80},//PAR RF AGC MINIMUM
    {0x2DD,0x80},//PRA IF AGC MINIMUM 1
    {0x2DE,0x80},//PRA IF AGC MINIMUM 2   
    {0x00,0x00}//end flag
#endif
};
#endif // SONYRF_DUMMY_DEBUG

/* Tuner control data type. */
typedef struct sonyrf_tuner_ctx
{
    sonyrf_ascot2e_t tuner;
    sonyrf_i2c_t     i2c;
} sonyrf_tuner_ctx;

/* Tuner control data instance. */
static sonyrf_tuner_ctx g_sonyrf_tuner_ctx;
#if defined(CUST_LCCT_DRV)
UINT8 fgTunerNormal_GW = 0; //wenming 2012-7-4:flag for tuner Normal or Gateway access, 0 for Normal, 1 for Gateway
UINT8 fgTunerI2C_SPI = 0;   //wenming 2012-7-4:flag for tuner I2C or SPI access, 0 for I2C, 1 for SPI

#elif defined(CC_MT5135)
UINT8 fgTunerNormal_GW = 0; //wenming 2012-7-4:flag for tuner Normal or Gateway access, 0 for Normal, 1 for Gateway
UINT8 fgTunerI2C_SPI = 1;   //wenming 2012-7-4:flag for tuner I2C or SPI access, 0 for I2C, 1 for SPI

#else
UINT8 fgTunerNormal_GW = 0; //wenming 2012-7-4:flag for tuner Normal or Gateway access, 0 for Normal, 1 for Gateway
UINT8 fgTunerI2C_SPI = 0;   //wenming 2012-7-4:flag for tuner I2C or SPI access, 0 for I2C, 1 for SPI

#endif

//-----------------------------------------------------------------------------
/** RE227_TunerInit
 *  Tuner initialzation for RE227.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval void
 */
//-----------------------------------------------------------------------------
VOID RE227_TunerInit(
    ITUNER_CTX_T *pTCtx
    )
{
    sonyrf_result_t sonyrf_result = SONYRF_RESULT_OK;
    sonyrf_ascot2e_t *pTuner = &g_sonyrf_tuner_ctx.tuner;
    sonyrf_i2c_t *pI2c = &g_sonyrf_tuner_ctx.i2c;

#ifndef SONYRF_DUMMY_DEBUG

    SPECIFIC_MEMBER_US_CTX * pUSCtx= &(pTCtx->specific_member.us_ctx);
    SPECIFIC_MEMBER_EU_CTX * pEUCtx= &(pTCtx->specific_member.eu_ctx);
	ITUNER_CTX_T *psTunerCtx = pTCtx;

   

	psTunerCtx->I2cAddress =C_RE227_LO_ADDRESS;
	psTunerCtx->u2IF_Freq = C_RE227_IF_FREQUENCY_PAL_BG;
    psTunerCtx->u2IF_Freq_A = C_RE227_IF_FREQUENCY_DVBC;//analog if frequency for ATD
    
	
    //ATD patch selection
    psTunerCtx->u1AtdPatchSwitch =0x09;
    psTunerCtx->fgRFAGC = FALSE;//indicate if tuner need extern RF_AGC
    psTunerCtx->fgRFTuner =FALSE;//not sawless tuner,it include BPF internal
    // AGC para
	 psTunerCtx->sAtdAgcPara.AgcRfBias = 0x2F;   
	 psTunerCtx->sAtdAgcPara.AgcRfMin  = 0x80;
	 psTunerCtx->sAtdAgcPara.AgcIfMin  = 0xD5;      
	 psTunerCtx->sAtdAgcPara.AgcDrSel  = 0x0B;

	pEUCtx->m_SAW_BW  = C_RE227_SAW_BW;
   // pEUCtx->m_Ana_Top = C_RE215_TOP_SET_ANA;
	 	//Tuner frequency range
    pEUCtx->m_s4FreqBoundUpper = C_RE227_FREQ_DBOUND_UPPER;
    pEUCtx->m_s4FreqBoundLower = C_RE227_FREQ_DBOUND_LOWER;
    pEUCtx->m_s4AnaFreqBoundUpper = C_RE227_FREQ_DBOUND_UPPER_Ana;
    pEUCtx->m_s4AnaFreqBoundLower = C_RE227_FREQ_DBOUND_LOWER_Ana;

	pEUCtx->m_Small_Step = C_RE227_CHANNEL_SCAN_JUMP_SMALL_STEP;
    pEUCtx->m_Middle_Step_Up = C_RE227_CHANNEL_SCAN_JUMP_MIDDLE_STEP_UP;
    pEUCtx->m_Middle_Step_Down = C_RE227_CHANNEL_SCAN_JUMP_MIDDLE_STEP_DOWN;

	pEUCtx->m_aucPara[2] = (UINT8)C_UINT8_RE227_AGC_IF_SLP;
    pEUCtx->m_aucPara[3] = (UINT8)C_UINT8_RE227_AGC_IF_INT;
    pEUCtx->m_aucPara[4] = (UINT8)C_UINT8_RE227_AGC_IF_MAX;
    pEUCtx->m_aucPara[5] = (UINT8)0xf2;//C_UINT8_RE227_AGC_IF_MIN;
    
    pEUCtx->m_aucPara[6] = C_RE227_AGC_COEF;
   // pEUCtx->m_aucPara[7] = C_RE227_DEMOD_INPUT_POWER;
    
    pEUCtx->m_aucPara[ 8] = (UINT8)C_UINT8_RE227_MAX_IF_GAIN_SET;
    pEUCtx->m_aucPara[ 9] = (UINT8)C_UINT8_RE227_MIN_IF_GAIN_SET;
    pEUCtx->m_aucPara[10] = (UINT8)C_UINT8_RE227_MAX_RF_GAIN_SET;
    pEUCtx->m_aucPara[11] = (UINT8)C_UINT8_RE227_MIN_RF_GAIN_SET;
    pEUCtx->m_aucPara[12] = (UINT8)C_UINT8_RE227_IF_GAIN_RANGE;
    pEUCtx->m_aucPara[13] = (UINT8)C_UINT8_RE227_RF_GAIN_RANGE;
    pEUCtx->m_aucPara[14] = C_RE227_POWER_CALIBRATE;
	//IF MAX setting!
    pEUCtx->m_aucPara[21] = 0x7F; //xiangfu 20100526

    pUSCtx->u2LO_Step = 100; //*10HZ
	// init EQ script
	#if defined(CC_MT5890)
	if(IS_IC_5861())
	{
	pRE227EqNormal[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN]  = EQRE227M_5861;
	pRE227EqNormal[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]  = EQRE227BG_5861;
	pRE227EqNormal[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]  = EQRE227DK_5861;
	pRE227EqNormal[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]  = EQRE227I_5861;
	pRE227EqNormal[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQRE227L_5861;
	pRE227EqNormal[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN]  = EQRE227L1_5861;
	
	pRE227EqWeak[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN]  = EQRE227MWeak_5861;
	pRE227EqWeak[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]  = EQRE227BGWeak_5861;
	pRE227EqWeak[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]  = EQRE227DKWeak_5861;
	pRE227EqWeak[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]  = EQRE227IWeak_5861;
	pRE227EqWeak[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQRE227LWeak_5861;
	pRE227EqWeak[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN]  = EQRE227L1Weak_5861;	
	}
	else
	{
	pRE227EqNormal[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN]  = EQRE227M;
	pRE227EqNormal[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]  = EQRE227BG;
	pRE227EqNormal[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]  = EQRE227DK;
	pRE227EqNormal[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]  = EQRE227I;
	pRE227EqNormal[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQRE227L;
	pRE227EqNormal[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN]  = EQRE227L1;
	
	pRE227EqWeak[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN]  = EQRE227MWeak;
	pRE227EqWeak[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]  = EQRE227BGWeak;
	pRE227EqWeak[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]  = EQRE227DKWeak;
	pRE227EqWeak[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]  = EQRE227IWeak;
	pRE227EqWeak[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQRE227LWeak;
	pRE227EqWeak[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN]  = EQRE227L1Weak;
    }
	#else
	pRE227EqNormal[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN]  = EQRE227M;
	pRE227EqNormal[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]  = EQRE227BG;
	pRE227EqNormal[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]  = EQRE227DK;
	pRE227EqNormal[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]  = EQRE227I;
	pRE227EqNormal[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQRE227L;
	pRE227EqNormal[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN]  = EQRE227L1;
	
	pRE227EqWeak[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN]  = EQRE227MWeak;
	pRE227EqWeak[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]  = EQRE227BGWeak;
	pRE227EqWeak[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]  = EQRE227DKWeak;
	pRE227EqWeak[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]  = EQRE227IWeak;
	pRE227EqWeak[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQRE227LWeak;
	pRE227EqWeak[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN]  = EQRE227L1Weak;
	#endif
#endif // SONYRF_DUMMY_DEBUG

    // I2C struct initialization
    // NOTE: Please change following line depend on RF tuner I2C.
    if(0 == fgTunerNormal_GW)
	{
		sonyrf_i2c_MTK_CreateI2c(pI2c); // Normal (via I2C repeater)
	}
	else
	{
		sonyrf_i2c_MTK_CreateI2cGw(pI2c, 0xD8, 0x09); // I2C gateway
	}
 		
    mcDBG_MSG_L1((" %s \n", __FUNCTION__));
    // Tuner struct initialization
    sonyrf_ascot2e_re227_Create(pTuner, pI2c);

    sonyrf_result = sonyrf_ascot2e_re227_Initialize(pTuner);
    if(sonyrf_result != SONYRF_RESULT_OK){
        mcDBG_MSG_L2(("RE227Tuner: sonyrf_ascot2e_re227_Initialize is failed. (%d)\n", sonyrf_result));
    }
}

//----------------------------------------------------------------------------- 
/**  RE227_TunerGetStatus
 *  Set Tuner PLL staus for RE227
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval TRUE        PLL lock
 *  @retval FALSE       PLL unlock
 */
//-----------------------------------------------------------------------------
static BOOL RE227_TunerGetStatus(ITUNER_CTX_T *pTCtx)
{
    sonyrf_result_t sonyrf_result = SONYRF_RESULT_OK;
    sonyrf_ascot2e_t *pTuner = &g_sonyrf_tuner_ctx.tuner;

    UINT8 data = 0;
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

    /* Get DLD */
    sonyrf_result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x1D, &data, 1);
    if(sonyrf_result != SONYRF_RESULT_OK){
        return FALSE;
    }

    if(data & 0x02){
        return TRUE;
    }else{
        return FALSE;
    }
}

//----------------------------------------------------------------------------- 
/** RE227_TunerSetFreq
 *  Set Tuner PLL for RE227 to tune RF frequency.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @param 	param    Pointer of structure of parameters
 *  @retval  0 success
                    1  fail   out of range 
                   -1  fail  I2C error 
 */
//-----------------------------------------------------------------------------
INT16 RE227_TunerSetFreq(
    ITUNER_CTX_T *pTCtx,
    PARAM_SETFREQ_T* param
    )
{
    sonyrf_result_t sonyrf_result = SONYRF_RESULT_OK;
    sonyrf_ascot2e_t *pTuner = &g_sonyrf_tuner_ctx.tuner;
    sonyrf_ascot2e_tv_system_t system = SONYRF_ASCOT2E_TV_SYSTEM_UNKNOWN;

    UINT32 Freq = param->Freq; // transfer to video freq(KHz)
    UINT8 Mode = param->Modulation;
    UINT8 bandWidth = pTCtx->specific_member.eu_ctx.m_SAW_BW;

    mcDBG_MSG_L4((" %s \n", __FUNCTION__));
    pTCtx->u4RF_Freq = Freq; // Update current frequency to context

    mcDBG_MSG_L2(("RE227Tuner: Set Freq %d KHz\n",Freq));
    if (Freq < (C_RE227_FREQ_DBOUND_LOWER/1000) || 
        Freq > (C_RE227_FREQ_DBOUND_UPPER/1000))
    {
        mcDBG_MSG_L4(("RE227Tuner: Out of range for LO!\n"));
        //return 1;
    }

	if(1 == fgTunerNormal_GW)
	{
		mcDBG_MSG_L4(("RE227Tuner fgTunerNormal_GW: Now using gateway I2C\n"));
	}
	else
	{
		mcDBG_MSG_L4(("RE227Tuner fgTunerNormal_GW: Now using normal I2C\n"));
	}

	if(1 == fgTunerI2C_SPI)
	{
		mcDBG_MSG_L4(("RE227Tuner fgTunerI2C_SPI: Now using SPI\n"));
	}
	else
	{
		mcDBG_MSG_L4(("RE227Tuner fgTunerI2C_SPI: Now using I2C\n"));
	}

    switch(Mode){
    /* Digital */
    case MOD_DVBT:
        switch(bandWidth){
        case (UINT8)SAW_BW_6M:
            system = SONYRF_ASCOT2E_DTV_DVBT_6;
			pTCtx->u2IF_Freq=C_RE227_IF_FREQUENCY_DVBT_6M;
            break;
        case (UINT8)SAW_BW_7M:
            system = SONYRF_ASCOT2E_DTV_DVBT_7;
			pTCtx->u2IF_Freq=C_RE227_IF_FREQUENCY_DVBT_7M;
            break;
        case (UINT8)SAW_BW_8M:
            system = SONYRF_ASCOT2E_DTV_DVBT_8;
			pTCtx->u2IF_Freq=C_RE227_IF_FREQUENCY_DVBT_8M;
            break;
        }
        break;
    case MOD_DVBT2:
        switch(bandWidth){
        case (UINT8)SAW_BW_6M:
            system = SONYRF_ASCOT2E_DTV_DVBT2_6;
			pTCtx->u2IF_Freq=C_RE227_IF_FREQUENCY_DVBT2_6M;
            break;
        case (UINT8)SAW_BW_7M:
            system = SONYRF_ASCOT2E_DTV_DVBT2_7;
			pTCtx->u2IF_Freq=C_RE227_IF_FREQUENCY_DVBT2_7M;
            break;
        case (UINT8)SAW_BW_8M:
            system = SONYRF_ASCOT2E_DTV_DVBT2_8;
			pTCtx->u2IF_Freq=C_RE227_IF_FREQUENCY_DVBT2_8M;
            break;
        }
        break;
    case MOD_DVBC:
        system = SONYRF_ASCOT2E_DTV_DVBC;
		pTCtx->u2IF_Freq=C_RE227_IF_FREQUENCY_DVBC;
        break;
	case MOD_DTMB:
        system = SONYRF_ASCOT2E_DTV_DTMB;
        pTCtx->u2IF_Freq=C_RE227_IF_FREQUENCY_DTMB;
        break;
    
    /* Analog */
    case MOD_PAL_BG:
        system = SONYRF_ASCOT2E_ATV_BG;
		pTCtx->u2IF_Freq_A=C_RE227_IF_FREQUENCY_PAL_BG;
        break;
    case MOD_PAL_I:
        system = SONYRF_ASCOT2E_ATV_I;
		pTCtx->u2IF_Freq_A=C_RE227_IF_FREQUENCY_PAL_I;
        break;
    case MOD_PAL_DK:
        system = SONYRF_ASCOT2E_ATV_DK;
		pTCtx->u2IF_Freq_A=C_RE227_IF_FREQUENCY_PAL_DK;
        break;
    case MOD_SECAM_L:
        system = SONYRF_ASCOT2E_ATV_L;
		pTCtx->u2IF_Freq_A=C_RE227_IF_FREQUENCY_SEMCAL_L;
        break;
    case MOD_SECAM_L1:
        system = SONYRF_ASCOT2E_ATV_L_DASH;
		pTCtx->u2IF_Freq_A=C_RE227_IF_FREQUENCY_SEMCAL_L1;
        break;
    case MOD_NTSC_M:
        system = SONYRF_ASCOT2E_ATV_MN_SAP;
		pTCtx->u2IF_Freq_A=C_RE227_IF_FREQUENCY_NTSC;
        break;
    default:
       // mcDBG_MSG_ERR(("Unknown MOD_XXX type! \n"));
       // return 1;
       break;
    }

    sonyrf_result = sonyrf_ascot2e_re227_Tune(pTuner, Freq, system);
    if(sonyrf_result != SONYRF_RESULT_OK){
        return -1;
    }

    return 0;
}

//----------------------------------------------------------------------------- 
/** RE227_TunerGetVer
 *  Get Tuner type version
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval tuner model name
 */
//-----------------------------------------------------------------------------
static CHAR *RE227_TunerGetVer(VOID)
{
    return (C_SONY_RE227_STR);
}

//----------------------------------------------------------------------------- 
/** RE227_SetSawBw
 *  Set Tuner for Cable or Air input mode
 *  @param  pTunerCtx       Pointer of pi tuner driver context.
 *  @param  sawbw           Bandwidth (SAW_BW_XM)
 *  @retval void
 */
//-----------------------------------------------------------------------------
static void  RE227_SetSawBw(ITUNER_CTX_T * pTunerCtx, UINT8 sawbw)
{
    pTunerCtx->specific_member.eu_ctx.m_SAW_BW = sawbw;

    mcDBG_MSG_L2(("RE227Tuner: Saw BW = %d\n", sawbw));
}

//----------------------------------------------------------------------------- 
/** RE227_GetRSSI
 *  Get RSSI value in dB x 100
 *  @retval INT32 RSSI value
 */
//-----------------------------------------------------------------------------
static INT32 RE227_GetRSSI(VOID)
{
    sonyrf_ascot2e_t *pTuner = &g_sonyrf_tuner_ctx.tuner;
    sonyrf_result_t sonyrf_result = SONYRF_RESULT_OK;
    INT16 rssi_x100 = 0;

    sonyrf_result = sonyrf_ascot2e_re227_RSSI(pTuner, &rssi_x100);
    if(sonyrf_result != SONYRF_RESULT_OK){
        rssi_x100 = 0;
    }
    mcDBG_MSG_L2(("RE227Tuner: RSSI (dB x 100) = %d\n", rssi_x100));

    return rssi_x100;
}

#ifndef SONYRF_DUMMY_DEBUG

static void RE227_SetSSICondition(ITUNER_CTX_T* pTunerCtx, VOID * pInOutVal)
{
    pTunerCtx->sSSIPara.i2Mod = *((INT16*) pInOutVal);
    pTunerCtx->sSSIPara.i2CR = *((INT16*) pInOutVal+1);
    pTunerCtx->sSSIPara.i2Ssi_RF= *((INT16 *) pInOutVal+2);
    pTunerCtx->sSSIPara.i2Ssi_IF= *((INT16 *) pInOutVal+3);
    pTunerCtx->sSSIPara.i2Ssi_BB= *((INT16 *) pInOutVal+4);
}

//----------------------------------------------------------------------------- 
/** RE227_GetSSIIndex
 *  Get SSI index
 *  @param fgDVBTC: 0 for get DVBT SSI, 1 for get DVBC SSI
 *  @param 
 *  @retval UINT32 RSSI index
 */
//-----------------------------------------------------------------------------
static UINT32 RE227_GetSSIIndex(UINT8 fgDVBTC,ITUNER_CTX_T* pTunerCtx) 
{
	static const INT16 DVBC_Ref[5] = 
	{
		-82, -79, -76, -73, -70
	};

	static const INT16 DVBT_Ref[3][5]=
	{
		/*CR 1/2   2/3   3/4   5/6   7/8 */        /*Modulation*/
        { -93, -91, -90, -89, -88 },    /*  QPSK */
        { -87, -85, -84, -83, -82 },    /* 16QAM */
        { -82, -80, -78, -77, -76 }     /* 64QAM */
	};
	
    INT32 i4Power_ref = 0;
    INT32 i4Power_rec = 0;
    INT32 i4Power_rel = 0;
    UINT32 u4SSI_Indicator = 0;
	UINT8 ucQamModeUnknown = 3;
	UINT8 ucQamMode = 0;
	UINT8 ucCodeRate = 0;
    SPECIFIC_MEMBER_EU_CTX * pCtx= &(pTunerCtx->specific_member.eu_ctx);
    pCtx->m_SigLvScan = 0;
	ucQamMode = pTunerCtx->sSSIPara.i2Mod;
	ucCodeRate = pTunerCtx->sSSIPara.i2CR;
	
	if(0 == fgDVBTC)
	{
		mcDBG_MSG_L2(("RE227_GETSSI_DVBT: Mod=%d, Code Rate=%d",ucQamMode,ucCodeRate));
		ucQamModeUnknown = 3;
	}
	else
	{
		mcDBG_MSG_L2(("RE227_GETSSI_DVBC: Mod=%d",ucQamMode));
		ucQamModeUnknown = 5; 
	}
	if(ucQamMode == ucQamModeUnknown)
	{
		mcDBG_MSG_L1(("QAM mode unknown!\n"));
		return u4SSI_Indicator;
	}
	else
	{
		if(0 == fgDVBTC)
		{
			i4Power_ref = DVBT_Ref[ucQamMode][ucCodeRate];
		}
		else
		{
			i4Power_ref = DVBC_Ref[ucQamMode];
		}	
	}
	i4Power_rec = (RE227_GetRSSI()-50)/100;  //*100->*1
    i4Power_rel = i4Power_rec - i4Power_ref;
	
	mcDBG_MSG_L2(("RE227_GETSSI: Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel));
    if(i4Power_rel < -15)
    {
        u4SSI_Indicator = 0;
    }
    else if((i4Power_rel >= -15) && (i4Power_rel < 0))
    {
        u4SSI_Indicator = 2*(i4Power_rel+15)/3;
    }
    else if((i4Power_rel >= 0) && (i4Power_rel < 20))
    {
        u4SSI_Indicator = 4*i4Power_rel+10;
    }
    else if((i4Power_rel >= 20) && (i4Power_rel < 35))
    {
        u4SSI_Indicator = 2*(i4Power_rel-20)/3+90;
    }
    else if(i4Power_rel >= 35)
    {
        u4SSI_Indicator = 100;
    }
    pCtx->m_SigLvScan = (UINT8) u4SSI_Indicator;
    return u4SSI_Indicator;
}

#endif // SONYRF_DUMMY_DEBUG

//----------------------------------------------------------------------------- 
/** RE227_TunerOP
 * Set/Get function
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @param  eOperation       To Get/Set operation id
 *  @retval  ITUNER_OK    Operation Success
 *  @retval  ITUNER_NOT_OK    Operation fail
 *  @retval  ITUNER_NOT_SUPPORTED Operation not supported in  driver
 */
//-----------------------------------------------------------------------------
INT16 RE227_TunerOP(ITUNER_CTX_T * pTCtx, ITUNEROP_T eOperation, UINT8 SetVal,  VOID * pInOutVal)
{
    switch(eOperation){
    case itGetStatus:
        {
            BOOL * Retval=(BOOL *)pInOutVal;
            *Retval = RE227_TunerGetStatus(pTCtx);
        }
        break;
    case itGetVer:
        {
            CHAR ** RetStr=(CHAR **)pInOutVal;
            *RetStr = RE227_TunerGetVer();
        }
        break;
    case itSetSawBw:
        RE227_SetSawBw(pTCtx, SetVal);
        break;

#ifndef SONYRF_DUMMY_DEBUG

    case itGetEqScriptNormal: 
        *(UINT8**)pInOutVal = pRE227EqNormal[SetVal];
        break;
        
    case itGetEqScriptWeak: 
        *(UINT8**)pInOutVal = pRE227EqWeak[SetVal];
        break;
    case itGetTunerRelReg:
    {
		UINT8 subSysId = SetVal;
	    UINT8 u1PRATar,u1Dagc,u1DagcTarH,u1DagcTarL,u1PRAConf;

	    u1PRATar =0x10;
	    u1DagcTarH=0x2c;
	    u1DagcTarL=0x2c;
	    u1PRAConf=0x88;
		u1Dagc=0x08;
	    switch(subSysId)
	    {
	    case MOD_SECAM_L:
	    case MOD_SECAM_L1:
	        u1PRATar =0x05;
	        u1Dagc=0x88;
	        u1DagcTarH=0x2a;
	        u1DagcTarL=0x2a;
	        u1PRAConf=0xBB;
	        break;
	    case MOD_NTSC_M:
	        u1PRATar =0x10;
	        u1Dagc=0x08;
	        u1DagcTarH=0x2a;
	        u1DagcTarL=0x2a;
	        break;
		case MOD_PAL_I:
			u1PRATar =0x10;
	        u1Dagc=0x08;
	        u1DagcTarH=0x2a;
	        u1DagcTarL=0x2a;
			break;
	    default://PAL
	      
	        break;
	    }
	    //PRA target	
	    arrAtdRelReg[PRA_TARGET_H_INDEX].u1RegVal=u1PRATar;
	    arrAtdRelReg[PRA_TARGET_L_INDEX].u1RegVal=u1PRATar;
	    arrAtdRelReg[SLD_DAGC_00_INDEX].u1RegVal=u1Dagc;
	    arrAtdRelReg[DAGC_TARGET_LEVEL_H_INDEX].u1RegVal=u1DagcTarH;
	    arrAtdRelReg[DAGC_TARGET_LEVEL_L_INDEX].u1RegVal=u1DagcTarL;
	    arrAtdRelReg[PRA_LOOP_CONF_INDEX].u1RegVal=u1PRAConf;

	    *(ATD_TUNER_REL_REG_T **)pInOutVal=arrAtdRelReg; 
		}
    break;

	case itSetSSICond:
		{
			RE227_SetSSICondition(pTCtx, pInOutVal);	
	    }
		break;
	case itGetSSIIndex:
        {
            UINT32 * Retval = (UINT32*)pInOutVal;
            *Retval = RE227_GetSSIIndex(SetVal, pTCtx);
        }
        break;
#endif // SONYRF_DUMMY_DEBUG
	case itGetRSSI:
		{
            INT32 * Retval = (INT32*)pInOutVal;
            *Retval = RE227_GetRSSI();
        }
		break;
	case itSetTrapThresold:
        {
            UINT32 threshold = *(UINT32*)pInOutVal;
            if(SetVal == 0){ // Digital
                // Change digital threshold
                g_TrapThresholdFrequencyDigital = threshold;
                mcDBG_MSG_L2(("RE227Tuner: Trap threshold (digital) = %d\n", threshold));
            }else{ // Analog
                // Change analog threshold
                g_TrapThresholdFrequencyAnalog = threshold;
                mcDBG_MSG_L2(("RE227Tuner: Trap threshold (analog) = %d\n", threshold));
            }
        }
        break;

    case itSetRSetting:
        {
            switch(SetVal)
                {
            #if AD_R_setting    //have IF attenuator
                    case MOD_PAL_BG: 
                    case MOD_PAL_DK: 
                    case MOD_PAL_I:
                    case MOD_NTSC_M:
                        *(UINT32 *)pInOutVal=0x06 ;
                    break;
                    case MOD_SECAM_L:
                    case MOD_SECAM_L1:
                        *(UINT32 *)pInOutVal=0x06 ;
                    break;
                    case MOD_ATSC_8VSB:
                    case MOD_J83B_64QAM:
                    case MOD_J83B_256QAM:
                    case MOD_J83B_AUTO_QAM:
                        *(UINT32 *)pInOutVal=0x06 ;
                    break;
                    case MOD_DVBT:
                    case MOD_DVBC:
                        *(UINT32 *)pInOutVal=0x06 ;
                    break;
            #else               //No IF attenuator
                    case MOD_PAL_BG: 
                    case MOD_PAL_DK: 
                    case MOD_PAL_I:
                    case MOD_NTSC_M:
                        *(UINT32 *)pInOutVal=0x00 ;
                    break;
                    case MOD_SECAM_L:
                    case MOD_SECAM_L1:
                        *(UINT32 *)pInOutVal=0x00 ;
                    break;
                    case MOD_ATSC_8VSB:
                    case MOD_J83B_64QAM:
                    case MOD_J83B_256QAM:
                    case MOD_J83B_AUTO_QAM:
                        *(UINT32 *)pInOutVal=0x00 ;
                    break;
                    case MOD_DVBT:
                    case MOD_DVBC:
                        *(UINT32 *)pInOutVal=0x00 ;
                    break;
            #endif
                    default:
                        *(UINT32 *)pInOutVal=0x06 ;

            }
        }
        break;

    default:
        return ITUNER_NOT_SUPPORTED;
    }

    return ITUNER_OK;
}

/* For driver debug trace --------------------------------------------------- */
#ifdef SONYRF_TRACE_ENABLE
void sonyrf_trace_log_enter(const char* funcname, const char* filename, unsigned int linenum)
{
    mcDBG_MSG_L4(("RE227_ENTER : %s (%d)\n", funcname, linenum));
}
void sonyrf_trace_log_return(sonyrf_result_t result, const char* filename, unsigned int linenum)
{
    if(result != SONYRF_RESULT_OK){
        mcDBG_MSG_L4(("RE227_ERROR_RETURN(%d) : (line %d)\n", result, linenum));
    }else{
        mcDBG_MSG_L4(("RE227_RETURN : (line %d))\n", linenum));
    }
}

void sonyrf_trace_i2c_log_enter(const char* funcname, const char* filename, unsigned int linenum)
{
    sonyrf_trace_log_enter(funcname, filename, linenum);
}
void sonyrf_trace_i2c_log_return(sonyrf_result_t result, const char* filename, unsigned int linenum)
{
    sonyrf_trace_log_return(result, filename, linenum);
}
#endif /* SONYRF_TRACE_ENABLE */
