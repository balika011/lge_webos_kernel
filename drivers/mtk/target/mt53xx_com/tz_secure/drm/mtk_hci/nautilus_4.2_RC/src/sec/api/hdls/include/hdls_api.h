/** @addtogroup hdls hdls
 *  @ingroup api
 *  @{ */
/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2007,2008,2009,2010,2011,2012 Sony Corporation.
 */

#ifndef HDLS_API_H
#define HDLS_API_H

#ifdef __cplusplus
extern "C" {
#endif

    /*===========================================================================*/
    /** @file
     *
     * @brief Map a given handle (pointer) to an index structure within
     * the given handle map, given the handle map.
     *
     * @author Valentin Mesaros <valentinm@sonycom.com>
     * @date   23 Nov, 2006 last modified by valentin
     */
    /*===========================================================================*/


    /*---------------------------------------------------------------------------*
     * macro definitions
     *---------------------------------------------------------------------------*/

#define HDLS_HDLBUF_LEN   BASE_SERVICE_HANDLE_MAP_SIZE

    /*---------------------------------------------------------------------------*
     * type definition
     *---------------------------------------------------------------------------*/

    /** Generic handle type */
    typedef void *hdls_anyhdl_t;

    /** Handle context structure */
    typedef struct {
        u_int32_t          hdlidx;
    } hdls_context_t;

    /** Handle map structure */
    typedef struct {
        hdls_anyhdl_t      hdls[HDLS_HDLBUF_LEN];  /**> mapping table: handle index => handle structure */
        bool_t             initialized;            /**> flag indicating whether the handle table has been initialized */
    } hdls_hdlmap_t;

    /**
     * @brief Initialize the values within the given handle map to NULL,
     * if not already done.
     *
     * @param[in,out] io_hdlmap     pointer to the handle map to initialize
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_ILLEGALARGS
     */
    extern np_ret_t
    hdls_InitHdlMap(hdls_hdlmap_t     *io_hdlmap);

    /**
     * @brief Map a given handle (pointer) to an index structure within
     * the given handle map, given the handle map.
     *
     * @param[in]      in_anyhdl       a handle to a given memory structure
     * @param[in,out]  io_hdlmap       handle map
     * @param[out]     out_context     the corresponding index structure
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_INTERNALERROR
     */
    extern np_ret_t
    hdls_MapHdlToIdx(hdls_anyhdl_t      in_anyhdl,
                     hdls_hdlmap_t      *io_hdlmap,
                     hdls_context_t     *out_context);

    /**
     * @brief Map a given index structure to the corresponding handle
     * (pointer), given the handle map.
     *
     * @param[in]     in_context             the corresponding index structure
     * @param[in,out] io_hdlmap              handle map
     * @param[out]    out_anyhdl             the corresponding to given memory structure
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_ILLEGALHANDLE
     */
    extern np_ret_t
    hdls_MapIdxToHdl(hdls_context_t      in_context,
                     hdls_hdlmap_t      *io_hdlmap,
                     hdls_anyhdl_t      *out_anyhdl);

    /**
     * @brief Set the mapping of the given index structure to the value of
     * in_anyhdl, given the handle map.
     *
     * @param[in]     in_context        index for a given memory structure
     * @param[in]     in_anyhdl         handle value to set
     * @param[in,out] io_hdlmap         handle map
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_ILLEGALHANDLE
     */
    extern np_ret_t
    hdls_SetHdlMapping(hdls_context_t      in_context,
                       hdls_anyhdl_t       in_anyhdl,
                       hdls_hdlmap_t      *io_hdlmap);


#ifdef __cplusplus
}
#endif

#endif /* HDLS_API_H */
/** @} */
