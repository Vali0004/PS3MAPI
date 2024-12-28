#pragma once
#include "ps3mapi_types.h"
#include "endian_swap.h"

enum class PS3MAPICommand : u8 {
	ReadProcessMemory = 2,
	WriteProcessMemory,
	GetProcessList,
	GetProcessName,
};
inline u8 GetSizeFromCommand(PS3MAPICommand command) {
	switch (command) {
		case PS3MAPICommand::ReadProcessMemory: {
			return 0x20;
		} break;
		case PS3MAPICommand::WriteProcessMemory: {
			return 0x20;
		} break;
		case PS3MAPICommand::GetProcessList: {
			return 0x10;
		} break;
		case PS3MAPICommand::GetProcessName: {
			return 0x14;
		} break;
	}
	return NULL;
}