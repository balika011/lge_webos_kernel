#include "x_os.h"
#include "x_typedef.h"

// define this for building IC verification code
//#define IMG_IC_VERIFICATION 

#define GFX3D_IMG_OK         (0)
#define GFX3D_IMG_FAILED     (-1)
#define MAX_BINARY_LITE_SCRIPT_SIZE (92 * 1024 * 1024)
#define MAX_GOLDEN_SIZE (68 * 1024 * 1024)

#define EMU_LIST_LOCATION    (0x26000000)
#define PATTERN_FIX_LOCATION (0x27000000)

//use this pattern Taiji-ref5_f2208-2209_1920x1080_aa  for
//MT5399 SLT test
#define G3D_SLT_FB_ADDR 0x22442000 //(0x3391E000)

#define MOUNT_USB (0)

#define BYTES_PER_READ (512)
#define MAX_FILE_SIZE (32 * 1024 * 1024)
#define MAX_LENGTH_FILE_NAME (128)

//#define GFX3D_DEBUG_SLT

#if defined(__MODEL_slt__) || defined(IMG_IC_VERIFICATION)
extern INT32 Gfx3dEmuInit(void);
extern INT32 Gfx3dEmuDisplay(void);
extern INT32 Gfx3dEmuStart(INT32 i4Argc, const CHAR** szArgv);
#endif /* defined(__MODEL_slt__) || defined(IMG_IC_VERIFICATION) */

