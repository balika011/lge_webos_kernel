#include "x_debug.h"
#include "../../sony_drv_inc/Sony_drv_cb.h"
#include "../../sony_drv_inc/htl_clk_if.h"
#include "htl_clk.h"
#include "sif_if.h"
#include "eeprom_if.h"
#include "drvcust_if.h"
#include "x_printf.h"
#include "x_os.h"
#include "x_dbg.h"

/* Definitions */
/*#define HTLCLK_DEBUG*/

#ifdef HTLCLK_DEBUG
#define HTLCLK_DBG(...) x_dbg_stmt(__VA_ARGS__)
#else
#define HTLCLK_DBG(...)
#endif

#define ON 1
#define OFF 0

/* Static Variables */
/* ui1HtlClkMode:
   0 for OFF, and 1 for ON
   Defaults to OFF
*/
static UINT8 ui1HtlClkMode = OFF;

/* Static Functions */
static UINT8 _pack2bcd(UINT8 ui1Value);
static UINT16 _HTL_CLK_WRITE(UINT8 ui1DevSubAddr, UINT8 *pui1Buf, UINT16 ui2ByteCnt);
static UINT16 _HTL_CLK_READ(UINT8 ui1DevSubAddr, UINT8 *pui1Buf, UINT16 ui2ByteCnt);

static UINT16 _crcCalcByte(UINT16 ui2CrcValue, UINT8 ui1Uc);
static UINT16 _calculateCrc(UINT8 *pui1DataBuffer, UINT16 ui2DataLen);
static UINT8 _crcChecked(UINT8* pui1DataBuffer, UINT16 ui2DataLen, UINT8 ui1Crc_1, UINT8 ui1Crc_2);

UINT16 aui2Crc[256] = {
    0x0000, 0x8005, 0x800f, 0x000a, 0x801b, 0x001e, 0x0014, 0x8011,
    0x8033, 0x0036, 0x003c, 0x8039, 0x0028, 0x802d, 0x8027, 0x0022,
    0x8063, 0x0066, 0x006c, 0x8069, 0x0078, 0x807d, 0x8077, 0x0072,
    0x0050, 0x8055, 0x805f, 0x005a, 0x804b, 0x004e, 0x0044, 0x8041,
    0x80c3, 0x00c6, 0x00cc, 0x80c9, 0x00d8, 0x80dd, 0x80d7, 0x00d2,
    0x00f0, 0x80f5, 0x80ff, 0x00fa, 0x80eb, 0x00ee, 0x00e4, 0x80e1,
    0x00a0, 0x80a5, 0x80af, 0x00aa, 0x80bb, 0x00be, 0x00b4, 0x80b1,
    0x8093, 0x0096, 0x009c, 0x8099, 0x0088, 0x808d, 0x8087, 0x0082,
    0x8183, 0x0186, 0x018c, 0x8189, 0x0198, 0x819d, 0x8197, 0x0192,
    0x01b0, 0x81b5, 0x81bf, 0x01ba, 0x81ab, 0x01ae, 0x01a4, 0x81a1,
    0x01e0, 0x81e5, 0x81ef, 0x01ea, 0x81fb, 0x01fe, 0x01f4, 0x81f1,
    0x81d3, 0x01d6, 0x01dc, 0x81d9, 0x01c8, 0x81cd, 0x81c7, 0x01c2,
    0x0140, 0x8145, 0x814f, 0x014a, 0x815b, 0x015e, 0x0154, 0x8151,
    0x8173, 0x0176, 0x017c, 0x8179, 0x0168, 0x816d, 0x8167, 0x0162,
    0x8123, 0x0126, 0x012c, 0x8129, 0x0138, 0x813d, 0x8137, 0x0132,
    0x0110, 0x8115, 0x811f, 0x011a, 0x810b, 0x010e, 0x0104, 0x8101,
    0x8303, 0x0306, 0x030c, 0x8309, 0x0318, 0x831d, 0x8317, 0x0312,
    0x0330, 0x8335, 0x833f, 0x033a, 0x832b, 0x032e, 0x0324, 0x8321,
    0x0360, 0x8365, 0x836f, 0x036a, 0x837b, 0x037e, 0x0374, 0x8371,
    0x8353, 0x0356, 0x035c, 0x8359, 0x0348, 0x834d, 0x8347, 0x0342,
    0x03c0, 0x83c5, 0x83cf, 0x03ca, 0x83db, 0x03de, 0x03d4, 0x83d1,
    0x83f3, 0x03f6, 0x03fc, 0x83f9, 0x03e8, 0x83ed, 0x83e7, 0x03e2,
    0x83a3, 0x03a6, 0x03ac, 0x83a9, 0x03b8, 0x83bd, 0x83b7, 0x03b2,
    0x0390, 0x8395, 0x839f, 0x039a, 0x838b, 0x038e, 0x0384, 0x8381,
    0x0280, 0x8285, 0x828f, 0x028a, 0x829b, 0x029e, 0x0294, 0x8291,
    0x82b3, 0x02b6, 0x02bc, 0x82b9, 0x02a8, 0x82ad, 0x82a7, 0x02a2,
    0x82e3, 0x02e6, 0x02ec, 0x82e9, 0x02f8, 0x82fd, 0x82f7, 0x02f2,
    0x02d0, 0x82d5, 0x82df, 0x02da, 0x82cb, 0x02ce, 0x02c4, 0x82c1,
    0x8243, 0x0246, 0x024c, 0x8249, 0x0258, 0x825d, 0x8257, 0x0252,
    0x0270, 0x8275, 0x827f, 0x027a, 0x826b, 0x026e, 0x0264, 0x8261,
    0x0220, 0x8225, 0x822f, 0x022a, 0x823b, 0x023e, 0x0234, 0x8231,
    0x8213, 0x0216, 0x021c, 0x8219, 0x0208, 0x820d, 0x8207, 0x0202
};

