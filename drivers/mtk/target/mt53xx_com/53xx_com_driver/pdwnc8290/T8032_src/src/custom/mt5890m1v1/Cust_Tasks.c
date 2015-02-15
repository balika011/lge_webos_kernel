/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
#include "general.h"
//#include "drv_hdmicec.h"
void vWDTRebootProcess(void)
{
     LogS("  WDT");
 }



#if SUPORT_COLD_REBOOT
UINT8 u1ColdRebootChk;

void vTask1Init_cold_rebootInit(void)
{
    u1ColdRebootChk = 0;
    vSetTimer(COLD_REBOOT_TIMER_ID, 0);
}

void vTask1_cold_reboot_state_machine(void)
{
    if((MGET_ARM_POWER_STATE() != ARM_POWER_OFF) || (u1ColdRebootChk == 0))
        return;
    if(fgIsTimerUp(COLD_REBOOT_TIMER_ID))
    {
#if UART_DBG        
        LogS("ColdReboot TimeOut"); 
#endif
        vTask3Init_cold_rebootInit();
        vWakeUpSystem(PDWNC_WAKE_UP_REASON_CUSTOM_4, 0);
    }

}
#endif

#if CUST_P_SX_SUPPORT
#define RES_ACK 0
#define RES_NACK 1
#define RES_NAV 2
UINT8 fgSerialXpressEnable;
UART_DRV_OBJ* fgSerialXpressData;
#if CUS_SH_HOTEL_MODE 
UINT8 fgCust_SH_HotelModeUartWakeupEn;    
#endif

void CUS_P_SX_send(UINT8* pPAyload, UINT8 count)
{
    UINT8 i;
    UINT8 Cmd_Array[8];
    UINT8 u1Checksum;
    u1Checksum=0;


    Cmd_Array[0]=0xe;
    Cmd_Array[1]=count+10;
    Cmd_Array[2]=0;
    Cmd_Array[3]=0;
    Cmd_Array[4]=5;
    Cmd_Array[5]=count+1;
    Cmd_Array[6]=0;
    Cmd_Array[7]=0xc;

    
    for(i=0;i<8;i++)
        vIO32Write1B(PDWNC_DATAW, Cmd_Array[i]);

    for(i=0;i<(count-1);i++,pPAyload++)
    {
        vIO32Write1B(PDWNC_DATAW,  *pPAyload);
        u1Checksum ^=(*pPAyload);
    }
    vIO32Write1B(PDWNC_DATAW,  u1Checksum);

    vIO32Write1B(PDWNC_DATAW,  0xa5);
    vIO32Write1B(PDWNC_DATAW,  0xa5);    
}

void CUS_P_SX_ReportEvent(UINT8 response)
{
    UINT8 Report_array[2];
    
    Report_array[0]=0x16;
    Report_array[1]=response;
    CUS_P_SX_send(Report_array,3); //bema add check sum 2-->3
}

void CUS_P_SX_ReportPowerState(UINT8 power_state)
{
    UINT8 Report_array[3];

    Report_array[0]=0x22;
    Report_array[1]=0x18;
    Report_array[2]=power_state;
    CUS_P_SX_send(Report_array,4); //bema add check sum 2-->3
}

