#ifdef I2C_LIGHT_SENSOR
#include "x_debug.h"
#include "sif_if.h"		
#include "CustomCtrl.h"

//-----------------------------------------------------------------------------
// Prototypes 
//-----------------------------------------------------------------------------
extern void CM3217_Setup(UINT8 u1Gain, UINT8 u1ITTimes, UINT8 u1FDIT);

//mapping to 300 Lux
#define SensorMax_E		184//276 // E type
#define SensorMax_C		184//230 // C type
#define SensorMax_P		46	// P type
#define SensorMax_L		184//230	// L type
#define LS_SIF_CLK_DIV     0x120 //0x100
#define LS_SIF_BUS              SIF_BUS_SECOND_I2C
UINT16 ui2MaxStep=46;  //Ap access the range 0~46

UINT32 u4ModelIndex;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
VOID CM3217_Init(void)
{
    //atm-liang add for light sensor : begin
    UINT8 buff;
    UINT16 uiRet;

    u4ModelIndex = CustomModelOptionQueryFunc();
	
    switch (u4ModelIndex)
    {
        case DRV_CUSTOM_MODEL_OPTION_P:
            buff = 0x2E;
    	    uiRet = SIF_X_Write(LS_SIF_BUS, LS_SIF_CLK_DIV,0x20,0, 0,&buff,1);
            if(uiRet)
                Printf("[GPIO_IbizaPowerOnFunc] Light sensor enter normal mode - successful!!\n");
            else
                Printf("[GPIO_IbizaPowerOnFunc] Light sensor enter normal mode - failed!!\n");

            buff = 0x60;
            uiRet = SIF_X_Write(LS_SIF_BUS, LS_SIF_CLK_DIV,0x22,0, 0,&buff,1);
            if(uiRet)
                Printf("[GPIO_IbizaPowerOnFunc] Light sensor write 0x22 successful!!\n");
            else
                Printf("[GPIO_IbizaPowerOnFunc] Light sensor write 0x22 error!!\n");
            break;

        default:
            buff = 0x26;
            uiRet = SIF_X_Write(LS_SIF_BUS, LS_SIF_CLK_DIV,0x20,0, 0,&buff,1);
            if(uiRet)
                Printf("[GPIO_IbizaPowerOnFunc] Light sensor enter normal mode - successful!!\n");
            else
                Printf("[GPIO_IbizaPowerOnFunc] Light sensor enter normal mode - failed!!\n");

            buff = 0x20;
            uiRet = SIF_X_Write(LS_SIF_BUS, LS_SIF_CLK_DIV,0x22,0, 0,&buff,1);
            if(uiRet)
                Printf("[GPIO_IbizaPowerOnFunc] Light sensor write 0x22 successful!!\n");
            else
                Printf("[GPIO_IbizaPowerOnFunc] Light sensor write 0x22 error!!\n");
            break;
    }
}

