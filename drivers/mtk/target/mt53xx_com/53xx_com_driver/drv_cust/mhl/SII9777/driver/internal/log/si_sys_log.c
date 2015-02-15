/******************************************************************************
 *
 * Copyright 2013, Silicon Image, Inc.  All rights reserved.
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of
 * Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
 *
 *****************************************************************************/
/**
 * @file si_sys_obj.c
 *
 * @brief Object creater
 *
 *****************************************************************************/

/***** #include statements ***************************************************/

#include "si_datatypes.h"
#include "si_sys_log_api.h"
#include "si_sys_obj_api.h"
#include "si_sys_time_api.h"
#include "sii9777_platform_api.h"

/***** Register Module name **************************************************/

SII_MODULE_NAME_SET(LibLog);

/***** local macro definitions ***********************************************/

#define LOG_LINE_WRAP              (80)
#define LOG_LENGTH_LHS_MAX         (34)
#define LOG_LENGTH_CLASS_NAME      ((LOG_LENGTH_LHS_MAX-2)/3)
#define LOG_LENGTH_OBJECT_NAME     ((LOG_LENGTH_LHS_MAX-2)/3)

/***** local type definitions ************************************************/

/***** local prototypes ******************************************************/

static void sPutChar( char character );
static uint8_t sPutString( const char* pStr );
static uint8_t sLogLimited( uint8_t size, const char* pStr );
static void sLogPutString( char* pStr );

/***** local data objects ****************************************************/

static uint16_t sLinePos = 0;

/***** public functions ******************************************************/

void SiiSysLogTimeStamp(const char* pClassStr, const char* pFuncStr, void* pObj)
{
    if( pFuncStr )
    {
        SiiSysTimeMilli_t mSec  = (SiiSysTimeMilli_t)SiiPlatformTimeMilliGet();
        uint16_t          tot   = 0;
        uint16_t          tsLen = 0;

        /* Print time stamp */
        {
            char str[40];

            sPutChar('\n');
            SII_SPRINTF(str, "%ld", mSec/1000);
            tsLen += sPutString(str);
            sPutChar('.');
            SII_SPRINTF(str, "%03ld", mSec%1000);
            tsLen += sPutString(str);
            sPutChar('-');
            tsLen += 3;
        }

        /* Print module name */
        tot += sLogLimited(LOG_LENGTH_CLASS_NAME, pClassStr);

        /* If instance print instance name */
        if( pObj )
        {
            /* Separation character */
            sPutChar('.');
            tot++;

            tot += sLogLimited(LOG_LENGTH_OBJECT_NAME, SiiSysObjNameGet(pObj));
        }

        if( *pFuncStr )
        {
            /* Separation character */
            sPutChar('.');
            tot++;

            tot += sLogLimited(LOG_LENGTH_LHS_MAX - tot, pFuncStr);
        }

        /* Print alignment space characters */
        tot = (LOG_LENGTH_LHS_MAX < tot) ? (0) : (LOG_LENGTH_LHS_MAX - tot);
        while( tot-- )
            sPutChar(' ');

        /* Print end of preamble */
        sLinePos  = tsLen + LOG_LENGTH_LHS_MAX - 1; /* - 1 to compensate for the newline counted with the timestamp */
        sLinePos += sPutString(": ");
    }
}

void SiiSysLogPrintf( char* pFrm, ...)
{
    va_list  arg;
    uint16_t chars;
    char     str[160]; // CEC_LOGGER requires 160

    va_start(arg, pFrm);
    chars = SII_VSPRINTF(str, pFrm, arg);
    va_end(arg);
    SII_ASSERT(((int)sizeof(str))>chars);

    sLogPutString(str);
}

/***** local functions *******************************************************/

static uint8_t sPutString( const char* pStr )
{
    uint8_t len = SII_STRLEN(pStr);

     SiiPlatformLogPutString(pStr);
    return len;
}

static void sPutChar( char character )
{
    char str[2];

    str[0] = character;
    str[1] = 0;
     SiiPlatformLogPutString(str);
}

static uint8_t sLogLimited( uint8_t size, const char* pStr )
{
    uint8_t i = 0;
	uint8_t len;

	len = strlen(pStr);
	/* does string length exceed size limitation? */
	if (len > size)
	{
		/* truncate left of string */
        sPutChar('*');
		pStr += (len - size + 1);
		i++;
	}

    i += sPutString(pStr);
    
    return i;
}

static void sLogPutString( char* pStr )
{
    uint8_t lastSpace = 0;
    uint8_t index = 0;
    
    while( pStr[index] )
    {
        if( sLinePos == 0 )
        {
            sLinePos = sPutString("\n    ");;
        }

        if( pStr[index] == ' ' )
        {
            lastSpace = index;
        }
        
        if( pStr[index] == '\n' )
        {
            pStr[index] = 0x00;
            sPutString(pStr);
            sLinePos = 0;

            pStr += (index + 1);
            lastSpace = index = 0;
        }
        else
        {
            index++;
        }

        if( (sLinePos + index) >= LOG_LINE_WRAP )
        {
            if (lastSpace != 0)
            {
                pStr[lastSpace] = 0x00;
                sPutString(pStr);
                pStr += (lastSpace + 1);
            }

            sLinePos = sPutString("\n    ");
            lastSpace = index = 0;
        }
	}
    
    if( index != 0 )
    {
        sLinePos += sPutString(pStr);
    }
}

/***** end of file ***********************************************************/
