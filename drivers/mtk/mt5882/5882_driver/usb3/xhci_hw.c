/*
 * xHCI host controller driver
 *
 * Copyright (C) 2008 Intel Corp.
 *
 * Author: Sarah Sharp
 * Some code borrowed from the Linux EHCI driver.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * Ring initialization rules:
 * 1. Each segment is initialized to zero, except for link TRBs.
 * 2. Ring cycle state = 0.  This represents Producer Cycle State (PCS) or
 *    Consumer Cycle State (CCS), depending on ring function.
 * 3. Enqueue pointer = dequeue pointer = address of first TRB in the segment.
 *
 * Ring behavior rules:
 * 1. A ring is empty if enqueue == dequeue.  This means there will always be at
 *    least one free TRB in the ring.  This is useful if you want to turn that
 *    into a link TRB and expand the ring.
 * 2. When incrementing an enqueue or dequeue pointer, if the next TRB is a
 *    link TRB, then load the pointer with the address in the link TRB.  If the
 *    link TRB had its toggle bit set, you may need to update the ring cycle
 *    state (see cycle bit rules).  You may have to do this multiple times
 *    until you reach a non-link TRB.
 * 3. A ring is full if enqueue++ (for the definition of increment above)
 *    equals the dequeue pointer.
 *
 * Cycle bit rules:
 * 1. When a consumer increments a dequeue pointer and encounters a toggle bit
 *    in a link TRB, it must toggle the ring cycle state.
 * 2. When a producer increments an enqueue pointer and encounters a toggle bit
 *    in a link TRB, it must toggle the ring cycle state.
 *
 * Producer rules:
 * 1. Check if ring is full before you enqueue.
 * 2. Write the ring cycle state to the cycle bit in the TRB you're enqueuing.
 *    Update enqueue pointer between each write (which may update the ring
 *    cycle state).
 * 3. Notify consumer.  If SW is producer, it rings the doorbell for command
 *    and endpoint rings.  If HC is the producer for the event ring,
 *    and it generates an interrupt according to interrupt modulation rules.
 *
 * Consumer rules:
 * 1. Check if TRB belongs to you.  If the cycle bit == your ring cycle state,
 *    the TRB is owned by the consumer.
 * 2. Update dequeue pointer (which may update the ring cycle state) and
 *    continue processing TRBs until you reach a TRB which is not owned by you.
 * 3. Notify the producer.  SW is the consumer for the event ring, and it
 *   updates event ring dequeue pointer.  HC is the consumer for the command and
 *   endpoint rings; it generates events on the event ring for these.
 */

#include "x_hal_5381.h"
#include "x_hal_arm.h"
#include "x_os.h"
#include "x_assert.h"
#include "x_timer.h"
#include "x_printf.h"
#include "x_ckgen.h"

#include "sif_if.h"

#include "xhci-mtk-power.h"
#include "xhci.h"

struct xhci_hcd _mtk_xhci;

static struct xhci_port rh_port[RH_PORT_NUM];
static int g_device_reconnect;

static u32 _u4TrEventField2;

static HANDLE_T _hXhci_Sema = NULL;
static CRIT_STATE_T _rxhciLock;
#define xhci_lock() do { _rxhciLock = x_crit_start(); } while (0)
#define xhci_unlock() do {  x_crit_end(_rxhciLock); } while (0)

static void xhci_free_container_ctx(struct xhci_hcd *xhci, struct xhci_container_ctx *ctx);
int xhci_queue_slot_control(struct xhci_hcd *xhci, u32 trb_type, u32 slot_id);
int xhci_queue_address_device(struct xhci_hcd *xhci, dma_addr_t in_ctx_ptr, u32 slot_id);
int xhci_queue_configure_endpoint(struct xhci_hcd *xhci, dma_addr_t in_ctx_ptr,
        u32 slot_id, bool command_must_succeed);
struct xhci_ep_ctx *xhci_get_ep_ctx(struct xhci_hcd *xhci,
                    struct xhci_container_ctx *ctx, unsigned int ep_index);

/**
 * fls - find last (most-significant) bit set
 * @x: the word to search
 *
 * This is defined the same way as ffs.
 * Note fls(0) = 0, fls(1) = 1, fls(0x80000000) = 32.
 */
static int fls(int x)
{
    int r = 32;

    if (!x)
            return 0;
    if (!(x & 0xffff0000u)) {
            x <<= 16;
            r -= 16;
    }
    if (!(x & 0xff000000u)) {
            x <<= 8;
            r -= 8;
    }
    if (!(x & 0xf0000000u)) {
            x <<= 4;
            r -= 4;
    }
    if (!(x & 0xc0000000u)) {
            x <<= 2;
            r -= 2;
    }
    if (!(x & 0x80000000u)) {
            x <<= 1;
            r -= 1;
    }
    return r;
}

static u32* xhci_dma_alloc(u32 size)
{
    UINT32 u4Addr;

    u4Addr = BSP_AllocAlignedDmaMemory(size, 64);
    ASSERT(u4Addr != 0);
    u4Addr |= 0x80000000; // Set to non-cache area
    
    return (u32*)u4Addr;
}

static void xhci_dma_free(void* ptr)
{
    UINT32 u4Addr;
    BOOL fgRet;

    UNUSED(fgRet);

    u4Addr = (UINT32)ptr;
    ASSERT((u4Addr & 0x80000000) != 0);
    u4Addr &= ~0xC0000000;

    fgRet = BSP_FreeAlignedDmaMemory(u4Addr);
    ASSERT(fgRet);    
}

static void rmb(void)
{
}

static void wmb(void)
{
}


static int handshake(struct xhci_hcd *xhci, void __iomem *ptr,
              u32 mask, u32 done, int usec)
{
    u32 result;

    do {
        result = xhci_readl(xhci, ptr);
        if (result == ~(u32)0)      /* card removed */
            return -1;
        result &= mask;
        if (result == done)
            return 0;
        HAL_Delay_us(1);
        usec--;
    } while (usec > 0);
    return -1;
}

u32 xhci_port_state_to_neutral(u32 state)
{
    /* Save read-only status and port state */
    return (state & XHCI_PORT_RO) | (state & XHCI_PORT_RWS);
}

void enableXhciAllPortPower(struct xhci_hcd *xhci){
    int i, num_u3_port, num_u2_port;
    u32 port_id, temp;
    u32 __iomem *addr;

    num_u3_port = SSUSB_U3_PORT_NUM(readl(SSUSB_IP_CAP));
    num_u2_port = SSUSB_U2_PORT_NUM(readl(SSUSB_IP_CAP));
    
    for(i=1; i<=num_u3_port; i++){
        port_id=i;
        addr = &xhci->op_regs->port_status_base + NUM_PORT_REGS*(port_id-1 & 0xff);
        temp = readl(addr);
        temp = xhci_port_state_to_neutral(temp);
        temp |= PORT_POWER;
        writel(temp, addr);
    }
    for(i=1; i<=num_u2_port; i++){
        port_id=i+num_u3_port;
        addr = &xhci->op_regs->port_status_base + NUM_PORT_REGS*(port_id-1 & 0xff);
        temp = readl(addr);
        temp = xhci_port_state_to_neutral(temp);
        temp |= PORT_POWER;
        writel(temp, addr);
    }
}

void enableAllClockPower(void){
    int i, num_u3_port, num_u2_port;
    u32 temp;
    
    num_u3_port = SSUSB_U3_PORT_NUM(readl(SSUSB_IP_CAP));
    num_u2_port = SSUSB_U2_PORT_NUM(readl(SSUSB_IP_CAP));

    //  Enable xHC
    writel(readl(SSUSB_IP_PW_CTRL) | (SSUSB_IP_SW_RST), SSUSB_IP_PW_CTRL);
    writel(readl(SSUSB_IP_PW_CTRL) & (~SSUSB_IP_SW_RST), SSUSB_IP_PW_CTRL);
    writel(readl(SSUSB_IP_PW_CTRL_1) & (~SSUSB_IP_PDN), SSUSB_IP_PW_CTRL_1);
    
    //1.    Enable target ports 
    for(i=0; i<num_u3_port; i++){
        temp = readl(SSUSB_U3_CTRL(i));
        temp = temp & (~SSUSB_U3_PORT_PDN) & (~SSUSB_U3_PORT_DIS);
        writel(temp, SSUSB_U3_CTRL(i));
    }
    for(i=0; i<num_u2_port; i++){
        temp = readl(SSUSB_U2_CTRL(i));
        temp = temp & (~SSUSB_U2_PORT_PDN) & (~SSUSB_U2_PORT_DIS);
        writel(temp, SSUSB_U2_CTRL(i));
    }
    x_thread_delay(100);
#if 0
    //3.    Enable port power of target ports
    temp = readl(0xf0040420);
    temp = xhci_port_state_to_neutral(temp);
    temp |= PORT_POWER;
    writel(temp, 0xf0040420);
#endif
}

//(X)disable clock/power of a port 
//(X)if all ports are disabled, disable IP ctrl power
//disable all ports and IP clock/power, this is just mention HW that the power/clock of port 
//and IP could be disable if suspended.
//If doesn't not disable all ports at first, the IP clock/power will never be disabled
//(some U2 and U3 ports are binded to the same connection, that is, they will never enter suspend at the same time
//port_index: port number
//port_rev: 0x2 - USB2.0, 0x3 - USB3.0 (SuperSpeed)
void disablePortClockPower(void){
    int i, num_u3_port, num_u2_port;
    u32 temp;

    num_u3_port = SSUSB_U3_PORT_NUM(readl(SSUSB_IP_CAP));
    num_u2_port = SSUSB_U2_PORT_NUM(readl(SSUSB_IP_CAP));
    
    for(i=0; i<num_u3_port; i++){
        temp = readl(SSUSB_U3_CTRL(i));
        temp = temp | (SSUSB_U3_PORT_PDN);
        writel(temp, SSUSB_U3_CTRL(i));
    }
    for(i=0; i<num_u2_port; i++){
        temp = readl(SSUSB_U2_CTRL(i));
        temp = temp | (SSUSB_U2_PORT_PDN);
        writel(temp, SSUSB_U2_CTRL(i));
    }
    writel(readl(SSUSB_IP_PW_CTRL_1) | (SSUSB_IP_PDN), SSUSB_IP_PW_CTRL_1);
}

//if IP ctrl power is disabled, enable it
//enable clock/power of a port
//port_index: port number
//port_rev: 0x2 - USB2.0, 0x3 - USB3.0 (SuperSpeed)
void enablePortClockPower(int port_index, int port_rev){
    u32 temp;
    
    writel(readl(SSUSB_IP_PW_CTRL_1) & (~SSUSB_IP_PDN), SSUSB_IP_PW_CTRL_1);

    if(port_rev == 0x3){
        temp = readl(SSUSB_U3_CTRL(port_index));
        temp = temp & (~SSUSB_U3_PORT_PDN);
        writel(temp, SSUSB_U3_CTRL(port_index));
    }
    else if(port_rev == 0x2){
        temp = readl(SSUSB_U2_CTRL(port_index));
        temp = temp & (~SSUSB_U2_PORT_PDN);
        writel(temp, SSUSB_U2_CTRL(port_index));
    }
}

void setLatchSel(void){
    __u32 __iomem *latch_sel_addr;
    u32 latch_sel_value;
    latch_sel_addr = (__u32*)U3_PIPE_LATCH_SEL_ADD;
    latch_sel_value = ((U3_PIPE_LATCH_TX)<<2) | (U3_PIPE_LATCH_RX);
    writel(latch_sel_value, latch_sel_addr);
}

#ifndef CC_FPGA
#define U3_PHY_PG_U2PHY     0x00
#define U3_PHY_PG_PHYD      0x10
#define U3_PHY_PG_PHYD2     0x20
#define U3_PHY_PG_PHYA      0x30
#define U3_PHY_PG_PHYA_DA   0x40
//#define U3_PHY_PG_CHIP         0x50
#define U3_PHY_PG_FM        0xF0

static int u4PhyBankOffset = 0xF0070000;
#endif

static void _U3_Write_Bank(UINT8 bankValue)
{
#ifdef CC_FPGA
    UINT8 au1Buf[1];

    au1Buf[0] = bankValue;
    if(SIF_Write(0x100, U3_PHY_I2C_DEV, U3_PHY_PAGE, au1Buf, 1) != 1)
    {
        ASSERT(0);
    }
#else
        switch (bankValue) {
        case U3_PHY_PG_U2PHY:
            u4PhyBankOffset = 0xF0070800;
            break;
        case U3_PHY_PG_PHYD:
            u4PhyBankOffset = 0xF0070900;
            break;
        case U3_PHY_PG_PHYD2:
            u4PhyBankOffset = 0xF0070A00;
            break;
        case U3_PHY_PG_PHYA:
            u4PhyBankOffset = 0xF0070B00;
            break;
        case U3_PHY_PG_PHYA_DA:
            u4PhyBankOffset = 0xF0070C00;
            break;
        case U3_PHY_PG_FM:
            u4PhyBankOffset = 0xF0070100;
            break;
        default:
            break;
    }
#endif
}

static int _U3Write_Reg(UINT32 address, UINT8 value)
{   
#ifdef CC_FPGA
    UINT8 au1Buf[1];

    au1Buf[0] = value;
    if(SIF_Write(0x100, U3_PHY_I2C_DEV, address, au1Buf, 1) != 1)
    {
        ASSERT(0);
    }
    Printf("Write I2C reg[0x%x] value[0x%x]\n", address, value);
#else
    int add_buf,u4Value;
    add_buf = address & 0xFC;
	u4Value = readl(add_buf+u4PhyBankOffset);

	switch (address % 4) {
		case 0:
			u4Value = (u4Value & 0xFFFFFF00) | ((int)value);
			break;
		case 1:
			u4Value = (u4Value & 0xFFFF00FF) | (((int)value)<<8);
			break;
		case 2:
			u4Value = (u4Value & 0xFF00FFFF) | (((int)value)<<16);
			break;
		default:
			u4Value = (u4Value & 0x00FFFFFF) | (((int)value)<<24);
			break;
	}

    writel(u4Value, add_buf+u4PhyBankOffset);
	printf("Write U3 reg[0x%x] value[0x%x]\n", address, value);
#endif

    return 0;
}

static UINT8 _U3Read_Reg(int address)
{
#ifdef CC_FPGA
    UINT8 au1Buf[1];
    
    if(SIF_Read(0x100, U3_PHY_I2C_DEV, address, au1Buf, 1) == 0)
    {
        return 0;
    }
    return au1Buf[0];
#else
    int add_buf,reval;
    UINT8 ret;
    add_buf = address & 0xFC;
	reval = readl(add_buf+u4PhyBankOffset);
    
	switch (address % 4) {
		case 0:
			ret = (UINT8)(reval & 0xFF);
			break;
		case 1:
			ret = (UINT8)((reval>>8) & 0xFF);
			break;
		case 2:
			ret = (UINT8)((reval>>16) & 0xFF);
			break;
		default:
			ret = (UINT8)((reval>>24) & 0xFF);
			break;
	}
    return ret;
#endif
}

static void dbg_u3init(void)
{
    char temp;
    
    _U3_Write_Bank(U3_PHY_PG_U2PHY);
//    x_thread_delay(100);
    temp = _U3Read_Reg(0x00); //RG_USB20_BGR_EN = 1
    temp |= 0x1;
    _U3Write_Reg(0x00, temp);
//    x_thread_delay(100);        
    temp = _U3Read_Reg(0x01); //RG_USB20_REF_EN = 1
    temp |= 0x80;
    _U3Write_Reg(0x01, temp);
//    x_thread_delay(100);        
    temp = _U3Read_Reg(0x66); //RG_USB20_SW_PLLMODE = 2
    temp &= ~(0x3<<2);
    temp |= (0x2<<2);
    _U3Write_Reg(0x66, temp);
//    x_thread_delay(100);        

    _U3_Write_Bank(U3_PHY_PG_PHYD);
//    x_thread_delay(100);
    temp = _U3Read_Reg(0x08);
//    x_thread_delay(100);
    temp &= ~0x1F;
    temp |= 0x19;
    _U3Write_Reg(0x08, temp);
//    x_thread_delay(100);
    //   _U3Write_Reg(0x06, 0x00);  //****** turn off phy clock gating *******//
    //   mdelay(100);
    temp = _U3Read_Reg(0x09);
//    x_thread_delay(100);
    temp |= 0x80;
    _U3Write_Reg(0x09, temp);
//    x_thread_delay(100);
    temp = _U3Read_Reg(0x60); //pllband_recal
//    x_thread_delay(100);
    temp |= 0x10;
    _U3Write_Reg(0x60, temp);
//    x_thread_delay(100);

    _U3_Write_Bank(U3_PHY_PG_PHYD2);
//    x_thread_delay(100);
    temp = _U3Read_Reg(0x02);
//    x_thread_delay(100);
    temp &= ~0x03;
    temp |= 0x01;
    _U3Write_Reg(0x02, temp);
//    x_thread_delay(100);
        
    _U3_Write_Bank(U3_PHY_PG_PHYA);
//    x_thread_delay(100);
    temp = _U3Read_Reg(0x1d);
//    x_thread_delay(100);
    temp &= ~0x40;
    _U3Write_Reg(0x1d, temp);
//    x_thread_delay(100);

    _U3_Write_Bank(U3_PHY_PG_PHYA_DA);
//    x_thread_delay(100);
    _U3Write_Reg(0x42, 0x50);   // set SSC to pass electrical compliance SSC min
//    x_thread_delay(100);

#ifdef CC_FPGA //SOC don't need it
    _U3_Write_Bank(U3_PHY_PG_CHIP);
    x_thread_delay(100);
    temp = _U3Read_Reg(0x07);
    x_thread_delay(100);
    temp |= 0x01;
    _U3Write_Reg(0x07, temp);
    x_thread_delay(100);
    _U3Write_Reg(0x0A, 0x10);   // PIPE drv = 2
    x_thread_delay(100);
    //_U3Write_Reg(0x0B, 0x8c);
    _U3Write_Reg(0x0B, 0x3C);
    x_thread_delay(100);
#endif
}

