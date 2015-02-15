#include "x_printf.h"

#ifdef CC_AUD_APOLLO_SUPPORT

#define OCD_STATUS       (*((volatile unsigned *)(0x5700)))
#define OCD_INSTR_IN     (*((volatile unsigned *)(0x5704)))
#define OCD_DATA_PI      (*((volatile unsigned *)(0x5708)))
#define OCD_INSTR_EXEC   (*((volatile unsigned *)(0x570c)))
#define OCD_DATA_WE      (*((volatile unsigned *)(0x5710)))
#define OCD_DATA_PO      (*((volatile unsigned *)(0x5714)))
#define OCD_STALL        (((OCD_STATUS)>>1) & 0x1)
#define CURRENT_CID      0x1
#define CMD(x)           (((CURRENT_CID)<<11) | (x))
#define DM_TMP_ADDR      0x10000
#define ocd_printf(x...) do { printf(x); } while(0)

//typedef unsigned int   UINT32;
//typedef unsigned short UINT16;
//typedef unsigned char  UINT8;

const int DBG_MODE_INDX           = 0;
const int DBG_BP_HIT_INDX         = 5;
const int DBG_SWBREAK_INDX        = 8;

const int DBG_DATA_REG_INSTR      = 0x000;
const int DBG_ADDR_REG_INSTR      = 0x001;
const int DBG_INSTR_REG_INSTR     = 0x002;
const int DBG_STATUS_REG_INSTR    = 0x003;
const int DBG_REQUEST_INSTR       = 0x011;
const int DBG_RESUME_INSTR        = 0x012;
const int DBG_RESET_INSTR         = 0x013;
const int DBG_STEP_INSTR          = 0x014;
const int DBG_EXECUTE_INSTR       = 0x015;
const int DBG_BP0_ENABLE_INSTR    = 0x020;
const int DBG_BP0_DISABLE_INSTR   = 0x022;
const int DBG_BP1_ENABLE_INSTR    = 0x024;
const int DBG_BP1_DISABLE_INSTR   = 0x026;
const int DBG_BP2_ENABLE_INSTR    = 0x028;
const int DBG_BP2_DISABLE_INSTR   = 0x02a;
const int DBG_BP3_ENABLE_INSTR    = 0x02c;
const int DBG_BP3_DISABLE_INSTR   = 0x02e;
const int DBG_PMb_LOAD_INSTR      = 0x040;
const int DBG_PMb_STORE_INSTR     = 0x041;
const int DBG_DMb_LOAD_INSTR      = 0x042;
const int DBG_DMb_STORE_INSTR     = 0x043;

#define ocd_pause ocd_request
#define ocd_pmr(x...) ocd_read_pmw(x)
#define ocd_pmw(x...) ocd_write_pmw(x)
#define ocd_dmr(x...) ocd_read_dmb(x)
#define ocd_dmw(x...) ocd_write_dmb(x)

enum reg_idx {
	r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15,
	sr, ipc, isr,
	lf, ls0, le0, lc0, ls1, le1, lc1, ls2, le2, lc2,
	ar0g, ar0h, ar0l, ar1g, ar1h, ar1l,
	srm, b0, b1, m0, m1, l0, l1, o0, o1,
	v0l, v1l, v2l, v3l, v0h, v1h, v2h, v3h,
	rc, pc
};

int ocd_open(void) {
	OCD_STATUS = 1;
	return 0;
}

int ocd_close(void) {
	OCD_STATUS = 0;
	return 0;
}

#if 0
void ocd_iw(UINT32 cmd) {
	OCD_INSTR_IN = cmd;
	OCD_INSTR_EXEC = 1;
	while (OCD_STALL);
	return;
}

void ocd_dw(UINT32 cmd,UINT32 data) {
	OCD_INSTR_IN = cmd;
	OCD_DATA_PI = data;
	OCD_DATA_WE = 1;
	while (OCD_STALL);
	return;
}

