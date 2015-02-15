#include "x_dbg.h"
#include "x_os.h"
#include "x_assert.h"
#include "../../../../../../../../../custom/Sony/2K12/res/app_util/config/acfg_atv_eep.h"

#include "../../sony_priv_inc/sony_tuner.h"
#include "../../sony_drv_inc/sony_tuner_if.h"
#include "../../sony_drv_inc/Sony_drv_cb.h"
#include "eeprom_if.h"

/*-----------------------------------------------------------------------------
 * structure, constants, macro definitions
 *---------------------------------------------------------------------------*/
//#define SONY_TUNER_LOG
//#define ENABLE_TUNER_LOG

#if defined (ENABLE_TUNER_LOG)
#include "x_debug.h"
#define TU_LOG(fmt...)         Printf(fmt)
#else
#define TU_LOG(fmt...)            
#endif

#define SREG_SONY_TUNER_DUMMY 0xFFFF
#define EEP_OFFSET_EMCS_TUNER_DUMMY 0xFFFF
#define SYS_DRV_CS          0xA1
#define SONY_TUNER_EEP_SIZE (EEP_OFFSET_EMCS_SONY_TUNER_END-EEP_OFFSET_EMCS_SONY_TUNER_START)
#define SONY_TUNER_AGC_REG_CNT  4
typedef struct _T_SYS_DRV_NVM{
	const UINT16 offset;
	const UINT16 sreg;
	const UINT8  byte_size;
	const UINT16 init;
}T_SYS_DRV_NVM;
typedef struct _T_SYS_DRV_SREG{
	const UINT16 sreg;
	const UINT8  byte_size;
    const UINT16 regAddr;
}T_SYS_DRV_SREG;
typedef enum{
    reg_FAIL = 0,
    reg_NOERR
}reg_err;
const T_SYS_DRV_NVM tbl_sony_tuner_eep[] = {
                /* Offset */                        /*SREG*/                /* Byte Size */       /* Init */
    {   EEP_OFFSET_EMCS_TUNER_DRV_CS,       SREG_SONY_TUNER_DUMMY,          1,               SYS_DRV_CS },
    {   EEP_OFFSET_EMCS_PRA_RFMAX,          SREG_TUNER_PRA_RFMAX,           1,                  0x7F    },
    {   EEP_OFFSET_EMCS_PRA_IFMAX,          SREG_TUNER_PRA_IFMAX,           1,                  0x7F    },
    {   EEP_OFFSET_EMCS_PRA_RFMIN,          SREG_TUNER_PRA_RFMIN,           1,                  0x80    },
    {   EEP_OFFSET_EMCS_PRA_IFMIN,          SREG_TUNER_PRA_IFMIN,           1,                  0xFB    },
    {   EEP_OFFSET_EMCS_PRA_IFMIN_CIS,      SREG_TUNER_PRA_IFMIN_CIS,       1,                  0x01    },
};
const T_SYS_DRV_SREG tbl_sony_tuner_sreg[] = {
                /*SREG*/         /* Byte Size */       /* RegAddr */
    {   SREG_TUNER_PRA_RFMAX,           1,              0x955    },
    {   SREG_TUNER_PRA_IFMAX,           1,              0x956    },
    {   SREG_TUNER_PRA_RFMIN,           1,              0x957    },
    {   SREG_TUNER_PRA_IFMIN,           1,              0x958    },
    {   SREG_TUNER_PRA_IFMIN_CIS,       1,              0x958    },
    {   SREG_TUNER_OVM,                 1,              0x9A0    },
    {   SREG_TUNER_CR_STATE,            1,              0x9A1    },
    {   SREG_TUNER_CFO1,                2,              0x9A2    },
    {   SREG_TUNER_CFO2,                2,              0x9A4    },
    {   SREG_TUNER_DIV,                 2,              SREG_SONY_TUNER_DUMMY    },
    {   SREG_TUNER_DATA3,               2,              SREG_SONY_TUNER_DUMMY    },
    {   SREG_TUNER_DATA4,               2,              SREG_SONY_TUNER_DUMMY    },
};
/*-----------------------------------------------------------------------------
 * variable declarations
 *---------------------------------------------------------------------------*/
UINT16 SONY_TUNER_SREG[SREG_TUNER_MAX];
/*-----------------------------------------------------------------------------
 * private methods declarations
 *---------------------------------------------------------------------------*/
