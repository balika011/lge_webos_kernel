/****************************************************************************/
/* (c) Copyright Ocean Blue Software Ltd. - ALL RIGHTS RESERVED             */
/* This document is the CONFIDENTIAL property of Ocean Blue Software Ltd.   */
/****************************************************************************/
/*                                                                          */
/*   FILE:    ccmsg.h                                                       */
/*   DATE:    18/01/09                                                      */
/*   PURPOSE: CI+ Content Control Resource: Message processing              */
/*                                                                          */
/****************************************************************************/
#ifndef __CCMSG_H_
#define __CCMSG_H_

/*--------------------------------------------------------------------------*/
/* Symbol Definitions                                                       */
/*--------------------------------------------------------------------------*/

typedef enum cc_msg_type
{
   CC_MSG_TYPE_HOST_CAPS,
   CC_MSG_TYPE_AUTH_NONCE,
   CC_MSG_TYPE_AUTH_SIGNATURE,
   CC_MSG_TYPE_AUTH_KEY,
   CC_MSG_TYPE_SAC_DATA,
   CC_MSG_TYPE_SAC_DATA_LEGACY,
   CC_MSG_TYPE_SAC_SYNC,
   CC_MSG_TYPE_KEY_DATA,
   CC_MSG_TYPE_KEY_SYNC,
   CC_MSG_TYPE_URI_VERSIONS,
   CC_MSG_TYPE_URI_MESSAGE,
   CC_MSG_TYPE_SRM_DATA,
   CC_MSG_TYPE_RECORD_START,
   CC_MSG_TYPE_RECORD_START_STATUS,
   CC_MSG_TYPE_CICAM_LICENSE,
   CC_MSG_TYPE_LICENSE_RCVD_STATUS,
   CC_MSG_TYPE_LICENSE_CHECK,
   CC_MSG_TYPE_LICENSE_STATUS,
   CC_MSG_TYPE_MODE_CHANGE,
   CC_MSG_TYPE_MODE_CHANGE_STATUS,
   CC_MSG_TYPE_RECORD_STOP,
   CC_MSG_TYPE_RECORD_STOP_STATUS,
   CC_MSG_TYPE_PLAYBACK_LICENSE,
   CC_MSG_TYPE_PLAYBACK_LICENSE_REPLY,
   CC_MSG_TYPE_TASK,
   CC_MSG_TYPE_UNKNOWN
} E_CC_MSG_TYPE;

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
 * @brief    Return message type
 * @param    tag - APDU tag
 * @param    data - message data
 * @param    len - message length
 * @return   Message type
 ****************************************************************************/
E_CC_MSG_TYPE CC_GetMessageType(U8BIT *data, U32BIT len);

/*!**************************************************************************
 * @brief    Return length of response for message
 * @param    tag - APDU tag
 * @param    data - message data
 * @param    len - message length
 * @return   Length of response (message data only, not including APDU header)
 ****************************************************************************/
U32BIT CC_GetResponseLength(U32BIT tag, U8BIT *data, U32BIT len);

/*!**************************************************************************
 * @brief    Return number of items to send in the response
 * @param    data - message data
 * @param    len - message length
 * @return   Number of items in the response
 * @note     This function assumes that the message is a valid data request,
 *           either cc_data_req or cc_sac_data_req.
 ****************************************************************************/
U8BIT CC_GetSendItemCount(U8BIT *data, U32BIT len);

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
                      void *userdata);

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
                           void *userdata);

#endif   /* !__CCMSG_H */
