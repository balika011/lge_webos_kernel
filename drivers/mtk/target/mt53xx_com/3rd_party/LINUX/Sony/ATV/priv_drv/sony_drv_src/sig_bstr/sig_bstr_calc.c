#include "x_dbg.h"
#include "x_os.h"
#include "x_assert.h"
#include "../../../../../../../../custom/Sony/2K12/res/app_util/config/acfg_atv_eep.h"

#include "../../sony_priv_inc/sig_bstr.h"
#include "../../sony_drv_inc/sig_bstr_if.h"
#include "../../sony_drv_inc/Sony_drv_cb.h"
#include "eeprom_if.h"
#include "../../sony_priv_inc/sony_tuner.h"
#include "../../sony_drv_inc/sony_tuner_if.h"

/*-----------------------------------------------------------------------------
 * structure, constants, macro definitions
 *---------------------------------------------------------------------------*/
//#define SIG_BSTR_LOG
#define SREG_SIG_BSTR_DUMMY 0xFFFF
#define SYS_DRV_CS          0xA1
#define SIG_BSTR_EEP_SIZE (EEP_OFFSET_EMCS_SIG_BSTR_END-EEP_OFFSET_EMCS_SIG_BSTR_START)
typedef struct _T_SYS_DRV_NVM{
	const UINT16 offset;
	const UINT16 sreg;
	const UINT8  byte_size;
	const UINT16 init;
}T_SYS_DRV_NVM;
const T_SYS_DRV_NVM tbl_sig_bstr_eep[] = {
                /* Offset */                        /*SREG*/                /* Byte Size */       /* Init */
    {   EEP_OFFSET_EMCS_SIG_BSTR_DRV_CS,    SREG_SIG_BSTR_DUMMY,            1,               SYS_DRV_CS },
    {   EEP_OFFSET_EMCS_LNA_WAIT_TIME,      SREG_SIG_BSTR_LNA_WAIT_TIME,    1,                  15      },
    {   EEP_OFFSET_EMCS_LNA_WAIT_TIME2,     SREG_SIG_BSTR_LNA_WAIT_TIME2,   1,                  10      },
    {   EEP_OFFSET_EMCS_LNA_AGC_TH,         SREG_SIG_BSTR_LNA_AGC_TH,       1,                  0       },
    {   EEP_OFFSET_EMCS_LNA_SNR_TH,         SREG_SIG_BSTR_LNA_SNR_TH,       2,                  0x200   },
    {   EEP_OFFSET_EMCS_LNA_SN_DIFF_TH,     SREG_SIG_BSTR_LNA_SN_DIFF_TH,   1,                  0       },
};
/*-----------------------------------------------------------------------------
 * variable declarations
 *---------------------------------------------------------------------------*/
typedef enum{
    SIG_BSTR_ERR_NOERR = 0,
    SIG_BSTR_ERR_FAIL
}sig_bstr_err;

typedef enum{
    SETLNA_FAIL = 0,
    SETLNA_NOERR
}setlna_err;

typedef enum{
    LNA_WAIT1 = 0,
    LNA_WAIT2
}sig_bstr_lnawait;

typedef enum{
    LNA_OFF = 0,
    LNA_ON
}sig_bstr_lna;
UINT16 SIG_BSTR_SREG[SREG_SIG_BSTR_MAX];
/*-----------------------------------------------------------------------------
 * private methods declarations
 *---------------------------------------------------------------------------*/
static UINT8 _sig_bstr_set_lna(VOID);
static UINT8 _sig_bstr_get_agc_lvl(VOID);
static UINT8 _sig_bstr_get_snr_lvl(UINT16 sreg);
static VOID _sig_bstr_sreg_get_snr_lvl(UINT32 cvbs_snr);
static VOID _sig_bstr_lna_wait(UINT8 lnawait);
static BOOL _sig_bstr_lna_sig_check(VOID);
static VOID _sig_bstr_lna_effect_check(VOID);
static VOID _SYS_EEPRead(UINT16 offset, UINT8* buffer, UINT16 len);
static VOID _SYS_EEPWrite(UINT16 offset, UINT8* buffer, UINT16 len);
static void _sig_bstr_sreg_write(UINT16 sreg,UINT16 val);
static VOID _sig_bstr_eep_init(UINT8* buffer);
static INT8 _sig_bstr_calc_agc_ifmin_lvl(UINT8 agc_th, INT8 ifmin);
/*-----------------------------------------------------------------------------
 * private methods implementations
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Name:    _sig_bstr_set_lna
 * Description: set tuner lna on/off
 * Input:   setLNA      Determine whether to on or off LNA
 * Output:  -
 * Returns  TVR_OK      This function executed successfully.
 *          Otherwise   This function failed.
 *---------------------------------------------------------------------------*/

