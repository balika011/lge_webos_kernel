/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "CommTypes.h"
#include "StorageTypes.h"
#include "StorageAgent.h"
#include "StorageAgentTPI.h"
#include "TeeDebug.h"
#include "TeeInternalDpi.h"
#include "ManufSS.h"


/*  this code will run in the trust zone , we can get idfile from the km interface.
* 	so , hard code idfile data in here .
*
*/
extern uint8_t IDFILE_BUF[];
extern uint32_t idFile_len;
static int GetIdFileData(uint8_t** fileData , uint32_t* fileLen, uint32_t fileMaxLen)
{
	#if 0
	uint8_t idfileBuf[102] = {0x4d,0x54,0x4b,0x4d,0x54,0x35,0x2d,0x30,0x30,0x33,0x30,0x30,0x30,0x30,0x30,0x30,
								0x30,0x30,0x30,0x30,0x30,0x31,0x30,0x30,0x30,0x30,0x31,0x31,0x30,0x38,0x0d,0x0a,
								0x70,0x70,0x6a,0x6f,0x61,0x78,0x6e,0x62,0x54,0x68,0x4b,0x2f,0x66,0x34,0x73,0x4e,
								0x55,0x69,0x69,0x54,0x61,0x41,0x3d,0x3d,0x0d,0x0a,0x5a,0x76,0x79,0x51,0x33,0x6f,
								0x58,0x31,0x52,0x6c,0x56,0x45,0x30,0x32,0x4a,0x50,0x57,0x65,0x35,0x69,0x4e,0x4b,
								0x56,0x48,0x68,0x2f,0x73,0x6e,0x49,0x5a,0x6d,0x63,0x70,0x61,0x32,0x5a,0x56,0x32,
								0x61,0x75,0x62,0x4b,0x30,0x3d};
			
	teeMemcpy(fileData,idfileBuf,102);
	*fileLen = 102;
	#endif
	
	*fileData = IDFILE_BUF;
	*fileLen = idFile_len;
	
	return 1;

}


/*
 * This is the definition for the manufacturing secure store table. This
 * table is shared with StorageAgentRead.c
 */
nfSecureStoreTableRow_t saManufSSTable[saMANUF_SS_TABLE_SIZE] = {{0, NULL, 0, NULL}};
static nfStorageAgentState_t saStorageAgentState = nfSAS_UNINIT;

// an all-zeros clientID means "any"
static nfTeeClientId_t saClientIdAny = {0x00};

/*********************************************************************
 * @function saGetStorageAgentState
 *
 * @brief get storageAgent state
 *
 *
 * @return nfTSR_SUCCESS, error otherwise
 *
 *********************************************************************/
nfStorageAgentState_t saGetStorageAgentState()
{
	return saStorageAgentState;
}

/*********************************************************************
 * @function saSetStorageAgentState
 *
 * @brief get storageAgent state
 *
 *
 * @return nfTSR_SUCCESS, error otherwise
 *
 *********************************************************************/
nfTeeResult_t saSetStorageAgentState(nfStorageAgentState_t NewState )
{
	saStorageAgentState = NewState;
	return nfTGR_SUCCESS;
}

/*********************************************************************
 * @function saLookupKey
 *
 * @brief find data for matching client key
 *
 * @param[in/out] ClientId - client ID for requester (must match key)
 * @param[in] Key - the client key
 * @param[in] DataPtr - pointer to where you want data
 * @param[out] DataLengthPtr - pointer to where you want length returned
 * @param[in] MaxLength - max size of data
 *
 * @return nfTSR_SUCCESS, error otherwise
 *
 * NOTE: If you pass in the special mgmt client ID, this will be
 *       over-written with the actual client ID associated with
 *       the key upon return...
 *
 *********************************************************************/
