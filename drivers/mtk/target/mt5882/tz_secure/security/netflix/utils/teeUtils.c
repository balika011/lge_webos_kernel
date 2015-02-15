/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "CommonIncludes.h"

#include "DebugTypes.h"
#include "TeeDebug.h"

#define nfSS_MAX_FILE_SIZE	8092

// internal buffer for holding manuf secure store in memory
//static uint8_t	_saMemBuf[nfSS_MAX_FILE_SIZE];

#define nfDEBUG_MAX_OUTPUT_SIZE	256
#if TEE_DEBUG_ENABLE
static uint8_t	_dbgMemBuf[nfDEBUG_MAX_OUTPUT_SIZE];
#endif

/***************************************************************************
 * @function teeDebugPrint
 *
 * @brief formatted debug print, like printf
 *
 * @param[in] FormatStrPtr - pointer to format string
 * @param[in] va_list - list of format arguments (like printf)
 *
 * @return   void
 *
 **************************************************************************/
void teeDebugPrint( const char *FormatStrPtr, ...)
{
	#if 0
	va_list ArgList;

	va_start(ArgList, FormatStrPtr);

	vsnPrintf((char *) _dbgMemBuf, sizeof(_dbgMemBuf), FormatStrPtr, ArgList);

	va_end(ArgList);

	// now, output
	//Printf("%s", _dbgMemBuf);	
	Printf( " %s", _dbgMemBuf);
	#endif
	return;
}/* end teeDebugPrint */

/***************************************************************************
 * @function teePrintHexDump
 *
 * @brief formatted hex dump to stdout
 *
 * @param[in] BufferPtr - pointer to buffer
 * @param[in] BufferLength - length of buffer
 *
 * @return   void
 *
 **************************************************************************/
void teePrintHexDump( uint8_t *BufferPtr, uint32_t BufferLength )
{
		#if TEE_DEBUG_ENABLE
    uint32_t    i;
    uint32_t    j;
#define BYTES_PER_LINE 16 // bytes dumped per line
    char        DisplayedBytes[BYTES_PER_LINE + 1]; // output line
    uint16_t    Offset = 0;
    uint32_t    OffsetMax = (1 << (sizeof(Offset) * 8)) - 1;

    // null-terminate output line
    DisplayedBytes[BYTES_PER_LINE] = '\0';

    // limit buffer size (offset is uint16_t)
    if ( BufferLength > OffsetMax )
        BufferLength = OffsetMax;

    // dump it. dump it all.
    while ( BufferLength > 0 )
    {
        // print data offset
        Printf( "%04x: ", Offset );
        Offset += BYTES_PER_LINE;

        // print BYTES_PER_LINE hex bytes
        for ( i = 0; i < BYTES_PER_LINE; ++i )
        {
            // if out of data, print ## instead...
            if ( i == BufferLength )
            {
                /* partial line - print ## for rest of line */
                for ( j = BYTES_PER_LINE - 1; j >= BufferLength; --j )
                {
                    Printf( "## ");
                    DisplayedBytes[j] = '#';
                }

               /* print DisplayedBytes + newline */
                Printf(  " >%s<\n", DisplayedBytes );
                return;
            }

            // print current byte in hex
           Printf(  "%02x ", BufferPtr[i] );

            // save display byte value until eol
            //if ( isprint( BufferPtr[i] ) )
               // DisplayedBytes[i] = BufferPtr[i];
            //else
              // DisplayedBytes[i] = '.';

        } /* end for ( i = 0; i < BYTES_PER_LINE; ++i ) */

        /* print display byte values + newline */
        Printf(  " >%s<\n", DisplayedBytes );

        /* update pointer and buffer length */
        BufferPtr += BYTES_PER_LINE;
        BufferLength -= BYTES_PER_LINE;

    }/* end while */
	#endif
} /* end teePrintHexDump */

