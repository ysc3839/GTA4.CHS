#include "font.h"
#include "game.h"
#include "plugin.h"
#include "table.h"
#include "renderer.h"

CFont FontObject;

static const float fChsWidth = 32.0f;
static const float fSpriteWidth = 64.0f;
static const float fSpriteHeight = 80.0f;
static const float fTextureResolution = 4096.0f;
static const float fTextureRowsCount = 51.2f;
static const float fTextureColumnsCount = 64.0f;
static const float fRatio = 4.0f;

static void* pChsFont;

bool CFont::IsNaiveCharacter(std::uint16_t character)
{
    return (character < 0x100 || character == 0xFFFF);
}

void* __fastcall CFont::LoadTextureHook(void* pDictionary, int, std::uint32_t hash)
{

    void* result = CGame::Dictionary_GetElementByKey(pDictionary, hash);

    pChsFont = CGame::Dictionary_GetElementByKey(pDictionary, CGame::Hash_HashStringFromSeediCase("font_chs"));

    return result;
}

std::uint16_t* CFont::SkipAWord(std::uint16_t* text)
{
    //跳过一个英语单词，或者一个汉字

    if (text == nullptr)
    {
        return text;
    }

    std::uint16_t* begin = text;
    std::uint16_t* current = text;

    while (true)
    {
        std::uint16_t character = *current;

        if (character == ' ' || character == '~' || character == 0)
        {
            break;
        }

        if (!IsNaiveCharacter(character))
        {
            if (current == begin)
            {
                ++current;
            }

            break;
        }
        else
        {
            ++current;
        }
    }

    return current;
}

float CFont::GetCHSCharacterSizeNormal()
{
    std::uint8_t index = CGame::Font_GetRenderIndex();

    return ((fChsWidth / *GameMeta.pFont_ResolutionX + GameMeta.pFont_Details[index].fEdgeSize2) * GameMeta.pFont_Details[index].fScaleX);
}

float CFont::GetCharacterSizeNormalDispatch(std::uint16_t character)
{
    if (IsNaiveCharacter(character + 0x20))
    {
        return injector::cstd<float(std::uint16_t)>::call(GameMeta.pFont_GetCharacterSizeNormal, character);
    }
    else
    {
        return GetCHSCharacterSizeNormal();
    }
}

float CFont::GetCHSCharacterSizeDrawing(bool useextrawidth)
{
    float extrawidth = 0.0f;

    if (useextrawidth)
    {
        extrawidth = 1.0f;
    }

    return (((fChsWidth + extrawidth) / *GameMeta.pFont_ResolutionX + GameMeta.pFont_RenderState->fEdgeSize) * GameMeta.pFont_RenderState->fScaleX);
}

float CFont::GetCharacterSizeDrawingDispatch(std::uint16_t character, bool useextrawidth)
{
    if (IsNaiveCharacter(character + 0x20))
    {
        return CGame::Font_GetCharacterSizeDrawing(character, useextrawidth);
    }
    else
    {
        return GetCHSCharacterSizeDrawing(useextrawidth);
    }
}

void CFont::PrintCHSChar(float posx, float posy, std::uint16_t character)
{
    CRageRect screenrect, texturerect;

    if (posy < -0.06558f || posy > 1.0f)
    {
        return;
    }

    if (-(GetCHSCharacterSizeDrawing(true) / GameMeta.pFont_RenderState->fScaleX) > posx || posx > 1.0f)
    {
        return;
    }

    auto pos = TableObject.GetCharPos(character);

    float sprite_width = fSpriteWidth / fTextureResolution;
    float character_width = (fChsWidth / *GameMeta.pFont_ResolutionX + GameMeta.pFont_RenderState->fEdgeSize) * GameMeta.pFont_RenderState->fScaleX;
    float character_height = GameMeta.pFont_RenderState->fScaleY * 0.06558f;

    screenrect.fBottomLeftX = posx;
    screenrect.fBottomLeftY = posy + character_height;
    screenrect.fTopRightX = posx + character_width;
    screenrect.fTopRightY = posy;

    texturerect.fTopRightY = (pos.row - 0.045f / fRatio) * fSpriteHeight / fTextureResolution + 8.0f / fTextureResolution;
    if (texturerect.fTopRightY > 1.0f)
    {
        texturerect.fTopRightY = 1.0f;
    }
    texturerect.fBottomLeftY = (pos.row - 0.045f / fRatio) * fSpriteHeight / fTextureResolution + 79.0f / fTextureResolution - 0.001f / fRatio + 0.0048f / fRatio;
    texturerect.fBottomLeftX = pos.column / fTextureColumnsCount;
    texturerect.fTopRightX = pos.column / fTextureColumnsCount + sprite_width;

    switch (GameMeta.pFont_RenderState->nFont)
    {
    case 0:
    case 1:
    case 3:
        CGame::Graphics_SetRenderState(pChsFont);
        break;

    default:
        break;
    }

    CGame::Font_Render2DPrimitive(&screenrect, &texturerect, GameMeta.pFont_RenderState->field_18, false);
}

void CFont::PrintCharDispatch(float posx, float posy, std::uint16_t character, bool buffered)
{
    if (GameMeta.pFont_RenderState->TokenType != 0 || IsNaiveCharacter(character + 0x20))
    {
        CGame::Font_PrintChar(posx, posy, character, buffered);
    }
    else
    {
        PrintCHSChar(posx, posy, character + 0x20);
    }
}

const char* CFont::GetTextFileNameHook(int)
{
    return "CHINESE.GXT";
}

__declspec(naked) void CFont::GetStringWidthHook()
{
    static void* retaddr;

    __asm
    {
        pop retaddr; //DDBE45

        movzx eax, word ptr[esi];
        mov cl, [ebp + 0xC];
        cmp ax, ' ';
        jz space;
        push eax;
        call IsNaiveCharacter;
        add esp, 4;
        test al, al;
        movzx eax, word ptr[esi];
        mov cl, [ebp + 0xC];
        jnz normal;
        jmp chs;

    space:
        add retaddr, 0x3;
        push retaddr;
        ret;

    normal:
        add retaddr, 0xB;
        push retaddr;
        ret;

    chs:
        test cl, cl; //get all
        jnz normal;
        mov dl, [esp + 0x12]; //has char
        test dl, dl;
        jz normal;
        add retaddr, 0x22E;
        push retaddr;
        ret;
    }
}