nfTeeResult_t saLookupKey( nfTeeClientId_t 			ClientId,
				 	 	   nfSecureStoreClientKey_t	Key,
				 	 	   uint8_t					*DataPtr,
				 	 	   uint32_t					*DataLengthPtr,
				 	 	   uint32_t					MaxLength )
{
	int row;

	/*
	 * We need to do the following:
	 * 	- validate arguments
	 * 	- look for matching key/clientid in saManufSSTable
	 * 	- if found
	 * 		- make sure there is enough room at DataPtr
	 * 		- copy data
	 * 		- set DataLengthPtr
	 * 		- copy client ID
	 * 		- return
	 * 	- else
	 * 		- return error
	 */
	//assert(Key != 0);
	//assert(DataPtr);
	//assert(DataLengthPtr);

	STORAGEDBG("%s: looking for key %0x in saManufSSTable[]\n ", __FUNCTION__, Key);
	for ( row = 0; row < nfSSRI_INVALID; ++row )
	{
		STORAGEDBG("%s: comparing to key %0x in row %d\n", __FUNCTION__, saManufSSTable[row].ClientKey, row );
		if ( saManufSSTable[row].ClientKey == Key )
		{
			STORAGEDBG("%s: found match in row %d, looking at clientId\n", __FUNCTION__, row );
			if ( ( teeMemcmp( saManufSSTable[row].ClientIdPtr, saClientIdAny, sizeof(nfTeeClientId_t) ) == 0 )
			  || ( teeMemcmp( ClientId, saManufSSTable[row].ClientIdPtr, sizeof(nfTeeClientId_t)) == 0 ) )
			{
				STORAGEDBG("%s: client id's match, checking length (%d/%d)\n", __FUNCTION__,
						 saManufSSTable[row].DataLength, MaxLength );
#if 0
				STORAGEDBG("\tdumping stored client ID:\n");
				teePrintHexDump(saManufSSTable[row].ClientIdPtr, sizeof(nfTeeClientId_t));
				STORAGEDBG("\tdumping passeded client ID:\n");
				teePrintHexDump(ClientId, sizeof(nfTeeClientId_t));
#endif

				// found it -- how long is it?
				if ( saManufSSTable[row].DataLength <= MaxLength )
				{
					STORAGEDBG("%s: found data for key %08x, length is %d\n", __FUNCTION__, Key, saManufSSTable[row].DataLength );
					*DataLengthPtr = saManufSSTable[row].DataLength;
					teeMemcpy( DataPtr, saManufSSTable[row].DataPtr, *DataLengthPtr );
#if 0
					STORAGEDBG("\tHere's the data I'm returning:\n");
					teePrintHexDump(DataPtr, *DataLengthPtr);
#endif
					return nfTGR_SUCCESS;
				}
				else
				{
					STORAGEDBG("%s: not enough room in passed buffer (%d/%d)\n", __FUNCTION__,
							saManufSSTable[row].DataLength, MaxLength );
					return nfTSR_BUFFER_TOO_SHORT;
				}
			}/* end dual teeMemcmp() */
		}
	}/* end for ( row = 0; row < nfSSRI_INVALID; ++row ) */

	/*
	 * Each TEE agent can request its own personal key. These keys must be
	 * device unique. We provide a TPI function for this purpose.
	 *
	 * So far, cryptoAgent is the only one who uses this service. In that
	 * case, it is used to generate the keys used for "persisting" keys.
	 */
	// if we get here, we didn't find it. Is it a magic internal key?
	if ( Key == ntbaAGENT_KEY_INTERNAL )
	{
		STORAGEDBG("%s: creating application key ladder base.\n", __FUNCTION__ );
		return saTpiGetTAKeyLadderBase( ClientId, DataPtr, MaxLength, DataLengthPtr );

	}/* end if ( Key == ntbaAGENT_KEY_INTERNAL ) */

	STORAGEDBG("%s: returing not-found for key %0x\n", __FUNCTION__, Key);
	return nfTSR_NOT_FOUND;

}/* end saLookupKey */


/*********************************************************************
 * @function storageAgentInit_data
 *
 * @brief Storage Agent initialization
 *
 * @param[in] SecureStoreFileData - data of secure store file
 *
 * @returns nfTGR_SUCCESS, nfTSR_MANUF_SSTORE_INVALID
 *
 *********************************************************************/
uint8_t 	manufSSDataBuf[nsMAX_SS_PROVISION_SIZE];
uint32_t	manufSSDataLen = 0;


