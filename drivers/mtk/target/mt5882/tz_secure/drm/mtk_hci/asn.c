/*
 * Nautilus version 3.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright (C) 2004,2005,2006,2007,2008,2009 Sony Corporation, All Rights Reserved.
 */

/*===========================================================================*
 * ASN.1 BER parser/composer/accessor
 * $Id: //DTV/RELEASE_BR/r_DTV_X_IDTV1401_002306_12_001_37_001/kernel/drivers/mtk/target/mt5882/tz_secure/drm/mtk_hci/asn.c#1 $
 *===========================================================================*/

#include "basic_types.h"
#include "dbg_api.h"
#include "asn_api.h"
#include "binstr_api.h"
//#include "hci_api_util.h"

/*
 * compat
 */

#define _OK			ERR_OK
#define _NG			ERR_NG
#define _TRUE			1
#define _FALSE			0

#define TASN_DUMP		1
#define TASN_PREFIX(x)		asn_##x
typedef asn_t			TASN_T;
typedef asn_time_t		TASN_TIME_T;
typedef retcode_t		TASN_RESULT_T;
typedef unsigned char		TASN_UCHAR;
typedef u_int32_t		TASN_UINT32;

/*
 * defs
 */
#define ASN_MALLOC(size)	tom_malloc(size)
#define ASN_FREE(ptr)		tom_free(ptr)
#define ASN_REALLOC(ptr, size)	tom_realloc(ptr, size)

#define ASN_FLAG_INDEFINITE	0x01	/* use indefinite length     */
#define ASN_FLAG_RAW		0x02	/* use raw encoded image     */
#define ASN_FLAG_REFERENCE	0x04	/* body refers its source    */
#define ASN_FLAG_ENCODED	0x08	/* already ASN.1 BER encoded */
#define ASN_FLAG_BUILTIN	0x80	/* built-in variable         */

#define IS_INDEFINITE(v)	((v)->flags & ASN_FLAG_INDEFINITE)
#define IS_RAW(v)		((v)->flags & ASN_FLAG_RAW)
#define IS_REFERENCE(v)		((v)->flags & ASN_FLAG_REFERENCE)
#define IS_ENCODED(v)		((v)->flags & ASN_FLAG_ENCODED)
#define IS_BUILTIN(v)		((v)->flags & ASN_FLAG_BUILTIN)

#define ASN_DECODE_NORMAL	0
#define ASN_DECODE_NOCOPY	1
#define ASN_DECODE_CHECK	2
#define ASN_DECODE_GET_LEN	3

#define ASN_INDEF_LEN_ID        -1

static const TASN_T anull = { ASN_CLASS_UNIVERSAL, 0, ASN_TAG_NULL, ASN_FLAG_BUILTIN, 0, { 0 }, 0, 0 };


/*--------------------------------------------------------------
 * internal prototypes
 *--------------------------------------------------------------*/

static TASN_RESULT_T
_asn_new(
    int			in_class,
    int			in_stflag,
    int			in_tag,
    TASN_T**		out_asn);

static TASN_RESULT_T
_asn_new_init(
    int			in_class,
    int			in_stflag,
    int			in_tag,
    np_size_t		in_length,
    void*		in_body,
    int			is_nocopy,
    TASN_T**		out_asn);


/*--------------------------------------------------------------
 * BER composer
 *--------------------------------------------------------------*/

static TASN_RESULT_T
_asn_get_length_size(
    np_size_t	in_length,
    np_size_t*	out_size)
{
    if ( !(in_length >> 7) )
        *out_size = 1;
    else if ( !(in_length >> 8) )
        *out_size = 2;
    else if ( !(in_length >> 16) )
        *out_size = 3;
    else if ( !(in_length >> 24) )
        *out_size = 4;
    else
        *out_size = 5;
    return _OK;
}

