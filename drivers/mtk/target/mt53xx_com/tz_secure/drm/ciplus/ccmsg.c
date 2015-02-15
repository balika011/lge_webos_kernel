/*****************************************************************************/
/* (c) Copyright Ocean Blue Software Ltd. - ALL RIGHTS RESERVED              */
/* This document is the CONFIDENTIAL property of Ocean Blue Software Ltd.    */
/*****************************************************************************/
/*                                                                           */
/* File Name:     cctype.c                                                   */
/*                                                                           */
/* Description:   CI+ Content Control Resource: Message processing           */
/*                                                                           */
/* Project:       CI / CI+ Library                                           */
/*                                                                           */
/* Creation Date: 28/01/2009                                                 */
/*                                                                           */
/*****************************************************************************/

/* --- includes for this file --- */
/* Ocean Blue header files */
#include "ccdef.h"
#include "stbcihw.h"
#include "ccactx.h"
#include "ccfunc.h"
#include "ccmsg.h"
#include "ccsac.h"
#include "cccert.h"
#include "sectk.h"
#include "ci_trust_zone.h"

/* --- constant definitions for this file --- */

#define CC_DEBUG_PRINT(x) CC_PRINT(x)
#define CC_DEBUG_BUFFER(x,y) CC_PRINT_BUFFER(x,y)

#define CC_DEBUG_PRINT_FORCE(x) CC_PRINT(x)
#define CC_DEBUG_BUFFER_FORCE(x,y) CC_PRINT_BUFFER(x,y)

#define FUNCTION_START(x)
#define FUNCTION_FINISH(x)
/* --- local typedef structs for this file --- */

typedef struct req_msg_info
{
   E_CC_MSG_TYPE msg_type;
   U8BIT *send_datatype;
   U32BIT send_datatype_nbr;
   U8BIT *request_datatype;
   U32BIT request_datatype_nbr;
} S_REQ_MSG_INFO;
typedef struct cnf_msg_info
{
   E_CC_MSG_TYPE msg_type;
   U8BIT *send_datatype;
   U32BIT send_datatype_nbr;
} S_CNF_MSG_INFO;

/* --- local (static) variable declarations for this file --- */
/* (internal variables declared static to make them local) */

/* Sent and requested data for each message */
static U8BIT auth_nonce_send_ids[] =
{
   DT_ID_AUTH_NONCE
};
static U8BIT auth_nonce_req_ids[] =
{
   DT_ID_DHPH,
   DT_ID_SIGNATURE_A,
   DT_ID_HOST_DEV_CERT,
   DT_ID_HOST_BRAND_CERT
};
static U8BIT auth_signature_send_ids[] =
{
   DT_ID_DHPM,
   DT_ID_SIGNATURE_B,
   DT_ID_CICAM_DEV_CERT,
   DT_ID_CICAM_BRAND_CERT
};
static U8BIT auth_signature_req_ids[] =
{
   DT_ID_STATUS_FIELD
};
static U8BIT auth_key_send_ids[] =
{
   /* No items -- ANSI C doesn't like this, so a dummy item follows*/
   0
};
static U8BIT auth_key_req_ids[] =
{
   DT_ID_AKH
};
static U8BIT sac_data_send_ids[] =
{
   DT_ID_CICAM_ID,
   DT_ID_NS_MODULE
};
static U8BIT sac_data_legacy_send_ids[] =
{
   /* This was true in v1.00, but was fixed in v1.1 */
   DT_ID_CICAM_ID,
   DT_ID_NS_MODULE,
   DT_ID_KEY_REGISTER
};
static U8BIT sac_data_req_ids[] =
{
   DT_ID_HOST_ID,
   DT_ID_NS_HOST
};
static U8BIT key_data_send_ids[] =
{
   DT_ID_CICAM_ID,
   DT_ID_KP,
   DT_ID_KEY_REGISTER
};
static U8BIT key_data_req_ids[] =
{
   DT_ID_HOST_ID,
   DT_ID_STATUS_FIELD
};
static U8BIT uri_versions_send_ids[] =
{
   /* No items -- ANSI C doesn't like this, so a dummy item follows*/
   0
};
static U8BIT uri_versions_req_ids[] =
{
   DT_ID_URI_VERSIONS
};
static U8BIT uri_message_send_ids[] =
{
   DT_ID_URI_MESSAGE,
   DT_ID_PROGRAM_NUMBER
};
static U8BIT uri_message_req_ids[] =
{
   DT_ID_URI_CONFIRM
};
static U8BIT srm_data_send_ids[] =
{
   DT_ID_SRM_DATA
};
static U8BIT srm_data_req_ids[] =
{
   DT_ID_STATUS_FIELD,
   DT_ID_SRM_CONFIRM
};
static U8BIT record_start_send_ids[] =
{
   DT_ID_OPERATING_MODE,
   DT_ID_PROGRAM_NUMBER
};
static U8BIT record_start_pin_send_ids[] =
{
   DT_ID_OPERATING_MODE,
   DT_ID_PROGRAM_NUMBER,
   DT_ID_PINCODE_DATA
};
static U8BIT record_start_req_ids[] =
{
   DT_ID_RECORD_START_STATUS
};
static U8BIT cicam_license_send_ids[] =
{
   DT_ID_PROGRAM_NUMBER,
   DT_ID_LICENSE_STATUS,
   DT_ID_URI_MESSAGE
};
static U8BIT cicam_license_ex_send_ids[] =
{
   DT_ID_PROGRAM_NUMBER,
   DT_ID_LICENSE_STATUS,
   DT_ID_URI_MESSAGE,
   DT_ID_CICAM_LICENSE
};
static U8BIT cicam_license_req_ids[] =
{
   DT_ID_LICENSE_RCVD_STATUS
};
static U8BIT license_check_send_ids[] =
{
   DT_ID_HOST_LICENSE
};
static U8BIT license_check_req_ids[] =
{
   DT_ID_LICENSE_STATUS,
   DT_ID_PLAY_COUNT
};
static U8BIT mode_change_send_ids[] =
{
   DT_ID_OPERATING_MODE,
   DT_ID_PROGRAM_NUMBER
};
static U8BIT mode_change_req_ids[] =
{
   DT_ID_MODE_CHANGE_STATUS
};
static U8BIT record_stop_send_ids[] =
{
   DT_ID_PROGRAM_NUMBER
};
static U8BIT record_stop_req_ids[] =
{
   DT_ID_RECORD_STOP_STATUS
};
static U8BIT playback_license_send_ids[] =
{
   DT_ID_PROGRAM_NUMBER,
   DT_ID_HOST_LICENSE
};
static U8BIT playback_license_req_ids[] =
{
   DT_ID_PROGRAM_NUMBER,
   DT_ID_LICENSE_STATUS,
   DT_ID_URI_MESSAGE,
   DT_ID_CICAM_LICENSE
};

