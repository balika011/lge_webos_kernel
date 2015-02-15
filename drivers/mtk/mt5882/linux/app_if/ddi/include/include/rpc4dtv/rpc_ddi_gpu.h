/**
 * @file    rpc_ddi_gpu.h
 * @brief   DDI GPU(GFX) API for 2D graphics acceralation function.
 * @author  Ku Bong Min
 * @date    Wed Jan 16 16:27:21 KST 2008
 *
 *  Header file which implement DDI_GPU(GFX) APIs. These APIs can be called via RPC(Remote Procedure Call) using IPC(Inter Process Call) based on Unix Socket.\n
 *  $Id: //DTV/RELEASE_BR/r_DTV_X_IDTV1401_002306_12_001_37_001/kernel/drivers/mtk/mt5882/linux/app_if/ddi/include/include/rpc4dtv/rpc_ddi_gpu.h#1 $\n
 *  $HeadURL: https://wolf.lge.com/svn_internal/rpc4dtv/trunk/rpc_ddi_gpu/rpc_ddi_gpu.h $\n
 */

/**
  @mainpage rpc_ddi_gpu
  @section INTRO
  - To support 2D graphics H/W accelaration, this library implement DDI GPU(GFX) APIs. And these APIs can be called by using RPC mechanism.
  @section CREATEINFO
  - Author : Ku Bong Min
  - Date : Wed Jan 16 16:27:21 KST 2008 
  @section MODIFYINFO
  - $LastChangedDate: 2009-02-20 11:34:24 +0900 (Fri, 20 Feb 2009) $
  - $LastChangedRevision: 85 $
  - $LastChangedBy: yducky $
  */


