/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
/*#include <linux/mt53xx_eep.h>*/
#include <linux/semaphore.h>
#include <linux/slab.h>                        // For kmalloc
#include <asm/uaccess.h>
#include "x_typedef.h"

#ifdef CUSTOM_CFG_FILE
#include CUSTOM_CFG_FILE
#endif /* CUSTOM_CFG_FILE */

/* Hard code temprarily to fix the ACFG init failure */
#ifndef SYSTEM_EEPROM_SIZE
#error "No SYSTEM_EEPROM_SIZE defined"
#endif

/* drv_default.h MUST be placed after CUSTOM_CFG_FILE */
#include "drv_default.h"
#include <linux/version.h>

#include "eepdef.h"

#define EEP_DEVICE_MAX           (3)

#define IO_NODE_MAJOR_EEP0       (228)
#define IO_NODE_MAJOR_EEP1       (227)
#define IO_NODE_MAJOR_EEP2       (226)

#define DEV_EEP0                 "/dev/eeprom_0"
#define DEV_EEP1                 "/dev/eeprom_3"
#define DEV_EEP2                 "/dev/eeprom_4"


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)
#define DECLARE_MUTEX(mutex)  DEFINE_SEMAPHORE(mutex)
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
#define USE_UNLOCK_IOCTL
#endif

typedef struct
{
    unsigned int            major;
    unsigned int            minor;
    char                    *name;
    umode_t                 mode;
    struct file_operations  *fops;
} DEVLIST_T;

typedef struct
{
    unsigned int           ui4_base;
    unsigned int           ui4_size;
} EEP_ARGUMENT;

static ssize_t _eep0_read(struct file * fd, char __user * buf, size_t count, loff_t * poffset);
static ssize_t _eep1_read(struct file * fd, char __user * buf, size_t count, loff_t * poffset);
static ssize_t _eep2_read(struct file * fd, char __user * buf, size_t count, loff_t * poffset);
static ssize_t _eep0_write(struct file * fd , const char __user * buf, size_t count, loff_t * poffset);
static ssize_t _eep1_write(struct file * fd , const char __user * buf, size_t count, loff_t * poffset);
static ssize_t _eep2_write(struct file * fd , const char __user * buf, size_t count, loff_t * poffset);

#ifndef USE_UNLOCK_IOCTL
static int _eep0_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long param);
static int _eep1_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long param);
static int _eep2_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long param);

#define FOPS_IOCTL(a)   .ioctl = a ## _ioctl
#else
static long _eep0_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long param);
static long _eep1_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long param);
static long _eep2_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long param);

#define FOPS_IOCTL(a)   .unlocked_ioctl = a ## _unlocked_ioctl
#endif

struct file_operations eep0_fops = {
    .read  = _eep0_read,
    .write = _eep0_write,
    .llseek = default_llseek,
    FOPS_IOCTL(_eep0), 
    };

struct file_operations eep1_fops = {
    .read  = _eep1_read,
    .write = _eep1_write,
    .llseek = default_llseek,
    FOPS_IOCTL(_eep1), 
    };

struct file_operations eep2_fops = {
    .read  = _eep2_read,
    .write = _eep2_write,
    .llseek = default_llseek,
    FOPS_IOCTL(_eep2),
    };

static const DEVLIST_T devlist_eep[] =
{
    {IO_NODE_MAJOR_EEP0, 0, DEV_EEP0, S_IRUSR | S_IWUSR | S_IRGRP, &eep0_fops},
    {IO_NODE_MAJOR_EEP1, 0, DEV_EEP1, S_IRUSR | S_IWUSR | S_IRGRP, &eep1_fops},
    {IO_NODE_MAJOR_EEP2, 0, DEV_EEP2, S_IRUSR | S_IWUSR | S_IRGRP, &eep2_fops},
};

#if 0 /* Modify for hard code */
static const EEP_ARGUMENT eep_argument[] =
{
    {(2048 - 928),928},
    {0,2048},
};
#else
static const EEP_ARGUMENT eep_argument[] =
{
    {(DRVCUST_EEPROM_MW_OFFSET + DRVCUST_EEPROM_MW_SIZE), (SYSTEM_EEPROM_SIZE - DRVCUST_EEPROM_MW_OFFSET - DRVCUST_EEPROM_MW_SIZE)},
    {0,SYSTEM_EEPROM_SIZE},
    {DRVCUST_EEPROM_MW_OFFSET, DRVCUST_EEPROM_MW_SIZE},
};

