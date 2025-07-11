#pragma once

#include <fmt/format.h>
#include <fmt/xchar.h>
#include <string>

#define RUNES_LOG(message, ...)                                                           \
    do                                                                                    \
    {                                                                                     \
        std::wstring _runes_message = fmt::format(L##message, ## __VA_ARGS__);            \
        printf("%S\n", _runes_message.c_str());                                           \
    } while(false)

#define RUNES_LOG_INFO(message, ...) RUNES_LOG("[INFO]  " message, ## __VA_ARGS__)

#define RUNES_LOG_WARN(message, ...) RUNES_LOG("[WARN]  " message, ## __VA_ARGS__)

#define RUNES_LOG_FATAL(message, ...) RUNES_LOG("[FATAL] " message, ## __VA_ARGS__)

#define RUNES_BREAK() \
	/* int3 is the x86 instruction to break, nop is there so the debugger */ \
	/* actually knows to stop on this line and not the next               */ \
	__asm volatile("int3\nnop")

#define RUNES_CRASH() \
	*(uint8_t*)0 = 0

#define RUNES_ASSERT(condition, message, ...) \
	do \
	{ \
		if (!(condition)) \
		{ \
			RUNES_LOG_FATAL(message, ## __VA_ARGS__); \
			RUNES_BREAK(); \
		} \
	} while(false)
