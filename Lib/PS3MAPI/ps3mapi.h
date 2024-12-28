#pragma once
// Implementation of https://github.com/aldostools/webMAN-MOD/blob/36ce87a0d26f665f974d3a007a0b6313e346861b/include/ps3mapi/ps3mapi_server.h as a client
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <vector>
#include <format>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <iostream>
#include "ps3mapi_types.h"
#include "socket_helpers.h"
#include "string_helpers.h"

class Connection
{
	struct Data
	{
		char ip[17]{};
		s32 port{};
		u64 socket{};
		u16 GetPort()
		{
			return htons(port);
		}
		in_addr GetInAddress()
		{
			in_addr addr{};
			addr.S_un.S_addr = inet_addr(ip);
			return addr;
		}
	};
public:
	Connection(const char* ip = NULL, s32 port = NULL, bool data_socket = false)
	{
		Create(ip, port, data_socket);
	}
	~Connection()
	{}
	void Create(const char* ip, s16 port, bool data_socket)
	{
		size_t ipLen{ strlen(ip) };
		if (ipLen)
		{
			strncpy(m_data.ip, ip, ipLen);
		}
		m_data.ip[ipLen + 1] = '\0';
		m_data.port = port;
		m_data_socket = data_socket;
	}

	s8 CreateConnection()
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
	std::pair<s8, std::string> SendCommand(const std::string& command, A... args)
	{
		s8 status{ CreateConnection() };
		std::string msg{ std::vformat(command, std::make_format_args(args...)) };
		std::string formattedCommand{ std::format("{}\r\n", msg) };
		if (send(m_data.socket, formattedCommand.c_str(), static_cast<int>(formattedCommand.size()), 0) == SOCKET_ERROR)
		{
			return std::make_pair(PS3MAPIError_SocketSendFailed, std::string{});
		}
		return ReceiveResponse();
	}

	s8 SendBufferedResponse(const char* buffer, size_t size)
	{
		if (send(m_data.socket, buffer, static_cast<int>(size), 0) == SOCKET_ERROR)
		{
			return PS3MAPIError_SocketSendFailed;
		}

		return PS3MAPIError_NoError;
	}

	std::pair<s8, int> ReceiveBufferedResponse(char* buffer, size_t size)
	{
		int bytesRead{ recv(m_data.socket, buffer, static_cast<int>(size), 0) };
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

	std::pair<s8, std::string> ReceiveResponse()
	{
		char buffer[2049];
		std::pair<s8, int> res{ ReceiveBufferedResponse(buffer, sizeof(buffer)) };
		buffer[res.second] = '\0';
		return std::make_pair(res.first, buffer);
	}

	s8 KillConnection()
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
		return strlen(m_data.ip) && m_data_socket;
	}
public:
	Data m_data{};
	bool m_data_socket{};
};

struct PS3MAPI
{
	PS3MAPI()
	{}
	PS3MAPI(const char* ip, uint16_t port = 7887)
	{
		Create(ip, port);
	}
	~PS3MAPI()
	{
		Destroy();
	}

	void Create(const char* ip, uint16_t port)
	{
		size_t ipLen{ strlen(ip) };
		strncpy(m_ip, ip, ipLen);
		m_ip[ipLen + 1] = '\0';
		m_connection.Create(ip, port, false);
		m_connection.StartSocket();
	}

	void Destroy()
	{
		m_connection.KillSocket();
	}

	s8 Connect()
	{
		return m_connection.CreateConnection();
	}
	s8 Disconnect()
	{
		return m_connection.KillConnection();
	}

