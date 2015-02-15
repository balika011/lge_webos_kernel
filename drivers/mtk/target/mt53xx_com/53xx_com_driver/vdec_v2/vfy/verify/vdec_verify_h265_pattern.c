/********************************************************************************************
 *     LEGAL DISCLAIMER 
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES 
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED 
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS 
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, 
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY 
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, 
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK 
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION 
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *     
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH 
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, 
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE 
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
 *     
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS 
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.  
 ************************************************************************************************/

#include "x_typedef.h"
#include "x_os.h"
#include "drv_config.h"
#include "vdec_verify_mpv_prov.h"
#include "RvdUtil.h"
#include "vdec_ide.h"
#include "vdec_type.h"
#include "x_printf.h"
#include "x_debug.h"
#include "vdec_verify_file_common.h"
#include "vdec_verify_filesetting.h"
#include "vdec_verify_common.h"
#include "risc_pattern.h"
#include "vdec_hw_common.h"

#ifdef CONFIG_TV_DRV_VFY 
#include "x_hal_5381.h"
#include "x_hal_926.h"
#endif

#define LOAD_DATA_USB   1

/////////////////////////////////////////////
/*gloable variable*/
/////////////////////////////////////////////
unsigned int width;
unsigned int height;

int debug_mode;
UINT32 error_rate;
char bitstream_name[200];
unsigned int current_dpb_idx;
unsigned int pre_dpb_idx;
int isDecodeTimeOut;

BOOL UFO_MODE;
BOOL Golden_UFO;

UINT32 Dpb_addr[MAX_DPB_NUM], mv_buf_addr[MAX_DPB_NUM];
// for UFO_MODE
UINT32 Dpb_ufo_Y_len_addr[MAX_DPB_NUM], Dpb_ufo_C_len_addr[MAX_DPB_NUM];

UINT32 u4RefCtlUFO;
UINT32 u4DpbUFO;

#define x_alloc_aligned_verify_mem(u4Size, u4Align, fgChannelA) VIRTUAL(BSP_AllocAlignedDmaMemory(u4Size,u4Align))
extern void get_random_bytes(void *buf, int nbytes);

int  Wait_decode_finished( unsigned long  start_time )
{
    UINT32 u4RetRegValue = 0;

    RISCRead_MISC( 41 , &u4RetRegValue, 0);
    #if SIM_LOG
    #else
    while ( ((u4RetRegValue>>16) & 0x1) !=  1){
        RISCRead_MISC(41, &u4RetRegValue, 0 );
        if ( ( jiffies -start_time > 3700) ){
            printk("Polling int time out!!!\n");
            return 1;
        }
    }
    #endif

    RISCRead_MISC( 41, &u4RetRegValue, 0 );
    RISCWrite_MISC( 41, u4RetRegValue | 0x1, 0 );
    RISCRead_MISC( 41, &u4RetRegValue, 0 );
    RISCWrite_MISC( 41, u4RetRegValue | (1 << 4), 0);
    RISCWrite_MISC( 41, u4RetRegValue & 0xffffffef, 0); 

    RISCRead_MISC( 41 , &u4RetRegValue, 0 );
    RISCWrite_MISC( 41 , u4RetRegValue | (0x1<<12), 0 ) ; // clear for VP mode

    return 0;
}


void RISCWrite_HEVC_VLD( UINT32 u4Addr, UINT32 u4Value ){
    UINT32 HEVC_VLD_BASE = HEVC_VLD_REG_OFFSET0;
    #if SIM_LOG
    #else
    DRV_WriteReg( HEVC_VLD_BASE , u4Addr*4 , u4Value);
    #endif
    if (debug_mode>0 || SIM_LOG)
        printk("        RISCWrite_HEVC_VLD( %d, %d );  //u4val = 0x%08X\n", u4Addr, u4Value, u4Value);
}

void RISCRead_HEVC_VLD ( UINT32 u4Addr , UINT32* pu4Value){
    UINT32 HEVC_VLD_BASE = HEVC_VLD_REG_OFFSET0;
    #if SIM_LOG
    #else
    (*pu4Value) = DRV_ReadReg( HEVC_VLD_BASE, u4Addr*4  );
    #endif
    if (debug_mode>0 || SIM_LOG)
        printk("        RISCRead_HEVC_VLD( %d); /* return 0x%08X */\n", u4Addr, (*pu4Value));
}

void HEVC_SHIFT_BITS( UINT32 u4Value ){
    UINT32 tempVal;
    int i;
    for ( i=0; i< u4Value/32; i++ ) 
        RISCRead_HEVC_VLD( 32 , &tempVal); 
        
    //if( u4Value%32 != 0 ) 
        RISCRead_HEVC_VLD( u4Value%32 , &tempVal);
}

void  INIT_SEARCH_START_CODE( UINT32 shift_bit ){

    UINT32 u4RetRegValue, bits_cnt;
/*
    RISCRead_HEVC_VLD( 33, &u4RetRegValue);
    RISCWrite_HEVC_VLD( 33, u4RetRegValue | 0x1<<4);
*/

    if ( shift_bit > 0){
        RISCWrite_HEVC_VLD( 37, 0x1<<8);
        RISCRead_HEVC_VLD( 37, &u4RetRegValue);
        while( (u4RetRegValue>>8) & 0x1 !=0  ){
            RISCRead_HEVC_VLD( 37, &u4RetRegValue);
        }
        RISCRead_HEVC_VLD( 72, &bits_cnt);
    }

    while( shift_bit > (bits_cnt+16) ){
        RISCWrite_HEVC_VLD( 37, 0x1<<8);
        RISCRead_HEVC_VLD( 37, &u4RetRegValue);
        while( (u4RetRegValue>>8) & 0x1 !=0  ){
            RISCRead_HEVC_VLD( 37, &u4RetRegValue);
        }
        RISCRead_HEVC_VLD( 72, &bits_cnt);
    }
    
}



void INIT_BARREL_SHIFTER( UINT32 r_ptr ){

    UINT32 u4RetRegValue;
    UINT32 u4VldRptr;

    u4VldRptr = PHYSICAL(r_ptr);
    // HEVC_FLAG
    RISCWrite_HEVC_VLD( 33, 1);
    
    RISCRead_VLD(  59 , &u4RetRegValue, 0 );
    RISCWrite_VLD( 59, u4RetRegValue |(1 << 28), 0 );

    // polling Sram stable 
    RISCRead_VLD( 61 , &u4RetRegValue, 0 );
    
    #if SIM_LOG
    //break while
    u4RetRegValue = 0x00;
    #endif
    
    if( (u4RetRegValue >> 15 ) & 0x1 == 1) 
    {
       // polling VLD_61  [0] == 1
        RISCRead_VLD( 61 , &u4RetRegValue, 0 );
        while ( (u4RetRegValue & 0x1)   !=  1)    
            RISCRead_VLD( 61 , &u4RetRegValue , 0);
    }

    // read pointer
    RISCWrite_VLD( 44, u4VldRptr, 0);

    //BITstream DMA async_FIFO  local reset
    RISCRead_VLD(  66 , &u4RetRegValue, 0 );
    RISCWrite_VLD(  66, u4RetRegValue |(1 << 8), 0 );
    RISCWrite_VLD(  66, 0, 0);

    //initial fetch
    RISCRead_VLD(  35 , &u4RetRegValue, 0 );
    RISCWrite_VLD(  35, u4RetRegValue |(1 << 20), 0 );

    if (debug_mode>0)
        printk("        wait(`VDEC_INI_FETCH_READY == 1);\n");
    
    RISCRead_VLD( 58 , &u4RetRegValue, 0 );
    #if SIM_LOG
    //break while
    u4RetRegValue = 0x01;
    #endif
    while ( (u4RetRegValue & 0x1) !=  1)
        RISCRead_VLD( 58, &u4RetRegValue, 0 );

    //initial barrel shifter
    RISCRead_VLD( 35 , &u4RetRegValue, 0 );
    RISCWrite_VLD( 35, u4RetRegValue|(1 << 23), 0 );

    //byte address
    //HEVC_SHIFT_BITS( (r_ptr&0xF) * 8 ); 
    #if SIM_LOG
    // 
    #else
    INIT_SEARCH_START_CODE( (u4VldRptr&0xF) * 8 );
    #endif

    //------------------------------
}

void PRED_WEIGHT_TABLE(){

    UINT32 u4RetRegValue;

    RISCRead_HEVC_VLD( 37, &u4RetRegValue );

    RISCWrite_HEVC_VLD( 37 , u4RetRegValue |0x1 );
    RISCRead_HEVC_VLD( 37, &u4RetRegValue );
    if (debug_mode>0)
        printk("        wait(`HEVC_FW_DEC_WP_BUSY == 0);\n");
    #if SIM_LOG
    //break while
    u4RetRegValue = 0x00;
    #endif
    while ( (u4RetRegValue & 0x1) !=  0 )
        RISCRead_HEVC_VLD( 37, &u4RetRegValue );

}

void REF_PIC_LIST_MOD(){

    UINT32 u4RetRegValue;

    RISCRead_HEVC_VLD( 37, &u4RetRegValue );

    RISCWrite_HEVC_VLD( 37 , u4RetRegValue |(0x1<<4) );
    RISCRead_HEVC_VLD( 37, &u4RetRegValue );
    if (debug_mode>0)
        printk("        wait(`HEVC_FW_DEC_RPLM_BUSY == 0);\n");
    #if SIM_LOG
    //break while
    u4RetRegValue = (0x1<<4);
    #endif
    while ( (u4RetRegValue & (0x1<<4)) !=  0 )
        RISCRead_HEVC_VLD( 37, &u4RetRegValue );

}

void HEVC_VDEC_TRIG(){
    RISCWrite_HEVC_VLD( 36, 1);
}

void RISCRead_GCON ( UINT32 u4Addr , UINT32* pu4Value){
    
}

void RISCWrite_GCON( UINT32 u4Addr, UINT32 u4Value ){

}

void  HW_RESET( ){
    UINT32 u4RetRegValue;
    // HW reset
    RISCWrite_VLD(66,  0x101, 0);
    RISCWrite_MISC(50,  0x0, 0);
    RISCWrite_MISC(51, 0x0, 0);
    RISCWrite_MISC(33, 0x2, 0);
    RISCWrite_VLD( 66 , 0x0, 0);
    //RISCWrite_GCON(6, 0x1);
/*
    RISCRead_MISC( 41, &u4RetRegValue );
    printk("MISC_41 = %08X\n",u4RetRegValue);
    */
}


void Dump_yuv( char*  PP_OUT_Y_ADDR , char*  PP_OUT_C_ADDR, UINT32 PIC_SIZE_Y ){
    //transform 16*32 block mode to yuv video
}


int Dump_mem( unsigned char* buf, unsigned int size , int  frame_num , unsigned int type , bool isTimeout){
    printk("dump nothing\n");
    return 1;
}

BOOL fgGoldenCmp(UINT32 u4DecBuf,UINT32 u4GoldenBuf,UINT32 u4Size)
{
    UINT32 i;
    UINT32 *pSrc,*pDes;
    pSrc = (UINT32 *)u4DecBuf;
    pDes = (UINT32 *)u4GoldenBuf;
    //width 64 align,height 32 align
    for(i = 0; i < u4Size/4; i++)
    {
        if(*pSrc != *pDes)
        {
            printk("Decbuf 0x%x [0x%x] != GoldBuf 0x%x [0x%x] offset %d\n",u4DecBuf,*pSrc,u4GoldenBuf,*pDes,i);
            return 1;
        }
        pSrc ++;
        pDes ++;
    }

    return 0;
}

int Dump_mem_ex( unsigned char* buf, unsigned int size , int  frame_num , unsigned int type , bool isTimeout){

    
	UCHAR fpDumpFile[100] = "d:\\ChkFolder\\DecY_";
	UCHAR fpDumpFileC[100] = "d:\\ChkFolder\\DecC_";
	UCHAR *fpDump;
	UINT32 u4ReadSize;
	UINT32 u4Temp;
	FILE *pFile = NULL;

	if(type == 1)
	{
	    fpDump = fpDumpFile;
	}
	else
	{
	    fpDump = fpDumpFileC;
	}
	
	u4Temp = strlen(fpDump);
	u4Temp += sprintf(fpDump + u4Temp,"%d",frame_num);
	u4Temp += sprintf(fpDump + u4Temp,"%s",".bin");
	
    pFile = fopen(fpDump,"wb");
	if(pFile == NULL)
	{
		UTIL_Printf("Create file error !\n");
	}

	u4ReadSize = fwrite ((char* )(buf), 1, size, pFile);
	UTIL_Printf("read file len = %d @ 0x%x\n",u4ReadSize,(UINT32)buf);
	fclose(pFile);
    return 0;
}

