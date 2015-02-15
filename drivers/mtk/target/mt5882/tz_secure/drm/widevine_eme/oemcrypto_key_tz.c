
#include "oemcrypto_key_tz.h"


KeyType keyType(WvKey * tmpkey)
{
    return tmpkey->type_;
}
const uint8_t* value( WvKey * tmpkey)
{
    return tmpkey->keyvalue_;
}
const KeyControlBlock* control( WvKey * tmpkey)
{
    return &(tmpkey->control_);
}

bool isDeviceKey( WvKey * tmpkey)
{
    return (KEYTYPE_DEVICE == tmpkey->type_);
}
bool isRootKey( WvKey * tmpkey)
{
    return (KEYTYPE_ROOT == tmpkey->type_);
}
bool isPreprovKey( WvKey * tmpkey)
{
    return (KEYTYPE_PREPROV == tmpkey->type_);
}
bool isContentKey( WvKey * tmpkey)
{
    bool ctypes = (KEYTYPE_CONTENT == tmpkey->type_) ||
                  (KEYTYPE_CONTENT_AUDIO == tmpkey->type_) ||
                  (KEYTYPE_CONTENT_VIDEO == tmpkey->type_);
    return ctypes;
}
bool isValidType( WvKey * tmpkey)
{
    return ((KEYTYPE_UNKNOWN < tmpkey->type_) && (KEYTYPE_MAX > tmpkey->type_));
}
bool isValid( WvKey * tmpkey)
{
    return tmpkey->valid_;
}

void clear( WvKey * tmpkey)
{
    memset(tmpkey->keyvalue_, 0, tmpkey->keydatalen_);
    tmpkey->valid_ = FALSE;
}


bool Validate(KeyControlBlock* conbk)
{
    conbk->valid_ = FALSE;
    if (0x6b63746c != conbk->verification_)    // kctl.
    {
        DMSG("KCB: BAD verification string: %08X (not %08X)", conbk->verification_,
             0x6b63746c);
        return FALSE;
    }

   
    if (conbk->control_bits_ & kControlObserveDataPath)
    {
        DMSG("KCB: data_path_type set for refresh.");
    }
    if (conbk->control_bits_ & kControlObserveHDCP)
    {
        DMSG("KCB: HDCP setting set for refresh.");
    }
    if (conbk->control_bits_ & kControlObserveCGMS)
    {
        DMSG("KCB: CGMS setting set for refresh.");
    }

    conbk->valid_ = TRUE;
    return conbk->valid_;
}

// This extracts 4 bytes in network byte order to a 32 bit integer in
// host byte order.
uint32_t ExtractField(KeyControlBlock* conbk, const uint8_t* str, int stlen, int idx)
{
    int bidx = idx * 4;
    uint32_t t = (unsigned char)(str[bidx]) << 24;
    t |= (unsigned char)(str[bidx + 1]) << 16;
    t |= (unsigned char)(str[bidx + 2]) << 8;
    t |= (unsigned char)(str[bidx + 3]);
    return t;
}

bool SetFromString(KeyControlBlock* conbk,const uint8_t* key_control_string, int keyconlen)
{
    if (keyconlen < KEY_CONTROL_SIZE)
    {
        DMSG("KCB: BAD Size: %d (not %d)",keyconlen,
             KEY_CONTROL_SIZE);
        return FALSE;
    }

    conbk->verification_ = ExtractField(conbk,key_control_string, keyconlen,0);
    conbk->duration_     = ExtractField(conbk,key_control_string, keyconlen,1);
    conbk->nonce_        = ExtractField(conbk,key_control_string, keyconlen,2);
    conbk->control_bits_ = ExtractField(conbk,key_control_string, keyconlen,3);

    return Validate(conbk);
}


bool setValue(WvKey * tmpkey, const char* key_string, size_t key_string_length)
{
    tmpkey->valid_ = FALSE;
    if (!key_string || key_string_length == 0)
    {
        return FALSE;
    }

    memcpy(tmpkey->keyvalue_,key_string, key_string_length);

    if (isValidType(tmpkey) && tmpkey->has_control_)
    {
        tmpkey->valid_ = TRUE;
    }
    return tmpkey->valid_;
}

bool setType(WvKey * tmpkey, KeyType ktype)
{
    tmpkey->valid_ = FALSE;
    tmpkey->type_ = ktype;
    if (tmpkey->keydatalen_ <= 0)
    {
    	DMSG("setType: tmpkey->keydatalen_ <= 0.\n");
        return FALSE;
    }

    if (isValidType(tmpkey) && tmpkey->has_control_)
    {
        tmpkey->valid_ = TRUE;
    }
    return tmpkey->valid_;
}

bool setControl(WvKey * tmpkey, const KeyControlBlock* control)
{
    tmpkey->valid_ = FALSE;
    if (!control->valid_)
    {
    	DMSG("setControl: control->valid_ not valid.\n");
        return FALSE;
    }

    memcpy(&(tmpkey->control_), control,sizeof(KeyControlBlock));
    tmpkey->has_control_ = TRUE;

    if (isValidType(tmpkey) && !(tmpkey->keydatalen_ <= 0))
    {
        tmpkey->valid_ = TRUE;
    }
    return tmpkey->valid_;
}

bool UpdateDurationKey(WvKey * tmpkey, const KeyControlBlock* control)
{
    tmpkey->valid_ = FALSE;
    if (!control->valid_ || !tmpkey->has_control_)
    {
        DMSG("UpdateDurationKey: control block not valid.\n");
        return FALSE;
    }

    tmpkey->control_.duration_ = control->duration_;

    if (isValidType(tmpkey) && !(tmpkey->keydatalen_ <= 0))
    {
    	DMSG("UpdateDurationKey: tmpkey->valid_ = TRUE.\n");
		tmpkey->valid_ = TRUE;
    }
    else
    {
        DMSG("UpdateDurationKey: value or type bad.\n");
    }
    return tmpkey->valid_;
}


