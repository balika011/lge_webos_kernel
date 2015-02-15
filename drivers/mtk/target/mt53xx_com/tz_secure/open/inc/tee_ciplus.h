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
 * $RCSfile: tee_securestorage.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

#ifndef TEE_CIPLUS_H
#define TEE_CIPLUS_H
/* ============================================================================
    Includes
============================================================================ */

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Function definitions
//-----------------------------------------------------------------------------
BOOL _teeCIPLUSExtractCred(UINT8 *pu1SrcBuf, UINT32 u4SrcLen,UINT8 *pu1CustCode, UINT32 u4CustCodeLen);
BOOL _teeCIPLUSSetEncryAKH(UINT8 *pu1SrcBuf, UINT32 u4SrcLen);
BOOL _teeCIPLUSGetEncryAKH(UINT8 *pu1DstBuf, UINT32* pu4SrcLen);
BOOL _teeCILPLUSVerifyCred(UINT8 *pu1SrcBuf, UINT32 u4SrcLen,UINT8 *pu1Ref, UINT32 u4RefLen,BOOL *pbValid);
BOOL _teeCIPLUSCheckCred(UINT8 *pu1SrcBuf, UINT32 u4SrcLen,UINT8 *pu1CustCode, UINT32 u4CustCodeLen,BOOL *pbValid);
BOOL _teeCIPLUS_DRM_Init(UINT32 *drm_handle,UINT32* err_code);
BOOL _teeCIPLUS_DRM_Remove(UINT32 drm_handle,UINT32* err_code);
BOOL _teeCIPLUS_DRM_Get_HostDevCert(UINT32 drm_handle, UINT8 *dev_certificate,UINT16 *dev_cert_len,UINT32* err_code);
BOOL _teeCIPLUS_DRM_Get_HostBrandCert(UINT32 drm_handle, UINT8 *dev_brand_certificate, UINT16 *dev_brand_cert_len,UINT32* err_code);
BOOL _teeCIPLUS_DRM_Set_UTC_Time(UINT32 drm_handle,UINT32 epoch_time,UINT32* err_code);
BOOL _teeCIPLUS_DRM_Verify_ModuleCert(UINT32 drm_handle, UINT8 *dev_certificate, UINT32 dev_cert_len, UINT8 *dev_brand_certificate, UINT32 dev_brand_cert_len,UINT32* err_code);
BOOL _teeCIPLUS_DRM_Sign_Message(UINT32 drm_handle, UINT8 *auth_nonce, UINT16 auth_nonce_len,UINT8 *SignMsg, UINT16 *Signlen,UINT32 *err_code);
BOOL _teeCIPLUS_DRM_Verify_SignatureB(UINT32 drm_handle, UINT8 *DHPM,UINT16 DHPM_len, UINT8* SignMsg, UINT16 Signlen,UINT32 *err_code);
BOOL _teeCIPLUS_DRM_Get_Dhph(UINT32 drm_handle, UINT8* DHPH,UINT16 *DHPH_len,UINT32 *err_code);
BOOL _teeCIPLUS_DRM_Get_HostId(UINT32 drm_handle, UINT8 *host_id,UINT16 *host_id_len,UINT32 *err_code);
BOOL _teeCIPLUS_DRM_Get_CamId(UINT32 drm_handle, UINT8 *cam_id,UINT16* cam_id_len,UINT32 *err_code);
BOOL _teeCIPLUS_DRM_Get_CamBrandId(UINT32 drm_handle,UINT16 *brand_id,UINT32 *err_code);
BOOL _teeCIPLUS_DRM_Generate_Akh(UINT32 drm_handle, UINT8 *AKH,UINT16 *AKH_len,UINT32 *err_code);
BOOL _teeCIPLUS_DRM_Generate_DHSK(UINT32 drm_handle,UINT32 *err_code);
BOOL _teeCIPLUS_DRM_Generate_SacKeys(UINT32 drm_handle,UINT8 *Ns_Module, UINT16 Ns_Module_len,UINT8 *CICAM_ID,UINT16 CICAM_ID_len,UINT32 *err_code);
BOOL _teeCIPLUS_DRM_Get_Ns(UINT32 drm_handle, UINT8 *Ns_Host,UINT16 *Ns_Host_len,UINT32 *err_code);
BOOL _teeCIPLUS_DRM_Encrypt_MessageSac(UINT32 drm_handle, UINT8* clearmsg, UINT16 clear_msg_len, UINT8 *enc_msg, UINT16 *enc_msg_len,UINT32 *err_code);
BOOL _teeCIPLUS_DRM_Decrypt_MessageSac(UINT32 drm_handle, UINT8 *encmsg, UINT16 enc_msg_length, UINT8 *dec_msg, UINT16 *dec_msg_length, UINT8 *decrypted,UINT32 *err_code);
BOOL _teeCIPLUS_DRM_Get_KeyId(UINT32 drm_handle, UINT32 * puKeyID,UINT32 *err_code);
bool _teeCIPLUS_DRM_Data_Confirm(UINT32 drm_handle, UINT8* data, UINT16 data_length, UINT8* confirm, UINT16* confirm_length,UINT32 *err_code);
BOOL _teeCIPLUS_DRM_Get_NextAuthContext(UINT32 drm_handle,UINT8 *AKH,UINT16 *AKH_len,UINT8 *cam_id,UINT16 *cam_id_len,UINT16 *cam_brand_id,UINT32 *err_code);

BOOL _teeCIPLUSGetCCK(UINT32 key_id,UINT8 *cipher,UINT8* key_register,UINT8 *cck, UINT16* cck_len,UINT8 *civ,UINT16* civ_len);


#endif
