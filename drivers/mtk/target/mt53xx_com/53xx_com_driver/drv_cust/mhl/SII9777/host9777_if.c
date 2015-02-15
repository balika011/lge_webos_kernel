#include "si_datatypes.h"
#include "sii9777_platform_api.h"
#include "sii9777rx_api.h"
#include "host9777_if.h"
#include "x_os.h"
#include "x_assert.h"

SiiInst_t sii9777Instance;

int hwId = 0;


SiiLibEvtCbFunc_t Sii9777CallBackHandler;

void SiiEventCbFunc( SiiInst_t inst, Sii9777EventFlags_t eventFlags );

/*

#define SII9777_BUFFER_SIZE   256 
static FILE* sp9777BinFile = NULL;
*/
/* This function is supposed to be called from user application */
/*
void FirmwareUpdateRequest( void )
{
    if( !sp9777BinFile )
    {
        
        sp9777BinFile = fopen("sii9777rx.bin", "rt");

        if( sp9777BinFile )
        {
            Sii9777FirmwareInit();
        }
    }
}
*/
extern UINT8 fgSwitchInitFlag;
void SiiEventCbFunc( SiiInst_t inst, Sii9777EventFlags_t eventFlags )
{
	if(eventFlags & SII9777_EVENT_FLAGS__FLASH_DONE)
	{
		
		/*
		if( sp9777BinFile ) // More data to be programmed
		{
			uint8_t  buffer[SII9777_BUFFER_SIZE];
			uint16_t size = 0;

		   // Retrieve binary data from file 
			   while( SII9777_BUFFER_SIZE > size )
				{
					if( feof(sp9777BinFile) )
					{
						fclose(sp9777BinFile);
						sp9777BinFile = NULL;
						break;
					}
					else
					{
						buffer[size] = fgetc(sp9777BinFile);
						size++;
					}
				}

				Sii9777FirmwareUpdate(inst, &buffer, size);
			}
			else
			{
				Sii9777FirmwareRestart(inst);
			}
			*/
	}
	
	if(eventFlags & SII9777_EVENT_FLAGS__BOOT_DONE)
	{
		//boot done success , Firwadonere normal in flash
		fgSwitchInitFlag = 1;
		
		{
			UINT8 set9777Port = 1;
			UINT8 DualLinkAuto = 1;
			UINT8 splitMode = 0;
			UINT8 dualLink = 0;
			u1HostSii9777SplitModeSet(&splitMode);
			u1HostSii9777DualLinkSet(&dualLink);
			u1HostSii9777DualLinkAutoSet(&DualLinkAuto);
			u1HostSii9777TxPortSet(&set9777Port);
		}
		printf("sii977 Boot /n");
	}
}

const Sii9777Config_t config9777 = 
{
	"sii9777",
	SII9777_TX_PORT__1
};
UINT32 siiPlatformInit(void)
{	
	Sii9777CallBackHandler = SiiEventCbFunc;
    sii9777Instance = Sii9777Create(hwId, Sii9777CallBackHandler, &config9777);
	return (UINT32)sii9777Instance;
}
UINT32 siiPlatformEndrun(void)
{
	Sii9777Delete(sii9777Instance);
	return (UINT32)sii9777Instance;
}


void vHost9777Handle(void) 
{
   Sii9777Handle(sii9777Instance);
}
UINT8 u1HostSii9777RxPortSet(UINT8 *port)
{
	Sii9777RxPort_t sii9777rxPort;
	switch(*port)
	{
		case 0:
			sii9777rxPort = SII9777_RX_PORT__0;
			break;
		case 1:
			sii9777rxPort = SII9777_RX_PORT__1;
			break;
#ifdef SII9777_REF_ES1
		case 2:
			sii9777rxPort = SII9777_RX_PORT__2;
			break;
		case 3:
			sii9777rxPort = SII9777_RX_PORT__3;
			break;
#endif
		default :
		sii9777rxPort = SII9777_RX_PORT__0;
		break;
	}
	Sii9777InputSelectSet(sii9777Instance,&sii9777rxPort);
	return 1;
}
UINT8 u1HostSii9777RxPortGet(UINT8 *port)
{
	Sii9777RxPort_t rxPort;
	Sii9777InputSelectGet(sii9777Instance,&rxPort);
	*port = (UINT8)rxPort;
	return 1;
} 
UINT8 u1HostSii9777TxPortSet(UINT8 *port)
{
	Sii9777TxPort_t sii9777TxPort;
	switch(*port)
	{
		case 0:
			sii9777TxPort = SII9777_TX_PORT__0;
			break;
		case 1:
			sii9777TxPort = SII9777_TX_PORT__1;
			break;
		case 2:
			sii9777TxPort = SII9777_TX_PORT__2;
			break;
		default :
			sii9777TxPort = SII9777_TX_PORT__1;
			break;
	}
	Sii9777PrimTxPortSet(sii9777Instance,&sii9777TxPort);
	return 1;
}
UINT8  u1HostSii9777TxPortGet(UINT8 *port)
{
	Sii9777TxPort_t txPort;
	Sii9777PrimTxPortGet(sii9777Instance,&txPort);
	*port = (UINT8)txPort;
	return 1;
}



