#ifndef X_BIM_8580_H
#define X_BIM_8580_H

//============================================================================
// Include files
//============================================================================
#define HAL_READ8(_reg_)            (*((volatile UINT8*)(_reg_)))
#define HAL_READ16(_reg_)           (*((volatile UINT16*)(_reg_)))
#define HAL_READ32(_reg_)           (*((volatile UINT32*)(_reg_)))

// Macros of register write
#define HAL_WRITE8(_reg_, _val_)    (*((volatile UINT8*)(_reg_)) = (_val_))
#define HAL_WRITE16(_reg_, _val_)   (*((volatile UINT16*)(_reg_)) = (_val_))
#define HAL_WRITE32(_reg_, _val_)   (*((volatile UINT32*)(_reg_)) = (_val_))

// Macros for read/write access
#define HAL_REG8(_reg_)				HAL_READ8((_reg_))
#define HAL_REG16(_reg_)			HAL_READ16((_reg_))
#define HAL_REG32(_reg_)			HAL_READ32((_reg_))


#define IO_READ8(base, offset)                          HAL_READ8((base) + (offset))
#define IO_READ16(base, offset)                         HAL_READ16((base) + (offset))
#define IO_READ32(base, offset)                         HAL_READ32((base) + (offset))

//============================================================================
// Macros for register write
//============================================================================
#define IO_WRITE8(base, offset, value)                  HAL_WRITE8((base) + (offset), (value))
#define IO_WRITE16(base, offset, value)                 HAL_WRITE16((base) + (offset), (value))
#define IO_WRITE32(base, offset, value)                 HAL_WRITE32((base) + (offset), (value))

//============================================================================
// Macros for register read/write access
//============================================================================
#define IO_REG8(base, offset)                           HAL_REG8((base) + (offset))
#define IO_REG16(base, offset)                          HAL_REG16((base) + (offset))
#define IO_REG32(base, offset)                          HAL_REG32((base) + (offset))

//============================================================================
// Macros for register read/write
//============================================================================
#define BIM_READ8(offset)               IO_READ8(BIM_BASE, offset)
#define BIM_READ16(offset)              IO_READ16(BIM_BASE, offset)
#define BIM_READ32(offset)              IO_READ32(BIM_BASE, offset)

#define BIM_WRITE8(offset, value)		IO_WRITE8(BIM_BASE, offset, (value))
#define BIM_WRITE16(offset, value)		IO_WRITE16(BIM_BASE, offset, (value))
#define BIM_WRITE32(offset, value)		IO_WRITE32(BIM_BASE, offset, (value))

#define BIM_REG8(offset)				IO_REG8(BIM_BASE, offset)
#define BIM_REG16(offset)				IO_REG16(BIM_BASE, offset)
#define BIM_REG32(offset)				IO_REG32(BIM_BASE, offset)

//============================================================================
// Macros for memory read/write
//============================================================================
#define WRITEMEM8(Address, Value)		*(volatile UINT8 *)(Address) = Value
#define READMEM8(Address)				*(volatile UINT8 *)(Address)

#define WRITEMEM16(Address, Value)		*(volatile UINT16 *)(Address) = Value
#define READMEM16(Address)				*(volatile UINT16 *)(Address)

#define WRITEMEM32(Address, Value)		*(volatile UINT32 *)(Address) = Value
#define READMEM32(Address)				*(volatile UINT32 *)(Address)

//============================================================================
// BIM Registers
//============================================================================
#define REG_RO_ICE						    0x0000                 //RISC ICE Register
  #define ICE_STRAP                             (1U << 0)          //HDMI JTAG strapping and HDMI JTAG enable register.
  #define BDOPT_ICE                             (1U << 1)          //Bonding decided ICE strap value
  #define DBGACK                                (1U << 2)          //RISC Debug Mode Acknowledge
  #define CPUM                                  (1U << 3)          //In CPU model test mode
  #define SECURE_EN                             (1U << 4)          //Secure Access Enable
  #define DBGACK_1                              (1U << 10)         //RISC2 Debug Mode Acknowledge
#define REG_RO_ADDREN                       0x0004                 //RISC Address Enable Register
#define REG_RO_DRAMA_BA                     0x0010                 //DRAM-A Base Address Register
#define REG_RO_DRAMB_BA                     0x0014                 //DRAM-B Base Address Register
#define REG_RO_IOBASE_BA                    0x0018                 //IO Base Address Register
#define REG_RO_REMAP                        0x001C                 //Remap Register
#define REG_RO_DRAMB_OFF                    0x0020                 //DRAM-B Offset Address Register
#define REG_RO_BUSTIME                      0x0024                 //BUS Access Timing Register

