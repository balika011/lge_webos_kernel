/*****************************************************************************/
/* (c) Copyright Ocean Blue Software Ltd. - ALL RIGHTS RESERVED              */
/* This document is the CONFIDENTIAL property of Ocean Blue Software Ltd.    */
/*****************************************************************************/
/*                                                                           */
/* File Name:     ccactx.c                                                   */
/*                                                                           */
/* Description:   CI+ Content Control Resource: Authentication contexts      */
/*                                                                           */
/* Project:       CI / CI+ Library                                           */
/*                                                                           */
/* Creation Date: 27/01/2009                                                 */
/*                                                                           */
/*****************************************************************************/

/* --- includes for this file --- */
/* Ocean Blue header files */
#include "ccdef.h"
#include "stbcihw.h"
#include "ccactx.h"
#include "ccfunc.h"
#include "ccmsg.h"
#include "ccsac.h"
#include "cccert.h"
#include "sectk.h"
#include "cclru.h"
#include "ci_trust_zone.h"
/* --- constant definitions for this file --- */
#define CC_DEBUG_PRINT(x) CC_PRINT(x)
#define CC_DEBUG_BUFFER(x,y) CC_PRINT_BUFFER(x,y)
#define FUNCTION_START(x)
#define FUNCTION_FINISH(x)

#define VALID_OFFSET                (0)
#define VALID_SIZE                  (1)
#define DHSK_OFFSET                 ((VALID_OFFSET) + (VALID_SIZE))
#define DHSK_SIZE                   (BITS_TO_BYTES(DT_SIZE_DHSK))
#define AKH_OFFSET                  ((DHSK_OFFSET) + (DHSK_SIZE))
#define AKH_SIZE                    (BITS_TO_BYTES(DT_SIZE_AKH))
#define CICAM_ID_OFFSET             ((AKH_OFFSET) + (AKH_SIZE))
#define CICAM_ID_SIZE               (BITS_TO_BYTES(DT_SIZE_CICAM_ID))
#define CICAM_BRAND_ID_OFFSET       ((CICAM_ID_OFFSET) + (CICAM_ID_SIZE))
#define CICAM_BRAND_ID_SIZE         (2)
#define CIPHER_OFFSET               ((CICAM_BRAND_ID_OFFSET) + \
                                     (CICAM_BRAND_ID_SIZE))
#define CIPHER_SIZE                 (1)
#define TIMESTAMP_OFFSET            ((CIPHER_OFFSET) + (CIPHER_SIZE))
#define TIMESTAMP_SIZE              (1)
#define LAST_OFFSET                 ((TIMESTAMP_OFFSET) + (TIMESTAMP_SIZE))

#define BUFFER_SIZE                 (LAST_OFFSET)
#define DATA_SIZE                   ((BUFFER_SIZE) * (CC_NUM_AUTH_CONTEXTS))

#define CHECKSUM_OFFSET             (DATA_SIZE)
#define CHECKSUM_SIZE               (32)

#define TOTAL_SIZE                  ((DATA_SIZE) + (CHECKSUM_SIZE))

/* --- local typedef structs for this file --- */

/* --- local (static) variable declarations for this file --- */
/* (internal variables declared static to make them local) */

static U8BIT context_data[TOTAL_SIZE];
static S_CC_LRU lru;

/* --- local function prototypes for this file --- */
/* (internal functions declared static to make them local) */

static BOOL ValidateContextData(void);
static void ResetContextData(void);
static BOOL RecalculateChecksum(void);
static U8BIT *GetContextBuffer(U8BIT idx);
static BOOL IsValidContext(U8BIT *buffer);
static void CopyInvalidAuthContext(CI_CC_INFO *cc);
static void SetContext(void *data, U8BIT idx, void *context);
static U8BIT GetTimestamp(void *data, U8BIT idx);
static void SetTimestamp(void *data, U8BIT idx, U8BIT timestamp);
static BOOL Equals(void *data, void *context, U8BIT idx);
static void ReadContextFromBuffer(CI_CC_INFO *cc,
                                  U8BIT *buffer);
