// Copyright 2013 Google Inc. All Rights Reserved.
//
// Log - Platform independent interface for a Logging class
//
#ifndef MTKCRYPTO_LOG_H_
#define MTKCRYPTO_LOG_H_

//#define ENABLE_LOG

// Simple logging class. The implementation is platform dependent.

typedef enum {
  LOG_ERROR,
  LOG_WARN,
  LOG_INFO,
  LOG_DEBUG,
  LOG_VERBOSE
} LogPriority;

// Log APIs

#ifndef __cplusplus
#define printf(...)
#endif

#ifdef ENABLE_LOG

#ifdef __cplusplus
extern "C" 
#endif
void log_write(LogPriority priority, const char* fmt, ...);

#define LOGE(...) ((void)log_write(LOG_ERROR, __VA_ARGS__))
#define LOGW(...) ((void)log_write(LOG_WARN, __VA_ARGS__))
#define LOGI(...) ((void)log_write(LOG_INFO, __VA_ARGS__))
#define LOGD(...) ((void)log_write(LOG_DEBUG, __VA_ARGS__))
#define LOGV(...) ((void)log_write(LOG_VERBOSE, __VA_ARGS__))

#else

#define LOGE(...) 
#define LOGW(...) 
#define LOGI(...)
#define LOGD(...) 
#define LOGV(...) 

#endif

#endif  // MTKCRYPTO_LOG_H_