#endif

extern int EEPROM_GetSize(void);
extern INT16 EEPROM_Init(VOID);
extern unsigned int EEPROM_Read(unsigned long long u8Offset, unsigned int u4MemPtr, unsigned int u4MemLen);
extern unsigned int EEPROM_Write(unsigned long long u8Offset, unsigned int u4MemPtr, unsigned int u4MemLen);

#if 0
#define DEBUG
#endif
#ifdef DEBUG
    #define DBG(fmt...)    printk("%s %d:",__FILE__,(unsigned int)__LINE__);  printk(fmt)
#else
    #define DBG(fmt...)
#endif

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
/*lint -save -e* */
static DECLARE_MUTEX(_bMutex_eep);

/*lint -restore */
static int _eep_get_size(unsigned int * pu4Size)
{
    *pu4Size = (unsigned int)EEPROM_GetSize();
    return 0;
}

static int _eep_init(void)
{
    unsigned char i  = 0;

    /* Register nodes and create devices */
    for (i = 0; i < EEP_DEVICE_MAX; i++)
    {
        if (register_chrdev(devlist_eep[i].major, devlist_eep[i].name, devlist_eep[i].fops))
        {
            DBG("[***EEP***]Unable to get major %d for %s devs\n", devlist_eep[i].major,
                devlist_eep[i].name);
            return -1;
        }
    }
    
    return 0;
}

static int _eep_release(void)
{
    unsigned char i  = 0;

    /* Unregister nodes and destroy devices */
    for (i = 0; i < EEP_DEVICE_MAX; i++)
    {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
	unregister_chrdev(devlist_eep[i].major, devlist_eep[i].name);
#else
        if (unregister_chrdev(devlist_eep[i].major, devlist_eep[i].name))
        {
            DBG("[***EEP***]Unable to release major %d for %s devs\n", devlist_eep[i].major,
               devlist_eep[i].name);
            return -1;
        }
#endif
    }
    
    return 0;
}

ssize_t __eep_read(unsigned char uc_id, char __user * buf, size_t count, loff_t * poffset)
{
    int iRet = 0;   
    /* char ctmp = 0;*/
    unsigned int u4EepSize = 0;   
    /* unsigned int u4idx = 0; */
    char * kmem = 0;  /* kernel memory */
    char * kmt = 0;    /* kernel memory */
    DBG("[***EEP***]eep_read, uc_id=%d, buf=%08X, count=%d, offset=%d\n", uc_id, (unsigned int)buf, count, (unsigned int)*poffset);
    DBG("\n=================READ BEGIN====================\n");

    if (down_interruptible(&_bMutex_eep))
    {
        return (-ERESTARTSYS);
    }

    if(uc_id >= EEP_DEVICE_MAX)
    {
        iRet = -EINVAL;
        goto _EEP_RELEASE_SEMA_;         //invalid argument
    }

    if ( (*poffset < 0) || (count == 0) )
    {
        iRet = -EINVAL;
        goto _EEP_RELEASE_SEMA_;         //invalid argument
    }

    {
        int ret = 0;
        unsigned int ui4_size = eep_argument[uc_id].ui4_size;

        ret = _eep_get_size(&u4EepSize);                     /*check is it at the End-of-File (last byte of eeprom)*/

        if(ret != 0)
        {
            iRet = -ENODEV;
            goto _EEP_RELEASE_SEMA_;         
        }

        if(eep_argument[uc_id].ui4_base + ui4_size > u4EepSize)
        {
            iRet = -EINVAL;
            goto _EEP_RELEASE_SEMA_;
        }

        if( *poffset > ui4_size)
        {
            iRet = -EINVAL;
            goto _EEP_RELEASE_SEMA_;         //invalid argument
        }
    
        if( *poffset + count > ui4_size)
        {
            count = ui4_size - *poffset;
        }

    }

    kmem=kmalloc(count, GFP_KERNEL);        //return 0 -->out of memory
    
    if( !kmem )
    {
        iRet = -ENOMEM;     //out of memory
        goto _EEP_RELEASE_SEMA_;
    }

    kmt = kmem;

#if 1
    iRet = EEPROM_Read( (unsigned long long)((*poffset)+ eep_argument[uc_id].ui4_base) , (unsigned int)kmt, count);
    if(0 == iRet)
    {
        iRet = count ;
    }

    *poffset += count;
#else
    do
    {
        iRet = EEPROM_Read( (unsigned long long)((*poffset) + eep_argument[uc_id].ui4_base + kmt - kmem) , (unsigned int)&ctmp, 1);
        if(iRet == 0)
        {
            (*kmt++) = ctmp ; 
        }
    }while( (iRet ==0) && (kmt<(kmem+count)));
    iRet = (kmt - kmem);            //accumulate real number of bytes read
    *poffset += (kmt - kmem);
#endif
    
    if (copy_to_user((void __user *)buf, (void *)kmem, count) )  //eeprom data kernel-->user
    {
        iRet = -ENOBUFS;
    }

_EEP_RELEASE_SEMA_:
    {
        up(&_bMutex_eep);
        kfree(kmem);
    }

    DBG("\n=================READ END====================\n");

    return iRet;
}