void teePrintHexDump_tag(char* name ,uint8_t *BufferPtr, uint32_t BufferLength )
{
	#if TEE_DEBUG_ENABLE
    uint32_t    i;
    uint32_t    j;
#define BYTES_PER_LINE 16 // bytes dumped per line
    char        DisplayedBytes[BYTES_PER_LINE + 1]; // output line
    uint16_t    Offset = 0;
    uint32_t    OffsetMax = (1 << (sizeof(Offset) * 8)) - 1;

	Printf( "%s : %d\n", name, BufferLength);

    // null-terminate output line
    DisplayedBytes[BYTES_PER_LINE] = '\0';

    // limit buffer size (offset is uint16_t)
    if ( BufferLength > OffsetMax )
        BufferLength = OffsetMax;

    // dump it. dump it all.
    while ( BufferLength > 0 )
    {
        // print data offset
        Printf(  "%04x: ", Offset );
        Offset += BYTES_PER_LINE;

        // print BYTES_PER_LINE hex bytes
        for ( i = 0; i < BYTES_PER_LINE; ++i )
        {
            // if out of data, print ## instead...
            if ( i == BufferLength )
            {
                /* partial line - print ## for rest of line */
                for ( j = BYTES_PER_LINE - 1; j >= BufferLength; --j )
                {
                    Printf( "## ");
                    DisplayedBytes[j] = '#';
                }

               /* print DisplayedBytes + newline */
                Printf(  " >%s<\n", DisplayedBytes );
                return;
            }

            // print current byte in hex
            Printf(  "%02x ", BufferPtr[i] );

            // save display byte value until eol
           // if ( isprint( BufferPtr[i] ) )
               // DisplayedBytes[i] = BufferPtr[i];
            //else
            //   DisplayedBytes[i] = '.';

        } /* end for ( i = 0; i < BYTES_PER_LINE; ++i ) */

        /* print display byte values + newline */
        Printf(  " >%s<\n", DisplayedBytes );

        /* update pointer and buffer length */
        BufferPtr += BYTES_PER_LINE;
        BufferLength -= BYTES_PER_LINE;

    }/* end while */
	#endif
} /* end teePrintHexDump */


/*
* 
*/
typedef unsigned short int uint16;  
typedef unsigned long int uint32;  


#define BigLittleSwap16(A)        ((((uint16)(A) & 0xff00) >> 8) | \
                                                   (((uint16)(A) & 0x00ff) << 8))

#define BigLittleSwap32(A)        ((((uint32)(A) & 0xff000000) >> 24) | \
                                                   (((uint32)(A) & 0x00ff0000) >> 8) | \
                                                   (((uint32)(A) & 0x0000ff00) << 8) | \
                                                   (((uint32)(A) & 0x000000ff) << 24))

int checkCPUendian()
{
    union{
            unsigned long int i;
            unsigned char s[4];
    }c = {0x12345678};

    return (0x12 == c.s[0]);
}

#define HtoNl(A)        (checkCPUendian() ? (A) : BigLittleSwap32(A))
#define NtoHl(A)        HtoNl(A)
#define HtoNs(A)        (checkCPUendian() ? (A) : BigLittleSwap16(A))
#define NtoHs(A)        HtoNs(A)



// ENDIANESS functions
/***************************************************************************
 * @function teeNtohl
 *
 * @brief non-libc ntohl
 *
 * @param[in] NetLong - long in network byte order
 *
 * @returns   uint32_t size
 *
 **************************************************************************/
uint32_t teeNtohl(const uint32_t NetLong)
{
	//return (ntohl(NetLong));
	return NtoHl(NetLong);

}/* end teeNtohl */

/***************************************************************************
 * @function teeHtonl
 *
 * @brief non-libc htonl
 *
 * @param[in] HostLong - long in host byte order
 *
 * @returns   value in host order
 *
 **************************************************************************/
uint32_t teeHtonl(const uint32_t HostLong)
{
	//return (htonl(HostLong));
	return HtoNl(HostLong);
}/* end teeHtonl */

/***************************************************************************
 * @function teeNtohs
 *
 * @brief non-libc ntohs
 *
 * @param[in] NetShort - short in network byte order
 *
 * @returns   value in network byte order
 *
 **************************************************************************/
uint16_t teeNtohs(const uint16_t NetShort)
{
	//return (ntohs(NetShort));
	return NtoHs(NetShort);

}/* end teeNtohs */

/***************************************************************************
 * @function teeHtons
 *
 * @brief non-libc htons
 *
 * @param[in] HostShort - short in host byte order
 *
 * @returns   value in host order
 *
 **************************************************************************/
uint16_t teeHtons(const uint32_t HostShort)
{
	//return (htons(HostShort));
	return HtoNs(HostShort);
}/* end teeHtons */

