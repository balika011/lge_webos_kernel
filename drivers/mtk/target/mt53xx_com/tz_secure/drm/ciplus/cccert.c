/*****************************************************************************/
/* (c) Copyright Ocean Blue Software Ltd. - ALL RIGHTS RESERVED              */
/* This document is the CONFIDENTIAL property of Ocean Blue Software Ltd.    */
/*****************************************************************************/
/*                                                                           */
/* File Name:     ccsac.c                                                    */
/*                                                                           */
/* Description:   CI+ Content Control Resource: Secure Authenticated Channel */
/*                                                                           */
/* Project:       CI / CI+ Library                                           */
/*                                                                           */
/* Creation Date: 16/01/2009                                                 */
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
#include "ci_trust_zone.h"

/* --- constant definitions for this file --- */

#define CC_DEBUG_PRINT(x) CC_PRINT(x)
#define CC_DEBUG_BUFFER(x,y) CC_PRINT_BUFFER(x,y)

#define CC_DEBUG_PRINT_FORCE(x) CC_PRINT(x)
#define CC_DEBUG_BUFFER_FORCE(x,y) CC_PRINT_BUFFER(x,y)

#define FUNCTION_START(x)
#define FUNCTION_FINISH(x)

/* --- local typedef structs for this file --- */

/* --- local (static) variable declarations for this file --- */
/* (internal variables declared static to make them local) */

/* --- local function prototypes for this file --- */
/* (internal functions declared static to make them local) */

static E_CC_CERT_CHK CheckIssuer(void *parent_cert, void *child_cert);
static E_CC_CERT_CHK CheckValidityPeriod(void *cert,UINT32 epoch_time);
static E_CC_CERT_CHK CheckBrandExtensions(void *brand_cert, void *root_cert);
static E_CC_CERT_CHK CheckBrandKeyUsage(S_SECTK_CERT_EXT *ext);
static E_CC_CERT_CHK CheckBrandSubjectKeyIdentifier(S_SECTK_CERT_EXT *ext,
                                                    void *brand_cert);
static E_CC_CERT_CHK CheckBrandAuthorityKeyIdentifier(S_SECTK_CERT_EXT *ext,
                                                      void *root_cert);
static E_CC_CERT_CHK CheckBrandBasicConstraints(S_SECTK_CERT_EXT *ext);
static E_CC_CERT_CHK CheckDeviceExtensions(void *device_cert,
                                           void *brand_cert);
static E_CC_CERT_CHK CheckDeviceKeyUsage(S_SECTK_CERT_EXT *ext);
static E_CC_CERT_CHK CheckDeviceAuthorityKeyIdentifier(S_SECTK_CERT_EXT *ext,
                                                       void *brand_cert);
static E_CC_CERT_CHK CheckDeviceBasicConstraints(S_SECTK_CERT_EXT *ext);
static E_CC_CERT_CHK CheckDeviceScramblerCapabilities(S_SECTK_CERT_EXT *ext);
static E_CC_CERT_CHK CheckDeviceCicamBrandIdentifier(S_SECTK_CERT_EXT *ext);
static E_CC_CERT_CHK CheckKeyIdentifier(void *parent_cert, void *child_cert);
static E_CC_CERT_CHK CheckCertificateSignature(void *parent_cert,
                                               void *child_cert);
static E_CC_CERT_CHK CheckDeviceId(void *cert);


/*---------------------------------------------------------------------------*/
/* global function definitions                                               */
/*---------------------------------------------------------------------------*/

/*!**************************************************************************
 * @brief    Check CI+ certificates
 * @param    root_cert - CI+ root certificate
 * @param    brand_cert - CI+ brand certificate
 * @param    device_cert - CI+ device certificate
 * @return   Result of certificate check
 ****************************************************************************/
