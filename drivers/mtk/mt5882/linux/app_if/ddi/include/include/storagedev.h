#ifndef	_STORAGE_DEV_H_
#define	_STORAGE_DEV_H_

#define MTD_DEVICE_NAME		"/dev/mtd"
#define MTD_BLKDEV_NAME		"/dev/mtdblock"
#define MTD_CHARDEV_NAME		"/dev/mtd"

#define MMC_DEVICE_NAME		"/dev/mmcblk0p"
#define MMC_BLKDEV_NAME		"/dev/mmcblk0p"
#define MMC_CHARDEV_NAME		"/dev/mmcblk0p"

#ifdef	USE_EMMC
#define	INTL_STO_BLKDEV_NAME	MMC_BLKDEV_NAME			/* Internal storage block device name */
#define	INTL_STO_CHARDEV_NAME	MMC_BLKDEV_NAME		/* Internal storage char device name */
#else
#define	INTL_STO_BLKDEV_NAME	MTD_BLKDEV_NAME
#define	INTL_STO_CHARDEV_NAME	MTD_CHARDEV_NAME
#endif

#endif	/* _STORAGE_DEV_H_ */