UINT32 ocd_dr(UINT32 cmd) {
	OCD_INSTR_IN = cmd;
	while (OCD_STALL);
	return (OCD_DATA_PO);
}

UINT32 ocd_dwr(UINT32 cmd,UINT32 data) {
	OCD_INSTR_IN = cmd;
	OCD_DATA_PI = data;
	OCD_DATA_WE = 1;
	while (OCD_STALL);
	return (OCD_DATA_PO);
}
#else
extern UINT32 AUD_ApolloProbeOcdIW(UINT16 u2Cmd);
extern UINT32 AUD_ApolloProbeOcdDR(UINT16 u2Cmd);
extern UINT32 AUD_ApolloProbeOcdDW(UINT16 u2Cmd, UINT32 u4Data);
extern UINT32 AUD_ApolloProbeOcdDRW(UINT16 u2Cmd, UINT32 u4Data);
#define ocd_iw(x) AUD_ApolloProbeOcdIW(x)
#define ocd_dr(x) AUD_ApolloProbeOcdDR(x)
#define ocd_dw(x,y) AUD_ApolloProbeOcdDW(x,y)
#define ocd_drw(x,y) AUD_ApolloProbeOcdDRW(x,y) 
#endif
inline void ocd_request(void) {
	// assert ocd_req to pause core
	ocd_iw(CMD(DBG_REQUEST_INSTR));
}

inline void ocd_resume(void) {
	ocd_iw(CMD(DBG_RESUME_INSTR));
}

inline void ocd_reset(void) {
	ocd_iw(CMD(DBG_RESET_INSTR));
}

inline void ocd_step(void) {
	ocd_iw(CMD(DBG_STEP_INSTR));
}

inline void ocd_bp0_enable(UINT32 addr) {
	ocd_dw(CMD(DBG_ADDR_REG_INSTR),addr);
	ocd_iw(CMD(DBG_BP0_ENABLE_INSTR));
}

inline void ocd_bp1_enable(UINT32 addr) {
	ocd_dw(CMD(DBG_ADDR_REG_INSTR),addr);
	ocd_iw(CMD(DBG_BP1_ENABLE_INSTR));
}

inline void ocd_bp2_enable(UINT32 addr) {
	ocd_dw(CMD(DBG_ADDR_REG_INSTR),addr);
	ocd_iw(CMD(DBG_BP2_ENABLE_INSTR));
}

inline void ocd_bp3_enable(UINT32 addr) {
	ocd_dw(CMD(DBG_ADDR_REG_INSTR),addr);
	ocd_iw(CMD(DBG_BP3_ENABLE_INSTR));
}

inline void ocd_bp0_disable(void) {
	ocd_iw(CMD(DBG_BP0_DISABLE_INSTR));
}

inline void ocd_bp1_disable(void) {
	ocd_iw(CMD(DBG_BP1_DISABLE_INSTR));
}

inline void ocd_bp2_disable(void) {
	ocd_iw(CMD(DBG_BP2_DISABLE_INSTR));
}

inline void ocd_bp3_disable(void) {
	ocd_iw(CMD(DBG_BP3_DISABLE_INSTR));
}

void ocd_execute(UINT32 insn) {
	ocd_dw(CMD(DBG_INSTR_REG_INSTR),insn);
	ocd_iw(CMD(DBG_EXECUTE_INSTR));
}


UINT32 ocd_read_pmw(UINT32 addr) {
	ocd_dw(CMD(DBG_ADDR_REG_INSTR),addr);
	ocd_iw(CMD(DBG_PMb_LOAD_INSTR));
	return ocd_dr(CMD(DBG_INSTR_REG_INSTR));
}

void ocd_write_pmw(UINT32 addr,UINT32 wdata) {
	ocd_dw(CMD(DBG_ADDR_REG_INSTR),addr);
	ocd_dw(CMD(DBG_INSTR_REG_INSTR),wdata);
	ocd_iw(CMD(DBG_PMb_STORE_INSTR));
}

