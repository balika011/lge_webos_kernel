
#ifndef __SDEC_COMMON_HEADER__
#define __SDEC_COMMON_HEADER__

/******************************************************************************
 	상수 정의(Constant Definitions)
******************************************************************************/
#define	INVALID_PID	0x1FFF
#define	SECTION_FIELD_MASK		0


/******************************************************************************
    매크로 함수 정의 (Macro Definitions)
******************************************************************************/


/******************************************************************************
	형 정의 (Type Definitions)
******************************************************************************/

/**
 * SDEC Platform Type
*/
typedef enum {
	SDEC_PLATFORM_ATSC = 0,
	SDEC_PLATFORM_DVB,
	SDEC_PLATFORM_ISDB,
	SDEC_PLATFORM_COMMERCIAL,
	SDEC_PLATFORM_MAX
} HAL_SDEC_PLATFORM_T;

/**
 * SDEC Country
*/
typedef enum {
	SDEC_COUNTRY_KR = 0,
	SDEC_COUNTRY_US,
	SDEC_COUNTRY_EU,
	SDEC_COUNTRY_CN,
	SDEC_COUNTRY_MA,
	SDEC_COUNTRY_JP,
	SDEC_COUNTRY_COLUMBIA,
	SDEC_COUNTRY_JP_OLED,
	SDEC_COUNTRY_MAX = 0xFF
} HAL_SDEC_COUNTRY_T;

/**
 * SDEC PID Type For Filtering.
*/
typedef enum {
	SDEC_PID_VIDEO = 0,
	SDEC_PID_AUDIO,
	SDEC_PID_AUDIO_DESCRIPTION = SDEC_PID_AUDIO,
	SDEC_PID_PCR,
	SDEC_PID_TELETEXT,
	SDEC_PID_SUBTITLE ,
	SDEC_PID_DCC = SDEC_PID_SUBTITLE,
	SDEC_PID_PSI,
	SDEC_PID_GP,
	SDEC_PID_TYPE_NUM
} SDEC_PID_TYPE_T;

/**
 * Destination of PES Filter.
*/
typedef enum {
	PES_DEST_NONE = 0,
	PES_DEST_ADEC0,
	PES_DEST_ADEC1,
	PES_DEST_VDEC0,
	PES_DEST_VDEC1,
	PES_DEST_STC0,
	PES_DEST_STC1,
	PES_DEST_BUFFER,
	PES_DEST_RECORD_A,
	PES_DEST_RECORD_B,
	PES_DEST_TSO,
	PES_DEST_NUM
} SDEC_PES_DEST_T;

/**
 * Input Channel.
*/
typedef enum {
	SDEC_CH_MIN	= 0,
	SDEC_CH_A	= SDEC_CH_MIN,	/* Live TV1 */
	SDEC_CH_B,					/* Live TV2, Recording Channel For Watch&Record, Trusted Reception For Live TV1 */
	SDEC_CH_C,					/* Live TV3 */
	SDEC_CH_CHB,				/* Channel Browser */
	SDEC_CH_PLAYER_A,			/* For Playback Recorded Contents  */
	SDEC_CH_PLAYER_B,			/* For Playback Recorded Contents  */
	SDEC_CH_PLAYER_C,			/* For Playback Recorded Contents  */
	SDEC_CH_UHD,				/* For Live UHD Channel in D1x */
	SDEC_CH_MAX
} SDEC_CHANNEL_T;

/**
 * Input Type
*/
typedef enum {
	SDEC_INPUT_SERIAL = 0,
	SDEC_INPUT_PARALLEL,
	SDEC_INPUT_ENABLE,
	SDEC_INPUT_DISABLE,
	SDEC_INPUT_MEM,
	SDEC_INPUT_NUM,

	SDEC_INPUT_NULL //temp
} SDEC_INPUT_TYPE_T;

/**
 * CAS Type
*/
typedef enum {
	SDEC_WITHOUT_CAS = 0,
	SDEC_WITH_CAS
} SDEC_CAS_TYPE_T;

