/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
#include "Si2151_I2C_API.h"
#include "sif_if.h"            // For SIF_NIMReadNoSubAddr & SIF_NIMWriteNoSubAddr
#include "x_assert.h"
//#include "PI_Def.h"
//#include "PD_Def.h"
//#include "ctrl_bus.h"
#include "i2c_api.h"
#include "tunerDebug.h"

//#define cI2C_TIMING           0x100
//#define cI2C_TIMING           1080//270
#define cMAX_I2C_LEN            32
#define cSi2151_I2C_TIMING      270// CLK = 27M/cSi2151_I2C_TIMING
/***********************************************************************/
/*              I2C Interface                                          */
/***********************************************************************/
/***********************************************************************/
/*                                                                     */
/*   FUNCTION DESCRIPTION                        u1I2cRead             */
/*                                                                     */
/*                                                                     */
/*   ROUTINES CALLED                                                   */
/*                                                                     */
/*   INPUTS                                                            */
/*      UINT8 u1RegAddr - Register Address to read                     */
/*      UINT8 u2ByteCount - number of bytes to read                    */
/*                                                                     */
/*   OUTPUTS                                                           */
/*      UINT8 *pu1Buffer - read data bytes are copied to this buffer   */
/*                                                                     */
/*   RETURN:                                                           */
/*    0 for success                                                    */
/*    non-zero for failure                                             */
/*                                                                     */
/***********************************************************************/

UINT8 SI2151_ucI2cReadOnly(UINT8 ucI2cAddr, UINT8 *pucBuffer, UINT16 ucByteCount)
{
	if (ICtrlBus_I2cTunerRead(cSi2151_I2C_TIMING, ucI2cAddr, pucBuffer, ucByteCount) != 0)
    {
        mcDBG_MSG_ERR(("ICtrlBus_TunerReadNoSubAddr failed------------------\n"));
        return 1;
    }
    return 0;
}

UINT8 SI2151_ucI2cWriteOnly(UINT8 ucI2cAddr, UINT8 *pucBuffer, UINT16 ucByteCount)
{
    if (ucByteCount > cMAX_I2C_LEN)
        mcDBG_MSG_L4(("%d exceed max I2C length! (%d)\n", ucByteCount, cMAX_I2C_LEN));
	if (ICtrlBus_I2cTunerWrite(cSi2151_I2C_TIMING, ucI2cAddr, pucBuffer, ucByteCount) != 0)
    {
        mcDBG_MSG_ERR(("ICtrlBus_TunerWriteNoSubAddr failed-----------------------\n"));
        return 1;
    }
    return 0;                               // TRUE
}