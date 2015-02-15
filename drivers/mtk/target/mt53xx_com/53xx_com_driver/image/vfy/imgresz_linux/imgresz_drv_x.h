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

#ifndef __IMGRESZ_DRV_X_H_
#define __IMGRESZ_DRV_X_H_


/*-----------------------------------------------------------------------------
                    include files
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/

/*! \name Command of IMG_RESZ
*     The Get/Set Type command of Image Resizer
* @{
*/
#if 0
#define IMG_RESZ_GET_TYPE_GET_TICKET            (RM_GET_TYPE_LAST_ENTRY + ((DRV_GET_TYPE_T) 0)) ///< get a scale instance (open a component)
#define IMG_RESZ_GET_TYPE_SCALING_STATE         (RM_GET_TYPE_LAST_ENTRY + ((DRV_GET_TYPE_T) 1)) ///< get the scaling status when fire do scale command

#define IMG_RESZ_SET_TYPE_RELEASE_TICKET        (RM_SET_TYPE_LAST_ENTRY + ((DRV_SET_TYPE_T) 0)) ///< release the scale instsnce (close a component)
//#define IMG_RESZ_SET_TYPE_RESET_IMGRESZ         (RM_SET_TYPE_LAST_ENTRY + ((DRV_SET_TYPE_T) 1)) ///< reset the image resizer before do scale
#define IMG_RESZ_SET_TYPE_SET_SCALE_MODE        (RM_SET_TYPE_LAST_ENTRY + ((DRV_SET_TYPE_T) 2)) ///< set the scale mode, frame/partial
#define IMG_RESZ_SET_TYPE_SET_SRC_BUF_INFO      (RM_SET_TYPE_LAST_ENTRY + ((DRV_SET_TYPE_T) 3)) ///< set the source buffer information
#define IMG_RESZ_SET_TYPE_SET_DST_BUF_INFO      (RM_SET_TYPE_LAST_ENTRY + ((DRV_SET_TYPE_T) 4)) ///< set the destination buffer information
#define IMG_RESZ_SET_TYPE_SET_PARTIAL_BUF_INFO  (RM_SET_TYPE_LAST_ENTRY + ((DRV_SET_TYPE_T) 5)) ///< set the partial buufer infomation
#define IMG_RESZ_SET_TYPE_SET_DO_SCAL           (RM_SET_TYPE_LAST_ENTRY + ((DRV_SET_TYPE_T) 6)) ///< fire to do scale
#define IMG_RESZ_SET_TYPE_SET_STOP              (RM_SET_TYPE_LAST_ENTRY + ((DRV_SET_TYPE_T) 7)) ///< stop the current scale
#else
#define IMGRESZ_GET_TYPE_LAST_ENTRY             0x1000
#define IMGRESZ_GET_TYPE_T                      0x100
#define IMGRESZ_SET_TYPE_T                      0x200

#define IMG_RESZ_GET_TYPE_GET_TICKET            (IMGRESZ_GET_TYPE_LAST_ENTRY + ((IMGRESZ_GET_TYPE_T) + 0)) ///< get a scale instance (open a component)
#define IMG_RESZ_GET_TYPE_SCALING_STATE         (IMGRESZ_GET_TYPE_LAST_ENTRY + ((IMGRESZ_GET_TYPE_T) + 1)) ///< get the scaling status when fire do scale command

#define IMG_RESZ_SET_TYPE_RELEASE_TICKET        (IMGRESZ_GET_TYPE_LAST_ENTRY + ((IMGRESZ_SET_TYPE_T) + 0)) ///< release the scale instsnce (close a component)
#define IMG_RESZ_SET_TYPE_SET_SCALE_MODE        (IMGRESZ_GET_TYPE_LAST_ENTRY + ((IMGRESZ_SET_TYPE_T) + 2)) ///< set the scale mode, frame/partial
#define IMG_RESZ_SET_TYPE_SET_SRC_BUF_INFO      (IMGRESZ_GET_TYPE_LAST_ENTRY + ((IMGRESZ_SET_TYPE_T) + 3)) ///< set the source buffer information
#define IMG_RESZ_SET_TYPE_SET_DST_BUF_INFO      (IMGRESZ_GET_TYPE_LAST_ENTRY + ((IMGRESZ_SET_TYPE_T) + 4)) ///< set the destination buffer information
#define IMG_RESZ_SET_TYPE_SET_PARTIAL_BUF_INFO  (IMGRESZ_GET_TYPE_LAST_ENTRY + ((IMGRESZ_SET_TYPE_T) + 5)) ///< set the partial buufer infomation
#define IMG_RESZ_SET_TYPE_SET_DO_SCAL           (IMGRESZ_GET_TYPE_LAST_ENTRY + ((IMGRESZ_SET_TYPE_T) + 6)) ///< fire to do scale
#define IMG_RESZ_SET_TYPE_SET_STOP              (IMGRESZ_GET_TYPE_LAST_ENTRY + ((IMGRESZ_SET_TYPE_T) + 7)) ///< stop the current scale
#endif
/*! @} */