//----------------------------------------------------------------------------
// IRQ/FIQ 
#define REG_IRQST                           0x0030                  //RISC L1 IRQ Status Register
  #define EXT_I                                 (1<<3)              //External Interrupt Status
  #define EXT2_I                                (1<<5)              //External Interrupt 2 Status  
#define REG_IRQEN                           0x0034                  //RISC L1 IRQ Enable Register
  #define EXT_INT_EN_                           (1<<3)              //External Interrupt Enable
#define REG_IRQCL                           0x0038                  //RISC L1 IRQ Clear Register
#define REG_FIQST                           0x003C                  //RISC L1 FIQ Status Register
#define REG_FIQEN                           0x0040                  //RISC L1 FIQ Enable Register
#define REG_FIQCL                           0x0044                  //RISC L1 FIQ Clear Register
                                                                    
#define REG_IRQST2                          0x0048                  //RISC L2 IRQ Status Register
  #define EXT4_I                                (1<<7)              //External Interrupt 4 Status  
#define REG_IRQEN2                          0x004C                  //RISC L2 IRQ Enable Register
#define REG_IRQCL2                          0x0050                  //RISC L2 IRQ Clear Register
#define REG_FIQST2                          0x0054                  //RISC L2 FIQ Status Register
#define REG_FIQEN2                          0x0058                  //RISC L2 FIQ Enable Register
#define REG_FIQCL2                          0x005C                  //RISC L2 FIQ Clear Register
                                                                    
#define REG_IRQST3                          0x0060                  //RISC L3 IRQ Status Register
#define REG_IRQEN3                          0x0064                  //RISC L3 IRQ Enable Register
#define REG_IRQCL3                          0x0068                  //RISC L3 IRQ Clear Register
#define REG_FIQST3                          0x006C                  //RISC L3 FIQ Status Register
#define REG_FIQEN3                          0x0070                  //RISC L3 FIQ Enable Register
#define REG_FIQCL3                          0x0074                  //RISC L3 FIQ Clear Register

#define REG_RW_SEC_MISC2                    0x0088

#define REG_RW_EN_GP1                       0x01A0
#define REG_RW_EN_GP2                       0x01A4
#define REG_RW_EN_GP3                       0x01A8   
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// IRQ Vectors
#include "mt85xx_irqs_vector.h"
//----------------------------------------------------------------------------
// Timer
#define REG_RW_TIMER0_LMT                   0x0148                  //RISC Timer 0 Limit Register
#define REG_RW_TIMER0_COUNT                 0x014C                  //RISC Timer 0 Count Register
#define REG_RW_TIMER1_LMT                   0x0154                  //RISC Timer 1 Limit Register
#define REG_RW_TIMER1_COUNT                 0x0158                  //RISC Timer 1 Count Register
#define REG_RW_TIMER2_LMT                   0x015C                  //RISC Timer 2 Limit Register
#define REG_RW_TIMER2_COUNT                 0x0160                  //RISC Timer 2 Count Register
#define REG_RW_TIMER_CTRL                   0x0164                  //RISC Timer Control Register
  #define TMR0_CNTDWN_EN                        (1U << 0)           //Timer 0 Enable to Count Down
  #define TMR0_AUTOLD_EN                        (1U << 1)           //Timer 0 Auto-Load Enable
  #define TMR1_CNTDWN_EN                        (1U << 8)           //Timer 1 Enable to Count Down
  #define TMR1_AUTOLD_EN                        (1U << 9)           //Timer 1 Auto-Load Enable
  #define TMR2_CNTDWN_EN                        (1U << 16)          //Timer 2 Enable to Count Down
  #define TMR2_AUTOLD_EN                        (1U << 17)          //Timer 2 Auto-Load Enable
  #define TMR_CNTDWN_EN(x)                      (1U << (x*8))
  #define TMR_AUTOLD_EN(x)                      (1U << (1+(x*8)))

//----------------------------------------------------------------------------
// DEBUG
#define REG_RO_DBGDO                        0x007C                  //Debug Data Output Register
#define REG_RO_DBGDOSEL                     0x0080                  //Debug Data Select Register
#define REG_RW_PTM_CTL                      0x0090
  #define HSTP_EN                               (1U << 12)
  #define ETB_SRAM_0000_EN                      (1U << 11) 
  #define ETB_SRAM_2000_EN                      (1U << 10) 
  #define ETB_SRAM_4000_EN                      (1U << 9) 
  #define ETB_SRAM_6000_EN                      (1U << 8)   
                                                
