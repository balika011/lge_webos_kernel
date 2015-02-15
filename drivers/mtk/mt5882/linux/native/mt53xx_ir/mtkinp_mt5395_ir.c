#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <asm/io.h>

#include "os.h"
#include "ir_if.h"
#include "u_drv_cust.h"
typedef struct _strctIRMsg
{
    IRRX_RAW_DATA_T rRaw;
    UINT32 u4BtnState;
    UINT32 u4Btn;
} LINUX_IRRC_NOTIFY_MSG;
static HANDLE_T _hLinuxMsgQue = (HANDLE_T)NULL;

struct event_dev {
    struct input_dev *input;    
};

struct ir_dev {
	char *name;
	struct event_dev dev;
};

struct ir_dev ir_table[] = {
	{ "mtkinp_events" },
	{ "mtkinp_events" }
};

#ifndef ARRAY_SIZE
	#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#endif

extern INT32 IRRC_Init(UINT32 u4PriType);
extern INT32 IRRC_WaitLinuxIr(UINT32* prBtnState, UINT32* prKey, IRRX_RAW_DATA_T* rRawData);
typedef int (*mtkinp_event_notify)(unsigned type, unsigned code, int value, void * tag);

static mtkinp_event_notify ir_nfy = NULL;
#define UNDEFINED_KEY -1

#ifdef SUPPORT_IR_CUSTOMIZE_SCAN_CODE
extern INT32 custom_ir_scan_code_translation(unsigned state,    /* in */
                                             int keysrc,        /* in */
                                             UINT32  key,       /* in */
                                             unsigned* pCode);  /* out */
#endif