/// Image Resizer Scale Mode
typedef enum
{
    IMGRESZ_NONE_SCALE,
    IMGRESZ_FRAME_SCALE,            ///< frame mode
    IMGRESZ_JPEG_PIC_SCALE,         ///< jpeg picture mode decode
    IMGRESZ_PARTIAL_SCALE,          ///< partial mode decode
    IMGRESZ_SCALE_MODE_MAX
} IMGRESZ_SCALE_MODE;

/// Image Resizer Scale Priority
typedef enum
{
    IMGRESZ_PRIORITY_LOW = 1,       ///< for normal image decoder driver
    IMGRESZ_PRIORITY_MID = 3,       ///< for video decoder driver SD source using
    IMGRESZ_PRIORITY_HIGH = 10,     ///< for video decoder driver HD source using
    IMGRESZ_PRIORITY_MAX
} IMGRESZ_SCALE_PRIORITY;

/// Image Resizer State Machine
typedef enum
{
    IMGRESZ_STATE_NONE,
    IMGRESZ_STATE_START,
    IMGRESZ_STATE_SCALING,
    IMGRESZ_STATE_IDLE,
    IMGRESZ_STATE_MAX
} IMGRESZ_STATE;

/// Image Resizer Input(Source Buffer) Color Mode
typedef enum
{
    IMGRESZ_INPUT_COL_MD_NONE = 0,      ///< not available
    IMGRESZ_INPUT_COL_MD_JPG_DEF,       ///< video mode, only use in Jpeg partial and picture mode
    IMGRESZ_INPUT_COL_MD_420_BLK,       ///< video mode
    IMGRESZ_INPUT_COL_MD_422_BLK,       ///< video mode
    IMGRESZ_INPUT_COL_MD_420_RS,        ///< video mode
    IMGRESZ_INPUT_COL_MD_422_RS,        ///< video mode
    IMGRESZ_INPUT_COL_MD_2BPP_IDX,      ///< graph mode
    IMGRESZ_INPUT_COL_MD_4BPP_IDX,      ///< graph mode
    IMGRESZ_INPUT_COL_MD_8BPP_IDX,      ///< graph mode
    IMGRESZ_INPUT_COL_MD_ARGB_1555,     ///< graph mode
    IMGRESZ_INPUT_COL_MD_RGB_565,       ///< graph mode
    IMGRESZ_INPUT_COL_MD_ARGB_4444,     ///< graph mode
    IMGRESZ_INPUT_COL_MD_ARGB_8888      ///< graph mode
} IMGRESZ_SRC_COLOR_MODE;

/// Image Resizer Output(Destination Buffer) Color Mode
typedef enum
{
    IMGRESZ_OUTPUT_COL_MD_NONE = 0,     ///< not available
    IMGRESZ_OUTPUT_COL_MD_420_BLK,      ///< video mode
    IMGRESZ_OUTPUT_COL_MD_422_BLK,      ///< video mode
    IMGRESZ_OUTPUT_COL_MD_420_RS,       ///< video mode
    IMGRESZ_OUTPUT_COL_MD_422_RS,       ///< video mode
    IMGRESZ_OUTPUT_COL_MD_AYUV,         ///< video mode
    IMGRESZ_OUTPUT_COL_MD_ARGB_1555,    ///< graph mode
    IMGRESZ_OUTPUT_COL_MD_RGB_565,      ///< graph mode
    IMGRESZ_OUTPUT_COL_MD_ARGB_4444,    ///< graph mode
    IMGRESZ_OUTPUT_COL_MD_ARGB_8888     ///< graph mode
} IMGRESZ_DST_COLOR_MODE;

