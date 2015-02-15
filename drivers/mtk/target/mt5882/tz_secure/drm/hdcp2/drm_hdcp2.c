#include "tzst.h"

#include "tz_dmx_if.h"

#include "tomcrypt.h"
#include "drm_hdcp2.h"
#include "x_os.h"

#include "tee_securestorage.h"




typedef struct
{
    UINT8 au1LC128[LC128_SIZE];
}HDCP2_KEYLC128_T;


typedef struct
{
    UINT8 au1RxID[KEYLEN_RXID];
    UINT8 au1Public[KEYLEN_PUBLIC];
    UINT8 au1Reserved[KEYLEN_RESERVED];
    UINT8 au1RootSign[KEYLEN_ROOTSIGN];
}HDCP2_KEYCERT_T;

typedef struct
{
    UINT8 au1P[KEYLEN_P];
    UINT8 au1Q[KEYLEN_Q];
    UINT8 au1DP[KEYLEN_DP];
    UINT8 au1DQ[KEYLEN_DQ];
    UINT8 au1QINV[KEYLEN_QINV];
#ifndef A2_KEY_STRUCTURE
    UINT8 au1sha1[20];
#endif
}HDCP2_KEYPRIT_T;




typedef struct
{
#if 1 //#ifdef HDCP_A2_KEY_STRUCTURE
    HDCP2_KEYLC128_T rKeyLc128;
#endif
    HDCP2_KEYCERT_T rKeyCert;
    HDCP2_KEYPRIT_T rKeyPrit;
}HDCP2_KEYDECDCP_T;



#define ENCRYPTION_KEYSET_LENGTH (864+16)      
#define ENCRYPTION_KEYSET_BLOCKS (ENCRYPTION_KEYSET_LENGTH / 16)
#define ENCRYPTION_KEYSET_SIZE (ENCRYPTION_KEYSET_BLOCKS * 16)

/******************************/



#ifdef WFD_HDCP_USE_TEST_VECTOR
#define DDI_TEST 1
#endif

#define ENABLE_HDCP2X_TX (0)
#define ENABLE_DRV_HDCP2_DBG

#define DRV_HDCP2_DBG_ERR       1
#define DRV_HDCP2_DBG_WARN      2
#define DRV_HDCP2_DBG_INFO      4
#define DRV_HDCP2_DBG_VERB      8


#define HDCP_DUMP_KEY (1)

#ifdef ENABLE_DRV_HDCP2_DBG
static int drv_hdcp2_dbglvl = (DRV_HDCP2_DBG_ERR | DRV_HDCP2_DBG_WARN | DRV_HDCP2_DBG_INFO);

#define DRV_HDCP2_DBG(lvl,x...) do{if(drv_hdcp2_dbglvl&lvl) dprintf("~~~~~~~\n[DRV_HDCP2]" x);}while(0)
#define DRV_HDCP2_DUMP16(lvl,x,y,idx) \
                            do{\
                                if (drv_hdcp2_dbglvl&lvl) \
                                    dprintf("~~~~~~\n[DRV_HDCP2]" x \
                                        "=[%02x %02x %02x %02x %02x %02x %02x %02x" \
                                        " %02x %02x %02x %02x %02x %02x %02x %02x]\n", \
                                        y[idx],y[idx+1],y[idx+2],y[idx+3],y[idx+4],y[idx+5],y[idx+6],y[idx+7], \
                                        y[idx+8],y[idx+9],y[idx+10],y[idx+11],y[idx+12],y[idx+13],y[idx+14],y[idx+15]); \
                            }while(0)
                            
#define DRV_HDCP2_DUMP8(lvl,x,y,idx) \
                            do{\
                                if (drv_hdcp2_dbglvl&lvl) \
                                    dprintf("~~~~~~\n[DRV_HDCP2]" x \
                                           "=[%02x %02x %02x %02x %02x %02x %02x %02x]\n", \
                                           y[idx],y[idx+1],y[idx+2],y[idx+3],y[idx+4],y[idx+5],y[idx+6],y[idx+7]); \
                            }while(0)
#else
#define DRV_HDCP2_DBG(lvl,x...) do{;}while(0)
#define DRV_HDCP2_DUMP16(lvl,x,y,idx) do{;} while(0)
#define DRV_HDCP2_DUMP8(lvl,x,y,idx) do{;} while(0)
#endif




/* for HDCP2.X ALIGN */
enum
{
    HDCP2_KEY_SET_TYPE_NORMAL = 0,
	HDCP2_KEY_SET_TYPE_SONY_2K14,	
    HDCP2_KEY_SET_TYPE_PHILIPS_2K13,
    HDCP2_KEY_SET_TYPE_PHILIPS_2K14,
    HDCP2_KEY_SET_TYPE_KM,
    HDCP2_KEY_SET_TYPE_MAX
};

#define HDCP2_KEYSET_LEN_MAX 1019
typedef struct
{
    unsigned char au1EncKeySet[HDCP2_KEYSET_LEN_MAX];
	unsigned char au1KeyType;
	unsigned int au1KeySize;
	
}HDCP2_KEYSET_ENC_T;


unsigned char key_p[KEY_P_SIZE] = {0xff};
unsigned char key_q[KEY_Q_SIZE] = {0xff};
unsigned char key_dP[KEY_DP_SIZE] = {0xff};
unsigned char key_dQ[KEY_DQ_SIZE] = {0xff};
unsigned char key_qInv[KEY_QINV_SIZE] = {0xff};


unsigned char lc128[KEYLEN_LC128] = {0xff};
unsigned char key_DecDcp[KEYLEN_ENCDCP] = {0xff};
unsigned char repeater = 0x00;

unsigned char km[KM_SIZE] = {0xff};



unsigned char facskey_kpubdcp_hdcp2_2[384] =
{
	0xA2, 0xC7, 0x55, 0x57, 0x54, 0xCB, 0xAA, 0xA7, 0x7A,
	0x27, 0x92, 0xC3, 0x1A, 0x6D, 0xC2, 0x31, 0xCF, 0x12,
	0xC2, 0x24, 0xBF, 0x89, 0x72, 0x46, 0xA4, 0x8D, 0x20,
	0x83, 0xB2, 0xDD, 0x04, 0xDA, 0x7E, 0x01, 0xA9, 0x19,
	0xEF, 0x7E, 0x8C, 0x47, 0x54, 0xC8, 0x59, 0x72, 0x5C,
	0x89, 0x60, 0x62, 0x9F, 0x39, 0xD0, 0xE4, 0x80, 0xCA,
	0xA8, 0xD4, 0x1E, 0x91, 0xE3, 0x0E, 0x2C, 0x77, 0x55,
	0x6D, 0x58, 0xA8, 0x9E, 0x3E, 0xF2, 0xDA, 0x78, 0x3E,
	0xBA, 0xD1, 0x05, 0x37, 0x07, 0xF2, 0x88, 0x74, 0x0C,
	0xBC, 0xFB, 0x68, 0xA4, 0x7A, 0x27, 0xAD, 0x63, 0xA5,
	0x1F, 0x67, 0xF1, 0x45, 0x85, 0x16, 0x49, 0x8A, 0xE6,
	0x34, 0x1C, 0x6E, 0x80, 0xF5, 0xFF, 0x13, 0x72, 0x85,
	0x5D, 0xC1, 0xDE, 0x5F, 0x01, 0x86, 0x55, 0x86, 0x71,
	0xE8, 0x10, 0x33, 0x14, 0x70, 0x2A, 0x5F, 0x15, 0x7B,
	0x5C, 0x65, 0x3C, 0x46, 0x3A, 0x17, 0x79, 0xED, 0x54,
	0x6A, 0xA6, 0xC9, 0xDF, 0xEB, 0x2A, 0x81, 0x2A, 0x80,
	0x2A, 0x46, 0xA2, 0x06, 0xDB, 0xFD, 0xD5, 0xF3, 0xCF,
	0x74, 0xBB, 0x66, 0x56, 0x48, 0xD7, 0x7C, 0x6A, 0x03,
	0x14, 0x1E, 0x55, 0x56, 0xE4, 0xB6, 0xFA, 0x38, 0x2B,
	0x5D, 0xFB, 0x87, 0x9F, 0x9E, 0x78, 0x21, 0x87, 0xC0,
	0x0C, 0x63, 0x3E, 0x8D, 0x0F, 0xE2, 0xA7, 0x19, 0x10,
	0x9B, 0x15, 0xE1, 0x11, 0x87, 0x49, 0x33, 0x49, 0xB8,
	0x66, 0x32, 0x28, 0x7C, 0x87, 0xF5, 0xD2, 0x2E, 0xC5,
	0xF3, 0x66, 0x2F, 0x79, 0xEF, 0x40, 0x5A, 0xD4, 0x14,
	0x85, 0x74, 0x5F, 0x06, 0x43, 0x50, 0xCD, 0xDE, 0x84,
	0xE7, 0x3C, 0x7D, 0x8E, 0x8A, 0x49, 0xCC, 0x5A, 0xCF,
	0x73, 0xA1, 0x8A, 0x13, 0xFF, 0x37, 0x13, 0x3D, 0xAD,
	0x57, 0xD8, 0x51, 0x22, 0xD6, 0x32, 0x1F, 0xC0, 0x68,
	0x4C, 0xA0, 0x5B, 0xDD, 0x5F, 0x78, 0xC8, 0x9F, 0x2D,
	0x3A, 0xA2, 0xB8, 0x1E, 0x4A, 0xE4, 0x08, 0x55, 0x64,
	0x05, 0xE6, 0x94, 0xFB, 0xEB, 0x03, 0x6A, 0x0A, 0xBE,
	0x83, 0x18, 0x94, 0xD4, 0xB6, 0xC3, 0xF2, 0x58, 0x9C,
	0x7A, 0x24, 0xDD, 0xD1, 0x3A, 0xB7, 0x3A, 0xB0, 0xBB,
	0xE5, 0xD1, 0x28, 0xAB, 0xAD, 0x24, 0x54, 0x72, 0x0E,
	0x76, 0xD2, 0x89, 0x32, 0xEA, 0x46, 0xD3, 0x78, 0xD0,
	0xA9, 0x67, 0x78, 0xC1, 0x2D, 0x18, 0xB0, 0x33, 0xDE,
	0xDB, 0x27, 0xCC, 0xB0, 0x7C, 0xC9, 0xA4, 0xBD, 0xDF,
	0x2B, 0x64, 0x10, 0x32, 0x44, 0x06, 0x81, 0x21, 0xB3,
	0xBA, 0xCF, 0x33, 0x85, 0x49, 0x1E, 0x86, 0x4C, 0xBD,
	0xF2, 0x3D, 0x34, 0xEF, 0xD6, 0x23, 0x7A, 0x9F, 0x2C,
	0xDA, 0x84, 0xF0, 0x83, 0x83, 0x71, 0x7D, 0xDA, 0x6E,
	0x44, 0x96, 0xCD, 0x1D, 0x05, 0xDE, 0x30, 0xF6, 0x1E,
	0x2F, 0x9C, 0x99, 0x9C, 0x60, 0x07
};



unsigned char facskey_km_hdcp2_2_r1[16] =
{
	0x68, 0xbc, 0xc5, 0x1b, 0xa9, 0xdb, 0x1b, 0xd0, 
	0xfa, 0xf1, 0x5e, 0x9a, 0xd8, 0xa5, 0xaf, 0xb9
};



unsigned char facskey_kpubrx_n_hdcp2_2_r1[128] =
{
    0xaf, 0xb5, 0xc5, 0xc6, 0x7b, 0xc5,
		0x3a, 0x34, 0x90, 0xa9, 0x54, 0xc0,
		0x8f, 0xb7, 0xeb, 0xa1, 0x54, 0xd2,
		0x4f, 0x22, 0xde, 0x83, 0xf5, 0x03,
		0xa6, 0xc6, 0x68, 0x46, 0x9b, 0xc0,
		0xb8, 0xc8, 0x6c, 0xdb, 0x26, 0xf9,
		0x3c, 0x49, 0x2f, 0x02, 0xe1, 0x71,
		0xdf, 0x4e, 0xf3, 0x0e, 0xc8, 0xbf,
		0x22, 0x9d, 0x04, 0xcf, 0xbf, 0xa9,
		0x0d, 0xff, 0x68, 0xab, 0x05, 0x6f,
		0x1f, 0x12, 0x8a, 0x68, 0x62, 0xeb,
		0xfe, 0xc9, 0xea, 0x9f, 0xa7, 0xfb,
		0x8c, 0xba, 0xb1, 0xbd, 0x65, 0xac,
		0x35, 0x9c, 0xa0, 0x33, 0xb1, 0xdd,
		0xa6, 0x05, 0x36, 0xaf, 0x00, 0xa2,
		0x7f, 0xbc, 0x07, 0xb2, 0xdd, 0xb5,
		0xcc, 0x57, 0x5c, 0xdc, 0xc0, 0x95,
		0x50, 0xe5, 0xff, 0x1f, 0x20, 0xdb,
		0x59, 0x46, 0xfa, 0x47, 0xc4, 0xed,
		0x12, 0x2e, 0x9e, 0x22, 0xbd, 0x95,
		0xa9, 0x85, 0x59, 0xa1, 0x59, 0x3c,
		0xc7, 0x83
};

unsigned char facskey_kpubrx_e_hdcp2_2_r1[3] =
{
	0x01, 0x00, 0x01
};