static UINT8 _sig_bstr_set_lna(VOID){

    UINT8 err_lna = SETLNA_FAIL;
    UINT16 setLNA = main_sig_bstr_sreg_read(SREG_SIG_BSTR_LNA_SET);

    err_lna = vSigBstrSetLNA(setLNA);
#ifdef SIG_BSTR_LOG
    if(err_lna == SETLNA_FAIL){
        x_dbg_stmt("\n[SIG_BSTR]_sig_bstr_set_lna: setLNA %d: FAIL!!!\n\n", setLNA);       
    }else{
        x_dbg_stmt("\n[SIG_BSTR]_sig_bstr_set_lna: setLNA %d: SUCCESS!!!\n", setLNA);
    } 
#endif

    return err_lna;
}
static UINT8 _sig_bstr_get_agc_lvl(VOID){
    UINT8 er = SIG_BSTR_ERR_NOERR;
    UINT16 agc_lvl16;
    UINT8 agc_lvl;
#ifdef SIG_BSTR_LOG
	UINT8 polarity_chk=0;
#endif

    agc_lvl16 = vSigBstrGetIFAGC();
    agc_lvl = (UINT8) ((agc_lvl16 & 0xff00)>>8);
    _sig_bstr_sreg_write(SREG_SIG_BSTR_IFAGC,(UINT16)agc_lvl);

	//negative or positive check
	if((agc_lvl&0x80)==0x80){
		agc_lvl=0;
#ifdef SIG_BSTR_LOG
		polarity_chk = 1;
#endif
	}else{
#ifdef SIG_BSTR_LOG
		polarity_chk = 0;
#endif
	}
#ifdef SIG_BSTR_LOG
    x_dbg_stmt("\n[SIG_BSTR]_sig_bstr_get_agc_lvl 16: 0x%x [%d]",agc_lvl16,agc_lvl16);
    x_dbg_stmt("\n[SIG_BSTR]_sig_bstr_get_agc_lvl 8:0x%x [%d] chk:%d\n",agc_lvl,agc_lvl,polarity_chk);
#endif
    return er;
}
static INT8 _sig_bstr_calc_agc_ifmin_lvl(UINT8 agc_th, INT8 ifmin){
    INT8 final_agc;
    INT16 I16_final_agc;
#ifdef SIG_BSTR_LOG
            x_dbg_stmt("\n\n[SIG_BSTR]_sig_bstr_calc_agc_ifmin_lvl IFMIN: %d",ifmin);
#endif
    I16_final_agc = (INT16)agc_th+(INT16)ifmin;
    if(I16_final_agc>=127){
        I16_final_agc = 127;
        #ifdef SIG_BSTR_LOG
        x_dbg_stmt("\n*****************[SIG_BSTR]I16_final_agc>127********************");
        #endif 
    }else if(I16_final_agc<= (-128)){
        I16_final_agc = -128;
        #ifdef SIG_BSTR_LOG
        x_dbg_stmt("\n*****************[SIG_BSTR]I16_final_agc>-128********************");
        #endif
    }
    final_agc = (INT8) I16_final_agc;
    _sig_bstr_sreg_write(SREG_SIG_BSTR_CALC_AGC_TH,final_agc);
#ifdef SIG_BSTR_LOG
        x_dbg_stmt("\n[SIG_BSTR]_sig_bstr_calc_agc_ifmin_lvl final_agc: %d\n",final_agc);
#endif
    return final_agc;
}

