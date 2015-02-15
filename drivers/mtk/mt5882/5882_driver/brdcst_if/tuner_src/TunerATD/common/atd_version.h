#ifndef ATD_VERSION_H
#define ATD_VERSION_H
//d120711_Haibo:ATD0003:[ATSC]A2 Korea issue-expand the CHF to 7M
//d120711_Haibo:ATD0004:[ATSC]modift CVBS swing 0x245 from 0xE2 to 0xE0(NTSC)
//d120723_Haibo:ATD0005:[DVB_ATSC] PQ noise after unplug and then plug RF cable issue(Replace AWGN with CVBS SNRin ATD_NCR_NegBW_Patch to identify weak signal )
//d120814_Haibo:ATD0006:[DVB] [DTV00439591]SECAM L PQ Flicker
//d120828_Haibo:ATD0007:[DVB] [DTV00444495]SECAM L audio SNR issue
//d121015_Haibo:ATD0008:[ATSC][DTV00444664][B0_A2][VIDEO][ATV] White noise is blinking for 1 seconds, When ATV channel is changed, In ATV, In No-Signal,
//d121015_Haibo:ATD0009:[DVB][DTV00454479][A2][A_Demod]temporary noise in the Russia SECAM-DK field stream
//d121018_Haibo:ATD0010:[DVB][DTV00455963][A2][A_demod]broken of ATV picture, when repeat plug-unplug of RF cable
//d121019_Haibo:ATD0011:[DVB][DTV00456729][A2][A-demod]low level of CVBS_Out
#define ATD_DRV_VERSION "ATDT013"
#endif
