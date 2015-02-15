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

#include "rpc_ddi_gpu_data.h"

enum RequestCode
{
	R_DDI_GPU_INITIALIZE = 0,
	R_DDI_GPU_FINALIZE,
	R_DDI_GPU_GETLAYERREGIONINFO,
	R_DDI_GPU_SETLAYERREGION,
	R_DDI_GPU_FLIPLAYERREGION,
	R_DDI_GPU_UPDATELAYERREGION,
	R_DDI_GPU_ALLOCSURFACE,
	R_DDI_GPU_SETSURFACEPALETTE,
	R_DDI_GPU_DEALLOCSURFACE,
	R_DDI_GPU_BLIT,
	R_DDI_GPU_STRETCHBLIT,
	R_DDI_GPU_FILLRECT,
	R_DDI_GPU_DRAWRECT,
	R_DDI_GPU_DRAWLINE,
	R_DDI_GPU_GRAPHICSYNC,
	R_DDI_GPU_DECODEIMAGE	
};

struct InitializeParam
{
	unsigned int param1;
};

struct FinalizeParam
{
	unsigned int param1;
};

struct GetLayerRegionInfoParam 
{
	int layer;
};

struct SetLayerRegionParam
{
	int layer;
	unsigned int region_flags;
	unsigned short rect_x;
	unsigned short rect_y;
	unsigned short rect_w;
	unsigned short rect_h;
	unsigned int region_alpha;
};

struct FilpLayerRegionParam
{
	int layer;
	unsigned int buffer_offset;
};

struct UpdateLayerRegionParam
{
	int layer;
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
};

struct SetSurfacePalette
{
	unsigned int buffer_offset;
	unsigned int palette[256];
	int palette_length;
};

struct DeallocSurfaceParam
{
	unsigned int buffer_offset;
	int buffer_id;
};

struct BlitParam
{
	unsigned int src_offset;
	unsigned short src_x;
	unsigned short src_y;
	unsigned short src_w;
	unsigned short src_h;
	unsigned int dest_offset;
	unsigned short dest_x;
	unsigned short dest_y;
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

struct StretchBlitParam
{
	unsigned int src_offset;
	unsigned short src_x;
	unsigned short src_y;
	unsigned short src_w;
	unsigned short src_h;
	unsigned int dest_offset;
	unsigned short dest_x;
	unsigned short dest_y;
	unsigned short dest_w;
	unsigned short dest_h;
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
	unsigned int dest_offset;
	unsigned short dest_x;
	unsigned short dest_y;
	unsigned short dest_w;
	unsigned short dest_h;
	unsigned int color;
	int drawing_flags;
	int src_blend;
	int dst_blend;
	unsigned int dst_colorkey;
};

struct DrawRectParam
{
	unsigned int dest_offset;
	unsigned short dest_x;
	unsigned short dest_y;
	unsigned short dest_w;
	unsigned short dest_h;
	unsigned int color;
	int drawing_flags;
	int src_blend;
	int dst_blend;
	unsigned int dst_colorkey;
};

struct DrawLineParam
{
	unsigned int dest_offset;
	unsigned short dest_x1;
	unsigned short dest_y1;
	unsigned short dest_x2;
	unsigned short dest_y2;
	unsigned int color;
	int drawing_flags;
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
	unsigned int dest_offset;
	char str_buffer[1024];
};


struct RequestCommand
{
	enum RequestCode request;
	union 
	{
		struct InitializeParam initialize;
		struct FinalizeParam finalize;
		struct GetLayerRegionInfoParam getlayerregioninfo;
		struct SetLayerRegionParam setlayerregion;
		struct FilpLayerRegionParam fliplayerregion;
		struct UpdateLayerRegionParam updatelayerregion;
		struct AllocSurfaceParam allocsurface;
		struct SetSurfacePalette setsurfacepalette;
		struct DeallocSurfaceParam deallocsurface;
		struct BlitParam blit;
		struct StretchBlitParam stretchblit;
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
