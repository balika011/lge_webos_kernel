/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/


#ifndef TZ_NAUTILUS_H
#define TZ_NAUTILUS_H


#define TZ_CHECK    0

#ifdef MODULE

//
// TZ driver only
//

#include "x_typedef.h"
#include "u_handle.h"

//
// TZ basic service
//
#define TZ_DRIVER_REMOVE_ID     0
#define TZ_DRIVER_INSTALL_ID    1

extern int TzServiceCall(HANDLE_T hDriver, ...);
extern HANDLE_T TzDriverInstall(void *phy, size_t size);
extern void TzDriverRemove(HANDLE_T hDriver);

extern HANDLE_T g_hSecureHandle;

#else // MODULE

//
// Secure code only
//

//#include <stddef.h>

#define TzCritStart(x) \
    asm volatile ( \
    "mrs	%0, cpsr\n" \
    "	cpsid	if" \
    : "=r" (x) : : "memory", "cc")

#define TzCritEnd(x) \
    asm volatile ( \
    "msr	cpsr_c, %0"	\
    : : "r" (x) : "memory", "cc")

extern void HalFlushInvalidateDCache(void);
extern void *TzSecureMemoryAllocate(unsigned long  size);
extern void TzSecureMemoryRelease(void *ptr);
extern void TzYield(int retval);
extern void Tz_BIM_EnableIrq(unsigned long u4Vector);
extern void Tz_BIM_DisableIrq(unsigned long u4Vector);
extern void Tz_BIM_ClearIrq(unsigned long u4Vector);

extern unsigned long __copy_from_normal(void *to, const void *from, unsigned long n);
extern unsigned long __copy_to_normal(void *to, const void *from, unsigned long n);

#ifndef CC_SECURE_WORLD
static inline unsigned long copy_from_normal(void *to, const void *from, unsigned long n)
{
    n = __copy_from_normal(to, from, n);
    return n;
}

static inline unsigned long copy_to_normal(void *to, const void *from, unsigned long n)
{
    n = __copy_to_normal(to, from, n);
    return n;
}
#endif
#endif // MODULE

#include "u_tz_def.h"

//
// shared
//
#define TZ_DRIVER_EXIT_ID                           (TZ_IDBOUND_KRN + 0)
#define TZ_DRIVER_INIT_ID                           (TZ_IDBOUND_KRN + 1)
#define TZ_DRIVER_TEST_ID                           (TZ_IDBOUND_KRN + 2)

