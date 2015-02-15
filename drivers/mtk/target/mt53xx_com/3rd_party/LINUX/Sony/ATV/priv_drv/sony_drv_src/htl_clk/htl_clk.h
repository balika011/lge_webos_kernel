#ifndef HTL_CLK_H
#define HTL_CLK_H
#include "x_typedef.h"
#include "../../../../../../../../../custom/Sony/2K12/res/app_util/config/acfg_atv_eep.h"

/* Configurations */
#define HTL_CLK_DEV_ADDR 0x50
#define HTL_CLK_SUB_ADDR_CONFIG 0x10
#define HTL_CLK_SUB_ADDR_SET_TIME 0x00
#define HTL_CLK_SUB_ADDR_READ_TIME 0x00
#define HTL_CLK_SUB_ADDR_GET_VERSION 0x0C
#define SIF_CLK_DIV 0x100
#define HTL_CLK_EEPROM_CHKSUM 0x31

/* EEPROM Utilization */
enum{
    HTL_CLK_EEPROM_HTLCLK_MODEL     = (EEP_OFFSET_EMCS_HOTEL_DRV_START),    /*0x3000, 2 byte*/
    EEP_OFFSET_EMCS_HTL_DRV_MAX_IDX = (EEP_OFFSET_EMCS_HOTEL_DRV_END)
};

#endif /* HTL_CLK_H */