// Address SWAP
#define REG_RW_SWAP_RG0_BGN                 0x0094                  //Swap Region 0 Begin address register
#define REG_RW_SWAP_RG0_END                 0x0098                  //Swap Region 0 End address register
#define REG_RW_SWAP_RG1_BGN                 0x009C                  //Swap Region 1 Begin address register
#define REG_RW_SWAP_RG1_END                 0x00A0                  //Swap Region 1 End address register
#define REG_RW_SWAP_RG2_BGN                 0x01D0                  //Swap Region 2 Begin address register
#define REG_RW_SWAP_RG2_END                 0x01D4                  //Swap Region 2 End address register
#define REG_RW_SWAP_RG3_BGN                 0x01D8                  //Swap Region 3 Begin address register
#define REG_RW_SWAP_RG3_END                 0x01DC                  //Swap Region 3 End address register

#define REG_RW_SWAP_CTRL                    0x00A4                  //RISC Swap Control Register
  #define SWP_RG0_WREN                          (1U << 0)           //Region 0 address swap write enable
  #define SWP_RG0_RDEN                          (1U << 1)           //Region 0 address swap read enable
  #define SWP_RG1_WREN                          (1U << 2)           //Region 1 address swap write enable
  #define SWP_RG1_RDEN                          (1U << 3)           //Region 1 address swap read enable
  #define SWP_RG2_WREN                          (1U << 16)          //Region 2 address swap write enable
  #define SWP_RG2_RDEN                          (1U << 17)          //Region 2 address swap read enable
  #define SWP_RG3_WREN                          (1U << 18)          //Region 3 address swap write enable
  #define SWP_RG3_RDEN                          (1U << 19)          //Region 3 address swap read enable

// MISC Reg
#define EXT_INT_CTRL_                       0x00A8
  #define TRST_SEL0                             (1<<0)              /* TRST of JTAG Selection for the 1st ARM */   
  #define EXT_INT1_TRIGGER_POLARITY_H_          (1<<1)              /* active high or rising edge */
  #define EXT_INT1_LEVEL_TRIGGER_               (1<<2)              /* triggered by level */
  #define EXT_INT4_TRIGGER_POLARITY_H_          (1<<3)              /* active high or rising edge */
  #define EXT_INT4_LEVEL_TRIGGER_               (1<<4)              /* triggered by level */
  #define TRST_SEL0_SW                          (1<<5)              /* TRST of JTAG for the 1st ARM */             
  #define EXT_INT2_TRIGGER_POLARITY_H_          (1<<16)             /* active high or rising edge */
  #define EXT_INT2_LEVEL_TRIGGER_               (1<<17)             /* triggered by level */
  #define EXT_INT1_SAMPLE_EN_                   (1<<20)             /* sample enable */

#define REG_RW_MISC2                        0x00AC                  //MISC2


//----------------------------------------------------------------------------
// General Purpose Register
#define REG_RW_GPRB0                        0x00E0                  //RISC Byte General Purpose Register 0
#define REG_RW_GPRB1                        0x00E4                  //RISC Byte General Purpose Register 1
#define REG_RW_GPRB2                        0x00E8                  //RISC Byte General Purpose Register 2
#define REG_RW_GPRB3                        0x00EC                  //RISC Byte General Purpose Register 3
#define REG_RW_GPRB4                        0x00F0                  //RISC Byte General Purpose Register 4
#define REG_RW_GPRB5                        0x00F4                  //RISC Byte General Purpose Register 5
#define REG_RW_GPRB6                        0x00F8                  //RISC Byte General Purpose Register 6
#define REG_RW_GPRB7                        0x00FC                  //RISC Byte General Purpose Register 7
#define REG_RW_GPRDW0                       0x0100                  //RISC Double Word General Purpose Register 0
#define REG_RW_GPRDW1                       0x0104                  //RISC Double Word General Purpose Register 1
#define REG_RW_LED                          0x010C                  //Seven segment display
#define REG_RW_GPRDW2                       0x0120                  //RISC Double Word General Purpose Register 2
#define REG_RW_GPRDW3                       0x0124                  //RISC Double Word General Purpose Register 3
#define REG_RW_GPRDW4                       0x0110                  //RISC Double Word General Purpose Register 4
#define REG_RW_GPRDW5                       0x0114                  //RISC Double Word General Purpose Register 5
#define REG_RW_GPRDW6                       0x0118                  //RISC Double Word General Purpose Register 6
#define REG_RW_GPRDW7                       0x011C                  //RISC Double Word General Purpose Register 7

