#include "ps3mapi.h"
#include <cstdio>
#include <iostream>
#define HEX(x) std::uppercase << std::hex << (u64)(x) << std::dec

int main()
{
	PS3MAPI ps3mapi{ "10.0.0.205" };
	ps3mapi.Connect();
	ps3mapi.RingBuzzer(BuzzerType_Single);
	ps3mapi.VshNotify(NotifyIcon_Info, "Hello!");
	int cpu_temp{}, rsx_temp{};
	ps3mapi.GetTemperature(&cpu_temp, &rsx_temp);
	std::cout << "CPU Temp: " << cpu_temp << std::endl;
	std::cout << "RSX Temp: " << rsx_temp << std::endl;
	ConsoleInfo info{ ps3mapi.GetConsoleList().second[0] };
	std::cout << "Console IP: " << info.ip << std::endl; 
	u32 pid;
	ps3mapi.GetCurrentProcess(&pid);
	std::cout << "Current PID: " << pid << std::endl;
	auto processes{ ps3mapi.GetAllProcesses().second };
	for (auto& process : processes)
	{
		std::cout << "PID: 0" << HEX(process.pid) << " | Name: " << process.name << std::endl;
	}
	// Test of how to use read/write memory
	/*u8 data[128] = {};
	ps3mapi.ReadMemory(0x186B99C, sizeof(data), data);
	memset(data, 0x00, 10);
	ps3mapi.WriteMemory(0x186B99C, sizeof(data), data);
	for (u32 i{}; i != sizeof(data); ++i)
	{
		std::cout << HEX(data[i]);
	}
	std::cout << std::endl;*/
	ps3mapi.Disconnect();
	return 0;
}