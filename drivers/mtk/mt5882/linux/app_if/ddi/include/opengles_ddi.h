/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file       opengles_ddi.h
 *
 * This source file defines the DDI functions related to opengle es driver control
 *
 *  @author     Jae hak Lee (jaehak.lee@lge.com)
 *  @version    1.0
 *  @date       2010.10.25
 *  @note       Additional information.
 *  @see
 */

/******************************************************************************
    Header File Guarder
******************************************************************************/
#ifndef _OPENGLES_DDI_H_
#define _OPENGLES_DDI_H_

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
    Control Constants
******************************************************************************/
/******************************************************************************
    File Inclusions
******************************************************************************/
/******************************************************************************
    Macro Definitions
******************************************************************************/
/******************************************************************************
    Type Definitions
******************************************************************************/

/******************************************************************************
    Function Declaration
******************************************************************************/
/**
 *  OpenGL ES Driver Init Information Return 
 *
 * @return  BOOLEN 
 */
BOOLEAN DDI_OPENGLES_IsInitialize(void);

/**
 *  OpenGL ES Driver Initialize for DTV Process
 *
 * @return  the function succeeds, the return value is OK..
 */
DTV_STATUS_T DDI_OPENGLES_Initialize(void);

/**
 * Get Frame buffer From DTV Process
 *
 * @param  pBufferConfig [in] Configuration to Create Frame Buffer
 * @param  pBufferPhysicalInfo [in] Physical iniformation about created Frame Buffer
 * @return  the function succeeds, the return value is OK.
 */
DTV_STATUS_T DDI_OPENGLES_GetFrameBuffer(UINT8 *pBufferConfig, UINT8 *pBufferPhysicalInfo);

/**
 * Get Frame buffer From DTV Process
 *
 * @param  pMemConfig [in] Configuration to Create Rednering Memory
 * @param  pMemPhysicalInfo [in] Physical iniformation about created Rendering Memory
 * @return  the function succeeds, the return value is OK.
 */
DTV_STATUS_T DDI_OPENGLES_GetRenderingMemory(UINT8 *pMemConfig, UINT8 *pMemPhysicalInfo);

/**
 * Display from framebuffer to screen
 *
 * @param  bufferIndex [in] framebuffer index
 * @return  the function succeeds, the return value is OK.
 */
DTV_STATUS_T DDI_OPENGLES_DisplayToScreen(UINT8 bufferIndex);

/**
 * OpenGL ES Driver Finalize for DTV Process
 *
 * @return  the function succeeds, the return value is OK..
 */
DTV_STATUS_T DDI_OPENGLES_Finalize(void);


#ifdef __cplusplus
}
#endif

#endif /*_OPENGLES_DDI_H_ */



