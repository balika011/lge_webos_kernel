/**
 * @addtogroup octcxt octcxt
 * @ingroup marlin
 * @{
 */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010 Sony Corporation.
 */
#ifndef OCTCXT_TYPES_H
#define OCTCXT_TYPES_H

/**
 * @file
 */

/**
 *  Dynamic containers used in case of /Octopus/Action/Parameters
 */
typedef struct {
    int32_t  handle;
    int32_t  type;
    void    *params;
} octcxt_l_dyn_containers_t;

#define DYN_CONTAINERS_MAX       8
#define DYN_CONTAINER_TYPE_PARAM 0
#define DYN_CONTAINER_TYPE_ATTR  1


/**
 * High level record of directed paths between nodes. When a path
 * is calculated it is stored in the linkmanager to remove repeated
 * calculations of these paths.
 */
typedef struct octcxt_l_reachable_tag {
    binstr_t                        *linkto;    /**< End node in the path*/
    struct octcxt_l_reachable_tag   *linkfrom;  /**< Starting node in the path */
    octobj_control_t                *control;   /**< control object that governs actions on the end node */
} octcxt_l_reachable_t;

/**
 *  \nd
 *
 */
typedef struct {
    octobj_vec_t *links;
    octobj_vec_t *reachables;
} octcxt_l_linkmanager_t;

/**
 * The DRM Context as it applies to Marlin
 *
 * The Marlin DRM implements the Octopus DRM architecture using
 * the SeaShell object store, this context wraps a single node
 * with it's licence, assertions and associates it with a
 * link manager and storage handler.
 */
typedef struct {
    octobj_node_t               *personality;      /**< entity in the system */
    octobj_license_t            *license;          /**< Governance rules for the node */
    octobj_assertion_t          *assertion[OCTCXT_ASSERTION_MAX_IDX]; /**< Array of Role and DCS assertions */
    int32_t                      a_idx;            /**< Index for assertions (@0, @1,...) */
    octcxt_l_linkmanager_t      *linkmanager;
    binstr_t                    *sl;               /**< Serialized License Suspension list */
    seashell_handle_t           seashell;          /**< object store handler */
    binstr_t                    *contextid;        /**< Identifier for this context */
    octobj_params_t             *params;           /**< Parameter blocks */
    octcxt_l_dyn_containers_t    dyn_containers[DYN_CONTAINERS_MAX]; /**< Dynamic containers table */
} octcxt_l_context_t;

/** @} */

#endif /* OCTCXT_TYPES_H */