UINT8 u1HostHpdSet(UINT8 port,UINT8 *hpdSet)
{	
	Sii9777RxPort_t sii9777rxPort;
	bool_t siiHpdSet;
	siiHpdSet = (*hpdSet)?1:0;
	switch(port)
	{
		case 0:
			sii9777rxPort = SII9777_RX_PORT__0;
			break;
		case 1:
			sii9777rxPort = SII9777_RX_PORT__1;
			break;
#ifdef SII9777_REF_ES1
		case 2:
			sii9777rxPort = SII9777_RX_PORT__2;
			break;
		case 3:
			sii9777rxPort = SII9777_RX_PORT__3;
			break;
#endif
		default :
		sii9777rxPort = SII9777_RX_PORT__0;
		break;
	}
	Sii9777HpdSet(sii9777Instance,sii9777rxPort,&siiHpdSet);
	return 1;
}
UINT8 u1HostHpdGet(UINT8 port,UINT8 *hpdGet)
{	
	Sii9777RxPort_t sii9777rxPort;
	bool_t sii9777HpdStatus;
	switch(port)
	{
		case 0:
			sii9777rxPort = SII9777_RX_PORT__0;
			break;
		case 1:
			sii9777rxPort = SII9777_RX_PORT__1;
			break;
#ifdef SII9777_REF_ES1
		case 2:
			sii9777rxPort = SII9777_RX_PORT__2;
			break;
		case 3:
			sii9777rxPort = SII9777_RX_PORT__3;
			break;
#endif
		default :
		sii9777rxPort = SII9777_RX_PORT__0;
		break;
	}
	
	Sii9777HpdStatusGet(sii9777Instance,sii9777rxPort,&sii9777HpdStatus);
	(*hpdGet) = sii9777HpdStatus?1:0;
	return 1;
}
UINT8 u1HostSii9777HpdReplicateSet(UINT8 *u1Replicate)
{
	bool_t replicate = (*u1Replicate)?1:0;
	Sii9777HpdReplicateSet(sii9777Instance,&replicate);
	return 1;
}
UINT8 u1HostSii9777HpdReplicateGet(UINT8 *u1Replicate)
{
	bool_t replicate;
	Sii9777HpdReplicateGet(sii9777Instance,&replicate);
	(*u1Replicate) = replicate?1:0;
	return 1;
}

UINT8 u1HostSii9777DualLinkAutoSet(UINT8 *u1AutoSet)
{
	bool_t autoSet = (*u1AutoSet)?1:0;
	Sii9777DualLinkAutoSet(sii9777Instance,&autoSet);
	return 1;
}
UINT8 u1HostSii9777DualLinkAutoGet(UINT8 *u1Autoget)
{
	bool_t autoSet = (*u1Autoget)?1:0;
	Sii9777DualLinkAutoGet(sii9777Instance,&autoSet);
	*u1Autoget = autoSet?1:0;
	return 1;
}

UINT8 u1HostSii9777SplitModeSet(UINT8 *splitMode)
{
	Sii9777SplitMode_t localSplictMode = (Sii9777SplitMode_t)(*splitMode);
	Sii9777SplitModeSet(sii9777Instance,&localSplictMode);
	return 1;
}
UINT8 u1HostSii9777SplitModeGet(UINT8 *splitMode)
{
	Sii9777SplitMode_t sii977splitMode;
	Sii9777SplitModeGet(sii9777Instance,&sii977splitMode);
	(*splitMode) = (UINT8)sii977splitMode;
	return 1;
}

UINT8 u1HostSii9777DualLinkSet(UINT8 *dualLinkEn)
{
	bool_t localDualLink = (*dualLinkEn)?1:0;
	Sii9777DualLinkSet(sii9777Instance,&localDualLink);
	return 1;
}
UINT8 u1HostSii9777DualLinkGet(UINT8 *dualLinkEn)
{
	bool_t dualLinkEnable;
	Sii9777DualLinkGet(sii9777Instance,&dualLinkEnable);
	(*dualLinkEn) = (UINT8)dualLinkEnable;
	return 1;
}
UINT8 u1HostSii9777DualLinkStatusGet(UINT8 *dualLinkStatusGet)
{
	bool_t sii9777dualLinkSta;
	Sii9777DualLinkStatusGet(sii9777Instance,&sii9777dualLinkSta);
	*dualLinkStatusGet = sii9777dualLinkSta;
	return 1;
}