unsigned char hdcp_2_2_R1_test_key_cert[KEYLEN_CERTRX] = {
		0x74, 0x5b, 0xb8, 0xbd, 0x04, 0xaf, 0xb5, 0xc5, 0xc6, 0x7b, 0xc5, 0x3a, 0x34, 0x90,
		0xa9, 0x54, 0xc0, 0x8f, 0xb7, 0xeb, 0xa1, 0x54, 0xd2, 0x4f, 0x22, 0xde, 0x83, 0xf5,
		0x03, 0xa6, 0xc6, 0x68, 0x46, 0x9b, 0xc0, 0xb8, 0xc8, 0x6c, 0xdb, 0x26, 0xf9, 0x3c,
		0x49, 0x2f, 0x02, 0xe1, 0x71, 0xdf, 0x4e, 0xf3, 0x0e, 0xc8, 0xbf, 0x22, 0x9d, 0x04,
		0xcf, 0xbf, 0xa9, 0x0d, 0xff, 0x68, 0xab, 0x05, 0x6f, 0x1f, 0x12, 0x8a, 0x68, 0x62,
		0xeb, 0xfe, 0xc9, 0xea, 0x9f, 0xa7, 0xfb, 0x8c, 0xba, 0xb1, 0xbd, 0x65, 0xac, 0x35,
		0x9c, 0xa0, 0x33, 0xb1, 0xdd, 0xa6, 0x05, 0x36, 0xaf, 0x00, 0xa2, 0x7f, 0xbc, 0x07,
		0xb2, 0xdd, 0xb5, 0xcc, 0x57, 0x5c, 0xdc, 0xc0, 0x95, 0x50, 0xe5, 0xff, 0x1f, 0x20,
		0xdb, 0x59, 0x46, 0xfa, 0x47, 0xc4, 0xed, 0x12, 0x2e, 0x9e, 0x22, 0xbd, 0x95, 0xa9,
		0x85, 0x59, 0xa1, 0x59, 0x3c, 0xc7, 0x83, 0x01, 0x00, 0x01, 0x10, 0x00, 0x0b, 0xa3,
		0x73, 0x77, 0xdd, 0x03, 0x18, 0x03, 0x8a, 0x91, 0x63, 0x29, 0x1e, 0xa2, 0x95, 0x74,
		0x42, 0x90, 0x78, 0xd0, 0x67, 0x25, 0xb6, 0x32, 0x2f, 0xcc, 0x23, 0x2b, 0xad, 0x21,
		0x39, 0x3d, 0x14, 0xba, 0x37, 0xa3, 0x65, 0x14, 0x6b, 0x9c, 0xcf, 0x61, 0x20, 0x44,
		0xa1, 0x07, 0xbb, 0xcf, 0xc3, 0x4e, 0x95, 0x5b, 0x10, 0xcf, 0xc7, 0x6f, 0xf1, 0xc3,
		0x53, 0x7c, 0x63, 0xa1, 0x8c, 0xb2, 0xe8, 0xab, 0x2e, 0x96, 0x97, 0xc3, 0x83, 0x99,
		0x70, 0xd3, 0xdc, 0x21, 0x41, 0xf6, 0x0a, 0xd1, 0x1a, 0xee, 0xf4, 0xcc, 0xeb, 0xfb,
		0xa6, 0xaa, 0xb6, 0x9a, 0xaf, 0x1d, 0x16, 0x5e, 0xe2, 0x83, 0xa0, 0x4a, 0x41, 0xf6,
		0x7b, 0x07, 0xbf, 0x47, 0x85, 0x28, 0x6c, 0xa0, 0x77, 0xa6, 0xa3, 0xd7, 0x85, 0xa5,
		0xc4, 0xa7, 0xe7, 0x6e, 0xb5, 0x1f, 0x40, 0x72, 0x97, 0xfe, 0xc4, 0x81, 0x23, 0xa0,
		0xc2, 0x90, 0xb3, 0x49, 0x24, 0xf5, 0xb7, 0x90, 0x2c, 0xbf, 0xfe, 0x04, 0x2e, 0x00,
		0xa9, 0x5f, 0x86, 0x04, 0xca, 0xc5, 0x3a, 0xcc, 0x26, 0xd9, 0x39, 0x7e, 0xa9, 0x2d,
		0x28, 0x6d, 0xc0, 0xcc, 0x6e, 0x81, 0x9f, 0xb9, 0xb7, 0x11, 0x33, 0x32, 0x23, 0x47,
		0x98, 0x43, 0x0d, 0xa5, 0x1c, 0x59, 0xf3, 0xcd, 0xd2, 0x4a, 0xb7, 0x3e, 0x69, 0xd9,
		0x21, 0x53, 0x9a, 0xf2, 0x6e, 0x77, 0x62, 0xae, 0x50, 0xda, 0x85, 0xc6, 0xaa, 0xc4,
		0xb5, 0x1c, 0xcd, 0xa8, 0xa5, 0xdd, 0x6e, 0x62, 0x73, 0xff, 0x5f, 0x7b, 0xd7, 0x3c,
		0x17, 0xba, 0x47, 0x0c, 0x89, 0x0e, 0x62, 0x79, 0x43, 0x94, 0xaa, 0xa8, 0x47, 0xf4,
		0x4c, 0x38, 0x89, 0xa8, 0x81, 0xad, 0x23, 0x13, 0x27, 0x0c, 0x17, 0xcf, 0x3d, 0x83,
		0x84, 0x57, 0x36, 0xe7, 0x22, 0x26, 0x2e, 0x76, 0xfd, 0x56, 0x80, 0x83, 0xf6, 0x70,
		0xd4, 0x5c, 0x91, 0x48, 0x84, 0x7b, 0x18, 0xdb, 0x0e, 0x15, 0x3b, 0x49, 0x26, 0x23,
		0xe6, 0xa3, 0xe2, 0xc6, 0x3a, 0x23, 0x57, 0x66, 0xb0, 0x72, 0xb8, 0x12, 0x17, 0x4f,
		0x86, 0xfe, 0x48, 0x0d, 0x53, 0xea, 0xfe, 0x31, 0x48, 0x7d, 0x86, 0xde, 0xeb, 0x82,
		0x86, 0x1e, 0x62, 0x03, 0x98, 0x59, 0x00, 0x37, 0xeb, 0x61, 0xe9, 0xf9, 0x7a, 0x40,
		0x78, 0x1c, 0xba, 0xbc, 0x0b, 0x88, 0xfb, 0xfd, 0x9d, 0xd5, 0x01, 0x11, 0x94, 0xe0,
		0x35, 0xbe, 0x33, 0xe8, 0xe5, 0x36, 0xfb, 0x9c, 0x45, 0xcb, 0x75, 0xaf, 0xd6, 0x35,
		0xff, 0x78, 0x92, 0x7f, 0xa1, 0x7c, 0xa8, 0xfc, 0xb7, 0xf7, 0xa8, 0x52, 0xa9, 0xc6,
		0x84, 0x72, 0x3d, 0x1c, 0xc9, 0xdf, 0x35, 0xc6, 0xe6, 0x00, 0xe1, 0x48, 0x72, 0xce,
		0x83, 0x1b, 0xcc, 0xf8, 0x33, 0x2d, 0x4f, 0x98, 0x75, 0x00, 0x3c, 0x41, 0xdf, 0x7a,
		0xed, 0x38, 0x53, 0xb1
};


unsigned char hdcp_2_2_R1_test_key_priv[320] = {
	0xec, 0xbe, 0xe5, 0x5b, 0x9e, 0x7a, 0x50, 0x8a, 0x96, 0x80, 0xc8, 0xdb, 0xb0, 0xed, 0x44, 0xf2, 0xba, 0x1d, 0x5d, 0x80,
	0xc1, 0xc8, 0xb3, 0xc2, 0x74, 0xde, 0xee, 0x28, 0xec, 0xdc, 0x78, 0xc8, 0x67, 0x53, 0x07, 0xf2, 0xf8, 0x75, 0x9c, 0x4c,
	0xa5, 0x6c, 0x48, 0x94, 0xc8, 0xeb, 0xad, 0xd7, 0x7d, 0xd2, 0xea, 0xdf, 0x74, 0x20, 0x62, 0xc9, 0x81, 0xa8, 0x3c, 0x36,
	0xb9, 0xea, 0x40, 0xfd,
	
	//Q:
	0xbe, 0x00, 0x19, 0x76, 0xc6, 0xb4, 0xba, 0x19, 0xd4, 0x69, 0xfa, 0x4d, 0xe2, 0xf8, 0x30, 0x27, 0x36, 0x2b, 0x4c, 0xc4,
	0x34, 0xab, 0xd3, 0xd9, 0x8c, 0xd6, 0xb8, 0x0d, 0x37, 0x5e, 0x59, 0x4b, 0x76, 0x70, 0x68, 0x2b, 0x1f, 0x4c, 0x3d, 0x47,
	0x5f, 0xa5, 0xb1, 0xcd, 0x74, 0x56, 0x88, 0xfe, 0x7c, 0xf8, 0x3b, 0x30, 0x6f, 0xfd, 0xc3, 0xed, 0x87, 0x3c, 0xa1, 0x53,
	0x84, 0xc3, 0xd2, 0x7f,
	
	//d, 0xmod, 0x(p-1):
	0x60, 0x71, 0x9b, 0xe9, 0xe8, 0xf3, 0x97, 0x1f, 0xfe, 0x13, 0xd4, 0xbf, 0x7a, 0xa2, 0x0d, 0xf6, 0x7b, 0xcf, 0x3e, 0xaa,
	0x17, 0x47, 0x75, 0xc3, 0x7f, 0xec, 0xd9, 0x44, 0x9e, 0xc9, 0x6a, 0x02, 0xe9, 0xe4, 0xaf, 0x56, 0x51, 0xd5, 0x47, 0xa9,
	0x09, 0xb2, 0xc5, 0x16, 0xa7, 0x8b, 0x2b, 0x34, 0xa0, 0x33, 0x6e, 0x2f, 0x3d, 0x95, 0x7b, 0xe8, 0xef, 0x02, 0xe4, 0x14,
	0xbf, 0x44, 0x28, 0xd9,
	
	//d, 0xmod, 0x(q-1):
	0x10, 0x0e, 0x2e, 0x18, 0xad, 0x5d, 0xe4, 0x43, 0xfe, 0x81, 0x1e, 0x17, 0xaa, 0xd0, 0x52, 0x31, 0x5e, 0x10, 0x76, 0xa2,
	0x35, 0xd9, 0x37, 0x43, 0xb0, 0xf5, 0x0c, 0x04, 0x81, 0xe3, 0x45, 0x24, 0x6d, 0x53, 0xbe, 0x59, 0xb6, 0x81, 0x58, 0xc4,
	0x49, 0x3e, 0xd5, 0x31, 0x89, 0x5d, 0x2e, 0xa2, 0x62, 0xa9, 0x0f, 0x47, 0x5e, 0x8f, 0x51, 0x19, 0x27, 0x4e, 0x66, 0x4b,
	0x8a, 0x72, 0x89, 0xbd,
	
	//q^-1, 0xmod, 0xp	:
	0x3e, 0x53, 0x0a, 0xf4, 0x8e, 0x75, 0xe1, 0x52, 0xc6, 0x24, 0xe9, 0xf7, 0xbb, 0xac, 0x3f, 0x22, 0x5f, 0xe8, 0xe0, 0x79,
	0x35, 0xff, 0x91, 0xee, 0x22, 0x56, 0xd2, 0x00, 0x68, 0x32, 0xc4, 0xe1, 0x5f, 0xff, 0xf8, 0xb1, 0x1d, 0xee, 0xdc, 0x57,
	0x81, 0xd1, 0xab, 0x8b, 0x37, 0x22, 0xe3, 0x9f, 0xd0, 0xa1, 0xc1, 0xce, 0x1d, 0xd0, 0x24, 0x23, 0xa0, 0x0e, 0xf7, 0xa6,
	0xdb, 0xa3, 0xea, 0xd3

};

unsigned char hdcp_2_2_R1_test_key_lc128[16] = {0x93, 0xce, 0x5a, 0x56, 0xa0, 0xa1, 0xf4, 0xf7, 0x3c, 0x65, 0x8a, 0x1b, 0xd2, 0xae, 0xf0, 0xf7};


unsigned char facskey_ks_hdcp2_2_r1[16]
={
	0xf3, 0xdf, 0x1d, 0xd9, 0x57, 0x96,
	0x12, 0x3f, 0x98, 0x97, 0x89,
	0xb4, 0x21, 0xe1, 0x2d, 0xe1
};



//hdcp2.2 test R1 KM:
#if 0
unsigned char km[KM_SIZE] = {0x68, 0xbc, 0xc5, 0x1b, 0xa9, 0xdb, 
							 0x1b, 0xd0, 0xfa, 0xf1, 0x5e, 0x9a, 
							 0xd8, 0xa5, 0xaf, 0xb9};
#endif


unsigned char kd[KD_SIZE] = {0xff};
unsigned char kh[KH_SIZE] = {0xff};
unsigned char ks[KS_SIZE] = {0xff};
unsigned char ks_XOR_lc128[KS_SIZE] = {0xff};

#if 0
/* for test */
unsigned char ks_XOR_lc128[KS_SIZE] = 
		{0xe9, 0x72, 0xd0, 0x30,0x94, 0x91, 0x47,0xcd,
		 0x85, 0x2c, 0xd1, 0x9d, 0x7c, 0x8f, 0xca, 0x00};
#endif


unsigned char kpubrx_n[128] = {0xff};
unsigned char kpubrx_e[3] = {0xff};



unsigned char lsa[20] = {
    0x38,0xED,0x6E,0xD4,0xFE,0x89,0x63,0x93,0x61,0x28,0x26,0x58,0x1F,0x9D,0x6E,0x1A,0xD5,0x96,0xC0,0x8B
};

unsigned char auprikey[PRIV_KEY_SIZE] = 
{
    0x07, 0x14, 0x01, 0x03, 0x1F, 0x5D, 0x34, 0x78, 0xCA, 0x31, 0xC3, 0xA3, 0x55, 0x4A, 0x9B, 0x24,     
};

/* aes key for PDKEY (KsXORlc128) */
unsigned char auprikey_for_pdkey[PRIV_KEY_SIZE] = 
{
    0x17, 0x24, 0xab, 0x13, 0x7e, 0x5f, 0x25, 0x0a, 0x1c, 0x15, 0xde, 0x3f, 0xee, 0x2f, 0xf1, 0x35,     
};

/* encrypted ks_XOR_lc128 */
unsigned char pdkey[KS_SIZE] = {0xff};


/* for HDCP TX */

// fix
unsigned char DEV_COUNT = 0x00;
unsigned char current_pairing = 0x00;
unsigned char key_N[128] = {
0xA7, 0x9B, 0xA0, 0x1A, 0x00, 0x2D, 0xFF, 0x8D, 0x2F, 0xED, 0x70, 0x15, 0xC5, 0xE0, 0x11, 0xBB,
0xC8, 0xEF, 0x5B, 0x2C, 0xB3, 0x12, 0x0F, 0xBE, 0x88, 0x7C, 0x98, 0x44, 0x3C, 0x65, 0x45, 0xBC, 
0x20, 0xAC, 0x07, 0xE2, 0x4C, 0x74, 0x2A, 0xB4, 0xB1, 0x0E, 0x47, 0x2A, 0xD6, 0x20, 0x19, 0xCE,
0x75, 0x18, 0x45, 0x28, 0x90, 0x4F, 0x84, 0x42, 0x81, 0x37, 0xED, 0x1D, 0x0B, 0x48, 0xF7, 0x53,
0xE3, 0x92, 0xF2, 0xEB, 0xDF, 0x7A, 0x91, 0xDF, 0xE8, 0xDB, 0xB1, 0xC4, 0xFD, 0xFD, 0xC1, 0xAD, 
0x4E, 0xCC, 0xBE, 0x11, 0xE2, 0x76, 0x9B, 0x78, 0x2B, 0xB8, 0xF4, 0x0E, 0x9D, 0x05, 0xD6, 0x08, 
0xD0, 0x76, 0x2C, 0xE8, 0x4D, 0xEE, 0x3D, 0x31, 0xDA, 0xC4, 0xF7, 0x01, 0x12, 0x8F, 0x5D, 0x94,
0xE6, 0xCB, 0x15, 0xFE, 0x53, 0x42, 0xB2, 0x51, 0x8C, 0x5D, 0xC7, 0x64, 0xDE, 0x14, 0x8F, 0xAF, 
};

unsigned char key_E[3] = {
 0xC1, 0xAF, 0x36,
};





typedef struct
{
	unsigned char id[KEYLEN_RXID];
	unsigned char repeater;
	unsigned char km[KM_SIZE];
	unsigned char Ekh_km[ENCKMKH_SIZE];
	unsigned char m[KEYLEN_M];
	
}HDCP2X_PAIRING_LIST_T;

HDCP2X_PAIRING_LIST_T pairing[MAX_PAIRING_SIZE];


int hdcp2x_kernel_space_test_key_enable = 0;		/* test:  1 enable,  0 disable*/


#define HDCP_log(x...) do{dprintf("~~~~~~~\n[DRV_HDCP2]" x);}while(0)
int use_hdcp2_2_keyd = 0;

void hdcp2x_cmmDumpHex(const char *pName, const unsigned char *data, int len)
{

#if HDCP_DUMP_KEY

    int pos = 0;
    
	if(NULL == data || len <= 0)
    {
        return;
    }
	//enableDebug(TRUE);

    dprintf(" --- ^^^^^ TZ_Dump %s ---", pName);
    dprintf( "\n---------------------------------------");

    while(pos < len) 			// only print message ID
    {
        if (pos%8 == 0)
            dprintf("\n[%04x]|", pos);
        dprintf("0x%02x, ", data[pos]); // secure data, cannot show in mtktool
        pos ++;
    }
    dprintf("\n--------^^^^^---------\n");

	//enableDebug(FALSE);

#endif	
    
}








DTV_STATUS_T DRV_HDCP2_SetEncLc128 (unsigned char *pEncKey, UINT32 u4Length)
{    
    //fix compile warning,UINT16 u2Size;
    int i;
    symmetric_key aeskey; 
    unsigned char  au1Buf[16] = {0}; 
    unsigned char  au1DecBuf[16] = {0}; 
    hash_state md;
    unsigned char  tmp[20] = {0};

	//TEE REVIEW
	if(NULL == pEncKey || u4Length != KEYLEN_LC128)
	{
		HDCP_log("DRV_HDCP2_SetEncLc128 invalid arguments\n");
		return NOT_OK;
	}


    // decrypt lc128
    aes_setup(auprikey, PRIV_KEY_SIZE, 0, &aeskey);
    aes_ecb_decrypt(pEncKey,au1Buf, &aeskey); 
    aes_done(&aeskey); 
        
    for ( i = 0 ; i < 16 ; i ++)
    {
       *(au1DecBuf + i)  = *(auprikey + i) ^ *(au1Buf + i);
    }

    // sha1 check  
    sha1_init(&md);
    sha1_process(&md, (unsigned char*)au1DecBuf, 16);
    sha1_done(&md, tmp);

    if (x_memcmp(tmp,lsa, 20) != 0)
    {
        return (NOT_OK);
    }
    else
    {
        //x_memcpy(lc128, au1DecBuf, sizeof(lc128));
		//TEE REVIEW
		memcpy_s2s_chk(lc128, au1DecBuf, sizeof(lc128), NULL);

    }

     
    return OK;
}

