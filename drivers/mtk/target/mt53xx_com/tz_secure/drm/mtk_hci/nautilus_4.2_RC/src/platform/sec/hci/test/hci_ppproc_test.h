/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 *
 */

#ifndef HCI_PPPROC_TEST_H
#define HCI_PPPROC_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#if (1 == HCI_L_API_HCI_IMPORTPPPROCDATA) \
   && ((PPPROC_FORMAT_VAL == HCI_L_PPPROC_FORMAT_DEFAULT) \
   || (PPPROC_FORMAT_VAL == HCI_L_PPPROC_FORMAT_MOA_DRBD) \
   || (PPPROC_FORMAT_VAL == HCI_L_PPPROC_FORMAT_MOA_QMU))

    extern int32_t
    hci027_01(void);

#if PPPROC_FORMAT_VAL == HCI_L_PPPROC_FORMAT_DEFAULT
    extern int32_t
    hci027_02(void);

    extern int32_t
    hci027_03(void);
#endif

#if PPPROC_FORMAT_VAL == HCI_L_PPPROC_FORMAT_MOA_DRBD
    extern int32_t
    hci027_04(void);

    extern int32_t
    hci027_05(void);
#endif

#if PPPROC_FORMAT_VAL == HCI_L_PPPROC_FORMAT_MOA_QMU
    extern int32_t
    hci027_06(void);

    extern int32_t
    hci027_07(void);
#endif
    extern int32_t
    hci027_09(void);
#else
    extern int32_t
    hci027_08(void);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* HCI_PPPROC_TEST_H */
