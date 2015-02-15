/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008,2009 Sony Corporation
 */
#include "basic_types_iptves.h"
#include "dbg_api_iptves.h"
#include "prof_api_iptves.h"
#include "exh_api_iptves.h"
#include "base_api_iptves.h"
#include "mem_api_iptves.h"
#include "byte_api_iptves.h"
#include "strm_api_iptves.h"
#include "crypto_api_iptves.h"
#include "keymng_api.h"
#include "certmng_api.h"
#include "sp_api.h"
#include "sac_api.h"

COMPONENT(SAC);

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
/**
 * header parameters
 */
#define SAC_HEADER_PROTOCOL_ID          byte_4CC('I','P','T','V')
#define SAC_HEADER_PROTOCOL_VER         0x0100

/**
 * payload type of sac message
 */
#define SAC_PAYLOAD_TYPE_INIT           0x0000
#define SAC_PAYLOAD_TYPE_CHALLENGE      0x0001
#define SAC_PAYLOAD_TYPE_RESCHALLENGE   0x0002
#define SAC_PAYLOAD_TYPE_RESREQ         0x0003
#define SAC_PAYLOAD_TYPE_REQ            0x0004
#define SAC_PAYLOAD_TYPE_REP            0x0005
#define SAC_PAYLOAD_TYPE_PLAINCMD       0x0006
#define SAC_PAYLOAD_TYPE_ENCCMD         0x0007
#define SAC_PAYLOAD_TYPE_RESCOMMIT      0x0008

/**
 * sequence number
 */
#define SAC_SEQNUM_FIRST                1
#define SAC_SEQNUM_RESCOMMIT            SAC_SEQNUM_FIRST
#define SAC_SEQNUM_RESREQ               SAC_SEQNUM_FIRST
#define SAC_SEQNUM_REQ_MAX              (SAC_SEQNUM_MAX - 2 -2)/* for commit */
#define SAC_SEQNUM_COMMIT_MAX           (SAC_SEQNUM_MAX - 2)
#define SAC_SEQNUM_MAX                  ((1 << 24) - 1)

/**
 * transaction flag record flag
 */
#define SAC_TRANS_FLAG_REC_FLAG_TRUE    0x01
#define SAC_TRANS_FLAG_REC_FLAG_FALSE   0x00

/**
 * transaction flag
 */
#define SAC_TRANS_FLAG_EVEN             0x00
#define SAC_TRANS_FLAG_ODD              0x01
#define SAC_TRANS_FLAG_NONE             0xFF

/**
 * command
 */
#define SAC_CMD_ACK                     0x0001
#define SAC_CMD_ERR                     0x0002
#define SAC_CMD_COMMIT                  0x0003

/**
 * status values
 */
#define SAC_STATUS_SUCCESS              0x0000
#define SAC_STATUS_OTHERS               0x8001
#define SAC_STATUS_MSG_ERR              0x8002
#define SAC_STATUS_AUTH_ERR             0x8003
#define SAC_STATUS_REVOKED              0x8004
#define SAC_STATUS_CERT_ISSR_ERR        0x8005

/**
 * data length
 */
#define SAC_LENGTH_CERT_SIZE            0x02
#define SAC_LENGTH_RANDOM_NUMBER        0x10
#define SAC_LENGTH_ECDH_PHASE1          0x38
#define SAC_LENGTH_SIGNATURE            0x38
#define SAC_LENGTH_ENC_DATA_SIZE        0x04
#define SAC_LENGTH_SEQ_NUMBER           0x03
#define SAC_LENGTH_TRANS_FLAG           0x01
#define SAC_LENGTH_TRANS_FLAG_REC_FLAG  0x01
#define SAC_LENGTH_COMMAND              0x02
#define SAC_LENGTH_STATUS               0x02
#define SAC_LENGTH_MESSAGE_DIGEST       0x20
#define SAC_LENGTH_SENDER_ID            0x08

#define l_GetPayloadSizeForChallenge(c) (SAC_LENGTH_RANDOM_NUMBER       + \
                                         SAC_LENGTH_CERT_SIZE           + \
                                         (c))
#define l_GetPayloadSizeForResCommit()  (SAC_LENGTH_ECDH_PHASE1         + \
                                         SAC_LENGTH_SIGNATURE           + \
                                         SAC_LENGTH_ENC_DATA_SIZE       + \
                                         SAC_LENGTH_SEQ_NUMBER          + \
                                         SAC_LENGTH_TRANS_FLAG          + \
                                         SAC_LENGTH_MESSAGE_DIGEST)
#define l_GetPayloadSizeForResReq(r)    (SAC_LENGTH_ECDH_PHASE1         + \
                                         SAC_LENGTH_SIGNATURE           + \
                                         SAC_LENGTH_ENC_DATA_SIZE       + \
                                         SAC_LENGTH_SEQ_NUMBER          + \
                                         SAC_LENGTH_TRANS_FLAG          + \
                                         (r)                            + \
                                         SAC_LENGTH_MESSAGE_DIGEST)
#define l_GetPayloadSizeForReq(r)       (SAC_LENGTH_ENC_DATA_SIZE       + \
                                         SAC_LENGTH_SEQ_NUMBER          + \
                                         SAC_LENGTH_TRANS_FLAG          + \
                                         (r)                            + \
                                         SAC_LENGTH_MESSAGE_DIGEST)
#define l_GetPayloadSizeForEncCmd()     (SAC_LENGTH_ENC_DATA_SIZE       + \
                                         SAC_LENGTH_SEQ_NUMBER          + \
                                         SAC_LENGTH_TRANS_FLAG          + \
                                         SAC_LENGTH_COMMAND             + \
                                         SAC_LENGTH_STATUS              + \
                                         SAC_LENGTH_MESSAGE_DIGEST)
#define l_GetPayloadSizeForPlainCmd()   (SAC_LENGTH_COMMAND             + \
                                         SAC_LENGTH_STATUS)

#define l_GetEncDataSizeForResCommit()  (SAC_LENGTH_SEQ_NUMBER          + \
                                         SAC_LENGTH_TRANS_FLAG          + \
                                         SAC_LENGTH_MESSAGE_DIGEST)
#define l_GetEncDataSizeForResReq(r)    (SAC_LENGTH_SEQ_NUMBER          + \
                                         SAC_LENGTH_TRANS_FLAG          + \
                                         (r)                            + \
                                         SAC_LENGTH_MESSAGE_DIGEST)
#define l_GetEncDataSizeForReq(r)       (SAC_LENGTH_SEQ_NUMBER          + \
                                         SAC_LENGTH_TRANS_FLAG          + \
                                         (r)                            + \
                                         SAC_LENGTH_MESSAGE_DIGEST)
#define l_GetEncDataSizeForEncCmd()     (SAC_LENGTH_SEQ_NUMBER          + \
                                         SAC_LENGTH_TRANS_FLAG          + \
                                         SAC_LENGTH_COMMAND             + \
                                         SAC_LENGTH_STATUS              + \
                                         SAC_LENGTH_MESSAGE_DIGEST)

#define l_GetReplySizeForRep(p)         ((p)                            - \
                                         SAC_LENGTH_ENC_DATA_SIZE       - \
                                         SAC_LENGTH_SEQ_NUMBER          - \
                                         SAC_LENGTH_TRANS_FLAG_REC_FLAG - \
                                         SAC_LENGTH_MESSAGE_DIGEST)

