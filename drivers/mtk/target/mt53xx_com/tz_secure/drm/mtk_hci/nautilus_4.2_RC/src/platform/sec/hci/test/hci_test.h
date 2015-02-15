/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 *
 */

#ifndef HCI_TEST_H
#define HCI_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#define APPLICATION_DATA  NULL
#define AES128_KEY_SIZE   (16)

    retcode_t
    hci_l_Scramble(binstr_t *io_key);

    /* hci_GetDevIds */
    int32_t hci006_01(void);
    int32_t hci006_02(void);
    int32_t hci006_03(void);
    int32_t hci006_04(void);

    int32_t hci010_01(void);
    int32_t hci010_02(void);
    int32_t hci010_03(void);

    int32_t hci011_01(void);
    int32_t hci011_02(void);
    int32_t hci011_03(void);
    int32_t hci011_04(void);
    int32_t hci011_05(void);

    int32_t hci012_01(void);
    int32_t hci012_02(void);
    int32_t hci012_03(void);
    int32_t hci012_04(void);
    int32_t hci012_05(void);
    int32_t hci012_06(void);

    int32_t hci016_01(void);
    int32_t hci016_02(void);
    int32_t hci016_03(void);
    int32_t hci016_04(void);
    int32_t hci016_05(void);

#if 1 == HCI_L_API_HCI_CHECKHASH
    int32_t hci017_01(void);
    int32_t hci017_02(void);
    int32_t hci017_03(void);
    int32_t hci017_04(void);
    int32_t hci017_05(void);
#else
    int32_t hci017_06(void);
#endif

#if 1 == HCI_L_API_HCI_GENERATECONTENTKEY
    int32_t hci023_01(void);
    int32_t hci023_02(void);
    int32_t hci023_03(void);
    int32_t hci023_04(void);
#else
    int32_t hci023_05(void);
#endif

#if 1 == HCI_L_API_HCI_LOADKEY
    int32_t hci024_01(void);
    int32_t hci024_02(void);
    int32_t hci024_03(void);
    int32_t hci024_04(void);
    int32_t hci024_05(void);
#else
    int32_t hci024_06(void);
#endif

#if 1 == HCI_L_API_HCI_GENERATEHMAC
    int32_t hci025_01(void);
    int32_t hci025_02(void);
    int32_t hci025_03(void);
    int32_t hci025_04(void);
    int32_t hci025_05(void);
    int32_t hci025_06(void);
    int32_t hci025_07(void);
    int32_t hci025_08(void);
#else
    int32_t hci025_09(void);
#endif

    int32_t hci032_01(void);
    int32_t hci032_02(void);

#ifdef __cplusplus
}
#endif

#endif  /* HCI_TEST_H */