/* Message information */
static S_REQ_MSG_INFO valid_req_messages[] =
{
   {
      CC_MSG_TYPE_AUTH_NONCE,
      auth_nonce_send_ids,
      sizeof(auth_nonce_send_ids),
      auth_nonce_req_ids,
      sizeof(auth_nonce_req_ids),
   },
   {
      CC_MSG_TYPE_AUTH_SIGNATURE,
      auth_signature_send_ids,
      sizeof(auth_signature_send_ids),
      auth_signature_req_ids,
      sizeof(auth_signature_req_ids)
   },
   {
      CC_MSG_TYPE_AUTH_KEY,
      auth_key_send_ids,
      0,
      auth_key_req_ids,
      sizeof(auth_key_req_ids)
   },
   {
      CC_MSG_TYPE_SAC_DATA,
      sac_data_send_ids,
      sizeof(sac_data_send_ids),
      sac_data_req_ids,
      sizeof(sac_data_req_ids)
   },
   {
      CC_MSG_TYPE_SAC_DATA_LEGACY,
      sac_data_legacy_send_ids,
      sizeof(sac_data_legacy_send_ids),
      sac_data_req_ids,
      sizeof(sac_data_req_ids)
   }
};

static S_REQ_MSG_INFO valid_sac_req_messages[] =
{
   {
      CC_MSG_TYPE_KEY_DATA,
      key_data_send_ids,
      sizeof(key_data_send_ids),
      key_data_req_ids,
      sizeof(key_data_req_ids)
   },
   {
      CC_MSG_TYPE_URI_VERSIONS,
      uri_versions_send_ids,
      0,
      uri_versions_req_ids,
      sizeof(uri_versions_req_ids)
   },
   {
      CC_MSG_TYPE_URI_MESSAGE,
      uri_message_send_ids,
      sizeof(uri_message_send_ids),
      uri_message_req_ids,
      sizeof(uri_message_req_ids)
   },
   {
      CC_MSG_TYPE_SRM_DATA,
      srm_data_send_ids,
      sizeof(srm_data_send_ids),
      srm_data_req_ids,
      sizeof(srm_data_req_ids)
   },
   {
      CC_MSG_TYPE_RECORD_START,
      record_start_send_ids,
      sizeof(record_start_send_ids),
      record_start_req_ids,
      sizeof(record_start_req_ids)
   },
   {
      CC_MSG_TYPE_RECORD_START,
      record_start_pin_send_ids,
      sizeof(record_start_pin_send_ids),
      record_start_req_ids,
      sizeof(record_start_req_ids)
   },
   {
      CC_MSG_TYPE_CICAM_LICENSE,
      cicam_license_send_ids,
      sizeof(cicam_license_send_ids),
      cicam_license_req_ids,
      sizeof(cicam_license_req_ids)
   },
   {
      CC_MSG_TYPE_CICAM_LICENSE,
      cicam_license_ex_send_ids,
      sizeof(cicam_license_ex_send_ids),
      cicam_license_req_ids,
      sizeof(cicam_license_req_ids)
   },
   {
      CC_MSG_TYPE_LICENSE_CHECK,
      license_check_send_ids,
      sizeof(license_check_send_ids),
      license_check_req_ids,
      sizeof(license_check_req_ids)
   },
   {
      CC_MSG_TYPE_MODE_CHANGE,
      mode_change_send_ids,
      sizeof(mode_change_send_ids),
      mode_change_req_ids,
      sizeof(mode_change_req_ids)
   },
   {
      CC_MSG_TYPE_RECORD_STOP,
      record_stop_send_ids,
      sizeof(record_stop_send_ids),
      record_stop_req_ids,
      sizeof(record_stop_req_ids)
   },
   {
      CC_MSG_TYPE_PLAYBACK_LICENSE,
      playback_license_send_ids,
      sizeof(playback_license_send_ids),
      playback_license_req_ids,
      sizeof(playback_license_req_ids)
   }
};