E_CC_CERT_CHK CC_CheckCertificates(void *root_cert, void *brand_cert,
                                   void *device_cert,UINT32 epoch_time)
{
   E_CC_CERT_CHK chk;

   FUNCTION_START(CC_CheckCertificates);

   chk = CC_CERT_CHK_OK;

   if (chk == CC_CERT_CHK_OK)
   {
      /* Check that the Issuer of the brand certificate is identical to
       * the Subject of the root certificate (sec. 9.4.1)
       */
      chk = CheckIssuer(root_cert, brand_cert);
   }

   if (chk == CC_CERT_CHK_OK)
   {
      /* Check that the validity period of the brand certificate includes
       * the current date and time (sec. 9.4.1)
       */
      CC_DEBUG_PRINT(("{MW CI} check cam brand certification valid period\n"));
      chk = CheckValidityPeriod(brand_cert,epoch_time);
   }

   if (chk == CC_CERT_CHK_OK)
   {
      /* Check that the Issuer of the device certificate is identical to
       * the Subject of the brand certificate (sec. 9.4.2)
       */
      chk = CheckIssuer(brand_cert, device_cert);
   }

   if (chk == CC_CERT_CHK_OK)
   {
      /* Check that the validity period of the device certificate includes
       * the current date and time (sec. 9.4.2)
       */
      CC_DEBUG_PRINT(("{MW CI} check cam device certification valid period\n"));
      chk = CheckValidityPeriod(device_cert,epoch_time);
   }

   if (chk == CC_CERT_CHK_OK)
   {
      /* Check that each mandatory extension listed in section 9.3.9 exists and
       * the values are valid. Check that no other extension is marked as
       * critical.
       */
      chk = CheckBrandExtensions(brand_cert, root_cert);
   }

   if (chk == CC_CERT_CHK_OK)
   {
      /* Check that each mandatory extension listed in section 9.3.9 exists and
       * the values are valid. Check that no other extension is marked as
       * critical.
       */
      chk = CheckDeviceExtensions(device_cert, brand_cert);
   }

   if (chk == CC_CERT_CHK_OK)
   {
      /* Verify that the KeyIdentifier in the brand certificate's authority key
       * identifier extension is identical to the KeyIdentifier in the root
       * certificate's subject key identifier extension.
       */
      chk = CheckKeyIdentifier(root_cert, brand_cert);
   }

   if (chk == CC_CERT_CHK_OK)
   {
      /* Verify that the KeyIdentifier in the device certificate's authority
       * key identifier extension is identical to the KeyIdentifier in the
       * brand certificate's subject key identifier extension.
       */
      chk = CheckKeyIdentifier(brand_cert, device_cert);
   }

   if (chk == CC_CERT_CHK_OK)
   {
      /* Verify the certificate's signature by using the RSASSA-PSS
       * verification described in RSA PKCS#1 [976H [1]], section 8.1.2.
       */
      chk = CheckCertificateSignature(root_cert, brand_cert);
   }

   if (chk == CC_CERT_CHK_OK)
   {
      /* Verify the certificate's signature by using the RSASSA-PSS
       * verification described in RSA PKCS#1 [976H [1]], section 8.1.2.
       */
      chk = CheckCertificateSignature(brand_cert, device_cert);
   }
   
   if (chk == CC_CERT_CHK_OK)
   {
      /* Verify the device id in certificate*/
      chk = CheckDeviceId(device_cert);
      CC_DEBUG_PRINT(("CheckDeviceId reslut %d",chk));
   }

   FUNCTION_FINISH(CC_CheckCertificates);

   return chk;
}

/*---------------------------------------------------------------------------*/
/* local function definitions                                                */
/*---------------------------------------------------------------------------*/

/*!**************************************************************************
 * @brief    Check certificate issuer
 *
 * This function checks that the Issuer of the child certificate is identical
 * to the Subject of the parent certificate
 *
 * @param    parent_cert - CI+ parent (root, brand) certificate
 * @param    child_cert - CI+ child (brand, device) certificate
 * @return   Result of check
 ****************************************************************************/
