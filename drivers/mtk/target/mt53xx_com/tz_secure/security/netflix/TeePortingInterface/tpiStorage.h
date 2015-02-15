/*
 * tpiStorage.h
 *
 *  Created on: Jan 31, 2014
 *      Author: scott
 */

#ifndef TPISTORAGE_H_
#define TPISTORAGE_H_

/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef STORAGEAGENTINTERNALS_H_
#define STORAGEAGENTINTERNALS_H_

/*
 * This file contains definitions relating to the reference secure store
 * implementation.
 *
 * Implementing the secure store in the TEE has some definite challenges. The
 * secure store is essentially a database, but given the sensitivity of the
 * TEE, you need to minimize the amount of code used to implement this. So,
 * you need to implement _a very simple_ database. That's what we try to
 * accomplish here.
 *
 * You should only include this file if (1) your code is part of the runtime
 * secure store implementation (in the TEE), or (2) you are writing a
 * provisioning app that creates the "manufacturing" secure store.
 *
 * Put slightly differently:
 * ----------------------------------------------------------------------------
 *  THE DEFINITIONS IN THIS FILE ARE VERY SENSITIVE, AND SHOULD NOT BE VISIBLE
 *  OUTSIDE OF THE TEE AND/OR THE PROVISIONING APPLICATION!!!
 * ----------------------------------------------------------------------------
 */



/*
 * This reference secure store implementation uses multiple files to store
 * protected data. In a "real" TEE, you would need APIs to the rich OS in order
 * to read/write these files, but in this reference implementation, we abstract
 * the file-related calls and use the Linux fileystem.
 *
 * The secure store consists of an arbitrary number of sections, with one
 * section for each secure store client. Each client has an ID, and this ID
 * is combined with a (simulated) hardware key to create client-specific
 * integrity and encryption keys.
 *
 * Here are the files we currently use:
 *
 * - manufacturing secure store (manuf_ss.bin)
 *		- contains read-only data (management section, ESN,Kpe,Kph, PlayReady model keys)
 *      - this file is considered read-only. You could add APIs for field reprovisioning,
 *        but you would definitely want to isolate these from "normal" APIs. We don't
 *        currently provide this capability in the reference TEE; if you need to replace
 *        credentials, you should generate a new manufacturing secure store file.
 *
 *
 * Each section is access controlled. Currently, this utilizes only a client ID.
 * The client ID is 16 bytes (128 bits). Note that any client that has this
 * ID can read/write the data in the associated section, so it is important to
 * think about this when you create these IDs, and when you add them to this
 * file. It means this file contains a password list that must be protected.
 *
 * The manufacturing secure store file is formatted as follows:
 *
 * File Header:
 *
 *  <FILE MAGIC1>
 *  <FILE MAGIC2>
 *  <VERSION>
 *  <Total Length>
 *  <HMAC-SHA256 of File Header>
 *
 * NOTE: Anywhere a length is used, it is the length of everything following
 *       it. So, <Total Length> above includes the HMAC and everything
 *       following, but it does not include itself or the file magic.
 *
 * File body:
 *
 * <Mgmt section>
 * <NTBA RO section>
 * <DRM RO section>
 * <EOF>
 *
 * I've added line-breaks to make this comment more readable, but the file is
 * binary, so everything kind of runs together. The file header and section
 * headers are unencrypted; everything else is encrypted.
 *
 * file magic and version values are defined below. All values are stored
 * in big-endian format
 *
 * The management section contains data needed by the secure store agent to
 * manage the secure store. Right now, it contains only the mapping between
 * nfSectionID values and Client ID values.

 * Each client section is specific to some client, and is formatted like this:
 *
 * <unique ID><16-bit section length>
 * <encrypted data>
 *
 * The <unique ID> is a 32-bit value that maps to the client ID. We need an
 * identifier to let us know what key and client ID correspond to each
 * section, and it's simplest if we can include this in the secure store in
 * plaintext. The unique ID is defined here for that purpose.
 *
 * If we decide later to add support for dynamic secure store creation (e.g.
 * by a rich OS application that is added after shipping), the secure store
 * application can simply generate a new, unique value that doesn't match any
 * of the others. Since this value is not security-sensitive, it can be
 * implemented as a counter, and the new client can be given the next available
 * value.
 *
 * The encrypted data is of the following format:
 *
 *       <16-bit pad length>
 *       <key><length><data>
 *       <key><length><data>
 *              :
 *       <key><length><data>
 *       <optional padding (pkcs7), added only if block cipher requires it>
 *       <sha1 hash of entire record>
 *     ---------------------------------------------------------------------
 *
 * For key-length-value elements, Keys are 32-bit application-defined values
 * that must be unique within the scope of that particular section. Length is
 * a 16-bit value, so maximum data length is 65535 (O is never valid). As with
 * all length values in this file, this is the length of <data> only.
 *
 * The following operations are supported:
 *
 *  - insert value
 *  - delete value
 *  - retrieve value
 *
 * Note that since the key value is defined by the application, you can easily
 * shoot yourself in the foot. If you insert a value that already exists, it
 * will overwrite the original value. Be careful.
 *
 */