/* CRC Functions */
static UINT16 _crcCalcByte(UINT16 ui2CrcValue, UINT8 ui1Uc)
{
    return (((ui2CrcValue & 0xff) << 8) ^ aui2Crc[ui1Uc] ^ aui2Crc [(ui2CrcValue >> 8)]);
}

static UINT16 _calculateCrc(UINT8 *pui1DataBuffer, UINT16 ui2DataLen)
{
    UINT16 ui2N;
    UINT16 ui2CrcValue = 0xffff;
    for (ui2N = 0; ui2N < ui2DataLen; ui2N++) {
        ui2CrcValue = _crcCalcByte(ui2CrcValue, pui1DataBuffer[ui2N]);
    }
    return(ui2CrcValue);
}

static UINT8 _crcChecked(UINT8* pui1DataBuffer, UINT16 ui2DataLen, UINT8 ui1Crc_1, UINT8 ui1Crc_2)
{
    UINT16 ui2CrcValue;
    ui2CrcValue = _calculateCrc(pui1DataBuffer, ui2DataLen);
    return ((ui1Crc_1 == ((ui2CrcValue >> 8) & 0xff)) && ((ui1Crc_2 == (ui2CrcValue & 0xff))));
}

/* READ and WRITE functions */
static UINT16 _HTL_CLK_WRITE(UINT8 ui1DevSubAddr, UINT8 *pui1Buf, UINT16 ui2ByteCnt)
{
    UINT16 ui2WriteCount = 0;

    if(ON == ui1HtlClkMode)
    {
        ui2WriteCount = vSIFWrite(SIF_CLK_DIV, HTL_CLK_DEV_ADDR, ui1DevSubAddr, pui1Buf, ui2ByteCnt);
    }

    if(!ui2WriteCount)
    {
        HTLCLK_DBG("Error: _HTL_CLK_WRITE...\n");
    }
    return ui2WriteCount;
}

static UINT16 _HTL_CLK_READ(UINT8 ui1DevSubAddr, UINT8 *pui1Buf, UINT16 ui2ByteCnt)
{
    UINT16 ui2ReadCount = 0;

    if(ON == ui1HtlClkMode)
    {
        ui2ReadCount = vSIFRead(SIF_CLK_DIV, HTL_CLK_DEV_ADDR, ui1DevSubAddr, pui1Buf, ui2ByteCnt);
    }

    if(ui2ByteCnt != ui2ReadCount)
    {
        HTLCLK_DBG("Error: _HTL_CLK_READ...\n");
    }
    return ui2ReadCount;
}

/* Helper Functions */
static UINT8 _pack2bcd(UINT8 ui1Value)
{
    UINT8 a, b;
    a = ui1Value % 10;
    b = ui1Value / 10;

    return ((b << 4) | a);
}

