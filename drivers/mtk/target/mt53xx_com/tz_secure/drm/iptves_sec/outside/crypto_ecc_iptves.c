//#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#include "mtk_hdcpkeyenc_keysource.c"
//#include <tomcrypt.h>
//#include <tomcrypt_misc.h>
//#include <tfm.h>

#define LTC_SOURCE
#include "crypto_ecc_iptves.h"

#define ECC_DEBUG	1


prng_state g_prng;



void run_cmd(int res, int line, char *file, char *cmd);

//#define DO(x) do { fprintf(stderr, "%s:\n", #x); run_cmd((x), __LINE__, __FILE__, #x); } while (0);
#define DO(x) do {  run_cmd((x), __LINE__, __FILE__, #x); } while (0);



//extern unsigned long yarrow_read(unsigned char *out, unsigned long outlen, prng_state *prng);

void dump_hex(unsigned char *buf, int len);
void draw_mp(char *name, void *a);
void draw_point(char *name, ecc_point *p);
int ltc_ecc_mul2add_ntd(ecc_point *A, void *kA,
                    ecc_point *B, void *kB,
                    ecc_point *C,
                         void *modulus,
                         void *a);

int ltc_ecc_mulmod_ntd(void *k, ecc_point *G, ecc_point *R, void *modulus, void *a);
int ecc_make_test_point(int _mod, int _a, int _b, int gx[], int gy[]);
int ecc_verify_custom_curve_point(void *a, void *b, ecc_point *G, void *modulus);
void ecc_my_free(custom_ecc_key *key);
int ecc_my_test(void);
int ecc_make_keypub(const unsigned char   in_pubkey[CRYPTO_ECDSA_PUBKEY_LEN], int keysize, custom_ecc_key *key, custom_ecc_set_type *dp);
int ecc_make_keypriv(const unsigned char   in_privkey[CRYPTO_ECDSA_PRIVKEY_LEN], custom_ecc_key *key, custom_ecc_set_type *dp);
int ecc_make_key_ex_general(prng_state *prng, int wprng, custom_ecc_key *key, const  custom_ecc_set_type *dp);
int ecc_make_key_general(prng_state *prng, int wprng, custom_ecc_key *key, custom_ecc_set_type *dp);


void dump_hex(unsigned char *buf, int len)
{
    int idx = 0;
    
    while (idx < len)
    {
        if (idx%28 == 0)
        {
            if (idx != 0)
                dprintf("\n");
            dprintf("0x%04x| ", idx);
        }
        dprintf("%02x", buf[idx]);
        idx++;
    }
    dprintf("\n");
}



void draw_mp(char *name, void *a)
{
#if ECC_DEBUG
	unsigned char temp[1024] = {0};
	int size = 0;
	if(NULL == a)
	{
		return;
	}

	if(NULL != name)
	{
	    dprintf("%s:\n", name);	
	}

	memset(temp, 0, 1024);
	mp_to_unsigned_bin(a, temp);
	size = mp_unsigned_bin_size(a);
	dump_hex(temp, size);

	dprintf("\n");

#endif

}


void draw_point(char *name, ecc_point *p)
{

#if ECC_DEBUG
	if(name == NULL || NULL == p)
	{
		return;
	}

	dprintf("PT %s:\n", name);

	draw_mp(NULL, p->x);
	draw_mp(NULL, p->y);
	draw_mp(NULL, p->z);

	dprintf("\n");
#endif

}




void dump_hex_name(char *name, unsigned char *buf, int len)
{
    int idx = 0;
    
    dprintf("%s:\n--------------------------------\n", name);
    while (idx < len)
    {
        if (idx%28 == 0)
        {
            if (idx != 0)
                dprintf("\n");
            dprintf("0x%04x| ", idx);
        }
        dprintf("%02x", buf[idx]);
        idx++;
    }
    dprintf("\n");
}

void dump_char_name(char *name, unsigned char *buf, int len)
{
    int idx = 0;
    
    dprintf("(%s):\n--------------------------------\n", name);
    while (idx < len)
    {

        dprintf("%c", buf[idx]);
        idx++;
    }
    dprintf("\n--------------------------------\n");
}





void run_cmd(int res, int line, char *file, char *cmd)
{
   if (res != CRYPT_OK) {
      dprintf("%s (%d)\n%s:%d:%s\n", error_to_string(res), res, file, line, cmd);
      if (res != CRYPT_NOP) {
         return -1;
      }
   }
}








static int tfm_ecc_projective_dbl_ntd(ecc_point *P, ecc_point *R, void *modulus,  void *a)
{
	void	*t1, *t2, *x, *y, *z, *k2;;


	int err = -1;
	
	
	
	void *x1,  *x2, *y1,  *k;
	void *x3, *y3;

	/* init */
	err = mp_init_multi(&t1, &t2, &x, &y, &z, &k2, &x1, &x2, &y1, &k, &x3, &y3,  NULL);
	if(err != CRYPT_OK)
	{
		dprintf(">>>%s: err0\n", __FUNCTION__);
		return -1;
	}

	


	




	//dprintf(">>%s\n", __FUNCTION__);
	//mp_drawhex(P->x);
	//mp_drawhex(P->y);	
	//mp_drawhex(P->z);	

	

	/* TO-DO: add fix for infinite point */
	/* P = INF ? */
	if((mp_cmp_d(P->x, 0) == LTC_MP_EQ) &&
	(mp_cmp_d(P->y, 1) == LTC_MP_EQ) &&
	(mp_cmp_d(P->z, 0) == LTC_MP_EQ))
	{
		 mp_copy(P->x, R->x);
		 mp_copy(P->y, R->y);
		 mp_copy(P->z, R->z);
	
		 //dprintf("<<%s P = INF\n", __FUNCTION__);	 
		 
		 return CRYPT_OK;
	
	}



	

	if(mp_cmp_d(P->y, 0) == LTC_MP_EQ)
	{


	
		dprintf("dbl_ntd invalid parameters\n");

		dprintf("x:\n");
		mp_drawhex(P->x);
		
		dprintf("y:\n");
		mp_drawhex(P->y);
		
		dprintf("z:\n");
		mp_drawhex(P->z);


		
		//dprintf("<<%s\n", __FUNCTION__);		
		return -1;
	}

	
	
	LTC_ARGCHK(P	   != NULL);
	LTC_ARGCHK(R	   != NULL);
	LTC_ARGCHK(modulus != NULL);

	

	
	 /* init P, Q into (x1, y1)  (x2, y2)*/
	 mp_copy(P->x, x1);
	 mp_copy(P->y, y1);
	 mp_copy(P->x, x2);

	


	
	 /* k = (3*x^2 +a)/2y1 mod p*/

	 /*t1 = x*x */
	 mp_set(t1, 0);
	 mp_sqr(x1, t1);

	// dprintf("\t(x1 ^2)     :");
	// mp_drawhex(&t1);



	 /*t1 = 3 *t1 */
	 mp_mul_d(t1, 3, t1);

	 //dprintf("\t(3* x1 ^2)     :");
	 //mp_drawhex(&t1);



	 /* t1 = t1 +a */

	 mp_add(t1, a, t1);

	 //dprintf("\t(3* x1 ^2)  + a :");
	 //mp_drawhex(&t1);


	 /* t2 = 2*y1 */
	 mp_mul_d(y1, 2, t2);

	 //dprintf("\t 2y1:     :");
	 //mp_drawhex(&t2);


	 /* t2 = 1/t2 */
	 mp_invmod(t2, modulus, t2); 
	 //dprintf("\t(inv 2y1)     :");
	 //mp_drawhex(&t2);


	 /* k = (3*x^2 +a)/2y1 = t1*t2 */
	 mp_mulmod(t1, t2, modulus, k);
	 

	 //dprintf("\tk      :");
	 //
	 //mp_drawhex(&k);
	
	 /*k2 = k*k*/
	 mp_set(k2, 0);
	 mp_sqr(k, k2);
	 
     //dprintf("k2:\n");
	 //mp_drawhex(&k2);

     //dprintf("x1:\n");
	 //mp_drawhex(&x1);


	
	 /*k2 = k2 -x1*/
	 mp_sub(k2, x1, k2);

     //dprintf("k2 -x1:\n");
	 //
	 //mp_drawhex(&k2);
	
	
	 mp_set(x3, 0);
	 /*x3 = k2 -x2*/
	 mp_submod(k2, x2, modulus, x3);

	

	 
	 //dprintf("\n\t dbl: x3     :");
	 //mp_drawhex(&x3);
	
	
	 //dprintf("k:\n");
	 //mp_drawhex(&k);
	
	 /* t1 = x1 -x3   */
	 mp_submod(x1, x3, modulus, t1);
	
	 /* t1 = t1 *k	   */
	 mp_mulmod(t1, k, modulus, t1);
	
	 /* t1 = t1 - y1*/
	 mp_submod(t1, y1, modulus, y3);
	 //dprintf("\t dbl: y3     :");
	 //mp_drawhex(&y3);
	
	mp_copy(x3, R->x);
	mp_copy(y3, R->y);
	mp_set(R->z, 1);

	
	//dprintf("<<%s\n", __FUNCTION__);

	mp_clear_multi(t1, t2, x, y, z, k2, x1, x2, y1, k, x3, y3,  NULL);

  
	
   return CRYPT_OK;
}


/*
	
	define infinite point INF(0, 1, 0)
	
*/


/**
   Add two ECC points
   @param P        The point to add
   @param Q        The point to add
   @param R        [out] The destination of the double
   @param modulus  The modulus of the field the ECC curve is in
   @param mp       The "b" value from montgomery_setup()
   @return CRYPT_OK on success
*/
static int tfm_ecc_projective_add_point_ntd(ecc_point *P, ecc_point *Q, ecc_point *R, void *modulus,  void *a)
{

   int err = -1;
  
   void  *t1, *t2, *x, *y, *z;
   void *x1, *x2, *y1, *y2, *x2_x1, *y2_y1, *k, *k2;
   void *x3, *y3;
   void *tmp1, *tmp2;



   //dprintf("\n\n\n>>%s\n", __FUNCTION__);
	
   LTC_ARGCHK(P       != NULL);
   LTC_ARGCHK(Q       != NULL);
   LTC_ARGCHK(R       != NULL);
   LTC_ARGCHK(modulus != NULL);

   //dprintf("ntd add: \n");


   /* P = INF ? */
   if((mp_cmp_d(P->x, 0) == LTC_MP_EQ) &&
   (mp_cmp_d(P->y, 1) == LTC_MP_EQ) &&
   (mp_cmp_d(P->z, 0) == LTC_MP_EQ))
   {
		mp_copy(Q->x, R->x);
		mp_copy(Q->y, R->y);
		mp_copy(Q->z, R->z);

		dprintf("<<%s P = INF\n", __FUNCTION__);		
		
		return CRYPT_OK;

   }

   

   /* Q == INF ? */
   if((mp_cmp_d(Q->x, 0) == LTC_MP_EQ) &&
   (mp_cmp_d(Q->y, 1) == LTC_MP_EQ) &&
   (mp_cmp_d(Q->z, 0) == LTC_MP_EQ))
   {
	   mp_copy(P->x, R->x);
	   mp_copy(P->y, R->y);
	   mp_copy(P->z, R->z);

	   dprintf("<<%s Q = INF\n", __FUNCTION__);	
	  
	   return CRYPT_OK;

   }




   if ( (mp_cmp(P->x, Q->x) == LTC_MP_EQ) && 
		   (Q->z != NULL && mp_cmp(P->z, Q->z) == LTC_MP_EQ) &&
		   (mp_cmp(P->y, Q->y) == LTC_MP_EQ ))


   {

		//dprintf(">>>add use dbl\n");
        tfm_ecc_projective_dbl_ntd(P, R, modulus,  a);
		
		//draw_point("R", R);
		
		return CRYPT_OK;
   }


	/* init */

	err = mp_init_multi(&t1, &t2, &x, &y, &z, &x1, &x2,  &y1, &y2, &x2_x1, &y2_y1,
						 &k, &k2, &tmp1, &tmp2, &x3, &y3, NULL);
	if(err != CRYPT_OK)
	{
		dprintf(">>>%s: err0\n", __FUNCTION__);
		return -1;
	}




	/* init P, Q into (x1, y1)  (x2, y2)*/
	mp_copy(P->x, x1);
	mp_copy(P->y, y1);

    mp_copy(Q->x, x2);
    mp_copy(Q->y, y2);
	




	/* k = (y2-y1)/(x2-x1) mod p*/
	mp_sub(y2, y1, y2_y1);
	mp_sub(x2, x1, x2_x1);


	//x2 smaller than x1 cuased LTM bug !!!!!!!!!
	if(mp_cmp_d(x2_x1, 0) == LTC_MP_LT)
	{
		/* 
			x2_x1 = 0 - x2_x1 
			y2_y1 = 0 - y2_y1
		*/
		mp_set(k, 0);
		mp_sub(k, x2_x1, x2_x1);
		mp_sub(k, y2_y1, y2_y1);
	
	}
	

	if(mp_cmp_d(x2_x1, 0) == LTC_MP_EQ)
	{
		draw_point("P", P);
		draw_point("Q", Q);


		mp_addmod(y2, y1, modulus, k);
		if(mp_cmp_d(k, 0) == LTC_MP_EQ)		/* should be zero */
		{
			dprintf(">> ## %s:add_point: Q + (-Q)\n", __FUNCTION__);

			mp_set(R->x, 0);
			mp_set(R->y, 1);
			mp_set(R->z, 0);
			
			return CRYPT_OK;
		}
		else
		{
			dprintf(">>add_point: invalid parameters \n\n\n");
						
			draw_mp("x1", x1);
			draw_mp("y1", y1);
			draw_mp("z1", P->z);
					
			draw_mp("x2", x2);
			draw_mp("y2", y2);
			draw_mp("y3", Q->z);
			
			
			return CRYPT_ERROR;

		}
	
	}



	/* x3: = k*k -x1 -x2 (mod p)*/

	/* x2_x1 = 1/(x2_x1) mod p*/

	mp_invmod(x2_x1, modulus, x2_x1);
	//draw_mp("x2_x1 ^ 1", x2_x1);


	/* k: */
	mp_set(k, 0);
//	mp_mulmod(y2_y1, x2_x1, modulus, k);
	mp_mul(y2_y1, x2_x1, k);
	//draw_mp("k", k);
	
	mp_mod(k, modulus, k);
	//draw_mp("k", k);



	/*k2 = k*k*/
	mp_set(k2, 0);
	mp_sqrmod(k, modulus, k2);
	//draw_mp("k2", k2);



	/*k2 = k2 -x1*/
	//draw_mp("x1", x1);
	mp_submod(k2, x1, modulus,  k2);
	//draw_mp("k2 - x1", k2);




	/*x3 = k2 -x2*/
	mp_submod(k2, x2, modulus, x3);
	//draw_mp("k2 - x2 = x3", x3);



	/* y3: = k(x1-x3) - y1 (mod p)*/

	mp_set(t1, 0);			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! my god....
	/* t1 = x1 -x3   */
	mp_submod(x1, x3, modulus, t1);
	//draw_mp("x1 - x3 = t1", t1);

	/* t1 = t1 *k     */
	mp_mulmod(t1, k, modulus, t1);
	//draw_mp("t1 * k = t1", t1);


	/* incase k1 too small*/
//	k1 + mod + mod
	mp_add(t1, modulus, t1);
	mp_add(t1, modulus, t1);
	//draw_mp("t1", t1);




	/* t1 = t1 - y1*/
	mp_submod(t1, y1, modulus, y3);
	//draw_mp("y3", y3);


    mp_copy(x3, R->x);
    mp_copy(y3, R->y);
    mp_set(R->z, 1);


 	//dprintf("\n\n\n\n");

	   

   mp_clear_multi(t1, t2, x, y, z, x1, x2,  y1, y2, x2_x1, y2_y1,
							k, k2, tmp1, tmp2, x3, y3, NULL);

   
   return CRYPT_OK;
}









