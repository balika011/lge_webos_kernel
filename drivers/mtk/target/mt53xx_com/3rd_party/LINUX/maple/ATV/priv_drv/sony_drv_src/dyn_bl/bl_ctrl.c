/* ========================================================================== */
/*                                                                            */
/*   bl_ctrl.c                                                               */
/*   (c) 2009 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */
#include "x_dbg.h"
#include "x_os.h"
#include "x_assert.h"
#include "x_typedef.h"
#include "acfg_atv_eep.h"
#include "ostg_if.h"
#include "eeprom_if.h"
#include "sony_priv_inc/utility.h"
#include "sony_priv_inc/bl_reg.h"
#include "sony_priv_inc/bl_ctrl.h"
#include "sony_drv_inc/temp_setup_if.h"

//-----------------------------------------------------------------------------
//                              CONSTANT DEFINITIONS
//-----------------------------------------------------------------------------
#if defined (DBL_ENABLE_LOG)
#define DBL_LOG(fmt...)         \
get_time_reference();           \
x_dbg_stmt(fmt)
#else
#define DBL_LOG(fmt...)               
#endif

#define DYN_BL_THREAD_STACK_SIZE    1024
#define DYN_BL_THREAD_PRIORITY      100

#define BL_BOTTOM_MAX       255

#define EMG_NORMAL     0
#define EMG_DECREASE   1
#define EMG_LOWEST     2

#define LIGHT_SENSOR_OFF    0
#define LIGHT_SENSOR_ON     1

#define DIMMER_AVG_COUNT 10

const UINT8 DIMMER_MAX = 0xFF;
const UINT8 USER_DATA_INVALID = 0xFF;

//-----------------------------------------------------------------------------
//                              TYPE/ENUM DEFINITIONS
//-----------------------------------------------------------------------------
    UINT8  m_userBackLight;      // UserBackLight Level
    UINT8  m_ecoMode;            // EcoMode
    UINT8  m_userBLChange;        // UserBackLightChangeFlag
    UINT8  m_dimmer;             // OutputDimmer
    UINT8  m_normalBLCount;      // NormalBackLightCount
    UINT16 m_temperature;       // Temperature
    UINT8  m_isTempSensorOn;      // TempSensor On/Off Flag
    UINT8  m_beforeDimmer;       // BeforeDimmer
    UINT8  m_NotifyDimmerOnFlag;  // NotifyDimmer on flag
    UINT8  m_beforeTemp;         // BeforeTemp
    UINT8  m_AvgBaseData[DIMMER_AVG_COUNT];  // Data to calculate average
    UINT8  m_AvgBaseCount;       // Number of average data
    UINT8  m_AvgDataCompFlag;     // Average data acquisition completion flag

enum {
    UI_ECO_MODE_OFF,
    UI_ECO_MODE_LOW,
    UI_ECO_MODE_HIGH = 3,
    UI_ECO_MODE_PIC_OFF
};

enum {
    UI_LIGHT_SENSOR_OFF,
    UI_LIGHT_SENSOR_ON
};
//-----------------------------------------------------------------------------
//                              GLOBAL VARIABLE
//-----------------------------------------------------------------------------
UINT8 setDimmerCalculation(UINT8 temp);
void mainDisposition(void); 
void setTemperatureUpDate(void);
//-----------------------------------------------------------------------------
//                              STATIC VARIABLE
//-----------------------------------------------------------------------------
static VOID vUserBacklightUpdate(DYN_BL_MEMBER_T* pt_this);
static VOID _Dyn_BL_DetectThread(VOID);
static VOID _Dyn_BlCtrlTimeout(HANDLE_T  h_timer, VOID *pv_tag);
static DYN_BL_MEMBER_T t_dyn_bl;

#if defined (DBL_ENABLE_LOG)
void get_time_reference(void)
{
    INT32 time_reference;
    time_reference = x_os_get_sys_tick() * x_os_get_sys_tick_period();
    x_dbg_stmt("DBL[%d]-",time_reference);
}
#endif

