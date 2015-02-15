/**
 * @file    rpc_ddi_gpu_data.h
 * @brief   type definitions for the DDI_GPU APIs 
 * @author  Ku Bong Min
 * @date    Wed Jan 16 16:27:21 KST 2008
 *
 *  Header file which defines various structure based types for the DDI_GPU APIs.
 *  $Id: //DTV/RELEASE_BR/r_DTV_X_IDTV1401_002306_12_001_37_001/kernel/drivers/mtk/mt5882/linux/app_if/ddi/include/include/rpc4dtv/rpc_ddi_gpu_data.h#1 $\n
 *  $HeadURL: https://wolf.lge.com/svn_internal/rpc4dtv/trunk/rpc_ddi_gpu/rpc_ddi_gpu_data.h $\n
 */

#ifndef __RPC_DDI_GPU_DATA_H__
#define __RPC_DDI_GPU_DATA_H__
#ifdef __cplusplus
extern "C" {
#endif

#if (PLATFORM_TYPE==MSTAR_PLATFORM)

#ifndef UINT8 // If not defined, it's stand-alone client.
typedef unsigned char			UINT8;
typedef unsigned short			UINT16; 
typedef unsigned int			UINT32;
typedef unsigned long			ULONG;
typedef unsigned long long		UINT64;

typedef signed char 			SINT8;
typedef signed short			SINT16;
typedef signed int				SINT32;
typedef signed long 			LONG;
typedef signed long long		SINT64;

typedef unsigned int			BOOLEAN;

/**
 * @brief This enumeration describes the API return type. 
 */
typedef enum 
{
	OK					= 0, 	/**< success (no error) */
	NOT_OK				= -1,	/**< generic error */
	INVALID_PARAMS		= -2, 	/**< input parameter error */
	NOT_ENOUGH_RESOURCE	= -3,	/**< not enough resource */
	NOT_SUPPORTED		= -4,	/**< not supported */
	NOT_PERMITTED		= -5,	/**< not permitted */
	TIMEOUT				= -6,	/**< timeout */
	NO_DATA_RECEIVED	= -7,	/**< no data received */
	DN_BUF_OVERFLOW 	= -8	/**< buffer overflow error */
} DTV_STATUS_T;
#endif


/**
 * @brief This enumeration describes the supported pixel formats. 
 */
typedef enum 
{
	GPU_PIXEL_FORMAT_ARGB = 0,	/**< 32 bit ARGB (4 byte, alpha 8\@24, red 8\@16, green 8\@8, blue 8\@0) */
	GPU_PIXEL_FORMAT_LUT8, 		/**< 8 bit LUT (8 bit color and alpha lookup from palette) */
	GPU_PIXEL_FORMAT_ARGB1555, 	/**< 16 bit ARGB (2 byte, alpha 1\@15, red 5\@10, green 5\@5, blue 5\@0) */
	GPU_PIXEL_FORMAT_RGB16, 	/**< 16 bit RGB (2 byte, red 5\@11, green 6\@5, blue 5\@0) */
	GPU_PIXEL_FORMAT_ARGB4444, 	/**< 16 bit ARGB (2 byte, alpha 4\@12, red 4\@8, green 4\@4, blue 4\@0) */
	GPU_PIXEL_FORMAT_A8, 		/**< 8 bit A (1 byte, alpha 8\@0) */
	GPU_PIXEL_FORMAT_MAX		/**< Maximum number of GPU_PIXEL_FORMAT_T */
} GPU_PIXEL_FORMAT_T;

/**
 * @brief This structure describes a information about imageFormat.
 */
typedef enum {
    GPU_IMAGE_FORMAT_JPEG           = 0x00000000,
    GPU_IMAGE_FORMAT_PNG,
    GPU_IMAGE_FORMAT_GIF,
    GPU_IMAGE_FORMAT_BMP,
    GPU_IMAGE_FORMAT_BUFFER,
    GPU_IMAGE_FORMAT_MAX
} GPU_IMAGE_FORMAT_T;


/* type definitions */
/**
 * @brief This structure describes a rectangle specified by a point and a dimension.
 */
typedef struct
{
	UINT16 x; /**< x cordinate of its top-letf point */
	UINT16 y; /**< y cordinate of its top-left point */
	UINT16 w; /**< width of it */
	UINT16 h; /**< height of it */
} GPU_RECT_T;

/**
 * @brief This structure describes a line specified by two points.
 */
typedef struct
{
	UINT16 x1; /**< x cordinate of its top-letf point1 */
	UINT16 y1; /**< y cordinate of its top-left point1 */
	UINT16 x2; /**< x cordinate of its top-letf point2 */
	UINT16 y2; /**< y cordinate of its top-left point2 */
} GPU_LINE_T;

/**
 * @brief This structure describes display.
 */
typedef struct
{
	UINT32 physicalAddr;  /**< base physical address of video memory area */
	UINT32 virtualAddr;  /**< base virtual address of video memory area */
	UINT16 width; 	/**< width of display (vertical display resolution)*/
	UINT16 height; 	/**< height of display (horizontal display resolution)*/
} GPU_DISPLAY_INFO_T;


/** 
 * @brief This structure describes layer.
 */
typedef struct
{
	UINT32 offset1; /**< bytes offset (from the start of video memory) of the first surface */
	UINT32 offset2; /**< bytes offset (from the start of video memory) of the second surface for the double buffering (if-any) */
	UINT16 pitch; /**< pitch: length of horizontal line */
	UINT16 bpp; /**< bits per pixel */
	UINT16 width; /**< width of the layer */
	UINT16 height; /**< height of the layer */ 
	GPU_RECT_T region; /**< viewable region of the layer */
	GPU_PIXEL_FORMAT_T pixelFormat;
} GPU_LAYER_REGION_INFO_T;


/** 
 * @brief This structure describes a surface specified by video memory offset, pitch and bpp.
 */
typedef struct
{
	UINT32 offset; /**< bytes offset from the start of video memory */
	UINT16 pitch; /**< pitch: length of horizontal line */
	UINT16 bpp; /**< bits per pixel */
	SINT32 id; /**< surface identifier */
} GPU_SURFACE_INFO_T;


/**
 * @brief Flags controlling surface masks set.
 */
typedef enum {
	GPU_SURFACE_MASK_NONE      = 0x00000000,  /**< None of these. */
	GPU_SURFACE_MASK_STENCIL   = 0x00000001,  /**< Take <b>x</b> and <b>y</b> as fixed start coordinates in the mask. */
	GPU_SURFACE_MASK_ALL       = 0x00000001,  /**< All of these. */
} GPU_SURFACE_MASK_FLAGS_T;


/** 
 * @brief This structure describes source mask.
 */
typedef struct
{
	UINT32 offset; /**< bytes offset from the start of video memory */
	UINT16 x; /**< x */
	UINT16 y; /**< y */
	GPU_SURFACE_MASK_FLAGS_T flags; /**< STENCIL or not*/
} GPU_SOURCE_MASK_T;


/**
 * @brief This enumeration describes the flags controlling blitting commands.
 */
typedef enum
{
	GPU_BLIT_NOFX					= 0x00000000, /**< uses none of the effects */
	GPU_BLIT_BLEND_ALPHACHANNEL	= 0x00000001, /**< enables blending and uses
													  alphachannel from source */
	GPU_BLIT_BLEND_COLORALPHA		= 0x00000002, /**< enables blending and uses
													  alpha value from color */
	GPU_BLIT_COLORIZE				= 0x00000004, /**< modulates source color with
													  the color's r/g/b values */
	GPU_BLIT_SRC_COLORKEY			= 0x00000008, /**< don't blit pixels matching the source color key */
	GPU_BLIT_DST_COLORKEY			= 0x00000010, /**< write to destination only if the destination pixel
													  matches the destination color key */
	GPU_BLIT_SRC_PREMULTIPLY		= 0x00000020, /**< modulates the source color with the (modulated)
													  source alpha */
	GPU_BLIT_DST_PREMULTIPLY		= 0x00000040, /**< modulates the dest. color with the dest. alpha */
	GPU_BLIT_DEMULTIPLY			= 0x00000080, /**< divides the color by the alpha before writing the
													  data to the destination */
	GPU_BLIT_DEINTERLACE			= 0x00000100, /**< deinterlaces the source during blitting by reading
													  only one field (every second line of full
													  image) scaling it vertically by factor two */
	GPU_BLIT_SRC_PREMULTCOLOR		= 0x00000200, /**< modulates the source color with the color alpha */
	GPU_BLIT_XOR					= 0x00000400, /**< bitwise xor the destination pixels with the
													 source pixels after premultiplication */
	GPU_BLIT_INDEX_TRANSLATION	= 0x00000800, /**< do fast indexed to indexed translation,
													 this flag is mutual exclusive with all others */ //zuel
	GPU_BLIT_ROTATE90				= 0x00001000, /**< rotate the image by 90 degree clockwise */ //zuel
	GPU_BLIT_ROTATE180				= 0x00002000, /**< rotate the image by 180 degree clockwise */ //zuel
	GPU_BLIT_ROTATE270				= 0x00004000, /**< rotate the image by 270 degree clockwise */
	GPU_BLIT_COLORKEY_PROTECT		= 0x00010000, /**< make sure written pixels don't match color key (internal only ATM) */
	GPU_BLIT_SRC_MASK_ALPHA		= 0x00100000, /**< modulate source alpha channel with alpha channel from source mask */
	GPU_BLIT_SRC_MASK_COLOR		= 0x00200000,  /**< modulate source color channels with color channels from source mask */
	GPU_BLIT_MAX  /**< the maximum value */
} GPU_BLIT_FLAGS_T;


/**
 * @brief This enumeration describes the flags controlling drawing commands.
 */
typedef enum
{
	GPU_DRAW_NOFX					= 0x00000000, /**< uses none of the effects */
	GPU_DRAW_BLEND				= 0x00000001, /**< uses alpha from color */
	GPU_DRAW_DST_COLORKEY			= 0x00000002, /**< write to destination only if the destination pixel matches the destination color key */
	GPU_DRAW_SRC_PREMULTIPLY		= 0x00000004, /**< muliplies the color's rgb channels by the alpha channel before drawing */
	GPU_DRAW_DST_PREMULTIPLY		= 0x00000008, /**< modulates the dest. color with the dest. alpha */
	GPU_DRAW_DEMULTIPLY			= 0x00000010, /**< divides the color by the alpha before writing the data to the destination */
	GPU_DRAW_XOR					= 0x00000020,  /**< bitwise xor the destination pixels with the specified color after premultiplication */
	GPU_DRAW_MAX  /**< the maximum value */
} GPU_DRAW_FLAGS_T;


/**
 * @brief This enumeration describes the blend functions to use for source and destination blending.
 */
typedef enum
{
	GPU_BLEND_UNKNOWN			= 0,  /**< Uknown blending function */
	GPU_BLEND_ZERO               = 1,  /**< zero or, 0 */
	GPU_BLEND_ONE                = 2,  /**< 1 */
	GPU_BLEND_SRCCOLOR           = 3,  /**< source color */
	GPU_BLEND_INVSRCCOLOR        = 4,  /**< inverse source alpha */
	GPU_BLEND_SRCALPHA           = 5,  /**< source alpha */
	GPU_BLEND_INVSRCALPHA        = 6,  /**< inverse source alpha (1-Sa)*/
	GPU_BLEND_DESTALPHA          = 7,  /**< destination alpha */
	GPU_BLEND_INVDESTALPHA       = 8,  /**< inverse destination lapha (1-Da)*/
	GPU_BLEND_DESTCOLOR          = 9,  /**< destination color */
	GPU_BLEND_INVDESTCOLOR       = 10, /**< inverse destination color */
	GPU_BLEND_SRCALPHASAT        = 11, /**< source alpha saturation */
	GPU_BLEND_MAX  /**< the maximum value */
} GPU_BLEND_FUNCTION_T;


/**
 * @brief This enumeration describes the layer region flags for layer setting.
 */
typedef enum 
{
     GPU_LAYER_CONFIG_NONE         = 0x00000000,	/**< none */
     GPU_LAYER_CONFIG_WIDTH        = 0x00000001,	/**< width (in pixels) of viewable region */
     GPU_LAYER_CONFIG_HEIGHT       = 0x00000002,	/**< height (in pixels) of viewable region */
     //GPU_LAYER_CONFIG_FORMAT       = 0x00000004,
     //GPU_LAYER_CONFIG_SURFACE_CAPS = 0x00000008,
     //GPU_LAYER_CONFIG_BUFFERMODE   = 0x00000010,
     //GPU_LAYER_CONFIG_OPTIONS      = 0x00000020,
     //GPU_LAYER_CONFIG_SOURCE_ID    = 0x00000040,
     //GPU_LAYER_CONFIG_SOURCE       = 0x00000100,
     //GPU_LAYER_CONFIG_DEST         = 0x00000200,
     //GPU_LAYER_CONFIG_CLIPS        = 0x00000400,
     GPU_LAYER_CONFIG_OPACITY      = 0x00001000,	/**< layer opacity (0x00: invisible(layer-off), 0x01~0xfe: transparent layer, 0xff: fully opaque.) */
     //GPU_LAYER_CONFIG_ALPHA_RAMP   = 0x00002000,
     //GPU_LAYER_CONFIG_SRCKEY       = 0x00010000,
     //GPU_LAYER_CONFIG_DSTKEY       = 0x00020000,
     //GPU_LAYER_CONFIG_PARITY       = 0x00100000,
     //GPU_LAYER_CONFIG_SURFACE      = 0x10000000,
     //GPU_LAYER_CONFIG_PALETTE      = 0x20000000,
     //GPU_LAYER_CONFIG_FREEZE       = 0x80000000,
     GPU_LAYER_CONFIG_ALL          = 0xFFFFFFFF	/**< all */ 
} GPU_LAYER_CONFIG_FLAGS_T;

/** 
 * @brief This structure describes settings for draw operation.
 */
typedef struct
{
	GPU_BLEND_FUNCTION_T srcBlend; /**< source blend function */
	GPU_BLEND_FUNCTION_T dstBlend; /**< destination blend function */
	UINT32 dstColorkey;           /**< destination colorkey */
} GPU_DRAW_SETTINGS_T;

/** 
 * @brief This structure describes settings for blit operation.
 */
typedef struct
{
	GPU_BLEND_FUNCTION_T srcBlend; /**< source blend function */
	GPU_BLEND_FUNCTION_T dstBlend; /**< destination blend function */
	UINT32 alpha;                 /**< global alpha value */
	UINT32 srcColorkey;           /**< source colorkey */
	UINT32 dstColorkey;           /**< destination colorkey */
	GPU_SOURCE_MASK_T srcMask;    /**< source mask information*/
} GPU_BLIT_SETTINGS_T;

/** 
 * @This structure describes settings for trapezoid blit operation.
 */
typedef struct
{
    GPU_RECT_T      srcRect;
    UINT8           alphaConst;
    BOOLEAN         bAntiAliasing;
    UINT8           trapDir;
    UINT8           srcRectRotateDir;
    UINT8           dstTrapRotateDir;
    UINT16          dstTrapEdge0Pos;
    UINT16          dstTrapEdge0St;
    UINT16          dstTrapEdge0End;
    UINT16          dstTrapEdge1St;
    UINT16          dstTrapEdge1End;
    UINT16          dstTrapDistance;
} GPU_TRAPEZOID_T;

/** 
 * @This structure describes a information about image.
 */
typedef struct {
    GPU_IMAGE_FORMAT_T  imageFormat;
    UINT16              width;
    UINT16              height;
    UINT32              imageLength ;
    GPU_PIXEL_FORMAT_T  pixelFormat;
    BOOLEAN             bImageFromBuffer;
    SINT8               *imagePath;
    UINT32              *imagePtr;
} GPU_IMAGE_INFO_T;

#else
//The following code is for BCM3556

#ifndef UINT8 // If not defined, it's stand-alone client.
#ifndef UINT8
typedef	unsigned char			__UINT8;
#define UINT8 __UINT8
#endif

#ifndef SINT8
typedef	signed char				__SINT8;
#define SINT8 __SINT8
#endif

#ifndef CHAR
typedef	char					__CHAR;
#define CHAR __CHAR
#endif

#ifndef UINT16
typedef	unsigned short			__UINT16;
#define UINT16 __UINT16
#endif

#ifndef SINT16
typedef	signed short			__SINT16;
#define SINT16 __SINT16
#endif

#ifndef UINT32
typedef	unsigned long			__UINT32;
#define UINT32 __UINT32
#endif

#ifndef SINT32
typedef signed long				__SINT32;
#define SINT32 __SINT32
#endif

#ifndef BOOLEAN
typedef	unsigned long			__BOOLEAN;
#define BOOLEAN __BOOLEAN
#endif

#ifndef UINT64
typedef	unsigned long long		__UINT64;
#define UINT64 __UINT64
#endif

#ifndef SINT64
typedef	signed long long		__SINT64;
#define SINT64 __SINT64
#endif

/**
 * @brief This enumeration describes the API return type. 
 */
typedef enum 
{
	OK					= 0, 	/**< success (no error) */
	NOT_OK				= -1,	/**< generic error */
	INVALID_PARAMS		= -2, 	/**< input parameter error */
	NOT_ENOUGH_RESOURCE	= -3,	/**< not enough resource */
	NOT_SUPPORTED		= -4,	/**< not supported */
	NOT_PERMITTED		= -5,	/**< not permitted */
	TIMEOUT				= -6,	/**< timeout */
	NO_DATA_RECEIVED	= -7,	/**< no data received */
	DN_BUF_OVERFLOW 	= -8	/**< buffer overflow error */
} DTV_STATUS_T;
#endif


/**
 * @brief This enumeration describes the supported pixel formats. 
 */
typedef enum 
{
	GPU_PIXEL_FORMAT_ARGB = 0,	/**< 32 bit ARGB (4 byte, alpha 8\@24, red 8\@16, green 8\@8, blue 8\@0) */
	GPU_PIXEL_FORMAT_LUT8, 		/**< 8 bit LUT (8 bit color and alpha lookup from palette) */
	GPU_PIXEL_FORMAT_ARGB1555, 	/**< 16 bit ARGB (2 byte, alpha 1\@15, red 5\@10, green 5\@5, blue 5\@0) */
	GPU_PIXEL_FORMAT_RGB16, 	/**< 16 bit RGB (2 byte, red 5\@11, green 6\@5, blue 5\@0) */
	GPU_PIXEL_FORMAT_ARGB4444, 	/**< 16 bit ARGB (2 byte, alpha 4\@12, red 4\@8, green 4\@4, blue 4\@0) */
	GPU_PIXEL_FORMAT_A8, 		/**< 8 bit A (1 byte, alpha 8\@0) */
	GPU_PIXEL_FORMAT_MAX		/**< Maximum number of GPU_PIXEL_FORMAT_T */
} GPU_PIXEL_FORMAT_T;


/**
 * @brief This structure describes a information about image format.
 */
typedef enum
{
	GPU_IMAGE_FORMAT_JPEG = 0x00000000,
	GPU_IMAGE_FORMAT_PNG,
	GPU_IMAGE_FORMAT_GIF,
	GPU_IMAGE_FORMAT_BMP,
	GPU_IMAGE_FORMAT_MAX
} GPU_IMAGE_FORMAT_T;


/* type definitions */
/**
 * @brief This structure describes a rectangle specified by a point and a dimension.
 */
typedef struct
{
	UINT16 x; /**< x cordinate of its top-letf point */
	UINT16 y; /**< y cordinate of its top-left point */
	UINT16 w; /**< width of it */
	UINT16 h; /**< height of it */
} GPU_RECT_T;


/**
 * @brief This structure describes a line specified by two points.
 */
typedef struct
{
	UINT16 x1; /**< x cordinate of its top-letf point1 */
	UINT16 y1; /**< y cordinate of its top-left point1 */
	UINT16 x2; /**< x cordinate of its top-letf point2 */
	UINT16 y2; /**< y cordinate of its top-left point2 */
} GPU_LINE_T;


/**
 * @brief This structure describes display.
 */
typedef struct
{
	UINT32 physicalAddr;  /**< base physical address of video memory area */
	UINT32 virtualAddr;  /**< base virtual address of video memory area */
	UINT16 width; 	/**< width of display (vertical display resolution)*/
	UINT16 height; 	/**< height of display (horizontal display resolution)*/
} GPU_DISPLAY_INFO_T;


/** 
 * @brief This structure describes layer.
 */
typedef struct
{
	UINT32 offset1; /**< bytes offset (from the start of video memory) of the first surface */
	UINT32 offset2; /**< bytes offset (from the start of video memory) of the second surface for the double buffering (if-any) */
	UINT16 pitch; /**< pitch: length of horizontal line */
	UINT16 bpp; /**< bits per pixel */
	UINT16 width; /**< width of the layer */
	UINT16 height; /**< height of the layer */ 
	GPU_RECT_T region; /**< viewable region of the layer */
	GPU_PIXEL_FORMAT_T pixelFormat;
} GPU_LAYER_REGION_INFO_T;


/** 
 * @brief This structure describes a surface specified by video memory offset, pitch and bpp.
 */
typedef struct
{
	UINT32 offset; /**< bytes offset from the start of video memory */
	UINT16 pitch; /**< pitch: length of horizontal line */
	UINT16 bpp; /**< bits per pixel */
	SINT32 id; /**< surface identifier */
} GPU_SURFACE_INFO_T;


/**
 * @brief Flags controlling surface masks set.
 */
typedef enum {
	GPU_SURFACE_MASK_NONE      = 0x00000000,  /**< None of these. */
	GPU_SURFACE_MASK_STENCIL   = 0x00000001,  /**< Take <b>x</b> and <b>y</b> as fixed start coordinates in the mask. */
	GPU_SURFACE_MASK_ALL       = 0x00000001,  /**< All of these. */
} GPU_SURFACE_MASK_FLAGS_T;


/** 
 * @brief This structure describes source mask.
 */
typedef struct
{
	UINT32 offset; /**< bytes offset from the start of video memory */
	UINT16 x; /**< x */
	UINT16 y; /**< y */
	GPU_SURFACE_MASK_FLAGS_T flags; /**< STENCIL or not*/
} GPU_SOURCE_MASK_T;


/**
 * @brief This enumeration describes the flags controlling blitting commands.
 */
typedef enum
{
	GPU_BLIT_NOFX					= 0x00000000, /**< uses none of the effects */
	GPU_BLIT_BLEND_ALPHACHANNEL	= 0x00000001, /**< enables blending and uses
													  alphachannel from source */
	GPU_BLIT_BLEND_COLORALPHA		= 0x00000002, /**< enables blending and uses
													  alpha value from color */
	GPU_BLIT_COLORIZE				= 0x00000004, /**< modulates source color with
													  the color's r/g/b values */
	GPU_BLIT_SRC_COLORKEY			= 0x00000008, /**< don't blit pixels matching the source color key */
	GPU_BLIT_DST_COLORKEY			= 0x00000010, /**< write to destination only if the destination pixel
													  matches the destination color key */
	GPU_BLIT_SRC_PREMULTIPLY		= 0x00000020, /**< modulates the source color with the (modulated)
													  source alpha */
	GPU_BLIT_DST_PREMULTIPLY		= 0x00000040, /**< modulates the dest. color with the dest. alpha */
	GPU_BLIT_DEMULTIPLY			= 0x00000080, /**< divides the color by the alpha before writing the
													  data to the destination */
	GPU_BLIT_DEINTERLACE			= 0x00000100, /**< deinterlaces the source during blitting by reading
													  only one field (every second line of full
													  image) scaling it vertically by factor two */
	GPU_BLIT_SRC_PREMULTCOLOR		= 0x00000200, /**< modulates the source color with the color alpha */
	GPU_BLIT_XOR					= 0x00000400, /**< bitwise xor the destination pixels with the
													 source pixels after premultiplication */
	GPU_BLIT_INDEX_TRANSLATION	= 0x00000800, /**< do fast indexed to indexed translation,
													 this flag is mutual exclusive with all others */
	GPU_BLIT_ROTATE180			= 0x00001000, /**< rotate the image by 180 degree */
	GPU_BLIT_COLORKEY_PROTECT		= 0x00010000, /**< make sure written pixels don't match color key (internal only ATM) */
	GPU_BLIT_SRC_MASK_ALPHA		= 0x00100000, /**< modulate source alpha channel with alpha channel from source mask */
	GPU_BLIT_SRC_MASK_COLOR		= 0x00200000,  /**< modulate source color channels with color channels from source mask */
	GPU_BLIT_BLEND_ALPHACHANNEL_BCM = 0x01000000, /* BCM specific mechanism similar to SRC_PREMULTCOLOR | BLEND_ALPHACHANNEL | DST_PREUMLTCOLOR */
	GPU_BLIT_BLEND_ALPHACHANNEL_COLORALPHA_BCM = 0x02000000, /* BCM specific mechanism similar to SRC_PREMULTCOLOR | BLEND_ALPHACHANNEL | BLEND_COLORALPHA | DST_PREUMLTCOLOR */
	GPU_BLIT_BLEND_ALPHACHANNEL_COLORIZE_BCM = 0x04000000, /* BCM specific mechanism similar to SRC_PREMULTCOLOR | BLEND_ALPHACHANNEL | COLORIZE | DST_PREUMLTCOLOR */
	GPU_BLIT_MAX  /**< the maximum value */
} GPU_BLIT_FLAGS_T;


/**
 * @brief This enumeration describes the flags controlling drawing commands.
 */
typedef enum
{
	GPU_DRAW_NOFX					= 0x00000000, /**< uses none of the effects */
	GPU_DRAW_BLEND				= 0x00000001, /**< uses alpha from color */
	GPU_DRAW_DST_COLORKEY			= 0x00000002, /**< write to destination only if the destination pixel matches the destination color key */
	GPU_DRAW_SRC_PREMULTIPLY		= 0x00000004, /**< muliplies the color's rgb channels by the alpha channel before drawing */
	GPU_DRAW_DST_PREMULTIPLY		= 0x00000008, /**< modulates the dest. color with the dest. alpha */
	GPU_DRAW_DEMULTIPLY			= 0x00000010, /**< divides the color by the alpha before writing the data to the destination */
	GPU_DRAW_XOR					= 0x00000020,  /**< bitwise xor the destination pixels with the specified color after premultiplication */
	GPU_DRAW_MAX  /**< the maximum value */
} GPU_DRAW_FLAGS_T;


/**
 * @brief This enumeration describes the blend functions to use for source and destination blending.
 */
typedef enum
{
	GPU_BLEND_UNKNOWN			= 0,  /**< Uknown blending function */
	GPU_BLEND_ZERO               = 1,  /**< zero or, 0 */
	GPU_BLEND_ONE                = 2,  /**< 1 */
	GPU_BLEND_SRCCOLOR           = 3,  /**< source color */
	GPU_BLEND_INVSRCCOLOR        = 4,  /**< inverse source alpha */
	GPU_BLEND_SRCALPHA           = 5,  /**< source alpha */
	GPU_BLEND_INVSRCALPHA        = 6,  /**< inverse source alpha (1-Sa)*/
	GPU_BLEND_DESTALPHA          = 7,  /**< destination alpha */
	GPU_BLEND_INVDESTALPHA       = 8,  /**< inverse destination lapha (1-Da)*/
	GPU_BLEND_DESTCOLOR          = 9,  /**< destination color */
	GPU_BLEND_INVDESTCOLOR       = 10, /**< inverse destination color */
	GPU_BLEND_SRCALPHASAT        = 11, /**< source alpha saturation */
	GPU_BLEND_MAX  /**< the maximum value */
} GPU_BLEND_FUNCTION_T;


/**
 * @brief This enumeration describes the layer region flags for layer setting.
 */
typedef enum 
{
     GPU_LAYER_CONFIG_NONE         = 0x00000000,	/**< none */
     GPU_LAYER_CONFIG_WIDTH        = 0x00000001,	/**< width (in pixels) of viewable region */
     GPU_LAYER_CONFIG_HEIGHT       = 0x00000002,	/**< height (in pixels) of viewable region */
     //GPU_LAYER_CONFIG_FORMAT       = 0x00000004,
     //GPU_LAYER_CONFIG_SURFACE_CAPS = 0x00000008,
     //GPU_LAYER_CONFIG_BUFFERMODE   = 0x00000010,
     //GPU_LAYER_CONFIG_OPTIONS      = 0x00000020,
     //GPU_LAYER_CONFIG_SOURCE_ID    = 0x00000040,
     //GPU_LAYER_CONFIG_SOURCE       = 0x00000100,
     //GPU_LAYER_CONFIG_DEST         = 0x00000200,
     //GPU_LAYER_CONFIG_CLIPS        = 0x00000400,
     GPU_LAYER_CONFIG_OPACITY      = 0x00001000,	/**< layer opacity (0x00: invisible(layer-off), 0x01~0xfe: transparent layer, 0xff: fully opaque.) */
     //GPU_LAYER_CONFIG_ALPHA_RAMP   = 0x00002000,
     //GPU_LAYER_CONFIG_SRCKEY       = 0x00010000,
     //GPU_LAYER_CONFIG_DSTKEY       = 0x00020000,
     //GPU_LAYER_CONFIG_PARITY       = 0x00100000,
     //GPU_LAYER_CONFIG_SURFACE      = 0x10000000,
     //GPU_LAYER_CONFIG_PALETTE      = 0x20000000,
     //GPU_LAYER_CONFIG_FREEZE       = 0x80000000,
     GPU_LAYER_CONFIG_ALL          = 0xFFFFFFFF	/**< all */ 
} GPU_LAYER_CONFIG_FLAGS_T;


/** 
 * @brief This structure describes settings for draw operation.
 */
typedef struct
{
	GPU_BLEND_FUNCTION_T srcBlend; /**< source blend function */
	GPU_BLEND_FUNCTION_T dstBlend; /**< destination blend function */
	UINT32 dstColorkey;           /**< destination colorkey */
} GPU_DRAW_SETTINGS_T;


/** 
 * @brief This structure describes settings for blit operation.
 */
typedef struct
{
	GPU_BLEND_FUNCTION_T srcBlend; /**< source blend function */
	GPU_BLEND_FUNCTION_T dstBlend; /**< destination blend function */
	UINT32 alpha;                 /**< global alpha value */
	UINT32 color;                 /**< global color value */
	UINT32 srcColorkey;           /**< source colorkey */
	UINT32 dstColorkey;           /**< destination colorkey */
	GPU_SOURCE_MASK_T srcMask;    /**< source mask information*/
} GPU_BLIT_SETTINGS_T;

 /** 
 * @This structure describes settings for trapezoid blit operation.
 */
typedef struct
{
    GPU_RECT_T      srcRect;
    UINT8           alphaConst;
    BOOLEAN         bAntiAliasing;
    UINT8           trapDir;
    UINT8           srcRectRotateDir;
    UINT8           dstTrapRotateDir;
    UINT16          dstTrapEdge0Pos;
    UINT16          dstTrapEdge0St;
    UINT16          dstTrapEdge0End;
    UINT16          dstTrapEdge1St;
    UINT16          dstTrapEdge1End;
    UINT16          dstTrapDistance;
} GPU_TRAPEZOID_T;

/**
 * @brief This structure describes a information about image including imageFormat, width, height, pixelFormat, image location Info(image path or buffer memory pointer)
 */
typedef struct
{
	GPU_IMAGE_FORMAT_T imageFormat;
	GPU_RECT_T rect;
	UINT32 imageLength;
	GPU_PIXEL_FORMAT_T pixelFormat;
	BOOLEAN bImageFromBuffer;
	CHAR *imagePath;
	UINT32 imagePtr;
} GPU_IMAGE_INFO_T;
#endif

#ifdef __cplusplus
}
#endif
#endif /* __RPC_DDI_GPU_DATA_H__ */
