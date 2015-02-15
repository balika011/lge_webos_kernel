/** @addtogroup ssmem ssmem
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2006,2007,2008,2009,2010,2011 Sony Corporation.
 */
#ifndef SSMEM_API_H
#define SSMEM_API_H

/** @file */

/*
 * Secure object types
 */
#define SSMEM_OBJTYPE_MIN                          1

#define SSMEM_OBJTYPE_NEM_DCSASSERTION             2
#define SSMEM_OBJTYPE_OPPU                         3
#define SSMEM_OBJTYPE_OPPR                         4
#define SSMEM_OBJTYPE_OCT_LINKSTATUS               5
#define SSMEM_OBJTYPE_OCT_NODE                     6
#define SSMEM_OBJTYPE_OCT_LINK                     7
#define SSMEM_OBJTYPE_OCT_LICENSESTATUS            8
#define SSMEM_OBJTYPE_SYS_TRUSTEDTIME              9
#define SSMEM_OBJTYPE_HMN_SECURITY                 10
#define SSMEM_OBJTYPE_BB_LSL                       11
#define SSMEM_OBJTYPE_OCT_SEASHELL                 12
#define SSMEM_OBJTYPE_BB_CRL                       13
#define SSMEM_OBJTYPE_LIL                          14
#define SSMEM_OBJTYPE_NPPU                         15
#define SSMEM_OBJTYPE_NPPR                         16
#define SSMEM_OBJTYPE_NROL                         17
#define SSMEM_OBJTYPE_BKBC                         18
#define SSMEM_OBJTYPE_LOPU                         19
#define SSMEM_OBJTYPE_LOPR                         20
#define SSMEM_OBJTYPE_ROPU                         21
#define SSMEM_OBJTYPE_ROPR                         22
#define SSMEM_OBJTYPE_INPU                         23
#define SSMEM_OBJTYPE_INPR                         24
#define SSMEM_OBJTYPE_IROL                         25
#define SSMEM_OBJTYPE_RROL                         26
#define SSMEM_OBJTYPE_LROL                         27
#define SSMEM_OBJTYPE_CDLC                         28
#define SSMEM_OBJTYPE_LDLC                         29


#define SSMEM_OBJTYPE_MAX                          30
#define SSMEM_OBJNUM_MAX                           32

/*
 * ssmem tags
 */
/* personality */
#define SSMEM_TAG_PERSONALITY_START                0x0000
#define SSMEM_TAG_PERSONALITY_OPPU                 (SSMEM_TAG_PERSONALITY_START+0)
#define SSMEM_TAG_PERSONALITY_OPPR                 (SSMEM_TAG_PERSONALITY_START+1)
#define SSMEM_TAG_PERSONALITY_NPPU                 (SSMEM_TAG_PERSONALITY_START+3)
#define SSMEM_TAG_PERSONALITY_NPPR                 (SSMEM_TAG_PERSONALITY_START+4)
#define SSMEM_TAG_PERSONALITY_NROL                 (SSMEM_TAG_PERSONALITY_START+5)
#define SSMEM_TAG_PERSONALITY_LOPU                 (SSMEM_TAG_PERSONALITY_START+6)
#define SSMEM_TAG_PERSONALITY_LOPR                 (SSMEM_TAG_PERSONALITY_START+7)
#define SSMEM_TAG_PERSONALITY_INPU                 (SSMEM_TAG_PERSONALITY_START+8)
#define SSMEM_TAG_PERSONALITY_INPR                 (SSMEM_TAG_PERSONALITY_START+9)
#define SSMEM_TAG_PERSONALITY_LROL                 (SSMEM_TAG_PERSONALITY_START+10)
#define SSMEM_TAG_PERSONALITY_RROL                 (SSMEM_TAG_PERSONALITY_START+11)
#define SSMEM_TAG_PERSONALITY_IROL                 (SSMEM_TAG_PERSONALITY_START+12)
#define SSMEM_TAG_PERSONALITY_ROPU                 (SSMEM_TAG_PERSONALITY_START+13)
#define SSMEM_TAG_PERSONALITY_ROPR                 (SSMEM_TAG_PERSONALITY_START+14)
#define SSMEM_TAG_PERSONALITY_CDLC                 (SSMEM_TAG_PERSONALITY_START+15)
#define SSMEM_TAG_PERSONALITY_LDLC                 (SSMEM_TAG_PERSONALITY_START+16)

#define SSMEM_TAG_PERSONALITY_END                  0x001F

/* security metadata */
#define SSMEM_TAG_SECURITY              0x0020

/* trusted time */
#define SSMEM_TAG_TT                    0x0021

/* CRL */
#define SSMEM_TAG_CRL                   0x0022

/* LIL */
#define SSMEM_TAG_LIL                   0x0023