#if RW_REG_INTERFACE
static MemoryRW_DbgTask()
{

		UINT16 data u2Addr;
		UINT32 data u4Data;

		 if(
		   (fgSerialXpressData->uRs232Data[0] == 'w') &&
		  (fgSerialXpressData->uRs232Data[1] == ' ') &&
		  (fgSerialXpressData->uRs232Data[2] == '0') &&
		  (fgSerialXpressData->uRs232Data[3] == 'x') &&
//		  (fgSerialXpressData->uRs232Data[4] == '0') &&
		  (fgSerialXpressData->uRs232Data[8] == ' ')  &&
		  (fgSerialXpressData->uRs232Data[9] == '0')  &&	  
		  (fgSerialXpressData->uRs232Data[10] == 'x') 
		  )
	  {
		 u2Addr = ((fgSerialXpressData->uRs232Data[4]) >= 'a') ? ((UINT16)((fgSerialXpressData->uRs232Data[4]) - 87) << 12) : ((UINT16)((fgSerialXpressData->uRs232Data[4]) - 48) << 12);
		u2Addr += ((fgSerialXpressData->uRs232Data[5]) >= 'a') ? ((UINT16)((fgSerialXpressData->uRs232Data[5]) - 87) << 8) : ((UINT16)((fgSerialXpressData->uRs232Data[5]) - 48) << 8);
		u2Addr += ((fgSerialXpressData->uRs232Data[6]) >= 'a') ? ((UINT16)((fgSerialXpressData->uRs232Data[6]) - 87) << 4) : ((UINT16)((fgSerialXpressData->uRs232Data[6]) - 48) << 4);
		u2Addr += ((fgSerialXpressData->uRs232Data[7]) >= 'a') ? ((UINT16)(fgSerialXpressData->uRs232Data[7]) - 87)  : ((UINT16)(fgSerialXpressData->uRs232Data[7]) - 48) ;
	
		 u4Data = ((fgSerialXpressData->uRs232Data[11]) >= 'a') ?  ((UINT32)((fgSerialXpressData->uRs232Data[11]) - 87) << 28) : ((UINT32)((fgSerialXpressData->uRs232Data[11]) - 48) << 28);
		 u4Data += ((fgSerialXpressData->uRs232Data[12]) >= 'a') ? ((UINT32)((fgSerialXpressData->uRs232Data[12]) - 87) << 24) : ((UINT32)((fgSerialXpressData->uRs232Data[12]) - 48) << 24);
		 u4Data += ((fgSerialXpressData->uRs232Data[13]) >= 'a') ? ((UINT32)((fgSerialXpressData->uRs232Data[13]) - 87) << 20) : ((UINT32)((fgSerialXpressData->uRs232Data[13]) - 48) << 20);
		 u4Data += ((fgSerialXpressData->uRs232Data[14]) >= 'a') ? ((UINT32)((fgSerialXpressData->uRs232Data[14]) - 87) << 16) : ((UINT32)((fgSerialXpressData->uRs232Data[14]) - 48) << 16);
		 u4Data += ((fgSerialXpressData->uRs232Data[15]) >= 'a') ? ((UINT32)((fgSerialXpressData->uRs232Data[15]) - 87) << 12) : ((UINT32)((fgSerialXpressData->uRs232Data[15]) - 48) << 12);
		 u4Data += ((fgSerialXpressData->uRs232Data[16]) >= 'a') ? ((UINT32)((fgSerialXpressData->uRs232Data[16]) - 87) << 8) : ((UINT32)((fgSerialXpressData->uRs232Data[16]) - 48) << 8); 	
		 u4Data += ((fgSerialXpressData->uRs232Data[17]) >= 'a') ? ((UINT32)((fgSerialXpressData->uRs232Data[17]) - 87) << 4) : ((UINT32)((fgSerialXpressData->uRs232Data[17]) - 48) << 4); 	
		 u4Data += ((fgSerialXpressData->uRs232Data[18]) >= 'a') ? ((UINT32)((fgSerialXpressData->uRs232Data[18]) - 87) ) : ((UINT32)((fgSerialXpressData->uRs232Data[18]) - 48) ); 		 
	
		while (!(RegReadFldAlign(PDWNC_STAB, FLD_WR_ALLOW))){};    vIO32Write1B(PDWNC_DATAW, 0x0D);
		while (!(RegReadFldAlign(PDWNC_STAB, FLD_WR_ALLOW))){};    vIO32Write1B(PDWNC_DATAW, 0x0A);    
		_LogS("Write Addr :");
		while (!(RegReadFldAlign(PDWNC_STAB, FLD_WR_ALLOW))){}; 	 vIO32Write1B(PDWNC_DATAW, '0');
		while (!(RegReadFldAlign(PDWNC_STAB, FLD_WR_ALLOW))){}; 	 vIO32Write1B(PDWNC_DATAW, 'x');	 
		_Print8bitsHex((UINT8)(u2Addr >> 8));
		_Print8bitsHex((UINT8)(u2Addr & 0x00ff));
		while (!(RegReadFldAlign(PDWNC_STAB, FLD_WR_ALLOW))){};    vIO32Write1B(PDWNC_DATAW, 0x0D);
		while (!(RegReadFldAlign(PDWNC_STAB, FLD_WR_ALLOW))){};    vIO32Write1B(PDWNC_DATAW, 0x0A);    
		_LogS("Data:");
		while (!(RegReadFldAlign(PDWNC_STAB, FLD_WR_ALLOW))){}; 	 vIO32Write1B(PDWNC_DATAW, '0');
		while (!(RegReadFldAlign(PDWNC_STAB, FLD_WR_ALLOW))){}; 	 vIO32Write1B(PDWNC_DATAW, 'x'); 
	  _Print8bitsHex((UINT8)((u4Data >> 24) & 0x000000ff));
	  _Print8bitsHex((UINT8)((u4Data >> 16) & 0x000000ff));
	  _Print8bitsHex((UINT8)((u4Data >> 8) & 0x000000ff));
		_Print8bitsHex((UINT8)(u4Data & 0x000000ff));
		while (!(RegReadFldAlign(PDWNC_STAB, FLD_WR_ALLOW))){};    vIO32Write1B(PDWNC_DATAW,  0x0D);
		while (!(RegReadFldAlign(PDWNC_STAB, FLD_WR_ALLOW))){};    vIO32Write1B(PDWNC_DATAW,  0x0A);	
		vRegWrite4B(u2Addr, u4Data);
	  }
	  else	if(
		   (fgSerialXpressData->uRs232Data[0] == 'r') &&
		  (fgSerialXpressData->uRs232Data[1] == ' ') &&
		  (fgSerialXpressData->uRs232Data[2] == '0') &&
		  (fgSerialXpressData->uRs232Data[3] == 'x') /*&&
		  (fgSerialXpressData->uRs232Data[4] == '0')*/
		  ) 	 
	  {
		//	u2Addr	= (fgSerialXpressData->uRs232Data[5]) ;
		  u2Addr = ((fgSerialXpressData->uRs232Data[4]) >= 'a') ? ((UINT16)((fgSerialXpressData->uRs232Data[4]) - 87) << 12) : ((UINT16)((fgSerialXpressData->uRs232Data[4]) - 48) << 12);		
		  u2Addr += ((fgSerialXpressData->uRs232Data[5]) >= 'a') ? ((UINT16)((fgSerialXpressData->uRs232Data[5]) - 87) << 8) : ((UINT16)((fgSerialXpressData->uRs232Data[5]) - 48) << 8);
		  u2Addr += ((fgSerialXpressData->uRs232Data[6]) >= 'a') ? ((UINT16)((fgSerialXpressData->uRs232Data[6]) - 87) << 4) : ((UINT16)((fgSerialXpressData->uRs232Data[6]) - 48) << 4);
		  u2Addr += ((fgSerialXpressData->uRs232Data[7]) >= 'a') ? ((UINT16)(fgSerialXpressData->uRs232Data[7]) - 87) : ((UINT16)(fgSerialXpressData->uRs232Data[7]) - 48) ;
		  u4Data = u4RegRead4B(u2Addr);
	
		while (!(RegReadFldAlign(PDWNC_STAB, FLD_WR_ALLOW))){}; 	 vIO32Write1B(PDWNC_DATAW, 0x0D);
		while (!(RegReadFldAlign(PDWNC_STAB, FLD_WR_ALLOW))){}; 	 vIO32Write1B(PDWNC_DATAW, 0x0A);	 
	  _LogS("Read Addr ");
		while (!(RegReadFldAlign(PDWNC_STAB, FLD_WR_ALLOW))){};  vIO32Write1B(PDWNC_DATAW, '0');
		while (!(RegReadFldAlign(PDWNC_STAB, FLD_WR_ALLOW))){};  vIO32Write1B(PDWNC_DATAW, 'x');  
		while (!(RegReadFldAlign(PDWNC_STAB, FLD_WR_ALLOW))){}; 	 
	  _Print8bitsHex((UINT8)(u2Addr >> 8));
	  _Print8bitsHex((UINT8)(u2Addr & 0x00ff));
		while (!(RegReadFldAlign(PDWNC_STAB, FLD_WR_ALLOW))){};  vIO32Write1B(PDWNC_DATAW, 0x0D);
		while (!(RegReadFldAlign(PDWNC_STAB, FLD_WR_ALLOW))){};  vIO32Write1B(PDWNC_DATAW, 0x0A);	 
	  _LogS("==> ");
		while (!(RegReadFldAlign(PDWNC_STAB, FLD_WR_ALLOW))){};  vIO32Write1B(PDWNC_DATAW, '0');
		while (!(RegReadFldAlign(PDWNC_STAB, FLD_WR_ALLOW))){};  vIO32Write1B(PDWNC_DATAW, 'x');	
	  _Print8bitsHex((UINT8)((u4Data >> 24) & 0x000000ff));
	  _Print8bitsHex((UINT8)((u4Data >> 16) & 0x000000ff));
	  _Print8bitsHex((UINT8)((u4Data >> 8) & 0x000000ff));
	  _Print8bitsHex((UINT8)(u4Data & 0x000000ff));
		while (!(RegReadFldAlign(PDWNC_STAB, FLD_WR_ALLOW))){};  vIO32Write1B(PDWNC_DATAW,	0x0D);
		while (!(RegReadFldAlign(PDWNC_STAB, FLD_WR_ALLOW))){};  vIO32Write1B(PDWNC_DATAW,	0x0A);	  
		
	  }
	
}
#endif