static TASN_RESULT_T
_asn_get_serialized_size(
    TASN_T*		in_asn,
    np_size_t*		out_length)
{
    TASN_RESULT_T ret = _OK;
    TASN_T *av = in_asn;
    np_size_t len = 0;

    if ( !av || !out_length )
        RET(ERR_ASN_INPUT);

    if ( IS_ENCODED(av) ) {
        *out_length = av->_src_size;
    } else if ( av->stflag ) {
        np_size_t i, total = 0;
        for (i = 0; i < av->length; i++) {
            RET(_asn_get_serialized_size(av->v.seq[i], &len));
            total += len;
        }
        if ( IS_INDEFINITE(av) ) {
            *out_length = 2 + total + 2;
        } else {
            RET(_asn_get_length_size(total, &len));
            *out_length = 1 + len + total;
        }
    } else {
        if ( IS_INDEFINITE(av) ) {
            *out_length = 2 + av->length + 2;
        } else {
            RET(_asn_get_length_size(av->length, &len));
            *out_length = 1 + len + av->length;
        }
    }

EXIT:
    return ret;
}

static TASN_RESULT_T
_asn_set_length(
    np_size_t		in_length,
    unsigned char*	out_buf)
{
    TASN_RESULT_T ret = _OK;
    unsigned char *ptr = out_buf;
    np_size_t len, vlen;

    RET(_asn_get_length_size(in_length, &len));
    vlen = len;

    while ( len-- ) {
        if ( len == 0 && vlen > 1 )
            ptr[len] = 0x80 | (vlen - 1);
        else
            ptr[len] = in_length & 0xff;
        in_length >>= 8;
    }

EXIT:
    return ret;
}

static TASN_RESULT_T
_asn_encode(
    TASN_T*		in_asn,
    unsigned char**	out_ptr)
{
    TASN_RESULT_T ret = _OK;
    TASN_T *av = in_asn;
    unsigned char *ptr;
    np_size_t len;

    if ( !av || !out_ptr )
        RET(ERR_ASN_INPUT);

    ptr = *out_ptr;

    if ( IS_ENCODED(av) ) {
        tom_memcpy(ptr, av->_src_addr, av->_src_size);
        *out_ptr = ptr + av->_src_size;
    } else if ( IS_RAW(av) ) {
        ptr[0] = (av->clazz << 6) | (av->stflag ? 0x20 : 0x00) | (av->tag & 0x1f);
        if ( IS_INDEFINITE(av) ) {
            if ( !av->stflag )
                RET(ERR_ASN_INDEFLEN);
            ptr[1] = 0x80;
            tom_memcpy(ptr+2, av->v.body, av->length);
            ptr[2+av->length+0] = 0x00;
            ptr[2+av->length+1] = 0x00;
            *out_ptr = ptr + 2 + av->length + 2;
        } else {
            RET(_asn_set_length(av->length, ptr+1));
            RET(_asn_get_length_size(av->length, &len));
            tom_memcpy(ptr+1+len, av->v.body, av->length);
            *out_ptr = ptr + 1 + len + av->length;
        }
    } else if ( av->stflag ) {
        np_size_t i, total;
        /* dump all */
        ptr += 2;
        for (i = 0; i < av->length; i++) {
            RET(_asn_encode(av->v.seq[i], &ptr));
        }
        total = ptr - *out_ptr - 2; ptr = *out_ptr;
        /* fill header & copy body & next pointer */
        ptr[0] = (av->clazz << 6) | 0x20 | (av->tag & 0x1f);
        if ( IS_INDEFINITE(av) ) {
            ptr[1] = 0x80;
            ptr[2+total+0] = 0x00;
            ptr[2+total+1] = 0x00;
            *out_ptr = ptr + 2 + total + 2;
        } else {
            RET(_asn_get_length_size(total, &len));
            tom_memmove(ptr+1+len, ptr+2, total);
            RET(_asn_set_length(total, ptr+1));
            *out_ptr = ptr + 1 + len + total;
        }
    } else {
        /* fill header & copy body & next pointer */
        ptr[0] = (av->clazz << 6) | (av->tag & 0x1f);
        if ( IS_INDEFINITE(av) ) {
            RET(ERR_ASN_INDEFLEN);
        } else {
            RET(_asn_set_length(av->length, ptr+1));
            RET(_asn_get_length_size(av->length, &len));
            tom_memcpy(ptr+1+len, av->v.body, av->length);
            *out_ptr = ptr + 1 + len + av->length;
        }
    }

EXIT:
    return ret;
}


/*--------------------------------------------------------------
 * BER parser
 *--------------------------------------------------------------*/

