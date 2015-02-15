/** @addtogroup tl tl
 * @ingroup platform
 * @{
 *
 * @attention
 * These functions are for testing and reference only. They do not constitute
 * transport layer modules for nautilus. Applications that require non-local
 * transport layers SHOULD implement their own in accordance with the SMI
 * high level document.
 */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010 Sony Corporation.
 *
 */
#ifndef PLATFORM_TRANSPORTLAYER_H
#define PLATFORM_TRANSPORTLAYER_H

NP_BEGIN_EXTERN_C

/**
 * @brief Initializes a transport layer
 *
 * Initializes a transport layer (either TCPIP transpot layer or USB transport layer)
 *
 * @param[in] in_tinfo Target information for this transport
 *
 * @return ::ERR_OK                 <br>Indicating Success
 * @return ::ERR_TL_OK              <br>Indicating Success
 * @return ::ERR_TL_INTERNALERROR   <br>An unrecoverable error has occurred
 * @return ::ERR_TL_ILLEGALARGS     <br>The input arguments are incorrect. Arguments cannot be NULL
 */
tl_ret_t
platform_tl_Init( np_target_info_t *in_tinfo );

/**
 * @brief Finalizes a transport layer
 *
 * Finalizes a transport layer for TCPIP TL or USB TL
 *
 * @param[in] in_tinfo Target information for this transport
 *
 * @return ::ERR_TL_OK              <br>Indicating Success
 */
tl_ret_t
platform_tl_Fin( np_target_info_t *in_tinfo );

NP_END_EXTERN_C

#endif /* PLATFORM_TRANSPORTLAYER_H */
/** @} */