static UINT8 _sig_bstr_get_snr_lvl(UINT16 sreg){
    UINT8 er = SIG_BSTR_ERR_NOERR;
    UINT16 snr_lvl;
    UINT32 snr32;

    snr32 = vSigBstrGetNoiseLevel();
    _sig_bstr_sreg_get_snr_lvl(snr32);
    snr_lvl = (UINT16) ((snr32&0x0007ffff)>>3);
    _sig_bstr_sreg_write(sreg,snr_lvl);
#ifdef SIG_BSTR_LOG
    switch(sreg){
        case SREG_SIG_BSTR_SNR1:
            x_dbg_stmt("\n[SIG_BSTR]_sig_bstr_get_snr_lvl SREG_SIG_BSTR_SNR1");
            break;
        case SREG_SIG_BSTR_SNR2:
            x_dbg_stmt("\n[SIG_BSTR]_sig_bstr_get_snr_lvl SREG_SIG_BSTR_SNR2");
            break;
        default:
            x_dbg_stmt("\n[SIG_BSTR]_sig_bstr_get_snr_lvl SREG not defined");
            break;
    }
    x_dbg_stmt("\n[SIG_BSTR]_sig_bstr_get_snr_lvl 32: 0x%x [%d]",snr32,snr32);
    x_dbg_stmt("\n[SIG_BSTR]_sig_bstr_get_snr_lvl 16: 0x%x [%d]\n",snr_lvl,snr_lvl);
#endif
    return er;
}
static VOID _sig_bstr_sreg_get_snr_lvl(UINT32 cvbs_snr){
    UINT16 snr1, snr2, snr3;
    snr1 = (UINT16)((cvbs_snr & 0x0007f800)>>11);
    snr2 = (UINT16)((cvbs_snr & 0x000007f8)>>3);
    snr3 = (UINT16)(cvbs_snr & 0x00000007);
    _sig_bstr_sreg_write(SREG_SIG_BSTR_CVBS_SNR3,snr3);
    _sig_bstr_sreg_write(SREG_SIG_BSTR_CVBS_SNR2,snr2);
    _sig_bstr_sreg_write(SREG_SIG_BSTR_CVBS_SNR1,snr1);
}
static VOID _sig_bstr_lna_wait(UINT8 lnawait){

#ifdef SIG_BSTR_LOG
		x_dbg_stmt("\n++++++++++++++++[SIG_BSTR]_sig_bstr_lna_wait%d++++++++++++++++++\n\n",lnawait);
#endif

	if(lnawait==LNA_WAIT1){
    	x_thread_delay(main_sig_bstr_sreg_read(SREG_SIG_BSTR_LNA_WAIT_TIME)*10);
	}else{
		x_thread_delay(main_sig_bstr_sreg_read(SREG_SIG_BSTR_LNA_WAIT_TIME2)*10);
	}
}

static BOOL _sig_bstr_lna_sig_check(VOID){
    BOOL lna_check = FALSE;
    BOOL agc_check = FALSE;
    BOOL noise_check = FALSE;
    UINT8 err_agc = SIG_BSTR_ERR_NOERR;
    INT8 agc_lvl=0;
    UINT8 agc_th = (UINT8)main_sig_bstr_sreg_read(SREG_SIG_BSTR_LNA_AGC_TH);
    UINT16 snr_th = main_sig_bstr_sreg_read(SREG_SIG_BSTR_LNA_SNR_TH);
    INT8 if_min = (INT8)main_sony_tuner_sreg_read(SREG_TUNER_PRA_IFMIN);
    INT8 final_agc_th;
#ifdef SIG_BSTR_LOG
    x_dbg_stmt("\n============================================================");
	x_dbg_stmt("\n********lna_sig_check: snr_th:0x%.4x agc_th: 0x%.4x*********", snr_th,agc_th);
    x_dbg_stmt("\n============================================================");
#endif

    err_agc = _sig_bstr_get_agc_lvl();
    if(err_agc == SIG_BSTR_ERR_FAIL){
        #ifdef SIG_BSTR_LOG
            x_dbg_stmt("\n[SIG_BSTR]lna_sig_check: AGC get error!!!\n\n");
        #endif
        lna_check = FALSE;
        return lna_check;
    }
    _sig_bstr_get_snr_lvl(SREG_SIG_BSTR_SNR1);
    agc_lvl = (INT8)main_sig_bstr_sreg_read(SREG_SIG_BSTR_IFAGC);
    #ifdef SIG_BSTR_LOG
        x_dbg_stmt("\n[SIG_BSTR]lna_sig_check: AGC value: %d",main_sig_bstr_sreg_read(SREG_SIG_BSTR_IFAGC));
        x_dbg_stmt("\n[SIG_BSTR]lna_sig_check: AGC value: %d",agc_lvl);
        x_dbg_stmt("\n[SIG_BSTR]lna_sig_check: SNR value: %d",main_sig_bstr_sreg_read(SREG_SIG_BSTR_SNR1));
    #endif
    final_agc_th = _sig_bstr_calc_agc_ifmin_lvl(agc_th, if_min);
    if(agc_lvl>final_agc_th){
        agc_check = TRUE;
        #ifdef SIG_BSTR_LOG
        x_dbg_stmt("\n\n*****************[SIG_BSTR]final_agc>agc_th********************");
        #endif 
    }
    if(main_sig_bstr_sreg_read(SREG_SIG_BSTR_SNR1)>snr_th){
        noise_check = TRUE;
        #ifdef SIG_BSTR_LOG
        x_dbg_stmt("\n*****************[SIG_BSTR]snr1_lvl>snr_th********************");
        #endif 
    }

    if(agc_check&&noise_check){
        lna_check = TRUE;
        _sig_bstr_sreg_write(SREG_SIG_BSTR_LNA_SET,LNA_ON);
        #ifdef SIG_BSTR_LOG
            x_dbg_stmt("\n[SIG_BSTR]lna_sig_check: LNA ON");
        #endif
    }else{
        lna_check = FALSE;
        _sig_bstr_sreg_write(SREG_SIG_BSTR_LNA_SET,LNA_OFF);
        #ifdef SIG_BSTR_LOG
            x_dbg_stmt("\n[SIG_BSTR]lna_sig_check: LNA OFF");
        #endif
    }
   _sig_bstr_set_lna();
#ifdef SIG_BSTR_LOG
    x_dbg_stmt("\n============================================================");
    x_dbg_stmt("\n**********************lna_sig_check END**********************");
	x_dbg_stmt("\n=============================================================\n\n");
#endif
    return lna_check;
}