UINT8 Get_CM3217_Step(void)
{
    static BOOL bInit = TRUE;
    UINT8 CM3217_value_MSB, CM3217_value_LSB, Step;	
    UINT16 CM3217_value_16bit, uiRet;
			
    if (bInit)
    {
        CM3217_Init();
        bInit = FALSE;
    }

    //Read CM3217 LSB data	
    uiRet = SIF_X_Read(LS_SIF_BUS,LS_SIF_CLK_DIV,0x22, 0, 0,&CM3217_value_LSB,1);	
    if(uiRet==0)        return 0xff;
					
    //Read CM3217 MSB data	
    uiRet = SIF_X_Read(LS_SIF_BUS,LS_SIF_CLK_DIV,0x20, 0, 0,&CM3217_value_MSB,1);	
    if(uiRet==0)        return 0xff;

    CM3217_value_16bit = (UINT16)(CM3217_value_MSB<<8)|((UINT16)CM3217_value_LSB);	
    //Printf("[LightSensor] Light Sensor Value = %d\n",CM3217_value_16bit);		

    switch (u4ModelIndex)
    {
        case DRV_CUSTOM_MODEL_OPTION_E:
            if(CM3217_value_16bit < SensorMax_E) 
            {	
                Step = CM3217_value_16bit/(SensorMax_E/ui2MaxStep);
                return Step;
            }
            break;
        case DRV_CUSTOM_MODEL_OPTION_C:
            if(CM3217_value_16bit < SensorMax_C) 
            {
                Step = CM3217_value_16bit/(SensorMax_C/ui2MaxStep);
                return Step;
            }
            break;
        case DRV_CUSTOM_MODEL_OPTION_P:
            if(CM3217_value_16bit < SensorMax_P) 
            {
                Step = CM3217_value_16bit/(SensorMax_P/ui2MaxStep);
                return Step;
            }
            break;
        case DRV_CUSTOM_MODEL_OPTION_L:
            if(CM3217_value_16bit < SensorMax_L) 
            {
                Step = CM3217_value_16bit/(SensorMax_L/ui2MaxStep);
                return Step;
            }
            break;	
        default:
            if(CM3217_value_16bit < 184) //Ibiza default max value  = 184, mapping to 300 Lux
            {
                Step = CM3217_value_16bit/4;
                return  Step;
            }
            break;
    }

    return ui2MaxStep;
}
// function to setup 3 registers of light sensor
//gain: 0:  /1
//      1:  /2
//      2:  /4
//      3:  /8
//ITTimes: 0:  1/2 T
//         1:    1 T
//         2:    2 T
//         3:    4 T
//FD_IT: 0: 800 ms
//       1: 400 ms
//       2: 266 ms
//       3: 200 ms
//       4: 130 ms
//       5: 100 ms
//       6:  80 ms
//       7:  66 ms
void CM3217_Setup(UINT8 u1Gain, UINT8 u1ITTimes, UINT8 u1FDIT)
{
    UINT8 u1Buf = 0;
    UINT16 u2Ret = 0;

    UINT16 u2FinalIT = 0;
    //boundary check
    if (u1Gain > 3)
    {
        u1Gain = 3;
    }
    if (u1ITTimes > 3)
    {
        u1ITTimes = 3;
    }
    if (u1FDIT > 7)
    {
        u1FDIT = 7;
    }
    printf ("set gain: %d, IT_Times %d, FD_ID %d\n", u1Gain, u1ITTimes, u1FDIT);
    
    //setup register
    u1Buf = 0x22;   //Reserve = 10, WDM = 1, SD = 0
    u1Buf |= (u1Gain << 6);
    u1Buf |= (u1ITTimes << 2);
    
    u2Ret = SIF_X_Write(LS_SIF_BUS, LS_SIF_CLK_DIV,0x20,0, 0,&u1Buf,1);
    if (u2Ret == 0)
    {
        Printf ("write light sensor 0x20 fail\n");
    }
    
    u1Buf = 0;
    u1Buf |= (u1FDIT << 5);
    u2Ret = SIF_X_Write(LS_SIF_BUS, LS_SIF_CLK_DIV,0x22,0, 0,&u1Buf,1);
    if (u2Ret == 0)
    {
        Printf ("write light sensor 0x22 fail\n");
    }
    switch (u1FDIT)
    {
        case 0: u2FinalIT = 800; break;
        case 1: u2FinalIT = 400; break;
        case 2: u2FinalIT = 266; break;
        case 3: u2FinalIT = 200; break;
        case 4: u2FinalIT = 130; break;
        case 5: u2FinalIT = 100; break;
        case 6: u2FinalIT =  80; break;
        case 7: u2FinalIT =  66; break;
        default: u2FinalIT = 0; break;
    }
    switch (u1ITTimes)
    {
        case 0: u2FinalIT /= 2; break;
        case 1: break;
        case 2: u2FinalIT *= 2; break;
        case 3: u2FinalIT *= 4; break;
        default: u2FinalIT = 0; break;
    }

    printf ("gain: 1/%d, IT: %d\n", (1 << u1Gain), u2FinalIT);
}
// a function to setup integration time related registers, and leave just 1 parameter
// gain is fixed
void CM3217_Setup_Simple(UINT8 u1Id)
{
    UINT8 u1Gain = 0, u1ITTimes = 0,u1FDIT = 7;
    switch (u1Id)
    {
        case 0:  u1ITTimes = 3; u1FDIT = 0; break;
        case 1:  u1ITTimes = 3; u1FDIT = 1; break;
        case 2:  u1ITTimes = 3; u1FDIT = 2; break;
        case 3:  u1ITTimes = 3; u1FDIT = 3; break;
        case 4:  u1ITTimes = 2; u1FDIT = 2; break;
        case 5:  u1ITTimes = 3; u1FDIT = 4; break;
        case 6:  u1ITTimes = 3; u1FDIT = 5; break;
        case 7:  u1ITTimes = 3; u1FDIT = 6; break;
        case 8:  u1ITTimes = 3; u1FDIT = 7; break;
        case 9:  u1ITTimes = 2; u1FDIT = 5; break;
        case 10: u1ITTimes = 2; u1FDIT = 6; break;
        case 11: u1ITTimes = 2; u1FDIT = 7; break;
        case 12: u1ITTimes = 1; u1FDIT = 5; break;
        case 13: u1ITTimes = 1; u1FDIT = 6; break;
        case 14: u1ITTimes = 1; u1FDIT = 7; break;
        case 15: u1ITTimes = 0; u1FDIT = 5; break;
        case 16: u1ITTimes = 0; u1FDIT = 6; break;
        case 17: u1ITTimes = 0; u1FDIT = 7; break;
    }
    CM3217_Setup(u1Gain, u1ITTimes, u1FDIT);
}
//direct return value from light sensor
UINT16 CM3217_ReadNative(void)
{
    UINT8 u1MSB, u1LSB;	
    UINT16 u2Ret;

    //Read CM3217 LSB data	
    u2Ret = SIF_X_Read(LS_SIF_BUS, LS_SIF_CLK_DIV,0x22, 0, 0,&u1LSB,1);	
    if(u2Ret==0)
    {
        return 0xff;
    }
					
    //Read CM3217 MSB data	
    u2Ret = SIF_X_Read(LS_SIF_BUS, LS_SIF_CLK_DIV,0x20, 0, 0,&u1MSB,1);	
    if(u2Ret==0)
    {
        return 0xff;
    }
    u2Ret = u1MSB;
    u2Ret <<= 8;
    u2Ret |= u1LSB;
    return u2Ret;
}
#elif defined(ADC_LIGHT_SENSOR)
#include "x_debug.h"
extern UINT32 PDWNC_ReadServoADCChannelValue(UINT32 u4Channel);

