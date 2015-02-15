/*****************************************************************
|
|   SKB Engine - Generic Implementation
|
|   $Id: //DTV/RELEASE_BR/r_DTV_X_IDTV1401_002306_12_001_37_001/kernel/drivers/mtk/target/mt53xx_com/tz_secure/drm/mtk_skb_driver/SkbGenericEngine.c#1 $
|   Original author:  Gilles Boccon-Gibod
|
|   This software is provided to you pursuant to your agreement
|   with Intertrust Technologies Corporation ("Intertrust").
|   This software may be used only in accordance with the terms
|   of the agreement.
|
|   Copyright (c) 2005-2011 by Intertrust. All rights reserved.
|
****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "SkbGenericSecureKeyBox.h"

#define MAX_ENGINE (32)
/*----------------------------------------------------------------------
|   constants
+---------------------------------------------------------------------*/
/* FIXME: this value is for testing, it MUST be replaced */
static const unsigned char SKB_GenericImplTestPersistentExportKey[] = {
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7
};



/*----------------------------------------------------------------------
|   logging
+---------------------------------------------------------------------*/
MTK_SET_LOCAL_LOGGER("skb.generic.engine")

/*----------------------------------------------------------------------
|   shared objects
+---------------------------------------------------------------------*/
static               MTK_Mutex* SKB_SingletonMutex = NULL;
static unsigned char SKB_GenericImplCrossEngineExportKey[16];
static               SKB_Engine* SKB_GenericImplSharedEngine = NULL;
static int           SKB_GenericImplSharedStateIsInitialized = 0;

/* tee review */
static unsigned int	 SKB_GenericImplSharedEngine_handle = -1;


/*----------------------------------------------------------------------
|   SKB_MapTomCryptResult
+---------------------------------------------------------------------*/
SKB_Result
SKB_MapTomCryptResult(int crypt_result)
{
    switch (crypt_result) {
        case CRYPT_OK:
            return SKB_SUCCESS;
        default:
            MTK_LOG_WARNING_1("libtomcrypt failed with error %d",
                              crypt_result);
            return SKB_FAILURE;
    }
}