/** Computes kA*A + kB*B = C using Shamir's Trick
  @param A        First point to multiply
  @param kA       What to multiple A by
  @param B        Second point to multiply
  @param kB       What to multiple B by
  @param C        [out] Destination point (can overlap with A or B
  @param modulus  Modulus for curve 
  @return CRYPT_OK on success
*/ 
int ltc_ecc_mul2add_ntd(ecc_point *A, void *kA,
                    ecc_point *B, void *kB,
                    ecc_point *C,
                         void *modulus,
                         void *a)
{
  ecc_point     *precomp[16];
  unsigned       bitbufA, bitbufB, lenA, lenB, len, x, y, nA, nB, nibble;
  unsigned char *tA, *tB;
  int            err, first;
  void          *mu;
 
  /* argchks */
  LTC_ARGCHK(A       != NULL);
  LTC_ARGCHK(B       != NULL);
  LTC_ARGCHK(C       != NULL);
  LTC_ARGCHK(kA      != NULL);
  LTC_ARGCHK(kB      != NULL);
  LTC_ARGCHK(modulus != NULL);

  /* allocate memory */
  tA = XCALLOC(1, ECC_BUF_SIZE);
  if (tA == NULL) {
     return CRYPT_MEM;
  }
  tB = XCALLOC(1, ECC_BUF_SIZE);
  if (tB == NULL) {
     XFREE(tA);
     return CRYPT_MEM;
  }

  /* get sizes */
  lenA = mp_unsigned_bin_size(kA);
  lenB = mp_unsigned_bin_size(kB);
  len  = MAX(lenA, lenB);

  /* sanity check */
  if ((lenA > ECC_BUF_SIZE) || (lenB > ECC_BUF_SIZE)) {
     err = CRYPT_INVALID_ARG;
     goto ERR_T;
  }

 


//  if(A == B)
  if(LTC_MP_EQ == mp_cmp(A->x, B->x) &&
  	LTC_MP_EQ == mp_cmp(A->y, B->y) &&
  	LTC_MP_EQ == mp_cmp(A->z, B->z) )
  {
	/* use mul */
	mp_add(kA, kB, kA);

	dprintf("@@@@ use mul mode\n");
	mp_drawhex(kA);
	return ltc_ecc_mulmod_ntd(kA, A, C, modulus,  a);

	 
  }

  

  /* extract and justify kA */
  mp_to_unsigned_bin(kA, (len - lenA) + tA);

  /* extract and justify kB */
  mp_to_unsigned_bin(kB, (len - lenB) + tB);





  //dprintf("len:(%x) len A(%x) len B(%x)\n", len, lenA, lenB);

  
  //dprintf("Ka:\n");
 // mp_drawhex(kA);


  
  //
  //dprintf("Kb:\n");
 // mp_drawhex(kB);



  /* allocate the table */
  for (x = 0; x < 16; x++) {
     precomp[x] = ltc_ecc_new_point();
     if (precomp[x] == NULL) {
         for (y = 0; y < x; ++y) {
            ltc_ecc_del_point(precomp[y]);
         }
         err = CRYPT_MEM;
         goto ERR_T;
     }
  }

   /* init montgomery reduction */
   //if ((err = mp_montgomery_setup(modulus, &mp)) != CRYPT_OK) {
   //   goto ERR_P;
   //}
   if ((err = mp_init(&mu)) != CRYPT_OK) {
      goto ERR_MP;
   }
   //if ((err = mp_montgomery_normalization(mu, modulus)) != CRYPT_OK) {
   //   goto ERR_MU;
   //}

  /* copy ones ... */
#if 0  
  if ((err = mp_mulmod(A->x, mu, modulus, precomp[1]->x)) != CRYPT_OK)                                         { goto ERR_MU; }
  if ((err = mp_mulmod(A->y, mu, modulus, precomp[1]->y)) != CRYPT_OK)                                         { goto ERR_MU; }
  if ((err = mp_mulmod(A->z, mu, modulus, precomp[1]->z)) != CRYPT_OK)                                         { goto ERR_MU; }

  if ((err = mp_mulmod(B->x, mu, modulus, precomp[1<<2]->x)) != CRYPT_OK)                                      { goto ERR_MU; }
  if ((err = mp_mulmod(B->y, mu, modulus, precomp[1<<2]->y)) != CRYPT_OK)                                      { goto ERR_MU; }
  if ((err = mp_mulmod(B->z, mu, modulus, precomp[1<<2]->z)) != CRYPT_OK)      
#endif
  if ((err = mp_copy(A->x, precomp[1]->x)) != CRYPT_OK)                                         { goto ERR_MU; }
  if ((err = mp_copy(A->y, precomp[1]->y)) != CRYPT_OK)                                         { goto ERR_MU; }
  if ((err = mp_copy(A->z, precomp[1]->z)) != CRYPT_OK)                                         { goto ERR_MU; }

  if ((err = mp_copy(B->x,  precomp[1<<2]->x)) != CRYPT_OK)                                      { goto ERR_MU; }
  if ((err = mp_copy(B->y,  precomp[1<<2]->y)) != CRYPT_OK)                                      { goto ERR_MU; }
  if ((err = mp_copy(B->z,  precomp[1<<2]->z)) != CRYPT_OK)   



  { goto ERR_MU; }

  /* precomp [i,0](A + B) table */
  if ((err = tfm_ecc_projective_dbl_ntd(precomp[1], precomp[2],  modulus, a)) != CRYPT_OK)                               { goto ERR_MU; }
  if ((err = tfm_ecc_projective_add_point_ntd(precomp[1], precomp[2], precomp[3],  modulus, a)) != CRYPT_OK)                   { goto ERR_MU; }

  /* precomp [0,i](A + B) table */
  if ((err = tfm_ecc_projective_dbl_ntd(precomp[1<<2], precomp[2<<2],  modulus, a)) != CRYPT_OK)                         { goto ERR_MU; }
  if ((err = tfm_ecc_projective_add_point_ntd(precomp[1<<2], precomp[2<<2], precomp[3<<2],  modulus, a)) != CRYPT_OK)          { goto ERR_MU; }

  /* precomp [i,j](A + B) table (i != 0, j != 0) */
  for (x = 1; x < 4; x++) {
     for (y = 1; y < 4; y++) {
        if ((err = tfm_ecc_projective_add_point_ntd(precomp[x], precomp[(y<<2)], precomp[x+(y<<2)],  modulus, a)) != CRYPT_OK) { goto ERR_MU; }
     }
  }   

  nibble  = 3;
  first   = 1;
  bitbufA = tA[0];
  bitbufB = tB[0];

  /* for every byte of the multiplicands */
  for (x = -1;; ) {
     /* grab a nibble */
     if (++nibble == 4) {
        ++x; if (x == len) break;
        bitbufA = tA[x];
        bitbufB = tB[x];
        nibble  = 0;
     }

     /* extract two bits from both, shift/update */
     nA = (bitbufA >> 6) & 0x03;
     nB = (bitbufB >> 6) & 0x03;
     bitbufA = (bitbufA << 2) & 0xFF;   
     bitbufB = (bitbufB << 2) & 0xFF;   

     /* if both zero, if first, continue */
     if ((nA == 0) && (nB == 0) && (first == 1)) {
        continue;
     }

     /* double twice, only if this isn't the first */
     if (first == 0) {
        /* double twice */
        if ((err = tfm_ecc_projective_dbl_ntd(C, C,  modulus, a)) != CRYPT_OK)                  { goto ERR_MU; }
        if ((err = tfm_ecc_projective_dbl_ntd(C, C,  modulus, a)) != CRYPT_OK)                  { goto ERR_MU; }
     }

     /* if not both zero */
     if ((nA != 0) || (nB != 0)) {
        if (first == 1) {
           /* if first, copy from table */
           first = 0;
           if ((err = mp_copy(precomp[nA + (nB<<2)]->x, C->x)) != CRYPT_OK)           { goto ERR_MU; }
           if ((err = mp_copy(precomp[nA + (nB<<2)]->y, C->y)) != CRYPT_OK)           { goto ERR_MU; }
           if ((err = mp_copy(precomp[nA + (nB<<2)]->z, C->z)) != CRYPT_OK)           { goto ERR_MU; }
        } else {
           /* if not first, add from table */
           if ((err = tfm_ecc_projective_add_point_ntd(C, precomp[nA + (nB<<2)], C,  modulus, a)) != CRYPT_OK) { goto ERR_MU; }
        }
     }
  }

  /* reduce to affine */
  //err = ltc_ecc_map(C, modulus, mp);
  err = CRYPT_OK;

  /* clean up */
ERR_MU:
   mp_clear(mu);
ERR_MP:
   //mp_montgomery_free(mp);
//ERR_P:
   for (x = 0; x < 16; x++) {
       ltc_ecc_del_point(precomp[x]);
   }
ERR_T:
#ifdef LTC_CLEAN_STACK
   zeromem(tA, ECC_BUF_SIZE);
   zeromem(tB, ECC_BUF_SIZE);
#endif
   XFREE(tA);
   XFREE(tB);

   return err;
}





/**
   Perform a point multiplication 
   @param k    The scalar to multiply by
   @param G    The base point
   @param R    [out] Destination for kG
   @param modulus  The modulus of the field the ECC curve is in
   @param map      Boolean whether to map back to affine or not (1==map, 0 == leave in projective)
   @return CRYPT_OK on success
*/
#define WINSIZE 4

