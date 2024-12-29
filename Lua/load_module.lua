local ffi = require('ffi')
require('ps3mapi')

print('Enter your console ip: ')
ip = io.read("*l")

function GetCurrentProcess(instance)
    local pid_ptr = ffi.new("unsigned int[1]")
    PS3MAPI_GetCurrentProcess(instance, pid_ptr)
    return pid_ptr[0]
end
function GetProcessName(instance, pid)
    local process_name_buf = ffi.new("char[64]")
    PS3MAPI_GetProcessName(instance, pid, process_name_buf, 64)
    return ffi.string(process_name_buf, 64)
end

local instance = PS3MAPI_CreateInstance(ip, 7887)
PS3MAPI_Connect(instance)
local pid = GetCurrentProcess(instance)
local process_name = GetProcessName(instance, pid)

if string.find(process_name, 'main_vsh') == false then
    print('Enter module: (/dev_hdd0/tmp/*.sprx)')
    module = '/dev_hdd0/tmp/' .. io.read("*l") .. '.sprx'
    --PS3MAPI_LoadModule(instance);
    PS3MAPI_RingBuzzer(instance, BuzzerType_Single)
else
    print('Not a valid process!')
    PS3MAPI_RingBuzzer(instance, BuzzerType_Tripple)
end

PS3MAPI_Disconnect(instance)
PS3MAPI_DestroyInstance(instance)