static TASN_RESULT_T
_asn_get_length(
    unsigned char*	in_start,
    unsigned char*	in_end,
    unsigned char**	out_next,
    int*		out_length)
{
    u_int32_t i;
    int length = 0;
    u_int32_t len_octets = (in_start[0] & 0x7f);
    u_int32_t asn_in_sz = in_end - in_start;

    if ( in_start[0] & 0x80 ) {
	/* As per the spec the number of len octets must be less than 4. Also
	 * check the asn buffer is not malformed and contains the len octets
	 */
        if ( (len_octets > ASN_MAX_NUM_LEN_OCTS)  || 
	     (len_octets > (asn_in_sz-1)) )
            return ERR_ASN_SIZE;
	
        for (i = 0; i < len_octets; i++) {
            length <<= 8;
            length += in_start[i+1];
        }

        if ( length < 0 )
            return ERR_ASN_SIZE;

        *out_length = length;
        *out_next = in_start + i + 1;

    } else {
        *out_length = in_start[0];
        *out_next = in_start + 1;
    }

    if ( in_end && (in_end < (*out_next + (*out_length))) )
	return ERR_ASN_OVERFLOW;

    return _OK;
}


static TASN_RESULT_T
_asn_decode(
    int			in_mode,
    unsigned char*	in_start,
    unsigned char*	in_end,
    np_size_t*          out_len,
    unsigned char**	out_next,
    TASN_T**		out_asn)
{
    TASN_RESULT_T ret = _OK;
    unsigned char *ptr = in_start;
    int len; 
    int tag = ASN_TAG_GET(ptr[0]);
    int stflag = ASN_CLASS_IDENTIFER_GET(ptr[0]);
    int clazz = ASN_CLASS_GET(ptr[0]);
    TASN_T *av = NULL;
    np_size_t asn_len;
    u_int32_t in_elem_sz = in_end - in_start;    
    TASN_T *child=NULL;

    /* Protect against being passed a malformed ASN buffer */
    if ( in_elem_sz <  ASN_ELEM_MIN_SEQ_SZ )
	RET(ERR_ASN_SIZE);

    if ( stflag ) {

        if ( (in_mode != ASN_DECODE_CHECK) && 
             (in_mode != ASN_DECODE_GET_LEN) ) {
            RET(_asn_new(clazz, stflag, tag, &av));
        }

        if ( ptr[1] == ASN_LEN_FORM_INDEF ) {
            if ( (in_mode != ASN_DECODE_CHECK) && 
                 (in_mode != ASN_DECODE_GET_LEN) )
                av->flags |= ASN_FLAG_INDEFINITE;
            len = ASN_INDEF_LEN_ID; 
            ptr += 2;
        } else {
            RET(_asn_get_length(ptr + 1, in_end, &ptr, &len));
            in_end = ptr + len;
        }

        while ( !in_end || ptr < in_end ) {
            if ( (len < 0 && !ptr[0] && !ptr[1]) ) {
                ptr += 2;
                break;
            }
            RET(_asn_decode(in_mode, ptr, in_end, &asn_len, &ptr, &child));
            if ( (in_mode != ASN_DECODE_CHECK) &&
        	 (in_mode != ASN_DECODE_GET_LEN) ) {
                RET(TASN_PREFIX(Append)(av, child)); 
                child=NULL;
	    }
        }

    } else {

        /* An indefinite length for a primitive identifer is not allowed */
        if ( ptr[1] == ASN_LEN_FORM_INDEF ) {
            RET(ERR_ASN_INDEFLEN);
        } else {
            RET(_asn_get_length(ptr + 1, in_end, &ptr, &len));

            if ( (in_mode != ASN_DECODE_CHECK) &&
		 (in_mode != ASN_DECODE_GET_LEN) ) {
                RET(_asn_new_init(clazz, stflag, tag, 
				  (np_size_t)len, ptr, 
				  in_mode == ASN_DECODE_NOCOPY, &av));
	    }
            ptr += len;
        }
    }

    /* save hints */
    if ( (in_mode != ASN_DECODE_CHECK) &&
	 (in_mode != ASN_DECODE_GET_LEN) ) {
        av->_src_addr = in_start;
        av->_src_size = ptr - in_start;
    }
    
    if ( out_len )  *out_len  = ptr - in_start;
    if ( out_next ) *out_next = ptr;
    if ( out_asn  ){
        *out_asn  = av;
        av=NULL;
    }

EXIT:

    if( av ) { TASN_PREFIX(Free)(av);    }
    if(child){ TASN_PREFIX(Free)(child); }

    return ret;
}

