/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, MediaTek Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: tee_securestorage.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/* ============================================================================
    Includes
============================================================================ */


#include "stdlib.h"
#include "string.h"
#include "tzst.h"
#include "tee_securestorage.h"
#include "tee_uli.h"

#ifdef TEE_ULI_CEM_MG
#include TEE_ULI_CEM_MG
#endif

#include <tomcrypt.h>

/* ============================================================================
    function definitions
============================================================================ */
#ifdef TEE_ULI_CEM_MG
#define TEE_NFLX_KPE_SIZE   (16)
#define TEE_NFLX_KPH_SIZE   (32)
#define TEE_PLAYREADY_ZGPRIV_SIZE   (84)
extern BYTE au1_TEE_NFLX_kpe[TEE_NFLX_KPE_SIZE];
extern BYTE au1_TEE_NFLX_kph[TEE_NFLX_KPH_SIZE];
extern TEE_SECURESTORAGE_USERKEY_T t_TEE_SS_SecretUserKey;

VOID _teeUliNetflixActionFctKpe(
    UINT8*     ps_in_buff,
    UINT32     ui4_in_buff_sz,
    UINT8*     ps_out_buff,
    UINT32*    pui4_out_buff_sz
);

VOID _teeUliNetflixActionFctKph(
    UINT8*     ps_in_buff,
    UINT32     ui4_in_buff_sz,
    UINT8*     ps_out_buff,
    UINT32*    pui4_out_buff_sz
);

VOID _teeUliPlayReadyActionFctZgpriv(
    UINT8*     ps_in_buff,
    UINT32     ui4_in_buff_sz,
    UINT8*     ps_out_buff,
    UINT32*    pui4_out_buff_sz
);
#endif

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define TEE_DUMP(x,y,z) Printf("\n[TEE_DUMP]%s:", (x)); \
    for(i = 0; i < (y); i++) { \
        if(i % 16 == 0) \
            Printf("\n"); \
        Printf("0x%02x ", (z)[i]); \
    } \
    Printf("\n\n");

#ifdef TEE_ULI_CEM_MG

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define TEE_ULI_OWNER_NAME_SZ     12
#define TEE_ULI_OBJECT_NAME_SZ    TEE_ULI_OWNER_NAME_SZ

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

typedef INT32 (* teeUliCSPAction_fct) (UINT8* ps_in_buff, UINT32 ui4_in_buff_sz, UINT8* ps_out_buff, UINT32* pui4_out_buff_sz);

typedef struct
{
    CHAR                   ac_owner[TEE_ULI_OWNER_NAME_SZ];
    CHAR                   ac_name[TEE_ULI_OBJECT_NAME_SZ];
    teeUliCSPAction_fct    fp_action_fct;
} TEE_ULI_CSP_ACTION_T;

/* ============================================================================
    Global Variable
============================================================================ */
static TEE_ULI_CSP_ACTION_T g_t_csp_action[] =
{
    {NETFLIX_OWNER,    NETFLIX_KPE,            _teeUliNetflixActionFctKpe},
    {NETFLIX_OWNER,    NETFLIX_KPH,            _teeUliNetflixActionFctKph},
    {PLAYREADY_OWNER1, PLAYREADY_DEVCERT_TEMP, NULL},
    {PLAYREADY_OWNER1, PLAYREADY_PRIV,         NULL},
    {PLAYREADY_OWNER2, PLAYREADY_BGROUP_CERT,  NULL},
    {PLAYREADY_OWNER2, PLAYREADY_ZGPRIV,       _teeUliPlayReadyActionFctZgpriv},
    {AMAZON_OWNER,     AMAZON_CRT,             NULL},
    {AMAZON_OWNER,     AMAZON_KEY,             NULL},
    {RHAPSODY_OWNER,   RHAPSODY_CRT,           NULL},
    {RHAPSODY_OWNER,   RHAPSODY_KEY,           NULL},
    {WIDEVINE_OWNER,   WIDEVINE_KEYBOX,        NULL},
};

/* ============================================================================
    Internal Functions
============================================================================ */

