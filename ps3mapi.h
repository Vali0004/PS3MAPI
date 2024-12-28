#pragma once
// Implementation of https://github.com/aldostools/webMAN-MOD/blob/36ce87a0d26f665f974d3a007a0b6313e346861b/include/ps3mapi/ps3mapi_server.h as a client
// Why is there so little documentation????
#include <string>
#include <vector>
#include <format>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <iostream>
#include "ps3mapi_commands.h"
#include "http_request.h"
#include "string_helpers.h"
#define PS3MAPI_RECV_SIZE 2048
#define PS3MAPI_CMD_LEN	19
#define PS3MAPI_MAX_LEN	383
#define MAX_PID 16 
#define HEX(x) std::uppercase << std::hex << (u64)(x) << std::dec

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
inline std::string PS3MAPIErrorCodeToString(PS3MAPIError errorCode)
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

class ConsoleId
{
public:
	u8 value[16];
};

class ConsoleInfo
{
public:
	std::string name{};
	std::string ip{};
};

class Connection
{
	struct Data
	{
		std::string ip{};
		s32 port{};
		s32 socket{};
		u16 GetPort()
		{
			return htons(port);
		}
		in_addr GetInAddress()
		{
			in_addr addr{};
			addr.S_un.S_addr = inet_addr(ip.c_str());
			return addr;
		}
	};
public:
	Connection(const std::string& ip, s32 port = NULL, bool data_socket = false)
		: m_data({ ip, port }), m_data_socket(data_socket)
	{}
	Connection()
	{}
	~Connection()
	{}

	PS3MAPIError CreateConnection()
	{
		if (m_data.socket)
		{
			return PS3MAPIError_AlreadyConnected;
		}
		sockaddr_in socketDestination{ AF_INET, m_data.GetPort(), m_data.GetInAddress() };
		m_data.socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (connect(m_data.socket, reinterpret_cast<sockaddr*>(&socketDestination), sizeof(sockaddr_in)) != NULL)
		{
			KillConnection();
			return PS3MAPIError_ConnectionFailed;
		}
		if (!m_data_socket)
		{
			if (ReceiveResponse().second.find("220 OK") == std::string::npos)
			{
				return PS3MAPIError_InvalidResponseCode;;
			}
			if (ReceiveResponse().second.find("230 OK") == std::string::npos)
			{
				return PS3MAPIError_InvalidResponseCode;
			}
		}
		return PS3MAPIError_NoError;
	}

	template <typename ...A>
	std::pair<PS3MAPIError, std::string> SendCommand(const std::string& command, A... args)
	{
		PS3MAPIError status{ CreateConnection() };
		std::string msg{ std::vformat(command, std::make_format_args(args...)) };
		std::string formattedCommand{ std::format("{}\r\n", msg) };
		if (send(m_data.socket, formattedCommand.c_str(), static_cast<int>(formattedCommand.size()), 0) == SOCKET_ERROR)
		{
			return std::make_pair(PS3MAPIError_SocketSendFailed, std::string{});
		}
		return ReceiveResponse();
	}

	PS3MAPIError SendBufferedResponse(const char* buffer, size_t size)
	{
		if (send(m_data.socket, buffer, static_cast<int>(size), 0) == SOCKET_ERROR)
		{
			return PS3MAPIError_SocketSendFailed;
		}

		return PS3MAPIError_NoError;
	}

	std::pair<PS3MAPIError, int> ReceiveBufferedResponse(char* buffer, size_t size)
	{
		//std::cout << "Stating recv for " << size << " bytes" << std::endl;
		int bytesRead{ recv(m_data.socket, buffer, size, 0) };
		//std::cout << "Finish." << std::endl;
		if (bytesRead > 0)
		{
			return std::make_pair(PS3MAPIError_NoError, bytesRead);
		}
		else if (bytesRead == 0)
		{
			return std::make_pair(PS3MAPIError_SocketBufferEmpty, bytesRead);
		}

		return std::make_pair(PS3MAPIError_SocketReceiveFailed, bytesRead);
	} 

	std::pair<PS3MAPIError, std::string> ReceiveResponse()
	{
		char buffer[2049];
		std::pair<PS3MAPIError, int> res{ ReceiveBufferedResponse(buffer, sizeof(buffer)) };
		buffer[res.second] = '\0';
		return std::make_pair(res.first, buffer);
	}