static S_CNF_MSG_INFO valid_sac_cnf_messages[] =
{
   {
      CC_MSG_TYPE_RECORD_START_STATUS,
      record_start_req_ids,
      sizeof(record_start_req_ids),
   },
   {
      CC_MSG_TYPE_LICENSE_RCVD_STATUS,
      cicam_license_req_ids,
      sizeof(cicam_license_req_ids)
   },
   {
      CC_MSG_TYPE_LICENSE_STATUS,
      license_check_req_ids,
      sizeof(license_check_req_ids)
   },
   {
      CC_MSG_TYPE_MODE_CHANGE_STATUS,
      mode_change_req_ids,
      sizeof(mode_change_req_ids)
   },
   {
      CC_MSG_TYPE_RECORD_STOP_STATUS,
      record_stop_req_ids,
      sizeof(record_stop_req_ids)
   },
   {
      CC_MSG_TYPE_PLAYBACK_LICENSE_REPLY,
      playback_license_req_ids,
      sizeof(playback_license_req_ids)
   }
};

/* --- local function prototypes for this file --- */
/* (internal functions declared static to make them local) */

static E_CC_MSG_TYPE GetOpenReqType(U8BIT *data, U32BIT len);
static E_CC_MSG_TYPE GetDataReqType(U8BIT *data, U32BIT len);
static E_CC_MSG_TYPE GetSyncReqType(U8BIT *data, U32BIT len);
static E_CC_MSG_TYPE GetSacDataReqType(U8BIT *data, U32BIT len);
static E_CC_MSG_TYPE GetSacDataCnfType(U8BIT *data, U32BIT len);
static E_CC_MSG_TYPE GetSacSyncReqType(U8BIT *data, U32BIT len);
static BOOL IsValidDataReq(U8BIT *data, U32BIT len);
static BOOL IsValidDataCnf(U8BIT *data, U32BIT len);
static E_CC_MSG_TYPE IdentifyDataReq(U8BIT *data, U32BIT len);
static E_CC_MSG_TYPE IdentifySacDataReq(U8BIT *data, U32BIT len);
static E_CC_MSG_TYPE IdentifySacDataCnf(U8BIT *data, U32BIT len);
static E_CC_MSG_TYPE SearchReqMessage(U8BIT *data, U32BIT len, 
                                      S_REQ_MSG_INFO *messages,
                                      U32BIT message_count);
static E_CC_MSG_TYPE SearchCnfMessage(U8BIT *data, U32BIT len, 
                                      S_CNF_MSG_INFO *messages,
                                      U32BIT message_count);
 
static U32BIT GetOpenCnfLen(U8BIT *data, U32BIT len);
static U32BIT GetDataCnfLen(U8BIT *data, U32BIT len);
static U32BIT GetSyncCnfLen(U8BIT *data, U32BIT len);
static U32BIT GetSacDataCnfLen(U8BIT *data, U32BIT len);
static U32BIT GetSacSyncCnfLen(U8BIT *data, U32BIT len);
static void AddDatatypeLength(U8BIT id, void *userdata);
static void AddRequestedItem(U8BIT id, void *userdata);

static void LoopSentItems(U8BIT *data, U32BIT len,
                          void (*callback)(U8BIT id, U8BIT *data, U16BIT len,
                                           void *userdata),
                          void *userdata);
static void LoopRequestedItems(U8BIT *data, U32BIT len,
                               void (*callback)(U8BIT id, void *userdata),
                               void *userdata);

/*---------------------------------------------------------------------------*/
/* global function definitions                                               */
/*---------------------------------------------------------------------------*/

/*!**************************************************************************
 * @brief    Return message type
 * @param    tag - APDU tag
 * @param    data - message data
 * @return   Message type
 ****************************************************************************/
E_CC_MSG_TYPE CC_GetMessageType(U8BIT *data, U32BIT len)
{
   E_CC_MSG_TYPE msg_type;

   FUNCTION_START(CC_GetMessageType);

   msg_type = GetSacDataReqType(data, len);

   FUNCTION_FINISH(CC_GetMessageType);

   return msg_type;
}