static E_CC_CERT_CHK CheckIssuer(void *parent_cert, void *child_cert)
{
   E_CC_CERT_CHK chk;
   void *parent_subject;
   void *child_issuer;
   BOOL identical;

   FUNCTION_START(CheckIssuer);

   parent_subject = SECTK_GetCertificateSubject(parent_cert);
   child_issuer = SECTK_GetCertificateIssuer(child_cert);
   identical = SECTK_CompareCertificateName(parent_subject, child_issuer);
   if (identical)
   {
      chk = CC_CERT_CHK_OK;
   }
   else
   {
      chk = CC_CERT_CHK_SUBJECT_ISSUER_MISMATCH;
   }

   FUNCTION_FINISH(CheckIssuer);

   return chk;
}

/*!**************************************************************************
 * @brief    Check certificate validity period
 *
 * This function checks that the validity period of the certificate includes
 * the current date and time
 *
 * @param    cert - CI+ certificate
 * @return   Result of check
 ****************************************************************************/
static E_CC_CERT_CHK CheckValidityPeriod(void *cert,UINT32 epoch_time)
{
   E_CC_CERT_CHK chk;
   BOOL not_before, not_after;

   FUNCTION_START(CheckValidityPeriod);

   /* Note: Certificate terminology is confusing. The certificate
    *       contains "notBefore" and "notAfter".
    *
    *       If the current time is before the "notBefore" value, then
    *       the certificate validity is in the future (probably wrong
    *       date/time on the receiver).
    *
    *       If the current time is after the "notAfter" value, then the
    *       certificate has expired.
    */
   not_after = SECTK_CheckCertificateNotAfter(cert, epoch_time);
   if (not_after)
   {
      not_before = SECTK_CheckCertificateNotBefore(cert, epoch_time);
      if (not_before)
      {
         chk = CC_CERT_CHK_OK;
      }
      else
      {
         CC_DEBUG_PRINT(("{MW CI} check cam certification current_time >= certification validity.notBefore failed \n"));
         chk = CC_CERT_CHK_INVALID_DATE_TIME;
      }
   }
   else
   {
      CC_DEBUG_PRINT(("{MW CI} check cam certification current_time <= certification validity.notAfter failed,so cam ci plus key expired!!! \n"));
      chk = CC_CERT_CHK_EXPIRED;
   }

   FUNCTION_FINISH(CheckValidityPeriod);

   return chk;
}

/*!**************************************************************************
 * @brief    Check brand certificate extensions
 *
 * This function checks that each mandatory extension exists and the values
 * are valid. It also checks that no other extensions are marked as critical.
 *
 * @param    brand_cert - CI+ brand certificate
 * @param    root_cert - CI+ root certificate
 * @return   Result of check
 ****************************************************************************/