static VOID _sig_bstr_lna_effect_check(VOID){
    UINT16 lna_effect;
    UINT16 snr_diff_th = main_sig_bstr_sreg_read(SREG_SIG_BSTR_LNA_SN_DIFF_TH);
    UINT16 snr1, snr2;
    UINT8 err_snr = SIG_BSTR_ERR_NOERR;
    BOOL lna_effect_negative=FALSE;
#ifdef SIG_BSTR_LOG    
    x_dbg_stmt("\n============================================================");
	x_dbg_stmt("\n*******_sig_bstr_lna_effect_check: snr_diff_th:0x%.4x*******",snr_diff_th);
    x_dbg_stmt("\n============================================================");
#endif

    err_snr = _sig_bstr_get_snr_lvl(SREG_SIG_BSTR_SNR2);
    if(err_snr == SIG_BSTR_ERR_FAIL){
        #ifdef SIG_BSTR_LOG
                x_dbg_stmt("\n[SIG_BSTR]lna_effect_check: SNR get ERROR\n\n");
        #endif
    }
    snr1 = main_sig_bstr_sreg_read(SREG_SIG_BSTR_SNR1);
    snr2 = main_sig_bstr_sreg_read(SREG_SIG_BSTR_SNR2);
    if(snr2>=snr1){
        lna_effect = snr2-snr1;
        lna_effect_negative=FALSE;
        #ifdef SIG_BSTR_LOG
        x_dbg_stmt("\n\n*****************[SIG_BSTR]snr2>=snr1: TRUE********************");
        #endif 
    }else{
        lna_effect = 0;
        lna_effect_negative=TRUE;
        #ifdef SIG_BSTR_LOG
        x_dbg_stmt("\n\n*****************[SIG_BSTR]snr2>=snr1: FALSE********************");
        #endif 
    }
    _sig_bstr_sreg_write(SREG_SIG_BSTR_SNR_DIFF,lna_effect);
	#ifdef SIG_BSTR_LOG
        x_dbg_stmt("\n[SIG_BSTR]lna_effect_check: SNR1: %d  SNR2: %d",snr1,snr2);
		x_dbg_stmt("\n[SIG_BSTR]lna_effect_check: lna_effect: %d",lna_effect);
    #endif
    if(lna_effect_negative==FALSE){
        if(lna_effect>=snr_diff_th){
            _sig_bstr_sreg_write(SREG_SIG_BSTR_LNA_SET,LNA_OFF);
            _sig_bstr_set_lna();

            #ifdef SIG_BSTR_LOG
                x_dbg_stmt("\n[SIG_BSTR]lna_effect_check: LNA OFF\n");
            #endif
        }else{
            #ifdef SIG_BSTR_LOG
                x_dbg_stmt("\n[SIG_BSTR]lna_effect_check: LNA ON\n");
            #endif
        }
    }else{
        #ifdef SIG_BSTR_LOG
            x_dbg_stmt("\n[SIG_BSTR]lna_effect_check: LNA ON\n");
        #endif
    }
#ifdef SIG_BSTR_LOG
    x_dbg_stmt("\n============================================================");
	x_dbg_stmt("\n************_sig_bstr_lna_effect_check ENDS*****************");
    x_dbg_stmt("\n============================================================\n\n");
#endif
}
//-------------------------------------------------------------------------
// _SYS_EEPRead
//
/**
 *  local function to read EEP
 */