/* Public Functions */
BOOL HTLCLK_Init(VOID)
{
    UINT8 aui1Buf[2]={0,0};
    BOOL bRet=FALSE;

    EEPROM_Read((UINT64)HTL_CLK_EEPROM_HTLCLK_MODEL, (UINT32)aui1Buf, 2);
    if(HTL_CLK_EEPROM_CHKSUM != aui1Buf[1]) /* Invalid value, we'll write the default into eeprom then */
    {
        UINT8 aui1WriteBuf[2] = { 0x00, HTL_CLK_EEPROM_CHKSUM };
        EEPROM_Write((UINT64)HTL_CLK_EEPROM_HTLCLK_MODEL, (UINT32)aui1WriteBuf, 2);
        ui1HtlClkMode = OFF;
    }
    else
    {
        ui1HtlClkMode = ((ON == aui1Buf[0]) ? ON : OFF);

        if(ON == ui1HtlClkMode)
        {
            UINT8 ui1Hour, ui1Minute, ui1Second, ui1DayOfWeek, ui1IsValid;
            int j = 0;

            while(3 > j++) /* Try retrieving time from hotel clock 3 times */
            {
                bRet = HTLCLK_GetTime(&ui1Hour, &ui1Minute, &ui1Second, &ui1DayOfWeek, &ui1IsValid);

                if(TRUE == bRet)
                {
                    break;
                }
            }

            if(FALSE == bRet)
            {
                HTLCLK_DBG("HTLCLK Init: I2C Error...!\n");
                ui1HtlClkMode = OFF;
                return FALSE;
            }
        }
    }
    return TRUE;
}
#define HTLCLK_CONFIG_LEN	4
#define HTLCLK_CONFIG_CRC_LEN	2
BOOL HTLCLK_Config(UINT8 ui1Display, UINT8 ui1Intensity, UINT8 ui1Reset)
{
    UINT8 aui1Buf[HTLCLK_CONFIG_LEN];
    UINT16 ui2WriteCount = 0;
    UINT16 ui2CrcValue;
    BOOL bRetVal = TRUE;

    aui1Buf[0] = ui1Display & 0x01;
    //aui1Buf[1] = ui1Intensity & 0x01;
    aui1Buf[1] = ui1Reset & 0x01;

    ui2CrcValue = _calculateCrc(aui1Buf, HTLCLK_CONFIG_CRC_LEN);

    aui1Buf[2] = (ui2CrcValue >> 8) & 0xff;
    aui1Buf[3] = (ui2CrcValue & 0xff);

    //HTLCLK_DBG("HTLCLK_Config: %x, %x, %x, %x, %x\n", aui1Buf[0], aui1Buf[1], aui1Buf[2], aui1Buf[3], aui1Buf[4]);
    HTLCLK_DBG("HTLCLK_Config: %x, %x, %x, %x\n", aui1Buf[0], aui1Buf[1], aui1Buf[2], aui1Buf[3]);

    ui2WriteCount = _HTL_CLK_WRITE(HTL_CLK_SUB_ADDR_CONFIG, aui1Buf, HTLCLK_CONFIG_LEN);

    if(!ui2WriteCount)
    {
        HTLCLK_DBG("HTLCLK_Config FAIL...\n");
        bRetVal = FALSE;
    }

    return bRetVal;
}

#define HTLCLK_SETTIME_LEN	9
#define HTCLK_TIME_CRC_LEN	7
BOOL HTLCLK_SetTime(UINT8 ui1Hour, UINT8 ui1Minutes, UINT8 ui1Seconds, UINT8 ui1Weekday)
{
    UINT8 aui1Buf[HTLCLK_SETTIME_LEN];
    UINT16 ui2WriteCount = 0;
    UINT16 ui2CrcValue;
    BOOL bRetVal = TRUE;

    aui1Buf[0] = _pack2bcd(ui1Hour);
    aui1Buf[1] = _pack2bcd(ui1Minutes);
    aui1Buf[2] = _pack2bcd(ui1Seconds);
    aui1Buf[3] = _pack2bcd(ui1Weekday);
    aui1Buf[4] = 0;
    aui1Buf[5] = 0;
    aui1Buf[6] = 0;

    ui2CrcValue = _calculateCrc(aui1Buf, HTCLK_TIME_CRC_LEN);

    aui1Buf[7] = (ui2CrcValue >> 8) & 0xff;
    aui1Buf[8] = (ui2CrcValue & 0xff);

    HTLCLK_DBG("HTLCLK_SetTime: %x, %x, %x, %x, %x, %x\n", aui1Buf[0], aui1Buf[1], aui1Buf[2], aui1Buf[3], aui1Buf[7], aui1Buf[8]);

    ui2WriteCount = _HTL_CLK_WRITE(HTL_CLK_SUB_ADDR_SET_TIME, aui1Buf, HTLCLK_SETTIME_LEN);

    if(!ui2WriteCount)
    {
        HTLCLK_DBG("HTLCLK_SetTime FAIL...\n");
        bRetVal = FALSE;
    }

    return bRetVal;
}