	PS3MAPIError KillConnection()
	{
		if (m_data.socket)
		{
			if (!m_data_socket)
			{
				SendCommand("DISCONNECT");
			}
			if (closesocket(m_data.socket) == SOCKET_ERROR)
			{
				return PS3MAPIError_ConnectionClosureFailed;
			}
		}
		return PS3MAPIError_NoError;
	}

	void StartSocket()
	{
#ifdef W32
		WSADATA data{};
		WSAStartup(MAKEWORD(2, 2), &data);
#endif
	}
	void KillSocket()
	{
#ifdef W32
		WSACleanup();
#endif
	}
	SOCKET GetSock()
	{
		return m_data.socket;
	}
	bool WasOpened()
	{
		return !m_data.ip.empty() && m_data_socket;
	}
private:
	Data m_data{};
	bool m_data_socket{};
};

struct ProcessInfo
{
	u32 pid{};
	std::string name{};
};
class PS3MAPI
{
public:
	PS3MAPI(const std::string& ip, uint16_t port = 7887) : m_ip(ip), m_connection(ip, port)
	{
		m_connection.StartSocket();
	}
	~PS3MAPI()
	{
		m_connection.KillSocket();
	}

	PS3MAPIError Connect()
	{
		return m_connection.CreateConnection();
	}
	PS3MAPIError Disconnect()
	{
		return m_connection.KillConnection();
	}

	std::pair<PS3MAPIError, std::vector<ConsoleInfo>> GetConsoleList()
	{
		auto ipSysInfo{ GetSystemInfo(32) };
		if (ipSysInfo.first != PS3MAPIError_NoError)
			return std::make_pair(ipSysInfo.first, std::vector<ConsoleInfo>{});
		std::string ip{ ipSysInfo.second };
		ConsoleInfo info{};
		if (size_t ip_seperator{ ip.find(" ") }; ip_seperator != std::string::npos)
		{
			info.name = ip.substr(0, ip_seperator);
			info.ip = ip.substr(0, ip_seperator);
		}
		else
		{
			return std::make_pair(PS3MAPIError_InvalidResponse, std::vector<ConsoleInfo>{});
		}
		// We don't support multiple consoles
		return std::make_pair(PS3MAPIError_NoError, std::vector<ConsoleInfo>{ info });
	}

	PS3MAPIError GetCurrentProcess(u32* pid)
	{
		std::pair<PS3MAPIError, std::string> res{ m_connection.SendCommand("PROCESS GETCURRENTPID") };
		PS3MAPIError status{ res.first };
		if (res.second.find("200 ") == std::string::npos)
		{
			return PS3MAPIError_InvalidResponseCode;
		}
		std::string pid_str{ res.second.substr(4) };
		*pid = static_cast<u32>(stoul(pid_str));
		AttachProcess(*pid);
		return status;
	}

	PS3MAPIError GetProcessName(u32 pid, std::string* name)
	{
		std::pair<PS3MAPIError, std::string> res{ m_connection.SendCommand("PROCESS GETNAME {}", pid) };
		PS3MAPIError status{ res.first };
		if (res.second.find("200 ") == std::string::npos)
		{
			return PS3MAPIError_InvalidResponseCode;
		}
		std::string name_str{ find_replace(res.second.substr(4), "\r\n", {}) };
		*name = name_str;
		return status;
	}