static VOID mtkinp_ir_loop(VOID*  pv_arg)
{    
    UINT32 state;
    UINT32 key;
    IRRX_RAW_DATA_T raw;
    int value;
    unsigned type;
    unsigned code;
    int keysrc;
    
    printk("Start ir loop \n");    
    while(1)
    {
        IRRC_WaitLinuxIr(&state, &key, &raw);
        type = EV_KEY;
        if(state == IRRX_COND_BTN_DOWN )
        {
            value = 1;
        }
        else
        {
            value = 0;
        }
		
        if(key & 0x10000000)
        {
             keysrc = 1; /*Panel key*/
             key -= 0x10000000;
        }
        else
        {
             keysrc = 0; /*IR key*/
        }

        /* reset code */
        code = 0;

#ifdef SUPPORT_IR_CUSTOMIZE_SCAN_CODE
        {
            INT32 i4_ret = 0;
            i4_ret = custom_ir_scan_code_translation(state, value, key, &code);
            if (i4_ret == 1)
            {
                printk("[%s IR] KEY 0x%x map to %d. keysrc=%d\n", BRAND, key, code, keysrc);
                if(keysrc == 1)
                {
                    ir_nfy(type, code, value, &ir_table[1].dev);
                }
                else
                {
                    ir_nfy(type, code, value, &ir_table[0].dev);
                }
            }
            continue;
        }
#endif        

        /* code 500 to 540 : Additional keys. */
        /* code 541 to 640 : BTN_CUSTOM_1 to BTN_CUSTOM_100 */
        switch(key)
        {
            case BTN_TTX:           code = 500; break;
            case BTN_POP:           code = 501; break; 
            case BTN_TV:            code = 502; break;                   
            case BTN_TV_ANA:        code = 503; break;                   
            case BTN_TV_DIG:        code = 504; break;                   
            case BTN_WIDGET:        code = 505; break;                   
            case BTN_INDEX:         code = 506; break;                   
            case BTN_TOOLS:         code = 507; break;                   
            case BTN_NETFLIX:       code = 508; break;                   
            case BTN_ENTER:         code = 509; break;                   
            case BTN_SURROUND:      code = 510; break;                   
            case BTN_POWER_ON:      code = 511; break;                   
            case BTN_POWER_OFF:     code = 512; break;                   
            case BTN_COMPONENT_1:   code = 513; break;                   
            case BTN_COMPONENT_2:   code = 514; break;                   
            case BTN_PLAY:          code = 515; break;                   
            case BTN_PAUSE:         code = 516; break;                   
            case BTN_POWER_SAVING:  code = 517; break;                   
            case BTN_3D:            code = 518; break;                   
            case BTN_FUNCTION_1:    code = 519; break;                   
            case BTN_FUNCTION_2:    code = 520; break;                   
            case BTN_FUNCTION_3:    code = 521; break;                   
            case BTN_FUNCTION_4:    code = 522; break;                   
            case BTN_FUNCTION_5:    code = 523; break;        
            default: break;           
        }
        #if 0
        if (code == 0) /* not matched in above, check BTN_CUSTOM_1 to 100 no continue orz... so we could not do this */
        {
            if (key >= BTN_CUSTOM_1 && key <= BTN_CUSTOM_100)
            {
                code = key - BTN_CUSTOM_1 + 541;   /* starting from 541 */
            }
        }
        #endif

        if (code == 0) /* not matched in above, check BTN_CUSTOM_1 to 18 */
        {
            if (key >= BTN_CUSTOM_1 && key <= BTN_CUSTOM_18)
            {
                code = key - BTN_CUSTOM_1 + 541;   /* starting from 541 */
            }
        }

        if (code == 0) /* not matched in above, check BTN_CUSTOM_19 to 70 */
        {
            if (key >= BTN_CUSTOM_19 && key <= BTN_CUSTOM_70)
            {
                code = key - BTN_CUSTOM_19 + 559;   /* starting from 559 */
            }
        }

        if (code == 0) /* not matched in above, check BTN_CUSTOM_71 to 100 */
        {
            if (key >= BTN_CUSTOM_71 && key <= BTN_CUSTOM_100)
            {
                code = key - BTN_CUSTOM_71 + 611;   /* starting from 611 */
            }
        }
        
        if (code != 0) /* match in above, directly notify back */
        {
            printk("Got CUSTOMIZED\n");
	    printk("KEY 0x%x map to %d. keysrc=%d\n", key, code, keysrc);
            if(keysrc == 1)
            {
                ir_nfy(type, code, value, &ir_table[1].dev);
            }
            else
            {
                ir_nfy(type, code, value, &ir_table[0].dev);
            }
            continue;
        }

        switch(key & KEY_GROUP_MASK)
        {
            case KEY_GROUP_CURSOR:
                printk("Got CURSOR MOVING\n");
                switch(key)
                {
                    case BTN_CURSOR_LEFT:
                        code = KEY_LEFT;
                        break;
                    case BTN_CURSOR_RIGHT:
                        code = KEY_RIGHT;
                        break;
                    case BTN_CURSOR_UP:
                        code = KEY_UP;
                        break;
                    case BTN_CURSOR_DOWN:
                        code = KEY_DOWN;
                        break;
					default:
						code = UNDEFINED_KEY;
						break;
                }
                break;
			case KEY_GROUP_DIGIT:
                printk("Got DIGIT MOVING\n");
                switch(key)
                {
                    case BTN_DIGIT_0:
                        code = KEY_0;
                        break;
                    case BTN_DIGIT_1:
                        code = KEY_1;
                        break;
                    case BTN_DIGIT_2:
                        code = KEY_2;
                        break;
                    case BTN_DIGIT_3:
                        code = KEY_3;
                        break;
					case BTN_DIGIT_4:
                        code = KEY_4;
                        break;
                    case BTN_DIGIT_5:
                        code = KEY_5;
                        break;
                    case BTN_DIGIT_6:
                        code = KEY_6;
                        break;
                    case BTN_DIGIT_7:
                        code = KEY_7;
                        break;
					 case BTN_DIGIT_8:
                        code = KEY_8;
                        break;
                    case BTN_DIGIT_9:
                        code = KEY_9;
                        break;
					case BTN_DIGIT_DOT:
						code = KEY_DOT;
						break;
					default:
						code = UNDEFINED_KEY;
						break;
                }
                break;
            case KEY_GROUP_SEL_CTRL:
                printk("Got SEL \n");
                switch(key)
                {
                    case BTN_SELECT:
                        printk("ENTER\n");
                        code = KEY_ENTER;
                        break;
					case BTN_EXIT:
                        printk("EXIT\n");
                        code = KEY_EXIT;
                        break;
                    default:
						code = UNDEFINED_KEY;
						break;                        
                }
                break;
				
			case KEY_GROUP_USER_DEF:
                printk("Got USER_DEF \n");
                switch(key)
                {
                    case BTN_RED:
                        code = KEY_PLAYCD;
                        break;
					case BTN_GREEN:
						code = KEY_PAUSECD;
						break;
					case BTN_YELLOW:
						code = KEY_PROG3;
						break;
					case BTN_BLUE:
						code = KEY_PROG4;
						break;
                    default:
						code = UNDEFINED_KEY;
						break;                        
                }
                break;
				
			case KEY_GROUP_DVD_CTRL:
                printk("Got DVD \n");
                switch(key)
                {
                    case BTN_ROOT_MENU:
                        code = KEY_DASHBOARD;
                        break;
					case BTN_TITLE_PBC:
						code = KEY_SUSPEND;
						break;
					case BTN_SUB_TITLE:
						code = KEY_CLOSE;
						break;
					case BTN_REPEAT:
						code = KEY_PLAY;
						break;
                    default:
						code = UNDEFINED_KEY;
						break;                        
                }
                break;
				
			case KEY_GROUP_STRM_CTRL:
                printk("Got STRM \n");
                switch(key)
                {
                    case BTN_RECORD:
                        code = KEY_FASTFORWARD;
                        break;
                    case BTN_STOP:
                        code = KEY_DOCUMENTS;  /* same as BTN_FAV_CH */
                        break;
					case BTN_FR:
						code = KEY_BASSBOOST;
						break;
					case BTN_FF:
						code = KEY_PRINT;
						break;
					case BTN_PREV:
						code = KEY_HP;
						break;
					case BTN_NEXT:
						code = KEY_CAMERA;
						break;
                    default:
						code = UNDEFINED_KEY;
						break;                        
                }
                break;
				
			  case KEY_GROUP_FCT_CTRL:
                printk("Got FCT \n");
                switch(key)
                {
                    case BTN_MENU:
                        printk("MENU\n");
                        code = KEY_MENU;
                        break;
					case BTN_POWER:
                        printk("POWER\n");
                        code = KEY_POWER;
                        break;
					case BTN_EPG:
						code = KEY_SOUND;
						break;
					case BTN_MEM_CARD:
						code = KEY_QUESTION;
						break;
                    default:
						code = UNDEFINED_KEY;
						break;                        
                }
                break;

			 case KEY_GROUP_AUD_CTRL:
                printk("Got AUD \n");
                switch(key)
                {
                    case BTN_VOL_UP:
                        code = KEY_VOLUMEUP;
                        break;
					case BTN_VOL_DOWN:
                        code = KEY_VOLUMEDOWN;
                        break;
					case BTN_MUTE:
                        code = KEY_MUTE;
                        break;
					case BTN_S_EFFECT:
						code = KEY_EMAIL;
						break;
					case BTN_MTS:
						code = KEY_CHAT;
						break;
                    default:
						code = UNDEFINED_KEY;
						break;                        
                }
                break;	
				
			 case KEY_GROUP_PRG_CTRL:
                printk("Got PRG MOVING\n");
                switch(key)
                {
                    case BTN_PRG_UP:
                        code = KEY_SEARCH;
                        break;
					case BTN_PREV_PRG:
						code = KEY_CONNECT;
						break;
					case BTN_PRG_DOWN:
                        code = KEY_FINANCE;
                        break;
					case BTN_INPUT_SRC:
						code = KEY_SPORT;
						break;
					case BTN_TIMER:
						code = KEY_SHOP;
						break;
					case BTN_SLEEP:
						code = KEY_ALTERASE;
						break;
					case BTN_ZOOM:
						code = KEY_CANCEL;
						break;
					case BTN_P_EFFECT:
						code = KEY_BRIGHTNESSDOWN;
						break;
					case BTN_ASPECT:
						code = KEY_BRIGHTNESSUP;
						break;
					case BTN_PIP_POP:
						code = KEY_MEDIA;
						break;
					case BTN_PIP_POS:
						code = KEY_SWITCHVIDEOMODE;
						break;
					case BTN_PIP_SIZE:
						code = KEY_KBDILLUMTOGGLE;
						break;
					case BTN_CC:
						code = KEY_KBDILLUMDOWN;
						break;
					case BTN_SWAP:
						code = KEY_KBDILLUMUP;
						break;
					case BTN_FREEZE:
						code = KEY_SEND;
						break;
					case BTN_PRG_INFO:
						code = KEY_REPLY;
						break;
					case BTN_ADD_ERASE:
						code = KEY_FORWARDMAIL;
						break;
					case BTN_FAVORITE:
						code = KEY_SAVE;
						break;
					case BTN_FAV_CH:
						code = KEY_DOCUMENTS;
						break;
					default:
						code = UNDEFINED_KEY;
						break;
                }
                break;
            default:
				code = UNDEFINED_KEY;
				break;
        }
             
		
		if (UNDEFINED_KEY == code)
		{	
			//printk ("UNDEFINED KEY\n");
		}
		else
		{
			printk("KEY 0x%x map to %d\n", key, code);
            if(keysrc == 1)
            {
                ir_nfy(type, code, value, &ir_table[1].dev);
            }
            else
            {
                ir_nfy(type, code, value, &ir_table[0].dev);
            }
		}
    }
}
INT32 IRRC_WaitLinuxIr(UINT32* prBtnState, UINT32* prKey, IRRX_RAW_DATA_T* rRawData)
{
    INT32 i4Ret;
    CRIT_STATE_T cState;
    UINT16 u2MsgIdx;
    LINUX_IRRC_NOTIFY_MSG rMsg;
    SIZE_T zMsgSize = sizeof(LINUX_IRRC_NOTIFY_MSG);
    if ((prBtnState == NULL) || (prKey == NULL) || (rRawData == NULL))
    {
        printk("[%s]: Parameters error (null)\n", __FUNCTION__);
        return IR_FAIL;
    }
    if (!_hLinuxMsgQue)
    {
        printk("[%s]: __hLinuxMsgQue is uninit... \n", __FUNCTION__);     
        *prKey = BTN_NONE;
        //x_thread_delay(1000000);
        return IR_SUCC;//IR_FAIL;
    }    
    
    #if defined(CC_FAST_INIT) || defined(CC_SUPPORT_STR)
    i4Ret = x_msg_q_receive_timeout(&u2MsgIdx, &rMsg, &zMsgSize, &_hLinuxMsgQue, 1, 500);        
    #else
    i4Ret = x_msg_q_receive(&u2MsgIdx, &rMsg, &zMsgSize, &_hLinuxMsgQue, 1, X_MSGQ_OPTION_WAIT);    
    #endif
    //LOG(10, "[IRRC_WaitLinuxIr]: BtnState(0x%8x), Key(0x%8x)\n", rMsg.u4BtnState, rMsg.u4Btn);        
    if (i4Ret == OSR_OK)
    {
        cState = x_crit_start();
        
        rRawData->u1Protocol = rMsg.rRaw.u1Protocol;
        rRawData->u4Len = rMsg.rRaw.u4Len;
        x_memcpy(rRawData->au1Data, rMsg.rRaw.au1Data, rMsg.rRaw.u4Len);
        
        *prBtnState = rMsg.u4BtnState;
        *prKey = rMsg.u4Btn;
        x_crit_end(cState);

    }
    else
    {
        //printk("[%s]: _x_msg_q_rcv error(0x%x) \n", __FUNCTION__, i4Ret);     
        #if !defined(CC_FAST_INIT) && !defined(CC_SUPPORT_STR)
        //ASSERT((i4Ret == OSR_OK) || (i4Ret == OSR_TIMEOUT));
        #endif
        *prKey = BTN_NONE;
        #if defined(CC_FAST_INIT) || defined(CC_SUPPORT_STR)
        return IR_SUCC;
        #else
        return IR_FAIL;
        #endif
    }

    if (*prKey == BTN_NONE)
    {
        //LOG(9, "%s return Key:0x%08x\n", __FUNCTION__, *prKey);
    }
    return IR_SUCC;
}
#if defined(CC_SUPPORT_STR)
extern unsigned int skip_send_key;
#endif
static void Linux_IRRC_NotifyFunc(UINT32 u4BtnState, UINT32 u4Key, IRRX_RAW_DATA_T* prRaw)
{
    INT32 i4Ret;
    SIZE_T zMsgSize = sizeof(LINUX_IRRC_NOTIFY_MSG);
    LINUX_IRRC_NOTIFY_MSG rMsg;    
#if defined(CC_SUPPORT_STR)
    if (skip_send_key)
    {
	printk("(yjdbg) skip key: 0x%x\n", u4Key);
	return;
    }
#endif
    rMsg.rRaw.u1Protocol = (UINT8)prRaw->u1Protocol;
    rMsg.rRaw.u4Len = (SIZE_T)prRaw->u4Len;
    x_memcpy(rMsg.rRaw.au1Data, prRaw->au1Data, prRaw->u4Len);
    rMsg.u4BtnState = u4BtnState;
    rMsg.u4Btn = u4Key;
    i4Ret = x_msg_q_send(_hLinuxMsgQue, &rMsg, zMsgSize,1);
    if (i4Ret != OSR_OK)
    {
        printk("%s(%d) x_msg_q_send is fault. i4Ret:%d\n", __FILE__, __LINE__, i4Ret);
    }
}