nfTeeResult_t storageAgentInit_data( const char*  SecureStoreFileData)
{
	nfTeeStorageResult_t 	Result;
	nfStorageAgentState_t 	State = saStorageAgentState;

	STORAGEDBG("%s: calling storageAgentInit_data()\n", __FUNCTION__ );

	 /*
	   * as for BD/TV , we can get idFile data from KM indirectly instead of read id file ,
	   * we could not support file IO operation in our tz.
	   * 
	   * assume we can read file in tee.
	   *
	 */

	//char * pszIdFilePath = "/3rd_rw/netflix/idfile_sdk.bin";
	//char * pszDrmFile_zgpriv = "./data/dpi/playready1/zgpriv.dat";
	//char * pszDrmFile_bgroupcert = "./data/dpi/playready1/bgroupcert.dat";
	//char * pszDrmFile_devcerttemplate = "./data/dpi/playready1/devcerttemplate.dat";
	//STORAGEDBG("%s: pszIdFilePath\n", pszIdFilePath );

	uint8_t* 	idfileDataBufPtr = NULL;
	uint32_t 	idfileDataLen = 0;

	uint8_t 	drmZgPrivDataBuf[nfDRM_ZGPRIV_LEN];
	uint32_t 	drmZgPrivLen = 0;

	uint8_t 	drmCertTemplateDataBuf[nfDRM_CERT_TEMPLATE_SIZE];
	uint32_t 	drmCertTemplateDataLen = 0;

	uint8_t 	drmBGroupCertDataBuf[nfDRM_BGROUPCERT_SIZE];
	uint32_t 	drmBGroupCertDataLen = 0;

	//GetFileData(pszIdFilePath,idfileDataBuf,&idfileDataLen,nfIDFLIE_MAX_FILE_SIZE);	
	
	GetIdFileData(&idfileDataBufPtr,&idfileDataLen,nfIDFLIE_MAX_FILE_SIZE);
	teeBzero(drmZgPrivDataBuf,nfDRM_ZGPRIV_LEN);
	teeBzero(drmCertTemplateDataBuf,nfDRM_CERT_TEMPLATE_SIZE);
	teeBzero(drmBGroupCertDataBuf,nfDRM_BGROUPCERT_SIZE);
	teeBzero(manufSSDataBuf,nsMAX_SS_PROVISION_SIZE);

	#if 0
	GetFileData(pszDrmFile_zgpriv,drmZgPrivDataBuf,&drmZgPrivLen,nfDRM_ZGPRIV_LEN);
	GetFileData(pszDrmFile_bgroupcert,drmBGroupCertDataBuf,&drmBGroupCertDataLen,nfDRM_BGROUPCERT_SIZE);
	GetFileData(pszDrmFile_devcerttemplate,drmCertTemplateDataBuf,&drmCertTemplateDataLen,nfDRM_CERT_TEMPLATE_SIZE);
	#endif

	drmZgPrivLen = 32;
	drmBGroupCertDataLen = 32;
	drmCertTemplateDataLen = 32;
	
	teePrintHexDump_tag("IDFILE",idfileDataBufPtr,idfileDataLen);

	//check getFileData return value ......

	/*
	 * generate manufSS.bin and restore it's data
	 * this operation need be done in tee , so we could not use any file IO operation and orther linux sys functions ,
	 * so the manufSS.bin is  not be created acturally.
	*/
	
	generate(idfileDataBufPtr,idfileDataLen,
		drmZgPrivDataBuf,drmZgPrivLen,
		drmBGroupCertDataBuf,drmBGroupCertDataLen,
		drmCertTemplateDataBuf,drmCertTemplateDataLen,
		manufSSDataBuf,&manufSSDataLen);


	Result = saTpiInit( (uint8_t *) manufSSDataBuf, manufSSDataLen, &State );

	 STORAGEDBG("%s: new state is %d\n", __FUNCTION__, State);

	 saStorageAgentState = State;

	 return (nfTeeResult_t) Result;

}/* end storageAgentInit */
/*********************************************************************
 * @function storageAgentInit
 *
 * @brief Storage Agent initialization
 *
 * @param[in] SecureStoreFilePath - path to secure store file
 *
 * @returns nfTGR_SUCCESS, nfTSR_MANUF_SSTORE_INVALID
 *
 *********************************************************************/
nfTeeResult_t storageAgentInit( char *SecureStoreFilePath )
{
	 nfTeeStorageResult_t 	Result;
	 nfStorageAgentState_t 	State = saStorageAgentState;

	 STORAGEDBG("%s: calling saTpiInit()\n", __FUNCTION__ );

	 Result = saTpiInit( (uint8_t *) SecureStoreFilePath, 0, &State );

	 STORAGEDBG("%s: new state is %d\n", __FUNCTION__, State);

	 saStorageAgentState = State;

	 return (nfTeeResult_t) Result;

}/* end storageAgentInit */