ssize_t __eep_write(unsigned char uc_id, const char __user * buf, size_t count, loff_t * poffset)
{
    int iRet = 0;   
    unsigned int u4EepSize = 0;   
    char * kmem = 0; /*kernel memory */
    char * kmt = 0;    /*kernel memory */
    DBG("[***EEP***]eep_write, uc_id=%d, buf=%08X, count=%d, offset=%d\n", uc_id, (unsigned int)buf, count, (unsigned int)*poffset);
    DBG("\n=================WRITE BEGIN====================\n");

    if (down_interruptible(&_bMutex_eep))
    {
        return (-ERESTARTSYS);
    }

    if(uc_id >= EEP_DEVICE_MAX)
    {
        iRet = -EINVAL;
        goto _EEP_RELEASE_SEMA_;         //invalid argument
    }

    if ( (*poffset < 0) || (count == 0) )
    {
        iRet = -EINVAL;         //invalid argument
        goto _EEP_RELEASE_SEMA_;         //invalid argument
    }

    {
        int ret = 0;
        unsigned int ui4_size = eep_argument[uc_id].ui4_size;

        ret = _eep_get_size(&u4EepSize);                     /*check is it at the End-of-File (last byte of eeprom)*/

        if(ret != 0)
        {
            iRet = -ENODEV;
            goto _EEP_RELEASE_SEMA_;
        }

        if(eep_argument[uc_id].ui4_base + ui4_size > u4EepSize)
        {
            iRet = -EINVAL;
            goto _EEP_RELEASE_SEMA_;
        }

        if( *poffset > ui4_size)
        {
            iRet = -EINVAL;
            goto _EEP_RELEASE_SEMA_;         //invalid argument
        }

        if( *poffset + count > ui4_size)
        {
            count = ui4_size - *poffset;
        }

    }

    kmem=kmalloc(count, GFP_KERNEL);        //return 0 -->out of memory
    
    if( !kmem )
    {
        iRet = -ENOMEM;     //out of memory
        goto _EEP_RELEASE_SEMA_;
    } 
    
    if (copy_from_user( (void*)kmem , (void __user *)buf, count ) )   //user-->kernel
    {
        iRet = -ENOBUFS;
        goto _EEP_RELEASE_SEMA_;
    }

    kmt = kmem;

#if 1
    iRet = EEPROM_Write( (unsigned long long)((*poffset) + eep_argument[uc_id].ui4_base) , (unsigned int)kmt, count);
    if(iRet == 0)
    {
        iRet = count;
    }

    *poffset += count;
#else
    do
    {
        iRet = EEPROM_Write( (unsigned long long)((*poffset) + eep_argument[uc_id].ui4_base + kmt - kmem) , (unsigned int)kmt, 1);
        if(iRet == 0)
        {
            kmt++; 
        }
    }while( (iRet ==0) && (kmt<(kmem+count)));
    iRet = (kmt - kmem);            //accumulate real number of bytes read
    *poffset += (kmt - kmem);
#endif
    
_EEP_RELEASE_SEMA_:
    {
        up(&_bMutex_eep);
        kfree(kmem);
    }

    DBG("\n=================WRITE END====================\n");

    return iRet;
}