/***************************************************************************
 * @function teeMemcmp
 *
 * @brief non-libc memcmp
 *
 * @param[in] Ptr1 - pointer to first location
 * @param[in] Ptr2 - pointer to second location
 * @param[in] Length - length to compare
 *
 * @return   -1 if *Ptr1 < *Ptr2, 0 if equal, 1 if *Ptr1 > *Ptr2
 *
 **************************************************************************/
int teeMemcmp( const void *Ptr1, const void *Ptr2, uint32_t Length )
{
	// NRD prototype has libc...
	return x_memcmp( Ptr1, Ptr2, (size_t)Length );
}/* end teeMemcmp */


/***************************************************************************
 * @function teeMemcpy
 *
 * @brief non-libc memcpy
 *
 * @param[in] DstPtr - pointer to destination
 * @param[in] SrcPtr - pointer to source
 * @param[in] Length - length to copy
 *
 * @returns   DstPtr
 *
 **************************************d************************************/
void *teeMemcpy( void *DstPtr, const void *SrcPtr, uint32_t Length )
{
	// NRD prototype has libc...
	return x_memcpy( DstPtr, SrcPtr, (size_t)Length );

}/* end teeMemcpy */

/***************************************************************************
 * @function teeBzero
 *
 * @brief non-libc bzero
 *
 * @param[in] DstPtr - pointer to destination
 * @param[in] Length - length to zeroize
 *
 * @returns  void
 *
 **************************************************************************/
void teeBzero( void *DstPtr, uint32_t Length )
{
	// NRD prototype has libc...
	x_memset(DstPtr,0,Length);

}/* end teeBzero */


/***************************************************************************
 * @function teeMemset
 *
 * @brief non-libc memset
 *
 * @param[in] DstPtr - pointer to destination
 * @param[in] Value - value to set
 * @param[in] Length - length
 *
 * @returns   DstPtr
 *
 **************************************************************************/
void *teeMemset( void *DstPtr, int Value, uint32_t Length )
{
	// NRD prototype has libc...
	return x_memset( DstPtr, Value, (size_t)Length );

}/* end teeMemset */

/***************************************************************************
 * @function teeMemAlloc
 *
 * @brief non-libc malloc
 *
 * @param[in] Size - number of bytes to allocate
 *
 * @returns   pointer to allocated memory if successful, NULL otherwise
 *
 **************************************************************************/
uint8_t *teeMemAlloc( uint32_t Size )
{
	// Just use malloc for now...
	return (uint8_t *) x_alloc_s_dram_mem( Size );

}/* end teeMemAlloc */

/***************************************************************************
 * @function teeMemFree
 *
 * @brief non-libc free
 *
 * @param[in] MemPtrPtr - pointer to pointer to memory you want freed
 *
 * @returns   void
 *
 **************************************************************************/
void teeMemFree( uint8_t **MemPtrPtr )
{
	// log error and return
	if ( ( MemPtrPtr == NULL ) || ( *MemPtrPtr == NULL ) )
	{
		MEMDBG("%s: invalid pointer, Ptr: %p, *Ptr: %p)\n", __FUNCTION__, MemPtrPtr,
				MemPtrPtr ? *MemPtrPtr : NULL );
		return;
	}

	// Just use free for now...
	(void) x_free_s_dram_mem( *MemPtrPtr );

	// clear pointer
	*MemPtrPtr = NULL;

	return;

}/* end teeMemAlloc */

/***************************************************************************
 * @function teeMemRealloc
 *
 * @brief non-libc realloc
 *
 * @param[in] ptr  - pointer which needs reallocate
 * @param[in] size - number of bytes to allocate
 *
 * @returns   pointer to allocated memory if successful, NULL otherwise
 *
 **************************************************************************/
uint8_t *teeMemRealloc( uint8_t * ptr, uint32_t size )
{
	return ((uint8_t*) x_realloc_s_dram_mem (ptr, size));
}/* end teeMemRealloc */

/***************************************************************************
 * @function teeStrlen
 *
 * @brief non-libc strlen
 *
 * @param[in] str - pointer to string
 *
 * @returns   uint32_t size
 *
 **************************************************************************/
int teeStrlen (const char* str)
{
	return ((int) x_strlen(str));
}/* end teeStrlen */

