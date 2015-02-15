/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2010,2011 Sony Corporation.
 */

/**
 * @addtogroup usb usb
 * @ingroup goby
 * @{
 *
 * @file
 * @brief  USB message handlers for Marlin protocol
 * @note   this API is incomplete and subject to change
 * @note  usbinit_ is the prefix for USB initiator, usbresp_ for the USB responder.
 */

#ifndef USB_PROTOCOL_H
#define USB_PROTOCOL_H


/**
 * @brief handle to a USB initiator context
 */
typedef void  *usbinit_handle_t;

/**
 * @brief handle to a USB responder context
 */
typedef void  *usbresp_handle_t;

/**
 * @brief See HelloResponse fields in [MUSBDF]
 */
typedef struct usbinit_musb_hello {
    u_int16_t  version_major;
    u_int16_t  version_minor;
    u_int16_t  supported_options;
    binstr_t  *client_info;
    binstr_t  *vendor_ext;
} usbinit_musb_hello_t;

/**
 * @brief creates a context for USB Initiator
 *
 * @param[in]     in_client       The description of a client (including call-backs to USB stack)
 * @param[in]     in_general_info General info to send to protocol peer (from host to device or conversely)
 * @param[out]    out_handle      Handle to the created context
 * @param[out]    out_clientinfo  General info returned from client in HelloResponse
 *
 * @note   Return codes are incomplete
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 */
np_ret_t
usbinit_Init( goby_client_t          *in_client,
              binstr_t               *in_general_info,
              usbinit_handle_t       *out_handle,
              usbinit_musb_hello_t   *out_clientinfo);


/**
 * @brief  Finalizes a USB Initiator and frees its context
 *
 * @param[in,out]  io_handle  the context of a USB initiator
 */
void
usbinit_Fin( usbinit_handle_t   io_handle );


/**
 * @brief Starts managing rights on an attached device, by sending an action token
 *
 * @param[in,out]   io_handle         handle to a USB initiator context
 * @param[in]       in_action_token   an action token for managing rights
 * @param[in]       in_service_id           Global unique identifier for the service information
 * @param[in]       in_service_information  service-specific information (aka relatedToken in [MUSB])
 * @param[in]       in_puttrigger           PutTrigger message sent if true, GetTrigger otherwise
 *
 * @note the TriggerDataResponse is stored in the context for later processing by usbinit_StepManageRights.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_TL_CALLBACK
 * @return ::ERR_NP_MUSB_PROTOCOL
 */
np_ret_t
usbinit_InitManageRights( usbinit_handle_t   io_handle,
                          binstr_t          *in_action_token,
                          binstr_t          *in_service_id,
                          binstr_t          *in_service_information,
                          np_bool_t          in_puttrigger);

/**
 * @brief Finalizes USB initiator's context for managing rights on attached device
 *
 * @param[in,out]  io_handle    a USB initiator's context
 *
 * @return ::ERR_NP_OK
 */
np_ret_t
usbinit_FinManageRights( usbinit_handle_t  io_handle );


/**
 * @brief Executes one step in [MUSB] protocol for managing rights on attached device
 *
 * @param[in,out]    io_handle           handle to a USB initiator context
 * @param[in]        in_http_response    NULL, or an HTTP response to be sent to attached device
 * @param[out]       out_result          the results found in the response from the attached device
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_NOTSUPPORTED
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_TL_CALLBACK
 * @return ::ERR_NP_MUSB_PROTOCOL
 * @return ::usbdf_CreateReplyData
 * @return ::usbdf_ParseMessage
 */
np_ret_t
usbinit_StepManageRights( usbinit_handle_t        io_handle,
                          ns_http_response_t     *in_http_response,
                          ns_step_result_t      **out_result );

/**
 * @brief Function for steping through LTP.
 *
 * @param[in]    in_handle  Handle to a USB initiator context
 * @param[in]    in_step    Step result from the host to send to the device
 * @param[out]   out_step   Device response
 */
np_ret_t
usbinit_StepTransfer( usbinit_handle_t     in_handle,
                      ns_step_result_t    *in_step,
                      ns_http_response_t  *out_step );