/**
 * convert from status values to error code
 */
#define RET_SAC_STATUS(s)               ((s) - SAC_STATUS_OTHERS + RET_SAC_STATUS_OTHERS)

/*============================================================================
 * Type Definitions
 *==========================================================================*/
/**
 * SAC context
 */
typedef struct {
    po_thid_t   thid;
    u_int32_t   payload_type;
    u_int8_t    devid[PO_DEVICE_ID_LEN];
    u_int32_t   seq_num;
    u_int8_t    trans_flag;
    bool_t      need_to_rec;
    u_int8_t    srv_sbj[KEYMNG_SRV_SBJ_LEN];
    u_int8_t    svr_rn[CRYPTO_RANDOM_NUMBER_LEN];
    u_int8_t    svr_p1v[CRYPTO_ECDH_PHASE1VALUE_LEN];
    u_int8_t    client_rn[CRYPTO_RANDOM_NUMBER_LEN];
    u_int8_t    client_p1v[CRYPTO_ECDH_PHASE1VALUE_LEN];
    u_int8_t    sskey[CRYPTO_AES_KEY_LEN];
    sp_handle_t sp_handle;
} sac_ctx_t;

/*============================================================================
 * Global Variables
 *==========================================================================*/

sac_ctx_t       l_sac_ctx[PO_THID_MAX];

/*============================================================================
 * Local Functions
 *==========================================================================*/
	
	
	

