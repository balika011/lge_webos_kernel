/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "CommTypes.h"
#include "CryptoTypes.h"
#include "TeeDebug.h"
#include "TeeInternalDpi.h"
#include "KeyMgmt.h"

/*********************************************************************
 * @function cryptoAgentInit
 *
 * @brief Crypto Agent initialization
 *
 * @param[in] argc - argument count
 * @param[in] argv - pointer to argument vector
 *
 * @returns nfTSR_SUCCESS,
 *
 *********************************************************************/

nfTeeCryptoResult_t cryptoAgentInit( int argc, char **argv )
{
	//TODO: these lines are added just to avoid compiler warnings, deal with the args properly
	if ((argc > 0) && (argv) && (*argv))
	{
		CRYPTODBG("%s: we have some arguments, do something with them!\n", __FUNCTION__);
	}

	nfTeeCryptoResult_t	Result;

	// need to call key mgmt init function(s)
	CRYPTODBG("%s: calling keyMgmtInit\n", __FUNCTION__);
	Result = keyMgmtInit();
	CRYPTODBG("%s: keyMgmtInit successfully completed, returning %08x\n", __FUNCTION__, Result);

	return Result;
}/* end cryptoAgentInit */