VOID _teeUliNetflixActionFctKpe(
    UINT8*     ps_in_buff,
    UINT32     ui4_in_buff_sz,
    UINT8*     ps_out_buff,
    UINT32*    pui4_out_buff_sz
)
{
    unsigned int i;
    Printf("_teeUliNetflixActionFctKpe Enter!!!\n");

    /* check the buffer length */
    if (ui4_in_buff_sz != TEE_NFLX_KPE_SIZE)
    {
        /* return directly */
        Printf("_teeUliNetflixActionFctKpe() Error!!!\n");
        return;
    }

    memcpy_s2s_chk(au1_TEE_NFLX_kpe, ps_in_buff, TEE_NFLX_KPE_SIZE, NULL);
    memset(ps_out_buff, 0, *pui4_out_buff_sz);

    return;
}

VOID _teeUliNetflixActionFctKph(
    UINT8*     ps_in_buff,
    UINT32     ui4_in_buff_sz,
    UINT8*     ps_out_buff,
    UINT32*    pui4_out_buff_sz
)
{
    unsigned int i;
    Printf("_teeUliNetflixActionFctKph Enter!!!\n");

    /* check the buffer length */
    if (ui4_in_buff_sz != TEE_NFLX_KPH_SIZE)
    {
        /* return directly */
        Printf("_teeUliNetflixActionFctKph() Error!!!\n");
        return;
    }

    memcpy_s2s_chk(au1_TEE_NFLX_kph, ps_in_buff, TEE_NFLX_KPH_SIZE, NULL);
    memset(ps_out_buff, 0, *pui4_out_buff_sz);

    return;
}

VOID _teeUliPlayReadyActionFctZgpriv(
    UINT8*     ps_in_buff,
    UINT32     ui4_in_buff_sz,
    UINT8*     ps_out_buff,
    UINT32*    pui4_out_buff_sz
)
{
    Printf("_teeUliPlayReadyActionFctZgpriv Enter!!!\n");

    /* check the buffer length */
    if (*pui4_out_buff_sz <= TEE_PLAYREADY_ZGPRIV_SIZE)
    {
        /* return directly */
        Printf("_teeUliNetflixActionFctKph() Error!!!\n");
        return;
    }

    /* Encrypt */
    _teeSecureStorageEnc(&t_TEE_SS_SecretUserKey, ps_in_buff, ui4_in_buff_sz, ps_out_buff, pui4_out_buff_sz);

    return;
}
#endif