int ltc_ecc_mulmod_ntd(void *k, ecc_point *G, ecc_point *R, void *modulus,  void *a)
{
   ecc_point *tG, *M[8];
   int        i, j, err;
   void        *mu;
   unsigned long buf;
   int        first, bitbuf, bitcpy, bitcnt, mode, digidx;

   int   k_is_odd = -1;

   LTC_ARGCHK(k       != NULL);
   LTC_ARGCHK(G       != NULL);
   LTC_ARGCHK(R       != NULL);
   LTC_ARGCHK(modulus != NULL);


	//dprintf(">>%s:", __FUNCTION__);

	//draw_mp("modulus", modulus);
	//dprintf("map[%d]\n", map);
	//draw_mp("a", a);


	/* G == infinite ?*/
	if( LTC_MP_EQ == mp_cmp_d(G->x, 0) &&
		LTC_MP_EQ == mp_cmp_d(G->y, 1) &&
		LTC_MP_EQ == mp_cmp_d(G->z, 0)  )
	{
		/* 0P = infinite */
		dprintf("\nG = infinite:\n");
		mp_set(R->x, 0);
		mp_set(R->y, 1);	
		mp_set(R->z, 0);

		return CRYPT_OK;
	}
	




	if(LTC_MP_EQ == mp_cmp_d(k, 0))
	{
		/* 0P = infinite */
		dprintf("\n0P = infinite:\n");
		mp_set(R->x, 0);
		mp_set(R->y, 1);	
		mp_set(R->z, 0);

		return CRYPT_OK;
	}
	




	if(LTC_MP_EQ == mp_cmp_d(k, 1))
	{
		mp_copy(G->x, R->x);
		mp_copy(G->y, R->y);	
		mp_copy(G->z, R->z);

		return CRYPT_OK;
	}
	

	if(LTC_MP_YES == mp_isodd(k))
	{
		 //dprintf("########%s k is odd\n", __FUNCTION__);

		 k_is_odd = 1;

		 //draw_mp("k0", k);

		 mp_sub_d(k, 1, k);
		 
		 //draw_mp("k1", k);	 
	}







   /* init montgomery reduction */
   //if ((err = mp_montgomery_setup(modulus, &mp)) != CRYPT_OK) {
   //   return err;
   //}
   if ((err = mp_init(&mu)) != CRYPT_OK) {
      //mp_montgomery_free(mp);
      return err;
   }
   //if ((err = mp_montgomery_normalization(mu, modulus)) != CRYPT_OK) {
   //   mp_montgomery_free(mp);
   //   mp_clear(mu);
   //   return err;
   //}
  
  /* alloc ram for window temps */
  for (i = 0; i < 8; i++) {
      M[i] = ltc_ecc_new_point();
      if (M[i] == NULL) {
         for (j = 0; j < i; j++) {
             ltc_ecc_del_point(M[j]);
         }
         //mp_montgomery_free(mp);
         mp_clear(mu);
         return CRYPT_MEM;
      }
  }

   /* make a copy of G incase R==G */
   tG = ltc_ecc_new_point();
   if (tG == NULL)                                                                   { err = CRYPT_MEM; goto done; }

   //draw_mp("mu", mu);
   //draw_point("G", G);


   /* tG = G  and convert to montgomery */
   //if (mp_cmp_d(mu, 1) == LTC_MP_EQ) {
   	  //dprintf("convert to montgomery:\n");
      if ((err = mp_copy(G->x, tG->x)) != CRYPT_OK)                                  { goto done; }
      if ((err = mp_copy(G->y, tG->y)) != CRYPT_OK)                                  { goto done; }
      if ((err = mp_copy(G->z, tG->z)) != CRYPT_OK)                                  { goto done; }
   //}
   //else {   
   //
   //   if ((err = mp_mulmod(G->x, mu, modulus, tG->x)) != CRYPT_OK)                   { goto done; }
   //   if ((err = mp_mulmod(G->y, mu, modulus, tG->y)) != CRYPT_OK)                   { goto done; }
   //   if ((err = mp_mulmod(G->z, mu, modulus, tG->z)) != CRYPT_OK)                   { goto done; }
   //}
   mp_clear(mu);
   mu = NULL;

   //draw_point("tG", tG);




   
   /* calc the M tab, which holds kG for k==8..15 */
   /* M[0] == 8G */
   if ((err = tfm_ecc_projective_dbl_ntd(tG, M[0],  modulus, a)) != CRYPT_OK)                 { goto done; }
   if ((err = tfm_ecc_projective_dbl_ntd(M[0], M[0],  modulus, a)) != CRYPT_OK)               { goto done; }
   if ((err = tfm_ecc_projective_dbl_ntd(M[0], M[0],  modulus, a)) != CRYPT_OK)               { goto done; }

  // draw_point("M[0]", M[0]);



   /* now find (8+k)G for k=1..7 */
   for (j = 9; j < 16; j++) {
       if ((err = tfm_ecc_projective_add_point_ntd(M[j-9], tG, M[j-8],  modulus, a)) != CRYPT_OK)   { goto done; }


	   //dprintf("%s M[%d] 8G:   \n",  __FUNCTION__, j-8);
	   //draw_point(" ", M[j-8]);

	   //while(1);			//fix

   }

   //dprintf("/* now find (8+k)G for k=1..7 */\n");




   /* setup sliding window */
   mode   = 0;
   bitcnt = 1;
   buf    = 0;
   digidx = mp_get_digit_count(k) - 1;


   
   //dprintf("\t##--->k: ");    
   //draw(" ",k);


   //dprintf("\t##--->digidx: %x\n", digidx);
   //dprintf("\t##--->bitcnt: %d\n", bitcnt);

   
   bitcpy = bitbuf = 0;
   first  = 1;

   /* perform ops */
   for (;;) {

	 //dprintf("\t@@--->digidx: %d\n", digidx);


	
     /* grab next digit as required */
	if (--bitcnt == 0)
	{

		if (digidx == -1) 
		{
			//dprintf("\t@@--->break: %d\n", digidx);
			break;
		}

		buf    = mp_get_digit(k, digidx);

		//dprintf("\t@@--->buf: %d\n", buf);

		bitcnt = (int) ltc_mp.bits_per_digit;

		//dprintf("\t@@--->bitcnt: %d\n", bitcnt);		
		--digidx;
	}
   
	 
	

     /* grab the next msb from the ltiplicand */
     i = (buf >> (ltc_mp.bits_per_digit - 1)) & 1;
     buf <<= 1;

     /* skip leading zero bits */
     if (mode == 0 && i == 0) {
        continue;
     }

     /* if the bit is zero and mode == 1 then we double */
     if (mode == 1 && i == 0) {
        if ((err = tfm_ecc_projective_dbl_ntd(R, R,  modulus, a)) != CRYPT_OK)                 { goto done; }
        continue;
     }

     /* else we add it to the window */
     bitbuf |= (i << (WINSIZE - ++bitcpy));
     mode = 2;

	 //dprintf("\t@@@-->bitbuf: %d\n", bitbuf);


     if (bitcpy == WINSIZE) {
       /* if this is the first window we do a simple copy */
       if (first == 1) {
          /* R = kG [k = first window] */
          if ((err = mp_copy(M[bitbuf-8]->x, R->x)) != CRYPT_OK)                     { goto done; }
          if ((err = mp_copy(M[bitbuf-8]->y, R->y)) != CRYPT_OK)                     { goto done; }
          if ((err = mp_copy(M[bitbuf-8]->z, R->z)) != CRYPT_OK)                     { goto done; }
          first = 0;
       } else {
         /* normal window */
         /* ok window is filled so double as required and add  */
         /* double first */
         for (j = 0; j < WINSIZE; j++) {
           if ((err = tfm_ecc_projective_dbl_ntd(R, R,  modulus, a)) != CRYPT_OK)              { goto done; }
         }

         /* then add, bitbuf will be 8..15 [8..2^WINSIZE] guaranteed */
         if ((err = tfm_ecc_projective_add_point_ntd(R, M[bitbuf-8], R,  modulus, a)) != CRYPT_OK)   { goto done; }
       }
       /* empty window and reset */
       bitcpy = bitbuf = 0;
       mode = 1;
    }
  }

   /* if bits remain then double/add */
   if (mode == 2 && bitcpy > 0) {
     /* double then add */
     for (j = 0; j < bitcpy; j++) {
       /* only double if we have had at least one add first */
       if (first == 0) {

          if ((err = tfm_ecc_projective_dbl_ntd(R, R,  modulus, a)) != CRYPT_OK)              { goto done; }


		  
       }

       bitbuf <<= 1;
       if ((bitbuf & (1 << WINSIZE)) != 0) {
         if (first == 1){
            /* first add, so copy */
            if ((err = mp_copy(tG->x, R->x)) != CRYPT_OK)                           { goto done; }
            if ((err = mp_copy(tG->y, R->y)) != CRYPT_OK)                           { goto done; }
            if ((err = mp_copy(tG->z, R->z)) != CRYPT_OK)                           { goto done; }
            first = 0;
         } else {
            /* then add */

            if ((err = tfm_ecc_projective_add_point_ntd(R, tG, R,  modulus, a)) != CRYPT_OK)        { goto done; }

		
			
         }
       }




	   
     }
   }



	if(1 == k_is_odd)
	{
		/*ex:
		k=5 --->  4 + 1  (4mul +1add)
		*/

		//draw_point("G", G);
		//draw_point("R", R);

		DO(tfm_ecc_projective_add_point_ntd(G, R, R,  modulus, a));

		mp_add_d(k, 1, k);
		//draw_point("R1", R);

	}


   /* map R back from projective space */
   //if (map) {
   //   err = ltc_ecc_map(R, modulus, mp);
   //} else {
      err = CRYPT_OK;
   //}



   
done:
   if (mu != NULL) {
      mp_clear(mu);
   }
   //mp_montgomery_free(mp);
   ltc_ecc_del_point(tG);
   for (i = 0; i < 8; i++) {
       ltc_ecc_del_point(M[i]);
   }
   return err;
}



#undef WINSIZE


/*
  make point on y^2 = x^3 + ax + b
  
  mod:       modules, (p)
  (gx, gy):   store point  on the curve

  return:  the order of the curve
  
*/
int ecc_make_test_point(int _mod, int _a, int _b, int gx[], int gy[])
{
	int err = -1;
	void  *a,  *b;



	int i = 0, j = 0;

	int point_count = 0;

	void	*t1, *t2, *x, *y, *modulus;




	int is_a0;	/* _a < 0 ?*/




	/* init */
	err = mp_init_multi(&t1, &t2, &x, &y, &a, &b, &modulus,   NULL);
	if(err != CRYPT_OK)
	{
		dprintf(">>>%s: err0\n", __FUNCTION__);
		return -1;
	}



	if(_a <0)
	{

		//dprintf("a<0:");
		mp_set(a, -_a);
		is_a0 = 1;
	}
	else
	{
		mp_set(a, _a);
		is_a0 = 0;

	}

	
	mp_set(b, _b);	


	mp_set(modulus, _mod);

	//dprintf("modulus:\n");
	//mp_drawhex(modulus);

	for(i=0; i<_mod; i++)
	{


		/*x: prims[i]*/
		//x = primes[i];
		mp_set(x, i);


		/* x^3 + ax +b */
		mp_set(t1, 0);
		mp_sqr(x, t1);
		mp_mul(x, t1, t1);
		//dprintf("x^3:\n");
		//mp_drawhex(&t1);
		 
		
		/*ax*/
		mp_mul(x, a, t2);
		//dprintf("ax:\n");
		//mp_drawhex(&t2);
		
		/* x^3 + ax +b */
		if(is_a0==0)		
		{
			mp_add(t1, t2, t1);
		}
		else
		{
			mp_sub(t1, t2, t1);
		}
		
		mp_addmod(t1, b,  modulus, t1);



		//dprintf("right:\n");
		//mp_drawhex(&t1);
		
		
		


		/*y exited?*/
		for(j=0; j<_mod; j++)
		{


			//y = primes[j];
			mp_set(y, j);

		
			/* y^2	*/
			mp_set(t2, 0);
			mp_sqrmod(y, modulus, t2);

			//dprintf("left:\n");
			//mp_drawhex(&t2);
			


			if(0 == mp_cmp(t1, t2))
			{

				gx[point_count] = i;
				gy[point_count] = j;
				point_count ++;	
				dprintf("GOT IT [%03d:][%02d %02d]\n", point_count, i, j);

	
								
			}
			else
			{
				//dprintf("NOT THE SAME!\n");

			}
			
			/**/
		}

		

	}


	/* find out the order*/

	mp_clear_multi(t1, t2, x, y, a, b, modulus,   NULL);
	

	return point_count;

}


/**
  Perform on the ECC system
  @return CRYPT_OK if successful
*/

int ecc_verify_nist_curve_point(void *a, void *b, ecc_point *G, void *modulus)
{

	int err;



//	int j = 0;

	void *t1;
	void *t2;

	mp_init(&t1);
	mp_init(&t2);


	//dprintf("a	  :   ");
	//mp_drawhex(a);

	//dprintf("b	 :	 ");
	//mp_drawhex(b);


	//dprintf("modulus:\n");
	//mp_drawhex(modulus);

	//dprintf("mp:\n");
	//mp_drawhex(mp);


	//dprintf("G:\n");
	//mp_drawhex(G->x);
	//mp_drawhex(G->y); 	   


	/*verify the curver */
	/* x^3 + ax +b */
	mp_set(t1, 0);
	mp_sqr(G->x, t1);
	mp_mul(G->x, t1, t1);
	//dprintf("x^3:\n");
	//mp_drawhex(t1);


	/*ax*/
	mp_mul(G->x, a, t2);
	//dprintf("ax:\n");
	//mp_drawhex(&t2);

	/* x^3 - ax +b */
	mp_sub(t1, t2, t1);
	mp_addmod(t1, b, modulus, t1);
	dprintf("RIGHT:\n");
	mp_drawhex(t1);



	/* */
	/* y^2  */
	mp_set(&t2, 0);
	mp_sqrmod(G->y, modulus, t2);

	dprintf("LEFT:\n");
	mp_drawhex(t2);




	if (mp_cmp(&t1, &t2) != LTC_MP_EQ) 
	{
		err = CRYPT_FAIL_TESTVECTOR;
		goto done;
	}
	else
	{
		dprintf("%s verify ok\n", __FUNCTION__);
	}

	err = CRYPT_OK;

done:





	dprintf("%s end[%d]\n", __FUNCTION__, err);
	DO(err);


   return err;
}






/**
  Perform on the ECC system
  @return CRYPT_OK if successful
*/

int ecc_verify_custom_curve_point(void *a, void *b, ecc_point *G, void *modulus)
{

	int  err;



//	int j = 0;

	void *t1;
	void *t2;



	//dprintf("a	  :   ");
	//mp_drawhex(a);

	//dprintf("b	 :	 ");
	//mp_drawhex(b);


	//dprintf("modulus:\n");
	//mp_drawhex(modulus);

	//dprintf("mp:\n");
	//mp_drawhex(mp);


	//dprintf("G:\n");
	//mp_drawhex(G->x);
	//mp_drawhex(G->y); 	   

	mp_init(&t1);
	mp_init(&t2);


	/*verify the curver */
	/* x^3 + ax +b */
	mp_set(t1, 0);
	mp_sqr(G->x, t1);
	mp_mul(G->x, t1, t1);
	//dprintf("x^3:\n");
	//mp_drawhex(&t1);


	/*ax*/
	mp_mul(G->x, a, t2);
	//dprintf("ax:\n");
	//mp_drawhex(&t2);

	/* x^3 + ax +b */
	mp_add(t1, t2, t1);
	mp_addmod(t1, b, modulus, t1);			// fix

	//dprintf("RIGHT:\n");
	//mp_drawhex(t1);



	/* */
	/* y^2  */
	mp_set(t2, 0);
	mp_sqrmod(G->y, modulus, t2);

	//dprintf("LEFT:\n");
	//mp_drawhex(t2);




	if (mp_cmp(t1, t2) != LTC_MP_EQ) 
	{
		err = CRYPT_FAIL_TESTVECTOR;
		goto done;
	}
	else
	{
		dprintf("%s !! verify ok\n", __FUNCTION__);
	}

	err = CRYPT_OK;

done:





	//dprintf("%s end[%d]\n", __FUNCTION__, err);
	DO(err);


   return err;
}






