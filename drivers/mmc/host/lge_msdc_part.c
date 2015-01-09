/*
 * mtk emmc partition operation functions
 */

#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/semaphore.h>
#include <linux/scatterlist.h>
#include <linux/blkdev.h>
#include <linux/module.h>

#include <linux/mmc/host.h>
#include <linux/mmc/card.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sd.h>
#include <linux/mmc/mtk_msdc_part.h>

#include "mtk_msdc_host_hw.h"
#include "mtk_msdc_slave_hw.h"
#include "mtk_msdc_drv.h"
extern unsigned int lgemmc_get_part_offsest_and_size(int partnum, unsigned long long *poffset, unsigned long long *psize);

struct msdc_host *msdc_host_boot;
int (*msdc_send_command)(struct msdc_host *host, struct mmc_command *cmd);





// The max I/O is 32KB for finetune r/w performance.
#define MSDC_MAXPAGE_ORD    3
#define MSDC_MAXBUF_CNT     (PAGE_SIZE * (1<<MSDC_MAXPAGE_ORD))
static struct page *_blkpages = NULL;
static u_char *_pu1blkbuf = NULL;

static int msdc_alloc_reqpages(void)
{
    if (!_blkpages)
    {
        _blkpages = alloc_pages(GFP_KERNEL, MSDC_MAXPAGE_ORD);
        if (!_blkpages)
        {
            printk(KERN_ERR "alloc raw r/w page buffer fail!\n");
            return -ENOMEM;
        }

        _pu1blkbuf = (u_char *)page_address(_blkpages);
    }

    return 0;
}
static int msdc_free_reqpages(void)
{
    if (!_blkpages)
    {
        __free_pages(_blkpages, MSDC_MAXPAGE_ORD);
        _blkpages = NULL;
        _pu1blkbuf = NULL;
    }
    return 0;
}
static int msdc_sendrequest(uint64_t offset, uint64_t size, struct page *page, int read)
{
    u32 readcmd, writecmd;
    struct mmc_host *mmc = msdc_host_boot->mmc;
    struct mmc_card *card = mmc->card;
    struct mmc_command cmd, stop;
    struct mmc_data data;
    struct scatterlist sg;
    
    BUG_ON((offset%512) || (size%512));
    BUG_ON(size > MSDC_MAXBUF_CNT);
    
    // switch to user-area access
    //if(msdc_switch_part(0x00))
    //printk(KERN_ERR "switch part access to usr-area failed!\n");
    
    memset(&cmd,  0, sizeof(struct mmc_command));
    memset(&stop, 0, sizeof(struct mmc_command));
    memset(&data, 0, sizeof(struct mmc_data));
    cmd.data = &data;
    
    if (mmc_card_blockaddr(card))
    {
        cmd.arg = offset>>9;
    }
    else
    {
        cmd.arg = offset;
    }
    
    cmd.flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC;
    data.blksz = 512;
    stop.opcode = MMC_STOP_TRANSMISSION;
    stop.arg = 0;
    stop.flags = MMC_RSP_SPI_R1B | MMC_RSP_R1B | MMC_CMD_AC;
    data.blocks = size>>9;

    if (data.blocks > 1)
    {
        // Add this for auto command12
        data.stop = &stop;
    	readcmd = MMC_READ_MULTIPLE_BLOCK;
    	writecmd = MMC_WRITE_MULTIPLE_BLOCK;
    }
    else
    {
    	readcmd = MMC_READ_SINGLE_BLOCK;
    	writecmd = MMC_WRITE_BLOCK;
    }
        
    if (read)
    {
        cmd.opcode = readcmd;
        data.flags |= MMC_DATA_READ;
    }
    else
    {
        cmd.opcode = writecmd;
        data.flags |= MMC_DATA_WRITE;
    }
    
    mmc_set_data_timeout(&data, card);
    
    sg_init_table(&sg, 1);
    sg_set_page(&sg, page, size, 0);
    sg_mark_end(&sg);
    
	data.sg = &sg;
	data.sg_len = 1;
    
    down(&msdc_host_boot->msdc_sema);
    
    msdc_send_command(msdc_host_boot, &cmd);
    
	up(&msdc_host_boot->msdc_sema);
    
    if (cmd.error)
    {
        printk(KERN_ERR "msdc_sendrequest fail: offset=0x%016llX, size=0x%016llX, read=%d!\n", offset, size, read);
        return -1;
    }
    
    return 0;
}