#ifndef __RPC_DDI_GPU_H__
#define __RPC_DDI_GPU_H___
#ifdef __cplusplus
extern "C" {
#endif

#include "rpc_ddi_gpu_data.h"

#ifndef USE_DLOPEN
#define USE_DLOPEN 0
#endif

#if !USE_DLOPEN

#define MAX_NUMBER_OF_SURFACE (2000)

/**
 * @defgroup RPCClient RPC Client-side APIs
 */
/*@{*/

/**
 * @brief  Open RPC connection: initialize IPC connection which is used by RPC. You should call this API before call any RPC_DDI_* APIs in client. (RPC Client-side API)
 * @param[in] sock not used yet. (just pass 0 or NULL ;) )
 * @return return 0(OK) on successful termination, -1(NOT_OK) on failure.
 */
DTV_STATUS_T RPC_OpenDDI_GPU(SINT32 sock);

/**
 *  @brief Close RPC connection: close IPC connection which is used by RPC. You should call this before terminating client process. (RPC Client-side API) 
 */
void RPC_CloseDDI_GPU(void);

/*@}*/


/**
  @defgroup RPCServer RPC Server-side APIs
 */
/*@{*/

/**
  @brief  process DDI Requests from RPC client. This funcion runs a loop while the client alives. (RPC Server-side API)
  @param[in] sock client sockect handler.
  @return return 0 on successful termination, negative value on failure.
  */
SINT32 RPC_ProcessRequestDDI_GPU(SINT32 sock);

/*@}*/


/**
  @defgroup RPCAPI RPC APIs - DDI_GPU_* APIs which is called by RPC mechanism
  */
/*@{*/

/**
  @brief Initializes GPU module.
  Call Chipset driver initialization function and create resources for GPU module such as OSD framebuffer and layers.
  @return If successful, it returns physical and virtual base address for chipset memory, width and height of display.\n
  If fail, every field should be filled with zeros.
 */
GPU_DISPLAY_INFO_T DDI_GPU_Initialize(void);

/**
  @brief Finalize 2D graphics engine and free OSD framebuffer and layers.
  @return If the function succeeds, the return value is OK.
  If the function fails, the return value is NOT_OK.
  */
DTV_STATUS_T DDI_GPU_Finalize(void);

/**
  @brief Gets information about the viewable region and the primary surfaces assigned to each layer in the video memory area.
  @param[in] layer layer identifier (number)
  @return  If successful, it returns layer information (video memory offset1 of 1st surface, video memory offset2 of 2nd surface(if exist) or 0, pitch, bpp, width, height, viewable region rectangle).\n
  If fail, every field should be filled with zeros.
  */
GPU_LAYER_REGION_INFO_T DDI_GPU_GetLayerRegionInfo(SINT32 layer);

/**
  @brief Sets region(viewable area) about layer to support layer changes in resolution or opacity.
  @param[in] layer layer identifier (number)
  @param[in] regionFlags flags which indicates updated events
  @param[in] regionRect rectangle which represent layer region
  @param[in] regionAlpha opacity value for layer region (0x0:invisible~0xff:fully opaque)
  @return If the function succeeds, the return value is OK.\n
  If the function fails, the return value is NOT_OK.
  */
DTV_STATUS_T DDI_GPU_SetLayerRegion(SINT32 layer, GPU_LAYER_CONFIG_FLAGS_T regionFlags, GPU_RECT_T regionRect, UINT32 regionAlpha);

/**
  @brief Filps between layer surfaces (if 2nd layer surface is available). Surface which has bufferOffset is changed to front buffer.
  @param[in] layer identifier (number)
  @param[in] bufferOffset video memory offset for the surface buffer
  @return If the function succeeds, the return value is OK.\n
  If the function fails, the return value is NOT_OK.
  */
DTV_STATUS_T DDI_GPU_FlipLayerRegion(SINT32 layer, UINT32 bufferOffset);

/**
  @brief Processes update event on layer surface.
  @param[in] layer identifier (number)
  @param[in] updateRect rectangle which represent updated area in the layer surface
  @return If the function succeeds, the return value is OK.\n
  If the function fails, the return value is NOT_OK.
  */
DTV_STATUS_T DDI_GPU_UpdateLayerRegion(SINT32 layer, GPU_RECT_T updateRect);

/**
  @brief Allocates memory buffer for a surface in the video memory area. (for using H/W accerated graphics)
  @param[in] width surface width in pixels
  @param[in] height surface height in pixels
  @param[in] pixelFormat pixel format
  @return If successful, it returns information about newly allocated surface (video memory offset, pitch, height).\n
  If fail, every field should be filled with zeros.
  */
GPU_SURFACE_INFO_T DDI_GPU_AllocSurface(UINT16 width, UINT16 height, GPU_PIXEL_FORMAT_T pixelFormat);

/**
  @brief Writes entries to the palette of a surface which is indexed color format such as PIXEL_FORMAT_LUT8.
  @param[in] bufferOffset video memory offset for the surface buffer
  @param[in] palette palette which is array of 32bit ARGB color entries
  @param[in] paletteLength number of entries of palette 
  @return If the function succeeds, the return value is OK.\n
  If the function fails, the return value is NOT_SUPPORTED on un-supported request, API_NOT_ENOUGH_RESOURCE on resource failure, NOT_OK on the other cases.
  */
DTV_STATUS_T DDI_GPU_SetSurfacePalette(UINT32 bufferOffset, UINT32 palette[256], SINT32 paletteLength);

/**
  @brief Dellocates buffer for a surface in the video memory area.
  @param bufferOffset video memroy offset for the surface.
  @param bufferId video memroy identifier for the surface.
  */
void DDI_GPU_DeallocSurface(UINT32 bufferOffset, SINT32 bufferId);

/**
  @brief Blit an area from one surface to other surface based on blit flags.
  @param[in] srcOffset video memory offset for the source surface
  @param[in] srcRect source rectangle which represent blited area in the source surface
  @param[in] destOffset video memory offset for the destination surface
  @param[in] dx x in destination surface
  @param[in] dy y in destination surface
  @param[in] blitFlags blitting flags which indicate various blit operations
  @param[in] blitSettings settings for blit operation
  @return If the function succeeds, the return value is OK.\n
  If the function fails, the return value is NOT_SUPPORTED on un-supported request, NOT_OK on the other cases.
  */
DTV_STATUS_T DDI_GPU_Blit(UINT32 srcOffset, GPU_RECT_T srcRect, UINT32 destOffset, UINT16 dx, UINT16 dy, GPU_BLIT_FLAGS_T blitFlags, GPU_BLIT_SETTINGS_T blitSettings);



/**
  @brief Stretch blit an area from one surface to other surface based on blit flags.
  @param[in] srcOffset video memory offset for the source surface
  @param[in] srcRect source rectangle which represent blited area in the source surface
  @param[in] destOffset video memory offset for the destination surface
  @param[in] destRect destination rectangle which represent blited area in the destination surface
  @param[in] blitFlags blitting flags which indicate various blit operations
  @param[in] blitSettings settings for blit operation
  @return If the function succeeds, the return value is OK.\n
  If the function fails, the return value is NOT_SUPPORTED on un-supported request, NOT_OK on the other cases.
  */
DTV_STATUS_T DDI_GPU_StretchBlit(UINT32 srcOffset, GPU_RECT_T srcRect, UINT32 destOffset, GPU_RECT_T destRect, GPU_BLIT_FLAGS_T blitFlags, GPU_BLIT_SETTINGS_T blitSettings);

/**
  @brief Fills a rectangle to a destination surface based on drawing flags.
  @param[in] destOffset video memory offset for the surface
  @param[in] destRect rectangle which represent drawing area in the surface
  @param[in] color color value for the drawing
  @param[in] drawingFlags drawing flags which indicate various blit operations
  @param[in] drawSettings settings for draw operation
  @return If the function succeeds, the return value is OK.\n
  If the function fails, the return value is NOT_SUPPORTED on un-supported request, NOT_OK on the other cases.
  */
DTV_STATUS_T DDI_GPU_FillRectangle(UINT32 destOffset, GPU_RECT_T destRect, UINT32 color, GPU_DRAW_FLAGS_T drawingFlags, GPU_DRAW_SETTINGS_T drawSettings);

/**
  @brief Draws a rectangle to a destination surface based on drawing flags.
  @param[in] destOffset video memory offset for the surface
  @param[in] destRect rectangle which represent drawing area in the surface
  @param[in] color color value for the drawing
  @param[in] drawingFlags drawing flags which indicate various blit operations
  @param[in] drawSettings settings for draw operation
  @return If the function succeeds, the return value is OK.\n
  If the function fails, the return value is NOT_SUPPORTED on un-supported request, NOT_OK on the other cases.
  */
DTV_STATUS_T DDI_GPU_DrawRectangle(UINT32 destOffset, GPU_RECT_T destRect, UINT32 color, GPU_DRAW_FLAGS_T drawingFlags, GPU_DRAW_SETTINGS_T drawSettings);

/**
  @brief Draws a line to a destination surface based on drawing flags.
  @param[in] destOffset video memory offset for the surface
  @param[in] destLine line which represent drawing area in the surface
  @param[in] color color value for the drawing
  @param[in] drawingFlags drawing flags which indicate various blit operations
  @param[in] drawSettings settings for draw operation
  @return If the function succeeds, the return value is OK.\n
  If the function fails, the return value is NOT_SUPPORTED on un-supported request, NOT_OK on the other cases.
  */
DTV_STATUS_T DDI_GPU_DrawLine(UINT32 destOffset, GPU_LINE_T destLine, UINT32 color, GPU_DRAW_FLAGS_T drawingFlags, GPU_DRAW_SETTINGS_T drawSettings);

/**
  @brief Waits for the all the H/W accelerated graphics operations to be done.
  @return If the function succeeds, the return value is OK.\n
  If the function fails, the return value is NOT_SUPPORTED on un-supported request, NOT_OK on the other cases.
  */
DTV_STATUS_T DDI_GPU_SyncGraphic(void);

/**
  @brief TrapezoidBlits an area from one surface to other surface based on blit flags
  @param[in] srcSurface source surface handler
  @param[in] dstSurface destination surface handler
  @param[in] trapezoidSet src image and trapezoid information
  @param[in] blitFlags blitting flags which indicate various blit operation
  @param[in] blitSettings settings for blit operation
  @return If the function succeeds, the return value is OK.\n
  If the function fails, the return value is NOT_SUPPORTED on un-supported request, NOT_OK on the other cases.
  */
DTV_STATUS_T DDI_GPU_TrapezoidBlit(GPU_SURFACE_INFO_T srcSurface, GPU_SURFACE_INFO_T dstSurface, GPU_TRAPEZOID_T trapezoidSet, GPU_BLIT_FLAGS_T blitFlags, GPU_BLIT_SETTINGS_T blitSettings);

/**
  @brief Draw an image from encoded image to destination surface based on imageFromBuffer flags.
  @param[in] source Image Information
  @param[in] destination surface handler
  @return If the function succeeds, the return value is OK.\n
  If the function fails, the return value is NOT_SUPPORTED on un-supported request, NOT_OK on the other cases.
 */
//DTV_STATUS_T DDI_GPU_DecodeImage(GPU_IMAGE_INFO_T imageInfo, GPU_SURFACE_INFO_T dstSurface) ;
DTV_STATUS_T DDI_GPU_DecodeImage(GPU_IMAGE_INFO_T imageInfo, UINT32 dstOffset) ;
/*@}*/

DTV_STATUS_T DDI_GPU_SetMirror(BOOLEAN bHMirror, BOOLEAN bVMirror);

#else

//#define DDI_GPU_PRINTF printf
#define DDI_GPU_PRINTF(x...) 

#include <dlfcn.h>
#include <pthread.h>

static DTV_STATUS_T (*RPC_OpenDDI_GPU)(SINT32 sock) = NULL;
static GPU_DISPLAY_INFO_T (*DDI_GPU_Initialize)(void) = NULL;
static void (*RPC_CloseDDI_GPU)() = NULL;
static DTV_STATUS_T (*DDI_GPU_Finalize)(void) = NULL;
static GPU_LAYER_REGION_INFO_T (*DDI_GPU_GetLayerRegionInfo)(SINT32 layer) = NULL;
static DTV_STATUS_T (*DDI_GPU_SetLayerRegion)(SINT32 layer, UINT32 regionFlags , GPU_RECT_T regionRect, UINT32 regionAlpha) = NULL;
static DTV_STATUS_T (*DDI_GPU_FlipLayerRegion)(SINT32 layer, UINT32 bufferOffset) = NULL;
static DTV_STATUS_T (*DDI_GPU_UpdateLayerRegion)(SINT32 layer, GPU_RECT_T updateRect) = NULL;
static GPU_SURFACE_INFO_T (*DDI_GPU_AllocSurface)(UINT16 width, UINT16 height, GPU_PIXEL_FORMAT_T pixelFormat) = NULL;
static DTV_STATUS_T (*DDI_GPU_SetSurfacePalette)(UINT32 bufferOffset, UINT32 palette[256], SINT32 paletteLength) = NULL;
static void (*DDI_GPU_DeallocSurface)(UINT32 bufferOffset, SINT32 bufferId) = NULL;
static DTV_STATUS_T (*DDI_GPU_Blit)(UINT32 srcOffset, GPU_RECT_T srcRect, UINT32 destOffset, UINT16 dx, UINT16 dy, GPU_BLIT_FLAGS_T blitFlags, GPU_BLIT_SETTINGS_T blitSettings) = NULL;
static DTV_STATUS_T (*DDI_GPU_StretchBlit)(UINT32 srcOffset, GPU_RECT_T srcRect, UINT32 destOffset, GPU_RECT_T destRect, GPU_BLIT_FLAGS_T blitFlags, GPU_BLIT_SETTINGS_T blitSettings) = NULL;
static DTV_STATUS_T (*DDI_GPU_FillRectangle)(UINT32 destOffset, GPU_RECT_T destRect, UINT32 color, GPU_DRAW_FLAGS_T drawingFlags, GPU_DRAW_SETTINGS_T drawSettings) = NULL;
static DTV_STATUS_T (*DDI_GPU_DrawRectangle)(UINT32 destOffset, GPU_RECT_T destRect, UINT32 color, GPU_DRAW_FLAGS_T drawingFlags, GPU_DRAW_SETTINGS_T drawSettings) = NULL;
static DTV_STATUS_T (*DDI_GPU_DrawLine)(UINT32 destOffset, GPU_LINE_T destLine, UINT32 color, GPU_DRAW_FLAGS_T drawingFlags, GPU_DRAW_SETTINGS_T drawSettings) = NULL;
static DTV_STATUS_T (*DDI_GPU_SyncGraphic)(void) = NULL;
static DTV_STATUS_T (*DDI_GPU_TrapezoidBlit)(GPU_SURFACE_INFO_T srcSurface, GPU_SURFACE_INFO_T dstSurface, GPU_TRAPEZOID_T trapezoidSet, GPU_BLIT_FLAGS_T blitFlags, GPU_BLIT_SETTINGS_T blitSettings) = NULL;
//static DTV_STATUS_T (*DDI_GPU_DecodeImage)(GPU_IMAGE_INFO_T imageInfo, GPU_SURFACE_INFO_T dstSurface) = NULL;
static DTV_STATUS_T (*DDI_GPU_DecodeImage)(GPU_IMAGE_INFO_T imageInfo, UINT32 dstOffset) = NULL;


static void* gRPCHandle;

static inline void assign_symbol_DDI_GPU_FUNCS(pthread_mutex_t *pMutex)
{
	/*
	if(pMutex){
		pthread_mutex_lock(pMutex);
	}
	gRPCHandle = dlopen(NULL, RTLD_NOW);
	if(pMutex){
		pthread_mutex_unlock(pMutex);
	}
	if(!gRPCHandle)
	{
		DDI_GPU_PRINTF("[%s:%d-%s] Error: dlopen failure!\n", __FILE__, __LINE__, __FUNCTION__);
	} else {

		if(pMutex){
			pthread_mutex_lock(pMutex);
		}
		*(void **) (&RPC_OpenDDI_GPU) = dlsym(gRPCHandle, "RPC_OpenDDI_GPU");
		*(void **) (&RPC_CloseDDI_GPU) = dlsym(gRPCHandle, "PRC_CloseDDI_GPU");
		*(void **) (&DDI_GPU_Initialize) = dlsym(gRPCHandle, "DDI_GPU_Initialize");
		*(void **) (&DDI_GPU_Finalize) = dlsym(gRPCHandle, "DDI_GPU_Finalize");
		*(void **) (&DDI_GPU_GetLayerRegionInfo) = dlsym(gRPCHandle, "DDI_GPU_GetLayerRegionInfo");
		*(void **) (&DDI_GPU_SetLayerRegion) = dlsym(gRPCHandle, "DDI_GPU_SetLayerRegion");
		*(void **) (&DDI_GPU_FlipLayerRegion) = dlsym(gRPCHandle, "DDI_GPU_FlipLayerRegion");
		*(void **) (&DDI_GPU_UpdateLayerRegion) = dlsym(gRPCHandle, "DDI_GPU_UpdateLayerRegion"); 
		*(void **) (&DDI_GPU_AllocSurface) = dlsym(gRPCHandle, "DDI_GPU_AllocSurface");
		*(void **) (&DDI_GPU_SetSurfacePalette) = dlsym(gRPCHandle, "DDI_GPU_SetSurfacePalette");
		*(void **) (&DDI_GPU_DeallocSurface) = dlsym(gRPCHandle, "DDI_GPU_DeallocSurface");
		*(void **) (&DDI_GPU_Blit) = dlsym(gRPCHandle, "DDI_GPU_Blit");
		*(void **) (&DDI_GPU_StretchBlit) = dlsym(gRPCHandle, "DDI_GPU_StretchBlit");
		*(void **) (&DDI_GPU_FillRectangle) = dlsym(gRPCHandle, "DDI_GPU_FillRectangle");
		*(void **) (&DDI_GPU_DrawRectangle) = dlsym(gRPCHandle, "DDI_GPU_DrawRectangle");
		*(void **) (&DDI_GPU_DrawLine) = dlsym(gRPCHandle, "DDI_GPU_DrawLine");
		*(void **) (&DDI_GPU_SyncGraphic) = dlsym(gRPCHandle, "DDI_GPU_SyncGraphic");
		*(void **) (&DDI_GPU_DecodeImage) = dlsym(gRPCHandle, "DDI_GPU_DecodeImage");
		if(pMutex){
			pthread_mutex_unlock(pMutex);
		}
	}
	*/

	if(DDI_GPU_Initialize == NULL)
	{
		/* dlclose(gRPCHandle); */
		if(pMutex){
			pthread_mutex_lock(pMutex);
		}
		gRPCHandle = dlopen("librpc4dtv.so.1", RTLD_NOW);
		if(pMutex){
			pthread_mutex_unlock(pMutex);
		}
		if(!gRPCHandle)
		{
			DDI_GPU_PRINTF("Error: dlopen... Missing library-librpc4dtv.so..\n");
		} else {

			if(pMutex){
				pthread_mutex_lock(pMutex);
			}
			*(void **) (&RPC_OpenDDI_GPU) = dlsym(gRPCHandle, "RPC_OpenDDI_GPU");
			*(void **) (&RPC_CloseDDI_GPU) = dlsym(gRPCHandle, "PRC_CloseDDI_GPU");
			*(void **) (&DDI_GPU_Initialize) = dlsym(gRPCHandle, "DDI_GPU_Initialize");
			*(void **) (&DDI_GPU_Finalize) = dlsym(gRPCHandle, "DDI_GPU_Finalize");
			*(void **) (&DDI_GPU_GetLayerRegionInfo) = dlsym(gRPCHandle, "DDI_GPU_GetLayerRegionInfo");
			*(void **) (&DDI_GPU_SetLayerRegion) = dlsym(gRPCHandle, "DDI_GPU_SetLayerRegion");
			*(void **) (&DDI_GPU_FlipLayerRegion) = dlsym(gRPCHandle, "DDI_GPU_FlipLayerRegion");
			*(void **) (&DDI_GPU_UpdateLayerRegion) = dlsym(gRPCHandle, "DDI_GPU_UpdateLayerRegion"); 
			*(void **) (&DDI_GPU_AllocSurface) = dlsym(gRPCHandle, "DDI_GPU_AllocSurface");
			*(void **) (&DDI_GPU_SetSurfacePalette) = dlsym(gRPCHandle, "DDI_GPU_SetSurfacePalette");
			*(void **) (&DDI_GPU_DeallocSurface) = dlsym(gRPCHandle, "DDI_GPU_DeallocSurface");
			*(void **) (&DDI_GPU_Blit) = dlsym(gRPCHandle, "DDI_GPU_Blit");
			*(void **) (&DDI_GPU_StretchBlit) = dlsym(gRPCHandle, "DDI_GPU_StretchBlit");
			*(void **) (&DDI_GPU_FillRectangle) = dlsym(gRPCHandle, "DDI_GPU_FillRectangle");
			*(void **) (&DDI_GPU_DrawRectangle) = dlsym(gRPCHandle, "DDI_GPU_DrawRectangle");
			*(void **) (&DDI_GPU_DrawLine) = dlsym(gRPCHandle, "DDI_GPU_DrawLine");
			*(void **) (&DDI_GPU_SyncGraphic) = dlsym(gRPCHandle, "DDI_GPU_SyncGraphic");
			*(void **) (&DDI_GPU_TrapezoidBlit) = dlsym(gRPCHandle, "DDI_GPU_TrapezoidBlit");
			*(void **) (&DDI_GPU_DecodeImage) = dlsym(gRPCHandle, "DDI_GPU_DecodeImage");

			if(pMutex){
				pthread_mutex_unlock(pMutex);
			}
			//DDI_GPU_PRINTF("[DDI_GPU_Funcs] This is Client DirectFB!\n");
		}
	}
	else
	{
		//DDI_GPU_PRINTF("[DDI_GPU_Funcs] This is server DirectFB!\n");
	}
}

#endif

#ifdef __cplusplus
}
#endif
#endif /* __RPC_DDI_GPU_H__ */