//----------------------------------------------------------------------------

#define REG_RO_DSL                          0x0184                  //Delay Select Register
                                                                    
#define REG_RO_DWABRT0ADR                   0x0190                  //Data Write Abort Address 0 Register
#define REG_RO_DWABRT1ADR                   0x0194                  //Data Write Abort Address 1 Register
#define REG_RO_DRABRT0ADR                   0x0198                  //Data Read Abort Address 0 Register
#define REG_RO_DRABRT1ADR                   0x019C                  //Data Read Abort Address 1 Register
#define REG_RO_DWABRT2ADR                   0x0300                  //Data Write Abort Address 2 Register
#define REG_RO_DWABRT3ADR                   0x0304                  //Data Write Abort Address 3 Register
#define REG_RO_DRABRT2ADR                   0x0308                  //Data Read Abort Address 2 Register
#define REG_RO_DRABRT3ADR                   0x030C                  //Data Read Abort Address 3 Register
#define REG_RO_DRABRT4ADR                   0x0310                  //Data Read Abort Address 4 Register
#define REG_RO_DRABRT5ADR                   0x0314                  //Data Read Abort Address 5 Register
#define REG_RO_DRABRT6ADR                   0x0318                  //Data Read Abort Address 6 Register
#define REG_RO_DRABRT7ADR                   0x031C                  //Data Read Abort Address 7 Register

#define REG_RW_HSMPHE                       0x01B4                  //Hardware Semaphore Register
  #define HSMPHE_UART1                          (1U << 0)           //Hardware Semaphore 0
  #define HSMPHE_DC_SMPHE                       (1U << 1)           //Hardware Semaphore 1 (for Dual Communication Semaphore)
  #define HSMPHE_NAND                           (1U << 2)           //NAND semaphore
  #define HSMPHE_LZHS                           (1U << 3)           //LZHS semaphore
  #define HSMPHE_SINFO4                         (1U << 4)           //SINFO4_REG semaphore
  #define HSMPHE_PNG                            (1U << 5)           //PNG semaphore
  #define HSMPHE_PWR_UP_CFG                     (1U << 6)           //POWER UP REG semaphore

#define REG_RW_PCTCFG                       0x01BC                  //PC Trace Configuration
#define REG_RW_PCT_STARADR                  0x01C0
#define REG_RW_PCT_STAWADR                  0x01C4
#define REG_RW_PCT_STPRADR                  0x01C8
#define REG_RW_PCT_STPWADR                  0x01CC

#define REG_RO_L2_MON                       0x01E0
#define REG_RO_L2_INT                       0x01E4

#define REG_RO_LSS_BEG                      0x0204                  //Local Arbiter Begin
#define REG_RO_LSS_LEN_CHA                  0x0208                  
#define REG_RO_LSS_CYC_CHA                  0x020C                  
#define REG_RO_LSS_LEN_CHB                  0x0210                  
#define REG_RO_LSS_CYC_CHB                  0x0214                  
#define REG_RO_BIMIO_BEG                    0x0218                  //Local Arbiter Begin
#define REG_RO_BIMIO_TM0                    0x021C                  //Arbitration Timer
#define REG_RO_BIMIO_LEN                    0x0220                  //Monitored Agent Bandwidth
#define REG_RO_BIMIO_CYC                    0x0224                  //Monitored Cycles
#define REG_RO_SM_CTL                       0x023C          
#define REG_RO_SM_GTM                       0x0240
#define REG_RO_SM_LEN                       0x0244
#define REG_RO_SM_CYC                       0x0248
#define REG_RO_SMRD_CTL                     0x024C
#define REG_RO_SMRD_GTM                     0x0250
#define REG_RO_SMRD_LEN                     0x0254
#define REG_RO_SMRD_CYC                     0x0258
#define REG_RO_IORW_CFG0                    0x0320
#define REG_RO_IORW_CFG1                    0x0324
#define REG_RO_IORW_CFG2                    0x0328
#define REG_RO_IORW_CFG3                    0x032C
#define REG_RO_IORW_CFG4                    0x0330
#define REG_RO_IORW_CFG5                    0x0334
#define REG_RO_IORW_CFG6                    0x0338
#define REG_RO_IORW_CFG7                    0x033C
#define REG_RO_GPRSM_CFG                    0x0340
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ARM2 SHARE INFO REGISTERS