/**
 * @brief Get all the available link
 *
 * The links are defined by the sequence of node pairs.
 *
 * @param[in]    in_handle  Handle to a USB initiator context
 * @param[out]   out_num    The number of nodes
 * @param[out]   out_links  The list of nodes
 *
 */
np_ret_t
usbinit_GetAvailableLinks( usbinit_handle_t    in_handle,
                           u_int32_t          *out_num,
                           binstr_t         ***out_links);


/**
 * @brief Check presence and validity of NEMO and OCTOPUS personality for a device
 *
 * @param[in]    in_handle  Handle to a USB initiator context
 * @param[in]    out_value  0x00000000 indicates valid DeviceKey
 *                          0x00100000 indicates invalid DeviceKey
 *                          0x00100100 indicates invalid DeviceKey and indicates GetLastError
 *                                     can be used for more detail
 *
 */
np_ret_t
usbinit_CheckPersonality( usbinit_handle_t  in_handle,
                          u_int32_t        *out_value);

/**
 * @brief CheckLicense. Get license validity periods and nodes
 *
 * @param[in]    in_handle  Handle to a USB initiator context
 * @param[in]    in_license The license to be queried
 * @param[out]   out_info   The license validity data
 *
 */
np_ret_t
usbinit_CheckLicense( usbinit_handle_t             in_handle,
                      binstr_t                    *in_license,
                      ns_check_license_result_t  **out_licenseinfo);


/**
 * @brief Sends a DeleteLink message to the responder
 *
 * @param[in]    in_handle
 * @param[in]    in_from
 * @param[in]    in_to
 *
 */
np_ret_t
usbinit_DeleteLink( usbinit_handle_t  in_handle,
                    binstr_t         *in_from,
                    binstr_t         *in_to );

/**
 * @brief Get the link validity period
 *
 * @param[in]  in_handle        The USB initiator's context
 * @param[in]  in_from          The link identification by its from node, use NULL for the personality
 * @param[in]  in_to            The link identified by its to node
 * @param[out] out_period_start The link's period start, undefined if the from, to don't identify a valid link
 * @param[out] out_period_end   The link's period ending, undefined if the from, to don't identify a valid link
 * @return                      ERR_NP_OK if all is well
 */
np_ret_t
usbinit_GetLinkPeriod( usbinit_handle_t  in_handle,
                       binstr_t         *in_from,
                       binstr_t         *in_to,
                       u_int32_t        *out_period_start,
                       u_int32_t        *out_period_end);

/**
 * @brief Sends a GetDUIDParameters request to the responder.
 *
 * @param[in]  in_handle        The USB initiator's context
 * @param[in]  in_cinfo         Context Information
 * @param[in]  out_duid_params  Hex string containing the DUID parameters.
 * @return                      ERR_NP_OK if all is well
 */
np_ret_t
usbinit_GetDUIDParameters( usbinit_handle_t   in_handle,
                           u_int32_t          in_cinfo,
                           binstr_t         **out_duid_params );

/**
 * @brief Get the links associated with a service identification and token
 *
 * @param[in]  in_handle        The USB initiator's context
 * @param[in]  in_bbsvcid       The broadband service identification (from an action token)
 * @param[in]  in_rtoken        The application or service specific token associated with the link to be returned
 * @param[out] out_num          The number of nodes
 * @param[out] out_links        The list of nodes; each pair defines a link as a from-node, to-node
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_TL_CALLBACK
 * @return ::ERR_NP_MUSB_PROTOCOL
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 * @return ::usbdf_CreateGetLinksForServiceIdRequestPacket
 * @return ::usbdf_ParseMessage
 */
np_ret_t
usbinit_GetLinksForServiceId( usbinit_handle_t    in_handle,
                              binstr_t           *in_bbsvcid,
                              binstr_t           *in_rtoken,
                              u_int32_t          *out_num,
                              binstr_t         ***out_links);

