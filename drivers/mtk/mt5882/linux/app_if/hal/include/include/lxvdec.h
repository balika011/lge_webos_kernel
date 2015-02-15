#ifndef _LXVDEC_H
#define _LXVDEC_H

#include "lxvo.h"

typedef struct _lxvdec lxvdec;

enum _LXVDEC_INPUT_DEVICE
{
	LXVDEC_INPUT_DEVICE_DEMUX,
	LXVDEC_INPUT_DEVICE_DEMUX0 = LXVDEC_INPUT_DEVICE_DEMUX,
	LXVDEC_INPUT_DEVICE_DEMUX1,
	LXVDEC_INPUT_DEVICE_DEMUX2,

	LXVDEC_INPUT_DEVICE_MEMORY,
	LXVDEC_INPUT_DEVICE_MEMORY_TVP,
};
typedef enum _LXVDEC_INPUT_DEVICE LXVDEC_INPUT_DEVICE;

enum _LXVDEC_OUTPUT_DEVICE
{
	LXVDEC_OUTPUT_DEVICE_DE,
	LXVDEC_OUTPUT_DEVICE_DE0 = LXVDEC_OUTPUT_DEVICE_DE,
	LXVDEC_OUTPUT_DEVICE_DE1,
	LXVDEC_OUTPUT_DEVICE_MEMORY,
};
typedef enum _LXVDEC_OUTPUT_DEVICE LXVDEC_OUTPUT_DEVICE;


/**
 * LXVDEC_READ:
 * return value of lxvdec_read().
 *
 * @LXVDEC_READ_NONE: read nothing. call lxvdec_read() again.
 *
 * @LXVDEC_READ_SEQHEAD: sequence init done. deprecated.
 *
 * @LXVDEC_READ_DISPLAY_PICTURE: there is something to display.
 *
 * @LXVDEC_READ_ERROR_NOT_SUPPORTED: stream cannot be decode with some error.
 * detailed error reason can be obtained by lxvdec_get_error()
 *
 * @LXVDEC_READ_USERDATA: there is user data.
 *
 * @LXVDEC_READ_PICTURE_DISPLAYED: picture display completed for
 * LXVDEC_OUTPUT_DEVICE_DE.
 *
 * @LXVDEC_READ_REQUEST_DPB: need DPB memory for lxvdec_use_user_dpb()
 *
 * @LXVDEC_READ_EOS:
 */
typedef enum _LXVDEC_READ
{
	LXVDEC_READ_NONE	= 0,
	LXVDEC_READ_SEQHEAD,
	LXVDEC_READ_DISPLAY_PICTURE,
	LXVDEC_READ_PICTURE = LXVDEC_READ_DISPLAY_PICTURE,
	LXVDEC_READ_ERROR_NOT_SUPPORTED,
	LXVDEC_READ_USERDATA,
	LXVDEC_READ_PICTURE_DISPLAYED,
	LXVDEC_READ_REQUEST_DPB,
	LXVDEC_READ_EOS,
} LXVDEC_READ;


/**
 * LXVDEC_ERROR
 * return value of lxvdec_get_error()
 *
 * @LXVDEC_ERROR_NONE:
 *
 * @LXVDEC_ERROR_SEQUENCE_INIT_FAILED: sequence init failed. broken
 * sequence data or no sequence data.  cannot proceed further decoding.
 *
 * @LXVDEC_ERROR_DECODE_FAILED: frame decoding failed.  but, decoding
 * can be proceeding.
 *
 * @LXVDEC_ERROR_NOT_SUPPORTED_STREAM: this hardware cannot decode
 * this video ES. out of hardware spec.
 *
 * @LXVDEC_ERROR_RESOURCE_BUSY: hardware resource is insufficient to
 * decode the video ES. after closing some other decoder instance,
 * decoder may decode the ES.
 */
typedef enum _LXVDEC_ERROR
{
	LXVDEC_ERROR_NONE = 0,

	LXVDEC_ERROR_SEQUENCE_INIT_FAILED,
	LXVDEC_ERROR_DECODE_FAILED,
	LXVDEC_ERROR_NOT_SUPPORTED_STREAM,
	LXVDEC_ERROR_RESOURCE_BUSY,
} LXVDEC_ERROR;

enum _LXVDEC_SCAN_PICTURE
{
	LXVDEC_SCAN_PICTURE_ALL,
	LXVDEC_SCAN_PICTURE_I,
	LXVDEC_SCAN_PICTURE_IP,

	LXVDEC_SCAN_PICTURE_I_BRAINFART_DISH_TRICK,
};
typedef enum _LXVDEC_SCAN_PICTURE LXVDEC_SCAN_PICTURE;

enum _LXVDEC_MEMORY_FORMAT
{
	LXVDEC_MEMORY_FORMAT_RASTER,	/* decode on liner memory something like NV12 */
	LXVDEC_MEMORY_FORMAT_TILED,
};
typedef enum _LXVDEC_MEMORY_FORMAT LXVDEC_MEMORY_FORMAT;

enum _LXVDEC_CLOCKSOURCE
{
	LXVDEC_CLOCKSOURCE_STC,
	LXVDEC_CLOCKSOURCE_LOCAL,	// GSTC, local clock
};
typedef enum _LXVDEC_CLOCKSOURCE LXVDEC_CLOCKSOURCE;

enum _LXVDEC_PICTURE_TYPE
{
	LXVDEC_PICTURE_TYPE_I,
	LXVDEC_PICTURE_TYPE_P,
	LXVDEC_PICTURE_TYPE_B,
};
typedef enum _LXVDEC_PICTURE_TYPE LXVDEC_PICTURE_TYPE;


