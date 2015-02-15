#ifndef CI_TRUST_ZONE_H_
#define CI_TRUST_ZONE_H_

#define  U8BIT        UINT8
#define  U16BIT       UINT16
#define  U32BIT       UINT32

/* Host credentials and licence constants */
typedef enum tz_stb_ciplus_key_type
{
  TZ_STB_CIPLUS_KEY_ROOT_CERT,
  TZ_STB_CIPLUS_KEY_BRAND_CERT,
  TZ_STB_CIPLUS_KEY_DEVICE_CERT,
  TZ_STB_CIPLUS_KEY_PRNG_SEED,
  TZ_STB_CIPLUS_KEY_PRNG_KEY_K,
  TZ_STB_CIPLUS_KEY_DH_P,
  TZ_STB_CIPLUS_KEY_DH_G,
  TZ_STB_CIPLUS_KEY_DH_Q,
  TZ_STB_CIPLUS_KEY_HDQ,
  TZ_STB_CIPLUS_KEY_SIV,
  TZ_STB_CIPLUS_KEY_SLK,
  TZ_STB_CIPLUS_KEY_CLK
} E_TZ_STB_CIPLUS_KEY_TYPE;


typedef U32BIT CI_CC_RESULT_T;
#define CI_CC_OK                      ((CI_CC_RESULT_T)    0)    /**< The routine is successful     */
#define CI_CC_FAILED                  ((CI_CC_RESULT_T)    -1)    /**<The routine is failed         */
#define CI_CC_INV_ARG                 ((CI_CC_RESULT_T)    -2)    /**< Indicates at least one argument is invalid. */
#define CI_CC_CERT_EXPIRED            ((CI_CC_RESULT_T)    -3)    /**< Indicates at certificate is expired. */
#define CI_CC_CERT_ERROR              ((CI_CC_RESULT_T)    -4)    /**< Indicates at certificate has error. */
#define CI_CC_NOT_ENOUGH_SPACE        ((CI_CC_RESULT_T)    -5)    /**< Indicates at input buffer size is not enough */