#define TZ_GCPU_HAL_INIT_ID                         (TZ_IDBOUND_KRN + 3)
#define TZ_GCPU_HAL_UNINIT_ID                       (TZ_IDBOUND_KRN + 4)
#define TZ_GCPU_HAL_RESET_ID                        (TZ_IDBOUND_KRN + 5)
#define TZ_GCPU_CMDSET_ID                           (TZ_IDBOUND_KRN + 6)
#define TZ_GCPU_CMDGET_ID                           (TZ_IDBOUND_KRN + 7)
#define TZ_GCPU_CMDEXE_ID                           (TZ_IDBOUND_KRN + 8)
#define TZ_GCPU_CMDEXE_NOINTR_ID                    (TZ_IDBOUND_KRN + 9)
#define TZ_GCPU_CLRINTR_ID                          (TZ_IDBOUND_KRN + 10)
#define TZ_GCPU_CSSCMD_ID                           (TZ_IDBOUND_KRN + 11)
#define TZ_GCPU_CSSRST_ID                           (TZ_IDBOUND_KRN + 12)
#define TZ_GCPU_CPPMCMD_ID                          (TZ_IDBOUND_KRN + 13)
#define TZ_GCPU_CPPMRST_ID                          (TZ_IDBOUND_KRN + 14)
#define TZ_GCPU_AESNOINTCMD_ID                      (TZ_IDBOUND_KRN + 15)
#define TZ_GCPU_AESCMD_ID                           (TZ_IDBOUND_KRN + 16)
#define TZ_GCPU_AESRST_ID                           (TZ_IDBOUND_KRN + 17)
#define TZ_GCPU_VCPSCMD_ID                          (TZ_IDBOUND_KRN + 18)
#define TZ_GCPU_VCPSRST_ID                          (TZ_IDBOUND_KRN + 19)
#define TZ_GCPU_AACSCMD_ID                          (TZ_IDBOUND_KRN + 20)
#define TZ_GCPU_AACSRST_ID                          (TZ_IDBOUND_KRN + 21)
#define TZ_GCPU_DESCMD_ID                           (TZ_IDBOUND_KRN + 22)
#define TZ_GCPU_DESRST_ID                           (TZ_IDBOUND_KRN + 23)
#define TZ_GCPU_BDRECMD_ID                          (TZ_IDBOUND_KRN + 24)
#define TZ_GCPU_BDRERST_ID                          (TZ_IDBOUND_KRN + 25)
#define TZ_GCPU_EFCMD_ID                            (TZ_IDBOUND_KRN + 26)
#define TZ_GCPU_EFRST_ID                            (TZ_IDBOUND_KRN + 27)
#define TZ_GCPU_KPCMD_ID                            (TZ_IDBOUND_KRN + 28)
#define TZ_GCPU_KPRST_ID                            (TZ_IDBOUND_KRN + 29)
#define TZ_GCPU_MISCCMD_ID                          (TZ_IDBOUND_KRN + 30)
#define TZ_GCPU_MISCRST_ID                          (TZ_IDBOUND_KRN + 31)
#define TZ_GCPU_EFCMD_NOINTR_ID                     (TZ_IDBOUND_KRN + 32)
#define TZ_GCPU_AESCMD_NOINTR_ID                    (TZ_IDBOUND_KRN + 33)
#define TZ_GCPU_KPCMD_NOINTR_ID                     (TZ_IDBOUND_KRN + 34)
#define TZ_GCPU_SETSLOT_ID                          (TZ_IDBOUND_KRN + 35)
#define TZ_GCPU_GETSLOT_ID                          (TZ_IDBOUND_KRN + 36)
#define TZ_GCPU_SEC_SW_RNG_INIT_ID                  (TZ_IDBOUND_KRN + 37)
#define TZ_GCPU_SEC_SW_RNG_ID                       (TZ_IDBOUND_KRN + 38)
#define TZ_GCPU_SEC_SW_RNS_HASH_ID                  (TZ_IDBOUND_KRN + 39)
#define TZ_GCPU_SEC_SW_ECDSA_SGN_160_ID             (TZ_IDBOUND_KRN + 40)
#define TZ_GCPU_SEC_SW_SFE_AUTH_SIGN_ID             (TZ_IDBOUND_KRN + 41)
#define TZ_GCPU_SEC_SW_SFE_AUTH_VERIFY_ID           (TZ_IDBOUND_KRN + 42)
#define TZ_GCPU_SEC_SW_SMP_GEN_KB_ID                (TZ_IDBOUND_KRN + 43)
#define TZ_GCPU_SEC_SW_VEN_HMAC_SHA256_ID           (TZ_IDBOUND_KRN + 44)
#define TZ_GCPU_SEC_SW_VEN_AES128_ID                (TZ_IDBOUND_KRN + 45)
#define TZ_GCPU_SEC_SW_MARLIN_INFO_INIT_ID          (TZ_IDBOUND_KRN + 46)
#define TZ_GCPU_SEC_SW_MARLIN_INFO_UNINIT_ID        (TZ_IDBOUND_KRN + 47)
#define TZ_GCPU_SEC_SW_IF_INIT_ID                   (TZ_IDBOUND_KRN + 48)
#define TZ_GCPU_SEC_SW_IF_UNINIT_ID                 (TZ_IDBOUND_KRN + 49)
#define TZ_GCPU_SEC_SW_DERIVE_CW_ID                 (TZ_IDBOUND_KRN + 50)
#define TZ_GCPU_ROMCHECK_ID                         (TZ_IDBOUND_KRN + 51)
#define TZ_GCPU_REGWRITE_ID                         (TZ_IDBOUND_KRN + 52)
#define TZ_GCPU_REGREAD_ID                         (TZ_IDBOUND_KRN + 53)
#define TZ_Get_Bonding_Val                          (TZ_IDBOUND_KRN + 54)
#define TZ_Get_Bonding_Table                        (TZ_IDBOUND_KRN + 55)
#define TZ_VSF_WRITE_SRAM_ID                        (TZ_IDBOUND_KRN + 56)
#define TZ_VSF_READ_SRAM_ID                         (TZ_IDBOUND_KRN + 57)
#define TZ_ICE_SWITCH_ID                            (TZ_IDBOUND_KRN + 58)   // can not change this ID number
#define TZ_BSP_GetIcSubVersion_ID                   (TZ_IDBOUND_KRN + 59)
#define TZ_Get_IC_Val                               (TZ_IDBOUND_KRN + 60)
#define TZ_SUSPEND_TO_DRAM                          (TZ_IDBOUND_KRN + 61)
#define TZ_GCPU_HW_DECFW                            (TZ_IDBOUND_KRN + 62)    // for FW decryption task
#define TZ_GCPU_HW_ENCFW                            (TZ_IDBOUND_KRN + 63)    // for FW decryption task
#define TZ_Power_Down			                    (TZ_IDBOUND_KRN + 64)
#define TZ_ARM2_ENV			                    (TZ_IDBOUND_KRN + 65)


//
// additional definiton for marlin
//

#ifndef UINT_MAX
#define UINT_MAX    ((sizeof(int) == 2) ? 65535 : 4294967295UL)
#endif  /* UINT_MAX */

#ifndef INT_MAX
#define INT_MAX     ((sizeof(int) == 2) ? 32767 : 2147483647UL)
#endif /* INT_MAX */

#endif // TZ_NAUTILUS_H