/*!**************************************************************************
 * @brief    Return length of response for message
 * @param    tag - APDU tag
 * @param    data - message data
 * @param    len - message length
 * @return   Length of response (message data only, not including APDU header)
 ****************************************************************************/
U32BIT CC_GetResponseLength(U32BIT tag, U8BIT *data, U32BIT len)
{
   U32BIT response_len;

   FUNCTION_START(CC_GetResponseLength);

   switch (tag)
   {
   case CC_OPEN_CNF_TAG:
      response_len = GetOpenCnfLen(data, len);
      break;
   case CC_DATA_CNF_TAG:
      response_len = GetDataCnfLen(data, len);
      break;
   case CC_SYNC_CNF_TAG:
      response_len = GetSyncCnfLen(data, len);
      break;
   case CC_SAC_DATA_CNF_TAG:
      response_len = GetSacDataCnfLen(data, len);
      break;
   case CC_SAC_SYNC_CNF_TAG:
      response_len = GetSacSyncCnfLen(data, len);
      break;
   default:
      response_len = 0;
      ASSERT(0);
   }

   FUNCTION_FINISH(CC_GetResponseLength);

   return response_len;
}

/*!**************************************************************************
 * @brief    Return number of items to send in the response
 * @param    data - message data
 * @param    len - message length
 * @return   Number of items in the response
 * @note     This function assumes that the message is a valid data request,
 *           either cc_data_req or cc_sac_data_req.
 ****************************************************************************/
U8BIT CC_GetSendItemCount(U8BIT *data, U32BIT len)
{
   U8BIT request_datatype_nbr;

   FUNCTION_START(CC_GetSendItemCount);

   request_datatype_nbr = 0;
   LoopRequestedItems(data, len, AddRequestedItem, &request_datatype_nbr);

   FUNCTION_FINISH(CC_GetSendItemCount);

   return request_datatype_nbr;
}

/*!**************************************************************************
 * @brief    Loop over sent items and call the callback for each
 * @param    data - message data
 * @param    len - message length
 * @param    callback - callback to call for each item
 * @param    userdata - user data to pass to callback
 * @note     This function assumes that the message is a valid data request,
 *           either cc_data_req or cc_sac_data_req.
 ****************************************************************************/
void CC_LoopSentItems(U8BIT *data, U32BIT len,
                      void (*callback)(U8BIT id, U8BIT *data, U16BIT len,
                                       void *userdata),
                      void *userdata)
{
   FUNCTION_START(CC_LoopSentItems);

   LoopSentItems(data, len, callback, userdata);

   FUNCTION_FINISH(CC_LoopSentItems);
}

/*!**************************************************************************
 * @brief    Loop over requested items and call the callback for each
 * @param    data - message data
 * @param    len - message length
 * @param    callback - callback to call for each item
 * @param    userdata - user data to pass to callback
 * @note     This function assumes that the message is a valid data request,
 *           either cc_data_req or cc_sac_data_req.
 ****************************************************************************/
void CC_LoopRequestedItems(U8BIT *data, U32BIT len,
                           void (*callback)(U8BIT id, void *userdata),
                           void *userdata)
{
   FUNCTION_START(CC_LoopRequestedItems);

   LoopRequestedItems(data, len, callback, userdata);

   FUNCTION_FINISH(CC_LoopRequestedItems);
}

/*---------------------------------------------------------------------------*/
/* local function definitions                                                */
/*---------------------------------------------------------------------------*/

/*!**************************************************************************
 * @brief    Return message type for cc_open_req APDU
 * @param    data - message data
 * @param    len - message length
 * @return   Message type
 ****************************************************************************/
static E_CC_MSG_TYPE GetOpenReqType(U8BIT *data, U32BIT len)
{
   E_CC_MSG_TYPE msg_type;

   FUNCTION_START(GetOpenReqType);

   USE_UNWANTED_PARAM(data);

   if (len == 0)
   {
      msg_type = CC_MSG_TYPE_HOST_CAPS;
   }
   else
   {
      msg_type = CC_MSG_TYPE_UNKNOWN;
   }

   FUNCTION_FINISH(GetOpenReqType);

   return msg_type;
}

/*!**************************************************************************
 * @brief    Return message type for cc_data_req APDU
 * @param    data - message data
 * @param    len - message length
 * @return   Message type
 ****************************************************************************/
static E_CC_MSG_TYPE GetDataReqType(U8BIT *data, U32BIT len)
{
   E_CC_MSG_TYPE msg_type;
   BOOL valid;

   FUNCTION_START(GetDataReqType);

   msg_type = CC_MSG_TYPE_UNKNOWN;
   valid = IsValidDataReq(data, len);
   if (valid)
   {
      msg_type = IdentifyDataReq(data, len);
   }

   FUNCTION_FINISH(GetDataReqType);

   return msg_type;
}

