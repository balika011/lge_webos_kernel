#include<stdio.h>
#include<string.h>

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


INT32 Verifier( unsigned char *serial_no,unsigned char *b64_challenge,unsigned char *b64_return)
{
    int ret;
  //  unsigned char *serial_no;
    unsigned char random_number[DEF_RANDOM_LEN];

    unsigned char challenge[DEF_CHALLENGE_LEN];
   // unsigned char *b64_challenge;
    int b64_challenge_len;
  // LDR_Loader_Sig* prLoaderVerify = (LDR_Loader_Sig*)(0x2d00000);

    unsigned char response[DEF_RESPONSE_LEN];
    unsigned char b64_response[DEF_B64_RESPONSE_LEN+1];

//    serial_no = argv[1];
    b64_challenge_len = x_strlen(b64_challenge);
    if( b64_challenge_len > DEF_B64_CHALLENGE_LEN ){
        CSL_ERR_PRINT("Challenge len is too big %d < %d\n",DEF_CHALLENGE_LEN, b64_challenge_len);
        return -1;
    }
  //  b64_challenge = argv[2];

    //printf("serial_no=%s b64_challenge=%s len=%d\n",serial_no, b64_challenge, b64_challenge_len );

    unsigned char enc_key[DEF_ENC_KEY_LEN];
    unsigned char enc_iv[DEF_ENC_IV_LEN];
	
	x_memset(enc_key,0x55,DEF_ENC_KEY_LEN);
	x_memset(enc_iv,0xAA,DEF_ENC_IV_LEN);
//	x_memcpy(enc_key,prLoaderVerify->u4AesKey2,DEF_ENC_KEY_LEN);
//	x_memcpy(enc_iv,prLoaderVerify->u4InitialVecter,DEF_ENC_IV_LEN);


    //Verify Challenge
    ret = base64_decode( b64_challenge, b64_challenge_len, challenge, DEF_CHALLENGE_LEN );
    if( ret < 0 ){
        CSL_ERR_PRINT("base64 decoding is error.\n");
        return -1;
    }

    ret = VerifyChallenge( serial_no, challenge, random_number, enc_key, DEF_ENC_KEY_LEN, enc_iv, DEF_ENC_IV_LEN );
    if( ret != 0 ){
        CSL_ERR_PRINT("Challenge verification is error.\n");
        //return -1;
    }

    //Create Response
    ret = CreateResponse( random_number, response, enc_key, DEF_ENC_KEY_LEN, enc_iv, DEF_ENC_IV_LEN );
    if( ret != 0 ){
        CSL_ERR_PRINT("Create Response is error.\n");
        return -1;
    }

    ret = base64_encode( response, DEF_RESPONSE_LEN, b64_response, DEF_B64_RESPONSE_LEN );
    if( ret < 0 ){
        CSL_ERR_PRINT("base64_encode error ret=%d\n",ret);
         return -1;
    }
    b64_response[ret] = '\0';
	x_memcpy(b64_return,b64_response,sizeof(b64_response));
    CSL_PRINT("\n Response = %s\n",b64_response);

    return 0;
}