static void WriteContextToBuffer(CI_CC_INFO *cc,
                                 U8BIT *buffer);

/*---------------------------------------------------------------------------*/
/* global function definitions                                               */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
void CC_Set_AuthContexts(U8BIT* contextdata,U32BIT data_len)
{
  if(contextdata == NULL || data_len<TOTAL_SIZE)
  {
    CC_DEBUG_PRINT(("CC_Set_AuthContexts() invalid argument! \n"));
    return;
  }
  memcpy_s2s_chk(context_data,contextdata,TOTAL_SIZE,NULL);
}
void CC_Get_AuthContexts(U8BIT* contextdata,U32BIT data_len)
{
  if(contextdata == NULL || data_len<TOTAL_SIZE)
  {
    CC_DEBUG_PRINT(("CC_Set_AuthContexts() invalid argument! \n"));
    return;
  }
  memcpy_s2s_chk(contextdata,context_data,TOTAL_SIZE,NULL);
}

void STB_CIResetAuthKey(void)
{    
    ResetContextData();        
   // STB_CIWriteSecureNVM(context_data,TOTAL_SIZE);        
    CC_DEBUG_PRINT(("***********Reset auth key***********\n"));    
}

/*!**************************************************************************
 * @brief    Read authentication contexts
 ****************************************************************************/
void CC_ReadAuthContexts(void)
{
   BOOL success = TRUE;

   FUNCTION_START(CC_ReadAuthContexts);

  // success = STB_CIReadSecureNVM(context_data, TOTAL_SIZE);
   if (success)
   {
      success = ValidateContextData();
   }
   if (success == FALSE)
   {
      /* Make sure nothing is left in the buffer */
      ResetContextData();
   }

   lru.data = context_data;
   lru.context_count = CC_NUM_AUTH_CONTEXTS;
   lru.set_context = SetContext;
   lru.get_timestamp = GetTimestamp;
   lru.set_timestamp = SetTimestamp;
   lru.equals = Equals;

   FUNCTION_FINISH(CC_ReadAuthContexts);
}

/*!**************************************************************************
 * @brief    Reset authentication context iterator
 * @param session_info - content control session information
 ****************************************************************************/
void CC_ResetAuthContextIter(CI_CC_INFO *cc)
{
   FUNCTION_START(CC_ResetAuthContextIter);

   cc->auth_context = 0;

   FUNCTION_FINISH(CC_ResetAuthContextIter);
}

/*!**************************************************************************
 * @brief    Return next authentication context
 *
 * This function should be called after authentication contexts have been
 * read and after a call to CC_ResetAuthContextIter.
 *
 * When there are no more valid contexts, this function returns invalid (i.e.
 * all-zeros) data. This is in accordance with the CI+ specification.
 *
 * @param session_info - content control session information
 *
 ****************************************************************************/
void CC_CopyNextAuthContext(CI_CC_INFO *cc)
{
   BOOL valid;
   U8BIT *buffer;
   BOOL search;

   FUNCTION_START(CC_CopyNextAuthContext);

   search = TRUE;
   while (search && cc->auth_context < CC_NUM_AUTH_CONTEXTS)
   {
      buffer = GetContextBuffer(cc->auth_context);
      if(buffer != NULL)
      {
         valid = IsValidContext(buffer);
         if (valid)
         {
               /* Found a candidate */
               ReadContextFromBuffer(cc, buffer);
               search = FALSE;
         }
   
         ++cc->auth_context;
      }
   }

   if (search)
   {
      /* Still searching, but no more candidates: use invalid data */
      
      CC_DEBUG_PRINT(("CC_CopyNextAuthContext: use invaid\n"));
      CopyInvalidAuthContext(cc);
   }

   FUNCTION_FINISH(CC_CopyNextAuthContext);
}

/*!**************************************************************************
 * @brief    Update authentication context
 * @param    session_info - content control session information
 ****************************************************************************/