DTV_STATUS_T DRV_HDCP2_SetEncKey (unsigned char *pEncKey, UINT32 u4Length)
{    
    UINT16 u2Size;
    int i;
    symmetric_key aeskey; 
    unsigned char  au1Buf[ENCRYPTION_DATA_LENGTH] = {0}; 
    hash_state md;
    unsigned char  tmp[20] = {0};
    HDCP2_KEYDECDCP_T* pDecDcpKey;
	
#if 1 //#ifdef HDCP_A2_KEY_STRUCTURE
    long decryptedKeyLength = 0;
    int OneCnts = 0;
    int m, k;
    int i8Ksv = 0;
    unsigned char ReceiverID[5];
#endif


    u2Size = (UINT16)sizeof(key_DecDcp);



	//TEE REVIEW
	if(NULL == pEncKey)
	{
		HDCP_log("DRV_HDCP2_SetEncKey invalid arguments");
		return NOT_OK;
	}
#if 0 //#ifndef HDCP_A2_KEY_STRUCTURE
    if (u4Length > u2Size)
    {
        u4Length = u2Size;
    }
	//TEE REVIEW
	if(u4Length < u2Size)
	{
		HDCP_log("DRV_HDCP2_SetEncKey invalid arguments");
		return NOT_OK;
	}
	
#endif

    /* resetting keys */
    x_memset(km, 0, sizeof(km));
    x_memset(kd, 0, sizeof(kd));
    x_memset(ks, 0, sizeof(ks));

#if 1 //#ifdef HDCP_A2_KEY_STRUCTURE

    // decrypt pEncKey
    _teeSecureStorageSecretDec(pEncKey, u4Length, au1Buf, &decryptedKeyLength); // Get decryptedKeyLength    

	HDCP_log("[DRV_HDCP2_SetEncKey] _teeSecureStorageSecretDec(), decryptedKeyLength = %ld", decryptedKeyLength);
    //if(decryptedKeyLength != ENCRYPTION_DATA_LENGTH) // decrypted key length be equal to ENCRYPTION_DATA_LENGTH
    //{
    //    return 3;
    //}
    //x_memcpy(au1Buf, pEncKey, ENCRYPTION_DATA_LENGTH);
    //_teeSecureStorageSecretDec(pEncKey, u4Length, au1Buf, &decryptedKeyLength); // decrypt pEncKey

	
    //x_memcpy(key_DecDcp, au1Buf, ENCRYPTION_DATA_LENGTH);
	//TEE REVIEW
	memcpy_s2s_chk(key_DecDcp, au1Buf, decryptedKeyLength, NULL);

    // Receiver ID check (should be 20 1s and 20 0s)
    OneCnts = 0;
    m = 0;
    i8Ksv = 0;
	
    //x_memcpy(ReceiverID, key_DecDcp+16, 5);
	//TEE REVIEW
	memcpy_s2s_chk(ReceiverID, key_DecDcp+16, 5, NULL);
    for (k = 4; k >=0; k--)
    {
         i8Ksv = ((i8Ksv << 8) + ReceiverID[k]);
         for (m = 0; m < 8; m++) {
             if ((ReceiverID[k]>>m) & 0x1 ) {
                 OneCnts++;
             }
         }
    }
    
    if(OneCnts!=20)
    {
        return 4;
    }
    
    // write lc128 
    //x_memcpy(lc128, key_DecDcp, LC128_SIZE);   
	//TEE REVIEW
	memcpy_s2s_chk(lc128, key_DecDcp, LC128_SIZE, NULL);   

#else
    if (u4Length != ENCRYPTION_DATA_LENGTH)
    {
        HDCP_log( "(%s) !!! dcp enc key size mismatching \n", __FUNCTION__);
    } 

    aes_setup(auprikey, PRIV_KEY_SIZE, 0, &aeskey);

    zeromem(au1Buf,sizeof(au1Buf));


    //x_memcpy(au1Buf, pEncKey, ENCRYPTION_DATA_LENGTH);

	//TEE REVIEW
	memcpy_n2s_chk(au1Buf, pEncKey, ENCRYPTION_DATA_LENGTH, NULL);

	
    for ( i = 0 ; i < ENCRYPTION_SIZE ; i += 16)
    {	
        aes_ecb_decrypt((pEncKey+ i),(au1Buf + i), &aeskey);
    }

    aes_done(&aeskey); 
    
    //x_memcpy(key_DecDcp, au1Buf, DECRYPTION_DATA_LENGTH);
	//TEE REVIEW
	memcpy_s2s_chk(key_DecDcp, au1Buf, DECRYPTION_DATA_LENGTH, NULL);

    // sha1 check  
    sha1_init(&md);
    sha1_process(&md, (unsigned char*)key_DecDcp, 842);
    sha1_done(&md, tmp);
    
    if (x_memcmp(tmp,(key_DecDcp+842), 20) != 0) {
       return 2;
    }    
#endif

#ifdef WFD_USE_HDCP_FACSIMILE_KEY
    {
        static HDCP2_KEYDECDCP_T facs_keydecdcp;

        //x_memcpy(facs_keydecdcp.rKeyPrit.au1P, facskey_p, sizeof(facs_keydecdcp.rKeyPrit.au1P));
        //x_memcpy(facs_keydecdcp.rKeyPrit.au1Q, facskey_q, sizeof(facs_keydecdcp.rKeyPrit.au1Q));
        //x_memcpy(facs_keydecdcp.rKeyPrit.au1DP, facskey_dmod_p1, sizeof(facs_keydecdcp.rKeyPrit.au1DP));
        //x_memcpy(facs_keydecdcp.rKeyPrit.au1DQ, facskey_dmod_q1, sizeof(facs_keydecdcp.rKeyPrit.au1DQ));
        //x_memcpy(facs_keydecdcp.rKeyPrit.au1QINV, facskey_q1mod_p, sizeof(facs_keydecdcp.rKeyPrit.au1QINV));

		//TEE REVIEW
		memcpy_s2s_chk(facs_keydecdcp.rKeyPrit.au1P, facskey_p, sizeof(facs_keydecdcp.rKeyPrit.au1P), NULL);
		memcpy_s2s_chk(facs_keydecdcp.rKeyPrit.au1Q, facskey_q, sizeof(facs_keydecdcp.rKeyPrit.au1Q), NULL);
		memcpy_s2s_chk(facs_keydecdcp.rKeyPrit.au1DP, facskey_dmod_p1, sizeof(facs_keydecdcp.rKeyPrit.au1DP), NULL);
		memcpy_s2s_chk(facs_keydecdcp.rKeyPrit.au1DQ, facskey_dmod_q1, sizeof(facs_keydecdcp.rKeyPrit.au1DQ), NULL);
		memcpy_s2s_chk(facs_keydecdcp.rKeyPrit.au1QINV, facskey_q1mod_p, sizeof(facs_keydecdcp.rKeyPrit.au1QINV), NULL);



        pDecDcpKey = &facs_keydecdcp;
    }
#else /* !WFD_USE_HDCP_FACSIMILE_KEY */

    pDecDcpKey = (HDCP2_KEYDECDCP_T*) key_DecDcp;

#endif /* WFD_USE_HDCP_FACSIMILE_KEY */


    //x_memcpy(key_p,pDecDcpKey->rKeyPrit.au1P,sizeof(key_p));
    //x_memcpy(key_q,pDecDcpKey->rKeyPrit.au1Q,sizeof(key_q));
    //x_memcpy(key_dP,pDecDcpKey->rKeyPrit.au1DP,sizeof(key_dP));
    //x_memcpy(key_dQ,pDecDcpKey->rKeyPrit.au1DQ,sizeof(key_dQ));
    //x_memcpy(key_qInv,pDecDcpKey->rKeyPrit.au1QINV,sizeof(key_qInv));

	//TEE REVIEW
    memcpy_s2s_chk(key_p,pDecDcpKey->rKeyPrit.au1P,sizeof(key_p), NULL);
    memcpy_s2s_chk(key_q,pDecDcpKey->rKeyPrit.au1Q,sizeof(key_q), NULL);
    memcpy_s2s_chk(key_dP,pDecDcpKey->rKeyPrit.au1DP,sizeof(key_dP), NULL);
    memcpy_s2s_chk(key_dQ,pDecDcpKey->rKeyPrit.au1DQ,sizeof(key_dQ), NULL);
    memcpy_s2s_chk(key_qInv,pDecDcpKey->rKeyPrit.au1QINV,sizeof(key_qInv), NULL);

    return OK;
}

DTV_STATUS_T DRV_HDCP2_GetCertInfo (unsigned char *pData, UINT32 u4length)
{
	HDCP_log("DRV_HDCP2_GetCertInfo u4length: %d certlen %d\n", u4length, sizeof(HDCP2_KEYCERT_T));

	//TEE REVIEW
	if(NULL == pData || u4length != sizeof(HDCP2_KEYCERT_T))
	{
		HDCP_log("DRV_HDCP2_GetCertInfo invalid arguments\n");

		return NOT_OK;
	}


#ifndef WFD_USE_HDCP_FACSIMILE_KEY
    HDCP2_KEYDECDCP_T* pDecDcpKey;
#endif

#ifdef WFD_USE_HDCP_FACSIMILE_KEY

    //x_memcpy(pData, facskey_certrx, u4length);

	//TEE REVIEW
	memcpy_s2n_chk(pData, facskey_certrx, u4length, NULL);

#else /* !WFD_USE_HDCP_FACSIMILE_KEY */

	if(0 == hdcp2x_kernel_space_test_key_enable)
	{
		pDecDcpKey = (HDCP2_KEYDECDCP_T*) key_DecDcp;
		memcpy_s2n_chk(pData, (HDCP2_KEYDECDCP_T*)pDecDcpKey->rKeyCert.au1RxID, u4length, NULL);
	}
	else
	{
		HDCP_log("%s test mode!", __FUNCTION__);
		pDecDcpKey = (HDCP2_KEYDECDCP_T*) hdcp_2_2_R1_test_key_cert;
		memcpy_s2n_chk(pData, (HDCP2_KEYDECDCP_T*)pDecDcpKey->rKeyCert.au1RxID, u4length, NULL);    
	}

    
#endif

    
    return OK;
}

DTV_STATUS_T DRV_HDCP2_DecryptRSAESOAEP (unsigned char *pEkpub_km)
{
    unsigned long outlen=KM_SIZE;
    rsa_key key;
    rsa_key* pKey = &key;
    int stat,i,ret[5];
    unsigned long lparamlen = 0;
    int hash_idx = 0;
    int prng_idx = 0;
    /* Initialize key for fixing klockwork issue*/
    HDCP_log( "Entering %s...\n", __FUNCTION__);
	if(NULL == pEkpub_km)
	{
		HDCP_log("invalid arguments\n");
		return NOT_OK;
	}
    key.e = key.d = key.N = key.p = key.q = key.qP = key.dP = key.dQ = (void*) &key;

    /* Set math decriptor and register Psuedo Random Number Generator and hash function*/
    //ltc_mp = ltm_desc;    
    if(register_prng(&sprng_desc)<0){
        DRV_HDCP2_DBG(DRV_HDCP2_DBG_ERR, "%s: register_prng fail\n", __FUNCTION__);
        return NOT_OK;
    }
    if(register_hash(&sha256_desc)<0){
        DRV_HDCP2_DBG(DRV_HDCP2_DBG_ERR, "%s: register_hash fail\n", __FUNCTION__);
        return NOT_OK;
    }
    
    hash_idx = find_hash("sha256");
    prng_idx = find_prng("sprng");
       
    /* Initialize key */
    if ((stat = ltc_init_multi(&pKey->e, &pKey->d, &pKey->N, &pKey->dQ, &pKey->dP, &pKey->qP, &pKey->p, &pKey->q, NULL)) != CRYPT_OK) {
        DRV_HDCP2_DBG(DRV_HDCP2_DBG_ERR, "%s: ltc_init_multi fail\n", __FUNCTION__);
        return NOT_OK;
    }
    
    /* Set values of rsa private key */
    key.type = PK_PRIVATE;
    
    ret[0] = mp_read_unsigned_bin(key.p,key_p,sizeof(key_p));
    ret[1] = mp_read_unsigned_bin(key.q,key_q,sizeof(key_q));
    ret[2] = mp_read_unsigned_bin(key.dP,key_dP,sizeof(key_dP));
    ret[3] = mp_read_unsigned_bin(key.dQ,key_dQ,sizeof(key_dQ));
    ret[4] = mp_read_unsigned_bin(key.qP,key_qInv,sizeof(key_qInv));
    
    for(i=0;i<5;i++){
        if(ret[i]==CRYPT_ERROR){
            DRV_HDCP2_DBG(DRV_HDCP2_DBG_ERR, "%s: mp_read_unsigned_bin fail\n", __FUNCTION__);
            return NOT_OK;            
        }
    }
    
    if ((stat = mp_mul( key.p,  key.q,  key.N)) != CRYPT_OK)
    {
        DRV_HDCP2_DBG(DRV_HDCP2_DBG_ERR, "%s: mp_mul fail\n", __FUNCTION__);
    }    

    /* Do RSAEP-OAEP decryption */
    rsa_decrypt_key_ex(pEkpub_km,PEKPUB_KM_SIZE,km,&outlen,NULL,lparamlen,hash_idx,LTC_PKCS_1_OAEP,&stat,&key);    
    if(stat != 1){
        DRV_HDCP2_DBG(DRV_HDCP2_DBG_ERR, "%s: rsa_decrypt_key_ex fail\n", __FUNCTION__);
        return NOT_OK;
    }    
    rsa_free(&key);

    DRV_HDCP2_DUMP16(DRV_HDCP2_DBG_VERB, "km", km, 0);



	 return OK;
}

DTV_STATUS_T DRV_HDCP2_KdKeyDerivation (unsigned char *pRtx, UINT32 u4length)
{
    unsigned char IV[IV_SIZE];
    symmetric_key aeskey;
    unsigned char dkey[2][DKEY_SIZE];
    int i,cnt;    


    HDCP_log( "Entering %s...\n", __FUNCTION__);


	//TEE REVIEW
	if(NULL == pRtx || PRTX_SIZE != u4length)
	{
		HDCP_log("invalid arguments\n");
		return NOT_OK;
	}


    /* Setup IV */
    zeromem(IV,sizeof(IV));
    zeromem(dkey,sizeof(dkey));
    for(i=0;i<PRTX_SIZE;i++){
        IV[i] = pRtx[i];
    }
    /* Do AES encryption to get dkey0 and dkey1 */
    aes_setup(km,KM_SIZE,0,&aeskey);
    aes_ecb_encrypt(IV,dkey[0],&aeskey);
    IV[sizeof(IV)-1] = 0x01;
    aes_setup(km,KM_SIZE,0,&aeskey);
    aes_ecb_encrypt(IV,dkey[1],&aeskey);
    aes_done(&aeskey);

    /* Concatenate dkey0 and dkey2 to kd*/
    for(i=0,cnt=0;i<DKEY_SIZE;i++,cnt++){
        kd[cnt] = dkey[0][i];
    }
    for(i=0;i<DKEY_SIZE;i++,cnt++){
        kd[cnt] = dkey[1][i];
    } 

    //DRV_HDCP2_DUMP16(DRV_HDCP2_DBG_VERB, "kd-1", kd, 0);
    //DRV_HDCP2_DUMP16(DRV_HDCP2_DBG_VERB, "kd-2", kd, 16);
    hdcp2x_cmmDumpHex("kd:", kd, sizeof(kd));


	use_hdcp2_2_keyd = 0;

	HDCP_log("use hdcp2.0 keyd [%d]\n\n", use_hdcp2_2_keyd);

    	 return OK;
}
DTV_STATUS_T DRV_HDCP2_ComputeHprime (unsigned char *pH_prime, unsigned char Repeater, unsigned char *pRtx)
{
    int iRet;
    hmac_state hmac;
    int hash_idx;
    unsigned long resultLen = 32; //PH_PRIME_SIZE;
    HDCP_log( "Entering %s...\n", __FUNCTION__);


	//TEE REVIEW
	if(NULL == pH_prime || NULL == pRtx)
	{
		HDCP_log("invalid arguments\n");
		return NOT_OK;
	}

    if((hash_idx=register_hash(&sha256_desc))<0){
        DRV_HDCP2_DBG(DRV_HDCP2_DBG_ERR, "%s: register_hash fail\n", __FUNCTION__);
        return 10;
    }

    /* Do HMAC-SHA256 */
    iRet = hmac_init(&hmac,hash_idx,kd,KD_SIZE);
    if (iRet != OK)
    {
        DRV_HDCP2_DBG(DRV_HDCP2_DBG_ERR, "%s: hmac_init fail\n", __FUNCTION__);
        return iRet;
    }

    pRtx[7] ^= Repeater;
    iRet = hmac_process(&hmac,pRtx,PRTX_SIZE);
    if (iRet != OK)
    {
        DRV_HDCP2_DBG(DRV_HDCP2_DBG_ERR, "%s: hmac_process fail\n", __FUNCTION__);
        return iRet;
    }
    
    iRet = hmac_done(&hmac,pH_prime,&resultLen);
    if (iRet != OK)
    {
        DRV_HDCP2_DBG(DRV_HDCP2_DBG_ERR, "%s: hmac_done fail\n", __FUNCTION__);
        return iRet;
    }

    DRV_HDCP2_DUMP16(DRV_HDCP2_DBG_VERB, "Hprime-1", pH_prime, 0);
    DRV_HDCP2_DUMP16(DRV_HDCP2_DBG_VERB, "Hprime-2", pH_prime, 16);
	hdcp2x_cmmDumpHex("HDCP2 H:", pH_prime, PH_PRIME_SIZE);
   
    return OK;

}