/* ============================================================================
    Exported Entry Points
============================================================================ */
UINT32 _teeUliObjectDecrypt(
    CHAR*                        ps_owner_name,
    CHAR*                        ps_object_name,
    UINT8*                       pui1_in_buff,
    UINT32                       ui4_in_buff_sz,
    UINT8*                       pui1_out_buff,
    UINT32*                      pui4_out_buff_sz,
    TEE_ULI_ENCRYPTED_POTK_T*    pt_encrypted_potk,
    UINT32*                      pui4_clear_obj_sz
)
{
    UINT32                       ui4_ret;
    UINT8                        au1_clear_potk[16];
    UINT8                        au1_clear_iv[16];
    symmetric_CBC                t_sym_cbc;
    INT32                        i4_dst_len = 0;
    BOOL                         b_csp_action = FALSE;
    UINT8*                       pui1_tmp_buff = NULL;

    /* check the buffer size first */
    if (*pui4_out_buff_sz < ui4_in_buff_sz)
    {
        return TZ_ERROR_SHORT_BUFFER;
    }

    if (*pui4_out_buff_sz < *pui4_clear_obj_sz)
    {
        return TZ_ERROR_SHORT_BUFFER;
    }

    /* reset the buffer, IV are all always zeros */
    memset(&(au1_clear_potk[0]), 0, sizeof(au1_clear_potk));
    memset(&(au1_clear_iv[0]), 0, sizeof(au1_clear_iv));

    i4_dst_len = sizeof(au1_clear_potk);

    /* get the clear POTK key first */
    ui4_ret = _teeSecureStorageSecretDec(&(pt_encrypted_potk->au1_key[0]),
                                         sizeof(pt_encrypted_potk->au1_key),
                                         &(au1_clear_potk[0]),
                                         &i4_dst_len);
    if (TZ_SUCCESS != ui4_ret)
    {
        Printf("_teeSecureStorageSecretDec() fail, ui4_ret = %d\n", ui4_ret);

        return ui4_ret;
    }

    /* allocate the tmp buffer */
    pui1_tmp_buff = malloc(*pui4_clear_obj_sz);
    if (NULL == pui1_tmp_buff)
    {
        return TZ_ERROR_MEMORY;
    }

    /* then the content */
    cbc_start(find_cipher("aes"), &(au1_clear_iv[0]), &(au1_clear_potk[0]), sizeof(au1_clear_potk), 0, &t_sym_cbc);
    cbc_decrypt(pui1_in_buff, pui1_tmp_buff, ui4_in_buff_sz, &t_sym_cbc);
    cbc_done(&t_sym_cbc);

#ifdef TEE_ULI_CEM_MG
    UINT8    ui1_i;

    for (ui1_i = 0 ; ui1_i < (sizeof (g_t_csp_action) / sizeof(TEE_ULI_CSP_ACTION_T)) ; ui1_i++)
    {
        if (0 == memcmp((g_t_csp_action[ui1_i]).ac_owner, ps_owner_name, TEE_ULI_OWNER_NAME_SZ)
            && 0 == memcmp((g_t_csp_action[ui1_i]).ac_name, ps_object_name, TEE_ULI_OBJECT_NAME_SZ)
            && NULL != g_t_csp_action[ui1_i].fp_action_fct)
        {
            b_csp_action = TRUE;
            break;
        }
    }
#endif

    if (FALSE == b_csp_action)
    {
        *pui4_out_buff_sz = *pui4_clear_obj_sz;
        memcpy_s2n_chk(pui1_out_buff, pui1_tmp_buff, *pui4_clear_obj_sz, NULL);
    }
    else
    {
        g_t_csp_action[ui1_i].fp_action_fct(pui1_tmp_buff, *pui4_clear_obj_sz, pui1_out_buff, pui4_out_buff_sz);
    }

    /* free the tmp buffer */
    free(pui1_tmp_buff);

    return TZ_SUCCESS;
}

UINT32 _teeUlChkPOTK(
    TEE_ULI_ENCRYPTED_POTK_T*    pt_encrypted_potk,
    UINT8*                       pui1_buff
)
{
#ifdef HAVE_PRE_SHARED_SECRET
    UINT32                       ui4_ret;
    UINT8                        au1_clear_potk[16];
    UINT8                        au1_clear_iv[16];
    symmetric_CBC                t_sym_cbc;
    UINT8                        aui1_out_buff[16];
    INT32                        i4_dst_len = 0;
    unsigned int                 i;

    /* reset the buffer, IV are all always zeros */
    memset(&(au1_clear_potk[0]), 0, sizeof(au1_clear_potk));
    memset(&(au1_clear_iv[0]), 0, sizeof(au1_clear_iv));

    i4_dst_len = sizeof(au1_clear_potk);

    /* get the clear POTK key first */
    ui4_ret = _teeSecureStorageSecretDec(&(pt_encrypted_potk->au1_key[0]),
                                         sizeof(pt_encrypted_potk->au1_key),
                                         &(au1_clear_potk[0]),
                                         &i4_dst_len);
    if (TZ_SUCCESS != ui4_ret)
    {
        Printf("_teeSecureStorageSecretDec() fail, ui4_ret = %d\n", ui4_ret);

        return ui4_ret;
    }

    memset(&(aui1_out_buff[0]), 0, sizeof(aui1_out_buff));

    /* then check the pre-shard secret */
    cbc_start(find_cipher("aes"), &(au1_clear_iv[0]), &(au1_clear_potk[0]), sizeof(au1_clear_potk), 0, &t_sym_cbc);
    cbc_decrypt(pui1_buff, &(aui1_out_buff[0]), 16, &t_sym_cbc);
    cbc_done(&t_sym_cbc);

    /* compare with the pre-shared secret */
    return (0 == memcmp(&(aui1_out_buff[0]), g_au1_pre_shared_secret, sizeof(g_au1_pre_shared_secret))) ? TZ_SUCCESS : TZ_ERROR_CRYPTO;
#else
    return TZ_ERROR_CRYPTO;
#endif
}