int msdc_partread(int partno, uint64_t offset, uint64_t size, void *pvmem)
{
    uint64_t u8partoffs, u8partsize;
    
    u_char *p_buffer = (u_char *)pvmem;
    int read_length, left_to_read = size;
#if 0        
    BUG_ON(msdc_check_partition());

    u8partoffs = msdc_getpart_offs(partno);
    u8partsize = msdc_getpart_size(partno);
#else
    lgemmc_get_part_offsest_and_size(partno, &u8partoffs, &u8partsize);  
    printk(KERN_INFO "msdc_partread, lgemmc_get_part partno=%d offset=0x%016llX, size=0x%016llX!\n", partno, u8partoffs, u8partsize);
#endif
    BUG_ON(offset+size > u8partsize);

    if (msdc_alloc_reqpages())
    {
        goto read_fail;
    }

    offset += u8partoffs;

    // read non-block alignment offset.
    if (offset % 512)
    {
        int block_offset;
        
        block_offset = offset % 512;
        offset -= block_offset;
        
		if (left_to_read < (512 - block_offset))
		{
			read_length = left_to_read;
		}
		else
		{
			read_length = 512 - block_offset;
		}

        if (msdc_sendrequest(offset, 512, _blkpages, 1))
        {
            goto read_fail;
        }
        
        memcpy(p_buffer, _pu1blkbuf + block_offset, read_length);

        offset += 512;
        left_to_read -= read_length;
        p_buffer += read_length;
    }

    // read block alignment offset and size. (MSDC_MAXBUF_CNT)
    while (left_to_read >= MSDC_MAXBUF_CNT)
    {
        read_length = MSDC_MAXBUF_CNT;
        
        if (msdc_sendrequest(offset, read_length, _blkpages, 1))
        {
            goto read_fail;
        }
        
        memcpy(p_buffer, _pu1blkbuf, read_length);
        
        offset += read_length;
        left_to_read -= read_length;
        p_buffer += read_length;
    }
    
    // read block alignment offset and size. (X512B)
    if (left_to_read >= 512)
    {
        read_length = (left_to_read>>9)<<9;
        
        if (msdc_sendrequest(offset, read_length, _blkpages, 1))
        {
            goto read_fail;
        }
        
        memcpy(p_buffer, _pu1blkbuf, read_length);
        
        offset += read_length;
        left_to_read -= read_length;
        p_buffer += read_length;
    }

    // read non-block alignment size.
    if (left_to_read)
    {
        read_length = left_to_read;
        
        if (msdc_sendrequest(offset, 512, _blkpages, 1))
        {
            goto read_fail;
        }
        
        memcpy(p_buffer, _pu1blkbuf, read_length);
    }
    msdc_free_reqpages();    
    return 0;

read_fail:
    printk(KERN_ERR "msdc_partread fail: partno=%d, offset=0x%llX, size=0x%llX, pvmem=0x%08X\n", 
        partno, offset, size, (int)pvmem);
    msdc_free_reqpages();
    return -1;
}
int msdc_partwrite(int partno, uint64_t offset, uint64_t size, void *pvmem)
{
    printk(KERN_ERR "msdc_partwrite is not implemented\n");
    return 0;
}

EXPORT_SYMBOL(msdc_host_boot);
EXPORT_SYMBOL(msdc_send_command);
EXPORT_SYMBOL(msdc_partread);
EXPORT_SYMBOL(msdc_partwrite);

