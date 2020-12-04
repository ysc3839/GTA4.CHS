#pragma once
#define NOMINMAX
#include <cstdint>
#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <d3d9.h>

struct SCharTexture
{
    //最后使用的
    int gta_width;
    IDirect3DTexture9* pTexture;

    SCharTexture()
        :pTexture(nullptr)
    {

    }

    ~SCharTexture()
    {
        if (pTexture != nullptr)
        {
            pTexture->Release();
        }
    }
};

class CCharRenderer
{
public:
    static constexpr int FTPixelSize = 60;
    static constexpr int FTTopAdjust = 60;
    static constexpr int FTLeftAdjust = 2;
    static constexpr int GTABitmapWidth = 64;
    static constexpr int GTABitmapHeight = 80;

    CCharRenderer();
    CCharRenderer(CCharRenderer&& another) noexcept;
    CCharRenderer& operator=(CCharRenderer&& another) noexcept;
    ~CCharRenderer();

    CCharRenderer(const CCharRenderer&) = delete;
    CCharRenderer& operator=(const CCharRenderer&) = delete;

    bool FTInitialized() const;
    bool FaceLoaded() const;
    void CloseFT();

    bool LoadFont(const std::filesystem::path& filename, int face_index = 0);
    void UnloadFont();

    const SCharTexture& LazyGetCharData(std::uint16_t code);

    void CacheChar(std::uint16_t code);
    void CacheChars(std::wstring_view wstr);

    std::size_t GetCacheCount() const;
    bool IsCacheEmpty() const;
    void ClearCache();

private:
    FT_Library m_ftLibrary;
    FT_Face m_ftFace;
    std::unordered_map<std::uint16_t, SCharTexture> m_charBuffer;
    SCharTexture m_specialChar;

    bool m_isPRCMap; //否则为unicode

    bool InitializeFT();
    std::uint16_t Unicode2GB2312(std::uint16_t unic) const;
    SCharTexture MakeCharData(std::uint16_t code) const;
    static SCharTexture MakeSpecialChar();

    static float Fix26_6ToFloat(long fix_point);
    static float Fix16_16ToFloat(long fix_point);
};