DTV_STATUS_T DRV_HDCP2_ComputeLprime (unsigned char *pL_prime, unsigned char *pRn, unsigned char *pRrx)
{
    int iRet;
    unsigned long resultLen = 32; //PL_PRIME_SIZE;
    unsigned char kd_XOR_pRrx[KD_SIZE];
    int offset = KD_SIZE - PRRX_SIZE;
    int hash_idx;
    int i;
    hmac_state hmac;   

    HDCP_log( "Entering %s...\n", __FUNCTION__);


	//TEE REVIEW
	if(NULL == pL_prime || NULL == pRn || NULL == pRrx)
	{
		HDCP_log("invalid arguments\n");
		return NOT_OK;
	}



    hash_idx = find_hash("sha256");


    /* kd XOR Rrx*/
    //XMEMCPY(kd_XOR_pRrx,kd,KD_SIZE);


	//TEE REVIEW
	memcpy_s2s_chk(kd_XOR_pRrx,kd,KD_SIZE, NULL);
    for(i=0;i<PRRX_SIZE;i++){
        kd_XOR_pRrx[offset+i] ^= pRrx[i];
    }
    /* Do HMAC-SHA256 */
    iRet = hmac_init(&hmac,hash_idx,kd_XOR_pRrx,KD_SIZE);
    if (iRet != OK)
    {    
        DRV_HDCP2_DBG(DRV_HDCP2_DBG_ERR, "%s: hmac_init fail\n", __FUNCTION__);
        return iRet;
    }
    
    iRet = hmac_process(&hmac,pRn,PRN_SIZE);
    if (iRet != OK)
    {
        DRV_HDCP2_DBG(DRV_HDCP2_DBG_ERR, "%s: hmac_process fail\n", __FUNCTION__);
        return iRet;
    }
    
    iRet = hmac_done(&hmac,pL_prime,&resultLen);
    if (iRet != OK)
    {
        DRV_HDCP2_DBG(DRV_HDCP2_DBG_ERR, "%s: hmac_done fail\n", __FUNCTION__);
        return iRet;
    }

    DRV_HDCP2_DUMP16(DRV_HDCP2_DBG_VERB, "Lprime-1", pL_prime, 0);
    DRV_HDCP2_DUMP16(DRV_HDCP2_DBG_VERB, "Lprime-2", pL_prime, 16);
    
      
    return OK;

}

DTV_STATUS_T DRV_HDCP2_ComputeKh (void)
{
    unsigned char sha256_output[SHA256_OUTPUT_SIZE];
    hash_state sha256;
    unsigned char kprivrx[KPRIVRX_SIZE];
    int i,cnt=0,offset;

    tom_sha256_init(&sha256);
    
    /* Concatenate p, q, dP, dQ and qInv to kprivrx */
    for(i=0;i<KEY_P_SIZE;i++,cnt++)
        kprivrx[cnt] = key_p[i];
    for(i=0;i<KEY_P_SIZE;i++,cnt++)
        kprivrx[cnt] = key_q[i];
    for(i=0;i<KEY_P_SIZE;i++,cnt++)
        kprivrx[cnt] = key_dP[i];
    for(i=0;i<KEY_P_SIZE;i++,cnt++)
        kprivrx[cnt] = key_dQ[i];
    for(i=0;i<KEY_P_SIZE;i++,cnt++)
        kprivrx[cnt] = key_qInv[i];

    /* Do SHA256 hashing to get kh*/
    sha256_process(&sha256,kprivrx,KPRIVRX_SIZE);
    sha256_done(&sha256,sha256_output);

    /* Set kh as sha256_ouput[127:0] */
    offset = SHA256_OUTPUT_SIZE-KH_SIZE;
    for(i=0;i<KH_SIZE;i++){
        kh[i] = sha256_output[offset+i];
    }

    DRV_HDCP2_DUMP16(DRV_HDCP2_DBG_VERB, "kh", kh, 0);



    return OK;
}
DTV_STATUS_T DRV_HDCP2_EncryptKmUsingKh (unsigned char *pEkh_km, unsigned char *pM)
{
    symmetric_key skey;
    int i;

    HDCP_log( "Entering %s...\n", __FUNCTION__);


	//TEE REVIEW
	if(NULL == pEkh_km || NULL == pM )
	{
		HDCP_log("invalid arguments\n");
		return NOT_OK;
	}
    /* Do AES encryption to get pEkh_km */
    /* Setup IV */
    aes_setup(kh,KH_SIZE,0,&skey);
    aes_ecb_encrypt(pM,pEkh_km,&skey);
    aes_done(&skey);
    
    for(i=0;i<16;i++){
        pEkh_km[i] ^= km[i];
        //printf("%x ",pEkh_km[i]);
    }

    DRV_HDCP2_DUMP16(DRV_HDCP2_DBG_VERB, "EKmKh", pEkh_km, 0);



    return OK;
}

DTV_STATUS_T DRV_HDCP2_DecryptKmUsingKh (unsigned char *pM, unsigned char *ekh_km)
{
    symmetric_key skey;
    unsigned char arBuf[16];
    int i;

    HDCP_log( "Entering %s...\n", __FUNCTION__);
	if(NULL == pM || NULL == ekh_km )
	{
		HDCP_log("invalid arguments\n");
		return NOT_OK;
	}


	

    /* Do AES decryption and xor with ekh_km to get km */
    aes_setup(kh,KH_SIZE,0,&skey);
    aes_ecb_encrypt((unsigned char*)pM,km,&skey);
    aes_done(&skey);

    //x_memcpy(arBuf,ekh_km,sizeof(arBuf));

	//TEE REVIEW
	memcpy_n2s_chk(arBuf,ekh_km,sizeof(arBuf), NULL);
    for(i=0;i<KM_SIZE;i++){
        km[i] ^= arBuf[i];
    }  

    DRV_HDCP2_DUMP16(DRV_HDCP2_DBG_VERB, "km", km, 0);


    return OK;
}   
DTV_STATUS_T DRV_HDCP2_dkey2 (unsigned char *dkey2, 
  									 unsigned char *pRn, 
  									 unsigned char *pRtx,  
  									 unsigned char *pRrx)
{
	symmetric_key skey;
	unsigned char IV[IV_SIZE] = {0xff};
	int offset = 0, i = 0;
	
	unsigned char kmXORrn[KM_SIZE] = {0x00};
	unsigned char ctr[PRTX_SIZE] = {0x00};
    HDCP_log( "Entering %s...\n", __FUNCTION__);


	//TEE REVIEW
	if(NULL == dkey2 || NULL == pRn || NULL == pRtx || NULL ==  pRrx)
	{
		HDCP_log("invalid arguments\n");
		return NOT_OK;
	}



	/* Setup IV */	  
	/* IV = Rtx || (Rrx XOR ctr )*/

	zeromem(ctr,sizeof(ctr));

	/* Setup Rtx || ctr , where ctr = 2*/
	
	zeromem(IV,sizeof(IV));


	for(i=0;i<PRTX_SIZE;i++){
		IV[i] = pRtx[i];
	}


	if(0 == use_hdcp2_2_keyd)
	{
		HDCP_log(  "%s: 2.0 IV\n",__FUNCTION__);
			

		IV[IV_SIZE-1]=0x02;
	}
	else
	{
		HDCP_log(  "%s: 2.2 IV\n",__FUNCTION__);
		
		ctr[PRRX_SIZE-1] = 0x2;
		for(i=0; i<PRRX_SIZE; i++)
		{
			IV[i + PRRX_SIZE] = pRrx[i] ^ ctr[i];
		}

	}
	
	
	/* Setup km XOR rn*/
	//x_memcpy(kmXORrn,km,KM_SIZE);

	//TEE REVIEW
	memcpy_s2s_chk(kmXORrn,km,KM_SIZE, NULL);
	
	
	offset = KM_SIZE-PRN_SIZE;
	for(i=0;i<8;i++){
		kmXORrn[offset+i] ^= pRn[i];
	}

	
	/* Do AES encryption to get dkey2 */
	aes_setup(kmXORrn,sizeof(kmXORrn),0,&skey);
	aes_ecb_encrypt(IV,dkey2,&skey);
	aes_done(&skey);


    return OK;
}
DTV_STATUS_T DRV_HDCP2_DecryptEKs (unsigned char *pEks, unsigned char *pRtx, unsigned char *pRrx, unsigned char *pRn)
{
	unsigned char dkey2[DKEY_SIZE] = {0xff};  ///  !!!!!!!!!!

    int i = 0,offset = 0;


    HDCP_log( "Entering %s...\n", __FUNCTION__);


	//TEE REVIEW
	if(NULL == pEks || NULL == pRn || NULL == pRtx || NULL ==  pRrx)
	{
		HDCP_log("invalid arguments\n");
		return NOT_OK;
	}


	DRV_HDCP2_dkey2(dkey2, pRn, pRtx, pRrx);
	hdcp2x_cmmDumpHex("HDCP2_2: dkey2:", dkey2, 16);



    /* dkey2 xor pRrx */
    offset = DKEY_SIZE-PRRX_SIZE;
    for(i=0;i<PRRX_SIZE;i++){
        dkey2[offset+i] ^= pRrx[i];
    }
    /* pEks xor (dkey2 xor pRrx) */
    for(i=0;i<KS_SIZE;i++){
        ks[i] = dkey2[i] ^ pEks[i];
    }

    DRV_HDCP2_DUMP16(DRV_HDCP2_DBG_ERR, "Ks", ks, 0);


    return OK;
}

DTV_STATUS_T DRV_HDCP2_KsXorLC128 (void)
{
    int i;

    HDCP_log( "Entering %s...\n", __FUNCTION__);



    DRV_HDCP2_DUMP16(DRV_HDCP2_DBG_VERB, "Lc128", lc128, 0);
    /* ks xor lc128 */
    for(i=0;i<KS_SIZE;i++){
        ks_XOR_lc128[i] = ks[i] ^ lc128[i];
    }

    DRV_HDCP2_DUMP16(DRV_HDCP2_DBG_ERR, "KsXorLc128", ks_XOR_lc128, 0);

	
	if(AES_HDCP2_SECURE_KEY_PTR < 0)
	{
		HDCP_log("invalid arguments: AES_HDCP2_SECURE_KEY_PTR\n");
		return NOT_OK;
	}


    GCPU_SetSecureSlot(AES_HDCP2_SECURE_KEY_PTR, ks_XOR_lc128, 16);


    return OK;
}

DTV_STATUS_T DRV_HDCP2_SetRiv_Pid(unsigned char *pRiv, UINT32 pid)
{
    /* Set Pidx, KeyIdx, Session Key and random number Riv to dmx for data decryption */
    //if(!DMX_SetHDCPKeyIv(pid,0,ks,pRiv))  
    DRV_HDCP2_DBG(DRV_HDCP2_DBG_INFO, "%s: Pid=%d(0x%x)\n", __FUNCTION__, pid, pid);
    DRV_HDCP2_DUMP8(DRV_HDCP2_DBG_VERB, "Riv", pRiv, 0);
	int i;


    HDCP_log( "Entering %s...\n", __FUNCTION__);


	//TEE REVIEW
	if(NULL == pRiv)
	{
		HDCP_log("invalid arguments\n");
		return NOT_OK;
	}
	//enableDebug(TRUE);
	dprintf("Set riv and key in here to dmx\n");
	dprintf("---------------------------------\n");
	for( i = 0; i < KS_XOR_LC128_SIZE; i++)
		dprintf("%02x ", ks_XOR_lc128[i]);
	dprintf("\n---------------------------------\n");
	for( i = 0; i < 8; i++)
		dprintf("%02x ", pRiv[i]);
	dprintf("\n---------------------------------\n");
	//enableDebug(FALSE);
    if(!DMX_SetHDCPKeyIv(pid, 0, ks_XOR_lc128, pRiv))
        return NOT_OK;
    else
        return OK;
}

DTV_STATUS_T DRV_HDCP2_PresetKsLC128(unsigned char *pKs, unsigned char *pLc128)
{
    /* Set ks and lc128 manually*/
    //x_memcpy(ks,pKs,KS_SIZE);

	//TEE REVIEW
	memcpy_n2s_chk(ks,pKs,KS_SIZE, NULL);


	//fix compile warning,UINT16 u2Size;
    int i;
    symmetric_key aeskey; 
    unsigned char  au1Buf[16]; 
    unsigned char  au1DecBuf[16]; 
    //fix compile warning,hash_state md;
    //fix compile warning,unsigned char  tmp[20];


    HDCP_log( "Entering %s...\n", __FUNCTION__);


	//TEE REVIEW
	if(NULL == pKs || NULL == pLc128)
	{
		HDCP_log("invalid arguments\n");
		return NOT_OK;
	}

    // decrypt lc128
    aes_setup(auprikey, PRIV_KEY_SIZE, 0, &aeskey);
    aes_ecb_decrypt(pLc128 ,au1Buf, &aeskey); 
    aes_done(&aeskey); 

		
    for ( i = 0 ; i < 16 ; i ++)
    {
       *(au1DecBuf + i)  = *(auprikey + i) ^ *(au1Buf + i);
    }

	//enableDebug(TRUE);
	dprintf("####Dump Lc128####\n");
	dprintf("--------au1Buf dump-------\n");
	for( i = 0; i < 16; i++)
	{
		dprintf("%02x ", au1Buf[i]);
	}
	dprintf("\n");
	dprintf("-------au1DecBuf dump---------\n");
	for( i = 0; i < 16; i++)
	{
		dprintf("%02x ", au1DecBuf[i]);
	}
	dprintf("\n");
	dprintf("####End dump####\n");
	//enableDebug(FALSE);



	//x_memcpy(lc128, au1Buf, sizeof(lc128));

	//TEE REVIEW

	memcpy_s2s_chk(lc128, au1Buf, sizeof(lc128), NULL);

	return OK;
}

/* For HDCP TX */

DTV_STATUS_T DRV_HDCP2_Generate_Km (unsigned char *pMaskedDB, unsigned char *pDB, unsigned char *pDBmask)
{ 


#if ENABLE_HDCP2X_TX

    prng_state prng;
    unsigned char entropy[10];
    unsigned char tx_km[KM_SIZE];
//    unsigned char tx_km[KM_SIZE] = { 0xca, 0x9f, 0x83, 0x95, 0x70, 0xd0, 0xd0, 0xf9, 0xcf, 0xe4, 0xeb, 0x54, 0x7e, 0x09, 0xfa, 0x3b};

    int err, i;


	HDCP_log( "Entering %s...\n", __FUNCTION__);

	//TEE REVIEW
	if(NULL == pMaskedDB || NULL == pDB || NULL == pDBmask)
	{
		HDCP_log("invalid arguments\n");
		return NOT_OK;
	}


    /* Generate random number km */    
    /* start it */
    if ((err = yarrow_start(&prng)) != CRYPT_OK) {
        DRV_HDCP2_DBG(DRV_HDCP2_DBG_INFO, "HDCP TX yarrow start error: %s\n", error_to_string(err));
    }
    /* add entropy */
    TzGetRandomBytes(entropy, 10);    
    if ((err = yarrow_add_entropy(entropy, 10, &prng)) != CRYPT_OK) {
        DRV_HDCP2_DBG(DRV_HDCP2_DBG_INFO, "HDCP TX yarrow add_entropy error: %s\n", error_to_string(err));
    }
    /* ready and read */
    if ((err = yarrow_ready(&prng)) != CRYPT_OK) {
        DRV_HDCP2_DBG(DRV_HDCP2_DBG_INFO, "HDCP TX yarrow ready error: %s\n", error_to_string(err));
    }
    yarrow_read(tx_km, KM_SIZE, &prng);
    DRV_HDCP2_DUMP16(DRV_HDCP2_DBG_VERB, "tx_km", tx_km, 0);

    /* Copy km to secure storage */
	//x_memcpy(km, tx_km, KM_SIZE);

	// TEE REVIEW
	memcpy_s2s_chk(km, tx_km, KM_SIZE, NULL);
	
	
    /* Copy km to pDB */
    for(i=0;i<KM_SIZE;i++)
    {
        pDB[DB_SIZE - KM_SIZE + i] = tx_km[i];
    }
    /* DB xor DBMask to get MaskedDB */
    for(i=0;i<DB_SIZE;i++)
    {
        pMaskedDB[i] = pDB[i] ^ pDBmask[i];
    }
    /* Adding 4 zero to pMaskedDB*/
    for(i=0;i<4;i++)
    {
        pMaskedDB[MASKED_DB_SIZE - 1 - i] = 0x00;
    }
    return 0;

#else

	HDCP_log("%s NOT ENABLED\n", __FUNCTION__);
	return NOT_OK;
	
#endif

}