#define LIGHT_SENSOR_DRIVER_MAX_VALUE (46)

UINT8 Custom_LightSensor_Gain(UINT8* pui1_LightSensor_Gain)
{
    static UINT8 ui1_LightSensor_Gain = 1;

    if(NULL == pui1_LightSensor_Gain)
    {
        //get function
        return ui1_LightSensor_Gain;
    }
    else
    {
        //set function
        ui1_LightSensor_Gain = *(pui1_LightSensor_Gain);
        return 1;
    }
    //printf("\n=====ui1_LightSensor_Gain=====%d\n",ui1_LightSensor_Gain); //Jim 
}

UINT8 Custom_LightSensor_Offset(UINT8* pui1_LightSensor_Offset)
{
    static UINT8 ui1_LightSensor_Offset = 0;

    if(NULL == pui1_LightSensor_Offset)
    {
        //get function
        return ui1_LightSensor_Offset;
    }
    else
    {
        //set function
        ui1_LightSensor_Offset = *(pui1_LightSensor_Offset);
        return 1;
    }
    //printf("\n=====ui1_LightSensor_Offset=====%d\n",ui1_LightSensor_Offset); //Jim 
}



VOID CM3217_Init(void)
{
    static BOOL bInit = TRUE;
    UINT8 u4Value=1;

    if (bInit)
    {
  
        Custom_LightSensor_Gain(&u4Value);
        Custom_LightSensor_Offset(&u4Value);
        bInit = FALSE;
    }
//    Printf ("======Light sensor, ADC type\n===========");
}

UINT8 Get_CM3217_Step(void)
{
    //Jim modify for ADC light sensor
    UINT32 ui2ADCvalue;
    UINT32 ui2ADCGain; 
    UINT32 ui2ADCOffset;

    CM3217_Init();

    ui2ADCGain=Custom_LightSensor_Gain(NULL);
    ui2ADCOffset=Custom_LightSensor_Offset(NULL);
    ui2ADCvalue=(PDWNC_ReadServoADCChannelValue(7)*ui2ADCGain)/10+ui2ADCOffset;

//    printf("\n=====ui1_LightSensor_Gain=====%d\n",ui2ADCGain); //Jim 
//    printf("\n=====ui1_LightSensor_Offset=====%d\n",ui2ADCOffset); //Jim 

    if(ui2ADCvalue > LIGHT_SENSOR_DRIVER_MAX_VALUE)
        ui2ADCvalue = LIGHT_SENSOR_DRIVER_MAX_VALUE;

    return ui2ADCvalue;
}
#else
#include "x_debug.h"

VOID CM3217_Init(void)
{
    Printf ("Light sensor, null type for demo board\n");
}

UINT8 Get_CM3217_Step(void)
{
    //do nothing now
    return 46;
}

#endif
