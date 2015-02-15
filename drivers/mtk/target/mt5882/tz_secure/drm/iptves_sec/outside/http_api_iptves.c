/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008,2009 Sony Corporation
 */

/*==========================================================================*/
/**
 * @file
 * HTTP Library
 *
 * require:
 */

#include "basic_types_iptves.h"

#include "http_api_iptves.h"

/*----------------------------------------------------------------------------
 * Functions for SDK
 *--------------------------------------------------------------------------*/
/**
 * Initialize component (not MT-safe)
 *
 * @retval RET_HTTP_OK                  Success
 * @retval RET_HTTP_NG                  Fail
 */
u_int32_t
http_Init(void)
{

	return RET_HTTP_OK;
}

/**
 * Finalize component (not MT-safe)
 *
 * @retval RET_HTTP_OK                  Success
 * @retval RET_HTTP_NG                  Fail
 */
u_int32_t
http_Fin(void)
{

	return RET_HTTP_OK;
}



/**
 * Get initialized HTTP handle.The cookie engine will start.
 * This function set "Content-Type: application/octet-stream"
 *
 * @param [in]     in_svr_uri           DRM Server URI (NULL terminated)
 * @param [out]    out_handle           HTTP Handle
 * @retval RET_HTTP_OK                  Success
 * @retval RET_HTTP_NG                  Fail
 * @retval RET_HTTP_ILLEGAL_ARGS        Illegal arguments are passed to this API
 */
u_int32_t
http_OpenConnection(const char         *in_svr_uri,
                    http_handle_t      *out_handle)
{

	return RET_HTTP_OK;
}





/**
 * Cleanup http handle.
 *
 * @param [in,out] io_handle            HTTP Handle
 * @retval RET_HTTP_OK                  Success
 * @retval RET_HTTP_NG                  Fail
 * @retval RET_HTTP_ILLEGAL_ARGS        Illegal arguments are passed to this API
 */
u_int32_t
http_CloseConnection(http_handle_t      io_handle)
{

	return RET_HTTP_OK;
}




/**
 * Execute HTTP GET/POST request.
 *
 * @param [in,out] io_handle            HTTP Handle
 * @param [in]     in_last_req_flag     Last request flag
 * @param [in]     in_req               HTTP Request (if NULL, it means GET request)
 * @param [out]    out_res              HTTP Response
 * @retval RET_HTTP_OK                  Success
 * @retval RET_HTTP_NG                  Fail
 */
u_int32_t
http_SendRecv(http_handle_t             io_handle,
              bool_t                    in_last_req_flag,
              const http_msg_t         *in_req,
              http_msg_t               *out_res)
{

	return RET_HTTP_OK;
}





/**
 * Cleanup response data.
 *
 * @param [in,out] io_handle            HTTP Handle
 * @param [in,out] io_res               HTTP Response
 * @retval RET_HTTP_OK                  Success
 * @retval RET_HTTP_NG                  Fail
 * @retval RET_HTTP_ILLEGAL_ARGS        Illegal arguments are passed to this API
 */
u_int32_t
http_FreeResponse(http_handle_t         io_handle,
                  http_msg_t           *io_res)

{

	return RET_HTTP_OK;
}





/**
 * Abort current HTTP request.
 * This function must be called from other thread.
 *
 * @param [in,out] io_handle            HTTP Handle
 * @retval RET_HTTP_OK                  Success
 * @retval RET_HTTP_NG                  Fail
 * @retval RET_HTTP_ILLEGAL_ARGS        Illegal arguments are passed to this API
 */
u_int32_t
http_Abort(http_handle_t                io_handle)
{

	return RET_HTTP_OK;
}



/*----------------------------------------------------------------------------
 * Functions for Application
 *--------------------------------------------------------------------------*/
/**
 * Initialize HTTP Library (not MT-safe)
 *
 * @retval RET_HTTP_OK                  Success
 * @retval RET_HTTP_NG                  Fail
 */
u_int32_t
http_GlobalInit(void)
{

	return RET_HTTP_OK;
}





/**
 * Finalize HTTP Library (not MT-safe)
 *
 * @retval RET_HTTP_OK                  Success
 * @retval RET_HTTP_NG                  Fail
 */
u_int32_t
http_GlobalFin(void)
{

	return RET_HTTP_OK;
}



/**
 * Set cookie storage file to be used by http_OpenConnection.
 * Checks that the file exists.
 *
 * @param [in]     in_cookie_file       Cookie storage file.  May be "".
 * @retval RET_HTTP_OK                  Success
 * @retval RET_HTTP_NG                  Fail
 * @retval RET_HTTP_ILLEGAL_ARGS        Illegal arguments are passed to this API
 */
u_int32_t
http_SetCookieFile(const char          *in_cookie_file)
{

	return RET_HTTP_OK;
}





/**
 * Set USER AGENT string (not MT-safe)
 *
 * @param [in]     in_user_agent        USER AGENT string (NULL means reset)
 * @retval RET_HTTP_OK                  Success
 * @retval RET_HTTP_NG                  Fail
 */
u_int32_t
http_SetUserAgent(const char           *in_user_agent)
{

	return RET_HTTP_OK;
}


/**
 * Set the maximum time in seconds that you allow
 * the libcurl transfer operation to take. (not MT-safe)
 *
 * @param [in]     in_timeout           The maximum time in seconds (0 means reset)
 * @retval RET_HTTP_OK                  Success
 */
u_int32_t
http_SetTimeout(u_int32_t               in_timeout)
{

	return RET_HTTP_OK;
}




/**
 * Set proxy host and port number. (not MT-safe)
 *
 * @param [in]     in_proxy_name        The proxy host name (NULL means reset)
 * @param [in]     in_proxy_port        The proxy host port number (0 means reset)
 * @retval RET_HTTP_OK                  Success
 * @retval RET_HTTP_NG                  Fail
 * @retval RET_HTTP_ILLEGAL_ARGS        Illegal arguments are passed to this API
 */
u_int32_t
http_SetProxy(const char               *in_proxy_name,
              u_int32_t                 in_proxy_port)
{

	return RET_HTTP_OK;
}




#if !defined(IPTVES_PUBLISH_BUILD)
/*----------------------------------------------------------------------------
 * Functions for Test/Tools
 *--------------------------------------------------------------------------*/
/**
 * Show cookies own by HTTP handle to stdout.
 *
 * @param [in,out] io_handle            HTTP Handle
 * @retval RET_HTTP_OK                  Success
 */
u_int32_t
http_PrintCookies(http_handle_t io_handle)
{

	return RET_HTTP_OK;
}




#endif /* !IPTVES_PUBLISH_BUILD */