int Dump_reg( UINT32 base_r, UINT32 start_r, UINT32 end_r , char* pBitstream_name , UINT32 frame_number, BOOL bDecodeDone){
#if 1
    
    unsigned char* buf;
    struct file* filp;
    struct file* filp_write;
    char file_name[200];
    char buffer[200];
    int ret,i;
    UINT32 u4Value;
   
    //initKernelEnv(); 
    
    sprintf(file_name, "%s_%d_regDump", pBitstream_name ,frame_number  );

    #if 0
    filp = openFile(file_name,O_RDONLY,0); 
    if (IS_ERR(filp) ){   // if info file exitst-> append; not exitst -> create
        filp_write = filp_open( file_name, O_CREAT|O_RDWR,0777 );

    } else {
        closeFile(filp); 
        filp_write = filp_open( file_name , O_APPEND|O_RDWR,0777 );
    }
    #endif

    if (bDecodeDone){
        sprintf( buffer,"================== Decode Done register dump ==================\n");
        //ret = filp_write->f_op->write( filp_write, buffer , strlen(buffer) , &filp_write->f_pos );
    }else{
        sprintf( buffer,"================== Before trigger decode register dump ==================\n");
        //ret = filp_write->f_op->write( filp_write, buffer , strlen(buffer) , &filp_write->f_pos );
    }

    for ( i=start_r ; i<=end_r ; i++ ){
        u4Value = HEVC_DRV_ReadReg( base_r, i*4 );
        if(0 == ((i - start_r)%2))
        {
            sprintf(buffer,"\\");
        }
        if ( base_r == VLD_REG_OFFSET0 )
            sprintf( buffer,"VLD[%d] = 0x%08.0X    ",i,u4Value, base_r + 4*i);
        if ( base_r == MC_REG_OFFSET0 )
            sprintf( buffer,"MC[%d] = 0x%08.0X    ",i,u4Value, base_r + 4*i);
        if ( base_r == HEVC_VLD_REG_OFFSET0 )
            sprintf( buffer,"HEVC_VLD[%d] = 0x%08.0X    ",i,u4Value, base_r + 4*i);
        if ( base_r == HEVC_PP_REG_OFFSET0 )
            sprintf( buffer,"PP[%d] = 0x%08.0X    ",i,u4Value, base_r + 4*i);
        if ( base_r == HEVC_MV_REG_OFFSET0 )
            sprintf( buffer,"MV[%d] = 0x%08.0X    ",i,u4Value, base_r + 4*i);
        if ( base_r == HEVC_MISC_REG_OFFSET0 )
            sprintf( buffer,"MISC[%d] = 0x%08.0X    ",i,u4Value, base_r + 4*i);
        if ( base_r == VLD_TOP_REG_OFFSET0 )
            sprintf( buffer,"VLD_TOP[%d] = 0x%08.0X    ",i,u4Value, base_r + 4*i);
        if ( base_r == VLD_REG_OFFSET1 )
            sprintf( buffer,"VLD1[%d] = 0x%08.0X    ",i,u4Value, base_r + 4*i);
        if ( base_r == MC_REG_OFFSET1 )
            sprintf( buffer,"MC1[%d] = 0x%08.0X    ",i,u4Value, base_r + 4*i);
        if ( base_r == HEVC_VLD_REG_OFFSET1 )
            sprintf( buffer,"HEVC_VLD1[%d] = 0x%08.0X    ",i,u4Value, base_r + 4*i);
        if ( base_r == HEVC_PP_REG_OFFSET1 )
            sprintf( buffer,"PP1[%d] = 0x%08.0X    ",i,u4Value, base_r + 4*i);
        if ( base_r == HEVC_MV_REG_OFFSET1 )
            sprintf( buffer,"MV1[%d] = 0x%08.0X    ",i,u4Value, base_r + 4*i);
        if ( base_r == HEVC_MISC_REG_OFFSET1 )
            sprintf( buffer,"MISC1[%d] = 0x%08.0X    ",i,u4Value, base_r + 4*i);
        if ( base_r == VLD_TOP_REG_OFFSET1 )
            sprintf( buffer,"VLD_TOP1[%d] = 0x%08.0X    ",i,u4Value, base_r + 4*i);
        if ( base_r == MVDEC_TOP_OFFSET0 )
            sprintf( buffer,"VLD_MCore[%d] = 0x%08.0X    ",i,u4Value, base_r + 4*i);
        if ( base_r == LAE_VLD_OFFSET0 )
            sprintf( buffer,"LAE_VLD_OFFSET0[%d] = 0x%08.0X    ",i,u4Value, base_r + 4*i);
        if ( base_r == LAE_VLDTOP_OFFSET0 )
            sprintf( buffer,"LAE_VLDTOP_OFFSET0[%d] = 0x%08.0X    ",i,u4Value, base_r + 4*i);
        if ( base_r == LAE_AVCVLD_OFFSET0 )
            sprintf( buffer,"LAE_AVCVLD_OFFSET0[%d] = 0x%08.0X    ",i,u4Value, base_r + 4*i);
        if ( base_r == LAE_MISC_OFFSET0 )
            sprintf( buffer,"LAE_MISC_OFFSET0[%d] = 0x%08.0X    ",i,u4Value, base_r + 4*i);
        if ( base_r == LAE_HEVCVLD_OFFSET0 )
            sprintf( buffer,"LAE_HEVCVLD_OFFSET0[%d] = 0x%08.0X    ",i,u4Value, base_r + 4*i);
        if ( base_r == LAE_LARB_OFFSET0 )
            sprintf( buffer,"LAE_LARB_OFFSET0[%d] = 0x%08.0X    ",i,u4Value, base_r + 4*i);

        printk("%s     ", buffer);
        if((i - start_r)%2)
        {
            printk("\n");
        }
        
        //ret = filp_write->f_op->write( filp_write, buffer , strlen(buffer) , &filp_write->f_pos );
    }
    printk("\n");
    //ret = filp_write->f_op->write( filp_write, "\n" , strlen("\n") , &filp_write->f_pos );
    //closeFile( filp_write );
    //set_fs( oldfs );
#endif
    return 0;
}

void Margin_padding(UINT32 Ptr_output_Y, UINT32 Ptr_output_C, UINT32 PIC_SIZE_Y )
{
    UINT32 u4RetRegValue;
    UINT32 u4LCUsize;
    UINT32 u4W_lcu, u4H_lcu;
    UINT32 u4Offset, u4CurrentPtr;
    UINT32 u4PadOffset, u4PadSize;
    UINT32 u4RealWidth, u4RealHeight;
    int i;

    RISCRead_VLD_TOP(28, &u4RetRegValue, 0);
    u4RealWidth = u4RetRegValue & 0xFFFF;
    u4RealHeight = (u4RetRegValue>>16) & 0xFFFF;

    RISCRead_HEVC_VLD( 42, &u4RetRegValue);
    u4LCUsize = 1<<((u4RetRegValue>>4) & 0x7);
    u4W_lcu = ((u4RealWidth + 64-1)/64)*64;

    if (debug_mode>0)
        printk("u4RealWidth %d; u4RealHeight %d\n", u4RealWidth,u4RealHeight );

    if ( u4RealHeight%u4LCUsize != 0){

        // PP_OUT_Y_ADDR 0 padding
        u4Offset = (u4RealHeight/32)*32*u4W_lcu;
        u4PadOffset = (u4RealHeight%32)*16;
        u4PadSize = (32-(u4RealHeight%32)) * 16;
        if (debug_mode>0)
            printk("PP_OUT_Y_ADDR:\nu4LCUsize = %d; u4Offset = %d; u4PadOffset =  %d; u4PadSize = %d\n", u4LCUsize, u4Offset,  u4PadOffset, u4PadSize );

        u4CurrentPtr = Ptr_output_Y+u4Offset;
        if (debug_mode>0)
            printk("Ptr_output_Y = 0x%08x; u4CurrentPtr start = 0x%08x\n", Ptr_output_Y, u4CurrentPtr);
        for (i = 0; i < u4W_lcu/16; i++){

            memset ( u4CurrentPtr + u4PadOffset, 0, u4PadSize);
            u4CurrentPtr += (u4PadOffset+u4PadSize);
            if (debug_mode>0)
                printk("i : %d; u4CurrentPtr =  0x%08x\n", i,  u4CurrentPtr);

        }
        memset ( u4CurrentPtr, 0, PIC_SIZE_Y-(u4CurrentPtr-(UINT32)Ptr_output_Y));

        // PP_OUT_C_ADDR 0 padding
        u4Offset = ((u4RealHeight/2)/16)*16*u4W_lcu;
        u4PadOffset = ((u4RealHeight/2)%16)*16;
        u4PadSize = (16-((u4RealHeight/2)%16)) * 16;
        if (debug_mode>0)
            printk("PP_OUT_C_ADDR:\nu4LCUsize = %d; u4Offset = %d; u4PadOffset =  %d; u4PadSize = %d\n", u4LCUsize, u4Offset,  u4PadOffset, u4PadSize );

        u4CurrentPtr = Ptr_output_C+u4Offset;
        if (debug_mode>0)
            printk("Ptr_output_C = 0x%08x; u4CurrentPtr start = 0x%08x\n", Ptr_output_C, u4CurrentPtr);
        for (i = 0; i < u4W_lcu/16; i++){
            memset ( u4CurrentPtr + u4PadOffset, 0, u4PadSize);
            u4CurrentPtr += (u4PadOffset+u4PadSize);
            if (debug_mode>0)
                printk("i : %d; u4CurrentPtr =  0x%08x\n", i,  u4CurrentPtr);
        }
        memset ( u4CurrentPtr, 0, PIC_SIZE_Y/2-(u4CurrentPtr-(UINT32)Ptr_output_C));
        
    }

    if ( u4RealWidth%64 != 0){
        
         // PP_OUT_Y_ADDR 0 padding
        u4Offset = (u4RealWidth/16)*16*32;
        u4PadOffset = u4RealWidth%16;
        u4PadSize = 16-(u4RealWidth%16);
        if (debug_mode>0)
             printk("PP_OUT_Y_ADDR:\nu4Offset = %d; u4PadOffset =  %d; u4PadSize = %d\n", u4Offset,  u4PadOffset, u4PadSize );

        u4CurrentPtr = Ptr_output_Y+u4Offset;
        if (debug_mode>0)
            printk("Ptr_output_Y = 0x%08x; u4CurrentPtr start = 0x%08x\n", Ptr_output_Y, u4CurrentPtr);

        for (i = 1; i <= ((u4RealHeight+31)/32)*32; i++){
            if (debug_mode>0)
                printk("i : %d; u4CurrentPtr =  0x%08x\n", i,  u4CurrentPtr);
            memset( u4CurrentPtr + u4PadOffset, 0,  u4PadSize);
            if ( i%32==0 ){
                u4CurrentPtr += 16;
                memset (  u4CurrentPtr, 0,  ((u4W_lcu/16 - (u4RealWidth/16))-1) * 16*32);
                u4CurrentPtr += (u4Offset+((u4W_lcu/16 - (u4RealWidth/16))-1) * 16*32);
            }else{
                u4CurrentPtr += 16;
            }

        }

        // PP_OUT_C_ADDR 0 padding
        u4Offset = (u4RealWidth/16)*16*16;
        u4PadOffset = u4RealWidth%16;
        u4PadSize = 16-(u4RealWidth%16);
        if (debug_mode>0)
             printk("PP_OUT_C_ADDR:\nu4Offset = %d; u4PadOffset =  %d; u4PadSize = %d\n", u4Offset,  u4PadOffset, u4PadSize );

        u4CurrentPtr = Ptr_output_C+u4Offset;
        if (debug_mode>0)
            printk("Ptr_output_C = 0x%08x; u4CurrentPtr start = 0x%08x\n", Ptr_output_C, u4CurrentPtr);
        for (i = 1; i <= ((u4RealHeight/2+15)/16)*16 ; i++){
            if (debug_mode>0)
                printk("i : %d; u4CurrentPtr =  0x%08x\n", i,  u4CurrentPtr);
            memset( u4CurrentPtr + u4PadOffset, 0,  u4PadSize);
            if ( i%16==0 ){
                u4CurrentPtr += 16;
                memset (  u4CurrentPtr, 0,  ((u4W_lcu/16 - (u4RealWidth/16))-1) * 16*16);
                u4CurrentPtr += (u4Offset+((u4W_lcu/16 - (u4RealWidth/16))-1) * 16*16);
            } else {
                u4CurrentPtr += 16;
            }

        }
   }

    
}