void CUS_P_SX_parse(void)
{
#if CUS_SH_HOTEL_MODE 
#else
    UINT8 data i = 0;
    UINT8 data num_payload_size;
    UINT8 data bchecksum;
    bchecksum=0;
#endif    
    #if 0// SX_DBG
    
    for(i=0;i<32;i++)
        vIO32Write1B(PDWNC_DATAW, fgSerialXpressData->uRs232Data[i]);

        fgSerialXpressData->uRs232Rxidx=0;
    #endif
#if RW_REG_INTERFACE
	if(fgSerialXpressData->uRs232Rxidx == 0)//david
	{
		return;
	}
#endif        
    fgSerialXpressData->uRs232Rxidx=0;

#if RW_REG_INTERFACE
	MemoryRW_DbgTask();
#endif        
    
//for T_comp H_proj project
  if( (fgSerialXpressData->uRs232Data[0] == 0x57) &&
         (fgSerialXpressData->uRs232Data[1] == 0x01) &&
         (fgSerialXpressData->uRs232Data[2] == 0x02) &&
         (fgSerialXpressData->uRs232Data[3] == 0x00) &&
         (fgSerialXpressData->uRs232Data[4] == 0x00) &&
    (fgSerialXpressData->uRs232Data[5] == 0x5a)
    )
  {
       vWakeUpSystem(PDWNC_WAKE_UP_REASON_UART_NORMAL, 0);
  }

//for A_COMP BXX project
  if( (fgSerialXpressData->uRs232Data[0] == 0xbe) &&
         (fgSerialXpressData->uRs232Data[1] == 0x41) &&
         (fgSerialXpressData->uRs232Data[2] == 0x01) &&
         (fgSerialXpressData->uRs232Data[3] == 0x06) &&
         (fgSerialXpressData->uRs232Data[4] == 0x50) &&
         (fgSerialXpressData->uRs232Data[5] == 0x00) &&
         (fgSerialXpressData->uRs232Data[6] == 0x00) &&
         (fgSerialXpressData->uRs232Data[7] == 0x00) &&
         (fgSerialXpressData->uRs232Data[8] == 0x00) &&
         (fgSerialXpressData->uRs232Data[9] == 0x00) &&         
         (fgSerialXpressData->uRs232Data[10] == 0x00) &&                  
    (fgSerialXpressData->uRs232Data[11] == 0xa8)
    )
  {
       vWakeUpSystem(PDWNC_WAKE_UP_REASON_UART_NORMAL, 0);
  }

#if Cust_B_Debug_UART
	LogSB("Urx_len:",fgSerialXpressData->uRs232Rxidx);
	LogSB("Urx[0]:", fgSerialXpressData->uRs232Data[0]);
	LogSB("Urx[1]:", fgSerialXpressData->uRs232Data[1]);
	LogSB("Urx[2]:", fgSerialXpressData->uRs232Data[2]);
	LogSB("Urx[3]:", fgSerialXpressData->uRs232Data[3]);
	LogSB("Urx[4]:", fgSerialXpressData->uRs232Data[4]);
	LogSB("Urx[5]:", fgSerialXpressData->uRs232Data[5]);
	LogSB("Urx[6]:", fgSerialXpressData->uRs232Data[6]);
	LogSB("Urx[7]:", fgSerialXpressData->uRs232Data[7]);
	LogSB("Urx[8]:", fgSerialXpressData->uRs232Data[8]);
	LogSB("Urx[9]:", fgSerialXpressData->uRs232Data[9]);
	LogSB("Urx[10]:",fgSerialXpressData->uRs232Data[10]);
	LogSB("Urx[11]:",fgSerialXpressData->uRs232Data[11]);
	LogSB("Urx[12]:",fgSerialXpressData->uRs232Data[12]);
#endif	
/* A_COMP Hotel Op code: LCD TV ON
    Cust_B 20091215	
*/
	if( (fgSerialXpressData->uRs232Data[0] == '#') &&
         (fgSerialXpressData->uRs232Data[1] == 'S') &&
         (fgSerialXpressData->uRs232Data[2] == '0') &&
         (fgSerialXpressData->uRs232Data[3] == '0') &&
         (fgSerialXpressData->uRs232Data[4] == '1') &&
    (fgSerialXpressData->uRs232Data[5] == '#')
    )	
  {
       LogS(" [Hotel Op code]: LCD TV ON");	//Add by Cust_B 20091215
	   vWakeUpSystem(PDWNC_WAKE_UP_REASON_UART_NORMAL, 0);
  }
  
#if CUS_SH_HOTEL_MODE  
  //only for 9600 baud rate 
  if( (fgCust_SH_HotelModeUartWakeupEn == 1) &&
        (fgSerialXpressData->uRs232Data[0] == 'P') &&
       (fgSerialXpressData->uRs232Data[1] == 'O') &&
       (fgSerialXpressData->uRs232Data[2] == 'W') &&
       (fgSerialXpressData->uRs232Data[3] == 'R') &&
       (fgSerialXpressData->uRs232Data[4] == '1') &&
       (fgSerialXpressData->uRs232Data[5] == '#') &&
       (fgSerialXpressData->uRs232Data[6] == '#') &&
       (fgSerialXpressData->uRs232Data[7] == '#') &&
        (fgSerialXpressData->uRs232Data[8] == '<'))
    {
        vIO32Write1B(PDWNC_DATAW,  'O');
        vIO32Write1B(PDWNC_DATAW,  'K'); 
        vIO32Write1B(PDWNC_DATAW,  '<'); 
        vWakeUpSystem(PDWNC_WAKE_UP_REASON_UART_NORMAL, 0);
  }
  else
  {
      vIO32Write1B(PDWNC_DATAW,  'E');
      vIO32Write1B(PDWNC_DATAW,  'R'); 
      vIO32Write1B(PDWNC_DATAW,  'R'); 
      vIO32Write1B(PDWNC_DATAW,  '<');             
  }
#else
    if( (fgSerialXpressData->uRs232Data[0] == 0xE) &&
         (fgSerialXpressData->uRs232Data[2] == 0) &&
         (fgSerialXpressData->uRs232Data[3] == 0) &&
         (fgSerialXpressData->uRs232Data[4] == 5) &&
         (fgSerialXpressData->uRs232Data[6] == 0) &&
    (fgSerialXpressData->uRs232Data[7] == 0xc)
    )
    {
        num_payload_size = (fgSerialXpressData->uRs232Data[1]-10);

        for(i=0; i<(num_payload_size-1); i++)
        bchecksum ^= fgSerialXpressData->uRs232Data[8+i];

        if( (fgSerialXpressData->uRs232Data[5] == num_payload_size+1) &&
        (fgSerialXpressData->uRs232Data[8+num_payload_size-1] == bchecksum) &&
        (fgSerialXpressData->uRs232Data[8+num_payload_size] == 0xa5) &&
        (fgSerialXpressData->uRs232Data[8+num_payload_size+1] == 0xa5)
        )
        {
        
            switch(fgSerialXpressData->uRs232Data[8])
            {
                case 0x20:  //Set command
                if(fgSerialXpressData->uRs232Data[9] == 0xA3) //set RS232 control
                {

                   // CUS_P_SX_ReportEvent(RES_NAV); //Currently not support, since uP cannot access NVM
                    if(fgSerialXpressData->uRs232Data[10] == 0x01)    //Set Power on
                    {
                    
			    fgSerialXpressEnable = 1;
                    }
                    else
                    {
                
			    fgSerialXpressEnable = 0;
                    	}
                    CUS_P_SX_ReportEvent(RES_ACK);
                }
                else if(fgSerialXpressData->uRs232Data[9] == 0x18 && fgSerialXpressEnable == 1)    //set power state
                {
                
                    if(fgSerialXpressData->uRs232Data[10] == 0x01)    //Set Power on
                    {
                    
                        CUS_P_SX_ReportEvent(RES_ACK);
                        //SwitchBack_PORT0(); //switch back port0 to ARM
                        //set 0x28818 bit 2 low
		//vRegWrite1B(REG_RS232_MODE, 0); 
                        //vRegWrite4B(0x0120, u4RegRead4B(0x0120)|0x00020000); //Enable 8032 wake up
                       // SwitchBack_PORT0(); //switch back port0 to ARM                                    
/*                        vWakeUpSystem();
                        LED1_ON;
                        LED2_OFF;
                        u1WakeupReason = 3;
*/
                        vWakeUpSystem(PDWNC_WAKE_UP_REASON_UART_NORMAL, 0);
                    }
                }
                else    //command not support in standby mode
                {
                    CUS_P_SX_ReportEvent(RES_NAV);
                }
                break;

                case 0x21:  //Get command
                if(fgSerialXpressData->uRs232Data[9] == 0x18) //Get power state
                {
                    CUS_P_SX_ReportEvent(RES_ACK);
                    CUS_P_SX_ReportPowerState(0x0); //power state is standby
                }
                else    //command not support in standby mode
                {
                    CUS_P_SX_ReportEvent(RES_NAV);
                }
                break;

                default:    //not set/get command
                CUS_P_SX_ReportEvent(RES_NAV);
                break;
            }
        }
        else //protocol error, should response Nack
        {
            CUS_P_SX_ReportEvent(RES_NACK);
        }
    }
    else //protocol error, should response Nack
    {
    	if(fgSerialXpressData->uRs232Data[0] == 0xE)
        CUS_P_SX_ReportEvent(RES_NACK);
    }
#endif //CUS_SH_HOTEL_MODE
}
#endif