void CC_UpdateAuthContext(CI_CC_INFO *cc)
{
   BOOL dirty_flag;
   //BOOL retval;

   FUNCTION_START(CC_UpdateAuthContext);


   CC_UpdateLruContexts(&lru, cc);

   dirty_flag = RecalculateChecksum();
   if (dirty_flag)
   {
       CC_DEBUG_PRINT(("CC_UpdateLruContexts: update Checksum \n"));
   }

   CC_DEBUG_PRINT(("CC_UpdateLruContexts: only update to buffer\n"));
   return TRUE;
   FUNCTION_FINISH(CC_UpdateAuthContext);
}

/*---------------------------------------------------------------------------*/
/* local function definitions                                                */
/*---------------------------------------------------------------------------*/

/*!**************************************************************************
 * @brief    Validate context data
 *
 * Context data should contain a checksum (SHA-256) at the end of it. This
 * function recalculates the checksum and compares it to the stored checksum.
 *
 * @return   TRUE if context data is valid, FALSE otherwise
 ****************************************************************************/
static BOOL ValidateContextData(void)
{
   U8BIT digest[32];
   BOOL valid;

   FUNCTION_START(ValidateContextData);

   valid = FALSE;
   SECTK_Sha256(context_data, DATA_SIZE, digest);
   if (memcmp(digest, context_data + CHECKSUM_OFFSET, CHECKSUM_SIZE) == 0)
   {
      valid = TRUE;
   }

   CC_DEBUG_PRINT(("check digest:\n"));
   CC_DEBUG_BUFFER(digest, CHECKSUM_SIZE);

   CC_DEBUG_PRINT(("check context_data:\n"));
   CC_DEBUG_BUFFER((context_data+CHECKSUM_OFFSET), CHECKSUM_SIZE);

   CC_DEBUG_PRINT(("valid : %d\n", valid));  
   
   FUNCTION_FINISH(ValidateContextData);

   return valid;
}

/*!**************************************************************************
 * @brief    Reset context data
 ****************************************************************************/
static void ResetContextData(void)
{
   FUNCTION_START(ResetContextData);

   memset(context_data, 0, DATA_SIZE);
   (void)RecalculateChecksum();

   FUNCTION_FINISH(ResetContextData);
}

/*!**************************************************************************
 * @brief    Recalculate checksum for context data
 * @return   TRUE if new checksum is different to the stored checksum
 ****************************************************************************/
static BOOL RecalculateChecksum(void)
{
   U8BIT digest[32];
   BOOL dirty_flag;

   FUNCTION_START(RecalculateChecksum);

   SECTK_Sha256(context_data, DATA_SIZE, digest);
   dirty_flag = TRUE;
   if (memcmp(context_data + CHECKSUM_OFFSET, digest, CHECKSUM_SIZE) == 0)
   {
      dirty_flag = FALSE;
   }
   else
   {
      memcpy_s2s_chk(context_data + CHECKSUM_OFFSET, digest, CHECKSUM_SIZE,NULL);
   }

   CC_DEBUG_PRINT(("Recalculate digest:\n"));
   CC_DEBUG_BUFFER(digest, CHECKSUM_SIZE);

   CC_DEBUG_PRINT(("Recalculate context_data:\n"));
   CC_DEBUG_BUFFER((context_data+CHECKSUM_OFFSET), CHECKSUM_SIZE);

   CC_DEBUG_PRINT(("dirty_flag : %d\n", dirty_flag)); 
   
   FUNCTION_FINISH(RecalculateChecksum);

   return dirty_flag;
}

/*!**************************************************************************
 * @brief    Return context buffer given index
 * @return   Context buffer
 ****************************************************************************/
static U8BIT *GetContextBuffer(U8BIT index)
{
   U8BIT *buffer = NULL;

   FUNCTION_START(GetContext);

   /* fix klocwork */
   if ((index * BUFFER_SIZE) < TOTAL_SIZE)
   {   
      buffer = context_data + index * BUFFER_SIZE;
   }

   FUNCTION_FINISH(GetContext);

   return buffer;
}