/*
 * Current manufacturing secure store looks like this:
 *
 * Field                                  Size
 *--------------------------------------------
 * MAGIC1                              4 bytes
 * MAGIC2                              4 bytes
 * VERSION                             2 bytes
 * TOTAL LENGTH                        2 bytes
 * HEADER HMAC                        32 bytes
 *----------------------------------------------- Header total: 44 bytes (fixed)
 *===================================================================
 *--------------------- BEGIN ENCRYPTED DATA ------------------------
 *===================================================================
 * PADDING LENGTH                      2 bytes (for AES)
 *------------------------------------------------ padding total:2 bytes (fixed)
 * TEE MGMT Section ID                 4 bytes
 * section length                      2 bytes
 * NTBA RO section ID                  4 bytes
 * NTBA RO client ID                  16 bytes
 * NTBA RW section ID                  4 bytes
 * NTBA RW client ID                  16 bytes
 * DRM RO section ID                  4 bytes
 * DRM RO client ID                  16 bytes
 * DRM RW section ID                  4 bytes
 * DRM RW client ID                  16 bytes
 * NRD section ID                     4 bytes
 * NRD client ID                     16 bytes
 *------------------------------------------------ Mgmt section total:106 bytes (fixed)
 * NTBA RO Section ID                  4 bytes
 * section length                      2 bytes
 * ESN key                             4 bytes
 * length                              2 bytes
 * data (ESN)                         42 bytes (variable, 11-42 bytes)
 * KPE key                             4 bytes
 * length                              2 bytes
 * data (128 aes key)                 16 bytes
 * KPH key                             4 bytes
 * length                              2 bytes
 * data (SHA-256 key)                 32 bytes
 *----------------------------------------------- NTBA RO section total: 126 bytes (maximum)
 * DRM RO Section ID                   4 bytes
 * section length                      2 bytes
 * MODEL_PRIV Key                      4 bytes
 * length                              2 bytes
 * data (private key)                 32 bytes (256 bit ECC key)
 * MODEL_CERT Key                      4 bytes
 * length                              2 bytes
 * data (model cert)                1596 bytes
 * DEVCERT_TEMPLATE Key                4 bytes
 * length                              2 bytes
 * data (device cert template)      3974 bytes
 *----------------------------------------------- DRM RO section total: 5628 bytes (fixed)
 * padding (for AES)                  15 bytes (max, may be 0)
 * MAC value                          20 bytes (SHA1 of all sections, including pad)
 *----------------------------------------------- TRAILER total: 35 bytes (maximum)
 *===================================================================
 *---------------------- END ENCRYPTED DATA -------------------------
 *===================================================================
 *
 * If we add anything else, this will grow, so be sure to adjust
 * nsMAX_SS_PROVISION_SIZE definition in teetest.c...
 *
 * NOTE: this file does not change under "normal" conditions. It could be changed via
 * field-reprovisioning, but this should be a rare event.
 *
 * EACH section of the dynamic secure store is placed in its own file. Those
 * files have the following format:
 * 	- filename: sstore_<nfSectionID as hex>.bin (e.g. sstore_0000EE00.bin for NRD application)
 *
 * - file format
 *       <nfSectionID> (4 bytes)
 *       <16-bit length> (total length of encrypted data)
 *       <16-bit version> (same as manuf. secure store version)
 *       <file header hmac> (hmac-sha256)
 *----------------------------------------------- Header total: 40 bytes (fixed)
 *===================================================================
 *--------------------- BEGIN ENCRYPTED DATA ------------------------
 *===================================================================
 *       <16-bit pad length>
 *       <key><length><data>
 *       <key><length><data>
 *              :
 *       <key><length><data>
 *       <optional padding (pkcs7), added only if block cipher requires it>
 *       <sha1 hash of entire record>
 *===================================================================
 *---------------------- END ENCRYPTED DATA -------------------------
 *===================================================================
 *
 *
 */

#define nfSS_MAX_FILE_SIZE	8092 // max secure store file size

#define nfSS_MGMT_SECTION_SIZE  106

#define nfMANUF_SECSTORE_FILENAME "manuf_ss.bin"

#ifdef nfSS_NEED_KEYS
/*
 * NOTE: secure store requires an AES key as the key ladder base, and
 * and an HMAC key. for validating the header. For now, I am
 * defining these here. In a real system, these would be loaded/derived
 * from OTP somehow, but they will still have to be somehow available to
 * the manufacturing provisioning system.
 */

uint8_t _SecStoreHmacKey[32] = { 0x4b, 0x40, 0xf4, 0x7e, 0xf6, 0x29, 0xd0, 0x1c,
                              0x1f, 0x37, 0xe8, 0x3d, 0x3d, 0x30, 0xcd, 0x28,
                              0x70, 0x22, 0xee, 0x40, 0x10, 0xf5, 0x81, 0x61,
                              0x64, 0x2a, 0xe1, 0x7e, 0xe1, 0x7e, 0xec, 0x19 };