void set_mimas_variable_bridge(DYN_BL_MEMBER_T *dyn_bl_sreg)
{
	int i;
	
	dyn_bl_sreg->ecoMode = &m_ecoMode;	 
	dyn_bl_sreg->userBackLight = &m_userBackLight;
	
	m_userBackLight = 0;
	m_ecoMode = 0;
	m_userBLChange = 0;        
	m_dimmer = 0;             
	m_normalBLCount = 0;      
	m_temperature = 0;       
	m_isTempSensorOn = 1;     
	m_beforeDimmer = 0;       
	m_NotifyDimmerOnFlag = 0;  
	m_beforeTemp = 0;         
	m_AvgBaseCount = 0;       
	m_AvgDataCompFlag = 0;     

	for(i=0;i<DIMMER_AVG_COUNT;i++)
	{
		m_AvgBaseData[i] = 0;  
	}

	m_isTempSensorOn = 1;
	m_beforeDimmer = 0xFF;
	m_NotifyDimmerOnFlag = TRUE;
}

/*
 * Get power saving status from Application
 * @param eco_mode specify Power saving status
 * @return
 */
VOID vPwrSavingStatus(UINT8 ui1_value)
{
    UINT8 eco_mode;   
    
    switch (ui1_value)
    {
        case UI_ECO_MODE_LOW:
            eco_mode = ECO_DECREASE1;
            break;	
        
        case UI_ECO_MODE_HIGH:
            eco_mode = ECO_DECREASE2;
            break;	
        
        case UI_ECO_MODE_PIC_OFF:
            eco_mode = ECO_BLACKOUT;
            break;
        
        case UI_ECO_MODE_OFF:
            default:
            eco_mode = ECO_NORMAL;
            break;
    }
    if(*t_dyn_bl.ecoMode != eco_mode)
    {
        *t_dyn_bl.ecoMode = eco_mode;
        vUserBacklightUpdate(&t_dyn_bl);
    }
}

/*
 * Get user backlight lvl from Application
 * @param eco_mode specify Power saving status
 * @return
 */
void vUserBacklightLvl(UINT8 ui1_value)
{  
    if(*t_dyn_bl.userBackLight != ui1_value)
    {
        *t_dyn_bl.userBackLight = ui1_value;
        vUserBacklightUpdate(&t_dyn_bl);
    }
}

/*
 * Check Backlight Update from User
 * @param 
 * @return
 */
void vUserBacklightUpdate(DYN_BL_MEMBER_T* pt_this)
{
    if (pt_this->h_dynBlThreadSema != NULL_HANDLE)
    {
      pt_this->dbl_event_ui_change = TRUE;
      SignalSemaphore(pt_this->h_dynBlThreadSema);
    }
}

/*
 * Check Light Sensor On/Off Status
 * @param 
 * @return
 */
void vLightSensorStatus(UINT8 ui1_value)
{
    UINT8 lightSensor;
    
    switch (ui1_value)
    {   
        case UI_LIGHT_SENSOR_ON:
            lightSensor = LIGHT_SENSOR_ON;
            break;
            
        case UI_LIGHT_SENSOR_OFF:
        default:
            lightSensor = LIGHT_SENSOR_OFF;
            break;
    }

    bl_sreg_write(SREG_OPT_BL_SENSOR, lightSensor);
}

/*
 * Dynamic backlight control timeout
 * @param pi2Value specify value of user backlight 0~10
 * @return
 */
static VOID _Dyn_BlCtrlTimeout(HANDLE_T  h_timer, VOID *pv_tag)
{
    DYN_BL_MEMBER_T* pt_this = &t_dyn_bl;
    
    if (pt_this->h_dynBlThreadSema != NULL_HANDLE)
    {
      pt_this->dbl_event_timer_timeout = TRUE;
      SignalSemaphore(pt_this->h_dynBlThreadSema);
    }
}

/*
 * Dynamic backlight thread
 * @param 
 * @return
 */