/**
 * @brief Generates the Personalization request
 *
 * @param[in]    in_handle    The USB initiator's context
 * @param[out]   out_request  Personalization request HTTP body as will be used by the application
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::usbdf_CreateGeneratePersonalizationRequestPacket
 * @return ::usbdf_ParseMessage
 */
np_ret_t
usbinit_GeneratePersonalizationRequest( usbinit_handle_t   in_handle,
                                        binstr_t         **out_request);

/**
 * @brief Processes the Personalization response from the server
 *
 * @param[in]   in_handle       The USB initiator's context
 * @param[in]   in_personality  Personalization response HTTP body
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::usbdf_CreateProcessPersonalizationResponseRequest
 * @return ::usbdf_ParseMessage
 * @return ::usbdf_CreateResultData
 */
np_ret_t
usbinit_ProcessPersonalizationResponseRequest( usbinit_handle_t   in_handle,
                                               binstr_t          *in_personality);


/**
 * @brief Executes Action Routines
 *
 * @param[in]  in_handle        The USB initiator's context
 * @param[in]  in_license       The license to be executed
 * @param[out] out_esb          The string representation of ESB structure
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_TL_CALLBACK
 * @return ::ERR_NP_MUSB_PROTOCOL
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 * @return ::usbdf_CreateExecActionRoutineRequest
 * @return ::usbdf_ParseMessage
 */
np_ret_t
usbinit_DescribeActionPlay( usbinit_handle_t   in_handle,
                            binstr_t          *in_license,
                            binstr_t         **out_esb );


/**
 * @brief creates a context for USB Responder
 *
 * @param[in]     in_client         The description of a client (including call-backs to USB stack)
 * @param[in]     in_responder_info Responder info to send to protocol peer (from host to device or conversely)
 * @param[out]    out_handle        Handle to the created context
 *
 * @note   Return codes are incomplete
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 */
np_ret_t
usbresp_Init( goby_client_t          *in_client,
              binstr_t               *in_responder_info,
              usbresp_handle_t       *out_handle);


/**
 * @brief  Finalizes a USB Responder and frees its context
 *
 * @param[in,out]  io_handle  the context of a USB responder
 */
void
usbresp_Fin( usbresp_handle_t   io_handle );

/**
 * @brief Processes an incoming message and extract relevant data for goby
 *
 * @param[in]    in_handle     The USB responder's context
 * @param[in]    in_message    An incoming message
 * @param[in]    in_status     If an error was encountered it is needed for GetLastError requests
 * @param[in]    in_finfo      If a soap fault was encountered perviously it is needed for GetLastError requests
 * @param[out]   out_message   A message to send back to goby for processing
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::usbdf_CreateResultData
 */
np_ret_t
usbresp_ProcessMessage( usbresp_handle_t        in_handle,
                        binstr_t               *in_message,
                        np_ret_t                in_status,
                        np_faultinfo_t         *in_finfo,
                        ns_processed_message_t *out_message );

/**
 * @brief Prepare a message to send, incorporating relevant data from an action and/or request
 *
 * @param[in]  in_handle             The USB responder's context
 * @param[in]  in_status             Error code of Nautilus before calling this function.
 *                                   If it is not OK a suitable error result message will be returned.
 * @param[in]  in_action             Result of a step, possibly containing licence etc
 * @param[in]  in_request            HTTP request to add to post or get get data message
 * @parma[in]  in_result             The results from repsonse that return some answer
 * @param[out] out_response          A string representation of the outgoing message
 *
 */
np_ret_t
usbresp_PrepareMessage( usbresp_handle_t          in_handle,
                        np_ret_t                  in_status,
                        ns_action_result_t       *in_action,
                        ns_http_request_t        *in_request,
                        ns_response_result_t     *in_result,
                        binstr_t                **out_response );

/**
 * @brief Updates the Enable write data flag in responder context
 *
 * @param[in]  in_handle     The USB responder's context
 * @param[in]  in_enable     Enable write data flag
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 */
np_ret_t
usbresp_EnableWriteData( usbresp_handle_t  in_handle,
                         np_bool_t         in_enable );


#endif /* USB_PROTOCOL_H */
/** @} */