/*
	!!!!!   MAKE SURE NOT OVERFLOW
*/
#define MAX_POINT (1024)



/**
  Free an ECC key from memory
  @param key   The key you wish to free
*/
void ecc_my_free(custom_ecc_key *key)
{
   LTC_ARGCHKVD(key != NULL);
   mp_clear_multi(key->pubkey.x, key->pubkey.y, key->pubkey.z, key->k, NULL);
}


int ecc_my_test(void)
{
	/* rand number */
	int err = -1;
	unsigned char buf[ECC_BUF_SIZE];
	prng_state l_prng;

	
	unsigned int size = 0;
	
	
	int p = 0, i = 0, j = 0;
	int order = 0;
	//int point_ok = -1;
	int gx[MAX_POINT] = {0}, gy[MAX_POINT] = {0};
	int a = 0, b = 0;



	void *modulus, *kA, *kB, *rA, *rB;
	ecc_point *G, *A, *B, *C1, *C2, *C3;
	

	void *AA, *BB;


//	int test_times = 0;
	

	
	//ltc_mp = tfm_desc;


	/* rand number engine init */
	register_prng(&yarrow_desc);
	if ((err = rng_make_prng(128, find_prng("yarrow"), &l_prng, NULL)) != CRYPT_OK) {
	   dprintf("rng_make_prng failed: %s\n", error_to_string(err));
	   return -1;
	}
		
	
	if(register_hash(&sha256_desc)<0){
		dprintf("%s: register_hash fail\n", __FUNCTION__);
		return -1;
	}
	
		
	
	/* start it */
	if ((err = yarrow_start(&l_prng)) != CRYPT_OK) {
		dprintf("yarrow_starterror: %s\n", error_to_string(err));
	}
	
	
	err = yarrow_read(buf, 14, &l_prng);
	dprintf("%s rand number engine init done[%d]\n", __FUNCTION__, 	err);


	mp_init(&AA);
	mp_init(&BB);

	DO(mp_init_multi(&kA, &kB, &rA, &rB, &modulus, NULL));
	LTC_ARGCHK((G  = ltc_ecc_new_point()) != NULL);
	LTC_ARGCHK((A  = ltc_ecc_new_point()) != NULL);
	LTC_ARGCHK((B  = ltc_ecc_new_point()) != NULL);
	LTC_ARGCHK((C1 = ltc_ecc_new_point()) != NULL);
	LTC_ARGCHK((C2 = ltc_ecc_new_point()) != NULL);
	LTC_ARGCHK((C3 = ltc_ecc_new_point()) != NULL);




	/* setting */
    p = 29;
	a = 4;
	b = 20;

	
    /*
		P:23, a:1, b:1
	*/
	order = ecc_make_test_point(p, a, b, gx, gy);
	dprintf("ecc curve Epab(%d %d %d) made:\n", p, a, b);
	for(i=0; i<order; i++)
	{
		dprintf("[%03d:][%02d %02d]\n", i, gx[i], gy[i]);

	}


	/* montgomery setting */

	


	mp_read_radix(modulus, "24163241384221333188237658810894846872976096974919190794300310889851",  10);
	//DO(mp_montgomery_setup(modulus, &mp));
	//dprintf("modulus		 :	 ");
	//mp_drawhex(modulus);
	draw_mp("modulus", modulus);


	/* set x^2 + AAx +BB */
	//mp_set(AA, a);		
	//mp_set(BB, b);

	/* TEST SONY A*/
	mp_read_radix(AA, "24163241384221333188237658810894846872976096974919190794300310889848",  10);
	mp_read_radix(BB, "2587742116360477437854941586996965223027589545310398799785139463622", 10);

	j = 2;
	mp_set(C1->x, gx[j]);
	mp_set(C1->y, gy[j]);
	mp_set(C1->z, 1);

	mp_set(C2->x, gx[j]);
	mp_set(C2->y, gy[j]);
	mp_set(C2->z, 1);	

	mp_set(C3->x, 0);
	mp_set(C3->y, 0);
	mp_set(C3->z, 1);

	{
         size = 28;
		
		 /* load it */
		 //mp_copy(C1->x, G->x);
		 //mp_copy(C1->y, G->y);
		 mp_read_radix(G->x, "11375220201996271814923170291964317095551923549910272164456425049335",  10);
		 mp_read_radix(G->y, "1761478402820299707060885881943897573132537129120890735323665717780",  10);

		 
		 mp_set(G->z, 1);


			
		 dprintf("/* do 100 random tests */\n");
		 /* do 100 random tests */
		 for (i = 0; i < 100; i++) {
			/* pick a random r1, r2 */

			LTC_ARGCHK(yarrow_read(buf, size, &l_prng) == size);
			DO(mp_read_unsigned_bin(rA, buf, size));
			LTC_ARGCHK(yarrow_read(buf, size, &l_prng) == size);
			DO(mp_read_unsigned_bin(rB, buf, size));


			/*test value: */
			mp_set(rA, 10);
			mp_set(rB, 11);			
		
			mp_mod(rA, modulus, rA);
			mp_mod(rB, modulus, rB);			
			
			/* compute rA * G = A */

			//mp_add_d(rA, 1, rA);
			//mp_add_d(rB, 1, rB);


			//dprintf("\n\n\nmodulus:\n");
			//mp_drawhex(modulus);
			draw_mp("modulus", modulus);



			//dprintf("\n\n\nAA:\n");
			//mp_drawhex(AA);
			draw_mp("AA", AA);



			//dprintf("\n\n\nG:\n");
			//mp_drawhex(G->x);
			//mp_drawhex(G->y);
			//mp_drawhex(G->z);

			draw_point("G", G);



			draw_mp("rA", rA);
			draw_mp("rB", rB);


			//DO(ltc_mp.ecc_ptmul(rA, G, A, modulus, 1));
			ltc_ecc_mulmod_ntd(rA, G, A, modulus,  AA);

			//dprintf("\ncompute rA * G = A:\n");
			draw_point("A:", A);


					

	
			/* compute rB * G = B */
			
			//DO(ltc_mp.ecc_ptmul(rB, G, B, modulus, 1));
			ltc_ecc_mulmod_ntd(rB, G, B, modulus,  AA);

			//dprintf("\ncompute rB * G = B\n");
			draw_point("B:", B);





	
			/* pick a random kA, kB */
			//dprintf("\n\npick a random kA, kB\n");
			LTC_ARGCHK(yarrow_read(buf, size, &l_prng) == size);
			DO(mp_read_unsigned_bin(kA, buf, size));
			LTC_ARGCHK(yarrow_read(buf, size, &l_prng) == size);
			DO(mp_read_unsigned_bin(kB, buf, size));


			/*test value: */
			mp_set(kA, 12);
			mp_set(kB, 13);	


			mp_mod(kA, modulus, kA);
			mp_mod(kB, modulus, kB);


			draw_mp("kA", kA);
			draw_mp("kB", kB);


			//while(1);


			/* now, compute kA*A + kB*B = C1 using the older method */
			//DO(ltc_mp.ecc_ptmul(kA, A, C1, modulus, 0));
			ltc_ecc_mulmod_ntd(kA, A, C1, modulus,  AA);

			draw_point("C1", C1);



			//DO(ltc_mp.ecc_ptmul(kB, B, C2, modulus, 0));
			ltc_ecc_mulmod_ntd(kB, B, C2, modulus, AA);


			draw_point("C2", C2);




			//dprintf("\n@@@kA*A + kB*B = C3:\n");

			//DO(ltc_mp.ecc_ptadd(C1, C2, C1, modulus, mp));
			tfm_ecc_projective_add_point_ntd(C1, C2, C3, modulus,  AA);


			draw_point("C3", C3);


				

		    //dprintf("\n");
			//DO(ltc_mp.ecc_map(C1, modulus, mp));

			#if 1
			//dprintf("now compute using mul2add:\n");
			/* now compute using mul2add */
			DO(ltc_ecc_mul2add_ntd(A, kA, B, kB, C2, modulus, AA));


			draw_point("C2", C2);


			
			#endif

	
			/* is they the sames?  */
			if ((mp_cmp(C3->x, C2->x) != LTC_MP_EQ) || (mp_cmp(C3->y, C2->y) != LTC_MP_EQ) || (mp_cmp(C3->z, C2->z) != LTC_MP_EQ)) {
			   dprintf("ECC failed shamir test: size=%d, \n", size);
			   //while(1);
			   return 1;
			}
		}
		//mp_montgomery_free(mp);
	}



    dprintf("%s test times[%d] ok\n", __FUNCTION__, i);

	
	ltc_ecc_del_point(C2);
	ltc_ecc_del_point(C1);
	ltc_ecc_del_point(B);
	ltc_ecc_del_point(A);
	ltc_ecc_del_point(G);
	mp_clear_multi(kA, kB, rA, rB, modulus, NULL);

	return 0;
}


custom_ecc_set_type sony_ecc_para =
	{	
		/* size:    */				CRYPTO_ECDSA_PRIVKEY_LEN,	
		/* name:  */				"ECC_SONY",
		/* prime(modulus):   */	"E5719607982A1BD3B26F209D0E8FFD687A5D7F9DF0FB31518BEF257B",	
		/* A:*/					"E5719607982A1BD3B26F209D0E8FFD687A5D7F9DF0FB31518BEF2578",
		/* B:*/					"189273DF5DF7F84A865EFD12906D6D50ED0913A2F75C223DF47565C6",	
		/* order:*/				"E5719607982A1BD3B26F209D0E9183E556C62DD3BD94495543FB060F",	
		/* gx:    */				"6C03A0E91D755CA28FE6409C412E8C8963548137150780FF974BCCF7",	
		/* gy:   */				"10B9EAB38449EE2B0CF231792DDBC63E0E5E0D08D1008BA3FA7CDA14"
	};





int ecc_make_keypub(const unsigned char   _in_pubkey[CRYPTO_ECDSA_PUBKEY_LEN], int keysize, custom_ecc_key *key, custom_ecc_set_type *dp)
{
	int err;

	unsigned char in_pubkey[CRYPTO_ECDSA_PUBKEY_LEN] = {0};
	memcpy(in_pubkey, _in_pubkey, CRYPTO_ECDSA_PUBKEY_LEN);

	if(NULL == key || NULL == dp)
	{
		return -1;
	}


	dprintf(">>> %s\n", __FUNCTION__);


	/* init */
	err = mp_init_multi(&(key->pubkey.x), &(key->pubkey.y), &(key->pubkey.z), NULL);
	if(err != CRYPT_OK)
	{
		dprintf(">>>%s: err0\n", __FUNCTION__);
		goto end;
	}

	key->dp  = dp;
	key->idx = -1;
	key->type= PK_PUBLIC;


	



	//dump_hex_name("pub", in_pubkey, CRYPTO_ECDSA_PUBKEY_LEN);

	err = mp_init_multi(&(key->pubkey.x), &(key->pubkey.y), &(key->pubkey.z), NULL);
	if(err != CRYPT_OK)
	{
		goto end;
	}
	

	

	err = mp_read_unsigned_bin(key->pubkey.x, in_pubkey,		 keysize);
	if(err != CRYPT_OK)
	{
		goto end;
	}
	


	
	err = mp_read_unsigned_bin(key->pubkey.y, in_pubkey+keysize, keysize);
	if(err != CRYPT_OK)
	{
		goto end;
	}

	if ((err = mp_set(key->pubkey.z, 1)) != CRYPT_OK)
	{
		goto end;
	}



	//fdprintf(stderr, ">>>%s: ok[%d]\n", __FUNCTION__,err);

	//dprintf("ecc key pub:\n");
	//mp_drawhex(key->pubkey.x);
	//mp_drawhex(key->pubkey.y);
	//mp_drawhex(key->pubkey.z);

#if 0
	dprintf("ecc key dp:\n");
	dprintf("\t size		(%d)\n", key->dp->size);
	dprintf("\t name		(%s)\n", key->dp->name);
	dprintf("\t prime		(%s)\n", key->dp->prime);

	dprintf("\t a		(%s)\n", key->dp->A);
	dprintf("\t b		(%s)\n", key->dp->B);

	dprintf("\t order		(%s)\n", key->dp->order);
	
	dprintf("\t gx		(%s)\n", key->dp->Gx);
	dprintf("\t gy		(%s)\n", key->dp->Gy);


	dprintf("\n\n");
#endif

	return 0;

end:

	return err;


}



/*
	ECDH:
	client : generate d random number: b, B=b*G; (out_client_p1v: B)
		    received the A from server (A = a*G,   in_svr_p1v: A)
		    shared key Q = B*A;
	
*/

