/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2007,2008,2009,2010,2011 Sony Corporation.
 */

#ifndef HDLIDX_API_H
#define HDLIDX_API_H

NP_BEGIN_EXTERN_C

/*---------------------------------------------------------------------------*
 * macro definitions
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 * type definitions
 *---------------------------------------------------------------------------*/

/**
 * @brief handles buffer
 */
typedef struct {
    void       **hdlbuf;
    u_int32_t  hdllen;
} hdlidx_hdls_t;


/*---------------------------------------------------------------------------*/
/**
 * @brief Given a buffer of handles, initialize each handle to NULL.
 *
 * @param io_hdls[in,out]         handles buffer
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS     when either in_hdls or in_hdls->hdlbuf is NULL
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
hdlidx_InitHandles(hdlidx_hdls_t *io_hdls);


/*---------------------------------------------------------------------------*/
/**
 * @brief Get the first index (position) of a free handle from the
 * given buffer of handles.
 *
 * @param in_hdls[in]          handles buffer
 * @param out_hdlidx[out]      free handle buffer index
 *
 * @return ::ERR_OK
 * @return ::ERR_INTERNALERROR   when there exists no free handle index
 * @return ::ERR_ILLEGALARGS     when either in_hdls or in_hdls->hdlbuf is NULL
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
hdlidx_GetFreeHandleIdx(hdlidx_hdls_t *in_hdls,
                        u_int32_t     *out_hdlidx);


/*---------------------------------------------------------------------------*/
/**
 * @brief Set the value of the handle indexed by in_hdlidx into io_hdls.
 *
 * @param io_hdls[in,out]      handles buffer
 * @param in_hdlidx[in]        handle buffer index
 * @param in_hdl[in]           the corresponding handle
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS     when either in_hdls or in_hdls->hdlbuf is NULL,
 *                             or when in_hdlidx is an invalid index
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
hdlidx_SetHandle(hdlidx_hdls_t  *io_hdls,
                 u_int32_t      in_hdlidx,
                 void           *in_hdl);


/*---------------------------------------------------------------------------*/
/**
 * @brief Get the handle indexed by in_hdlidx from in_hdls.
 *
 * @param in_hdls[in]          handles buffer
 * @param in_hdlidx[in]        handle buffer index
 * @param out_hdl[out]         the corresponding handle
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS     when either in_hdls or in_hdls->hdlbuf is NULL,
 *                             or when in_hdlidx is an invalid index
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
hdlidx_GetHandle(hdlidx_hdls_t  *in_hdls,
                 u_int32_t      in_hdlidx,
                 void           **out_hdl);


NP_END_EXTERN_C

#endif /* HDLIDX_API_H */