DTV_STATUS_T DRV_HDCP2_EncryptKs(unsigned char *pKs, unsigned char *pRtx, unsigned char *pRrx, unsigned char *pRn)
{
	unsigned char dkey2[DKEY_SIZE] = {0xff};  ///  !!!!!!!!!!

    int i = 0,offset = 0;

	//memcpy(ks, pKs, KS_SIZE);
	//TEE REVIEW

	memcpy_n2s_chk(ks, pKs, KS_SIZE, NULL);

	DRV_HDCP2_dkey2(dkey2, pRn, pRtx, pRrx);
	hdcp2x_cmmDumpHex("HDCP2_2: dkey2:", dkey2, 16);


	offset = DKEY_SIZE-PRRX_SIZE;
	for(i=0;i<PRRX_SIZE;i++) 
	{
        dkey2[offset+i] ^= pRrx[i];
    }
	
	for(i=0;i<KS_SIZE;i++){
        pKs[i] = dkey2[i] ^ pKs[i];
    }

	//HDCP_log( "Dumping eks\n");
    hdcp2x_cmmDumpHex("Eks:", pKs, KS_SIZE);

	return OK;
}






DTV_STATUS_T DRV_HDCP2_Generate_Ks (unsigned char *pEks, unsigned char *pRrx, unsigned char *pRtx, unsigned char *pRn)
{
#if ENABLE_HDCP2X_TX
	unsigned char eks[KS_SIZE] = {0};

	if(0 == hdcp2x_kernel_space_test_key_enable)
	{


	    prng_state prng;
	    unsigned char entropy[10];
	    unsigned char tx_ks[KS_SIZE];

	    int err;
		HDCP_log( "Entering %s...\n", __FUNCTION__);

		//TEE REVIEW
		if(NULL == pEks || NULL == pRrx || NULL == pRtx || NULL == pRn)
		{
			HDCP_log("invalid arguments\n");
			return NOT_OK;
		}
	 	    /* Generate random number ks */
	    /* start it */
	    if ((err = yarrow_start(&prng)) != CRYPT_OK) {
	        DRV_HDCP2_DBG(DRV_HDCP2_DBG_INFO, "HDCP TX yarrow start error: %s\n", error_to_string(err));
	    }
	    /* add entropy */
	    TzGetRandomBytes(entropy, 10);    
	    if ((err = yarrow_add_entropy(entropy, 10, &prng)) != CRYPT_OK) {
	        DRV_HDCP2_DBG(DRV_HDCP2_DBG_INFO, "HDCP TX yarrow add_entropy error: %s\n", error_to_string(err));
	    }
	    /* ready and read */
	    if ((err = yarrow_ready(&prng)) != CRYPT_OK) {
	        DRV_HDCP2_DBG(DRV_HDCP2_DBG_INFO, "HDCP TX yarrow ready error: %s\n", error_to_string(err));
	    }
	    yarrow_read(tx_ks, KS_SIZE, &prng);
	    DRV_HDCP2_DUMP16(DRV_HDCP2_DBG_VERB, "tx_ks", tx_ks, 0);


		
	    /* share ks for other apis */
	    //x_memcpy(ks, tx_ks, KS_SIZE);

		//TEE REVIEW
		memcpy_s2s_chk(ks, tx_ks, KS_SIZE, NULL);
	}

	else
	{
		/* set ks and km for test mode */
		HDCP_log("%s test mode!", __FUNCTION__);
		//x_memcpy(ks, facskey_ks_hdcp2_2_r1, KS_SIZE);
		//x_memcpy(km, facskey_km_hdcp2_2_r1, KM_SIZE);

		// TEE REVIEW
		memcpy_s2s_chk(ks, facskey_ks_hdcp2_2_r1, KS_SIZE, NULL);
		memcpy_s2s_chk(km, facskey_km_hdcp2_2_r1, KM_SIZE, NULL);

	}

	//x_memcpy(eks, ks, KS_SIZE);

	//TEE REVIEW
	memcpy_s2s_chk(eks, ks, KS_SIZE, NULL);



	/* encrypt the ks to eks*/
	DRV_HDCP2_EncryptKs(eks, pRtx, pRrx, pRn);



	/* return the eks --> pEks*/
	x_memcpy(pEks, eks, KS_SIZE);

	// TEE REVIEW
	memcpy_s2n_chk(pEks, eks, KS_SIZE, NULL);





#else

	HDCP_log("%s NOT ENABLED\n", __FUNCTION__);
	return NOT_OK;
	
#endif


	
}

DTV_STATUS_T DRV_HDCP2_SetEncKm (unsigned char *pEncKm)
{

#if ENABLE_HDCP2X_TX



	HDCP_log( "Entering %s...\n", __FUNCTION__);

	//TEE REVIEW
	if(NULL == pEncKm)
	{
		HDCP_log("invalid arguments\n");
		return NOT_OK;
	}




#if 1 //#ifdef HDCP_A2_KEY_STRUCTURE
    long decryptedKmLength=0;
    _teeSecureStorageSecretDec(pEncKm, SECUREKM_SIZE, NULL, &decryptedKmLength); // Get decryptedKeyLength    

    if(decryptedKmLength != 16)
    {
        return NOT_OK;
    }
    _teeSecureStorageSecretDec(pEncKm, SECUREKM_SIZE, km, &decryptedKmLength); // decryption    
    return OK; 
#else
    return NOT_OK;
#endif


#else

	HDCP_log("%s NOT ENABLED\n", __FUNCTION__);
	return NOT_OK;
	
#endif



}

DTV_STATUS_T DRV_HDCP2_GetEncKm (unsigned char *pEncKm)
{

#if ENABLE_HDCP2X_TX


	HDCP_log( "Entering %s...\n", __FUNCTION__);

	//TEE REVIEW
	if(NULL == pEncKm)
	{
		HDCP_log("invalid arguments\n");
		return NOT_OK;
	}




#if 1 //#ifdef HDCP_A2_KEY_STRUCTURE
    TEE_SECURESTORAGE_USERKEY_T rSecretusrkey;
    long i4EncryptedLength = KM_SIZE+64;

    memset((void*)&rSecretusrkey, 0xff, sizeof(rSecretusrkey));

    _teeSecureStorageEnc(&rSecretusrkey, km, KM_SIZE, pEncKm, &i4EncryptedLength);

    return OK;
#else
    return NOT_OK;
#endif



#else

	HDCP_log("%s NOT ENABLED\n", __FUNCTION__);
	return NOT_OK;
	
#endif


}






/*
	for HDCP 2.2
*/
DTV_STATUS_T DRV_HDCP2_2_KdKeyDerivation (unsigned char *pRtx, unsigned char *pRrx, UINT32 u4length)
{
	unsigned char IV[IV_SIZE];
	unsigned char ctr[PRTX_SIZE];
	symmetric_key aeskey;
	unsigned char dkey[2][DKEY_SIZE];
	int i,cnt;	  



	HDCP_log( "Entering %s...\n", __FUNCTION__);

	//TEE REVIEW
	if(NULL == pRtx || NULL == pRrx ||PRTX_SIZE != u4length)
	{
		HDCP_log("invalid arguments\n");
		return NOT_OK;
	}




	/* Setup IV */	  
	/* IV = Rtx || (Rrx XOR ctr )*/



	zeromem(IV,sizeof(IV));
	zeromem(dkey,sizeof(dkey));
	zeromem(ctr,sizeof(ctr));


	/* <1> put Rtx[7 ... 0] into IV[7 ... 0]*/
	for(i=0;i<PRTX_SIZE;i++)
	{
		IV[i] = pRtx[i];
	}

	hdcp2x_cmmDumpHex("Km:", km, KM_SIZE);
	hdcp2x_cmmDumpHex("Rrx:", pRrx, PRRX_SIZE);
	hdcp2x_cmmDumpHex("Rtx:", pRtx, PRTX_SIZE);



	//enableDebug(TRUE);
	
	dprintf("####Dump Rrx####\n");
	for( i = 0; i < PRRX_SIZE; i++)
	{
		dprintf("%02x ", pRrx[i]);
	}
	dprintf("\n");

	
	dprintf("####Dump Rtx####\n");
	for( i = 0; i < PRTX_SIZE; i++)
	{
		dprintf("%02x ", pRtx[i]);
	}
	dprintf("\n");

	dprintf("####End dump####\n");
	//enableDebug(FALSE);









	/*
		<2> Rrx ^ ctr: ctr = 0x0;
		
	*/
	for(i=0; i<PRRX_SIZE; i++)
	{
		IV[i + PRRX_SIZE] = pRrx[i] ^ ctr[i];
	}

	hdcp2x_cmmDumpHex("IV:", IV, IV_SIZE);
	
	/* Do AES encryption to get dkey0 and dkey1 */
	aes_setup(km,KM_SIZE,0,&aeskey);
	aes_ecb_encrypt(IV,dkey[0],&aeskey);





	/*
		<3> Rrx ^ ctr: ctr = 0x1;
		
	*/

	ctr[PRRX_SIZE-1] = 0x1;
	for(i=0; i<PRRX_SIZE; i++)
	{
		IV[i + PRRX_SIZE] = pRrx[i] ^ ctr[i];
	}




	
	aes_setup(km,KM_SIZE,0,&aeskey);
	aes_ecb_encrypt(IV,dkey[1],&aeskey);
	aes_done(&aeskey);

	/* Concatenate dkey0 and dkey2 to kd*/
	for(i=0,cnt=0;i<DKEY_SIZE;i++,cnt++){
		kd[cnt] = dkey[0][i];
	}
	for(i=0;i<DKEY_SIZE;i++,cnt++){
		kd[cnt] = dkey[1][i];
	} 




	use_hdcp2_2_keyd = 1;
	
	HDCP_log("use hdcp2.2 keyd [%d]\n\n", use_hdcp2_2_keyd);




	hdcp2x_cmmDumpHex("HDCP2_2: kd:", kd, sizeof(kd));

	return OK;

}



	
	
DTV_STATUS_T DRV_HDCP2_2_ComputeHprime 
(unsigned char *pH_prime, 
unsigned char Repeater, 
unsigned char *pRtx,
unsigned char  rx_version,
unsigned short  in_rx_cap_mask,
unsigned char  tx_version,
unsigned short  in_tx_cap_mask
)
{
	int iRet;
	hmac_state hmac;
	unsigned long resultLen = 32; //PH_PRIME_SIZE;
	//unsigned char	*rx_cap_mask = NULL;
	//unsigned char	*tx_cap_mask = NULL;

	int i = 0;
	int hash_idx = -1;

	
	unsigned char hmac_in[PRTX_SIZE + 1 + 2 + 1 + 2] = {0};




	HDCP_log( "Entering %s...\n", __FUNCTION__);

	//TEE REVIEW
	if(NULL == pRtx || NULL == pH_prime)
	{
		HDCP_log("invalid arguments\n");
		return NOT_OK;
	}



	zeromem(hmac_in,sizeof(hmac_in));

	//rx_cap_mask = (unsigned char *)&in_rx_cap_mask;
	//tx_cap_mask = (unsigned char *)&in_tx_cap_mask;

    if((hash_idx=register_hash(&sha256_desc))<0){                                  
        DRV_HDCP2_DBG(DRV_HDCP2_DBG_ERR, "%s: register_hash fail\n", __FUNCTION__);
        return 10;                                                                 
    }        


	HDCP_log(  "DRV_HDCP2_2_ComputeHprime repeater %d :\n",  Repeater);
	/* Do HMAC-SHA256 */
	iRet = hmac_init(&hmac,hash_idx, kd,KD_SIZE);
	if (iRet != OK) return iRet;



	/* pRtx ^ repeater*/
	pRtx[7] = pRtx[7] ^ Repeater;
	/* init hamc_in */

	for(i=0; i<PRTX_SIZE; i++)
		hmac_in[i] = pRtx[i];

	hmac_in[i]	 = rx_version;
	hmac_in[++i] = in_rx_cap_mask&0xff;
	hmac_in[++i] = (in_rx_cap_mask>>8)&0xff;
	hmac_in[++i] = tx_version;
	hmac_in[++i] = in_tx_cap_mask&0xff;
	hmac_in[++i] = (in_tx_cap_mask>>8)&0xff;

	hdcp2x_cmmDumpHex("hmac_in:", hmac_in, (PRTX_SIZE + 1 + 2 + 1 + 2));	
	
	iRet = hmac_process(&hmac,hmac_in,PRTX_SIZE + 1 + 2 + 1 + 2);
	if (iRet != OK) return iRet;
	
	iRet = hmac_done(&hmac,pH_prime,&resultLen);
	if (iRet != OK) return iRet;

	hdcp2x_cmmDumpHex("HDCP2_2: H:", pH_prime, PH_PRIME_SIZE);
   
	return OK;

}





	
	
	
	
	
	
	


DTV_STATUS_T DRV_HDCP2_2_ComputeLprime (unsigned char *pL_prime, unsigned char *pRn, unsigned char *pRrx)
{
	int iRet;
	unsigned long resultLen = 32; //PL_PRIME_SIZE;
	unsigned char kd_XOR_pRrx[KD_SIZE];
	int offset = KD_SIZE - PRRX_SIZE;
	int i;
	hmac_state hmac;   
	int hash_idx = -1;

	unsigned char hmac_in[PRN_SIZE<<1] = {0};

	HDCP_log(  ">>> %s  \n",__FUNCTION__);


	//TEE REVIEW
	if(NULL == pL_prime || NULL == pRn || NULL == pRrx)
	{
		HDCP_log("invalid arguments\n");
		return NOT_OK;
	}









	hash_idx = find_hash("sha256");

	/* kd XOR Rrx*/
	//x_memcpy(kd_XOR_pRrx,kd,KD_SIZE);

	// TEE REVIEW
	memcpy_s2s_chk(kd_XOR_pRrx, kd, KD_SIZE, NULL);
	
	for(i=0;i<PRRX_SIZE;i++){
		kd_XOR_pRrx[offset+i] ^= pRrx[i];
	}

	zeromem(hmac_in, sizeof(hmac_in));

	
	//x_memcpy(hmac_in,  pRn, PRN_SIZE);
	///x_memcpy(hmac_in + PRN_SIZE,  pRn, PRN_SIZE);

	// TEE REVIEW
	memcpy_n2s_chk(hmac_in,  pRn, PRN_SIZE, NULL);
	memcpy_n2s_chk(hmac_in + PRN_SIZE,  pRn, PRN_SIZE, NULL);


	hdcp2x_cmmDumpHex("hmac_in:", hmac_in, (PRN_SIZE<<1));	
	
	/* Do HMAC-SHA256 */
	iRet = hmac_init(&hmac,hash_idx,kd_XOR_pRrx,KD_SIZE);
	if (iRet != OK) return iRet;
	
	iRet = hmac_process(&hmac,hmac_in,(PRN_SIZE<<1));
	if (iRet != OK) return iRet;
	
	iRet = hmac_done(&hmac,pL_prime,&resultLen);
	if (iRet != OK) return iRet;
	
	hdcp2x_cmmDumpHex("HDCP2_2: L:", pL_prime, PL_PRIME_SIZE);
	  
	return OK;

}
	


/* for HDCP2.X ALIGN*/