static VOID _SYS_EEPRead(UINT16 offset, UINT8* buffer, UINT16 len);
static VOID _SYS_EEPWrite(UINT16 offset, UINT8* buffer, UINT16 len);
static void _sony_tuner_sreg_write(UINT16 sreg,UINT16 val);
static VOID _sony_tuner_eep_init(UINT8* buffer);
static VOID _sony_tuner_sreg_init(UINT8* buffer);
static UINT16 _sony_tuner_sreg_country_sel(UINT16 sreg);
/*-----------------------------------------------------------------------------
 * private methods implementations
 *---------------------------------------------------------------------------*/

//-------------------------------------------------------------------------
// _SYS_EEPRead
//
/**
 *  local function to read EEP
 */
//-------------------------------------------------------------------------
static VOID _SYS_EEPRead(UINT16 offset, UINT8* buffer, UINT16 len)
{  
    if((offset >= EEP_OFFSET_EMCS_SONY_TUNER_START) &&
       (offset <= EEP_OFFSET_EMCS_SONY_TUNER_END))
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
    if((offset >= EEP_OFFSET_EMCS_SONY_TUNER_START) &&
       (offset <= EEP_OFFSET_EMCS_SONY_TUNER_END))
    {
      UNUSED(EEPROM_Write((UINT64)offset, (UINT32)buffer, len));
    }
}
//-------------------------------------------------------------------------
// _sony_tuner_sreg_write
//
/**
 *  local function to write to sreg
 */
//-------------------------------------------------------------------------
static void _sony_tuner_sreg_write(UINT16 sreg,UINT16 val)
{
  
	if(sreg<SREG_TUNER_MAX)
	{
		SONY_TUNER_SREG[sreg] = val;
	}
}
//-------------------------------------------------------------------------
// sony_tuner_eep_init
//
/**
 *  Initialize EEP data if checksum not correct
 */
//-------------------------------------------------------------------------
static VOID _sony_tuner_eep_init(UINT8* buffer)
{
    UINT16 tbl_cnt;
    UINT16 data;
    UINT16 len = 0;

    #ifdef SONY_TUNER_LOG
    x_dbg_stmt("\n\n[SONY_TUNER]sony_tuner_eep_init start check\n\n");
    #endif
    if(tbl_sony_tuner_eep[0].offset == EEP_OFFSET_EMCS_TUNER_DRV_CS) /* First element in table is checksum byte */
    {
      x_memset(buffer, 0, (sizeof(UINT8)*SONY_TUNER_EEP_SIZE));
      _SYS_EEPRead(tbl_sony_tuner_eep[0].offset, buffer, tbl_sony_tuner_eep[0].byte_size);
      data = buffer[0];
      
      if(data != SYS_DRV_CS)
      {
        /* sigbstsr Data area corruption, init with default data */
        /* Prepare data to write */
        x_memset(buffer, 0, (sizeof(UINT8)*SONY_TUNER_EEP_SIZE));
        for (tbl_cnt=0; tbl_cnt<(sizeof(tbl_sony_tuner_eep)/sizeof(tbl_sony_tuner_eep[0])); tbl_cnt++)
        {
            if(tbl_sony_tuner_eep[tbl_cnt].byte_size == 2)
            {
              buffer[len] = (UINT8)((tbl_sony_tuner_eep[tbl_cnt].init & 0xFF00) >> 8);
              len++;
              buffer[len] = (UINT8)(tbl_sony_tuner_eep[tbl_cnt].init & 0xFF);
              len++;
            }
            else
            {
              buffer[len] = (UINT8)(tbl_sony_tuner_eep[tbl_cnt].init & 0xFF);
              len++;
            }
        }
        _SYS_EEPWrite(tbl_sony_tuner_eep[0].offset, buffer, len);
      }
    }    
}
//-------------------------------------------------------------------------
// sony_tuner_sreg_init
//
/**
 *  Initialize SREG from EEP data
 */
