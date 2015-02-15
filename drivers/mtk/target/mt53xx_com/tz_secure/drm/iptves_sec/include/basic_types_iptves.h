/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008 Sony Corporation
 */



#ifndef BASIC_TYPES_IPTVES_H

#define BASIC_TYPES_IPTVES_H


#ifndef __linux__
#define __linux__
#endif


#include <po_basictypes.h>
#include <basic_types_iptves_.h>


#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/
/**
 * @file
 * Basic type and macro definition (PO internal)
 *
 * require: none
 */
/*==========================================================================*/

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
/**
 * def: Configuarable Values
 */
#define PO_MAX_CONNECTION       4
#define PO_MAX_CERT_LEN         (1024 * 2)
#define PO_MAX_CRL_LEN          (1024 * 5)
#define PO_MAX_REQ_LEN          (1024 * 3)
#define PO_MAX_RES_LEN          (1024 * 3)
#define PO_MAX_CRL_DP_LEN       256

/*============================================================================
 * Type Definitions
 *==========================================================================*/
/**
 * def: error code
 */
//typedef u_int32_t               int;

/*============================================================================
 * Error Codes
 *==========================================================================*/
/*============================================================================
 * ERROR MAP
 *
 *     3322 2222 2222 1111 1111 1100 0000 0000
 *     1098 7654 3210 9876 5432 1098 7654 3210
 *    |    |    |         |
 *     A   B    C         D
 *
 *    A: Layer          (4bit  : 0 - 15)
 *    B: Module         (4bit  : 0 - 15)
 *    C: Component      (8bit  : 0 - 255)
 *    D: Error          (16bit : 0 - 65535)
 *===========================================================================*/

/*--------- SHIFT SIZE ------------------------------------------------------*/
#define RET_LYR_SFT             28
#define RET_MOD_SFT             24
#define RET_CMP_SFT             16

/*--------- ERROR MASK ------------------------------------------------------*/
#define RET_LYR_MASK            0xF0000000
#define RET_MOD_MASK            0x0F000000
#define RET_CMP_MASK            0x00FF0000
#define RET_BASE_MASK           0xFFFF0000

#define RET_LYR(err)            ((err) & RET_LYR_MASK)
#define RET_MOD(err)            ((err) & RET_MOD_MASK)
#define RET_BASE(err)           ((err) & RET_BASE_MASK)

/*--------- ERROR LAYER -----------------------------------------------------*/
#define RET_LYR_CMN             (1<<RET_LYR_SFT)
#define RET_LYR_OUTSIDE         (2<<RET_LYR_SFT)
#define RET_LYR_SEC             (3<<RET_LYR_SFT)
#define RET_LYR_NSEC            (4<<RET_LYR_SFT)

/*--------- ERROR MODULE ----------------------------------------------------*/

/*--------- ERROR COMPONENT -------------------------------------------------*/
/*----- CMN -----*/
#define RET_CMP_FIO             (1<<RET_CMP_SFT)
#define RET_CMP_STRM            (2<<RET_CMP_SFT)
/*----- OUTSIDE -----*/
#define RET_CMP_HTTP            (1<<RET_CMP_SFT)
#define RET_CMP_CRYPTO          (2<<RET_CMP_SFT)
#define RET_CMP_TIME            (3<<RET_CMP_SFT)
#define RET_CMP_REND            (4<<RET_CMP_SFT)
/*----- SEC ----*/
#define RET_CMP_ASN             (1<<RET_CMP_SFT)
#define RET_CMP_CERT            (2<<RET_CMP_SFT)
#define RET_CMP_NVS             (3<<RET_CMP_SFT)
#define RET_CMP_CACHE           (4<<RET_CMP_SFT)
#define RET_CMP_KEYMNG          (5<<RET_CMP_SFT)
#define RET_CMP_CERTMNG         (6<<RET_CMP_SFT)
#define RET_CMP_ECM             (7<<RET_CMP_SFT)
#define RET_CMP_DRM             (8<<RET_CMP_SFT)
#define RET_CMP_SDI             (9<<RET_CMP_SFT)
#define RET_CMP_SP              (10<<RET_CMP_SFT)
#define RET_CMP_SAC             (11<<RET_CMP_SFT)

/*--------- ERROR BASE -----------------------------------------------------*/
/*
 * Cmn
 */
#define RET_FIO_BASE            (RET_LYR_CMN|RET_CMP_FIO)
#define RET_STRM_BASE           (RET_LYR_CMN|RET_CMP_STRM)

/*
 * outside
 */
#define RET_HTTP_BASE           (RET_LYR_OUTSIDE|RET_CMP_HTTP)
#define RET_CRYPTO_BASE         (RET_LYR_OUTSIDE|RET_CMP_CRYPTO)
#define intIME_BASE           (RET_LYR_OUTSIDE|RET_CMP_TIME)
#define RET_REND_BASE           (RET_LYR_OUTSIDE|RET_CMP_REND)

/*
 * sec
 */
#define RET_ASN_BASE            (RET_LYR_SEC|RET_CMP_ASN)
#define RET_CERT_BASE           (RET_LYR_SEC|RET_CMP_CERT)
#define RET_NVS_BASE            (RET_LYR_SEC|RET_CMP_NVS)
#define RET_CACHE_BASE          (RET_LYR_SEC|RET_CMP_CACHE)
#define RET_KEYMNG_BASE         (RET_LYR_SEC|RET_CMP_KEYMNG)
#define RET_CERTMNG_BASE        (RET_LYR_SEC|RET_CMP_CERTMNG)
#define RET_ECM_BASE            (RET_LYR_SEC|RET_CMP_ECM)
#define RET_DRM_BASE            (RET_LYR_SEC|RET_CMP_DRM)
#define RET_SDI_BASE            (RET_LYR_SEC|RET_CMP_SDI)
#define RET_SP_BASE             (RET_LYR_SEC|RET_CMP_SP)
#define RET_SAC_BASE            (RET_LYR_SEC|RET_CMP_SAC)

/*--------- ERROR BASIC ----------------------------------------------------*/

#define RET_OK                  0
#define RET_NG                  1
#define RET_NOMEM               2
#define RET_ILLEGALARGS         3
#define RET_ILLEGALHANDLE       4
#define RET_UNSUPPORTED         5
#define RET_INTERNALERROR       6

/*============================================================================
 * APIs
 *==========================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* BASIC_TYPES_H */
