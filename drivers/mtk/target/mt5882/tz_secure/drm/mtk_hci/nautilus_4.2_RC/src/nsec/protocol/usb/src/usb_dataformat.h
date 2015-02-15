/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2010,2011 Sony Corporation.
 */

/**
 * @addtogroup dataformat dataformat
 * @ingroup usb
 * @{
 *
 * @file
 * @brief Data format parser and creator for USB specification
 */

#ifndef USB_DATA_FORMAT_H
#define USB_DATA_FORMAT_H

NP_BEGIN_EXTERN_C

#define USB_MESSAGE_HEADER_SIZE    16


/* ====== Messages ===== */
#define USB_MESSAGE_NONE                              (u_int32_t)0x00001000
#define USB_MESSAGE_PUT_TRIGGER                       (u_int32_t)0x00000400
#define USB_MESSAGE_GET_TRIGGER                       (u_int32_t)0x00000401
#define USB_MESSAGE_GET_DATA                          (u_int32_t)0x00000402
#define USB_MESSAGE_POST_DATA                         (u_int32_t)0x00000403
#define USB_MESSAGE_REPLY_DATA                        (u_int32_t)0x00000404
#define USB_MESSAGE_WRITE_DATA                        (u_int32_t)0x00000405
#define USB_MESSAGE_RESULT_DATA                       (u_int32_t)0x00000406
#define USB_MESSAGE_CHECK_LICENSE_REQUEST             (u_int32_t)0x00000500
#define USB_MESSAGE_CHECK_LICENSE_RESPONSE            (u_int32_t)0x00000501
#define USB_MESSAGE_GET_AVAILABLE_LINKS_REQUEST       (u_int32_t)0x00000502
#define USB_MESSAGE_GET_AVAILABLE_LINKS_RESPONSE      (u_int32_t)0x00000503
#define USB_MESSAGE_GET_LINK_PERIOD_REQUEST           (u_int32_t)0x00000504
#define USB_MESSAGE_GET_LINK_PERIOD_RESPONSE          (u_int32_t)0x00000505
#define USB_MESSAGE_GET_LINKS_FOR_SERVICE_ID_REQUEST  (u_int32_t)0x00000506
#define USB_MESSAGE_GET_LINKS_FOR_SERVICE_ID_RESPONSE (u_int32_t)0x00000507
#define USB_MESSAGE_HELLO_REQUEST                     (u_int32_t)0x00000601
#define USB_MESSAGE_HELLO_RESPONSE                    (u_int32_t)0x00000602
#define USB_MESSAGE_GET_LAST_ERROR_REQUEST            (u_int32_t)0x00000603
#define USB_MESSAGE_GET_LAST_ERROR_RESPONSE           (u_int32_t)0x00000604

/* ====== USB version number for all message types ===== */
#define USB_VERSION                                   0x0001

/* ====== Sony specific messages ===== */
#define USB_MESSAGE_GENERATE_PERSONALIZATION_REQUEST          (u_int32_t)0x000010a0
#define USB_MESSAGE_PERSONALIZATION_REQUEST_RESPONSE          (u_int32_t)0x000010a1
#define USB_MESSAGE_PROCESS_PERSONALIZATION_RESPONSE_REQUEST  (u_int32_t)0x000010a2
#define USB_MESSAGE_DELETE_LINK_REQUEST                       (u_int32_t)0x000010a4
#define USB_MESSAGE_CHECK_PERSONALITY_REQUEST                 (u_int32_t)0x000010a6
#define USB_MESSAGE_CHECK_PERSONALITY_RESPONSE                (u_int32_t)0x000010a7
#define USB_MESSAGE_GET_DUID_PARAMETERS_REQUEST               (u_int32_t)0x000010ae
#define USB_MESSAGE_GET_DUID_PARAMETERS_RESPONSE              (u_int32_t)0x000010af
#define USB_MESSAGE_DESCRIBE_ACTION_PLAY_REQUEST              (u_int32_t)0x000010b0
#define USB_MESSAGE_DESCRIBE_ACTION_PLAY_RESPONSE             (u_int32_t)0x000010b1