/* Extract the length of the encoded data. The benefits of this call is
 * avoidance of generating the ASN tree. There are 3 possible cases to
 * extract the len for, the encoded data:
 * 1. is primitive. In this case the length must be definite and is
 *    extracted from the first encoded identifier
 * 2. is constructed and has a definite length which is extracted from
 *    the first encoded identifier
 * 3. is constructed and has an indefinite len. In this case the encoded
 *    data is parsed to calculate the len but the ASN tree is not
 *    generated.
 */

static TASN_RESULT_T
_asn_info_get(unsigned char*  in_elem,
	      np_size_t       in_elem_sz,
	      unsigned char** out_next_elem,
	      np_size_t*      out_content_len,
	      np_size_t*      out_elem_len)
{
    retcode_t ret;
    int       stflag;
    u_int8_t* in_elem_end = in_elem + in_elem_sz;

    /* Protect against being passed a malformed ASN buffer */
    if ( in_elem_sz <  ASN_ELEM_MIN_SEQ_SZ )
	RET(ERR_ASN_SIZE);

    stflag = ASN_CLASS_IDENTIFER_GET(in_elem[0]);
    if ( ASN_CLASS_CONSTRUCTED == stflag ) {

	/* Return the length of the identifier if the length is definite
	 * and the class idenifier is constructed. Case2
	 */
	if ( (ASN_LEN_FORM_INDEF != in_elem[1]) ) {
	    RET(_asn_get_length( (in_elem + 1), in_elem_end, 
				 out_next_elem, (int*)out_content_len) );
	    *out_elem_len = (*out_next_elem - in_elem) + *out_content_len;
	} else {

	    /* Case3 */
	    RET(_asn_decode(ASN_DECODE_GET_LEN,
			    (unsigned char *)in_elem,
			    (unsigned char *)in_elem_end,
			    out_content_len, out_next_elem, NULL));

	    *out_next_elem = in_elem + (ASN_INDEF_ELEM_ID_SZ +
					ASN_INDEF_ELEM_LEN_SZ);

	    /* For the indefinite case we need to adjust the content len
	     * to account for the indef identifer, length and end octects
	     */
	    *out_elem_len = *out_content_len;
	    *out_content_len = *out_content_len - (ASN_INDEF_ELEM_ID_SZ +
						   ASN_INDEF_ELEM_LEN_SZ +
						   ASN_INDEF_ELEM_END_SZ);
	}

    } else {
	
	/* Return the length for the primitive only if the len is
	 * definite as per the ASN encoding rule spec X.690
	 */
	if ( in_elem[1] == ASN_LEN_FORM_INDEF ) {
            RET(ERR_ASN_INDEFLEN);
        } else {
	    /* Case1 */
            RET(_asn_get_length( (in_elem + 1), in_elem_end, 
				 out_next_elem, (int*)out_content_len) );
	    *out_elem_len = (*out_next_elem - in_elem) + *out_content_len;
	}
    }

    ret = ERR_OK;
 EXIT:
    return ret;
}


/*--------------------------------------------------------------
 * abstract data accessor
 *--------------------------------------------------------------*/

static TASN_RESULT_T
_asn_new(
    int			in_class,
    int			in_stflag,
    int			in_tag,
    TASN_T**		out_asn)
{
    TASN_T *av;

    if ( !out_asn || in_class >= 4 || in_tag >= 0x20 )
        return ERR_ASN_INPUT;

    av = (TASN_T *)ASN_MALLOC(sizeof(TASN_T));
    if ( av == NULL ) return ERR_NOMEM;

    tom_memset(av, 0, sizeof(TASN_T));
    av->clazz    = in_class;
    av->stflag   = in_stflag;
    av->tag      = in_tag;
    /* av->flags    = 0; */
    /* av->length   = 0; */
    *out_asn = av;

    return _OK;
}