typedef UINT8 CC_CI_PLUS_KEY_TYPE;
#define CC_CI_PLUS_KEY_TYPE_TEST                  ((CC_CI_PLUS_KEY_TYPE)    0)    /**<TV ci plus key config to test key        */
#define CC_CI_PLUS_KEY_TYPE_PRODUCT               ((CC_CI_PLUS_KEY_TYPE)    1)    /**< TV ci plus key config to product key    */
/***************************************************************************
    Description:
        To Initialize the CI PLus DRM Context.

    Input Parameters:
        None
    Output Parameters:
        drm_handle: DRM Context Handle

    Return Value:
        CI_CC_INV_ARG: input drm_handle is NULL
        CI_CC_NOT_ENOUGH_SPACE:memory is not enough
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully
**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_init(U32BIT *drm_handle);


/***************************************************************************
    Description:
        Removes the CI PLus DRM Context.

    Input Parameters:
        drm_handle: DRM Context Handle

    Output Parameters:
        Nil
    Return Value:    
        CI_CC_INV_ARG: input drm_handle is NULL
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_remove(U32BIT drm_handle);



/***************************************************************************
    Description:
        Gets the content of Host Certificate in Memory Buffer.

    Input Parameters:
        drm_handle: DRM Context Handle
        dev_cert_len:dev_certificate buffer length    

    Output Parameters:
        dev_certificate: Memory Buffer Pointer for Device Certificate
        dev_cert_len: actual Device Certificate Length
    Return Value:
        CI_CC_INV_ARG: input drm_handle is not valid or dev_certificate,dev_cert_len is NULL
        CI_CC_NOT_ENOUGH_SPACE:dev_cert_len is not enough, in this case dev_cert_len will be output to actual needed buffer size
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_getHostDevCertificates(U32BIT drm_handle, U8BIT *dev_certificate, U16BIT *dev_cert_len);

/***************************************************************************
    Description:
        Gets the content of Host Brand Certificate in Memory Buffer.

    Input Parameters:
        drm_handle: DRM Context Handle
        dev_brand_cert_len:dev_brand_certificate buffer length

    Output Parameters:
        dev_brand_certificate: Memory Buffer Pointer for Brand Certificate
        dev_brand_cert_len: actual Brand Certificate Length
   Return Value:
       CI_CC_INV_ARG: input drm_handle is not valid or dev_brand_certificate,dev_brand_cert_len is NULL
       CI_CC_NOT_ENOUGH_SPACE:dev_cert_len is not enough, in this case dev_brand_cert_len will be output to actual needed buffer size
       CI_CC_FAILED:failed
       CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_getHostBrandCertificates(U32BIT drm_handle, U8BIT *dev_brand_certificate, UINT16 *dev_brand_cert_len);

CI_CC_RESULT_T ci_cc_drm_setUTCTime(U32BIT drm_handle,UINT32 epoch_time);
/***************************************************************************
    Description:
        Verifies the CICAM Module and Brand Certificate with Root Certificate.

    Input Parameters:
        drm_handle: DRM Context Handle
        dev_certificate: Buffer having Module Certificate
        dev_cert_len: Module Certificate Length
        dev_brand_certificate: Buffer having Brand Certificate
        dev_brand_cert_len: Brand Certificate Length

    Output Parameters:
        Nil
    Return Value:
        CI_CC_INV_ARG: input drm_handle is not valid or dev_certificate,dev_brand_certificate is NULL
        CI_CC_CERT_EXPIRED:CAM certificate expired
        CI_CC_CERT_ERROR:CAM cerdificate has error
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_verifyModuleCertificates(U32BIT drm_handle, U8BIT *dev_certificate, U32BIT dev_cert_len, U8BIT *dev_brand_certificate, U32BIT dev_brand_cert_len);


/***************************************************************************
    Description:
        To sign a Message.

    Input Parameters:
        drm_handle: DRM Handle Context
        auth_nonce: Random Number
       auth_nonce_len:auth_nonce buffer length       
       Signlen: input SingMsg buffer length

    Output Parameters:
        SignMsg: Signed Message
        Signlen: Signed Message Length
    Return Value:
        CI_CC_INV_ARG: input drm_handle is not valid or auth_nonce,SignMsg ,Signlen is NULL
        CI_CC_NOT_ENOUGH_SPACE:Signlen is not enough, in this case Signlen will be output to actual needed buffer size
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_signMessage(U32BIT drm_handle, U8BIT *auth_nonce, U16BIT auth_nonce_len,U8BIT *SignMsg, U16BIT *Signlen);


/***************************************************************************
    Description:
        To Verify the Signature Message.

    Input Parameters:
        drm_handle: DRM Handle Context
        DHPM: CICAM DHPM
        DHPM_len:DHPM buffer length
        SignMsg: Signed Message
        Signlen: Signed Message Length

    Output Parameters:
        Nil
    Return Value:
        CI_CC_INV_ARG: input drm_handle is not valid or DHPM,SignMsg is NULL
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_verifySignatureB(U32BIT drm_handle, U8BIT *DHPM,U16BIT DHPM_len, U8BIT* SignMsg, U16BIT Signlen);


/***************************************************************************
    Description:
        To Get the DHPH.

    Input Parameters:
        drm_handle: DRM Handle Context
        DHPH_len:DHPH buffer length

    Output Parameters:
        DHPH: DHPH of Host
        DHPH_len:actual DHPH data length
    Return Value:
        CI_CC_INV_ARG: input drm_handle is not valid or DHPH is NULL
        CI_CC_NOT_ENOUGH_SPACE:DHPH_len is not enough, in this case DHPH_len will be output to actual needed buffer size
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_getDhph(U32BIT drm_handle, U8BIT* DHPH,U16BIT *DHPH_len);


/***************************************************************************
    Description:
        Get the Host Id from Host Device Certificate.

    Input Parameters:
        drm_handle: DRM Handle Context
        host_id_len:host_id buffer length

    Output Parameters:
        host_id: Host id from Host Certificate
        host_id_len:actual host_id data len
    Return Value:
        CI_CC_INV_ARG: input drm_handle is not valid or host_id,host_id_len is NULL
        CI_CC_NOT_ENOUGH_SPACE:host_id_len is not enough, in this case host_id_len will be output to actual needed buffer size
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_getHostId(U32BIT drm_handle, U8BIT *host_id,U16BIT *host_id_len);

/***************************************************************************
    Description:
        Get the CAM Id from CAM Device Certificate.

    Input Parameters:
        drm_handle: DRM Handle Context
        cam_id_len:host_id buffer length

    Output Parameters:
        cam_id: CAM id from CAM Certificate
        cam_id_len:actual cam_id data len
    Return Value:
        CI_CC_INV_ARG: input drm_handle is not valid or host_id,host_id_len is NULL
        CI_CC_NOT_ENOUGH_SPACE:cam_id_len is not enough, in this case cam_id_len will be output to actual needed buffer size
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_getCamId(U32BIT drm_handle, U8BIT *cam_id,UINT16* cam_id_len);

CI_CC_RESULT_T ci_cc_drm_getCamBrandId(U32BIT drm_handle,U16BIT *brand_id);

/***************************************************************************
    Description:
        Generates DHSK and then finally AKH.

    Input Parameters:
        drm_handle: DRM Handle Context
        AKH_len: akh buffer length

    Output Parameters:
        AKH: AKH Generated
        AKH_len:actual AKH data length
    Return Value:
        CI_CC_INV_ARG: input drm_handle is not valid or AKH,AKH_len is NULL
        CI_CC_NOT_ENOUGH_SPACE:AKH_len is not enough, in this case AKH_len will be output to actual needed buffer size
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_generateAkh(U32BIT drm_handle, U8BIT *AKH,U16BIT *AKH_len);

CI_CC_RESULT_T ci_cc_drm_generateDHSK(U32BIT drm_handle);

/***************************************************************************
    Description:
        Generates SAC Keys.

    Input Parameters:
        drm_handle: DRM handle Context
        Ns_Module: Random Number from Module
        Ns_Module_len:Ns_Module buffer length
        CICAM_ID: CI Cam Id
        CICAM_ID_len:CICAM_ID buffer length
        
    Output Parameters:
        Nil
    Return Value:
        CI_CC_INV_ARG: input drm_handle is not valid or Ns_Module,CICAM_ID is NULL
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_generateSacKeys(U32BIT drm_handle,U8BIT *Ns_Module, U16BIT Ns_Module_len,U8BIT *CICAM_ID,U16BIT CICAM_ID_len);


/***************************************************************************
    Description:
        Generates 64 Bits Random Numbers.

    Input Parameters:
        drm_handle: DRM Handle Context
        Ns_Host_len:Ns_Host buffer length

    Output Parameters:
        Ns_Host: random Number Generated
       Ns_Host_len:actual Ns_Host data length
    Return Value:
        CI_CC_INV_ARG: input drm_handle is not valid or Ns_Host,Ns_Host_len is NULL
        CI_CC_NOT_ENOUGH_SPACE:Ns_Host_len is not enough, in this case Ns_Host_len will be output to actual needed buffer size
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_getNs(U32BIT drm_handle, U8BIT *Ns_Host,U16BIT *Ns_Host_len);


/***************************************************************************
    Description:

This function is used to encrypt and sign a SAC message to send it to the CICAM over the SAC.
After use, the encrypted message memory should be freed by the caller using free().

Input Parameters:
drm_handle: DRM Handle Context
clearmsg Pointer to buffer holding the clear message
clear_msg_len pointer to length of clear message
enc_msg_len: enc_msg buffer length

Output Parameters:
enc_msg  pointer to the encrypted and signed message.
enc_msg_len actual encrypted and signed message length
Return Value:
    CI_CC_INV_ARG: input drm_handle is not valid or clearmsg,enc_msg ,enc_msg_len is NULL
    CI_CC_NOT_ENOUGH_SPACE:enc_msg_len is not enough, in this case enc_msg_len will be output to actual needed buffer size
    CI_CC_FAILED:failed
    CI_CC_OK:sucessfully

**************************************************************************/