uint8_t _SecStoreAesKey[16] = { 0xd8, 0x95, 0x7a, 0x33, 0x14, 0x4d, 0xe9, 0x30,
                             0x02, 0xcd, 0x32, 0x89, 0x6e, 0x57, 0xdf, 0xfc };

#endif /* nfSS_NEED_KEYS */

// the following definitions depend on this file version
#define nfSS_FILE_VERSION_SIZE  2
#define nfSS_FILE_VERSION       0x0001

// total length field size
#define nfSS_FILE_LENGTH_SIZE   2 /* sizeof(uint16_t) */

// file magic for secure store file (first two words of file)
#define nfSS_FILE_MAGIC_SIZE    8
#define nfSS_FILE_MAGIC1        0xF00DFEED
#define nfSS_FILE_MAGIC2        0xFADEFEED

// each client gets its own section, which has its own magic number
#define nfSS_SECTION_MAGIC  0xFEEDBEEF

#define nfSS_FILE_HEADER_SIZE  (nfSS_FILE_MAGIC_SIZE + nfSS_FILE_VERSION_SIZE + nfSS_FILE_LENGTH_SIZE)
#define nfSS_HEADER_HMAC_SIZE   32  // HMAC_SHA256 is 32 bytes

#define nfSS_RECORD_ICV_SIZE   20 // we are currently using SHA1 as the ICV

/*
 * Following are the currently defined ("authorized") secure store clients
 * that live in the TEE. Note that for NTBA, we define two client IDs; this
 * allows segregation of the read-only elements, so that we don't have to
 * recompute the integrity hash over those every time the read-write stuff
 * (e.g. Kce/Kch) changes.
 */
#define nfSS_CLIENT_ID_SIZE     16 /* client ID is 16 bytes */

// NB: special client ID for tee mgmt (all 0x00)
#define nfSS_CLIENT_ID_TEE_MGMT   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}

#define nfSS_CLIENT_ID_TEE_NTBA_RO   {0x9d,0x4a,0x88,0xc4,0x32,0x62,0x78,0x7c,0x8f,0x27,0xba,0xed,0xa8,0x71,0x61,0x85}
#define nfSS_CLIENT_ID_TEE_NTBA_RW   {0x72,0x90,0xae,0x07,0xbd,0x8d,0x07,0x74,0xac,0x6a,0xde,0x07,0xd5,0xa7,0xce,0xc9}

// DRM agent has two different sections, one for model key(s) and one for device keys
#define nfSS_CLIENT_ID_TEE_DRM_RO  {0xb9,0x0e,0x81,0x64,0x91,0x67,0x59,0xaa,0xa8,0x63,0x1d,0x0d,0xc9,0xac,0xb8,0x66}
#define nfSS_CLIENT_ID_TEE_DRM_RW  {0xc3,0x1e,0x29,0x98,0x11,0x00,0x6c,0x5a,0xff,0x1f,0xdd,0x73,0x29,0x90,0x0b,0x01}


// NRD app has one client ID
#define nfSS_CLIENT_ID_NRD_APP  {0x15,0xe1,0x25,0xe9,0x11,0x00,0xF1,0x99,0x03,0x33,0xd1,0xfd,0x90,0x87,0xbb,0x40}

/*
 * The section IDs are visible (unencrypted) in the secure store; only the
 * secstore mgmt code can map the section IDs to client IDs
 */
typedef enum nfSectionID
{
    // add section IDs for NRD app here...
    teeSID_NRD_APP1 = 0x0000EE00,

    // add section IDs for TEE agents below...
    teeSID_TEE_MGMT = 0xF00FEE00, // tee agent section IDs must be >= this
    teeSID_NTBA_RO = 0xF00FEE01,
    teeSID_NTBA_RW = 0xF00FEE02,
    teeSID_DRM_RO = 0xF00FEE03,
    teeSID_DRM_RW = 0xF00FEE04,
} nfSectionID_t;


// write-protect flag -- normal insert/delete routines cannot modify these records
#define nfSS_WRITE_PROTECT_FLAG 0x80000000

#define nfSS_IS_WRITE_PROTECTED(x) (((x) & nfSS_WRITE_PROTECT_FLAG) != 0)

/*
 * NOTE: client keys must be unique within that client's domain. If you
 * add more NTBA keys, make sure they are different than the ones defined
 * below. Using an enum is a good way to ensure that this is true.
 *
 * Here are the keys used by the NTBA agent for ESN, Kpe, Kph:
 */
#if 0
enum ntba_RW_AgentClientKeys
{
    // these elements are dynamically updated
    ntbaAGENT_KEY_KCE = 0x00000001,
    ntbaAGENT_KEY_KCH = 0x00000002,
};
#endif


#endif /* STORAGEAGENTINTERNALS_H_ */


#endif /* TPISTORAGE_H_ */
