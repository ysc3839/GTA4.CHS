#include "plugin.h"
#include "byte_pattern.h"

BOOL WINAPI DllMain(HMODULE module, DWORD reason, void *reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        byte_pattern::start_log("gta4.chs");
        if (!Plugin::Init(module))
        {
            return FALSE;
        }
    }

    return TRUE;
}
