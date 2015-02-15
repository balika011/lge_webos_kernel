/** @addtogroup drm drm
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011,2012 Sony Corporation.
 */
#ifndef DRM_TYPES_H
#define DRM_TYPES_H

/** @file */

#define DRM_L_ASSERTION_ROLE            0
#define DRM_L_ASSERTION_DATACERT        1

/**
 * DRM profile for Marlin
 *
 * @todo: there is a lot of duplication between the handles to
 * various contexts and personalities, licences, links and
 * assertions. Should the handles remain and the pointers
 * be removed?
 *
 *
 */
typedef struct {
    octobj_node_t       *personality; /**< All octopus objects in the DRM system */
    octobj_license_t    *license;     /**< Pointer to the licences governing the DRM */
    int32_t           useckflags;   /**< bit map of what licence content keys are registered and in use */
    octcxt_handle_t   hostcontext;  /**< Octopus context used by the \ref pkt "plankton" system */
    keymng_handle_t   keymanager;   /**< All DRM keys are managed by this entity */
    pkt_hostcontext_t pktcontext;   /**< \ref pkt "plankton" module hostcontext */
    pkt_handle_t      pkthandle;    /**< handle for the \ref pkt "plankton" reference */
    int32_t           flags;        /**< bit map that keeps tracks of what actions have been initialized */
    int32_t           sony_flags;        /**< bit map that keeps tracks of what sony actions have been initialized */
} drm_l_context_t;

#define DRM_L_CONTEXT_FLAGS_ACTION_PLAY_INIT (1<<DRM_ACTIONTYPE_PLAY)
#define DRM_L_CONTEXT_FLAGS_ACTION_WRITETOMEDUM_INIT (1<<DRM_ACTIONTYPE_WRITETOMEDIUM)

#endif /* DRM_TYPES_H */
/** @} */
