// Copyright 2013 Google Inc. All Rights Reserved.
//
// Log - implemented using the standard Android logging mechanism

#define LOG_TAG "WVCdm"
#define LOG_BUF_SIZE 1024

#include "log.h"
#include "utils/Log.h"

extern "C"
void log_write(LogPriority level, const char* fmt, ...) {
  va_list ap;
  char buf[LOG_BUF_SIZE];
  va_start(ap, fmt);
  vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
  va_end(ap);

  android_LogPriority prio = ANDROID_LOG_VERBOSE;

  switch(level) {
    case LOG_ERROR: prio = ANDROID_LOG_ERROR; break;
    case LOG_WARN: prio = ANDROID_LOG_WARN; break;
    case LOG_INFO: prio = ANDROID_LOG_INFO; break;
    case LOG_DEBUG: prio = ANDROID_LOG_DEBUG; break;
    case LOG_VERBOSE: prio = ANDROID_LOG_VERBOSE; break;
  }

  __android_log_write(prio, LOG_TAG, buf);
}

