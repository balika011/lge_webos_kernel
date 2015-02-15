/****************************************************************************/
/* (c) Copyright Ocean Blue Software Ltd. - ALL RIGHTS RESERVED             */
/* This document is the CONFIDENTIAL property of Ocean Blue Software Ltd.   */
/****************************************************************************/
/*                                                                          */
/*   FILE:    cccert.h                                                      */
/*   DATE:    21/04/09                                                      */
/*   PURPOSE: CI+ Content Control Resource: Certificate Processing          */
/*                                                                          */
/****************************************************************************/
#ifndef __CCCERT_H_
#define __CCCERT_H_

/*--------------------------------------------------------------------------*/
/* Symbol Definitions                                                       */
/*--------------------------------------------------------------------------*/

typedef enum
{
   CC_CERT_CHK_OK,
   CC_CERT_CHK_EXPIRED,
   CC_CERT_CHK_SUBJECT_ISSUER_MISMATCH,
   CC_CERT_CHK_INVALID_DATE_TIME,
   CC_CERT_CHK_INVALID_EXTENSION,
   CC_CERT_CHK_MISSING_EXTENSION,
   CC_CERT_CHK_INVALID_SIGNATURE
} E_CC_CERT_CHK;

/*--------------------------------------------------------------------------*/
/* Macros Definitions                                                       */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Structures Definitions                                                   */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Function Prototypes                                                      */
/*--------------------------------------------------------------------------*/

/*!**************************************************************************
 * @brief    Check CI+ certificates
 * @param    root_cert - CI+ root certificate
 * @param    brand_cert - CI+ brand certificate
 * @param    device_cert - CI+ device certificate
 * @return   Result of certificate check
 ****************************************************************************/
E_CC_CERT_CHK CC_CheckCertificates(void *root_cert, void *brand_cert,
                                   void *device_cert,UINT32 epoch_time);

#endif   /* !__CCCERT_H */