UINT8 u1HostSii9777GetFirmwareVersion(UINT32 *version)
{
	uint32_t sii9777version;
	Sii9777FirmwareVersionStatusGet(sii9777Instance,&sii9777version);
	*version = sii9777version;
	return 1;
}
UINT8 u1HostSii9777EdidReplicateSet(UINT8 *ReplicatedEn)
{
	bool_t bReplicatedEn = (*ReplicatedEn)?1:0;
	Sii9777EdidReplicateSet(sii9777Instance,&bReplicatedEn);
}
UINT8 u1HostSii9777EdidReplicateGet(UINT8 *ReplicatedEn)
{
	bool_t bReplicatedEn;
	Sii9777EdidReplicateGet(sii9777Instance,&bReplicatedEn);
	*ReplicatedEn = (UINT8)bReplicatedEn;
	return 1;
}
UINT8 u1HostSii9777EdidSet(UINT8 port,UINT8 *pEdidData)
{
	Sii9777EdidSet(sii9777Instance,port,0,pEdidData,256);
	return 1;
}
UINT8 u1HostSii9777EdidGet(UINT8 port,UINT8 *pEdidData)
{
	Sii9777EdidGet(sii9777Instance,port,0,pEdidData,256);
	return 1;
}
UINT8 u1HostSii9777VideoTimingGet(HostSii9777VideoTiming_t *HostSii9777Timing)
{
	Sii9777VideoTiming_t sii9777Timing;
	Sii9777VideoTimingStatusGet(sii9777Instance,&sii9777Timing);
	HostSii9777Timing->hActive = sii9777Timing.hActive;
	HostSii9777Timing->hFrontPorch = sii9777Timing.hFrontPorch;
	HostSii9777Timing->hSync = sii9777Timing.hSync;
	HostSii9777Timing->hTotal = sii9777Timing.hTotal;
	HostSii9777Timing->interlaced = sii9777Timing.interlaced?1:0;
	HostSii9777Timing->pixelFreq = sii9777Timing.pixelFreq;
	HostSii9777Timing->posHsync = sii9777Timing.posHsync;
	HostSii9777Timing->posVsync = sii9777Timing.posVsync;
	HostSii9777Timing->vActive = sii9777Timing.vActive;
	HostSii9777Timing->vFrontPorch = sii9777Timing.vFrontPorch;
	HostSii9777Timing->vSync =  sii9777Timing.vSync;
	HostSii9777Timing->vTotal = sii9777Timing.vTotal;
	return 1;
}



UINT8 u1HostSii9777ProductIdStatusGet(UINT32 *ProductId)
{
	uint32_t Sii9777ProductID; 
	Sii9777ProductIdStatusGet(sii9777Instance,&Sii9777ProductID);
	(*ProductId) = Sii9777ProductID;
	return 1;
}
UINT8 u1HostSii9777ChipRevisionStatusGet(UINT8 *chipVer)
{
	uint8_t chipVersion; 
	Sii9777ChipRevisionStatusGet(sii9777Instance,&chipVersion);
	*chipVer = chipVersion;
	return 1;
}
UINT8 u1HostSii9777ChipIdStatusGet(UINT32 *chipIDnum)
{
	uint32_t chipID; 
	Sii9777ChipIdStatusGet(sii9777Instance,&chipID);
	*chipIDnum = chipID;
	return 1;
}

UINT8 u1HostSii9777FirmwareTimeStampStatusGet(UINT32 *timeStampGet)
{
	uint32_t timeStamp; 
	Sii9777FirmwareTimeStampStatusGet(sii9777Instance,&timeStamp);
	(*timeStampGet) = timeStamp;
	return 1;
}

UINT8 u1HostSii9777BootStatusGet(UINT8 *bootSta)
{
	return 1;//driver not implement yet.
}

UINT8 u1HostSii9777AvLinkStatusGet(UINT8 *avLinkStatusGet)
{
	Sii9777AvLink_t avLinkStatus;
	Sii9777AvLinkStatusGet(sii9777Instance,&avLinkStatus);
	(*avLinkStatusGet) =  avLinkStatus;
	return 1;
}
UINT8 u1HostSii9777HdcpStatusGet(UINT8 *hpdStaGet)
{
	Sii9777HdcpStatus_t hpdStatus;
	Sii9777HdcpStatusGet(sii9777Instance,&hpdStatus);
	(*hpdStaGet) = hpdStatus;
	return 1;
}
UINT8 u1HostSii9777AvMuteStatusGet(UINT8 *avMuteStatusGet)
{
	bool_t avMuteStatus;
	Sii9777AvMuteStatusGet(sii9777Instance,&avMuteStatus);
	(*avMuteStatusGet) = avMuteStatus;
	return 1;
}
UINT8 u1HostSii9777Plus5vStatusGet(UINT8 *Rx5VStatus)
{
	bool_t rx5VStatus;
	Sii9777Plus5vStatusGet(sii9777Instance,&rx5VStatus);
	*Rx5VStatus = rx5VStatus;
	return 1;
}



