/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2010,2011 Sony Corporation.
 */
#ifndef NS_UTIL_MESSAGES_H
#define NS_UTIL_MESSAGES_H

NP_BEGIN_EXTERN_C

#define NS_HTTP_METHOD_GET                     1
#define NS_HTTP_METHOD_POST                    2

#define NS_NOTIFY_STAT_DORMANT                 0
#define NS_NOTIFY_STAT_READY                   1
#define NS_NOTIFY_STAT_NEMOPERSONALIZATION     2
#define NS_NOTIFY_STAT_OCTOPUSPERSONALIZATION  3
#define NS_NOTIFY_STAT_DUS                     4
#define NS_NOTIFY_STAT_DCS                     5
#define NS_NOTIFY_STAT_NODEACQ                 6
#define NS_NOTIFY_STAT_LINKACQ                 7
#define NS_NOTIFY_STAT_DEREG                   8
#define NS_NOTIFY_STAT_LICENSEACQ              9

#define NS_PROCESSED_MESSAGE_IS_PUT_TRIGGER                0
#define NS_PROCESSED_MESSAGE_IS_GET_TRIGGER                1
#define NS_PROCESSED_MESSAGE_IS_DATA                       2
#define NS_PROCESSED_MESSAGE_IS_GET_NODE                   3
#define NS_PROCESSED_MESSAGE_IS_LICENSE_CHECK              4
#define NS_PROCESSED_MESSAGE_IS_RESPONSE                   5
#define NS_PROCESSED_MESSAGE_IS_DELETE_LINK                6
#define NS_PROCESSED_MESSAGE_IS_GET_LINK_PERIOD            7
#define NS_PROCESSED_MESSAGE_IS_GET_AVAILABLE_LINKS        8
#define NS_PROCESSED_MESSAGE_IS_CHECK_PERSONALITY          9
#define NS_PROCESSED_MESSAGE_IS_GET_DUID_PARAMETERS       10
#define NS_PROCESSED_MESSAGE_IS_GET_LINKS_FOR_SERVICE_ID  11
#define NS_PROCESSED_MESSAGE_IS_GENERATE_PERSONALIZATION  12
#define NS_PROCESSED_MESSAGE_IS_PROCESS_PERSONALIZATION   13
#define NS_PROCESSED_MESSAGE_IS_DESCRIBE_ACTION_PLAY      14
#define NS_PROCESSED_MESSAGE_IS_HELLO_REQUEST             15

#define NS_RESPONSE_LTP                              0 /**< Response is a device response when device is source */
#define NS_RESPONSE_GET_LINK_PERIOD_ID               1 /**< Identifies GetLinkPeriod response values*/
#define NS_RESPONSE_GET_AVAILABLE_LINKS_ID           2 /**< Identifies GetAvailableLinks response values*/
#define NS_RESPONSE_GET_CHECK_LICENSE_ID             3 /**< Identifies CheckLicense response values*/
#define NS_RESPONSE_CHECK_PERSONALITY_ID             4 /**< Identifies CheckPersonality response values*/
#define NS_RESPONSE_GET_DUID_PARAMETERS_ID           5 /**< Identifies GetDUIDParameters response values*/
#define NS_RESPONSE_GET_LINKS_FOR_SERVICE_ID_ID      6 /**< Identifies getLinksForServiceId response values*/
#define NS_RESPONSE_GENERATE_PERSONALIZATION_ID      7 /**< Identifies GeneratePersonalization response values*/
#define NS_RESPONSE_PROCESS_PERSONALIZATION_ID       8 /**< Identifies ProcessPersonalization response values*/
#define NS_RESPONSE_DESCRIBE_ACTION_PLAY_ID          9 /**< Identifies DescribeActionPlay response values*/

#define NS_CHECKPERSONALITY_VALID_DEVKEY             0x00000000 /**< Sony specific usb message values */
#define NS_CHECKPERSONALITY_INVALID_DEVKEY           0x00100100 /**< Sony specific usb message values */