#define HTLCLK_GETTIME_LEN	12
#define HTCLK_VALID_CRC_LEN	1
//Hours[0], Min[1], sec[2], weekday[3], days[4], month[5], year[6], CRC time[7:8], TimeValid[9], Valid CRC[10:11]
BOOL HTLCLK_GetTime(UINT8 *pui1Hour, UINT8 *pui1Minutes, UINT8 *pui1Seconds, UINT8 *pui1Weekday, UINT8 *pui1TimeValid)
{
    UINT8 aui1Buf[HTLCLK_GETTIME_LEN];
    UINT16 ui2ReadCount = 0;
    BOOL bRetVal = FALSE;

    x_memset(aui1Buf, 0, sizeof(aui1Buf));

    ui2ReadCount = _HTL_CLK_READ(HTL_CLK_SUB_ADDR_READ_TIME, aui1Buf, HTLCLK_GETTIME_LEN);

    if(ui2ReadCount != HTLCLK_GETTIME_LEN)
    {
        HTLCLK_DBG("HTLCLK_GetTime FAIL...\n");
    }
    else if(!_crcChecked(aui1Buf, HTCLK_TIME_CRC_LEN, aui1Buf[7], aui1Buf[8]))
    {
        HTLCLK_DBG("HTLCLK_GetTime: Time CRC Error...\n");
    }
    else if(!_crcChecked((aui1Buf+9), HTCLK_VALID_CRC_LEN, aui1Buf[10], aui1Buf[11]))
    {
        HTLCLK_DBG("HTLCLK_GetTime:valid  CRC Error...\n");
	}
    else
    {
        *pui1Hour = ((((aui1Buf[0] >> 4) & 0x03) * 10) + (aui1Buf[0] & 0x0F));
        *pui1Minutes = ((((aui1Buf[1] >> 4) & 0x07) * 10) + (aui1Buf[1] & 0x0F));
        *pui1Seconds = ((((aui1Buf[2] >> 4) & 0x07) * 10) + (aui1Buf[2] & 0x0F));
        *pui1Weekday = (aui1Buf[3] & 0x07);
        *pui1TimeValid = (aui1Buf[9] & 0x01);
        bRetVal = TRUE;
    }
    return bRetVal;
}

#define HTLCLK_GETVER_LEN	4	
#define HTCLK_VER_CRC_LEN	2
//ver[0xc:0xd], CRC time[0xe:0xf]
BOOL HTLCLK_GetVersion(CHAR *pui1VersionStr)
{
    UINT8 aui1Buf[HTLCLK_GETVER_LEN];
    UINT16 ui2ReadCount = 0;
    UINT16 ui2Version;
    BOOL bRetVal = FALSE;

    ui2ReadCount = _HTL_CLK_READ(HTL_CLK_SUB_ADDR_GET_VERSION, aui1Buf, HTLCLK_GETVER_LEN);

    if(ui2ReadCount != HTLCLK_GETVER_LEN)
    {
        HTLCLK_DBG("HTLCLK_GetVersion FAIL...\n");
    }
    else if(!_crcChecked(aui1Buf, HTCLK_VER_CRC_LEN, aui1Buf[2], aui1Buf[3]))
    {
        HTLCLK_DBG("HTLCLK_GetVersion: CRC Error...\n");
    }
    else
    {
        ui2Version = ((aui1Buf[0] << 8) | (aui1Buf[1]));
        x_snprintf(pui1VersionStr, 9, "CM %01d.%03d", ui2Version/1000, ui2Version%1000);
        bRetVal = TRUE;
    }

    return bRetVal;
}
VOID HTLCLK_SetMode(UINT8 u1OnOff)
{
    UINT8 aui1WriteBuf[2];
    if (u1OnOff)
    {
        aui1WriteBuf[0] = 0x01;
        aui1WriteBuf[1] = HTL_CLK_EEPROM_CHKSUM;
    }
    else
    {
        aui1WriteBuf[0] = 0x00;
        aui1WriteBuf[1] = HTL_CLK_EEPROM_CHKSUM;
    }
    EEPROM_Write((UINT64)HTL_CLK_EEPROM_HTLCLK_MODEL, (UINT32)aui1WriteBuf, 2);
}
BOOL HTLCLK_GetMode(UINT8 *pui1Mode)
{
    *pui1Mode = (ui1HtlClkMode == OFF) ? 0 : 1;
    return TRUE;
}