int  Golden_comparison( int inst_id,int frame_num, unsigned int PIC_SIZE_Y, UINT32 Ptr_output_Y, UINT32 Ptr_output_C , UINT32 MV_COL_PIC_SIZE, bool isDump,
                                               UINT32 Dpb_ufo_Y_len_addr, UINT32 Dpb_ufo_C_len_addr, UINT32 UFO_LEN_SIZE_Y, UINT32 UFO_LEN_SIZE_C )
{
    //golden result comparison
    unsigned char *buf_Golden;
    unsigned char *buf_Golden_C;
    char *ptr_base = NULL;
    char file_name[200] = {0};
    struct file *fd; 
    int ret, file_num, file_len; 
    UINT32 u4CompareRange;
    UINT32 u4RetRegValue;
    UINT32 u4LCUsize;
    UINT32 u4W_lcu;
    UINT32 u4RealWidth, u4RealHeight;
    bool bCmpFailFlag = 0;
    bool fgOpen = FALSE;


    RISCRead_VLD_TOP(28, &u4RetRegValue, 0);
    u4RealWidth = u4RetRegValue & 0xFFFF;
    u4RealHeight = (u4RetRegValue>>16) & 0xFFFF;
    u4W_lcu = ((u4RealWidth + 64-1)/64)*64;
    
    //initKernelEnv();

    // Load golden file
    //buf_Golden = (unsigned char *) vmalloc( PIC_SIZE_Y );
    buf_Golden = g2YGolden[inst_id];
    if (Golden_UFO){
        sprintf(file_name, "%s_pat/ufo_pat/ufo_%d_bits_Y.out", bitstream_name, frame_num);
    }else{
        sprintf(file_name, "%s_pat/sao_pat/frame_%d_Y.dat", bitstream_name, frame_num);
    }

    //dump Y golden
    _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
    _tFBufFileInfo[inst_id].pucTargetAddr = buf_Golden;
    _tFBufFileInfo[inst_id].u4TargetSz = PIC_SIZE_Y;  
    _tFBufFileInfo[inst_id].u4FileLength = 0;  
    memset ( buf_Golden , 0 ,PIC_SIZE_Y );
    fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
    if (fgOpen == FALSE)
    {
        UTIL_Printf("Open golden file error : %s\n",file_name);
    }
    else
    {
        
    }

    //buf_Golden_C = (unsigned char *) vmalloc( PIC_SIZE_Y/2 );
    buf_Golden_C = g2CGolden[inst_id];
    
    if (Golden_UFO){
        sprintf(file_name, "%s_pat/ufo_pat/ufo_%d_bits_CbCr.out", bitstream_name, frame_num);
    }else{
        sprintf(file_name, "%s_pat/sao_pat/frame_%d_C.dat", bitstream_name, frame_num);
    }

    //dump C golden
    _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
    _tFBufFileInfo[inst_id].pucTargetAddr = buf_Golden_C;
    _tFBufFileInfo[inst_id].u4TargetSz = (PIC_SIZE_Y>>1);  
    _tFBufFileInfo[inst_id].u4FileLength = 0;  
    memset ( buf_Golden_C , 0 ,(PIC_SIZE_Y>>1) );
    fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
    if (fgOpen == FALSE)
    {
        UTIL_Printf("Open golden file error : %s\n",file_name);
    }
    else
    {
        
    }
    
    if (Golden_UFO && isDump){
        Margin_padding(buf_Golden, buf_Golden_C, PIC_SIZE_Y);
    }
    
    //////////////Y golden comparison////////////////////
    vVDec_InvDCacheRange(Ptr_output_Y,PIC_SIZE_Y);
    u4CompareRange = (((u4RealHeight+31)>>5)<<5) * u4W_lcu;
    ret = memcmp(buf_Golden, Ptr_output_Y, u4CompareRange );
    if ( isDump || ret==0 )
        printk("\n======== Frame %d Golden Y test: %d ========\n", frame_num, ret );

    ret = fgGoldenCmp(Ptr_output_Y,buf_Golden,PIC_SIZE_Y);
    printk("\n======== Frame %d Golden Y test[2]: %d ========\n", frame_num, ret );
    
    if (ret !=0 ){
        if (isDump)
            Dump_mem_ex( Ptr_output_Y, PIC_SIZE_Y, frame_num , 1, isDecodeTimeOut); 
        bCmpFailFlag = 1;
    }

    //////////////C golden comparison////////////////////
    vVDec_InvDCacheRange(Ptr_output_C,(PIC_SIZE_Y >> 1));
    u4CompareRange = (((u4RealHeight/2+15)>>4)<<4) * u4W_lcu;
    ret =  memcmp(buf_Golden_C, Ptr_output_C, u4CompareRange );
    if ( isDump || ret==0 )
        printk("\n======== Frame %d Golden C test: %d ========\n", frame_num, ret );

    ret = fgGoldenCmp(Ptr_output_C,buf_Golden_C,(PIC_SIZE_Y >> 1));
    printk("\n======== Frame %d Golden C test[2]: %d ========\n", frame_num, ret );
 
    if (ret !=0 ){
        if (isDump)
            Dump_mem_ex( Ptr_output_C, PIC_SIZE_Y>>1, frame_num, 2, isDecodeTimeOut );
        bCmpFailFlag = 1;
    }
    //vfree(buf_Golden_C);


/*
    //////////////MV_buf golden comparison////////////////////
    if (ptr_base){ iounmap(ptr_base); }
    ptr_base = ioremap_nocache( mv_buf_addr[current_dpb_idx], MV_COL_PIC_SIZE );

    sprintf(file_name, "%s_pat/mv_pat/mv_col_buf_%d.dat", bitstream_name, frame_num );

    fd = openFile(file_name,O_RDONLY,0); 
    memset ( buf_Golden , 0 ,MV_COL_PIC_SIZE );
    if (IS_ERR(fd) ){
        printk("[Error] Miss file: frameC!!!!!!!!!!!!!\n");
    } else {  
        readFile(fd ,buf_Golden, MV_COL_PIC_SIZE );
        closeFile(fd); 
    }

    ret =  memcmp( buf_Golden, ptr_base, MV_COL_PIC_SIZE );
    if ( isDump || ret==0 )
        printk("\n======== Frame %d Golden MV_buff[%d] test: %d ========\n\n\n", frame_num, current_dpb_idx ,ret );

    if (ret !=0 ){
        if (isDump)
            Dump_mem( ptr_base, MV_COL_PIC_SIZE , frame_num, 3, isDecodeTimeOut );
        vfree(buf_Golden);
        iounmap(ptr_base);
        set_fs( oldfs );
        return 1;

    }
*/

    if (Golden_UFO){

        UINT32 cmp_size;

         //////////////Y LEN comparison////////////////////
        sprintf(file_name, "%s_pat/ufo_pat/ufo_%d_len_Y.out", bitstream_name, frame_num);

        //dump Y golden
        _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
        _tFBufFileInfo[inst_id].pucTargetAddr = buf_Golden;
        _tFBufFileInfo[inst_id].u4TargetSz = UFO_LEN_SIZE_Y;  
        _tFBufFileInfo[inst_id].u4FileLength = 0;  
        memset ( buf_Golden , 0 ,UFO_LEN_SIZE_Y );
        fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
        if (fgOpen == FALSE)
        {
            UTIL_Printf("Open golden file error : %s\n",file_name);
        }
        else
        {
            
        }
        
        ret = memcmp(buf_Golden, Dpb_ufo_Y_len_addr, (PIC_SIZE_Y+255)>>8 );
        if ( isDump || ret==0 )
            printk("\n======== Frame %d UFO Y LEN test: %d ========\n", frame_num, ret );

        if (ret !=0 ){
            if (isDump)
                Dump_mem( Dpb_ufo_Y_len_addr, cmp_size, frame_num , 4, isDecodeTimeOut); 
            bCmpFailFlag = 1;
        }

        //////////////C LEN comparison////////////////////
        sprintf(file_name, "%s_pat/ufo_pat/ufo_%d_len_CbCr.out", bitstream_name, frame_num);

        //dump Y golden
        _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
        _tFBufFileInfo[inst_id].pucTargetAddr = buf_Golden;
        _tFBufFileInfo[inst_id].u4TargetSz = UFO_LEN_SIZE_C;  
        _tFBufFileInfo[inst_id].u4FileLength = 0;  
        memset ( buf_Golden , 0 ,UFO_LEN_SIZE_C );
        fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
        if (fgOpen == FALSE)
        {
            UTIL_Printf("Open golden file error : %s\n",file_name);
        }
        else
        {
            
        }

        ret =  memcmp(buf_Golden, Dpb_ufo_C_len_addr, UFO_LEN_SIZE_Y>>1 );
        if ( isDump || ret==0 )
            printk("\n======== Frame %d UFO C LEN test: %d ========\n", frame_num, ret );

        if (ret !=0 ){
            if (isDump)
                Dump_mem( Dpb_ufo_C_len_addr, cmp_size, frame_num, 5, isDecodeTimeOut );
            bCmpFailFlag = 1;
        }

    }

    if ( bCmpFailFlag){
        return 1;    
    } else {
        return 0;    
    }
    
}


void Load_reference_frame(UINT32 inst_id, UINT32* Dpb_addr,  UINT32* mv_buf_addr, UINT32 frame_num, UINT32 MV_COL_PIC_SIZE, UINT32 PIC_SIZE, UINT32 PIC_SIZE_Y, UINT32 UFO_LEN_SIZE_Y, UINT32 UFO_LEN_SIZE_C   )
{
    char buff[512] = {0};
    char *ptr_base = NULL;
    char *ptr_base2 = NULL;  
    char *ptr_base3 = NULL;  
    char *ptr_base4 = NULL;  
    char file_name[200] = {0};
    struct file *fd; 
    int ret, ref_num; 
    int Dpb_frame_id[MAX_DPB_NUM];
    int i, file_num, file_len;
    BOOL fgOpen = FALSE;
    //initKernelEnv();
    // fd = ttyGS0_open();

    // read ref frame_num
    sprintf(file_name, "%s_pat/mv_pat/dpb_size_%d.dat", bitstream_name, frame_num);

    #if LOAD_DATA_USB
    //Open dpb_size_xxxx
    _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
    _tFBufFileInfo[inst_id].pucTargetAddr = buff;
    _tFBufFileInfo[inst_id].u4TargetSz = 512;  
    _tFBufFileInfo[inst_id].u4FileLength = 0; 
    memset ( buff , 0 , 512 );
    fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
    if (fgOpen == FALSE)
    {
        UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
    }
    #else
    fd = openFile(file_name,O_RDONLY,0); 
    memset ( buff , 0 , 512 );
    if (IS_ERR(fd) ){
        printk("[Error] Miss file: dpb_size_%d!!!!!!!!!!!!!\n", frame_num );
    } else {            
        readFile(fd ,buff, 512 );
        closeFile(fd); 
    }
    #endif

    
    sscanf (buff, "%i ", &ref_num);
    //printk("%d\n", ref_num);
    memset (buff, 0, 512 );


    // read Dpb_frame_id
    sprintf(file_name, "%s_pat/mv_pat/dpb_info_%d.dat", bitstream_name, frame_num);

    #if LOAD_DATA_USB
    //Open dpb_info_xxxx
    _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
    _tFBufFileInfo[inst_id].pucTargetAddr = buff;
    _tFBufFileInfo[inst_id].u4TargetSz = 512;  
    _tFBufFileInfo[inst_id].u4FileLength = 0; 
    memset ( buff , 0 , 512 );
    fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
    if (fgOpen == FALSE)
    {
        UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
    }
    #else
    fd = openFile(file_name,O_RDONLY,0); 
    memset ( buff , 0 , 512 );
    if (IS_ERR(fd) ){
        printk("[Error] Miss file: dpb_info_%d!!!!!!!!!!!!!\n", frame_num);
    } else {            
        readFile(fd ,buff, 512 );
        closeFile(fd); 
    }
    #endif
    
    sscanf (buff, "%i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i"
        , &Dpb_frame_id[0], &Dpb_frame_id[1], &Dpb_frame_id[2], &Dpb_frame_id[3], &Dpb_frame_id[4], &Dpb_frame_id[5]
        , &Dpb_frame_id[6], &Dpb_frame_id[7], &Dpb_frame_id[8], &Dpb_frame_id[9], &Dpb_frame_id[10], &Dpb_frame_id[11]
        , &Dpb_frame_id[12], &Dpb_frame_id[13], &Dpb_frame_id[14], &Dpb_frame_id[15], &Dpb_frame_id[16], &Dpb_frame_id[17]
        , &Dpb_frame_id[18], &Dpb_frame_id[19], &Dpb_frame_id[20], &Dpb_frame_id[21], &Dpb_frame_id[22], &Dpb_frame_id[23], &Dpb_frame_id[24] );
    memset (buff, 0, 512 );


    for ( i = 0; i < ref_num; i++ ){

        if ( Dpb_frame_id[i] == frame_num ){
            printk("Frame %d Output:  Dpb buffer[%d] at 0x%08X -0x%08X\n\n", Dpb_frame_id[i], i , Dpb_addr[i], mv_buf_addr[i]+MV_COL_PIC_SIZE );
            continue;
        } 

        // read Dpb_addr ref frame
        //ptr_base = ioremap_nocache( Dpb_addr[i] , PIC_SIZE );
        ptr_base = Dpb_addr[i];
        
        if (UFO_MODE)
            sprintf(file_name, "%s_pat/ufo_pat/ufo_%d_bits_Y.out", bitstream_name, Dpb_frame_id[i]);
        else
            sprintf(file_name, "%s_pat/sao_pat/frame_%d_Y.dat", bitstream_name, Dpb_frame_id[i]);

        #if LOAD_DATA_USB
        //Open frame_xxxx
        _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
        _tFBufFileInfo[inst_id].pucTargetAddr = ptr_base;
        _tFBufFileInfo[inst_id].u4TargetSz = PIC_SIZE_Y;  
        _tFBufFileInfo[inst_id].u4FileLength = 0; 
        fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
        if (fgOpen == FALSE)
        {
            UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
        }
        #else
        fd = openFile( file_name, O_RDONLY,0); 
        if (IS_ERR(fd) ){
            printk("[Error] Miss file: frameY_%d!!!!!!!!!!!!!\n",Dpb_frame_id[i]);
        } else {             
            readFile(fd ,ptr_base, PIC_SIZE_Y );
            closeFile(fd); 
        }
        #endif
        
        if (UFO_MODE)
            sprintf(file_name, "%s_pat/ufo_pat/ufo_%d_bits_CbCr.out", bitstream_name, Dpb_frame_id[i]);
        else
            sprintf(file_name, "%s_pat/sao_pat/frame_%d_C.dat", bitstream_name, Dpb_frame_id[i]);

        #if LOAD_DATA_USB
        //Open frame_xxxx
        _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
        _tFBufFileInfo[inst_id].pucTargetAddr = (ptr_base+PIC_SIZE_Y);
        _tFBufFileInfo[inst_id].u4TargetSz = (PIC_SIZE -PIC_SIZE_Y);  
        _tFBufFileInfo[inst_id].u4FileLength = 0; 
        fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
        if (fgOpen == FALSE)
        {
            UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
        }
        #else
        fd = openFile( file_name, O_RDONLY,0); 
        if (IS_ERR(fd) ){
            printk("[Error] Miss file: frameC_%d!!!!!!!!!!!!!\n",Dpb_frame_id[i]);
        } else {
            readFile(fd , ptr_base+PIC_SIZE_Y, PIC_SIZE -PIC_SIZE_Y );
            closeFile(fd); 
        }
        #endif

        // read list0 mv_col_buf
        //ptr_base2 = ioremap_nocache( mv_buf_addr[i] , MV_COL_PIC_SIZE );
        ptr_base2 = mv_buf_addr[i];
        sprintf(file_name, "%s_pat/mv_pat/mv_col_buf_%d.dat", bitstream_name, Dpb_frame_id[i]);

        #if LOAD_DATA_USB
        //Open frame_xxxx
        _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
        _tFBufFileInfo[inst_id].pucTargetAddr = ptr_base2;
        _tFBufFileInfo[inst_id].u4TargetSz = MV_COL_PIC_SIZE;  
        _tFBufFileInfo[inst_id].u4FileLength = 0; 
        fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
        if (fgOpen == FALSE)
        {
            UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
        }
        #else
        fd = openFile( file_name, O_RDONLY,0); 
        if (IS_ERR(fd) ){
            printk("[Error] Miss file: mv_col_buf_%d!!!!!!!!!!!!!\n",Dpb_frame_id[i]);
        } else {
            readFile(fd ,ptr_base2, MV_COL_PIC_SIZE );
            closeFile(fd); 
        }
        #endif

        if (UFO_MODE){
            // read UFO_LEN_SIZE_Y
            //ptr_base3 = ioremap_nocache( Dpb_ufo_Y_len_addr[i] , UFO_LEN_SIZE_Y );
            ptr_base3 = Dpb_ufo_Y_len_addr[i];
            sprintf(file_name, "%s_pat/ufo_pat/ufo_%d_len_Y.out", bitstream_name, Dpb_frame_id[i]);

            #if LOAD_DATA_USB
            //Open frame_xxxx
            _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
            _tFBufFileInfo[inst_id].pucTargetAddr = ptr_base3;
            _tFBufFileInfo[inst_id].u4TargetSz = UFO_LEN_SIZE_Y;  
            _tFBufFileInfo[inst_id].u4FileLength = 0; 
            fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
            if (fgOpen == FALSE)
            {
                UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
            }
            #else
            fd = openFile( file_name, O_RDONLY,0); 
            if (IS_ERR(fd) ){
                printk("[Error] Miss file: ufo_%d_len_Y.out!!!!!!!!!!!!!\n",Dpb_frame_id[i]);
            } else {
                readFile(fd ,ptr_base3, UFO_LEN_SIZE_Y );
                closeFile(fd); 
            }
            #endif
            //iounmap(ptr_base3);

            // read UFO_LEN_SIZE_C
            //ptr_base4 = ioremap_nocache( Dpb_ufo_C_len_addr[i] , UFO_LEN_SIZE_C );
            ptr_base4 = Dpb_ufo_C_len_addr[i];
            sprintf(file_name, "%s_pat/ufo_pat/ufo_%d_len_CbCr.out", bitstream_name, Dpb_frame_id[i]);

            #if LOAD_DATA_USB
            _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
            _tFBufFileInfo[inst_id].pucTargetAddr = ptr_base4;
            _tFBufFileInfo[inst_id].u4TargetSz = UFO_LEN_SIZE_C;  
            _tFBufFileInfo[inst_id].u4FileLength = 0; 
            fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
            if (fgOpen == FALSE)
            {
                UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
            }
            #else
            fd = openFile( file_name, O_RDONLY,0); 
            if (IS_ERR(fd) ){
                printk("[Error] Miss file: ufo_%d_len_CbCr.out!!!!!!!!!!!!!\n",Dpb_frame_id[i]);
            } else {
                readFile(fd ,ptr_base4, UFO_LEN_SIZE_C );
                closeFile(fd); 
            }
            #endif
            //iounmap(ptr_base4);

        }

        printk("Frame loaded:%d!! Dpb buffer[%d] at 0x%08X -0x%08X\n\n", Dpb_frame_id[i], i , Dpb_addr[i], mv_buf_addr[i]+MV_COL_PIC_SIZE );

    }
    
    //set_fs( oldfs );

}


