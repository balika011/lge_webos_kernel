/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 */

#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Enable/disable optional hci tests.
     *
     */

    /*activate tests that use hci_LoadPersonalityKey to overwrite a particular personality (i.e. tampering)*/
#define HCI_L_TEST_OVERWRITEPERSONALITY         1

    /*activate tests that assume that hci_Lock can return ALREADYLOCKED, or that hci_Unlock can return NOTLOCKED*/
#define HCI_L_TEST_LOCKING                      1

    /* to modify the test case hci019-04 */
#define HCI_L_TEST_MOVENEMOKEY                  1

    /* activate tests that use hci_GenerateContentKey with pc-specific, 16-byte plaintext pv */
#define HCI_L_TEST_NONPRIVATECONTENTKEY         1

#ifdef __cplusplus
}
#endif

#endif /* #ifndef TEST_CONFIG_H */
