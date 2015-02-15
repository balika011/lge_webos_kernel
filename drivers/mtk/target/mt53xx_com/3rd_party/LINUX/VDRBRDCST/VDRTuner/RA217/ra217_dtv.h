/*------------------------------------------------------------------------------

 Copyright 2010 Sony Corporation

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
 No part of this file may be copied, modified, sold, and distributed in any
 form or by any means without prior explicit permission in writing from
 Sony Corporation.

 Date: 2010/07/07
 Revision: 1.0.1.0

------------------------------------------------------------------------------*/

#ifndef RA217_DTV_H
#define RA217_DTV_H

/* Digital terrestrial System definitions */
typedef enum {
	RA217_DTV_SYSTEM_UNKNOWN,
	RA217_DTV_SYSTEM_QAM,      /* QAM */
	RA217_DTV_SYSTEM_ATSC,     /* ATSC(8VSB) */
	RA217_DTV_SYSTEM_ISDBT,    /* ISDB-T */
	RA217_DTV_SYSTEM_DVBT_6,   /* DVB-T 6MHzBW */
	RA217_DTV_SYSTEM_DVBT_7,   /* DVB-T 7MHzBW */
	RA217_DTV_SYSTEM_DVBT_8,   /* DVB-T 8MHzBW */
	RA217_DTV_SYSTEM_DVBT2_5,  /* DVB-T2 5MHzBW */
	RA217_DTV_SYSTEM_DVBT2_6,  /* DVB-T2 6MHzBW */
	RA217_DTV_SYSTEM_DVBT2_7,  /* DVB-T2 7MHzBW */
	RA217_DTV_SYSTEM_DVBT2_8,  /* DVB-T2 8MHzBW */
	RA217_DTV_SYSTEM_DVBC,     /* DVB-C */
	RA217_DTV_SYSTEM_DTMB      /* DTMB */
} ra217_dtv_system_t;

#endif /* RA217_DTV_H */