int ecc_dh(const unsigned char              _in_svr_p1v[CRYPTO_ECDH_PHASE1VALUE_LEN],
            unsigned char                    out_client_p1v[CRYPTO_ECDH_PHASE1VALUE_LEN],
            unsigned char                    out_pubkey[CRYPTO_ECDH_PUBKEY_LEN])
{
	/* rand number */
	int err = -1;
	unsigned char buf[ECC_BUF_SIZE];
	prng_state l_prng;
	unsigned char			   in_svr_p1v[CRYPTO_ECDH_PHASE1VALUE_LEN];
	unsigned int size = 0;
	int x_size = 0, y_size = 0;
	void *modulus,   *rb,  *AA, *BB;
	ecc_point *G,  *B, *A, *Q;


	memcpy(in_svr_p1v, _in_svr_p1v, CRYPTO_ECDH_PHASE1VALUE_LEN);
	
	/* rand number engine init */
	register_prng(&yarrow_desc);
	if ((err = rng_make_prng(128, find_prng("yarrow"), &l_prng, NULL)) != CRYPT_OK) {
	   dprintf("rng_make_prng failed: %s\n", error_to_string(err));
	   return -1;
	}
		
	
	if(register_hash(&sha256_desc)<0){
		dprintf("%s: register_hash fail\n", __FUNCTION__);
		return -1;
	}
	
		
	
	/* start it */
	if ((err = yarrow_start(&l_prng)) != CRYPT_OK) {
		dprintf("yarrow_starterror: %s\n", error_to_string(err));
	}
		

	/* init */
	DO(mp_init_multi(&rb, &modulus, &AA, &BB, NULL));
	LTC_ARGCHK((G  = ltc_ecc_new_point()) != NULL);
	LTC_ARGCHK((B  = ltc_ecc_new_point()) != NULL);
	LTC_ARGCHK((A  = ltc_ecc_new_point()) != NULL);
	LTC_ARGCHK((Q  = ltc_ecc_new_point()) != NULL);

	/* set up ecc cruver parameters */
	mp_read_radix(modulus, "24163241384221333188237658810894846872976096974919190794300310889851",  10);
	mp_read_radix(AA, "24163241384221333188237658810894846872976096974919190794300310889848",  10);
	mp_read_radix(BB, "2587742116360477437854941586996965223027589545310398799785139463622", 10);	
    mp_read_radix(G->x, "11375220201996271814923170291964317095551923549910272164456425049335",  10);
	mp_read_radix(G->y, "1761478402820299707060885881943897573132537129120890735323665717780",  10);	
	mp_set(G->z, 1);

	
	//draw_mp("modulus", modulus);

	size = CRYPTO_ECDH_PRIVKEY_LEN;
			 
	
	/* pick a random rb */
	LTC_ARGCHK(yarrow_read(buf, size, &l_prng) == size);
	DO(mp_read_unsigned_bin(rb, buf, size));
	mp_add_d(rb, 1, rb);
	mp_mod(rb, modulus, rb);
	//draw_mp("rb", rb);




	/* compute shared key: rb * A = Q */
	/* A: (out_client_p1v)*/

	/* A on the curve ?*/
	//DumpHex("A(in_svr_p1v)", in_svr_p1v, CRYPTO_ECDH_PHASE1VALUE_LEN);
	mp_read_unsigned_bin(A->x, in_svr_p1v, CRYPTO_ECDH_PHASE1VALUE_LEN>>1);
	mp_read_unsigned_bin(A->y, 
		          in_svr_p1v + (CRYPTO_ECDH_PHASE1VALUE_LEN>>1), 
		          CRYPTO_ECDH_PHASE1VALUE_LEN>>1);
	mp_set(A->z, 1);
	//draw_point("server p1v:", A);


#if 0
	if ( 0 == ecc_verify_custom_curve_point(AA, BB,
								  A,
								   modulus) )
	{
		dprintf("A(server p1v) is on the curver\n");
	}
	else
	{
		dprintf("A(server p1v) is NOT on the curver !!!\n");

	}

#endif	

		
	ltc_ecc_mulmod_ntd(rb, A, Q, modulus,  AA);
	
	//draw_point("public key:", Q);
	
    x_size = mp_unsigned_bin_size(Q->x);
    y_size = mp_unsigned_bin_size(Q->y);

	mp_to_unsigned_bin(Q->x, out_pubkey + (CRYPTO_ECDH_PRIVKEY_LEN-x_size));
	mp_to_unsigned_bin(Q->y, out_pubkey +  CRYPTO_ECDH_PRIVKEY_LEN + (CRYPTO_ECDH_PRIVKEY_LEN-y_size));
	
#if 0
	if ( 0 == ecc_verify_custom_curve_point(AA, BB,
								  Q,
								   modulus) )
	{
		dprintf("Q(pub key) is on the curver\n");
	}
	else
	{
		dprintf("Q(pub key) is NOT on the curver !!!\n");

	}

#endif



	/* compute B:  rb * G = B */
	ltc_ecc_mulmod_ntd(rb, G, B, modulus,  AA);
	
	//draw_point("client p1v:", B);	

    x_size = mp_unsigned_bin_size(B->x);
    y_size = mp_unsigned_bin_size(B->y);

	mp_to_unsigned_bin(B->x, out_client_p1v + (CRYPTO_ECDH_PRIVKEY_LEN-x_size));
	mp_to_unsigned_bin(B->y, out_client_p1v +  CRYPTO_ECDH_PRIVKEY_LEN + (CRYPTO_ECDH_PRIVKEY_LEN-y_size));
	






#if 0
	if ( 0 == ecc_verify_custom_curve_point(AA, BB,
								  B,
								   modulus) )
	{
		dprintf("B(client p1v) is on the curver\n");
	}
	else
	{
		dprintf("B(client p1v) is NOT on the curver !!!\n");

	}
#endif
	



	

	ltc_ecc_del_point(A);
	ltc_ecc_del_point(B);
	ltc_ecc_del_point(G);
	ltc_ecc_del_point(Q);
	mp_clear_multi(rb, modulus, AA, BB, NULL);


	return 0;
}
























int ecc_make_keypriv(const unsigned char   in_privkey[CRYPTO_ECDSA_PRIVKEY_LEN], custom_ecc_key *key, custom_ecc_set_type *dp)
{
	int			  err;
	ecc_point	 *base;
	void 		 *prime, *order;
	unsigned char *buf;
	void 		 *a;
	int			  keysize;

	LTC_ARGCHK(key		  != NULL);
	LTC_ARGCHK(ltc_mp.name != NULL);
	LTC_ARGCHK(dp		  != NULL);


	key->idx = -1;
	key->dp	= dp;
	keysize	= dp->size;

	if(NULL == key || NULL == dp)
	{
	return -1;
	}



	/* allocate ram */
	base = NULL;
	buf	= XMALLOC(ECC_MAXSIZE);
	if (buf == NULL) {
		return CRYPT_MEM;
	}



	/* setup the key variables */
	if ((err = mp_init_multi(&key->pubkey.x, &key->pubkey.y, &key->pubkey.z, &key->k, &prime, &order, &a, NULL)) != CRYPT_OK) {
		goto ERR_BUF;
	}
	
	base = ltc_ecc_new_point();
	if (base == NULL) {
		err = CRYPT_MEM;
		goto errkey;
	}

	/* read in the specs for this key */
	if ((err = mp_read_radix(prime,	 (char *)key->dp->prime, 16)) != CRYPT_OK)					{ goto errkey; }
	if ((err = mp_read_radix(order,	 (char *)key->dp->order, 16)) != CRYPT_OK)					{ goto errkey; }
	if ((err = mp_read_radix(base->x, (char *)key->dp->Gx, 16)) != CRYPT_OK) 					{ goto errkey; }
	if ((err = mp_read_radix(base->y, (char *)key->dp->Gy, 16)) != CRYPT_OK) 					{ goto errkey; }
	if ((err = mp_set(base->z, 1)) != CRYPT_OK)													{ goto errkey; }
	if ((err = mp_read_unsigned_bin(key->k, (unsigned char *)in_privkey, keysize)) != CRYPT_OK) 		{ goto errkey; }

	if ((err = mp_read_radix(a, (char *)key->dp->A, 16)) != CRYPT_OK)					 { goto errkey; }


	/* the key should be smaller than the order of base point */
	if (mp_cmp(key->k, order) != LTC_MP_LT) {
	if((err = mp_mod(key->k, order, key->k)) != CRYPT_OK)									{ goto errkey; }
	}
	/* make the public key */
	if ((err = ltc_ecc_mulmod_ntd(key->k, base, &key->pubkey, prime,  a)) != CRYPT_OK) 			 { goto errkey; }
	key->type = PK_PRIVATE;

	/* free up ram */
	err = CRYPT_OK;



	//fdprintf(stderr, ">>>%s: ok[%d]\n", __FUNCTION__,err);

#if 0
	dprintf("ecc key priv:\n");
	mp_drawhex(key->k);



	dprintf("ecc key pub:\n");
	mp_drawhex(key->pubkey.x);
	mp_drawhex(key->pubkey.y);
	mp_drawhex(key->pubkey.z);

	dprintf("ecc key dp:\n");
	dprintf("\t size	   (%d)\n", key->dp->size);
	dprintf("\t name	   (%s)\n", key->dp->name);
	dprintf("\t prime 	   (%s)\n", key->dp->prime);

	dprintf("\t a 	   (%s)\n", key->dp->A);
	dprintf("\t b 	   (%s)\n", key->dp->B);

	dprintf("\t order 	   (%s)\n", key->dp->order);

	dprintf("\t gx	   (%s)\n", key->dp->Gx);
	dprintf("\t gy	   (%s)\n", key->dp->Gy);


	dprintf("\n\n");

#endif

	goto cleanup;
	errkey:
	mp_clear_multi(key->pubkey.x, key->pubkey.y, key->pubkey.z, key->k, NULL);
	cleanup:
	ltc_ecc_del_point(base);
	mp_clear_multi(prime, order, NULL);
	ERR_BUF:
#ifdef LTC_CLEAN_STACK
	zeromem(buf, ECC_MAXSIZE);
#endif
	XFREE(buf);
	return err;



}




int ecc_make_key_ex_general(prng_state *prng, int wprng, custom_ecc_key *key, const  custom_ecc_set_type *dp)
{
   int            err;
   ecc_point     *base;
   void          *prime, *order;
   unsigned char *buf;
   void			 *a;
   int            keysize;




   LTC_ARGCHK(key         != NULL);
   LTC_ARGCHK(ltc_mp.name != NULL);
   LTC_ARGCHK(dp          != NULL);

   /* good prng? */
   if ((err = prng_is_valid(wprng)) != CRYPT_OK) {
      return err;
   }

   key->idx = -1;
   key->dp  = dp;
   keysize  = dp->size;

   /* allocate ram */
   base = NULL;
   buf  = XMALLOC(ECC_MAXSIZE);
   if (buf == NULL) {
      return CRYPT_MEM;
   }

   /* make up random string */
   if (prng_descriptor[wprng].read(buf, (unsigned long)keysize, prng) != (unsigned long)keysize) {
      err = CRYPT_ERROR_READPRNG;
      goto ERR_BUF;
   }

   /* setup the key variables */
   if ((err = mp_init_multi(&key->pubkey.x, &key->pubkey.y, &key->pubkey.z, &key->k, &prime, &order, &a, NULL)) != CRYPT_OK) {
      goto ERR_BUF;
   }
   base = ltc_ecc_new_point();
   if (base == NULL) {
      err = CRYPT_MEM;
      goto errkey;
   }

   /* read in the specs for this key */
   if ((err = mp_read_radix(prime,   (char *)key->dp->prime, 16)) != CRYPT_OK)                  { goto errkey; }
   if ((err = mp_read_radix(order,   (char *)key->dp->order, 16)) != CRYPT_OK)                  { goto errkey; }
   if ((err = mp_read_radix(base->x, (char *)key->dp->Gx, 16)) != CRYPT_OK)                     { goto errkey; }
   if ((err = mp_read_radix(base->y, (char *)key->dp->Gy, 16)) != CRYPT_OK)                     { goto errkey; }
   if ((err = mp_set(base->z, 1)) != CRYPT_OK)                                                  { goto errkey; }
   if ((err = mp_read_unsigned_bin(key->k, (unsigned char *)buf, keysize)) != CRYPT_OK)         { goto errkey; }


   if ((err = mp_read_radix(a, (char *)key->dp->A, 16)) != CRYPT_OK)                     { goto errkey; }


   /* the key should be smaller than the order of base point */
   if (mp_cmp(key->k, order) != LTC_MP_LT) {
       if((err = mp_mod(key->k, order, key->k)) != CRYPT_OK)                                    { goto errkey; }
   }
   /* make the public key */
   if ((err = ltc_ecc_mulmod_ntd(key->k, base, &key->pubkey, prime, a)) != CRYPT_OK)              { goto errkey; }
   key->type = PK_PRIVATE;

   /* free up ram */
   err = CRYPT_OK;
   goto cleanup;
errkey:
   mp_clear_multi(key->pubkey.x, key->pubkey.y, key->pubkey.z, key->k, NULL);
cleanup:
   ltc_ecc_del_point(base);
   mp_clear_multi(prime, order, NULL);
ERR_BUF:
#ifdef LTC_CLEAN_STACK
   zeromem(buf, ECC_MAXSIZE);
#endif
   XFREE(buf);
   return err;
}