static VOID _Dyn_BL_DetectThread(VOID)
{
    UINT64 i4_ret;
    UINT32 wait_time;
		        
    DYN_BL_MEMBER_T* pt_this = &t_dyn_bl;

    mainDisposition();
    while(1)
    {   
        wait_time = 500;
        i4_ret = x_timer_start(pt_this->h_dynBlTimer,
                             wait_time,
                             X_TIMER_FLAG_ONCE,
                             _Dyn_BlCtrlTimeout,
                             NULL);
        if(i4_ret)
        {
        #ifdef DBL_ENABLE_LOG
            DBL_LOG("DBL Error Handler needed?? \n");
        #endif                
        }
                     
        WaitSemaphore(pt_this->h_dynBlThreadSema);
        
        if(pt_this->dbl_event_ui_change == TRUE)
        {
            m_userBLChange = 1;
            pt_this->dbl_event_ui_change = FALSE;
        }
        if(pt_this->dbl_event_timer_timeout == TRUE)
        {
            setTemperatureUpDate();
            pt_this->dbl_event_timer_timeout = FALSE;
        }
        mainDisposition();

    }
}

/*
 * Dynamic backlight control startup
 * @param 
 * @return
 */
VOID Dyn_BacklightCtrl_Init(VOID)
{
    DYN_BL_MEMBER_T* pt_this = &t_dyn_bl; 
   
    x_memset(pt_this, 0, sizeof(DYN_BL_MEMBER_T));
    set_mimas_variable_bridge(&t_dyn_bl);
    pt_this->dbl_event_ui_change = TRUE;
    CreateMutex(pt_this->h_blSregMutex);
    CreateSemaphore(pt_this->h_dynBlThreadSema);
    set_initial_sreg();
    VERIFY(x_timer_create(&pt_this->h_dynBlTimer) == OSR_OK);
    VERIFY(x_thread_create(&pt_this->h_dynBlCtrlThread, "DBLHDL", DYN_BL_THREAD_STACK_SIZE, DYN_BL_THREAD_PRIORITY, (x_os_thread_main_fct)_Dyn_BL_DetectThread, 0, NULL) == OSR_OK);
}

/*
 *  Set Temp Update
 * @param 
 * @return
 */
void setTemperatureUpDate(void) {
    UINT16 readTemp;

#if 0
    /* TEMP DATA SET */
    readTemp = m_pDevTempSensor->ReadTemp();
    if( readTemp & 0x8000 ) {
        /* The freezing point under */
        readTemp = 0;
    } else {
        readTemp = ( readTemp >> 7 ) * 5;
    }
    bl_sreg_write(SREG_TEMPERATURE_DATA, readTemp);
#endif
	readTemp = vAPIGetTempSensorValue(); 
    m_temperature = readTemp;
}

/*
 *  mainDisposition(void)
 * @param 
 * @return
 */
