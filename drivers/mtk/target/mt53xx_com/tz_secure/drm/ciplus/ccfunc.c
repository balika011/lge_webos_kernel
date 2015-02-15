/*****************************************************************************/
/* (c) Copyright Ocean Blue Software Ltd. - ALL RIGHTS RESERVED              */
/* This document is the CONFIDENTIAL property of Ocean Blue Software Ltd.    */
/*****************************************************************************/
/*                                                                           */
/* File Name:     ccsac.c                                                    */
/*                                                                           */
/* Description:   CI+ Content Control Resource: Licensee functions           */
/*                                                                           */
/* Project:       CI / CI+ Library                                           */
/*                                                                           */
/* Creation Date: 02/07/2009                                                 */
/*                                                                           */
/*****************************************************************************/

/* --- includes for this file --- */
/* compiler library header files */
/* Ocean Blue header files */
#include "ccdef.h"
#include "stbcihw.h"
#include "ccactx.h"
#include "ccfunc.h"
#include "ccmsg.h"
#include "ccsac.h"
#include "cccert.h"
#include "sectk.h"
#include "ci_trust_zone.h"
/* --- constant definitions for this file --- */
#define CC_DEBUG_PRINT(x) CC_PRINT(x)
#define CC_DEBUG_BUFFER(x,y) CC_PRINT_BUFFER(x,y)

#define FUNCTION_START(x) do { CC_PRINT((">> %s\n",#x)); } while(0)
/* #define FUNCTION_FINISH(x) do { printf("<< %s\n",#x); } while(0) */
#define FUNCTION_FINISH(x) do { CC_PRINT(("<< %s\n",#x)); } while(0)


static const unsigned char odd_parity[256]={
  1,  1,  2,  2,  4,  4,  7,  7,  8,  8, 11, 11, 13, 13, 14, 14,
 16, 16, 19, 19, 21, 21, 22, 22, 25, 25, 26, 26, 28, 28, 31, 31,
 32, 32, 35, 35, 37, 37, 38, 38, 41, 41, 42, 42, 44, 44, 47, 47,
 49, 49, 50, 50, 52, 52, 55, 55, 56, 56, 59, 59, 61, 61, 62, 62,
 64, 64, 67, 67, 69, 69, 70, 70, 73, 73, 74, 74, 76, 76, 79, 79,
 81, 81, 82, 82, 84, 84, 87, 87, 88, 88, 91, 91, 93, 93, 94, 94,
 97, 97, 98, 98,100,100,103,103,104,104,107,107,109,109,110,110,
112,112,115,115,117,117,118,118,121,121,122,122,124,124,127,127,
128,128,131,131,133,133,134,134,137,137,138,138,140,140,143,143,
145,145,146,146,148,148,151,151,152,152,155,155,157,157,158,158,
161,161,162,162,164,164,167,167,168,168,171,171,173,173,174,174,
176,176,179,179,181,181,182,182,185,185,186,186,188,188,191,191,
193,193,194,194,196,196,199,199,200,200,203,203,205,205,206,206,
208,208,211,211,213,213,214,214,217,217,218,218,220,220,223,223,
224,224,227,227,229,229,230,230,233,233,234,234,236,236,239,239,
241,241,242,242,244,244,247,247,248,248,251,251,253,253,254,254};

/* --- local typedef structs for this file --- */

/* --- local (static) variable declarations for this file --- */
/* (internal variables declared static to make them local) */

/* --- local function prototypes for this file --- */
/* (internal functions declared static to make them local) */

static void CalculateDesKey(U8BIT *kp, U8BIT *cck);
static void CalculateAesKey(U8BIT *kp, U8BIT *cck, U8BIT *civ);


static void _km_comp_cip_cipher(U8BIT *key, U8BIT *ks, U8BIT *km);
static void _ProductSacKey(U8BIT *ks, U8BIT *sak, U8BIT *sek);
static void _ProductDesKey(U8BIT *kp, U8BIT *cck);
static void _ProductAESKey(U8BIT *kp, U8BIT *cck, U8BIT *civ);

static void _SacKey(U8BIT *ks, U8BIT *sak, U8BIT *sek);
static void _DesKey(U8BIT *kp, U8BIT *cck);
static void _AESKey(U8BIT *kp, U8BIT *cck, U8BIT *civ);


extern CC_CI_PLUS_KEY_TYPE gui1_key_type;
extern VOID TZ_ciplus_read_cred (E_TZ_STB_CIPLUS_KEY_TYPE type, UINT8 **key, UINT16 *length);

