#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/version.h>

#include "os.h"
#include "ir_if.h"
#include "u_drv_cust.h"


typedef struct mtk_to_scan_code_key_map {
         int mtk_code;
         int scan_code;
 } MTK_TO_SCAN_CODE_T;

MTK_TO_SCAN_CODE_T ar_mtk_to_scan_code_table[] = {
{BTN_ROOT_MENU,	172                },
{BTN_EXIT,	158                },
{BTN_DIGIT_0,	11                 },
{BTN_DIGIT_1,	2                  },
{BTN_DIGIT_2,	3                  },
{BTN_DIGIT_3,	4                  },
{BTN_DIGIT_4,	5                  },
{BTN_DIGIT_5,	6                  },
{BTN_DIGIT_6,	7                  },
{BTN_DIGIT_7,	8                  },
{BTN_DIGIT_8,	9                  },
{BTN_DIGIT_9,	10                 },
{BTN_CUSTOM_37,	544                },
{BTN_ENTER,	545                },
{BTN_DIGIT_DOT,	52                 },
{BTN_VOL_UP,	115                },
{BTN_VOL_DOWN,	114                },
{BTN_MUTE,	113                },
{BTN_POWER,	116                },
{BTN_POWER_ON,	143                },
{BTN_POWER_OFF,	142                },
{BTN_CUSTOM_73,	413                },
{BTN_EPG,	362                },
{BTN_CUSTOM_63,	615                },
{BTN_SELECT,	353                },
{BTN_SELECT,	616                },
{BTN_PRG_UP,	402                },
{BTN_PRG_DOWN,	403                },
{BTN_CURSOR_UP,	103                },
{BTN_CURSOR_UP,	617                },
{BTN_CURSOR_DOWN,	108        },
{BTN_CURSOR_DOWN,	618        },
{BTN_CURSOR_LEFT,	105        },
{BTN_CURSOR_LEFT,	619        },
{BTN_CURSOR_RIGHT,	106        },
{BTN_CURSOR_RIGHT,	620        },
{BTN_TV_ANA,	548                },
{BTN_CUSTOM_81,	549                },
{BTN_CUSTOM_80,	550                },
{BTN_CUSTOM_79,	551                },
{BTN_CUSTOM_74,	552                },
{BTN_CUSTOM_87,	381                },
{BTN_CUSTOM_68,	553                },
{BTN_TV_DIG,	621                },
{BTN_CUSTOM_68,	622                },
{BTN_TV_ANA,	554                },
{BTN_PRG_INFO,	358                },
{BTN_CUSTOM_88,	138                },
{BTN_CUSTOM_47,	623                },
{BTN_CUSTOM_44,	555                },
{BTN_PIP_POP,	556                },
{BTN_SUB_TITLE,	370                },
{BTN_CC,	624                },
{BTN_3D,	557                },
{BTN_MTS,	558                },
{BTN_PREV_PRG,	559                },
{BTN_ASPECT,	375                },
{BTN_CUSTOM_60,	174                },
{BTN_CUSTOM_89,	385                },
{BTN_CUSTOM_95,	560                },
{BTN_CUSTOM_96,	561                },
{BTN_CUSTOM_97,	562                },
{BTN_CUSTOM_11,	377                },
{BTN_INPUT_SRC,	563                },
{BTN_CUSTOM_14,	564                },
{BTN_CUSTOM_15,	565                },
{BTN_CUSTOM_17,	376                },
{BTN_CUSTOM_31,	566                },
{BTN_COMPONENT_2,	567        },
{BTN_CUSTOM_26,	568                },
{BTN_CUSTOM_27,	569                },
{BTN_CUSTOM_56,	570                },
{BTN_CUSTOM_66,	571                },
{BTN_RED,	398                },
{BTN_GREEN,	399                },
{BTN_YELLOW,	400                },
{BTN_BLUE,	401                },
{BTN_CUSTOM_78,	572                },
{BTN_TTX,	388                },
{BTN_PAUSE,	164                },
{BTN_STOP,	128                },
{BTN_NEXT,	163                },
{BTN_PREV,	165                },
{BTN_FR,		168        },
{BTN_FF,		208        },
{BTN_CUSTOM_86,	573                },
{BTN_CUSTOM_85,	574                },
{BTN_PLAY,	207                },
{BTN_CUSTOM_58,	167                },
{BTN_CUSTOM_71,	575                },
{BTN_CUSTOM_67,	576                },
{BTN_CUSTOM_42,	577                },
{BTN_CUSTOM_43,	395                },
{BTN_CUSTOM_201,	578        },
{BTN_CUSTOM_202,	579        },
{BTN_CUSTOM_203,	580        },
{BTN_MENU,	581                },
{BTN_CUSTOM_34,	582                },
{BTN_NETFLIX,	583                },
{BTN_CUSTOM_54,	584                },
{BTN_CUSTOM_204,	585        },
{BTN_CUSTOM_64,	608                },
{BTN_CUSTOM_205,	609        },
{BTN_CUSTOM_92,	610                },
{BTN_CUSTOM_206,	611        },
{BTN_CUSTOM_207,	612        },
{BTN_P_EFFECT,	613                },
{BTN_S_EFFECT,	614                },
};

int num_of_ar_mtk_to_scan_code_table = sizeof(ar_mtk_to_scan_code_table) / sizeof(ar_mtk_to_scan_code_table[0]);

/*
    return values:
           1: dispatch
           0: ignore
*/
extern INT32 custom_ir_scan_code_translation(unsigned state,    /* in */
                                             int keysrc,        /* in */
                                             UINT32  key,       /* in */
                                             unsigned* pCode)   /* out */
{
    BOOL b_dispatch = FALSE;
    int i;

    for (i = 0 ; i < num_of_ar_mtk_to_scan_code_table ; i++)
    {
       if (key == ar_mtk_to_scan_code_table[i].mtk_code)
       {
           *pCode = ar_mtk_to_scan_code_table[i].scan_code;
           b_dispatch = TRUE;
           break;
       }
    }

    if (b_dispatch == FALSE)
    {
        if (key >= KEY_GROUP_DIGIT && key <= KEY_GROUP_MAX)
        {
            printk("[%s IR] KEY 0x%x not translated.\n", BRAND, key);
        }
    }

    if (b_dispatch == TRUE)
    {
        return 1;
    }

    return 0;
}

