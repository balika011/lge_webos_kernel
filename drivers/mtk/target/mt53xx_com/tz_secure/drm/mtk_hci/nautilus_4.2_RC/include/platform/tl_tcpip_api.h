/** @addtogroup tl
 * @ingroup platform
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010 Sony Corporation.
 */

#ifndef TL_TCPIP_API_H
#define TL_TCPIP_API_H

NP_BEGIN_EXTERN_C

#if defined( NPOPT_ENABLE_TCPIP_TL )
/**
 * @brief Initializes a TCP transport layer.
 *
 * Initialisation of a TCP transport layer. This function MUST be
 * called before messages can be sent through the transport layer
 * via ::tl_tcpip_SendRecv
 *
 * @param[in] Target connection info
 *
 * @return ::ERR_OK                 <br>Indicating Success
 * @return ::ERR_TL_OK              <br>Indicating Success
 * @return ::ERR_TL_ILLEGALARGS     <br>The input arguments are incorrect. Arguments cannot be NULL
 * @return ::ERR_TL_INTERNALERROR   <br>An unrecoverable error has occurred
 */
tl_ret_t
tl_tcpip_Init( np_target_info_t *in_conn );

/**
 * @brief Finalizes the TCP transport layer
 *
 * Finalisation of the TCP transport layer. Once this function is called
 * there can be no further messaging via this transport layer.
 *
 * @param[in] Target connection info
 *
 * @return ::ERR_TL_OK              <br>Indicating Success
 */
tl_ret_t
tl_tcpip_Fin( np_target_info_t *in_conn );

/**
 * @brief Sends and Receives a message via TCP/IP
 *
 * Sends a @ref smi "secure message" from the @ref nsec layer to @ref sec via TCP/IP
 *
 * @param[in]   in_conn  Connection info
 * @param[in]   in_req   @ref smi message
 * @param[out]  out_resp @ref smi response
 *
 * @return ::ERR_OK                 <br>Indicating Success
 * @return ::ERR_TL_OUTOFMEMORY     <br>A resource failure has occurred
 * @return ::ERR_TL_DROPPED         <br>A context size error has occurred during sending/reading the message
 */
tl_ret_t
tl_tcpip_SendRecv( void *in_conn, np_str_t *in_req, np_str_t **out_resp );

/**
 * @brief Frees the message
 *
 * SMI messages are allocated by the transport layer. It is the
 * transport layers responsibility to provide a method of freeing
 * the message.
 *
 * @param[in,out] io_msg The message to free
 *
 * @return ::ERR_TL_OK              <br>Indicating Success
 */
tl_ret_t
tl_tcpip_MsgFree( np_str_t *io_msg );
#endif

NP_END_EXTERN_C

#endif
/** @} */
