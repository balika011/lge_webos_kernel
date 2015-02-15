/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 *
 */

#ifndef HCI_DRM_TEST_H
#define HCI_DRM_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

    extern int32_t
    hci007_01(void);

    extern int32_t
    hci007_02(void);

    extern int32_t
    hci007_03(void);

    extern int32_t
    hci007_04(void);

    extern int32_t
    hci007_05(void);

    extern int32_t
    hci007_06(void);

    extern int32_t
    hci007_07(void);

    extern int32_t
    hci007_08(void);

    extern int32_t
    hci007_09(void);

    extern int32_t
    hci007_10(void);

    extern int32_t
    hci007_11(void);

    extern int32_t
    hci008_01(void);

    extern int32_t
    hci008_02(void);

    extern int32_t
    hci008_03(void);

    extern int32_t
    hci008_04(void);

    extern int32_t
    hci009_01(void);

    extern int32_t
    hci009_02(void);

    extern int32_t
    hci009_03(void);

    extern int32_t
    hci009_04(void);

#if 1 == HCI_L_API_HCI_DECRYPTSTREAM_WITH_IV
    extern int32_t
    hci013_01(void);

    extern int32_t
    hci013_02(void);

    extern int32_t
    hci013_03(void);

    extern int32_t
    hci013_04(void);

    extern int32_t
    hci013_05(void);

    extern int32_t
    hci013_06(void);

    extern int32_t
    hci013_07(void);

    extern int32_t
    hci013_08(void);

#else
    extern int32_t
    hci013_09(void);
#endif

#if 1 == HCI_L_API_HCI_DECRYPTSTREAM_WITH_IV
    extern int32_t
    hci013_10(void);
#endif

#if 1 == HCI_L_API_HCI_EXPORTKEY
    extern int32_t
    hci028_01(void);

    extern int32_t
    hci028_02(void);

    extern int32_t
    hci028_03(void);

    extern int32_t
    hci028_04(void);

    extern int32_t
    hci028_05(void);
#else
    extern int32_t
    hci028_06(void);
#endif

#if 1 == HCI_L_API_HCI_DECRYPTSTREAM_WITH_OFFSET
    extern int32_t
    hci033_01(void);

    extern int32_t
    hci033_02(void);
#else
    extern int32_t
    hci033_03(void);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* HCI_DRM_TEST_H */