/**
 * Input Port Type
*/
typedef enum {
	SDEC_PORT_NONE = 0,				/* No Port */
	SDEC_PORT_IN_DEMOD0,			/* Input From Internal Demodulator */
	SDEC_PORT_IN_DEMOD1,			/* Input From Internal Demodulator */
	SDEC_PORT_EXT_INPUT0,			/* SoC EXT Input ( such as External Demodulator or Channel Browser Chip */
	SDEC_PORT_EXT_INPUT1,			/* SoC EXT Input ( such as External Demodulator or Channel Browser Chip */
	SDEC_PORT_EXT_INPUT2,			/* SoC EXT Input ( such as External Demodulator or Channel Browser Chip */
	SDEC_PORT_FROM_MEM,				/* TS Input From Memory */
	SDEC_PORT_NUM,

	SDEC_PORT_NULL //temp
} SDEC_INPUT_PORT_T;

/**
 * IO Port Type
*/
typedef enum {
	SDEC_IO_PORT_PARALLEL0 = 0,		/* Parallel Output Port 0 */
	SDEC_IO_PORT_SERIAL0			/* Serial Output Port 0 */
} SDEC_IO_PORT_T;

/**
 * IO Type
*/
typedef enum {
	SDEC_IOPORT_INPUT = 0,
	SDEC_IOPORT_OUTPUT
} SDEC_IO_TYPE_T;

/**
 * PID filter cleaning scope.
*/
typedef enum
{
	CLEAN_ALL = 0,
	CLEAN_AV,
	CLEAN_VIDEO,
	CLEAN_AUDIO,
	CLEAN_PCR,
	CLEAN_WO_AV,
	CLEAN_SECTION,
	CLEAN_PES,
	CLEAN_RECORD_A,
	CLEAN_RECORD_B,
	CLEAN_PVR
} SDEC_CLEANING_SCOPE_T;


/**
 * Section filter options.
*/
typedef struct SECTION_FILTER
{
 	UINT32	  transaction_id;					/**< Transaction ID */
	UINT16    pid;								/**< PID filter value */
	UINT8	  table_id;							/**< Table ID value */
 	UINT8	  section_syntax_indicator;			/**< Section Syntax Indicator value */
	UINT16	  table_id_extension;				/**< Table ID Extension value */
	UINT8	  version_number;					/**< Version Number value */
	UINT8	  current_next_indicator;			/**< Current Next Indicator value */
	UINT8	  last_section_number;				/**< Last section number value */
	UINT8	  protocol_version;					/**< Protocol Version value */

	UINT8	  table_id_filter_mask;				/**< Table ID filter bit mask (8 bits) */
	UINT16	  table_id_extension_filter_mask;	/**< Table ID extension filter bit mask (16 bits) */
 	UINT8
 		section_syntax_indicator_filter	: 1,	/**< Section syntax indicator filter ON/OFF */
 		version_number_filter			: 1,	/**< Version number filter ON/OFF */
 		not_version_number_filter		: 1,	/**< Not version number filter ON/OFF */
		current_next_indicator_filter	: 1,	/**< Current next indicator filter ON/OFF */
 		last_section_number_filter		: 1,	/**< Last section number filter ON/OFF */
 		protocol_version_filter			: 1,	/**< Protocol version filter ON/OFF */
 		reserved_filter					: 2;	/**< reserved */

 	UINT8
 		once_flag						: 1,	/**< Once flag ON/OFF */
 		crc_chksum						: 2, 	/**< 0 : No CRC & Checksum, 1 : CRC, 2 : CheckSum
												     <= regardless of section_syntax_indicator*/
 		reserved						: 5;	/**< reserved */

} SDEC_SECTION_FILTER_T, *P_SDEC_SECTION_FILTER_T;


/**
 * To Send Section Data To PSIP/SI
 */
