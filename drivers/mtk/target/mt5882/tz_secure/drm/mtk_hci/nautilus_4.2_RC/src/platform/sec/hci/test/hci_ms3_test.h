/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 *
 */

#ifndef HCI_MS3_TEST_H
#define HCI_MS3_TEST_H

#define L_APPLICATION_DATA          "dummyappdata"
#define L_SAS_CID_SZ                 20

#ifdef __cplusplus
extern "C" {
#endif

#if 1 == HCI_L_API_HCI_IMPORTSAS
    extern int32_t
    hci022_01(void);

    extern int32_t
    hci022_02(void);

    extern int32_t
    hci022_03(void);

    extern int32_t
    hci022_04(void);

    extern int32_t
    hci022_05(void);

    extern int32_t
    hci022_07(void);

#else
    extern int32_t
    hci022_06(void);
#endif

#if 1 == HCI_L_API_HCI_FREESASKEYS
    extern int32_t
    hci030_01(void);

    extern int32_t
    hci030_02(void);

    extern int32_t
    hci030_03(void);

#else
    extern int32_t
    hci030_04(void);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* HCI_MS3_TEST_H */