/* Action results and cert standards are used in nsec/common, so we need a non-API
 * version of these structs, just like binstr <-> np_str.  binstr_t is defined using
 * the same typedef.  They are defined here because they are nsec only so don't need
 * to be in cmn.
 */
typedef np_action_result_t ns_action_result_t;
typedef np_certstandard_t  ns_certstandard_t;

/**
 * @brief Single http header made of a name-value pair
 */
typedef struct ns_header {
    char  *name;                          /**< Extension data name  */
    char  *value;                         /**< Extension data value */
} ns_header_t;

/**
 * @brief List of HTTP headers
 */
typedef struct ns_header_list {
    np_size_t      num;                  /**< Number of ::ns_header_t objects in the list  */
    ns_header_t  **headers;              /**< Header array  */
} ns_header_list_t;

/**
 * @brief HTTP request
 */
typedef struct ns_http_request {
    u_int32_t          method;            /**< ::NS_HTTP_METHOD_GET or ::NS_HTTP_METHOD_POST  */
    char              *url;               /**< Target of the request  */
    ns_header_list_t   header_list;       /**< List of headers for an ::NS_HTTP_METHOD_POST request, can be empty */
    binstr_t          *request;           /**< Body of the request for an NS_HTTP_METHOD_POST request  */
} ns_http_request_t;

/**
 * @brief HTTP response
 */
typedef struct ns_http_response {
    binstr_t  *response;                  /**< Body of a HTTP response */
} ns_http_response_t;

/**
 * @brief Progress notification
 * @note for dorado only
 */
typedef struct ns_progress_notification {
    /**
     * @brief Progress status.
     * Can be one of
     *   <br>::NS_NOTIFY_STAT_DORMANT
     *   <br>::NS_NOTIFY_STAT_READY
     *   <br>::NS_NOTIFY_STAT_NEMOPERSONALIZATION
     *   <br>::NS_NOTIFY_STAT_OCTOPUSPERSONALIZATION
     *   <br>::NS_NOTIFY_STAT_DUS
     *   <br>::NS_NOTIFY_STAT_DCS
     *   <br>::NS_NOTIFY_STAT_NODEACQ
     *   <br>::NS_NOTIFY_STAT_LINKACQ
     *   <br>::NS_NOTIFY_STAT_DEREG
     *   <br>::NS_NOTIFY_STAT_LICENSEACQ
     */
    int stat;
    int total;                            /**< Total number of steps in the action token  */
    int step;                             /**< Current step being processed  */
} ns_progress_notification_t;


/**
 * @brief Container for results of one step of action token processing
 */
typedef struct ns_step_result {
    ns_action_result_t          *action;       /**< Action result (meant for application)  */
    ns_http_request_t           *request;      /**< HTTP request (meant for application call-backs)  */
    ns_progress_notification_t   progress;     /**< Progress notification (for dorado)  */
} ns_step_result_t;

typedef struct ns_processed_message {
    u_int32_t    type;
    union {
        struct {
            binstr_t  *action;
            binstr_t  *service_id;
            binstr_t  *service_token;
        } trigger;
        struct {
            binstr_t  *uri;
            binstr_t  *data;
            binstr_t  *ext;
        } data;
        struct {
            binstr_t  *from;
            binstr_t  *to;
        } link;
        struct {
            binstr_t  *bbsvcid;
            binstr_t  *rtoken;
        } service_id;
        struct {
            u_int16_t  mjv;
            u_int16_t  mnv;
        } version;
        binstr_t  *personality;
        binstr_t  *license;
        binstr_t  *nodeid;
        binstr_t  *response;
        binstr_t  *host_info;
        u_int32_t  cinfo;
    } msg;
} ns_processed_message_t;

typedef struct ns_get_link_period_result {
    u_int32_t  period_start;
    u_int32_t  period_end;
} ns_get_link_period_result_t;

typedef struct ns_get_available_links_result {
    u_int32_t   num;
    binstr_t  **links;
} ns_get_available_links_result_t;

