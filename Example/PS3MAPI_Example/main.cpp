#include <stdio.h>
#define IMPL_BINDINGS
#include "../../Lib/PS3MAPI/c_bindings.h"

int main()
{
	struct PS3MAPI* instance = PS3MAPI_CreateInstance("10.0.0.205", 7887);
	PS3MAPI_Connect(instance);
	PS3MAPI_RingBuzzer(instance, BuzzerType_Single);
	PS3MAPI_Disconnect(instance);
	return 0;
}