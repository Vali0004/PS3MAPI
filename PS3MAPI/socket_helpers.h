#pragma once
#include "endian_swap.h"
#include "ps3mapi_types.h"
#if defined(_WIN32) || defined(WIN32)
#define W32
#endif
#ifdef W32
#include <windows.h>
#include <intrin.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <byteswap.h>
#include <unistd.h>
#include <cstring>
#endif
#define LITTLE_ENDIAN
#ifdef W32
#define close closesocket
#endif