	std::pair<s8, std::vector<ConsoleInfo>> GetConsoleList()
	{
		auto ipSysInfo{ GetSystemInfo(32) };
		if (ipSysInfo.first != PS3MAPIError_NoError)
			return std::make_pair(ipSysInfo.first, std::vector<ConsoleInfo>{});
		std::string ipInfo{ ipSysInfo.second };
		ConsoleInfo info{};
		if (size_t ip_seperator{ ipInfo.find(" ") }; ip_seperator != std::string::npos)
		{
			std::string name{ ipInfo.substr(0, ip_seperator) };
			std::string ip{ ipInfo.substr(0, ip_seperator) };
			strncpy(info.ip, ip.data(), ip.size());
			strncpy(info.name, name.data(), name.size());
		}
		else
		{
			return std::make_pair(PS3MAPIError_InvalidResponse, std::vector<ConsoleInfo>{});
		}
		// We don't support multiple consoles
		return std::make_pair(PS3MAPIError_NoError, std::vector<ConsoleInfo>{ info });
	}

	s8 GetCurrentProcess(u32* pid)
	{
		std::pair<s8, std::string> res{ m_connection.SendCommand("PROCESS GETCURRENTPID") };
		s8 status{ res.first };
		if (res.second.find("200 ") == std::string::npos)
		{
			return PS3MAPIError_InvalidResponseCode;
		}
		std::string pid_str{ res.second.substr(4) };
		*pid = static_cast<u32>(stoul(pid_str));
		AttachProcess(*pid);
		return status;
	}

	s8 GetProcessName(u32 pid, char* name, size_t nameSize)
	{
		std::pair<s8, std::string> res{ m_connection.SendCommand("PROCESS GETNAME {}", pid) };
		s8 status{ res.first };
		if (res.second.find("200 ") == std::string::npos)
		{
			return PS3MAPIError_InvalidResponseCode;
		}
		std::string name_str{ find_replace(res.second.substr(4), "\r\n", {}) };
		strncpy(name, name_str.c_str(), name_str.size() >= nameSize ? nameSize : name_str.size());
		return status;
	}

