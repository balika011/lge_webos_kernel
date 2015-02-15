#ifndef _SONY_TUNER_
#define _SONY_TUNER_


enum{
    EEP_OFFSET_EMCS_TUNER_DRV_CS        = (EEP_OFFSET_EMCS_SONY_TUNER_START),       /*0x2A10,            1 byte*/
    EEP_OFFSET_EMCS_PRA_RFMAX           = (EEP_OFFSET_EMCS_TUNER_DRV_CS + 1),       /*0x2A11,            1 byte*/
    EEP_OFFSET_EMCS_PRA_IFMAX           = (EEP_OFFSET_EMCS_PRA_RFMAX + 1),          /*0x2A12,            1 byte*/
    EEP_OFFSET_EMCS_PRA_RFMIN           = (EEP_OFFSET_EMCS_PRA_IFMAX + 1),          /*0x2A13,            1 byte*/
    EEP_OFFSET_EMCS_PRA_IFMIN           = (EEP_OFFSET_EMCS_PRA_RFMIN + 1),          /*0x2A14,            1 byte*/
    EEP_OFFSET_EMCS_PRA_IFMIN_CIS       = (EEP_OFFSET_EMCS_PRA_IFMIN + 1),          /*0x2A15,            1 byte*/
};

enum{
    /*NVM related SREG*/
    SREG_TUNER_PRA_RFMAX,
    SREG_TUNER_PRA_IFMAX,
    SREG_TUNER_PRA_RFMIN,
    SREG_TUNER_PRA_IFMIN,
    SREG_TUNER_PRA_IFMIN_CIS,
    SREG_TUNER_NVM, //dummy
    /*SREG for HW*/
    SREG_TUNER_OVM,
    SREG_TUNER_CR_STATE,
    SREG_TUNER_CFO1,
    SREG_TUNER_CFO2,
    SREG_TUNER_DIV,
    SREG_TUNER_DATA3,
    SREG_TUNER_DATA4,
    SREG_TUNER_COUNTRY_SEL,

    SREG_TUNER_MAX
};
typedef struct _PARAM_DRV_SONY_TUNER_GETSET
{
  UINT16  sreg;
  UINT16  val;
} PARAM_DRV_SONY_TUNER_GETSET;
typedef enum
{
    l_TU_FreqData,
    l_TU_StepData,
    l_TU_BandData,
} DRV_SONY_TUNER_DATA_local;
typedef struct _PARAM_ST_SONY_TUNER_DATA
{
    UINT8   TU_DATA;
    UINT16  value;
    BOOL    success;
} PARAM_ST_SONY_TUNER_DATA;
typedef enum
{
    TUNER_CNTY_OTHER,
    TUNER_CNTY_CIS,
} DRV_SONY_TUNER_Country;

#endif