UINT8 ocd_read_dmb(UINT32 addr) {
	ocd_dw(CMD(DBG_ADDR_REG_INSTR),addr);
	ocd_iw(CMD(DBG_DMb_LOAD_INSTR));
	return (ocd_dr(CMD(DBG_DATA_REG_INSTR)) & 0xff);
}

void ocd_write_dmb(UINT32 addr,UINT8 wdata) {
	ocd_dw(CMD(DBG_ADDR_REG_INSTR),addr);
	ocd_dw(CMD(DBG_DATA_REG_INSTR),wdata);
	ocd_iw(CMD(DBG_DMb_STORE_INSTR));
}

UINT32 ocd_add_sw_break(UINT32 addr) {
	UINT32 insn = ocd_pmr(addr & (~0x3));	
	if (addr & 0x2) { // unaligned
		ocd_pmw(addr & (~0x3), (insn & 0xffff0000) | 0xa003);
	}
	else {
		ocd_pmw(addr & (~0x3), (insn & 0xffff) | 0xa0030000);
	}
	return insn;
}

static UINT32 get_dm_word(UINT32 addr) {
  unsigned byte0,byte1,byte2,byte3;
	ocd_dw(CMD(DBG_ADDR_REG_INSTR),addr);
	ocd_iw(CMD(DBG_DMb_LOAD_INSTR)); byte0 = (ocd_dr(CMD(DBG_DATA_REG_INSTR)) & 0xff);
	ocd_iw(CMD(DBG_DMb_LOAD_INSTR)); byte1 = (ocd_dr(CMD(DBG_DATA_REG_INSTR)) & 0xff);
	ocd_iw(CMD(DBG_DMb_LOAD_INSTR)); byte2 = (ocd_dr(CMD(DBG_DATA_REG_INSTR)) & 0xff);
	ocd_iw(CMD(DBG_DMb_LOAD_INSTR)); byte3 = (ocd_dr(CMD(DBG_DATA_REG_INSTR)) & 0xff);
	return (byte0 | (byte1 << 8) | (byte2 << 16) | (byte3 << 24));
}

static void put_dm_word(UINT32 addr, UINT32 data) {
  unsigned byte0,byte1,byte2,byte3;
	ocd_dw(CMD(DBG_ADDR_REG_INSTR),addr);
	byte0 = (data >>  0) & 0xff;
	byte1 = (data >>  8) & 0xff;
	byte2 = (data >> 16) & 0xff;
	byte3 = (data >> 24) & 0xff;
	ocd_dw(CMD(DBG_DATA_REG_INSTR),byte0); ocd_iw(CMD(DBG_DMb_STORE_INSTR));
	ocd_dw(CMD(DBG_DATA_REG_INSTR),byte1); ocd_iw(CMD(DBG_DMb_STORE_INSTR));
	ocd_dw(CMD(DBG_DATA_REG_INSTR),byte2); ocd_iw(CMD(DBG_DMb_STORE_INSTR));
	ocd_dw(CMD(DBG_DATA_REG_INSTR),byte3); ocd_iw(CMD(DBG_DMb_STORE_INSTR));
}

inline static void set_r0(UINT32 data) {
	ocd_execute(0x0f000000 | ((data >> 16)<<8));
	ocd_execute(0x0d000000 | ((data & 0xffff)<<8));
}

inline static void set_r1(UINT32 data) {
	ocd_execute(0x0f000001 | ((data >> 16)<<8));
	ocd_execute(0x0d000011 | ((data & 0xffff)<<8));
}

inline static void clear_r0(void) {
	ocd_execute(0x00000000);
}

inline UINT32 ocd_read_pc(void) {
	return ocd_dr(CMD(DBG_ADDR_REG_INSTR));
}

void ocd_write_pc(UINT32 addr) {
	// setup PC to R0
	set_r0(addr);
	// jump to r0
	ocd_execute(0x40002030);
	// restore R0
	clear_r0();
}

