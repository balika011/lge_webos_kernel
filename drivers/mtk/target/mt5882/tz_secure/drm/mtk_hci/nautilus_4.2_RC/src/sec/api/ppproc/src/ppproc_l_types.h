/** @addtogroup ppproc ppproc
 *  @ingroup api
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010,2011 Sony Corporation.
 */

#ifndef PPPROC_L_TYPES_H
#define PPPROC_L_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

    /* Request Payload */

    /* 4CC of Box type */
#define L_BOXTYPE_NPID byte_4CHARCODE('N','P','I','D')
#define L_BOXTYPE_NPPU byte_4CHARCODE('N','P','P','U')
#define L_BOXTYPE_NPPR byte_4CHARCODE('N','P','P','R')
#define L_BOXTYPE_NROL byte_4CHARCODE('N','R','O','L')
#define L_BOXTYPE_OPID byte_4CHARCODE('O','P','I','D')
#define L_BOXTYPE_OPPU byte_4CHARCODE('O','P','P','U')
#define L_BOXTYPE_OPPR byte_4CHARCODE('O','P','P','R')
#define L_BOXTYPE_INID byte_4CHARCODE('I','N','I','D')
#define L_BOXTYPE_INPU byte_4CHARCODE('I','N','P','U')
#define L_BOXTYPE_INPR byte_4CHARCODE('I','N','P','R')
#define L_BOXTYPE_LROL byte_4CHARCODE('L','R','O','L')
#define L_BOXTYPE_RROL byte_4CHARCODE('R','R','O','L')
#define L_BOXTYPE_IROL byte_4CHARCODE('I','R','O','L')
#define L_BOXTYPE_LOID byte_4CHARCODE('L','O','I','D')
#define L_BOXTYPE_LOPU byte_4CHARCODE('L','O','P','U')
#define L_BOXTYPE_LOPR byte_4CHARCODE('L','O','P','R')
#define L_BOXTYPE_ROID byte_4CHARCODE('R','O','I','D')
#define L_BOXTYPE_ROPU byte_4CHARCODE('R','O','P','U')
#define L_BOXTYPE_ROPR byte_4CHARCODE('R','O','P','R')
#define L_BOXTYPE_CDLC byte_4CHARCODE('C','D','L','C')
#define L_BOXTYPE_LDLC byte_4CHARCODE('L','D','L','C')
#define L_BOXTYPE_BKBC byte_4CHARCODE('B','K','B','C')
#define L_BOXTYPE_CRLD byte_4CHARCODE('C','R','L','D')

#define L_BOX_BODYSIZE(boxsize) (boxsize-(sizeof(u_int32_t)*2))

#ifdef __cplusplus
}
#endif

#endif /* PPPROC_L_TYPES_H */
/** @} */
