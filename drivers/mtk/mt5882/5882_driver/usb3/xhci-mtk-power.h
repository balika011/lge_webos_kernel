#ifndef _XHCI_MTK_POWER_H
#define _XHCI_MTK_POWER_H

//#include <linux/usb.h>
#include "xhci.h"

#define MTK_HOST_NEW_POWER_SCHEME	1
#define MTK_HOST_CLOCK_GATING		(1 & MTK_HOST_NEW_POWER_SCHEME)

#if MTK_HOST_NEW_POWER_SCHEME
//#define NUM_U3_PORT	1
//#define NUM_U2_PORT 1

#ifdef CC_MT5396_USB3
#define SIFSLV_IPPC 		0xf0042000
#define SSUSB_IP_PW_CTRL	(SIFSLV_IPPC+0x0)
#define SSUSB_IP_SW_RST		1U<<0
#define SSUSB_IP_PW_CTRL_1	(SIFSLV_IPPC+0x4)
#define SSUSB_IP_PDN		1U<<1
#define SSUSB_U3_CTRL(p)	(SIFSLV_IPPC+0x100+(p*0x10))
#define SSUSB_U3_PORT_DIS	1U<<0
#define SSUSB_U3_PORT_PDN	1U<<1
#define SSUSB_U3_PORT_HOST_SEL	1U<<2
#define SSUSB_U3_PORT_CKBG_EN	1U<<3
#define SSUSB_U3_PORT_MAC_RST	1U<<4
#define SSUSB_U3_PORT_PHYD_RST	1U<<5
#define SSUSB_U2_CTRL(p)	(SIFSLV_IPPC+(0x180)+(p*0x10))
#define SSUSB_U2_PORT_DIS	1U<<0
#define SSUSB_U2_PORT_PDN	1U<<1
#define SSUSB_U2_PORT_HOST_SEL	1U<<2
#define SSUSB_U2_PORT_CKBG_EN	1U<<3
#define SSUSB_U2_PORT_MAC_RST	1U<<4
#define SSUSB_U2_PORT_PHYD_RST	1U<<5

#define SSUSB_IP_CAP			(SIFSLV_IPPC+0x018)
#define SSUSB_U3_PORT_NUM(p)	(p & 0xff)
#define SSUSB_U2_PORT_NUM(p)	((p>>8) & 0xff)
#else
#define SIFSLV_IPPC 		0xf0070700
#define SSUSB_IP_PW_CTRL	(SIFSLV_IPPC+0x0)
#define SSUSB_IP_SW_RST		1U<<0
#define SSUSB_IP_PW_CTRL_1	(SIFSLV_IPPC+0x4)
#define SSUSB_IP_PDN		1U<<1
#define SSUSB_U3_CTRL(p)	(SIFSLV_IPPC+0x30+(p*0x8))
#define SSUSB_U3_PORT_DIS	1U<<0
#define SSUSB_U3_PORT_PDN	1U<<1
#define SSUSB_U3_PORT_HOST_SEL	1U<<2
#define SSUSB_U3_PORT_CKBG_EN	1U<<3
#define SSUSB_U3_PORT_MAC_RST	1U<<4
#define SSUSB_U3_PORT_PHYD_RST	1U<<5
#define SSUSB_U2_CTRL(p)	(SIFSLV_IPPC+(0x50)+(p*0x8))
#define SSUSB_U2_PORT_DIS	1U<<0
#define SSUSB_U2_PORT_PDN	1U<<1
#define SSUSB_U2_PORT_HOST_SEL	1U<<2
#define SSUSB_U2_PORT_CKBG_EN	1U<<3
#define SSUSB_U2_PORT_MAC_RST	1U<<4
#define SSUSB_U2_PORT_PHYD_RST	1U<<5

#define SSUSB_IP_CAP			(SIFSLV_IPPC+0x024)
#define SSUSB_U3_PORT_NUM(p)	(p & 0xff)
#define SSUSB_U2_PORT_NUM(p)	((p>>8) & 0xff)
#endif // CC_MT5396_USB3

#define U3_PHY_I2C_DEV	0xc0
#define U3_PHY_PG_U2PHY     0x00
#define U3_PHY_PG_PHYD      0x10
#define U3_PHY_PG_PHYD2      0x20
#define U3_PHY_PG_PHYA        0x30
#define U3_PHY_PG_PHYA_DA     0x40
#define U3_PHY_PG_CHIP         0x50
#define U3_PHY_PG_FM           0xF0
#define U3_PHY_PAGE				0xff
#define U3_PHY_PG_SSUSB_PHYD	0x10
#define U3_PHY_PG_SSUSB_PHYA	0x30
#define U3_PHY_PG_A60802A	0x30
#define U3_PHY_I2C_PCLK_DRV_REG       0x0A
#define U3_PHY_I2C_PCLK_PHASE_REG       0x0B

// USB3 IP define
//#define IPRESET_ADDR 0xf0008098
//#define IPRESET_BIT 1;
#ifdef CC_MT5396_USB3
#define U3_PIPE_LATCH_SEL_ADD 0xf0042530
#else
#define U3_PIPE_LATCH_SEL_ADD 0xf0076530
#endif

#define U3_PIPE_LATCH_TX      0
#define U3_PIPE_LATCH_RX      0

void enableXhciAllPortPower(struct xhci_hcd *xhci);
void enableAllClockPower(void);
void disablePortClockPower(void);
void enablePortClockPower(int port_index, int port_rev);

#endif
#endif

