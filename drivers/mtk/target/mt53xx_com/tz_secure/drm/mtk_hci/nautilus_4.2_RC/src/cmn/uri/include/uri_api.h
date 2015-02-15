/** @addtogroup uri uri
 *  @ingroup cmn
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2010,2011,2012 Sony Corporation.
 */

/**
 * @file
 *
 * @brief Support for the Marlin URI Templates
 */

#ifndef URI_API_H
#define URI_API_H

NP_BEGIN_EXTERN_C

/* -------------------------------------------------------------------------- */
/*                                                                 PUBLIC API */
/* -------------------------------------------------------------------------- */

/**
 * URI template variable
 **/
typedef struct {
    binstr_t        *name_space;      /** Namespace of the variable */
    binstr_t        *name;           /** Name of the variable */
    binstr_t        *value;          /** Value of the variable */
} uri_var_t;


#define ERR_URI_BASE                        (0)
/** Doing the template transform failed */
#define ERR_URI_TEMPLATE_TRANSFORM_ERR      (ERR_URI_BASE|1)

/* -------------------------------------------------------------------------- */
/**
 * @brief Check if the supplied string a Template Variable that needs to be
 *        expanded
 *
 * @param[in]     in_str              String to check
 * @param[out]    out_is_present      Flag indicating if the string contains
 *                                    a template variable
 *
 * @return ::ERR_OK
 * @return ::ERR_URI_XYZ
 * @return ::
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
npi_uri_IsUriTemplatePresent( npi_cmncontext_t      in_context,
                              binstr_t             *in_str,
                              bool_t               *out_is_present );

/* -------------------------------------------------------------------------- */
/**
 * @brief Expand the template variables found in the supplied string using
 *        the array of variables
 *
 * @note If the in_str contain multiple matches then it is assumed that the
 *       multiple attributes are present in in_vars to match against
 *
 * @param[in]     in_str              String containing template variable
 *                                    to be expanded
 * @param[in]     in_num_vars         Number of template variables to use if
 *                                    necessary during transform
 * @param[in]     in_vars             Array of name:value template variables
 * @param[out]    out_expanded_str    Expanded string
 *
 * @return ::ERR_OK
 * @return ::ERR_URI_TEMPLATE_TRANSFORM_ERR
 * @return ::
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
npi_uri_TransformTemplate( npi_cmncontext_t      in_context,
                           binstr_t             *in_str,
                           u_int32_t             in_num_vars,
                           uri_var_t           **in_vars,
                           binstr_t            **out_expanded_str );


/* -------------------------------------------------------------------------- */
/**
 * @brief Check if the supplied string is a compound URI
 *
 * @param[in]     in_str              String to check
 * @param[out]    out_is_compound     Flag indicating if the string contains
 *                                    a compound uri
 *
 * @return ::ERR_OK
 * @return ::ERR_URI_XYZ
 * @return ::
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
npi_uri_IsCompoundURI( npi_cmncontext_t      in_context,
                       binstr_t             *in_str,
                       bool_t               *out_is_compound );


/* -------------------------------------------------------------------------- */
/**
 * @brief Return the the strings of a compound uri
 *
 * @param[in]     in_str              String to parse
 * @param[out]    out_uri1            Left side of the compound uri
 * @param[out]    out_uri1            Right side of the compound uri
 *
 * @return ::ERR_OK
 * @return ::ERR_URI_XYZ
 * @return ::
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
npi_uri_ParseCompoundURI( npi_cmncontext_t     in_context,
                          binstr_t            *in_str,
                          binstr_t           **out_uri_left,
                          binstr_t           **out_uri_right );

/* -------------------------------------------------------------------------- */
/**
 *
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
npi_uri_ConvertUriScheme( npi_cmncontext_t     in_context,
                          const char          *in_orig_scheme,
                          const char          *in_new_scheme,
                          binstr_t            *in_orig_uri,
                          binstr_t           **out_new_uri );
NP_END_EXTERN_C

#endif /* URI_API_H */
/** @} */