/*!**************************************************************************
 * @brief    Tell whether the context data is valid
 * @param    buffer - context buffer
 * @return   TRUE if context data is valid, FALSE otherwise
 ****************************************************************************/
static BOOL IsValidContext(U8BIT *buffer)
{
   BOOL valid;

   FUNCTION_START(IsValidContext);

   if (buffer[VALID_OFFSET] != 0)
   {
      valid = TRUE;
      
      CC_DEBUG_PRINT(("valid buffer \n" )); 
   }
   else
   {
      valid = FALSE;      
      CC_DEBUG_PRINT(("invalid buffer\n")); 
   }

   FUNCTION_FINISH(IsValidContext);

   return valid;
}
/*!**************************************************************************
 * @brief    Return timestamp from context
 * @param    data - context data
 * @param    index - index for context
 ****************************************************************************/
static U8BIT GetTimestamp(void *data, U8BIT index)
{
   U8BIT *buffer;
   U8BIT timestamp;

   FUNCTION_START(GetTimestamp);

   /* unused */
   data = data;

   ASSERT(sizeof(timestamp) == TIMESTAMP_SIZE);
   
   /* fix klocwork */
   timestamp = 0;
   if ((index * BUFFER_SIZE) < TOTAL_SIZE)
   {   
      buffer = context_data + index * BUFFER_SIZE;
      memcpy_s2s_chk(&timestamp, buffer + TIMESTAMP_OFFSET, TIMESTAMP_SIZE,NULL);
   }

   FUNCTION_FINISH(GetTimestamp);

   return timestamp;
}

/*!**************************************************************************
 * @brief    Set timestamp for context
 * @param    data - context data
 * @param    index - index for context
 ****************************************************************************/
static void SetTimestamp(void *data, U8BIT index, U8BIT timestamp)
{
   U8BIT *buffer;

   FUNCTION_START(SetTimestamp);

   /* unused */
   data = data;

   ASSERT(sizeof(timestamp) == TIMESTAMP_SIZE);
   
   /* fix klocwork */
   if ((index * BUFFER_SIZE) < TOTAL_SIZE)
   {
      buffer = context_data + index * BUFFER_SIZE;
      memcpy_s2s_chk(buffer + TIMESTAMP_OFFSET, &timestamp, TIMESTAMP_SIZE,NULL);
   }

   FUNCTION_FINISH(SetTimestamp);
}

/*!**************************************************************************
 * @brief    Copy invalid authentication context
 *
 * An invalid authentication context is defined as all-zeros by the CI+
 * specification.
 *
 * @param    session_info - content control session information
 ****************************************************************************/
static void CopyInvalidAuthContext(CI_CC_INFO *cc)
{

   FUNCTION_START(CopyInvalidAuthContext);

   /* DHSK */
   memset(cc->dhsk, 0, BITS_TO_BYTES(DT_SIZE_DHSK));
   cc->dhsk_generated = TRUE;

   /* AKH */
   memset(cc->akh, 0, BITS_TO_BYTES(DT_SIZE_AKH));
   cc->akh_generated = TRUE;

   /* CICAM_ID */
   memset(cc->local_cicam_id, 0, BITS_TO_BYTES(DT_SIZE_CICAM_ID));
   cc->local_cicam_id_extracted = TRUE;

   /* Cipher */
   cc->cipher = 0xff;
   cc->cipher_negotiated = TRUE;

   FUNCTION_FINISH(CopyInvalidAuthContext);
}

/*!**************************************************************************
 * @brief    Set context in context data
 *
 * This function copies the relevant bits of data from the context to the
 * context data storage space. The context is in fact the session information
 * structure that is holding the context.
 *
 * @param    data - context data
 * @param    index - index for context
 * @param    context - the context to store
 ****************************************************************************/