/**
  Make a new ECC key 
  @param prng         An active PRNG state
  @param wprng        The index of the PRNG you wish to use
  @param keysize      The keysize for the new key (in octets from 20 to 65 bytes)
  @param key          [out] Destination of the newly created key
  @return CRYPT_OK if successful, upon error all allocated memory will be freed
*/
int ecc_make_key_general(prng_state *prng, int wprng,  custom_ecc_key *key, custom_ecc_set_type *dp)
{
   int  err;


   err = ecc_make_key_ex_general(prng, wprng, key, dp);
   key->idx = -1;
   
   return err;
}








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
                        prng_state *prng, int wprng, custom_ecc_key *priv_key)
{
   /* temp store the k and kG*/	
   custom_ecc_key       tempkey;

   
   void          *r, *s, *e, *p;
   int           err;

   LTC_ARGCHK(in     != NULL);
   LTC_ARGCHK(out    != NULL);
   LTC_ARGCHK(outlen != NULL);
   LTC_ARGCHK(priv_key    != NULL);

   /* is this a private key? */
   if (priv_key->type != PK_PRIVATE) {
      return CRYPT_PK_NOT_PRIVATE;
   }
   
   /* is the IDX valid ?  */
  // if (ltc_ecc_is_valid_idx(key->idx) != 1) {
  //    return CRYPT_PK_INVALID_TYPE;
  // }
   
   if ((err = prng_is_valid(wprng)) != CRYPT_OK) {
      return err;
   }

   /* get the hash and load it as a bignum into 'e' */
   /* init the bignums */
   if ((err = mp_init_multi(&r, &s, &p, &e, NULL)) != CRYPT_OK) { 
      return err;
   }
   if ((err = mp_read_radix(p, (char *)priv_key->dp->order, 16)) != CRYPT_OK)                      { goto errnokey; }
   if ((err = mp_read_unsigned_bin(e, (unsigned char *)in, (int)inlen)) != CRYPT_OK)          { goto errnokey; }

   /* make up a key and export the public copy */
   for (;;) {

	  /* 1. select k:[1, n-1]*/
	  /* 2. Compute kP = (x1, y1) */

	  /* select a random integer k from [1, n-1]  ---> pubkey.k */
	  /* calculate the curve point (x1, y1) = kG   ---> pubkey.pubkey*/
      if ((err = ecc_make_key_ex_general(prng, wprng, &tempkey, priv_key->dp)) != CRYPT_OK) {
         goto errnokey;
      }

      /* find r = x1 mod n */
      if ((err = mp_mod(tempkey.pubkey.x, p, r)) != CRYPT_OK)                 { goto error; }

      if (mp_iszero(r) == LTC_MP_YES) {
         ecc_my_free(&tempkey);
      } else { 
        /* find s = (e + xr)/k */

		/* calculate 1/k */
        if ((err = mp_invmod(tempkey.k, p, tempkey.k)) != CRYPT_OK)            { goto error; } /* k = 1/k */

		/* calculate dA * r*/		
        if ((err = mp_mulmod(priv_key->k, r, p, s)) != CRYPT_OK)                  { goto error; } /* s = xr */

		/* calculate dA * r + e */		
        if ((err = mp_add(e, s, s)) != CRYPT_OK)                             { goto error; } /* s = e +  xr */
        if ((err = mp_mod(s, p, s)) != CRYPT_OK)                             { goto error; } /* s = e +  xr */


		/* calcute (dA * r + e)/k  */		
        if ((err = mp_mulmod(s, tempkey.k, p, s)) != CRYPT_OK)                { goto error; } /* s = (e + xr)/k */

		ecc_my_free(&tempkey);
		
        if (mp_iszero(s) == LTC_MP_NO) {
           break;
        }
      }
   }

   dprintf("\n\nECC sign:\nr:   ");
   mp_drawhex(r);


   dprintf("s:   ");
   mp_drawhex(s);

   dprintf("\n");


   /* store as SEQUENCE { r, s -- integer } */
   err = der_encode_sequence_multi(out, outlen,
                             LTC_ASN1_INTEGER, 1UL, r,
                             LTC_ASN1_INTEGER, 1UL, s,
                             LTC_ASN1_EOL, 0UL, NULL);


   
   dump_hex_name("SEQUENCE r,s:", out, *outlen);

   
   goto errnokey;
error:
   ecc_my_free(&tempkey);
errnokey:
   mp_clear_multi(r, s, p, e, NULL);
   return err;   
}










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
                    int *stat, custom_ecc_key *key)
{
   ecc_point    *mG, *mQ;
   void          *r, *s, *v, *w, *u1, *u2, *e, *p, *m, *a;
   void          *mp;
   int           err;

   int r_s_len;


   LTC_ARGCHK(sig  != NULL);
   LTC_ARGCHK(hash != NULL);
   LTC_ARGCHK(stat != NULL);
   LTC_ARGCHK(key  != NULL);

   /* default to invalid signature */
   *stat = 0;
   mp    = NULL;



   /* allocate ints */
   if ((err = mp_init_multi(&r, &s, &v, &w, &u1, &u2, &p, &e, &m, &a, NULL)) != CRYPT_OK) {
      return CRYPT_MEM;
   }

   /* allocate points */
   mG = ltc_ecc_new_point();
   mQ = ltc_ecc_new_point();
   if (mQ  == NULL || mG == NULL) {
      err = CRYPT_MEM;
      goto error;
   }


   /* siglen should not be odd */



   /*
   		fix:
   		sig(r, s)
 		init r, s
      */
    //dprintf("%s siglen:(%d)\n", __FUNCTION__, siglen);  
	r_s_len = (siglen>>1);

	if(r_s_len == 0)
	{
		goto error;
	}
	

#if 0
	mp_read_unsigned_bin(r, sig, r_s_len);
	dprintf("r: \n");
	mp_drawhex(r);


	mp_read_unsigned_bin(s, sig+r_s_len, r_s_len);
	dprintf("s: \n");
	mp_drawhex(s);


#else
	/* parse header */
	if ((err = der_decode_sequence_multi(sig, siglen,
								   LTC_ASN1_INTEGER, 1UL, r,
								   LTC_ASN1_INTEGER, 1UL, s,
								   LTC_ASN1_EOL, 0UL, NULL)) != CRYPT_OK) {
	   goto error;
	}

	dprintf("\n\ndecode sequence:\n");
	dprintf("r: \n");
	mp_drawhex(r);

	
	dprintf("s: \n");
	mp_drawhex(s);

	dprintf("\n\n");

#endif


   /* get the order */
   if ((err = mp_read_radix(p, (char *)key->dp->order, 16)) != CRYPT_OK)                                { goto error; }

   /* get the modulus */
   if ((err = mp_read_radix(m, (char *)key->dp->prime, 16)) != CRYPT_OK)                                { goto error; }

   /* get the a */
   if ((err = mp_read_radix(a, (char *)key->dp->A, 16)) != CRYPT_OK)                                { goto error; }   


   /* check for zero */
   //dprintf("p:");
   //mp_drawhex(p);
   
   if (mp_iszero(r) || mp_iszero(s) || mp_cmp(r, p) != LTC_MP_LT || mp_cmp(s, p) != LTC_MP_LT)
   {
      err = CRYPT_INVALID_PACKET;
	  dprintf("r s check failure\n");
  //    goto error;
   }



   /* read hash */
   if ((err = mp_read_unsigned_bin(e, (unsigned char *)hash, (int)hashlen)) != CRYPT_OK)                { goto error; }


   /*  w  = s^-1 mod n */
   if ((err = mp_invmod(s, p, w)) != CRYPT_OK)                                                          { goto error; }


   /* u1 = ew */
   if ((err = mp_mulmod(e, w, p, u1)) != CRYPT_OK)                                                      { goto error; }


   /* u2 = rw */
   if ((err = mp_mulmod(r, w, p, u2)) != CRYPT_OK)                                                      { goto error; }




   /* find mG and mQ */

   if ((err = mp_read_radix(mG->x, (char *)key->dp->Gx, 16)) != CRYPT_OK)                               { goto error; }
   if ((err = mp_read_radix(mG->y, (char *)key->dp->Gy, 16)) != CRYPT_OK)                               { goto error; }
   if ((err = mp_set(mG->z, 1)) != CRYPT_OK)                                                            { goto error; }



   if ((err = mp_copy(key->pubkey.x, mQ->x)) != CRYPT_OK)                                               { goto error; }
   if ((err = mp_copy(key->pubkey.y, mQ->y)) != CRYPT_OK)                                               { goto error; }
   if ((err = mp_copy(key->pubkey.z, mQ->z)) != CRYPT_OK)                                               { goto error; }


   /* compute u1*mG + u2*mQ = mG */


	if(1)
	{
   		/* use Shamir's trick to compute u1*mG + u2*mQ using half of the doubles */
   		if ((err = ltc_ecc_mul2add_ntd(mG, u1, mQ, u2, mG, m, a)) != CRYPT_OK)                                { goto error; }
	}
	else
	{
	   //or:
	   if ((err = ltc_ecc_mulmod_ntd(u1, mG, mG, m,  a)) != CRYPT_OK)										 { goto error; }
	   if ((err = ltc_ecc_mulmod_ntd(u2, mQ, mQ, m,  a)) != CRYPT_OK)										 { goto error; }
	   
	   /* find the montgomery mp */
	   //if ((err = mp_montgomery_setup(m, &mp)) != CRYPT_OK) 											 { goto error; }
	   
	   /* add them */
	   if ((err = tfm_ecc_projective_add_point_ntd(mQ, mG, mG, m,  a)) != CRYPT_OK) 									 { goto error; }
	   
	   /* reduce */
	   //if ((err = ltc_mp.ecc_map(mG, m, mp)) != CRYPT_OK)	 
	}

   
   

   /* v = X_x1 mod n */
   if ((err = mp_mod(mG->x, p, v)) != CRYPT_OK)                                                         { goto error; }

   /* does v == r */
   dprintf("v r\n");
   mp_drawhex(v);
   mp_drawhex(r);

   
   if (mp_cmp(v, r) == LTC_MP_EQ) {
      *stat = 1;
   }

   /* clear up and return */
   err = CRYPT_OK;
error:
   ltc_ecc_del_point(mG);
   ltc_ecc_del_point(mQ);
   mp_clear_multi(r, s, v, w, u1, u2, p, e, m, a,  NULL);
   if (mp != NULL) { 
      //mp_montgomery_free(mp);
   }
   
   return err;
}















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
                        prng_state *prng, int wprng, custom_ecc_key *priv_key)
{
   /* temp store the k and kG*/ 
   custom_ecc_key		tempkey;

   int i = 0;
   unsigned char hash_emsa1[28]  = {0};
   int r_size = 0, s_size = 0;
   void 		 *r, *s, *e, *p;
   int			 err;

   LTC_ARGCHK(in	 != NULL);
   LTC_ARGCHK(out	 != NULL);
   LTC_ARGCHK(outlen != NULL);
   LTC_ARGCHK(priv_key	 != NULL);

   /* is this a private key? */
   if (priv_key->type != PK_PRIVATE) {
	  return CRYPT_PK_NOT_PRIVATE;
   }


   memset(out, 0, 56);
   /* is the IDX valid ?  */
  // if (ltc_ecc_is_valid_idx(key->idx) != 1) {
  //	return CRYPT_PK_INVALID_TYPE;
  // }
   
   if ((err = prng_is_valid(wprng)) != CRYPT_OK) {
	  return err;
   }

   /* get the hash and load it as a bignum into 'e' */
   /* init the bignums */
   if ((err = mp_init_multi(&r, &s, &p, &e, NULL)) != CRYPT_OK) { 
	  return err;
   }

   
   for(i=0; i<28; i++)
   {
	   hash_emsa1[i] = in[i]&0xff;
   }
   
   
#if 0     
   
   dprintf("hash_emsa1:\n");
   for(i=0; i<28; i++)
   {
	   dprintf(" %02x ",   hash_emsa1[i]&0xff);
   }
   
   dprintf("\n\n");
#endif

   if ((err = mp_read_radix(p, (char *)priv_key->dp->order, 16)) != CRYPT_OK)					  { goto errnokey; }
   if ((err = mp_read_unsigned_bin(e, (unsigned char *)hash_emsa1, 28)) != CRYPT_OK)		  { goto errnokey; }




   /* make up a key and export the public copy */
   for (;;) {

	  /* 1. select k:[1, n-1]*/
	  /* 2. Compute kP = (x1, y1) */

	  /* select a random integer k from [1, n-1]  ---> pubkey.k */
	  /* calculate the curve point (x1, y1) = kG   ---> pubkey.pubkey*/
	  if ((err = ecc_make_key_ex_general(prng, wprng, &tempkey, priv_key->dp)) != CRYPT_OK) {
		 goto errnokey;
	  }



	  /* find r = x1 mod n */
	  if ((err = mp_mod(tempkey.pubkey.x, p, r)) != CRYPT_OK)				  { goto error; }

	  if (mp_iszero(r) == LTC_MP_YES) {
		 ecc_my_free(&tempkey);
	  } else { 
		/* find s = (e + xr)/k */

		/* calculate 1/k */
		if ((err = mp_invmod(tempkey.k, p, tempkey.k)) != CRYPT_OK) 		   { goto error; } /* k = 1/k */

		/* calculate dA * r*/		
		if ((err = mp_mulmod(priv_key->k, r, p, s)) != CRYPT_OK) 				 { goto error; } /* s = xr */

		/* calculate dA * r + e */		
		if ((err = mp_add(e, s, s)) != CRYPT_OK)							 { goto error; } /* s = e +  xr */
		if ((err = mp_mod(s, p, s)) != CRYPT_OK)							 { goto error; } /* s = e +  xr */


		/* calcute (dA * r + e)/k  */		
		if ((err = mp_mulmod(s, tempkey.k, p, s)) != CRYPT_OK)				  { goto error; } /* s = (e + xr)/k */

		ecc_my_free(&tempkey);
		
		if (mp_iszero(s) == LTC_MP_NO) {
		   break;
		}
	  }
   }

#if 0
   dprintf("\n\nECC sign:\nr:   ");
   mp_drawhex(r);


   dprintf("s:");
   mp_drawhex(s);

   dprintf("\n");
#endif   

   //mp_to_unsigned_bin(r,out);
   //mp_to_unsigned_bin(s,out+28);


   r_size = mp_unsigned_bin_size(r);
   s_size = mp_unsigned_bin_size(s);

   //dprintf("r size:(%d)  s size(%d)\n", r_size, s_size);


   mp_to_unsigned_bin(r,out+(28-r_size));
   mp_to_unsigned_bin(s,out+28 + (28-s_size));

   *outlen = 56;



   
   //dump_hex_name("r,s-->", out, *outlen);

   
   goto errnokey;
error:
   ecc_my_free(&tempkey);
errnokey:
   mp_clear_multi(r, s, p, e, NULL);
   return err;	 
}





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
int ecc_verify_hash_emsa1_sha256_ex(const unsigned char *in_sig,  unsigned long siglen,
                    const unsigned char *hash,
                    int *stat, custom_ecc_key *key)
{
   ecc_point    *mG, *mQ;
   void          *r, *s, *v, *w, *u1, *u2, *e, *p, *m, *a, *b;
   void          *mp;
   int           err;
   unsigned char *sig;  


   //static int times = 0;

   int i = 0;

   unsigned char hash_emsa1[28] = {0x0};


   int r_s_len;


   LTC_ARGCHK(in_sig  != NULL);
   LTC_ARGCHK(hash != NULL);
   LTC_ARGCHK(stat != NULL);
   LTC_ARGCHK(key  != NULL);


   sig = malloc(siglen);
   if(NULL == sig)
   {
	   return CRYPT_MEM;
   }
   memcpy(sig, in_sig, siglen);

   /* default to invalid signature */
   *stat = 0;
   mp    = NULL;


   //times ++;

   //dprintf("`````````````````test time[%d]````````````````````````````````\n", times);

   /* allocate ints */
   if ((err = mp_init_multi(&r, &s, &v, &w, &u1, &u2, &p, &e, &m, &a, &b, NULL)) != CRYPT_OK) {
	  dprintf("no mem\n");
      return CRYPT_MEM;
   }

   /* allocate points */
   mG = ltc_ecc_new_point();
   mQ = ltc_ecc_new_point();
   if (mQ  == NULL || mG == NULL) {
      err = CRYPT_MEM;
	  dprintf("no mem\n");
      goto error;
   }


   /* siglen should not be odd */



   /*
   		fix:
   		sig(r, s)
 		init r, s
      */
     
	r_s_len = (siglen>>1);

	if(r_s_len == 0)
	{
	    dprintf("r_s_len = 0\n");
		goto error;
	}

	for(i=0; i<28; i++)
	{
		hash_emsa1[i] = hash[i]&0xff;
	}


	//dump_hex_name("emsa1(sha256):", hash_emsa1, sizeof(hash_emsa1));


	mp_read_unsigned_bin(r, sig, r_s_len);
	//dprintf("\nr:\n");
	//mp_drawhex(r);


	mp_read_unsigned_bin(s, sig+r_s_len, r_s_len);
	//dprintf("s:\n");
	//mp_drawhex(s);

	//dprintf("\n\n");



#if 0
	dprintf("pub key:\n");
	
	mp_drawhex(key->pubkey.x);
	mp_drawhex(key->pubkey.y);
	mp_drawhex(key->pubkey.z);

	dprintf("\n\n");
#endif




   /* get the order */
   if ((err = mp_read_radix(p, (char *)key->dp->order, 16)) != CRYPT_OK)                                { goto error; }

   /* get the modulus */
   if ((err = mp_read_radix(m, (char *)key->dp->prime, 16)) != CRYPT_OK)                                { goto error; }

   /* get the a */
   if ((err = mp_read_radix(a, (char *)key->dp->A, 16)) != CRYPT_OK)                                { goto error; }   


   /* get the b */
   if ((err = mp_read_radix(b, (char *)key->dp->B, 16)) != CRYPT_OK)      


   /* check for zero */
   if (mp_iszero(r) || mp_iszero(s) || mp_cmp(r, p) != LTC_MP_LT || mp_cmp(s, p) != LTC_MP_LT)
   {
      err = CRYPT_INVALID_PACKET;
	  dprintf("r s check failure\n");
      goto error;
   }


   //dprintf("setting ok\n");

   /* read hash */
   if ((err = mp_read_unsigned_bin(e, (unsigned char *)hash_emsa1, 28)) != CRYPT_OK)                { goto error; }


   //dprintf("e reading ok\n");


   /*  w  = s^-1 mod n */
   if ((err = mp_invmod(s, p, w)) != CRYPT_OK)                                                          { goto error; }
   //mp_copy(s, w);


   /* u1 = ew */
   if ((err = mp_mulmod(e, w, p, u1)) != CRYPT_OK)                                                      { goto error; }



   /* u2 = rw */
   if ((err = mp_mulmod(r, w, p, u2)) != CRYPT_OK)                                                      { goto error; }




   /* find mG and mQ */

   if ((err = mp_read_radix(mG->x, (char *)key->dp->Gx, 16)) != CRYPT_OK)                               { goto error; }
   if ((err = mp_read_radix(mG->y, (char *)key->dp->Gy, 16)) != CRYPT_OK)                               { goto error; }
   if ((err = mp_set(mG->z, 1)) != CRYPT_OK)                                                            { goto error; }




   if ((err = mp_copy(key->pubkey.x, mQ->x)) != CRYPT_OK)                                               { goto error; }
   if ((err = mp_copy(key->pubkey.y, mQ->y)) != CRYPT_OK)                                               { goto error; }
   if ((err = mp_copy(key->pubkey.z, mQ->z)) != CRYPT_OK)                                               { goto error; }

#if 0

	if ( 0 == ecc_verify_custom_curve_point(a, b,
								  &(key->pubkey),
								   m) )
	{
		dprintf("pub is on the curver\n");
	}
	else
	{
		dprintf("pub is NOT on the curver !!!\n");

	}
#endif

  	 /* compute u1*mG + u2*mQ = mG */

  	 /* use Shamir's trick to compute u1*mG + u2*mQ using half of the doubles */
   	if ((err = ltc_ecc_mul2add_ntd(mG, u1, mQ, u2, mG, m, a)) != CRYPT_OK)                                { goto error; }

   

   /* v = X_x1 mod n */
   if ((err = mp_mod(mG->x, p, v)) != CRYPT_OK)                                                         { goto error; }


#if 0
	   /* MG  on the curver ?*/

 	   dprintf("mG: \n");
	   mp_drawhex(mG->x);
	   mp_drawhex(mG->y);
	   mp_drawhex(mG->z);
	   dprintf("\n");
  if(0==  ecc_verify_custom_curve_point(a, b,
									 mG,
									  m))
      
   {
	   dprintf("mG is on the curver\n");
   }
   	else
   {
	   dprintf("mG  is NOT on the curver !!!\n");
   
   }
   
	  
#endif






	/* does v == r */
	if (mp_cmp(v, r) == LTC_MP_EQ) {
		*stat = 1;
		//dprintf("\n%s ok !!\n", __FUNCTION__);
	}
	else
	{
		//dprintf("\n\n\tv:\n");
		//mp_drawhex(v);

		//dprintf("\n\n\tr:\n");
		//mp_drawhex(r);   
		dprintf("\n%s failure not match!!\n", __FUNCTION__);

		err = CRYPT_ERROR;
		//while(1);
		goto error;
	}



   /* clear up and return */
   err = CRYPT_OK;
error:

   if(err != CRYPT_OK)
   {
	   dprintf("\n%s ERROR !!\n", __FUNCTION__);
   }
	
   ltc_ecc_del_point(mG);
   ltc_ecc_del_point(mQ);
   mp_clear_multi(r, s, v, w, u1, u2, p, e, m, a,  NULL);
   if (mp != NULL) { 
      //mp_montgomery_free(mp);
   }

   if(NULL != sig)
   {
		free(sig);
		sig = NULL;
   }
   
   return err;
}