static TASN_RESULT_T
_asn_new_init(
    int			in_class,
    int			in_stflag,
    int			in_tag,
    np_size_t		in_length,
    void*		in_body,
    int			is_nocopy,
    TASN_T**		out_asn)
{
    TASN_RESULT_T ret = _OK;
    TASN_T *av = NULL;

    if (in_length && !in_body)
        RET(ERR_ASN_INPUT);

    RET(_asn_new(in_class, in_stflag, in_tag, &av));

    if ( is_nocopy ) {
        av->length = in_length;
        av->v.body = in_body;
        av->flags |= ASN_FLAG_REFERENCE;
    } else if ( in_length ) {
        av->v.body = (unsigned char *)ASN_MALLOC(in_length);
        if ( av->v.body == NULL )
            RET(ERR_NOMEM);
        av->length = in_length;
        tom_memcpy(av->v.body, in_body, in_length);
    } else {
        av->length = 0;
        av->v.body = NULL;
    }

    *out_asn = av;

EXIT:
    if ( ret != _OK ) {
        TASN_PREFIX(Free)(av); av = NULL;
    }
    return ret;
}

static TASN_RESULT_T
_asn_traverse_depth_first(
    TASN_T*		in_asn,
    TASN_RESULT_T	(*func)(TASN_T *, void *),
    void*		in_arg)
{
    TASN_RESULT_T ret = _OK;
    int i;

    if ( !in_asn )
        return _OK;

    if ( in_asn->stflag ) {
        for (i = 0; i < (int)in_asn->length; i++) {
            RET(_asn_traverse_depth_first(in_asn->v.seq[i], func, in_arg));
        }
    }

EXIT:
    return (ret == _OK) ? func(in_asn, in_arg) : ret;
}

static TASN_RESULT_T
_asn_free(
    TASN_T*		in_asn,
    void*		in_null)
{
    (void)in_null;
    if ( !IS_BUILTIN(in_asn) ) {
        if ( in_asn->stflag ) {
            ASN_FREE(in_asn->v.seq); in_asn->v.seq = NULL;
        } else {
            if ( !IS_REFERENCE(in_asn) ) {
                ASN_FREE(in_asn->v.body);
            }
            in_asn->v.body = NULL;
        }
        ASN_FREE(in_asn);
    }
    return _OK;
}


/*--------------------------------------------------------------
 * external API
 *--------------------------------------------------------------*/

TASN_RESULT_T
TASN_PREFIX(Free)(
    TASN_T*		in_asn)
{
    return _asn_traverse_depth_first(in_asn, _asn_free, NULL);
}


TASN_RESULT_T
TASN_PREFIX(DecodeWoCopy)(
    u_int8_t*		in_buf,
    np_size_t		in_bufsize,
    u_int8_t**		out_next,
    TASN_T**		out_asn)
{
    return _asn_decode(ASN_DECODE_NOCOPY,
                       (unsigned char *)in_buf,
                       (unsigned char *)in_buf + in_bufsize,
		       NULL,
                       (unsigned char**)out_next,
                       out_asn);
}

/*
 * structured operations
 */
TASN_RESULT_T
TASN_PREFIX(Insert)(
    TASN_T*		io_asn,
    int			in_index,
    TASN_T*		in_asn)
{
    TASN_T **ptr;

    /* check */
    if ( !io_asn )
        return ERR_ASN_INPUT;
    if ( !io_asn->stflag )
        return ERR_ASN_TYPE;

    /* normalize */
    if ( in_index < 0 ) return ERR_ASN_INPUT;
    if ( in_index > (int)io_asn->length )
        in_index = io_asn->length;

    /* realloc */
    ptr = (TASN_T **)ASN_REALLOC(io_asn->v.seq, sizeof(TASN_T*)*(io_asn->length+1));
    if ( ptr == NULL ) return ERR_NOMEM;
    io_asn->v.seq = ptr;
    io_asn->length++;

    /* insert */
    tom_memmove(ptr + in_index + 1, ptr + in_index,
            sizeof(TASN_T*) * (io_asn->length - in_index - 1));
    ptr[in_index] = in_asn;

    return _OK;
}

TASN_RESULT_T
TASN_PREFIX(Append)(
    TASN_T*		io_asn,
    TASN_T*		in_asn)

{
    if ( !io_asn )
        return ERR_ASN_INPUT;
    if ( !io_asn->stflag )
        return ERR_ASN_TYPE;
    return TASN_PREFIX(Insert)(io_asn, (int)io_asn->length, in_asn);
}