/*---------------------------------------------------------------------------*/
/* global function definitions                                               */
/*---------------------------------------------------------------------------*/

/*!**************************************************************************
 * @brief    Calculate SEK and SAK from Ks (f-SAC)
 * @param    ks - key seed (256 bits = 32 bytes)
 * @param    sak - SAC authentication key (128 bits = 16 bytes)
 * @param    sek - SAC encryption key (128 bits = 16 bytes);
 ****************************************************************************/
void CC_CalculateSacKeys(U8BIT *ks, U8BIT *sak, U8BIT *sek)
{
   FUNCTION_START(CC_CalculateSacKeys);

   ASSERT(ks != NULL);
   ASSERT(sak != NULL);
   ASSERT(sek != NULL);

  if(gui1_key_type == CC_CI_PLUS_KEY_TYPE_PRODUCT)
   {
       _ProductSacKey(ks, sak, sek);
   }
   else
   {
       _SacKey(ks, sak, sek);
   }
   FUNCTION_FINISH(CC_CalculateSacKeys);
}

/*!**************************************************************************
 * @brief    Calculate CCK and SIV from Kp (f-CC)
 * @param    kp - key precursor (256 bits = 32 bytes)
 * @param    cipher - the cipher to use (STB_CI_CIPHER_DES/STB_CI_CIPHER_AES)
 * @param    cck - content control key (length depends on cipher)
 * @param    civ - key initialisation vector (only for AES-128)
 ****************************************************************************/
void CC_CalculateCcKeys(U8BIT *kp, U8BIT cipher, U8BIT *cck, U8BIT *civ)
{
   FUNCTION_START(CC_CalculateCcKeys);
   switch (cipher)
   {
   case STB_CI_CIPHER_DES:
      CC_DEBUG_PRINT(("CC cipher = DES\n"));   	
      CalculateDesKey(kp, cck);
      #if 0
      /* dump key */
      CC_DEBUG_PRINT(("CCK:\n"));
      CC_DEBUG_BUFFER(cck, 8);
      #endif
      break;
   case STB_CI_CIPHER_AES:
      /* CalculateAesInitVec(kp, civ); */
      CC_DEBUG_PRINT(("CC cipher = AES\n"));
      CalculateAesKey(kp, cck, civ);
      #if 0
      /* dump key */
      CC_DEBUG_PRINT(("CCK:\n"));
      CC_DEBUG_BUFFER(cck, 16);      
      CC_DEBUG_PRINT(("CIV:\n"));
      CC_DEBUG_BUFFER(civ, 16);
      #endif
      break;
   default:
      ASSERT(0);
   };

   FUNCTION_FINISH(CC_CalculateCcKeys);
}

/*---------------------------------------------------------------------------*/
/* local function definitions                                                */
/*---------------------------------------------------------------------------*/

/*!**************************************************************************
 * @brief    Calculate DES key
 * @param    kp - key precursor (256 bits = 32 bytes)
 * @param    cck - content control key (64 bits)
 ****************************************************************************/
static void CalculateDesKey(U8BIT *kp, U8BIT *cck)
{
   FUNCTION_START(CalculateDesKey);

   #if 0
   if ((STB_CIGetCredentialType() == STB_CI_CRED_SMARDTV_TEST_V1)&&
   	   (PHYS_get_recognized_cam_type() == CI_CAM_SmarDTV_DVB_CA_MODULE))
   {
      SmarDTVIPPKitDesKey(kp, cck);
   }
   else
   {
      DummyDesKey(kp, cck);
   }
   #endif

   if(gui1_key_type == CC_CI_PLUS_KEY_TYPE_PRODUCT)
   {
       _ProductDesKey(kp, cck);
   }
   else
   {
       _DesKey(kp, cck);
   }

   FUNCTION_FINISH(CalculateDesKey);
}


/*!**************************************************************************
 * @brief    Calculate AES key
 * @param    kp - key precursor (256 bits = 32 bytes)
 * @param    cck - content control key (128 bits = 16 bytes)
 * @param    civ - initialisation vector (128 bits = 16 bytes)
 ****************************************************************************/
static void CalculateAesKey(U8BIT *kp, U8BIT *cck, U8BIT *civ)
{
   FUNCTION_START(CalculateAesKey);


   if(gui1_key_type == CC_CI_PLUS_KEY_TYPE_PRODUCT)
   {
       _ProductAESKey(kp, cck, civ);
   }
   else
   {
       _AESKey(kp, cck, civ);
   }

   FUNCTION_FINISH(CalculateAesKey);
}