/* ====== Triggers ===== */
#define USB_TRIGGER_TYPE_ACTION                       (u_int16_t)0x0000
#define USB_TRIGGER_TYPE_TT                           (u_int16_t)0x0001
#define USB_TRIGGER_TYPE_LTP                          (u_int16_t)0x0002

/* ====== Results ===== */
#define USB_RESULT_DATA_SUCCESS                       (u_int8_t)0x00
#define USB_RESULT_DATA_ERR_INIT                      (u_int8_t)0x10
#define USB_RESULT_DATA_ERR_TRIG                      (u_int8_t)0x11
#define USB_RESULT_DATA_ERR_CMD                       (u_int8_t)0x12
#define USB_RESULT_DATA_ERR_HELLO                     (u_int8_t)0x13
#define USB_RESULT_DATA_ERR_UNEXPECTED                (u_int8_t)0x80

#define USB_LINFO_ALLOWEDUSE_PLAY                     (u_int16_t)0x0000

/* ====== Hello specific ===== */
#define USB_PROTOCOL_VERSION_MAJOR                    (u_int16_t)0x0001
#define USB_PROTOCOL_VERSION_MINOR                    (u_int16_t)0x0000

#define USB_PROTOCOL_OPT_SUPPORTED_CHK_LICENSE        (u_int16_t)0x8000
#define USB_PROTOCOL_OPT_SUPPORTED_GET_NODES          (u_int16_t)0x4000
#define USB_PROTOCOL_OPT_SUPPORTED_GET_TRIGGER        (u_int16_t)0x2000

#define USB_POST_DATA_TYPE_MSG                        0


/* ====== Errors ===== */
#define USB_ERROR_TYPE_NO_DATA                        (u_int32_t)0x00000100
#define USB_ERROR_TYPE_FAULT                          (u_int32_t)0X00000101

/* ====== Sony specific errors ===== */
#if defined(NPOPT_ENABLE_SONY_EXTENSION)
#define USB_ERROR_TYPE_NPRET                          (u_int32_t)0x00010100
#endif


/* ====== Write data data-types ===== */
#define USB_WRITE_DATA_TYPE_MSG                       0x0000
#define USB_WRITE_DATA_TYPE_LICENSE                   0x0001
#define USB_WRITE_DATA_TYPE_NODE                      0x0002
#define USB_WRITE_DATA_TYPE_LINK                      0x0003
#define USB_WRITE_DATA_TYPE_CTKN                      0x0004
#define USB_WRITE_DATA_TYPE_CRL                       0x0005

/* ====== Sony specific write data types ===== */
#define USB_PERSONALIZATION_PROTOCOL_MAJOR_VERSION        0x0001
#define USB_PERSONALIZATION_PROTOCOL_MINOR_VERSION        0x0000
#define USB_GENERATE_PERSONALIZATION_REQUEST_RESERVED     12

typedef struct usbdf_put_trigger {
    u_int16_t   type;
    binstr_t   *data;
    binstr_t   *svcid;
    binstr_t   *token;
} usbdf_put_trigger_t;

typedef struct usbdf_get_trigger {
    u_int16_t   type;
} usbdf_get_trigger_t;

typedef struct usbdf_get_data {
    binstr_t  *uri;
} usbdf_get_data_t;

typedef struct usbdf_post_data {
    u_int16_t  type;
    binstr_t  *uri;
    binstr_t  *data;
    binstr_t  *ext;
} usbdf_post_data_t;

typedef struct usbdf_reply_data {
    binstr_t  *data;
} usbdf_reply_data_t;

typedef struct usbdf_write_data {
    u_int16_t  type;
    binstr_t  *data;
} usbdf_write_data_t;

typedef struct usbdf_check_license_request {
    binstr_t    *license;
} usbdf_check_license_request_t;

