/*!
 ****************************************************************************
 *  Copyright 2006 Ocean Blue Software Ltd. - ALL RIGHTS RESERVED
 *  This document is the CONFIDENTIAL property of Ocean Blue Software Ltd.
 ****************************************************************************
 * @brief    System Wide Global Technical Data Type Definitions.
 * @file     techtype.h
 * @Date     August'06
 * @Author   Paul Marshall
 ****************************************************************************
 *        Type        Bits  Bytes    Low Value        High Value
 * --------------------------------------------------------------------------
 *  signed char        8      1        -128              127
 *  unsigned char      8      1          0               255
 * --------------------------------------------------------------------------
 *  short             16      2       -32,768           32,767
 *  unsigned short    16      2          0              65,535
 * --------------------------------------------------------------------------
 *  bool              32      4          0                1
 *  enum              32      4    -2,147,483,648    2,147,483,647
 *  int               32      4    -2,147,483,648    2,147,483,647
 *  unsigned          32      4          0           4,294,967,295
 *  unsigned int      32      4          0           4,294,967,295
 *  long              32      4    -2,147,483,648    2,147,483,647
 *  unsigned long     32      4          0           4,294,967,295
 * --------------------------------------------------------------------------
 *  float             32      4     1.17549435E-38F  3.40282347E+38F
 * --------------------------------------------------------------------------
 *  double            64      8     2.2250738585072014E-308
 *                                  1.7976931348623157E+308
 ****************************************************************************/

#ifndef _TECHTYPE_H_
#define _TECHTYPE_H_


/* #define NEW_BUILD*/

/****************************************************************************
 *  System MACRO Definitions
 ****************************************************************************/

/* MACRO TO PREVENT "parameter not used" WARNING                           */
/* In some cases, function parameter lists are pre-defined and cannot be   */
/* changed, even though the parameters are not used.  Such cases produce   */
/* numerous unnecessary warnings which make it difficult to spot important */
/* warnings.  This macro can be used in such circumstances to fool the     */
/* compiler into thinking the function parameter is used without creating  */
/* unwanted code.                                                          */
#define  USE_UNWANTED_PARAM(param) param = param

#define  FUNC_USE(parm) parm = parm


/* Ocean Blue OUI registered with IEEE */
#define OBS_OUI     0x00045bL


/****************************************************************************
 *  Remove CONSTANT Definitions
 ****************************************************************************/

#undef FALSE
#undef TRUE
#undef NO
#undef YES
#undef loop
#undef NULL
#undef NULL_PTR
#undef FAILED
#undef SUCCEEDED


/****************************************************************************
 *  System CONSTANT Definitions
 ****************************************************************************/

/* System OLD BOOLEAN DATA VALUE Definitions */
/* Definition rather than Enumerated Type    */
/* for Compatibility with Old Ocean Blue Software Build. */

#ifndef NEW_BUILD

#define  FALSE         0
#define  TRUE          1

/* Logical Boolean Equivalents */

#define  NO            FALSE
#define  YES           TRUE
#define  FAILED        FALSE
#define  SUCCEEDED     TRUE

#endif  /* NEW_BUILD */


#ifndef FAILED
#define  FAILED        0
#endif

#ifndef SUCCEEDED
#define  SUCCEEDED     1
#endif


/* Generic NULL Definition */

#define  NULL          0
#define  NULL_PTR      ((void *)NULL)


/* Generic LOOP Definition */
/* Infinite loop ( C keyword extension, hence lower-case ) */

#define  loop      while(1)


/****************************************************************************
 *  System ENUM-TYPE Definitions
 ****************************************************************************/

/* System ENUMERATOR's are 32 bit data size as a signed integer */

/* System NEW BOOLEAN DATA VALUE Definitions */
/* Using an enumerated Boolean type allows the range of */
/* values (0, 1) used to be checked during compilation. */

#ifdef NEW_BUILD
#ifndef BOOL_E
#define BOOL_E

typedef enum bool_e
{
  FALSE = 0,
  TRUE
} bool_e;

#endif  /* BOOL_E */
#endif  /* NEW_BUILD */


/* System DATA TYPE SIZE Definitions  */

#ifdef NEW_BUILD

/* Utilise New Standard Data Type Definitions */

#define   U8BIT      UINT8
#define   S8BIT      INT8
#define   U16BIT     UINT16
#define   S16BIT     INT16
#define   U32BIT     UINT32
#define   S32BIT     INT32
typedef   bool_e     BOOLEAN;  /* BOOLEAN as 4 bytes */

#else

/* Utilise Old Build Data Type Definitions */

#define  U8BIT        UINT8
#define  S8BIT        INT8
#define  U16BIT       UINT16
#define  S16BIT       INT16
#define  U32BIT       UINT32
#define  S32BIT       INT32
//typedef UINT8 BOOLEAN;  /* BOOLEAN as 1 byte */

#endif  /* NEW_BUILD */


/* System CHAR & DATA TYPE POINTER Definitions */

/* The correct language pointer definitions should always match  */
/* the type of data being used.  For example, 'char*' for text   */
/* strings and 'unsigned char*' for data byte arrays.            */


#endif  /* _TECHTYPE_H */
