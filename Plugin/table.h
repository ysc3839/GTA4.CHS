#pragma once
#include "../common/stdinc.h"

class CCharacterTable
{
public:
    void LoadTable(const std::experimental::filesystem::v1::path& filename);
    CharacterPos GetCharPos(std::uint16_t chr) const;

private:
    std::unordered_map<std::uint32_t, CharacterPos> m_Table;
};

extern CCharacterTable globalTable;
