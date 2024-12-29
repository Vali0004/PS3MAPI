local ffi = require('ffi')

function GetIPInput()
    local f = io.open('ip.txt', 'r')
    local ip = f:read('*line')
    f:close()
    
    if not string.find(ip, '.') then
        print('Enter your console ip: ')
        ip = io.read('*l')
    end
    return ip
end

function GetCurrentProcess(instance)
    local pid_ptr = ffi.new('unsigned int[1]')
    PS3MAPI_GetCurrentProcess(instance, pid_ptr)
    return pid_ptr[0]
end

function GetProcessName(instance, pid)
    local process_name_buf = ffi.new('char[64]')
    PS3MAPI_GetProcessName(instance, pid, process_name_buf, 64)
    return ffi.string(process_name_buf, 64)
end