/*----------------------------------------------------------------------
|   SKB_InitializeSharedState
+---------------------------------------------------------------------*/
static SKB_Result
SKB_InitializeSharedState(void)
{
    if (!SKB_GenericImplSharedStateIsInitialized) {
        /* generate the cross-engine key */
        unsigned long size_check;
        size_check = rng_get_bytes(SKB_GenericImplCrossEngineExportKey,
                                   16,
                                   NULL);
        if (size_check != 16) {
            MTK_LOG_SEVERE("cannot get 16 random bytes...");
            return SKB_ERROR_INTERNAL;
        }

        /* register the tomcrypt algorithms we need */
        if (register_cipher(&aes_desc)  == -1 ||
            register_hash(&sha1_desc)   == -1 ||
            register_hash(&sha256_desc) == -1) {
            MTK_LOG_FATAL("cannot register tomcrypt crypto algorithm");
            return SKB_ERROR_INTERNAL;
        }

		
        /* all good! */
        SKB_GenericImplSharedStateIsInitialized = 1;
    }
    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_Engine_Create
+---------------------------------------------------------------------*/
static SKB_Result
SKB_Engine_Create(SKB_Engine** engine)
{
    /* ensure the shared state is initialized */
    MTK_CHECK_FATAL(SKB_InitializeSharedState());

    /* allocate the engine */
    *engine = (SKB_Engine*)MTK_AllocateZeroMemory(sizeof(SKB_Engine));
    if (*engine == NULL) return SKB_ERROR_OUT_OF_RESOURCES;
    (*engine)->reference_count = 1;

    /* keep a reference on the export keys */
    (*engine)->persistent_export_key   = SKB_GenericImplTestPersistentExportKey;
    (*engine)->cross_engine_export_key = SKB_GenericImplCrossEngineExportKey;

    /* setup the engine properties */
    (*engine)->property_count = 1;
    (*engine)->properties = (SKB_EngineProperty*)MTK_AllocateZeroMemory(1*sizeof(SKB_EngineProperty));
    if((*engine)->properties == NULL)
    {
        return SKB_ERROR_OUT_OF_RESOURCES;
    }
    (*engine)->properties[0].name  = "implementation";
    (*engine)->properties[0].value = "Sockeye Generic Secure Key Box Implementation";

    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_Engine_Destroy
+---------------------------------------------------------------------*/
static SKB_Result
SKB_Engine_Destroy(SKB_Engine* self)
{

    if (self->properties) 
	{
		MTK_FreeMemory(self->properties);
	}
	
    MTK_FreeMemory(self);
	
    return SKB_SUCCESS;

}

/*----------------------------------------------------------------------
|   SKB_Engine_GetInstance
+---------------------------------------------------------------------*/
SKB_Result
SKB_Engine_GetInstance(SKB_Engine** engine)
{
    SKB_Result result;
	unsigned long size_check = 0;

    /* lock singleton check/creation */
    result = MTK_Mutex_LockAutoCreate(&SKB_SingletonMutex);
    if (MTK_FAILED(result)) return SKB_ERROR_INTERNAL;

    if (SKB_GenericImplSharedEngine == NULL) 
	{
        result = SKB_Engine_Create(&SKB_GenericImplSharedEngine);
        if (SKB_FAILED(result)) 
		{
            MTK_LOG_WARNING_1("failed to create shared engine (%d)", result);
            *engine = NULL;
            goto end;
        }

		dprintf("\n\n\%s: creating engine handle...\n", __FUNCTION__);

		/*create engine handle */
        size_check = rng_get_bytes(&SKB_GenericImplSharedEngine_handle,
                                   4,
                                   NULL);
        if (size_check != 4) 
		{
            MTK_LOG_SEVERE("cannot get 4 random bytes...");
            result = SKB_ERROR_INTERNAL;
			goto end;
        }

		dprintf("\n\n\%s: engine handle[%x] created\n", __FUNCTION__, SKB_GenericImplSharedEngine_handle);

    } 
	else 
    {
        ++SKB_GenericImplSharedEngine->reference_count;
		
        //*engine = SKB_GenericImplSharedEngine;

		/* NOT RETUREN THE REAL ENGLINE */
		*engine = (SKB_Engine*)SKB_GenericImplSharedEngine_handle;
    }
	
    //*engine = SKB_GenericImplSharedEngine;
	/* NOT RETUREN THE REAL ENGLINE */
	*engine = (SKB_Engine*)SKB_GenericImplSharedEngine_handle;

	dprintf("\n\n\%s: real engine [%x] \n", __FUNCTION__,SKB_GenericImplSharedEngine);



end:
    /* unlock */
    MTK_Mutex_Unlock(SKB_SingletonMutex);
    return result;
}

/*----------------------------------------------------------------------
|   SKB_Engine_Release
+---------------------------------------------------------------------*/
SKB_Result
SKB_Engine_Release(SKB_Engine* self)
{
    SKB_Result result = MTK_Mutex_LockAutoCreate(&SKB_SingletonMutex);
    
	unsigned int engine_handle = (unsigned int)(self);
	SKB_Engine *real_engine = NULL;

	if (MTK_FAILED(result)) return SKB_ERROR_INTERNAL;


	dprintf("\n\n\%s: input engine_handle[%x] real engine handle[%x]\n", __FUNCTION__,
		engine_handle, SKB_GenericImplSharedEngine_handle);

	dprintf("\n\n\%s: real engine [%x] \n", __FUNCTION__,SKB_GenericImplSharedEngine);

	if(engine_handle != SKB_GenericImplSharedEngine_handle)
	{
		dprintf("\n\n\%s: input engine_handle[%x] != real engine handle[%x] \n", __FUNCTION__,
			engine_handle, SKB_GenericImplSharedEngine_handle);

		return SKB_FAILURE;
	}
	else
	{
		real_engine = SKB_GenericImplSharedEngine;
	}






    if (--real_engine->reference_count == 0) {
        SKB_Engine_Destroy(real_engine);
        SKB_GenericImplSharedEngine = NULL;
        /* For now, it's OK to leak the skb_generic_mutex object:
         * - it's very dangerous to destroy it at this point because another
         *   thread may be waiting on it (calling SKB_Engine_GetInstance for
         *   example).
         * - we may add an implementation specific Terminate method (non thread
         *   safe) in order to cleanup the skb_generic_mutex.
         * - we may also add a MTK_Mutex_UnlockAutoReset method but the cross-
         *   platform implementation is problematic.
         */
    }

    MTK_Mutex_Unlock(SKB_SingletonMutex);

    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_Engine_GetInfo
+---------------------------------------------------------------------*/
SKB_Result
SKB_Engine_GetInfo(const SKB_Engine* self, SKB_EngineInfo* info)
{

	unsigned int engine_handle = (unsigned int)(self);
	
	SKB_Engine *real_engine = NULL;

	dprintf("\n\n\%s: input engine_handle[%x] real engine handle[%x]\n", __FUNCTION__,
		engine_handle, SKB_GenericImplSharedEngine_handle);




	if(engine_handle != SKB_GenericImplSharedEngine_handle)
	{
		dprintf("\n\n\%s: input engine_handle[%x] != real engine handle[%x]\n", __FUNCTION__,
			engine_handle, SKB_GenericImplSharedEngine_handle);

		return SKB_FAILURE;
	}
	else
	{
		real_engine = SKB_GenericImplSharedEngine;
	}






    if (info == NULL) return SKB_ERROR_INVALID_PARAMETERS;
    info->api_version.major    = SKB_API_VERSION_MAJOR;
    info->api_version.minor    = SKB_API_VERSION_MINOR;
    info->api_version.revision = SKB_API_VERSION_REVISION;
    info->flags = 0;
    info->properties     = real_engine->properties;
    info->property_count = real_engine->property_count;

    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_Engine_CreateTransform
+---------------------------------------------------------------------*/
SKB_Result
SKB_Engine_CreateTransform(SKB_Engine*       self,
                           SKB_TransformType type,
                           const void*       parameters,
                           SKB_Transform**   transform)
{


	unsigned int engine_handle = (unsigned int)(self);

	
	SKB_Engine *real_engine = NULL;
	dprintf("\n\n\%s: input engine_handle[%x] real engine handle[%x]\n", __FUNCTION__,
		engine_handle, SKB_GenericImplSharedEngine_handle);




	if(engine_handle != SKB_GenericImplSharedEngine_handle)
	{
		dprintf("\n\n\%s: input engine_handle[%x] != real engine handle[%x]\n", __FUNCTION__,
			engine_handle, SKB_GenericImplSharedEngine_handle);

		return SKB_FAILURE;
	}
	else
	{
		real_engine = SKB_GenericImplSharedEngine;
	}


	dprintf("\n\n\%s: self(%p), type(%d) parameters(%p), transform(%p)\n", __FUNCTION__, 
		self, (int)type, parameters, transform);



//    return SKB_GenericTransform_Create(real_engine, type, parameters, transform);
return SKB_GenericTransform_Create(engine_handle, type, parameters, transform);

}

/*----------------------------------------------------------------------
|   SKB_Engine_CreateCipher
+---------------------------------------------------------------------*/
SKB_Result
SKB_Engine_CreateCipher(SKB_Engine*           self,
                        SKB_CipherAlgorithm   cipher_algorithm,
                        SKB_CipherDirection   cipher_direction,
                        unsigned int          cipher_flags,
                        const void*           cipher_parameters,
                        const SKB_SecureData* cipher_key,
                        SKB_Cipher**          cipher)
{

	unsigned int engine_handle = (unsigned int)(self);
	
	SKB_Engine *real_engine = NULL;

	dprintf("\n\n\%s: input engine_handle[%x] real engine handle[%x]\n", __FUNCTION__,
		engine_handle, SKB_GenericImplSharedEngine_handle);


	if(engine_handle != SKB_GenericImplSharedEngine_handle)
	{
		dprintf("\n\n\%s: input engine_handle[%x] != real engine handle[%x]\n", __FUNCTION__,
			engine_handle, SKB_GenericImplSharedEngine_handle);

		return SKB_FAILURE;
	}
	else
	{
		real_engine = SKB_GenericImplSharedEngine;
	}




#if 0
    return SKB_GenericCipher_Create(real_engine,
                                    cipher_algorithm,
                                    cipher_direction,
                                    cipher_flags,
                                    cipher_parameters,
                                    cipher_key,
                                    cipher);

#endif

return SKB_GenericCipher_Create(engine_handle,
								cipher_algorithm,
								cipher_direction,
								cipher_flags,
								cipher_parameters,
								cipher_key,
								cipher);


}


/*----------------------------------------------------------------------
|   SKB_Engine_CreateDataFromExported
+---------------------------------------------------------------------*/
SKB_Result
SKB_Engine_CreateDataFromExported(SKB_Engine*      self,
                                  const SKB_Byte*  exported,
                                  SKB_Size         exported_size,
                                  SKB_SecureData** data)
{
	unsigned int engine_handle = (unsigned int)(self);
	
	SKB_Engine *real_engine = NULL;

	dprintf("\n\n\%s: input engine_handle[%x] real engine handle[%x]\n", __FUNCTION__,
		engine_handle, SKB_GenericImplSharedEngine_handle);



	if(engine_handle != SKB_GenericImplSharedEngine_handle)
	{
		dprintf("\n\n\%s: input engine_handle[%x] != real engine handle[%x]\n", __FUNCTION__,
			engine_handle, SKB_GenericImplSharedEngine_handle);

		return SKB_FAILURE;
	}
	else
	{
		real_engine = SKB_GenericImplSharedEngine;
	}




	

    return SKB_GenericSecureData_CreateFromExported(engine_handle,
                                                    exported,
                                                    exported_size,
                                                    data);
}

/*----------------------------------------------------------------------
|   SKB_Engine_CreateDataFromWrapped
+---------------------------------------------------------------------*/
SKB_Result
SKB_Engine_CreateDataFromWrapped(SKB_Engine*           self,
                                 const SKB_Byte*       wrapped,
                                 SKB_Size              wrapped_size,
                                 SKB_DataType          wrapped_type,
                                 SKB_DataFormat        wrapped_format,
                                 SKB_CipherAlgorithm   wrapping_algorithm,
                                 const void*           wrapping_parameters,
                                 const SKB_SecureData* unwrapping_key,
                                 SKB_SecureData**      data)
{
	unsigned int engine_handle = (unsigned int)(self);
	
	SKB_Engine *real_engine = NULL;

	dprintf("\n\n\%s: input engine_handle[%x] real engine handle[%x]\n", __FUNCTION__,
		engine_handle, SKB_GenericImplSharedEngine_handle);



	if(engine_handle != SKB_GenericImplSharedEngine_handle)
	{
		dprintf("\n\n\%s: input engine_handle[%x] != real engine handle[%x]\n", __FUNCTION__,
			engine_handle, SKB_GenericImplSharedEngine_handle);

		return SKB_FAILURE;
	}
	else
	{
		real_engine = SKB_GenericImplSharedEngine;
	}






    return SKB_GenericSecureData_CreateFromWrapped(engine_handle,
                                                   wrapped,
                                                   wrapped_size,
                                                   wrapped_type,
                                                   wrapped_format,
                                                   wrapping_algorithm,
                                                   wrapping_parameters,
                                                   unwrapping_key,
                                                   data);
}



SKB_Byte *SKB_Engine_persistent_export_key(SKB_Engine*           self)
{

	unsigned int engine_handle = (unsigned int)(self);
	
	SKB_Engine *real_engine = NULL;

	dprintf("\n\n\%s: input engine_handle[%x] real engine handle[%x]\n", __FUNCTION__,
		engine_handle, SKB_GenericImplSharedEngine_handle);



	if(engine_handle != SKB_GenericImplSharedEngine_handle)
	{
		dprintf("\n\n\%s: input engine_handle[%x] != real engine handle[%x]\n", __FUNCTION__,
			engine_handle, SKB_GenericImplSharedEngine_handle);

		return NULL;
	}
	else
	{
		real_engine = SKB_GenericImplSharedEngine;
	}


	return real_engine->persistent_export_key;

}



SKB_Byte *SKB_Engine_cross_engine_export_key(SKB_Engine*           self)
{
	unsigned int engine_handle = (unsigned int)(self);
	
	SKB_Engine *real_engine = NULL;

	dprintf("\n\n\%s: input engine_handle[%x] real engine handle[%x]\n", __FUNCTION__,
		engine_handle, SKB_GenericImplSharedEngine_handle);



	if(engine_handle != SKB_GenericImplSharedEngine_handle)
	{
		dprintf("\n\n\%s: input engine_handle[%x] != real engine handle[%x]\n", __FUNCTION__,
			engine_handle, SKB_GenericImplSharedEngine_handle);

		return NULL;
	}
	else
	{
		real_engine = SKB_GenericImplSharedEngine;
	}


	return real_engine->cross_engine_export_key;

}

