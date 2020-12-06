#pragma once
#include "../common/stdinc.h"
#include "font.h"

class CGame
{
public:
    //8B 3D (? ? ? ?) 89 54 24 10
    IDirect3DDevice9** ppDirect3DDevice9;

    //A1 ? ? ? ? 80 7C 24 08 00
    void* pGraphics_SetRenderState;

    //53 55 56 57 8B F9 85 FF 74 3F
    void* pDictionary_GetElementByKey;

    //8B 54 24 08 53 56 8B 74 24 0C 80 3E 22
    void* pHash_HashStringFromSeediCase;

    //8B 34 ED (? ? ? ?) 0F 2E C1
    CFontDetails* pFont_Details;

    //81 3D (? ? ? ?) AD 7F 33 31
    CFontRenderState* pFont_RenderState;

    //F3 0F 11 05 (? ? ? ?) 66 0F 6E 84 24 AC 00 00
    float* pFont_ResolutionX;

    //A1 ? ? ? ? 83 F8 FF 75 1E
    void* pFont_GetRenderIndex;

    //83 EC 30 83 3D ? ? ? ? FF
    void* pFont_PrintChar;

    //51 55 56 E8
    void* pFont_GetCharacterSizeNormal;

    //8A 0D ? ? ? ? 0F B6 D1
    void* pFont_GetCharacterSizeDrawing;

    //83 EC 10 8B 44 24 14 F3 0F 7E 00
    void* pFont_Render2DPrimitive;

   static void Graphics_SetRenderState(void* texture, int = 1);
   static void* Dictionary_GetElementByKey(void* dictionary, std::uint32_t hash);
   static std::uint32_t Hash_HashStringFromSeediCase(const char* str, std::uint32_t hash = 0);
   static std::uint8_t Font_GetRenderIndex();
   static float Font_GetCharacterSizeNormal(std::uint16_t character);
   static float Font_GetCharacterSizeDrawing(std::uint16_t character, bool use_extra_width);
   static void Font_Render2DPrimitive(const CRageRect* screen_rect, const CRageRect* texture_rect, std::uint32_t color, bool buffered);
   static void Font_PrintChar(float posx, float posy, std::uint16_t character, bool buffered);
};

extern CGame GameMeta;