U32BIT ci_cc_drm_encryptMessageSac(U32BIT drm_handle,U8BIT* clearmsg, U16BIT clear_msg_len, U8BIT *enc_msg, U16BIT *enc_msg_len);

/***************************************************************************
    Description:

This function is used to authenticate and decrypt a SAC message and return the decrypted message contents.
If the SAC message contains the Key precursor Kp used for CCK derivation, the decrypted message is not output. 
Instead the CCK operation will be processed. In this case clearmsg/clear_msg_length 
will carry the reply message to be sent to the CICAM, and the decrypted' output variable will be set to 0.
If the SAC message does not contain the Key precursor Kp, the message is decrypted and returned in clearmsg/clear_msg_length 
and the decrypted' output variable is set to 1

Input Parameters:
drm_handle Pointer to DRM handle
enc_msg Pointer to buffer holding the encrypted message 
enc_msg_len Pointer to length of encrypted message
dec_msg_length:dec_msg buffer length

Output Parameters:
dec_msg  pointer to the clear message buffer / reply message buffer
decrypted Pointer to the clear message length/reply message length
decrypted Pointer to variable indicating which operation was performed. 1=message decrypted 0=reply generated.
Return Value:
    CI_CC_INV_ARG: input drm_handle is not valid or clearmsg,enc_msg ,enc_msg_len is NULL
    CI_CC_NOT_ENOUGH_SPACE:enc_msg_len is not enough, in this case enc_msg_len will be output to actual needed buffer size
    CI_CC_FAILED:failed
    CI_CC_OK:sucessfully
**************************************************************************/

