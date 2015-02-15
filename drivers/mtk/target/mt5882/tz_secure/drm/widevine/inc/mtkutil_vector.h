#ifndef _MTKUTIL_VECTOR_H
#define _MTKUTIL_VECTOR_H

#include "type_c.h"

typedef struct Vector {
  uint8_t *buf;
  size_t size;
}Vector ;


void VecInit(Vector *str);

void VecTerm(Vector *str);

//Vector Duplicate and Destroy should be paired
Vector * VecDup(const Vector *this);
void VecDestroy(Vector *this);

void VecCpy(Vector *des , const Vector *src);
int  VecCmp(const Vector *this ,const Vector  *diff);


void VecAssign(Vector *this , const uint8_t  *src, size_t size);

static inline bool VecIsEmpty(const Vector *this)
{
    return this->size ==0;
}

void VecResize(Vector *this,size_t size);

void VecPushBack(Vector *this, const uint8_t ch);
void VecAppend(Vector *this,const Vector *append);

void VecToCStr(Vector *this);
//String is a vector contain string and in \0

typedef Vector String;  

#define StringTerm VecTerm
#define StringInit VecInit
#define StringAssing VecAssign
#define StringIsEmpty VecIsEmpty
#define StringPushBack ERROR

#endif