typedef struct SDEC_MSG_TYPE
{
	UINT32	msgId;			///< 0x00: message id
	UINT32	channel;		///< 0x04: port id
	UINT8	filterId;		///< 0x05: filter id
	UINT16	pid;			///< 0x06: pid
	UINT8	*pData;			///< 0x08: pointer to section data
	UINT16	tableType;		///< 0x0C: table type
	UINT32	dataLen;		///< 0x0E: section data length
	UINT32	transactionId;	///< 0x10: transaction id
	SINT32	entryId;		///< 0x14: entry id
} SDEC_MSG_TYPE_T;


/**
 * Set/Get Parameters
 */
typedef enum
{
	SDEC_PARAM_PLATFORM = 0,
	SDEC_PARAM_COUNTRY,
	SDEC_PARAM_MODEL_NO,
	SDEC_PARAM_CHIP_ID,
	SDEC_PARAM_DRV_VER,
	SDEC_PARAM_PORT_NUM,
	SDEC_PARAM_AFIFO,
	SDEC_PARAM_MAX
} HAL_SDEC_PARAM_T;



#ifdef INCLUDE_ACAP
/**
 * Section filter information for ACAP.
*/
typedef struct ACAP_SECF_INFO
{
	UINT8	acapOwn;	/**< Section filter used for ACAP */
	UINT8	psipOwn;	/**< Section filter used for PSIP */
	UINT8	psipNotver;	/**< Section filter for PSIP is setted in Not version */
	UINT8	NotverbySW;	/**< Not version is done by SW(TRUE) or HW(FALSE) */
	SINT16	fgID;		/**< Section filter Group IDP */
	UINT8	maxFNum;	/**< Maximum Section filter number in Group */
	SINT16	fID;		/**< Section filter ID in Group */
	UINT8	reserve;	/**< Section filter reserved for ACAP */
	UINT16	pId;		/**< PID value used for ACAP */
	SINT8	tId;		/**< Section filter value used for ACAP */
	UINT8	enable;		/**< Section filter Enable */
} ACAP_SECF_INFO_T;
#endif	/* INCLUDE_ACAP */


#ifndef INCLUDED_BY_FILTER
typedef DTV_STATUS_T (*pfnSDECDataHandlingCB)(SDEC_MSG_TYPE_T *pMsg);
#endif /* INCLUDED_BY_FILTER */


/* Commercial Stub */
/**
 * Define SDEC Output Path to DRM
 */
typedef enum SDEC_RMX_PATH
{
	SDEC_RMX_PATH_INIT = 0,
	SDEC_RMX_PATH_RF_DRM,
	SDEC_RMX_PATH_SERIAL_OUT,
	SDEC_RMX_PATH_IP_DRM = SDEC_RMX_PATH_SERIAL_OUT,
	SDEC_RMX_PATH_MAX
} SDEC_RMX_PATH_T;

/**
 * Define SDEC Output Path Transition for Some SoC
 */
typedef enum SDEC_RMX_PATH_TRANSITION
{
	SDEC_RMX_PATH_TRANSITION_RF_TO_IP = 0,
	SDEC_RMX_PATH_TRANSITION_IP_TO_RF,
	SDEC_RMX_PATH_TRANSITION_MAX
} SDEC_RMX_PATH_TRANSITION_T;

/**
 * Define SDEC Data Channel
 */
typedef enum SDEC_DATA_CH_PATH
{
	SDEC_DATA_CH_PATH_RF = 0,
	SDEC_DATA_CH_PATH_IP,
	SDEC_DATA_CH_PATH_MAX
} SDEC_DATA_CH_PATH_T;

/**
 * Define UDP Packet Destination
 */
typedef enum IPM_PACKET_DESTINATION_TYPE
{
	IPM_PACKET_DESTINATION_DRM = 0,
	IPM_PACKET_DESTINATION_CH_B,
	IPM_PACKET_DESTINATION_BUFFER = IPM_PACKET_DESTINATION_CH_B,	// backward compatibility
	IPM_PACKET_DESTINATION_CH_A,
	IPM_PACKET_DESTINATION_MAX
} IPM_PACKET_DESTINATION_TYPE_T;