typedef struct ns_get_available_links_result ns_get_links_for_service_id_result_t;

typedef struct ns_get_duid_parameters_result {
    binstr_t  *duid_params;
} ns_get_duid_parameters_result_t;

typedef struct ns_check_personality_result {
    u_int32_t   value;
} ns_check_personality_result_t;

typedef struct ns_check_license_result {
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
} ns_check_license_result_t;

typedef struct ns_describe_action_play_result {
    binstr_t   *esb;
} ns_describe_action_play_result_t;

typedef struct ns_response_result {
    u_int32_t  type;
    union {
        binstr_t                            *ltp_response;
        ns_get_link_period_result_t          get_link_period;
        ns_get_available_links_result_t      get_available_links;
        ns_get_links_for_service_id_result_t get_links_for_service_id;
        ns_check_license_result_t            check_license;
        ns_check_personality_result_t        check_personality;
        ns_get_duid_parameters_result_t      get_duid_parameters;
        binstr_t                            *personalization_request;
        ns_describe_action_play_result_t     describe_action_play;
        binstr_t                            *device_info;
    } values;
} ns_response_result_t;

/**
 * @brief Create a HTTP header list from an arrayd of name/value pairs
 *
 * @param[in]   in_num   Size of array
 * @param[in]   in_list  Name/Value pair array
 * @param[out]  out_list Header list
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 */
np_ret_t
ns_CreateHTTPHeaders( np_size_t            in_num,
                      ns_header_t        **in_list,
                      ns_header_list_t    *out_list );


/**
 * @brief Free the contents of a header list
 *
 * @param[in,out] io_list   Header list
 */
extern void
ns_FreeHTTPHeaders( ns_header_list_t *io_list );


/**
 * @brief Frees an HTTP request
 *
 * @param[in,out]   io_request   HTTP request
 */
extern void
ns_FreeHTTPRequest(ns_http_request_t *io_request);


/**
 * @brief Frees the results of an action token processing step
 *
 * @param[in,out]  io_result   The results to be freed
 */
extern void
ns_FreeStepResult(ns_step_result_t *io_result);

extern void
ns_FreeActionResult(ns_action_result_t *io_action);

/**
 * @brief Free the contents of a processed message
 *
 * @param[in,out]  io_msg  Processed message
 */
extern void
ns_ClearProcessedMessage(ns_processed_message_t *io_msg);

/**
 * @brief Free the contents of a response result
 *
 * @param[in,out]  io_result  Response result
 */
extern void
ns_ClearResponseResult(ns_response_result_t *io_result);

/**
 * @brief Free certification standard structre
 *
 * @param[in,out] io_certstd  Certification standard structure
 */
extern void
ns_FreeCertStandard(ns_certstandard_t *io_certstd);

/**
 * @brief Allocate a certification standard structure
 *
 * @param[in]  in_dcsn_len     Length of DCS name
 * @param[in]  in_dcsn_buf     DCS name
 * @param[in]  in_validity_len Length of validity xsd:duration string
 * @param[in]  in_validity_buf Validity xsd:duration string
 * @param[in]  in_mandatory    Must/Should flag
 * @param[out] out_certstd     Certification standard structure
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 */
extern np_ret_t
ns_AllocCertStandard(np_size_t           in_dcsn_len,
                     u_int8_t           *in_dcsn_buf,
                     np_size_t           in_validity_len,
                     u_int8_t           *in_validity_buf,
                     u_int8_t            in_mandatory,
                     ns_certstandard_t **out_certstd);

/**
 * @brief Generates Domain ID based on account ID & subscription ID
 *
 * @param[in]  in_accountid        Account ID
 * @param[in]  in_subscriptionid   Subscription ID
 * @param[out] out_domainid        Generated DomainID
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 */
extern np_ret_t
ns_GenerateDomainID(np_str_t   *in_accountid,
                    np_str_t   *in_subscriptionid,
                    np_str_t  **out_domainid);

NP_END_EXTERN_C

#endif
