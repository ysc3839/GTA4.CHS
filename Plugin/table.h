#pragma once
#include "../common/stdinc.h"

class CCharacterTable
{
public:
    void LoadTable(const std::filesystem::path& filename);
    CharacterPos GetCharPos(std::uint16_t chr) const;

    static CCharacterTable GlobalTable;

private:
    std::unordered_map<std::uint32_t, CharacterPos> m_Table;
};
