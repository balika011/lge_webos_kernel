/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 *
 */

#ifndef HCI_LICGEN_TEST_H
#define HCI_LICGEN_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#define ENCRYPTED_DATA_LEN_AES      (32)
#define ENCRYPTED_DATA_LEN_RSA      (128)

#if 1 == HCI_L_API_HCI_ENCRYPTKEYDATA
    int32_t hci015_01(void);
    int32_t hci015_02(void);
    int32_t hci015_03(void);
    int32_t hci015_04(void);
    int32_t hci015_05(void);
#else
    int32_t hci015_06(void);
#endif

#if 1 == HCI_L_API_HCI_GENERATEHASH
    int32_t hci018_01(void);
    int32_t hci018_02(void);
    int32_t hci018_03(void);
    int32_t hci018_04(void);
    int32_t hci018_05(void);
#else
    int32_t hci018_06(void);
#endif

#if 1 == HCI_L_API_HCI_IMPORTBKBCONTAINER
    int32_t hci026_01(void);
    int32_t hci026_02(void);
    int32_t hci026_03(void);
    int32_t hci026_04(void);
    int32_t hci026_05(void);
    int32_t hci026_06(void);
    int32_t hci026_08(void);
#else
    int32_t hci026_07(void);
#endif

#if 1 == HCI_L_API_HCI_ENCRYPTDATA
    int32_t hci031_01(void);
    int32_t hci031_02(void);
    int32_t hci031_03(void);
    int32_t hci031_04(void);
#else
    int32_t hci031_05(void);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* HCI_LICGEN_TEST_H */