typedef struct usbdf_check_license_response {
    /**
     * AllowedUseType
     *   <br>::USB_LINFO_ALLOWEDUSE_PLAY
     */
    u_int16_t   utype;
    int32_t     vstart;    /**< Validity start : -1 for empty value  */
    int32_t     vend;      /**< Validity end   : -1 for empty value */
    int32_t     rtime;     /**< Remaining time : -1 for empty value  */
    int32_t     frtime;    /**< First remaining time : -1 for empty value */
    u_int32_t   nnids;     /**< Number of Node IDs */
    binstr_t  **nids;      /**< Node IDs   */
} usbdf_check_license_response_t;

typedef struct usbdf_link_pairs {
    u_int32_t       nnids;
    binstr_t      **nids;
} usbdf_link_pairs_t;

typedef struct usbdf_get_available_links_response {
    u_int32_t              nlinks;
    usbdf_link_pairs_t   **links;
} usbdf_get_available_links_response_t;

typedef struct usbdf_get_available_links_response usbdf_links_response_t;

typedef struct usbdf_check_personality_response {
    u_int32_t   value;
} usbdf_check_personality_response_t;

typedef struct usbdf_hello_request {
    binstr_t   *ginfo;
} usbdf_hello_request_t;

typedef struct usbdf_hello_response {
    u_int16_t   vmaj;
    u_int16_t   vmin;
    u_int16_t   sopt;
    binstr_t   *ginfo;
    binstr_t   *vext;
} usbdf_hello_response_t;

typedef struct usbdf_result_data {
    u_int8_t    data[4];
} usbdf_result_data_t;

typedef struct usbdf_get_link_period_request {
    binstr_t   *from;
    binstr_t   *to;
} usbdf_get_link_period_request_t;

typedef struct usbdf_get_links_for_service_id_request {
    binstr_t   *bbsvcid;
    binstr_t   *rtoken;
} usbdf_get_links_for_service_id_request_t;

typedef struct usbdf_get_last_error_response {
    u_int32_t        type;
    np_ret_t         status;
    np_faultinfo_t  *fault;
} usbdf_get_last_error_response_t;

typedef struct usbdf_message {
    u_int32_t   type;
    void       *contents;
} usbdf_message_t;

/* ====== Sony specific structures ===== */
typedef struct usbdf_delete_link_request {
    binstr_t   *from;
    binstr_t   *to;
} usbdf_delete_link_request_t;

typedef struct usbdf_get_duid_parameters_request {
    u_int32_t   cinfo;
} usbdf_get_duid_parameters_request_t;

typedef struct usbdf_get_duid_parameters_response {
    binstr_t   *duid_params;
} usbdf_get_duid_parameters_response_t;

typedef struct usbdf_generate_personalization_request {
    u_int16_t   mjv;
    u_int16_t   mnv;
} usbdf_generate_personalization_request_t;

typedef struct usbdf_personalization_request_response {
    binstr_t   *data;
} usbdf_personalization_request_response_t;

typedef struct usbdf_process_personalization_response_request {
    binstr_t   *personality;
} usbdf_process_personalization_response_request_t;

typedef struct usbdf_describe_action_play_request {
    binstr_t    *license;
} usbdf_describe_action_play_request_t;

typedef struct usbdf_describe_action_play_response {
    binstr_t    *esb;
} usbdf_describe_action_play_response_t;


np_ret_t
usbdf_CreatePutTrigger( u_int32_t   in_type,
                        binstr_t   *in_data,
                        binstr_t   *in_svcid,
                        binstr_t   *in_token,
                        binstr_t  **out_message );

np_ret_t
usbdf_CreateGetTrigger( u_int32_t    in_type,
                        binstr_t   **out_message );

np_ret_t
usbdf_CreateGetData( char       *in_uri,
                     binstr_t  **out_message );

np_ret_t
usbdf_CreatePostData( u_int32_t            in_type,
                      char                *in_uri,
                      binstr_t            *in_data,
                      ns_header_list_t    *in_list,
                      binstr_t           **out_message );

np_ret_t
usbdf_CreateReplyData( binstr_t   *in_data,
                       binstr_t  **out_message );