//-------------------------------------------------------------------------
static VOID _SYS_EEPRead(UINT16 offset, UINT8* buffer, UINT16 len)
{  
    if((offset >= EEP_OFFSET_EMCS_SIG_BSTR_START) &&
       (offset <= EEP_OFFSET_EMCS_SIG_BSTR_END))
    {
      UNUSED(EEPROM_Read((UINT64)offset, (UINT32)buffer, len));
    }
}
//-------------------------------------------------------------------------
// _SYS_EEPWrite
//
/**
 *  local function to write EEP
 */
//-------------------------------------------------------------------------
static VOID _SYS_EEPWrite(UINT16 offset, UINT8* buffer, UINT16 len)
{  
    if((offset >= EEP_OFFSET_EMCS_SIG_BSTR_START) &&
       (offset <= EEP_OFFSET_EMCS_SIG_BSTR_END))
    {
      UNUSED(EEPROM_Write((UINT64)offset, (UINT32)buffer, len));
    }
}
//-------------------------------------------------------------------------
// rgb_sreg_write
//
/**
 *  local function to write to sreg
 */
//-------------------------------------------------------------------------
static void _sig_bstr_sreg_write(UINT16 sreg,UINT16 val)
{
  
	if(sreg<SREG_SIG_BSTR_MAX)
	{
		SIG_BSTR_SREG[sreg] = val;
	}
}
//-------------------------------------------------------------------------
// _sig_bstr_eep_init
//
/**
 *  Initialize EEP data if checksum not correct
 */
//-------------------------------------------------------------------------
static VOID _sig_bstr_eep_init(UINT8* buffer)
{
    UINT16 tbl_cnt;
    UINT16 data;
    UINT16 len = 0;

    #ifdef SIG_BSTR_LOG
    x_dbg_stmt("\n\n[SIG_BSTR]sig_bstr_eep_init start check\n\n");
    #endif
    if(tbl_sig_bstr_eep[0].offset == EEP_OFFSET_EMCS_SIG_BSTR_DRV_CS) /* First element in table is checksum byte */
    {
      x_memset(buffer, 0, (sizeof(UINT8)*SIG_BSTR_EEP_SIZE));
      _SYS_EEPRead(tbl_sig_bstr_eep[0].offset, buffer, tbl_sig_bstr_eep[0].byte_size);
      data = buffer[0];
      
      if(data != SYS_DRV_CS)
      {
        /* sigbstsr Data area corruption, init with default data */
        /* Prepare data to write */
        x_memset(buffer, 0, (sizeof(UINT8)*SIG_BSTR_EEP_SIZE));
        for (tbl_cnt=0; tbl_cnt<(sizeof(tbl_sig_bstr_eep)/sizeof(tbl_sig_bstr_eep[0])); tbl_cnt++)
        {
            if(tbl_sig_bstr_eep[tbl_cnt].byte_size == 2)
            {
              buffer[len] = (UINT8)((tbl_sig_bstr_eep[tbl_cnt].init & 0xFF00) >> 8);
              len++;
              buffer[len] = (UINT8)(tbl_sig_bstr_eep[tbl_cnt].init & 0xFF);
              len++;
            }
            else
            {
              buffer[len] = (UINT8)(tbl_sig_bstr_eep[tbl_cnt].init & 0xFF);
              len++;
            }
        }
        _SYS_EEPWrite(tbl_sig_bstr_eep[0].offset, buffer, len);
      }
    }    
}
//-------------------------------------------------------------------------
// _sig_bstr_sreg_init
//
/**
 *  Initialize SREG from EEP data
 */
