/*******************************************************************************
 *
 * Copyright 2013 Google Inc. All Rights Reserved.
 *
 * mock implementation of OEMCrypto APIs
 *
 ******************************************************************************/

#ifndef OEMCRYPTO_KEY_CTRL_BLK_MOCK_H_
#define OEMCRYPTO_KEY_CTRL_BLK_MOCK_H_

#include "mtkutil_vector.h"

//=============================
//KEY CTRL BLK
//=============================

#define  kControlObserveDataPath  (1<<31)
#define  kControlObserveHDCP      (1<<30)
#define  kControlObserveCGMS      (1<<29)


#define  kControlAllowEncrypt     (1<<8)
#define  kControlAllowDecrypt     (1<<7)
#define  kControlAllowSign        (1<<6)
#define  kControlAllowVerify      (1<<5)
#define  kControlDataPathSecure   (1<<4)
#define  kControlNonceEnabled     (1<<3)
#define  kControlHDCPRequired     (1<<2)
#define  kControlCGMSMask         (0x03)
#define  kControlCGMSCopyFreely   (0x00)
#define  kControlCGMSCopyOnce     (0x02)
#define  kControlCGMSCopyNever    (0x03)

//Added for API Level 9
#define  kControlReplayMask        (0x03<<13)
#define  kControlNonceRequired     (0x01<<13)
#define  kControlNonceOrEntry      (0x02<<13)
#define  kControlHDCPVersionShift  (9)
#define  kControlHDCPVersionMask   (0x0F<<kControlHDCPVersionShift)

typedef struct KeyControlBlock {

  // Defualt Value need to be take care by user
  //KeyControlBlock(bool refresh) : valid_(false), refresh_(refresh) {}

  bool valid;
  bool refresh;
  uint32_t verification;
  uint32_t duration;
  uint32_t nonce;
  uint32_t control_bits;
}KeyControlBlock ;

void KCBInit(KeyControlBlock *this);

void KCBInitRefresh(KeyControlBlock *this, bool refresh);

static inline void KCBTerm(KeyControlBlock *this){};

void KCBInvalidate(KeyControlBlock *this);

bool KCBSetFromString(KeyControlBlock *this ,const Vector * key_control_string);

bool KCBValidate(KeyControlBlock *this);

#endif
