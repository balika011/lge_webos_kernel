#ifndef _HOST9777_H_
#define _HOST9777_H_
typedef struct
{
    UINT8                   interlaced ;        //!< 0: Progressive, 1: Interlaced
    UINT8                   posHsync;        //!< 0: negative hor. sync, 1: postoive hor. sync
    UINT8                   posVsync;        //!< 0: negative ver. sync, 1: postoive ver. sync

    UINT32                    pixelFreq;               //!< Pixel frequency in Hz

    UINT16                    vTotal;                  //!< Total number of lines per field (note: not frame)
    UINT16                    vActive;                 //!< Active number of lines per field (note: not frame)
    UINT16                    vSync;                   //!< V Sync width in number of field lines
    UINT16                    vFrontPorch;             //!< V Front Porch width in number of field lines

    UINT16                    hTotal;                  //!< Total number of un-replicated clocks per line
    UINT16                    hActive;                 //!< Active number of un-replicated clocks per line
    UINT16                    hSync;                   //!< H Sync width in number of un-replicated clocks
    UINT16                    hFrontPorch;             //!< H Front Porch width in number of un-replicated clocks
} HostSii9777VideoTiming_t;
typedef struct				
{					
	UINT16				  lineNumEndOfTopBar;	   //!< End of top bar
	UINT16				  lineNumStartOfBottomBar; //!< Start of bottom bar
} VertBarInfo_t;									 //!< Vertical bar info
typedef struct				
{					
	UINT16				  pixNumEndOfLeftBar;	   //!< End of left bar
	UINT16				  pixNumStartOfRightBar;   //!< Start of right bar
} HorizBarInfo_t;    

typedef struct
{
    UINT8                   videoVid;        
    UINT8                   video3Dmode;       
    UINT8                   pictureAR;  
	UINT8 					pixelRepetition;

	UINT8 					colorSpace;
	UINT8 					chromaSample;
	UINT8 					bitDepth;

	UINT8 					ActiveAR;
	UINT8 					pictureScaling;
	UINT8 					scanInfo;
	UINT8 					contentType;

	UINT8 					isITcontent;
	UINT8 					isActiveFmt;

	UINT8 					isVertBarInfoValid;
	UINT8 					isHorizBarInfoValid;
	VertBarInfo_t			vertBarInfo;
	HorizBarInfo_t			horizBarInfo;

   
} HostSii9777VideoFormat_t;

UINT32 siiPlatformInit(void);
UINT32 siiPlatformEndrun(void);
void vHost9777Handle(void);
UINT8 u1HostSii9777RxPortSet(UINT8 *port);
UINT8 u1HostSii9777RxPortGet(UINT8 *port);
UINT8 u1HostSii9777TxPortSet(UINT8 *port);
UINT8  u1HostSii9777TxPortGet(UINT8 *port);
UINT8 u1HostHpdSet(UINT8 port,UINT8 *hpdSet);
UINT8 u1HostHpdGet(UINT8 port,UINT8 *hpdGet);
UINT8 u1HostSii9777HpdReplicateSet(UINT8 *u1Replicate);
UINT8 u1HostSii9777HpdReplicateGet(UINT8 *u1Replicate);
UINT8 u1HostSii9777DualLinkAutoSet(UINT8 *u1AutoSet);
UINT8 u1HostSii9777DualLinkAutoGet(UINT8 *u1Autoget);
UINT8 u1HostSii9777SplitModeSet(UINT8 *splitMode);
UINT8 u1HostSii9777SplitModeGet(UINT8 *splitMode);
UINT8 u1HostSii9777DualLinkSet(UINT8 *dualLinkEn);
UINT8 u1HostSii9777DualLinkGet(UINT8 *dualLinkEn);
UINT8 u1HostSii9777GetFirmwareVersion(UINT32 *version);
UINT8 u1HostSii9777EdidReplicateSet(UINT8 *ReplicatedEn);
UINT8 u1HostSii9777EdidReplicateGet(UINT8 *ReplicatedEn);
UINT8 u1HostSii9777EdidSet(UINT8 port,UINT8 *pEdidData);
UINT8 u1HostSii9777EdidGet(UINT8 port,UINT8 *pEdidData);
UINT8 u1HostSii9777VideoTimingGet(HostSii9777VideoTiming_t *HostSii9777Timing);
UINT8 u1HostSii9777ProductIdStatusGet(UINT32 *ProductId);
UINT8 u1HostSii9777ChipRevisionStatusGet(UINT8 *chipVer);
UINT8 u1HostSii9777ChipIdStatusGet(UINT32 *chipIDnum);
UINT8 u1HostSii9777FirmwareTimeStampStatusGet(UINT32 *timeStampGet);
UINT8 u1HostSii9777BootStatusGet(UINT8 *bootSta);
UINT8 u1HostSii9777AvLinkStatusGet(UINT8 *avLinkStatusGet);
UINT8 u1HostSii9777HdcpStatusGet(UINT8 *hpdStaGet);
UINT8 u1HostSii9777AvMuteStatusGet(UINT8 *avMuteStatusGet);
UINT8 u1HostSii9777Plus5vStatusGet(UINT8 *Rx5VStatus);
void vCreateSii9777Thread(void);
UINT8 u1HostSii9777TmdsEnableSet(UINT8 txPort, UINT8 *enable);
UINT8 u1HostSii9777TmdsEnableGet(UINT8 txPort, UINT8 *enable);
UINT8 u1HostSii9777VideoFmtStatusGet(HostSii9777VideoFormat_t *vdoFormatGet);
UINT8 u1HostSii9777DualLinkStatusGet(UINT8 *dualLinkStatusGet);
#endif
