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
#include "exh_api_iptves.h"
#include "base_api_iptves.h"
#include "mem_api_iptves.h"
#include "byte_api_iptves.h"
#include "strm_api_iptves.h"
#include "time_api_iptves.h"
#include "keymng_api.h"
#include "sp_api.h"

COMPONENT(SP);

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
/**
 * device info (configurable)
 */


#define MANUFACTURER      "\x10\x00"
#define MANUFACTURER_RTDB "\x2f\xff"

#define L_DEVINFO_SPEC_VER_MAJOR        "\x01"
#define L_DEVINFO_SPEC_VER_MINOR        "\x00"
#define L_DEVINFO_CAPABILITY            "\x00"
#define L_DEVINFO_MANUFACTURER          MANUFACTURER
#define L_DEVINFO_MANUFACTURER_RTDB     MANUFACTURER_RTDB
#define L_DEVINFO_MANUFACTURER_CTS      "\x1f\xff"
#define L_DEVINFO_MANUFACTURER_RTDB_CTS "\x2f\xff"
#define L_DEVINFO_MODEL                 "\x00\x00"
#define L_DEVINFO_MODEL_VER_MAJOR       "\x00"
#define L_DEVINFO_MODEL_VER_MINOR       "\x00"
#define L_DEVINFO_RESERVED              "\x00\x00\x00"

#define L_PACKED_REQUEST_MAX            33

#define l_SetHeader(s,msg_id)                   \
    do {                                        \
        strm_BE2W(s, SP_PROTOCOL_VER);          \
        strm_BE2W(s, msg_id);                   \
    } while (0)

/*============================================================================
 * Type Definitions
 *==========================================================================*/
/**
 * sp context
 */
typedef struct {
    po_thid_t       thid;
    const po_req_t *req_lst;
    u_int32_t       req_num;
    u_int32_t       cur_make_req;
    u_int32_t       cur_process_req;
    u_int32_t       key_num;
    u_int32_t       tt;
    bool_t          is_tt_processed;
} sp_ctx_t;

/*============================================================================
 * Global Variables
 *==========================================================================*/

sp_ctx_t        l_sp_ctx[PO_THID_MAX];

static u_int8_t l_dev_info[SP_DEVINFO_LEN + 1] =        \
    L_DEVINFO_SPEC_VER_MAJOR                            \
    L_DEVINFO_SPEC_VER_MINOR                            \
    L_DEVINFO_CAPABILITY                                \
    L_DEVINFO_MANUFACTURER                              \
    L_DEVINFO_MODEL                                     \
    L_DEVINFO_MODEL_VER_MAJOR                           \
    L_DEVINFO_MODEL_VER_MINOR                           \
    L_DEVINFO_RESERVED;

static u_int8_t l_dev_info_rtdb[SP_DEVINFO_LEN + 1] =   \
    L_DEVINFO_SPEC_VER_MAJOR                            \
    L_DEVINFO_SPEC_VER_MINOR                            \
    L_DEVINFO_CAPABILITY                                \
    L_DEVINFO_MANUFACTURER_RTDB                         \
    L_DEVINFO_MODEL                                     \
    L_DEVINFO_MODEL_VER_MAJOR                           \
    L_DEVINFO_MODEL_VER_MINOR                           \
    L_DEVINFO_RESERVED;

/*============================================================================
 * Local Functions
 *==========================================================================*/
/*----------------------------------------------------------------------------
 * Check Request
 *--------------------------------------------------------------------------*/