static E_CC_CERT_CHK CheckBrandExtensions(void *brand_cert, void *root_cert)
{
   E_CC_CERT_CHK chk;
   S_SECTK_CERT_EXT ext;
   U32BIT iter;
   BOOL key_usage_found;
   BOOL subject_key_identifier_found;
   BOOL authority_key_identifier_found;
   BOOL basic_constraints_found;
   BOOL success;

   FUNCTION_START(CheckBrandExtensions);

   chk = CC_CERT_CHK_OK;

   key_usage_found = FALSE;
   subject_key_identifier_found = FALSE;
   authority_key_identifier_found = FALSE;
   basic_constraints_found = FALSE;

   success = SECTK_GetFirstCertificateExtension(brand_cert, &iter, &ext);
   while (chk == CC_CERT_CHK_OK && success)
   {
      CC_DEBUG_PRINT(("%s\n", ext.critical ? "Critical" : "Not critical"));
      CC_DEBUG_PRINT(("Type = %s\n",
                      ext.type == SECTK_CERT_EXT_KEY_USAGE ? "SECTK_CERT_EXT_KEY_USAGE" :
                      ext.type == SECTK_CERT_EXT_SUBJECT_KEY_IDENTIFIER ? "SECTK_CERT_EXT_SUBJECT_KEY_IDENTIFIER" :
                      ext.type == SECTK_CERT_EXT_AUTHORITY_KEY_IDENTIFIER ? "SECTK_CERT_EXT_AUTHORITY_KEY_IDENTIFIER" :
                      ext.type == SECTK_CERT_EXT_BASIC_CONSTRAINTS ? "SECTK_CERT_EXT_BASIC_CONSTRAINTS" :
                      ext.type == SECTK_CERT_EXT_SCRAMBLER_CAPABILITIES ? "SECTK_CERT_EXT_SCRAMBLER_CAPABILITIES" :
                      ext.type == SECTK_CERT_EXT_CI_PLUS_INFO ? "SECTK_CERT_EXT_CI_PLUS_INFO" :
                      ext.type == SECTK_CERT_EXT_CICAM_BRAND_IDENTIFIER ? "SECTK_CERT_EXT_CICAM_BRAND_IDENTIFIER" :
                      ext.type == SECTK_CERT_EXT_UNKNOWN ? "SECTK_CERT_EXT_UNKNOWN" :
                      "???"));

      /* Check extension */
      switch (ext.type)
      {
      case SECTK_CERT_EXT_KEY_USAGE:
         if (!key_usage_found)
         {
            chk = CheckBrandKeyUsage(&ext);
            key_usage_found = TRUE;
         }
         else
         {
            chk = CC_CERT_CHK_INVALID_EXTENSION;
         }
         break;
      case SECTK_CERT_EXT_SUBJECT_KEY_IDENTIFIER:
         if (!subject_key_identifier_found)
         {
            chk = CheckBrandSubjectKeyIdentifier(&ext, brand_cert);
            subject_key_identifier_found = TRUE;
         }
         else
         {
            chk = CC_CERT_CHK_INVALID_EXTENSION;
         }
         break;
      case SECTK_CERT_EXT_AUTHORITY_KEY_IDENTIFIER:
         if (!authority_key_identifier_found)
         {
            chk = CheckBrandAuthorityKeyIdentifier(&ext, root_cert);
            authority_key_identifier_found = TRUE;
         }
         else
         {
            chk = CC_CERT_CHK_INVALID_EXTENSION;
         }
         break;
      case SECTK_CERT_EXT_BASIC_CONSTRAINTS:
         if (!basic_constraints_found)
         {
            chk = CheckBrandBasicConstraints(&ext);
            basic_constraints_found = TRUE;
         }
         else
         {
            chk = CC_CERT_CHK_INVALID_EXTENSION;
         }
         break;
      default:
         if (ext.critical)
         {
            /* Unexpected critical extension */
            chk = CC_CERT_CHK_INVALID_EXTENSION;
         }
      }

      success = SECTK_GetNextCertificateExtension(brand_cert, &iter, &ext);
   }

   if (!key_usage_found ||
       !subject_key_identifier_found ||
       !authority_key_identifier_found ||
       !basic_constraints_found)
   {
      chk = CC_CERT_CHK_MISSING_EXTENSION;
   }

   FUNCTION_FINISH(CheckBrandExtensions);

   return chk;
}

/*!**************************************************************************
 * @brief    Check Key Usage extension for brand certificate
 * @param    ext - extension structure
 * @return   Result of check
 ****************************************************************************/
static E_CC_CERT_CHK CheckBrandKeyUsage(S_SECTK_CERT_EXT *ext)
{
   E_CC_CERT_CHK chk;

   FUNCTION_START(CheckBrandKeyUsage);

   chk = CC_CERT_CHK_OK;
   if (!ext->critical ||
       ext->data.key_usage.digitalSignature ||
       ext->data.key_usage.nonRepudiation ||
       ext->data.key_usage.keyEncipherment ||
       ext->data.key_usage.dataEncipherment ||
       ext->data.key_usage.keyAgreement ||
       !ext->data.key_usage.keyCertSign ||
       ext->data.key_usage.cRLSign ||
       ext->data.key_usage.encipherOnly ||
       ext->data.key_usage.decipherOnly)
   {
      chk = CC_CERT_CHK_INVALID_EXTENSION;
   }

   FUNCTION_FINISH(CheckBrandKeyUsage);

   return chk;
}

/*!**************************************************************************
 * @brief    Check Subject Key Identifier extension for brand certificate
 * @param    ext - extension structure
 * @param    brand_cert - CI+ brand certificate
 * @return   Result of check
 ****************************************************************************/