static int mtk_drv_ir_init(mtkinp_event_notify nfy)
{
    INT32 ret;
    HANDLE_T h_thd;
    
    ir_nfy = nfy;

    ret = IRRC_Init(IRRX_RC5_TYPE);
 	if(x_msg_q_create(&_hLinuxMsgQue, "LinuxIrrcMsgQue", sizeof(LINUX_IRRC_NOTIFY_MSG), 16) != OSR_OK)
	{
		printk("%s x_msg_q_create fail'n", __FUNCTION__);
		return -1;
	}
	UNUSED(IRRC_RegCallbackFunc(Linux_IRRC_NotifyFunc));
    printk("Call IRRC_Init ret:%d\n", ret);
		x_thread_create(&h_thd, "mtkinp_ir", 4*1024, 20, mtkinp_ir_loop, 0, 0);
   	return 0; 
}

/*
 * Avoid using driver function for waiting key code, we don't
 * want to bind with driver's OS layer. Because the waitting 
 * function may use special OS functions which require thread 
 * context.
 */
//struct event_dev {
//    struct input_dev *input;    
//};

static int mtkinp_events_cb(unsigned type, unsigned code, int value,void * dev_id)
{
    struct event_dev *edev = dev_id;
    input_event(edev->input, type, code, value);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
    input_sync(edev->input);
#endif
    return 0;
}