/*!**************************************************************************
 * @brief    compute km of f-cc or f-sac function by cip_cipher
 * @param    key - 128 bit AES ECB precursor (128 bits = 16 bytes)
 * @param    ks - keey seed material  (256 bits = 32 bytes)
 * @param    km - key material vector (128 bits = 16 bytes)
 ****************************************************************************/
static void _km_comp_cip_cipher(U8BIT *key, U8BIT *ks, U8BIT *km)
{
    U8BIT ks0[16];
    U8BIT ks1[16];
    U8BIT e0[16];
    U8BIT e1[16];
    U8BIT km1[16];
    U8BIT km2[16];
    U16BIT offset;

    memcpy_s2s_chk(ks0, ks, 16,NULL);
    memcpy_s2s_chk(e0, ks, 16,NULL);
    SECTK_AesEncrypt(e0, 16, key);
    
    memcpy_s2s_chk(ks1, ks+16, 16,NULL);
    memcpy_s2s_chk(e1, ks+16, 16,NULL);
    SECTK_AesEncrypt(e1, 16, key);
    for (offset = 0; offset < 16; ++offset)
    {
        km1[offset] = e0[offset] ^ ks0[offset];
        km2[offset] = e1[offset] ^ ks1[offset];        
    }
    memcpy_s2s_chk(km, km1, 16,NULL);
    memcpy_s2s_chk(km+16, km2, 16,NULL);    
}


/*!**************************************************************************
 * @brief    _ProductSacKey proprietary algorithm to calculate SEK and SAK from Ks (f-SAC)
 * @param    ks - key seed (256 bits = 32 bytes)
 * @param    sak - SAC authentication key (128 bits = 16 bytes)
 * @param    sek - SAC encryption key (128 bits = 16 bytes);
 ****************************************************************************/
static void _ProductSacKey(U8BIT *ks, U8BIT *sak, U8BIT *sek)
{
    FUNCTION_START(_ProductSacKey);
    ASSERT(ks != NULL);
    ASSERT(sak != NULL);
    ASSERT(sek != NULL);
 
    U8BIT km[32];
    U8BIT *slk = NULL;
    U16BIT slk_len = 0;
 
    TZ_ciplus_read_cred(TZ_STB_CIPLUS_KEY_SLK, &slk, &slk_len);
    ASSERT(slk != NULL);
    ASSERT(slk_len == 0);

    /* f-sac */
    /* SEK = MSB_128(Km)
       SAK = LSB_128(Km) */
    _km_comp_cip_cipher(slk, ks, km);
    memcpy_s2s_chk(sek, km, 16,NULL);
    memcpy_s2s_chk(sak, km+16, 16,NULL);
    
    FUNCTION_FINISH(_ProductSacKey);
}

/*!**************************************************************************
 * @brief    _ProductDesKey proprietary algorithm to Calculate DES key
 * @param    kp - key precursor (256 bits = 32 bytes)
 * @param    cck - content control key (64 bits)
 * @param    cck - content control key (64 bits) 
 ****************************************************************************/
static void _ProductDesKey(U8BIT *kp, U8BIT *cck)
{
   /* DES key: CCK is 64 bits with parity bits */
   FUNCTION_START(_ProductDesKey);   
   U16BIT i;
   U8BIT km[32];
   U8BIT k_dfast[7] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd};
   U8BIT k_dfast_break[8];
   U8BIT *clk = NULL;
   U16BIT clk_len = 0;   
   
   TZ_ciplus_read_cred(TZ_STB_CIPLUS_KEY_CLK, &clk, &clk_len);
   ASSERT(clk != NULL);
   ASSERT(clk_len == 0);

   /* f-cc */
   /* CCK_DES = MSB_56(Km) and then padded with odd parity bits */
   _km_comp_cip_cipher(clk, kp, km);
   memcpy_s2s_chk(k_dfast, km, 7,NULL);
   
   #if 0
   /* dump dfast buffer */
   CC_DEBUG_PRINT(("Km:\n"));
   CC_DEBUG_BUFFER(km, 7);     
   CC_DEBUG_PRINT(("Dfast:\n"));
   CC_DEBUG_BUFFER(k_dfast, 8);
   #endif
   
   k_dfast_break[0] = k_dfast[0]>>1;
   k_dfast_break[1] = (k_dfast[1]>>2)|((k_dfast[0]&0x01)<<6);
   k_dfast_break[2] = (k_dfast[2]>>3)|((k_dfast[1]&0x03)<<5);
   k_dfast_break[3] = (k_dfast[3]>>4)|((k_dfast[2]&0x07)<<4);
   k_dfast_break[4] = (k_dfast[4]>>5)|((k_dfast[3]&0x0F)<<3);
   k_dfast_break[5] = (k_dfast[5]>>6)|((k_dfast[4]&0x1F)<<2);
   k_dfast_break[6] = (k_dfast[6]>>7)|((k_dfast[5]&0x3F)<<1);
   k_dfast_break[7] = ((k_dfast[6]&0x7F)<<0);
   
   #if 0
   CC_DEBUG_PRINT(("Dfast_break:\n"));
   CC_DEBUG_BUFFER(k_dfast_break, 8);
   #endif
   
   /* padded with parity bits */
   for (i=0; i<8; i++)
   {
       cck[i]= odd_parity[(k_dfast_break[i]<<1)];
   }

   FUNCTION_FINISH(_ProductDesKey);
}