static int
l_ConvErr(int in_err)
{
    exh_InitOk();
    MSGENT();

    exh_Set(in_err);
    switch (exh_Get()) {
    case RET_OK:
    case RET_NOMEM:
    case RET_UNSUPPORTED:
        exh_Rethrow();

    /* from SP layer */
    case RET_SP_TOO_LONG_PACKED_REQ:
        exh_Throw(RET_SAC_TOO_LONG_REQUEST);
    case RET_SP_VERIFICATION_FAILURE:
    case RET_SP_INVALID_MSG_RECEIVED:
        exh_Throw(RET_SAC_VERIFICATION_FAILURE);
    case RET_SP_SAVE_ERROR:
        exh_Throw(RET_SAC_RWDATA_CORRUPTED);
    case RET_SP_STATUS_OTHERS:
    case RET_SP_STATUS_VERSION_ERR:
    case RET_SP_STATUS_DEVINFO_ERR:
        exh_Throw(RET_SAC_VERIFICATION_FAILURE);
    case RET_SP_STATUS_USAGERULEREF_ERR:
    case RET_SP_STATUS_ACTION_ID_ERR:
    case RET_SP_STATUS_ACTION_PARAM_ERR:
    case RET_SP_STATUS_ACTION_DENIED:
    case RET_SP_STATUS_ACTION_FAILED:
    case RET_SP_STATUS_MSG_FORMAT_ERR:
        exh_Throw(RET_SAC_REQUEST_DENIED);
    case RET_SP_ILLEGAL_REQUEST:
        exh_Throw(RET_SAC_ILLEGAL_REQUEST);
#if !defined(NOT_SUPPORT_DL)
    case RET_SP_TOO_MANY_LICENSES:
        exh_Throw(RET_SAC_TOO_MANY_LICENSES);
#endif

    /* from certmng layer */
    case RET_CERTMNG_CERT_ERR_FMT:
    case RET_CERTMNG_CERT_ERR_SRV_VERIFY:
    case RET_CERTMNG_CERT_ERR_CA_VERIFY:
    case RET_CERTMNG_CERT_ERR_SRV_FMT:
    case RET_CERTMNG_CERT_ERR_CA_FMT:
        exh_Throw(RET_SAC_CERT_VERIFICATION_FAILED);
    case RET_CERTMNG_CERT_ERR_SRV_VALIDITY:
    case RET_CERTMNG_CERT_ERR_CA_VALIDITY:
        exh_Throw(RET_SAC_CERT_VALIDITY_ERR);
    case RET_CERTMNG_CERT_ERR_SRV_REVOKED:
    case RET_CERTMNG_CERT_ERR_CA_REVOKED:
        exh_Throw(RET_SAC_CERT_REVOKED);
    case RET_CERTMNG_MSG_SIG_ERR_VERIFY:
        exh_Throw(RET_SAC_MSG_SIG_VERIFICATION_FAILED);

    /* from keymng layer */
    case RET_KEYMNG_SAVE_ERROR:
        exh_Throw(RET_SAC_RWDATA_CORRUPTED);

    default:
        MSG1(("SAC l_ConvErr: ret=%x->%x(RET_INTERNALERROR)\n",
              exh_Get(), RET_INTERNALERROR));
        exh_Throw(RET_INTERNALERROR);
        break;
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*----------------------------------------------------------------------------
 * Header Generation/Verification
 *--------------------------------------------------------------------------*/
static int
l_SetHeader(sac_ctx_t          *io_ctx,
            strm_t             *io_msg,
            u_int16_t           in_payload_type,
            u_int32_t           in_payload_size)
{
    exh_InitOk();
    MSGENT();

    strm_BE4W(io_msg, SAC_HEADER_PROTOCOL_ID);
    strm_BE2W(io_msg, SAC_HEADER_PROTOCOL_VER);
    strm_CPYW(io_msg, io_ctx->devid, SAC_LENGTH_SENDER_ID);
    strm_BE2W(io_msg, in_payload_type);
    strm_BE4W(io_msg, in_payload_size);

    io_ctx->payload_type = in_payload_type;

    MSGI3(("SAC Header\n"));
    MSGI3(("  Protocol ID     : 0x%08x\n", SAC_HEADER_PROTOCOL_ID));
    MSGI3(("  Protocol Version: 0x%04x\n", SAC_HEADER_PROTOCOL_VER));
    MSGI3(("  Sender ID       : ")); BDUMP3(SAC_LENGTH_SENDER_ID, io_ctx->devid); MSG3(("\n"));
    MSGI3(("  Payload Type    : 0x%04x\n", in_payload_type));
    MSGI3(("  Paylaod Size    : 0x%08x (%d)\n", in_payload_size, in_payload_size));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_VerifyHeader(sac_ctx_t       *io_ctx,
               strm_t          *io_msg,
               u_int16_t       *out_payload_type)
{
    u_int32_t   protocol_id;
    u_int16_t   protocol_ver;
    u_int8_t   *sender_id;
    u_int16_t   payload_type;
    u_int32_t   act_payload_size;
    u_int32_t   exp_payload_size;

    exh_InitOk();
    MSGENT();

    /*
     * [MIPTV] 4.1.4.1: Processing Rules for ProtocolID
     */
    strm_BE4R(io_msg, protocol_id);
    if (protocol_id != SAC_HEADER_PROTOCOL_ID) {
        MSG1(("*** Verification Failure: Protocol ID is wrong...\n"
              "      expected = 0x%08x, actual = 0x%08x\n",
              SAC_HEADER_PROTOCOL_ID, protocol_id));
        exh_Throw(RET_SAC_VERIFICATION_FAILURE);
    }

    /*
     * [MIPTV] 4.1.4.1: Processing Rules for ProtocolVersion
     */
    strm_BE2R(io_msg, protocol_ver);
    if (protocol_ver != SAC_HEADER_PROTOCOL_VER) {
        MSG1(("*** Verification Failure: Protocol Version is wrong...\n"
              "      expented = 0x%04x, actual = 0x%04x\n",
              SAC_HEADER_PROTOCOL_VER, protocol_ver));
        exh_Throw(RET_SAC_VERIFICATION_FAILURE);
    }

    strm_REFR(io_msg, sender_id, 8);
    strm_BE2R(io_msg, payload_type);
    strm_BE4R(io_msg, act_payload_size);

    /*
     * [MIPTV] 4.1.4.1: Processing Rules for PayloadSize
     */
    switch (payload_type) {
    case SAC_PAYLOAD_TYPE_ENCCMD:
        exp_payload_size = l_GetPayloadSizeForEncCmd();
        break;
    case SAC_PAYLOAD_TYPE_PLAINCMD:
        exp_payload_size = l_GetPayloadSizeForPlainCmd();
        break;
    default:
        exp_payload_size = strm_Len(io_msg);
    }
    if (act_payload_size != exp_payload_size) {
        MSG1(("*** Verification Failure: Payload size is wrong...\n"
              "      expected = %d, actual = %d\n",
              exp_payload_size, act_payload_size));
        exh_Throw(RET_SAC_VERIFICATION_FAILURE);
    }

    MSGI3(("SAC Header\n"));
    MSGI3(("  Protocol ID     : 0x%08x\n", protocol_id));
    MSGI3(("  Protocol Version: 0x%04x\n", protocol_ver));
    MSGI3(("  Sender ID       : ")); BDUMP3(SAC_LENGTH_SENDER_ID, sender_id); MSG3(("\n"));
    MSGI3(("  Payload Type    : 0x%04x\n", payload_type));
    MSGI3(("  Paylaod Size    : 0x%08x (%d)\n", act_payload_size, act_payload_size));

    *out_payload_type = payload_type;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*----------------------------------------------------------------------------
 * Session Key Initialization
 *--------------------------------------------------------------------------*/
static int
l_InitSessionKey(sac_ctx_t     *io_ctx)
{
    u_int8_t    ecdh_pub[CRYPTO_ECDH_PUBKEY_LEN];

    exh_InitOk();
    MSGENT();

    exh_Set(crypto_Ecdh(io_ctx->svr_p1v, io_ctx->client_p1v, ecdh_pub));
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

    /*
     * [MIPTV] 4.1.2: Crypto Algorithm
     *   The session key used SHALL be made up of low order 128 bit
     *   on the x coordinate of the shared secret value generated by the EC-DH.
     */
    exh_Check(base_Memcpy(io_ctx->sskey, CRYPTO_AES_KEY_LEN, ecdh_pub + 12, CRYPTO_AES_KEY_LEN));

    MSGI3(("----- SINK ECDH PHASE 1   -----\n")); DUMP3(CRYPTO_ECDH_PHASE1VALUE_LEN, io_ctx->client_p1v);
    MSGI3(("----- SOURCE ECDH PHASE 1 -----\n")); DUMP3(CRYPTO_ECDH_PHASE1VALUE_LEN, io_ctx->svr_p1v);
    MSGI3(("----- ECDH PUBLIC KEY     -----\n")); DUMP3(CRYPTO_ECDH_PUBKEY_LEN, ecdh_pub);
    MSGI3(("----- SESSION KEY         -----\n")); DUMP3(CRYPTO_AES_KEY_LEN, io_ctx->sskey);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*----------------------------------------------------------------------------
 * Message Encryption/Decryption
 *--------------------------------------------------------------------------*/
static int
l_EncryptMsg(sac_ctx_t         *io_ctx,
             strm_t            *io_msg)
{
    u_int8_t   *plain   = strm_Buf(io_msg);
    u_int8_t   *cipher  = strm_Buf(io_msg);
    u_int32_t   msg_len = strm_Len(io_msg);

    exh_InitOk();
    MSGENT();

    MSGI3(("----- PLAIN MESSAGE (before encryption) -----\n"));
    DUMP3(msg_len, plain);

    {
        u_int8_t  iv[CRYPTO_AES_BLOCK_LEN];

        memset(iv, 0, CRYPTO_AES_BLOCK_LEN);

        exh_Set(crypto_AesCbcofbEncrypt(plain, msg_len, io_ctx->sskey, iv, cipher));
        if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);
    }

    MSGI3(("----- CIPHER MESSAGE (after encryption) -----\n"));
    DUMP3(msg_len, cipher);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_DecryptMsg(sac_ctx_t         *io_ctx,
             strm_t            *io_msg)
{
    u_int8_t   *plain   = strm_Buf(io_msg);
    u_int8_t   *cipher  = strm_Buf(io_msg);
    u_int32_t   msg_len = strm_Len(io_msg);

    exh_InitOk();
    MSGENT();

    MSGI3(("----- CIPHER MESSAGE (before decryption) -----\n"));
    DUMP3(msg_len, cipher);

    {
        u_int8_t  iv[CRYPTO_AES_BLOCK_LEN];

        memset(iv, 0, CRYPTO_AES_BLOCK_LEN);
		
        exh_Set(crypto_AesCbcofbDecrypt(cipher, msg_len, io_ctx->sskey, iv, plain));

		//test_aescbcofb();
		
        if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);
    }

    MSGI3(("----- PLAIN MESSAGE (after decryption) -----\n"));
    DUMP3(msg_len, plain);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*----------------------------------------------------------------------------
 * Digest Generation/Verification
 *--------------------------------------------------------------------------*/
static int
l_MakeDigest(sac_ctx_t         *io_ctx,
             strm_t            *io_msg)
{
    u_int8_t   *target     = io_msg->buf;
    u_int32_t   target_len = io_msg->wp - io_msg->buf;
    u_int8_t   *calc_digest;

    exh_InitOk();
    MSGENT();

    strm_REFW(io_msg, calc_digest, SAC_LENGTH_MESSAGE_DIGEST);

    exh_Set(crypto_Sha256Hash(target, target_len, calc_digest));
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_VerifyDigest(sac_ctx_t       *io_ctx,
               strm_t          *io_msg)
{
    u_int8_t   *target     = io_msg->buf;
    u_int32_t   target_len = io_msg->rp - io_msg->buf;
    u_int8_t    calc_digest[SAC_LENGTH_MESSAGE_DIGEST];
    u_int8_t   *digest;

    exh_InitOk();
    MSGENT();

    strm_REFR(io_msg, digest, SAC_LENGTH_MESSAGE_DIGEST);

    exh_Set(crypto_Sha256Hash(target, target_len, calc_digest));
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

    /*
     * [MIPTV] 4.1.4.6: Processing Rules for MessageDigest
     * [MIPTV] 4.1.4.8: Processing Rules for MessageDigest
     */

    
	

	
    if (memcmp(calc_digest, digest, SAC_LENGTH_MESSAGE_DIGEST)) {
        dprintf("*** Verification Failure: MessageDigest is mismatched...(!!!ignored,to-fix!!!)");
        dprintf("      ----- Recieved Digest -----\n");
		DumpHex("digest", digest, SAC_LENGTH_MESSAGE_DIGEST);
        //DUMP1(SAC_LENGTH_MESSAGE_DIGEST, digest);
        dprintf("      ----- Calculated Digest -----\n");
		DumpHex("calc_digest", calc_digest, SAC_LENGTH_MESSAGE_DIGEST);
        //DUMP1(SAC_LENGTH_MESSAGE_DIGEST, calc_digest);
        //exh_Throw(RET_SAC_VERIFICATION_FAILURE);
    }
	else
	{
		dprintf("*** Verification ok: MessageDigest matched\n");
	}

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*----------------------------------------------------------------------------
 * Signature Generation/Verification
 *--------------------------------------------------------------------------*/
static int
l_GenSignature(sac_ctx_t       *io_ctx,
               u_int8_t         out_sig[SAC_LENGTH_SIGNATURE])
{
    u_int8_t    target[CRYPTO_RANDOM_NUMBER_LEN + CRYPTO_ECDH_PHASE1VALUE_LEN];
    int32_t     target_len = CRYPTO_RANDOM_NUMBER_LEN + CRYPTO_ECDH_PHASE1VALUE_LEN;

    exh_InitOk();
    MSGENT();

    exh_Check(base_Memcpy(target, CRYPTO_RANDOM_NUMBER_LEN, io_ctx->svr_rn, CRYPTO_RANDOM_NUMBER_LEN));
    exh_Check(base_Memcpy(target + CRYPTO_RANDOM_NUMBER_LEN, CRYPTO_ECDH_PHASE1VALUE_LEN, io_ctx->client_p1v, CRYPTO_ECDH_PHASE1VALUE_LEN));

    exh_Set(certmng_GenSignature(io_ctx->thid, target, target_len, out_sig));
    if (!exh_IsOk()) exh_Throw(l_ConvErr(exh_Get()));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_VerifySignature(sac_ctx_t    *io_ctx,
                  u_int8_t     *in_pkipath,
                  u_int32_t     in_pkipath_len,
                  u_int8_t      in_sig[SAC_LENGTH_SIGNATURE])
{
    u_int8_t    target[CRYPTO_RANDOM_NUMBER_LEN + CRYPTO_ECDH_PHASE1VALUE_LEN];
    int32_t     target_len = CRYPTO_RANDOM_NUMBER_LEN + CRYPTO_ECDH_PHASE1VALUE_LEN;

    exh_InitOk();
    MSGENT();

    exh_Check(base_Memcpy(target, CRYPTO_RANDOM_NUMBER_LEN, io_ctx->client_rn, CRYPTO_RANDOM_NUMBER_LEN));
    exh_Check(base_Memcpy(target + CRYPTO_RANDOM_NUMBER_LEN, CRYPTO_ECDH_PHASE1VALUE_LEN, io_ctx->svr_p1v, CRYPTO_ECDH_PHASE1VALUE_LEN));

    /*
     * [MIPTV] 4.1.4.3: Processing Rules for Signature
     * [MIPTV] 4.1.4.3: Processing Rules for SourceCertificate
     */
    if (in_pkipath_len == 0) {
        MSG1(("*** Verification Failure: SourceCertificateSize is 0...\n"));
        exh_Throw(RET_SAC_VERIFICATION_FAILURE);
    }
    exh_Set(certmng_VerifySignature(io_ctx->thid,
                                    target, target_len,
                                    in_pkipath, in_pkipath_len,
                                    in_sig, io_ctx->srv_sbj));

    MSGI3(("server subject : ")); BDUMP3(KEYMNG_SRV_SBJ_LEN, io_ctx->srv_sbj); MSG3(("\n"));

    if (!exh_IsOk()) exh_Check(l_ConvErr(exh_Get()));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*----------------------------------------------------------------------------
 * Sequence Number Management
 *--------------------------------------------------------------------------*/
static int
l_InitSeqNum(sac_ctx_t         *io_ctx)
{
    exh_InitOk();
    MSGENT();

    io_ctx->seq_num = SAC_SEQNUM_FIRST;

    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_GetSeqNum(sac_ctx_t          *io_ctx,
            u_int32_t          *out_seq_num)
{
    exh_InitOk();
    MSGENT();

    *out_seq_num = io_ctx->seq_num++;

    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_CheckSeqNum(sac_ctx_t        *io_ctx,
              u_int32_t         in_seq_num)
{
    exh_InitOk();
    MSGENT();

    /*
     * [MIPTV] 4.1.4.6: Processing Rules for SequenceNumber
     * [MIPTV] 4.1.4.8: Processing Rules for SequenceNumber
     */
    if (in_seq_num == io_ctx->seq_num)
        io_ctx->seq_num++;
    else {
        MSG1(("*** Verification Failure: SequenceNumber is wrong...\n"
              "      expected = %d, actual = %d)\n",
              io_ctx->seq_num, in_seq_num));
        exh_Throw(RET_SAC_VERIFICATION_FAILURE);
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*----------------------------------------------------------------------------
 * Transaction Flag Managementn
 *--------------------------------------------------------------------------*/
static int
l_InitTransFlag(sac_ctx_t      *io_ctx)
{
    exh_InitOk();
    MSGENT();

    io_ctx->trans_flag = SAC_TRANS_FLAG_EVEN;

    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ReverseTransFlag(sac_ctx_t   *io_ctx)
{
    exh_InitOk();
    MSGENT();

    if (io_ctx->trans_flag == SAC_TRANS_FLAG_EVEN)
        io_ctx->trans_flag = SAC_TRANS_FLAG_ODD;
    else
        io_ctx->trans_flag = SAC_TRANS_FLAG_EVEN;

    MSGEXT(exh_Get());
    exh_Return();
}

#if !defined(NOT_SUPPORT_DL)
static int
l_DelTransFlag(sac_ctx_t       *io_ctx)
{
    exh_InitOk();
    MSGENT();

    exh_Set(keymng_EnableContentKey(io_ctx->thid, io_ctx->srv_sbj));
    if (!exh_IsOk()) exh_Check(l_ConvErr(exh_Get()));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}
#endif

/*----------------------------------------------------------------------------
 * Make Message
 *--------------------------------------------------------------------------*/
static int
l_MakeChallenge(sac_ctx_t      *io_ctx,
                strm_t         *io_msg)
{
    u_int32_t   payload_size;
    u_int8_t    pkipath[PO_MAX_CERT_LEN];
    u_int32_t   pkipath_len;

    exh_InitOk();
    MSGENT();

    exh_Check(l_InitSeqNum(io_ctx));

    exh_Set(crypto_GetRandomNumber(SAC_LENGTH_RANDOM_NUMBER, io_ctx->client_rn));
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

    exh_Set(certmng_GetDevicePkiPath(io_ctx->thid, pkipath, &pkipath_len));
    if (!exh_IsOk()) exh_Throw(l_ConvErr(exh_Get()));

    payload_size = l_GetPayloadSizeForChallenge(pkipath_len);

    exh_Check(l_SetHeader(io_ctx, io_msg, SAC_PAYLOAD_TYPE_CHALLENGE, payload_size));

    strm_CPYW(io_msg, io_ctx->client_rn, SAC_LENGTH_RANDOM_NUMBER);
    strm_BE2W(io_msg, pkipath_len);
    strm_CPYW(io_msg, pkipath, pkipath_len);

    MSGI3(("----- SINK RANDOM -----\n"));
    DUMP3(SAC_LENGTH_RANDOM_NUMBER, io_ctx->client_rn);

    MSGI3(("----- CHALLENGE MESSAGE -----\n"));
    DUMP3(strm_Len(io_msg), strm_Buf(io_msg));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

#if !defined(NOT_SUPPORT_DL)
static int
l_MakeResCommit(sac_ctx_t      *io_ctx,
                strm_t         *io_msg)
{
    u_int32_t   payload_size;
    u_int32_t   enc_data_size;
    u_int8_t    sig[SAC_LENGTH_SIGNATURE];
    u_int32_t   seq_num;
    strm_t      enc_data;

    exh_InitOk();
    MSGENT();

    exh_Check(l_InitSessionKey(io_ctx));
    exh_Check(l_GenSignature(io_ctx, sig));
    exh_Check(l_GetSeqNum(io_ctx, &seq_num));
    if (seq_num != SAC_SEQNUM_RESCOMMIT)
        exh_Throw(RET_INTERNALERROR);

    payload_size  = l_GetPayloadSizeForResCommit();
    enc_data_size = l_GetEncDataSizeForResCommit();

    exh_Check(l_SetHeader(io_ctx, io_msg, SAC_PAYLOAD_TYPE_RESCOMMIT, payload_size));

    strm_CPYW(io_msg, io_ctx->client_p1v, SAC_LENGTH_ECDH_PHASE1);
    strm_CPYW(io_msg, sig, SAC_LENGTH_SIGNATURE);
    strm_BE4W(io_msg, enc_data_size); strm_Set(&enc_data, io_msg->wp, enc_data_size);
    strm_BE3W(io_msg, seq_num);
    strm_BE1W(io_msg, io_ctx->trans_flag);

    exh_Check(l_MakeDigest(io_ctx, io_msg));

    MSGI3(("----- SINK ECDH PHASE1 VALUE -----\n"));
    DUMP3(SAC_LENGTH_ECDH_PHASE1, io_ctx->client_p1v);
    MSGI3(("----- SINK SIGNATURE -----\n"));
    DUMP3(SAC_LENGTH_SIGNATURE, sig);
    MSGI3(("  Sequence Number : %d\n", seq_num));
    MSGI3(("  Transaction Flag: 0x%02x\n", io_ctx->trans_flag));

    MSGI3(("----- RESPONSE & COMMIT MESSAGE (plain) -----\n"));
    DUMP3(strm_Len(io_msg), strm_Buf(io_msg));

    exh_Check(l_EncryptMsg(io_ctx, &enc_data));

    MSGI3(("----- RESPONSE & COMMIT MESSAGE (cipher) -----\n"));
    DUMP3(strm_Len(io_msg), strm_Buf(io_msg));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}
#endif /* !NOT_SUPPORT_DL */

static int
l_MakeResReq(sac_ctx_t         *io_ctx,
             strm_t            *io_msg)
{
    u_int32_t   payload_size;
    u_int32_t   enc_data_size;
    u_int8_t    sig[SAC_LENGTH_SIGNATURE];
    u_int32_t   seq_num;
    u_int8_t    sp_req[PO_MAX_REQ_LEN];
    u_int32_t   sp_req_len = PO_MAX_REQ_LEN;
    strm_t      enc_data;

    exh_InitOk();
    MSGENT();

    exh_Check(sp_MakeMessage(io_ctx->sp_handle, sp_req, &sp_req_len, &io_ctx->need_to_rec));

    exh_Check(l_InitSessionKey(io_ctx));
    exh_Check(l_GenSignature(io_ctx, sig));
    exh_Check(l_GetSeqNum(io_ctx, &seq_num));
    if (seq_num != SAC_SEQNUM_RESREQ)
        exh_Throw(RET_INTERNALERROR);
    exh_Check(l_InitTransFlag(io_ctx));

    payload_size  = l_GetPayloadSizeForResReq(sp_req_len);
    enc_data_size = l_GetEncDataSizeForResReq(sp_req_len);

    exh_Check(l_SetHeader(io_ctx, io_msg, SAC_PAYLOAD_TYPE_RESREQ, payload_size));

    strm_CPYW(io_msg, io_ctx->client_p1v, SAC_LENGTH_ECDH_PHASE1);
    strm_CPYW(io_msg, sig, SAC_LENGTH_SIGNATURE);
    strm_BE4W(io_msg, enc_data_size); strm_Set(&enc_data, io_msg->wp, enc_data_size);
    strm_BE3W(io_msg, seq_num);
    strm_BE1W(io_msg, io_ctx->trans_flag);
    strm_CPYW(io_msg, sp_req, sp_req_len);

    exh_Check(l_MakeDigest(io_ctx, io_msg));

    MSGI3(("----- SINK ECDH PHASE1 VALUE -----\n"));
    DUMP3(SAC_LENGTH_ECDH_PHASE1, io_ctx->client_p1v);
    MSGI3(("----- SINK SIGNATURE -----\n"));
    DUMP3(SAC_LENGTH_SIGNATURE, sig);
    MSGI3(("  Sequence Number : %d\n", seq_num));
    MSGI3(("  Transaction Flag: 0x%02x\n", io_ctx->trans_flag));
    MSGI3(("----- SP REQUEST  -----\n"));
    DUMP3(sp_req_len, sp_req);

    MSGI3(("----- RESPONSE & REQUEST MESSAGE (plain) -----\n"));
    DUMP3(strm_Len(io_msg), strm_Buf(io_msg));

    exh_Check(l_EncryptMsg(io_ctx, &enc_data));

    MSGI3(("----- RESPONSE & REQUEST MESSAGE (cipher) -----\n"));
    DUMP3(strm_Len(io_msg), strm_Buf(io_msg));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_MakeReq(sac_ctx_t            *io_ctx,
          strm_t               *io_msg)
{
    u_int32_t   payload_size;
    u_int32_t   enc_data_size;
    u_int32_t   seq_num;
    u_int8_t    sp_req[PO_MAX_REQ_LEN];
    u_int32_t   sp_req_len = PO_MAX_REQ_LEN;
    strm_t      enc_data;

    exh_InitOk();
    MSGENT();

    exh_Check(sp_MakeMessage(io_ctx->sp_handle, sp_req, &sp_req_len, &io_ctx->need_to_rec));

    exh_Check(l_GetSeqNum(io_ctx, &seq_num));
    if (seq_num > SAC_SEQNUM_REQ_MAX)
        exh_Throw(RET_SAC_TOO_MANY_SEQUENCE);

    payload_size  = l_GetPayloadSizeForReq(sp_req_len);
    enc_data_size = l_GetEncDataSizeForReq(sp_req_len);

    exh_Check(l_SetHeader(io_ctx, io_msg, SAC_PAYLOAD_TYPE_REQ, payload_size));

    strm_BE4W(io_msg, enc_data_size); strm_Set(&enc_data, io_msg->wp, enc_data_size);
    strm_BE3W(io_msg, seq_num);
    strm_BE1W(io_msg, io_ctx->trans_flag);
    strm_CPYW(io_msg, sp_req, sp_req_len);

    exh_Check(l_MakeDigest(io_ctx, io_msg));

    MSGI3(("  Sequence Number : %d\n", seq_num));
    MSGI3(("  Transaction Flag: 0x%02x\n", io_ctx->trans_flag));
    MSGI3(("----- SP REQUEST  -----\n"));
    DUMP3(sp_req_len, sp_req);
    MSGI3(("----- REQUEST MESSAGE (plain) -----\n"));
    DUMP3(strm_Len(io_msg), strm_Buf(io_msg));

    exh_Check(l_EncryptMsg(io_ctx, &enc_data));
    MSGI3(("----- REQUEST MESSAGE (cipher) -----\n"));
    DUMP3(strm_Len(io_msg), strm_Buf(io_msg));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_MakeEncCmd(sac_ctx_t         *io_ctx,
             strm_t            *io_msg)
{
    u_int32_t   payload_size;
    u_int32_t   enc_data_size;
    u_int32_t   seq_num;
    strm_t      enc_data;

    exh_InitOk();
    MSGENT();

    exh_Check(l_GetSeqNum(io_ctx, &seq_num));
    if (seq_num > SAC_SEQNUM_COMMIT_MAX)
        exh_Throw(RET_SAC_TOO_MANY_SEQUENCE);

    payload_size  = l_GetPayloadSizeForEncCmd();
    enc_data_size = l_GetEncDataSizeForEncCmd();

    exh_Check(l_SetHeader(io_ctx, io_msg, SAC_PAYLOAD_TYPE_ENCCMD, payload_size));

    strm_BE4W(io_msg, enc_data_size); strm_Set(&enc_data, io_msg->wp, enc_data_size);
    strm_BE3W(io_msg, seq_num);
    strm_BE1W(io_msg, io_ctx->trans_flag);
    strm_BE2W(io_msg, SAC_CMD_COMMIT);
    strm_BE2W(io_msg, SAC_STATUS_SUCCESS);

    exh_Check(l_MakeDigest(io_ctx, io_msg));

    MSGI3(("  Sequence Number : %d\n", seq_num));
    MSGI3(("  Transaction Flag: 0x%02x\n", io_ctx->trans_flag));
    MSGI3(("  Command         : 0x%04x\n", SAC_CMD_COMMIT));
    MSGI3(("  Status          : 0x%04x\n", SAC_STATUS_SUCCESS));

    MSGI3(("----- ENCRYPTED COMMAND MESSAGE (plain) -----\n"));
    DUMP3(strm_Len(io_msg), strm_Buf(io_msg));

    exh_Check(l_EncryptMsg(io_ctx, &enc_data));

    MSGI3(("----- ENCRYPTED COMMAND MESSAGE (cipher) -----\n"));
    DUMP3(strm_Len(io_msg), strm_Buf(io_msg));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*----------------------------------------------------------------------------
 * Process Message
 *--------------------------------------------------------------------------*/
static int
l_ProcessResChallenge(sac_ctx_t        *io_ctx,
                      strm_t           *io_msg)
{
    u_int32_t   payload_size = strm_Len(io_msg);
    u_int8_t   *sig;
    u_int8_t   *pkipath;
    u_int32_t   pkipath_len;

    exh_InitOk();
    MSGENT();

    MSGI3(("----- RESPONSE & CHALLENGE MESSAGE -----\n"));
    DUMP3(io_msg->len, io_msg->buf);

    strm_CPYR(io_msg, io_ctx->svr_rn, CRYPTO_RANDOM_NUMBER_LEN);
    strm_CPYR(io_msg, io_ctx->svr_p1v, CRYPTO_ECDH_PHASE1VALUE_LEN);
    strm_REFR(io_msg, sig, SAC_LENGTH_SIGNATURE);
    strm_BE2R(io_msg, pkipath_len);
    strm_REFR(io_msg, pkipath, pkipath_len);

    MSGI3(("----- SOURCE RANDOM NUMBER -----\n"));
    DUMP3(CRYPTO_RANDOM_NUMBER_LEN, io_ctx->svr_rn);
    MSGI3(("----- SOURCE ECDH PHASE1 VALUE -----\n"));
    DUMP3(CRYPTO_ECDH_PHASE1VALUE_LEN, io_ctx->svr_p1v);
    MSGI3(("----- SOURCE SIGNATURE -----\n"));
    DUMP3(SAC_LENGTH_SIGNATURE, sig);

    /*
     * [MIPTV] 4.1.4.3: Processing Rules for SourceCertificateSize
     */
    if (pkipath_len != payload_size - 130) {
        MSG1(("*** Verification Failure: SourceCertificateSize is wrong...\n"
              "      expected = %d, actual = %d\n",
              payload_size - 130, pkipath_len));
        exh_Throw(RET_SAC_VERIFICATION_FAILURE);
    }

    exh_Check(l_VerifySignature(io_ctx, pkipath, pkipath_len, sig));

    io_ctx->payload_type = SAC_PAYLOAD_TYPE_RESCHALLENGE;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ProcessRep(sac_ctx_t         *io_ctx,
             strm_t            *io_msg)
{
    u_int32_t   payload_size = strm_Len(io_msg);
    u_int32_t   enc_data_size;
    u_int32_t   seq_num;
    u_int8_t    trans_flag_rec_flag;
    u_int8_t   *sp_rep;
    u_int32_t   sp_rep_len = l_GetReplySizeForRep(payload_size);
    u_int8_t   *srv_sbj = NULL;

    exh_InitOk();
    MSGENT();

    /*
     * [MIPTV] 4.1.4.6: Processing Rules for EncryptedDataSize
     */
    strm_BE4R(io_msg, enc_data_size);
    if (enc_data_size != (payload_size - 4)) {
        MSG1(("*** Verification Failure: Encrypted data size is wrong...\n"
              "      expected = %d, actual = %d\n",
              payload_size - 4, enc_data_size));
        exh_Throw(RET_SAC_VERIFICATION_FAILURE);
    }

    MSGI3(("----- REPLY MESSAGE (cipher) -----\n"));
    DUMP3(io_msg->len, io_msg->buf);

    exh_Check(l_DecryptMsg(io_ctx, io_msg));

    MSGI3(("----- REPLY MESSAGE (plain) -----\n"));
    DUMP3(io_msg->len, io_msg->buf);

    strm_BE3R(io_msg, seq_num);
    strm_BE1R(io_msg, trans_flag_rec_flag);
    strm_REFR(io_msg, sp_rep, sp_rep_len);

    MSGI3(("  Sequence Number             : %d\n", seq_num));
    MSGI3(("  Transaction Flag Record Flag: 0x%02x\n", trans_flag_rec_flag));

    exh_Check(l_CheckSeqNum(io_ctx, seq_num));
    exh_Check(l_VerifyDigest(io_ctx, io_msg));

#if !defined(NOT_SUPPORT_DL)
    /*
     * [MIPTV] 4.1.4.6: Processing Rules for TransactionFlagRecordFlag
     */
    if (io_ctx->need_to_rec) {
        switch (trans_flag_rec_flag) {
        case SAC_TRANS_FLAG_REC_FLAG_TRUE:
            srv_sbj = io_ctx->srv_sbj;
            break;
        case SAC_TRANS_FLAG_REC_FLAG_FALSE:
            break;
        default:
            MSG1(("*** Verification Failure: Unexpected TransactionFlagRecordFlag (0x%02x)\n",
                  trans_flag_rec_flag));
            exh_Throw(RET_SAC_VERIFICATION_FAILURE);
        }
    }

    exh_Check(l_DelTransFlag(io_ctx));
#endif /* !NOT_SUPPORT_DL */

    exh_Set(sp_ProcessMessage(io_ctx->sp_handle, sp_rep, sp_rep_len, srv_sbj, io_ctx->trans_flag));
    if (!exh_IsOk()) exh_Check(l_ConvErr(exh_Get()));

    exh_Check(l_ReverseTransFlag(io_ctx));

    io_ctx->payload_type = SAC_PAYLOAD_TYPE_REP;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ProcessEncCmd(sac_ctx_t      *io_ctx,
                strm_t         *io_msg)
{
    u_int32_t   payload_size = strm_Len(io_msg);
    u_int32_t   enc_data_size;
    u_int32_t   seq_num;
    u_int8_t    trans_flag;
    u_int16_t   command;
    u_int16_t   status;

    exh_InitOk();
    MSGENT();

    /*
     * [MIPTV] 4.1.4.8: Processing Rules for EncryptedDataSize
     */
    strm_BE4R(io_msg, enc_data_size);
    if (enc_data_size != (payload_size - 4)) {
        MSG1(("*** Verification Failure: EncryptedDataSize is wrong...\n"
              "      expected = %d, actual = %d\n",
              payload_size - 4, enc_data_size));
        exh_Throw(RET_SAC_VERIFICATION_FAILURE);
    }

    MSGI3(("----- ENCRYPTED COMMAND MESSAGE (cipher) -----\n"));
    DUMP3(io_msg->len, io_msg->buf);

    exh_Check(l_DecryptMsg(io_ctx, io_msg));

    MSGI3(("----- ENCRYPTED COMMAND MESSAGE (plain) -----\n"));
    DUMP3(io_msg->len, io_msg->buf);

    strm_BE3R(io_msg, seq_num);
    strm_BE1R(io_msg, trans_flag);
    strm_BE2R(io_msg, command);
    strm_BE2R(io_msg, status);

    MSGI3(("  Sequence Number : %d\n",     seq_num));
    MSGI3(("  Transaction Flag: 0x%02x\n", trans_flag));
    MSGI3(("  Command         : 0x%04x\n", command));
    MSGI3(("  Status          : 0x%04x\n", status));

    exh_Check(l_CheckSeqNum(io_ctx, seq_num));
    exh_Check(l_VerifyDigest(io_ctx, io_msg));

    /*
     * [MIPTV] 4.1.4.8: Processing Rules for Command
     */
    if (io_ctx->payload_type == SAC_PAYLOAD_TYPE_ENCCMD) {
        if (command != SAC_CMD_ACK && command != SAC_CMD_ERR)
            exh_Throw(RET_SAC_VERIFICATION_FAILURE);
#if !defined(NOT_SUPPORT_DL)
    } else if (io_ctx->payload_type == SAC_PAYLOAD_TYPE_RESCOMMIT) {
        if (command != SAC_CMD_ACK)
            exh_Throw(RET_SAC_VERIFICATION_FAILURE);
#endif /* !NOT_SUPPORT_DL */
    } else {
        if (command != SAC_CMD_ERR)
            exh_Throw(RET_SAC_VERIFICATION_FAILURE);
    }

    /*
     * [MIPTV] 4.1.4.8: Processing Rules for Status
     */
    if (command == SAC_CMD_ERR) {
        if (status < SAC_STATUS_OTHERS || SAC_STATUS_AUTH_ERR < status)
            exh_Throw(RET_SAC_VERIFICATION_FAILURE);
        else
            exh_Throw(RET_SAC_STATUS(status));
    } else {/* command == SAC_CMD_ACK */
        if (status != SAC_STATUS_SUCCESS)
            exh_Throw(RET_SAC_VERIFICATION_FAILURE);

#if !defined(NOT_SUPPORT_DL)
        exh_Check(l_DelTransFlag(io_ctx));
#endif /* !NOT_SUPPORT_DL */
    }

    io_ctx->payload_type = SAC_PAYLOAD_TYPE_INIT;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ProcessPlainCmd(sac_ctx_t    *io_ctx,
                  strm_t       *io_msg)
{
    u_int16_t   command;
    u_int16_t   status;

    exh_InitOk();
    MSGENT();

    MSGI3(("----- PLAIN COMMAND MESSAGE -----\n"));
    DUMP3(io_msg->len, io_msg->buf);

    strm_BE2R(io_msg, command);
    strm_BE2R(io_msg, status);

    MSGI3(("  Command: 0x%04x\n", command));
    MSGI3(("  Status : 0x%04x\n", status));

    /*
     * [MIPTV] 4.1.4.7: Processing Rules for Command
     */
    if (command != SAC_CMD_ERR)
        exh_Throw(RET_SAC_VERIFICATION_FAILURE);

    /*
     * [MIPTV] 4.1.4.7: Processing Rules for Command
     */
    if (status < SAC_STATUS_OTHERS || SAC_STATUS_REVOKED < status) {
        exh_Throw(RET_SAC_VERIFICATION_FAILURE);
    } else {
        exh_Throw(RET_SAC_STATUS(status));
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*============================================================================
 * Global Functions
 *==========================================================================*/
int
sac_Construct(po_thid_t         in_thid,
              u_int32_t         in_req_num,
              const po_req_t   *in_req_lst,
              sac_handle_t     *out_handle)
{
    sac_ctx_t  *sac_ctx = NULL;

    exh_InitOk();
    MSGENT();

    if ((in_thid >= PO_THID_MAX) ||
        (in_req_num != 0 && in_req_lst == NULL) ||
        (in_req_num == 0 && in_req_lst != NULL) ||
        out_handle == NULL)
        exh_Throw(RET_ILLEGALARGS);

    sac_ctx = &l_sac_ctx[in_thid];
    memset(sac_ctx, 0, sizeof(sac_ctx_t));
    exh_Set(certmng_GetDeviceID(in_thid, sac_ctx->devid));

    if (!exh_IsOk()) exh_Check(l_ConvErr(exh_Get()));

    sac_ctx->thid         = in_thid;
    sac_ctx->payload_type = SAC_PAYLOAD_TYPE_INIT;
    sac_ctx->need_to_rec  = FALSE;
    sac_ctx->trans_flag   = SAC_TRANS_FLAG_NONE;

    exh_Set(sp_Construct(in_thid, in_req_num, in_req_lst, &sac_ctx->sp_handle));
    if (!exh_IsOk()) exh_Check(l_ConvErr(exh_Get()));

    *out_handle = (sac_handle_t *)sac_ctx;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
sac_Destruct(sac_handle_t       io_handle,
             u_int32_t         *out_success_num,
             u_int32_t         *out_trans_flag_status)
{
    sac_ctx_t  *sac_ctx = (sac_ctx_t *)io_handle;
    u_int32_t   success_num = 0;
#if !defined(NOT_SUPPORT_DL)
    u_int8_t    trans_flag = SAC_TRANS_FLAG_NONE;
#endif

    exh_InitOk();
    MSGENT();

    if (out_trans_flag_status == NULL)
        exh_Throw(RET_ILLEGALARGS);

    *out_trans_flag_status = SAC_TRANS_FLAG_UNPROCESSED;
    if (sac_ctx) {
#if !defined(NOT_SUPPORT_DL)
        exh_Set(keymng_GetTransactionFlag(sac_ctx->thid, sac_ctx->srv_sbj, &trans_flag));
        if (exh_IsOk()) {
            if (trans_flag != SAC_TRANS_FLAG_NONE)
                *out_trans_flag_status = SAC_TRANS_FLAG_TRUE;
            else
                *out_trans_flag_status = SAC_TRANS_FLAG_FALSE;
        } else
            exh_Set(RET_INTERNALERROR);
#endif /* !NOT_SUPPORT_DL */
        if (sac_ctx->sp_handle) {
            sp_Destruct(sac_ctx->sp_handle, &success_num);
        }
    }

exh_CLEANUP:
    if (out_success_num)
        *out_success_num = success_num;

    MSGEXT(exh_Get());
    exh_Return();
}

int
sac_MakeMessage(sac_handle_t    io_handle,
                u_int8_t       *out_msg,
                u_int32_t      *io_msg_len,
                bool_t         *out_last_req_flag)
{
    sac_ctx_t  *sac_ctx = (sac_ctx_t *)io_handle;
    u_int8_t    req[PO_MAX_REQ_LEN];
    strm_t      msg;
    bool_t      last_req_flag = FALSE;

    exh_InitOk();

    PROF_INIT();
    PROF_TIMESTART(__FUNCTION__);
    MSGENT();

    if (sac_ctx == NULL)
        exh_Throw(RET_ILLEGALHANDLE);
    if (out_msg == NULL || io_msg_len == NULL)
        exh_Throw(RET_ILLEGALARGS);

    strm_Init(&msg, req, PO_MAX_REQ_LEN);

    switch (sac_ctx->payload_type) {
    case SAC_PAYLOAD_TYPE_INIT:
        exh_Check(l_MakeChallenge(sac_ctx, &msg));

        break;
    case SAC_PAYLOAD_TYPE_RESCHALLENGE:
#if !defined(NOT_SUPPORT_DL)
        if (sac_ctx->trans_flag != SAC_TRANS_FLAG_NONE) {
            exh_Check(l_MakeResCommit(sac_ctx, &msg));
            last_req_flag = TRUE;
        } else
#endif /* !NOT_SUPPORT_DL */
            exh_Check(l_MakeResReq(sac_ctx, &msg));
        break;
    case SAC_PAYLOAD_TYPE_REP:
        exh_Set(l_MakeReq(sac_ctx, &msg));
        if (!exh_IsOk()) {
            if (exh_Get() == RET_SP_NO_MORE_REQUEST) {
                CLEARLOG(); exh_SetOk();
                exh_Check(l_MakeEncCmd(sac_ctx, &msg));

                last_req_flag = TRUE;
            } else
                exh_Rethrow();
        }

        break;
    default:
        exh_Throw(RET_INTERNALERROR);
    }

    if (*io_msg_len < (u_int32_t)strm_Len(&msg))
        exh_Throw(RET_SAC_TOO_LONG_REQUEST);

    exh_Check(base_Memcpy(out_msg, strm_Len(&msg), strm_Buf(&msg), strm_Len(&msg)));
    *io_msg_len = strm_Len(&msg);

exh_CLEANUP:
    if (exh_Get() == RET_STRM_WRITE_ERROR)
        exh_Set(RET_SAC_TOO_LONG_REQUEST);

    *out_last_req_flag = last_req_flag;

    PROF_TIMESTOP();
    PROF_TIMEPRINT();
    PROF_FIN();
    MSGEXT(exh_Get());
    exh_Return();
}

int
sac_ProcessMessage(sac_handle_t io_handle,
                   u_int8_t    *in_msg,
                   u_int32_t    in_msg_len,
                   bool_t      *out_end_flag)
{
    sac_ctx_t  *sac_ctx = (sac_ctx_t *)io_handle;
    u_int8_t    res[PO_MAX_RES_LEN];
    strm_t      msg;
    u_int16_t   payload_type;
    bool_t      end_flag = FALSE;




	dprintf(">>tz sac_ProcessMessage msg len:%d\n", in_msg_len);
	DumpHex("tz_in_msg", in_msg, in_msg_len);



    exh_InitOk();

    PROF_INIT();
    PROF_TIMESTART(__FUNCTION__);
    MSGENT();

    if (sac_ctx == NULL)
        exh_Throw(RET_ILLEGALHANDLE);
    if (in_msg == NULL || out_end_flag == NULL)
        exh_Throw(RET_ILLEGALARGS);

    if (in_msg_len > PO_MAX_RES_LEN)
        exh_Throw(RET_ILLEGALARGS);
    exh_Check(base_Memcpy(res, in_msg_len, in_msg, in_msg_len));

    strm_Set(&msg, res, in_msg_len);

    exh_Check(l_VerifyHeader(sac_ctx, &msg, &payload_type));

    switch (sac_ctx->payload_type) {
    case SAC_PAYLOAD_TYPE_CHALLENGE:
        switch (payload_type) {
        case SAC_PAYLOAD_TYPE_RESCHALLENGE:
            exh_Check(l_ProcessResChallenge(sac_ctx, &msg));

#if !defined(NOT_SUPPORT_DL)
            exh_Set(keymng_GetTransactionFlag(sac_ctx->thid, sac_ctx->srv_sbj, &sac_ctx->trans_flag));
            if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

            if (sac_ctx->trans_flag == SAC_TRANS_FLAG_NONE)
#endif /* !NOT_SUPPORT_DL */
                if (!sac_ctx->sp_handle)
                    end_flag = TRUE;
            break;
        case SAC_PAYLOAD_TYPE_PLAINCMD:
            exh_Check(l_ProcessPlainCmd(sac_ctx, &msg));
            break;
        default:
            exh_Throw(RET_SAC_VERIFICATION_FAILURE);
        }

        break;
    case SAC_PAYLOAD_TYPE_RESCOMMIT:
        switch (payload_type) {
        case SAC_PAYLOAD_TYPE_ENCCMD:
            exh_Check(l_ProcessEncCmd(sac_ctx, &msg));

            end_flag = TRUE;
            break;
        case SAC_PAYLOAD_TYPE_PLAINCMD:
            exh_Check(l_ProcessPlainCmd(sac_ctx, &msg));
            break;
        default:
            exh_Throw(RET_SAC_VERIFICATION_FAILURE);
        }

        break;
    case SAC_PAYLOAD_TYPE_RESREQ:
        switch (payload_type) {
        case SAC_PAYLOAD_TYPE_REP:
            exh_Check(l_ProcessRep(sac_ctx, &msg));
            break;
        case SAC_PAYLOAD_TYPE_PLAINCMD:
            exh_Check(l_ProcessPlainCmd(sac_ctx, &msg));
            break;
        default:
            exh_Throw(RET_SAC_VERIFICATION_FAILURE);
        }

        break;
    case SAC_PAYLOAD_TYPE_REQ:
        switch (payload_type) {
        case SAC_PAYLOAD_TYPE_REP:
            exh_Check(l_ProcessRep(sac_ctx, &msg));
            break;
        case SAC_PAYLOAD_TYPE_ENCCMD:
            exh_Check(l_ProcessEncCmd(sac_ctx, &msg));
            break;
        default:
            exh_Throw(RET_SAC_VERIFICATION_FAILURE);
        }

        break;
    case SAC_PAYLOAD_TYPE_ENCCMD:
        switch (payload_type) {
        case SAC_PAYLOAD_TYPE_ENCCMD:
            exh_Check(l_ProcessEncCmd(sac_ctx, &msg));

            end_flag = TRUE;

            break;
        default:
            exh_Throw(RET_SAC_VERIFICATION_FAILURE);
        }

        break;
    default:
        exh_Throw(RET_INTERNALERROR);
    }

exh_CLEANUP:
    if (exh_Get() == RET_STRM_READ_ERROR)
        exh_Set(RET_SAC_VERIFICATION_FAILURE);

    *out_end_flag = end_flag;

    PROF_TIMESTOP();
    PROF_TIMEPRINT();
    PROF_FIN();
    MSGEXT(exh_Get());
    exh_Return();
}
