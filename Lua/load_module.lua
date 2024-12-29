local ffi = require('ffi')
require('Modules\\ps3mapi')
require('Modules\\util')

ip = GetIPInput()

local instance = PS3MAPI_CreateInstance(ip, 7887)
PS3MAPI_Connect(instance)
local pid = GetCurrentProcess(instance)
local process_name = GetProcessName(instance, pid)
if string.find(process_name, 'main_vsh') then
    print('Not a valid process!')
    PS3MAPI_RingBuzzer(instance, BuzzerType_Tripple)
else
    print('Enter module: (/dev_hdd0/tmp/*.sprx)')
    local module_name = io.read('*l')
    local module_path = '/dev_hdd0/tmp/' .. module_name .. '.sprx'
    print('Module path: ' .. module_path)
    PS3MAPI_LoadModule(instance, pid, module_path)
    PS3MAPI_RingBuzzer(instance, BuzzerType_Single)
end

PS3MAPI_Disconnect(instance)
PS3MAPI_DestroyInstance(instance)