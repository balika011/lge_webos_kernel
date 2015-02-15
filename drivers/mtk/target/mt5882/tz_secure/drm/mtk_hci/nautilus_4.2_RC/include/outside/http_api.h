/** @addtogroup http http
 * @ingroup outside
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011 Sony Corporation.
 */

#ifndef HTTP_API_H
#define HTTP_API_H

#ifdef __cplusplus
extern "C" {
#endif

    /*===========================================================================*/
    /** @file
     * @brief HTTP Protocol Stack Library
     *
     * dependency
     * @author Hirofumi SHIMADA <Hirofumi.Shimada@jp.sony.com>
     * @date   08 Mar, 2005 last modified by Hirofumi
     */
    /*===========================================================================*/

    /*---------------------------------------------------------------------------*
     * macro definitions
     *---------------------------------------------------------------------------*/

    /**
     * @brief Error code of HTTP protocol stack library
     */
#define ERR_HTTP_BASE               0x00001300
#define ERR_HTTP_OK                 0
#define ERR_HTTP_NOTIMPL            (ERR_HTTP_BASE|1)
#define ERR_HTTP_OUTOFMEMORY        (ERR_HTTP_BASE|2)
#define ERR_HTTP_INIT               (ERR_HTTP_BASE|3)
#define ERR_HTTP_FIN                (ERR_HTTP_BASE|4)
#define ERR_HTTP_TIMEOUT            (ERR_HTTP_BASE|5)
#define ERR_HTTP_SOCKET             (ERR_HTTP_BASE|6)
#define ERR_HTTP_INVALIDARG         (ERR_HTTP_BASE|7)
#define ERR_HTTP_FATAL              (ERR_HTTP_BASE|8)
#define ERR_HTTP_NO_CONTENT_LENGTH  (ERR_HTTP_BASE|9)
#define ERR_HTTP_IN_USE             (ERR_HTTP_BASE|10)
#define ERR_HTTP_INTERNALSERVERERROR (ERR_HTTP_BASE|11)
#define ERR_HTTP_NOTFOUND            (ERR_HTTP_BASE|12)

    /**
     * @brief Enumeration Type to Enable/Disable a function
     */
#define HTTP_ENABLE             1
#define HTTP_DISABLE            0

    /*---------------------------------------------------------------------------*
     * type definitions
     *---------------------------------------------------------------------------*/

    /**
     * @brief HTTP connection handle
     */
    typedef void *http_connection_t;

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Initializes HTTP protocol stack
     *
     * Initilization of the HTTP Protocol stack
     *
     * @return ::ERR_HTTP_OK            <br>Indicating success
     * @return ::ERR_HTTP_INIT          <br>Http initialization error
     */
    /*---------------------------------------------------------------------------*/
    extern u_int32_t
    http_Init(void);


    /*---------------------------------------------------------------------------*/
    /**
     * @brief Finalizes HTTP protocol ctack
     *
     * Finalization of HTTP Protocol stack
     *
     * @return ::ERR_HTTP_OK            <br>Indicating success
     */
    /*---------------------------------------------------------------------------*/
    extern u_int32_t
    http_Fin(void);


    /*---------------------------------------------------------------------------*/
    /**
     * @brief Creates Connection
     *
     * Creates connection with the peer(server) whose URI is specified
     *
     * @param[out] out_con connection handle
     * @param[in] in_uri  peer URI (must be null-terminated)
     *                            including port number if necessary
     * @param[in] in_keepAlive  deprecated parameter; value is ignored
     * @return ::ERR_HTTP_OK                <br>Indicating success
     * @return ::ERR_HTTP_OUTOFMEMORY       <br>A resource failure has occurred
     * @return ::ERR_HTTP_INVALIDARG        <br>The input arguments are incorrect. Arguments cannot be NULL
     * @return ::ERR_HTTP_NOTIMPL           <br>This funtionality is not implemented
     * @return ::ERR_HTTP_SOCKET            <br>A socket error occurred
     * @return ::ERR_HTTP_FATAL             <br>http connection or context error has occurred
     */
    /*---------------------------------------------------------------------------*/
    extern u_int32_t
    http_CreateConnection(
        http_connection_t *out_con,
        char              *in_uri,
        int                in_keepAlive
    );

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Destroys Connection
     *
     * Destroys the connection with the in_con connection handle
     *
     * @param[in] in_con connection handle to be destroyed
     * @return ::ERR_HTTP_OK            <br>Indicating success
     * @return ::ERR_HTTP_NOTIMPL       <br>This funtionality is not implemented
     * @return ::ERR_HTTP_INVALIDARG    <br>The input arguments are not valid. Arguments cannot be NULL
     */
    /*---------------------------------------------------------------------------*/
    extern u_int32_t
    http_DestroyConnection(
        http_connection_t in_con
    );

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Sends Data and Get its Resonse in a single Round-trip
     *
     * Sends data to the server and Receive the data from the server in Response
     *
     * @param[in] in_con  connection
     * @param[in] in_bufReq  buffer for outgoing datagram
     * @param[in] in_sizeReq  datagram size to be sent
     * @param[out] out_bufRes incoming buffer pointer address
     * @param io_sizeRes [i/o] datagram size to be read (in), actual read size (out)
     * @return ::ERR_HTTP_OK                    <br>Indicating success
     * @return ::ERR_HTTP_NOTIMPL               <br>This funtionality is not implemented
     * @return ::ERR_HTTP_OUTOFMEMORY           <br>A resource failure has occurred
     * @return ::ERR_HTTP_INVALIDARG            <br>The input arguments are not valid. Arguments cannot be NULL
     * @return ::ERR_HTTP_FATAL                 <br>http connection or context error has occurred
     * @return ::ERR_HTTP_INTERNALSERVERERROR   <br>An unrecoverable error has occurred on server. Our test code currently doesn't handle this error
     * @return ::ERR_HTTP_NOTFOUND              <br>Http server has not found
      */
    /*---------------------------------------------------------------------------*/
    extern u_int32_t
    http_SendRecv(
        http_connection_t  in_con,
        u_int8_t          *in_bufReq,
        u_int32_t          in_sizeReq,
        u_int8_t          **out_bufRes,
        u_int32_t         *io_sizeRes
    );

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Free Response buffer returned from http_SendRecv
     *
     * @param[in] in_bufRes response buffer returned from http_SendRecv
     * @return ::ERR_HTTP_OK                         <br>Indicating success
      */
    /*---------------------------------------------------------------------------*/
    extern u_int32_t
    http_FreeResponseBuffer(
        u_int8_t          *in_bufRes
    );


    /*---------------------------------------------------------------------------*/
    /**
     * @brief Sets Extra HTTP Header
     *
     * @param[in] in_con connection
     * @param[in] in_name property name
     * @param[in] in_value property value
     * @param[in] in_overwritef flag regarding whether overwriting is allowed
     *                           or not
     * @return ::ERR_HTTP_OK                <br>Indicating success
     * @return ::ERR_HTTP_NOTIMPL           <br>This funtionality is not implemented
     * @return ::ERR_HTTP_OUTOFMEMORY       <br>A resource failure has occurred
     * @return ::ERR_HTTP_INVALIDARG        <br>The input arguments are not valid. Arguments cannot be NULL
     * @return ::ERR_HTTP_FATAL             <br>http connection or context error has occurred
     */
    /*---------------------------------------------------------------------------*/
    extern u_int32_t
    http_SetExtraHeader(
        http_connection_t  in_con,
        const char        *in_name,
        const char        *in_value,
        int                in_overwritef
    );


    /*---------------------------------------------------------------------------*/
    /**
     * @brief Gets Response Header Function
     *
     * Retreives the Response Header of specified header name in in_name
     *
     * @param[in] in_con connection
     * @param[in] in_name header name
     * @param[in] in_value_len  length of out_value buffer
     * @param[out] out_value header value
     * @return ::ERR_HTTP_OK            <br>Indicating success
     * @return ::ERR_HTTP_NOTIMPL       <br>This funtionality is not implemented
     * @return ::ERR_HTTP_INVALIDARG    <br>The input arguments are not valid. Arguments cannot be NULL
     */
    /*---------------------------------------------------------------------------*/
    extern u_int32_t
    http_GetResponseHeaderByName(
        http_connection_t  in_con,
        const char        *in_name,
        u_int32_t          in_value_len,
        char              *out_value
    );

#ifdef __cplusplus
};
#endif

#endif /* HTTP_API_H */
/** @} */