	std::pair<PS3MAPIError, std::vector<ProcessInfo>> GetAllProcesses()
	{
		std::vector<ProcessInfo> processes{};
		std::pair<PS3MAPIError, std::string> res{ m_connection.SendCommand("PROCESS GETALLPID") };
		PS3MAPIError status{ res.first };
		if (res.second.find("200 ") == std::string::npos)
		{
			return std::make_pair(PS3MAPIError_InvalidResponseCode, processes);
		}
		if (status != PS3MAPIError_NoError)
		{
			return std::make_pair(status, processes);
		}

		std::string pids_str{ find_replace(res.second.substr(4), "\r\n", {}) };
		std::vector<std::string> pids{ split_string(pids_str, '|') };
		for (size_t i{}; i != pids.size() - 1; ++i)
		{
			std::string& pid{ pids[i] };
			if (pid.empty() || pid.size() <= 1)
			{
				continue;
			}
			ProcessInfo info{ stoul(pid) };
			if (info.pid)
			{
				GetProcessName(info.pid, &info.name);
				processes.push_back(info);
			}
		}
		return std::make_pair(PS3MAPIError_NoError, processes);
	}
private:
	PS3MAPIError SetupDataSocket()
	{
		std::pair<PS3MAPIError, std::string> res{ m_connection.SendCommand("PASV") };
		std::string status_message{ "227 Entering Passive Mode (" };	
		if (size_t status_message_pos{ res.second.find(status_message) }; status_message_pos == std::string::npos)
		{
			return PS3MAPIError_InvalidResponseCode;
		}
		PS3MAPIError status{ res.first };
		std::string status_data{ find_replace(res.second.substr(status_message.size()), "\r\n", {}) };
		std::string socket_info{ status_data.substr(0, status_data.size()) };
		std::vector<std::string> data{ split_string(socket_info, ',') };
		size_t offset{ 4 };
		std::string ip{ std::format("{}.{}.{}.{}", data[0], data[1], data[2], data[3]) };
		s32 p1x{ stoi(data.at(offset+0)) }, p2x{ stoi(data.at(offset+1)) };
		s32 port{ ((p1x << 8) + p2x) };
		m_data_socket = Connection{ ip, port, true };
		return PS3MAPIError_NoError;
	}
public:
	PS3MAPIError ReadMemory(u64 address, u32 size, void* data)
	{
		if (!size || !data || !address)
		{
			return PS3MAPIError_Error;
		}
		PS3MAPIError status{ SetupDataSocket() };
		if (status != PS3MAPIError_NoError)
		{
			return status;
		}
		std::cout << "Data socket created" << std::endl;

		status = m_data_socket.CreateConnection();
		if (status != PS3MAPIError_NoError)
		{
			return status;
		}
		std::cout << "Connection established" << std::endl;

		std::pair<PS3MAPIError, std::string> res{ m_connection.SendCommand("MEMORY GET {} {:X} {}", GetAttachedProcess(), address, size) };
		if (res.first != PS3MAPIError_NoError)
		{
			return res.first;
		}

		std::pair<PS3MAPIError, int> buffer_response{ m_data_socket.ReceiveBufferedResponse((char*)data, size) };
		if (buffer_response.first != PS3MAPIError_NoError)
		{
			return buffer_response.first;
		}
		res = m_connection.ReceiveResponse();
		if (res.second.find("226 OK:") == std::string::npos)
		{
			return PS3MAPIError_InvalidResponseCode;
		}
		return m_data_socket.KillConnection();
	}

	PS3MAPIError WriteMemory(u64 address, u32 size, const void* data)
	{
		if (!size || !data || !address)
		{
			return PS3MAPIError_Error;
		}

		PS3MAPIError status{ SetupDataSocket() };
		if (status != PS3MAPIError_NoError && status != PS3MAPIError_AlreadyConnected)
		{	   
			return status;
		}
		std::cout << "Data socket created" << std::endl;

		status = m_data_socket.CreateConnection();
		if (status != PS3MAPIError_NoError)
		{
			return status;
		}
		std::cout << "Connection established" << std::endl;

		std::pair<PS3MAPIError, std::string> res{ m_connection.SendCommand("MEMORY SET {} {:X}", GetAttachedProcess(), address) };
		if (res.first != PS3MAPIError_NoError)
		{
			return res.first;
		}

		status = m_data_socket.SendBufferedResponse((const char*)data, size);
		if (status != PS3MAPIError_NoError)
		{
			std::cout << "Failed to send data!" << std::endl;
			return status;
		};
		return m_data_socket.KillConnection();
	}

	PS3MAPIError GetTemperature(int* cell, int* rsx)
	{
		std::pair<PS3MAPIError, std::string> res{ m_connection.SendCommand("PS3 GETTEMP") };
		PS3MAPIError status{ res.first };
		if (res.second.find("200 ") == std::string::npos)
		{
			return PS3MAPIError_InvalidResponseCode;
		}
		if (status != PS3MAPIError_NoError)
		{
			return status;
		}
		std::string temps{ find_replace(res.second.substr(4), "\r\n", {}) };
		size_t seperator{ temps.find("|") };
		if (seperator == std::string::npos)
		{
			return PS3MAPIError_InvalidResponse;
		}
		std::string cpu_temp{ temps.substr(0, seperator) };
		std::string rsx_temp{ temps.substr(seperator + 1) };
		if (cell)
		{
			*cell = stoi(cpu_temp);
		}
		if (cell)
		{
			*rsx = stoi(rsx_temp);
		}
		return status;
	}

