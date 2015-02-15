/*****************************************************************************
* (c) Copyright Ocean Blue Software Ltd. - ALL RIGHTS RESERVED
* This document is the CONFIDENTIAL property of Ocean Blue Software Ltd.
******************************************************************************
*
* File Name:     stbhwci.h
*
* Description:   Header File for DVB-CI low-level Physical layer
*
* Project:       Common Interface Library
*
* Creation Date: 26/03/2001
*
******************************************************************************/


#ifndef STBCIHW_H_
#define STBCIHW_H_
#include "mtk_util.h"
#include "x_os.h"
#include "x_assert.h"
#include "tzst.h"

#include "techtype.h"
#ifdef __cplusplus
extern "C"
{
#endif

/* Host credentials and licence constants */
typedef enum 
{
  STB_CI_KEY_ROOT_CERT,
  STB_CI_KEY_BRAND_CERT,
  STB_CI_KEY_DEVICE_CERT,
  STB_CI_KEY_PRNG_SEED,
  STB_CI_KEY_PRNG_KEY_K,
  STB_CI_KEY_DH_P,
  STB_CI_KEY_DH_G,
  STB_CI_KEY_DH_Q,
  STB_CI_KEY_HDQ,
  STB_CI_KEY_SIV,
  STB_CI_KEY_SLK,
  STB_CI_KEY_CLK
} E_STB_CI_KEY_TYPE;

/*****************************************************************************
 * @brief    Return CI+ host key
 * @param    type - type of host key
 * @param    key - pointer to the key data
 * @param    length - number of bytes in key data
 * @note     The pointer must remain valid while the CI+ stack is running
 * @param    slot_id - Zero-based CI slot identifier (0, 1, ...)
 *****************************************************************************/
void STB_CIGetHostKey(E_STB_CI_KEY_TYPE type, U8BIT **key, U16BIT *length);
/*****************************************************************************
 * @brief    Read data from secure non-volatile area
 * @param    buffer - pointer to data buffer to read into
 * @param    len - number of bytes to read
 * @return   TRUE if read operation was successful, FALSE otherwise
 *****************************************************************************/
BOOL STB_CIReadSecureNVM(U8BIT* buffer, U32BIT len);

/*****************************************************************************
 * @brief    Write data into secure non-volatile area
 * @param    buffer - pointer to data buffer to write
 * @param    len - number of bytes to write
 * @return   TRUE if read operation was successful, FALSE otherwise
 *****************************************************************************/
BOOL STB_CIWriteSecureNVM(U8BIT* buffer, U32BIT len);
void STB_CIResetAuthKey(void);
#ifdef __cplusplus
 }
#endif
#endif   /* STBHWCI_H_ */

/******************************************************************************
 * End of File
 *****************************************************************************/
