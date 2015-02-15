/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2010,2011,2012 Sony Corporation.
 */

/**
 * @addtogroup reg  reg
 * @ingroup sec
 * @{
 *
 * @file
 * @brief  link master table
 * @note   this code does not access link files themselves
 */

#ifndef REG_LINKS_H
#define REG_LINKS_H

NP_BEGIN_EXTERN_C

typedef struct reg_links {
    struct reg_links *next;
    u_int8_t  hash_to[SHA1_DIGESTSIZE];
    u_int8_t  hash_from[SHA1_DIGESTSIZE];
    u_int8_t  domainid[SHA1_DIGESTSIZE];

    bool_t    nfull_id;
    u_int32_t idx;

    binstr_t *from_id;
    binstr_t *to_id;
    binstr_t *svcid;
    binstr_t *sinfo;
} reg_links_t;

/**
 * @brief   Writes link master file to ssmem
 *
 * @param[in] in_links  list of link entries
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 */
extern retcode_t
reg_links_Write(
    reg_links_t *in_links);

/**
 * @brief   Reads link master file from ssmem
 *
 * @param[out]  out_links  List of link entries
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_CORRUPTED
 * @return ::ERR_NG
 */
extern retcode_t
reg_links_Read(
    reg_links_t **out_links);

/**
 * @brief   Frees list of link entries
 *
 * @param[out]  io_links  List of link entries
 *
 * @return ::ERR_OK
 */
extern retcode_t
reg_links_Free(
    reg_links_t *io_links);

/**
 * @brief   Inserts a link entry to the list (or overwrites it if from-id and to-id are same).
 *
 * @note  If an N3.3 entry is found for the from-id and to-id and in_domainid is NULL, then this will
*         upgrade it first to an N3.4 entry.
 * @note returns ERR_REG_REGISTEREDMAX in case of table overflow
 *
 * @param[in,out]  io_links     List of link entries
 * @param[in]      in_from_id   NULL, or the id of the from-node
 * @param[in]      in_to_id     the id of the to-node
 * @param[in]      in_domainid  NULL, or a domainid (this will create an N3.3 entry if domainid != NULL).
 * @param[in]      in_svcid     NULL, or the service ID
 * @param[in]      in_sinfo     NULL, or the service info
 * @param[out]     out_index    The index to use for the link file itself
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 */
extern retcode_t
reg_links_Insert(
    reg_links_t **io_links,
    binstr_t     *in_from_id,
    binstr_t     *in_to_id,
    binstr_t     *in_domainid,
    binstr_t     *in_svcid,
    binstr_t     *in_sinfo,
    u_int32_t    *out_index);

/**
 * @brief   Searches for an N3.3 entry by domainID.
 *
 * @param[in,out]  in_links     List of link entries
 * @param[in]      in_domainid  The domain id
 * @param[out]     out_link     Pointer within the list of entries to the link found.Should
 *                              not be freed explicitly as it would be freed as part of in_links.
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_REG_NOLINKREGISTERED
 */
extern retcode_t
reg_links_GetByDomainID(
    reg_links_t   *in_links,
    binstr_t      *in_domainid,
    reg_links_t  **out_link);

/**
 * @brief   Searches for a link by from-node and to-node
 *
 * @note  finds both N3.3 and N3.4 entries
 *
 * @param[in,out]  in_links     List of link entries
 * @param[in]      in_from_id   NULL, or the id of the from-node
 * @param[in]      in_to_id     The id of the to-node
 * @param[out]     out_link     Pointer within the list of entries to the link found.Should
 *                              not be freed explicitly as it would be freed as part of in_links.
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_REG_NOLINKREGISTERED
 */
extern retcode_t
reg_links_GetByFromTo(
    reg_links_t   *in_links,
    binstr_t      *in_from_id,
    binstr_t      *in_to_id,
    reg_links_t  **out_link);

/**
 * @brief   Removes a link entry from the list.
 *
 * @note  Propagates deletion to isolated nodes
 *
 * @param[in,out]  io_links     List of link entries
 * @param[in]      in_link      Pointer to a link within the list; the link to remove
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_INTERNALERROR
 */
extern retcode_t
reg_links_Remove(
    reg_links_t **io_links,
    reg_links_t  *in_link);

/**
 * @brief   Removes a link entry from the list for a given index.
 *
 * @note  Propagates deletion to isolated nodes
 *
 * @param[in]  u_int32_t      Index entry to be deleted
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_INTERNALERROR
 */
extern retcode_t
reg_links_RemoveByIndex(
    u_int32_t  in_index);

/**
 * @brief   Deletes links whose hashed from-id or to-id is the given sha1
 * (sha1 is typically the hashed id of a node recently deleted)
 *
 * @note  Can remove both N3.3 and N3.4 entries
 * @note  Propagates deletion to isolated nodes
 *
 * @param[in,out]  io_links     List of link entries
 * @param[in]      in_sha1      The hashed id of a node recently deleted
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_INTERNALERROR
 */
extern retcode_t
reg_links_RemoveDanglingLinks(
    reg_links_t **io_links,
    u_int8_t     *in_sha1);

/**
 * @brief   Upgrades an N3.3 entry into an N3.4 entry.
 *
 * @param[in,out]  io_links     List of link entries
 * @param[in]      in_from_id   NULL, or the id of the from-node
 * @param[in]      in_to_id     the id of the to-node
 * @param[in]      in_svcid     NULL, or the service ID
 * @param[in]      in_sinfo     NULL, or the service info
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 */
extern retcode_t
reg_links_Upgrade(
    reg_links_t  *io_links,
    binstr_t     *in_from_id,
    binstr_t     *in_to_id,
    binstr_t     *in_svcid,
    binstr_t     *in_sinfo);

NP_END_EXTERN_C

#endif /* REG_LINKS_H */
/** @} */
