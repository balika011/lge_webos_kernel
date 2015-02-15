#ifndef __IPTVES_CRYPTO_ECC_
#define __IPTVES_CRYPTO_ECC_


//#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#include "mtk_hdcpkeyenc_keysource.c"
#include <tomcrypt.h>
//#include <tomcrypt_test.h>

//#include <tfm.h>



/** Structure defines a NIST GF(p) curve */
typedef struct {
   /** The size of the curve in octets */
   int size;

   /** name of curve */
   char *name; 

   /** The prime that defines the field the curve is in (encoded in hex) */
   char *prime;

   /** The fields A param (hex) */
   char *A;

   /** The fields B param (hex) */
   char *B;

   /** The order of the curve (hex) */
   char *order;
  
   /** The x co-ordinate of the base point on the curve (hex) */
   char *Gx;
 
   /** The y co-ordinate of the base point on the curve (hex) */
   char *Gy;
  
} custom_ecc_set_type;



/** An ECC key */
typedef struct {
    /** Type of key, PK_PRIVATE or PK_PUBLIC */
    int type;

    /** Index into the ltc_ecc_sets[] for the parameters of this curve; if -1, then this key is using user supplied curve in dp */
    int idx;

	/** pointer to domain parameters; either points to NIST curves (identified by idx >= 0) or user supplied curve */
	const custom_ecc_set_type *dp;

    /** The public key */
    ecc_point pubkey;

    /** The private key */
    void *k;


} custom_ecc_key;



extern custom_ecc_set_type sony_ecc_para;



#define MP_DIGIT_BIT                 ltc_mp.bits_per_digit

/* some handy macros */
#define mp_init(a)                   ltc_mp.init(a)
#define mp_init_multi                ltc_init_multi
#define mp_clear(a)                  ltc_mp.deinit(a)
#define mp_clear_multi               ltc_deinit_multi
#define mp_init_copy(a, b)           ltc_mp.init_copy(a, b)

#define mp_neg(a, b)                 ltc_mp.neg(a, b)
#define mp_copy(a, b)                ltc_mp.copy(a, b)


#define mp_set(a, b)                 ltc_mp.set_int(a, b)
#define mp_set_int(a, b)             ltc_mp.set_int(a, b)
#define mp_get_int(a)                ltc_mp.get_int(a)
#define mp_get_digit(a, n)           ltc_mp.get_digit(a, n)
#define mp_get_digit_count(a)        ltc_mp.get_digit_count(a)
#define mp_cmp(a, b)                 ltc_mp.compare(a, b)
#define mp_cmp_d(a, b)               ltc_mp.compare_d(a, b)
#define mp_count_bits(a)             ltc_mp.count_bits(a)
#define mp_cnt_lsb(a)                ltc_mp.count_lsb_bits(a)
#define mp_2expt(a, b)               ltc_mp.twoexpt(a, b)

#define mp_read_radix(a, b, c)       ltc_mp.read_radix(a, b, c)
#define mp_toradix(a, b, c)          ltc_mp.write_radix(a, b, c)
#define mp_unsigned_bin_size(a)      ltc_mp.unsigned_size(a)
#define mp_to_unsigned_bin(a, b)     ltc_mp.unsigned_write(a, b)
#define mp_read_unsigned_bin(a, b, c) ltc_mp.unsigned_read(a, b, c)

#define mp_add(a, b, c)              ltc_mp.add(a, b, c)
#define mp_add_d(a, b, c)            ltc_mp.addi(a, b, c)
#define mp_sub(a, b, c)              ltc_mp.sub(a, b, c)
#define mp_sub_d(a, b, c)            ltc_mp.subi(a, b, c)
#define mp_mul(a, b, c)              ltc_mp.mul(a, b, c)
#define mp_mul_d(a, b, c)            ltc_mp.muli(a, b, c)
#define mp_sqr(a, b)                 ltc_mp.sqr(a, b)
#define mp_div(a, b, c, d)           ltc_mp.mpdiv(a, b, c, d)
#define mp_div_2(a, b)               ltc_mp.div_2(a, b)
#define mp_mod(a, b, c)              ltc_mp.mpdiv(a, b, NULL, c)
#define mp_mod_d(a, b, c)            ltc_mp.modi(a, b, c)
#define mp_gcd(a, b, c)              ltc_mp.gcd(a, b, c)
#define mp_lcm(a, b, c)              ltc_mp.lcm(a, b, c)

#define mp_mulmod(a, b, c, d)        ltc_mp.mulmod(a, b, c, d)
#define mp_sqrmod(a, b, c)           ltc_mp.sqrmod(a, b, c)
#define mp_invmod(a, b, c)           ltc_mp.invmod(a, b, c)

#define mp_montgomery_setup(a, b)    ltc_mp.montgomery_setup(a, b)
#define mp_montgomery_normalization(a, b) ltc_mp.montgomery_normalization(a, b)
#define mp_montgomery_reduce(a, b, c)   ltc_mp.montgomery_reduce(a, b, c)
#define mp_montgomery_free(a)        ltc_mp.montgomery_deinit(a)

#define mp_exptmod(a,b,c,d)          ltc_mp.exptmod(a,b,c,d)
#define mp_prime_is_prime(a, b, c)   ltc_mp.isprime(a, c)

#define mp_iszero(a)                 (mp_cmp_d(a, 0) == LTC_MP_EQ ? LTC_MP_YES : LTC_MP_NO)
#define mp_isodd(a)                  (mp_get_digit_count(a) > 0 ? (mp_get_digit(a, 0) & 1 ? LTC_MP_YES : LTC_MP_NO) : LTC_MP_NO)
#define mp_exch(a, b)                do { void *ABC__tmp = a; a = b; b = ABC__tmp; } while(0);