const char **lxvdec_get_mimetypes (void);
lxvdec *lxvdec_open (void);
void lxvdec_close (lxvdec *v);


/* set options */
int lxvdec_set_hwpath (lxvdec *v, int hw);
#define LXVDEC_FOURCC(a,b,c,d)	((a)<<0|(b)<<8|(c)<<16|(d)<<24)
int lxvdec_set_mediainfo (lxvdec *v,
		const char *container, unsigned int fourcc,
		int frame_width, int frame_height);
int lxvdec_set_mediainfo_gstreamer (lxvdec *v, const char *caps);
int lxvdec_set_input_device (lxvdec *v, LXVDEC_INPUT_DEVICE input);
int lxvdec_set_output_device (lxvdec *v, LXVDEC_OUTPUT_DEVICE output);
int lxvdec_set_memory_format (lxvdec *v, LXVDEC_MEMORY_FORMAT format);
int lxvdec_set_decoding_queue_size (lxvdec *v, int size);
int lxvdec_drop_error_frame (lxvdec *v, int threshold);
int lxvdec_use_copy_dpb (lxvdec *v, int copy_dpb);
int lxvdec_use_user_dpb (lxvdec *v, int user_dpb);
int lxvdec_add_user_dpb (lxvdec *v,
		unsigned long y, unsigned long cb, unsigned long cr,
		int stride);
int lxvdec_add_user_dpb_ion (lxvdec *v,
		int y, int cb, int cr,
		int stride, unsigned long priv);

int lxvdec_get_decoding_queue_size (lxvdec *v);
int lxvdec_get_decoding_queue_bytes (lxvdec *v);
int lxvdec_get_decoding_queue_size_max (lxvdec *v);
int lxvdec_get_decoding_queue_bytes_max (lxvdec *v);

int lxvdec_set_scan_picture (lxvdec *v, LXVDEC_SCAN_PICTURE scan);

int lxvdec_set_priv (lxvdec *v, void *priv);
void *lxvdec_get_priv (lxvdec *v);
int lxvdec_write_extra_data (lxvdec *v, unsigned char *data, int size);
int lxvdec_write_timestamp (lxvdec *v, long long ts);
int lxvdec_write_data (lxvdec *v, const unsigned char *data, int size);
int lxvdec_eos (lxvdec *v);
int lxvdec_get_device_filedescriptor (lxvdec *v);
int lxvdec_set_no_pollerr (lxvdec *v);



/* read decoded informations */
LXVDEC_READ lxvdec_read (lxvdec *v);
int lxvdec_read_picture (lxvdec *v,
		int *width, int *height, int *stride,
		unsigned long *y, unsigned long *cb, unsigned long *cr);
int lxvdec_get_display_size (lxvdec *v,
		int *width, int *height);
unsigned long lxvdec_read_tile_base (lxvdec *v);
int lxvdec_get_framerate (lxvdec *v, int *num, int *den);
long long lxvdec_get_sorted_pts (lxvdec *v);
unsigned int lxvdec_get_pts (lxvdec *v);
int lxvdec_get_crop (lxvdec *v,
		int *left, int *right, int *top, int *bottom);
int lxvdec_get_multi_picture (lxvdec *v);
int lxvdec_get_3d_type (lxvdec *v, enum lxvo_3d *type);
int lxvdec_get_par (lxvdec *v, unsigned int *par_w, unsigned int *par_h);
unsigned int lxvdec_get_interlace (lxvdec *v);
int lxvdec_is_interlace (lxvdec *v);
int lxvdec_get_error_blocks (lxvdec *v);
int lxvdec_get_userdata (lxvdec *v,
		unsigned char **data, int *size, int *decoder_id);
LXVDEC_PICTURE_TYPE lxvdec_get_picture_type (lxvdec *v);
int lxvdec_get_afd (lxvdec *v);
int lxvdec_get_minimum_user_dpb (lxvdec *v);
unsigned long lxvdec_get_user_dpb_priv (lxvdec *v);


/* read displayed informations */
int lxvdec_get_display_result (lxvdec *v, int *matched, int *display_queued);


/* vdec operations */
int lxvdec_unref_decode_buffer (lxvdec *v,
		unsigned long y, unsigned long cb, unsigned long cr);
int lxvdec_get_decoded_queue_size (lxvdec *v);
int lxvdec_flush (lxvdec *v);
int lxvdec_freeze (lxvdec *v, int on);
int lxvdec_set_display_sync (lxvdec *v, int on);

unsigned int lxvdec_get_global_stc (lxvdec *v, unsigned int *mask);

int lxvdec_steal_user_data (lxvdec *v, int steal);
int lxvdec_set_id (lxvdec *v, unsigned long id);
int lxvdec_set_low_latency (lxvdec *v, int low_latency);
int lxvdec_set_3d_type (lxvdec *v, enum lxvo_3d type);
int lxvdec_set_display_offset (lxvdec *v, int offset);

int lxvdec_get_cpb_info (lxvdec *v, unsigned long *addr, int *size,
		int *read_offset, int *write_offset);
int lxvdec_set_cpb (lxvdec *v, unsigned long addr, int size);
int lxvdec_set_request_pictures (lxvdec *v, int num);

int lxvdec_set_clocksource (lxvdec *v, LXVDEC_CLOCKSOURCE cs);
int lxvdec_set_basetime (lxvdec *v, long long stc_before, long long pts);
int lxvdec_set_speed (lxvdec *v, double speed);
int lxvdec_set_step (lxvdec *v);
int lxvdec_set_audio_channel (lxvdec *v, int channel);
LXVDEC_ERROR lxvdec_get_error (lxvdec *v);

#endif