void reinitIP(void)
{
#ifdef CC_FPGA
    SIF_Init();
#endif
    dbg_u3init();
/*    
    __u32 __iomem *ip_reset_addr;
    u32 ip_reset_value;
    //reset USB IP
    ip_reset_addr = (__u32*)IPRESET_ADDR;
    ip_reset_value = readl(ip_reset_addr);
    ip_reset_value = ip_reset_value | IPRESET_BIT;
    writel(ip_reset_value, ip_reset_addr);
    ip_reset_value = ip_reset_value & ~IPRESET_BIT;
    writel(ip_reset_value, ip_reset_addr);
*/
#if MTK_HOST_NEW_POWER_SCHEME
    enableAllClockPower();
#endif
    setLatchSel();

/*
#if PERF_PROBE
    mtk_probe_init(0x71717171);
    mtk_probe_out(0x0);
#endif
#if WEB_CAM_PROBE
    mtk_probe_init(0x70707070);
    mtk_probe_out(0x0);
#endif
*/
}

dma_addr_t xhci_trb_virt_to_dma(struct xhci_segment *seg,
        union xhci_trb *trb)
{
    unsigned long segment_offset;

    if (!seg || !trb || trb < seg->trbs)
        return 0;
    /* offset in TRBs */
    segment_offset = trb - seg->trbs;
    if (segment_offset > TRBS_PER_SEGMENT)
        return 0;
    return seg->dma + (segment_offset * sizeof(*trb));
}

/* Is this TRB a link TRB or was the last TRB the last TRB in this event ring
 * segment?  I.e. would the updated event TRB pointer step off the end of the
 * event seg?
 */
static int last_trb(struct xhci_hcd *xhci, struct xhci_ring *ring,
        struct xhci_segment *seg, union xhci_trb *trb)
{
    if (ring == xhci->event_ring)
        return trb == &seg->trbs[TRBS_PER_SEGMENT];
    else
        return (le32_to_cpu(trb->link.control) & TRB_TYPE_BITMASK)
            == TRB_TYPE(TRB_LINK);
}


static int enqueue_is_link_trb(struct xhci_ring *ring)
{
    struct xhci_link_trb *link = &ring->enqueue->link;
    return ((le32_to_cpu(link->control) & TRB_TYPE_BITMASK) ==
        TRB_TYPE(TRB_LINK));
}


/* Does this link TRB point to the first segment in a ring,
 * or was the previous TRB the last TRB on the last segment in the ERST?
 */
static bool last_trb_on_last_seg(struct xhci_hcd *xhci, struct xhci_ring *ring,
        struct xhci_segment *seg, union xhci_trb *trb)
{
    if (ring == xhci->event_ring)
        return (bool)((trb == &seg->trbs[TRBS_PER_SEGMENT]) &&
            (seg->next == xhci->event_ring->first_seg));
    else
        return (bool)(le32_to_cpu(trb->link.control) & LINK_TOGGLE);
}


/*
 * See Cycle bit rules. SW is the consumer for the event ring only.
 * Don't make a ring full of link TRBs.  That would be dumb and this would loop.
 *
 * If we've just enqueued a TRB that is in the middle of a TD (meaning the
 * chain bit is set), then set the chain bit in all the following link TRBs.
 * If we've enqueued the last TRB in a TD, make sure the following link TRBs
 * have their chain bit cleared (so that each Link TRB is a separate TD).
 *
 * Section 6.4.4.1 of the 0.95 spec says link TRBs cannot have the chain bit
 * set, but other sections talk about dealing with the chain bit set.  This was
 * fixed in the 0.96 specification errata, but we have to assume that all 0.95
 * xHCI hardware can't handle the chain bit being cleared on a link TRB.
 *
 * @more_trbs_coming:   Will you enqueue more TRBs before calling
 *          prepare_transfer()?
 */
static void inc_enq(struct xhci_hcd *xhci, struct xhci_ring *ring,
        bool consumer, bool more_trbs_coming, bool isoc)
{
    u32 chain;
    union xhci_trb *next;

    chain = le32_to_cpu(ring->enqueue->generic.field[3]) & TRB_CHAIN;
    next = ++(ring->enqueue);

    ring->enq_updates++;
    /* Update the dequeue pointer further if that was a link TRB or we're at
     * the end of an event ring segment (which doesn't have link TRBS)
     */
    while (last_trb(xhci, ring, ring->enq_seg, next)) {
        if (!consumer) {
            if (ring != xhci->event_ring) {
                /*
                 * If the caller doesn't plan on enqueueing more
                 * TDs before ringing the doorbell, then we
                 * don't want to give the link TRB to the
                 * hardware just yet.  We'll give the link TRB
                 * back in prepare_ring() just before we enqueue
                 * the TD at the top of the ring.
                 */
                if (!chain && !more_trbs_coming)
                    break;
#if 1
                /* If we're not dealing with 0.95 hardware or
                 * isoc rings on AMD 0.96 host,
                 * carry over the chain bit of the previous TRB
                 * (which may mean the chain bit is cleared).
                 */
                if (/*!(isoc && (xhci->quirks & XHCI_AMD_0x96_HOST))
                        && */!xhci_link_trb_quirk(xhci)) {
                    next->link.control &= cpu_to_le32(~TRB_CHAIN);
                    next->link.control |= cpu_to_le32(chain);
                }
                /* Give this link TRB to the hardware */
                wmb();
                next->link.control ^= cpu_to_le32(TRB_CYCLE);
#endif
            }
            /* Toggle the cycle bit after the last ring segment. */
            if (last_trb_on_last_seg(xhci, ring, ring->enq_seg, next)) 
            {
                ring->cycle_state = (ring->cycle_state ? 0 : 1);
            }
        }
        ring->enq_seg = ring->enq_seg->next;
        ring->enqueue = ring->enq_seg->trbs;
        next = ring->enqueue;
    }
}


static void inc_deq(struct xhci_hcd *xhci, struct xhci_ring *ring, bool consumer)
{
    union xhci_trb *next = ++(ring->dequeue);

    ring->deq_updates++;
    /* Update the dequeue pointer further if that was a link TRB or we're at
     * the end of an event ring segment (which doesn't have link TRBS)
     */
    while (last_trb(xhci, ring, ring->deq_seg, next)) {
        if (consumer && last_trb_on_last_seg(xhci, ring, ring->deq_seg, next)) {
            ring->cycle_state = (ring->cycle_state ? 0 : 1);
        }
        ring->deq_seg = ring->deq_seg->next;
        ring->dequeue = ring->deq_seg->trbs;
        next = ring->dequeue;
    }
}


static void queue_trb(struct xhci_hcd *xhci, struct xhci_ring *ring,
        bool consumer, bool more_trbs_coming, bool isoc,
        u32 field1, u32 field2, u32 field3, u32 field4)
{
    struct xhci_generic_trb *trb;

    trb = &ring->enqueue->generic;
    trb->field[0] = cpu_to_le32(field1);
    trb->field[1] = cpu_to_le32(field2);
    trb->field[2] = cpu_to_le32(field3);
    trb->field[3] = cpu_to_le32(field4);
    inc_enq(xhci, ring, consumer, more_trbs_coming, isoc);
}


static struct xhci_segment *xhci_segment_alloc(struct xhci_hcd *xhci)
{
    struct xhci_segment *seg;
    dma_addr_t  dma;

    seg = x_mem_alloc(sizeof *seg);
    x_memset((void*)seg, 0, (sizeof *seg));
    if (!seg)
        return NULL;
    xhci_dbg(xhci, "Allocating priv segment structure at %p\n", seg);

    //seg->trbs = dma_pool_alloc(xhci->segment_pool, &dma);
    seg->trbs = (union xhci_trb *)xhci_dma_alloc(SEGMENT_SIZE);
    dma = ((dma_addr_t)seg->trbs) & 0x7FFFFFFF;
    if (!seg->trbs) {
        ASSERT(0);
        //kfree(seg);
        //return NULL;
    }
    xhci_dbg(xhci, "// Allocating segment at %p (virtual) 0x%llx (DMA)\n",
            seg->trbs, (unsigned long long)dma);

    x_memset(seg->trbs, 0, SEGMENT_SIZE);
    seg->dma = dma;
    seg->next = NULL;

    return seg;
}

static void xhci_segment_free(struct xhci_hcd *xhci, struct xhci_segment *seg)
{
    if (!seg)
        return;
    if (seg->trbs) {
        xhci_dbg(xhci, "Freeing DMA segment at %p (virtual) 0x%llx (DMA)\n",
                seg->trbs, (unsigned long long)seg->dma);
        xhci_dma_free(seg->trbs);
        seg->trbs = NULL;
    }
    xhci_dbg(xhci, "Freeing priv segment structure at %p\n", seg);
    x_mem_free(seg);
}

void xhci_ring_free(struct xhci_hcd *xhci, struct xhci_ring *ring)
{
    struct xhci_segment *seg;
    struct xhci_segment *first_seg;

    if (!ring)
        return;
    if (ring->first_seg) {
        first_seg = ring->first_seg;
        seg = first_seg->next;
        xhci_dbg(xhci, "Freeing ring at %p\n", ring);
        while (seg != first_seg) {
            struct xhci_segment *next = seg->next;
            xhci_segment_free(xhci, seg);
            seg = next;
        }
        xhci_segment_free(xhci, first_seg);
        ring->first_seg = NULL;
    }
    x_mem_free(ring);
}


static void xhci_link_segments(struct xhci_hcd *xhci, struct xhci_segment *prev,
        struct xhci_segment *next, bool link_trbs, bool isoc)
{
    u32 val;

    if (!prev || !next)
        return;
    prev->next = next;
    if (link_trbs) {
        prev->trbs[TRBS_PER_SEGMENT-1].link.
            segment_ptr = cpu_to_le64(next->dma);

        /* Set the last TRB in the segment to have a TRB type ID of Link TRB */
        val = le32_to_cpu(prev->trbs[TRBS_PER_SEGMENT-1].link.control);
        val &= ~TRB_TYPE_BITMASK;
        val |= TRB_TYPE(TRB_LINK);
        /* Always set the chain bit with 0.95 hardware */
        /* Set chain bit for isoc rings on AMD 0.96 host */
        prev->trbs[TRBS_PER_SEGMENT-1].link.control = cpu_to_le32(val);
    }
    xhci_dbg(xhci, "Linking segment 0x%llx to segment 0x%llx (DMA)\n",
            (unsigned long long)prev->dma,
            (unsigned long long)next->dma);
}

static void xhci_initialize_ring_info(struct xhci_ring *ring)
{
    /* The ring is empty, so the enqueue pointer == dequeue pointer */
    ring->enqueue = ring->first_seg->trbs;
    ring->enq_seg = ring->first_seg;
    ring->dequeue = ring->enqueue;
    ring->deq_seg = ring->first_seg;
    /* The ring is initialized to 0. The producer must write 1 to the cycle
     * bit to handover ownership of the TRB, so PCS = 1.  The consumer must
     * compare CCS to the cycle bit to check ownership, so CCS = 1.
     */
    ring->cycle_state = 1;
    /* Not necessary for new rings, but needed for re-initialized rings */
    ring->enq_updates = 0;
    ring->deq_updates = 0;
}

static struct xhci_ring *xhci_ring_alloc(struct xhci_hcd *xhci,
        unsigned int num_segs, bool link_trbs, bool isoc)
{
    struct xhci_ring    *ring;
    struct xhci_segment *prev;

    ring = x_mem_alloc(sizeof *(ring));
    x_memset((void*)ring, 0, (sizeof *(ring)));
    xhci_dbg(xhci, "Allocating ring at %p\n", ring);
    if (!ring)
        return NULL;

    INIT_LIST_HEAD(&ring->td_list);
    if (num_segs == 0)
        return ring;

    ring->first_seg = xhci_segment_alloc(xhci);
    if (!ring->first_seg)
        goto fail;
    num_segs--;

    prev = ring->first_seg;
    while (num_segs > 0) {
        struct xhci_segment *next;

        next = xhci_segment_alloc(xhci);
        if (!next)
            goto fail;
        xhci_link_segments(xhci, prev, next, link_trbs, isoc);

        prev = next;
        num_segs--;
    }
    xhci_link_segments(xhci, prev, ring->first_seg, link_trbs, isoc);

    if (link_trbs) {
        /* See section 4.9.2.1 and 6.4.4.1 */
        prev->trbs[TRBS_PER_SEGMENT-1].link.
            control |= cpu_to_le32(LINK_TOGGLE);
        xhci_dbg(xhci, "Wrote link toggle flag to"
                " segment %p (virtual), 0x%llx (DMA)\n",
                prev, (unsigned long long)prev->dma);
    }
    xhci_initialize_ring_info(ring);
    return ring;

fail:
    xhci_ring_free(xhci, ring);
    return NULL;
}


struct xhci_input_control_ctx *xhci_get_input_control_ctx(struct xhci_hcd *xhci,
                          struct xhci_container_ctx *ctx)
{
    ASSERT(ctx->type == XHCI_CTX_TYPE_INPUT);
    return (struct xhci_input_control_ctx *)ctx->bytes;
}


unsigned int xhci_last_valid_endpoint(u32 added_ctxs)
{
    return fls(added_ctxs) - 1;
}


static void xhci_set_hc_event_deq(struct xhci_hcd *xhci)
{
    u64 temp;
    dma_addr_t deq;

    deq = xhci_trb_virt_to_dma(xhci->event_ring->deq_seg,
            xhci->event_ring->dequeue);

    /* Update HC event ring dequeue pointer */
    temp = xhci_read_64(xhci, &xhci->ir_set->erst_dequeue);
    temp &= ERST_PTR_MASK;
    /* Don't clear the EHB bit (which is RW1C) because
     * there might be more events to service.
     */
    temp &= ~ERST_EHB;
    xhci_dbg(xhci, "// Write event ring dequeue pointer, "
            "preserving EHB bit\n");
    xhci_write_64(xhci, ((u64) deq & (u64) ~ERST_PTR_MASK) | temp,
            &xhci->ir_set->erst_dequeue);
}

void xhci_quiesce(struct xhci_hcd *xhci)
{
    u32 halted;
    u32 cmd;
    u32 mask;

    mask = ~(XHCI_IRQS);
    halted = xhci_readl(xhci, &xhci->op_regs->status) & STS_HALT;
    if (!halted)
        mask &= ~CMD_RUN;

    cmd = xhci_readl(xhci, &xhci->op_regs->command);
    cmd &= mask;
    xhci_writel(xhci, cmd, &xhci->op_regs->command);
}

int xhci_halt(struct xhci_hcd *xhci)
{
    int ret;

    xhci_quiesce(xhci);

    ret = handshake(xhci, &xhci->op_regs->status,
            STS_HALT, STS_HALT, XHCI_MAX_HALT_USEC);
    if (!ret)
        xhci->xhc_state |= XHCI_STATE_HALTED;
    return ret;
}

/*
 * Set the run bit and wait for the host to be running.
 */
static int xhci_start(struct xhci_hcd *xhci)
{
    u32 temp;
    int ret;

    temp = xhci_readl(xhci, &xhci->op_regs->command);
    temp |= (CMD_RUN);
    xhci_writel(xhci, temp, &xhci->op_regs->command);

    /*
     * Wait for the HCHalted Status bit to be 0 to indicate the host is
     * running.
     */
    ret = handshake(xhci, &xhci->op_regs->status, STS_HALT, 0, XHCI_MAX_HALT_USEC);
    if (ret < 0)
        xhci_err(xhci, "Host took too long to start, waited %u microseconds.\n",
                XHCI_MAX_HALT_USEC);
    if (!ret)
        xhci->xhc_state &= ~XHCI_STATE_HALTED;
    return ret;
}

/*
 * Reset a halted HC.
 *
 * This resets pipelines, timers, counters, state machines, etc.
 * Transactions will be terminated immediately, and operational registers
 * will be set to their defaults.
 */
int xhci_reset(struct xhci_hcd *xhci)
{
    u32 command;
    u32 state;
    int ret;

    state = xhci_readl(xhci, &xhci->op_regs->status);
    if ((state & STS_HALT) == 0) {
        xhci_warn(xhci, "Host controller not halted, aborting reset.\n");
        return 0;
    }

    xhci_dbg(xhci, "// Reset the HC\n");
    command = xhci_readl(xhci, &xhci->op_regs->command);
    command |= CMD_RESET;
    xhci_writel(xhci, command, &xhci->op_regs->command);

    ret = handshake(xhci, &xhci->op_regs->command, CMD_RESET, 0, 250 * 1000);
    if (ret)
        return ret;

    xhci_dbg(xhci, "Wait for controller to be ready for doorbell rings\n");
    /*
     * xHCI cannot write to any doorbells or operational registers other
     * than status until the "Controller Not Ready" flag is cleared.
     */
    return handshake(xhci, &xhci->op_regs->status, STS_CNR, 0, 250 * 1000);
}

int xhci_run(struct xhci_hcd *xhci)
{
    u32 temp;
//  u64 temp_64;
//  u32 ret;

    /* Start the xHCI host controller running only after the USB 2.0 roothub
     * is setup.
     */

    if (xhci_start(xhci)) {
        xhci_halt(xhci);
        return -1;
    }
    //xhci->shared_hcd->state = HC_STATE_RUNNING; ?

    xhci_dbg(xhci, "xhci_run\n");
#if 0
    /* unregister the legacy interrupt */
    if (hcd->irq)
        free_irq(hcd->irq, hcd);
    hcd->irq = -1;

    /* Some Fresco Logic host controllers advertise MSI, but fail to
     * generate interrupts.  Don't even try to enable MSI.
     */
    if (xhci->quirks & XHCI_BROKEN_MSI)
        goto legacy_irq;

    ret = xhci_setup_msix(xhci);
    if (ret)
        /* fall back to msi*/
        ret = xhci_setup_msi(xhci);

    if (ret) {
legacy_irq:
        /* fall back to legacy interrupt*/
        ret = request_irq(pdev->irq, &usb_hcd_irq, IRQF_SHARED,
                    hcd->irq_descr, hcd);
        if (ret) {
            xhci_err(xhci, "request interrupt %d failed\n",
                    pdev->irq);
            return ret;
        }
        hcd->irq = pdev->irq;
    }
#endif

    temp = xhci_readl(xhci, &xhci->ir_set->irq_control);
    temp &= ~ER_IRQ_INTERVAL_MASK;
    temp |= (u32) 160;
    xhci_writel(xhci, temp, &xhci->ir_set->irq_control);

    /* Set the HCD state before we enable the irqs */
    temp = xhci_readl(xhci, &xhci->op_regs->command);
    temp |= (CMD_EIE);
    xhci_writel(xhci, temp, &xhci->op_regs->command);

    temp = xhci_readl(xhci, &xhci->ir_set->irq_pending);
    xhci_writel(xhci, ER_IRQ_ENABLE(temp), &xhci->ir_set->irq_pending);

    enableXhciAllPortPower(xhci);

    return 0;
}