#if UART_TASK
void vRS232CustTaskInit(void)
{
	#if CUST_P_SX_SUPPORT
    	fgSerialXpressData = pGetUartDrvObj();
	#endif
}

void vRS232CustTask(void)
{
    #if CUST_P_SX_SUPPORT
        CUS_P_SX_parse();
    #endif
}
#endif

#if  SYSTEM_WAKEUP_DELAY
#define  TASK2_START 1
#define  TASK2_END   2
#define  TASK2_RUN   3
#define  TEST2_UN    4
static UINT8 Task2Status;
static UINT32 HW_WAKEN;
void vTask2Init_SystemWakeupDelay(void)
{
	vSetTimer(WAKEUP_DELAY_TIMER_ID, 0);	
	HW_WAKEN = 0;
}
void vTask2_SystemWakeupDelay_EnterTask(void)
{
	HW_WAKEN = u4RegRead4B(PDWNC_WAKEN);
	vRegWrite4B(PDWNC_WAKEN,0);
	vSetTimer(WAKEUP_DELAY_TIMER_ID, 150); //20ms * 150 = 3000ms;
}
void vTask2_SystemWakeupDelay_state_machine(void)
{
	if(MGET_ARM_POWER_STATE() != ARM_POWER_OFF)
		return -1;

	if(fgIsTimerUp(WAKEUP_DELAY_TIMER_ID))   //timeout
	{
		vRegWriteFldAlign(PDWNC_IRCLR, 1, FLD_IRCLR); //IR clear  register 
		vRegWrite4B(PDWNC_WAKEN,HW_WAKEN);
	}
}
#endif


#if POWERBOARD_GPIO_CTRL
void vTask9PowerBoardCtrl_Init(void)
{
	return;
}
void vTask9PowerBoardCtrl_state_machine(void)
{
	if(MGET_ARM_POWER_STATE() == ARM_POWER_OFF)
	{
		if(u4IO32Read4B(PDWNC_PDSTAT0) || u4IO32Read4B(PDWNC_PDSTAT1))
		{
			GPIO_SetOut(247,0); //OPWRSB gpio num:247,(Oryx).
		}
	}
}
#endif