UINT32 ocd_read_reg(enum reg_idx r) {
	UINT32 dm_tmp_bak = 0;
	UINT32 r1_tmp_bak = 0;
	UINT32 ret = 0;
	int ret_from_dm = 1;
	// setup DM_TMP_ADDR
	set_r0(DM_TMP_ADDR);
	// backup DM_TMP
	dm_tmp_bak = get_dm_word(DM_TMP_ADDR);
	// backup R1
	ocd_execute(0x06700010);
	r1_tmp_bak = get_dm_word(DM_TMP_ADDR);

	switch(r) {
		case r0: ret = 0; ret_from_dm = 0; break;
		case r1: ret = r1_tmp_bak; ret_from_dm = 0; break;
		case r2:   ocd_execute(0x06700020); break;
		case r3:   ocd_execute(0x06700030); break;
		case r4:   ocd_execute(0x06700040); break;
		case r5:   ocd_execute(0x06700050); break;
		case r6:   ocd_execute(0x06700060); break;
		case r7:   ocd_execute(0x06700070); break;
		case r8:   ocd_execute(0x06700080); break;
		case r9:   ocd_execute(0x06700090); break;
		case r10:  ocd_execute(0x067000a0); break;
		case r11:  ocd_execute(0x067000b0); break;
		case r12:  ocd_execute(0x067000c0); break;
		case r13:  ocd_execute(0x067000d0); break;
		case r14:  ocd_execute(0x067000e0); break;
		case r15:  ocd_execute(0x067000f0); break;
		case ar0g: ocd_execute(0x40006680); break;
		case ar0h: ocd_execute(0x40006640); break;
		case ar0l: ocd_execute(0x40006600); break;
		case ar1g: ocd_execute(0x40006740); break;
		case ar1h: ocd_execute(0x40006700); break;
		case ar1l: ocd_execute(0x400066c0); break;
		case sr:   ocd_execute(0x04154001); ocd_execute(0x06700010); break;
		case ipc:  ocd_execute(0x04154011); ocd_execute(0x06700010); break;
		case isr:  ocd_execute(0x04154021); ocd_execute(0x06700010); break;
		case lf:   ocd_execute(0x04154041); ocd_execute(0x06700010); break;
		case ls0:  ocd_execute(0x04154051); ocd_execute(0x06700010); break;
		case le0:  ocd_execute(0x04154061); ocd_execute(0x06700010); break;
		case lc0:  ocd_execute(0x04154071); ocd_execute(0x06700010); break;
		case ls1:  ocd_execute(0x04154081); ocd_execute(0x06700010); break;
		case le1:  ocd_execute(0x04154091); ocd_execute(0x06700010); break;
		case lc1:  ocd_execute(0x041540a1); ocd_execute(0x06700010); break;
		case ls2:  ocd_execute(0x041540b1); ocd_execute(0x06700010); break;
		case le2:  ocd_execute(0x041540c1); ocd_execute(0x06700010); break;
		case lc2:  ocd_execute(0x041540d1); ocd_execute(0x06700010); break;
		case srm:  ocd_execute(0x40006001); ocd_execute(0x06700010); break;
		case b0:   ocd_execute(0x40006101); ocd_execute(0x06700010); break;
		case b1:   ocd_execute(0x40006111); ocd_execute(0x06700010); break;
		case m0:   ocd_execute(0x40006121); ocd_execute(0x06700010); break;
		case m1:   ocd_execute(0x40006131); ocd_execute(0x06700010); break;
		case l0:   ocd_execute(0x40006141); ocd_execute(0x06700010); break;
		case l1:   ocd_execute(0x40006151); ocd_execute(0x06700010); break;
		case o0:   ocd_execute(0x40006161); ocd_execute(0x06700010); break;
		case o1:   ocd_execute(0x40006171); ocd_execute(0x06700010); break;
		case v0l:  ocd_execute(0x20001010); ocd_execute(0x06700010); break;
		case v1l:  ocd_execute(0x20001110); ocd_execute(0x06700010); break;
		case v2l:  ocd_execute(0x20001210); ocd_execute(0x06700010); break;
		case v3l:  ocd_execute(0x20001310); ocd_execute(0x06700010); break;
		case v0h:  ocd_execute(0x20001401); ocd_execute(0x06700010); break;
		case v1h:  ocd_execute(0x20001501); ocd_execute(0x06700010); break;
		case v2h:  ocd_execute(0x20001601); ocd_execute(0x06700010); break;
		case v3h:  ocd_execute(0x20001701); ocd_execute(0x06700010); break;
		case rc: ret = 0; ret_from_dm = 0; break;
		case pc: ret = ocd_dr(CMD(DBG_ADDR_REG_INSTR)); ret_from_dm = 0; break;
		default: ret = 0xdeadbeef; ret_from_dm = 0;
	}
	if (ret_from_dm) {
		ret = get_dm_word(DM_TMP_ADDR);
	}
	// restore R0
	clear_r0();
	// restore R1
	set_r1(r1_tmp_bak);
	// restore DM_TMP
	put_dm_word(DM_TMP_ADDR,dm_tmp_bak);
	return ret;
}