#define IMGRESZ_Y_COMPONENT     (1U << 0)
#define IMGRESZ_CB_COMPONENT    (1U << 1)
#define IMGRESZ_CR_COMPONENT    (1U << 2)
#define IMGRESZ_RGB_COMPONENT   (1U << 3)

/// Image Resizer Specific Component Factor, only for jpeg picture scale using
typedef struct _IMGRESZ_COMPONENT_FACTOR_T
{
    UINT32                      u4Components;
    UINT8                       u1YComponentFactor;
    UINT8                       u1CbComponentFactor;
    UINT8                       u1CrComponentFactor;
} IMGRESZ_COMPONENT_FACTOR_T;

/// Image Resizer Source Buffer Information
typedef struct _IMGRESZ_SRC_BUF_INFO_T
{
    IMGRESZ_SRC_COLOR_MODE      eSrcColorMode;  ///< the color mode of input source buffer
    UINT32                      u4YBufAddr;     ///< in graph mode, it means the graph buffer
    UINT32                      u4CbBufAddr;    ///< only used in video and jpeg mode
    UINT32                      u4CrBufAddr;    ///< only used in jpeg partial and picture mode
    UINT32                      u4BufWidth;     ///< the buffer width
    UINT32                      u4BufHeight;    ///< the buffer height
    UINT32                      u4PicPosX;      ///< the picture position X
    UINT32                      u4PicPosY;      ///< the picture position Y
    UINT32                      u4PicWidth;     ///< the picture width
    UINT32                      u4PicHeight;    ///< the picture height
    IMGRESZ_COMPONENT_FACTOR_T  rCompFactor;    ///< used when eSrcColorMode == IMGRESZ_INPUT_COL_MD_JPG_DEF
} IMGRESZ_SRC_BUF_INFO_T;

/// Image Resizer Destination Buffer Information
typedef struct _IMGRESZ_DST_BUF_INFO_T
{
    IMGRESZ_DST_COLOR_MODE      eDstColorMode;  ///< the color mode of output destination buffer
    UINT32                      u4YBufAddr;     ///< in graph mode, it means the graph buffer
    UINT32                      u4CBufAddr;     ///< only used in video mode
    UINT32                      u4BufWidth;     ///< the buffer width
    UINT32                      u4BufHeight;    ///< the buffer height
    UINT32                      u4PicPosX;      ///< the picture position X
    UINT32                      u4PicPosY;      ///< the picture position Y
    UINT32                      u4PicWidth;     ///< the picture width
    UINT32                      u4PicHeight;    ///< the picture height
} IMGRESZ_DST_BUF_INFO_T;

/// Image Resizer Partial Buffer Info
typedef struct _IMGRESZ_PARTIAL_INFO_T
{
    UINT32                      u4YBufAddr;     ///< in graph mode, it means the graph buffer
    UINT32                      u4CbBufAddr;    ///< only used in jpeg mode
    UINT32                      u4CrBufAddr;    ///< only used in jpeg mode
    UINT32                      u4YBufLine;     ///< Y buffer line number (if eSrcColorMode != IMGRESZ_INPUT_COL_MD_JPG_DEF)
    UINT32                      u4CbBufLine;    ///< Cb buffer line number (if eSrcColorMode != IMGRESZ_INPUT_COL_MD_JPG_DEF)
    UINT32                      u4CrBufLine;    ///< Cr buffer line number (if eSrcColorMode != IMGRESZ_INPUT_COL_MD_JPG_DEF)
    BOOL                        fgFirstRow;     ///< the First row
    BOOL                        fgLastRow;      ///< the Last row
} IMGRESZ_PARTIAL_INFO_T;

/// Image Resizer Do Scale Structure
typedef struct _IMGRESZ_DO_SCALE_T
{
    UINT32                      u4ScaleComponents;  ///< the Scale Components
    IMGRESZ_SCALE_PRIORITY      eScalePriority;     ///< the Scale command priority
} IMGRESZ_DO_SCALE_T;

/// Image Resizer Get Scaling State
typedef struct _IMGRESZ_CALE_STATE_T
{
    IMGRESZ_STATE               eState;      ///< the Scale State
} IMGRESZ_SCALE_STATE_T;

/// Image Resizer Get Ticket
typedef struct _IMGRESZ_TICKET_T
{
    UINT32                      u4Ticket;      ///< the ticket
} IMGRESZ_TICKET_T;

#endif /* __IMGRESZ_DRV_X_H_ */
