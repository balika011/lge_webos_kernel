/**
 * @file    ipc_ddi_gpu.h
 * @brief   definition of ipc control commands about ddi gpu operations
 * @author  Ku Bong Min
 * @date    Wed Jan 30 16:44:38 KST 2008
 *
 *  Header file which defines IPC(Inter Process Call) commands betweedn cilent and servert.\n
 *  $Id:\n
 *  $HeadURL:\n
 */

#ifndef __IPC_DDI_GPU_H__
#define __IPC_DDI_GPU_H___
#ifdef __cplusplus
extern "C" {
#endif

#include "ddi_gpu_data.h"
enum RequestCode
{
	R_DDI_GPU_INITIALIZE = 0,
	R_DDI_GPU_GETDEVICECAPABILITY,
	R_DDI_GPU_INITLAYER,
	R_DDI_GPU_INITSURFACEPOOL,
	R_DDI_GPU_FINALIZE,
	R_DDI_GPU_GETLAYERREGIONINFO,
	R_DDI_GPU_SETLAYERREGION,
	R_DDI_GPU_FLIPLAYERREGION,
	R_DDI_GPU_UPDATELAYERREGION,
	R_DDI_GPU_INDICATELAYERUPDATE,
	R_DDI_GPU_ALLOCSURFACE,
	R_DDI_GPU_SETSURFACEPALETTE,
	R_DDI_GPU_DEALLOCSURFACE,
	R_DDI_GPU_BLIT,
	R_DDI_GPU_STRETCHBLIT,
	R_DDI_GPU_TRAPEZOIDBLIT,
	R_DDI_GPU_FILLRECT,
	R_DDI_GPU_DRAWRECT,
	R_DDI_GPU_DRAWLINE,
	R_DDI_GPU_GRAPHICSYNC,
	R_DDI_GPU_DECODEIMAGE
};

struct InitializeParam
{
	unsigned int width;
	unsigned int height;
	GPU_PIXEL_FORMAT_T pixelFormat;
	GPU_SCREEN_RESOLUTION_T screenResolution;
};

struct GetDeviceCapabilityParam
{
	GPU_DEVICE_CAPABILITY_INFO_T device_capability;
};

struct InitLayerParam
{
	unsigned int layerId;
	GPU_LAYER_CONFIG_T layer_config;
	unsigned int palette[256];
};

struct InitSurfacePoolParam
{
	unsigned int surfacePoolSize;
	unsigned int surfacePoolAlignment;
	GPU_SURFACE_POOL_INFO_T surfacePoolInfo;
};

struct FinalizeParam
{
	unsigned int param1;
};

struct GetLayerRegionInfoParam 
{
	int layer;
	GPU_LAYER_REGION_INFO_T layer_region_info;
};

struct SetLayerRegionParam
{
	int layer;
	unsigned int region_flags;
	unsigned short viewrect_x;
	unsigned short viewrect_y;
	unsigned short viewrect_w;
	unsigned short viewrect_h;
	unsigned short disprect_x;
	unsigned short disprect_y;
	unsigned short disprect_w;
	unsigned short disprect_h;
	unsigned int region_alpha;
	unsigned int z_order;
	unsigned int property;
	unsigned int pixel_format;
};

struct FilpLayerRegionParam
{
	int layer;
	unsigned int buffer_offset;
};

struct UpdateLayerRegionParam
{
	int layer;
	unsigned int buffer_offset;
	unsigned short rect_x;
	unsigned short rect_y;
	unsigned short rect_w;
	unsigned short rect_h;
};

struct IndicateLayerUpdateParam
{
	int layer;
	unsigned int buffer_offset;
	unsigned short rect_x;
	unsigned short rect_y;
	unsigned short rect_w;
	unsigned short rect_h;
};

struct AllocSurfaceParam
{
	unsigned short width;
	unsigned short height;
	unsigned short pixel_format;
	GPU_SURFACE_INFO_T surface_info;
};

struct SetSurfacePalette
{
	GPU_SURFACE_INFO_T surface;
	unsigned int palette[256];
	int palette_length;
};

struct DeallocSurfaceParam
{
	GPU_SURFACE_INFO_T surface;
};

struct BlitParam
{
	GPU_SURFACE_INFO_T src_surface;
	GPU_RECT_T src_rect;
	GPU_SURFACE_INFO_T dst_surface;
	UINT16 d_x;
	UINT16 d_y;
	GPU_BLIT_FLAGS_T blit_flags; 
	GPU_BLIT_SETTINGS_T blit_settings;
};

struct StretchBlitParam
{
	GPU_SURFACE_INFO_T src_surface;
	GPU_RECT_T src_rect;
	GPU_SURFACE_INFO_T dst_surface;
	GPU_RECT_T dest_rect;
	GPU_BLIT_FLAGS_T blit_flags; 
	GPU_BLIT_SETTINGS_T blit_settings;
};

struct TrapezoidBlitParam
{
	GPU_SURFACE_INFO_T src_surface;
	GPU_SURFACE_INFO_T dst_surface;
	GPU_TRAPEZOID_T trapezoid_settings;
	int blit_flags; 
	int src_blend;
	int dst_blend;
	unsigned int alpha;
	unsigned int src_colorkey;
	unsigned int dst_colorkey;
	unsigned int mask_offset;
	unsigned short mask_x;
	unsigned short mask_y;
	int mask_flags;
};

struct FillRectParam
{
	GPU_SURFACE_INFO_T dst_surface;
	unsigned short dst_x;
	unsigned short dst_y;
	unsigned short dst_w;
	unsigned short dst_h;
	unsigned int color;
	int draw_flags;
	int src_blend;
	int dst_blend;
	unsigned int dst_colorkey;
};

struct DrawRectParam
{
	GPU_SURFACE_INFO_T dst_surface;
	unsigned short dst_x;
	unsigned short dst_y;
	unsigned short dst_w;
	unsigned short dst_h;
	unsigned int color;
	int draw_flags;
	int src_blend;
	int dst_blend;
	unsigned int dst_colorkey;
};

struct DrawLineParam
{
	GPU_SURFACE_INFO_T dst_surface;
	unsigned short dst_x1;
	unsigned short dst_y1;
	unsigned short dst_x2;
	unsigned short dst_y2;
	unsigned int color;
	int draw_flags;
	int src_blend;
	int dst_blend;
	unsigned int dst_colorkey;
};

struct GraphicSyncParam
{
	unsigned int param1;
};

struct DecodeImageParam
{
	GPU_IMAGE_INFO_T image_info;
	GPU_SURFACE_INFO_T dst_surface;
	GPU_DECODEIMAGE_FLAGS_T decode_flags;
	CHAR str_buffer[1024];
};


struct RequestCommand
{
	enum RequestCode request;
	union 
	{
		struct InitializeParam initialize;
		struct GetDeviceCapabilityParam getdevicecapability;
		struct InitLayerParam initlayer;
		struct InitSurfacePoolParam initsurfacepool;
		struct FinalizeParam finalize;
		struct GetLayerRegionInfoParam getlayerregioninfo;
		struct SetLayerRegionParam setlayerregion;
		struct FilpLayerRegionParam fliplayerregion;
		struct UpdateLayerRegionParam updatelayerregion;
		struct IndicateLayerUpdateParam indicatelayerupdate;
		struct AllocSurfaceParam allocsurface;
		struct SetSurfacePalette setsurfacepalette;
		struct DeallocSurfaceParam deallocsurface;
		struct BlitParam blit;
		struct StretchBlitParam stretchblit;
		struct TrapezoidBlitParam trapezoidblit;
		struct FillRectParam fillrect;
		struct DrawRectParam drawrect;
		struct DrawLineParam drawline;
		struct GraphicSyncParam graphicsync;
		struct DecodeImageParam decodeimage;
	};
};


#ifdef __cplusplus
}
#endif
#endif /* __IPC_DDI_GPU_H__ */