static inline void set_vr(const enum reg_idx r,UINT32 data) {
	UINT32 dm_tmp_bak = 0;
	UINT32 r1_tmp_bak = 0;
	set_r0(DM_TMP_ADDR);
	dm_tmp_bak = get_dm_word(DM_TMP_ADDR);
	ocd_execute(0x06700010);
	r1_tmp_bak = get_dm_word(DM_TMP_ADDR);
	switch(r) {
		case v0l: ocd_execute(0x20001810); set_r0(data); ocd_execute(0x20001c10); break;
		case v1l: ocd_execute(0x20001910); set_r0(data); ocd_execute(0x20001d10); break;
		case v2l: ocd_execute(0x20001a10); set_r0(data); ocd_execute(0x20001e10); break;
		case v3l: ocd_execute(0x20001b10); set_r0(data); ocd_execute(0x20001f10); break;
		case v0h: ocd_execute(0x20001810); set_r1(data); ocd_execute(0x20001c10); break;
		case v1h: ocd_execute(0x20001910); set_r1(data); ocd_execute(0x20001d10); break;
		case v2h: ocd_execute(0x20001a10); set_r1(data); ocd_execute(0x20001e10); break;
		case v3h: ocd_execute(0x20001b10); set_r1(data); ocd_execute(0x20001f10); break;
		default: break;
	}
	clear_r0();
	set_r1(r1_tmp_bak);
	put_dm_word(DM_TMP_ADDR,dm_tmp_bak);
}

void ocd_write_reg(enum reg_idx r, UINT32 data) {
	// write R0
	set_r0(data);
	switch(r) {
		case r0: break;
		case r1:  ocd_execute(0x08000001); break;
		case r2:  ocd_execute(0x08000002); break;
		case r3:  ocd_execute(0x08000003); break;
		case r4:  ocd_execute(0x08000004); break;
		case r5:  ocd_execute(0x08000005); break;
		case r6:  ocd_execute(0x08000006); break;
		case r7:  ocd_execute(0x08000007); break;
		case r8:  ocd_execute(0x08000008); break;
		case r9:  ocd_execute(0x08000009); break;
		case r10: ocd_execute(0x0800000a); break;
		case r11: ocd_execute(0x0800000b); break;
		case r12: ocd_execute(0x0800000c); break;
		case r13: ocd_execute(0x0800000d); break;
		case r14: ocd_execute(0x0800000e); break;
		case r15: ocd_execute(0x0800000f); break;
		case lf : ocd_execute(0x04182004); break;
		case ls0: ocd_execute(0x04182005); break;
		case le0: ocd_execute(0x04182006); break;
		case lc0: ocd_execute(0x04182007); break;
		case ls1: ocd_execute(0x04182008); break;
		case le1: ocd_execute(0x04182009); break;
		case lc1: ocd_execute(0x0418200a); break;
		case ls2: ocd_execute(0x0418200b); break;
		case le2: ocd_execute(0x0418200c); break;
		case lc2: ocd_execute(0x0418200d); break;
		case ipc: ocd_execute(0x04182001); break;
		case isr: ocd_execute(0x04182002); break;
		case sr:  ocd_execute(0x04182000); break;
		case srm: ocd_execute(0x40006010); break;
		case m0:  ocd_execute(0x400060a0); break;
		case m1:  ocd_execute(0x400060b0); break;
		case b0:  ocd_execute(0x40006080); break;
		case b1:  ocd_execute(0x40006090); break;
		case o0:  ocd_execute(0x400060e0); break;
		case o1:  ocd_execute(0x400060f0); break;
		case l0:  ocd_execute(0x400060c0); break;
		case l1:  ocd_execute(0x400060d0); break;
		case v0l:
		case v1l:
		case v2l:
		case v3l:
		case v0h:
		case v1h:
		case v2h:
		case v3h: set_vr(r,data); break;
		case rc: /* do nothing */ break;
		case pc: ocd_execute(0x40002030); break;
		default: break;
	}
	// restore R0
	clear_r0();
}

