/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 *
 */

#ifndef HCI_MOVE_TEST_H
#define HCI_MOVE_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#if 1 == HCI_L_API_HCI_MOVEKEY
    extern int32_t
    hci019_01(void);

    extern int32_t
    hci019_02(void);

    extern int32_t
    hci019_03(void);

    extern int32_t
    hci019_04(void);

    extern int32_t
    hci019_05(void);

    extern int32_t
    hci019_06(void);

    extern int32_t
    hci019_07(void);
#else
    extern int32_t
    hci019_08(void);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* HCI_MOVE_TEST_H */