//-------------------------------------------------------------------------
static VOID _sig_bstr_sreg_init(UINT8* buffer)
{
    UINT16 tbl_cnt;
    UINT16 data;
    UINT16 len = 0;
    
    x_memset(buffer, 0, (sizeof(UINT8)*SIG_BSTR_EEP_SIZE));
    for (tbl_cnt=0; tbl_cnt<(sizeof(tbl_sig_bstr_eep)/sizeof(tbl_sig_bstr_eep[0])); tbl_cnt++)
    {
        if ((tbl_sig_bstr_eep[tbl_cnt].offset >= EEP_OFFSET_EMCS_LNA_WAIT_TIME) &&
          (tbl_sig_bstr_eep[tbl_cnt].offset <= EEP_OFFSET_EMCS_SIG_BSTR_END))
        {
    
        len += tbl_sig_bstr_eep[tbl_cnt].byte_size;
        }
    }
    
    _SYS_EEPRead(EEP_OFFSET_EMCS_LNA_WAIT_TIME, buffer, len);

    /*init all sreg 0*/
    for(tbl_cnt=1; tbl_cnt<SREG_SIG_BSTR_MAX;tbl_cnt++){
        _sig_bstr_sreg_write(tbl_cnt,0);
    }
    
    len=0;
    for (tbl_cnt=0; tbl_cnt<(sizeof(tbl_sig_bstr_eep)/sizeof(tbl_sig_bstr_eep[0])); tbl_cnt++)
    {
        if ((tbl_sig_bstr_eep[tbl_cnt].offset >= EEP_OFFSET_EMCS_LNA_WAIT_TIME) &&
          (tbl_sig_bstr_eep[tbl_cnt].offset <= EEP_OFFSET_EMCS_SIG_BSTR_END))
        {
            if(tbl_sig_bstr_eep[tbl_cnt].byte_size == 2)
            {
              data = (UINT16)buffer[len];
              len++;
              data = data << 8;
              data = data | ((UINT16)buffer[len] & 0xFF);
              len++; 
              _sig_bstr_sreg_write(tbl_sig_bstr_eep[tbl_cnt].sreg, data);
            }
            else
            {
              data = (UINT16)buffer[len];
              len++;
              _sig_bstr_sreg_write(tbl_sig_bstr_eep[tbl_cnt].sreg, data);
            }
        }
    }
}
static VOID _sig_bstr_calc_func(BOOL setSigBooster)
{
    UINT16 wait_time2 = main_sig_bstr_sreg_read(SREG_SIG_BSTR_LNA_WAIT_TIME2);
    if(setSigBooster==TRUE){
		_sig_bstr_lna_wait(LNA_WAIT1);
        if(_sig_bstr_lna_sig_check()){
            if(wait_time2!=0){
                _sig_bstr_lna_wait(LNA_WAIT2);
            }
            _sig_bstr_lna_effect_check();
        }
    }else{
        #ifdef SIG_BSTR_LOG
            x_dbg_stmt("[SIG_BSTR]_sig_bstr_calc_func FALSE\n");
        #endif
        _sig_bstr_sreg_write(SREG_SIG_BSTR_LNA_SET,LNA_OFF);
        _sig_bstr_set_lna();
    }
}
/*-----------------------------------------------------------------------------
 * exported methods implementations
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
* Name
*      sig_bstr_calc
* Description
*      The contains algorithm for signal booster
* Input arguments
*      BOOL setSigBooster  - informed by MW (ATV_gpio.h) from APP whether is set to Auto or OFF
* Output arguments
*      -
* Returns
*      -
*---------------------------------------------------------------------------*/
VOID sig_bstr_calc(BOOL setSigBooster)
{
    BOOL IP_status;
    vSonyTunerGetIPStatus(&IP_status);
    #ifdef SIG_BSTR_LOG
        x_dbg_stmt("\n\n**********************************************\n");
        x_dbg_stmt("******SIGNAL BOOSTER CALCULATION STARTS>>>>>>>>\n");
        x_dbg_stmt("**********************************************\n\n\n");
    #endif
    if(IP_status==TRUE){
        #ifdef SIG_BSTR_LOG
           x_dbg_stmt("[SIG_BSTR]sig_bstr_calc IP = TRUE\n");
        #endif
        _sig_bstr_calc_func(setSigBooster);
    }else{
        #ifdef SIG_BSTR_LOG
           x_dbg_stmt("[SIG_BSTR]sig_bstr_calc IP = FALSE\n");
        #endif
        _sig_bstr_sreg_write(SREG_SIG_BSTR_LNA_SET,LNA_OFF);
        _sig_bstr_set_lna();
    }
    #ifdef SIG_BSTR_LOG
        x_dbg_stmt("\n**********************************************\n");
        x_dbg_stmt("<<<<<<<SIGNAL BOOSTER CALCULATION ENDS*********\n");
        x_dbg_stmt("**********************************************\n\n");
    #endif
}
/*-----------------------------------------------------------------------------
* Name
*      SIG_BSTR_IP_calc
* Description
*      The contains algorithm for signal booster
* Input arguments
*      BOOL setSigBooster  - informed by APP via d_interface whether is set to Auto or OFF by user
* Output arguments
*      -
* Returns
*      -
*---------------------------------------------------------------------------*/
VOID SIG_BSTR_IP_calc(BOOL IP_status,BOOL SigBstr_status)
{
    #ifdef SIG_BSTR_LOG
            x_dbg_stmt("\n\n**********************************************\n");
            x_dbg_stmt("*****SIGNAL BOOSTER IP CALCULATION STARTS>>>>>>>\n");
            x_dbg_stmt("**********************************************\n\n");
    #endif
    if(IP_status==TRUE){
        #ifdef SIG_BSTR_LOG
           x_dbg_stmt("\n[SIG_BSTR]SIG_BSTR_IP_calc IP = TRUE\n");
        #endif
        _sig_bstr_calc_func(SigBstr_status);
    }else{
        #ifdef SIG_BSTR_LOG
           x_dbg_stmt("\n[SIG_BSTR]SIG_BSTR_IP_calc IP = FALSE\n");
        #endif
        _sig_bstr_sreg_write(SREG_SIG_BSTR_LNA_SET,LNA_OFF);
        _sig_bstr_set_lna();
    }
    #ifdef SIG_BSTR_LOG
        x_dbg_stmt("\n**********************************************\n");
        x_dbg_stmt("<<<<<<SIGNAL BOOSTER IP CALCULATION ENDS********\n");
        x_dbg_stmt("**********************************************\n\n");
    #endif
}