void mainDisposition(void) 
{

    UINT8 nowTemp = 0, tmpBlMidKnee = 0, tmpTgt = 0, setTgt = 0;
    UINT8 tempThLow = 0, tempThHigh = 0, tempBlLimit = 0;
    float tmpBlTop = 0, tmpBlBtm = 0;
    float tmpBlMid = 0, tmpTotal = 0;

#ifdef DBL_ENABLE_LOG
    DBL_LOG("mainDisposition START\n");
#endif
    /* UserData check */
    if( ( m_userBackLight == USER_DATA_INVALID ) || ( m_ecoMode == USER_DATA_INVALID ) ) {
        return;
    }

    if( m_userBLChange == 1 ) {
        /* ECO Mode Select */
        if( ( m_ecoMode == ECO_NORMAL ) || ( m_ecoMode == ECO_DECREASE1 ) ) {
            tmpBlTop        = bl_sreg_read(SREG_BL_TOP);
            tmpBlBtm        = bl_sreg_read(SREG_BL_BOTTOM);
            tmpBlMid        = bl_sreg_read(SREG_BL_MID);
            tmpBlMidKnee    = bl_sreg_read(SREG_BL_MID_KNEE);
            /* Max Over Check */
            if( BL_BOTTOM_MAX < tmpBlBtm ) {
                tmpBlBtm = BL_BOTTOM_MAX;
            }
            if( ( tmpBlMidKnee == 0 ) || ( tmpBlMidKnee == 10 ) ) {
                tmpTotal = ( ( tmpBlTop - tmpBlBtm ) / 10 ) * m_userBackLight + tmpBlBtm;
            } else if ( ( 0 < tmpBlMidKnee ) && ( tmpBlMidKnee < 10 ) ) {
                if( m_userBackLight < tmpBlMidKnee ) {
                    tmpTotal = ( ( tmpBlMid - tmpBlBtm ) / tmpBlMidKnee ) * m_userBackLight + tmpBlBtm;
                } else {
                    tmpTotal = ( ( tmpBlTop - tmpBlMid ) / (10 - tmpBlMidKnee) ) 
                               * (m_userBackLight - tmpBlMidKnee) + tmpBlMid;
                }
            }
            if( m_ecoMode == ECO_NORMAL ){
                m_dimmer = tmpTotal;
            } else {
                if( tmpTotal <= bl_sreg_read(SREG_BL_SETUDEN) ){
                    m_dimmer = 0;
                } else {                
                    m_dimmer = tmpTotal - bl_sreg_read(SREG_BL_SETUDEN);                      
                }
            }
        } else if( m_ecoMode == ECO_DECREASE2 ) {
            if( (bl_sreg_read(SREG_BL_PSH)) <= DIMMER_MAX ) {
                m_dimmer = bl_sreg_read(SREG_BL_PSH);
            } else {
                m_dimmer = DIMMER_MAX;
            }
        } else if( m_ecoMode == ECO_BLACKOUT ) {
            m_dimmer = 0;
        } else {
#if 0
            /* ECO_PICTUREFRAME */
            if(GetModelId()->destination == SM_MODEL_ID_REGION_AEP) {
                if( (bl_sreg_read(SREG_BL_PSH)) <= DIMMER_MAX ) {
                    m_dimmer = bl_sreg_read(SREG_BL_PSH);
                } else {
                    m_dimmer = DIMMER_MAX;
                }
            } else {
                TDEBUG_PRINTF((SM_LOG_DETAIL, "Not AEP Model\n"));
            }
#endif
        }
        m_normalBLCount = m_dimmer;
        bl_sreg_write(SREG_NORMAL_BRIGHT_CONT, m_dimmer);
    }
    tempThLow = bl_sreg_read(SREG_TEMP_TH_LOW);
    tempThHigh = bl_sreg_read(SREG_TEMP_TH_HIGH);
    /* TEMP DATA SET */
    nowTemp = (UINT8)( m_temperature / 10 );
    //DBL_LOG("Temp Value=%d\n", nowTemp);
    if( tempThLow > nowTemp ) {
        setTgt = tempThLow;
    } else if(tempThHigh < nowTemp ) {
        setTgt = tempThHigh;
    } else {
        setTgt = nowTemp;
    }
    tmpTgt = setDimmerCalculation( setTgt );
    bl_sreg_write( SREG_BL_TEMP_TGT,tmpTgt );

    if( tempThHigh <= nowTemp ) {
        /* EMG LOWEST MODE */
        bl_sreg_write( SREG_EMG_MODE, EMG_LOWEST );
        tempBlLimit = bl_sreg_read(SREG_TEMP_BL_LIMIT);
        if( m_normalBLCount > tempBlLimit ) {
            m_dimmer = tempBlLimit;
        } else {
            m_dimmer = m_normalBLCount;
        }
    } else {
        if( tempThLow > nowTemp ) {
            /* EMG NORMAL MODE */
            bl_sreg_write( SREG_EMG_MODE, EMG_NORMAL );
        } else {
            /* EMG DECREASE MODE */
            bl_sreg_write( SREG_EMG_MODE, EMG_DECREASE );
        }
        if( m_dimmer <= tmpTgt ) {
            bl_sreg_write( SREG_BL_TEMP_CNT, (bl_sreg_read(SREG_BL_TEMP_CNT)+1) );
            if( bl_sreg_read(SREG_BL_TEMP_CNT) >= bl_sreg_read(SREG_BL_TEMP_CNTTH) ) {
                bl_sreg_write( SREG_BL_TEMP_CNT, 0 );
                if( (tmpTgt - m_dimmer) > bl_sreg_read(SREG_BL_TEMP_INC) ) {
                    if( (m_dimmer + bl_sreg_read(SREG_BL_TEMP_INC) ) > DIMMER_MAX ) {
                        m_dimmer = DIMMER_MAX;
                    } else {
                        m_dimmer += bl_sreg_read(SREG_BL_TEMP_INC);
                    }
                } else {
                    m_dimmer = tmpTgt;
                }
                if( m_normalBLCount < m_dimmer ){
                    m_dimmer = m_normalBLCount;
                }
            }
        } else {
            bl_sreg_write( SREG_BL_TEMP_CNT, 0 );
            m_dimmer = tmpTgt;
        }
    }

    /* UserBackLightChange ON? */
    if( m_userBLChange == 1 ) {
        m_userBLChange = 0;
    }
    /* TempSensor ON ? */
    if( ( m_isTempSensorOn == 1 ) && 
      ( ( m_dimmer != m_beforeDimmer ) || ( m_NotifyDimmerOnFlag == TRUE ) ) ) {
        bl_sreg_write(SREG_LCD_BACKLIGHT_CONTROL, m_dimmer);
      #ifdef DBL_ENABLE_LOG
        DBL_LOG("mainDisposition Reply Dimmer:[%d]\n", m_dimmer);
      #endif
#if 0
        if(m_tempSensorReq) {
            m_tempSensorReq->Reply( new SmSensorTempDimmerReplyMsg( m_dimmer ) );
        }
#endif
        m_NotifyDimmerOnFlag = FALSE;
    }
    /* BeforeDimmer Set */
    m_beforeDimmer = m_dimmer;
    /* BeforeTemp Set */
    m_beforeTemp = nowTemp;
    //DBL_LOG("mainDisposition END\n");
}

