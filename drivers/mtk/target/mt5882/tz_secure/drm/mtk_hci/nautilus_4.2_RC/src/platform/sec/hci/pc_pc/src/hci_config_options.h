/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011 Sony Corporation.
 */

#ifndef HCI_CONFIG_OPTIONS_H
#define HCI_CONFIG_OPTIONS_H


#define HCI_L_API_HCI_MOVEKEY                   1
#define HCI_L_API_HCI_DECRYPTSTREAM_WITH_IV     1
#define HCI_L_API_HCI_DECRYPTSTREAM_WITH_OFFSET 1

#if 1 == HCI_L_SERV_DRM || 1 == HCI_L_SERV_IMPORT
#   define HCI_L_API_HCI_ENCRYPTKEYDATA         1
#   define HCI_L_API_HCI_LOADKEY                1
#endif

#if defined( NPOPT_ENABLE_EXPORT_ACTION )
#   define HCI_L_API_HCI_EXPORTKEY              1
#else
#   define HCI_L_API_HCI_EXPORTKEY              0
#endif

#if 1 == HCI_L_SERV_NEMO || 1 == HCI_L_SERV_DRM
#   define HCI_L_API_HCI_CHECKHASH              1
#endif

#if 1 == HCI_L_SERV_IMPORT
#   define HCI_L_API_HCI_GENERATECONTENTKEY     1
#   define HCI_L_API_HCI_GENERATEHMAC           1
#   define HCI_L_API_HCI_ENCRYPTDATA            1
#   define HCI_L_API_HCI_ENCRYPTSTREAM_WITH_IV  1
#   define HCI_L_API_HCI_IMPORTBKBCONTAINER     1
#   define HCI_L_API_HCI_GENERATEHASH           1
#endif

#if 1 == HCI_L_SERV_BBTS
#   define HCI_L_API_HCI_GENERATECONTENTKEY     1
#   define HCI_L_CRYPTO_DECRYPTAES128RTB        1
#endif

#if 1 == HCI_L_SERV_PPPROC
#   define HCI_L_API_HCI_IMPORTPPPROCDATA       1
#endif

#if 1 == HCI_L_SERV_MS3
#   define HCI_L_API_HCI_IMPORTSAS              1
#   define HCI_L_API_HCI_FREESASKEYS            1
#endif


#endif /* #ifndef HCI_CONFIG_OPTIONS_H */
