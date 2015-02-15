#include "ManufSS.h"
#include "TeeInternalDpi.h"
#include "TeeDebug.h"
// private variables


enum ntba_RO_AgentClientKeys
{
    // these elements are write-protected
    ntbaAGENT_KEY_ESN = 0x80000001,
    ntbaAGENT_KEY_KPE = 0x80000002,
    ntbaAGENT_KEY_KPH = 0x80000003,
    ntbaAGENT_KEY_KPW = 0x80000004,
    ntbaAGENT_KEY_INTERNAL = 0x80000005, // crypto agent "personal" key
};

enum drm_RO_AgentClientKeys
{
    // these elements are write-protected
    drmAGENT_KEY_MODEL_PRIV = 0x80010001,
    drmAGENT_KEY_MODEL_CERT = 0x80010002,
    drmAGENT_KEY_DEVCERT_TEMPLATE = 0x80010003,
};


uint8_t secStoreHmacKey[32] = { 0x4b, 0x40, 0xf4, 0x7e, 0xf6, 0x29, 0xd0, 0x1c,
                              0x1f, 0x37, 0xe8, 0x3d, 0x3d, 0x30, 0xcd, 0x28,
                              0x70, 0x22, 0xee, 0x40, 0x10, 0xf5, 0x81, 0x61,
                              0x64, 0x2a, 0xe1, 0x7e, 0xe1, 0x7e, 0xec, 0x19 };

uint8_t secStoreAesKey[16] = { 0xd8, 0x95, 0x7a, 0x33, 0x14, 0x4d, 0xe9, 0x30,
                             0x02, 0xcd, 0x32, 0x89, 0x6e, 0x57, 0xdf, 0xfc };
uint8_t fileBuffer[nsMAX_SS_PROVISION_SIZE];
char readBuffer[nsMAX_SS_PROVISION_SIZE];
const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";





