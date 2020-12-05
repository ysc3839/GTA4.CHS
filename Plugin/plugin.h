#pragma once
#include "../common/stdinc.h"

class CPlugin
{
public:
    bool Init(HMODULE);
    bool PatchGame();
    std::string ReadFontFileName(const std::filesystem::path &filename);
};

extern CPlugin PluginObject;
