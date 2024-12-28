#pragma once
#define PS3MAPI_RECV_SIZE 2048
#define PS3MAPI_CMD_LEN	19
#define PS3MAPI_MAX_LEN	383
#define MAX_PID 16

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

enum PS3MAPIError : s8
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
};

enum PS3MAPIResponseCode : s16
{
	PS3MAPIResponseCode_RequestSuccessful = 200,
	PS3MAPIResponseCode_CommandOK = 150,
	PS3MAPIResponseCode_ConnectionClosed = 221,
	PS3MAPIResponseCode_SyntaxError = 501,
	PS3MAPIResponseCode_CommandNotImplemented = 502,
	PS3MAPIResponseCode_LocalError = 451,
};

enum ConIdType : u8
{
	ConIdType_IDPS,
	ConIdType_PSID
};
enum ConType : u8
{
	ConType_Unknown,
	ConType_Retail, // CEX
	ConType_Debug,  // DEX
	ConType_Tool    // TOOL
};
enum ShutdownAction : u8
{
	ShutdownAction_Shutdown,
	ShutdownAction_SoftReboot,
	ShutdownAction_HardReboot,
	ShutdownAction_ForceReboot
};
enum BuzzerType : u8
{
	BuzzerType_Continious,
	BuzzerType_Single,
	BuzzerType_Double,
	BuzzerType_Tripple
};
enum LEDColor : u8
{
	LEDColor_None,
	LEDColor_Green,
	LEDColor_Red
};
enum LEDState : u8
{
	LEDState_Off,
	LEDState_On,
	LEDState_BlinkSlow,
	LEDState_BlinkFast
};
enum NotifyIcon : u8
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
};

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

static const char* PS3MAPIErrorCodeToString(PS3MAPIError errorCode)
{
#define DEF_CASE(err) case PS3MAPIError_##err##: { return #err##; } break;
#define DEF_SPECIAL_CASE(err, str) case PS3MAPIError_##err##: { return str; } break;
	switch (errorCode)
	{
		DEF_CASE(NoError);
		DEF_CASE(InvalidResponse);
		DEF_CASE(InvalidResponseCode);
		DEF_CASE(ConnectionFailed);
		DEF_CASE(AlreadyConnected);
		DEF_CASE(ConnectionClosureFailed);
		DEF_CASE(SocketSendFailed);
		DEF_CASE(CommandTooLong);
		DEF_CASE(BufferTooLarge);
		DEF_CASE(BufferTooSmall);
		DEF_CASE(SocketBufferEmpty);
		DEF_CASE(SocketReceiveFailed);
		DEF_CASE(SocketPrematurelyClosed);
		DEF_SPECIAL_CASE(Error, "Unknown error!");
	}
	return "Invalid error code!";
}