void Check_dpb(UINT32 inst_id, UINT32* Dpb_addr,  UINT32* mv_buf_addr, UINT32 frame_num, UINT32 MV_COL_PIC_SIZE, UINT32 PIC_SIZE, UINT32 PIC_SIZE_Y )
{
    char buff[512] = {0};
    char *buff_cmp = NULL;
    char *ptr_base = NULL;
    char *ptr_base2 = NULL;  
    char file_name[200] = {0};
    struct file *fd; 
    int ret, ref_num; 
    int Dpb_frame_id[MAX_DPB_NUM];
    int i, file_num, file_len;
    BOOL fgOpen = FALSE;

    //initKernelEnv();
    // fd = ttyGS0_open();

    // read ref frame_num
    sprintf(file_name, "%s_pat/mv_pat/dpb_size_%d.dat", bitstream_name, frame_num);

    #if LOAD_DATA_USB
    _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
    _tFBufFileInfo[inst_id].pucTargetAddr = buff;
    _tFBufFileInfo[inst_id].u4TargetSz = 512;  
    _tFBufFileInfo[inst_id].u4FileLength = 0; 
    memset ( buff , 0 , 512 );
    fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
    if (fgOpen == FALSE)
    {
        UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
    }
    #else
    fd = openFile(file_name,O_RDONLY,0); 
    memset ( buff , 0 , 512 );
    if (IS_ERR(fd) ){
        printk("[Error] Miss file: dpb_size_%d!!!!!!!!!!!!!\n", frame_num );
    } else {            
        readFile(fd ,buff, 512 );
        closeFile(fd); 
    }
    #endif

    sscanf (buff, "%i ", &ref_num);
    //printk("%d\n", ref_num);
    memset (buff, 0, 512 );
    // read Dpb_frame_id
    sprintf(file_name, "%s_pat/mv_pat/dpb_info_%d.dat", bitstream_name, frame_num);

    #if LOAD_DATA_USB
    _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
    _tFBufFileInfo[inst_id].pucTargetAddr = buff;
    _tFBufFileInfo[inst_id].u4TargetSz = 512;  
    _tFBufFileInfo[inst_id].u4FileLength = 0; 
    memset ( buff , 0 , 512 );
    fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
    if (fgOpen == FALSE)
    {
        UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
    }
    #else
    fd = openFile(file_name,O_RDONLY,0); 
    memset ( buff , 0 , 512 );
    if (IS_ERR(fd) ){
        printk("[Error] Miss file: dpb_info_%d!!!!!!!!!!!!!\n", frame_num);
    } else {            
        readFile(fd ,buff, 512 );
        closeFile(fd); 
    }
    #endif
    
    sscanf (buff, "%i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i"
        , &Dpb_frame_id[0], &Dpb_frame_id[1], &Dpb_frame_id[2], &Dpb_frame_id[3], &Dpb_frame_id[4], &Dpb_frame_id[5]
        , &Dpb_frame_id[6], &Dpb_frame_id[7], &Dpb_frame_id[8], &Dpb_frame_id[9], &Dpb_frame_id[10], &Dpb_frame_id[11]
        , &Dpb_frame_id[12], &Dpb_frame_id[13], &Dpb_frame_id[14], &Dpb_frame_id[15], &Dpb_frame_id[16], &Dpb_frame_id[17]
        , &Dpb_frame_id[18], &Dpb_frame_id[19], &Dpb_frame_id[20], &Dpb_frame_id[21], &Dpb_frame_id[22], &Dpb_frame_id[23], &Dpb_frame_id[24] );

    memset (buff, 0, 512 );


    for ( i = 0; i < ref_num; i++ ){

        if ( Dpb_frame_id[i] == frame_num ){
            printk("\nFrame %d Output:  Dpb buffer[%d] at 0x%08X -0x%08X\n\n", Dpb_frame_id[i], i , Dpb_addr[i], mv_buf_addr[i]+MV_COL_PIC_SIZE );
            continue;
        } 
        
        // read Dpb_addr ref frame
        // Y check
        #if 0
        buff_cmp = (char *) vmalloc( PIC_SIZE_Y );
        ptr_base = ioremap_nocache( Dpb_addr[i] , PIC_SIZE );
        #else
        buff_cmp = g2Temp[inst_id];
        ptr_base = Dpb_addr[i];
        #endif
        
        sprintf(file_name, "%s_pat/sao_pat/frame_%d_Y.dat", bitstream_name, Dpb_frame_id[i]);

        #if LOAD_DATA_USB
        _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
        _tFBufFileInfo[inst_id].pucTargetAddr = buff_cmp;
        _tFBufFileInfo[inst_id].u4TargetSz = PIC_SIZE_Y;  
        _tFBufFileInfo[inst_id].u4FileLength = 0; 
        fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
        if (fgOpen == FALSE)
        {
            UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
        }
        #else
        fd = openFile( file_name, O_RDONLY,0); 
        if (IS_ERR(fd) ){
            printk("[Error] Miss file: frameY_%d!!!!!!!!!!!!!\n",Dpb_frame_id[i]);
        } else {             
            readFile(fd ,buff_cmp, PIC_SIZE_Y );
            closeFile(fd); 
        }
        #endif
        
        ret =  memcmp(buff_cmp, ptr_base, PIC_SIZE_Y );
        printk("\n======== DPB [%d] content frame_%d_Y check %d ========\n", i,Dpb_frame_id[i], ret );
        //vfree(buff_cmp);
        
        if (ret !=0 ){
            Dump_mem( ptr_base, PIC_SIZE_Y, Dpb_frame_id[i] , 1, 0); 
        }


        //C check
        //use g2Temp[inst_id] too
        //buff_cmp = (char *) vmalloc( PIC_SIZE -PIC_SIZE_Y );
        
        sprintf(file_name, "%s_pat/sao_pat/frame_%d_C.dat", bitstream_name, Dpb_frame_id[i]);

        #if LOAD_DATA_USB
        _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
        _tFBufFileInfo[inst_id].pucTargetAddr = buff_cmp;
        _tFBufFileInfo[inst_id].u4TargetSz = (PIC_SIZE -PIC_SIZE_Y);  
        _tFBufFileInfo[inst_id].u4FileLength = 0; 
        fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
        if (fgOpen == FALSE)
        {
            UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
        }
        #else
        fd = openFile( file_name, O_RDONLY,0); 
        if (IS_ERR(fd) ){
            printk("[Error] Miss file: frameC_%d!!!!!!!!!!!!!\n",Dpb_frame_id[i]);
        } else {
            readFile(fd , buff_cmp , PIC_SIZE -PIC_SIZE_Y );
            closeFile(fd); 
        }
        #endif
        
        ret =  memcmp(buff_cmp, ptr_base+PIC_SIZE_Y, PIC_SIZE -PIC_SIZE_Y );
        printk("======== DPB [%d] content frame_%d_C check %d ========\n", i,Dpb_frame_id[i], ret );
        //vfree(buff_cmp);

        if (ret !=0 ){
            Dump_mem( ptr_base+PIC_SIZE_Y, PIC_SIZE -PIC_SIZE_Y, Dpb_frame_id[i] , 2, 0); 
        }


        // read list0 mv_col_buf
        // MV check
        #if 0
        buff_cmp = (char *) vmalloc( MV_COL_PIC_SIZE );
        ptr_base2 = ioremap_nocache( mv_buf_addr[i] , MV_COL_PIC_SIZE );
        #else
        buff_cmp = g2Temp[inst_id];
        ptr_base2 = mv_buf_addr[i];
        #endif
        sprintf(file_name, "%s_pat/mv_pat/mv_col_buf_%d.dat", bitstream_name, Dpb_frame_id[i]);

        #if LOAD_DATA_USB
        _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
        _tFBufFileInfo[inst_id].pucTargetAddr = buff_cmp;
        _tFBufFileInfo[inst_id].u4TargetSz = MV_COL_PIC_SIZE;  
        _tFBufFileInfo[inst_id].u4FileLength = 0; 
        fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
        if (fgOpen == FALSE)
        {
            UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
        }
        #else
        fd = openFile( file_name, O_RDONLY,0); 
        if (IS_ERR(fd) ){
            printk("[Error] Miss file: mv_col_buf_%d!!!!!!!!!!!!!\n",Dpb_frame_id[i]);
        } else {
            readFile(fd ,buff_cmp, MV_COL_PIC_SIZE );
            closeFile(fd); 
        }
        #endif
        ret =  memcmp(buff_cmp, ptr_base2 ,  MV_COL_PIC_SIZE );
        printk("======== DPB [%d] content mv_col_buf_%d check %d ========\n", i,Dpb_frame_id[i], ret );
        //vfree(buff_cmp);
        
        if (ret !=0 ){
            Dump_mem( ptr_base2, MV_COL_PIC_SIZE, Dpb_frame_id[i] , 3, 0); 
        }

    }

}


int  Set_error_bitstream( UINT32  bitstream_addr, UINT32 buffer_size, UINT32 file_size, UINT32 offset ){
    int error_count = 0;
    if ( error_rate >0 ){
        
        
    }
    return error_count;

}

bool UINT32_get_bit(UINT32 val, UINT32 index)
{
    return ((val >> index) & 0x1);
}

void UINT32_set_bit(UINT32* val, UINT32 index, bool bit){
    *val &= (~( 0x1<<index ));
    *val |= ( bit << index  );
}

void set_Dpb_UFO_ctl( UINT32 risc_val1, int Dpb_index )
{
    if(UFO_MODE && (risc_val1>=0) && (risc_val1<32)){
        //RefCtl.bIsUFO[risc_val1] = Dpb.bIsUFO[Dpb_index];
        UINT32_set_bit(&u4RefCtlUFO, risc_val1, 1);
    }
}

void Dump_ESA_NBM_performane_log(char* pBName)
{    
    char file_name[200];
    char info_buff[500];
    UINT32 NBM_DLE_NUM,ESA_REQ_DATA_NUM,MC_REQ_DATA_NUM,MC_MBX,MC_MBY,CYC_SYS,INTRA_CNT,LAT_BUF_BYPASS,Y_BLK_CNT,C_BLK_CNT,WAIT_CNT,REQ_CNT,MC_DLE_NBM,CYCLE_DRAM;

    RISCRead_MC(476, &NBM_DLE_NUM, 0);
    RISCRead_MC(558, &ESA_REQ_DATA_NUM, 0); 
    RISCRead_MC(650, &MC_REQ_DATA_NUM, 0);
    RISCRead_MC(10, &MC_MBX, 0);
    RISCRead_MC(11, &MC_MBY, 0);
    RISCRead_MC(632, &CYC_SYS, 0);
    RISCRead_MC(633, &INTRA_CNT, 0);
    RISCRead_VDEC_TOP(60, &LAT_BUF_BYPASS, 0);
    LAT_BUF_BYPASS &= 0x1;
    RISCRead_MC(634, &Y_BLK_CNT,0);
    RISCRead_MC(635, &C_BLK_CNT,0);
    RISCRead_MC(636, &WAIT_CNT,0);
    RISCRead_MC(493, &REQ_CNT,0);
    RISCRead_MC(477, &MC_DLE_NBM,0);
    RISCRead_MC(478, &CYCLE_DRAM,0);
    
    UTIL_Printf("%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u\n", 
        NBM_DLE_NUM,ESA_REQ_DATA_NUM,MC_REQ_DATA_NUM,MC_MBX,MC_MBY,CYC_SYS,INTRA_CNT,LAT_BUF_BYPASS,Y_BLK_CNT,C_BLK_CNT,WAIT_CNT,REQ_CNT,MC_DLE_NBM,CYCLE_DRAM);

}