//-------------------------------------------------------------------------
static VOID _sony_tuner_sreg_init(UINT8* buffer)
{
    UINT16 tbl_cnt;
    UINT16 data;
    UINT16 len = 0;
    
    x_memset(buffer, 0, (sizeof(UINT8)*SONY_TUNER_EEP_SIZE));
    for (tbl_cnt=0; tbl_cnt<(sizeof(tbl_sony_tuner_eep)/sizeof(tbl_sony_tuner_eep[0])); tbl_cnt++)
    {
        if ((tbl_sony_tuner_eep[tbl_cnt].offset >= EEP_OFFSET_EMCS_PRA_RFMAX) &&
          (tbl_sony_tuner_eep[tbl_cnt].offset <= EEP_OFFSET_EMCS_SONY_TUNER_END))
        {
    
        len += tbl_sony_tuner_eep[tbl_cnt].byte_size;
        }
    }
    
    _SYS_EEPRead(EEP_OFFSET_EMCS_PRA_RFMAX, buffer, len);

    /*init all sreg 0*/
    for(tbl_cnt=1; tbl_cnt<SREG_TUNER_MAX;tbl_cnt++){
        _sony_tuner_sreg_write(tbl_cnt,0);
    }
    
    len=0;
    for (tbl_cnt=0; tbl_cnt<(sizeof(tbl_sony_tuner_eep)/sizeof(tbl_sony_tuner_eep[0])); tbl_cnt++)
    {
        if ((tbl_sony_tuner_eep[tbl_cnt].offset >= EEP_OFFSET_EMCS_PRA_RFMAX) &&
          (tbl_sony_tuner_eep[tbl_cnt].offset <= EEP_OFFSET_EMCS_SONY_TUNER_END))
        {
            if(tbl_sony_tuner_eep[tbl_cnt].byte_size == 2)
            {
              data = (UINT16)buffer[len];
              len++;
              data = data << 8;
              data = data | ((UINT16)buffer[len] & 0xFF);
              len++; 
              _sony_tuner_sreg_write(tbl_sony_tuner_eep[tbl_cnt].sreg, data);
            }
            else
            {
              data = (UINT16)buffer[len];
              len++;
              _sony_tuner_sreg_write(tbl_sony_tuner_eep[tbl_cnt].sreg, data);
            }
        }
    }
}
static UINT16 _sony_tuner_sreg_country_sel(UINT16 sreg)
{
    if(sreg==SREG_TUNER_PRA_IFMIN)
    {
        if(SONY_TUNER_SREG[SREG_TUNER_COUNTRY_SEL]==TUNER_CNTY_CIS)
        {
            return SREG_TUNER_PRA_IFMIN_CIS;
        }else{
            return SREG_TUNER_PRA_IFMIN;
        }
    }else{
        return sreg;
    }
            
}
//-------------------------------------------------------------------------
// _sony_tuner_set_agc_reg
//
/**
 *  Set ATD reg from EEP data
 */
//-------------------------------------------------------------------------
VOID SONY_TUNER_set_agc_reg(VOID)
{
    UINT16 tbl_cnt;
    UINT16 i, set_cnt=0;
    UINT8 data;
    ST_PARAM_ATD_GETSETREG param;
    UINT8 sreg[SONY_TUNER_AGC_REG_CNT] = 
        { SREG_TUNER_PRA_RFMAX,SREG_TUNER_PRA_IFMAX,SREG_TUNER_PRA_RFMIN,SREG_TUNER_PRA_IFMIN };
    sreg[3]=_sony_tuner_sreg_country_sel(sreg[3]);

    TU_LOG("\n\n*********SONY_TUNER_set_agc_reg run!************\n");
    for (tbl_cnt=0; tbl_cnt<(sizeof(tbl_sony_tuner_sreg)/sizeof(tbl_sony_tuner_sreg[0])); tbl_cnt++){
        for(i=0; i<(sizeof(sreg)/sizeof(sreg[0])); i++){
            if(tbl_sony_tuner_sreg[tbl_cnt].sreg==sreg[i]){
                data = SONY_TUNER_SREG[sreg[i]];
                param.RegAddr = tbl_sony_tuner_sreg[tbl_cnt].regAddr;
                param.pData   = &data;
                param.ByteCnt = tbl_sony_tuner_sreg[tbl_cnt].byte_size;
                vSonyTunerSetAGCReg(&param);
                ++set_cnt;
                break;
            }
        }
        TU_LOG("\n\n*********SONY_TUNER_set_agc_reg[%d]: 0x%x************\n",sreg[i],data);
        if(set_cnt >= SONY_TUNER_AGC_REG_CNT){
            break;
        }
    }
    TU_LOG("\n\n");
}
VOID SONY_TUNER_cis_setting(BOOL fgCIS)
{
    ST_PARAM_ATD_GETSETREG param;
    UINT16 i=0xFFFF,tbl_cnt;
    UINT16 sreg;
    UINT8 data;
    if(fgCIS==TRUE)
    {
        _sony_tuner_sreg_write(SREG_TUNER_COUNTRY_SEL,TUNER_CNTY_CIS);
        sreg = SREG_TUNER_PRA_IFMIN_CIS;
    }else{
        _sony_tuner_sreg_write(SREG_TUNER_COUNTRY_SEL,TUNER_CNTY_OTHER);
        sreg = SREG_TUNER_PRA_IFMIN;
    }
    data = (UINT8)SONY_TUNER_SREG[sreg];

    if(sreg<SREG_TUNER_NVM){
        for (tbl_cnt=0; tbl_cnt<(sizeof(tbl_sony_tuner_sreg)/sizeof(tbl_sony_tuner_sreg[0])); tbl_cnt++){
            if(tbl_sony_tuner_sreg[tbl_cnt].sreg==sreg){
                i = tbl_cnt;
                break;
            }
        }
        param.RegAddr = tbl_sony_tuner_sreg[i].regAddr;
        param.ByteCnt = tbl_sony_tuner_sreg[i].byte_size;
        param.pData   = &data;
        vSonyTunerSetAGCReg(&param);
        if((param.Success)==reg_NOERR){
            TU_LOG("\n\n*********SONY_TUNER_cis_setting success[%d]: 0x%x************\n\n",sreg,data);
        }else{
            TU_LOG("\n\n*********SONY_TUNER_cis_setting fail[%d]: 0x%x************\n\n",sreg,data);
        }
    }
}