CI_CC_RESULT_T ci_cc_drm_decryptMessageSac(U32BIT drm_handle,U8BIT *encmsg, U16BIT enc_msg_length, U8BIT *dec_msg, U16BIT *dec_msg_length, U8BIT *decrypted);

/***************************************************************************
    Description:

   This API returns key ID which shall be associated with DMX for content decryption 
   This function only needs to be called after the initial CCK establishment. Any subsequent CCK refresh will use the same KeyID. 
   The KeyID actually represents a key pair (ODD, EVEN), and key rotation will be handled automatically 
   Then processing SAC messages containing Key precursor Kp

Input Parameters:
        drm_handle pointer to DRM handle

        Output Parameters:
        KeyID  Key ID for use with AMP DMX.
Return Value:
    CI_CC_INV_ARG: input drm_handle is not valid or puKeyID is NULL
    CI_CC_FAILED:failed
    CI_CC_OK:sucessfully
**************************************************************************/


CI_CC_RESULT_T ci_cc_drm_getKeyId(U32BIT drm_handle, U32BIT * puKeyID);

#if 0
CI_CC_RESULT_T ci_cc_drm_getCCKId(U32BIT drm_handle,U8BIT *cck, U8BIT *civ,U8BIT *cipher,U8BIT* key_register);
#endif
/***************************************************************************
    Description:

This API generates a confirmation message for either the URI refresh protocol or the SRM transfer protocol. 
he output message will be the SHA-256 checksum of a message consisting of the input data and the UCK. The UCK is a SHA-256 checksum of the SAK

Input Parameters:
rm_handle Pointer to DRM handle
Data Pointer to data of message body
Data_length Length of message body
confirm_length:confirm buffer length

Output Parameters:
    Confirm  pointer to confirmation message
    Cofirm_length output actural length of confirmation message
Return Value:
    CI_CC_INV_ARG: input drm_handle is not valid or data,confirm ,confirm_length is  NULL
    CI_CC_NOT_ENOUGH_SPACE:confirm_length is not enough, in this case confirm_length will be output to actual needed buffer size
    CI_CC_FAILED:failed
    CI_CC_OK:sucessfully

**************************************************************************/

U32BIT ci_cc_drm_dataConfirm(U32BIT drm_handle, U8BIT* data, U16BIT data_length, U8BIT* confirm, U16BIT* confirm_length);
/***************************************************************************
    Description:
        To query tv stored AKH, tv will maximum store 5 AKHs.

    Input Parameters:
        drm_handle: DRM Handle Context
        AKH_len: akh buffer length

    Output Parameters:
        AKH: AKH queried
        AKH_len:actual AKH data length
Return Value:
    CI_CC_INV_ARG: input drm_handle is not valid or AKH,AKH_len is NULL
    CI_CC_NOT_ENOUGH_SPACE:AKH_len is not enough, in this case AKH_len will be output to actual needed buffer size
    CI_CC_FAILED:failed
    CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T ci_cc_drm_GetNextAuthContext(U32BIT drm_handle,U8BIT *AKH,U16BIT *AKH_len,U8BIT *cam_id,U16BIT *cam_id_len,U16BIT *cam_brand_id,U16BIT *cam_brand_id_len);
/***************************************************************************
    Description:
        ask TV To store auth context data.

    Input Parameters:
        drm_handle: DRM Context Handle
    Output Parameters:
        Null
    Return Value:    
        CI_CC_INV_ARG: input drm_handle is NULL
        CI_CC_OK:sucessfully

**************************************************************************/
void CC_Set_AuthContexts(U8BIT* contextdata,U32BIT data_len);
void CC_Get_AuthContexts(U8BIT* contextdata,U32BIT data_len);

CI_CC_RESULT_T ci_cc_drm_getCCKValue(UINT32 key_id,UINT8 *cipher,UINT8* key_register,UINT8 *cck, UINT16* cck_len,UINT8 *civ,UINT16* civ_len);

#endif //CI_TRUST_ZONE_H_