np_ret_t
usbdf_CreateWriteData( u_int32_t   in_type,
                       binstr_t   *in_data,
                       binstr_t  **out_message );

np_ret_t
usbdf_CreateCheckLicenseRequest( binstr_t   *in_license,
                                 binstr_t  **out_message );

np_ret_t
usbdf_CreateCheckLicenseResponse( ns_check_license_result_t *in_license_result,
                                  binstr_t      **out_message );

np_ret_t
usbdf_CreateGetAvailableLinksResponse( ns_get_available_links_result_t  *results,
                                       binstr_t                        **out_message );

np_ret_t
usbdf_CreateHelloRequest( binstr_t   *in_ginfo,
                          binstr_t  **out_message );



np_ret_t
usbdf_CreateHelloResponse( u_int16_t   in_vmaj,
                           u_int16_t   in_vmin,
                           u_int16_t   in_sopt,
                           binstr_t   *in_ginfo,
                           binstr_t   *in_vext,
                           binstr_t  **out_message );

np_ret_t
usbdf_CreateGetLinkPeriodRequest( binstr_t   *in_from,
                                  binstr_t   *in_to,
                                  binstr_t  **out_message );

np_ret_t
usbdf_CreateGetLinkPeriodResponse( ns_get_link_period_result_t   *in_response,
                                   binstr_t                     **out_packet );

np_ret_t
usbdf_CreateGetLinksForServiceIdRequestPacket( binstr_t  *in_bbsvcid,
                                               binstr_t  *in_rtoken,
                                               binstr_t **out_packet );
np_ret_t
usbdf_CreateGetLinksForServiceIdResponsePacket( ns_get_links_for_service_id_result_t  *in_response,
                                                binstr_t                             **out_packet );

np_ret_t
usbdf_CreateGetLastErrorRequest( binstr_t  **out_message );

np_ret_t
usbdf_CreateGetLastErrorResponse( u_int32_t         in_type,
                                  np_ret_t          in_status,
                                  np_faultinfo_t   *in_data,
                                  binstr_t        **out_message );

np_ret_t
usbdf_CreateResultData( u_int8_t     in_error,
                        np_bool_t    in_fault,
                        binstr_t   **out_message );

np_ret_t
usbdf_CreateGetAvailableLinksRequest( binstr_t  **out_message );

np_ret_t
usbdf_CreateCheckPersonalityRequest( binstr_t  **out_message );

np_ret_t
usbdf_CreateCheckPersonalityResponse( ns_check_personality_result_t   *in_response,
                                      binstr_t                       **out_message );

np_ret_t
usbdf_CreateDeleteLinkRequest( binstr_t   *in_from,
                               binstr_t   *in_to,
                               binstr_t  **out_message );
np_ret_t
usbdf_ParseMessage( binstr_t         *in_message,
                    usbdf_message_t  *out_message );

void
usbdf_FreeMessage( usbdf_message_t  *io_message );

np_ret_t
usbdf_CreateGetDUIDParametersRequest( u_int32_t    in_cinfo,
                                      binstr_t   **out_message );

np_ret_t
usbdf_CreateGetDUIDParametersResponse( ns_get_duid_parameters_result_t  *in_results,
                                       binstr_t                        **out_message );

np_ret_t
usbdf_CreateGeneratePersonalizationRequest( binstr_t  **out_message );

np_ret_t
usbdf_CreatePersonalizationRequestResponse( binstr_t    *in_results,
                                            binstr_t   **out_message );

np_ret_t
usbdf_CreateProcessPersonalizationResponseRequest( binstr_t   *in_personality,
                                                   binstr_t  **out_message );

np_ret_t
usbdf_CreateDescribeActionPlayRequest( binstr_t   *license,
                                       binstr_t  **out_message );

np_ret_t
usbdf_CreateDescribeActionPlayResponse( ns_describe_action_play_result_t  *in_results,
                                        binstr_t                         **out_message );

NP_END_EXTERN_C

#endif /* USB_DATA_FORMAT_H */
/** @} */