static E_CC_CERT_CHK CheckBrandSubjectKeyIdentifier(S_SECTK_CERT_EXT *ext,
                                                    void *brand_cert)
{
   U8BIT *key_identifier;
   E_CC_CERT_CHK chk;

   FUNCTION_START(CheckBrandSubjectKeyIdentifier);

   chk = CC_CERT_CHK_OK;

   if (ext->critical)
   {
      /* This extension MUST NOT be marked critical (RFC 3280) */
      chk = CC_CERT_CHK_INVALID_EXTENSION;
   }
   else
   {
      key_identifier = ext->data.subject_key_identifier.keyIdentifier;
      if (!SECTK_VerifyCertificateKeyIdentifier(brand_cert, key_identifier))
      {
         chk = CC_CERT_CHK_INVALID_EXTENSION;
      }
   }

   FUNCTION_FINISH(CheckBrandSubjectKeyIdentifier);

   return chk;
}

/*!**************************************************************************
 * @brief    Check Authority Key Identifier extension for brand certificate
 * @param    ext - extension structure
 * @param    root_cert - CI+ root certificate
 * @return   Result of check
 ****************************************************************************/
static E_CC_CERT_CHK CheckBrandAuthorityKeyIdentifier(S_SECTK_CERT_EXT *ext,
                                                      void *root_cert)
{
   U8BIT *key_identifier;
   E_CC_CERT_CHK chk;

   FUNCTION_START(CheckBrandAuthorityKeyIdentifier);

   chk = CC_CERT_CHK_OK;

   if (ext->critical)
   {
      /* This extension MUST NOT be marked critical (RFC 3280) */
      chk = CC_CERT_CHK_INVALID_EXTENSION;
   }
   else
   {
      key_identifier = ext->data.authority_key_identifier.keyIdentifier;
      if (!SECTK_VerifyCertificateKeyIdentifier(root_cert, key_identifier))
      {
         chk = CC_CERT_CHK_INVALID_EXTENSION;
      }
   }

   FUNCTION_FINISH(CheckBrandAuthorityKeyIdentifier);

   return chk;
}

/*!**************************************************************************
 * @brief    Check Basic Constraints extension for brand certificate
 * @param    ext - extension structure
 * @return   Result of check
 ****************************************************************************/
static E_CC_CERT_CHK CheckBrandBasicConstraints(S_SECTK_CERT_EXT *ext)
{
   E_CC_CERT_CHK chk;

   FUNCTION_START(CheckBrandBasicConstraints);

   chk = CC_CERT_CHK_OK;
   if (!ext->critical ||
       !ext->data.basic_constraints.cA ||
       !ext->data.basic_constraints.pathLenConstraint_provided ||
       ext->data.basic_constraints.pathLenConstraint != 0)
   {
      chk = CC_CERT_CHK_INVALID_EXTENSION;
   }

   FUNCTION_FINISH(CheckBrandBasicConstraints);

   return chk;
}

/*!**************************************************************************
 * @brief    Check device certificate extensions
 *
 * This function checks that each mandatory extension exists and the values
 * are valid. It also checks that no other extensions are marked as critical.
 *
 * @param    device_cert - CI+ device certificate
 * @param    brand_cert - CI+ brand certificate
 * @return   Result of check
 ****************************************************************************/