/*
	ecc general tests based on sha256
*/
int ecc_general_tests (void)
{
  //unsigned char buf[4][4096];
  unsigned char sign[4096] = {0};
  unsigned long x;
  int           stat2;
  custom_ecc_key usera,  pubKey, privKey;
  
  unsigned char hash[SHA_LEN] = {0};
  hash_state md;
  prng_state l_prng;

  prng_state yarrow_prng;
  int err = -1;

  int i = 0;

  unsigned char ecc_msg[] = {
  
	  0x30, 0x82, 0x01, 0xb3, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x08, 0x29, 0xfd, 0x58, 0x8b, 0xea, 0xdd, 0xde, 0x64, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d,
	  0x04, 0x03, 0x02, 0x30, 0x2d, 0x31, 0x2b, 0x30, 0x29, 0x06, 0x0b, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x81, 0xb9, 0x2f, 0x01, 0x01, 0x01, 0x0c, 0x1a, 0x75, 0x72, 0x6e, 0x3a,
	  0x6d, 0x61, 0x72, 0x6c, 0x69, 0x6e, 0x3a, 0x70, 0x65, 0x65, 0x72, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x61, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x30, 0x1e, 0x17, 0x0d, 0x30, 0x37,
	  0x30, 0x31, 0x30, 0x39, 0x30, 0x33, 0x34, 0x38, 0x34, 0x33, 0x5a, 0x17, 0x0d, 0x32, 0x37, 0x30, 0x31, 0x30, 0x39, 0x30, 0x33, 0x34, 0x38, 0x34, 0x33, 0x5a, 0x30, 0x53,
	  0x31, 0x51, 0x30, 0x4f, 0x06, 0x0b, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x81, 0xb9, 0x2f, 0x01, 0x01, 0x01, 0x0c, 0x40, 0x75, 0x72, 0x6e, 0x3a, 0x6d, 0x61, 0x72, 0x6c, 0x69,
	  0x6e, 0x3a, 0x63, 0x6c, 0x69, 0x65, 0x6e, 0x74, 0x63, 0x61, 0x3a, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x74, 0x72, 0x75, 0x73, 0x74, 0x2d, 0x70, 0x64, 0x63, 0x2d, 0x63, 0x6c,
	  0x69, 0x65, 0x6e, 0x74, 0x2d, 0x63, 0x61, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x30,
	  0x49, 0x30, 0x0b, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x05, 0x00, 0x03, 0x3a, 0x00, 0x04, 0x00, 0x08, 0x14, 0x07, 0x88, 0x1c, 0x25, 0x3f, 0xb0, 0x76,
	  0xd7, 0x40, 0xbf, 0x73, 0x3a, 0x6f, 0x96, 0x39, 0x6b, 0xef, 0x78, 0x0e, 0x59, 0x1b, 0xaa, 0x7c, 0x2a, 0xc0, 0xa8, 0xa1, 0xe2, 0xde, 0x83, 0x96, 0x9e, 0x76, 0xa3, 0x25,
	  0x79, 0x1e, 0x2c, 0xb4, 0x19, 0x0b, 0x1a, 0xf5, 0x55, 0x8d, 0x38, 0x7c, 0x55, 0xc8, 0xe6, 0xa6, 0x6a, 0xaf, 0xa3, 0x81, 0xa6, 0x30, 0x81, 0xa3, 0x30, 0x1f, 0x06, 0x03,
	  0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 0x9d, 0xae, 0xe6, 0xfb, 0x9f, 0x45, 0x5d, 0xe0, 0xa0, 0x42, 0x19, 0x24, 0x7d, 0x07, 0x85, 0x95, 0xd3, 0xc0, 0x6d,
	  0xe6, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0xd6, 0xe1, 0x78, 0x3f, 0xf4, 0x22, 0xee, 0xb7, 0xc5, 0x19, 0x57, 0x58, 0x75, 0x84, 0xfb, 0xdf,
	  0x4f, 0xba, 0xac, 0xf4, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01, 0x01, 0xff, 0x04, 0x04, 0x03, 0x02, 0x02, 0x04, 0x30, 0x0f, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01,
	  0x01, 0xff, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xff, 0x30, 0x40, 0x06, 0x03, 0x55, 0x1d, 0x1f, 0x04, 0x39, 0x30, 0x37, 0x30, 0x35, 0xa0, 0x33, 0xa0, 0x31, 0x86, 0x2f,
	  0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x69, 0x70, 0x74, 0x76, 0x2d, 0x63, 0x72, 0x6c, 0x2e, 0x6d, 0x61, 0x72, 0x6c, 0x69, 0x6e, 0x2d, 0x74, 0x6d, 0x6f, 0x2e, 0x63,
	  0x6f, 0x6d, 0x2f, 0x63, 0x72, 0x6c, 0x2f, 0x69, 0x70, 0x74, 0x76, 0x63, 0x72, 0x6c, 0x73, 0x2e, 0x63, 0x72, 0x6c
  };
  
  







  /* rand number engine init */
  register_prng(&yarrow_desc);
  if ((err = rng_make_prng(224, find_prng("yarrow"), &yarrow_prng, NULL)) != CRYPT_OK) {
	 dprintf("rng_make_prng failed: %s\n", error_to_string(err));
	 return -1;
  }
	  
  
  if(register_hash(&sha256_desc)<0){
	  dprintf("%s: register_hash fail\n", __FUNCTION__);
	  return -1;
  }
  
	  
  
  /* start it */
  if ((err = yarrow_start(&l_prng)) != CRYPT_OK) {
	  dprintf("yarrow_starterror: %s\n", error_to_string(err));
  }
  
  


  //int size = 28;//

  //for (s = 0; s < (sizeof(sizes)/sizeof(sizes[0])); s++) 

  {
     /* make up keys:  private key d, and public key Q (Q= dp = dG)*/
     DO(ecc_make_key_general (&yarrow_prng, find_prng ("yarrow"),  &usera, &sony_ecc_para));
	 dprintf("ecc_make_key_general usera done\n");


	dprintf("msg len:%lu\n" ,sizeof(ecc_msg));
	dump_hex_name("msg:", ecc_msg, sizeof(ecc_msg));

    
    tom_sha256_init(&md);
    sha256_process(&md, (unsigned char*)ecc_msg, sizeof(ecc_msg));
    sha256_done(&md, hash);

	dprintf("\n");
	dump_hex_name("msg sha256:", hash, SHA_LEN);

	/* sign(r,s ): */

	/* sha256: */

	for(i=0; i<20; i++)
	{
		 dprintf("\n\necc test times[%d]:\n", i);

		 /* priv: */
		 dprintf("\n\n\npriv:\n\t");
		 mp_drawhex(usera.k);

		 x = sizeof(sign);
		 ecc_sign_hash_general (hash, SHA_LEN, sign, &x, &yarrow_prng, find_prng ("yarrow"), &usera);

		 
		 /* pub: */  
		 dprintf("\npub:\n");
		 dprintf("\tx:  ");
		 mp_drawhex(usera.pubkey.x);
		 dprintf("\ty:  ");
		 mp_drawhex(usera.pubkey.y);
		 dprintf("\tz:  ");
		 mp_drawhex(usera.pubkey.z);

		 //dump_hex_name("deque (r,s):", buf[1], x);
		 stat2 = 0;

		 DO(ecc_verify_hash_general (sign, sizeof(sign), hash, SHA_LEN, &stat2, &usera));
			 if ( stat2 == 0) { 
				dprintf("\necc_verify_hash failed %d, ", stat2);
				return 1;
			 }
			 else
			 {
				dprintf("\n%s ecc test times[%d] ok\n", __FUNCTION__, i);
			 
			 }


		dprintf("\n\n\n\n");

  	}

	 
     ecc_my_free (&usera); 
     ecc_my_free (&pubKey);
     ecc_my_free (&privKey);
  }

  return 0;

}







