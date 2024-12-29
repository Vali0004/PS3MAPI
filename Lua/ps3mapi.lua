local ffi = require 'ffi'

ffi.cdef([[
struct PS3MAPI;
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;
typedef enum
{
	PS3MAPIError_Error = -1,
	PS3MAPIError_NoError,
	PS3MAPIError_InvalidResponse,
	PS3MAPIError_InvalidResponseCode,
	PS3MAPIError_ConnectionFailed,
	PS3MAPIError_AlreadyConnected,
	PS3MAPIError_ConnectionClosureFailed,
	PS3MAPIError_SocketSendFailed,
	PS3MAPIError_CommandTooLong,
	PS3MAPIError_BufferTooLarge,
	PS3MAPIError_BufferTooSmall,
	PS3MAPIError_SocketBufferEmpty,
	PS3MAPIError_SocketReceiveFailed,
	PS3MAPIError_SocketPrematurelyClosed
} PS3MAPIError;
typedef enum
{
	PS3MAPIResponseCode_RequestSuccessful = 200,
	PS3MAPIResponseCode_CommandOK = 150,
	PS3MAPIResponseCode_ConnectionClosed = 221,
	PS3MAPIResponseCode_SyntaxError = 501,
	PS3MAPIResponseCode_CommandNotImplemented = 502,
	PS3MAPIResponseCode_LocalError = 451,
} PS3MAPIResponseCode;
typedef enum
{
	ConIdType_IDPS,
	ConIdType_PSID
} ConIdType;
typedef enum
{
	ConType_Unknown,
	ConType_Retail, // CEX
	ConType_Debug,  // DEX
	ConType_Tool    // TOOL
} ConType;
typedef enum
{
	ShutdownAction_Shutdown,
	ShutdownAction_SoftReboot,
	ShutdownAction_HardReboot,
	ShutdownAction_ForceReboot
} ShutdownAction;
typedef enum
{
	BuzzerType_Continious,
	BuzzerType_Single,
	BuzzerType_Double,
	BuzzerType_Tripple
} BuzzerType;
typedef enum
{
	LEDColor_None,
	LEDColor_Green,
	LEDColor_Red
} LEDColor;
typedef enum
{
	LEDState_Off,
	LEDState_On,
	LEDState_BlinkSlow,
	LEDState_BlinkFast
} LEDState;
typedef enum
{
	NotifyIcon_Info,
	NotifyIcon_Caution,
	NotifyIcon_Friend,
	NotifyIcon_Slider,
	NotifyIcon_WrongWay,
	NotifyIcon_Dialog,
	NotifyIcon_DialogShadow,
	NotifyIcon_Text,
	NotifyIcon_Pointer,
	NotifyIcon_Grab,
	NotifyIcon_Hand,
	NotifyIcon_Pen,
	NotifyIcon_Finger,
	NotifyIcon_Arrow,
	NotifyIcon_RightArrow,
	NotifyIcon_Progress,
	NotifyIcon_Trophy,
	NotifyIcon_Trophy2,
	NotifyIcon_Trophy3,
	NotifyIcon_Trophy4
} NotifyIcon;
typedef struct
{
	u8 value[16];
} ConsoleId;
typedef struct
{
	char name[32];
	char ip[17];
} ConsoleInfo;
typedef struct
{
	u32 pid;
	char name[256];
} ProcessInfo;
struct PS3MAPI* PS3MAPI_CreateInstance(const char* ip, u16 port);
s8 PS3MAPI_DestroyInstance(struct PS3MAPI* instance);
s8 PS3MAPI_Connect(struct PS3MAPI* instance);
s8 PS3MAPI_Disconnect(struct PS3MAPI* instance);
s8 PS3MAPI_GetConsoles(struct PS3MAPI* instance, ConsoleInfo* consoles, size_t maxConsoles);
s8 PS3MAPI_GetCurrentProcess(struct PS3MAPI* instance, u32* pid);
s8 PS3MAPI_GetProcesses(struct PS3MAPI* instance, ProcessInfo* processes, size_t maxProcesses);
s8 PS3MAPI_ReadMemory(struct PS3MAPI* instance, u64 address, u32 size, void* data);
s8 PS3MAPI_WriteMemory(struct PS3MAPI* instance, u64 address, u32 size, const void* data);
s8 PS3MAPI_GetTemperature(struct PS3MAPI* instance, int* cell, int* rsx);
s8 PS3MAPI_Shutdown(struct PS3MAPI* instance, ShutdownAction action);
s8 PS3MAPI_RingBuzzer(struct PS3MAPI* instance, BuzzerType type);
s8 PS3MAPI_SetConsoleLed(struct PS3MAPI* instance, LEDColor color, LEDState state);
s8 PS3MAPI_SetConsoleIds(struct PS3MAPI* instance, ConIdType type, const ConsoleId* ids);
s8 PS3MAPI_VshNotify(struct PS3MAPI* instance, NotifyIcon icon, const char* message);
s8 PS3MAPI_GetFirmware(struct PS3MAPI* instance, u32* fw);
s8 PS3MAPI_GetVersion(struct PS3MAPI* instance, u32* ver);
s8 PS3MAPI_GetSystemInfo(struct PS3MAPI* instance, u32 op, char* data, size_t dataSize);
u32 PS3MAPI_GetAttachedProcess(struct PS3MAPI* instance);
void PS3MAPI_AttachProcess(struct PS3MAPI* instance, u32 pid);
]])

local PS3MAPI = ffi.load('PS3MAPI')