// Must sync with /BDP_Linux/linux/drivers/ata/sata_mt85xx.h
#define REG_RW_SINFO4_REG                   REG_RW_GPRDW7           //Dual Core Share Info Register 4
  #define FB_MEDIA_PRESENT                      (1U << 0)           //[Fastboot]ARM2->ARM1 Detect Media Present
  #define FB_ARM2_ALIVE                         (1U << 1)           //[Fastboot]ARM2->ARM1 Fastboot Function is Alive
  #define FB_ARM1_SATA_INIT                     (1U << 2)           //[Fastboot]ARM1->ARM2 SATA Driver Init
  #define FB_ARM1_LIRC_INIT                     (1U << 3)           //[Fastboot]ARM1->ARM2 LIRC Driver Init
  #define FB_ARM1_EJECT_KEY                     (1U << 4)           //[Fastboot]ARM1->ARM2 Eject Key is Pressed
  #define FB_ARM1_IRRX_INIT                     (1U << 5)           //[Fastboot]ARM1->ARM2 IRRX Driver Init
  #define FB_ARM1_PINMUX_INIT                   (1U << 6)           //[Fastboot]ARM1->ARM2 PINMUX Driver Init
  #define FL_DSP_SHOW_DONE	              (1U << 16)  //[Fastlogo]DSP->ARM1 fastlogo done
  #define FL_OSD_WAIT_RL_FLMEM            (1U << 17)  //[Fastlogo]ARM1->DSP waiting for release fastlogo usd memory

//----------------------------------------------------------------------------
// 64b_TIMER
#define BIM_64b_TIMER_NUM                  1
#define REG_RW_T64b_LO_0                   0x0728
#define REG_RW_T64b_HI_0                   0x072C
#define REG_RW_T64b_EN_0                   0x0730

//----------------------------------------------------------------------------
//============================================================================
// Public functions
//============================================================================
EXTERN BOOL BIM_EnableIrq(UINT32 u4Vector);
EXTERN BOOL BIM_DisableIrq(UINT32 u4Vector);
EXTERN VOID BIM_Workaround(UINT32 u4Vector);
EXTERN BOOL BIM_IsIrqEnabled(UINT32 u4Vector);
EXTERN BOOL BIM_IsIrqPending(UINT32 u4Vector);
EXTERN BOOL BIM_ClearIrq(UINT32 u4Vector);

EXTERN BOOL BIM_EnableFiq(UINT32 u4Vector);
EXTERN BOOL BIM_DisableFiq(UINT32 u4Vector);
EXTERN BOOL BIM_IsFiqEnabled(UINT32 u4Vector);
EXTERN BOOL BIM_IsFiqPending(UINT32 u4Vector);
EXTERN BOOL BIM_ClearFiq(UINT32 u4Vector);

EXTERN BOOL BIM_GETHWSemaphore(UINT32 u4Number, UINT32 u4TimeOut);
EXTERN BOOL BIM_ReleaseHWSemaphore(UINT32 u4Number);

EXTERN VOID BIM_ClearIntFromARM1(VOID);
EXTERN VOID BIM_ClearIntFromARM2(VOID);


EXTERN VOID BIM_RegResetIrKey(UINT32 u4ResetIrM, UINT32 u4ResetIrL);
EXTERN VOID BIM_GetResetIrKey(UINT32* pu4ResetIrM, UINT32* pu4ResetIrL);
EXTERN VOID BIM_WatchDogIrReset(UINT32 u4Val);

#define RESET_MODE_NONE                     0                       // no reset, while loop
#define RESET_MODE_AUTO                     1                       // Automatically Reset
#define RESET_MODE_IR                       2                       // IR Power Key Reset
                                                                    
EXTERN VOID BIM_SetSysHaltResetMode(UINT32 u4Mode);                 // 0: no reset, while loop. 1: Automatically Reset. 2: IR Power Key Reset
EXTERN UINT32 BIM_GetSysHaltResetMode(VOID);

EXTERN BOOL BIM_AddrSwap(
    UINT32 u4Region,       ///< [IN] address swap region: 0~3
    UINT32 u4BeginAddr,    ///< [IN] address swap begin addrss
    UINT32 u4EndAddr,      ///< [IN] address swap end address
    UINT32 u4SwapMode      ///< [IN] address swap mode: 0/1/2, 0: Off
       );

EXTERN UINT32 BIM_WatchCounter(VOID);
EXTERN VOID BIM_WatchDog(UINT32 u4Val);

#endif  // X_BIM_8580_H
