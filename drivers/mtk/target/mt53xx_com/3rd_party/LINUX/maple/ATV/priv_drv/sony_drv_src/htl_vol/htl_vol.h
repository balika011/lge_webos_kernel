#ifndef HTL_VOL_H
#define HTL_VOL_H
#include "x_typedef.h"
//#include "../../../../../../../../custom/Sony/2K12/res/app_util/config/acfg_atv_eep.h"
#include "acfg_atv_eep.h"

#define HTL_VOL_EEPROM_CHKSUM 0x61 /* Just any constant number, since my lucky number is 7, so I took 61 */

/* EEPROM Utilization */
enum{
    HTL_VOL_EEPROM_HTLVOL_MODEL     = (EEP_OFFSET_EMCS_HTLVOL_MODEL),    /*0x3002, 1 byte*/
    HTL_VOL_EEPROM_HTLVOL_INITVOL   = (EEP_OFFSET_EMCS_HTLVOL_INITVOL),  /*0x3003, 1 byte*/
    HTL_VOL_EEPROM_HTLVOL_STEPTIMER = (EEP_OFFSET_EMCS_HTLVOL_STEPTIMER),/*0x3004, 1 byte*/
    HTL_VOL_EEPROM_HTLVOL_CHECKSUM  = (EEP_OFFSET_EMCS_HTLVOL_CHECKSUM), /*0x3005, 1 byte*/
    EEP_OFFSET_EMCS_HTL_DRV_MAX_IDX = (EEP_OFFSET_EMCS_HOTEL_DRV_END)
};

#endif /* HTL_VOL_H */