PS3MAPIError_Error = -1
PS3MAPIError_NoError = 0
PS3MAPIError_InvalidResponse = 1
PS3MAPIError_InvalidResponseCode = 2
PS3MAPIError_ConnectionFailed = 3
PS3MAPIError_AlreadyConnected = 4
PS3MAPIError_ConnectionClosureFailed = 5
PS3MAPIError_SocketSendFailed = 6
PS3MAPIError_CommandTooLong = 7
PS3MAPIError_BufferTooLarge = 8
PS3MAPIError_BufferTooSmall = 9
PS3MAPIError_SocketBufferEmpty = 10
PS3MAPIError_SocketReceiveFailed = 11
PS3MAPIError_SocketPrematurelyClosed = 12

PS3MAPIResponseCode_RequestSuccessful = 200
PS3MAPIResponseCode_CommandOK = 150
PS3MAPIResponseCode_ConnectionClosed = 221
PS3MAPIResponseCode_SyntaxError = 501
PS3MAPIResponseCode_CommandNotImplemented = 502
PS3MAPIResponseCode_LocalError = 451

ConIdType_IDPS = 0
ConIdType_PSID = 1

ConType_Unknown = 0
ConType_Retail = 1 -- CEX
ConType_Debug = 2  -- DEX
ConType_Tool = 3   -- TOOL

ShutdownAction_Shutdown = 0
ShutdownAction_SoftReboot = 1
ShutdownAction_HardReboot = 2
ShutdownAction_ForceReboot = 3

BuzzerType_Continious = 0
BuzzerType_Single = 1
BuzzerType_Double = 2
BuzzerType_Tripple = 3

LEDColor_None = 0
LEDColor_Green = 1
LEDColor_Red = 2

LEDState_Off = 0
LEDState_On = 1
LEDState_BlinkSlow = 2
LEDState_BlinkFast = 3

NotifyIcon_Info = 0
NotifyIcon_Caution = 1
NotifyIcon_Friend = 2
NotifyIcon_Slider = 3
NotifyIcon_WrongWay = 4
NotifyIcon_Dialog = 5
NotifyIcon_DialogShadow = 6
NotifyIcon_Text = 7
NotifyIcon_Pointer = 8
NotifyIcon_Grab = 9
NotifyIcon_Hand = 12
NotifyIcon_Pen = 13
NotifyIcon_Finger = 14
NotifyIcon_Arrow = 15
NotifyIcon_RightArrow = 16
NotifyIcon_Progress = 17
NotifyIcon_Trophy = 18
NotifyIcon_Trophy2 = 19
NotifyIcon_Trophy3 = 20
NotifyIcon_Trophy4 = 21

function PS3MAPI_CreateInstance(ip, port)
    return PS3MAPI.PS3MAPI_CreateInstance(ip, port)
end
function PS3MAPI_DestroyInstance(instance)
    return PS3MAPI.PS3MAPI_DestroyInstance(instance)
end
function PS3MAPI_Connect(instance)
    return PS3MAPI.PS3MAPI_Connect(instance)
end
function PS3MAPI_Disconnect(instance)
    return PS3MAPI.PS3MAPI_Disconnect(instance)
end
function PS3MAPI_GetConsoles(instance, consoles, maxConsoles)
    return PS3MAPI.PS3MAPI_GetConsoles(instance, consoles, maxConsoles)
end
function PS3MAPI_GetCurrentProcess(instance, pid)
    return PS3MAPI.PS3MAPI_GetCurrentProcess(instance, pid)
end
function PS3MAPI_GetProcesses(instance, processes, maxProcesses)
    return PS3MAPI.PS3MAPI_GetProcesses(instance, processes, maxProcesses)
end
function PS3MAPI_ReadMemory(instance, address, size, data)
    return PS3MAPI.PS3MAPI_ReadMemory(instance, address, size, data)
end
function PS3MAPI_WriteMemory(instance, address, size, data)
    return PS3MAPI.PS3MAPI_WriteMemory(instance, address, size, data)
end
function PS3MAPI_GetTemperature(instance, cell, rsx)
    return PS3MAPI.PS3MAPI_GetTemperature(instance, cell, rsx)
end
function PS3MAPI_Shutdown(instance, action)
    return PS3MAPI.PS3MAPI_Shutdown(instance, action)
end
function PS3MAPI_RingBuzzer(instance, buzzerType)
    return PS3MAPI.PS3MAPI_RingBuzzer(instance, buzzerType)
end
function PS3MAPI_SetConsoleLed(instance, color, state)
    return PS3MAPI.PS3MAPI_SetConsoleLed(instance, color, state)
end
function PS3MAPI_SetConsoleIds(instance, conIdType, ids)
    return PS3MAPI.PS3MAPI_SetConsoleIds(instance, conIdType, ids)
end
function PS3MAPI_VshNotify(instance, icon, message)
    return PS3MAPI.PS3MAPI_VshNotify(instance, icon, message)
end
function PS3MAPI_GetFirmware(instance, fw)
    return PS3MAPI.PS3MAPI_GetFirmware(instance, fw)
end
function PS3MAPI_GetVersion(instance, ver)
    return PS3MAPI.PS3MAPI_GetVersion(instance, ver)
end
function PS3MAPI_GetSystemInfo(instance, op, data, dataSize)
    return PS3MAPI.PS3MAPI_GetSystemInfo(instance, op, data, dataSize)
end
function PS3MAPI_GetAttachedProcess(instance)
    PS3MAPI.PS3MAPI_GetAttachedProcess(instance)
end
function PS3MAPI_AttachProcess(instance, pid)
    PS3MAPI.PS3MAPI_AttachProcess(instance, pid)
end