int ecc_verify_hash_emsa1_sha256(	
			const unsigned char  		*in_public_key_xy,
			const unsigned char 		*msg, 
			int 				        msg_len,
			const unsigned char		*expected_sign,
			custom_ecc_set_type      *ecc_set
		  )
{
	int err = -1;

	custom_ecc_key key;
	int stat = -1;

    hash_state md;	
	unsigned char hash[SHA_LEN] = {0};
	

	if(NULL == in_public_key_xy || NULL == msg || NULL == ecc_set || NULL == expected_sign)
	{
		dprintf("%s invalid parameters\n", __FUNCTION__);


		dprintf("pub:%p\n", in_public_key_xy);
		dprintf("msg:%p\n", msg);
		dprintf("ecc_set:%p\n", ecc_set);
		dprintf("expected_sign:%p\n", expected_sign);

		
		return -1;
	}


//	ltc_mp = tfm_desc;


	ecc_make_keypub(in_public_key_xy, 28, &key, ecc_set);

 	//dump_hex_name("msg:", msg, msg_len);

	memset(hash, 0, sizeof(SHA_LEN));
    /*  msg sha256 */
    tom_sha256_init(&md);
    sha256_process(&md,  msg, msg_len);
    sha256_done(&md, hash);


	err = ecc_verify_hash_emsa1_sha256_ex(expected_sign,
		56,
		hash,
		&stat,
		&key);

	//dprintf("err:%d stat:%d\n\n\n\n", err, stat);

	if(0!=err || 1 != stat)
	{
		return CRYPT_ERROR;
	}

	return CRYPT_OK;
}







int ecc_sign_hash_emsa1_sha256(	
			const unsigned char  		*in_priv_key_xy,
//			int					 priv_key_len,
			const unsigned char 		*msg, 
			int 				 msg_len,
			unsigned char		*out_sign,
			unsigned int   	   out_sign_len,
			custom_ecc_set_type *ecc_set,
			unsigned char       *export_pubkey
		  )
{
	int err = -1;

	custom_ecc_key key;
	prng_state l_prng;

	int x_size = 0, y_size = 0;
    hash_state md;	
	unsigned char hash[SHA_LEN] = {0};

	prng_state yarrow_prng;	

	if(NULL == in_priv_key_xy || NULL == msg || NULL == ecc_set || NULL == out_sign)
	{
		return -1;
	}

	memset(export_pubkey, 0, 56);

//	ltc_mp = tfm_desc;

	/* rand number engine init */
	register_prng(&yarrow_desc);
	if ((err = rng_make_prng(CRYPTO_ECDH_PRIVKEY_LEN*8, find_prng("yarrow"), &yarrow_prng, NULL)) != CRYPT_OK) {
	   dprintf("rng_make_prng failed: %s\n", error_to_string(err));
	   return -1;
	}
		
	
	if(register_hash(&sha256_desc)<0){
		dprintf("%s: register_hash fail\n", __FUNCTION__);
		return -1;
	}
	
		
	
	/* start it */
	if ((err = yarrow_start(&l_prng)) != CRYPT_OK) {
		dprintf("yarrow_starterror: %s\n", error_to_string(err));
	}
	

	
	if( CRYPT_OK != ecc_make_keypriv(in_priv_key_xy, &key, ecc_set))
	{
		dprintf("\tmake ecc priv key error\n");
		return err;
	}
	else
	{
		//dprintf("\tmake ecc priv key ok\n");
	}


	if(NULL != export_pubkey)
	{


		//mp_to_unsigned_bin(key.pubkey.x, export_pubkey);
		//mp_to_unsigned_bin(key.pubkey.y, export_pubkey+key.dp->size);

		x_size = mp_unsigned_bin_size(key.pubkey.x);
		y_size = mp_unsigned_bin_size(key.pubkey.y);

		//dprintf("x_size:(%d)  y_size(%d)\n", x_size, y_size);

		mp_to_unsigned_bin(key.pubkey.x, export_pubkey+(key.dp->size-x_size));
		mp_to_unsigned_bin(key.pubkey.y, export_pubkey+key.dp->size + (key.dp->size-y_size));



		//dump_hex_name("\t\nexport pubkey:", export_pubkey, 2*key.dp->size);
		
	}


 	//dump_hex_name("msg:", msg, msg_len);

	memset(hash, 0, SHA_LEN);
    /*  msg sha256 */
    tom_sha256_init(&md);
    sha256_process(&md,  msg, msg_len);
    sha256_done(&md, hash);


//	dump_hex_name("\t\nhash sha256:", hash, SHA_LEN);

	out_sign_len = 56;

	err = ecc_sign_hash_emsa1_sha256_ex ( hash, 
										  out_sign, 
										  &out_sign_len, 
										  &yarrow_prng, 
										  find_prng ("yarrow"), 
										  &key);


	//dprintf("\terr:%d stat:%d\n\n\n\n", err, stat);


	
	return err;
}













/*
	ecc general tests based on sha256
*/
int ecc_emsa_sha256_tests (void)
{

	unsigned char sign[4096] = {0};
	unsigned int  out_sign_len;
	//int           stat, stat2;


	//int hash[SHA_LEN] = {0};
	//hash_state md;

	
	prng_state yarrow_prng;

	int err = -1;

	int i = 0;

	unsigned char ecc_msg[] = {

								0x30, 0x82, 0x01, 0xb3, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x08, 0x29, 0xfd, 0x58, 0x8b, 0xea, 0xdd, 0xde, 0x64, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d,
								0x04, 0x03, 0x02, 0x30, 0x2d, 0x31, 0x2b, 0x30, 0x29, 0x06, 0x0b, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x81, 0xb9, 0x2f, 0x01, 0x01, 0x01, 0x0c, 0x1a, 0x75, 0x72, 0x6e, 0x3a,
								0x6d, 0x61, 0x72, 0x6c, 0x69, 0x6e, 0x3a, 0x70, 0x65, 0x65, 0x72, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x61, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x30, 0x1e, 0x17, 0x0d, 0x30, 0x37,
								0x30, 0x31, 0x30, 0x39, 0x30, 0x33, 0x34, 0x38, 0x34, 0x33, 0x5a, 0x17, 0x0d, 0x32, 0x37, 0x30, 0x31, 0x30, 0x39, 0x30, 0x33, 0x34, 0x38, 0x34, 0x33, 0x5a, 0x30, 0x53,
								0x31, 0x51, 0x30, 0x4f, 0x06, 0x0b, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x81, 0xb9, 0x2f, 0x01, 0x01, 0x01, 0x0c, 0x40, 0x75, 0x72, 0x6e, 0x3a, 0x6d, 0x61, 0x72, 0x6c, 0x69,
								0x6e, 0x3a, 0x63, 0x6c, 0x69, 0x65, 0x6e, 0x74, 0x63, 0x61, 0x3a, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x74, 0x72, 0x75, 0x73, 0x74, 0x2d, 0x70, 0x64, 0x63, 0x2d, 0x63, 0x6c,
								0x69, 0x65, 0x6e, 0x74, 0x2d, 0x63, 0x61, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x30,
								0x49, 0x30, 0x0b, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x05, 0x00, 0x03, 0x3a, 0x00, 0x04, 0x00, 0x08, 0x14, 0x07, 0x88, 0x1c, 0x25, 0x3f, 0xb0, 0x76,
								0xd7, 0x40, 0xbf, 0x73, 0x3a, 0x6f, 0x96, 0x39, 0x6b, 0xef, 0x78, 0x0e, 0x59, 0x1b, 0xaa, 0x7c, 0x2a, 0xc0, 0xa8, 0xa1, 0xe2, 0xde, 0x83, 0x96, 0x9e, 0x76, 0xa3, 0x25,
								0x79, 0x1e, 0x2c, 0xb4, 0x19, 0x0b, 0x1a, 0xf5, 0x55, 0x8d, 0x38, 0x7c, 0x55, 0xc8, 0xe6, 0xa6, 0x6a, 0xaf, 0xa3, 0x81, 0xa6, 0x30, 0x81, 0xa3, 0x30, 0x1f, 0x06, 0x03,
								0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 0x9d, 0xae, 0xe6, 0xfb, 0x9f, 0x45, 0x5d, 0xe0, 0xa0, 0x42, 0x19, 0x24, 0x7d, 0x07, 0x85, 0x95, 0xd3, 0xc0, 0x6d,
								0xe6, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0xd6, 0xe1, 0x78, 0x3f, 0xf4, 0x22, 0xee, 0xb7, 0xc5, 0x19, 0x57, 0x58, 0x75, 0x84, 0xfb, 0xdf,
								0x4f, 0xba, 0xac, 0xf4, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01, 0x01, 0xff, 0x04, 0x04, 0x03, 0x02, 0x02, 0x04, 0x30, 0x0f, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01,
								0x01, 0xff, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xff, 0x30, 0x40, 0x06, 0x03, 0x55, 0x1d, 0x1f, 0x04, 0x39, 0x30, 0x37, 0x30, 0x35, 0xa0, 0x33, 0xa0, 0x31, 0x86, 0x2f,
								0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x69, 0x70, 0x74, 0x76, 0x2d, 0x63, 0x72, 0x6c, 0x2e, 0x6d, 0x61, 0x72, 0x6c, 0x69, 0x6e, 0x2d, 0x74, 0x6d, 0x6f, 0x2e, 0x63,
								0x6f, 0x6d, 0x2f, 0x63, 0x72, 0x6c, 0x2f, 0x69, 0x70, 0x74, 0x76, 0x63, 0x72, 0x6c, 0x73, 0x2e, 0x63, 0x72, 0x6c
	};


	unsigned char priv[CRYPTO_ECDSA_PRIVKEY_LEN] =   {0x0};
//	void *k;
	void *order;
	unsigned char pubxy[CRYPTO_ECDSA_PUBKEY_LEN] = {0x0};
//	unsigned char buf[CRYPTO_ECDSA_PRIVKEY_LEN] = {0};
	

	/* rand number engine init */
	register_prng(&yarrow_desc);
	if ((err = rng_make_prng(CRYPTO_ECDH_PRIVKEY_LEN*8, find_prng("yarrow"), &yarrow_prng, NULL)) != CRYPT_OK) {
	   dprintf("rng_make_prng failed: %s\n", error_to_string(err));
	   return -1;
	}
		
	
	if(register_hash(&sha256_desc)<0){
		dprintf("%s: register_hash fail\n", __FUNCTION__);
		return -1;
	}
	
	if(register_hash(&sha256_desc)<0){
		dprintf("%s: register_hash fail\n", __FUNCTION__);
		return -1;
	}

		
	
	/* start it */
	if ((err = yarrow_start(&yarrow_prng)) != CRYPT_OK) {
		dprintf("yarrow_starterror: %s\n", error_to_string(err));
	}


	mp_init(&order);
	mp_read_radix(order,  sony_ecc_para.order , 16);
	dprintf("order:");
	mp_drawhex(order);

	for(i=0; i<10000; i++)	
	{
			
			err = yarrow_read(priv, CRYPTO_ECDSA_PRIVKEY_LEN, &yarrow_prng);
			//dprintf("%s rand number engine init done[%d]\n", __FUNCTION__,	err);


			

			
			dprintf(">>%s\n", __FUNCTION__);

			dump_hex_name("[1]random priv generated:", priv, CRYPTO_ECDSA_PRIVKEY_LEN);


			out_sign_len = 56;



			err = ecc_sign_hash_emsa1_sha256(priv,
										//28,
										ecc_msg,
										sizeof(ecc_msg),
										sign,
										out_sign_len,
										&sony_ecc_para,
										pubxy);
			dprintf("\n[2]sign the message [%d]", err);
			dump_hex_name("", sign, out_sign_len);





			dprintf("\n\nnow verify it:\n");
			DO(ecc_verify_hash_emsa1_sha256(pubxy, 
											//28,
											ecc_msg, 
											sizeof(ecc_msg), 
											sign, 
											//out_sign_len, 
											&sony_ecc_para));


			dprintf("\n\n\n\n");

	}

  return 0;

}


