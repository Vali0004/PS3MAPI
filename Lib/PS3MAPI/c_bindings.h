#pragma once
#include "ps3mapi_types.h"
#define EXPORT __declspec(dllexport)
struct PS3MAPI;

#ifdef IMPL_BINDINGS
#ifdef __cplusplus
extern "C" {
#endif
	EXPORT struct PS3MAPI* PS3MAPI_CreateInstance(const char* ip, u16 port);
	EXPORT PS3MAPIError PS3MAPI_Connect(struct PS3MAPI* instance);
	EXPORT PS3MAPIError PS3MAPI_Disconnect(struct PS3MAPI* instance);
	EXPORT PS3MAPIError PS3MAPI_GetConsoles(struct PS3MAPI* instance, ConsoleInfo* consoles, size_t maxConsoles);
	EXPORT PS3MAPIError PS3MAPI_GetCurrentProcess(struct PS3MAPI* instance, u32* pid);
	EXPORT PS3MAPIError PS3MAPI_GetProcesses(struct PS3MAPI* instance, ProcessInfo* processes, size_t maxProcesses);
	EXPORT PS3MAPIError PS3MAPI_ReadMemory(struct PS3MAPI* instance, u64 address, u32 size, void* data);
	EXPORT PS3MAPIError PS3MAPI_WriteMemory(struct PS3MAPI* instance, u64 address, u32 size, const void* data);
	EXPORT PS3MAPIError PS3MAPI_GetTemperature(struct PS3MAPI* instance, int* cell, int* rsx);
	EXPORT PS3MAPIError PS3MAPI_Shutdown(struct PS3MAPI* instance, ShutdownAction action);
	EXPORT PS3MAPIError PS3MAPI_RingBuzzer(struct PS3MAPI* instance, BuzzerType type);
	EXPORT PS3MAPIError PS3MAPI_SetConsoleLed(struct PS3MAPI* instance, LEDColor color, LEDState state);
	EXPORT PS3MAPIError PS3MAPI_SetConsoleIds(struct PS3MAPI* instance, ConIdType type, const ConsoleId* ids);
	EXPORT PS3MAPIError PS3MAPI_VshNotify(struct PS3MAPI* instance, NotifyIcon icon, const char* message);
	EXPORT PS3MAPIError PS3MAPI_GetFirmware(struct PS3MAPI* instance, u32* fw);
	EXPORT PS3MAPIError PS3MAPI_GetVersion(struct PS3MAPI* instance, u32* ver);
	EXPORT PS3MAPIError PS3MAPI_GetSystemInfo(struct PS3MAPI* instance, u32 op, char* data, size_t dataSize);
	EXPORT u32 PS3MAPI_GetAttachedProcess(struct PS3MAPI* instance);
	EXPORT void PS3MAPI_AttachProcess(struct PS3MAPI* instance, u32 pid);
#ifdef __cplusplus
};
#endif
#endif