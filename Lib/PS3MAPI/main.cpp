#include "ps3mapi.h"
#include "c_bindings.h"
#define EXPORT __declspec(dllexport)

#ifdef __cplusplus
extern "C" {
#endif
	EXPORT struct PS3MAPI* PS3MAPI_CreateInstance(const char* ip, uint16_t port)
	{
		struct PS3MAPI* instance = (struct PS3MAPI*)malloc(sizeof(struct PS3MAPI));
		if (!instance)
		{
			return nullptr;
		}
		memset(instance, 0, sizeof(*instance));
		instance->Create(ip, port);
		return instance;
	}
	EXPORT PS3MAPIError PS3MAPI_DestroyInstance(struct PS3MAPI* instance)
	{
		PS3MAPIError status;
		if (!instance)
		{
			return PS3MAPIError_NoError;
		}
		status = instance->Disconnect(); // Just make sure we disconnect first.
		instance->Destroy();
		free(instance);
		return status;
	}
	EXPORT PS3MAPIError PS3MAPI_Connect(struct PS3MAPI* instance)
	{
		return instance->Connect();
	}
	EXPORT PS3MAPIError PS3MAPI_Disconnect(struct PS3MAPI* instance)
	{
		return instance->Disconnect();
	}
	EXPORT PS3MAPIError PS3MAPI_GetConsoles(struct PS3MAPI* instance, ConsoleInfo* consoles, size_t maxConsoles)
	{
		std::pair<PS3MAPIError, std::vector<ConsoleInfo>> res{ instance->GetConsoleList() };
		for (size_t i{}; i != res.second.size() && i != maxConsoles; i++)
		{
			memcpy(&consoles[i], &res.second[i], sizeof(ConsoleInfo));
		}
		return res.first;
	}
	EXPORT PS3MAPIError PS3MAPI_GetCurrentProcess(struct PS3MAPI* instance, u32* pid)
	{
		return instance->GetCurrentProcess(pid);
	}
	EXPORT PS3MAPIError PS3MAPI_GetProcessName(struct PS3MAPI* instance, u32 pid, char* name, size_t nameSize)
	{
		return instance->GetProcessName(pid, name, nameSize);
	}
	EXPORT PS3MAPIError PS3MAPI_GetProcesses(struct PS3MAPI* instance, ProcessInfo* processes, size_t maxProcesses)
	{
		std::pair<PS3MAPIError, std::vector<ProcessInfo>> res{ instance->GetAllProcesses() };
		for (size_t i{}; i != res.second.size() && i != maxProcesses; i++)
		{
			memcpy(&processes[i], &res.second[i], sizeof(ProcessInfo));
		}
		return res.first;
	}
	EXPORT PS3MAPIError PS3MAPI_ReadMemory(struct PS3MAPI* instance, u64 address, u32 size, void* data)
	{
		return instance->ReadMemory(address, size, data);
	}
	EXPORT PS3MAPIError PS3MAPI_WriteMemory(struct PS3MAPI* instance, u64 address, u32 size, const void* data)
	{
		return instance->WriteMemory(address, size, data);
	}
	EXPORT PS3MAPIError PS3MAPI_GetTemperature(struct PS3MAPI* instance, int* cell, int* rsx)
	{
		return instance->GetTemperature(cell, rsx);
	}
	EXPORT PS3MAPIError PS3MAPI_Shutdown(struct PS3MAPI* instance, ShutdownAction action)
	{
		return instance->Shutdown(action);
	}
	EXPORT PS3MAPIError PS3MAPI_RingBuzzer(struct PS3MAPI* instance, BuzzerType type)
	{
		return instance->RingBuzzer(type);
	}
	EXPORT PS3MAPIError PS3MAPI_SetConsoleLed(struct PS3MAPI* instance, LEDColor color, LEDState state)
	{
		return instance->SetConsoleLed(color, state);
	}
	EXPORT PS3MAPIError PS3MAPI_SetConsoleIds(struct PS3MAPI* instance, ConIdType type, const ConsoleId* ids)
	{
		return instance->SetConsoleIds(type, ids);
	}
	EXPORT PS3MAPIError PS3MAPI_VshNotify(struct PS3MAPI* instance, NotifyIcon icon, const char* message)
	{
		return instance->VshNotify(icon, message);
	}
	EXPORT PS3MAPIError PS3MAPI_GetFirmware(struct PS3MAPI* instance, u32* fw)
	{
		return instance->GetFirmware(fw);
	}
	EXPORT PS3MAPIError PS3MAPI_GetVersion(struct PS3MAPI* instance, u32* ver)
	{
		return instance->GetVersion(ver);
	}
	EXPORT PS3MAPIError PS3MAPI_GetSystemInfo(struct PS3MAPI* instance, u32 op, char* data, size_t dataSize)
	{
		std::pair<PS3MAPIError, std::string> res{ instance->GetSystemInfo(op) };
		std::string info{ res.second };
		strncpy(data, info.c_str(), info.size() >= dataSize ? dataSize : info.size());
		return res.first;
	}
	EXPORT u32 PS3MAPI_GetAttachedProcess(struct PS3MAPI* instance)
	{
		return instance->GetAttachedProcess();
	}
	EXPORT void PS3MAPI_AttachProcess(struct PS3MAPI* instance, u32 pid)
	{
		return instance->AttachProcess(pid);
	}
#ifdef __cplusplus
};
#endif