/*
	for philips 2k14
*/
#define PHILIPS_2K14_ENCRYPTION_DATA_LENGTH (912 +64)
#define PHILIPS_2K14_HDCP2_PERM_KEYLEN_ENC_LEN  (948)
DTV_STATUS_T DRV_HDCP2_DecryptPermKey(unsigned char *keyEnc)
{


	int ret = 0;
//	int i = 0;
	char clear_key[PHILIPS_2K14_HDCP2_PERM_KEYLEN_ENC_LEN] = {0x0};
	char ptUserKey[16];
	long key_len = PHILIPS_2K14_HDCP2_PERM_KEYLEN_ENC_LEN;
	long lSrcLen = PHILIPS_2K14_HDCP2_PERM_KEYLEN_ENC_LEN;


	HDCP_log(  ">>> %s  \n",__FUNCTION__);


	//TEE REVIEW
	if(NULL == keyEnc)
	{
		HDCP_log("invalid arguments\n");
		return NOT_OK;
	}
	dprintf("\n\n@@@@@@ enc key len:(%d) @@@@@\n\n", PHILIPS_2K14_HDCP2_PERM_KEYLEN_ENC_LEN);	


	/* decrypt the perm encrypted key ... */
	hdcp2x_cmmDumpHex("enc key:", keyEnc, 16);

	//hdcp2x_cmmDumpHex("enc hdcp key:", clear_key, key_len);

    x_memset(ptUserKey, 0, sizeof(ptUserKey));

	ret = _teeSecureStorageDec((TEE_SECURESTORAGE_USERKEY_T * )ptUserKey,
					keyEnc, 
					lSrcLen,
					clear_key,
					&key_len);


	dprintf("\n\n\n ##### DRV_HDCP2_DecryptPermKey: ret[%d] #####\n\n\n\n", ret);

	hdcp2x_cmmDumpHex("clear hdcp key:",  clear_key, key_len);

	
	//x_memcpy(key_DecDcp, clear_key + 36 , KEYLEN_ENCDCP);
	//x_memcpy(lc128, clear_key , 16);


	// TEE REVIEW
	memcpy_s2s_chk(key_DecDcp, clear_key + 36 , KEYLEN_ENCDCP, NULL);
	memcpy_s2s_chk(lc128, clear_key , 16, NULL);
	
	return ret;

}




DTV_STATUS_T DRV_HDCP2_SetEncKeySet (unsigned char *pEncKeySet, UINT32 u4Length)
{    
    UINT16 u2Size;
    int i;
    symmetric_key aeskey; 
    unsigned char  clear_key_set[ENCRYPTION_KEYSET_LENGTH]; 

	unsigned char lc128_ok[KEYLEN_LC128] = {0xb5, 0xd8, 0xe9, 0xab, 0x5f, 0x8a, 0xfe, 0xca,
								 0x38, 0x55, 0xb1, 0xa5, 0x1e, 0xc9, 0xbc, 0x0f};


	unsigned char auprikey1[PRIV_KEY_SIZE] = 
	{
		0x07, 0x14, 0x01, 0x03, 0x1F, 0x5D, 0x34, 0x78, 0xCA, 0x31, 0xC3, 0xA3, 0x55, 0x4A, 0x9B, 0x24, 	
	};

	unsigned char auprikey2[PRIV_KEY_SIZE]={0x2D, 0xC6, 0xE0, 0xC5, 
										   0x49, 0xAA, 0xAB, 0x83, 
										   0x8C, 0x94, 0x84, 0xE1, 
										   0x90, 0xF5, 0x23, 0x12};


						  
    hash_state md;
    unsigned char  tmp[20];
    HDCP2_KEYDECDCP_T* pDecDcpKey;

	HDCP2_KEYSET_ENC_T *KeySet =  (HDCP2_KEYSET_ENC_T *)pEncKeySet;

    u2Size = (UINT16)sizeof(pEncKeySet);

    /* resetting keys */
    x_memset(km, 0, sizeof(km));
    x_memset(kd, 0, sizeof(kd));
    x_memset(ks, 0, sizeof(ks));
	use_hdcp2_2_keyd = 0;



	HDCP_log( "(%s) >>>>> keyset: type(%d) size(%d)\n", __FUNCTION__, KeySet->au1KeyType, KeySet->au1KeySize);
	HDCP_log( "(%s) >>>>> test key flag(%d)\n", __FUNCTION__, hdcp2x_kernel_space_test_key_enable);
	//TEE REVIEW
	if(NULL == pEncKeySet)
	{
		HDCP_log("invalid arguments\n");
		return NOT_OK;
	}



	if(0 != hdcp2x_kernel_space_test_key_enable)
	{

		HDCP2_KEYDECDCP_T facs_keydecdcp;

		HDCP_log("USE HDCP2.2 TEST KEY (R1):\n");

		XMEMCPY(lc128, hdcp_2_2_R1_test_key_lc128, 16);
		hdcp2x_cmmDumpHex("lc128:", lc128, 16);


		
		XMEMCPY((unsigned char*)&facs_keydecdcp.rKeyPrit, hdcp_2_2_R1_test_key_priv, 320);
		hdcp2x_cmmDumpHex("key private:", (unsigned char*)&facs_keydecdcp.rKeyPrit, 320);

		pDecDcpKey = &facs_keydecdcp;


		//x_memcpy(key_p,pDecDcpKey->rKeyPrit.au1P,sizeof(key_p));
		//x_memcpy(key_q,pDecDcpKey->rKeyPrit.au1Q,sizeof(key_q));
		//x_memcpy(key_dP,pDecDcpKey->rKeyPrit.au1DP,sizeof(key_dP));
		//x_memcpy(key_dQ,pDecDcpKey->rKeyPrit.au1DQ,sizeof(key_dQ));
		//x_memcpy(key_qInv,pDecDcpKey->rKeyPrit.au1QINV,sizeof(key_qInv));


		// TEE REVIEW
		memcpy_s2s_chk(key_p,pDecDcpKey->rKeyPrit.au1P,sizeof(key_p), NULL);
		memcpy_s2s_chk(key_q,pDecDcpKey->rKeyPrit.au1Q,sizeof(key_q), NULL);
		memcpy_s2s_chk(key_dP,pDecDcpKey->rKeyPrit.au1DP,sizeof(key_dP), NULL);
		memcpy_s2s_chk(key_dQ,pDecDcpKey->rKeyPrit.au1DQ,sizeof(key_dQ), NULL);
		memcpy_s2s_chk(key_qInv,pDecDcpKey->rKeyPrit.au1QINV,sizeof(key_qInv), NULL);

		



		return 0;
	}
	switch (KeySet->au1KeyType)
	{
		case HDCP2_KEY_SET_TYPE_NORMAL:	//ok
		{
			HDCP_log("DRV_HDCP2_SetEncKeySet:HDCP2_KEY_SET_TYPE_NORMAL:\n");

			if(HDCP2X_RX_NORMAL_ENC_KEY_SET_SIZE != KeySet->au1KeySize)
			{
				HDCP_log("invalid keyset len(%d)\n", KeySet->au1KeySize);
				return NOT_OK;
			}


			/* aes decryption the keyset */
		    aes_setup(auprikey1, PRIV_KEY_SIZE, 0, &aeskey);
		    zeromem(clear_key_set,sizeof(clear_key_set));
		

			hdcp2x_cmmDumpHex("enckey:", pEncKeySet, ENCRYPTION_KEYSET_LENGTH);

		    //x_memcpy(clear_key_set,pEncKeySet,ENCRYPTION_KEYSET_LENGTH);

			// TEE REVIEW
			memcpy_n2s_chk(clear_key_set,pEncKeySet,ENCRYPTION_KEYSET_LENGTH, NULL);
			
		    for ( i = 0 ; i < ENCRYPTION_KEYSET_SIZE ; i += 16)
		    {	
		        aes_ecb_decrypt((pEncKeySet+ i),(clear_key_set + i), &aeskey);
		    }

		    aes_done(&aeskey); 


			//x_memcpy(lc128, clear_key_set,      KEYLEN_LC128);

			// TEE REVIEW
			memcpy_s2s_chk(lc128, clear_key_set,      KEYLEN_LC128, NULL);

			hdcp2x_cmmDumpHex("lc128:", lc128, KEYLEN_LC128);

			
		    //x_memcpy(key_DecDcp, clear_key_set + 16, KEYLEN_ENCDCP);

			// TEE REVIEW
			memcpy_s2s_chk(key_DecDcp, clear_key_set + 16, KEYLEN_ENCDCP, NULL);



		    // sha1 check  
		    sha1_init(&md);
		    sha1_process(&md, (unsigned char*)key_DecDcp, 842);
		    sha1_done(&md, tmp);
		    
		    if (x_memcmp(tmp,(key_DecDcp+842), 20) != 0) 
			{
		       return 2;
		    }    




		}
		break;


		case HDCP2_KEY_SET_TYPE_SONY_2K14: //ok
		{
			HDCP_log("DRV_HDCP2_SetEncKeySet:HDCP2_KEY_SET_TYPE_SONY_2K14:\n");

			if(HDCP2X_RX_NORMAL_ENC_KEY_SET_SIZE != KeySet->au1KeySize)
			{
				HDCP_log("invalid keyset len(%d)\n", KeySet->au1KeySize);
				return NOT_OK;
			}



			/* aes decryption the keyset */
			aes_setup(auprikey2, PRIV_KEY_SIZE, 0, &aeskey);
			zeromem(clear_key_set,sizeof(clear_key_set));


			hdcp2x_cmmDumpHex("enckey:", pEncKeySet, ENCRYPTION_KEYSET_LENGTH);

			//x_memcpy(clear_key_set,pEncKeySet,ENCRYPTION_KEYSET_LENGTH);

			// TEE REVIEW
			memcpy_n2s_chk(clear_key_set,pEncKeySet,ENCRYPTION_KEYSET_LENGTH, NULL);
			
			for ( i = 0 ; i < ENCRYPTION_KEYSET_SIZE ; i += 16)
			{	
				aes_ecb_decrypt((pEncKeySet+ i),(clear_key_set + i), &aeskey);
			}

			aes_done(&aeskey); 


			//x_memcpy(lc128, clear_key_set,		KEYLEN_LC128);

			// TEE REVIEW
			memcpy_s2s_chk(lc128, clear_key_set, KEYLEN_LC128, NULL);

			hdcp2x_cmmDumpHex("lc128:", lc128, KEYLEN_LC128);

			
			//x_memcpy(key_DecDcp, clear_key_set + 16, KEYLEN_ENCDCP);

			//TEE REVIEW
			memcpy_s2s_chk(key_DecDcp, clear_key_set + 16, KEYLEN_ENCDCP, NULL);
			

			// sha1 check  
			sha1_init(&md);
			sha1_process(&md, (unsigned char*)key_DecDcp, 842);
			sha1_done(&md, tmp);
			
			if (x_memcmp(tmp,(key_DecDcp+842), 20) != 0) 
			{
			   return 2;
			}	 




		}
		break;




		case HDCP2_KEY_SET_TYPE_PHILIPS_2K13:
		{
			HDCP_log("DRV_HDCP2_SetEncKeySet:HDCP2_KEY_SET_TYPE_PHILIPS_2K13:\n");			


			if(HDCP2X_RX_NORMAL_ENC_KEY_SET_SIZE != KeySet->au1KeySize)
			{
				HDCP_log("invalid keyset len(%d)\n", KeySet->au1KeySize);
				return NOT_OK;
			}


			
			//x_memcpy(lc128,      pEncKeySet,              KEYLEN_LC128);

			// TEE REVIEW
			memcpy_n2s_chk(lc128,      pEncKeySet,              KEYLEN_LC128, NULL);

			
			hdcp2x_cmmDumpHex("lc128:", lc128, KEYLEN_LC128);

			
			//x_memcpy(key_DecDcp, pEncKeySet+KEYLEN_LC128, KEYLEN_ENCDCP);

			// TEE REVIEW
			memcpy_n2s_chk(key_DecDcp, pEncKeySet+KEYLEN_LC128, KEYLEN_ENCDCP, NULL);
			
			hdcp2x_cmmDumpHex("key_DecDcp:", key_DecDcp, KEYLEN_ENCDCP);







			
		}
		break;

		case HDCP2_KEY_SET_TYPE_PHILIPS_2K14:
		{
			HDCP_log("DRV_HDCP2_SetEncKeySet:HDCP2_KEY_SET_TYPE_PHILIPS_2K14 (len: ): \n", KeySet->au1KeySize);			


			if(HDCP2X_RX_PHILIPS_2K14_ENC_KEY_SET_SIZE != KeySet->au1KeySize)
			{
				HDCP_log("invalid keyset len(%d)\n", KeySet->au1KeySize);
				return NOT_OK;
			}



			DRV_HDCP2_DecryptPermKey(KeySet->au1EncKeySet);
			
		}
		break;


		case HDCP2_KEY_SET_TYPE_KM:
		{
		}
		break;

		default:
			HDCP_log("unknown hdcp_keyset type:\n");
			return 4;
			break;


	}


	if(0 != x_memcmp(lc128_ok, lc128, KEYLEN_LC128))
	{
		HDCP_log("!!!HDCP_KEY_INVALID!!!\n");	
		return 5;
	}



	/* save the private key*/
    pDecDcpKey = (HDCP2_KEYDECDCP_T*) key_DecDcp;
    //x_memcpy(key_p,pDecDcpKey->rKeyPrit.au1P,sizeof(key_p));
    //x_memcpy(key_q,pDecDcpKey->rKeyPrit.au1Q,sizeof(key_q));
    //x_memcpy(key_dP,pDecDcpKey->rKeyPrit.au1DP,sizeof(key_dP));
    //x_memcpy(key_dQ,pDecDcpKey->rKeyPrit.au1DQ,sizeof(key_dQ));
    //x_memcpy(key_qInv,pDecDcpKey->rKeyPrit.au1QINV,sizeof(key_qInv));


	// TEE REVIEW
	
    memcpy_s2s_chk(key_p,pDecDcpKey->rKeyPrit.au1P,sizeof(key_p), NULL);
    memcpy_s2s_chk(key_q,pDecDcpKey->rKeyPrit.au1Q,sizeof(key_q), NULL);
    memcpy_s2s_chk(key_dP,pDecDcpKey->rKeyPrit.au1DP,sizeof(key_dP), NULL);
    memcpy_s2s_chk(key_dQ,pDecDcpKey->rKeyPrit.au1DQ,sizeof(key_dQ), NULL);
    memcpy_s2s_chk(key_qInv,pDecDcpKey->rKeyPrit.au1QINV,sizeof(key_qInv), NULL);

	

    return OK;
}












/* for HDCP TX */
DTV_STATUS_T DRV_HDCP2_RetrvEkh_km(unsigned char *pEkh_km, unsigned char *m, unsigned char *id)
{

#if ENABLE_HDCP2X_TX



#ifdef WFD_USE_HDCP_FACSIMILE_KEY
	unsigned char *m_tmp;
	m_tmp = malloc(M_SIZE);
	if(!m_tmp)
	{
		HDCP_log( "Malloc m_tmp failed!\n");
		return NOT_OK;
	}
	
	x_memset(m_tmp, 0, M_SIZE);

	// TEE REVIEW
	
	//x_memcpy(pEkh_km, facskey_ekh_km, 16);
	memcpy_s2n_chk(pEkh_km, facskey_ekh_km, 16, NULL);

	
	//x_memcpy(m_tmp, facskey_m, 16);
	memcpy_s2s_chk(m_tmp, facskey_m, 16, NULL);


	
	//x_memcpy(m, m_tmp, M_SIZE);
	memcpy_s2n_chk(m, m_tmp, M_SIZE);




	

	if(m_tmp)
	{
		HDCP_log( "Get Ekh and m successful!\n");
		free(m_tmp);
	}
	return OK;
    
#else
	int i;
	for(i = 0; i < MAX_PAIRING_SIZE; i++)
	{	
		if(x_memcmp(pairing[i].id, id, KEYLEN_RXID) == 0)
		{
		    HDCP_log( "Found paired idx:%d, with RxID=[%02x %02x %02x %02x %02x]\n",
                   i, id[0],id[1],id[2],id[3],id[4]);

			//TEE REVIEW
			
			//x_memcpy(pEkh_km, pairing[i].Ekh_km, ENCKMKH_SIZE);
			memcpy_s2n_chk(pEkh_km, pairing[i].Ekh_km, ENCKMKH_SIZE, NULL);

			
			hdcp2x_cmmDumpHex("pEkh_km:", pEkh_km, ENCKMKH_SIZE);
		
			//x_memcpy(m, pairing[i].m, M_SIZE);
			memcpy_s2n_chk(m, pairing[i].m, M_SIZE, NULL);

			
			hdcp2x_cmmDumpHex("m:", m, M_SIZE);
			
			//x_memcpy(km, pairing[i].km, KM_SIZE);
			memcpy_s2s_chk(km, pairing[i].km, KM_SIZE, NULL);

			
			hdcp2x_cmmDumpHex("km:", km, KM_SIZE);
		
			
			return OK;
		}
	}
	return NOT_OK;
#endif



#else	//TX ENABLED 
	return NOT_OK;
#endif

}





