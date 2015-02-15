/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2010,2011 Sony Corporation.
 */

/**
 * @addtogroup mps mps
 * @ingroup goby
 * @{
 *
 * @file
 * @brief  Data types for Marlin USB protocol sequencer
 * @note   this API is incomplete and subject to change
 */

#ifndef MPS_TYPES_H
#define MPS_TYPES_H

NP_BEGIN_EXTERN_C

#define MPS_ACTION_BB            0  /**< Initialising MPS with an action token for BB */
#define MPS_ACTION_LTP           1  /**< Initialising MPS with an action token for BB */
#define MPS_ACTION_TT            2  /**< Initialising MPS for TT update (no action token) */

#define MPS_HANDLE_NULL                         NULL

/**
 * @brief handle to an mps context
 */
typedef void *mps_handle_t;

NP_END_EXTERN_C

#endif /* MPS_TYPES_H */
/** @} */