/**
 * Define Data Pump Path
 */
typedef enum SDEC_DATA_PUMP_PATH
{
	SDEC_DATA_PUMP_PATH_CH_A = 0,
	SDEC_DATA_PUMP_PATH_CH_B,
	SDEC_DATA_PUMP_PATH_SERIAL_OUT,
	SDEC_DATA_PUMP_PATH_MAX
} SDEC_DATA_PUMP_PATH_T;


/*-----------------------------------------------------------------------
   Descrambler STUB
   20131031 jinhwan.bae Created. Requested to support C&M DVB-CSA
   -----------------------------------------------------------------------*/

/**
 * Descrambler Residual mode
 */
typedef enum
{
	SDEC_DESC_CLEAR		 				= 0,
	SDEC_DESC_CTS 						= 1,
	SDEC_DESC_OFB 						= 2,
	SDEC_DESC_SCTE52_2008 				= 3,
	SDEC_DESC_CTSOC						= 4
}SDEC_DESC_RES_MODE_T;


/**
 * Descrambler Block mode.
 */
typedef enum
{
	SDEC_DESC_ECB	 					= 0,
	SDEC_DESC_CBC 						= 1
}SDEC_DESC_BLK_MODE_T;

/**
 * Descrambler Cipher Type
 */
typedef enum
{
	SDEC_DESC_BYPASS		 			= 0,
	SDEC_DESC_DES 						= 1,
	SDEC_DESC_TDES 						= 2,
	SDEC_DESC_AES 						= 3,
	SDEC_DESC_CSA						= 4,
	SDEC_DESC_MULTI2 					= 5
}SDEC_DESC_CAS_TYPE_T;

/**
 * Descrambler Key Size - it's only for AES
 */
typedef enum
{
	SDEC_DESC_64			= 0,			/**< in  - 64bit */
	SDEC_DESC_128			= 1,			/**< in  -  128bit */
	SDEC_DESC_192			= 2,			/**< in  -  192 bit */
	SDEC_DESC_256			= 3				/**< in  -  256 bit */
}SDEC_DESC_KEY_SIZE_T;

/**
 * Descrambler Setting
 */
typedef struct
{
	SDEC_DESC_RES_MODE_T			eResMode;				/**< Descrambler mode for reset (Clear/CTS/OFB/SCTE52_2008/CTS-OC) */
	SDEC_DESC_BLK_MODE_T			eBlkMode;				/**< Blk Mode (ECB/CBC) */
	SDEC_DESC_CAS_TYPE_T			eCasType;				/**< Cas Type (Bypass/DES/TDES/AES/CSA) */
	SDEC_DESC_KEY_SIZE_T			eKeySize;				/**< Only available in AES */
}SDEC_DESC_SETTING_T;

/**
 * Descrambler Key Type
 */
typedef enum
{
	SDEC_DESC_KEY_EVEN				= 0,			/**< Even key */
	SDEC_DESC_KEY_ODD				= 1,			/**< Odd key */
	SDEC_DESC_KEY_ODD_IV 			= 2,			/**< Odd Initial Vectore key */
	SDEC_DESC_KEY_EVEN_IV			= 3, 			/**< Even Initial Vectore key */
	SDEC_DESC_KEY_SYS				= 4, 			/**< System Key ex. Multi-2 */
	SDEC_DESC_KEY_NUM
}SDEC_DESC_KEY_T;

/*-----------------------------------------------------------------------
   End of Descrambler STUB
   20131031 jinhwan.bae Created. Requested to support C&M DVB-CSA
   -----------------------------------------------------------------------*/


/******************************************************************************
	Extern 전역변수와 함수 prototype 선언
	(Extern Variables & Function Prototype Declarations)
*******************************************************************************/


#endif