/*!**************************************************************************
 * @brief    _ProductAESKey proprietary algorithm to Calculate AES key
 * @param    kp - key precursor (256 bits = 32 bytes)
 * @param    cck - content control key (128 bits = 16 bytes)
 * @param    civ - initialisation vector (128 bits = 16 bytes)
 ****************************************************************************/
static void _ProductAESKey(U8BIT *kp, U8BIT *cck, U8BIT *civ)
{
    FUNCTION_START(_ProductAESKey);
    ASSERT(kp != NULL);
    ASSERT(cck != NULL);
    ASSERT(civ != NULL);
 
    U8BIT km[32];
    U8BIT *clk = NULL;
    U16BIT clk_len = 0;
 
    TZ_ciplus_read_cred(TZ_STB_CIPLUS_KEY_CLK, &clk, &clk_len);
    ASSERT(clk != NULL);
    ASSERT(clk_len == 0);
    
    /* f-cc */
    /* CCK = MSB_128(Km)
       CIV = LSB_128(Km) */
    _km_comp_cip_cipher(clk, kp, km);
    memcpy_s2s_chk(cck, km, 16,NULL);
    memcpy_s2s_chk(civ, km+16, 16,NULL);
    
    CC_DEBUG_PRINT(("CCK:\n"));
    CC_DEBUG_BUFFER(cck, 16);     
    CC_DEBUG_PRINT(("CIV:\n"));
    CC_DEBUG_BUFFER(civ, 16);     
    FUNCTION_FINISH(_ProductAESKey);
}

/*!**************************************************************************
 * @brief    _SacKey proprietary algorithm to calculate SEK and SAK from Ks (f-SAC)
 * @param    ks - key seed (256 bits = 32 bytes)
 * @param    sak - SAC authentication key (128 bits = 16 bytes)
 * @param    sek - SAC encryption key (128 bits = 16 bytes);
 ****************************************************************************/
static void _SacKey(U8BIT *ks, U8BIT *sak, U8BIT *sek)
{
   FUNCTION_START(_SacKey);

   ASSERT(ks != NULL);
   ASSERT(sak != NULL);
   ASSERT(sek != NULL);

   U8BIT dual_SLK[32];
   U8BIT km[32];
   U8BIT digest[32];
   U16BIT offset;
   U8BIT *slk = NULL;
   U16BIT slk_len = 0;

   TZ_ciplus_read_cred(TZ_STB_CIPLUS_KEY_SLK, &slk, &slk_len);
   ASSERT(slk != NULL);
   ASSERT(slk_len == 0);
    
   /* SLK || SLK */
   memcpy_s2s_chk(dual_SLK, slk, 16,NULL);
   memcpy_s2s_chk(dual_SLK + 16, slk, 16,NULL);
  
   /* Km = SHA_256(Ks) XOR SLK_256*/
   SECTK_Sha256(ks, 32, digest);
  
   for (offset = 0; offset < 32; ++offset)
   {
      km[offset] = digest[offset] ^ dual_SLK[offset];
   }    
  
   /* SEK = MSB_128(Km)
      SAK = LSB_128(Km) */
   memcpy_s2s_chk(sek, km, 16,NULL);
   memcpy_s2s_chk(sak, km+16, 16,NULL);

   FUNCTION_FINISH(_SacKey);
}

/*!**************************************************************************
 * @brief    _DesKey proprietary algorithm to Calculate DES key
 * @param    kp - key precursor (256 bits = 32 bytes)
 * @param    cck - content control key (64 bits)
 ****************************************************************************/