	std::pair<s8, std::vector<ProcessInfo>> GetAllProcesses()
	{
		std::vector<ProcessInfo> processes{};
		std::pair<s8, std::string> res{ m_connection.SendCommand("PROCESS GETALLPID") };
		s8 status{ res.first };
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
				std::string name{};
				GetProcessName(info.pid, info.name, sizeof(info.name));
				processes.push_back(info);
			}
		}
		return std::make_pair(PS3MAPIError_NoError, processes);
	}

	s8 SetupDataSocket()
	{
		std::pair<s8, std::string> res{ m_connection.SendCommand("PASV") };
		std::string status_message{ "227 Entering Passive Mode (" };	
		if (size_t status_message_pos{ res.second.find(status_message) }; status_message_pos == std::string::npos)
		{
			return PS3MAPIError_InvalidResponseCode;
		}
		s8 status{ res.first };
		std::string status_data{ find_replace(res.second.substr(status_message.size()), "\r\n", {}) };
		std::string socket_info{ status_data.substr(0, status_data.size()) };
		std::vector<std::string> data{ split_string(socket_info, ',') };
		size_t offset{ 4 };
		std::string ip{ std::format("{}.{}.{}.{}", data[0], data[1], data[2], data[3]) };
		s32 p1x{ stoi(data.at(offset+0)) }, p2x{ stoi(data.at(offset+1)) };
		s32 port{ ((p1x << 8) + p2x) };
		m_data_socket = Connection{ ip.c_str(), port, true};
		return PS3MAPIError_NoError;
	}

	s8 ReadMemory(u64 address, u32 size, void* data)
	{
		if (!size || !data || !address)
		{
			return PS3MAPIError_Error;
		}
		s8 status{ SetupDataSocket() };
		if (status != PS3MAPIError_NoError)
		{
			return status;
		}

		status = m_data_socket.CreateConnection();
		if (status != PS3MAPIError_NoError)
		{
			return status;
		}

		std::pair<s8, std::string> res{ m_connection.SendCommand("MEMORY GET {} {:X} {}", GetAttachedProcess(), address, size) };
		if (res.first != PS3MAPIError_NoError)
		{
			return res.first;
		}

		std::pair<s8, int> buffer_response{ m_data_socket.ReceiveBufferedResponse((char*)data, size) };
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

	s8 WriteMemory(u64 address, u32 size, const void* data)
	{
		if (!size || !data || !address)
		{
			return PS3MAPIError_Error;
		}

		s8 status{ SetupDataSocket() };
		if (status != PS3MAPIError_NoError && status != PS3MAPIError_AlreadyConnected)
		{	   
			return status;
		}

		status = m_data_socket.CreateConnection();
		if (status != PS3MAPIError_NoError)
		{
			return status;
		}

		std::pair<s8, std::string> res{ m_connection.SendCommand("MEMORY SET {} {:X}", GetAttachedProcess(), address) };
		if (res.first != PS3MAPIError_NoError)
		{
			return res.first;
		}

		status = m_data_socket.SendBufferedResponse((const char*)data, size);
		if (status != PS3MAPIError_NoError)
		{
			return status;
		};
		return m_data_socket.KillConnection();
	}

	s8 GetTemperature(int* cell, int* rsx)
	{
		std::pair<s8, std::string> res{ m_connection.SendCommand("PS3 GETTEMP") };
		s8 status{ res.first };
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

	s8 Shutdown(ShutdownAction action)
	{
		std::pair<s8, std::string> res{};
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
		s8 status{ res.first };
		return status;
	}

	s8 RingBuzzer(BuzzerType type)
	{
		std::pair<s8, std::string> res{ m_connection.SendCommand("PS3 BUZZER{}", static_cast<u32>(type)) };
		s8 status{ res.first };
		return status;
	}

	s8 SetConsoleLed(LEDColor color, LEDState state)
	{
		std::pair<s8, std::string> res{ m_connection.SendCommand("PS3 LED {} {}", static_cast<u32>(color), static_cast<u32>(state)) };
		s8 status{ res.first };
		return status;
	}

	s8 SetConsoleIds(ConIdType type, const std::string& id)
	{
		if (id.size() != 32)
		{
			return PS3MAPIError_BufferTooSmall;
		}
		std::string part1{ id.substr(0, 16) }, part2{ id.substr(16, 16) };

		std::pair<s8, std::string> res{ m_connection.SendCommand("PS3 SET{} {} {}", type == ConIdType_IDPS ? "IDPS" : "PSID", part1, part2) };
		s8 status{ res.first };
		return status;
	}

	s8 SetConsoleIds(ConIdType type, const ConsoleId* ids)
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

	s8 VshNotify(NotifyIcon icon, const char* message)
	{
		std::pair<s8, std::string> res{ m_connection.SendCommand("PS3 NOTIFY {}&icon={}", message, static_cast<uint8_t>(icon)) };
		s8 status{ res.first };
		return status;
	}

	s8 GetFirmware(u32* fw)
	{
		std::pair<s8, std::string> res{ m_connection.SendCommand("PS3 GETFWVERSION") };
		s8 status{ res.first };
		if (res.second.find("200 ") == std::string::npos)
		{
			return PS3MAPIError_InvalidResponseCode;
		}
		std::string fw_str{ find_replace(res.second.substr(4), "\r\n", {}) };
		*fw = static_cast<u32>(stoi(fw_str));
		return status;
	}

	s8 GetVersion(u32* ver)
	{
		std::pair<s8, std::string> res{ m_connection.SendCommand("CORE GETVERSION") };
		s8 status{ res.first };
		if (res.second.find("200 ") == std::string::npos)
		{
			return PS3MAPIError_InvalidResponseCode;
		}
		std::string fw_str{ find_replace(res.second.substr(4), "\r\n", {}) };
		*ver = static_cast<u32>(stoi(fw_str));
		return status;
	}

	std::pair<s8, std::string> GetSystemInfo(u32 op)
	{
		ConsoleInfo info{}; 
		std::pair<s8, std::string> res{ m_connection.SendCommand("PS3 GETSYSINFO {}", op)};
		s8 status{ res.first };
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


	Connection m_connection{};
	Connection m_data_socket{};
	char m_ip[17]{};
	u32 m_processId{};
};