UINT8 u1HostSii9777VideoFmtStatusGet(HostSii9777VideoFormat_t *vdoFormatGet)
{
	Sii9777VideoFmt_t videoFormat;
	Sii9777Data_t dataValid;
	dataValid = Sii9777VideoFmtStatusGet(sii9777Instance,&videoFormat);
	if(dataValid == SII9777_DATA__SUCCESS)
	{
		vdoFormatGet->ActiveAR = videoFormat.activeAR;
		vdoFormatGet->bitDepth = videoFormat.pixFrm.bitDepth;
		vdoFormatGet->chromaSample = videoFormat.pixFrm.crSmpl;
		vdoFormatGet->colorSpace = videoFormat.pixFrm.clrFmt;
		vdoFormatGet->contentType = videoFormat.contentType;
		vdoFormatGet->horizBarInfo.pixNumEndOfLeftBar = videoFormat.horizBarInfo.pixNumEndOfLeftBar;
		vdoFormatGet->horizBarInfo.pixNumStartOfRightBar = videoFormat.horizBarInfo.pixNumStartOfRightBar;
		vdoFormatGet->isActiveFmt = videoFormat.isActiveFormat;
		vdoFormatGet->isITcontent = videoFormat.isITContent;
		vdoFormatGet->isHorizBarInfoValid = videoFormat.isHorizBarInfoValid;
		vdoFormatGet->isVertBarInfoValid = videoFormat.isVertBarInfoValid;
		vdoFormatGet->pictureAR = videoFormat.pictureAR;
		vdoFormatGet->pictureScaling = videoFormat.picScaling;
		vdoFormatGet->pixelRepetition = videoFormat.pixelRepetition;
		vdoFormatGet->scanInfo = videoFormat.scanInfo;
		vdoFormatGet->vertBarInfo.lineNumEndOfTopBar = videoFormat.vertBarInfo.lineNumEndOfTopBar;
		vdoFormatGet->vertBarInfo.lineNumStartOfBottomBar = videoFormat.vertBarInfo.lineNumStartOfBottomBar;
		vdoFormatGet->video3Dmode = videoFormat.mode3D;
		vdoFormatGet->videoVid = videoFormat.vidRes;
	}
	return (dataValid == SII9777_DATA__SUCCESS)?1:0;
}

UINT8 u1HostSii9777AudioFmtStatusGet(void)
{
	
}
void Sii9777TmdsEnableSet( SiiInst_t inst, Sii9777TxPort_t port, bool_t* bTmds );
void Sii9777TmdsEnableGet( SiiInst_t inst, Sii9777TxPort_t port, bool_t* bTmds );
UINT8 u1HostSii9777TmdsEnableSet(UINT8 txPort, UINT8 *enable)
{
	bool_t tmdsEnable;
	Sii9777TxPort_t sii9777txPort;
	tmdsEnable = *enable;
	switch(txPort)
		{
			case 0:
				sii9777txPort = SII9777_TX_PORT__0;
				break;
			case 1:
				sii9777txPort = SII9777_TX_PORT__1;
				break;
			case 2:
				sii9777txPort = SII9777_TX_PORT__2;
				break;
			default:
				sii9777txPort = SII9777_TX_PORT__1;
				break;
		}
	Sii9777TmdsEnableSet(sii9777Instance,sii9777txPort,&tmdsEnable);
	return 1;
}
UINT8 u1HostSii9777TmdsEnableGet(UINT8 txPort, UINT8 *enable)
{
	bool_t tmdsEnable;
	Sii9777TxPort_t sii9777txPort;
	switch(txPort)
		{
			case 0:
				sii9777txPort = SII9777_TX_PORT__0;
				break;
			case 1:
				sii9777txPort = SII9777_TX_PORT__1;
				break;
			case 2:
				sii9777txPort = SII9777_TX_PORT__2;
				break;
			default:
				sii9777txPort = SII9777_TX_PORT__1;
				break;
		}
	Sii9777TmdsEnableGet(sii9777Instance,sii9777txPort,&tmdsEnable);
	*enable = tmdsEnable?1:0;
	return 1;
}

extern void x_thread_delay(UINT32 u4Ms);
static HANDLE_T phSii9777ThreadHdl;
static void _Sii9777_Thread( void * pvArgs )
{
	while(1)
	{
		x_thread_delay(100);
		vHost9777Handle();  
	}
}
void vCreateSii9777Thread(void)
{
	 VERIFY(x_thread_create(&phSii9777ThreadHdl, 
			 "SII9777Thread", 2048,  60, _Sii9777_Thread, 0, NULL) == OSR_OK);
	 printf("create SII9777 thread\n");
}



