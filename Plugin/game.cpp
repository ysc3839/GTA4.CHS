#include "game.h"

namespace Game
{
    GameAddresses Addresses;

    void Graphics_SetRenderState(void* texture, int arg4)
    {
        injector::cstd<void(void*, int)>::call(Addresses.pGraphics_SetRenderState, texture, arg4);
    }

    void* Dictionary_GetElementByKey(void* dictionary, std::uint32_t hash)
    {
        return injector::thiscall<void* (void*, std::uint32_t)>::call(Addresses.pDictionary_GetElementByKey, dictionary, hash);
    }

    std::uint32_t Hash_HashStringFromSeediCase(const char* str, std::uint32_t hash)
    {
        return injector::cstd<std::uint32_t(const char*, std::uint32_t)>::call(Addresses.pHash_HashStringFromSeediCase, str, hash);
    }

    std::uint8_t Font_GetRenderIndex()
    {
        return injector::cstd<std::uint8_t()>::call(Addresses.pFont_GetRenderIndex);
    }

    float Font_GetCharacterSizeNormal(std::uint16_t character)
    {
        return injector::cstd<float(std::uint16_t)>::call(Addresses.pFont_GetCharacterSizeNormal, character);
    }

    float Font_GetCharacterSizeDrawing(std::uint16_t character, bool use_extra_width)
    {
        return injector::cstd<float(std::uint16_t, bool)>::call(Addresses.pFont_GetCharacterSizeDrawing, character, use_extra_width);
    }

    void Font_Render2DPrimitive(const Font::rageRect* screen_rect, const Font::rageRect* texture_rect, std::uint32_t color, bool buffered)
    {
        injector::cstd<void(const Font::rageRect*, const Font::rageRect*, std::uint32_t, bool)>::call(Addresses.pFont_Render2DPrimitive, screen_rect, texture_rect, color, buffered);
    }

    void Font_PrintChar(float posx, float posy, std::uint16_t character, bool buffered)
    {
        injector::cstd<void(float, float, std::uint16_t, bool)>::call(Addresses.pFont_PrintChar, posx, posy, character, buffered);
    }
}