static E_CC_CERT_CHK CheckDeviceExtensions(void *device_cert, void *brand_cert)
{
   E_CC_CERT_CHK chk;
   S_SECTK_CERT_EXT ext;
   U32BIT iter;
   BOOL key_usage_found;
   BOOL authority_key_identifier_found;
   BOOL basic_constraints_found;
   BOOL scrambler_capabilities_found;
   BOOL cicam_brand_identifier_found;
   BOOL success;

   FUNCTION_START(CheckDeviceExtensions);

   chk = CC_CERT_CHK_OK;

   key_usage_found = FALSE;
   authority_key_identifier_found = FALSE;
   basic_constraints_found = FALSE;
   scrambler_capabilities_found = FALSE;
   cicam_brand_identifier_found = FALSE;

   success = SECTK_GetFirstCertificateExtension(device_cert, &iter, &ext);
   while (chk == CC_CERT_CHK_OK && success)
   {
      CC_DEBUG_PRINT(("%s\n", ext.critical ? "Critical" : "Not critical"));
      CC_DEBUG_PRINT(("Type = %s\n",
                      ext.type == SECTK_CERT_EXT_KEY_USAGE ? "SECTK_CERT_EXT_KEY_USAGE" :
                      ext.type == SECTK_CERT_EXT_SUBJECT_KEY_IDENTIFIER ? "SECTK_CERT_EXT_SUBJECT_KEY_IDENTIFIER" :
                      ext.type == SECTK_CERT_EXT_AUTHORITY_KEY_IDENTIFIER ? "SECTK_CERT_EXT_AUTHORITY_KEY_IDENTIFIER" :
                      ext.type == SECTK_CERT_EXT_BASIC_CONSTRAINTS ? "SECTK_CERT_EXT_BASIC_CONSTRAINTS" :
                      ext.type == SECTK_CERT_EXT_SCRAMBLER_CAPABILITIES ? "SECTK_CERT_EXT_SCRAMBLER_CAPABILITIES" :
                      ext.type == SECTK_CERT_EXT_CI_PLUS_INFO ? "SECTK_CERT_EXT_CI_PLUS_INFO" :
                      ext.type == SECTK_CERT_EXT_CICAM_BRAND_IDENTIFIER ? "SECTK_CERT_EXT_CICAM_BRAND_IDENTIFIER" :
                      ext.type == SECTK_CERT_EXT_UNKNOWN ? "SECTK_CERT_EXT_UNKNOWN" :
                      "???"));

      /* Check extension */
      switch (ext.type)
      {
      case SECTK_CERT_EXT_KEY_USAGE:
         if (!key_usage_found)
         {
            chk = CheckDeviceKeyUsage(&ext);
            key_usage_found = TRUE;
         }
         else
         {
            chk = CC_CERT_CHK_INVALID_EXTENSION;
         }
         break;
      case SECTK_CERT_EXT_AUTHORITY_KEY_IDENTIFIER:
         if (!authority_key_identifier_found)
         {
            chk = CheckDeviceAuthorityKeyIdentifier(&ext, brand_cert);
            authority_key_identifier_found = TRUE;
         }
         else
         {
            chk = CC_CERT_CHK_INVALID_EXTENSION;
         }
         break;
      case SECTK_CERT_EXT_BASIC_CONSTRAINTS:
         if (!basic_constraints_found)
         {
            chk = CheckDeviceBasicConstraints(&ext);
            basic_constraints_found = TRUE;
         }
         else
         {
            chk = CC_CERT_CHK_INVALID_EXTENSION;
         }
         break;
      case SECTK_CERT_EXT_SCRAMBLER_CAPABILITIES:
         if (!scrambler_capabilities_found)
         {
            chk = CheckDeviceScramblerCapabilities(&ext);
            scrambler_capabilities_found = TRUE;
         }
         else
         {
            chk = CC_CERT_CHK_INVALID_EXTENSION;
         }
         break;
      case SECTK_CERT_EXT_CICAM_BRAND_IDENTIFIER:
         if (!cicam_brand_identifier_found)
         {
            chk = CheckDeviceCicamBrandIdentifier(&ext);
            cicam_brand_identifier_found = TRUE;
         }
         else
         {
            chk = CC_CERT_CHK_INVALID_EXTENSION;
         }
         break;
      default:
         if (ext.critical)
         {
            /* Unexpected critical extension */
            chk = CC_CERT_CHK_INVALID_EXTENSION;
         }
      }

      success = SECTK_GetNextCertificateExtension(device_cert, &iter, &ext);
   }

   if (!key_usage_found ||
       !authority_key_identifier_found ||
       !basic_constraints_found ||
       !scrambler_capabilities_found)
   {
      chk = CC_CERT_CHK_MISSING_EXTENSION;
   }

   #ifndef CICAM_BRAND_ID_OPTIONAL
   if (!cicam_brand_identifier_found)
   {
      chk = CC_CERT_CHK_MISSING_EXTENSION;
   }
   #endif  /* CICAM_BRAND_ID_OPTIONAL */

   FUNCTION_FINISH(CheckDeviceExtensions);

   return chk;
}