DTV_STATUS_T DRV_HDCP2_SavePairingInfo(unsigned char *ID, unsigned char *pEkh_km, unsigned char *m)
{
#if ENABLE_HDCP2X_TX




	HDCP_log( "Entering pairing info store Ekh km and m!\n");

	int i = 0;

	int index = -1;
	int empty = -1;
	unsigned char empty_id[5] = {0, 0, 0, 0, 0};

	
	
	/*the rx id already existed ?*/
	for(i = 0; i < MAX_PAIRING_SIZE; i++)
	{	
		if(x_memcmp(pairing[i].id, ID, KEYLEN_RXID) == 0)
		{
		    HDCP_log( "DRV_HDCP2_SavePairingInfo: Found paired idx:%d, with RxID=[%02x %02x %02x %02x %02x]\n",
                   i, ID[0],ID[1],ID[2],ID[3],ID[4]);

			index = i;
			
		}



		if(-1 == empty && x_memcmp(pairing[i].id, empty_id, KEYLEN_RXID) == 0)
		{
			HDCP_log( "Found Empty idx:%d \n", i);
		
			empty = i;
		}

		
	}



	if(-1 == index && -1 != empty)
	{
		index  = empty;
	}



	if(-1 != index)
	{
		HDCP_log( "Save Paired Info on idx:[%x]", index);

		//TEE REVIEW
		
		//memcpy(pairing[index].id, ID, KEYLEN_RXID);
		//memcpy(pairing[index].Ekh_km, pEkh_km, ENCKMKH_SIZE);
		//memcpy(pairing[index].km, km, KM_SIZE);
		//memcpy(pairing[index].m, m, KEYLEN_M);
		
		memcpy_n2s_chk(pairing[index].id, ID, KEYLEN_RXID, NULL);
		memcpy_n2s_chk(pairing[index].Ekh_km, pEkh_km, ENCKMKH_SIZE, NULL);
		memcpy_s2s_chk(pairing[index].km, km, KM_SIZE, NULL);
		memcpy_n2s_chk(pairing[index].m, m, KEYLEN_M, NULL);

		

		return OK;
	}
	else
	{
		return NOT_OK;
	}



#else	//TX ENABLED 
	return NOT_OK;
#endif



}






DTV_STATUS_T DRV_HDCP2_0_ComputeVprime (
	unsigned char *pV_prime, 
	unsigned char *id_list, 
	unsigned char DEPTH,
	unsigned char DEVICE_COUNT,
	unsigned char MAX_DEVS_EXCEEDED,
	unsigned char MAX_CASCADE_EXCEEDED)
{
#if ENABLE_HDCP2X_TX



	int i = 0;
	int iRet;
	hmac_state hmac;
	unsigned long resultLen = 32; //PH_PRIME_SIZE;


	unsigned char *hmac_in = NULL;
	int size = 0;


	HDCP_log( "[User space] DRV_HDCP2_0_ComputeVprime:\n");


	//HDCP_log( "dump ID_LIST:\n");
	hdcp2x_cmmDumpHex("id_list:", id_list, DEVICE_COUNT*KEYLEN_RXID);		






	HDCP_log( "DEPTH:%d\n", DEPTH);	
	HDCP_log( "DEVICE_COUNT:%d\n", DEVICE_COUNT); 
	HDCP_log( "MAX_DEVS_EXCEEDED:%d\n", MAX_DEVS_EXCEEDED);	
	HDCP_log( "MAX_CASCADE_EXCEEDED:%d\n", MAX_CASCADE_EXCEEDED); 

	size = KEYLEN_RXID*DEVICE_COUNT +4;

	hmac_in = malloc(size);

	if(NULL == hmac_in)
	{
		HDCP_log( "[HDCP2X] hmac_in malloc err in DRV_HDCP2_0_ComputeVprime \n");
		iRet = NOT_OK;
		goto err;
	}

	memset(hmac_in, 0, size);

	

	/* init hmac_in */
	for(i=0; i<5*DEVICE_COUNT; i++)
	{
		hmac_in[i] = id_list[i];//id_list[4-i];
	}
		
	hmac_in[i++] = DEPTH;
	hmac_in[i++] = DEVICE_COUNT;	
	hmac_in[i++] = MAX_DEVS_EXCEEDED;		
	hmac_in[i++] = MAX_CASCADE_EXCEEDED;			
	

	/* print hmac_in */
	//HDCP_log( "dump hmac_in::\n");
	hdcp2x_cmmDumpHex("hmac_in", hmac_in, size);	


	
	if(register_hash(&sha256_desc)<0){
		//printf("[_HDCP2_DecryptRSAESOAEP] register_hash fail\n");
		iRet = 10;
		goto err;
	}

	/* Do HMAC-SHA256 */
	iRet = hmac_init(&hmac,0,kd,KD_SIZE);
	if (iRet != OK) 
	{
		goto err;
	}

	iRet = hmac_process(&hmac,hmac_in,size);
	if (iRet != OK) 
	{
		goto err;
	}

	
	iRet = hmac_done(&hmac,pV_prime,&resultLen);
	if (iRet != OK) 
	{
		goto err;
	}




	//HDCP_log( "dump vprime::\n");
	hdcp2x_cmmDumpHex("vprime:", pV_prime, resultLen);	

	
	iRet = OK;


err:

	if(NULL != hmac_in)
	{
		free(hmac_in);
		hmac_in = NULL;
	}

  
	return iRet;


#else	//TX ENABLED 
	return NOT_OK;
#endif


}







DTV_STATUS_T DRV_HDCP2_2_ComputeVprime (
	unsigned char *pV_prime, 
	unsigned char *id_list, 
	unsigned char DEPTH,
	unsigned char DEVICE_COUNT,
	unsigned char MAX_DEVS_EXCEEDED,
	unsigned char MAX_CASCADE_EXCEEDED,
	unsigned char HDCP2_0_REPEATER_DOWNSTREAM,
	unsigned char HDCP1_DEVICE_DOWNSTREAM,
	int seq_num_V)
{
#if ENABLE_HDCP2X_TX



	int i = 0;
    int iRet;
    hmac_state hmac;
    unsigned long resultLen = 32; //PH_PRIME_SIZE;

	unsigned char *hmac_in = NULL;
	int size = 0;






	HDCP_log( "DRV_HDCP2_2_ComputeVprime:\n");


	hdcp2x_cmmDumpHex("ID_LIST:", id_list, DEVICE_COUNT*KEYLEN_RXID);		




	HDCP_log( "DEPTH:%d\n", DEPTH);	
	HDCP_log( "DEVICE_COUNT:%d\n", DEVICE_COUNT);	
	HDCP_log( "MAX_DEVS_EXCEEDED:%d\n", MAX_DEVS_EXCEEDED);	
	HDCP_log( "MAX_CASCADE_EXCEEDED:%d\n", MAX_CASCADE_EXCEEDED);	
	HDCP_log( "HDCP2_0_REPEATER_DOWNSTREAM:%d\n", HDCP2_0_REPEATER_DOWNSTREAM);	
	HDCP_log( "HDCP1_DEVICE_DOWNSTREAM:%d\n", HDCP1_DEVICE_DOWNSTREAM);	
	HDCP_log( "seq_num_V:%d\n", seq_num_V);		

	size = KEYLEN_RXID*DEVICE_COUNT +1 +1 +1 +1 +1 +1 +3;

	hmac_in = malloc(size);

	if(NULL == hmac_in)
	{
		HDCP_log( "[HDCP2X] hmac_in malloc err in DRV_HDCP2_2_ComputeVprime \n");
		iRet = NOT_OK;
		goto err;
	}

	memset(hmac_in, 0, size);

	

	/* init hmac_in */
	for(i=0; i<KEYLEN_RXID*DEVICE_COUNT; i++)
	{
		hmac_in[i] = id_list[i];//id_list[4-i];
	}
		
	hmac_in[i++] = DEPTH;
	hmac_in[i++] = DEVICE_COUNT;	
	hmac_in[i++] = MAX_DEVS_EXCEEDED;		
	hmac_in[i++] = MAX_CASCADE_EXCEEDED;			
	hmac_in[i++] = HDCP2_0_REPEATER_DOWNSTREAM;				
	hmac_in[i++] = HDCP1_DEVICE_DOWNSTREAM;					
	hmac_in[i++] = (seq_num_V)&0xff;						
	hmac_in[i++] = (seq_num_V>>8)&0xff;							
	hmac_in[i++] = (seq_num_V>>16)&0xff;	
	

	/* print hmac_in */
	hdcp2x_cmmDumpHex("hmac_in:", hmac_in, size);	


	
    if(register_hash(&sha256_desc)<0){
        //printf("[_HDCP2_DecryptRSAESOAEP] register_hash fail\n");
        iRet = 10;
		goto err;
    }

    /* Do HMAC-SHA256 */
    iRet = hmac_init(&hmac,0,kd,KD_SIZE);
    if (iRet != OK) 
    {
		goto err;
	}

    iRet = hmac_process(&hmac,hmac_in,size);
    if (iRet != OK) 
    {
		goto err;
	}

    
    iRet = hmac_done(&hmac,pV_prime,&resultLen);
    if (iRet != OK) 
    {
		goto err;
	}


	hdcp2x_cmmDumpHex("vprime:", pV_prime, resultLen);	
	
    iRet = OK;


err:

	if(NULL != hmac_in)
	{
		free(hmac_in);
		hmac_in = NULL;
	}

  
    return iRet;
#else	//TX ENABLED 
		return NOT_OK;
#endif

}









DTV_STATUS_T DRV_HDCP2_ComputeMprime (
	unsigned char *pM_prime, 
	unsigned char *STREAMID_TYPE, 
	unsigned char *seq_num_M,
	          int  STREAMID_TYPE_LEN)
{
#if ENABLE_HDCP2X_TX



	int i = 0;
	int iRet;
	hmac_state hmac;
	unsigned long resultLen = 32; //PM_PRIME_SIZE;
	unsigned char *hmac_in = NULL;
	int size = 0;

    hash_state sha256;
	unsigned char kd_sha256_output[SHA256_OUTPUT_SIZE];



	HDCP_log( "[User space] DRV_HDCP2_ComputeMprime:\n");



	hdcp2x_cmmDumpHex("dump STREAMID_TYPE:", STREAMID_TYPE, STREAMID_TYPE_LEN);		
	hdcp2x_cmmDumpHex("seq_num_M:", seq_num_M, seq_num_M_LEN);	



	size = STREAMID_TYPE_LEN + seq_num_M_LEN;

	hmac_in = malloc(size);

	if(NULL == hmac_in)
	{
		HDCP_log( "[HDCP2X] hmac_in malloc err in DRV_HDCP2_ComputeMprime \n");
		iRet = NOT_OK;
		goto err;
	}

	memset(hmac_in, 0, size);

	
	#if 1
	/* init hmac_in */
	for(i=0; i<STREAMID_TYPE_LEN; i++)
	{
		hmac_in[i] = STREAMID_TYPE[i];
	}
		
	hmac_in[i++] = seq_num_M[0];
	hmac_in[i++] = seq_num_M[1];	
	hmac_in[i++] = seq_num_M[2];		
			
	#endif

	//hmac_in[5] = 0x20;

	/* print hmac_in */

	hdcp2x_cmmDumpHex("hmac_in:", hmac_in, size);	


	
	if(register_hash(&sha256_desc)<0){
		//printf("[_HDCP2_DecryptRSAESOAEP] register_hash fail\n");
		iRet = 10;
		goto err;
	}



	/* Do SHA256 on kd*/
    tom_sha256_init(&sha256);
    
    /* Do SHA256 hashing to get kd_sha256_output*/
    sha256_process(&sha256,kd,KD_SIZE);
    sha256_done(&sha256,kd_sha256_output);



    hdcp2x_cmmDumpHex("kd_sha256_output:", kd_sha256_output, sizeof(kd_sha256_output));




	/* Do HMAC-SHA256 */
	iRet = hmac_init(&hmac,0,kd_sha256_output,SHA256_OUTPUT_SIZE);
	if (iRet != OK) 
	{
		goto err;
	}

	iRet = hmac_process(&hmac,hmac_in,size);
	if (iRet != OK) 
	{
		goto err;
	}

	
	iRet = hmac_done(&hmac,pM_prime,&resultLen);
	if (iRet != OK) 
	{
		goto err;
	}


	hdcp2x_cmmDumpHex("Mprime", pM_prime, resultLen);	

	
	iRet = OK;


err:

	if(NULL != hmac_in)
	{
		free(hmac_in);
		hmac_in = NULL;
	}

  
	return iRet;


#else	//TX ENABLED 
	return NOT_OK;
#endif


}






DTV_STATUS_T DRV_HDCP2_VerifySignature(unsigned char *pKpub, unsigned char *cert)
{

#if ENABLE_HDCP2X_TX
	

	unsigned long siglen = KEYLEN_ROOTSIGN;
	unsigned long hashlen = 32;
	rsa_key key;
	rsa_key *pKey = &key;

    /* Note! Fixme!! 'hash' & 'hashlen' are not initialized. */
	unsigned char hash[32] = {0};

	int hash_idx;
	int prng_idx;
	int saltlen = 0;
	int stat,ret[2],i;
	int ii;


	hdcp2x_cmmDumpHex("pKpub:", pKpub, KEYLEN_ROOTSIGN+1);
	hdcp2x_cmmDumpHex("cert:",   cert,   KEYLEN_CERTRX);


	HDCP_log("CLEAR RX PUB KEY:\n");
	memset(kpubrx_n, 0, sizeof(kpubrx_n));
	memset(kpubrx_e, 0, sizeof(kpubrx_e));	
	


	
	//extern const ltc_math_descriptor ltm_desc;
	//ltc_mp = ltm_desc;     fix
    if(register_prng(&sprng_desc)<0){
        HDCP_log( "[_HDCP2_DecryptRSAESOAEP] register_prng fail\n");
        return NOT_OK;
    }
    if(register_hash(&sha256_desc)<0){
        HDCP_log( "[_HDCP2_DecryptRSAESOAEP] register_hash fail\n");
        return NOT_OK;
    }
    
    hash_idx = find_hash("sha256");
  	prng_idx = find_hash("sprng");

	if ((stat = ltc_init_multi(&pKey->e, &pKey->d, &pKey->N, &pKey->dQ, &pKey->dP, &pKey->qP, &pKey->p, &pKey->q, NULL)) != CRYPT_OK) {
        HDCP_log( "[DDI_HDCP2_VerifySignature] rsa_key initialization fail\n");
        return NOT_OK;
    }

	key.type = PK_PUBLIC;

	// fix
	ret[0] = mp_read_unsigned_bin(key.e, pKpub+KEYLEN_ROOTSIGN, 1);
    ret[1] = mp_read_unsigned_bin(key.N, pKpub, KEYLEN_ROOTSIGN);

	for(i=0;i<2;i++){
        if(ret[i]==CRYPT_ERROR){
            HDCP_log( "[_HDCP2_EncryptRSAESOAEP] mp_read_unsigned_bin fail\n");
            return NOT_OK;            
        }
    }

	HDCP_log( "hash_idx[%d]!\n", hash_idx);

	ii = hash_memory(hash_idx, cert, KEYLEN_RX_CERT_CONTENT, hash, &hashlen);
	
	HDCP_log( "ret1[%d]\n", ii);
	
	hdcp2x_cmmDumpHex("cert hash:",   hash,   32);



	ii = rsa_verify_hash_ex(cert+KEYLEN_RX_CERT_CONTENT, siglen, hash,hashlen,LTC_PKCS_1_V1_5,hash_idx,saltlen,&stat,&key);
	HDCP_log( "ret2[%d]!\n", ii);
	if(stat != 1)
	{
		HDCP_log( "The signature is invalid!\n");
		return NOT_OK;
	}
	else
	{
		HDCP_log( "Save RX KEY PUB\n");

		
		//x_memcpy(kpubrx_n, cert + 5,  sizeof(kpubrx_n));
		//x_memcpy(kpubrx_e, cert + 5 + 128, sizeof(kpubrx_e));

		// TEE REVIEW
		
		memcpy_n2s_chk(kpubrx_n, cert + 5,  sizeof(kpubrx_n), NULL);
		memcpy_n2s_chk(kpubrx_e, cert + 5 + 128, sizeof(kpubrx_e), NULL);	
		
	}
	
	rsa_free(&key);

	return OK;

#else	//TX ENABLED 
	return NOT_OK;
#endif


	
}



DTV_STATUS_T DRV_HDCP2_CheckRxID(unsigned char *id)
{
#if ENABLE_HDCP2X_TX



	int i;
	for(i = 0;i < MAX_PAIRING_SIZE; i++)
	{

		if(x_memcmp(id, pairing[i].id, KEYLEN_RXID) == 0)
		{
			HDCP_log( "DRV_HDCP2_CheckRxID: id[%d] found!!\n");
			return OK;
		}
	}
	return NOT_OK;

#else	//TX ENABLED 
	return NOT_OK;
#endif


	
}