int generate(uint8_t* idFileDataBufPtr,uint32_t idFileLen,
					uint8_t* drmZgPrivDataBufPtr,uint32_t drmZgPrivLen,
					uint8_t* drmBGroupCertDataBufPtr,uint32_t drmBGroupCertDataLen,
					uint8_t* drmCertTemplateDataBufPtr,uint32_t drmCertTemplateDataLen,
					uint8_t* manufSSDataBufPtr,uint32_t* manufSSdataLenPtr)
{
	int 		rc;
	int 		bytesToWrite;
	//int 		bytesWritten;

	uint8_t 	*bufPtr;
	uint8_t 	*totalLengthPtr;

	uint8_t 	*padLengthPtr;
	uint16_t	totalLength = 0;
	uint16_t	padLength = 0;
	uint16_t	remainder;

	// clear buffer, get pointer to start
	x_memset( fileBuffer, 0,nsMAX_SS_PROVISION_SIZE );
	bufPtr = fileBuffer;

	/* first, create secure store header */
	// write file magic (secstore is big-endian)
	*bufPtr++ = (nfSS_FILE_MAGIC1 >> 24) & 0xff;
	*bufPtr++ = (nfSS_FILE_MAGIC1 >> 16) & 0xff;
	*bufPtr++ = (nfSS_FILE_MAGIC1 >> 8) & 0xff;
	*bufPtr++ = (nfSS_FILE_MAGIC1 & 0xff);
	*bufPtr++ = (nfSS_FILE_MAGIC2 >> 24) & 0xff;
	*bufPtr++ = (nfSS_FILE_MAGIC2 >> 16) & 0xff;
	*bufPtr++ = (nfSS_FILE_MAGIC2 >> 8) & 0xff;
	*bufPtr++ = (nfSS_FILE_MAGIC2 & 0xff);

	// write version
	*bufPtr++ = (nfSS_FILE_VERSION >> 8) & 0xff;
	*bufPtr++ = (nfSS_FILE_VERSION & 0xff);

	// save pointer to total length, we'll update later
	totalLengthPtr = bufPtr;
	bufPtr += sizeof(uint16_t);

	bufPtr += nfSS_HEADER_HMAC_SIZE;

	// save pointer to pad length, we'll update later
	padLengthPtr = bufPtr;
	bufPtr += sizeof(uint16_t);

	/*
	 * now, time to start building sections
	 */

	// first section: management section
	CRYPTODBG( "calling AddMgmtSection().\n" );
	if ( ( rc = addMgmtSection( &bufPtr,
			 nsMAX_SS_PROVISION_SIZE - (bufPtr - fileBuffer) ) ) != 0 )
	{
		CRYPTODBG( "AddMgmtSection failed, bailing.\n" );
		return -1;
	}

	teePrintHexDump_tag("after add mgmt section",fileBuffer,bufPtr-fileBuffer);

	
	// next section: NTBA read-only section (esn, kpe, kph)
	CRYPTODBG( "calling AddNtbaSection().\n" );
	if ( ( rc = addNtbaSection( &bufPtr,
			 nsMAX_SS_PROVISION_SIZE - (bufPtr - fileBuffer), idFileDataBufPtr ) ) != 0 )
	{
		CRYPTODBG( "AddNtbaSection failed, bailing.\n" );
		return -1;
	}
	teePrintHexDump_tag("after add ntba section",fileBuffer,bufPtr-fileBuffer);
	// next section: DRM read-only section (model cert/privkey, device cert template)
	CRYPTODBG( "calling AddDrmSection().\n" );
	if ( ( rc = addDrmSection( &bufPtr,
			 nsMAX_SS_PROVISION_SIZE - (bufPtr - fileBuffer), 
			 drmZgPrivDataBufPtr,drmZgPrivLen,
			 drmBGroupCertDataBufPtr,drmBGroupCertDataLen,
			 drmCertTemplateDataBufPtr,drmCertTemplateDataLen,manufSSDataBufPtr, manufSSdataLenPtr) ) != 0 )
	{
		CRYPTODBG( "AddDrmSection failed, bailing.\n" );
		return -1;

	}

	// Okay, no more sections, time to hash/encrypt
	CRYPTODBG( "Calculating pad.\n" );

	// calculate pad length and apply pkcs7 padding if appropriate
	totalLength = (uint16_t) (bufPtr - padLengthPtr);
	CRYPTODBG( "totalLength is %d\n", totalLength );
	if ( ( remainder = (totalLength + SHA1_BLOCK_SIZE) % CIPHER_BLOCK_SIZE ) != 0 )
	{
		int i;

		padLength = CIPHER_BLOCK_SIZE - remainder;

		for ( i = 1; i <= padLength; i++ )
			*bufPtr++ = (uint8_t) i;
	}

	CRYPTODBG( "padLength is %d\n", padLength );

	// update totalLength (it is the total, including padding and MAC)
	totalLength += padLength;

	// save padLength
	*padLengthPtr = (padLength >> 8) & 0xFF;
	*(padLengthPtr + 1) = padLength & 0xFF;

	/*
	 * compute ICV over data (SHA1), add to buffer
	 */
	rc = teeSha1_libtom( padLengthPtr, totalLength, bufPtr );
	if (rc != 0)
	{
		CRYPTODBG( "teeSha1_libtom returned %d, exiting.\n",rc );
		return -1;
	}

#if TEE_LIBTOM_TEST_ENABLE
	teePrintHexDump_tag("bufPtr",bufPtr,SHA1_BLOCK_SIZE);
	
	int rc__ = 0;
	uint8_t bufPtr__[SHA1_BLOCK_SIZE];
	teeBzero(bufPtr__,SHA1_BLOCK_SIZE);
	rc__ = teeSha1_libtom( padLengthPtr, totalLength ,bufPtr__);
	teePrintHexDump_tag("bufPtr__",bufPtr__,SHA1_BLOCK_SIZE);
	int Ret = compare_buffer(bufPtr,bufPtr__,SHA1_BLOCK_SIZE,SHA1_BLOCK_SIZE);
	//printf("Ret : %d\n",Ret);
	if(Ret != 0)
		exit(0);

#endif

	// update pointer, section length
	bufPtr += SHA1_BLOCK_SIZE;
	totalLength += SHA1_BLOCK_SIZE;
	*totalLengthPtr = (totalLength >> 8) & 0xFF;
	*(totalLengthPtr + 1) = totalLength & 0xFF;

	CRYPTODBG( "I think totalLength is %d\n", totalLength );

	CRYPTODBG( "calling Aes with %d bytes...\n", (uint32_t) (bufPtr - padLengthPtr) );
	if ( ( rc = teeAesCbc128EncryptZeroIV_libtom( padLengthPtr,
							(uint32_t) (bufPtr - padLengthPtr), secStoreAesKey ) ) != 0 )
	{
		// encryption failed, need to bail...
		CRYPTODBG( "EncryptWithClientKey returned %d, exiting.\n",
			rc );
		return -1;
	}

#if 0
	teePrintHexDump_tag("bufPtr",bufPtr,SHA1_BLOCK_SIZE);
	
	int rc__ = 0;
	uint8_t bufPtr__[SHA1_BLOCK_SIZE];
	teeBzero(bufPtr,SHA1_BLOCK_SIZE);
	rc__ =  teeAesCbc128EncryptZeroIV_libtom( padLengthPtr,
							(uint32_t) (bufPtr - padLengthPtr), secStoreAesKey ) 
	teePrintHexDump_tag("bufPtr__",bufPtr__,SHA1_BLOCK_SIZE);
	int Ret = compare_buffer(bufPtr,bufPtr__,SHA1_BLOCK_SIZE,SHA1_BLOCK_SIZE);
	if(Ret != 0)
		exit(0);

#endif
	

	// compute ICV over secstore header
	rc = teeHmacSha256_libtom( fileBuffer,
				nfSS_FILE_MAGIC_SIZE + nfSS_FILE_VERSION_SIZE + sizeof(uint16_t),
				secStoreHmacKey,
				nfSS_HEADER_HMAC_SIZE,
				fileBuffer + nfSS_FILE_MAGIC_SIZE + nfSS_FILE_VERSION_SIZE
				+ sizeof(uint16_t) );
	if ( rc < 0 )
	{
		CRYPTODBG( "hmacSha256 returned %d, exiting.\n",
			rc );
		return -1;
	}

#if TEE_LIBTOM_TEST_ENABLE
	teePrintHexDump_tag("bufPtr",fileBuffer + nfSS_FILE_MAGIC_SIZE + nfSS_FILE_VERSION_SIZE
				+ sizeof(uint16_t),32);

	uint8_t bufHmacPtr__[32];
	rc__ = teeHmacSha256_libtom( fileBuffer,
								nfSS_FILE_MAGIC_SIZE + nfSS_FILE_VERSION_SIZE + sizeof(uint16_t),
								secStoreHmacKey,
								nfSS_HEADER_HMAC_SIZE,
								bufHmacPtr__);
	teePrintHexDump_tag("bufPtr__",bufHmacPtr__,32);


#endif	


	// how many bytes?
	bytesToWrite = (int) (bufPtr - fileBuffer);
	//printf("secstore file is %d bytes\n", bytesToWrite);

	if(bytesToWrite >= nsMAX_SS_PROVISION_SIZE)
	{
		CRYPTODBG( "bytesToWrite %d is exceed the max value.\n",bytesToWrite  );
		return -1;
	}
	
	*manufSSdataLenPtr = bytesToWrite;
	teeMemcpy(manufSSDataBufPtr,fileBuffer,*manufSSdataLenPtr);

	return 0;
}



 
 int addMgmtSection( uint8_t **bufPtrPtr, uint32_t maxLength )
 {
	 // no mercy
	 if ((bufPtrPtr == NULL) || (*bufPtrPtr == NULL) || (maxLength <= nfSS_MGMT_SECTION_SIZE))
	 {
		 CRYPTODBG( "wrong input arguments\n");
		 return -1;
	 }
 
	 unsigned int i;
	 uint8_t	 *bufPtr;
	 uint8_t	*sectionLengthPtr;
	 uint16_t	 sectionLength = 0;
 
	 uint8_t	 NtbaROClientId[] = nfSS_CLIENT_ID_TEE_NTBA_RO;
	 uint8_t	 NtbaRWClientId[] = nfSS_CLIENT_ID_TEE_NTBA_RW;
	 uint8_t	 DrmROClientId[] = nfSS_CLIENT_ID_TEE_DRM_RO;
	 uint8_t	 DrmRWClientId[] = nfSS_CLIENT_ID_TEE_DRM_RW;
	 uint8_t	 NrdClientId[] = nfSS_CLIENT_ID_NRD_APP;
 
	 /* **************** NEW WAY *********************
	  * Here's what we need to add:
	  *
	  * TEE MGMT Section ID 				4 bytes
	  * section length						2 bytes
	  * NTBA RO section ID					4 bytes
	  * NTBA RO client ID				   16 bytes
	  * NTBA RW section ID					4 bytes
	  * NTBA RW client ID				   16 bytes
	  * DRM RO section ID				   4 bytes
	  * DRM RO client ID				  16 bytes
	  * DRM RW section ID				   4 bytes
	  * DRM RW client ID				  16 bytes
	  * NRD section ID					   4 bytes
	  * NRD client ID					  16 bytes
	  */
 
	 // get working pointer into buffer; we'll update *bufPtrPtr at the end
	 bufPtr = *bufPtrPtr;
 
	 // add TEE MGMT section ID
	 *bufPtr++ = (teeSID_TEE_MGMT >> 24) & 0xff;
	 *bufPtr++ = (teeSID_TEE_MGMT >> 16) & 0xff;
	 *bufPtr++ = (teeSID_TEE_MGMT >> 8) & 0xff;
	 *bufPtr++ = (teeSID_TEE_MGMT & 0xff);
 
	 // save section length pointer, we'll fill this in shortly
	 sectionLengthPtr = bufPtr;
	 bufPtr += sizeof(uint16_t);
 
	 // add NTBA RO section ID
	 *bufPtr++ = (teeSID_NTBA_RO >> 24) & 0xff;
	 *bufPtr++ = (teeSID_NTBA_RO >> 16) & 0xff;
	 *bufPtr++ = (teeSID_NTBA_RO >> 8) & 0xff;
	 *bufPtr++ = (teeSID_NTBA_RO & 0xff);
 
	 // add NTBA RO Client ID
	 for ( i = 0; i < sizeof(NtbaROClientId); ++i )
		 *bufPtr++ = NtbaROClientId[i];
 
	 // add NTBA RW section ID
	 *bufPtr++ = (teeSID_NTBA_RW >> 24) & 0xff;
	 *bufPtr++ = (teeSID_NTBA_RW >> 16) & 0xff;
	 *bufPtr++ = (teeSID_NTBA_RW >> 8) & 0xff;
	 *bufPtr++ = (teeSID_NTBA_RW & 0xff);
 
	 // add NTBA RW Client ID
	 for ( i = 0; i < sizeof(NtbaRWClientId); ++i )
		 *bufPtr++ = NtbaRWClientId[i];
 
 
	 // add DRM RO section ID
	 *bufPtr++ = (teeSID_DRM_RO >> 24) & 0xff;
	 *bufPtr++ = (teeSID_DRM_RO >> 16) & 0xff;
	 *bufPtr++ = (teeSID_DRM_RO >> 8) & 0xff;
	 *bufPtr++ = (teeSID_DRM_RO & 0xff);
 
	 // add DRM RO Client ID
	 for ( i = 0; i < sizeof(DrmROClientId); ++i )
		 *bufPtr++ = DrmROClientId[i];
 
	 // add DRM RW section ID
	 *bufPtr++ = (teeSID_DRM_RW >> 24) & 0xff;
	 *bufPtr++ = (teeSID_DRM_RW >> 16) & 0xff;
	 *bufPtr++ = (teeSID_DRM_RW >> 8) & 0xff;
	 *bufPtr++ = (teeSID_DRM_RW & 0xff);
 
	 // add DRM RW Client ID
	 for ( i = 0; i < sizeof(DrmRWClientId); ++i )
		 *bufPtr++ = DrmRWClientId[i];
 
	 // add NRD section ID
	 *bufPtr++ = (teeSID_NRD_APP1 >> 24) & 0xff;
	 *bufPtr++ = (teeSID_NRD_APP1 >> 16) & 0xff;
	 *bufPtr++ = (teeSID_NRD_APP1 >> 8) & 0xff;
	 *bufPtr++ = (teeSID_NRD_APP1 & 0xff);
 
	 // add NRD APP Client ID
	 for ( i = 0; i < sizeof(NrdClientId); ++i )
		 *bufPtr++ = NrdClientId[i];
 
	 // update sectionLength
	 sectionLength = (uint16_t) (bufPtr - *bufPtrPtr) - sizeof(teeSectionIDType) - sizeof(uint16_t);
	 CRYPTODBG( "sectionLength is %d\n", sectionLength );
	 *sectionLengthPtr = (sectionLength >> 8) & 0xFF;
	 *(sectionLengthPtr + 1) = sectionLength & 0xFF;
 
	 // adjust *bufPtrPtr before returning
	 *bufPtrPtr = bufPtr;
	 return 0;
 
  }/* end AddMgmtSection */



 int addDrmSection( uint8_t **bufPtrPtr, uint32_t maxLength,
	 							uint8_t* drmZgPrivDataBufPtr,uint32_t drmZgPrivLen,
	 							uint8_t* drmBGroupCertDataBufPtr,uint32_t drmBGroupCertDataLen,
								uint8_t* drmCertTemplateDataBufPtr,uint32_t drmCertTemplateDataLen,
								uint8_t* manufSSDataBufPtr,uint32_t* manufSSdataLenPtr)
{
    // no mercy
    if ((bufPtrPtr == NULL) || (*bufPtrPtr == NULL)
          || (maxLength <= nsMAX_SS_DRM_RO_SIZE)
          || (drmZgPrivDataBufPtr == NULL)
          || (drmCertTemplateDataBufPtr == NULL)
          || (drmBGroupCertDataBufPtr == NULL))
    {
		CRYPTODBG("%s: wrong input arguments\n", __FUNCTION__);
        return -1;
    }

   /*
     * Here's what we need to do:
     *
     *	- add DRM-RO section header
     *  - for file in {devcerttemplate.dat,groupcert.dat,grouppriv.dat} do
     *  	- construct path/filename
     *      - get filesize, validate against remaining space
     * 		- open file
     *  	- add key, length
     *		- read in data from file
     *		- close file
     *  - update total length
     *  - update *bufPtrPtr
     */
     
	uint8_t     *bufPtr;
	uint8_t    *sectionLengthPtr;
	uint16_t    sectionLength = 0;
	/*
	* add section to buffer...
	*/
	bufPtr = *bufPtrPtr;
	// add section ID
	*bufPtr++ = (teeSID_DRM_RO >> 24) & 0xff;
	*bufPtr++ = (teeSID_DRM_RO >> 16) & 0xff;
	*bufPtr++ = (teeSID_DRM_RO >> 8) & 0xff;
	*bufPtr++ = (teeSID_DRM_RO & 0xff);

	// save section length pointer, we'll fill this in shortly
	sectionLengthPtr = bufPtr;
	bufPtr += sizeof(uint16_t);

	// add key, length, and PlayReady model private key
	*bufPtr++ = (drmAGENT_KEY_MODEL_PRIV >> 24) & 0xff;
	*bufPtr++ = (drmAGENT_KEY_MODEL_PRIV >> 16) & 0xff;
	*bufPtr++ = (drmAGENT_KEY_MODEL_PRIV >> 8) & 0xff;
	*bufPtr++ = (drmAGENT_KEY_MODEL_PRIV & 0xff);


	*bufPtr++ = (drmZgPrivLen >> 8) & 0xff;
	*bufPtr++ = (drmZgPrivLen & 0xff);

	// copy
	teeMemcpy(bufPtr,drmZgPrivDataBufPtr,drmZgPrivLen);
	bufPtr += drmZgPrivLen;

	
	// add key, length, and PlayReady model cert
   *bufPtr++ = (drmAGENT_KEY_MODEL_CERT >> 24) & 0xff;
   *bufPtr++ = (drmAGENT_KEY_MODEL_CERT >> 16) & 0xff;
   *bufPtr++ = (drmAGENT_KEY_MODEL_CERT >> 8) & 0xff;
   *bufPtr++ = (drmAGENT_KEY_MODEL_CERT & 0xff);

   *bufPtr++ = (drmBGroupCertDataLen >> 8) & 0xff;
   *bufPtr++ = (drmBGroupCertDataLen & 0xff);

   	// copy
	teeMemcpy(bufPtr,drmBGroupCertDataBufPtr,drmBGroupCertDataLen);
	bufPtr += drmBGroupCertDataLen;


	//add key, length, and PlayReady model cert templeate
    *bufPtr++ = (drmAGENT_KEY_DEVCERT_TEMPLATE >> 24) & 0xff;
    *bufPtr++ = (drmAGENT_KEY_DEVCERT_TEMPLATE >> 16) & 0xff;
    *bufPtr++ = (drmAGENT_KEY_DEVCERT_TEMPLATE >> 8) & 0xff;
    *bufPtr++ = (drmAGENT_KEY_DEVCERT_TEMPLATE & 0xff);


    *bufPtr++ = (drmCertTemplateDataLen >> 8) & 0xff;
    *bufPtr++ = (drmCertTemplateDataLen & 0xff);

	   	// copy
	teeMemcpy(bufPtr,drmCertTemplateDataBufPtr,drmCertTemplateDataLen);
	bufPtr += drmCertTemplateDataLen;

	// update sectionLength
    sectionLength = (uint16_t) (bufPtr - *bufPtrPtr) - sizeof(teeSectionIDType) - sizeof(uint16_t);
   	CRYPTODBG( "sectionLength is %d\n", sectionLength );
    *sectionLengthPtr = (sectionLength >> 8) & 0xFF;
    *(sectionLengthPtr + 1) = sectionLength & 0xFF;

    // if we get here, we need to adjust *bufPtrPtr before returning
    *bufPtrPtr = bufPtr;
    return 0;


 }/* end AddDrmSection */

 int addNtbaSection( uint8_t **bufPtrPtr, uint32_t maxLength, uint8_t* fileDataBufPtr )
{
    // no mercy
    if ((bufPtrPtr == NULL) || (*bufPtrPtr == NULL)
          || (maxLength <= nsMAX_SS_NTBA_RO_SIZE)
          || (fileDataBufPtr == NULL))
    {
		CRYPTODBG("%s: wrong input arguments\n", __FUNCTION__);

        return -1;
    }

    int			esnLength;
    char 		esn[ESN_MAX + 1];
    char 		kpe[CIPHER_BLOCK_SIZE];
    char 		kph[HMAC_BLOCK_SIZE];
// sizes of b64-encoded keys
#define B64_KPE_MAX 24
#define B64_KPH_MAX 44
    uint8_t     *bufPtr;
    uint8_t    *sectionLengthPtr;
    uint16_t    sectionLength = 0;

			teeBzero(esn,ESN_MAX+1);
			teeBzero(kpe,CIPHER_BLOCK_SIZE);
			teeBzero(kph,HMAC_BLOCK_SIZE);
    /*
     * Here's what we need to do:
     *
     *  - construct idfile path/filename
     * 	- open idfile
     *  - read in esn, kpe, kph
     *  - ADD NTBA-RO section to *bufPtrPtr
     */

	uint16_t skipCount = 0;

	//first read in the esn , read ESN_MAX bytes

	char* readBufferPtr = (char*)fileDataBufPtr;
	char* tempPtr = readBufferPtr;


	while(tempPtr && *tempPtr != '\0')
	{
		if(*tempPtr == 0x0d || *tempPtr == 0x0a)
		{	
			skipCount ++ ;
			if(*(tempPtr+1) == 0x0a || *(tempPtr+1) == 0x0d)
				skipCount ++;
			teeBzero(esn,ESN_MAX+1);
			teeMemcpy(esn,readBufferPtr,tempPtr-readBufferPtr);	
			esnLength = tempPtr-readBufferPtr;	
			//esn[tempPtr-readBufferPtr] = '\0';
			break;
		}
		tempPtr ++ ;
	}

	readBufferPtr = tempPtr + skipCount;	
	tempPtr = readBufferPtr;
	skipCount = 0;
	
	while(tempPtr && *tempPtr != '\0')
	{
		if(*tempPtr == 0x0a || *tempPtr == 0x0d)
		{
			skipCount ++ ;
			*tempPtr = '\0';
			if(*(tempPtr+1) == 0x0a || *(tempPtr+1) == 0x0d)
			{
				skipCount ++ ;
				*(tempPtr+1) = '\0';
			}
			break;
		}
		tempPtr ++ ;
	}
	
  if (readBufferPtr == NULL)
  	return -1;
  	
	teeBzero(kpe, sizeof(kpe));
	
#ifndef NFLX_ULI_SUPPORT
	if ( b64_decode( readBufferPtr, kpe ) != 0 )
    {
       CRYPTODBG( "kpe decode failed, exiting.\n" );
        return -1;
    }
#else
	 teeMemcpy(kpe,readBufferPtr,CIPHER_BLOCK_SIZE);
#endif

	teePrintHexDump_tag((char*)"kpe",(uint8_t*)kpe,sizeof(kpe));	
	readBufferPtr = tempPtr + skipCount;
	skipCount = 0;
   //save decode kph
   teeBzero(kph, sizeof(kph));
   
#ifndef NFLX_ULI_SUPPORT
	if ( b64_decode( readBufferPtr, kph ) != 0 )
   {
	  CRYPTODBG( "kph decode failed, exiting.\n" );
	   return -1;
   }
#else
   teeMemcpy(kph,readBufferPtr,HMAC_BLOCK_SIZE);
#endif

   teePrintHexDump_tag((char*)"kph",(uint8_t*)kph,sizeof(kph));

   /*
     * add section to buffer...
     */
    bufPtr = *bufPtrPtr;

    // add section ID
    *bufPtr++ = (teeSID_NTBA_RO >> 24) & 0xff;
    *bufPtr++ = (teeSID_NTBA_RO >> 16) & 0xff;
    *bufPtr++ = (teeSID_NTBA_RO >> 8) & 0xff;
    *bufPtr++ = (teeSID_NTBA_RO & 0xff);

    // save section length pointer, we'll fill this in shortly
    sectionLengthPtr = bufPtr;
    bufPtr += sizeof(uint16_t);

    // add esn key, length, and esn
    *bufPtr++ = (ntbaAGENT_KEY_ESN >> 24) & 0xff;
    *bufPtr++ = (ntbaAGENT_KEY_ESN >> 16) & 0xff;
    *bufPtr++ = (ntbaAGENT_KEY_ESN >> 8) & 0xff;
    *bufPtr++ = (ntbaAGENT_KEY_ESN & 0xff);

    *bufPtr++ = (esnLength >> 8) & 0xff;
    *bufPtr++ = (esnLength & 0xff);
		
		
    teePrintHexDump_tag((char*)"ESN",(uint8_t*)esn,esnLength);
    //printf("offset:%d",bufPtr-*bufPtrPtr);
    (void) teeMemcpy( bufPtr, esn, esnLength );
    bufPtr += esnLength;
    

    // add kpe key, length, and kpe
    *bufPtr++ = (ntbaAGENT_KEY_KPE >> 24) & 0xff;
    *bufPtr++ = (ntbaAGENT_KEY_KPE >> 16) & 0xff;
    *bufPtr++ = (ntbaAGENT_KEY_KPE >> 8) & 0xff;
    *bufPtr++ = (ntbaAGENT_KEY_KPE & 0xff);

    // length of KPE is always the same
    *bufPtr++ = (sizeof(kpe) >> 8) & 0xff; // == 0
    *bufPtr++ = sizeof(kpe);
		//printf("offset:%d",bufPtr-*bufPtrPtr);
    (void) teeMemcpy( bufPtr, kpe, sizeof(kpe) );
    bufPtr += sizeof(kpe);

    // add kph key, length, and kph
    *bufPtr++ = (ntbaAGENT_KEY_KPH >> 24) & 0xff;
    *bufPtr++ = (ntbaAGENT_KEY_KPH >> 16) & 0xff;
    *bufPtr++ = (ntbaAGENT_KEY_KPH >> 8) & 0xff;
    *bufPtr++ = (ntbaAGENT_KEY_KPH & 0xff);

    // length of KPH is always the same
    *bufPtr++ = (sizeof(kph) >> 8) & 0xff; // == 0
    *bufPtr++ = sizeof(kph);
		
		//printf("offset:%d",bufPtr-*bufPtrPtr);
    (void) teeMemcpy( bufPtr, kph, sizeof(kph) );
    bufPtr += sizeof(kph);

    // update sectionLength
    sectionLength = (uint16_t) (bufPtr - *bufPtrPtr) - sizeof(teeSectionIDType) - sizeof(uint16_t);
   	CRYPTODBG( "sectionLength is %d\n", sectionLength );
    *sectionLengthPtr = (sectionLength >> 8) & 0xFF;
    *(sectionLengthPtr + 1) = sectionLength & 0xFF;

    // if we get here, we need to adjust *bufPtrPtr before returning
    *bufPtrPtr = bufPtr;
    
    //teePrintHexDump_tag("ESN222",esn,ESN_MAX);
	 
    return 0;
}