static void handle_cmd_completion(struct xhci_hcd *xhci,
        struct xhci_event_cmd *event)
{
    int slot_id = TRB_TO_SLOT_ID(le32_to_cpu(event->flags));
    u64 cmd_dma;
    dma_addr_t cmd_dequeue_dma;
    //struct xhci_input_control_ctx *ctrl_ctx;
    //struct xhci_virt_device *virt_dev;
    //unsigned int ep_index;
    //struct xhci_ring *ep_ring;
    //unsigned int ep_state;

    cmd_dma = le64_to_cpu(event->cmd_trb);
    cmd_dequeue_dma = xhci_trb_virt_to_dma(xhci->cmd_ring->deq_seg,
            xhci->cmd_ring->dequeue);
    /* Is the command ring deq ptr out of sync with the deq seg ptr? */
    if (cmd_dequeue_dma == 0) {
        xhci->error_bitmask |= 1 << 4;
        return;
    }
    /* Does the DMA address match our internal dequeue pointer address? */
    if (cmd_dma != (u64) cmd_dequeue_dma) {
        xhci->error_bitmask |= 1 << 5;
        return;
    }
    switch (le32_to_cpu(xhci->cmd_ring->dequeue->generic.field[3])
        & TRB_TYPE_BITMASK) {
    case TRB_TYPE(TRB_ENABLE_SLOT):
        if (GET_COMP_CODE(le32_to_cpu(event->status)) == COMP_SUCCESS)
            xhci->slot_id = slot_id;
        else
            xhci->slot_id = 0;
        UNUSED(x_sema_unlock(xhci->addr_dev));
        break;      
    case TRB_TYPE(TRB_DISABLE_SLOT):
        ASSERT(0);
#if 0       
        if (xhci->devs[slot_id]) {
            if (xhci->quirks & XHCI_EP_LIMIT_QUIRK)
                /* Delete default control endpoint resources */
                xhci_free_device_endpoint_resources(xhci,
                        xhci->devs[slot_id], true);
            xhci_free_virt_device(xhci, slot_id);
        }
        break;
#endif
    case TRB_TYPE(TRB_CONFIG_EP):
        //virt_dev = xhci->devs[slot_id];
        // eddie
        //if (handle_cmd_in_cmd_wait_list(xhci, virt_dev, event))
        //  break;
        /*
         * Configure endpoint commands can come from the USB core
         * configuration or alt setting changes, or because the HW
         * needed an extra configure endpoint command after a reset
         * endpoint command or streams were being configured.
         * If the command was for a halted endpoint, the xHCI driver
         * is not waiting on the configure endpoint command.
         */
        //ctrl_ctx = xhci_get_input_control_ctx(xhci,   virt_dev->in_ctx);
        /* Input ctx add_flags are the endpoint index plus one */
        //ep_index = xhci_last_valid_endpoint(le32_to_cpu(ctrl_ctx->add_flags)) - 1;
//bandwidth_change:
        xhci_dbg(xhci, "Completed config ep cmd\n");
        xhci->devs[slot_id]->cmd_status = GET_COMP_CODE(le32_to_cpu(event->status));
        UNUSED(x_sema_unlock(xhci->devs[slot_id]->cmd_completion));
        break;
    case TRB_TYPE(TRB_EVAL_CONTEXT):
#if 0
        virt_dev = xhci->devs[slot_id];
        if (handle_cmd_in_cmd_wait_list(xhci, virt_dev, event))
            break;
        xhci->devs[slot_id]->cmd_status = GET_COMP_CODE(le32_to_cpu(event->status));
        complete(&xhci->devs[slot_id]->cmd_completion);
#endif
        break;
    case TRB_TYPE(TRB_ADDR_DEV):
        xhci->devs[slot_id]->cmd_status = GET_COMP_CODE(le32_to_cpu(event->status));
        UNUSED(x_sema_unlock(xhci->addr_dev));
        break;
#if 0
    case TRB_TYPE(TRB_STOP_RING):
        handle_stopped_endpoint(xhci, xhci->cmd_ring->dequeue, event);
        break;
    case TRB_TYPE(TRB_SET_DEQ):
        handle_set_deq_completion(xhci, event, xhci->cmd_ring->dequeue);
        break;
    case TRB_TYPE(TRB_CMD_NOOP):
        break;
    case TRB_TYPE(TRB_RESET_EP):
        handle_reset_ep_completion(xhci, event, xhci->cmd_ring->dequeue);
        break;
    case TRB_TYPE(TRB_RESET_DEV):
        xhci_dbg(xhci, "Completed reset device command.\n");
        slot_id = TRB_TO_SLOT_ID(
            le32_to_cpu(xhci->cmd_ring->dequeue->generic.field[3]));
        virt_dev = xhci->devs[slot_id];
        if (virt_dev)
            handle_cmd_in_cmd_wait_list(xhci, virt_dev, event);
        else
            xhci_warn(xhci, "Reset device command completion "
                    "for disabled slot %u\n", slot_id);
        break;
#endif
    case TRB_TYPE(TRB_NEC_GET_FW):
        break;
    default:
        /* Skip over unknown commands on the event ring */
        xhci->error_bitmask |= 1 << 6;
        break;
    }
    inc_deq(xhci, xhci->cmd_ring, false);
}

unsigned int xhci_port_speed(unsigned int port_status)
{
    if (DEV_LOWSPEED(port_status))
        return USB_PORT_STAT_LOW_SPEED;
    if (DEV_HIGHSPEED(port_status))
        return USB_PORT_STAT_HIGH_SPEED;
    if (DEV_SUPERSPEED(port_status))
        return USB_PORT_STAT_SUPER_SPEED;
    /*
     * FIXME: Yes, we should check for full speed, but the core uses that as
     * a default in portspeed() in usb/core/hub.c (which is the only place
     * USB_PORT_STAT_*_SPEED is used).
     */
    return 0;
}

int rh_get_port_status(struct xhci_hcd *xhci, int port_id){
    u32 temp,status;
    u32 __iomem *addr;
    
    port_id--;
    status = 0;
    
    addr = &xhci->op_regs->port_status_base + NUM_PORT_REGS*(port_id & 0xff);
    temp = xhci_readl(xhci, addr);
    xhci_dbg(xhci, "get port status, actual port %d status  = 0x%x\n", port_id, temp);

    /* wPortChange bits */
    if (temp & PORT_CSC)
        status |= USB_PORT_STAT_C_CONNECTION << 16;
    if (temp & PORT_PEC)
        status |= USB_PORT_STAT_C_ENABLE << 16;
    if ((temp & PORT_OCC))
        status |= USB_PORT_STAT_C_OVERCURRENT << 16;
    if ((temp & PORT_RC))
        status |= USB_PORT_STAT_C_RESET << 16;
    if ((temp & PORT_PLC))
        status |= USB_PORT_STAT_C_SUSPEND << 16;
    /*
     * FIXME ignoring suspend, reset, and USB 2.1/3.0 specific
     * changes
     */
    if (temp & PORT_CONNECT) {
        status |= USB_PORT_STAT_CONNECTION;
        status |= xhci_port_speed(temp);
    }
    if (temp & PORT_PE)
        status |= USB_PORT_STAT_ENABLE;
    if (temp & PORT_OC)
        status |= USB_PORT_STAT_OVERCURRENT;
    if (temp & PORT_RESET)
        status |= USB_PORT_STAT_RESET;
    if (temp & PORT_POWER)
        status |= USB_PORT_STAT_POWER;

    temp = xhci_port_state_to_neutral(temp);
    xhci_dbg(xhci, "Actual port %d status  = 0x%x\n", port_id, temp);

    return status;
}

u32 xhci_port_state_to_clear_change(u32 state)
{
    return (state & XHCI_PORT_RO) | (state & XHCI_PORT_RWS) | (state & XHCI_PORT_RW1C);
}

void rh_port_clear_change(struct xhci_hcd *xhci, int port_id)
{
    u32 temp;
    u32 __iomem *addr;
    port_id--;
    
    addr = &xhci->op_regs->port_status_base + NUM_PORT_REGS*(port_id & 0xff);
    temp = temp = xhci_readl(xhci, addr);
    xhci_dbg(xhci, "to clear port change, actual port %d status  = 0x%x\n", port_id, temp);
    temp = xhci_port_state_to_clear_change(temp);
    xhci_writel(xhci, temp, addr);
    temp = xhci_readl(xhci, addr);
    xhci_dbg(xhci, "clear port change, actual port %d status  = 0x%x\n", port_id, temp);
}

static void handle_port_status(struct xhci_hcd *xhci, union xhci_trb *event)
{
    u32 port_id, temp;
    int port_status;
    u32 __iomem *addr;
    int port_index;
    struct xhci_port *port;
    
    /* Port status change events always have a successful completion code */
    if (GET_COMP_CODE(event->generic.field[2]) != COMP_SUCCESS) {
        xhci_warn(xhci, "WARN: xHC returned failed port status event\n");
        xhci->error_bitmask |= 1 << 8;
    }
    /* FIXME: core doesn't care about all port link state changes yet */
    port_id = GET_PORT_ID(event->generic.field[0]);
    port_index = port_id - 1; //get_port_index(port_id);
    xhci_dbg(xhci, "Port Status Change Event for port %d\n", port_id);
    xhci_dbg(xhci, "port_index: %d\n", port_index);
    if(port_index >= RH_PORT_NUM){
        xhci_err(xhci, "[ERROR] RH_PORT_NUM not enough\n");
        return;
    }
    port = &rh_port[port_index];
    port->port_id = port_id;
    
    port_status = rh_get_port_status(xhci, port_id);
    xhci_dbg(xhci, "port_status %x\n", port_status);
    
    if(port_status & (USB_PORT_STAT_C_CONNECTION << 16))
    {
        if(port_status & USB_PORT_STAT_CONNECTION)
        {
            xhci_dbg(xhci, "connect port status event, connected\n");
            xhci_lock();
            port->port_status = CONNECTED;
            xhci_unlock();
            if(!(port_status & USB_PORT_STAT_SUPER_SPEED))
            {
                //reset status
                addr = &xhci->op_regs->port_status_base + NUM_PORT_REGS*(port_id-1 & 0xff);
                temp = xhci_readl(xhci, addr);
                temp = xhci_port_state_to_neutral(temp);
                temp = (temp | PORT_RESET);
                xhci_writel(xhci, temp, addr);
                xhci_lock();
                port->port_status = RESET;
                xhci_unlock();
            }
            else
            {
                xhci_lock();
                if(port->port_reenabled == 1)
                {
                    port->port_reenabled = 2;
                }
                if(g_device_reconnect == 1)
                    g_device_reconnect = 2;
                xhci_unlock();
                
                addr = &xhci->op_regs->port_status_base + NUM_PORT_REGS*(port_id-1 & 0xff);
                temp = xhci_readl(xhci, addr);
                if(((temp & PORT_RESET) == 0) && (temp & PORT_PE) && (PORT_PLS(temp) == 0))
                {
                    xhci_lock();
                    port->port_status = ENABLED;
                    port->port_speed = USB_SPEED_SUPER;
                    xhci_unlock();
                    xhci_dbg(xhci, "port set: port_id %d, port_status %d, port_speed %d\n"
                        , port->port_id, port->port_status, port->port_speed);                  
                }
                else{
                    xhci_dbg(xhci, "Super speed port enabled failed!!\n");
                    xhci_dbg(xhci, "temp & PORT_RESET 0x%x\n", (temp & PORT_RESET));
                    xhci_dbg(xhci, "temp & PORT_PE 0x%x\n", (temp & PORT_PE));
                    xhci_dbg(xhci, "temp & PORT_PLS 0x%x\n", (PORT_PLS(temp)));
                    xhci_lock();
                    xhci_unlock();
                }
            }
        }
        else
        {   //port disconnect
            xhci_dbg(xhci, "connect port status event, disconnected\n");
//          port->port_id = 0;
            xhci_lock();
            port->port_speed = USB_SPEED_UNKNOWN;
            port->port_status = DISCONNECTED;
            if(port->port_reenabled == 0)
            {
                port->port_reenabled = 1;
            }
            if(g_device_reconnect == 0)
                g_device_reconnect = 1;
            xhci_unlock();
        }
    }

    xhci_lock();
    if((port_status & (USB_PORT_STAT_C_RESET << 16)) && (port_status & (USB_PORT_STAT_CONNECTION)) && 
        !(port_status & USB_PORT_STAT_SUPER_SPEED))
    {
        if(!(port_status & USB_PORT_STAT_RESET))
        {
            if(port_status & USB_PORT_STAT_LOW_SPEED){
                port->port_speed = USB_SPEED_LOW;
            }
            else if(port_status & USB_PORT_STAT_HIGH_SPEED){
                port->port_speed = USB_SPEED_HIGH;
            }
            else{
                port->port_speed = USB_SPEED_FULL;
            }
            port->port_status = ENABLED;
            if(port->port_reenabled == 1){
                port->port_reenabled = 2;
            }
            if(g_device_reconnect == 1)
                g_device_reconnect = 2;
            xhci_dbg(xhci, "port set: port_id %d, port_status %d, port_speed %d\n"
                , port->port_id, port->port_status, port->port_speed);                  
        }
        else{
        }
    }
    else if((port_status & (USB_PORT_STAT_C_RESET << 16)) && (port_status & (USB_PORT_STAT_CONNECTION)) 
        && (port_status & USB_PORT_STAT_SUPER_SPEED))
    {
        port->port_status = ENABLED;
    }
    xhci_unlock();

    rh_port_clear_change(xhci, port_id);
    /* Update event ring dequeue pointer before dropping the lock */
    inc_deq(xhci, xhci->event_ring, true);
    xhci_set_hc_event_deq(xhci);
}

struct xhci_segment *trb_in_td(struct xhci_segment *start_seg,
        union xhci_trb  *start_trb,
        union xhci_trb  *end_trb,
        dma_addr_t  suspect_dma)
{
    dma_addr_t start_dma;
    dma_addr_t end_seg_dma;
    dma_addr_t end_trb_dma;
    struct xhci_segment *cur_seg;

    start_dma = xhci_trb_virt_to_dma(start_seg, start_trb);
    cur_seg = start_seg;

    do {
        if (start_dma == 0)
        {
            //printk("trb_in_td return null 1\n");
            return NULL;
        }
        /* We may get an event for a Link TRB in the middle of a TD */
        end_seg_dma = xhci_trb_virt_to_dma(cur_seg,
                &cur_seg->trbs[TRBS_PER_SEGMENT - 1]);
        /* If the end TRB isn't in this segment, this is set to 0 */
        end_trb_dma = xhci_trb_virt_to_dma(cur_seg, end_trb);

        if (end_trb_dma > 0) {
            /* The end TRB is in this segment, so suspect should be here */
            if (start_dma <= end_trb_dma) {
                if (suspect_dma >= start_dma && suspect_dma <= end_trb_dma)
                    return cur_seg;
            } else {
                /* Case for one segment with
                 * a TD wrapped around to the top
                 */
                if ((suspect_dma >= start_dma &&
                            suspect_dma <= end_seg_dma) ||
                        (suspect_dma >= cur_seg->dma &&
                         suspect_dma <= end_trb_dma))
                    return cur_seg;
            }
            return NULL;
        } else {
            /* Might still be somewhere in this segment */
            if (suspect_dma >= start_dma && suspect_dma <= end_seg_dma)
                return cur_seg;
        }
        cur_seg = cur_seg->next;
        start_dma = xhci_trb_virt_to_dma(cur_seg, &cur_seg->trbs[0]);
    } while (cur_seg != start_seg);

    //printk("trb_in_td return null 3\n");
    return NULL;
}


/*
 * If this function returns an error condition, it means it got a Transfer
 * event with a corrupted Slot ID, Endpoint ID, or TRB DMA address.
 * At this point, the host controller is probably hosed and should be reset.
 */