	PS3MAPIError Shutdown(ShutdownAction action)
	{
		std::pair<PS3MAPIError, std::string> res{};
		switch (action)
		{
			case ShutdownAction_Shutdown:
			{
				res = m_connection.SendCommand("PS3 SHUTDOWN");
			} break;
			case ShutdownAction_SoftReboot:
			{
				res = m_connection.SendCommand("PS3 SOFTREBOOT");
			} break; 
			case ShutdownAction_HardReboot:
			{
				res = m_connection.SendCommand("PS3 HARDREBOOT");
			} break; 
			case ShutdownAction_ForceReboot:
			{
				res = m_connection.SendCommand("PS3 REBOOT");
			} break;
		}
		PS3MAPIError status{ res.first };
		return status;
	}

	PS3MAPIError RingBuzzer(BuzzerType type)
	{
		std::pair<PS3MAPIError, std::string> res{ m_connection.SendCommand("PS3 BUZZER{}", static_cast<u32>(type)) };
		PS3MAPIError status{ res.first };
		return status;
	}

	PS3MAPIError SetConsoleLed(LEDColor color, LEDState state)
	{
		std::pair<PS3MAPIError, std::string> res{ m_connection.SendCommand("PS3 LED {} {}", static_cast<u32>(color), static_cast<u32>(state)) };
		PS3MAPIError status{ res.first };
		return status;
	}

	PS3MAPIError SetConsoleIds(ConIdType type, const std::string& id)
	{
		if (id.size() != 32)
		{
			return PS3MAPIError_BufferTooSmall;
		}
		std::string part1{ id.substr(0, 16) }, part2{ id.substr(16, 16) };

		std::pair<PS3MAPIError, std::string> res{ m_connection.SendCommand("PS3 SET{} {} {}", type == ConIdType_IDPS ? "IDPS" : "PSID", part1, part2) };
		PS3MAPIError status{ res.first };
		return status;
	}

	PS3MAPIError SetConsoleIds(ConIdType type, const ConsoleId* ids)
	{
		const ConsoleId id1{ ids[0] }, id2{ ids[1] };
		std::string id{};
		for (u8 i{}; i != 2; ++i)
		{
			for (u8 j{}; j != sizeof(ConsoleId::value); ++j)
			{
				std::string fmt{ std::format("{:X}", ids[i].value[j]) };
				id.append(fmt, id.size(), fmt.size());
			}
		}
		return SetConsoleIds(type, id);
	}

	PS3MAPIError VshNotify(NotifyIcon icon, const std::string& message)
	{
		std::pair<PS3MAPIError, std::string> res{ m_connection.SendCommand("PS3 NOTIFY {}&icon={}", message, static_cast<uint8_t>(icon)) };
		PS3MAPIError status{ res.first };
		return status;
	}

	PS3MAPIError GetFirmware(u32* fw)
	{
		std::pair<PS3MAPIError, std::string> res{ m_connection.SendCommand("PS3 GETFWVERSION") };
		PS3MAPIError status{ res.first };
		if (res.second.find("200 ") == std::string::npos)
		{
			return PS3MAPIError_InvalidResponseCode;
		}
		std::string fw_str{ find_replace(res.second.substr(4), "\r\n", {}) };
		*fw = static_cast<u32>(stoi(fw_str));
		return status;
	}

	PS3MAPIError GetVersion(u32* ver)
	{
		std::pair<PS3MAPIError, std::string> res{ m_connection.SendCommand("CORE GETVERSION") };
		PS3MAPIError status{ res.first };
		if (res.second.find("200 ") == std::string::npos)
		{
			return PS3MAPIError_InvalidResponseCode;
		}
		std::string fw_str{ find_replace(res.second.substr(4), "\r\n", {}) };
		*ver = static_cast<u32>(stoi(fw_str));
		return status;
	}

	std::pair<PS3MAPIError, std::string> GetSystemInfo(u32 op)
	{
		ConsoleInfo info{}; 
		std::pair<PS3MAPIError, std::string> res{ m_connection.SendCommand("PS3 GETSYSINFO {}", op)};
		PS3MAPIError status{ res.first };
		if (res.second.find("200 ") == std::string::npos)
		{
			return std::make_pair(PS3MAPIError_InvalidResponseCode, std::string{});
		}
		std::string data{ find_replace(res.second.substr(4), "\r\n", {}) };
		return std::make_pair(PS3MAPIError_NoError, data);
	}

	u32 GetAttachedProcess()
	{
		return m_processId;
	}
	void AttachProcess(u32 pid)
	{
		m_processId = pid;
	}
private:
	Connection m_connection{};
	Connection m_data_socket{};
	std::string m_ip{};
	u32 m_processId{};
};