DTV_STATUS_T DRV_HDCP2_ResetPairingInfo(unsigned char *id)
{
#if ENABLE_HDCP2X_TX



	int i;
	
	HDCP_log( "DRV_HDCP2_ResetPairingInfo:\n");
	
	for(i = 0; i < MAX_PAIRING_SIZE; i++)
	{	
		if(x_memcmp(pairing[i].id, id, KEYLEN_RXID) == 0)
		{
		    HDCP_log( "Found paired idx:%d, with RxID=[%02x %02x %02x %02x %02x]\n",
                   i, id[0],id[1],id[2],id[3],id[4]);

			x_memset(pairing[i].id, 0, KEYLEN_RXID);
			x_memset(pairing[i].Ekh_km, 0, ENCKMKH_SIZE);
			x_memset(pairing[i].m, 0, M_SIZE);
			x_memset(pairing[i].km, 0, KM_SIZE);

		
			
			return OK;
		}
	}

	return OK;

#else	//TX ENABLED 
	return NOT_OK;
#endif


	
}







DTV_STATUS_T DRV_HDCP2_EncryptRSAESOAEP(
				unsigned char *pEkm, 
				unsigned char *pKm, 
				unsigned char *pRX_Kpub_N, 
				unsigned char *pRX_Kpub_E)
{

#if ENABLE_HDCP2X_TX


/* MTK_PART, only Tx */

	unsigned long outlen = PEKPUB_KM_SIZE;
	rsa_key key;
	rsa_key *pKey = &key;
	unsigned long lparamlen = 0;
	int hash_idx = 0;
	int prng_idx = 0;
	int stat,i,ret[2];
	//prng_state sprng_prng;
	prng_state *sprng_prng = NULL;
	
	extern const ltc_math_descriptor ltm_desc;

	HDCP_log("Encrypt km and store km in secure storage!\n");

	//x_memcpy(km, pKm, KM_SIZE);

	// TEE REVIEW
	memcpy_n2s_chk(km, pKm, KM_SIZE, NULL);

	
	hdcp2x_cmmDumpHex("km:", pKm,KM_SIZE);

	//ltc_mp = ltm_desc;     
    if(register_prng(&sprng_desc)<0){
        HDCP_log("[_HDCP2_DecryptRSAESOAEP] register_prng fail\n");
        return NOT_OK;
    }
    if(register_hash(&sha256_desc)<0){
        HDCP_log("[_HDCP2_DecryptRSAESOAEP] register_hash fail\n");
        return NOT_OK;
    }
    
    hash_idx = find_hash("sha256");
    prng_idx = find_prng("sprng");

	if ((stat = ltc_init_multi(&pKey->e, &pKey->d, &pKey->N, &pKey->dQ, &pKey->dP, &pKey->qP, &pKey->p, &pKey->q, NULL)) != CRYPT_OK) {
        HDCP_log("[DDI_HDCP2_EncryptRSAESOAEP] rsa_key initialization fail\n");
        return NOT_OK;
    }

	key.type = PK_PUBLIC;

	ret[0] = mp_read_unsigned_bin(key.e,pRX_Kpub_E,KEYLEN_PUBEE);
	ret[1] = mp_read_unsigned_bin(key.N,pRX_Kpub_N,KEYLEN_PUBMN);

	hdcp2x_cmmDumpHex("pRX_Kpub_E:", pRX_Kpub_E,KEYLEN_PUBEE);
	hdcp2x_cmmDumpHex("pRX_Kpub_N:", pRX_Kpub_N,KEYLEN_PUBMN);
#if 0
	ret[0] = mp_read_unsigned_bin(key.e,key_E,KEY_E_SIZE);
    ret[1] = mp_read_unsigned_bin(key.N,key_N,KEY_N_SIZE);
#endif
	for(i=0;i<2;i++){
        if(ret[i]==CRYPT_ERROR){
            HDCP_log("[_HDCP2_EncryptRSAESOAEP] mp_read_unsigned_bin fail\n");
            return NOT_OK;            
        }
    }

	sprng_prng = x_mem_alloc(sizeof(prng_state));
	if (sprng_prng == NULL)
	{
		HDCP_log("malloc sprng_prng fail \n");
	}
	
	if(rsa_encrypt_key_ex(pKm, 
		KM_SIZE,
		pEkm,
		&outlen,
		NULL,
		lparamlen, 
		sprng_prng,
		prng_idx,
		hash_idx,
		LTC_PKCS_1_OAEP,
		&key) != 0)   
    {
        HDCP_log("[DDI_HDCP2_EncryptRSAESOAEP] rsa_encrypt_key_ex fail\n");
        return NOT_OK;
    }
	rsa_free(&key);
	x_mem_free(sprng_prng);


	hdcp2x_cmmDumpHex("Ekm:", pEkm, PEKPUB_KM_SIZE);

	return OK;

#else	//TX ENABLED 
	return NOT_OK;
#endif

	
}




DTV_STATUS_T DRV_HDCP2_Generate_km_for_tx(unsigned char *pEncKm)
{

#if ENABLE_HDCP2X_TX


	/* generate km */
	int iRet = NOT_OK;


	unsigned char tx_ekm[PEKPUB_KM_SIZE] = {0};


	if(0 == hdcp2x_kernel_space_test_key_enable)
	{
		/* not test mode:  generate km*/
		prng_state prng;
	    unsigned char entropy[10];
	    unsigned char tx_km[KM_SIZE] = {0};
	    int err;

	          
	    /* Generate random number km */    
	    /* start it */
	    if ((err = yarrow_start(&prng)) != CRYPT_OK) {
	        DRV_HDCP2_DBG(DRV_HDCP2_DBG_INFO, "HDCP TX yarrow start error: %s\n", error_to_string(err));
	    }
	    /* add entropy */
	    TzGetRandomBytes(entropy, 10);    
	    if ((err = yarrow_add_entropy(entropy, 10, &prng)) != CRYPT_OK) {
	        DRV_HDCP2_DBG(DRV_HDCP2_DBG_INFO, "HDCP TX yarrow add_entropy error: %s\n", error_to_string(err));
	    }
	    /* ready and read */
	    if ((err = yarrow_ready(&prng)) != CRYPT_OK) {
	        DRV_HDCP2_DBG(DRV_HDCP2_DBG_INFO, "HDCP TX yarrow ready error: %s\n", error_to_string(err));
	    }
	    yarrow_read(tx_km, KM_SIZE, &prng);
	    //DRV_HDCP2_DUMP16(DRV_HDCP2_DBG_VERB, "tx_km", tx_km, 0);
		hdcp2x_cmmDumpHex("tx_km:",  tx_km, KM_SIZE);

		//x_memcpy(km, tx_km, KM_SIZE);

		// TEE REVIEW
		memcpy_s2s_chk(km, tx_km, KM_SIZE, NULL);
		
		
	}
	else
	{

		HDCP_log("%s test mode!", __FUNCTION__);
		//x_memcpy(km, facskey_km_hdcp2_2_r1, KM_SIZE);

		// TEE REVIEW
		memcpy_s2s_chk(km, facskey_km_hdcp2_2_r1, KM_SIZE, NULL);

		
		
		//x_memcpy(kpubrx_n, facskey_kpubrx_n_hdcp2_2_r1, KEYLEN_PUBMN);
		//x_memcpy(kpubrx_e, facskey_kpubrx_e_hdcp2_2_r1, KEYLEN_PUBEE);
	}

   
	/* encrypte it with  keypub of rx */
	
	/* test */
	iRet = DRV_HDCP2_EncryptRSAESOAEP(tx_ekm,
					km,
					kpubrx_n,
					kpubrx_e);


	//x_memcpy(pEncKm, tx_ekm, PEKPUB_KM_SIZE);

	// TEE REVIEW
	memcpy_s2n_chk(pEncKm, tx_ekm, PEKPUB_KM_SIZE, NULL);


    return iRet;


#else	//TX ENABLED 
	return NOT_OK;
#endif


}


DTV_STATUS_T DRV_HDCP2_SetTestKeyFlag(int flag)
{

	hdcp2x_kernel_space_test_key_enable = flag;
 
    HDCP_log("DRV_HDCP2_SetTestKeyFlag (%d)\n", hdcp2x_kernel_space_test_key_enable);

	/* reset all the keys */


	if(1 == flag)
	{
		use_hdcp2_2_keyd = 1;
		HDCP_log("%s set use_hdcp2_2_keyd (%d)\n", __FUNCTION__, use_hdcp2_2_keyd);		
	}
	else
	{
		HDCP_log("%s set use_hdcp2_2_keyd (%d)\n", __FUNCTION__, use_hdcp2_2_keyd);			
		use_hdcp2_2_keyd = 0;
	}
	
	HDCP_log("reset all the keys\n");		
    x_memset(km, 0, sizeof(km));
    x_memset(kd, 0, sizeof(kd));
    x_memset(ks, 0, sizeof(ks));	

		
    return 0;
}




	
/* sync from lgewbos */
DTV_STATUS_T DRV_HDCP2_HMAC_SHA256 (unsigned char *output, 
												unsigned char *key,
												unsigned int keylen,
												unsigned char *input,
												unsigned int inputlen)
{
	int iRet;
	unsigned long resultLen = 32; //PL_PRIME_SIZE;

	hmac_state hmac;   



	/* Do HMAC-SHA256 */
	iRet = hmac_init(&hmac,0, key, keylen);
	if (iRet != OK) return iRet;
	
	iRet = hmac_process(&hmac, input, inputlen);
	if (iRet != OK) return iRet;
	
	iRet = hmac_done(&hmac,output, &resultLen);
	
	if (iRet != OK) return iRet;
	
	  
	return OK;

}


DTV_STATUS_T DRV_HDCP2_DataDecrypt(
				unsigned char *ucDstFrame,
				unsigned char *ucSrcFrame,
				unsigned long ulCount,
				unsigned char *rx_p				/* iv */
                )
{


    symmetric_CTR ctr;
    int err;
    int idx;
            
	if(NULL == ucDstFrame ||
	   NULL == ucSrcFrame ||
	   NULL == rx_p ||
	   0 == ulCount)
	{
		HDCP_log("decrypt invalid parameters\n");
		return NOT_OK;
	}


	hdcp2x_cmmDumpHex("in_data", ucSrcFrame, ulCount);
	hdcp2x_cmmDumpHex("iv", rx_p, 16);
	hdcp2x_cmmDumpHex("ksxorlc", ks_XOR_lc128, 16);



    /* use libtom crypto API. */
    
    /* register cipher first */
    if (register_cipher(&aes_desc) == -1)
    {
        HDCP_log("Error registering cipher aes.\n");
        return NOT_OK;
    }
    
    /* AES can be under rijndael or aes... try to find it */ 
    if ( (idx = find_cipher("aes")) == -1 )
    {
        if ((idx = find_cipher("rijndael")) == -1) 
        {
            HDCP_log("Cannot find cipher to use \n");
            return NOT_OK;
        }
    }

  

    /* start up CTR mode */
    if ((err = ctr_start(
                    idx, /* index of desired cipher */
                    rx_p, /* the initial vector */
                    ks_XOR_lc128, /* the secret key */
                    16, /* length of secret key (16 bytes) */
                    0, /* 0 == default # of rounds */
                    CTR_COUNTER_BIG_ENDIAN, /* Big endian counter */
                    &ctr) /* where to store the CTR state */
                    ) != CRYPT_OK) 
    {
        HDCP_log("ctr_start error: %s\n", error_to_string(err));
        return NOT_OK;
    }
    
    if ((err = ctr_decrypt( ucSrcFrame, /* ciphertext */
                    ucDstFrame, /* plaintext */
                    ulCount, /* length of plaintext */
                    &ctr) /* CTR state */
                    ) != CRYPT_OK) 
    {
        HDCP_log("ctr_decrypt error: %s\n", error_to_string(err));
        return NOT_OK;
    }

    /* terminate the stream */
    if ((err = ctr_done(&ctr)) != CRYPT_OK) {
        HDCP_log(" ctr_done error: %s\n", error_to_string(err));
        return NOT_OK;
    }


	hdcp2x_cmmDumpHex("out", ucDstFrame, ulCount);

    return 0;
}

DTV_STATUS_T DRV_HDCP2_GetPDKey(unsigned char *pData, UINT32 *u4length)
{
	int idx = -1;
	int err = -1;
	int i   =  0;
	
	unsigned char acGetPadData[PROTECTED_KEY_MAX] = {0};
	symmetric_ECB ecb;

	if(NULL == pData || u4length == NULL)
	{
		HDCP_log("DRV_HDCP2_GetPDKey invalid arguments\n");
		return NOT_OK;
	}

	//get 32 byte data: the first 16 byte is ks_XOR_lc128; and later 16 byte is 0
	
	//memcpy_s2n_chk(acGetPadData, ks_XOR_lc128, KS_SIZE, NULL);
	for(i = 0; i < KS_SIZE; i++)
	{
		acGetPadData[i] = ks_XOR_lc128[i];
	}
	
	for(i = KS_SIZE; i < PROTECTED_KEY_MAX; i++)
	{
		acGetPadData[i] = 0;
	}

	/* register cipher first */
	if (register_cipher(&aes_desc) == -1)
	{
		HDCP_log("[DRV_HDCP2_GetPDKey] Error registering cipher aes.\n");
		return NOT_OK;
	}

	/* AES can be under rijndael or aes... try to find it */ 
    if ( (idx = find_cipher("aes")) == -1 )
    {
        if ((idx = find_cipher("rijndael")) == -1) 
        {
            HDCP_log("[DRV_HDCP2_GetPDKey] Cannot find cipher to use \n");
            return NOT_OK;
        }
    }

	err = ecb_start(idx, auprikey_for_pdkey, PRIV_KEY_SIZE, 0, &ecb);
	if (err != OK)
	{
		HDCP_log("[DRV_HDCP2_GetPDKey] ecb_start() failed\n");

		return NOT_OK;
	}

	err = ecb_encrypt(acGetPadData, pData, PROTECTED_KEY_MAX, &ecb);
	if (err != OK)
	{
		HDCP_log("[DRV_HDCP2_GetPDKey] ecb_encrypt() failed\n");

		return NOT_OK;
	}

	err = ecb_done(&ecb);
	if (err != OK)
	{
		HDCP_log("[DRV_HDCP2_GetPDKey]ecb_done() failed\n");

		return NOT_OK;
	}

	*u4length = PROTECTED_KEY_MAX;
	
	return OK;
}

DTV_STATUS_T DRV_HDCP2_SetPDKey(unsigned char *pData, UINT32 u4length)
{
	int i = 0;
	int idx = -1;
	int err = -1;
	
	symmetric_ECB ecbkey;
	unsigned char clear_pdkey[PROTECTED_KEY_MAX] = {0};

	//TEE REVIEW
	if(NULL == pData || u4length != PROTECTED_KEY_MAX)
	{
		HDCP_log("[DRV_HDCP2_SetPDKey] invalid arguments\n");
		return NOT_OK;
	}

	/* register cipher first */
	if (register_cipher(&aes_desc) == -1)
	{
		HDCP_log("[DRV_HDCP2_SetPDKey] Error registering cipher aes.\n");
		return NOT_OK;
	}

	/* AES can be under rijndael or aes... try to find it */ 
    if ( (idx = find_cipher("aes")) == -1 )
    {
        if ((idx = find_cipher("rijndael")) == -1) 
        {
            HDCP_log("[DRV_HDCP2_SetPDKey] Cannot find cipher to use \n");
            return NOT_OK;
        }
    }

	err = ecb_start(idx, auprikey_for_pdkey, PRIV_KEY_SIZE, 0, &ecbkey);
	if (err != OK)
	{
		HDCP_log("[DRV_HDCP2_SetPDKey] ecb_start() failed\n");

		return NOT_OK;
	}

	err = ecb_decrypt(pData, clear_pdkey, PROTECTED_KEY_MAX, &ecbkey);
	if (err != OK)
	{
		HDCP_log("[DRV_HDCP2_SetPDKey] ecb_decrypt() failed\n");

		return NOT_OK;
	}

	err = ecb_done(&ecbkey);
	if (err != OK)
	{
		HDCP_log("[DRV_HDCP2_SetPDKey] ecb_done() failed\n");

		return NOT_OK;
	}

	//memcpy(ks_XOR_lc128, clear_pdkey, KEYLEN_LC128);
	//memcpy_s2n_chk(ks_XOR_lc128, clear_pdkey, KEYLEN_LC128, NULL);
	
	for(i = 0; i < KEYLEN_LC128; i++)
	{
		ks_XOR_lc128[i] = clear_pdkey[i];
	}

	return OK;
}