/*BKB (BKB data, BK and  Time Stamp)*/
#define SSMEM_TAG_BKBC                   0x0024

/* Multiple object */
#define SSMEM_TAG_MULTIOBJ_START(x)    (0x0080+(SSMEM_OBJNUM_MAX*(x)))
#define SSMEM_TAG_MULTIOBJ_END(x)      (0x0080+((SSMEM_OBJNUM_MAX*((x)+1))-1))

/* link */
#define SSMEM_TAG_LINK_START            SSMEM_TAG_MULTIOBJ_START(0)
#define SSMEM_TAG_LINK_MASTER           (SSMEM_TAG_LINK_START+0)
#define SSMEM_TAG_LINK_END              SSMEM_TAG_MULTIOBJ_END(0)

/* assert */
#define SSMEM_TAG_ASSERT_START          SSMEM_TAG_MULTIOBJ_START(1)
#define SSMEM_TAG_ASSERT_DCS            (SSMEM_TAG_ASSERT_START+0)
#define SSMEM_TAG_ASSERT_END            SSMEM_TAG_MULTIOBJ_END(1)

/* license suspension list */
#define SSMEM_TAG_LSL_START        SSMEM_TAG_MULTIOBJ_START(2)
#define SSMEM_TAG_LSL              (SSMEM_TAG_LSL_START+0)
#define SSMEM_TAG_LSL_END          SSMEM_TAG_MULTIOBJ_END(2)

/* node */
#define SSMEM_TAG_NODE_START            SSMEM_TAG_MULTIOBJ_START(3)
#define SSMEM_TAG_NODE_MASTER           (SSMEM_TAG_NODE_START+0)
#define SSMEM_TAG_NODE_END              SSMEM_TAG_MULTIOBJ_END(3)

/* seashell */
#define SSMEM_TAG_SEASHELL_START        SSMEM_TAG_MULTIOBJ_START(4)
#define SSMEM_TAG_SEASHELL              (SSMEM_TAG_SEASHELL_START+0)
#define SSMEM_TAG_SEASHELL_END          SSMEM_TAG_MULTIOBJ_END(4)


/*---------------------------------------------------------------------------*/
/**
 * @brief Set a secure data object into the cache
 *
 * This function sets a secure data object into the store cache. By
 * definition, certain object types can appear multiple times (e.g. assertions)
 * whereas others can have only 1 instance (e.g. Nemo personality).
 * An index is used to differentiate between objects of the same type.
 *
 * @param[in] in_type   Object type
 * @param[in] in_index  Index (value irrelevant if the object type is singular)
 * @param[in] in_object Object
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_NOMEM
 *
 *
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
ssmem_SetObject(int32_t    in_type,
                int32_t    in_index,
                binstr_t  *in_object);

/*---------------------------------------------------------------------------*/
/**
 * @brief Retrieve a secure data object from the cache
 *
 * This function gets a secure data object from the secure data cache. By
 * definition, certain object types can appear multiple times (e.g. assertions)
 * whereas others can have only 1 instance (e.g. Nemo personality).
 * An index is used to differentiate between objects of the same type.
 *
 * @param[in] in_type   Object type
 * @param[in] in_index  Index (value irrelevant if the object type is singular)
 * @param[in] out_object Object
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NG
 *
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
ssmem_GetObject(int32_t    in_type,
                int32_t    in_index,
                binstr_t **out_object);


/*---------------------------------------------------------------------------*/
/**
 * @brief Returns the secure data cache content in serialized form
 *
 * @param[in] in_dirtyonly  Flag to indicate whether all secure data or
 *                           only the modified ('dirty') secure data should
 *                           be returned.
 * @param[out] out_storage Serialized snapshot of the secure data cache
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_NOMEM
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
ssmem_GetSdata(int32_t    in_dirtyonly,
               binstr_t **out_storage);

/*---------------------------------------------------------------------------*/
/**
 * @brief Initialize the secure data cache with given serialized
 *        storage content
 *
 * @param[in] in_storage Serialized secure data storage content
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_NOMEM
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
ssmem_SetSdata(binstr_t *in_storage);


/*---------------------------------------------------------------------------*/
/**
 * @brief Empty the secure data cache
 *
 * @return ::ERR_OK
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
ssmem_ClearSdata(void);

/*---------------------------------------------------------------------------*/
#ifdef NPOPT_USE_ENC_SDATA
/**
 * @brief Protect a secure data object
 *
 * @param in_tag    Object tag
 * @param in_obj    Object
 * @param out_obj   Object
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_NOMEM
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
ssmem_Protect(u_int32_t     in_tag,
              binstr_t     *in_obj,
              binstr_t    **out_obj);
#endif

#endif
/** @} */