static int handle_tx_event(struct xhci_hcd *xhci, struct xhci_transfer_event *event)
{
    struct xhci_virt_device *xdev;
    struct xhci_virt_ep *ep;
    struct xhci_ring *ep_ring;
    unsigned int slot_id;
    int ep_index;
    struct xhci_td *td = NULL;
    dma_addr_t event_dma;
    struct xhci_segment *event_seg;
    union xhci_trb *event_trb;
    //struct urb *urb = NULL;
    int status = -1;
    struct urb_priv *urb_priv;
    struct xhci_ep_ctx *ep_ctx;
    struct list_head *tmp;
    u32 trb_comp_code;
    int ret = 0;
    int td_num = 0;

    // eddie
    UNUSED(status);
    UNUSED(ret);
    UNUSED(urb_priv);

    slot_id = TRB_TO_SLOT_ID(le32_to_cpu(event->flags));
    xdev = xhci->devs[slot_id];
    if (!xdev) {
        xhci_err(xhci, "ERROR Transfer event pointed to bad slot\n");
        return -1;
    }

    /* Endpoint ID is 1 based, our index is zero based */
    ep_index = TRB_TO_EP_ID(le32_to_cpu(event->flags)) - 1;
    
    ep = &xdev->eps[ep_index];
    ep_ring = ep->ring;
    ep_ctx = xhci_get_ep_ctx(xhci, xdev->out_ctx, ep_index);
    if (!ep_ring || (le32_to_cpu(ep_ctx->ep_info) & EP_STATE_MASK) == EP_STATE_DISABLED) 
    {
        xhci_err(xhci, "ERROR Transfer event for disabled endpoint "
                "or incorrect stream ring\n");
        return -1;
    }

    /* Count current td numbers if ep->skip is set */
    if (ep->skip) {
        list_for_each(tmp, &ep_ring->td_list)
            td_num++;
    }

    event_dma = le64_to_cpu(event->buffer);
    trb_comp_code = GET_COMP_CODE(le32_to_cpu(event->transfer_len));
    /* Look for common error cases */
    switch (trb_comp_code) {
    /* Skip codes that require special handling depending on
     * transfer type
     */
    case COMP_SUCCESS:
        break;
    case COMP_SHORT_TX:
        //xhci_dbg(xhci, "COMP_SHORT_TX\n");
        break;
    case COMP_STOP:
        xhci_dbg(xhci, "Stopped on Transfer TRB\n");
        break;
    case COMP_STOP_INVAL:
        xhci_dbg(xhci, "Stopped on No-op or Link TRB\n");
        break;
    case COMP_STALL:
        xhci_warn(xhci, "WARN: Stalled endpoint\n");
        ep->ep_state |= EP_HALTED;
        status = -1;
        break;
    case COMP_TRB_ERR:
        xhci_warn(xhci, "WARN: TRB error on endpoint\n");
        status = -1;
        break;
    case COMP_BABBLE:
        xhci_warn(xhci, "WARN: babble error on endpoint\n");
        status = -1;
        break;
    case COMP_SPLIT_ERR:
    case COMP_TX_ERR:   
    case COMP_DB_ERR:
    case COMP_BW_OVER:
    case COMP_BUFF_OVER:
    case COMP_DEV_ERR:
        status = -1;
        break;
    case COMP_UNDERRUN:
        /*
         * When the Isoch ring is empty, the xHC will generate
         * a Ring Overrun Event for IN Isoch endpoint or Ring
         * Underrun Event for OUT Isoch endpoint.
         */
        xhci_dbg(xhci, "underrun event on endpoint\n");
        if (!list_empty(&ep_ring->td_list))
            xhci_dbg(xhci, "Underrun Event for slot %d ep %d still with TDs queued?\n",
                 TRB_TO_SLOT_ID(le32_to_cpu(event->flags)), ep_index);
        goto cleanup;
    case COMP_OVERRUN:
        xhci_dbg(xhci, "overrun event on endpoint\n");
        if (!list_empty(&ep_ring->td_list))
            xhci_dbg(xhci, "Overrun Event for slot %d ep %d still with TDs queued?\n",
                 TRB_TO_SLOT_ID(le32_to_cpu(event->flags)), ep_index);
        goto cleanup;
    case COMP_MISSED_INT:
        /*
         * When encounter missed service error, one or more isoc tds
         * may be missed by xHC.
         * Set skip flag of the ep_ring; Complete the missed tds as
         * short transfer when process the ep_ring next time.
         */
        ep->skip = true;
        xhci_dbg(xhci, "Miss service interval error, set skip flag\n");
        goto cleanup;
    default:
        xhci_warn(xhci, "ERROR Unknown event condition, HC probably busted\n");
        goto cleanup;
    }

    do {
        /* This TRB should be in the TD at the head of this ring's
         * TD list.
         */
        if (list_empty(&ep_ring->td_list)) 
        {
            xhci_warn(xhci, "WARN Event TRB for slot %d ep %d with no TDs queued?\n",
                  TRB_TO_SLOT_ID(le32_to_cpu(event->flags)),ep_index);
            //xhci_dbg(xhci, "Event TRB with TRB type ID %u\n",
            //     (unsigned int) (le32_to_cpu(event->flags) & TRB_TYPE_BITMASK)>>10);
            //xhci_print_trb_offsets(xhci, (union xhci_trb *) event);
            if (ep->skip) 
            {
                ep->skip = false;
                xhci_dbg(xhci, "td_list is empty while skip flag set. Clear skip flag.\n");
            }
            ret = 0;
            // eddie, temporary remove
            //goto cleanup;
            inc_deq(xhci, xhci->event_ring, true); // temporary add
            return ret;
        }

        /* We've skipped all the TDs on the ep ring when ep->skip set */
        if (ep->skip && td_num == 0) {
            ep->skip = false;
            xhci_dbg(xhci, "All tds on the ep_ring skipped. Clear skip flag.\n");
            ret = 0;
            goto cleanup;
        }

        td = list_entry(ep_ring->td_list.next, struct xhci_td, td_list);
        if (ep->skip)
            td_num--;

        /* Is this a TRB in the currently executing TD? */
        event_seg = trb_in_td(ep_ring->deq_seg, ep_ring->dequeue,
                              td->last_trb, event_dma);

        /*
         * Skip the Force Stopped Event. The event_trb(event_dma) of FSE
         * is not in the current TD pointed by ep_ring->dequeue because
         * that the hardware dequeue pointer still at the previous TRB
         * of the current TD. The previous TRB maybe a Link TD or the
         * last TRB of the previous TD. The command completion handle
         * will take care the rest.
         */
        if (!event_seg && trb_comp_code == COMP_STOP_INVAL) {
            ret = 0;
            goto cleanup;
        }

        if ((!event_seg) && (!ep->skip))
        {
            ASSERT(0);
        }
        
        if (ep->skip) {
            xhci_dbg(xhci, "Found td. Clear skip flag.\n");
            ep->skip = false;
        }
        event_trb = &event_seg->trbs[(event_dma - event_seg->dma) / sizeof(*event_trb)];
        /*
         * No-op TRB should not trigger interrupts.
         * If event_trb is a no-op TRB, it means the
         * corresponding TD has been cancelled. Just ignore
         * the TD.
         */
        if ((le32_to_cpu(event_trb->generic.field[3]) & TRB_TYPE_BITMASK) == TRB_TYPE(TRB_TR_NOOP)) 
         {
            xhci_dbg(xhci, "event_trb is a no-op TRB. Skip it\n");
            goto cleanup;
        }

        /* Now update the urb's actual_length and give back to
         * the core
         */
#if 0
        if (usb_endpoint_xfer_control(&td->urb->ep->desc))
            ret = process_ctrl_td(xhci, td, event_trb, event, ep,
                         &status);
        else if (usb_endpoint_xfer_isoc(&td->urb->ep->desc))
            ret = process_isoc_td(xhci, td, event_trb, event, ep,
                         &status);
        else
            ret = process_bulk_intr_td(xhci, td, event_trb, event,
                         ep, &status);
#endif
		/* Update ring dequeue pointer */
		while (ep_ring->dequeue != td->last_trb)
			inc_deq(xhci, ep_ring, false);
		inc_deq(xhci, ep_ring, false);

        // eddie, td handler
        //Printf("Delete td_list\n");
        list_del_init(&td->td_list);

cleanup:
        /*
         * Do not update event ring dequeue pointer if ep->skip is set.
         * Will roll back to continue process missed tds.
         */
        if (trb_comp_code == COMP_MISSED_INT || !ep->skip) {
            inc_deq(xhci, xhci->event_ring, true);
        }
#if 0 // eddie
        if (ret) {
            urb = td->urb;
            urb_priv = urb->hcpriv;
            /* Leave the TD around for the reset endpoint function
             * to use(but only if it's not a control endpoint,
             * since we already queued the Set TR dequeue pointer
             * command for stalled control endpoints).
             */
            if (usb_endpoint_xfer_control(&urb->ep->desc) ||
                (trb_comp_code != COMP_STALL &&
                    trb_comp_code != COMP_BABBLE))
                xhci_urb_free_priv(xhci, urb_priv);

            usb_hcd_unlink_urb_from_ep(bus_to_hcd(urb->dev->bus), urb);
            if ((urb->actual_length != urb->transfer_buffer_length &&
                        (urb->transfer_flags &
                         URB_SHORT_NOT_OK)) ||
                    status != 0)
                xhci_dbg(xhci, "Giveback URB %p, len = %d, "
                        "expected = %x, status = %d\n",
                        urb, urb->actual_length,
                        urb->transfer_buffer_length,
                        status);
            spin_unlock(&xhci->lock);
            /* EHCI, UHCI, and OHCI always unconditionally set the
             * urb->status of an isochronous endpoint to 0.
             */
            if (usb_pipetype(urb->pipe) == PIPE_ISOCHRONOUS)
                status = 0;
            usb_hcd_giveback_urb(bus_to_hcd(urb->dev->bus), urb, status);
            spin_lock(&xhci->lock);
        }
#endif
    /*
     * If ep->skip is set, it means there are missed tds on the
     * endpoint ring need to take care of.
     * Process them as short transfer until reach the td pointed by
     * the event.
     */
    } while (ep->skip && trb_comp_code != COMP_MISSED_INT);

    _u4TrEventField2 = le32_to_cpu(event->transfer_len);
    VERIFY(x_sema_unlock(_hXhci_Sema) == OSR_OK);

    return 0;
}


static int xhci_handle_event(struct xhci_hcd *xhci)
{
    union xhci_trb *event;
    int update_ptrs = 1;
    int ret = 0;

    if (!xhci->event_ring || !xhci->event_ring->dequeue) {
        xhci->error_bitmask |= 1 << 1;
        return 0;
    }

    event = xhci->event_ring->dequeue;
    /* Does the HC or OS own the TRB? */
    if ((le32_to_cpu(event->event_cmd.flags) & TRB_CYCLE) !=
        xhci->event_ring->cycle_state) {
        xhci->error_bitmask |= 1 << 2;
        return 0;
    }

    /*
     * Barrier between reading the TRB_CYCLE (valid) flag above and any
     * speculative reads of the event's flags/data below.
     */
    rmb();
    /* FIXME: Handle more event types. */
    switch ((le32_to_cpu(event->event_cmd.flags) & TRB_TYPE_BITMASK)) {
    case TRB_TYPE(TRB_COMPLETION):
        handle_cmd_completion(xhci, &event->event_cmd);
        break;
    case TRB_TYPE(TRB_PORT_STATUS):
        handle_port_status(xhci, event);
        update_ptrs = 0;
        break;
    case TRB_TYPE(TRB_TRANSFER):
        ret = handle_tx_event(xhci, &event->trans_event);
        if (ret < 0)
            xhci->error_bitmask |= 1 << 9;
        else
            update_ptrs = 0;
        break;
    default:
        Printf("Not recognize event\n");
        ASSERT(0);
    }
    /* Any of the above functions may drop and re-acquire the lock, so check
     * to make sure a watchdog timer didn't mark the host as non-responsive.
     */
    if (xhci->xhc_state & XHCI_STATE_DYING) {
        xhci_dbg(xhci, "xHCI host dying, returning from "
                "event handler.\n");
        return 0;
    }

    if (update_ptrs)
        /* Update SW event ring dequeue pointer */
        inc_deq(xhci, xhci->event_ring, true);

    /* Are there more items on the event ring?  Caller will call us again to
     * check.
     */
    return 1;
}

static VOID _xHCIIrqHandler(UINT16 u2Vector)
{
    struct xhci_hcd *xhci = &_mtk_xhci;
    u32 status;
    //union xhci_trb *trb;
    u64 temp_64;
    union xhci_trb *event_ring_deq;
    dma_addr_t deq;

    xhci_lock();
    //trb = xhci->event_ring->dequeue; ?
    /* Check if the xHC generated the interrupt, or the irq is shared */
    status = xhci_readl(xhci, &xhci->op_regs->status);
    if (status == 0xffffffff)
        goto hw_died;

    if (!(status & STS_EINT)) {
        xhci_unlock();
        return;
    }
    if (status & STS_FATAL) {
        xhci_warn(xhci, "WARNING: Host System Error\n");
        xhci_halt(xhci);
hw_died:
        xhci_unlock();
        return;
    }

    /*
     * Clear the op reg interrupt status first,
     * so we can receive interrupts from other MSI-X interrupters.
     * Write 1 to clear the interrupt status.
     */
    status |= STS_EINT;
    xhci_writel(xhci, status, &xhci->op_regs->status);
    /* FIXME when MSI-X is supported and there are multiple vectors */
    /* Clear the MSI-X event interrupt status */

    //if (hcd->irq != -1) 
    {
        u32 irq_pending;
        /* Acknowledge the PCI interrupt */
        irq_pending = xhci_readl(xhci, &xhci->ir_set->irq_pending);
        irq_pending |= 0x3;
        xhci_writel(xhci, irq_pending, &xhci->ir_set->irq_pending);
    }

    if (xhci->xhc_state & XHCI_STATE_DYING) {
        /* Clear the event handler busy flag (RW1C);
         * the event ring should be empty.
         */
        temp_64 = xhci_read_64(xhci, &xhci->ir_set->erst_dequeue);
        xhci_write_64(xhci, temp_64 | ERST_EHB,
                &xhci->ir_set->erst_dequeue);
        xhci_unlock();

        return;
    }

    event_ring_deq = xhci->event_ring->dequeue;
    /* FIXME this should be a delayed service routine
     * that clears the EHB.
     */
    while (xhci_handle_event(xhci) > 0) {}

    temp_64 = xhci_read_64(xhci, &xhci->ir_set->erst_dequeue);
    /* If necessary, update the HW's version of the event ring deq ptr. */
    if (event_ring_deq != xhci->event_ring->dequeue) {
        deq = xhci_trb_virt_to_dma(xhci->event_ring->deq_seg,
                xhci->event_ring->dequeue);
        if (deq == 0)
            xhci_warn(xhci, "WARN something wrong with SW event "
                    "ring dequeue ptr.\n");
        /* Update HC event ring dequeue pointer */
        temp_64 &= ERST_PTR_MASK;
        temp_64 |= ((u64) deq & (u64) ~ERST_PTR_MASK);
    }

    /* Clear the event handler busy flag (RW1C); event ring is empty. */
    temp_64 |= ERST_EHB;
    xhci_write_64(xhci, temp_64, &xhci->ir_set->erst_dequeue);

    xhci_unlock();
}

/*
 * Check to see if there's room to enqueue num_trbs on the ring.  See rules
 * above.
 * FIXME: this would be simpler and faster if we just kept track of the number
 * of free TRBs in a ring.
 */
static int room_on_ring(struct xhci_hcd *xhci, struct xhci_ring *ring,
        unsigned int num_trbs)
{
    int i;
    union xhci_trb *enq = ring->enqueue;
    struct xhci_segment *enq_seg = ring->enq_seg;
    struct xhci_segment *cur_seg;
    unsigned int left_on_ring;

    /* If we are currently pointing to a link TRB, advance the
     * enqueue pointer before checking for space */
    while (last_trb(xhci, ring, enq_seg, enq)) {
        enq_seg = enq_seg->next;
        enq = enq_seg->trbs;
    }

    /* Check if ring is empty */
    if (enq == ring->dequeue) {
        /* Can't use link trbs */
        left_on_ring = TRBS_PER_SEGMENT - 1;
        for (cur_seg = enq_seg->next; cur_seg != enq_seg; cur_seg = cur_seg->next)
            left_on_ring += TRBS_PER_SEGMENT - 1;

        /* Always need one TRB free in the ring. */
        left_on_ring -= 1;
        if (num_trbs > left_on_ring) {
            xhci_warn(xhci, "Not enough room on ring; "
                    "need %u TRBs, %u TRBs left\n",
                    num_trbs, left_on_ring);
            return 0;
        }
        return 1;
    }
    /* Make sure there's an extra empty TRB available */
    for (i = 0; i <= num_trbs; ++i) {
        if (enq == ring->dequeue)
            return 0;
        enq++;
        while (last_trb(xhci, ring, enq_seg, enq)) {
            enq_seg = enq_seg->next;
            enq = enq_seg->trbs;
        }
    }
    return 1;
}

static int prepare_ring(struct xhci_hcd *xhci, struct xhci_ring *ep_ring,
        u32 ep_state, unsigned int num_trbs, bool isoc)
{
    /* Make sure the endpoint has been added to xHC schedule */
    switch (ep_state) 
    {
    case EP_STATE_DISABLED:
        /*
         * USB core changed config/interfaces without notifying us,
         * or hardware is reporting the wrong state.
         */
        xhci_warn(xhci, "WARN urb submitted to disabled ep\n");
        return -1;
    case EP_STATE_ERROR:
        xhci_warn(xhci, "WARN waiting for error on ep to be cleared\n");
        /* FIXME event handling code for error needs to clear it */
        /* XXX not sure if this should be -ENOENT or not */
        return -1;
    case EP_STATE_HALTED:
        xhci_dbg(xhci, "WARN halted endpoint, queueing URB anyway.\n");
    case EP_STATE_STOPPED:
    case EP_STATE_RUNNING:
        break;
    default:
        xhci_err(xhci, "ERROR unknown endpoint state for ep\n");
        /*
         * FIXME issue Configure Endpoint command to try to get the HC
         * back into a known state.
         */
        return -1;
    }
    if (!room_on_ring(xhci, ep_ring, num_trbs)) {
        /* FIXME allocate more room */
        xhci_err(xhci, "ERROR no room on ep ring\n");
        return -1;
    }

    if (enqueue_is_link_trb(ep_ring)) 
    {
        struct xhci_ring *ring = ep_ring;
        union xhci_trb *next;

        next = ring->enqueue;

        while (last_trb(xhci, ring, ring->enq_seg, next)) {
#if 0
            /* If we're not dealing with 0.95 hardware or isoc rings
             * on AMD 0.96 host, clear the chain bit.
             */
            if (!xhci_link_trb_quirk(xhci) && !(isoc &&
                    (xhci->quirks & XHCI_AMD_0x96_HOST)))
                next->link.control &= cpu_to_le32(~TRB_CHAIN);
            else
                next->link.control |= cpu_to_le32(TRB_CHAIN);
#else
            next->link.control &= cpu_to_le32(~TRB_CHAIN);
#endif
            wmb();
            next->link.control ^= cpu_to_le32((u32) TRB_CYCLE);

            /* Toggle the cycle bit after the last ring segment. */
            if (last_trb_on_last_seg(xhci, ring, ring->enq_seg, next)) {
                ring->cycle_state = (ring->cycle_state ? 0 : 1);
            }
            ring->enq_seg = ring->enq_seg->next;
            ring->enqueue = ring->enq_seg->trbs;
            next = ring->enqueue;
        }
    }

    return 0;
}


/* Ring the host controller doorbell after placing a command on the ring */
void xhci_ring_cmd_db(struct xhci_hcd *xhci)
{
    xhci_dbg(xhci, "// Ding dong!\n");
    xhci_writel(xhci, DB_VALUE_HOST, &xhci->dba->doorbell[0]);
    /* Flush PCI posted writes */
    xhci_readl(xhci, &xhci->dba->doorbell[0]);
}


/* Generic function for queueing a command TRB on the command ring.
 * Check to make sure there's room on the command ring for one command TRB.
 * Also check that there's room reserved for commands that must not fail.
 * If this is a command that must not fail, meaning command_must_succeed = TRUE,
 * then only check for the number of reserved spots.
 * Don't decrement xhci->cmd_ring_reserved_trbs after we've queued the TRB
 * because the command event handler may want to resubmit a failed command.
 */
static int queue_command(struct xhci_hcd *xhci, u32 field1, u32 field2,
        u32 field3, u32 field4, bool command_must_succeed)
{
    int reserved_trbs = xhci->cmd_ring_reserved_trbs;
    int ret;

    xhci_dbg(xhci, "command: %d\n", TRB_FIELD_TO_TYPE(field4));

    if (!command_must_succeed)
        reserved_trbs++;

    ret = prepare_ring(xhci, xhci->cmd_ring, EP_STATE_RUNNING,
            reserved_trbs, false);
    if (ret < 0) {
        xhci_err(xhci, "ERR: No room for command on command ring\n");
        if (command_must_succeed)
            xhci_err(xhci, "ERR: Reserved TRB counting for "
                    "unfailable commands failed.\n");
        return ret;
    }
    queue_trb(xhci, xhci->cmd_ring, false, false, false, field1, field2,
            field3, field4 | xhci->cmd_ring->cycle_state);
    return 0;
}


