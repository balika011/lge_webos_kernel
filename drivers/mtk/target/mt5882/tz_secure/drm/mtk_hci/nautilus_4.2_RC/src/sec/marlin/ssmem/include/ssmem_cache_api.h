/** @addtogroup ssmem ssmem
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2010,2011 Sony Corporation.
 */
#ifndef SSMEM_CACHE_API_H
#define SSMEM_CACHE_API_H

/* These functions to the ssmem_override component implemented by a porter to reuse
 * the ssmem cache table and the associated mechanism for loading and storing the
 * cache in sstore.
 *
 * It allows the ssmem_override component to persistently store and load binary data
 * from the disk.
 *
 * This is like a backdoor into the ssmem to directly save objects into the
 * persistent store
 */

/*---------------------------------------------------------------------------*/
/**
 * @brief Set an object into the cache table
 *
 * @param[in] in_type
 * @param[in] in_index
 * @param[in] in_object
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_NOMEM
 */
/*---------------------------------------------------------------------------*/
retcode_t
ssmem_CacheTableSetObject(int32_t    in_type,
                          int32_t    in_index,
                          binstr_t  *in_object);

/*---------------------------------------------------------------------------*/
/**
 * @brief Free object that was read from the table
 *
 * @param[in] in_object Object to free
 *
 */
/*---------------------------------------------------------------------------*/
void
ssmem_FreeCacheTableObject(binstr_t *in_object);

/*---------------------------------------------------------------------------*/
/**
 * @brief Get an object from the cache table
 *
 * @param[in]  in_type
 * @param[in]  in_index
 * @param[out] out_object
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NG
 */
/*---------------------------------------------------------------------------*/
retcode_t
ssmem_CacheTableGetObject(int32_t    in_type,
                          int32_t    in_index,
                          binstr_t **out_object);

#endif /* SSMEM_CACHE_API_H */

/** @} */

