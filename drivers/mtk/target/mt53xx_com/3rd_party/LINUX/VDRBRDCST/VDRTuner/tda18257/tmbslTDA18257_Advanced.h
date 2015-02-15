/*
  Copyright (C) 2010 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmbslTDA18257_Advanced.h
 *
 * \date          %modify_time%
 *
 * \author        Alexandre TANT
 *
 * \brief         Describe briefly the purpose of this file.
 *
 * REFERENCE DOCUMENTS :
 *                TDA18257_Driver_User_Guide.pdf
 *
 * \section info Change Information
 *
*/

#ifndef _TMBSL_TDA18257_ADVANCED_H
#define _TMBSL_TDA18257_ADVANCED_H

#ifdef __cplusplus
extern "C"
{
#endif


/*============================================================================*/
/* Types and defines:                                                         */
/*============================================================================*/

/*============================================================================*/
/* Exported functions:                                                        */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18257_GetSWVersion(
    ptmSWVersion_t  pSWVersion  /*  I: Receives SW Version */
);

tmErrorCode_t
tmbslTDA18257_GetSWSettingsVersion(
    ptmSWSettingsVersion_t pSWSettingsVersion   /* O: Receives SW Settings Version */
);

tmErrorCode_t
tmbslTDA18257_SetGpio(
    tmUnitSelect_t          tUnit,           /* I: Unit number */
    UInt8                   bNumber,         /* I: Number of the GPIO to set */
    Bool                    bActive          /* I: GPIO enabled/disabled */
);

tmErrorCode_t
tmbslTDA18257_GetPowerState(
    tmUnitSelect_t  tUnit,      /* I: Unit number */
    tmPowerState_t* pPowerState /* O: Power state */
);

tmErrorCode_t
tmbslTDA18257_GetStandardMode(
    tmUnitSelect_t              tUnit,          /*  I: Unit number */
    TDA18257StandardMode_t    *pStandardMode  /*  O: Standard mode of this device */
);

tmErrorCode_t
tmbslTDA18257_GetRF(
    tmUnitSelect_t  tUnit,  /*  I: Unit number */
    UInt32*         pRF     /*  O: RF frequency in hertz */
);

tmErrorCode_t
tmbslTDA18257_GetCF_Offset(
    tmUnitSelect_t  tUnit,      /* I: Unit number */
    UInt32*         puOffset    /* O: Center frequency offset in hertz */
);

tmErrorCode_t
tmbslTDA18257_GetAgcStatus(
    tmUnitSelect_t          tUnit,       /* I: Unit number */
    Bool*                   pbAgcLock,   /* O: AGC Lock status */
    Bool*                   pbAgcMax     /* O: AGC Max status */
);

tmErrorCode_t
tmbslTDA18257_SetPllManual(
    tmUnitSelect_t  tUnit,         /* I: Unit number */
    Bool            bOverridePLL   /* I: Determine if we need to put PLL in manual mode in SetRF */
);

tmErrorCode_t
tmbslTDA18257_GetPllManual(
    tmUnitSelect_t  tUnit,         /* I: Unit number */
    Bool*           pbOverridePLL  /* O: Determine if we need to put PLL in manual mode in SetRF */
);

tmErrorCode_t
tmbslTDA18257_GetIRQ(
    tmUnitSelect_t  tUnit  /* I: Unit number */,
    Bool*           pbIRQ  /* O: IRQ triggered */
);

tmErrorCode_t
tmbslTDA18257_WaitIRQ(
    tmUnitSelect_t  tUnit,      /* I: Unit number */
    UInt32          timeOut,    /* I: timeOut for IRQ wait */
    UInt32          waitStep,   /* I: wait step */
    UInt8           irqStatus   /* I: IRQs to wait */
);

tmErrorCode_t
tmbslTDA18257_GetXtalCal_End(
    tmUnitSelect_t  tUnit           /* I: Unit number */,
    Bool*           pbXtalCal_End   /* O: XtalCal_End triggered */
);

tmErrorCode_t
tmbslTDA18257_SetRawRF(
    tmUnitSelect_t  tUnit,  /* I: Unit number */
    UInt32          uRF     /* I: RF frequency in hertz */
);

tmErrorCode_t
tmbslTDA18257_GetAGCGainRead(
    tmUnitSelect_t  tUnit,      /* I: Unit number */
    UInt8*          pAGC1Read,  /* O: AGC1 Read */
    UInt8*          pAGC2Read,  /* O: AGC2 Read */
    UInt8*          pAGC3Read   /* O: AGC3 Read */

);

tmErrorCode_t
tmbslTDA18257_GetRSSI(
    tmUnitSelect_t  tUnit,  /*  I: Unit number */
    UInt8*          puValue /*  O: Address of the variable to output item value */
);

tmErrorCode_t
tmbslTDA18257_SetLLPowerState(
    tmUnitSelect_t          tUnit,      /* I: Unit number */
    TDA18257PowerState_t    powerState  /* I: Power state of TDA18257 */
 );

tmErrorCode_t
tmbslTDA18257_GetLLPowerState(
    tmUnitSelect_t          tUnit,      /* I: Unit number */
    TDA18257PowerState_t*   pPowerState /* O: Power state of TDA18257 */
);

tmErrorCode_t
tmbslTDA18257_PLLcalc(
    tmUnitSelect_t  tUnit,      /* I: Unit number */
	UInt32          LO /*I input LO in KHz */
	);

tmErrorCode_t
tmbslTDA18257_WriteRegMap(
    tmUnitSelect_t  tUnit,      /* I: Unit number */
    UInt8           uAddress,   /* I: Data to write */
    UInt32          uWriteLen   /* I: Number of data to write */
);

tmErrorCode_t
tmbslTDA18257_ReadRegMap(
    tmUnitSelect_t  tUnit,      /* I: Unit number */
    UInt8           uAddress,   /* I: Data to read */
    UInt32          uReadLen    /* I: Number of data to read */
);

tmErrorCode_t
tmbslTDA18257_SetRegMap(
    tmUnitSelect_t  tUnit,      /* I: Unit number */
    UInt8           uAddress,   /* I: Data start address */
    UInt32          uDataLen,   /* I: Number of data */
    UInt8*          puData      /* I: Data to set */
);

tmErrorCode_t
tmbslTDA18257_GetRegMap(
    tmUnitSelect_t  tUnit,      /* I: Unit number */
    UInt8           uAddress,   /* I: Data start address */
    UInt32          uDataLen,   /* I: Number of data */
    UInt8*          puData      /* I: Data to get */
);


tmErrorCode_t
tmbslTDA18257_Write(
    tmUnitSelect_t              tUnit,      /* I: Unit number */
    const TDA18257_BitField_t*  pBitField,  /* I: Bitfield structure */
    UInt8                       uData,      /* I: Data to write */
    tmbslFrontEndBusAccess_t    eBusAccess  /* I: Access to bus */
);

tmErrorCode_t
tmbslTDA18257_Read(
    tmUnitSelect_t              tUnit,      /* I: Unit number */
    const TDA18257_BitField_t*  pBitField,  /* I: Bitfield structure */
    UInt8*                      puData,     /* I: Data to read */
    tmbslFrontEndBusAccess_t    eBusAccess  /* I: Access to bus */
);

tmErrorCode_t
tmbslTDA18257_GetPLLState(
    tmUnitSelect_t          tUnit,      /* I: Unit number */
    tmbslFrontEndState_t*   pLockStatus /* O: PLL Lock status */
);

tmErrorCode_t
tmbslTDA18257_SetXtal(
    tmUnitSelect_t  tUnit,      /* I: Unit number */
	TDA18257XtalFreq_t eXtal   /*I: Xtal freq */
);

tmErrorCode_t
tmbslTDA18257_GetXtal(
    tmUnitSelect_t  tUnit,      /* I: Unit number */
	TDA18257XtalFreq_t *peXtal /*O: Xtal freq */
);

tmErrorCode_t
tmbslTDA18257_AGCTOP23(
    tmUnitSelect_t              tUnit,
    Int8                        lDelta
);

tmErrorCode_t
tmbslTDA18257_GetMasterContribution(
    tmUnitSelect_t tUnit,
    UInt32 uRF,
    Int32* plDeltaGain
);

tmErrorCode_t
tmbslTDA18257_SetMasterContributionPointer(
    tmUnitSelect_t tUnit,
    GetMasterContributionPointer getMasterContribution
);

tmErrorCode_t
tmbslTDA18257_SetScanningMode(
    tmUnitSelect_t tUnit,
    TDA18257Scanning_Mode_t eScanningMode
);

tmErrorCode_t
tmbslTDA18257_GetScanningMode(
    tmUnitSelect_t tUnit,
    TDA18257Scanning_Mode_t *peScanningMode
);

#ifdef __cplusplus
}
#endif

#endif /* _TMBSL_TDA18257_ADVANCED_H */