/*
 *  setDimmerCalculation(UINT8 temp) 
 * @param 
 * @return
 */
UINT8 setDimmerCalculation(UINT8 temp) 
{
    UINT8 i;
    UINT16 dimmerTotal;
    float dimmer = 0,tmpBlBtm = 0, tmpBlThHigh = 0, tmpBlThLow = 0, tmpNormalBlCount = 0;
    tmpNormalBlCount = m_normalBLCount;
    tmpBlBtm = bl_sreg_read(SREG_TEMP_BL_BOTTOM);
    tmpBlThLow = bl_sreg_read(SREG_TEMP_TH_LOW);
    tmpBlThHigh = bl_sreg_read(SREG_TEMP_TH_HIGH);

    //DBL_LOG("setDimmerCalculation START\n");
    dimmer = ( tmpBlBtm - tmpNormalBlCount ) /
               ( tmpBlThHigh - tmpBlThLow ) *
                 ( temp - tmpBlThLow ) + tmpNormalBlCount;
    if( m_userBLChange == 1 ) {
        /* UserBackLightChange ON */
        for(i = 0; i < DIMMER_AVG_COUNT ; i++){
            m_AvgBaseData[i] = 0;
        }
        m_AvgBaseCount      = 0;
        m_AvgDataCompFlag   = 0;
        m_AvgBaseData[m_AvgBaseCount] = dimmer;
        m_AvgBaseCount++;
    } else {
        /* UserBackLightChange OFF */
        m_AvgBaseData[m_AvgBaseCount] = dimmer;
        m_AvgBaseCount++;
        if( m_AvgBaseCount >= DIMMER_AVG_COUNT ) {
            m_AvgBaseCount      = 0;
            m_AvgDataCompFlag   = 1;
        }
        if( m_AvgDataCompFlag == 1 ) {
            /* Average data acquisition completion */
            dimmerTotal = 0;
            for(i = 0; i < DIMMER_AVG_COUNT; i++){
                dimmerTotal += m_AvgBaseData[i];
            }
            dimmer = dimmerTotal / DIMMER_AVG_COUNT;
        }
    }
    //DBL_LOG("setDimmerCalculation END:[%d]\n", dimmer);
    return dimmer;
}

/*
 *  Return Static Dimmer value
 * @param 
 * @return
 */
UINT8 vAPIGetDimmingValue(void)
{
    return bl_sreg_read(SREG_LCD_BACKLIGHT_CONTROL);
}

/*
 *  Update backlight when change reg value from code2
 * @param 
 * @return
 */
void vAPIBacklightRegSet(void) 
{
    DYN_BL_MEMBER_T* pt_this = &t_dyn_bl;   
    pt_this->dbl_event_ui_change = TRUE;
}

/*
 *  
 * @param 
 * @return
 */
DYN_BL_MEMBER_T* t_dyn_bl_ext(void)
{
    return &t_dyn_bl;
}

UINT8 vAPIGetPowerSavingStatus(void)
{
    return m_ecoMode;
}
