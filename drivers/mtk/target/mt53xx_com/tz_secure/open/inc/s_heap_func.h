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
#ifndef S_HEAP_FUNC_H
#define S_HEAP_FUNC_H

//#include "section.h"

//#ifndef size_t
//  typedef unsigned int size_t;
//#endif
 /*-----------------------------------------------------------------------------
 * Name             - x_alloc_s_dram_mem
 * Description      - This function is to allocate memory in secure dram.
 * Input arguments  - u4Size : allocate how many bytes
 * Output arguments - none
 * Returns          - returns a pointer to the allocated memory
 *                    If the request fails,it will return NULL
 *---------------------------------------------------------------------------*/
 extern void* x_alloc_s_dram_mem(size_t u4Size);

  /*-----------------------------------------------------------------------------
 * Name             - x_free_s_dram_mem
 * Description      - frees the secure dram memory space pointed to by ptr
 * Input arguments  - p : point to the secure dram memory space
 * Output arguments - none
 * Returns          - returns no value
 *---------------------------------------------------------------------------*/
 extern void  x_free_s_dram_mem(void* p);

 /*-----------------------------------------------------------------------------
 * Name             - x_realloc_s_dram_mem
 * Description      - changes  the  size  of the memory block pointed to by p to u4Size bytes.
 * Input arguments  - p : point to the original secure dram memory space
 *                  - u4Size : size of newly secure dram memory block
 * Output arguments - none
 * Returns          - returns a pointer to the newly  allocated  memory.
 *                    If the request fails,it will return NULL
 *---------------------------------------------------------------------------*/
 extern void* x_realloc_s_dram_mem(void* p,size_t u4Size);

 //NAUTILUS_FOR_LINUX
 extern void* x_calloc_s_dram_mem(size_t a, size_t b);
#endif