void hevc_test( int inst_id,int frame_start , int frame_end )
{
#if 1
    unsigned int frame_width, frame_height;
    int frame_num;
    UINT32 i;
    
    UINT32 PIC_SIZE_Y, PIC_SIZE, MV_COL_PIC_SIZE;
    UINT32 CBCR_ADDR_OFFSET, MV_COL_LIST_0_RD_ADDR, MV_COL_LIST_1_RD_ADDR;

    struct file *fd; 
    struct file *fd_bitstream; 
    struct file *filp_info;
    char *ptr_base;
    char *ptr_base_test;
    char * file_name[200] = {0};

    UINT32 risc_val1, risc_val2, risc_val3, temp;
    UINT32 u4UFOtemp;
    UINT32 maxMC = 0;
    UINT32 maxVLD = 0;
    const int buff_risc_size = 16384;
    
    int i4tmp = 0;
    char risc_temp[100];
    char risc_type[100];
    char risc_addr[100];
    char *buff_risc;

    //int file_num, file_len, read_len;
    int ret, Dpb_empty, isFail;
    int repeat_count = 0;
    
    UINT32 u4RetRegValue;
    UINT32 * ptr_temp;
       
    UINT32 physAlloc = 0x90100000;
    UINT32 u4FileSize;
    UINT32 BITSTREAM_OFFSET;
    UINT32 BitstreamOffsetBase;     // for shift bits
    UINT32 FRAME_BUFFER_OFFSET = 0x9166d000;  
    //UINT32 BITSTREAM_BUFF_SIZE = 0x5000000;     //0x9700000; 0x5000000;  0x2300000
    UINT32 BITSTREAM_BUFF_SHIFT = 0x4600000;    //0x9600000; 0x4600000;  0x1E00000

    UINT32 PP_OUT_Y_ADDR;
    UINT32 PP_OUT_C_ADDR;
    UINT32 u4Align;

    // for UFO MODE
    UINT32 UFO_Y_LEN_ADDR, UFO_C_LEN_ADDR;
    UINT32 UFO_LEN_SIZE_Y, UFO_LEN_SIZE_C;
    UINT32 PIC_SIZE_REF, PIC_SIZE_BS, PIC_SIZE_Y_BS;
    UINT32 PIC_UFO_WIDTH, PIC_UFO_HEIGHT, PIC_W_H;

    UINT32 SliceStartLCURow, u4LCUsize, u4RealWidth, u4W_Dram;
    UINT32 pic_real_wifth, pic_real_height;
    UINT32 MC_130, MC_131, MC_608, VLD_TOP_26, VLD_TOP_28;

    BOOL fgOpen = FALSE;

    char* EC_output_Y;
    char* EC_output_C;
    char* Ptr_output_Y;
    char* Ptr_output_C;
    char* Ptr_Y_LEN;
    char* Ptr_C_LEN;

    debug_mode = 10;
    
    // read pic_width
    sprintf(file_name, "%s_pat/sao_pat/pic_width.dat", bitstream_name );

     #if LOAD_DATA_USB     
     _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
     _tFBufFileInfo[inst_id].pucTargetAddr = risc_temp;
     _tFBufFileInfo[inst_id].u4TargetSz = 100;  
     _tFBufFileInfo[inst_id].u4FileLength = 0; 
     memset ( risc_temp , 0 , 100 );
     fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
     if (fgOpen == FALSE)
     {
        UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
     }     
     #else     
     fd = openFile(file_name,O_RDONLY,0); 
     memset ( risc_temp , 0 , 100 );
     if (IS_ERR(fd) ){
         printk("[Error] Miss file: pic_width.dat!!!!!!!!!!!!!\n" );
     } else {            
         readFile(fd ,risc_temp, 100 );
         closeFile(fd); 
     }     
     #endif // end load_data_usb
     
     sscanf (risc_temp, "%i ", &width);

    // read pic_height
     sprintf(file_name, "%s_pat/sao_pat/pic_height.dat", bitstream_name );

     #if LOAD_DATA_USB
     _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
     _tFBufFileInfo[inst_id].pucTargetAddr = risc_temp;
     _tFBufFileInfo[inst_id].u4TargetSz = 100;  
     _tFBufFileInfo[inst_id].u4FileLength = 0; 
     memset ( risc_temp , 0 , 100 );
     fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
     if (fgOpen == FALSE)
     {
        UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
     }
     #else
     fd = openFile(file_name,O_RDONLY,0); 
     memset ( risc_temp , 0 , 100 );
     if (IS_ERR(fd) ){
         printk("[Error] Miss file: pic_height.dat!!!!!!!!!!!!!\n" );
     } else {            
         readFile(fd ,risc_temp, 100 );
         closeFile(fd); 
     }
     #endif
     sscanf (risc_temp, "%i ", &height);

    if (UFO_MODE)
        printk("\n==================== UFO_MODE ====================\n\n");

    printk("\n%s\nFrame:%d-%d  Width:%d Height:%d ErrorRate:%d Debug:%d\n",  bitstream_name, frame_start, frame_end, width, height, error_rate ,debug_mode );
        
    frame_width = ((width + 63) >> 6) << 6; //64 align
    frame_height = ((height + 63) >> 6) << 6; //64 align
    
    PIC_SIZE_Y = frame_width*frame_height;
    PIC_SIZE = (( PIC_SIZE_Y + ( PIC_SIZE_Y >> 1 ) + 511 )>> 9) << 9; //512 align
    MV_COL_PIC_SIZE = (frame_width*frame_height >> 8) << 4;
    CBCR_ADDR_OFFSET = PIC_SIZE_Y; // C buffer followed Y buffer 

    if (UFO_MODE){
        UFO_LEN_SIZE_Y = ((((PIC_SIZE_Y+255) >>8) + 63 + (16*8)) >>6) << 6;
        UFO_LEN_SIZE_C = (((UFO_LEN_SIZE_Y>>1) + 15 + (16*8)) >>4) <<4;
        PIC_SIZE_Y_BS = ((PIC_SIZE_Y + 4095) >>12) <<12;
        PIC_SIZE_BS = ((PIC_SIZE_Y_BS + (PIC_SIZE_Y >>1) + 511) >>9) <<9;
        PIC_SIZE_REF = (((PIC_SIZE_BS + (UFO_LEN_SIZE_Y << 1)) + 4095) >> 12) <<12;
    }

    
    //VLD_PAT: BITSTREAM
    #if 0
    u4Align = 128;
    BITSTREAM_OFFSET = (physAlloc + u4Align - 1) & (~(u4Align - 1));    //128 align
    BitstreamOffsetBase = BITSTREAM_OFFSET;
    physAlloc = BITSTREAM_OFFSET + BITSTREAM_BUFF_SIZE;       //FIFO for bitstream 100MB
    #else
    VDEC_Pattern_MemoryAllocate(inst_id,VDEC_H265);
    BITSTREAM_OFFSET = g2VFIFO[inst_id];
    BitstreamOffsetBase = BITSTREAM_OFFSET;
    physAlloc = BITSTREAM_OFFSET + BITSTREAM_BUFF_SIZE;       //FIFO for bitstream 100MB
    #endif
    if (debug_mode>0)
        printk("BITSTREAM_OFFSET 0x%08X\n",BITSTREAM_OFFSET);

  
    //load bitstream
    //ptr_base = ioremap_nocache( BITSTREAM_OFFSET, BITSTREAM_BUFF_SIZE );
    ptr_base = g2VFIFO[inst_id];
    memset(ptr_base, 0, BITSTREAM_BUFF_SIZE);
    if (debug_mode>0)
        printk("BITSTREAM_OFFSET ioremap ptr_base = 0x%08.0X\n",ptr_base);

    //initKernelEnv(); 

    if ( error_rate > 0 && debug_mode !=0 ){
        sprintf( file_name, "%s_EB_0x%08X_%d", bitstream_name, 0, error_rate );
    }else{
        sprintf(file_name, "%s_pat/vld_pat/bitstream.bin", bitstream_name  );
    }

    #if LOAD_DATA_USB
     _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
     _tFBufFileInfo[inst_id].pucTargetAddr = ptr_base;
     _tFBufFileInfo[inst_id].u4TargetSz = BITSTREAM_BUFF_SIZE;  
     _tFBufFileInfo[inst_id].u4FileLength = 0; 
     fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
     if (fgOpen == FALSE)
     {
        UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
     }
     #else
     fd = openFile(file_name,O_RDONLY,0); 
     if (IS_ERR(fd) ){
         printk("[Error] Miss file: bitstream!!!!!!!!!!!!!\n");
     } else {  
         u4FileSize = readFileSize(fd ,ptr_base,BITSTREAM_BUFF_SIZE);
         closeFile(fd); 
     }
    #endif
    if ( debug_mode == 0 )
        Set_error_bitstream( BITSTREAM_OFFSET, BITSTREAM_BUFF_SIZE , u4FileSize , 0);

    if (debug_mode>0)
        printk("\n\n==== Bitstream %s, file size = %d ====\n\n", file_name ,u4FileSize);

    if ( debug_mode == 3 ){
        physAlloc = FRAME_BUFFER_OFFSET;
    }

    #if 0
    //  Dpb_addr[i], mv_buf_addr[i] ;
    for ( i=0; i<MAX_DPB_NUM; i++ ){
        if (UFO_MODE){
            u4Align = 4096;
            Dpb_addr[i] = (physAlloc + u4Align - 1) & (~(u4Align - 1));    //4096 align
            Dpb_ufo_Y_len_addr[i] = Dpb_addr[i] + PIC_SIZE_BS;
            Dpb_ufo_C_len_addr[i] = Dpb_ufo_Y_len_addr[i] + UFO_LEN_SIZE_Y;
            if (debug_mode>0){
                printk("Dpb_addr[%d]:0x%08X ", i, Dpb_addr[i] );
                printk("Dpb_ufo_Y_len_addr[%d]:0x%08X ", i, Dpb_ufo_Y_len_addr[i] );
            }
            physAlloc = Dpb_addr[i] + PIC_SIZE_REF;  //PIC_SIZE_REF 4096 aligned
        } else {
            u4Align = 512;
            Dpb_addr[i] = (physAlloc + u4Align - 1) & (~(u4Align - 1));    //512 align
            if (debug_mode>0){
                printk("Dpb_Y_addr[%d]:0x%08X ", i, Dpb_addr[i] );
                printk("Dpb_C_addr[%d]:0x%08X ", i, Dpb_addr[i]+ PIC_SIZE_Y);
            }
            physAlloc = Dpb_addr[i] + PIC_SIZE;  //PIC_SIZE 512 aligned
        }
        u4Align = 128;
        mv_buf_addr[i] = (physAlloc + u4Align - 1) & (~(u4Align - 1));    //128 align
        if (debug_mode>0)
            printk("mv_buf_addr[%d]:0x%08.0X ", i, mv_buf_addr[i] );
        physAlloc = mv_buf_addr[i] + MV_COL_PIC_SIZE;
        if (debug_mode>0)
            printk("End:0x%08.0X\n", physAlloc );

    }
    #else
    //partition DPB & MV buffer
    HEVC_DPB_Partition(inst_id);
    #endif

     //Power on
    RISCWrite_MISC(61,  0x0, 0);

    if ( debug_mode == 5 ){
        // pattern test
        int output_bff_index = 1;
        RISC_instructions( );

    } else{
    
        #if 0
        sprintf(file_name, "%s_info", bitstream_name  );
        fd = openFile(file_name,O_RDONLY,0); 
        if (IS_ERR(fd) ){   // if info file exitst-> append; not exitst -> create
            filp_info = filp_open( file_name, O_CREAT|O_RDWR,0777 );
            sprintf( risc_temp, "Frame# MC476 VLD_TOP40\n" );
            ret = filp_info->f_op->write( filp_info, risc_temp, strlen(risc_temp), &filp_info->f_pos );
        } else {
            closeFile(fd); 
            filp_info = filp_open( file_name , O_APPEND|O_RDWR,0777 );
        }
        #endif
        
        Dpb_empty = 1;
        maxMC = 0;
        maxVLD = 0;
        
        int steps = 0;              // for bitstream shift
        for ( frame_num = frame_start; frame_num<=frame_end; frame_num++ ){ //Loop over frames
            
            UINT32 max_mc476[3] = {0};        //val, co-vldTop40, frame_num
            UINT32 max_vldTop40[3]= {0};      //val, co-mc476, frame_num
            unsigned char ucNalUnitType;
            char* clear_output_buff_Y;
            char* clear_output_buff_C;
        
            if (debug_mode>0)
                printk("\n\n======== HEVC test frame %d ========\n\n" , frame_num );

            //verify frame_number exist
            if (UFO_MODE){
                sprintf(file_name, "%s_pat/ufo_pat/ufo_%d_bits_Y.out", bitstream_name, frame_num);
            }else{
                sprintf(file_name, "%s_pat/sao_pat/frame_%d_Y.dat", bitstream_name, frame_num);
            }
            #if LOAD_DATA_USB
             _tFBufFileInfo[inst_id].fgGetFileInfo = FALSE;  
             _tFBufFileInfo[inst_id].pucTargetAddr = risc_temp;
             _tFBufFileInfo[inst_id].u4TargetSz = 0xF;  
             _tFBufFileInfo[inst_id].u4FileLength = 0; 
             fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
             if (fgOpen == FALSE)
             {
                UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
                break; // no frame stop test
             }
            #else
            fd = openFile(file_name,O_RDONLY,0);

            if ( IS_ERR(fd) ){
                break;  //no frame# stop test
            }else{
                closeFile(fd); 
            }
            #endif
            
            // get output buffer id for PP_out
            sprintf(file_name, "%s_pat/mv_pat/out_buf_id_%d.dat", bitstream_name, frame_num);

            #if LOAD_DATA_USB
             _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
             _tFBufFileInfo[inst_id].pucTargetAddr = risc_temp;
             _tFBufFileInfo[inst_id].u4TargetSz = 100;  
             _tFBufFileInfo[inst_id].u4FileLength = 0; 
             memset ( risc_temp , 0 , 100 );
             fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
             if (fgOpen == FALSE)
             {
                UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
                break; // no frame stop test
             }
            #else
            fd = openFile(file_name,O_RDONLY,0); 
            memset ( risc_temp , 0 , 100 );
            if (IS_ERR(fd) ){
               printk("[Error] Miss file: out_buf_id_%d!!!!!!!!!!!!!\n", frame_num );
            } else {  
               readFile(fd ,risc_temp, 100 );
               closeFile(fd); 
            } 
            #endif
            
            sscanf (risc_temp, "%i ", &current_dpb_idx);
            
            //PP_OUT_Y_ADDR 
            #if 0
            PP_OUT_Y_ADDR = Dpb_addr[current_dpb_idx];
            clear_output_buff_Y = ioremap_nocache( PP_OUT_Y_ADDR, PIC_SIZE_Y );
            memset ( clear_output_buff_Y , 0 ,PIC_SIZE_Y );
            iounmap(clear_output_buff_Y);
            #else
            PP_OUT_Y_ADDR = PHYSICAL(Dpb_addr[current_dpb_idx]);
            clear_output_buff_Y = Dpb_addr[current_dpb_idx];
            EC_output_Y = Dpb_addr[current_dpb_idx];
            Ptr_output_Y = EC_output_Y;
            memset ( clear_output_buff_Y , 0 ,PIC_SIZE_Y );
            #endif
            if (debug_mode>0)
                printk("\nPP_OUT_Y_ADDR 0x%08X (Dpb_addr[%u])\n",PP_OUT_Y_ADDR, current_dpb_idx );
            
            //PP_OUT_C_ADDR
            if (UFO_MODE){
                //to-do make sure pyhsical & virtual address correctly
                //mtk40343
                #if 0
                PP_OUT_C_ADDR = Dpb_addr[current_dpb_idx] + PIC_SIZE_Y_BS;
                UFO_Y_LEN_ADDR = Dpb_ufo_Y_len_addr[current_dpb_idx];
                UFO_C_LEN_ADDR = Dpb_ufo_C_len_addr[current_dpb_idx];
                
                clear_output_buff_Y = ioremap_nocache( UFO_Y_LEN_ADDR, UFO_LEN_SIZE_Y );
                memset ( clear_output_buff_Y , 0 ,UFO_LEN_SIZE_Y );
                iounmap(clear_output_buff_Y);

                clear_output_buff_C = ioremap_nocache( UFO_C_LEN_ADDR, UFO_LEN_SIZE_C );
                memset ( clear_output_buff_C , 0 ,UFO_LEN_SIZE_C );
                iounmap(clear_output_buff_C);
                #endif
            
            } else {
                #if 0
                PP_OUT_C_ADDR = Dpb_addr[current_dpb_idx] + PIC_SIZE_Y;
                #else
                PP_OUT_C_ADDR = PHYSICAL(Dpb_addr[current_dpb_idx] + PIC_SIZE_Y);
                EC_output_C = Dpb_addr[current_dpb_idx] + PIC_SIZE_Y;
                Ptr_output_C = EC_output_C;
                #endif
            }
            
            #if 0
            clear_output_buff_C = ioremap_nocache( PP_OUT_C_ADDR, PIC_SIZE_Y/2 );
            memset ( clear_output_buff_C , 0 ,PIC_SIZE_Y/2 );
            iounmap(clear_output_buff_C);
            #else
            clear_output_buff_C = Dpb_addr[current_dpb_idx] + PIC_SIZE_Y;
            memset ( clear_output_buff_C , 0 ,PIC_SIZE_Y/2 );
            #endif
            
            // for EC reference DPB set black
            if (error_rate > 0 &&  frame_num == frame_start ){
                for (i=0; i<MAX_DPB_NUM; i++){
                    #if 0
                    clear_output_buff_Y = ioremap_nocache( Dpb_addr[i], PIC_SIZE_Y );
                    clear_output_buff_C = ioremap_nocache( Dpb_addr[i]+PIC_SIZE_Y, PIC_SIZE_Y/2 );
                    memset ( clear_output_buff_Y , 0 ,PIC_SIZE_Y );
                    memset ( clear_output_buff_C , 0x80 ,PIC_SIZE_Y/2 );
                    iounmap(clear_output_buff_Y);
                    iounmap(clear_output_buff_C);
                    #else
                    clear_output_buff_Y = Dpb_addr[i];
                    clear_output_buff_C = Dpb_addr[i]+PIC_SIZE_Y;
                    memset ( clear_output_buff_Y , 0 ,PIC_SIZE_Y );
                    memset ( clear_output_buff_C , 0x80 ,PIC_SIZE_Y/2 );
                    #endif
                }
            }

            if (debug_mode>0)
                printk("PP_OUT_C_ADDR 0x%08X (Dpb_addr[%u])\n\n",PP_OUT_C_ADDR, current_dpb_idx);


            //Dpb_empty = 1;  //always load reference golden test flag
            
            if ( frame_num != 0 && Dpb_empty ){
                Load_reference_frame(inst_id, Dpb_addr, mv_buf_addr, frame_num, MV_COL_PIC_SIZE, PIC_SIZE, PIC_SIZE_Y, UFO_LEN_SIZE_Y, UFO_LEN_SIZE_C );
                Dpb_empty = 0;
            }
            
            // HW reset
            HW_RESET( );
            RISCWrite_HEVC_VLD( 33, 1);// HEVC FLAG
            
            //parse RISC

            if (UFO_MODE){

                PIC_UFO_WIDTH = width;
                PIC_UFO_HEIGHT = height;

                PIC_W_H = ((PIC_UFO_WIDTH/16-1)<<16) |(PIC_UFO_HEIGHT/16-1);
                RISCWrite_MC(700, PIC_W_H, 0);
                RISCWrite_MC(664, 0x00000011, 0);
                
                RISCWrite_MC(698, UFO_Y_LEN_ADDR, 0);
                RISCWrite_MC(699, UFO_C_LEN_ADDR, 0);

                RISCWrite_MC(663, PIC_SIZE_BS, 0);
                RISCWrite_MC(701, UFO_LEN_SIZE_Y, 0);
                RISCWrite_MC(343, PIC_SIZE_Y_BS, 0);
                UINT32_set_bit(&u4DpbUFO, current_dpb_idx, 1);
                Golden_UFO = 1;  
            } else {
                RISCWrite_MC(664, 0x0, 0);
                Golden_UFO = 0;
            }
#ifdef EC_SETTINGS
             //EC settings for all error_rate 
            if (pre_dpb_idx==current_dpb_idx){
                pre_dpb_idx = current_dpb_idx+1;
            }
            
            #if 0
            RISCWrite_MC(247, Dpb_addr[pre_dpb_idx]); 
            RISCWrite_MC(279, Dpb_addr[pre_dpb_idx]); 
            RISCWrite_MC(311, Dpb_addr[pre_dpb_idx]);
            RISCWrite_MV(0, mv_buf_addr[pre_dpb_idx]>>4); 
            RISCWrite_MV(16, mv_buf_addr[pre_dpb_idx]>>4); 
            #else
            RISCWrite_MC(247, PHYSICAL(Dpb_addr[pre_dpb_idx]),0); 
            RISCWrite_MC(279, PHYSICAL(Dpb_addr[pre_dpb_idx]),0); 
            RISCWrite_MC(311, PHYSICAL(Dpb_addr[pre_dpb_idx]),0);
            RISCWrite_MV(0, PHYSICAL(mv_buf_addr[pre_dpb_idx]>>4),0); 
            RISCWrite_MV(16, PHYSICAL(mv_buf_addr[pre_dpb_idx]>>4),0); 
            #endif
            
            RISCWrite_HEVC_VLD(56, 0xff7efffb);
            //RISCWrite_HEVC_VLD(52, total_bytes_in_curr_pic); 
            RISCWrite_HEVC_VLD(53, 0x04011101); // 06172013, turn on slice_reconceal_sel
#endif                       
            ///////////////////////////////////////////////////////////////////Read MV risc
            //buff_risc = (char *) vmalloc( buff_risc_size );
            buff_risc = g2RiscBuffer[inst_id];
            temp = 0;

            sprintf(file_name, "%s_pat/out/m_mv_risc_%d.pat", bitstream_name, frame_num);
            #if LOAD_DATA_USB
             _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
             _tFBufFileInfo[inst_id].pucTargetAddr = buff_risc;
             _tFBufFileInfo[inst_id].u4TargetSz = buff_risc_size;  
             _tFBufFileInfo[inst_id].u4FileLength = 0; 
             memset ( buff_risc , 0 ,buff_risc_size );
             fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
             if (fgOpen == FALSE)
             {
                UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
                break; // no frame stop test
             }
            #else
            fd = openFile(file_name,O_RDONLY,0); 
            memset ( buff_risc , 0 ,buff_risc_size );
            if (IS_ERR(fd) ){
                printk("[Error] Miss file: mv_risc!!!!!!!!!!!!!\n");
            } else {              
                readFile(fd ,buff_risc, buff_risc_size);
                closeFile(fd); 
            }
            #endif

            //parse MV
            while ( strlen(buff_risc+temp) > 5 ){

                sscanf(buff_risc+temp, "%s %i %s\n", risc_type, &risc_val1, risc_addr );
                i4tmp = simple_strtol(risc_addr, NULL,10);  //for signed overflow
                risc_val2 = (i4tmp<0)?  0x100000000+i4tmp : i4tmp ;
                
                if (debug_mode>0)
                    printk("//MV: %s ( %u , %s );\n", risc_type, risc_val1, risc_addr);
                if ( !strcmp( risc_type,"RISCWrite_MC" ) ){
                    if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_0" ) ){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[0]), 0 );
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_1" ) ){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[1]), 0 );
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_2" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[2]), 0 );
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_3" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[3]), 0 );
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_4" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[4]), 0);
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_5" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[5]), 0 );
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_6" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[6]), 0);
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_7" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[7]), 0);
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_8" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[8]), 0);
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_9" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[9]), 0);
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_10" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[10]), 0);
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_11" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[11]), 0);
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_12" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[12]), 0);
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_13" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[13]), 0 );
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_14" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[14]), 0);
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_15" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[15]), 0 );
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_16" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[16]), 0);
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_17" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[17]), 0);
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_18" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[18]), 0 );
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_19" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[19]), 0);
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_20" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[20]), 0);
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_21" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[21]), 0);
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_22" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[22]), 0);
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_23" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[23]), 0);
                    } else if ( !strcmp( risc_addr,"MC_BUF_Y_ADDR_24" )){
                        RISCWrite_MC(risc_val1, PHYSICAL(Dpb_addr[24]), 0);
                    } else{
                       printk("MV-mc parse Error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
                       printk("MV-mc: %s ( %u , %s );\n\n", risc_type, risc_val1, risc_addr );
                    }
                }else if ( !strcmp( risc_type,"RISCWrite_MV" ) ){
                    
                     if ( !strcmp( risc_addr,"MV_BUF_ADDR_0" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[0] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 0 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_1" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[1] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 1 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_2" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[2] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 2 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_3" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[3] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 3 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_4" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[4] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 4 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_5" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[5] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 5 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_6" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[6] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 6 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_7" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[7] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 7 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_8" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[8] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 8 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_9" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[9] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 9 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_10" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[10] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 10 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_11" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[11] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 11 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_12" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[12] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 12 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_13" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[13] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 13 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_14" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[14] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 14 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_15" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[15] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 15 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_16" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[16] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 16 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_17" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[17] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 17 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_18" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[18] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 18 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_19" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[19] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 19 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_20" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[20] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 20 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_21" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[21] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 21 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_22" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[22] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 22 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_23" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[23] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 23 );
                     } else if ( !strcmp( risc_addr,"MV_BUF_ADDR_24" ) ){
                        RISCWrite_MV(risc_val1, mv_buf_addr[24] >>4, 0 );
                        set_Dpb_UFO_ctl( risc_val1, 24 );
                     } else{
                        RISCWrite_MV(risc_val1,  risc_val2, 0);
                     }
                }else{
                    printk("MV parse Error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
                    printk("MV: %s ( %u , %s );\n\n", risc_type, risc_val1, risc_addr );
                }
                
                //buffer shift line
                while( (*(buff_risc+temp) != 0x0D) && (*(buff_risc+temp+1) != 0x0A) ){
                    temp ++;
                }
                temp += 2;

            }
            
            #if LOAD_DATA_USB
            #else
            vfree(buff_risc);
            #endif


              /////////////////////////////////////////////////////////////////////Read MC risc
              //buff_risc = (char *) vmalloc( buff_risc_size );
              buff_risc = g2RiscBuffer[inst_id];
              temp = 0;
             
              sprintf(file_name, "%s_pat/out/m_mc_risc_%d.pat", bitstream_name, frame_num);
              #if LOAD_DATA_USB
              _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
              _tFBufFileInfo[inst_id].pucTargetAddr = buff_risc;
              _tFBufFileInfo[inst_id].u4TargetSz = buff_risc_size;  
              _tFBufFileInfo[inst_id].u4FileLength = 0; 
              memset ( buff_risc , 0 ,buff_risc_size );
              fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
              if (fgOpen == FALSE)
              {
                 UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
                 break; // no frame stop test
              }
              #else
              fd = openFile(file_name,O_RDONLY,0); 
              memset ( buff_risc , 0 ,buff_risc_size );
              if (IS_ERR(fd) ){
                 printk("[Error] Miss file: mc_risc!!!!!!!!!!!!!\n");
              } else {  
                 readFile(fd ,buff_risc, buff_risc_size);
                 closeFile(fd); 
              }
              #endif
             
             //parse MC
             while ( strlen(buff_risc+temp) > 5 ){
             
                 sscanf(buff_risc+temp, "%s %i %s\n", risc_type, &risc_val1, risc_addr );
                 i4tmp = simple_strtol(risc_addr, NULL,10);  //for signed overflow
                 risc_val2 = (i4tmp<0)?  0x100000000+i4tmp : i4tmp ;
                 
                 if (debug_mode>0)
                     printk("//MC: %s ( %u , %s );\n", risc_type, risc_val1, risc_addr);
                 if ( risc_val1 == 589 ){
                     RISCWrite_MC(risc_val1, CBCR_ADDR_OFFSET, 0 );
                 }else{
                     RISCWrite_MC(risc_val1, risc_val2, 0 );
                 }
                 //buffer shift line
                 while( (*(buff_risc+temp) != 0x0D) && (*(buff_risc+temp+1) != 0x0A) ){
                     temp ++;
                 }
                 temp += 2;
             }
             
             #if LOAD_DATA_USB
             #else
             vfree(buff_risc);
             #endif


               

             /////////////////////////////////////////////////////////////////////Read PP risc
             //buff_risc = (char *) vmalloc( buff_risc_size );
             buff_risc = g2RiscBuffer[inst_id];
             temp = 0;

             sprintf(file_name, "%s_pat/out/m_pp_risc_%d.pat", bitstream_name, frame_num);

             #if LOAD_DATA_USB
              _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
              _tFBufFileInfo[inst_id].pucTargetAddr = buff_risc;
              _tFBufFileInfo[inst_id].u4TargetSz = buff_risc_size;  
              _tFBufFileInfo[inst_id].u4FileLength = 0; 
              memset ( buff_risc , 0 ,buff_risc_size );
              fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
              if (fgOpen == FALSE)
              {
                 UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
                 break; // no frame stop test
              }
              #else
              fd = openFile(file_name,O_RDONLY,0); 
              memset ( buff_risc , 0 ,buff_risc_size );
              if (IS_ERR(fd) ){
                 printk("[Error] Miss file: pp_risc!!!!!!!!!!!!!\n");
              } else {  
                 readFile(fd ,buff_risc, buff_risc_size);
                 closeFile(fd); 
              }
             #endif
             
            //parse PP
            while ( strlen(buff_risc+temp) > 5 ){

                sscanf(buff_risc+temp, "%s %i %s\n", risc_type, &risc_val1, risc_addr );
                i4tmp = simple_strtol(risc_addr, NULL,10);  //for signed overflow
                risc_val2 = (i4tmp<0)?  0x100000000+i4tmp : i4tmp ;
                
                if (debug_mode>0)
                    printk("//PP: %s ( %u , %s );\n", risc_type, risc_val1, risc_addr);
                if ( !strcmp(risc_addr, "PP_OUT_Y_ADDR" ) ){
                    RISCWrite_MC(risc_val1, PP_OUT_Y_ADDR>>9 , 0);
                }else  if ( !strcmp(risc_addr, "PP_OUT_C_ADDR" ) ){
                    RISCWrite_MC(risc_val1, PP_OUT_C_ADDR>>8 , 0);
                }else  if ( !strcmp(risc_type, "RISCWrite_MV" ) ){
                    RISCWrite_MV(risc_val1,  risc_val2, 0 );
                }else  if ( !strcmp(risc_type, "RISCWrite_MC" ) ){
                    RISCWrite_MC(risc_val1,  risc_val2, 0 );
                }else{
                    printk("PP parse Error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
                    printk("PP: %s ( %u , %s );\n\n", risc_type, risc_val1, risc_addr );
                }

                //buffer shift line
                while( (*(buff_risc+temp) != 0x0D) && (*(buff_risc+temp+1) != 0x0A) ){
                    temp ++;
                }
                temp += 2;

            }
            //vfree(buff_risc);


             /////////////////////////////////////////////////////////////////////Read SQT risc
             //buff_risc = (char *) vmalloc( buff_risc_size );
             buff_risc = g2RiscBuffer[inst_id];
             temp = 0;

             sprintf(file_name, "%s_pat/out/m_sqt_risc_%d.pat", bitstream_name, frame_num);

             #if LOAD_DATA_USB
              _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
              _tFBufFileInfo[inst_id].pucTargetAddr = buff_risc;
              _tFBufFileInfo[inst_id].u4TargetSz = buff_risc_size;  
              _tFBufFileInfo[inst_id].u4FileLength = 0; 
              memset ( buff_risc , 0 ,buff_risc_size );
              fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
              if (fgOpen == FALSE)
              {
                 UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
                 break; // no frame stop test
              }
             #else
              fd = openFile(file_name,O_RDONLY,0); 
              memset ( buff_risc , 0 ,buff_risc_size );
              if (IS_ERR(fd) ){
                 printk("[Error] Miss file: sqt_risc!!!!!!!!!!!!!\n");
              } else {  
                  readFile(fd ,buff_risc, buff_risc_size);
                  closeFile(fd); 
              }
             #endif
            //parse SQT
            while ( strlen(buff_risc+temp) > 5 ){

                sscanf(buff_risc+temp, "%s %i %s\n", risc_type, &risc_val1, risc_addr );
                i4tmp = simple_strtol(risc_addr, NULL,10);  //for signed overflow
                risc_val2 = (i4tmp<0)?  0x100000000+i4tmp : i4tmp ;

                if ( !strcmp(risc_type, "RISCWrite_PP" ) ){
                    if (debug_mode>0)
                        printk("//SQT: %s ( %u , %d );\n", risc_type, risc_val1,risc_val2 );
                    RISCWrite_PP(risc_val1, risc_val2, 0 );
                } else if ( !strcmp(risc_type, "RISCWrite_VLD" ) ){
                    if (debug_mode>0)
                       printk("//SQT: %s ( %u , %d );\n", risc_type, risc_val1, risc_val2);
                    RISCWrite_VLD(risc_val1, risc_val2, 0 );
                } else {
                    printk("SQT parse Error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
                    printk("SQT: %s ( %u , %s );\n\n", risc_type, risc_val1, risc_addr );
                }

                //buffer shift line
                while( (*(buff_risc+temp) != 0x0D) && (*(buff_risc+temp+1) != 0x0A) ){
                    temp ++;
                }
                temp += 2;

            }
            //vfree(buff_risc);


             /////////////////////////////////////////////////////////////////////Read VLD risc
         
             //buff_risc = (char *) vmalloc( buff_risc_size );
             buff_risc = g2RiscBuffer[inst_id];
             temp = 0;

             sprintf(file_name, "%s_pat/out/m_vld_risc_%d.pat", bitstream_name, frame_num);

             #if LOAD_DATA_USB
              _tFBufFileInfo[inst_id].fgGetFileInfo = TRUE;  
              _tFBufFileInfo[inst_id].pucTargetAddr = buff_risc;
              _tFBufFileInfo[inst_id].u4TargetSz = buff_risc_size;  
              _tFBufFileInfo[inst_id].u4FileLength = 0; 
              memset ( buff_risc , 0 ,buff_risc_size );
              fgOpen = fgOpenFile(inst_id, file_name,"r+b", &_tFBufFileInfo[inst_id]);
              if (fgOpen == FALSE)
              {
                 UTIL_Printf("[Error] Open fail %s @ %d!!!!!!!!!!!!!\n",file_name,__LINE__);
                 break; // no frame stop test
              }
             #else
              fd = openFile(file_name,O_RDONLY,0); 
              memset ( buff_risc , 0 ,buff_risc_size );
              if (IS_ERR(fd) ){
                 printk("[Error] Miss file: vld_risc!!!!!!!!!!!!!\n");
              } else {  
                  readFile(fd ,buff_risc, buff_risc_size);
                  closeFile(fd); 
              }
             #endif
            //parse VLD 
            while ( strlen(buff_risc+temp) > 5 ){

                sscanf(buff_risc+temp, "%s %i %s\n", risc_type, &risc_val1, risc_addr );
                i4tmp = simple_strtol(risc_addr, NULL,10);  //for signed overflow
                risc_val2 = (i4tmp<0)?  0x100000000+i4tmp : i4tmp ;
                
                if ( !strcmp(risc_type, "INIT_BARREL_SHIFTER" ) ){
                    sscanf(buff_risc+temp, "%s %s %i\n", risc_type, risc_temp, &risc_val1 );
                    if (debug_mode>0)
                        printk("//VLD: %s ( %s + %u );\n", risc_type, risc_temp, risc_val1 );
                    
                    if ( risc_val1 > BITSTREAM_BUFF_SHIFT  && steps != ((risc_val1-(risc_val1%BITSTREAM_BUFF_SHIFT))/BITSTREAM_BUFF_SHIFT) ){
                        //load bitstream
                        #if 0
                        steps = ((risc_val1-(risc_val1%BITSTREAM_BUFF_SHIFT))/BITSTREAM_BUFF_SHIFT);

                        memset(ptr_base, 0, BITSTREAM_BUFF_SIZE);

                        if ( error_rate >0 && debug_mode >0){
                            sprintf( file_name, "%s_EB_0x%08X_%d", bitstream_name+18, steps* BITSTREAM_BUFF_SHIFT, error_rate );
                            fd = openFile(file_name,O_RDONLY,0); 
                            readFile(fd ,ptr_base,BITSTREAM_BUFF_SIZE );
                        } else {
                            sprintf(file_name, "%s_pat/vld_pat/bitstream.bin", bitstream_name  );
                            fd = openFile(file_name,O_RDONLY,0); 
                            readFileOffset(fd ,ptr_base,BITSTREAM_BUFF_SIZE, steps* BITSTREAM_BUFF_SHIFT );
                        }
                        
                        BitstreamOffsetBase = BITSTREAM_OFFSET -steps*BITSTREAM_BUFF_SHIFT;
                        if ( debug_mode == 0 )
                            Set_error_bitstream( BITSTREAM_OFFSET, BITSTREAM_BUFF_SIZE , u4FileSize-steps* BITSTREAM_BUFF_SHIFT ,steps* BITSTREAM_BUFF_SHIFT);

                        if (debug_mode>0)
                            printk("\n\n======== bitstream shift 0x%08x (%d steps) done ========\n\n",  steps* BITSTREAM_BUFF_SHIFT , steps);
                    #endif
                    }
                    INIT_BARREL_SHIFTER( BitstreamOffsetBase + risc_val1 );

                } else if ( !strcmp(risc_type, "HEVC_SHIFT_BITS" ) ){
                    if (debug_mode>0)
                        printk("//VLD: %s ( %u);\n", risc_type, risc_val1 );
                    HEVC_SHIFT_BITS( 0 );
                    HEVC_SHIFT_BITS( risc_val1 );
                } else if ( !strcmp(risc_type, "RISCWrite_HEVC_VLD" ) ){
                    if (debug_mode>0)
                        printk("//VLD: %s ( %u , %d );\n", risc_type, risc_val1, risc_val2 );
#ifdef EC_SETTINGS
                    //EC settings
                    if ( risc_val1 == 45){       
                        if ( risc_val2 & 0x1){  //tiles_enabled_flag
                            RISCWrite_PP(16, 16793617, 0); // 0x0100_4011
                        } else {
                            RISCWrite_PP(16, 16805905, 0); // 0x0100_7011
                        }
                    }
#endif             
                    if ( error_rate > 0 && risc_val1 == 61){
                        ucNalUnitType = (risc_val2 >> 8) & 0xff;
                    }
                    if ( UFO_MODE && risc_val1 == 45){
                        if ( risc_val2 & 0x1){  //if tiles_enabled_flag turn off UFO encoder
                            UINT32_set_bit(&u4DpbUFO, current_dpb_idx, 0);
                            RISCWrite_MC(664, 0x10, 0);     // UFO decoder on
                            Golden_UFO = 0;                           
                        }

                        printk("u4DpbUFO: 0x%08X;  u4RefCtlUFO: 0x%08X  \n", u4DpbUFO, u4RefCtlUFO);
                        RISCWrite_MC(722, 0x1, 0 );
                        RISCWrite_MC(718, u4RefCtlUFO, 0 );
                        RISCWrite_MC(719, u4RefCtlUFO, 0 );
                        RISCWrite_MC(720, u4RefCtlUFO, 0 );
                        
                    }
                    RISCWrite_HEVC_VLD ( risc_val1, risc_val2 );
                } else if ( !strcmp(risc_type, "RISCWrite_VLD_TOP" ) ){
                    if (debug_mode>0)
                        printk("//VLD: %s ( %u, %d );\n", risc_type, risc_val1, risc_val2 );
                    RISCWrite_VLD_TOP  ( risc_val1, risc_val2, 0 );
                } else if ( !strcmp(risc_type, "PRED_WEIGHT_TABLE" ) ){
                    if (debug_mode>0)
                        printk("//VLD: PRED_WEIGHT_TABLE();\n" );
                    PRED_WEIGHT_TABLE();
                } else if ( !strcmp(risc_type, "REF_PIC_LIST_MOD" ) ){
                    if (debug_mode>0)
                        printk("//VLD: REF_PIC_LIST_MOD();\n" );
                    REF_PIC_LIST_MOD();
                } else if ( !strcmp(risc_type, "HEVC_VDEC_TRIG" ) ){

                    if (UFO_MODE){
                        //Set UFO garbage remove mode
                        RISCWrite_PP( 706  , 0x1, 0 );
                    }
                    RISCWrite_VLD_TOP  ( 21,  0x1, 0 );    // turn on VLD TOP [40] information

                    if (1){
                        // dump bitstream buffer before trigger decode
                        #if 0 //fats not support write feature.
                        if (debug_mode == 3){
                            sprintf( file_name, "bitstream_bf_trig.bin");
                            fd_bitstream = openFile( file_name, O_CREAT|O_RDWR, 0 );
                            ret = fd_bitstream->f_op->write( fd_bitstream, ptr_base, (BITSTREAM_BUFF_SIZE>u4FileSize)? u4FileSize : BITSTREAM_BUFF_SIZE, &fd_bitstream->f_pos );        
                            closeFile( fd_bitstream );
                        }
                        #endif

                        printk("Decode_Before!!\n");
                        Dump_reg (HEVC_VLD_REG_OFFSET0, 0, 0, bitstream_name, frame_num, 1);      //HEVC
                        Dump_reg (HEVC_VLD_REG_OFFSET0, 33, 37, bitstream_name, frame_num, 1);  //HEVC
                        Dump_reg (HEVC_VLD_REG_OFFSET0, 40, 255, bitstream_name, frame_num, 1);     //HEVC
                        //Dump_reg (0xF0088000, 40, 255, bitstream_name, frame_num, 1);     //HEVC
                        Dump_reg (VLD_REG_OFFSET0, 33, 255, bitstream_name, frame_num, 1);      //VLD
                        Dump_reg (AVC_MV_REG_OFFSET0, 0, 255, bitstream_name, frame_num, 1);        //MV
                        Dump_reg (MC_REG_OFFSET0, 0, 702, bitstream_name, frame_num, 1);        //MC
                        Dump_reg (HEVC_PP_REG_OFFSET0, 0, 1023, bitstream_name, frame_num, 1);       //PP
                        Dump_reg (VLD_TOP_REG_OFFSET0, 0, 128, bitstream_name, frame_num, 1);       //VLD_TOP
                        printk("//VLD: HEVC_VDEC_TRIG();\n");
                    }
                    HEVC_VDEC_TRIG();
                    break;
                    
                } else {
                    printk("VLD parse Error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
                    printk("VLD: %s ( %u , %s );\n\n", risc_type, risc_val1, risc_addr );
                }
                
                //buffer shift line
                while( (*(buff_risc+temp) != 0x0D) && (*(buff_risc+temp+1) != 0x0A) ){
                    temp ++;
                }
                temp += 2;

            }
            //vfree(buff_risc);

            //polling interrupt
            isDecodeTimeOut = Wait_decode_finished( jiffies );
            
            //Dump_ESA_NBM_performane_log( bitstream_name );


            if (debug_mode>0 ||isDecodeTimeOut ){
                // dump bitstream buffer before trigger decode
                #if 0 //fats not support write feature.
                if (debug_mode == 3){
                    sprintf( file_name, "bitstream_af_trig.bin");
                    fd_bitstream = openFile( file_name, O_CREAT|O_RDWR,0777 );
                    ret = fd_bitstream->f_op->write( fd_bitstream, ptr_base, (BITSTREAM_BUFF_SIZE>u4FileSize)? u4FileSize : BITSTREAM_BUFF_SIZE , &fd_bitstream->f_pos );        
                    closeFile( fd_bitstream );
                }
                #endif
                
                printk("Decode_finished!!\n");
                Dump_reg (HEVC_VLD_REG_OFFSET0, 0, 0, bitstream_name, frame_num, 1);      //HEVC
                Dump_reg (HEVC_VLD_REG_OFFSET0, 33, 37, bitstream_name, frame_num, 1);  //HEVC
                Dump_reg (HEVC_VLD_REG_OFFSET0, 40, 255, bitstream_name, frame_num, 1);     //HEVC
                //Dump_reg (0xF0088000, 40, 255, bitstream_name, frame_num, 1);     //HEVC
                Dump_reg (VLD_REG_OFFSET0, 33, 255, bitstream_name, frame_num, 1);      //VLD
                Dump_reg (AVC_MV_REG_OFFSET0, 0, 255, bitstream_name, frame_num, 1);        //MV
                Dump_reg (MC_REG_OFFSET0, 0, 702, bitstream_name, frame_num, 1);        //MC
                Dump_reg (HEVC_PP_REG_OFFSET0, 0, 1023, bitstream_name, frame_num, 1);       //PP
                Dump_reg (VLD_TOP_REG_OFFSET0, 0, 128, bitstream_name, frame_num, 1);       //VLD_TOP
             
            }


#ifdef EC_SETTINGS
            //VP mode for end of bitstream error
            RISCRead_HEVC_VLD( 57, &risc_val1 );     

            //if (frame_num == 1){  risc_val1 = 1; }      // force frame num turn on VP mode for debug
            if ( risc_val1 & 0x1 ){
                printk("VP mode\n");

                RISCRead_HEVC_VLD( 42, &risc_val1);
                u4LCUsize = 1<<((risc_val1>>4) & 0x7);             
                RISCRead_HEVC_VLD( 62, &risc_val1 );
                SliceStartLCURow = (risc_val1>>16)  & 0x3ff;
                RISCRead_VLD_TOP(28, &risc_val1, 0);
                u4RealWidth = risc_val1 & 0xFFFF; 
                u4W_Dram = ((u4RealWidth + 64-1)/64)*64;

                if ((SliceStartLCURow%2)==1 && u4LCUsize==16 ){
                    SliceStartLCURow--;
                }

                ///////// test
                RISCRead_MC(130, &pic_real_wifth, 0);
                MC_130 =  ((pic_real_wifth+15)>>4)<<4;
                RISCRead_MC(131, &pic_real_height, 0);
                pic_real_height -= SliceStartLCURow*u4LCUsize;
                MC_131 = ((pic_real_height+15)>>4)<<4;
                RISCRead_MC(608, &MC_608, 0);
                VLD_TOP_26 = ((((pic_real_height+15)/16-1)& 0x7ff)<<16) |(((pic_real_wifth+15)/16-1)& 0x7ff);
                VLD_TOP_28 =  (((pic_real_height+15)>>4)<<20) | (((pic_real_wifth+15)>>4)<<4);

                HW_RESET( );
                
                RISCRead_VLD_TOP( 36, &risc_val1, 0 );
                RISCWrite_VLD_TOP( 36,  risc_val1 | (0x1<<1), 0 );  //Turn on VP mode flag

                RISCWrite_MC( 130, MC_130, 0);
                RISCWrite_MC( 131, MC_131, 0);
                RISCWrite_MC( 608, MC_608, 0);
                
                RISCWrite_VLD_TOP( 26, VLD_TOP_26, 0 );
                RISCWrite_VLD_TOP( 28, VLD_TOP_28, 0 );

                RISCWrite_MC(9, 0x1, 0);
                #if 0
                RISCWrite_MC( 0, (Dpb_addr[pre_dpb_idx] + u4W_Dram*SliceStartLCURow*u4LCUsize)>>9 );
                RISCWrite_MC( 1, (Dpb_addr[pre_dpb_idx] + PIC_SIZE_Y + u4W_Dram*SliceStartLCURow*u4LCUsize/2)>>8);
                RISCWrite_MC( 2, (Dpb_addr[current_dpb_idx] + u4W_Dram*SliceStartLCURow*u4LCUsize)>>9  );
                RISCWrite_MC( 3, (Dpb_addr[current_dpb_idx] + PIC_SIZE_Y+ u4W_Dram*SliceStartLCURow*u4LCUsize/2)>>8);
                #else
                RISCWrite_MC(0, PHYSICAL((Dpb_addr[pre_dpb_idx] + u4W_Dram*SliceStartLCURow*u4LCUsize)>>9), 0 );
                RISCWrite_MC(1, PHYSICAL((Dpb_addr[pre_dpb_idx] + PIC_SIZE_Y + u4W_Dram*SliceStartLCURow*u4LCUsize/2)>>8), 0);
                RISCWrite_MC(2, PHYSICAL((Dpb_addr[current_dpb_idx] + u4W_Dram*SliceStartLCURow*u4LCUsize)>>9), 0);
                RISCWrite_MC(3, PHYSICAL((Dpb_addr[current_dpb_idx] + PIC_SIZE_Y+ u4W_Dram*SliceStartLCURow*u4LCUsize/2)>>8), 0);
                #endif
                
                RISCRead_VLD_TOP( 36, &risc_val1, 0 );
                RISCWrite_VLD_TOP( 36,  risc_val1 |0x1, 0 );  // Trigger VP mode 

                RISCRead_MISC( 41 , &risc_val1, 0 );
                RISCWrite_MISC( 41 , risc_val1 & (~(0x1<<12)), 0) ;

                isDecodeTimeOut = Wait_decode_finished( jiffies );
                
            }
 
            if (debug_mode>0 || isDecodeTimeOut){
                Dump_reg (0xF6021800, 26, 44, bitstream_name, frame_num, 1);       //VLD_TOP
                Dump_reg (0xF6021000, 33, 255, bitstream_name, frame_num, 1);      //VLD
                Dump_reg (0xF6022000, 0, 702, bitstream_name, frame_num, 1);        //MC
            }
#endif   
            pre_dpb_idx = current_dpb_idx;

            // write mc476 & vld 40 to file
            RISCRead_MC(476, &risc_val1, 0);
            if ( maxMC < risc_val1 ){
                maxMC = risc_val1;
            }
            printk("max MC[476] = 0x%08X ;", maxMC );

            RISCRead_VLD_TOP( 40, &risc_val2, 0);
            if ( maxVLD < risc_val2 ){
                maxVLD = risc_val2;
            }
            printk("max VLD_TOP[40] = 0x%08X \n", maxVLD );
            sprintf( risc_temp, "%d %u %u\n", frame_num ,risc_val1 ,risc_val2  );

            #if 0
            ret = filp_info->f_op->write( filp_info, risc_temp, strlen(risc_temp), &filp_info->f_pos );
            #endif

            if ( error_rate > 0 ){
                printk("\n======== Frame %d decode done ========\n", frame_num );

                //EC_output_Y = ioremap_nocache( PP_OUT_Y_ADDR, PIC_SIZE_Y );
                //EC_output_C = ioremap_nocache( PP_OUT_C_ADDR, PIC_SIZE_Y/2 );

                if (isDecodeTimeOut){
                    Dump_mem( EC_output_Y, PIC_SIZE_Y, frame_num , 1, isDecodeTimeOut ); 
                    Dump_mem( EC_output_C, PIC_SIZE_Y/2, frame_num , 2, isDecodeTimeOut ); 
                }

                Dump_yuv( EC_output_Y, EC_output_C, PIC_SIZE_Y);
                
                //iounmap(EC_output_Y);
                //iounmap(EC_output_C);
                if ( debug_mode == 4 )
                    Check_dpb(inst_id, Dpb_addr, mv_buf_addr, frame_num, MV_COL_PIC_SIZE, PIC_SIZE, PIC_SIZE_Y );

                isFail = 0;
                
            } else {
                

                //Ptr_output_Y = ioremap_nocache( PP_OUT_Y_ADDR, PIC_SIZE_Y );
                //Ptr_output_C = ioremap_nocache( PP_OUT_C_ADDR, PIC_SIZE_Y/2 );

                #if 0 //UFO need check
                if (UFO_MODE){
                    Ptr_Y_LEN = ioremap_nocache( UFO_Y_LEN_ADDR, UFO_LEN_SIZE_Y );
                    Ptr_C_LEN = ioremap_nocache( UFO_C_LEN_ADDR, UFO_LEN_SIZE_C );
                }
                #endif

                /*
                isFail = Golden_comparison( frame_num, PIC_SIZE_Y, Ptr_output_Y, Ptr_output_C, MV_COL_PIC_SIZE, 0, 
                                                           Ptr_Y_LEN, Ptr_C_LEN, UFO_LEN_SIZE_Y, UFO_LEN_SIZE_C);
                */
                isFail = 1;
                if ( isFail ){
                    printk("\nOutput buffer zero padding:\n");
                    printk("\nwithout Margin padding for debug:\n");
                    //Margin_padding(Ptr_output_Y, Ptr_output_C, PIC_SIZE_Y );

                    isFail = Golden_comparison(inst_id, frame_num, PIC_SIZE_Y, Ptr_output_Y, Ptr_output_C, MV_COL_PIC_SIZE, 1, 
                                                               Ptr_Y_LEN, Ptr_C_LEN, UFO_LEN_SIZE_Y, UFO_LEN_SIZE_C);
                    
                }
                //Dump_yuv( Ptr_output_Y, Ptr_output_C, PIC_SIZE_Y);
               
                //iounmap(Ptr_output_Y);
                //iounmap(Ptr_output_C);
                
                #if 0 //need check
                if (UFO_MODE){
                    iounmap(Ptr_Y_LEN);
                    iounmap(Ptr_C_LEN);
                }
                #endif
               
            }
            
            if ( abs(debug_mode) ==2 ){       // single frame repeat test
                repeat_count++;
                frame_num--;
                printk("Repeat count = %d\n", repeat_count);
                if (repeat_count >=  frame_end -frame_start )
                    break;
            }

            if (isDecodeTimeOut ){
                break;
            }
            if (isFail ){
                //Check_dpb( Dpb_addr, mv_buf_addr, frame_num, MV_COL_PIC_SIZE, PIC_SIZE, PIC_SIZE_Y );
                //Dpb_empty = 1;  //reload reference frame
                //mtk40343 continue decode
                printk("@@@@@@@@@@ compare mismatch !\n");
                break;
            }else{
                Dpb_empty = 0;
            }

            if ( error_rate > 0){
                if ( 16 <= ucNalUnitType &&  23 >= ucNalUnitType ){
                    #if 0 //do nothing
                    char *IDR_ptr_base;
                    // read Golden for EC
                    IDR_ptr_base = ioremap_nocache( Dpb_addr[current_dpb_idx] , PIC_SIZE );
                    sprintf(file_name, "%s_pat/sao_pat/frame_%d_Y.dat", bitstream_name, frame_num );
                    
                    fd = openFile( file_name, O_RDONLY,0); 
                    if (IS_ERR(fd) ){
                        printk("[Error] Miss file: frameY_%d!!!!!!!!!!!!!\n",frame_num);
                    } else {             
                        readFile(fd ,IDR_ptr_base, PIC_SIZE_Y );
                        closeFile(fd); 
                    }
                    sprintf(file_name, "%s_pat/sao_pat/frame_%d_C.dat", bitstream_name, frame_num );
                    
                    fd = openFile( file_name, O_RDONLY,0); 
                    if (IS_ERR(fd) ){
                        printk("[Error] Miss file: frameC_%d!!!!!!!!!!!!!\n",frame_num);
                    } else {
                        readFile(fd , IDR_ptr_base+PIC_SIZE_Y, PIC_SIZE -PIC_SIZE_Y );
                        closeFile(fd); 
                    }
                    iounmap(IDR_ptr_base);
                #endif
                }
                
            }
        } 

        //closeFile(filp_info);
        printk( "\n--%s test end #%d --\n\n", bitstream_name, frame_num );
        //iounmap(ptr_base);
    }
#endif 
}


// move to risc_pattern.c
#if 0
void VDEC_Pattern_MemoryAllocate(int inst_id,int vdec_type)
{
    switch (vdec_type)
    {
        case VDEC_H265:
            HEVC_Pattern_MemoryAllocate(inst_id);
            break;
        default:
            UTIL_Printf("Please check codec type\n");
            break;
    }
}


void HEVC_Pattern_MemoryAllocate(int inst_id)
{
    UTIL_Printf("==========HEVC memory allocate ==============\n");
    //allocate VFIFO buffer
    g2VFIFO[inst_id] = (UCHAR *)x_alloc_aligned_verify_mem(VFIFO_SZ,1024, 1);
    UTIL_Printf("VFIFO buffer[%d] 0x%08x size 0x%x\n",inst_id,g2VFIFO[inst_id],VFIFO_SZ);
    //allocate DPB buffer
    g2DPB[inst_id] = (UCHAR *)x_alloc_aligned_verify_mem(DPB_SZ,4096, 1);
    UTIL_Printf("DPB buffer[%d] 0x%08x size 0x%x\n",inst_id,g2DPB[inst_id],DPB_SZ);
    //allocate Y golden buffer 
    g2YGolden[inst_id] = (UCHAR *)x_alloc_aligned_verify_mem(PATTERN_Y_GOLDEN_SZ,1024, 1);
    UTIL_Printf("g2YGolden buffer[%d] 0x%08x size 0x%x\n",inst_id,g2YGolden[inst_id],PATTERN_Y_GOLDEN_SZ);
    //allocate C golden buffer 
    g2CGolden[inst_id] = (UCHAR *)x_alloc_aligned_verify_mem(PATTERN_C_GOLDEN_SZ,1024, 1);
    UTIL_Printf("g2CGolden buffer[%d] 0x%08x size 0x%x\n",inst_id,g2CGolden[inst_id],PATTERN_C_GOLDEN_SZ);
    //allocate Temp buffer 
    g2Temp[inst_id] = (UCHAR *)x_alloc_aligned_verify_mem(PATTERN_Y_GOLDEN_SZ,1024, 1);
    UTIL_Printf("g2Temp buffer[%d] 0x%08x size 0x%x\n",inst_id,g2Temp[inst_id],PATTERN_Y_GOLDEN_SZ);
    //allocate Temp buffer 
    g2MVBuff[inst_id] = (UCHAR *)x_alloc_aligned_verify_mem(MV_SZ,1024, 1);
    UTIL_Printf("g2MVBuff buffer[%d] 0x%08x size 0x%x\n",inst_id,g2MVBuff[inst_id],MV_SZ);

    //allocate risc buffer
    g2RiscBuffer[inst_id] = (UCHAR *)x_alloc_aligned_verify_mem(RISC_BUFFER_SIZE,1024, 1);
    UTIL_Printf("g2RiscBuffer buffer[%d] 0x%08x size 0x%x\n",inst_id,g2RiscBuffer[inst_id],RISC_BUFFER_SIZE);
}
#endif


//
// Output virtual address
// 

void HEVC_DPB_Partition(int inst_id)
{
    UINT32 i;
    UINT32 frame_width,frame_height;
    UINT32 PIC_SIZE_Y,PIC_SIZE,MV_COL_PIC_SIZE;
    
    frame_width = ((width + 63) >> 6) << 6; //64 align
    frame_height = ((height + 63) >> 6) << 6; //64 align
    PIC_SIZE_Y = frame_width*frame_height;
    PIC_SIZE = (( PIC_SIZE_Y + ( PIC_SIZE_Y >> 1 ) + 511 )>> 9) << 9; //512 align
    MV_COL_PIC_SIZE = (frame_width*frame_height >> 8) << 4;
    //for ( i = 0; i < MAX_DPB_NUM; i++ ){
    for ( i = 0; i < 5; i++ ){
        if (UFO_MODE)
        {
            
        } 
        else 
        {
            Dpb_addr[i] = g2DPB[inst_id] + i*PIC_SIZE;
            if (debug_mode>0){
                UTIL_Printf("Dpb_Y_addr[%d]:0x%08X \n", i, Dpb_addr[i] );
                UTIL_Printf("Dpb_C_addr[%d]:0x%08X \n", i, Dpb_addr[i]+ PIC_SIZE_Y);
            }
            if((Dpb_addr[i] + PIC_SIZE) >= (g2DPB[inst_id] + DPB_SZ))
            {
                UTIL_Printf("Error ++++++++++++++++++++++++++++++++++\n");
                UTIL_Printf("==========DPB Size Not Enough========\n");
                UTIL_Printf("Error ++++++++++++++++++++++++++++++++++\n");
            }
        }
        
        mv_buf_addr[i] = g2MVBuff[inst_id] + MV_COL_PIC_SIZE;    //128 align
        if (debug_mode>0)
            UTIL_Printf("mv_buf_addr[%d]:0x%08.0X \n", i, mv_buf_addr[i] );
        
        if((mv_buf_addr[i] + MV_COL_PIC_SIZE) >= (g2MVBuff[inst_id] + MV_SZ))
        {
            UTIL_Printf("Error ++++++++++++++++++++++++++++++++++\n");
            UTIL_Printf("==========MV Size Not Enough========\n");
            UTIL_Printf("Error ++++++++++++++++++++++++++++++++++\n");
        }

    }
}


void RISC_instructions()
{

}