UINT32 ocd_read_mmr(UINT32 addr) {
	UINT32 dm_tmp_bak = 0;
	UINT32 r1_tmp_bak = 0;
	UINT32 ret = 0;
	set_r0(DM_TMP_ADDR);
	dm_tmp_bak = get_dm_word(DM_TMP_ADDR);
	ocd_execute(0x06700010);
	r1_tmp_bak = get_dm_word(DM_TMP_ADDR);

	set_r0(addr & (~0x3));
	ocd_execute(0x06400010); // lw r1,#0(r0)
	set_r0(DM_TMP_ADDR);
	ocd_execute(0x06700010);
	ret = get_dm_word(DM_TMP_ADDR);

	clear_r0();
	set_r1(r1_tmp_bak);
	put_dm_word(DM_TMP_ADDR,dm_tmp_bak);
	return ret;
}

void ocd_write_mmr(UINT32 addr, UINT32 data) {
	UINT32 dm_tmp_bak = 0;
	UINT32 r1_tmp_bak = 0;
	set_r0(DM_TMP_ADDR);
	dm_tmp_bak = get_dm_word(DM_TMP_ADDR);
	ocd_execute(0x06700010);
	r1_tmp_bak = get_dm_word(DM_TMP_ADDR);

	set_r0(addr & (~0x3));
	set_r1(data);
	ocd_execute(0x06700010);

	clear_r0();
	set_r1(r1_tmp_bak);
	put_dm_word(DM_TMP_ADDR,dm_tmp_bak);
}

UINT32 ocd_read_status(void) {
	UINT32 stat = ocd_dr(CMD(DBG_STATUS_REG_INSTR));
	if (0==(stat & (0x1<<DBG_MODE_INDX))) {
		ocd_printf("apollo is running\n");
		return 0; // apollo is running
	}
	if (0!=(stat & (0x1<<(DBG_MODE_INDX+1+0)))) {
		ocd_printf("hit hardware breakpoint 0\n");
	}
	if (0!=(stat & (0x1<<(DBG_MODE_INDX+1+1)))) {
		ocd_printf("hit hardware breakpoint 1\n");
	}
	if (0!=(stat & (0x1<<(DBG_MODE_INDX+1+2)))) {
		ocd_printf("hit hardware breakpoint 2\n");
	}
	if (0!=(stat & (0x1<<(DBG_MODE_INDX+1+3)))) {
		ocd_printf("hit hardware breakpoint 3\n");
	}
	if (0!=(stat & (0x1<<DBG_BP_HIT_INDX))) {
		ocd_printf("hit hardware breakpoint\n");
	}
	if (0!=(stat & (0x1<<DBG_SWBREAK_INDX))) {
		ocd_printf("hit software breakpoint\n");
	}
	return stat;
}

#endif
