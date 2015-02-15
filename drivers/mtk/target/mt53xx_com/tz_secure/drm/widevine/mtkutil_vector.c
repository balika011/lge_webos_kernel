#include "mtkutil_vector.h"
#include "string_conversions.h"


void VecInit(Vector *this)
{
    this->buf =NULL;
    this->size=0;
}

void VecTerm(Vector *this)
{
    if(this->buf != NULL)
    {
        free(this->buf);
        this->buf=NULL;
    }
    this->size=0;
}

Vector *VecDup(const Vector *this)
{
    Vector *ret = malloc(sizeof(Vector));
    
    if(ret ==NULL){
        goto final;
    }
    
    ret->buf = malloc(this->size);
    if(ret->buf ==NULL)
    {
        ret->size=0;
    }
    else
    {
        memcpy(ret->buf,this->buf,this->size);
        ret->size = this->size;
    }
final:    
    return ret;
}

void VecDestroy(Vector *this)
{
    VecTerm(this);
    free(this);
}

int VecCmp(const Vector *this ,const Vector *diff)
{
    if(this->size != diff->size)
    {
        return 1;
    }
    
    return memcmp(this->buf , diff->buf , this->size);
}

void VecCpy(Vector *des , const Vector *src)
{
    if(des->buf != NULL)
    {
        free(des->buf);
    }
    des->buf = malloc(src->size);
    if(des->buf ==NULL)
    {
        des->size =0;
    }
    else
    {
        memcpy(des->buf,src->buf ,src->size);
        des->size = src->size;
    }
}


void VecAssign(Vector *this , const uint8_t  *src, size_t size)
{
    VecTerm(this);
    
    if(src ==NULL){
        this->buf =NULL;
        size=0;
        return;
    }
    
    this->buf = malloc(size);
    
    if(this->buf == NULL)
    {
        size=0;
    }
    else
    {
        memcpy(this->buf,src,size);
        this->size = size;
    }
}

void VecPushBack(Vector *this, const uint8_t ch)
{
    uint8_t *newbuf = malloc(this->size +1);
    if(newbuf !=NULL)
    {
        if(this->buf !=NULL)
        {
            memcpy((void *) newbuf , (void *) this->buf, this->size);
            free(this->buf);
        }
        this->buf = newbuf;
        *(this->buf + this->size) = ch;
        this->size ++;
    }
}

void VecAppend(Vector *this,const Vector *append)
{
    uint8_t *newbuf = malloc(this->size + append->size);
    if(newbuf !=NULL)
    {
        if(this->buf !=NULL)
        {
            memcpy(newbuf,this->buf,this->size);
            free(this->buf);
        }
        if(append->buf !=NULL)
        {
            memcpy(newbuf+this->size, append->buf , append->size);
        }
        this->buf = newbuf;
        this->size += append->size;
    }
}


void VecResize(Vector *this,size_t size)
{
    uint8_t *newbuf = malloc(size);
    size_t cpysize;

    if(newbuf ==NULL)
    {
        size = 0;
        goto final;
    }

    if(this->size >= size)
    {
        cpysize = size;
    }
    else
    {
        cpysize =this->size;
        memset(newbuf , 0 , size);
    }

    memcpy((void *) newbuf , (void *) this->buf, cpysize);

final:

    if(this->buf !=NULL)
    {
        free(this->buf);
    }

    this->buf = newbuf;
    this->size = size;

}

void VecToCStr(Vector *this)
{
    Vector str =b2a_hex(this);
    VecCpy(this,&str);
    VecTerm(&str);
}
