/******************************************************************************
 *
 * Copyright 2013, Silicon Image, Inc.  All rights reserved.
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of
 * Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
 *
 *****************************************************************************/
/**
 * @file sii9777_rx_func.h
 *
 * @brief Host Driver for SiI9777 Rx device.
 *
 *****************************************************************************/

#ifndef SII9777_RX_FUNC_H
#define SII9777_RX_FUNC_H

#ifdef __cplusplus
extern "C"{
#endif

/***** #include statements ***************************************************/

/***** public functions ******************************************************/

/*****************************************************************************/
/**
* @brief Active Input Select control
*
* Selects which input is used for down-stream processing.
*
* @param[in]  inst    Driver instance returned by @ref Sii9777Create()
* @param[in]  pPort   Input port
*
******************************************************************************/
void Sii9777InputSelectSet( SiiInst_t inst, Sii9777RxPort_t* pPort );
void Sii9777InputSelectGet( SiiInst_t inst, Sii9777RxPort_t* pPort );

/*****************************************************************************/
/**
* @brief RX EDID replicate enable/disable control
*
* Enables/disables EDID replication from down-stream EDID.
*
* @param[in]  inst  Driver instance returned by @ref Sii9777Create()
* @param[in]  bOn   - \c true  :    EDID replication is enabled though primary output port.
*                   - \c false :    EDID replication is disabled.
*
******************************************************************************/
void Sii9777EdidReplicateSet( SiiInst_t inst, bool_t* pbOn );
void Sii9777EdidReplicateGet( SiiInst_t inst, bool_t* pbOn );

/*****************************************************************************/
/**
* @brief RX HPD replicate enable/disable control
*
* Enables/disables HPD replication from down-stream HPD.
*
* @param[in]  inst  Driver instance returned by @ref Sii9777Create()
* @param[in]  bOn   - \c true  :    HPD replication is enabled though primary output port.
*                   - \c false :    HPD replication is disabled.
*
******************************************************************************/
void Sii9777HpdReplicateSet( SiiInst_t inst, bool_t* pbOn );
void Sii9777HpdReplicateGet( SiiInst_t inst, bool_t* pbOn );

/*****************************************************************************/
/**
* @brief Read/Write up-stream EDID-Rx (SRAM)
*
* Reads/Writes data from up-stream EDID SRAM.
*
* @param[in]  inst    Driver instance returned by @ref Sii9777Create()
* @param[in]  port    Input port
* @param[in]  offset  Offset in EDID SRAM memory
* @param[out] pData   Pointer to EDID data buffer
* @param[in]  length  Number of bytes to read/write
*
******************************************************************************/
void Sii9777EdidSet( SiiInst_t inst, Sii9777RxPort_t port, uint16_t offset, uint8_t *pData, uint16_t length );
void Sii9777EdidGet( SiiInst_t inst, Sii9777RxPort_t port, uint16_t offset, uint8_t *pData, uint16_t length );

/*****************************************************************************/
/**
* @brief Rx HPD user control setter
*
* Enables/Disables RX Hot Plug.
* The upstream Hot Plug signal is also controlled based on state of the downstream Hot Plug
* and selected configuration.
* The system makes sure that HPD low time is at least 500ms
* (the standard requires at least 100ms).
* Note: User requested HPD is only applied if Hpd replication is turned off.
*
* @param[in]  inst     Driver instance returned by @ref Sii9777Create()
* @param[in]  pbHpd    Upstream HPD user requested state
*                      - @ref true   Hot-Plug user state is enabled.
*                      - @ref false  Hot-Plug user state is disabled.
*
*****************************************************************************/
void Sii9777HpdSet( SiiInst_t inst, Sii9777RxPort_t port, bool_t *pbHpd );
void Sii9777HpdGet( SiiInst_t inst, Sii9777RxPort_t port, bool_t *pbHpd );

/*****************************************************************************/
/**
* @brief Rx HPD Interrogation function
*
* Indicates active applied Hot-Plug state by receiver.
* This function is particular useful in case Hpd replication is enabled.
*
* @param[in]  inst     Driver instance returned by @ref Sii9777Create()
* @param[in]  pbHpd    Upstream HPD state
*                      - @ref true   Active Hot-Plug signal is enabled.
*                      - @ref false  Active Hot-Plug signal is forced low.
*
*****************************************************************************/
void Sii9777HpdStatusGet( SiiInst_t inst, Sii9777RxPort_t port, bool_t *pbHpd );

/*****************************************************************************/
/**
* @brief HDCP 2.X RxInfo and HDCP 1.X BStatus configuration.
*
* @param[in]  inst               Adapter driver instance returned by @ref SiiDrvAdaptCreate()
* @param[out] pTopology          Pointer to a structure to return downstream topology data structure
*
*****************************************************************************/
void Sii9777HdcpTopologySet( SiiInst_t inst, Sii9777HdcpTopology_t *pTopology );
void Sii9777HdcpTopologyGet( SiiInst_t inst, Sii9777HdcpTopology_t *pTopology );

/*****************************************************************************/
/**
* @brief Downstream HDCP 1.X/2.X BKSV list configuration
*
* Configures downstream BKSV list
*
* @note This function is only expected to be called when Rx is used in AVR repeater case.
*
* @param[in]  inst         Adapter driver instance returned by @ref SiiDrvAdaptCreate()
* @param[out] pBksvList    Pointer to a buffer to return downstream BKSV/RxID list
* @param[in]  len          number of BKSVs to be read
*
* @retval Number of actual BKSVs written.
*
*****************************************************************************/
uint8_t Sii9777HdcpBksvListSet( SiiInst_t inst, Sii9777HdcpKsv_t* pBksvList, uint8_t len );
uint8_t Sii9777HdcpBksvListGet( SiiInst_t inst, Sii9777HdcpKsv_t* pBksvList, uint8_t len );

/*****************************************************************************/
/**
* @brief Downstream HDCP downstream repeater bit configuration
*
* Configures downstream repeater status.
*
* @note This function is only expected to be called when Rx is used in AVR repeater case.
*
* @param[in]  inst               Adapter driver instance returned by @ref SiiDrvAdaptCreate()
* @param[out] pRepeaterBit       Repeater bit configuration. Must be set true if any incoming
*                                content (audio or video) is also output on any of the system's
*                                HDCP compliant output interfaces.
*
*****************************************************************************/
void Sii9777HdcpRepeaterBitSet( SiiInst_t inst, bool_t *pRepeaterBit );
void Sii9777HdcpRepeaterBitGet( SiiInst_t inst, bool_t *pRepeaterBit );

/*****************************************************************************/
/**
* @brief Dual-link format Look-up table configuration.
*
* Any video resolution received on input that matches any of VidRes ID in LUT will
* force the video to be split through Tx1 and Tx2. Audio is output to 'primTxPort'.
* If no match is found then both audio and video will be routed to 'primTxPort'.
*
* @param[in]  inst     Driver instance returned by @ref Sii9777Create()
* @param[out] pVidRes  Array of VidRes IDs.
* @param[in]  len      Length of array of VidRes ID's. 
*
*****************************************************************************/
void Sii9777DualLinkLutSet( SiiInst_t inst, SII9777VidRes_t* pVidRes, uint8_t len );
void Sii9777DualLinkLutGet( SiiInst_t inst, SII9777VidRes_t* pVidRes, uint8_t len );

/*****************************************************************************/
/**
* @brief Manual Dual link control.
*
* User defined state of dual-link state.
* Note: User requested dual-link state only applied if automatic dual-link mode is disabled.
*
* @param[in]  inst           Driver instance returned by @ref Sii9777Create()
* @param[]    bDualLink      - @ref true    Dual output (Tx1/Tx2) enabled
*                            - @ref false   Dual output disabled.
*
*****************************************************************************/
void Sii9777DualLinkSet( SiiInst_t inst, bool_t* bDualLink );
void Sii9777DualLinkGet( SiiInst_t inst, bool_t* bDualLink );

/*****************************************************************************/
/**
* @brief Active dual-link state interrogation function
*
* Indicates whether currently dual-link is enabled or disabled.
* This function is particular useful in case dual-link is set to auto mode.
*
* @param[in]  inst           Driver instance returned by @ref Sii9777Create()
* @param[]    bDualLink      - @ref true    Dual output (Tx1/Tx2) enabled
*                            - @ref false   Dual output disabled.
*
*****************************************************************************/
void Sii9777DualLinkStatusGet( SiiInst_t inst, bool_t* bDualLink );

/*****************************************************************************/
/**
* @brief Auto/Manual Dual link control.
*
* Selects whether Dual-link function is asserted automatically through LUT configuration
* or controlled manually by '@ref Sii9777DualLinkSet()'
*
* @param[in]  inst           Driver instance returned by @ref Sii9777Create()
* @param[]    bAuto          - @ref true    Dual link is asserted automatically.
*                            - @ref false   Dual link is asserted manually.
*
*****************************************************************************/
void Sii9777DualLinkAutoSet( SiiInst_t inst, bool_t* bAuto );
void Sii9777DualLinkAutoGet( SiiInst_t inst, bool_t* bAuto );

/*****************************************************************************/
/**
* @brief Rx AV Link status interrogation
*
* Returns RX Link status.
*
* @param[in]  inst           Driver instance returned by @ref Sii9777Create()
* @param[out] pAvLinkStat    AV-Link status:
*                            - @ref SII9777_AV_LINK__NONE    No video
*                            - @ref SII9777_AV_LINK__DVI     DVI
*                            - @ref SII9777_AV_LINK__HDMI1   HDMI1.x
*                            - @ref SII9777_AV_LINK__HDMI2   HDMI2.x
*                            - @ref SII9777_AV_LINK__MHL     MHL
*
*****************************************************************************/
void Sii9777AvLinkStatusGet( SiiInst_t inst, Sii9777AvLink_t *pAvLink );

/*****************************************************************************/
/**
* @brief Rx HDCP status interrogation
*
* Returns HDCP status.
*
* @param[in]  inst           Driver instance returned by @ref Sii9777Create()
* @param[out] pHdcpStat      HDCP status:
*                            - @ref SII9777_HDCP_STATUS__OFF              Authentication is not enabled
*                            - @ref SII9777_HDCP_STATUS__SUCCESS_1X       Authentication succeeded for HDCP 1.X
*                            - @ref SII9777_HDCP_STATUS__SUCCESS_22       Authentication succeeded for HDCP 2.2
*                            - @ref SII9777_HDCP_STATUS__AUTHENTICATING   Authentication is in progress
*                            - @ref SII9777_HDCP_STATUS__FAILED           Authentication failed and does not re-try
*
*****************************************************************************/
void Sii9777HdcpStatusGet( SiiInst_t inst, Sii9777HdcpStatus_t *pHdcpStat );

/*****************************************************************************/
/**
* @brief Rx AV Mute status interrogation
*
* Returns RX Mute status.
*
* @param[in]  inst           Driver instance returned by @ref Sii9777Create()
* @param[out] pbAvMute       Pointer to a boolean variable to copy the mute status. \c *pbAvMuteStat:
*                            - \c true    Input signal is AV Muted
*                            - \c false   Input signal is not AV Muted
*
*****************************************************************************/
void Sii9777AvMuteStatusGet( SiiInst_t inst, bool_t *pbAvMute );

/*****************************************************************************/
/**
* @brief Rx +5V status interrogation
*
* Returns Rx +5V status.
*
* @param[in]  inst           Driver instance returned by @ref Sii9777Create()
* @param[out] pbPlus5v       Pointer to a boolean variable to copy RX +5V status, \c *pbPlus5vStat:
*                            - \c true    No +5V signal
*                            - \c false   +5V signal detected
*
*****************************************************************************/
void Sii9777Plus5vStatusGet( SiiInst_t inst, bool_t *pbPlus5v );

/*****************************************************************************/
/**
* @brief Rx Video timing interrogation
*
* Returns video timing structure.
*
* @param[in]  inst           Driver instance returned by @ref Sii9777Create()
* @param[out] pVideoTiming   Video timing structure:
*
*****************************************************************************/
void Sii9777VideoTimingStatusGet( SiiInst_t inst, Sii9777VideoTiming_t *pVideoTiming );

/*****************************************************************************/
/**
* @brief Incoming video format interrogation
*
* Retrieve data structure representing incoming video format.
* The structure is based on the following IF/Ps:
* - AVI
* - HDMI VSIF
* - HDMI-Forum VSIF
* - MHL VSIF
* - GBD (Gamut Boundary Packet)
*
* In case of DVI signal and also if measured data differs from the information
* in the received IF/Ps, the driver provides the video format according
* to the measurement information.
*
* Any detected format change generates @ref SII9777_EVENT__RX_VIDEO_FMT_CHNG
* notification.
*
* @param[in]  inst        Driver instance returned by @ref Sii9777Create()
* @param[out] poVideoFmt  Pointer to a buffer to copy the video format structure
*
* @retval #SII9777_DATA__SUCCESS     Retrieved information is valid
* @retval #SII9777_DATA__UNAVAILABLE Information is not available
*
*****************************************************************************/
Sii9777Data_t Sii9777VideoFmtStatusGet( SiiInst_t inst, Sii9777VideoFmt_t *poVideoFmt );

/*****************************************************************************/
/**
* @brief Incoming audio format interrogation
*
* Retrieve data structure representing Audio info frame and some channel status bytes.
*
* Any detected format change generates @ref SII9777_EVENT__RX_AUDIO_FMT_CHNG
* notification.
*
* @param[in]  inst        Driver instance returned by @ref Sii9777Create()
* @param[out] pAudioFmt   Pointer to a buffer to copy the audio format structure
*
* @retval #SII9777_DATA__SUCCESS     Retrieved information is valid
* @retval #SII9777_DATA__UNAVAILABLE Information is not available
*
*****************************************************************************/
Sii9777Data_t Sii9777AudioFmtStatusGet( SiiInst_t inst, Sii9777AudioFrm_t *pAudioFmt );

void Sii9777PrimTxPortSet( SiiInst_t inst, Sii9777TxPort_t* pPort );
void Sii9777PrimTxPortGet( SiiInst_t inst, Sii9777TxPort_t* pPort );

void Sii9777HdcpContentTypeStatusGet( SiiInst_t inst, Sii9777HdcpContentType_t* pContentType );

void Sii9777HpdStatusGet( SiiInst_t inst, Sii9777RxPort_t port, bool_t *pbHpd );

void Sii9777DualLinkStatusGet( SiiInst_t inst, bool_t* bDualLink );

#ifdef __cplusplus
}
#endif

#endif // SII9777_RX_FUNC_H