/*!**************************************************************************
 * @brief    Return message type for cc_sync_req APDU
 * @param    data - message data
 * @param    len - message length
 * @return   Message type
 ****************************************************************************/
static E_CC_MSG_TYPE GetSyncReqType(U8BIT *data, U32BIT len)
{
   E_CC_MSG_TYPE msg_type;

   FUNCTION_START(GetSyncReqType);

   USE_UNWANTED_PARAM(data);

   if (len == 0)
   {
      msg_type = CC_MSG_TYPE_SAC_SYNC;
   }
   else
   {
      msg_type = CC_MSG_TYPE_UNKNOWN;
   }

   FUNCTION_FINISH(GetSyncReqType);

   return msg_type;
}

/*!**************************************************************************
 * @brief    Return message type for cc_sac_data_req APDU
 * @param    data - message data
 * @param    len - message length
 * @return   Message type
 ****************************************************************************/
static E_CC_MSG_TYPE GetSacDataReqType(U8BIT *data, U32BIT len)
{
   E_CC_MSG_TYPE msg_type;
   BOOL valid;

   FUNCTION_START(GetSacDataReqType);

   msg_type = CC_MSG_TYPE_UNKNOWN;
   valid = IsValidDataReq(data, len);
   if (valid)
   {
      msg_type = IdentifySacDataReq(data, len);
   }

   FUNCTION_FINISH(GetSacDataReqType);

   return msg_type;
}

/*!**************************************************************************
 * @brief    Return message type for cc_sac_data_cnf APDU
 * @param    data - message data
 * @param    len - message length
 * @return   Message type
 ****************************************************************************/
static E_CC_MSG_TYPE GetSacDataCnfType(U8BIT *data, U32BIT len)
{
   E_CC_MSG_TYPE msg_type;
   BOOL valid;

   FUNCTION_START(GetSacDataCnfType);

   msg_type = CC_MSG_TYPE_UNKNOWN;
   valid = IsValidDataCnf(data, len);
   if (valid)
   {
      msg_type = IdentifySacDataCnf(data, len);
   }

   FUNCTION_FINISH(GetSacDataCnfType);

   return msg_type;
}

/*!**************************************************************************
 * @brief    Return message type for cc_sac_sync_req APDU
 * @param    data - message data
 * @param    len - message length
 * @return   Message type
 ****************************************************************************/
static E_CC_MSG_TYPE GetSacSyncReqType(U8BIT *data, U32BIT len)
{
   E_CC_MSG_TYPE msg_type;

   FUNCTION_START(GetSacSyncReqType);

   USE_UNWANTED_PARAM(data);
   USE_UNWANTED_PARAM(len);

   /* TODO: Currently allowing any length, because CAM sends a 1-byte
    *       message. This should be disabled once the CAM is fixed.
    */
   msg_type = CC_MSG_TYPE_KEY_SYNC;

   FUNCTION_FINISH(GetSacSyncReqType);

   return msg_type;
}

/*!**************************************************************************
 * @brief    Tell whether a message is a valid cc_data_req message
 * @param    data - message data
 * @param    len - message length
 * @return   TRUE if the message is valid, FALSE otherwise
 ****************************************************************************/
static BOOL IsValidDataReq(U8BIT *data, U32BIT len)
{
   U32BIT offset;
   U8BIT cc_system_id_bitmask;
   U8BIT send_datatype_nbr;
   U8BIT send_datatype_found;
   U8BIT request_datatype_nbr;
   U16BIT datatype_length;
   BOOL valid;

   FUNCTION_START(IsValidDataReq);

   valid = TRUE;
   if (len >= 2)
   {
      cc_system_id_bitmask = data[0];
      if ((cc_system_id_bitmask & CC_SYSTEM_ID_BITMASK) != 0)
      {
         /* Supported CC system */
         send_datatype_nbr = data[1];
         send_datatype_found = 0;
         offset = 2;
         while ((send_datatype_found < send_datatype_nbr) &&
                (offset + 2 < len) &&
                (valid))
         {
            datatype_length = data[offset+1] << 8 | data[offset+2];
            if (offset + 2 + datatype_length < len)
            {
               offset += 3 + datatype_length;
               ++send_datatype_found;
            }
            else
            {
               /* Data item too long */
               valid = FALSE;
            }
         }
         if (send_datatype_found != send_datatype_nbr)
         {
            /* Incorrect number of sent items */
            valid = FALSE;
         }

         if ((offset < len) && (valid))
         {
            request_datatype_nbr = data[offset];
            if (offset + 1 + request_datatype_nbr <= len)
            {
               /* TODO: check padding? */
               valid = TRUE;
            }
            else
            {
               /* Invalid number of requested items */
               valid = FALSE;
            }
         }
         else
         {
            /* No requested items */
            valid = FALSE;
         }
      }
      else
      {
         /* Unsupported CC system */
         valid = FALSE;
      }
   }
   else
   {
      /* Request too short */
      valid = FALSE;
   }

   FUNCTION_FINISH(IsValidDataReq);

   return valid;
}