/*-----------------------------------------------------------------------------
 * exported methods implementations
 *---------------------------------------------------------------------------*/

VOID SONY_TUNER_Init(VOID){
    UINT8* buf_ptr = NULL;

    buf_ptr = sony_x_mem_alloc(sizeof(UINT8)*(SONY_TUNER_EEP_SIZE+2));
    if(buf_ptr != NULL)
    {
        _sony_tuner_eep_init(buf_ptr);
        _sony_tuner_sreg_init(buf_ptr);
        sony_x_mem_free(buf_ptr);
        buf_ptr = NULL;
    }
    else
    {
        printf("[SONY_TUNER] mem alloc fail!!!\n");
    }
}
UINT16 main_sony_tuner_sreg_read(UINT16 sreg)
{
    UINT16 ret = 0;
    PARAM_ST_SONY_TUNER_DATA param;
    UINT16 real_sreg = _sony_tuner_sreg_country_sel(sreg);
    switch(real_sreg){
        case SREG_TUNER_DIV:
            param.TU_DATA=l_TU_FreqData;
            vSonyTunerGetDevData(&param);
            _sony_tuner_sreg_write(SREG_TUNER_DIV,param.value);
            break;
        case SREG_TUNER_DATA3:
            param.TU_DATA=l_TU_StepData;
            vSonyTunerGetDevData(&param);
            _sony_tuner_sreg_write(SREG_TUNER_DATA3,param.value);
            break;
        case SREG_TUNER_DATA4:
            param.TU_DATA=l_TU_BandData;
            vSonyTunerGetDevData(&param);
            _sony_tuner_sreg_write(SREG_TUNER_DATA4,param.value);
            break;
        default:
        	break;
    }
	
	if(real_sreg<SREG_TUNER_MAX)
	{
		ret = SONY_TUNER_SREG[real_sreg];
	}
	else
	{
		ret = 0;

	}
	return(ret);
}
VOID main_sony_tuner_sreg_write(UINT16 sreg,UINT16 val)
{
    UINT16 i=0xFFFF,tbl_cnt;
    UINT8 buffer[2];
    PARAM_ST_SONY_TUNER_DATA param;
    _sony_tuner_sreg_write(sreg,val);
    switch(sreg){
        case SREG_TUNER_DIV:
            param.TU_DATA=l_TU_FreqData;
            param.value = val;
            vSonyTunerSetDevData(&param);
            break;
        case SREG_TUNER_DATA3:
            param.TU_DATA=l_TU_StepData;
            param.value = val;
            vSonyTunerSetDevData(&param);
            break;
        case SREG_TUNER_DATA4:
            param.TU_DATA=l_TU_BandData;
            param.value = val;
            vSonyTunerSetDevData(&param);
            break;
        default:
        	break;
    }
    if(sreg<SREG_TUNER_NVM){
        for (tbl_cnt=0; tbl_cnt<(sizeof(tbl_sony_tuner_eep)/sizeof(tbl_sony_tuner_eep[0])); tbl_cnt++){
            if(tbl_sony_tuner_eep[tbl_cnt].sreg==sreg){
                i = tbl_cnt;
                break;
            }
        }
        if(i!=0xFFFF){
            if(tbl_sony_tuner_eep[i].byte_size == 2)
            {
              buffer[0] = (UINT8)((val & 0xFF00) >> 8);
              buffer[1] = (UINT8)(val & 0xFF);
            }
            else
            {
              buffer[0] = (UINT8)(val & 0xFF);
            }
            _SYS_EEPWrite(tbl_sony_tuner_eep[i].offset, buffer, tbl_sony_tuner_eep[i].byte_size);
        }
    }
}
VOID main_sony_tuner_sreg_read_reg(void* pvArg)
{
    //UINT8* buf_ptr = NULL;
    UINT8 buf_ptr[2];
    UINT16 i=0xFFFF,tbl_cnt;
    ST_PARAM_ATD_GETSETREG param;
    UINT16 data=0;
    UINT16 sreg;
    PARAM_DRV_SONY_TUNER_GETSET* struct_param;

    struct_param = (PARAM_DRV_SONY_TUNER_GETSET*) pvArg;
    sreg = _sony_tuner_sreg_country_sel(struct_param->sreg);

    if(sreg<SREG_TUNER_MAX){
        for (tbl_cnt=0; tbl_cnt<(sizeof(tbl_sony_tuner_sreg)/sizeof(tbl_sony_tuner_sreg[0])); tbl_cnt++){
            if(tbl_sony_tuner_sreg[tbl_cnt].sreg==sreg){
                i = tbl_cnt;
                break;
            }
        }
        if(i!=0xFFFF){
            //buf_ptr = sony_x_mem_alloc(sizeof(UINT8)*(tbl_sony_tuner_sreg[i].byte_size));
            param.RegAddr = tbl_sony_tuner_sreg[i].regAddr;
            param.ByteCnt = tbl_sony_tuner_sreg[i].byte_size;
            param.pData   = buf_ptr;
            vSonyTunerGetReg(&param);
            if((param.Success)==reg_NOERR){
                if(tbl_sony_tuner_sreg[i].byte_size == 2)
                {
                  data = buf_ptr[0]<<8;
                  data |= buf_ptr[1];
                }
                else
                {
                  data = buf_ptr[0];
                }
            }else{
                data = 0;    
            }
            //sony_x_mem_free(buf_ptr);
            //buf_ptr = NULL;
            _sony_tuner_sreg_write(sreg,data);
        }
    }
    struct_param->val = data;
}
VOID main_sony_tuner_sreg_write_reg(void* pvArg)
{
    //UINT8* buf_ptr = NULL;
    UINT8 buf_ptr;
    UINT16 i=0xFFFF,tbl_cnt;
    ST_PARAM_ATD_GETSETREG param;
    UINT16 sreg;
    UINT16 val;
    PARAM_DRV_SONY_TUNER_GETSET* struct_param;

    struct_param = (PARAM_DRV_SONY_TUNER_GETSET*) pvArg;
    sreg = _sony_tuner_sreg_country_sel(struct_param->sreg);
    val = struct_param->val;

    if(sreg<SREG_TUNER_NVM){
        for (tbl_cnt=0; tbl_cnt<(sizeof(tbl_sony_tuner_sreg)/sizeof(tbl_sony_tuner_sreg[0])); tbl_cnt++){
            if(tbl_sony_tuner_sreg[tbl_cnt].sreg==sreg){
                i = tbl_cnt;
                break;
            }
        }
        //buf_ptr = sony_x_mem_alloc(sizeof(UINT8)*(tbl_sony_tuner_sreg[i].byte_size));
        buf_ptr = (UINT8)val;
        param.RegAddr = tbl_sony_tuner_sreg[i].regAddr;
        param.ByteCnt = tbl_sony_tuner_sreg[i].byte_size;
        param.pData   = &buf_ptr;
        vSonyTunerSetAGCReg(&param);
        if((param.Success)==reg_NOERR){

        }else{
    
        }
        //sony_x_mem_free(buf_ptr);
        //buf_ptr = NULL;
        main_sony_tuner_sreg_write(sreg,val);
    }
}

/****************************************************************************
*    End of file
****************************************************************************/