static void SetContext(void *data, U8BIT index, void *context)
{
    CI_CC_INFO *cc;
   U8BIT *buffer;

   FUNCTION_START(SetContext);

   /* unused */
   data = data;

   /* fix klocwork */
   if ((index * BUFFER_SIZE) < TOTAL_SIZE)
   {
      cc = context;
      buffer = context_data + index * BUFFER_SIZE;
      
      WriteContextToBuffer(cc, buffer);   
   }

   FUNCTION_FINISH(SetContext);
}

/*!**************************************************************************
 * @brief    Tell whether the given context is the same as a stored context
 * @param    data - context data
 * @param    context - the context to store
 * @param    index - index of stored context
 ****************************************************************************/
static BOOL Equals(void *data, void *context, U8BIT index)
{
   CI_CC_INFO *cc;
   U8BIT *buffer;
   BOOL equals;

   FUNCTION_START(Equals);

   /* unused */
   data = data;

   equals = TRUE;
   cc = context;

   /* fix klocwork */
   if ((index * BUFFER_SIZE) >= TOTAL_SIZE)
   {
       return FALSE;
   }
   
   buffer = context_data + index * BUFFER_SIZE;
   
   if (equals && memcmp(cc->dhsk, buffer + DHSK_OFFSET, DHSK_SIZE) != 0)
   {
      equals = FALSE;
   }
   if (equals && memcmp(cc->akh, buffer + AKH_OFFSET, AKH_SIZE) != 0)
   {
      equals = FALSE;
   }
   if (equals && memcmp(cc->local_cicam_id, buffer + CICAM_ID_OFFSET,
                        CICAM_ID_SIZE) != 0)
   {
      equals = FALSE;
   }
   if (equals && memcmp(&cc->cicam_brand_id, buffer + CICAM_BRAND_ID_OFFSET,
                        CICAM_BRAND_ID_SIZE) != 0)
   {
      equals = FALSE;
   }
   if (equals && memcmp(&cc->cipher, buffer + CIPHER_OFFSET, CIPHER_SIZE) != 0)
   {
      equals = FALSE;
   }

   FUNCTION_FINISH(Equals);

   return equals;
}

/*!**************************************************************************
 * @brief    Read context from context buffer
 * @param    session_info - content control session information
 * @param    buffer - context buffer
 ****************************************************************************/
static void ReadContextFromBuffer(CI_CC_INFO *cc,
                                  U8BIT *buffer)
{

   FUNCTION_START(ReadContextFromBuffer);
   CC_DEBUG_PRINT(("ReadContextFromBuffer\n"));

   /* DHSK */
   memcpy_s2s_chk(cc->dhsk, buffer + DHSK_OFFSET, DHSK_SIZE,NULL);
   cc->dhsk_generated = TRUE;
   CC_DEBUG_PRINT(("DHSK:\n"));
   CC_DEBUG_BUFFER((buffer + DHSK_OFFSET), DHSK_SIZE);

   /* AKH */
   memcpy_s2s_chk(cc->akh, buffer + AKH_OFFSET, AKH_SIZE,NULL);
   cc->akh_generated = TRUE;
   CC_DEBUG_PRINT(("AKH:\n"));
   CC_DEBUG_BUFFER((buffer + AKH_OFFSET), AKH_SIZE);

   /* CICAM_ID */
   memcpy_s2s_chk(cc->local_cicam_id, buffer + CICAM_ID_OFFSET, CICAM_ID_SIZE,NULL);
   cc->local_cicam_id_extracted = TRUE;
   CC_DEBUG_PRINT(("CICAM_ID:\n"));
   CC_DEBUG_BUFFER((buffer + CICAM_ID_OFFSET), CICAM_ID_SIZE);

   /* CICAM brand ID */
   memcpy_s2s_chk(&cc->cicam_brand_id, buffer + CICAM_BRAND_ID_OFFSET,
          CICAM_BRAND_ID_SIZE,NULL);
   cc->cicam_brand_id_extracted = TRUE;
   CC_DEBUG_PRINT(("CICAM_BRAND_ID:\n"));
   CC_DEBUG_BUFFER((buffer + CICAM_BRAND_ID_OFFSET), CICAM_BRAND_ID_SIZE);

   /* Cipher */
   memcpy_s2s_chk(&cc->cipher, buffer + CIPHER_OFFSET, CIPHER_SIZE,NULL);
   cc->cipher_negotiated = TRUE;
      
  CC_DEBUG_PRINT(("CIPHER:\n"));
  CC_DEBUG_BUFFER((buffer + CIPHER_OFFSET), CIPHER_SIZE);

   FUNCTION_FINISH(ReadContextFromBuffer);
}