void decodeblock( unsigned char *in, unsigned char *out )
{
    out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
    out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
    out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);
}

int b64_decode( char *inPtr, char *outPtr )
{
    int retcode = 0;
    unsigned char in[4];
    unsigned char out[3];
    int v;
    int i, len;

    *in = (unsigned char) 0;
    *out = (unsigned char) 0;
    while( *inPtr != '\0' )
    {
        for( len = 0, i = 0; i < 4 && *inPtr != '\0'; i++ ) {
            v = 0;
            while ( *inPtr && v == 0 )
            {
                v = *inPtr++;
                v = ((v < 43 || v > 122) ? 0 : (int) cd64[ v - 43 ]);
                if( v != 0 )
                {
                    v = ((v == (int)'$') ? 0 : v - 61);
                }
            }/* end while ( *inPtr && v == 0 ) */
            if ( *inPtr )
            {
                len++;
                if( v != 0 )
                {
                    in[ i ] = (unsigned char) (v - 1);
                }
            }
            else
            {
                in[i] = (unsigned char) 0;
            }
        }
        if ( len > 0 )
        {
            decodeblock( in, out );
            for( i = 0; i < len - 1; i++ )
            {
                *outPtr++ = out[i];
            }
        }/* end if ( len > 0 ) */

    }/* end while( *inPtr != '\0' ) */
    return( retcode );
}

void trimNewLine(char** strPtr)
{
    char* str = *strPtr;
    int len = 0;

    if (str != NULL)
    {
    		len = x_strlen(str);
        if (str[len - 1] == '\r')	// ends in \r
            str[len - 1] = '\0';
        else if ((str[len - 1] == '\n') && (str[len - 2] == '\r')) // ends in /r/n
            str[len - 2] = '\0';
        else if (str[len - 1] == '\n') // ends in \n
            str[len - 1] = '\0';
    }
}