static struct xhci_container_ctx *xhci_alloc_container_ctx(struct xhci_hcd *xhci,
                            int type)
{
    struct xhci_container_ctx *ctx = x_mem_alloc(sizeof(*ctx));
    x_memset((void*)ctx, 0, sizeof(*ctx));
    if (!ctx)
        return NULL;

    ASSERT((type == XHCI_CTX_TYPE_DEVICE) || (type == XHCI_CTX_TYPE_INPUT));
    ctx->type = type;
    ctx->size = HCC_64BYTE_CONTEXT(xhci->hcc_params) ? 2048 : 1024;
    if (type == XHCI_CTX_TYPE_INPUT)
        ctx->size += CTX_SIZE(xhci->hcc_params);

    ctx->bytes = (u8*)xhci_dma_alloc(ctx->size);
    ctx->dma = (dma_addr_t)ctx->bytes & 0x7FFFFFFF;
    x_memset(ctx->bytes, 0, ctx->size);
    return ctx;
}

static void xhci_free_container_ctx(struct xhci_hcd *xhci,
                 struct xhci_container_ctx *ctx)
{
    if (!ctx)
        return;
    xhci_dma_free(ctx->bytes);
    x_mem_free(ctx);
}

void xhci_free_virt_device(struct xhci_hcd *xhci, int slot_id)
{
    struct xhci_virt_device *dev;
    int i;

    /* Slot ID 0 is reserved */
    if (slot_id == 0 || !xhci->devs[slot_id])
        return;

    dev = xhci->devs[slot_id];
    xhci->dcbaa->dev_context_ptrs[slot_id] = 0;
    if (!dev)
        return;

    for (i = 0; i < 31; ++i) {
        if (dev->eps[i].ring)
            xhci_ring_free(xhci, dev->eps[i].ring);
    }

    if (dev->in_ctx)
        xhci_free_container_ctx(xhci, dev->in_ctx);
    if (dev->out_ctx)
        xhci_free_container_ctx(xhci, dev->out_ctx);

    x_mem_free(xhci->devs[slot_id]);
    xhci->devs[slot_id] = NULL;
}