static int _eep_ioctl(unsigned char uc_id, struct file *file, unsigned int cmd, unsigned long param)
{
    int           iRet = 0;  
    unsigned int  ui4_size;
    UINT32 u4NptvDriverOffset = 0;
    UINT32 u4MacEEPAddress = 0;	
    
    if (down_interruptible(&_bMutex_eep))
    {
        return (-ERESTARTSYS);
    }

    do 
    {
        if (uc_id >= EEP_DEVICE_MAX)
        {
            iRet = -EINVAL;
            break;         
        }
    
        switch (cmd)
        {
            case 0x200:  /*get file size*/
                ui4_size = eep_argument[uc_id].ui4_size;
                
                if (copy_to_user((void __user *)param, (void *)&ui4_size, sizeof(int)))  
                {
                    iRet = -ENOBUFS;
                }
                break;
			case 0x300:
				if (DRVCUST_OptQuery(eEepromNptvDriverOffset, &u4NptvDriverOffset) != 0)
			    {
			        return -1;
			    }

			    u4MacEEPAddress = u4NptvDriverOffset + EEP_ETH_MAC_ADDR_START;

				if (copy_to_user((void __user *)param, (void *)&u4MacEEPAddress, sizeof(UINT32)))  			
                {
                    iRet = -ENOBUFS;
                }
				break;
            default:
                DBG("_eep_ioctl: unknow cmd=%u\r\n", cmd);
                iRet = -EINVAL;
        }
        
    } while (0);

    up(&_bMutex_eep);

    return iRet;
}


static ssize_t _eep0_read(struct file * fd, char __user * buf, size_t count, loff_t * poffset)
{
    return __eep_read(0, buf, count, poffset);
}

static ssize_t _eep1_read(struct file * fd, char __user * buf, size_t count, loff_t * poffset)
{
    return __eep_read(1, buf, count, poffset);
}

static ssize_t _eep2_read(struct file * fd, char __user * buf, size_t count, loff_t * poffset)
{
    return __eep_read(2, buf, count, poffset);
}

static ssize_t _eep0_write(struct file * fd , const char __user * buf, size_t count, loff_t * poffset)
{
    return __eep_write(0, buf, count, poffset);
}

static ssize_t _eep1_write(struct file * fd , const char __user * buf, size_t count, loff_t * poffset)
{
    return __eep_write(1, buf, count, poffset);
}

static ssize_t _eep2_write(struct file * fd , const char __user * buf, size_t count, loff_t * poffset)
{
    return __eep_write(2, buf, count, poffset);
}

#ifdef USE_UNLOCK_IOCTL
long _eep0_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long param)
{
    return _eep_ioctl(0, file, cmd, param);
}
      
long _eep1_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long param)
{
    return _eep_ioctl(1, file, cmd, param);
}

long _eep2_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long param)
{
    return _eep_ioctl(2, file, cmd, param);
}
#else
int _eep0_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long param)
{
    return _eep_ioctl(0, file, cmd, param);
}
      
int _eep1_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long param)
{
    return _eep_ioctl(1, file, cmd, param);
}

int _eep2_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long param)
{
    return _eep_ioctl(2, file, cmd, param);
}
#endif /* USE_UNLOCK_IOCTL */

int __init eeprom_module_init(void)
{
    unsigned int ui4_size = 0;
    int ret = 0;

    /* Init driver. */
    ret = _eep_init();

    DBG(">>>>>>>>>>>>>>>>>>>>>>>NEW_EEP>>>>>>>>>>>>>>>>>\r\n");

    if(ret != 0)
    {
        DBG("[***EEP***]Init EEP driver failed!");
    }

    ret  = _eep_get_size(&ui4_size);
    DBG(KERN_ALERT "[***EEP***]Init EEP driver - get size ret %d size 0x%X\r\n",ret,ui4_size);

	EEPROM_Init();
#if 0
    {
        struct file * fd = filp_open("/dev/eeprom_3", O_RDWR, 0);
        char buf[2048];
        memset(buf,0,2048);
        DBG("open ok\n");
        _eep1_write(fd, buf, 2048, 0);
        DBG("write ok\n");
    }
#endif
    return 0;
}

void __exit eeprom_module_exit(void)
{
    int ret = 0;

    DBG(KERN_ALERT "[***EEP***]Release EEP Driver\n");

    /* Init driver. */
    ret = _eep_release();

    if(ret != 0)
    {
        DBG("[***EEP***]Release EEP driver failed!");
    }

    return;
}

