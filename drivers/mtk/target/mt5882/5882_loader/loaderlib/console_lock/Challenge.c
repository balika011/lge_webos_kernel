#include<stdio.h>
#include <string.h>

#include "Define.h"
#include "Tools.h"

#include "x_printf.h"
#include "x_drv_cli.h"
#include "x_assert.h"
#include "x_serial.h"
#include "x_stl_lib.h"
#include "x_timer.h"
#include "x_hal_arm.h"
#include "x_hal_5381.h"
#include "x_bim.h"
#include "x_pdwnc.h"
#include "drvcust_if.h"
#include "x_typedef.h"
#include "x_hal_5381.h"
#include "loader_imghdr.h"
#include "loader_if.h"
#include "eeprom_if.h"
#include "c_model.h"
#include "x_ldr_env.h"

#define DEF_SERIAL_NO "MT5592-123456"
//#define DEF_KEY_FILE "../key/key"
//#define DEF_IV_FILE "../key/iv"

//INT32 Challenge( const unsigned char *input )
unsigned char random_number[DEF_RANDOM_LEN];
unsigned char b64_challenge[DEF_B64_CHALLENGE_LEN+1];

INT32 Challenge(unsigned char *input)
{
    int ret = 0;
    unsigned char challenge[DEF_CHALLENGE_LEN];
    unsigned char enc_key[DEF_ENC_KEY_LEN]={0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55} ;
    unsigned char enc_iv[DEF_ENC_IV_LEN]={0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA};
	
	unsigned char serial_no[1024];
	
	//LDR_Loader_Sig* prLoaderVerify = (LDR_Loader_Sig*)(0x2d00000);
	
	x_memset(serial_no,0,1024);
	x_memcpy(serial_no,input,x_strlen(input));;
	
    CSL_PRINT("serial_no : %s ,len= %d\n",serial_no,x_strlen(serial_no));

	//x_memset(enc_key,0x11,DEF_ENC_KEY_LEN);
	//x_memset(enc_iv,0x22,DEF_ENC_IV_LEN);
	//x_memcpy(enc_key,prLoaderVerify->u4AesKey2,DEF_ENC_KEY_LEN);
	//x_memcpy(enc_iv,prLoaderVerify->u4InitialVecter,DEF_ENC_IV_LEN);
    //Create Challenge.
    get_random_number( random_number );
    ret = CreateChallenge( serial_no, random_number, challenge, enc_key, DEF_ENC_KEY_LEN, enc_iv, DEF_ENC_IV_LEN );
    if( ret != 0 ){
         CSL_ERR_PRINT("CreateChallenge error\n");
         return -1;
    }

    ret = base64_encode( challenge, DEF_CHALLENGE_LEN, b64_challenge, DEF_B64_CHALLENGE_LEN );
    if( ret < 0 ){
         CSL_ERR_PRINT("base64_encode error\n");
         return -1;
    }
	
    b64_challenge[ret] = '\0';

    CSL_PRINT("serial_no = %s chanllenge =  %s ",serial_no, b64_challenge);

    return 0;
}
INT32 VerifierResponse(unsigned char *input)
{
	  //Verify Response.
	  int ret = 0;
	  unsigned char enc_key[DEF_ENC_KEY_LEN]={0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55} ;
	  unsigned char enc_iv[DEF_ENC_IV_LEN]={0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA};

	
	//LDR_Loader_Sig* prLoaderVerify = (LDR_Loader_Sig*)(0x2d00000);
	//x_memcpy(enc_key,prLoaderVerify->u4AesKey2,DEF_ENC_KEY_LEN);
	//x_memcpy(enc_iv,prLoaderVerify->u4InitialVecter,DEF_ENC_IV_LEN);
	
	//x_memset(enc_key,0x11,DEF_ENC_KEY_LEN);
	//x_memset(enc_iv,0x22,DEF_ENC_IV_LEN);
	  int input_len;
	  char b64_response[DEF_B64_RESPONSE_LEN];
	  int b64_response_len;
	  unsigned char response[DEF_RESPONSE_LEN];
	//	scanf("%1023s",input);
	
	  input_len = x_strlen(input);
	  if( input_len > DEF_B64_RESPONSE_LEN ){
		  CSL_ERR_PRINT("b64_response_len(%d) > (%d) error.\n",input_len, DEF_B64_RESPONSE_LEN);
		  return -1;
	  }
	
	  x_memset( b64_response, 0x0, DEF_B64_RESPONSE_LEN );
	  x_memcpy( b64_response, input, input_len );
	  b64_response_len = input_len;

	  
	//  printf("b64_response : %s,len = %d \n ", b64_response,input_len);
	
	  ret = base64_decode( b64_response, b64_response_len, response, DEF_RESPONSE_LEN );
	  if( ret < 0 ){
		  CSL_ERR_PRINT("base64 decoding is error.\n");
		  return -1;
	  }
	
	  ret = VerifyResponse( random_number, response, enc_key, DEF_ENC_KEY_LEN, enc_iv, DEF_ENC_IV_LEN );
	  if( ret != 0 ){
		  CSL_ERR_PRINT("Challenge verification is error.\n");
		  return -1;
	  }
	
	  printf("\nConsole unlocks.\n");
	  return 0;
	
}