/*!**************************************************************************
 * @brief    Check Key Usage extension for device certificate
 * @param    ext - extension structure
 * @return   Result of check
 ****************************************************************************/
static E_CC_CERT_CHK CheckDeviceKeyUsage(S_SECTK_CERT_EXT *ext)
{
   E_CC_CERT_CHK chk;

   FUNCTION_START(CheckDeviceKeyUsage);

   chk = CC_CERT_CHK_OK;
   if (!ext->critical ||
       !ext->data.key_usage.digitalSignature ||
       ext->data.key_usage.nonRepudiation ||
       ext->data.key_usage.keyEncipherment ||
       ext->data.key_usage.dataEncipherment ||
       ext->data.key_usage.keyAgreement ||
       ext->data.key_usage.keyCertSign ||
       ext->data.key_usage.cRLSign ||
       ext->data.key_usage.encipherOnly ||
       ext->data.key_usage.decipherOnly)
   {
      chk = CC_CERT_CHK_INVALID_EXTENSION;
   }

   FUNCTION_FINISH(CheckDeviceKeyUsage);

   return chk;
}

/*!**************************************************************************
 * @brief    Check Authority Key Identifier extension for device certificate
 * @param    ext - extension structure
 * @param    brand_cert - CI+ brand certificate
 * @return   Result of check
 ****************************************************************************/
static E_CC_CERT_CHK CheckDeviceAuthorityKeyIdentifier(S_SECTK_CERT_EXT *ext,
                                                       void *brand_cert)
{
   U8BIT *key_identifier;
   E_CC_CERT_CHK chk;

   FUNCTION_START(CheckDeviceAuthorityKeyIdentifier);

   chk = CC_CERT_CHK_OK;

   if (ext->critical)
   {
      /* This extension MUST NOT be marked critical (RFC 3280) */
      chk = CC_CERT_CHK_INVALID_EXTENSION;
   }
   else
   {
      key_identifier = ext->data.authority_key_identifier.keyIdentifier;
      if (!SECTK_VerifyCertificateKeyIdentifier(brand_cert, key_identifier))
      {
         chk = CC_CERT_CHK_INVALID_EXTENSION;
      }
   }

   FUNCTION_FINISH(CheckDeviceAuthorityKeyIdentifier);

   return chk;
}

/*!**************************************************************************
 * @brief    Check Basic Constraints extension for device certificate
 * @param    ext - extension structure
 * @return   Result of check
 ****************************************************************************/
static E_CC_CERT_CHK CheckDeviceBasicConstraints(S_SECTK_CERT_EXT *ext)
{
   E_CC_CERT_CHK chk;

   FUNCTION_START(CheckDeviceBasicConstraints);

   chk = CC_CERT_CHK_OK;
   if (!ext->critical ||
       ext->data.basic_constraints.cA ||
       ext->data.basic_constraints.pathLenConstraint_provided)
   {
      chk = CC_CERT_CHK_INVALID_EXTENSION;
   }

   FUNCTION_FINISH(CheckDeviceBasicConstraints);

   return chk;
}

/*!**************************************************************************
 * @brief    Check Scrambler Capabilities extension for device certificate
 * @param    ext - extension structure
 * @return   Result of check
 ****************************************************************************/
static E_CC_CERT_CHK CheckDeviceScramblerCapabilities(S_SECTK_CERT_EXT *ext)
{
   E_CC_CERT_CHK chk;

   FUNCTION_START(CheckDeviceScramblerCapabilities);

   chk = CC_CERT_CHK_OK;
   if (!ext->critical ||
       ext->data.scrambler_capabilities.capability < 0 ||
       ext->data.scrambler_capabilities.capability > 1)
   {
      chk = CC_CERT_CHK_INVALID_EXTENSION;
   }

   FUNCTION_FINISH(CheckDeviceScramblerCapabilities);

   return chk;
}