static int
l_CheckRequestVOD(const po_req_t       *in_req_lst)
{
    const po_req_t *req     = in_req_lst;
    u_int32_t       req_num = req[0].num;

    exh_InitOk();
    MSGENT();

    /*
     * [IPTVESVOD] 3.3.1.1: Packed Message Request Conbinations
     */
    if (req_num == 1) {
        /* do nothing */
    } else if (req_num == 2) {
        if (req[1].type != PO_REQ_TYPE_GETTT) {
            MSG1(("*** SDK ERROR: Illegal request... (VOD 2 packed)\n"));
            exh_Throw(RET_SP_ILLEGAL_REQUEST);
        }
    } else {
        MSG1(("*** SDK ERROR: Too many packed request... (%d)\n", req_num));
        exh_Throw(RET_SP_ILLEGAL_REQUEST);
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_CheckRequestIPMC(const po_req_t      *in_req_lst)
{
    const po_req_t *req     = in_req_lst;
    u_int32_t       req_num = req[0].num;

    exh_InitOk();
    MSGENT();

    if (req_num > L_PACKED_REQUEST_MAX) {
        MSG1(("*** SDK ERROR: Too many packed request... (%d)\n", req_num));
        exh_Throw(RET_SP_ILLEGAL_REQUEST);
    }

    while (req_num > 0) {
        if (req_num == 1) {
            if (req[0].type != PO_REQ_TYPE_GETTT) {
                MSG1(("*** SDK ERROR: Illegal request... (IPMC)\n"));
                exh_Throw(RET_SP_ILLEGAL_REQUEST);
            }

            req     += 1;
            req_num -= 1;
        } else {
            const u_int8_t *sp_id_odd, *sp_id_even;
            const u_int8_t *wkey_id_odd, *wkey_id_even;
            const u_int8_t *reserved_odd, *reserved_even;

            /*
             * [IPTVESMC] 3.1.3  : UsageRuleReference
             * [IPTVESMC] 3.3.1.1: Packed Message Request Conbinations
             */
            if (req[0].type != PO_REQ_TYPE_GETPERM ||
                req[0].key_ref.action_id != PO_ACTION_ID_EXTRACT_IPMC ||
                req[0].key_ref.action_param != PO_ACTION_PARAM_EXTRACT ||
                req[0].key_ref.urr[SP_URR_FLAG_OFFSET] != SP_URR_FLAG_ODD ||
                req[0].key_ref.urr_len < SP_URR_LEN_IPMC) {
                MSG1(("*** SDK ERROR: Illegal request... (IPMC Odd)\n"));
                exh_Throw(RET_SP_ILLEGAL_REQUEST);
            }
            if (req[1].type != PO_REQ_TYPE_GETPERM ||
                req[1].key_ref.action_id != PO_ACTION_ID_EXTRACT_IPMC ||
                req[1].key_ref.action_param != PO_ACTION_PARAM_EXTRACT ||
                req[1].key_ref.urr[SP_URR_FLAG_OFFSET] != SP_URR_FLAG_EVEN ||
                req[1].key_ref.urr_len < SP_URR_LEN_IPMC) {
                MSG1(("*** SDK ERROR: Illegal request... (IPMC Even)\n"));
                exh_Throw(RET_SP_ILLEGAL_REQUEST);
            }

            sp_id_odd  = &req[0].key_ref.urr[SP_URR_SP_OFFSET];
            sp_id_even = &req[1].key_ref.urr[SP_URR_SP_OFFSET];
            if (sp_id_odd[0] != sp_id_even[0] || sp_id_odd[1] != sp_id_even[1]) {
                MSG1(("*** SDK ERROR: ServiceProviderID is mismatched...\n"
                      "      Odd: 0x%02x%02x, Even: 0x%02x%02x\n",
                      sp_id_odd[0], sp_id_odd[1], sp_id_even[0], sp_id_even[1]));
                exh_Throw(RET_SP_ILLEGAL_REQUEST);
            }

            wkey_id_odd  = &req[0].key_ref.urr[SP_URR_WKEY_ID_OFFSET];
            wkey_id_even = &req[1].key_ref.urr[SP_URR_WKEY_ID_OFFSET];
            if (wkey_id_odd[0] != wkey_id_even[0] || wkey_id_odd[1] != wkey_id_even[1]) {
                MSG1(("*** SDK ERROR: WorkKeyManagementID is mismatched...\n"
                      "      Odd: 0x%02x%02x, Even: 0x%02x%02x\n",
                      wkey_id_odd[0], wkey_id_odd[1], wkey_id_even[0], wkey_id_even[1]));
                exh_Throw(RET_SP_ILLEGAL_REQUEST);
            }

            reserved_odd  = &req[0].key_ref.urr[SP_URR_RESERVED_OFFSET];
            reserved_even = &req[1].key_ref.urr[SP_URR_RESERVED_OFFSET];
            if (reserved_odd[0] != 0x00 || reserved_even[0] != 0x00) {
                MSG1(("*** SDK ERROR: ReservedByte is not 0x00...\n"
                      "      Odd: 0x%02x, Even: 0x%02x\n",
                      reserved_odd[0], reserved_even[0]));
                exh_Throw(RET_SP_ILLEGAL_REQUEST);
            }

            req     += 2;
            req_num -= 2;
        }
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

#if !defined(NOT_SUPPORT_DL)
static int
l_CheckRequestDL(const po_req_t        *in_req_lst)
{
    const po_req_t *req     = in_req_lst;
    u_int32_t       req_num = req[0].num;

    exh_InitOk();
    MSGENT();

    /*
     * [IPTVESDL] 3.4.1.1: Packed Message Request Conbinations
     */
    if (req_num == 1) {
        /* do nothing */
    } else if (req_num == 2) {
        if (req[0].type == PO_REQ_TYPE_GETPERM &&
            req[0].key_ref.action_id == PO_ACTION_ID_EXTRACT_DL) {
            if (!((req[1].type == PO_REQ_TYPE_GETPERM &&
                   req[1].key_ref.action_id == PO_ACTION_ID_EXPORT_CP_DL) ||
                  (req[1].type == PO_REQ_TYPE_GETTT))) {
                MSG1(("*** SDK ERROR: Illegal request... (DL 2 packed)\n"));
                exh_Throw(RET_SP_ILLEGAL_REQUEST);
            }
        } else if (req[0].type == PO_REQ_TYPE_GETPERM &&
                   req[0].key_ref.action_id == PO_ACTION_ID_EXPORT_CP_DL) {
            if (!(req[1].type == PO_REQ_TYPE_GETTT)) {
                MSG1(("*** SDK ERROR: Illegal request... (DL 2 packed)\n"));
                exh_Throw(RET_SP_ILLEGAL_REQUEST);
            }
        } else {
            MSG1(("*** SDK ERROR: Illegal request... (DL 2 packed)\n"));
            exh_Throw(RET_SP_ILLEGAL_REQUEST);
        }
    } else if (req_num == 3) {
        if (!((req[0].type == PO_REQ_TYPE_GETPERM &&
               req[0].key_ref.action_id == PO_ACTION_ID_EXTRACT_DL) &&
              (req[1].type == PO_REQ_TYPE_GETPERM &&
               req[1].key_ref.action_id == PO_ACTION_ID_EXPORT_CP_DL) &&
              (req[2].type == PO_REQ_TYPE_GETTT))) {
            MSG1(("*** SDK ERROR: Illegal request... (DL 3 packed)\n"));
            exh_Throw(RET_SP_ILLEGAL_REQUEST);
        }
    } else {
        MSG1(("*** SDK ERROR: Too many packed request... (%d)\n", req_num));
        exh_Throw(RET_SP_ILLEGAL_REQUEST);
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}
#endif /* !NOT_SUPPORT_DL */

static int
l_CheckRequest(u_int32_t                in_req_num,
               const po_req_t          *in_req_lst)
{
    const po_req_t *req;
    u_int32_t       req_num;
    u_int32_t       i, j;

    exh_InitOk();
    MSGENT();

    for (i = 0, req = in_req_lst; i < in_req_num; ) {
        req_num = req->num;
        if (req_num == 0) {
            MSG1(("*** SDK ERROR: The num of the head of packed request SHALL NOT be 0.\n"));
            exh_Throw(RET_SP_ILLEGAL_REQUEST);
        }
        if (req->type == PO_REQ_TYPE_GETPERM) {
            for (j = 1; j < req_num; j++) {
                if (req[j].num != 0) {
                    MSG1(("*** SDK ERROR: The num of the body of packed request SHALL be 0.\n"));
                    exh_Throw(RET_SP_ILLEGAL_REQUEST);
                }
            }
            if (req->key_ref.action_id == PO_ACTION_ID_EXTRACT_VOD) {
                exh_Check(l_CheckRequestVOD(req));
            } else if (req->key_ref.action_id == PO_ACTION_ID_EXTRACT_IPMC) {
                exh_Check(l_CheckRequestIPMC(req));
#if !defined(NOT_SUPPORT_DL)
            } else if (req->key_ref.action_id == PO_ACTION_ID_EXTRACT_DL ||
                       req->key_ref.action_id == PO_ACTION_ID_EXPORT_CP_DL) {
                exh_Check(l_CheckRequestDL(req));
#endif /* !NOT_SUPPORT_DL */
            } else {
                MSG1(("*** SDK ERROR: Unsupported action id (0x%02x)\n", req->key_ref.action_id));
                exh_Throw(RET_SP_ILLEGAL_REQUEST);
            }
        } else if (req->type == PO_REQ_TYPE_GETTT && req_num == 1) {
            /* do nothing */
        } else {
            MSG1(("*** SDK ERROR: Unsupported request type (0x%02x)\n", req->type));
            exh_Throw(RET_SP_ILLEGAL_REQUEST);
        }
        i   += req_num;
        req += req_num;
    }
    if (i != in_req_num) {
        MSG1(("*** SDK ERROR: Request number is mismatched...\n"));
        exh_Throw(RET_SP_ILLEGAL_REQUEST);
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*----------------------------------------------------------------------------
 * Check Reply
 *--------------------------------------------------------------------------*/
static int
l_CheckReplyVOD(const po_req_t         *in_req_lst,
                const po_key_info_t    *in_key_info_lst)
{
    exh_InitOk();
    MSGENT();

    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_CheckReplyIPMC(const po_req_t        *in_req_lst,
                 const po_key_info_t   *in_key_info_lst)
{
    const po_req_t *req     = in_req_lst;
    u_int32_t       req_num = req[0].num;

    exh_InitOk();
    MSGENT();

    while (req_num > 0) {
        if (req_num == 1) {
            assert(req[0].type == PO_REQ_TYPE_GETTT);

            req     += 1;
            req_num -= 1;
        } else {
            const po_key_info_t *key_info_odd  = &in_key_info_lst[0];
            const po_key_info_t *key_info_even = &in_key_info_lst[1];
            const u_int8_t *wkey_ver_odd, *wkey_ver_even;
            u_int8_t wkey_ver_odd_plus, wkey_ver_even_plus;
            u_int16_t priv_data_odd, priv_data_even;
            const u_int8_t *tierbits_odd, *tierbits_even;

            assert(req[0].type == PO_REQ_TYPE_GETPERM);
            assert(req[1].type == PO_REQ_TYPE_GETPERM);

            /*
             * [IPTVESMC] 3.1.2.2: Processing Rules for WorkKeyVersion
             */
            wkey_ver_odd       = &key_info_odd->ckey_id[SP_CID_WKEY_VER_OFFSET];
            wkey_ver_even      = &key_info_even->ckey_id[SP_CID_WKEY_VER_OFFSET];
            wkey_ver_odd_plus  = wkey_ver_odd[0]  + 1;
            wkey_ver_even_plus = wkey_ver_even[0] + 1;
            if (wkey_ver_odd_plus != wkey_ver_even[0] && wkey_ver_odd[0] != wkey_ver_even_plus) {
                MSG1(("*** Verification Failure: WorkKeyVersion is wrong...\n"
                      "      Odd: %d, Even: %d\n",
                      *wkey_ver_odd, *wkey_ver_even));
                exh_Throw(RET_SP_VERIFICATION_FAILURE);
            }

            /*
             * [IPTVESMC] 3.1.2.2: Processing Rules for PrivateData (Pair Check)
             */
            priv_data_odd      = key_info_odd->priv_data;
            priv_data_even     = key_info_even->priv_data;
            if (priv_data_odd != priv_data_even) {
                MSG1(("*** Verification Failure: PrivateData is mismatched...\n"
                      "      Odd: 0x%04x, Even: 0x%04x\n",
                      priv_data_odd, priv_data_even));
                exh_Throw(RET_SP_VERIFICATION_FAILURE);
            }

            /*
             * [IPTVESMC] 3.1.2.2: Processing Rules for SubscriptionTierBits (Pair Check)
             */
            tierbits_odd       = &key_info_odd->tierbits[0];
            tierbits_even      = &key_info_even->tierbits[0];
            if (memcmp(tierbits_odd, tierbits_even, SP_CID_TIERBITS_LEN)) {
                MSG1(("*** Verification Failure: SubscriptionTierBits is mismatched...\n"));
                MSG1(("      Odd : ")); BDUMP1(PO_TIERBITS_LEN, tierbits_odd);  MSG1(("\n"));
                MSG1(("      Even: ")); BDUMP1(PO_TIERBITS_LEN, tierbits_even); MSG1(("\n"));
                exh_Throw(RET_SP_VERIFICATION_FAILURE);
            }

            /*
             * [IPTVESMC] 3.1.2.2: Processing Rules for NotBefore/NotAfter (Pair Check)
             */
            if (key_info_odd->not_before != key_info_even->not_before ||
                key_info_odd->not_after  != key_info_even->not_after) {
                MSG1(("*** Verification Failure: NotBefore/NotAfter are mismached...\n"
                      "      Odd : not_before = 0x%08x, not_after = 0x%08x\n"
                      "      Even: not_before = 0x%08x, not_after = 0x%08x\n",
                      key_info_odd->not_before,  key_info_odd->not_after,
                      key_info_even->not_before, key_info_even->not_after));
                exh_Throw(RET_SP_VERIFICATION_FAILURE);
            }

            req     += 2;
            req_num -= 2;
            in_key_info_lst += 2;
        }
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

#if !defined(NOT_SUPPORT_DL)
static int
l_CheckReplyDL(const po_req_t          *in_req_lst,
               const po_key_info_t     *in_key_info_lst)
{
    exh_InitOk();
    MSGENT();

    MSGEXT(exh_Get());
    exh_Return();
}
#endif /* !NOT_SUPPORT_DL */

static int
l_CheckReply(const po_req_t            *in_req_lst,
             const po_key_info_t       *in_key_info_lst)
{
    const po_req_t *req = &in_req_lst[0];

    exh_InitOk();
    MSGENT();

    if (req->type == PO_REQ_TYPE_GETPERM) {
        if (req->key_ref.action_id == PO_ACTION_ID_EXTRACT_VOD)
            exh_Check(l_CheckReplyVOD(in_req_lst, in_key_info_lst));
        else if (req->key_ref.action_id == PO_ACTION_ID_EXTRACT_IPMC)
            exh_Check(l_CheckReplyIPMC(in_req_lst, in_key_info_lst));
#if !defined(NOT_SUPPORT_DL)
        else if (req->key_ref.action_id == PO_ACTION_ID_EXTRACT_DL ||
                 req->key_ref.action_id == PO_ACTION_ID_EXPORT_CP_DL)
            exh_Check(l_CheckReplyDL(in_req_lst, in_key_info_lst));
#endif /* !NOT_SUPPORT_DL */
        else {
            MSG1(("*** SDK ERROR: Unsupported action id (0x%02x)\n", req->key_ref.action_id));
            exh_Throw(RET_SP_ILLEGAL_REQUEST);
        }
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*----------------------------------------------------------------------------
 * Make Message
 *--------------------------------------------------------------------------*/
static int
l_MakeGetPermReq(sp_ctx_t              *io_ctx,
                 const po_key_ref_t    *in_key_ref,
                 strm_t                *io_msg,
                 bool_t                *out_need_to_rec)
{
    u_int8_t *dev_info;
    u_int16_t specific_cr_id;
    u_int8_t  priv_data_tag;

    exh_InitOk();
    MSGENT();

    l_SetHeader(io_msg, SP_MSG_ID_GETPERM_REQ);

    /*
     * [IPTVCRVOD] 6.1.1.1: Setting Rules for SpecificCRID and PrivateDataTag
     * [IPTVCRMC]  6.1.1.1: Setting Rules for SpecificCRID and PrivateDataTag
     * [IPTVCRDL]  6.1.1.1: Setting Rules for SpecificCRID and PrivateDataTag
     * [IPTVCRDL]  6.2.1.1: Setting Rules for SpecificCRID and PrivateDataTag
     */
    if (io_ctx->thid == PO_THID_COMMON) {
        dev_info = l_dev_info;

        if (in_key_ref->action_id == PO_ACTION_ID_EXTRACT_VOD  ||
            in_key_ref->action_id == PO_ACTION_ID_EXTRACT_IPMC ||
            in_key_ref->action_id == PO_ACTION_ID_EXTRACT_DL   ||
            in_key_ref->action_id == PO_ACTION_ID_EXPORT_CP_DL) {
            specific_cr_id = 0x0000;
            priv_data_tag  = 0x00;
        } else {
            specific_cr_id = 0xFFFF;
            priv_data_tag  = 0x00;
        }
    } else {
        dev_info = l_dev_info_rtdb;

        if (in_key_ref->action_id == PO_ACTION_ID_EXTRACT_IPMC) {
            specific_cr_id = 0x0001;
            priv_data_tag  = 0x00;
        } else {
            specific_cr_id = 0xFFFF;
            priv_data_tag  = 0x00;
        }
    }

    strm_CPYW(io_msg, dev_info, SP_DEVINFO_LEN);
    strm_CPYW(io_msg, in_key_ref->urr, PO_URR_LEN);
    strm_BE1W(io_msg, in_key_ref->action_id);
    strm_BE1W(io_msg, in_key_ref->action_param);
    strm_BE2W(io_msg, specific_cr_id);
    strm_BE1W(io_msg, priv_data_tag);

    /*
     * [IMIPTV] 4.2.1.2: Setting Rules for PrivateData
     */
    if (priv_data_tag == 0x00) {
        strm_SETW(io_msg, 0x00, SP_GETPERM_REQ_PRIVDATA_LEN);
    } else {
        exh_Throw(RET_INTERNALERROR);
    }

    if (in_key_ref->action_id == PO_ACTION_ID_EXPORT_CP_DL)
        *out_need_to_rec = TRUE;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_MakeGetTTReq(sp_ctx_t                *io_ctx,
               strm_t                  *io_msg)
{
    exh_InitOk();
    MSGENT();

    l_SetHeader(io_msg, SP_MSG_ID_GETTT_REQ);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_MakePackedMsgReq(sp_ctx_t            *io_ctx,
                   const po_req_t      *in_req_lst,
                   strm_t              *io_msg,
                   bool_t              *out_need_to_rec)
{
    u_int32_t req_num = in_req_lst[0].num;
    u_int32_t i;

    exh_InitOk();
    MSGENT();

    l_SetHeader(io_msg, SP_MSG_ID_PACKED_REQ);

    strm_BE2W(io_msg, req_num);

    for (i = 0; i < req_num; i++) {
        const po_req_t *req = &in_req_lst[i];
        u_int32_t       msg_len = (req->type == PO_REQ_TYPE_GETPERM) ?
            SP_MSG_LEN_GETPERM_REQ : SP_MSG_LEN_GETTT_REQ;

        strm_BE2W(io_msg, msg_len);

        if (req->type == PO_REQ_TYPE_GETPERM) {
            exh_Check(l_MakeGetPermReq(io_ctx, &req->key_ref, io_msg, out_need_to_rec));
        } else {
            exh_Check(l_MakeGetTTReq(io_ctx, io_msg));
        }
    }

    MSGI3(("----- PACKED MESSAGE (l_MakePackedMsgReq) -----\n"));
    DUMP3(strm_Len(io_msg), strm_Buf(io_msg));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*----------------------------------------------------------------------------
 * Process Message
 *--------------------------------------------------------------------------*/
static int
l_VerifyHeader(strm_t                  *io_msg,
               u_int16_t                in_msg_id)
{
    u_int16_t protocol_ver;
    u_int16_t msg_id;

    exh_InitOk();
    MSGENT();

    strm_BE2R(io_msg, protocol_ver);
    strm_BE2R(io_msg, msg_id);

    /*
     * [MIPTV] 4.2.4.2 : Processing Rules for ProtocolVersion
     * [MIPTV] 4.2.4.10: Processing Rules for ProtocolVersion
     * [MIPTV] 4.2.4.12: Processing Rules for ProtocolVersion
     */
    if (protocol_ver != SP_PROTOCOL_VER) {
        MSG1(("*** Verification Failure: ProtocolVersion is wrong...\n"
              "      expected = 0x%04x, actual = 0x%04x\n",
              SP_PROTOCOL_VER, protocol_ver));
        exh_Throw(RET_SP_VERIFICATION_FAILURE);
    }

    if (msg_id != in_msg_id) {
        MSG1(("*** SDK ERROR: MessageID is wrong...\n"
              "      expected = 0x%04x, actual = 0x%04x\n",
              in_msg_id, msg_id));
        exh_Throw(RET_SP_INVALID_MSG_RECEIVED);
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ProcessGetPermRep(sp_ctx_t           *io_ctx,
                    strm_t             *io_msg,
                    u_int32_t           in_msg_len,
                    const po_req_t     *in_req,
                    po_key_ref_t       *out_key_ref,
                    u_int8_t           *out_ckey,
                    po_key_info_t      *out_key_info)
{
    u_int16_t   status, extinfo_size;
    const u_int8_t     *urr_sp, *urr_reserved, *urr_wkey_id, *urr_wkey_flag;
    u_int8_t   *ckey_id_sp, *ckey_id_reserved, *ckey_id_wkey_id, *ckey_id_wkey_flag;

#if !defined(NOT_SUPPORT_DL)
    u_int16_t   expinfo_size;
    bool_t      expinfo_size_ver_fail = FALSE;
#endif /* !NOT_SUPPORT_DL */

    exh_InitOk();
    MSGENT();

    exh_Check(l_VerifyHeader(io_msg, SP_MSG_ID_GETPERM_REP));

    /*
     * [MIPTV] 4.2.4.2: Processing Rules for Status
     */
    strm_BE2R(io_msg, status);
    if (status != SP_STATUS_SUCCESS) {
        if (status < SP_STATUS_OTHERS || SP_STATUS_ACTION_DENIED < status) {
            MSG1(("*** Verification Failure: Unexpected status value (0x%04x)\n", status));
            exh_Throw(RET_SP_VERIFICATION_FAILURE);
        } else {
            exh_Throw(RET_SP_STATUS(status));
        }
    }

    /* status extension */
    memset(out_key_info, 0, sizeof(po_key_info_t));
    *out_key_ref = in_req->key_ref;

    switch (out_key_ref->action_id) {
    case PO_ACTION_ID_EXTRACT_VOD:
        strm_CPYR(io_msg, out_ckey, PO_CKEY_LEN);

        /*
         * [MIPTV] 4.2.4.3: Processing Rules for ExtractInfoSize
         */
        strm_BE2R(io_msg, extinfo_size);
        if(extinfo_size != SP_GETPERM_REQ_EXTINFO_LEN) {
            MSG1(("*** Verification Failure: ExtractInfoSize is wrong...\n"
                  "      expected = %d, actual = %d\n",
                  SP_GETPERM_REQ_EXTINFO_LEN, extinfo_size));
            exh_Throw(RET_SP_VERIFICATION_FAILURE);
        }

        strm_BE4R(io_msg, out_key_info->not_before);
        strm_BE4R(io_msg, out_key_info->not_after);
        strm_BE2R(io_msg, out_key_info->oci);

        out_key_info->flags = PO_KEY_FLAG_OCI | PO_KEY_FLAG_NOT_BEFORE_AFTER;

        if (out_key_info->not_before != 0 || out_key_info->not_after != 0) {
            MSG1(("*** SDK ERROR: Validity period should be all 0 at VOD...\n"
                  "      not_before = 0x%08x, not_after = 0x%08x\n",
                  out_key_info->not_before, out_key_info->not_after));
            exh_Throw(RET_SP_INVALID_MSG_RECEIVED);
        }

        break;
    case PO_ACTION_ID_EXTRACT_IPMC:
        strm_CPYR(io_msg, out_ckey, PO_CKEY_LEN);
        strm_CPYR(io_msg, out_key_info->ckey_id, PO_CKEY_ID_LEN);
        strm_BE2R(io_msg, out_key_info->priv_data);
        strm_CPYR(io_msg, out_key_info->tierbits, PO_TIERBITS_LEN);

        /*
         * [MIPTV] 4.2.4.4: Processing Rules for WorkKeyID (ReservedByte)
         */
        if (out_key_info->ckey_id[SP_CID_RESERVED_OFFSET] != 0x00) {
            MSG1(("*** Verification Failure: ReservedByte is wrong...\n"
                  "      expected = 0x%02x, actual = 0x%02x\n",
                  0x00, out_key_info->ckey_id[SP_CID_RESERVED_OFFSET]));
            exh_Throw(RET_SP_VERIFICATION_FAILURE);
        }

        /*
         * [MIPTV] 4.2.4.4: Processing Rules for ExtractInfoSize
         */
        strm_BE2R(io_msg, extinfo_size);
        if(extinfo_size != SP_GETPERM_REQ_EXTINFO_LEN) {
            MSG1(("*** Verification Failure: ExtractInfoSize is wrong...\n"
                  "      expected = %d, actual = %d\n",
                  SP_GETPERM_REQ_EXTINFO_LEN, extinfo_size));
            exh_Throw(RET_SP_VERIFICATION_FAILURE);
        }

        strm_BE4R(io_msg, out_key_info->not_before);
        strm_BE4R(io_msg, out_key_info->not_after);
        strm_BE2R(io_msg, out_key_info->oci);

        out_key_info->flags = PO_KEY_FLAG_CKEY_ID | PO_KEY_FLAG_PRIV_DATA
            | PO_KEY_FLAG_TIERBITS | PO_KEY_FLAG_OCI
            | PO_KEY_FLAG_NOT_BEFORE_AFTER;

        /*
         * [MIPTV] 4.2.4.4.1: Processing Rules for NotBefore/NotAfter
         */
        if (out_key_info->not_before == 0x00000000 ||
            out_key_info->not_after  == 0x00000000 ||
            (out_key_info->not_before != 0xFFFFFFFF &&
             out_key_info->not_before >= out_key_info->not_after)) {
            MSG1(("*** Verification Failure: Validity period is invalid...\n"
                  "      not_before = 0x%08x, not_after = 0x%08x\n",
                  out_key_info->not_before, out_key_info->not_after));
            exh_Throw(RET_SP_VERIFICATION_FAILURE);
        }

        /*
         * [IPTVESMC] 3.1.2.2: Processing Rules for NotAfter
         */
        if (out_key_info->priv_data != 0x0000 &&
            (out_key_info->not_after <  0x0000FFFF ||
             out_key_info->not_after == 0xFFFFFFFF)) {
            MSG1(("*** Verification Failure: Validity period is invalid...\n"
                  "      not_before = 0x%08x, not_after = 0x%08x, priv_data = 0x%04x\n",
                  out_key_info->not_before, out_key_info->not_after, out_key_info->priv_data));
            exh_Throw(RET_SP_VERIFICATION_FAILURE);
        }

        /*
         * [IPTVESMC] 3.1.2.2: Processing Rules for ServiceProviderID (Pair Check)
         *   - Pair check between SPID of URR(odd) and SPID of URR(even)
         *     have already been done in l_CheckRequestIPMC.
         *   - In addition to above condition, if SPID of URR and SPID of WKID
         *     are the same value for both of odd and even keys, it means that
         *     SPID of WKID(odd) and SPID of WKID(even) are the same value.
         */
        urr_sp = &out_key_ref->urr[SP_URR_SP_OFFSET];
        ckey_id_sp = &out_key_info->ckey_id[SP_CID_SP_OFFSET];
        if (urr_sp[0] != ckey_id_sp[0] || urr_sp[1] != ckey_id_sp[1]) {
            MSG1(("*** Verification Failure: ServiceProviderIDs are mismatched...\n"
                  "      URR: 0x%02x%02x, CkeyID: 0x%02x%02x\n",
                  urr_sp[0], urr_sp[1], ckey_id_sp[0], ckey_id_sp[1]));
            exh_Throw(RET_SP_VERIFICATION_FAILURE);
        }

        /*
         * [IPTVESMC] 3.1.2.2: Processing Rules for ReservedByte (Pair Check)
         *   - Pair check between RByte of URR(odd) and RByte of URR(even)
         *     have already been done in l_CheckRequestIPMC.
         *   - In addition to above condition, if RByte of URR and RByte of WKID
         *     are the same value for both of odd and even keys, it means that
         *     RByte of WKID(odd) and RByte of WKID(even) are the same value.
         */
        urr_reserved = &out_key_ref->urr[SP_URR_RESERVED_OFFSET];
        ckey_id_reserved = &out_key_info->ckey_id[SP_CID_RESERVED_OFFSET];
        if (urr_reserved[0] != ckey_id_reserved[0]) {
            MSG1(("*** Verification Fialure: ReservedBytes are mismatched...\n"
                  "      URR: 0x%02x, CkeyID: 0x%02x\n",
                  urr_reserved[0], ckey_id_reserved[0]));
            exh_Throw(RET_SP_VERIFICATION_FAILURE);
        }

        /*
         * [IPTVESMC] 3.1.2.2: Processing Rules for WorkKeyManagementID (Pair Check)
         *   - Pair check between WKMID of URR(odd) and WKMID of URR(even)
         *     have already been done in l_CheckRequestIPMC.
         *   - In addition to above condition, if WKMID of URR and WKMID of WKID
         *     are the same value for both of odd and even keys, it means that
         *     WKMID of WKID(odd) and WKMID of WKID(even) are the same value.
         */
        urr_wkey_id = &out_key_ref->urr[SP_URR_WKEY_ID_OFFSET];
        ckey_id_wkey_id = &out_key_info->ckey_id[SP_CID_WKEY_ID_OFFSET];
        if (urr_wkey_id[0] != ckey_id_wkey_id[0] || urr_wkey_id[1] != ckey_id_wkey_id[1]) {
            MSG1(("*** Verification Failure: WorkKeyManagementIDs are mismatched...\n"
                  "      URR: 0x%02x%02x, CkeyID: 0x%02x%02x\n",
                  urr_wkey_id[0], urr_wkey_id[1], ckey_id_wkey_id[0], ckey_id_wkey_id[1]));
            exh_Throw(RET_SP_VERIFICATION_FAILURE);
        }

        /*
         * Check whether LSB of WKV of WKID is as same as odd/evenID of URR.
         * If these values are same, the order of odd/even is correct.
         */
        urr_wkey_flag = &out_key_ref->urr[SP_URR_FLAG_OFFSET];
        ckey_id_wkey_flag = &out_key_info->ckey_id[SP_CID_WKEY_VER_OFFSET];
        if (urr_wkey_flag[0] != (ckey_id_wkey_flag[0] & 0x1)) {
            MSG1(("*** SDK ERROR: LSB of WorkKeyVersions are mismatched...\n"
                  "      URR: 0x%02x, CkeyID: 0x%02x\n",
                  urr_wkey_flag[0], ckey_id_wkey_flag[0] & 0x1));
            exh_Throw(RET_SP_INVALID_MSG_RECEIVED);
        }

        break;
#if !defined(NOT_SUPPORT_DL)
    case PO_ACTION_ID_EXTRACT_DL:
        strm_CPYR(io_msg, out_ckey, PO_CKEY_LEN);

        /*
         * [MIPTV] 4.2.4.5: Processing Rules for ExtractInfoSize
         */

        strm_BE2R(io_msg, extinfo_size);
        if(extinfo_size != SP_GETPERM_REQ_EXTINFO_LEN) {
            MSG1(("*** Verification Failure: ExtractInfoSize is wrong...\n"
                  "      expected = %d, actual = %d\n",
                  SP_GETPERM_REQ_EXTINFO_LEN, extinfo_size));
            exh_Throw(RET_SP_VERIFICATION_FAILURE);
        }

        strm_BE4R(io_msg, out_key_info->not_before);
        strm_BE4R(io_msg, out_key_info->not_after);
        strm_BE2R(io_msg, out_key_info->oci);

        out_key_info->flags = PO_KEY_FLAG_OCI | PO_KEY_FLAG_NOT_BEFORE_AFTER;

        /*
         * [MIPTV] 4.2.4.5.1: Processing Rules for NotBefore/NotAfter
         */
        if (out_key_info->not_before == 0x00000000 ||
            out_key_info->not_after  == 0x00000000 ||
            (out_key_info->not_before != 0xFFFFFFFF &&
             out_key_info->not_before >= out_key_info->not_after)) {
            MSG1(("*** Verification Failure: NotBefore/NotAfter are invalid...\n"
                  "      not_before = 0x%08x, not_after = 0x%08x\n",
                  out_key_info->not_before, out_key_info->not_after));
            exh_Throw(RET_SP_VERIFICATION_FAILURE);
        }

        break;
    case PO_ACTION_ID_EXPORT_CP_DL:
        strm_CPYR(io_msg, out_ckey, PO_CKEY_LEN);

        /*
         * [MIPTV] 4.2.4.6: Processing Rules for ExportInfo
         */
        strm_BE2R(io_msg, expinfo_size);
        if(expinfo_size != in_msg_len - 24) {
            MSG1(("*** Verification Failure: ExportInfoSize is wrong...\n"
                  "      expected = %d, actual = %d\n",
                  in_msg_len - 24, expinfo_size));
            exh_Throw(RET_SP_VERIFICATION_FAILURE);
        }

        switch (out_key_ref->action_param) {
        case PO_ACTION_PARAM_EXPORT_CPRM_DVD:
        case PO_ACTION_PARAM_EXPORT_MGR_SVR_MEMSTICKPRO:
        case PO_ACTION_PARAM_EXPORT_MGR_SVR_EMPR:
            if (expinfo_size != 1) expinfo_size_ver_fail = TRUE;
            break;
        case PO_ACTION_PARAM_EXPORT_DTCP:
        case PO_ACTION_PARAM_EXPORT_CPRM_SDV:
        case PO_ACTION_PARAM_EXPORT_CPRM_SDA:
        case PO_ACTION_PARAM_EXPORT_MGR_SAR_MEMSTICK_MEMSTICKPRO:
        case PO_ACTION_PARAM_EXPORT_MGR_SAR_ATRAC:
            if (expinfo_size != 2) expinfo_size_ver_fail = TRUE;
            break;
        case PO_ACTION_PARAM_EXPORT_VCPS:
        case PO_ACTION_PARAM_EXPORT_SAFIA_IVDR_TV_REC:
        case PO_ACTION_PARAM_EXPORT_SAFIA_IVDR_AUDIO_REC:
            if (expinfo_size != 4) expinfo_size_ver_fail = TRUE;
            break;
        case PO_ACTION_PARAM_EXPORT_AACS_BLURAY_BDR_RE:
        case PO_ACTION_PARAM_EXPORT_AACS_BLURAY_RED_LASER:
            if (expinfo_size != 16) expinfo_size_ver_fail = TRUE;
            break;
        default:
            expinfo_size_ver_fail = TRUE;
        }

        if (!expinfo_size_ver_fail) {
            strm_CPYR(io_msg, out_key_info->export_info, expinfo_size);
            out_key_info->export_info_len = expinfo_size;
        } else {
            MSG1(("*** SDK ERROR: ExportInfoSize is wrong...\n"
                  "      expected = [1 - PO_EXPORT_INFO_MAX_LEN], actual = %d\n",
                  expinfo_size));
            exh_Throw(RET_SP_INVALID_MSG_RECEIVED);
        }

        out_key_info->flags = PO_KEY_FLAG_EXPORT_INFO;

        break;
#else
    case PO_ACTION_ID_EXTRACT_DL:
    case PO_ACTION_ID_EXPORT_CP_DL:
#endif /* !NOT_SUPPORT_DL */
    case PO_ACTION_ID_EXPORT_MV_DL:
    case PO_ACTION_ID_EXPORT_IPMC:
    case PO_ACTION_ID_RECORD:
        MSG1(("*** SDK ERROR: Unsupported action id (0x%02x)\n", out_key_ref->action_id));
        exh_Throw(RET_UNSUPPORTED);
        break;
    default:
        exh_Throw(RET_INTERNALERROR);
        break;
    }

    io_ctx->key_num++;

#if !defined(IPTVES_PUBLISH_BUILD)
    MSGI2(("---------------------------------------------------\n"));
    MSGI2(("Content Key\n"));
    MSGI2(("  urr         : ")); BDUMP2(PO_URR_LEN, out_key_ref->urr); MSG2(("\n"));
    MSGI2(("  urr_len     : %d\n", out_key_ref->urr_len));
    MSGI2(("  action_id   : 0x%02x\n", out_key_ref->action_id));
    MSGI2(("  action_param: 0x%02x\n", out_key_ref->action_param));
    MSGI2(("  ckey        : ")); BDUMP2(PO_CKEY_LEN, out_ckey); MSG2(("\n"));
    if (out_key_info->flags & PO_KEY_FLAG_CKEY_ID) {
        MSGI2(("  ckey_id     : ")); BDUMP2(PO_CKEY_ID_LEN, out_key_info->ckey_id); MSG2(("\n"));
    }
    if (out_key_info->flags & PO_KEY_FLAG_PRIV_DATA) {
        MSGI2(("  priv_data   : 0x%04x\n", out_key_info->priv_data));
    }
    if (out_key_info->flags & PO_KEY_FLAG_TIERBITS) {
        MSGI2(("  tierbits    : ")); BDUMP2(PO_TIERBITS_LEN, out_key_info->tierbits); MSG2(("\n"));
    }
    if (out_key_info->flags & PO_KEY_FLAG_OCI) {
        MSGI2(("  oci         : 0x%04x\n", out_key_info->oci));
    }
    if (out_key_info->flags & PO_KEY_FLAG_NOT_BEFORE_AFTER) {
        MSGI2(("  not_before  : 0x%08x\n", out_key_info->not_before));
        MSGI2(("  not_after   : 0x%08x\n", out_key_info->not_after));
    }
    MSGI2(("---------------------------------------------------\n"));
#endif /* !IPTVES_PUBLISH_BUILD */

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ProcessGetTTRep(sp_ctx_t             *io_ctx,
                  strm_t               *io_msg)
{
    u_int16_t status;

    exh_InitOk();
    MSGENT();

    exh_Check(l_VerifyHeader(io_msg, SP_MSG_ID_GETTT_REP));

    /*
     * [MIPTV] 4.2.4.10: Processing Rules for Status
     */
    strm_BE2R(io_msg, status);
    if (status != SP_STATUS_SUCCESS) {
        if (status == SP_STATUS_OTHERS      ||
            status == SP_STATUS_VERSION_ERR ||
            status == SP_STATUS_ACTION_FAILED) {
            exh_Throw(RET_SP_STATUS(status));
        } else {
            MSG1(("*** Verification Failure: Unexpected status value (0x%04x)\n", status));
            exh_Throw(RET_SP_VERIFICATION_FAILURE);
        }
    }

    /* datetime */
    strm_BE4R(io_msg, io_ctx->tt); io_ctx->is_tt_processed = TRUE;

    MSGI2(("---------------------------------------------------\n"));
    MSGI2(("Trusted Time\n"));
    MSGI2(("  tt          : 0x%08x, %s\n", io_ctx->tt, base_CMintime(&io_ctx->tt)));
    MSGI2(("---------------------------------------------------\n"));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ProcessPackedMsgRep(sp_ctx_t         *io_ctx,
                      strm_t           *io_msg,
                      const po_req_t   *in_req_lst,
                      po_key_ref_t     *out_key_ref_lst,
                      u_int8_t         *out_ckey_lst,
                      po_key_info_t    *out_key_info_lst)
{
    u_int16_t status;
    u_int32_t req_num = in_req_lst[0].num;
    u_int32_t msg_num;
    u_int32_t all_packed_msg_len = strm_Len(io_msg);
    u_int32_t all_rep_msg_len = 0;
    u_int32_t i;

    exh_InitOk();
    MSGENT();

    exh_Check(l_VerifyHeader(io_msg, SP_MSG_ID_PACKED_REP));

    /*
     * [MIPTV] 4.2.4.12: Processing Rules for Status
     */
    strm_BE2R(io_msg, status);
    if (status != SP_STATUS_SUCCESS) {
        if (status == SP_STATUS_OTHERS      ||
            status == SP_STATUS_VERSION_ERR ||
            status == SP_STATUS_MSG_FORMAT_ERR) {
            exh_Throw(RET_SP_STATUS(status));
        } else {
            MSG1(("*** Verification Failure: Unexpected status value (0x%04x)\n", status));
            exh_Throw(RET_SP_VERIFICATION_FAILURE);
        }
    }

    strm_BE2R(io_msg, msg_num);
    if (msg_num != req_num) {
        MSG1(("*** SDK ERROR: NumberOfMessageBoxed is wrong...\n"
              "      expected = %d, actual = %d\n",
              req_num, msg_num));
        exh_Throw(RET_SP_INVALID_MSG_RECEIVED);
    }

    for (i = 0; i < msg_num; i++) {
        const po_req_t *req      = &in_req_lst[i];
        po_key_ref_t   *key_ref  = &out_key_ref_lst[i];
        u_int8_t       *ckey     = &out_ckey_lst[i * PO_CKEY_LEN];
        po_key_info_t  *key_info = &out_key_info_lst[i];
        u_int16_t       msg_len;
        u_int32_t       before;
        u_int32_t       after;

        strm_BE2R(io_msg, msg_len);

        before = strm_Len(io_msg);

        if (req->type == PO_REQ_TYPE_GETPERM)
            exh_Check(l_ProcessGetPermRep(io_ctx, io_msg, msg_len, req,
                                          key_ref, ckey, key_info));
        else
            exh_Check(l_ProcessGetTTRep(io_ctx, io_msg));

        after  = strm_Len(io_msg);

        if (before - after != msg_len) {
            MSG1(("*** SDK ERROR: ReplyMessageSize is wrong...\n"
                  "      expected = %d, actual = %d\n",
                  before - after, msg_len));
            exh_Throw(RET_SP_INVALID_MSG_RECEIVED);
        }

        all_rep_msg_len += msg_len;
    }
    exh_Check(l_CheckReply(in_req_lst, out_key_info_lst));

    /*
     * [MIPTV] 4.2.4.12: Processing Rules for ReplyMessageSize
     */
    if (all_rep_msg_len != (all_packed_msg_len - (8 + msg_num * 2))) {
        MSG1(("*** Verification Failure: ReplyMessageSize is wrong...\n"));
        exh_Throw(RET_SP_VERIFICATION_FAILURE);
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}


int dump_po_req(const po_req_t *in_req_lst)
{
	int i = 0;
	if(NULL == in_req_lst)
	{
		dprintf("%s invalid parameters\n");
		return -1;
	}
	dprintf("\t dump po_req:\n");
	dprintf("\t type:%02x\n", in_req_lst->type);
	dprintf("\t num:%02x\n", in_req_lst->num);
	dprintf("\t key_ref:\n");
	
	dprintf("\t\t  urr:\n");
	dprintf("\t\t\t");
	for(i=0; i<PO_URR_LEN; i++)
		dprintf(" %02x ", in_req_lst->key_ref.urr[i]);
	dprintf("\n");
	
	dprintf("\t\t  urr_len:%02x\n", in_req_lst->key_ref.urr_len);
	dprintf("\t\t  action_id:%02x\n", in_req_lst->key_ref.action_id);
	dprintf("\t\t  action_param:%02x\n", in_req_lst->key_ref.action_param);
	return 0;
}



/*============================================================================
 * Global Functions
 *==========================================================================*/
int
sp_Construct(po_thid_t                  in_thid,
             u_int32_t                  in_req_num,
             const po_req_t            *in_req_lst,
             sp_handle_t               *out_handle)
{
    sp_ctx_t *sp_ctx = NULL;
	int i = 0;
	
    exh_InitOk();
    MSGENT();

	dprintf(">>sp_Construct\n");
	for(i=0; i<in_req_num; i++)
	{
		dump_po_req(&(in_req_lst[i]));
	}

    if ((in_thid >= PO_THID_MAX) ||
        (in_req_num != 0 && in_req_lst == NULL) ||
        (in_req_num == 0 && in_req_lst != NULL) ||
        out_handle == NULL)
        exh_Throw(RET_ILLEGALARGS);

    if (in_req_num == 0 && in_req_lst == NULL) {
        *out_handle = (sp_handle_t *)NULL;
        exh_Rethrow();
    }

    exh_Check(l_CheckRequest(in_req_num, in_req_lst));

    sp_ctx = &l_sp_ctx[in_thid];
    memset(sp_ctx, 0, sizeof(sp_ctx_t));

    sp_ctx->thid            = in_thid;
    sp_ctx->req_num         = in_req_num;
	
    // fix: !!!! sp_ctx->req_lst         = in_req_lst;
    sp_ctx->req_lst = malloc(in_req_num * sizeof(po_req_t));
	if(NULL == sp_ctx->req_lst)
	{
		dprintf("%s malloc req_lst error", __FUNCTION__);
		*out_handle = NULL;
		return -1;
	}
	else
	{
		dprintf("%s malloc req_lst ok", __FUNCTION__);
	}
	memcpy_n2s_chk(sp_ctx->req_lst, in_req_lst, in_req_num * sizeof(po_req_t), NULL);
	dump_po_req(sp_ctx->req_lst);

	
    sp_ctx->cur_make_req    = 0;
    sp_ctx->cur_process_req = 0;
    sp_ctx->is_tt_processed = FALSE;
    sp_ctx->tt              = 0x00000000;

    *out_handle = (sp_handle_t *)sp_ctx;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
sp_Destruct(sp_handle_t                 io_handle,
            u_int32_t                  *out_success_num)
{
    sp_ctx_t *sp_ctx = (sp_ctx_t *)io_handle;
    u_int32_t success_num = 0;

    exh_InitOk();
    MSGENT();

    if (sp_ctx) {
        success_num = sp_ctx->cur_process_req;

		if(NULL == sp_ctx->req_lst)
		{
			dprintf("%s free req_lst ok", __FUNCTION__);
			free(sp_ctx->req_lst);
			sp_ctx->req_lst = NULL;
		}		
    }

    if (out_success_num)
        *out_success_num = success_num;

    MSGEXT(exh_Get());
    exh_Return();
}

int
sp_MakeMessage(sp_handle_t              io_handle,
               u_int8_t                *out_msg,
               u_int32_t               *io_msg_len,
               bool_t                  *out_need_to_rec)
{
    sp_ctx_t       *sp_ctx = (sp_ctx_t *)io_handle;
    const po_req_t *req_lst;
    u_int32_t       req_num;
    bool_t          need_to_rec = FALSE;
    strm_t          msg;

    exh_InitOk();
    MSGENT();

    if (sp_ctx == NULL)
        exh_Throw(RET_ILLEGALHANDLE);
    if (out_msg == NULL || io_msg_len == NULL)
        exh_Throw(RET_ILLEGALARGS);

    if (sp_ctx->cur_make_req >= sp_ctx->req_num) {
        MSGI3(("No more request.\n"));
        exh_Throw(RET_SP_NO_MORE_REQUEST);
    }

    strm_Init(&msg, out_msg, *io_msg_len);

    req_lst = &sp_ctx->req_lst[sp_ctx->cur_make_req];
    req_num = req_lst[0].num;
	
	dprintf("==============%s req_num:%d\n", __FUNCTION__, req_num);
	
    if (req_num == 1) {
        const po_req_t *req = &req_lst[0];

        if (req->type == PO_REQ_TYPE_GETPERM)
            exh_Check(l_MakeGetPermReq(sp_ctx, &req->key_ref, &msg, &need_to_rec));
        else
            exh_Check(l_MakeGetTTReq(sp_ctx, &msg));
    } else {
        exh_Check(l_MakePackedMsgReq(sp_ctx, req_lst, &msg, &need_to_rec));
    }

    sp_ctx->cur_make_req += req_num;

    *io_msg_len      = strm_Len(&msg);
    *out_need_to_rec = need_to_rec;

exh_CLEANUP:
    if (exh_Get() == RET_STRM_WRITE_ERROR)
        exh_Set(RET_SP_TOO_LONG_PACKED_REQ);

    MSGEXT(exh_Get());
    exh_Return();
}

int
sp_ProcessMessage(sp_handle_t           io_handle,
                  u_int8_t             *in_msg,
                  u_int32_t             in_msg_len,
                  u_int8_t             *in_srv_sbj,
                  u_int8_t              in_trans_flag)
{
    sp_ctx_t       *sp_ctx = (sp_ctx_t *)io_handle;
    u_int32_t       req_num;
    const po_req_t *req_lst;
    po_key_ref_t    key_ref_lst[L_PACKED_REQUEST_MAX];
    u_int8_t        ckey_lst[L_PACKED_REQUEST_MAX * PO_CKEY_LEN];
    po_key_info_t   key_info_lst[L_PACKED_REQUEST_MAX];
    strm_t          msg;

    exh_InitOk();
    MSGENT();

    if (sp_ctx == NULL)
        exh_Throw(RET_ILLEGALHANDLE);
    if (in_msg == NULL)
        exh_Throw(RET_ILLEGALARGS);

    if (sp_ctx->cur_process_req >= sp_ctx->req_num) {
        MSGI3(("No more request.\n"));
        exh_Throw(RET_SP_NO_MORE_REQUEST);
    }

    sp_ctx->is_tt_processed = FALSE;
    sp_ctx->key_num         = 0;

    strm_Set(&msg, in_msg, in_msg_len);

    req_lst = &sp_ctx->req_lst[sp_ctx->cur_process_req];
    req_num = req_lst->num;

    if (req_num == 1) {
        const po_req_t *req      = &req_lst[0];
        po_key_ref_t   *key_ref  = &key_ref_lst[0];
        u_int8_t       *ckey     = &ckey_lst[0];
        po_key_info_t  *key_info = &key_info_lst[0];

        if (req->type == PO_REQ_TYPE_GETPERM)
            exh_Check(l_ProcessGetPermRep(sp_ctx, &msg, in_msg_len, req,
                                          key_ref, ckey, key_info));
        else
            exh_Check(l_ProcessGetTTRep(sp_ctx, &msg));
    } else {
        exh_Check(l_ProcessPackedMsgRep(sp_ctx, &msg, req_lst,
                                        key_ref_lst, ckey_lst, key_info_lst));
    }

    if (strm_Len(&msg) > 0) {
        MSG1(("*** SDK ERROR: Reply message is too much.\n"));
        exh_Throw(RET_SP_INVALID_MSG_RECEIVED);
    }

    if (sp_ctx->key_num > 0) {
        exh_Set(keymng_SetContentKey(sp_ctx->thid,
                                     key_ref_lst, ckey_lst, key_info_lst,
                                     sp_ctx->key_num, in_srv_sbj, in_trans_flag));
        if (!exh_IsOk()) {
            if (exh_Get() == RET_NOMEM)
                exh_Rethrow();
#if !defined(NOT_SUPPORT_DL)
            else if (exh_Get() == RET_KEYMNG_TOO_MANY_LICENSES)
                exh_Throw(RET_SP_TOO_MANY_LICENSES);
#endif
            else if (exh_Get() == RET_KEYMNG_SAVE_ERROR)
                exh_Throw(RET_SP_SAVE_ERROR);
            else
                exh_Throw(RET_INTERNALERROR);
        }
    }

    if (sp_ctx->is_tt_processed) {
        exh_Set(time_SetTrustedTime(sp_ctx->tt));
        if (!exh_IsOk()) exh_Set(RET_INTERNALERROR);
    }

    sp_ctx->cur_process_req += req_num;

exh_CLEANUP:
    if (exh_Get() == RET_STRM_READ_ERROR)
        exh_Set(RET_SP_INVALID_MSG_RECEIVED);

    MSGEXT(exh_Get());
    exh_Return();
}

#if !defined(IPTVES_PUBLISH_BUILD)
/*----------------------------------------------------------------------------
 * Functions for Test/Tools
 *--------------------------------------------------------------------------*/
int
sp_SetManufacturerForCTS(void)
{
    exh_InitOk();
    MSGENT();

    exh_Check(base_Memcpy(l_dev_info      + 3, 2, L_DEVINFO_MANUFACTURER_CTS, 2));
    exh_Check(base_Memcpy(l_dev_info_rtdb + 3, 2, L_DEVINFO_MANUFACTURER_RTDB_CTS, 2));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}
#endif /* !IPTVES_PUBLISH_BUILD */
