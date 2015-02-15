/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 */

#ifndef NPSAC_TYPES_H
#define NPSAC_TYPES_H

NP_BEGIN_EXTERN_C

/* Indices to find back specific info in to np_sac_msg_info_t's msg_data field*/
/** Number of message data items*/
#define NP_SAC_MSG_INFO_NUMOF_ITEMS   3
/** index of the OCI type in sac_msg_info for OCI+optional CK*/
#define NP_SAC_MSG_INFO_TYPE_IDX      0
/** index of the server content id. in sac_msg_info for OCI+optional CK*/
#define NP_SAC_MSG_INFO_SERVER_ID_IDX 1
/** index of the client content id. in sac_msg_info for OCI+optional CK*/
#define NP_SAC_MSG_INFO_CLIENT_ID_IDX 2


/**
 * @brief SAC Message types
 *
 * Type to specify the type of SAC messages that can be sent between a client
 * and server
 **/
typedef enum {
    NP_SAC_OCI_CK_IMPORT_REQ = 0      /**< Import CK and OCI data to the client */
} np_sac_msg_t;

/**
 * @brief Handle types
 *
 * Type to specify the types of handles that can be supplied to the SAC
 **/
typedef enum {
    NP_SAC_SERVICE_HANDLE = 0         /**< Supply a service np_handle_t to the sac */
} np_sac_handle_type_t;

/* -------------------------------------------------------------------------- */
/*                        DATA STRUCTURES                                     */
/* -------------------------------------------------------------------------- */

/**
 * @brief SAC message information type
 *
 * Type to specify data that is required to build for the supplied message
 * type
 **/
typedef struct np_sac_msg_info_tag {
    np_size_t  num_msg_data_items;    /**< Number of data strings used to build
                                       *   the service message carried in the
                                       *   sac protocol */
    np_str_t **msg_data;              /**< Data string(s) */
} np_sac_msg_info_t;

/**
 * @brief Service Handle type
 *
 * A generic type to hold the handle used by the SAC when generating a
 * request (client), processing a request (server) or parsing a response
 * (client)
 **/
typedef struct np_sac_serv_handle_tag {
    np_sac_handle_type_t handle_type; /**< Type of handle */
    union {
        np_handle_t      service;     /**< Service handle */
    } handle;
} np_sac_serv_handle_t;

NP_END_EXTERN_C

#endif /* NPSAC_TYPES_H */
/** @} */