static int mtkinp_events_setup(void)
{
    struct input_dev *input_dev;
    int ret;
    int i;
    printk("*** mtkinp events probe ***\n");
    mtk_drv_ir_init(mtkinp_events_cb);
    
    for (i=0; i<ARRAY_SIZE(ir_table); i++)
    {
    input_dev = input_allocate_device();
	    ir_table[i].dev.input = input_dev;
    
        /* indicate that we generate key events */
    set_bit(EV_KEY, input_dev->evbit);
    set_bit(EV_REL, input_dev->evbit);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
#else
    set_bit(EV_ABS, input_dev->evbit);
#endif

    /* indicate that we generate *any* key event */

    bitmap_fill(input_dev->keybit, KEY_MAX);
    bitmap_fill(input_dev->relbit, REL_MAX);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
#else
    bitmap_fill(input_dev->absbit, ABS_MAX);
#endif

	    input_dev->name = ir_table[i].name;
/*    input_dev->private = edev; */
    
    ret = input_register_device(input_dev);
	}
    (void)ret;
    return 0;

#if 0 /* no error handling ? */
//fail:
    kfree(edev);
    input_free_device(input_dev);
    return -EINVAL;
#endif
}

/*static struct platform_driver mtkinp_events_driver = {
    .probe = mtkinp_events_probe,
    .driver = {
        .name = "mtkinp_events",
    },
};*/

int mtkinp_events_init(void)
{
    //int ret;
    /*ret = platform_driver_register(&mtkinp_events_driver);*/
    
    mtkinp_events_setup();
    return 0;
}


EXPORT_SYMBOL(mtkinp_events_init);
