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
 * @file sii9777_common.h
 *
 * @brief Host Driver for SiI9777 device.
 *
 *****************************************************************************/

#ifndef SII9777_COMMON_H
#define SII9777_COMMON_H

#ifdef __cplusplus
extern "C"{
#endif

/***** #include statements ***************************************************/

/***** public functions ******************************************************/

/*****************************************************************************/
/**
* @brief Adapter Driver creation
*
* Allocates resources for Adapter Driver and all child modules.
* This function has to be called before using of any other module function.
*
* This function also registers a callback function that is called
* when the driver status is changed.
* The callback is generated from @ref Sii9777TaskExecute().
* If \c pCallback is NULL, no callbacks are generated.
*
* @param[in] hwId       Hardware instantiation identification number.
*                       Set this as 0 for single Sii9777.
*                       When more than one instances of Sii9777 are designed into
*                       the system, Sii9777Create must be called as many times
*                       with incrementing value in hwId.
*                       hwId is passed into platform host interface layer and can
*                       be used to select a different I2C device address or a
*                       different SPI select signal.
* @param[in] pCallback  Pointer to a callback function to be called on status change
* @param[in] pConfig    Pointer to driver static configuration structure
*
* @return Handle to created object.
*
******************************************************************************/
SiiInst_t Sii9777Create( uint8_t devId, SiiLibEvtCbFunc_t pCallback, Sii9777Config_t* pConfig );

/*****************************************************************************/
/**
* @brief Adapter Driver deletion
*
* Frees resources taken by Adapter Driver and all child modules.
* After this function is called, no other module functions are allowed
* to be called until Sii9777Create() is called.
*
* @param[in] inst Driver instance returned by @ref Sii9777Create()
*
* @see Sii9777Create(), Sii9777Configure()
*
******************************************************************************/
void Sii9777Delete( SiiInst_t inst );

/*****************************************************************************/
/**
* @brief Adapter Driver handle
*
* This is a handle function is expected to be called uncondionally by user application
* any time the SiI9777 chip generates an interrupt. The returning value indicates the
* number of milli seconds that application can wait before calling this function again.
* This function checks if there is any pending notification from the SiI9777 chip,
* serves them if needed and may generate a SII9777_EVENT ‘notifications’ by
* calling SiiLibEvtCbFunc_t callback function registered through Sii9777Create().
*
* @param[in]  inst       Adapter driver instance returned by @ref Sii9777Handle()
*
******************************************************************************/
void Sii9777Handle( SiiInst_t inst );

/*****************************************************************************/
/**
* @brief Event Flags mask request
*
* Event mask that indicates on which events the driver calls SiiLibEvtCbFunc_t.
*
* @param[in]  inst        Driver instance returned by @ref Sii9777Create()
* @param[in]  pMask       event bit mask.
*
******************************************************************************/
void Sii9777EventFlagsMaskSet( SiiInst_t inst, Sii9777EventFlags_t* pEventFlags );
void Sii9777EventFlagsMaskGet( SiiInst_t inst, Sii9777EventFlags_t* pEventFlags );

/*****************************************************************************/
/**
* @brief Event Flags status inquiry
*
* Inquiries current outstanding events.
* Typically, only un-masked events maybe returned since masked events do
* get cleared upon event call-back of SiiLibEvtCbFunc_t.
*
* @param[in]  inst           Driver instance returned by @ref Sii9777Create()
* @param[in]  pEventFlags    event bit mask.
*
******************************************************************************/
void Sii9777EventFlagsStatusGet( SiiInst_t inst, Sii9777EventFlags_t* pEventFlags );

/*****************************************************************************/
/**
* @brief Event Flags clear request
*
* Clears internal event flag register. 
*
* @param[in]  inst           Driver instance returned by @ref Sii9777Create()
* @param[in]  pEventFlags    event bit mask.
*
******************************************************************************/
void Sii9777EventFlagsClear( SiiInst_t inst, Sii9777EventFlags_t* pEventFlags );

/*****************************************************************************/
/**
* @brief Firmware update initialization
*
* Erases the SPI flash memory that contains SiI9777’s on-chip processor firmware.
* This function also sets the firmware image’s byte offset to 0.
* It is mandatory to call Sii9777FirmwareUpdateInit() before Sii9777FirmwareUpdate().
*
* @param[in]  inst     Driver instance returned by @ref Sii9777Create()
*
******************************************************************************/
void Sii9777FirmwareInit( SiiInst_t inst );

/*****************************************************************************/
/**
* @brief Firmware update
*
* Updates SPI flash memory that contains SiI9777’s on-chip processor firmware and
* power-up default EDID configurations. Entire firmware binary may be updated in
* one go or in smaller chunks if the buffer size is smaller than the total size of
* firmware binary. Before calling this function again, when using smaller chunks,
* application processor must wait for a notification (viz., SII9777_EVENT__FLASH_DONE)
* from the driver that ensure previous operation has concluded. 
* It is mandatory to call Sii9777FirmwareUpdateInit() before Sii9777FirmwareUpdate().
*
* @param[in]  inst     Driver instance returned by @ref Sii9777Create()
* @param[in]  pBuffer  Pointer to a memory buffer containing firmware binary.
* @param[in]  length   Number of bytes in the firmware binary. Maximum is 256 bytes.
*
******************************************************************************/
void Sii9777FirmwareUpdate( SiiInst_t inst, const uint8_t* pData, uint16_t length );

/*****************************************************************************/
/**
* @brief Returns status of flash update operation.
*
* Returns status of flash update operation initiated by the last call to Sii9777FirmwareUpdate.
* This function is typically called upon 'SII9777_EVENT__ACCESS' notification.
* Once status function returns ‘FINISHED’ the firmware update operation has finished.
* User can now call ‘Sii9777Restart’ to execute new firmware version.
*
* @param[in]  inst      Driver instance returned by @ref Sii9777Create()
* @param[out] poStatus  Pointer to a variable in which the status information is returned:
*                       - @ref SII9777_FIRMWARE_STAT__READY        Flash Reading/writing is done successfully
*                       - @ref SII9777_FIRMWARE_STAT__IN_PROGRESS  Flash Reading/writing is in progress
*                       - @ref SII9777_FIRMWARE_STAT__FAILURE      Flash Reading/writing failed
*
******************************************************************************/
void Sii9777FirmwareStatusGet( SiiInst_t inst, Sii9777FlashStat_t* pFlashStat );

/*****************************************************************************/
/**
* @brief Restarts SiI9777 device.
*
* If firmware was updated then SiI9777 execute this new firmware version.
* Typically, this function is called after firmware has been updated.
*
* @param[in]  inst     Driver instance returned by @ref Sii9777Create()
*
******************************************************************************/
void Sii9777FirmwareRestart( SiiInst_t inst );

/*****************************************************************************/
/**
* @brief Video Split mode.
*
* Controls scheme of video split mode.
*
* @param[in]  inst  Driver instance returned by @ref Sii9777Create()
* @param[in]  mode  Video split mode:
*                   - @ref SII9777_SPLIT_MODE_LEFT_RIGHT   Video is splitted/merged by left/right half line
*                   - @ref SII9777_SPLIT_MODE_EVEN_ODD     Video is splitted/merged by even/odd pixel
*
******************************************************************************/
void Sii9777SplitModeSet( SiiInst_t inst, Sii9777SplitMode_t* pSplitMode );
void Sii9777SplitModeGet( SiiInst_t inst, Sii9777SplitMode_t* pSplitMode );

/*****************************************************************************/
/**
* @brief Chip ID inquiry
*
* Returns Chip ID.
*
* @param[in]  inst        Driver instance returned by @ref Sii9777Create().
* @param[out] poProductId Pointer to a variable in which the chip ID information
*                         is returned.
*
******************************************************************************/
void Sii9777ChipIdStatusGet( SiiInst_t inst, uint32_t* pChipIdStat );

/*****************************************************************************/
/**
* @brief Chip revision
*
* Returns Chip revision.
*
* @param[in]  inst           Driver instance returned by @ref Sii9777Create().
* @param[out] pChipRevStat   Pointer to a variable in which the chip revision information
*                            is returned.
*
*****************************************************************************/
void Sii9777ChipRevisionStatusGet( SiiInst_t inst, uint8_t* pChipRevStat );

/*****************************************************************************/
/**
* @brief Product ID
*
* Returns Product ID.
*
* @param[in]  inst        Driver instance returned by @ref Sii9777Create().
* @param[out] poProdId    sPointer to a variable in which the poduct ID information
*                         is returned.
*
*****************************************************************************/
void Sii9777ProductIdStatusGet( SiiInst_t inst, uint32_t* pProdIdStat );

/*****************************************************************************/
/**
* @brief Product ID
*
* Returns Product ID.
*
* @param[in]  inst        Driver instance returned by @ref Sii9777Create().
* @param[out] poVersion   Pointer to a variable in which the firmware version information
*                         is returned.
*
*****************************************************************************/
void Sii9777FirmwareVersionStatusGet( SiiInst_t inst, uint32_t* pVersionStat );

/*****************************************************************************/
/**
* @brief Firmware build time stamp
*
* Returns Firmware build time stamp.
*
* @param[in]  inst        Driver instance returned by @ref Sii9777Create().
* @param[out] pTimeStamp  Time stamp in number of seconds since 01/01/1970.
*
*****************************************************************************/
void Sii9777FirmwareTimeStampStatusGet( SiiInst_t inst, uint32_t* pTimeStamp );

/*****************************************************************************/
/**
* @brief Booting status inquiry
*
* Returns chip boot status.
*
* @param[in]  inst       Driver instance returned by @ref Sii9777Create()
* @param[out] poBootStat Pointer to a variable to return booting status
*                        - @ref SII9777_BOOT_STAT__SUCCESS       Booting is done successfully
*                        - @ref SII9777_BOOT_STAT__IN_PROGRESS   Booting is in progress
*                        - @ref SII9777_BOOT_STAT__FAILURE       Booting failed
*
******************************************************************************/
void Sii9777BootStatusGet( SiiInst_t inst, Sii9777BootStat_t *pBootStat );

#ifdef __cplusplus
}
#endif

#endif // SII9777_COMMON_H

