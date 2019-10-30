#ifndef LOGGER_H
#define LOGGER_H
// TODO(sdsmith): Temporary. Replace this with custom logging class.

#include <SDL2/SDL.h>



// NOTE(sdsmith): https://wiki.libsdl.org/CategoryLog

// Log type (category)
#define LOG_APPLICATION SDL_LOG_CATEGORY_APPLICATION
#define LOG_ERROR       SDL_LOG_CATEGORY_ERROR
#define LOG_SYSTEM      SDL_LOG_CATEGORY_SYSTEM
#define LOG_AUDIO       SDL_LOG_CATEGORY_AUDIO
#define LOG_VIDEO       SDL_LOG_CATEGORY_VIDEO
#define LOG_RENDER      SDL_LOG_CATEGORY_RENDER
#define LOG_INPUT       SDL_LOG_CATEGORY_INPUT

// Log priority level (each one being a subset of the messages from earlier levels)
#define LOG_LEVEL_VERBOSE  SDL_LOG_PRIORITY_VERBOSE
#define LOG_LEVEL_DEBUG    SDL_LOG_PRIORITY_DEBUG
#define LOG_LEVEL_INFO     SDL_LOG_PRIORITY_INFO
#define LOG_LEVEL_WARN     SDL_LOG_PRIORITY_WARN
#define LOG_LEVEL_ERROR    SDL_LOG_PRIORITY_ERROR
#define LOG_LEVEL_CRITICAL SDL_LOG_PRIORITY_CRITICAL

/*
  const char* fmt
 */

// log(1,...) has log_type = LOG_APPLICATION, priority_level=LOG_LEVEL_INFO
#define logMessage(log_type, log_level, fmt, ...)  SDL_LogMessage((log_type), (log_level), (fmt), ##__VA_ARGS__)
#define log(fmt, ...)                              SDL_Log((fmt), ##__VA_ARGS__)
#define logVerbose(log_type,  fmt, ...)            SDL_LogVerbose((log_type), (fmt), ##__VA_ARGS__)
#define logDebug(log_type,    fmt, ...)            SDL_LogDebug((log_type), (fmt), ##__VA_ARGS__)
#define logInfo(log_type,     fmt, ...)            SDL_LogInfo((log_type), (fmt), ##__VA_ARGS__)
#define logWarn(log_type,     fmt, ...)            SDL_LogWarn((log_type), (fmt), ##__VA_ARGS__)
#define logError(log_type,    fmt, ...)            SDL_LogError((log_type), (fmt), ##__VA_ARGS__)
#define logCritical(log_type, fmt, ...)            SDL_LogCritical((log_type), (fmt), ##__VA_ARGS__)
#define logSetPriority(log_type, log_level)        SDL_LogSetPriority((log_type), (log_level))
#define logSetAllPriority(log_level)               SDL_LogSetAllPriority((log_level))

#endif