/*!**************************************************************************
 * @brief    Tell whether a message is a valid cc_data_cnf message
 * @param    data - message data
 * @param    len - message length
 * @return   TRUE if the message is valid, FALSE otherwise
 ****************************************************************************/
static BOOL IsValidDataCnf(U8BIT *data, U32BIT len)
{
   U32BIT offset;
   U8BIT cc_system_id_bitmask;
   U8BIT send_datatype_nbr;
   U8BIT send_datatype_found;
   U16BIT datatype_length;
   BOOL valid;

   FUNCTION_START(IsValidDataCnf);

   valid = TRUE;
   if (len >= 2)
   {
      cc_system_id_bitmask = data[0];
      if ((cc_system_id_bitmask & CC_SYSTEM_ID_BITMASK) != 0)
      {
         /* Supported CC system */
         send_datatype_nbr = data[1];
         send_datatype_found = 0;
         offset = 2;
         while ((send_datatype_found < send_datatype_nbr) &&
                (offset + 2 < len) &&
                (valid))
         {
            datatype_length = data[offset+1] << 8 | data[offset+2];
            if (offset + 2 + datatype_length < len)
            {
               offset += 3 + datatype_length;
               ++send_datatype_found;
            }
            else
            {
               /* Data item too long */
               valid = FALSE;
            }
         }
         if (send_datatype_found != send_datatype_nbr)
         {
            /* Incorrect number of sent items */
            valid = FALSE;
         }
      }
      else
      {
         /* Unsupported CC system */
         valid = FALSE;
      }
   }
   else
   {
      /* Request too short */
      valid = FALSE;
   }

   FUNCTION_FINISH(IsValidDataCnf);

   return valid;
}

/*!**************************************************************************
 * @brief    Identify type of cc_data_req APDU
 * @param    data - message data
 * @param    len - message length
 * @return   Message type
 ****************************************************************************/
static E_CC_MSG_TYPE IdentifyDataReq(U8BIT *data, U32BIT len)
{
   E_CC_MSG_TYPE msg_type;

   FUNCTION_START(IdentifyDataReq);

   msg_type = SearchReqMessage(data, len, valid_req_messages,
                               sizeof(valid_req_messages) /
                               sizeof(*valid_req_messages));

   FUNCTION_FINISH(IdentifyDataReq);

   return msg_type;
}

/*!**************************************************************************
 * @brief    Identify type of cc_sac_data_req APDU
 * @param    data - message data
 * @param    len - message length
 * @return   Message type
 ****************************************************************************/
static E_CC_MSG_TYPE IdentifySacDataReq(U8BIT *data, U32BIT len)
{
   E_CC_MSG_TYPE msg_type;

   FUNCTION_START(IdentifySacDataReq);

   msg_type = SearchReqMessage(data, len, valid_sac_req_messages,
                               sizeof(valid_sac_req_messages) / 
                               sizeof(*valid_sac_req_messages));

   FUNCTION_FINISH(IdentifySacDataReq);

   return msg_type;
}

/*!**************************************************************************
 * @brief    Identify type of cc_sac_data_cnf APDU
 * @param    data - message data
 * @param    len - message length
 * @return   Message type
 ****************************************************************************/
static E_CC_MSG_TYPE IdentifySacDataCnf(U8BIT *data, U32BIT len)
{
   E_CC_MSG_TYPE msg_type;

   FUNCTION_START(IdentifySacDataCnf);

   msg_type = SearchCnfMessage(data, len, valid_sac_cnf_messages,
                               sizeof(valid_sac_cnf_messages) / 
                               sizeof(*valid_sac_cnf_messages));

   FUNCTION_FINISH(IdentifySacDataCnf);

   return msg_type;
}
/*!**************************************************************************
 * @brief    Search message in request message information array
 * @param    data - message data
 * @param    len - message length
 * @param    messages - message array
 * @param    message_count - number of messages in array
 * @return   Message type
 ****************************************************************************/