VOID SIG_BSTR_Init(VOID){
    UINT8* buf_ptr = NULL;

    buf_ptr = sony_x_mem_alloc(sizeof(UINT8)*(SIG_BSTR_EEP_SIZE+2));
    if(buf_ptr != NULL)
    {
        _sig_bstr_eep_init(buf_ptr);
        _sig_bstr_sreg_init(buf_ptr);
        sony_x_mem_free(buf_ptr);
        buf_ptr = NULL;
    }
    else
    {
        printf("[SIG_BSTR] mem alloc fail!!!\n");
    }
}
UINT16 main_sig_bstr_sreg_read(UINT16 sreg)
{
    UINT16 ret = 0;
	
	if(sreg<SREG_SIG_BSTR_MAX)
	{
		ret = SIG_BSTR_SREG[sreg];
	}
	else
	{
		ret = 0;

	}
	return(ret);
}
VOID main_sig_bstr_sreg_write(UINT16 sreg,UINT16 val)
{
    UINT16 i=0xFFFF,tbl_cnt;
    UINT8 buffer[2];
    _sig_bstr_sreg_write(sreg,val);
    if(sreg<SREG_SIG_BSTR_NVM){
        for (tbl_cnt=0; tbl_cnt<(sizeof(tbl_sig_bstr_eep)/sizeof(tbl_sig_bstr_eep[0])); tbl_cnt++){
            if(tbl_sig_bstr_eep[tbl_cnt].sreg==sreg){
                i = tbl_cnt;
                break;
            }
        }
        if(i!=0xFFFF){
            if(tbl_sig_bstr_eep[i].byte_size == 2)
            {
              buffer[0] = (UINT8)((val & 0xFF00) >> 8);
              buffer[1] = (UINT8)(val & 0xFF);
            }
            else
            {
              buffer[0] = (UINT8)(val & 0xFF);
            }
            _SYS_EEPWrite(tbl_sig_bstr_eep[i].offset, buffer, tbl_sig_bstr_eep[i].byte_size);
        }
    }
    switch(sreg){
        case SREG_SIG_BSTR_LNA_SET:
            if(val>LNA_ON){
                val = LNA_OFF;
            }
            _sig_bstr_set_lna();
            break;
        default:
            break;
    }
}
/****************************************************************************
*    End of file
****************************************************************************/
