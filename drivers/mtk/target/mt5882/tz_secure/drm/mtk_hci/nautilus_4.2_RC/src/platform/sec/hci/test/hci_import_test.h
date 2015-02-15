/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 *
 */

#ifndef HCI_IMPORT_TEST_H
#define HCI_IMPORT_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#if 1 == HCI_L_API_HCI_ENCRYPTSTREAM_WITH_IV
    extern int32_t
    hci014_01(void);

    extern int32_t
    hci014_02(void);

    extern int32_t
    hci014_03(void);

    extern int32_t
    hci014_04(void);

    extern int32_t
    hci014_05(void);

    extern int32_t
    hci014_06(void);

    extern int32_t
    hci014_07(void);

    extern int32_t
    hci014_08(void);

    extern int32_t
    hci014_09(void);

#else
    extern int32_t
    hci014_10(void);
#endif

#if 1 == HCI_L_API_HCI_ENCRYPTSTREAM_WITH_IV
    extern int32_t
    hci014_11(void);
#endif
#ifdef __cplusplus
}
#endif

#endif  /* HCI_IMPORT_TEST_H */