static void _DesKey(U8BIT *kp, U8BIT *cck)
{
   U16BIT i;

   FUNCTION_START(_DesKey);

   /* DES key: CCK is 64 bits with parity bits */
   U8BIT dual_CLK[32];
   U8BIT km[32];
   U8BIT digest[32];
   U16BIT offset;
   U8BIT k_dfast[7] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd};
   U8BIT k_dfast_break[8];
   U8BIT *clk = NULL;
   U16BIT clk_len = 0;   
   
   TZ_ciplus_read_cred(TZ_STB_CIPLUS_KEY_CLK, &clk, &clk_len);
   ASSERT(clk != NULL);
   ASSERT(clk_len == 0);
   
   /* CLK || CLK */
   memcpy_s2s_chk(dual_CLK, clk, 16,NULL);
   memcpy_s2s_chk(dual_CLK + 16, clk, 16,NULL);
   
   /* Km = SHA_256(Kp) XOR CLK_256*/
   SECTK_Sha256(kp, 32, digest);
   
   for (offset = 0; offset < 32; ++offset)
   {
       km[offset] = digest[offset] ^ dual_CLK[offset];
   }
   
   /* CCK_DES = MSB_56(Km) and then padded with odd parity bits */
   memcpy_s2s_chk(k_dfast, km, 7,NULL);
   
   /* dump dfast buffer */
   CC_DEBUG_PRINT(("Km:\n"));
   CC_DEBUG_BUFFER(km, 7);     
   CC_DEBUG_PRINT(("Dfast:\n"));
   CC_DEBUG_BUFFER(k_dfast, 7);
   
   k_dfast_break[0] = k_dfast[0]>>1;
   k_dfast_break[1] = (k_dfast[1]>>2)|((k_dfast[0]&0x01)<<6);
   k_dfast_break[2] = (k_dfast[2]>>3)|((k_dfast[1]&0x03)<<5);
   k_dfast_break[3] = (k_dfast[3]>>4)|((k_dfast[2]&0x07)<<4);
   k_dfast_break[4] = (k_dfast[4]>>5)|((k_dfast[3]&0x0F)<<3);
   k_dfast_break[5] = (k_dfast[5]>>6)|((k_dfast[4]&0x1F)<<2);
   k_dfast_break[6] = (k_dfast[6]>>7)|((k_dfast[5]&0x3F)<<1);
   k_dfast_break[7] = ((k_dfast[6]&0x7F)<<0);
   
   CC_DEBUG_PRINT(("Dfast_break:\n"));
   CC_DEBUG_BUFFER(k_dfast_break, 8);
   
   /* padded with parity bits */
   for (i=0; i<8; i++)
   {
       cck[i]= odd_parity[(k_dfast_break[i]<<1)];
   }     

   FUNCTION_FINISH(_DesKey);
}

/*!**************************************************************************
 * @brief    _AESKey proprietary algorithm to Calculate AES key
 * @param    kp - key precursor (256 bits = 32 bytes)
 * @param    cck - content control key (128 bits = 16 bytes)
 * @param    civ - initialisation vector (128 bits = 16 bytes)
 ****************************************************************************/
static void _AESKey(U8BIT *kp, U8BIT *cck, U8BIT *civ)
{
   FUNCTION_START(_AESKey);

   U8BIT dual_CLK[32];
   U8BIT km[32];
   U8BIT digest[32];
   U16BIT offset;
   U8BIT *clk = NULL;
   U16BIT clk_len = 0;

   TZ_ciplus_read_cred(TZ_STB_CIPLUS_KEY_CLK, &clk, &clk_len);
   ASSERT(clk != NULL);
   ASSERT(clk_len == 0);   
  
   /* CLK || CLK */
   memcpy_s2s_chk(dual_CLK, clk, 16,NULL);
   memcpy_s2s_chk(dual_CLK + 16, clk, 16,NULL);
  
   /* Km = SHA_256(Kp) XOR SLK_256*/
   SECTK_Sha256(kp, 32, digest);
  
   for (offset = 0; offset < 32; ++offset)
   {
      km[offset] = digest[offset] ^ dual_CLK[offset];
   }    
  
   /* CCK = MSB_128(Km)
      CIV = LSB_128(Km) */
   memcpy_s2s_chk(cck, km, 16,NULL);
   memcpy_s2s_chk(civ, km+16, 16,NULL);
   
   FUNCTION_FINISH(_AESKey);
}



/*****************************************************************************/
/* End of File                                                               */
/*****************************************************************************/
