/** @addtogroup ut_framework ut_framework
 *  @ingroup ut
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010,2011 Sony Corporation.
 *
 */
#ifndef UT_FRAMEWORK_API_H
#define UT_FRAMEWORK_API_H

NP_BEGIN_EXTERN_C

#include "ut_utils.h"

#define UT_MAX_ARGV 32
#define UT_ARGS_SEPARATOR ' '

typedef  struct unit_test_tag {
    const char *test_name;
    const char *args;
    np_ret_t    ret;
} unit_test_def;

/**
 * @brief  Callback definition of the main test function provided to ut_Exec()
 *
 */
typedef int32_t(*ut_main_test_function_t)(int32_t argc, char **argv);

/** @file
 * This library contains the Unit Test Framework functionalities
 *
 * @require base ut_utils
 *
 * @author Riccardo Ballarino <Riccardo.Ballarino@eu.sony.com>
 */


/**
 * @brief  Executes Unit Test cases
 *
 * @param[in] prog_name   Program name
 * @param[in] ut_num      Number of TestCases
 * @param[in] unit_tests  TestCases Structure
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_NG
 */
np_ret_t
ut_Exec( const char              *in_prog_name,
         np_size_t                in_ut_num,
         unit_test_def           *in_unit_tests,
         ut_main_test_function_t  in_main_test );


NP_END_EXTERN_C

#endif /* UT_FRAMEWORK_API_H */
/** @} */
