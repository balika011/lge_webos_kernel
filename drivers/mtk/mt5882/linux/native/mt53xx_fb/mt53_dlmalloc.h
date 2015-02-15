/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
#ifndef _MT53_DLMALLOC_H_
#define _MT53_DLMALLOC_H_

//==========================================================================
//
//      dlmalloc.h
//
//      Interface to the port of Doug Lea's malloc implementation
//
//==========================================================================

// init memory pool
// return true on success
extern int
MT53_GPU_dlmem_init(void* base, int size);

// get some memory, return NULL if none available
extern void*
MT53_GPU_dlmalloc(int size);

extern void*
MT53_GPU_dlcalloc(int nmemb, int size);

// resize existing allocation, if oldsize is non-NULL, previous
// allocation size is placed into it. If previous size not available,
// it is set to 0. NB previous allocation size may have been rounded up.
// Occasionally the allocation can be adjusted *backwards* as well as,
// or instead of forwards, therefore the address of the resized
// allocation is returned, or NULL if no resizing was possible.
// Note that this differs from ::realloc() in that no attempt is
// made to call malloc() if resizing is not possible - that is left
// to higher layers. The data is copied from old to new though.
// The effects of alloc_ptr==NULL or newsize==0 are undefined
extern void*
MT53_GPU_dlresize_alloc(void* alloc_ptr, int newsize, int* oldsize);

extern void*
MT53_GPU_dlrealloc(void* ptr, int size);

// free the memory back to the pool
// returns true on success
extern int
MT53_GPU_dlfree(void* ptr);

typedef struct MT53_GPU_mempool_status
{
    const unsigned char *arenabase;     // base address of entire pool
    int   arenasize;                    // total size of entire pool
    int   freeblocks;                   // number of chunks free for use
    int   totalallocated;               // total allocated space in bytes
    int   totalfree;                    // total space in bytes not in use
    int   blocksize;                    // block size if fixed block
    int   maxfree;                      // size of largest unused block
    char  waiting;                      // are there any threads waiting for memory?
    const unsigned char *origbase;      // address of original region used when pool
                                        // created
    int   origsize;                     // size of original region used when pool
                                        // created

    // maxoverhead is the *maximum* per-allocation overhead imposed by
    // the allocator implementation. Note: this is rarely the typical
    // overhead which often depends on the size of the allocation requested.
    // It includes overhead due to alignment constraints. For example, if
    // maxfree and maxoverhead are available for this allocator, then an
    // allocation request of (maxfree-maxoverhead) bytes must always succeed
    // Unless maxoverhead is set to -1 of course, in which case the allocator
    // does not support reporting this information.
    char    maxoverhead;
}mt_mempool_status;

// Flags to pass to mem_get_status() methods to tell it which stat(s) is/are
// being requested

typedef struct {
	unsigned int	physicalAddr;		/**< physical address of surface pool base */
	unsigned int	virtualAddr;		/**< virtual address of surface pool base */
	unsigned int	startOffset;		/**< the end of allocated layers (surface pool start with zero offset.) */
	unsigned int	size;				/**< size of surface pool (in bytes). It is not from startOffset but zero offset which means base point of surface pool. */
	unsigned short	byteOffsetAlign;	/**< bytes alignment constraint for the offset value of each surface allocation */
	unsigned short	bytePitchAlign;		/**< bytes alignment constraint for the surface's pitch value */
} MT53_HOA_GX_SURFACE_POOL_INFO_T;



#define MT53_GPU_MEMPOOL_STAT_ARENABASE       (1<<0)
#define MT53_GPU_MEMPOOL_STAT_ARENASIZE       (1<<1)
#define MT53_GPU_MEMPOOL_STAT_FREEBLOCKS      (1<<2)
#define MT53_GPU_MEMPOOL_STAT_TOTALALLOCATED  (1<<3)
#define MT53_GPU_MEMPOOL_STAT_TOTALFREE       (1<<4)
#define MT53_GPU_MEMPOOL_STAT_BLOCKSIZE       (1<<5)
#define MT53_GPU_MEMPOOL_STAT_MAXFREE         (1<<6)
#define MT53_GPU_MEMPOOL_STAT_WAITING         (1<<7)
#define MT53_GPU_MEMPOOL_STAT_ORIGBASE        (1<<9)
#define MT53_GPU_MEMPOOL_STAT_ORIGSIZE        (1<<10)
#define MT53_GPU_MEMPOOL_STAT_MAXOVERHEAD     (1<<11)
#define MT53_GPU_MEMPOOL_STAT_ALL             0xfff

// Get memory pool status
// flags is a bitmask of requested fields to fill in. The flags are
// defined in common.hxx
extern void
MT53_GPU_dlmem_get_status(unsigned int flags, struct MT53_GPU_mempool_status* status);

/*

#include <stdlib.h>

extern struct mallinfo
mallinfo(void);

*/

#endif // ifndef _DLMALLOC_H_

// EOF dlmalloc.h
