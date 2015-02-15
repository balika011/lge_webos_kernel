#ifndef __DEF_CHALLENGE_RESPONSE_DATA_H__
#define __DEF_CHALLENGE_RESPONSE_DATA_H__

int CreateChallenge( const unsigned char *serial_no, const unsigned char *random_number, unsigned char *challenge,
                     const unsigned char *enc_key, int key_len, unsigned char *enc_iv, int iv_len );
int VerifyChallenge( const unsigned char *serial_no, const unsigned char *challenge, unsigned char *random_number,
                     const unsigned char *enc_key, int key_len, unsigned char *enc_iv, int iv_len );
int CreateResponse( const unsigned char *random_number, unsigned char *response,
                    const unsigned char *enc_key, int key_len, unsigned char *enc_iv, int iv_len );
int VerifyResponse( const unsigned char *random_number, unsigned char *response,
                    const unsigned char *enc_key, int key_len, unsigned char *enc_iv, int iv_len );

#endif /*__DEF_CHALLENGE_RESPONSE_DATA_H__*/