#define mp_tohex(a, b)               mp_toradix(a, b, 16)

/* added by mtk */
#define mp_addmod(a, b, c, d)              ltc_mp.addmod(a, b, c, d)
#define mp_submod(a, b, c, d)              ltc_mp.submod(a, b, c, d)

#define mp_draw(a)                   	   ltc_mp.draw_d(a)
#define mp_drawhex(a)                      ltc_mp.draw_hex(a)

#define SHA1_LEN   (20)
#define SHA256_LEN (32)
#define SHA_LEN (SHA256_LEN)

#define CRYPTO_AES_BLOCK_LEN            16
#define CRYPTO_AES_KEY_LEN              16

#define CRYPTO_ECDSA_PRIVKEY_LEN        28
#define CRYPTO_ECDSA_PUBKEY_LEN         56
#define CRYPTO_ECDSA_SIG_LEN            56

#define CRYPTO_ECDH_PRIVKEY_LEN         28
#define CRYPTO_ECDH_PUBKEY_LEN          56
#define CRYPTO_ECDH_PHASE1VALUE_LEN     56

#define CRYPTO_RANDOM_NUMBER_LEN        16

#define CRYPTO_SHA256_DIGEST_LEN        32


int ecc_verify_nist_curve_point(void *a, void *b, ecc_point *G, void *modulus);




//extern unsigned long yarrow_read(unsigned char *out, unsigned long outlen, prng_state *prng);

void dump_hex_name(char *name, unsigned char *buf, int len);

void dump_char_name(char *name, unsigned char *buf, int len);
//extern ltc_math_descriptor tfm_desc;



/*
	!!!!!   MAKE SURE NOT OVERFLOW
*/
#define MAX_POINT (1024)









/**
  Sign a message digest
  @param in        The message digest to sign
  @param inlen     The length of the digest
  @param out       [out] The destination for the signature
  @param outlen    [in/out] The max size and resulting size of the signature
  @param prng      An active PRNG state
  @param wprng     The index of the PRNG you wish to use
  @param key       A private ECC key
  @return CRYPT_OK if successful
*/
int ecc_sign_hash_general(const unsigned char *in,  unsigned long inlen, 
                        unsigned char *out, unsigned long *outlen, 
                        prng_state *prng, int wprng, custom_ecc_key *priv_key);







/* verify 
 *
 * w  = s^-1 mod n
 * u1 = xw 
 * u2 = rw
 * X = u1*G + u2*Q
 * v = X_x1 mod n
 * accept if v == r
 */

/**
   Verify an ECC signature
   @param sig         The signature to verify
   @param siglen      The length of the signature (octets)
   @param hash        The hash (message digest) that was signed
   @param hashlen     The length of the hash (octets)
   @param stat        Result of signature, 1==valid, 0==invalid
   @param key         The corresponding public ECC key
   @return CRYPT_OK if successful (even if the signature is not valid)
*/
int ecc_verify_hash_general(const unsigned char *sig,  unsigned long siglen,
                    const unsigned char *hash, unsigned long hashlen, 
                    int *stat, custom_ecc_key *key);

/**
  Sign a message digest
  @param in        The message digest to sign
  @param inlen     The length of the digest
  @param out       [out] The destination for the signature
  @param outlen    [in/out] The max size and resulting size of the signature
  @param prng      An active PRNG state
  @param wprng     The index of the PRNG you wish to use
  @param key       A private ECC key
  @return CRYPT_OK if successful
*/
int ecc_sign_hash_emsa1_sha256_ex(const unsigned char *in, 
                        unsigned char *out, unsigned int *outlen, 
                        prng_state *prng, int wprng, custom_ecc_key *priv_key);


/* verify 
 *
 * w  = s^-1 mod n
 * u1 = xw 
 * u2 = rw
 * X = u1*G + u2*Q
 * v = X_x1 mod n
 * accept if v == r
 */

/**
   Verify an ECC signature
   @param sig         The signature to verify
   @param siglen      The length of the signature (octets)
   @param hash        The hash (message digest) that was signed
   @param hashlen     The length of the hash (octets)
   @param stat        Result of signature, 1==valid, 0==invalid
   @param key         The corresponding public ECC key
   @return CRYPT_OK if successful (even if the signature is not valid)
*/
int ecc_verify_hash_emsa1_sha256_ex(const unsigned char *sig,  unsigned long siglen,
                    const unsigned char *hash, 
                    int *stat, custom_ecc_key *key);
/*
	ecc general tests based on sha256
*/
int ecc_general_tests (void);







int ecc_verify_hash_emsa1_sha256(	
			const unsigned char  		*in_public_key_xy,
			//int					 public_key_len,
			const unsigned char 		*msg, 
			int 				 msg_len,
			const unsigned char		*expected_sign,
			//int					 expected_sign_len,
			custom_ecc_set_type *ecc_set
		  );

int ecc_sign_hash_emsa1_sha256(	
			const unsigned char  		 *in_priv_key_xy,
			//int					 priv_key_len,
			const unsigned char 		*msg, 
			int 				 msg_len,
			unsigned char		*out_sign,
			unsigned int		 out_sign_len,
			custom_ecc_set_type *ecc_set,
			unsigned char       *export_pubkey
		  );




/*
	ecc general tests based on sha256
*/
int ecc_emsa_sha256_tests (void);


int ecc_dh(const unsigned char              in_svr_p1v[CRYPTO_ECDH_PHASE1VALUE_LEN],
            unsigned char                    out_client_p1v[CRYPTO_ECDH_PHASE1VALUE_LEN],
            unsigned char                    out_pubkey[CRYPTO_ECDH_PUBKEY_LEN]);


#endif
