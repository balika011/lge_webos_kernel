/** @addtogroup http http
 * @ingroup outside
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011 Sony Corporation.
 */

#ifndef HTTPS_API_H
#define HTTPS_API_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        u_int32_t      cert_len;
        u_int8_t      *cert;
    } https_certchain_t;

    /* structure for holding certificates*/
    typedef struct {
        u_int32_t            num_certs;
        https_certchain_t  **certchain;    /* First cert in the array is the device certificate */
        u_int32_t            prvkey_len;
        u_int8_t            *prvkey;
        u_int32_t            cacert_len;
        u_int8_t            *cacert;
    } https_certs_t;

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Sends Data and Get its Response in a single Round-trip securely
     *
     * Sends data to the server and Receive the data from the server in Response
     *
     * @param[in] in_con                        connection
     * @param[in] in_bufReq                     buffer for outgoing datagram
     * @param[in] in_sizeReq                    datagram size to be sent
     * @param[in] in_cert_info                  certificates info required for secure session.
     *                                          (note: in_cert_info->cacert can be set NULL if no peer verification required )
     * @param[out] out_bufRes                   incoming buffer pointer address
     * @param[i/o] io_sizeRes                   datagram size to be read (in), actual read size (out)
     * @param[out] out_http_status              http status code
     * @param[out] out_mime_type                content type
     * @param[out] out_mime_type_len            content type length
     *
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
    https_SendRecv(
        http_connection_t  in_con,
        u_int8_t          *in_bufReq,
        u_int32_t          in_sizeReq,
        https_certs_t     *in_cert_info,
        u_int8_t         **out_bufRes,
        u_int32_t         *io_sizeRes,
        u_int32_t         *out_http_status,
        u_int8_t         **out_mime_type,
        u_int32_t         *out_mime_type_len
    );

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Free mime type returned from https_SendRecv
     *
     * @param[in] in_mime_type                    mime type returned from https_SendRecv
     * @return ::ERR_HTTP_OK                      <br>Indicating success
      */
    /*---------------------------------------------------------------------------*/
    u_int32_t
    https_FreeMimeType(
        u_int8_t          *in_mime_type
    );

#ifdef __cplusplus
};
#endif

#endif /* HTTPS_API_H */
/** @} */