static E_CC_MSG_TYPE SearchReqMessage(U8BIT *data, U32BIT len,
                                      S_REQ_MSG_INFO *messages,
                                      U32BIT message_count)
{
   S_REQ_MSG_INFO *message;
   U8BIT send_datatype_nbr;
   U8BIT request_datatype_nbr;
   U8BIT datatype_id;
   U16BIT datatype_length;
   U32BIT offset;
   E_CC_MSG_TYPE msg_type;
   U32BIT i, j;
   BOOL valid;

   FUNCTION_START(SearchMessage);

   msg_type = CC_MSG_TYPE_UNKNOWN;

   for (i = 0; i < message_count; ++i)
   {
      valid = FALSE;
      message = &messages[i];
      if (len > 1)
      {
         send_datatype_nbr = data[1];
         if (send_datatype_nbr == message->send_datatype_nbr)
         {
            valid = TRUE;
         }
      }

      if (valid)
      {
         offset = 2;
         for (j = 0; valid && j < send_datatype_nbr; ++j)
         {
            valid = FALSE;
            if (len > offset+2)
            {
               datatype_id = data[offset];
               if (datatype_id == message->send_datatype[j])
               {
                  datatype_length = data[offset+1] << 8 | data[offset+2];
                  offset += 3 + datatype_length;
                  valid = TRUE;
               }
            }
         }
      }
      if (valid)
      {
         valid = FALSE;
         if (len > offset)
         {
            request_datatype_nbr = data[offset];
            if (request_datatype_nbr == message->request_datatype_nbr)
            {
               valid = TRUE;
            }
         }
      }
      if (valid)
      {
         ++offset;
         for (j = 0; valid && j < request_datatype_nbr; ++j)
         {
            valid = FALSE;
            if (len > offset)
            {
               datatype_id = data[offset];
               if (datatype_id == message->request_datatype[j])
               {
                  ++offset;
                  valid = TRUE;
               }
            }
         }
      }
      if (valid)
      {
         msg_type = message->msg_type;
         break;
      }
   }

   FUNCTION_FINISH(SearchMessage);

   return msg_type;
}

/*!**************************************************************************
 * @brief    Search message in confirm message information array
 * @param    data - message data
 * @param    len - message length
 * @param    messages - message array
 * @param    message_count - number of messages in array
 * @return   Message type
 ****************************************************************************/
static E_CC_MSG_TYPE SearchCnfMessage(U8BIT *data, U32BIT len,
                                      S_CNF_MSG_INFO *messages,
                                      U32BIT message_count)
{
   S_CNF_MSG_INFO *message;
   U8BIT send_datatype_nbr;
   U8BIT datatype_id;
   U16BIT datatype_length;
   U32BIT offset;
   E_CC_MSG_TYPE msg_type;
   U32BIT i, j;
   BOOL valid;

   FUNCTION_START(SearchCnfMessage);

   msg_type = CC_MSG_TYPE_UNKNOWN;

   for (i = 0; i < message_count; ++i)
   {
      valid = FALSE;
      message = &messages[i];
      if (len > 1)
      {
         send_datatype_nbr = data[1];
         if (send_datatype_nbr == message->send_datatype_nbr)
         {
            valid = TRUE;
         }
      }

      if (valid)
      {
         offset = 2;
         for (j = 0; valid && j < send_datatype_nbr; ++j)
         {
            valid = FALSE;
            if (len > offset+2)
            {
               datatype_id = data[offset];
               if (datatype_id == message->send_datatype[j])
               {
                  datatype_length = data[offset+1] << 8 | data[offset+2];
                  offset += 3 + datatype_length;
                  valid = TRUE;
               }
            }
         }
      }
      if (valid)
      {
         msg_type = message->msg_type;
         break;
      }
   }

   FUNCTION_FINISH(SearchCnfMessage);

   return msg_type;
}

/*!**************************************************************************
 * @brief    Return length of cc_open_cnf APDU data
 * @param    data - message data
 * @param    len - message length
 * @return   APDU data length (not including APDU header) for response
 ****************************************************************************/
static U32BIT GetOpenCnfLen(U8BIT *data, U32BIT len)
{
   U32BIT response_len;

   FUNCTION_START(GetOpenCnfLen);

   USE_UNWANTED_PARAM(data);
   USE_UNWANTED_PARAM(len);

   response_len = 1;

   FUNCTION_FINISH(GetOpenCnfLen);

   return response_len;
}

/*!**************************************************************************
 * @brief    Return length of cc_data_cnf APDU data
 * @param    data - message data
 * @param    len - message length
 * @return   APDU data length (not including APDU header) for response
 ****************************************************************************/
static U32BIT GetDataCnfLen(U8BIT *data, U32BIT len)
{
   U32BIT response_len;

   FUNCTION_START(GetDataCnfLen);

   response_len = 2;
   LoopRequestedItems(data, len, AddDatatypeLength, &response_len);

   FUNCTION_FINISH(GetDataCnfLen);

   return response_len;
}

/*!**************************************************************************
 * @brief    Return length of cc_sync_cnf APDU data
 * @param    data - message data
 * @param    len - message length
 * @return   APDU data length (not including APDU header) for response
 ****************************************************************************/
static U32BIT GetSyncCnfLen(U8BIT *data, U32BIT len)
{
   U32BIT response_len;

   FUNCTION_START(GetSyncCnfLen);

   USE_UNWANTED_PARAM(data);
   USE_UNWANTED_PARAM(len);

   response_len = 1;

   FUNCTION_FINISH(GetSyncCnfLen);

   return response_len;
}

/*!**************************************************************************
 * @brief    Return length of cc_sac_data_cnf APDU data
 * @param    data - message data
 * @param    len - message length
 * @return   APDU data length (not including APDU header) for response
 ****************************************************************************/
