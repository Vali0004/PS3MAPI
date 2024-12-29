local ffi = require('ffi')
require('Modules\\ps3mapi')
require('Modules\\util')

ip = GetIPInput()

local instance = PS3MAPI_CreateInstance(ip, 7887)
PS3MAPI_Connect(instance)
PS3MAPI_Shutdown(instance, ShutdownAction_SoftReboot)
PS3MAPI_Disconnect(instance)
PS3MAPI_DestroyInstance(instance)