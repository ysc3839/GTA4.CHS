#include "game.h"

CGame GameMeta;

void CGame::Graphics_SetRenderState(void* texture, int arg4)
{
    injector::cstd<void(void*, int)>::call(GameMeta.pGraphics_SetRenderState, texture, arg4);
}

void* CGame::Dictionary_GetElementByKey(void* dictionary, std::uint32_t hash)
{
    return injector::thiscall<void* (void*, std::uint32_t)>::call(GameMeta.pDictionary_GetElementByKey, dictionary, hash);
}

std::uint32_t CGame::Hash_HashStringFromSeediCase(const char* str, std::uint32_t hash)
{
    return injector::cstd<std::uint32_t(const char*, std::uint32_t)>::call(GameMeta.pHash_HashStringFromSeediCase, str, hash);
}

std::uint8_t CGame::Font_GetRenderIndex()
{
    return injector::cstd<std::uint8_t()>::call(GameMeta.pFont_GetRenderIndex);
}

float CGame::Font_GetCharacterSizeNormal(std::uint16_t character)
{
    return injector::cstd<float(std::uint16_t)>::call(GameMeta.pFont_GetCharacterSizeNormal, character);
}

float CGame::Font_GetCharacterSizeDrawing(std::uint16_t character, bool use_extra_width)
{
    return injector::cstd<float(std::uint16_t, bool)>::call(GameMeta.pFont_GetCharacterSizeDrawing, character, use_extra_width);
}

void CGame::Font_Render2DPrimitive(const CRageRect* screen_rect, const CRageRect* texture_rect, std::uint32_t color, bool buffered)
{
    injector::cstd<void(const CRageRect*, const CRageRect*, std::uint32_t, bool)>::call(GameMeta.pFont_Render2DPrimitive, screen_rect, texture_rect, color, buffered);
}

void CGame::Font_PrintChar(float posx, float posy, std::uint16_t character, bool buffered)
{
    injector::cstd<void(float, float, std::uint16_t, bool)>::call(GameMeta.pFont_PrintChar, posx, posy, character, buffered);
}
