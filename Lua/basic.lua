require('Modules\\ps3mapi')

local instance = PS3MAPI_CreateInstance('10.0.0.205', 7887)
PS3MAPI_Connect(instance)
PS3MAPI_RingBuzzer(instance, BuzzerType_Single)
PS3MAPI_Disconnect(instance)
PS3MAPI_DestroyInstance(instance)