static int xhci_alloc_virt_device(struct xhci_hcd *xhci, int slot_id, struct usb_device *udev)
{
    struct xhci_virt_device *dev;

    /* Slot ID 0 is reserved */
    if (slot_id == 0 || xhci->devs[slot_id]) {
        xhci_warn(xhci, "Bad Slot ID %d\n", slot_id);
        return 0;
    }

    xhci->devs[slot_id] = x_mem_alloc(sizeof(*xhci->devs[slot_id]));
    x_memset((void*)xhci->devs[slot_id], 0, sizeof(*xhci->devs[slot_id]));
    if (!xhci->devs[slot_id])
        return 0;
    dev = xhci->devs[slot_id];

    /* Allocate the (output) device context that will be used in the HC. */
    dev->out_ctx = xhci_alloc_container_ctx(xhci, XHCI_CTX_TYPE_DEVICE);
    if (!dev->out_ctx)
        goto fail;

    /* Allocate the (input) device context for address device command */
    dev->in_ctx = xhci_alloc_container_ctx(xhci, XHCI_CTX_TYPE_INPUT);
    if (!dev->in_ctx)
        goto fail;

    /* Allocate endpoint 0 ring */
    dev->eps[0].ring = xhci_ring_alloc(xhci, 1, true, false);
    if (!dev->eps[0].ring)
        goto fail;

    /* Allocate pointers to the ring cache */
    dev->num_rings_cached = 0;

    VERIFY(x_sema_create(&dev->cmd_completion, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
    INIT_LIST_HEAD(&dev->cmd_list);
    dev->udev = udev;

    /* Point to output device context in dcbaa. */
    xhci->dcbaa->dev_context_ptrs[slot_id] = cpu_to_le64(dev->out_ctx->dma);
    return 1;
fail:
    xhci_free_virt_device(xhci, slot_id);
    return 0;
}


static unsigned int xhci_get_endpoint_index(struct usb_endpoint_descriptor *desc)
{
    unsigned int index;
    if (usb_endpoint_xfer_control(desc))
        index = (unsigned int) (usb_endpoint_num(desc)*2);
    else
        index = (unsigned int) (usb_endpoint_num(desc)*2) +
            (usb_endpoint_dir_in(desc) ? 1 : 0) - 1;
    return index;
}


static unsigned int xhci_get_endpoint_flag(struct usb_endpoint_descriptor *desc)
{
    return 1 << (xhci_get_endpoint_index(desc) + 1);
}


static struct xhci_slot_ctx *xhci_get_slot_ctx(struct xhci_hcd *xhci,
                    struct xhci_container_ctx *ctx)
{
    if (ctx->type == XHCI_CTX_TYPE_DEVICE)
        return (struct xhci_slot_ctx *)ctx->bytes;

    return (struct xhci_slot_ctx *)
        (ctx->bytes + CTX_SIZE(xhci->hcc_params));
}


/* Setup an xHCI virtual device for a Set Address command */
static int xhci_setup_addressable_virt_dev(struct xhci_hcd *xhci, struct usb_device *udev)
{
    struct xhci_virt_device *dev;
    struct xhci_ep_ctx  *ep0_ctx;
    struct xhci_slot_ctx    *slot_ctx;
    u32         port_num;
    //struct usb_device *top_dev;

    dev = xhci->devs[udev->slot_id];
    /* Slot ID 0 is reserved */
    if (!dev) 
    {
        return -1;
    }
    ep0_ctx = xhci_get_ep_ctx(xhci, dev->in_ctx, 0);
    slot_ctx = xhci_get_slot_ctx(xhci, dev->in_ctx);

    /* 3) Only the control endpoint is valid - one endpoint context */
    //slot_ctx->dev_info |= cpu_to_le32(LAST_CTX(1) | (u32) udev->route);
    // eddie, no need route
    slot_ctx->dev_info |= cpu_to_le32(LAST_CTX(1));
    switch (udev->speed) {
    case USB_SPEED_SUPER:
        slot_ctx->dev_info |= cpu_to_le32((u32) SLOT_SPEED_SS);
        break;
    case USB_SPEED_HIGH:
        slot_ctx->dev_info |= cpu_to_le32((u32) SLOT_SPEED_HS);
        break;
    case USB_SPEED_FULL:
        slot_ctx->dev_info |= cpu_to_le32((u32) SLOT_SPEED_FS);
        break;
    case USB_SPEED_LOW:
        slot_ctx->dev_info |= cpu_to_le32((u32) SLOT_SPEED_LS);
        break;
    case USB_SPEED_WIRELESS:
        xhci_dbg(xhci, "FIXME xHCI doesn't support wireless speeds\n");
        return -1;
    default:
        /* Speed was set earlier, this shouldn't happen. */
        ASSERT(0);
    }
    /* Find the root hub port this device is under */
    //port_num = xhci_find_real_port_number(xhci, udev);
    port_num = udev->portnum;
    if (!port_num)
        return -1;
    slot_ctx->dev_info2 |= cpu_to_le32((u32) ROOT_HUB_PORT(port_num));

    dev->port = port_num;
#if 0   
    /* Set the port number in the virtual_device to the faked port number */
    for (top_dev = udev; top_dev->parent && top_dev->parent->parent;
            top_dev = top_dev->parent)
        /* Found device below root hub */;
    dev->port = top_dev->portnum;

    xhci_dbg(xhci, "Set root hub portnum to %d\n", port_num);
    xhci_dbg(xhci, "Set fake root hub portnum to %d\n", dev->port);
    
    /* Is this a LS/FS device under an external HS hub? */
    if (udev->tt && udev->tt->hub->parent) {
        slot_ctx->tt_info = cpu_to_le32(udev->tt->hub->slot_id |
                        (udev->ttport << 8));
        if (udev->tt->multi)
            slot_ctx->dev_info |= cpu_to_le32(DEV_MTT);
    }
    xhci_dbg(xhci, "udev->tt = %p\n", udev->tt);
    xhci_dbg(xhci, "udev->ttport = 0x%x\n", udev->ttport);
#endif
    /* Step 4 - ring already allocated */
    /* Step 5 */
    ep0_ctx->ep_info2 = cpu_to_le32(EP_TYPE(CTRL_EP));
    /*
     * XXX: Not sure about wireless USB devices.
     */
    switch (udev->speed) {
    case USB_SPEED_SUPER:
        ep0_ctx->ep_info2 |= cpu_to_le32(MAX_PACKET(512));
        break;
    case USB_SPEED_HIGH:
    /* USB core guesses at a 64-byte max packet first for FS devices */
    case USB_SPEED_FULL:
        ep0_ctx->ep_info2 |= cpu_to_le32(MAX_PACKET(64));
        break;
    case USB_SPEED_LOW:
        ep0_ctx->ep_info2 |= cpu_to_le32(MAX_PACKET(8));
        break;
    case USB_SPEED_WIRELESS:
        xhci_dbg(xhci, "FIXME xHCI doesn't support wireless speeds\n");
        return -1;
    default:
        /* New speed? */
        ASSERT(0);
    }
    /* EP 0 can handle "burst" sizes of 1, so Max Burst Size field is 0 */
    ep0_ctx->ep_info2 |= cpu_to_le32(MAX_BURST(0) | ERROR_COUNT(3));

    ep0_ctx->deq = cpu_to_le64(dev->eps[0].ring->first_seg->dma |
                   dev->eps[0].ring->cycle_state);

    /* Steps 7 and 8 were done in xhci_alloc_virt_device() */

    return 0;
}

/*
 * Issue an Address Device command (which will issue a SetAddress request to
 * the device).
 * We should be protected by the usb_address0_mutex in khubd's hub_port_init, so
 * we should only issue and wait on one address command at the same time.
 *
 * We add one to the device address issued by the hardware because the USB core
 * uses address 1 for the root hubs (even though they're not really devices).
 */
int xhci_address_device(struct usb_device *udev)
{
    struct xhci_virt_device *virt_dev;
    int ret = 0;
    struct xhci_hcd *xhci = &_mtk_xhci;
    struct xhci_slot_ctx *slot_ctx;
    struct xhci_input_control_ctx *ctrl_ctx;

    virt_dev = xhci->devs[udev->slot_id];

    if(!udev)
    {
        return -1;
    }

    if (!virt_dev) 
    {
        return -1;
    }

    slot_ctx = xhci_get_slot_ctx(xhci, virt_dev->in_ctx);
    /*
     * If this is the first Set Address since device plug-in or
     * virt_device realloaction after a resume with an xHCI power loss,
     * then set up the slot context.
     */
    if (!slot_ctx->dev_info)
        xhci_setup_addressable_virt_dev(xhci, udev);
    /* Otherwise, update the control endpoint ring enqueue pointer. */
    else
    {
        ASSERT(0);
        //xhci_copy_ep0_dequeue_into_input_ctx(xhci, udev);
    }
    ctrl_ctx = xhci_get_input_control_ctx(xhci, virt_dev->in_ctx);
    ctrl_ctx->add_flags = cpu_to_le32(SLOT_FLAG | EP0_FLAG);
    ctrl_ctx->drop_flags = 0;

    xhci_lock();
    ret = xhci_queue_address_device(xhci, virt_dev->in_ctx->dma, udev->slot_id);
    if (ret) {
        xhci_unlock();
        xhci_dbg(xhci, "FIXME: allocate a command ring segment\n");
        return ret;
    }
    xhci_ring_cmd_db(xhci);
    xhci_unlock();

    if(OSR_OK != x_sema_lock_timeout(xhci->addr_dev, 5000))
    {
        return -1;
    }

    switch (virt_dev->cmd_status) {
    case COMP_CTX_STATE:
    case COMP_EBADSLT:
        xhci_err(xhci, "Setup ERROR: address device command for slot %d.\n",
                udev->slot_id);
        ret = -1;
        break;
    case COMP_TX_ERR:
        //dev_warn(&udev->dev, "Device not responding to set address.\n");
        ret = -1;
        break;
    case COMP_DEV_ERR:
        //dev_warn(&udev->dev, "ERROR: Incompatible device for address "
        //      "device command.\n");
        ret = -1;
        break;
    case COMP_SUCCESS:
        xhci_dbg(xhci, "Successful Address Device command\n");
        break;
    default:
        xhci_err(xhci, "ERROR: unexpected command completion "
                "code 0x%x.\n", virt_dev->cmd_status);
        //xhci_dbg(xhci, "Slot ID %d Output Context:\n", udev->slot_id);
        //xhci_dbg_ctx(xhci, virt_dev->out_ctx, 2);
        ret = -1;
        break;
    }
    if (ret) {
        return ret;
    }

    /*
     * USB core uses address 1 for the roothubs, so we add one to the
     * address given back to us by the HC.
     */
    slot_ctx = xhci_get_slot_ctx(xhci, virt_dev->out_ctx);
    /* Use kernel assigned address for devices; store xHC assigned
     * address locally. */
    virt_dev->address = (le32_to_cpu(slot_ctx->dev_state) & DEV_ADDR_MASK)
        + 1;
    /* Zero the input context control for later use */
    ctrl_ctx->add_flags = 0;
    ctrl_ctx->drop_flags = 0;

    xhci_dbg(xhci, "Internal device address = %d\n", virt_dev->address);

    // eddie
    udev->devnum = virt_dev->address;

    return 0;
}


static void xhci_add_in_port(struct xhci_hcd *xhci, unsigned int num_ports,
        __le32 __iomem *addr, u8 major_revision)
{
    u32 temp, port_offset, port_count;
    int i;

    if (major_revision > 0x03) {
        xhci_warn(xhci, "Ignoring unknown port speed, "
                "Ext Cap %p, revision = 0x%x\n",
                addr, major_revision);
        /* Ignoring port protocol we can't understand. FIXME */
        return;
    }

    /* Port offset and count in the third dword, see section 7.2 */
    temp = xhci_readl(xhci, addr + 2);
    port_offset = XHCI_EXT_PORT_OFF(temp);
    port_count = XHCI_EXT_PORT_COUNT(temp);
    xhci_dbg(xhci, "Ext Cap %p, port offset = %u, "
            "count = %u, revision = 0x%x\n",
            addr, port_offset, port_count, major_revision);
    /* Port count includes the current port offset */
    if (port_offset == 0 || (port_offset + port_count - 1) > num_ports)
        /* WTF? "Valid values are ????to MaxPorts" */
        return;
    port_offset--;
    for (i = port_offset; i < (port_offset + port_count); i++) {
        /* Duplicate entry.  Ignore the port if the revisions differ. */
        if (xhci->port_array[i] != 0) {
            xhci_warn(xhci, "Duplicate port entry, Ext Cap %p,"
                    " port %u\n", addr, i);
            xhci_warn(xhci, "Port was marked as USB %u, "
                    "duplicated as USB %u\n",
                    xhci->port_array[i], major_revision);
            /* Only adjust the roothub port counts if we haven't
             * found a similar duplicate.
             */
            if (xhci->port_array[i] != major_revision &&
                xhci->port_array[i] != DUPLICATE_ENTRY) {
                if (xhci->port_array[i] == 0x03)
                    xhci->num_usb3_ports--;
                else
                    xhci->num_usb2_ports--;
                xhci->port_array[i] = DUPLICATE_ENTRY;
            }
            /* FIXME: Should we disable the port? */
            continue;
        }
        xhci->port_array[i] = major_revision;
        if (major_revision == 0x03)
            xhci->num_usb3_ports++;
        else
            xhci->num_usb2_ports++;
    }
    /* FIXME: Should we disable ports not in the Extended Capabilities? */
}

static int xhci_setup_port_arrays(struct xhci_hcd *xhci)
{
    __le32 __iomem *addr;
    u32 offset;
    unsigned int num_ports;
    int i, port_index;

    addr = &xhci->cap_regs->hcc_params;
    offset = XHCI_HCC_EXT_CAPS(xhci_readl(xhci, addr));
    if (offset == 0) {
        xhci_err(xhci, "No Extended Capability registers, "
                "unable to set up roothub.\n");
        return -1;
    }

    num_ports = HCS_MAX_PORTS(xhci->hcs_params1);
    xhci->port_array = (u8*)x_mem_alloc(sizeof(*xhci->port_array)*num_ports);
    if (!xhci->port_array)
        return -1;

    /*
     * For whatever reason, the first capability offset is from the
     * capability register base, not from the HCCPARAMS register.
     * See section 5.3.6 for offset calculation.
     */
    addr = &xhci->cap_regs->hc_capbase + offset;
    while (1) {
        u32 cap_id;

        cap_id = xhci_readl(xhci, addr);
        if (XHCI_EXT_CAPS_ID(cap_id) == XHCI_EXT_CAPS_PROTOCOL)
            xhci_add_in_port(xhci, num_ports, addr, (u8) XHCI_EXT_PORT_MAJOR(cap_id));
        offset = XHCI_EXT_CAPS_NEXT(cap_id);
        if (!offset || (xhci->num_usb2_ports + xhci->num_usb3_ports)
                == num_ports)
            break;
        /*
         * Once you're into the Extended Capabilities, the offset is
         * always relative to the register holding the offset.
         */
        addr += offset;
    }

    if (xhci->num_usb2_ports == 0 && xhci->num_usb3_ports == 0) {
        xhci_warn(xhci, "No ports on the roothubs?\n");
        return -1;
    }
    xhci_dbg(xhci, "Found %u USB 2.0 ports and %u USB 3.0 ports.\n",
            xhci->num_usb2_ports, xhci->num_usb3_ports);

    /* Place limits on the number of roothub ports so that the hub
     * descriptors aren't longer than the USB core will allocate.
     */
    if (xhci->num_usb3_ports > 15) {
        xhci_dbg(xhci, "Limiting USB 3.0 roothub ports to 15.\n");
        xhci->num_usb3_ports = 15;
    }
    if (xhci->num_usb2_ports > USB_MAXCHILDREN) {
        xhci_dbg(xhci, "Limiting USB 2.0 roothub ports to %u.\n",
                USB_MAXCHILDREN);
        xhci->num_usb2_ports = USB_MAXCHILDREN;
    }

    /*
     * Note we could have all USB 3.0 ports, or all USB 2.0 ports.
     * Not sure how the USB core will handle a hub with no ports...
     */
    if (xhci->num_usb2_ports) {
        xhci->usb2_ports = x_mem_alloc(sizeof(*xhci->usb2_ports)*xhci->num_usb2_ports);
        if (!xhci->usb2_ports)
            return -1;

        port_index = 0;
        for (i = 0; i < num_ports; i++) {
            if (xhci->port_array[i] == 0x03 ||  xhci->port_array[i] == 0 ||
                    xhci->port_array[i] == DUPLICATE_ENTRY)
                continue;

            xhci->usb2_ports[port_index] = &xhci->op_regs->port_status_base + NUM_PORT_REGS*i;
            xhci_dbg(xhci, "USB 2.0 port at index %u, addr = %p\n", i,
                    xhci->usb2_ports[port_index]);
            port_index++;
            if (port_index == xhci->num_usb2_ports)
                break;
        }
    }
    if (xhci->num_usb3_ports) {
        xhci->usb3_ports = x_mem_alloc(sizeof(*xhci->usb3_ports)*xhci->num_usb3_ports);
        if (!xhci->usb3_ports)
            return -1;

        port_index = 0;
        for (i = 0; i < num_ports; i++)
            if (xhci->port_array[i] == 0x03) {
                xhci->usb3_ports[port_index] = &xhci->op_regs->port_status_base + NUM_PORT_REGS*i;
                xhci_dbg(xhci, "USB 3.0 port at index %u, addr = %p\n", i,
                        xhci->usb3_ports[port_index]);
                port_index++;
                if (port_index == xhci->num_usb3_ports)
                    break;
            }
    }
    return 0;
}

int xhci_mem_init(struct xhci_hcd *xhci)
{
    dma_addr_t  dma;
    u32 val, val2;
    u64     val_64;
    struct xhci_segment *seg;
    u32 page_size;
    int i;

    page_size = xhci_readl(xhci, &xhci->op_regs->page_size);
    xhci_dbg(xhci, "Supported page size register = 0x%x\n", page_size);
    for (i = 0; i < 16; i++) {
        if ((0x1 & page_size) != 0)
            break;
        page_size = page_size >> 1;
    }
    if (i < 16)
        xhci_dbg(xhci, "Supported page size of %iK\n", (1 << (i+12)) / 1024);
    else
        xhci_warn(xhci, "WARN: no supported page size\n");
    /* Use 4K pages, since that's common and the minimum the HC supports */
    xhci->page_shift = 12;
    xhci->page_size = 1 << xhci->page_shift;
    xhci_dbg(xhci, "HCD page size set to %iK\n", xhci->page_size / 1024);

    /*
     * Program the Number of Device Slots Enabled field in the CONFIG
     * register with the max value of slots the HC can handle.
     */
    val = HCS_MAX_SLOTS(xhci_readl(xhci, &xhci->cap_regs->hcs_params1));
    xhci_dbg(xhci, "// xHC can handle at most %d device slots.\n",
            (unsigned int) val);
    val2 = xhci_readl(xhci, &xhci->op_regs->config_reg);
    val |= (val2 & ~HCS_SLOTS_MASK);
    xhci_dbg(xhci, "// Setting Max device slots reg = 0x%x.\n",
            (unsigned int) val);
    xhci_writel(xhci, val, &xhci->op_regs->config_reg);

    /*
     * Section 5.4.8 - doorbell array must be
     * "physically contiguous and 64-byte (cache line) aligned".
     */
    //xhci->dcbaa = dma_alloc_coherent(dev,
    //  sizeof(*xhci->dcbaa), &dma, GFP_ATOMIC);
    xhci->dcbaa = (struct xhci_device_context_array *)xhci_dma_alloc(sizeof(*xhci->dcbaa));
    dma = ((dma_addr_t)xhci->dcbaa) & 0x7FFFFFFF;
        
    if (!xhci->dcbaa)
        goto fail;
    x_memset(xhci->dcbaa, 0, sizeof *(xhci->dcbaa));
    xhci->dcbaa->dma = dma;
    xhci_dbg(xhci, "// Device context base array address = 0x%llx (DMA), %p (virt)\n",
            (unsigned long long)xhci->dcbaa->dma, xhci->dcbaa);
    xhci_write_64(xhci, dma, &xhci->op_regs->dcbaa_ptr);

    /* Set up the command ring to have one segments for now. */
    xhci->cmd_ring = xhci_ring_alloc(xhci, 1, true, false);
    if (!xhci->cmd_ring)
        goto fail;
    xhci_dbg(xhci, "Allocated command ring at %p\n", xhci->cmd_ring);
    xhci_dbg(xhci, "First segment DMA is 0x%llx\n",
            (unsigned long long)xhci->cmd_ring->first_seg->dma);

    /* Set the address in the Command Ring Control register */
    val_64 = xhci_read_64(xhci, &xhci->op_regs->cmd_ring);
    val_64 = (val_64 & (u64) CMD_RING_RSVD_BITS) |
        (xhci->cmd_ring->first_seg->dma & (u64) ~CMD_RING_RSVD_BITS) |
        xhci->cmd_ring->cycle_state;
    xhci_dbg(xhci, "// Setting command ring address to 0x%x\n", val);
    xhci_write_64(xhci, val_64, &xhci->op_regs->cmd_ring);

    val = xhci_readl(xhci, &xhci->cap_regs->db_off);
    val &= DBOFF_MASK;
    xhci_dbg(xhci, "// Doorbell array is located at offset 0x%x"
            " from cap regs base addr\n", val);
    xhci->dba = (struct xhci_doorbell_array*)((u32) xhci->cap_regs + val);
    /* Set ir_set to interrupt register set 0 */
    xhci->ir_set = &xhci->run_regs->ir_set[0];

    /*
     * Event ring setup: Allocate a normal ring, but also setup
     * the event ring segment table (ERST).  Section 4.9.3.
     */
    xhci_dbg(xhci, "// Allocating event ring\n");
    xhci->event_ring = xhci_ring_alloc(xhci, ERST_NUM_SEGS, false, false);
    if (!xhci->event_ring)
        goto fail;

    //xhci->erst.entries = dma_alloc_coherent(dev,
    //  sizeof(struct xhci_erst_entry)*ERST_NUM_SEGS, &dma, GFP_ATOMIC);
    xhci->erst.entries = (struct xhci_erst_entry*)xhci_dma_alloc(sizeof(struct xhci_erst_entry)*ERST_NUM_SEGS);
    dma = ((dma_addr_t)xhci->erst.entries) & 0x7FFFFFFF;
    
    if (!xhci->erst.entries)
        goto fail;
    xhci_dbg(xhci, "// Allocated event ring segment table at 0x%llx\n",
            (unsigned long long)dma);

    x_memset(xhci->erst.entries, 0, sizeof(struct xhci_erst_entry)*ERST_NUM_SEGS);
    xhci->erst.num_entries = ERST_NUM_SEGS;
    xhci->erst.erst_dma_addr = dma;
    xhci_dbg(xhci, "Set ERST to 0; private num segs = %i, virt addr = %p, dma addr = 0x%llx\n",
            xhci->erst.num_entries,
            xhci->erst.entries,
            (unsigned long long)xhci->erst.erst_dma_addr);

    /* set ring base address and size for each segment table entry */
    for (val = 0, seg = xhci->event_ring->first_seg; val < ERST_NUM_SEGS; val++) {
        struct xhci_erst_entry *entry = &xhci->erst.entries[val];
        entry->seg_addr = cpu_to_le64(seg->dma);
        entry->seg_size = cpu_to_le32(TRBS_PER_SEGMENT);
        entry->rsvd = 0;
        seg = seg->next;
    }

    /* set ERST count with the number of entries in the segment table */
    val = xhci_readl(xhci, &xhci->ir_set->erst_size);
    val &= ERST_SIZE_MASK;
    val |= ERST_NUM_SEGS;
    xhci_dbg(xhci, "// Write ERST size = %i to ir_set 0 (some bits preserved)\n", val);
    xhci_writel(xhci, val, &xhci->ir_set->erst_size);

    xhci_dbg(xhci, "// Set ERST entries to point to event ring.\n");
    /* set the segment table base address */
    xhci_dbg(xhci, "// Set ERST base address for ir_set 0 = 0x%llx\n",
            (unsigned long long)xhci->erst.erst_dma_addr);
    val_64 = xhci_read_64(xhci, &xhci->ir_set->erst_base);
    val_64 &= ERST_PTR_MASK;
    val_64 |= (xhci->erst.erst_dma_addr & (u64) ~ERST_PTR_MASK);
    xhci_write_64(xhci, val_64, &xhci->ir_set->erst_base);

    /* Set the event ring dequeue address */
    xhci_set_hc_event_deq(xhci);
    xhci_dbg(xhci, "Wrote ERST address to ir_set 0.\n");
    //xhci_print_ir_set(xhci, 0);

    /*
     * XXX: Might need to set the Interrupter Moderation Register to
     * something other than the default (~1ms minimum between interrupts).
     * See section 5.5.1.2.
     */
    VERIFY(x_sema_create(&xhci->addr_dev, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
    for (i = 0; i < MAX_HC_SLOTS; ++i)
        xhci->devs[i] = NULL;
    for (i = 0; i < USB_MAXCHILDREN; ++i) {
        xhci->bus_state[0].resume_done[i] = 0;
        xhci->bus_state[1].resume_done[i] = 0;
    }

//  if (scratchpad_alloc(xhci, flags))
//      goto fail;
    if (xhci_setup_port_arrays(xhci))
        goto fail;
    return 0;

fail:
    xhci_warn(xhci, "Couldn't initialize memory\n");
    //xhci_mem_cleanup(xhci);
    ASSERT(0);
    return -1;
}

void xhci_ring_ep_doorbell(struct xhci_hcd *xhci, unsigned int slot_id,
        unsigned int ep_index, unsigned int stream_id)
{
    __le32 __iomem *db_addr = &xhci->dba->doorbell[slot_id];
    struct xhci_virt_ep *ep = &xhci->devs[slot_id]->eps[ep_index];
    unsigned int ep_state = ep->ep_state;

    /* Don't ring the doorbell for this endpoint if there are pending
     * cancellations because we don't want to interrupt processing.
     * We don't want to restart any stream rings if there's a set dequeue
     * pointer command pending because the device can choose to start any
     * stream once the endpoint is on the HW schedule.
     * FIXME - check all the stream rings for pending cancellations.
     */
    if ((ep_state & EP_HALT_PENDING) || (ep_state & SET_DEQ_PENDING) ||
        (ep_state & EP_HALTED))
        return;
    xhci_writel(xhci, DB_VALUE(ep_index, stream_id), db_addr);
    /* The CPU has better things to do at this point than wait for a
     * write-posting flush.  It'll get there soon enough.
     */
}

static void giveback_first_trb(struct xhci_hcd *xhci, int slot_id,
        unsigned int ep_index, unsigned int stream_id, int start_cycle,
        struct xhci_generic_trb *start_trb)
{
    /*
     * Pass all the TRBs to the hardware at once and make sure this write
     * isn't reordered.
     */
    wmb();
    if (start_cycle)
        start_trb->field[3] |= cpu_to_le32(start_cycle);
    else
        start_trb->field[3] &= cpu_to_le32(~TRB_CYCLE);
    xhci_ring_ep_doorbell(xhci, slot_id, ep_index, stream_id);
}

int xhci_queue_slot_control(struct xhci_hcd *xhci, u32 trb_type, u32 slot_id)
{
    return queue_command(xhci, 0, 0, 0,
            TRB_TYPE(trb_type) | SLOT_ID_FOR_TRB(slot_id), false);
}

/* Queue an address device command TRB */
int xhci_queue_address_device(struct xhci_hcd *xhci, dma_addr_t in_ctx_ptr,
        u32 slot_id)
{
    return queue_command(xhci, lower_32_bits(in_ctx_ptr),
            upper_32_bits(in_ctx_ptr), 0,
            TRB_TYPE(TRB_ADDR_DEV) | SLOT_ID_FOR_TRB(slot_id),
            false);
}

int xhci_queue_configure_endpoint(struct xhci_hcd *xhci, dma_addr_t in_ctx_ptr,
        u32 slot_id, bool command_must_succeed)
{
    return queue_command(xhci, lower_32_bits(in_ctx_ptr),
            upper_32_bits(in_ctx_ptr), 0,
            TRB_TYPE(TRB_CONFIG_EP) | SLOT_ID_FOR_TRB(slot_id),
            command_must_succeed);
}

struct xhci_ep_ctx *xhci_get_ep_ctx(struct xhci_hcd *xhci,
                    struct xhci_container_ctx *ctx,
                    unsigned int ep_index)
{
    /* increment ep index by offset of start of ep ctx array */
    ep_index++;
    if (ctx->type == XHCI_CTX_TYPE_INPUT)
        ep_index++;

    return (struct xhci_ep_ctx *)
        (ctx->bytes + (ep_index * CTX_SIZE(xhci->hcc_params)));
}

static int prepare_transfer(struct xhci_hcd *xhci,
        struct xhci_virt_device *xdev,
        unsigned int ep_index,
        unsigned int stream_id,
        unsigned int num_trbs,
        struct urb *urb,
        unsigned int td_index,
        bool isoc)
{
    int ret;
    struct xhci_td  *td;
    struct xhci_ring *ep_ring;
    struct xhci_ep_ctx *ep_ctx = xhci_get_ep_ctx(xhci, xdev->out_ctx, ep_index);

    ASSERT(td_index == 0);

    //ep_ring = xhci_stream_id_to_ring(xdev, ep_index, stream_id);
    ep_ring = xdev->eps[ep_index].ring;
    if (!ep_ring) {
        xhci_dbg(xhci, "Can't prepare ring for bad stream ID %u\n", stream_id);
        return -1;
    }

    ret = prepare_ring(xhci, ep_ring, le32_to_cpu(ep_ctx->ep_info) & EP_STATE_MASK,
                       num_trbs, isoc);
    if (ret)
        return ret;

    td = &(urb->td);

    INIT_LIST_HEAD(&td->td_list);
    INIT_LIST_HEAD(&td->cancelled_td_list);
/* eddie
    if (td_index == 0) {
        ret = usb_hcd_link_urb_to_ep(bus_to_hcd(urb->dev->bus), urb);
        if (unlikely(ret)){
            xhci_urb_free_priv(xhci, urb_priv);
            urb->hcpriv = NULL;
            return ret;
        }
    }
    // eddie
    //td->urb = urb;
*/  
    /* Add this TD to the tail of the endpoint ring's TD list */
    list_add_tail(&td->td_list, &ep_ring->td_list);
    //Printf("Add td_list\n");
    td->start_seg = ep_ring->enq_seg;
    td->first_trb = ep_ring->enqueue;

    return 0;
}


/* The following flags are used internally by usbcore and HCDs */
static inline int usb_urb_dir_in(struct urb *urb)
{
    return (urb->transfer_flags & URB_DIR_MASK) == URB_DIR_IN;
}


static u32 xhci_td_remainder(unsigned int td_transfer_size, unsigned int td_running_total
        , unsigned int maxp, unsigned trb_buffer_length)
{
        u32 max = 31;
        int remainder, td_packet_count, packet_transferred;
    
        //0 for the last TRB
        //FIXME: need to workaround if there is ZLP in this TD
        if (td_running_total + trb_buffer_length == td_transfer_size)
            return 0;
    
        //FIXME: need to take care of high-bandwidth (MAX_ESIT)
        packet_transferred = (td_running_total /*+ trb_buffer_length*/) / maxp;
        td_packet_count = DIV_ROUND_UP(td_transfer_size, maxp);
        remainder = td_packet_count - packet_transferred;
        
        if (remainder > max)
            return max << 17;
        else
            return remainder << 17;
}

/* Caller must have locked xhci->lock */
int xhci_queue_ctrl_tx(struct xhci_hcd *xhci, struct urb *urb, int slot_id, unsigned int ep_index)
{
    struct xhci_ring *ep_ring;
    int num_trbs;
    int ret;
    struct devrequest *setup;
    struct xhci_generic_trb *start_trb;
    int start_cycle;
    u32 field, length_field;
    struct xhci_td *td; 

    ep_ring = xhci->devs[slot_id]->eps[ep_index].ring;
    if (!ep_ring)
        return -1;

    /*
     * Need to copy setup packet into setup TRB, so we can't use the setup
     * DMA address.
     */
    if (!urb->setup_packet)
        return -1;

    /* 1 TRB for setup, 1 for status */
    num_trbs = 2;
    /*
     * Don't need to check if we need additional event data and normal TRBs,
     * since data in control transfers will never get bigger than 16MB
     * XXX: can we get a buffer that crosses 64KB boundaries?
     */
    if (urb->transfer_buffer_length > 0)
        num_trbs++;
    ret = prepare_transfer(xhci, xhci->devs[slot_id], ep_index, urb->stream_id,
            num_trbs, urb, 0, false);
    if (ret < 0)
        return ret;

    td = &(urb->td);

    /*
     * Don't give the first TRB to the hardware (by toggling the cycle bit)
     * until we've finished creating all the other TRBs.  The ring's cycle
     * state may change as we enqueue the other TRBs, so save it too.
     */
    start_trb = &ep_ring->enqueue->generic;
    start_cycle = ep_ring->cycle_state;

    /* Queue setup TRB - see section 6.4.1.2.1 */
    /* FIXME better way to translate setup_packet into two u32 fields? */
    setup = (struct devrequest *) urb->setup_packet;
    field = 0;
    field |= TRB_IDT | TRB_TYPE(TRB_SETUP);
    if (start_cycle == 0)
        field |= 0x1;

    /* xHCI 1.0 6.4.1.2.1: Transfer Type field */
    //if (xhci->hci_version == 0x100) {
    if (1) {
        if (urb->transfer_buffer_length > 0) {
            if (setup->requesttype & USB_DIR_IN)
                field |= TRB_TX_TYPE(TRB_DATA_IN);
            else
                field |= TRB_TX_TYPE(TRB_DATA_OUT);
        }
    }

    queue_trb(xhci, ep_ring, false, true, false,
          setup->requesttype | setup->request << 8 | le16_to_cpu(setup->value) << 16,
          le16_to_cpu(setup->index) | le16_to_cpu(setup->length) << 16,
          TRB_LEN(8) | TRB_INTR_TARGET(0),
          /* Immediate data in pointer */
          field);

    /* If there's data, queue data TRBs */
    /* Only set interrupt on short packet for IN endpoints */
    if (usb_urb_dir_in(urb))
        field = TRB_ISP | TRB_TYPE(TRB_DATA);
    else
        field = TRB_TYPE(TRB_DATA);

    //CC: MTK style, no scatter-gather for control transfer
    length_field = TRB_LEN(urb->transfer_buffer_length) | 0 | TRB_INTR_TARGET(0);   
    if (urb->transfer_buffer_length > 0) {
        if (setup->requesttype & USB_DIR_IN)
            field |= TRB_DIR_IN;
        queue_trb(xhci, ep_ring, false, true, false,
                lower_32_bits(urb->transfer_dma),
                upper_32_bits(urb->transfer_dma),
                length_field,
                field | ep_ring->cycle_state);
    }
    /* Save the DMA address of the last TRB in the TD */
    td->last_trb = ep_ring->enqueue;

    /* Queue status TRB - see Table 7 and sections 4.11.2.2 and 6.4.1.2.3 */
    /* If the device sent data, the status stage is an OUT transfer */
    if (urb->transfer_buffer_length > 0 && setup->requesttype & USB_DIR_IN)
        field = 0;
    else
        field = TRB_DIR_IN;
    queue_trb(xhci, ep_ring, false, false, false,
            0,
            0,
            TRB_INTR_TARGET(0),
            /* Event on completion */
            field | TRB_IOC | TRB_TYPE(TRB_STATUS) | ep_ring->cycle_state);

    giveback_first_trb(xhci, slot_id, ep_index, 0,
            start_cycle, start_trb);
    return 0;
}


/* This is very similar to what ehci-q.c qtd_fill() does */
int xhci_queue_bulk_tx(struct xhci_hcd *xhci, 
        struct urb *urb, int slot_id, unsigned int ep_index)
{
    struct xhci_ring *ep_ring;
    struct xhci_td *td;
    int num_trbs;
    struct xhci_generic_trb *start_trb;
    bool first_trb;
    bool more_trbs_coming;
    int start_cycle;
    u32 field, length_field;

    int running_total, trb_buff_len, ret;
    //unsigned int total_packet_count;
    u64 addr;
    int max_packet;

    //ep_ring = xhci_urb_to_transfer_ring(xhci, urb);
    ep_ring = xhci->devs[slot_id]->eps[ep_index].ring;
    if (!ep_ring)
        return -1;

    num_trbs = 0;
    /* How much data is (potentially) left before the 64KB boundary? */
    running_total = TRB_MAX_BUFF_SIZE -
        (urb->transfer_dma & (TRB_MAX_BUFF_SIZE - 1));
    running_total &= TRB_MAX_BUFF_SIZE - 1;

    /* If there's some data on this 64KB chunk, or we have to send a
     * zero-length transfer, we need at least one TRB
     */
    if (running_total != 0 || urb->transfer_buffer_length == 0)
        num_trbs++;
    /* How many more 64KB chunks to transfer, how many more TRBs? */
    while (running_total < urb->transfer_buffer_length) {
        num_trbs++;
        running_total += TRB_MAX_BUFF_SIZE;
    }
    /* FIXME: this doesn't deal with URB_ZERO_PACKET - need one more */

    if((urb->transfer_flags & URB_DIR_MASK) == URB_DIR_IN)
    {
        max_packet = urb->dev->epmaxpacketin[ep_index>>1];
    }
    else
    {
        max_packet = urb->dev->epmaxpacketout[(ep_index+1)>>1];
    }
    ASSERT(max_packet != 0);

    if((urb->transfer_flags & URB_ZERO_PACKET) 
        && ((urb->transfer_buffer_length % max_packet) == 0)){
        num_trbs++;
    }

    ret = prepare_transfer(xhci, xhci->devs[slot_id],
            ep_index, urb->stream_id, num_trbs, urb, 0, false);
    if (ret < 0)
        return ret;

    // eddie
    td = &(urb->td);

    /*
     * Don't give the first TRB to the hardware (by toggling the cycle bit)
     * until we've finished creating all the other TRBs.  The ring's cycle
     * state may change as we enqueue the other TRBs, so save it too.
     */
    start_trb = &ep_ring->enqueue->generic;
    start_cycle = ep_ring->cycle_state;

    running_total = 0;
    // eddie
    //total_packet_count = roundup(urb->transfer_buffer_length,
    //      le16_to_cpu(urb->ep->desc.wMaxPacketSize));
    /* How much data is in the first TRB? */
    addr = (u64) urb->transfer_dma;
    trb_buff_len = TRB_MAX_BUFF_SIZE -
        (urb->transfer_dma & (TRB_MAX_BUFF_SIZE - 1));
    if (trb_buff_len > urb->transfer_buffer_length)
        trb_buff_len = urb->transfer_buffer_length;

    first_trb = true;

    /* Queue the first TRB, even if it's zero-length */
    do {
        u32 remainder = 0;
        field = 0;

        /* Don't change the cycle bit of the first TRB until later */
        if (first_trb) {
            first_trb = false;
            if (start_cycle == 0)
                field |= 0x1;
        } else
            field |= ep_ring->cycle_state;

        /* Chain all the TRBs together; clear the chain bit in the last
         * TRB to indicate it's the last TRB in the chain.
         */
        if (num_trbs > 1) {
            field |= TRB_CHAIN;
        } else {
            /* FIXME - add check for ZERO_PACKET flag before this */
            td->last_trb = ep_ring->enqueue;
            field |= TRB_IOC;
        }

        /* Only set interrupt on short packet for IN endpoints */
        if (usb_urb_dir_in(urb))
            field |= TRB_ISP;

        remainder = xhci_td_remainder(urb->transfer_buffer_length, running_total, max_packet, trb_buff_len);
        
        length_field = TRB_LEN(trb_buff_len) |
            remainder |
            TRB_INTR_TARGET(0);

        if (num_trbs > 1)
            more_trbs_coming = true;
        else
            more_trbs_coming = false;
        queue_trb(xhci, ep_ring, false, more_trbs_coming, false,
                lower_32_bits(addr),
                upper_32_bits(addr),
                length_field,
                field | TRB_TYPE(TRB_NORMAL));
        --num_trbs;
        running_total += trb_buff_len;

        /* Calculate length for next transfer */
        addr += trb_buff_len;
        trb_buff_len = urb->transfer_buffer_length - running_total;
        if (trb_buff_len > TRB_MAX_BUFF_SIZE)
            trb_buff_len = TRB_MAX_BUFF_SIZE;
    } while (running_total < urb->transfer_buffer_length);

    //check_trb_math(urb, num_trbs, running_total);
    giveback_first_trb(xhci, slot_id, ep_index, urb->stream_id,
            start_cycle, start_trb);
    return 0;
}


/* Issue a configure endpoint command or evaluate context command
 * and wait for it to finish.
 */
static int xhci_configure_endpoint(struct xhci_hcd *xhci,
        struct usb_device *udev, bool ctx_change, bool must_succeed)
{
    int ret = 0;
    struct xhci_container_ctx *in_ctx;
    struct xhci_virt_device *virt_dev;

    // edddie
    UNUSED(udev);

    xhci_lock();
    virt_dev = xhci->devs[udev->slot_id];
    in_ctx = virt_dev->in_ctx;

    if (!ctx_change)
    {
        ret = xhci_queue_configure_endpoint(xhci, in_ctx->dma,
                udev->slot_id, must_succeed);
    }
    else
    {
        ASSERT(0);
        //ret = xhci_queue_evaluate_context(xhci, in_ctx->dma,
        //      udev->slot_id);
    }
    if (ret < 0) {
        xhci_unlock();
        xhci_dbg(xhci, "FIXME allocate a new ring segment\n");
        return -1;
    }
    xhci_ring_cmd_db(xhci);
    xhci_unlock();

    if(OSR_OK != x_sema_lock_timeout(virt_dev->cmd_completion, 5000))
    {
        return -1;
    }

    if (!ctx_change)
    {
        ret = -1;
        if(virt_dev->cmd_status == COMP_SUCCESS)
        {
            ret = 0;
        }
    }
    else
    {
        //ret = xhci_evaluate_context_result(xhci, udev, cmd_status);
        ASSERT(0);
    }

    return ret;
}


static void xhci_zero_in_ctx(struct xhci_hcd *xhci, struct xhci_virt_device *virt_dev)
{
    struct xhci_input_control_ctx *ctrl_ctx;
    struct xhci_ep_ctx *ep_ctx;
    struct xhci_slot_ctx *slot_ctx;
    int i;

    /* When a device's add flag and drop flag are zero, any subsequent
     * configure endpoint command will leave that endpoint's state
     * untouched.  Make sure we don't leave any old state in the input
     * endpoint contexts.
     */
    ctrl_ctx = xhci_get_input_control_ctx(xhci, virt_dev->in_ctx);
    ctrl_ctx->drop_flags = 0;
    ctrl_ctx->add_flags = 0;
    slot_ctx = xhci_get_slot_ctx(xhci, virt_dev->in_ctx);
    slot_ctx->dev_info &= cpu_to_le32(~LAST_CTX_MASK);
    /* Endpoint 0 is always valid */
    slot_ctx->dev_info |= cpu_to_le32(LAST_CTX(1));
    for (i = 1; i < 31; ++i) {
        ep_ctx = xhci_get_ep_ctx(xhci, virt_dev->in_ctx, i);
        ep_ctx->ep_info = 0;
        ep_ctx->ep_info2 = 0;
        ep_ctx->deq = 0;
        ep_ctx->tx_info = 0;
    }
}


int xhci_check_bandwidth(struct usb_device *udev)
{
    int i;
    int ret = 0;
    struct xhci_hcd *xhci;
    struct xhci_virt_device *virt_dev;
    struct xhci_input_control_ctx *ctrl_ctx;
    //struct xhci_slot_ctx *slot_ctx;

    //ret = xhci_check_args(hcd, udev, NULL, 0, true, __func__);
    //if (ret <= 0)
    //  return ret;
    xhci = &_mtk_xhci;
    if (xhci->xhc_state & XHCI_STATE_DYING)
        return -1;

    xhci_dbg(xhci, "%s called for udev %p\n", __func__, udev);
    virt_dev = xhci->devs[udev->slot_id];

    /* See section 4.6.6 - A0 = 1; A1 = D0 = D1 = 0 */
    ctrl_ctx = xhci_get_input_control_ctx(xhci, virt_dev->in_ctx);
    ctrl_ctx->add_flags |= cpu_to_le32(SLOT_FLAG);
    ctrl_ctx->add_flags &= cpu_to_le32(~EP0_FLAG);
    ctrl_ctx->drop_flags &= cpu_to_le32(~(SLOT_FLAG | EP0_FLAG));

    /* Don't issue the command if there's no endpoints to update. */
    if ((ctrl_ctx->add_flags == cpu_to_le32(SLOT_FLAG)) && (ctrl_ctx->drop_flags == 0))
        return 0;

    xhci_dbg(xhci, "New Input Control Context:\n");
    //slot_ctx = xhci_get_slot_ctx(xhci, virt_dev->in_ctx);

    ret = xhci_configure_endpoint(xhci, udev, false, false);
    if (ret) {
        /* Callee should call reset_bandwidth() */
        return ret;
    }

    /* Free any rings that were dropped, but not changed. */
    for (i = 1; i < 31; ++i) {
        if ((le32_to_cpu(ctrl_ctx->drop_flags) & (1 << (i + 1))) &&
            !(le32_to_cpu(ctrl_ctx->add_flags) & (1 << (i + 1))))
        {
            //xhci_free_or_cache_endpoint_ring(xhci, virt_dev, i);
            xhci_ring_free(xhci, virt_dev->eps[i].ring);
            virt_dev->eps[i].ring = NULL;
        }
    }
    xhci_zero_in_ctx(xhci, virt_dev);
    /*
     * Install any rings for completely new endpoints or changed endpoints,
     * and free or cache any old rings from changed endpoints.
     */
#if 0    
    for (i = 1; i < 31; ++i) {
        if (!virt_dev->eps[i].new_ring)
            continue;
        /* Only cache or free the old ring if it exists.
         * It may not if this is the first add of an endpoint.
         */
        if (virt_dev->eps[i].ring) {
            xhci_free_or_cache_endpoint_ring(xhci, virt_dev, i);
        }
        virt_dev->eps[i].ring = virt_dev->eps[i].new_ring;
        virt_dev->eps[i].new_ring = NULL;
    }
#endif

    return ret;
}

static unsigned int xhci_parse_exponent_interval(struct usb_device *udev,
        struct usb_endpoint_descriptor *epdesc)
{
    unsigned int interval;

    interval = clamp_val(epdesc->bInterval, 1, 16) - 1;

    if (udev->speed == USB_SPEED_FULL) {
        /*
         * Full speed isoc endpoints specify interval in frames,
         * not microframes. We are using microframes everywhere,
         * so adjust accordingly.
         */
        interval += 3;  /* 1 frame = 2^3 uframes */
    }

    return interval;
}


/*
 * Convert bInterval expressed in frames (in 1-255 range) to exponent of
 * microframes, rounded down to nearest power of 2.
 */
static unsigned int xhci_parse_frame_interval(struct usb_device *udev,
        struct usb_endpoint_descriptor *epdesc)
{
    unsigned int interval;

    interval = fls(8 * epdesc->bInterval) - 1;
    interval = clamp_val(interval, 3, 10);

    return interval;
}


static unsigned int xhci_get_endpoint_interval(struct usb_device *udev,
        struct usb_endpoint_descriptor *epdesc)
{
    unsigned int interval = 0;

    switch (udev->speed) {
    case USB_SPEED_HIGH:
        /* Max NAK rate */
        if (usb_endpoint_xfer_control(epdesc) || usb_endpoint_xfer_bulk(epdesc)) {
            interval = epdesc->bInterval;
            break;
        }
        /* Fall through - SS and HS isoc/int have same decoding */

    case USB_SPEED_SUPER:
        if (usb_endpoint_xfer_int(epdesc) ||
            usb_endpoint_xfer_isoc(epdesc)) {
            interval = xhci_parse_exponent_interval(udev, epdesc);
        }
        break;

    case USB_SPEED_FULL:
        if (usb_endpoint_xfer_isoc(epdesc)) {
            interval = xhci_parse_exponent_interval(udev, epdesc);
            break;
        }
        /*
         * Fall through for interrupt endpoint interval decoding
         * since it uses the same rules as low speed interrupt
         * endpoints.
         */

    case USB_SPEED_LOW:
        if (usb_endpoint_xfer_int(epdesc) || usb_endpoint_xfer_isoc(epdesc)) {

            interval = xhci_parse_frame_interval(udev, epdesc);
        }
        break;

    default:
        ASSERT(0);
    }
    return EP_INTERVAL(interval);
}

static u32 xhci_get_endpoint_type(struct usb_device *udev,
        struct usb_endpoint_descriptor *epdesc)
{
    int in;
    u32 type = 0;

    in = usb_endpoint_dir_in(epdesc);
    if (usb_endpoint_xfer_control(epdesc)) {
        type = EP_TYPE(CTRL_EP);
    } else if (usb_endpoint_xfer_bulk(epdesc)) {
        if (in)
            type = EP_TYPE(BULK_IN_EP);
        else
            type = EP_TYPE(BULK_OUT_EP);
    } else if (usb_endpoint_xfer_isoc(epdesc)) {
        if (in)
            type = EP_TYPE(ISOC_IN_EP);
        else
            type = EP_TYPE(ISOC_OUT_EP);
    } else if (usb_endpoint_xfer_int(epdesc)) {
        if (in)
            type = EP_TYPE(INT_IN_EP);
        else
            type = EP_TYPE(INT_OUT_EP);
    } else {
        ASSERT(0);
    }
    return type;
}


static int xhci_endpoint_init(struct xhci_hcd *xhci,
        struct xhci_virt_device *virt_dev,struct usb_device *udev,
        struct usb_endpoint_descriptor* epdesc)
{
    unsigned int ep_index;
    struct xhci_ep_ctx *ep_ctx;
    struct xhci_ring *ep_ring;
    unsigned int max_packet;
    unsigned int max_burst;
    u32 max_esit_payload;

    ep_index = xhci_get_endpoint_index(epdesc);
    ep_ctx = xhci_get_ep_ctx(xhci, virt_dev->in_ctx, ep_index);

    /* Set up the endpoint ring */
    /*
     * Isochronous endpoint ring needs bigger size because one isoc URB
     * carries multiple packets and it will insert multiple tds to the
     * ring.
     * This should be replaced with dynamic ring resizing in the future.
     */
    if (usb_endpoint_xfer_isoc(epdesc))
        virt_dev->eps[ep_index].ring = xhci_ring_alloc(xhci, 8, true, true);
    else
        virt_dev->eps[ep_index].ring = xhci_ring_alloc(xhci, 1, true, false);

    if(!virt_dev->eps[ep_index].ring)
    {
        return -1;
    }
    
    virt_dev->eps[ep_index].skip = false;
    ep_ring = virt_dev->eps[ep_index].ring;
    ep_ctx->deq = cpu_to_le64(ep_ring->first_seg->dma | ep_ring->cycle_state);

    // eddie
//  ep_ctx->ep_info = cpu_to_le32(xhci_get_endpoint_interval(udev, epdesc)
//                    | EP_MULT(xhci_get_endpoint_mult(udev, epdesc)));
    ep_ctx->ep_info = cpu_to_le32(xhci_get_endpoint_interval(udev, epdesc));

    /* FIXME dig Mult and streams info out of ep companion desc */

    /* Allow 3 retries for everything but isoc;
     * CErr shall be set to 0 for Isoch endpoints.
     */
    if (!usb_endpoint_xfer_isoc(epdesc))
        ep_ctx->ep_info2 = cpu_to_le32(ERROR_COUNT(3));
    else
        ep_ctx->ep_info2 = cpu_to_le32(ERROR_COUNT(0));

    ep_ctx->ep_info2 |= cpu_to_le32(xhci_get_endpoint_type(udev, epdesc));

    /* Set the max packet size and max burst */
    switch (udev->speed) {
    case USB_SPEED_SUPER:
        max_packet = le16_to_cpu(epdesc->wMaxPacketSize);
        ep_ctx->ep_info2 |= cpu_to_le32(MAX_PACKET(max_packet));
        /* dig out max burst from ep companion desc */
        // eddie
        //max_packet = ep->ss_ep_comp.bMaxBurst;
        //ep_ctx->ep_info2 |= cpu_to_le32(MAX_BURST(max_packet));
        break;
    case USB_SPEED_HIGH:
        /* bits 11:12 specify the number of additional transaction
         * opportunities per microframe (USB 2.0, section 9.6.6)
         */
        if (usb_endpoint_xfer_isoc(epdesc) || usb_endpoint_xfer_int(epdesc)) {
            max_burst = (le16_to_cpu(epdesc->wMaxPacketSize) & 0x1800) >> 11;
            ep_ctx->ep_info2 |= cpu_to_le32(MAX_BURST(max_burst));
        }
        /* Fall through */
    case USB_SPEED_FULL:
    case USB_SPEED_LOW:
        max_packet = GET_MAX_PACKET(le16_to_cpu(epdesc->wMaxPacketSize));
        ep_ctx->ep_info2 |= cpu_to_le32(MAX_PACKET(max_packet));
        break;
    default:
        ASSERT(0);
    }
    // Only support control and bulk
    max_esit_payload = 0; //xhci_get_max_esit_payload(xhci, udev, ep);
    ep_ctx->tx_info = cpu_to_le32(MAX_ESIT_PAYLOAD_FOR_EP(max_esit_payload));

    /*
     * XXX no idea how to calculate the average TRB buffer length for bulk
     * endpoints, as the driver gives us no clue how big each scatter gather
     * list entry (or buffer) is going to be.
     *
     * For isochronous and interrupt endpoints, we set it to the max
     * available, until we have new API in the USB core to allow drivers to
     * declare how much bandwidth they actually need.
     *
     * Normally, it would be calculated by taking the total of the buffer
     * lengths in the TD and then dividing by the number of TRBs in a TD,
     * including link TRBs, No-op TRBs, and Event data TRBs.  Since we don't
     * use Event Data TRBs, and we don't chain in a link TRB on short
     * transfers, we're basically dividing by 1.
     *
     * xHCI 1.0 specification indicates that the Average TRB Length should
     * be set to 8 for control endpoints.
     */
    if (usb_endpoint_xfer_control(epdesc) && xhci->hci_version == 0x100)
        ep_ctx->tx_info |= cpu_to_le32(AVG_TRB_LENGTH_FOR_EP(8));
    else
        ep_ctx->tx_info |= cpu_to_le32(AVG_TRB_LENGTH_FOR_EP(max_esit_payload));

    /* FIXME Debug endpoint context */
    return 0;
}


int xhci_add_endpoint(struct usb_device *udev, struct usb_endpoint_descriptor* epdesc)
{
    struct xhci_hcd *xhci;
    struct xhci_container_ctx *in_ctx;
    unsigned int ep_index;
    //struct xhci_ep_ctx *in_ep_ctx;;
    struct xhci_slot_ctx *slot_ctx;
    struct xhci_input_control_ctx *ctrl_ctx;
    u32 added_ctxs;
    unsigned int last_ctx;
    //u32 new_add_flags, new_drop_flags, new_slot_info;
    struct xhci_virt_device *virt_dev;

    xhci = &_mtk_xhci;
    if (xhci->xhc_state & XHCI_STATE_DYING)
        return -1;

    added_ctxs = xhci_get_endpoint_flag(epdesc);
    last_ctx = xhci_last_valid_endpoint(added_ctxs);
    if (added_ctxs == SLOT_FLAG || added_ctxs == EP0_FLAG) {
        /* FIXME when we have to issue an evaluate endpoint command to
         * deal with ep0 max packet size changing once we get the
         * descriptors
         */
        xhci_dbg(xhci, "xHCI %s - can't add slot or ep 0 %#x\n", __func__, added_ctxs);
        return 0;
    }

    virt_dev = xhci->devs[udev->slot_id];
    in_ctx = virt_dev->in_ctx;
    //out_ctx = virt_dev->out_ctx;
    ctrl_ctx = xhci_get_input_control_ctx(xhci, in_ctx);
    ep_index = xhci_get_endpoint_index(epdesc);
    //ep_ctx = xhci_get_ep_ctx(xhci, out_ctx, ep_index);

    /* If this endpoint is already in use, and the upper layers are trying
     * to add it again without dropping it, reject the addition.
     */
    if (virt_dev->eps[ep_index].ring && !(le32_to_cpu(ctrl_ctx->drop_flags) &
                xhci_get_endpoint_flag(epdesc))) 
    {
        return -1;
    }

    /* If the HCD has already noted the endpoint is enabled,
     * ignore this request.
     */
    if (le32_to_cpu(ctrl_ctx->add_flags) & xhci_get_endpoint_flag(epdesc)) 
    {
        return 0;
    }

    /*
     * Configuration and alternate setting changes must be done in
     * process context, not interrupt context (or so documenation
     * for usb_set_interface() and usb_set_configuration() claim).
     */
    if (xhci_endpoint_init(xhci, virt_dev, udev, epdesc) < 0) {
//      dev_dbg(&udev->dev, "%s - could not initialize ep %#x\n",
//              __func__, ep->desc.bEndpointAddress);
        return -1;
    }
#if 0
    in_ep_ctx = xhci_get_ep_ctx(xhci, in_ctx, ep_index);

    if(mtk_xhci_scheduler_add_ep(xhci, udev, ep, in_ep_ctx) != SCH_SUCCESS){
        xhci_err(xhci, "[MTK] not enough bandwidth\n");
        return -ENOSPC;
    }
#endif
    ctrl_ctx->add_flags |= cpu_to_le32(added_ctxs);
    //new_add_flags = le32_to_cpu(ctrl_ctx->add_flags);

    /* If xhci_endpoint_disable() was called for this endpoint, but the
     * xHC hasn't been notified yet through the check_bandwidth() call,
     * this re-adds a new state for the endpoint from the new endpoint
     * descriptors.  We must drop and re-add this endpoint, so we leave the
     * drop flags alone.
     */
    //new_drop_flags = le32_to_cpu(ctrl_ctx->drop_flags);

    slot_ctx = xhci_get_slot_ctx(xhci, in_ctx);
    /* Update the last valid endpoint context, if we just added one past */
    if ((le32_to_cpu(slot_ctx->dev_info) & LAST_CTX_MASK) <
        LAST_CTX(last_ctx)) {
        slot_ctx->dev_info &= cpu_to_le32(~LAST_CTX_MASK);
        slot_ctx->dev_info |= cpu_to_le32(LAST_CTX(last_ctx));
    }
    //new_slot_info = le32_to_cpu(slot_ctx->dev_info);

    return 0;
}

static void xhci_endpoint_zero(struct xhci_hcd *xhci,
        struct xhci_virt_device *virt_dev, struct usb_endpoint_descriptor *epdesc)
{
    unsigned int ep_index;
    struct xhci_ep_ctx *ep_ctx;

    ep_index = xhci_get_endpoint_index(epdesc);
    ep_ctx = xhci_get_ep_ctx(xhci, virt_dev->in_ctx, ep_index);

    ep_ctx->ep_info = 0;
    ep_ctx->ep_info2 = 0;
    ep_ctx->deq = 0;
    ep_ctx->tx_info = 0;
    /* Don't free the endpoint ring until the set interface or configuration
     * request succeeds.
     */
}


int xhci_drop_endpoint(struct usb_device *udev, struct usb_endpoint_descriptor* epdesc)
{
    struct xhci_hcd *xhci;
    struct xhci_container_ctx *in_ctx, *out_ctx;
    struct xhci_input_control_ctx *ctrl_ctx;
    struct xhci_slot_ctx *slot_ctx;
    unsigned int last_ctx;
    unsigned int ep_index;
    struct xhci_ep_ctx *ep_ctx;
    u32 drop_flag;
    //u32 new_add_flags, new_drop_flags, new_slot_info;
    //int ret;

    xhci = &_mtk_xhci;
    if (xhci->xhc_state & XHCI_STATE_DYING)
        return -1;

    xhci_dbg(xhci, "%s called for udev %p\n", __func__, udev);
    drop_flag = xhci_get_endpoint_flag(epdesc);
    if (drop_flag == SLOT_FLAG || drop_flag == EP0_FLAG) {
        xhci_dbg(xhci, "xHCI %s - can't drop slot or ep 0 %#x\n",
                __func__, drop_flag);
        return 0;
    }

    in_ctx = xhci->devs[udev->slot_id]->in_ctx;
    out_ctx = xhci->devs[udev->slot_id]->out_ctx;
    ctrl_ctx = xhci_get_input_control_ctx(xhci, in_ctx);
    ep_index = xhci_get_endpoint_index(epdesc);
    ep_ctx = xhci_get_ep_ctx(xhci, out_ctx, ep_index);
    /* If the HC already knows the endpoint is disabled,
     * or the HCD has noted it is disabled, ignore this request
     */
    if ((le32_to_cpu(ep_ctx->ep_info) & EP_STATE_MASK) == EP_STATE_DISABLED ||
        le32_to_cpu(ctrl_ctx->drop_flags) & xhci_get_endpoint_flag(epdesc)) 
    {
        return 0;
    }

    ctrl_ctx->drop_flags |= cpu_to_le32(drop_flag);
    //new_drop_flags = le32_to_cpu(ctrl_ctx->drop_flags);

    ctrl_ctx->add_flags &= cpu_to_le32(~drop_flag);
    //new_add_flags = le32_to_cpu(ctrl_ctx->add_flags);

    last_ctx = xhci_last_valid_endpoint(le32_to_cpu(ctrl_ctx->add_flags));
    slot_ctx = xhci_get_slot_ctx(xhci, in_ctx);
    /* Update the last valid endpoint context, if we deleted the last one */
    if ((le32_to_cpu(slot_ctx->dev_info) & LAST_CTX_MASK) >
        LAST_CTX(last_ctx)) {
        slot_ctx->dev_info &= cpu_to_le32(~LAST_CTX_MASK);
        slot_ctx->dev_info |= cpu_to_le32(LAST_CTX(last_ctx));
    }
    //new_slot_info = le32_to_cpu(slot_ctx->dev_info);

    xhci_endpoint_zero(xhci, xhci->devs[udev->slot_id], epdesc);
    
    return 0;
}


int xhci_alloc_dev(struct usb_device *udev)
{
    int ret, i, j;
    struct xhci_hcd *xhci;

    xhci = &_mtk_xhci;

    for(j=0; j<2000; j+= 50)
    {
        for(i=0; i<RH_PORT_NUM; i++)
        {
            xhci_lock();
            if(rh_port[i].port_status == ENABLED)
            {
                xhci_unlock();
                break;
            }
            xhci_unlock();
        }

        if(i != RH_PORT_NUM)
        {
            break;
        }
        x_thread_delay(50);
    }

    if(i == RH_PORT_NUM)
    {
        return -1;
    }

    udev->speed = rh_port[i].port_speed;
    udev->portnum = rh_port[i].port_id;    

    xhci_lock();
    ret = xhci_queue_slot_control(xhci, TRB_ENABLE_SLOT, 0);
    if (ret) {
        xhci_unlock();
        xhci_dbg(xhci, "FIXME: allocate a command ring segment\n");
        return 0;
    }
    xhci_ring_cmd_db(xhci);
    xhci_unlock();

    if(OSR_OK != x_sema_lock_timeout(xhci->addr_dev, 5000))
    {
        xhci_err(xhci, "Enable slot timeout\n");
        return 0;
    }

    if (!xhci->slot_id) {
        xhci_err(xhci, "Error while assigning device slot ID\n");
        return 0;
    }
    udev->slot_id = xhci->slot_id;

    /* Use GFP_NOIO, since this function can be called from
     * xhci_discover_or_reset_device(), which may be called as part of
     * mass storage driver error handling.
     */
    if (!xhci_alloc_virt_device(xhci, xhci->slot_id, udev)) {
        xhci_warn(xhci, "Could not allocate xHCI USB device data structures\n");
        goto disable_slot;
    }
    //udev->slot_id = xhci->slot_id;
    /* Is this a LS or FS device under a HS hub? */
    /* Hub or peripherial? */
    return 1;

disable_slot:
    /* Disable slot, if we can do it without mem alloc */
    xhci_lock();
    if (!xhci_queue_slot_control(xhci, TRB_DISABLE_SLOT, xhci->slot_id))
        xhci_ring_cmd_db(xhci);
    xhci_unlock();

    return 0;
}


int xhci_init(void)
{
    static bool bInited = false;
    x_os_isr_fct pfnOldIsr;
    struct xhci_hcd *xhci;
    int retval = 0, i;

#ifndef CC_MT5396_USB3    
    unsigned long regval;

//#ifdef CONFIG_USB_MTK_XHCI_HCD
	{
	    vIO32WriteFldAlign(CKGEN_REG_TOP_BACKUP3, 0x0, FLD_RG_USB20_MUX);
	    vIO32WriteFldAlign(CKGEN_REG_TOP_BACKUP3, 0x0, FLD_USBREF_CK_SEL);
	    vIO32WriteFldAlign(0xf0070700, 0x1, Fld(1,0,AC_MSKB0));
	}
//#endif
    regval = readl(0xF000D5AC);
    regval |= 0xF;    
    writel(regval, 0xF000D5AC);

    regval = readl(0xF000D5A8);
    regval &= ~(0x3 << 19);
    regval |= (0x2 << 19);
    writel(regval, 0xF000D5A8);
#endif

    for(i=0; i<RH_PORT_NUM; i++)
    {
        rh_port[i].port_id = i;
        rh_port[i].port_reenabled = 0;
        rh_port[i].port_speed = USB_SPEED_UNKNOWN;
        rh_port[i].port_status = DISCONNECTED;
    }
    g_device_reconnect = 0;

    reinitIP();

    x_memset((void*)&_mtk_xhci, 0, sizeof(struct xhci_hcd));
    xhci = &_mtk_xhci;

    xhci->cap_regs = (struct xhci_cap_regs*)XHC_IO_START;
    xhci->op_regs = (struct xhci_op_regs*)(XHC_IO_START +
            HC_LENGTH(xhci_readl(xhci, &xhci->cap_regs->hc_capbase)));
    xhci->run_regs = (struct xhci_run_regs*)(XHC_IO_START +
            (xhci_readl(xhci, &xhci->cap_regs->run_regs_off) & RTSOFF_MASK));
    /* Cache read-only capability registers */
    xhci->hcs_params1 = xhci_readl(xhci, &xhci->cap_regs->hcs_params1);
    xhci->hcs_params2 = xhci_readl(xhci, &xhci->cap_regs->hcs_params2);
    xhci->hcs_params3 = xhci_readl(xhci, &xhci->cap_regs->hcs_params3);
    xhci->hcc_params = xhci_readl(xhci, &xhci->cap_regs->hc_capbase);
    xhci->hci_version = HC_VERSION(xhci->hcc_params);
    xhci->hcc_params = xhci_readl(xhci, &xhci->cap_regs->hcc_params);

    /* Make sure the HC is halted. */
    retval = xhci_halt(xhci);
    if (retval)
        return -1;

    /* Reset the internal HC memory state and registers. */
    retval = xhci_reset(xhci);
    if (retval)
        return -1;
    
    retval = xhci_mem_init(xhci);   
    retval = xhci_run(xhci);

    if(!bInited)
    {
        if (x_reg_isr(XHC_IRQ, _xHCIIrqHandler, &pfnOldIsr) != OSR_OK)
        {
            return -1;
        }

        VERIFY(x_sema_create(&_hXhci_Sema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
        bInited = true;
    }

    return 0;
}


int submit_int_msg(struct usb_device *dev, unsigned long pipe, void *buffer,
            int transfer_len, int interval)
{
    UNUSED(dev);
    UNUSED(pipe);
    UNUSED(buffer);
    UNUSED(transfer_len);
    UNUSED(interval);
    return 0;
}


int submit_control_msg(struct usb_device *dev, unsigned long pipe, void *buffer,
            int transfer_len,struct devrequest *setup)
{
    struct urb ctrlurb;
    unsigned int ep_index = 0;

    x_memset((void*)&ctrlurb, 0, sizeof(struct urb));

    ctrlurb.dev = dev;
    ctrlurb.setup_packet = (unsigned char*)setup;
    if(usb_pipein(pipe))
    {
        ctrlurb.transfer_flags |= URB_DIR_IN;
        HalFlushInvalidateDCacheMultipleLine((UINT32)buffer, transfer_len);
    }
    else
    {
        ctrlurb.transfer_flags |= URB_DIR_OUT;
        HalFlushDCacheMultipleLine((UINT32)buffer, transfer_len);
    }
    ctrlurb.transfer_buffer = buffer;
    ctrlurb.transfer_buffer_length = transfer_len;
    ctrlurb.transfer_dma = (dma_addr_t)buffer;

    ep_index = usb_pipeendpoint(pipe);

    if(xhci_queue_ctrl_tx(&_mtk_xhci, &ctrlurb, dev->slot_id, ep_index) != 0)
    {
        return -1;
    }

    if(x_sema_lock(_hXhci_Sema, X_SEMA_OPTION_WAIT) != OSR_OK)
    {
        return -1;
    }

    if((GET_COMP_CODE(_u4TrEventField2) == COMP_SUCCESS) || 
       (GET_COMP_CODE(_u4TrEventField2) == COMP_SHORT_TX))
    {
        dev->status = 0;
        dev->act_len = (transfer_len -  (_u4TrEventField2 & 0xFFFFFF));

        if(usb_pipein(pipe))
        {
            HalFlushInvalidateDCacheMultipleLine((UINT32)buffer, transfer_len);
        }
    }

    return 0;
}


int submit_bulk_msg(struct usb_device *dev, unsigned long pipe, void *buffer,int transfer_len)
{
    struct urb bulkurb;
    unsigned int ep_index = 0;

    x_memset((void*)&bulkurb, 0, sizeof(struct urb));

    bulkurb.dev = dev;
    ep_index = usb_pipeendpoint(pipe);
    if(usb_pipein(pipe))
    {
        bulkurb.transfer_flags |= URB_DIR_IN;
        ep_index = (ep_index * 2);
        HalFlushInvalidateDCacheMultipleLine((UINT32)buffer, transfer_len);
    }
    else
    {
        bulkurb.transfer_flags |= URB_DIR_OUT;
        ep_index = (ep_index * 2) - 1;
        HalFlushDCacheMultipleLine((UINT32)buffer, transfer_len);
    }
    bulkurb.transfer_buffer = buffer;
    bulkurb.transfer_buffer_length = transfer_len;
    bulkurb.transfer_dma = (dma_addr_t)buffer;

    if(xhci_queue_bulk_tx(&_mtk_xhci, &bulkurb, dev->slot_id, ep_index) != 0)
    {
        return -1;
    }

    if(x_sema_lock(_hXhci_Sema, X_SEMA_OPTION_WAIT) != OSR_OK)
    {
        return -1;
    }

    if((GET_COMP_CODE(_u4TrEventField2) == COMP_SUCCESS) || 
       (GET_COMP_CODE(_u4TrEventField2) == COMP_SHORT_TX))
    {
        dev->status = 0;
        dev->act_len = (transfer_len -  (_u4TrEventField2 & 0xFFFFFF));

        if(usb_pipein(pipe))
        {
            HalFlushInvalidateDCacheMultipleLine((UINT32)buffer, transfer_len);
        }
    }   

    return 0;
}

