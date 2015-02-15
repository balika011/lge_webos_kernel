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

/** @file sata_file_io.h
 *  Demux hal - common definitions
 */


#ifndef SATA_FILE_IO_H
#define SATA_FILE_IO_H


#include "x_common.h"
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define FILE_OFFSET_ERROR 0x80000001
#define NOT_FIND_FILE_IN_LIST 0x80000002
#define FILE_OFFSET_LARGE_THAN_FILE_SZ 0x80000003

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Prototype  of inter-file functions
//-----------------------------------------------------------------------------
EXTERN void vReadFileListFormHDD(UINT32 u4TarAddr, UINT32 *u4FileCnt);
EXTERN INT32 i4ReadFileFormHDD(
	UINT32 u4InstID, CHAR *strFileName, UINT32 u4TarAddr, UINT32 u4Offset, 
	UINT32 u4Length, UINT32 *u4RealReadSize, UINT32 *pu4TotalFileLength);
EXTERN BOOL fgInitHDDFileAccess(UINT32 u4InstID);
#endif  // SATA_FILE_IO_H

