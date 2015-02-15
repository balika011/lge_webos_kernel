#ifndef _LXVO_H
#define _LXVO_H

struct lxvo;

typedef enum 
{
	/* from Frame Packing Arrangement(FPA) */
	lxvo_3d_checkerboard = 0,	// pixels are alternatively from L and R
	lxvo_3d_column_alternation,	// L and R are interlaced by column
	lxvo_3d_row_alternation,	// L and R are interlaced by row
	lxvo_3d_side_by_side,		// L is on the left, R on the right
	lxvo_3d_top_bottom,		// L is on top, R on bottom
	lxvo_3d_frame_alternation,	// one view per frame
	lxvo_3d_fpa_end = lxvo_3d_frame_alternation,

	/* user defined */
	lxvo_3d_dual,
	lxvo_3d_side_by_side_rl,
	lxvo_3d_bottom_top,
	lxvo_3d_2d,
}lxvo_3d;

#define LXVO_FCC(a,b,c,d)	(((a)<<24)|((b)<<16)|((c)<<8)|((d)<<0))
enum lxvo_info
{
	lxvo_info_none		= LXVO_FCC('N','O','N','E'),
	lxvo_info_displayed	= LXVO_FCC('D','I','S','P'),
	lxvo_info_dropped	= LXVO_FCC('D','R','O','P'),
	lxvo_info_empty		= LXVO_FCC('E','M','P','T'),
};

struct lxvo *lxvo_open (void);
void lxvo_close (struct lxvo *vo);
int lxvo_get_device_filedescriptor (struct lxvo *vo);
int lxvo_set_framebuffer_info (struct lxvo *vo,
		int width, int height, int stride);
int lxvo_set_framerate (struct lxvo *vo, int num, int den);
int lxvo_set_framerate_es (struct lxvo *vo, int num, int den);
int lxvo_set_framerate_container (struct lxvo *vo, int num, int den);
int lxvo_set_color_subsample (struct lxvo *vo, int ver, int hor);
int lxvo_set_3d (struct lxvo *vo, enum lxvo_3d trid);
int lxvo_set_tile_base (struct lxvo *vo, unsigned long base);
int lxvo_set_speed (struct lxvo *vo, float speed);
int lxvo_flush (struct lxvo *vo);
int lxvo_write_picture (struct lxvo *vo,
		unsigned long y, unsigned long c,
		long long pts, int droppable, void *priv);

enum lxvo_info lxvo_read_info (struct lxvo *vo);
void *lxvo_get_priv (struct lxvo *vo);
int lxvo_read_picture (struct lxvo *vo, void **priv);

int lxvo_set_displaying_queue_size (struct lxvo *vo, int size);
int lxvo_get_displaying_queue_size (struct lxvo *vo);
int lxvo_get_displayed_queue_size (struct lxvo *vo);
int lxvo_set_crop (struct lxvo *vo,
		int left, int right, int top, int bottom);
int lxvo_set_basetime (struct lxvo *vo, long long stc_before, long long pts);
int lxvo_get_stc (struct lxvo *vo, unsigned int *stc, unsigned int *mask);
int lxvo_set_display_offset (struct lxvo *vo, int display_offset);
int lxvo_set_multi_picture (struct lxvo *vo, int index);
int lxvo_set_par (struct lxvo *vo, unsigned int par_w, unsigned int par_h);

int lxvo_set_deid (struct lxvo *vo, int de_id);
int lxvo_set_mirror (struct lxvo *vo, int port);
int lxvo_set_output_port (struct lxvo *vo, int port);
int lxvo_set_low_latency (struct lxvo *vo, int low_latency);
int lxvo_set_freerun (struct lxvo *vo, int freerun);
int lxvo_set_constant_delay (struct lxvo *vo, int delay);
int lxvo_set_UHD (struct lxvo *vo, int uhd);
int lxvo_set_interlace (struct lxvo *vo, unsigned int interlace);

void *lxvo_alloc_privdata (struct lxvo *vo, const void *data, int size);
void lxvo_free_privdata (struct lxvo *vo, void *priv);

#endif
