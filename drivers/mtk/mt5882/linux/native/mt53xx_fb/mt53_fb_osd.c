#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>
#include "osd_drvif.h"
#include "osd_exp.h"

static VOID osd_vsync_isr(VOID);
static VOID osd_resume_suspend(BOOL fgSuspend);
static VOID osd_timing_changed(VOID);


static OSD_EXP_CALLBACK_T rOsdCallback = {
    .i4Identity = 0,
    .pfnVsyncIsr = osd_vsync_isr,
    .pfnResumeSuspend = osd_resume_suspend,
    .pfnTimingChanged = osd_timing_changed
};

static wait_queue_head_t _rVsyncQueue;
static BOOL _fgVsyncEnable = FALSE;

static VOID osd_vsync_isr(VOID)
{
    if(_fgVsyncEnable == FALSE)
    {
        _fgVsyncEnable = TRUE;
    }

    wake_up_all(&_rVsyncQueue);
}

static VOID osd_resume_suspend(BOOL fgSuspend)
{
    if(fgSuspend == TRUE)
    {
        _fgVsyncEnable = FALSE;
        wake_up_all(&_rVsyncQueue);
    }
}

static VOID osd_timing_changed(VOID)
{
}

void fb_osd_init(void)
{
    OSD_EXP_RET_T rRet;
    
    rRet = OSD_EXP_GenCallback(&rOsdCallback.i4Identity);
    if(rRet != OSD_EXP_RET_OK)
    {
        return;
    }

    rRet = OSD_EXP_RegisterCallback(&rOsdCallback);
    if(rRet != OSD_EXP_RET_OK)
    {
        return;
    }

    init_waitqueue_head(&_rVsyncQueue);
}

void fb_osd_wait_vsync(void)
{
    if(_fgVsyncEnable == TRUE)
    {
        DEFINE_WAIT(wait);
        prepare_to_wait(&_rVsyncQueue, &wait, TASK_UNINTERRUPTIBLE);
        schedule_timeout(500);
        finish_wait(&_rVsyncQueue, &wait);
    }
    else
    {
        x_thread_delay(20);
    }
}

void fb_osd_wait_stable_recovery(void)
{
    if(_fgVsyncEnable == FALSE)
    {
        x_thread_delay(20);
    }
}