static U32BIT GetSacDataCnfLen(U8BIT *data, U32BIT len)
{
   U32BIT response_len;

   FUNCTION_START(GetSacDataCnfLen);

   response_len = 2;
   LoopRequestedItems(data, len, AddDatatypeLength, &response_len);

   FUNCTION_FINISH(GetSacDataCnfLen);

   return response_len;
}

/*!**************************************************************************
 * @brief    Return length of cc_sac_sync_cnf APDU data
 * @param    data - message data
 * @param    len - message length
 * @return   APDU data length (not including APDU header) for response
 ****************************************************************************/
static U32BIT GetSacSyncCnfLen(U8BIT *data, U32BIT len)
{
   U32BIT response_len;

   FUNCTION_START(GetSacSyncCnfLen);

   USE_UNWANTED_PARAM(data);
   USE_UNWANTED_PARAM(len);

   response_len = 1;

   FUNCTION_FINISH(GetSacSyncCnfLen);

   return response_len;
}

/*!**************************************************************************
 * @brief    Add datatype length to accumulator
 * @param    id - datatype ID
 * @param    userdata - user data (pointer to accumulator)
 ****************************************************************************/
static void AddDatatypeLength(U8BIT id, void *userdata)
{
   U32BIT *accumulator;

   FUNCTION_START(AddDatatypeLength);

   accumulator = userdata;
   *accumulator += 3;
   
   if(id == 5)
    {
       *accumulator += BITS_TO_BYTES(DT_SIZE_HOST_ID);
    }
   else if(id == 30)
   {
        *accumulator += BITS_TO_BYTES(DT_SIZE_STATUS_FIELD);
    }
    else
    {
        CC_DEBUG_PRINT(("AddDatatypeLength error ID !"));
    }
   //*accumulator += CC_GetItemSize(id);

   FUNCTION_FINISH(AddDatatypeLength);
}

/*!**************************************************************************
 * @brief    Add requested datatype to accumulator
 * @param    id - datatype ID
 * @param    userdata - user data (pointer to accumulator)
 ****************************************************************************/
static void AddRequestedItem(U8BIT id, void *userdata)
{
   U8BIT *request_datatype_nbr;

   FUNCTION_START(AddRequestedItem);

   id = id;
   request_datatype_nbr = userdata;
   ++(*request_datatype_nbr);

   FUNCTION_FINISH(AddRequestedItem);
}

/*!**************************************************************************
 * @brief    Loop over sent items and call the callback for each
 * @param    data - message data
 * @param    len - message length
 * @param    callback - callback to call for each item
 * @param    userdata - user data to pass to callback
 ****************************************************************************/
static void LoopSentItems(U8BIT *data, U32BIT len,
                          void (*callback)(U8BIT id, U8BIT *data, U16BIT len,
                                           void *userdata),
                          void *userdata)
{
   U32BIT offset;
   U8BIT send_datatype_nbr;
   U8BIT datatype_id;
   U16BIT datatype_length;
   U8BIT i;

   FUNCTION_START(LoopSentItems);

   /* This function assumes that the message has already been checked and
    * is valid. No further checks are made.
    */
   USE_UNWANTED_PARAM(len);

   send_datatype_nbr = data[1];
   offset = 2;
   for (i = 0; i < send_datatype_nbr; ++i)
   {
      datatype_id = data[offset];
      datatype_length = data[offset+1] << 8 | data[offset+2];
      (*callback)(datatype_id, data + offset + 3, datatype_length, userdata);
      offset += 3 + datatype_length;
   }

   FUNCTION_FINISH(LoopSentItems);
}

/*!**************************************************************************
 * @brief    Loop over requested items and call the callback for each
 * @param    data - message data
 * @param    len - message length
 * @param    callback - callback to call for each item
 * @param    userdata - user data to pass to callback
 ****************************************************************************/
static void LoopRequestedItems(U8BIT *data, U32BIT len,
                               void (*callback)(U8BIT id, void *userdata),
                               void *userdata)
{
   U32BIT offset;
   U8BIT send_datatype_nbr;
   U8BIT datatype_id;
   U16BIT datatype_length;
   U8BIT request_datatype_nbr;
   U8BIT i;

   FUNCTION_START(LoopRequestedItems);

   /* This function assumes that the message has already been checked and
    * is valid. No further checks are made.
    */
   USE_UNWANTED_PARAM(len);

   send_datatype_nbr = data[1];
   offset = 2;
   for (i = 0; i < send_datatype_nbr; ++i)
   {
      datatype_id = data[offset];
      datatype_length = data[offset+1] << 8 | data[offset+2];
      offset += 3 + datatype_length;
   }
   request_datatype_nbr = data[offset];
   for (i = 0; i < request_datatype_nbr; ++i)
   {
      ++offset;
      datatype_id = data[offset];
      (*callback)(datatype_id, userdata);
   }

   FUNCTION_FINISH(LoopRequestedItems);
}

/*****************************************************************************/
/* End of File                                                               */
/*****************************************************************************/
