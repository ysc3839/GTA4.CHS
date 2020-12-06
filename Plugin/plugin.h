#pragma once
#include "../common/stdinc.h"

class CPlugin
{
public:
    bool Init(HMODULE);
    bool GetMemoryAddresses();
    bool PatchGame();
    std::string ReadFontFileName(const std::filesystem::path &filename);
};

extern CPlugin PluginObject;