/*!**************************************************************************
 * @brief    Write context into a buffer
 * @param    session_info - content control session information
 * @param    buffer - buffer to write into
 ****************************************************************************/
static void WriteContextToBuffer(CI_CC_INFO *cc,
                                 U8BIT *buffer)
{
   U8BIT valid;

   FUNCTION_START(WriteContextToBuffer);

   memset(buffer, 0, BUFFER_SIZE);

   valid = 1;
   ASSERT(sizeof(valid) == VALID_SIZE);
   memcpy_s2s_chk(buffer + VALID_OFFSET, &valid, VALID_SIZE,NULL);

   CC_DEBUG_PRINT(("VALID Flag:\n"));
   CC_DEBUG_BUFFER((buffer + VALID_OFFSET), VALID_SIZE);

   if (cc->dhsk_generated)
   {
      ASSERT(sizeof(cc->dhsk) == DHSK_SIZE);
      memcpy_s2s_chk(buffer + DHSK_OFFSET, cc->dhsk, DHSK_SIZE,NULL);
   
      CC_DEBUG_PRINT(("DHSK:\n"));
      CC_DEBUG_BUFFER((buffer + DHSK_OFFSET), DHSK_SIZE);
   }

   if (cc->akh_generated)
   {
      ASSERT(sizeof(cc->akh) == AKH_SIZE);
      memcpy_s2s_chk(buffer + AKH_OFFSET, cc->akh, AKH_SIZE,NULL);
      
      CC_DEBUG_PRINT(("AKH:\n"));
      CC_DEBUG_BUFFER((buffer + AKH_OFFSET), AKH_SIZE);
   }
   
   if (cc->local_cicam_id_extracted)
   {
      ASSERT(sizeof(cc->local_cicam_id) == CICAM_ID_SIZE);
      memcpy_s2s_chk(buffer + CICAM_ID_OFFSET, cc->local_cicam_id, CICAM_ID_SIZE,NULL);
      
      CC_DEBUG_PRINT(("CICAM_ID:\n"));
      CC_DEBUG_BUFFER((buffer + CICAM_ID_OFFSET), CICAM_ID_SIZE);
   }
   
   if (cc->cicam_brand_id_extracted)
   {
      ASSERT(sizeof(cc->cicam_brand_id) == CICAM_BRAND_ID_SIZE);
      memcpy_s2s_chk(buffer + CICAM_BRAND_ID_OFFSET, &cc->cicam_brand_id,
             CICAM_BRAND_ID_SIZE,NULL);
      
      CC_DEBUG_PRINT(("CICAM_BRAND_ID:\n"));
      CC_DEBUG_BUFFER((buffer + CICAM_BRAND_ID_OFFSET), CICAM_BRAND_ID_SIZE);
   }
  
   if (cc->cipher_negotiated)
   {
      ASSERT(sizeof(cc->cipher) == CIPHER_SIZE);
      memcpy_s2s_chk(buffer + CIPHER_OFFSET, &cc->cipher, CIPHER_SIZE,NULL);
      
      CC_DEBUG_PRINT(("CIPHER:\n"));
      CC_DEBUG_BUFFER((buffer + CIPHER_OFFSET), CIPHER_SIZE);
   }
   

   FUNCTION_FINISH(WriteContextToBuffer);
}
/*****************************************************************************/
/* End of File                                                               */
/*****************************************************************************/