/*!**************************************************************************
 * @brief    Check CICAM Brand Identifier extension for device certificate
 * @param    ext - extension structure
 * @return   Result of check
 ****************************************************************************/
static E_CC_CERT_CHK CheckDeviceCicamBrandIdentifier(S_SECTK_CERT_EXT *ext)
{
   E_CC_CERT_CHK chk;

   FUNCTION_START(CheckDeviceCicamBrandIdentifier);

   chk = CC_CERT_CHK_OK;
   if (ext->critical ||
       ext->data.cicam_brand_identifier.cicamBrandId < 1 ||
       ext->data.cicam_brand_identifier.cicamBrandId > 65535)
   {
      chk = CC_CERT_CHK_INVALID_EXTENSION;
   }

   FUNCTION_FINISH(CheckDeviceCicamBrandIdentifier);

   return chk;
}

/*!**************************************************************************
 * @brief    Check key register between certificates
 *
 * This function verifies that the autority key indentifier in the child
 * certificate is identical to the subject key identifier in the parent
 * certificate.
 *
 * @param    parent_cert - CI+ parent (root, brand) certificate
 * @param    child_cert - CI+ child (brand, device) certificate
 * @return   Result of check
 ****************************************************************************/
static E_CC_CERT_CHK CheckKeyIdentifier(void *parent_cert, void *child_cert)
{
   E_CC_CERT_CHK chk;
   S_SECTK_CERT_EXT parent_ext;
   S_SECTK_CERT_EXT child_ext;
   BOOL success;

   FUNCTION_START(CheckKeyIdentifier);

   chk = CC_CERT_CHK_INVALID_EXTENSION;

   memset(&parent_ext, 0, sizeof parent_ext);
   memset(&child_ext, 0, sizeof child_ext);
   success = SECTK_GetCertificateExtension(parent_cert, SECTK_CERT_EXT_SUBJECT_KEY_IDENTIFIER, &parent_ext);
   if (success)
   {
      success = SECTK_GetCertificateExtension(child_cert, SECTK_CERT_EXT_AUTHORITY_KEY_IDENTIFIER, &child_ext);
   }
   if (success)
   {
      if (memcmp(parent_ext.data.subject_key_identifier.keyIdentifier,
                 child_ext.data.authority_key_identifier.keyIdentifier,
                 20) == 0)
      {
         chk = CC_CERT_CHK_OK;
      }
   }

   FUNCTION_FINISH(CheckKeyIdentifier);

   return chk;
}

/*!**************************************************************************
 * @brief    Verify certificate signature
 *
 * This function verifies the certificate's signature by using the RSASSA-PSS
 * verification described in RSA PKCS#1 [976H [1]], section 8.1.2.
 *
 * @param    parent_cert - CI+ parent (root, brand) certificate
 * @param    child_cert - CI+ child (brand, device) certificate
 * @return   Result of check
 ****************************************************************************/
static E_CC_CERT_CHK CheckCertificateSignature(void *parent_cert,
                                               void *child_cert)
{
   E_CC_CERT_CHK chk;
   BOOL verify;

   FUNCTION_START(CheckCertificateSignature);

   chk = CC_CERT_CHK_INVALID_SIGNATURE;
   verify = SECTK_VerifyCertificate(child_cert, parent_cert);
   if (verify)
   {
      chk = CC_CERT_CHK_OK;
   }

   FUNCTION_FINISH(CheckCertificateSignature);

   return chk;
}
/*!**************************************************************************
 * @brief    Verify certificate Device id
 *
 * @param    cert - CI+ parent device certificate
 * @return   Result of check
 ****************************************************************************/

static E_CC_CERT_CHK CheckDeviceId(void *cert)
{
   E_CC_CERT_CHK chk;
   BOOL verify;

   FUNCTION_START(CheckDeviceId);

   chk = CC_CERT_CHK_INVALID_SIGNATURE;
   verify = SECTK_VerifyDeviceId(cert);
   if (verify)
   {
      chk = CC_CERT_CHK_OK;
   }

   FUNCTION_FINISH(CheckDeviceId);

   return chk;
}


/*****************************************************************************/
/* End of File                                                               */
/*****************************************************************************/
