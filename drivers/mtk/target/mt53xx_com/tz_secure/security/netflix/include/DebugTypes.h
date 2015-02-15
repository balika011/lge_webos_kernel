/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEBUGTYPES_H_
#define DEBUGTYPES_H_

#include "CommonIncludes.h"

typedef uint32_t nfDebugFlags_t;

// Debug flags
#define nfDEBUG_INIT   		0x00000001 // TEE initialization
#define nfDEBUG_COMM_AGENT 	0x00000002 // commAgent general
#define nfDEBUG_MGMT_AGENT 	0x00000004 // mgmtAgent general
#define nfDEBUG_MSG			0x00000008 // message debug
#define nfDEBUG_STORAGE		0x00000010 // storageAgent general
#define nfDEBUG_CRYPTO		0x00000020 // crypto lib debug
#define nfDEBUG_MEMORY		0x00000040 // mem alloc/free debug
#define nfDEBUG_RSA			0x00000080 // RSA debug
#define nfDEBUG_AES			0x00000100 // AES debug
#define nfDEBUG_DRM			0x00000200 // DRM general debug

/*
 * nfDEBUG_NAMES should ONLY be defined in one file per application,
 * and should be defined wherever you will parse the command line.
 *
 * If you add new debug flags, add the name you expect to be used
 * in the command line to enable that flag.
 */
#ifdef nfDEBUG_NAMES
// one name for each debug flag...
static char *DebugNames[] =
{
    "init",
    "commagent",
    "mgmtagent",
	"msg",
	"storage",
	"crypto",
	"memory",
	"rsa",
	"aes",
	"drm",
};
#endif /* nfDEBUG_NAMES */


#endif /* DEBUGTYPES_H_ */
