#include "renderer.h"

CCharRenderer::CCharRenderer()
    :m_ftLibrary(nullptr),
    m_ftFace(nullptr),
    m_d3dDevice(nullptr),
    m_isPRCMap(false)
{
    InitializeFT();
    m_specialChar = MakeSpecialChar();
}

CCharRenderer::CCharRenderer(CCharRenderer&& another) noexcept
{
    m_ftLibrary = another.m_ftLibrary;
    m_ftFace = another.m_ftFace;
    m_d3dDevice = another.m_d3dDevice;
    m_charBuffer = std::move(another.m_charBuffer);
    m_isPRCMap = another.m_isPRCMap;
}

CCharRenderer& CCharRenderer::operator=(CCharRenderer&& another) noexcept
{
    CloseFT();

    m_ftLibrary = another.m_ftLibrary;
    m_ftFace = another.m_ftFace;
    m_d3dDevice = another.m_d3dDevice;
    m_charBuffer = std::move(another.m_charBuffer);
    m_isPRCMap = another.m_isPRCMap;

    return *this;
}

CCharRenderer::~CCharRenderer()
{
    CloseFT();
}

bool CCharRenderer::InitializeFT()
{
    FT_Library library;

    if (FT_Init_FreeType(&library) == 0)
    {
        m_ftLibrary = library;
        return true;
    }
    else
    {
        return false;
    }
}

std::uint16_t CCharRenderer::Unicode2GB2312(std::uint16_t unic) const
{
    wchar_t wide_buffer[2];
    char narrow_buffer[10];
    char default_char = 0;

    BOOL usedDefaultChar;

    wide_buffer[0] = unic;
    wide_buffer[1] = 0;
    std::fill(std::begin(narrow_buffer), std::end(narrow_buffer), 0);

    auto length = WideCharToMultiByte(936, 0, wide_buffer, -1, narrow_buffer, 9, &default_char, &usedDefaultChar);

    if (length == 2)
    {
        return narrow_buffer[0];
    }
    else if (length == 3)
    {
        narrow_buffer[1] = std::exchange(narrow_buffer[0], narrow_buffer[1]);

        return *reinterpret_cast<std::uint16_t*>(narrow_buffer);
    }
    else
    {
        return 0;
    }
}

bool CCharRenderer::FTInitialized() const
{
    return m_ftLibrary != nullptr;
}

bool CCharRenderer::FaceLoaded() const
{
    return m_ftFace != nullptr;
}

void CCharRenderer::CloseFT()
{
    if (FTInitialized())
    {
        FT_Done_FreeType(m_ftLibrary);

        m_ftFace = nullptr;
        m_ftLibrary = nullptr;
        m_d3dDevice = nullptr;
        m_charBuffer.clear();
        m_isPRCMap = false;
    }
}

void CCharRenderer::SetD3DDevice(IDirect3DDevice9* device)
{
    m_d3dDevice = device;
}

bool CCharRenderer::LoadFont(const std::filesystem::path& filename, int face_index)
{
    if (!FTInitialized())
        return false;

    UnloadFont();

    FT_Face face;

    if (FT_New_Face(m_ftLibrary, filename.string().c_str(), face_index, &face) == 0)
    {
        //要求是可缩放字体
        if ((face->face_flags & FT_FACE_FLAG_SCALABLE) == 0)
        {
            FT_Done_Face(face);
            return false;
        }

        //检查是否正确选择了FT_ENCODING_UNICODE，否则选择FT_ENCODING_PRC，两者都失败则作为加载失败
        if (face->charmap == nullptr || face->charmap->encoding != FT_ENCODING_UNICODE)
        {
            if (FT_Select_Charmap(face, FT_ENCODING_PRC) == 0)
            {
                m_isPRCMap = true;
            }
            else
            {
                FT_Done_Face(face);
                return false;
            }
        }
        else
        {
            m_isPRCMap = false;
        }

        m_ftFace = face;
        FT_Set_Pixel_Sizes(m_ftFace, FTPixelSize, FTPixelSize);
        return true;
    }
    else
    {
        return false;
    }
}

SCharTexture CCharRenderer::LazyGetCharData(std::uint16_t code)
{
    if (!FaceLoaded())
        return m_specialChar;

    auto it = m_charBuffer.find(code);

    if (it == m_charBuffer.end())
    {
        return m_charBuffer.emplace(code, MakeCharData(code)).first->second;
    }
    else
    {
        return it->second;
    }
}

void CCharRenderer::UnloadFont()
{
    if (FaceLoaded())
    {
        FT_Done_Face(m_ftFace);
        m_ftFace = nullptr;
        m_isPRCMap = false;
    }
}

void CCharRenderer::CacheChar(std::uint16_t code)
{
    if (!FaceLoaded())
        return;

    auto it = m_charBuffer.find(code);

    if (it == m_charBuffer.end())
    {
        m_charBuffer.emplace(code, MakeCharData(code));
    }
}

void CCharRenderer::CacheChars(std::wstring_view wstr)
{
    for (auto chr : wstr)
    {
        CacheChar(chr);
    }
}

std::size_t CCharRenderer::GetCacheCount() const
{
    return m_charBuffer.size();
}

bool CCharRenderer::IsCacheEmpty() const
{
    return m_charBuffer.empty();
}

void CCharRenderer::ClearCache()
{
    m_charBuffer.clear();
}

SCharTexture CCharRenderer::MakeCharData(std::uint16_t code)
{
    std::uint16_t actual_code = code;

    if (m_isPRCMap)
    {
        actual_code = Unicode2GB2312(code);
    }

    if (actual_code != 0 &&
        FT_Load_Char(m_ftFace, actual_code, FT_LOAD_RENDER) == 0 &&
        m_ftFace->glyph->bitmap.width > 0 &&
        m_ftFace->glyph->bitmap.rows > 0
        )
    {
        SCharTexture result;
        std::vector<unsigned char> pixels;

        unsigned char* bitmap_buffer = m_ftFace->glyph->bitmap.buffer;
        int bitmap_width = m_ftFace->glyph->bitmap.width;
        int bitmap_height = m_ftFace->glyph->bitmap.rows;
        int bitmap_stride = abs(m_ftFace->glyph->bitmap.pitch);
        int bitmap_top = m_ftFace->glyph->bitmap_top;
        int bitmap_left = m_ftFace->glyph->bitmap_left;

        auto texture_width = std::max(bitmap_width + FTLeftAdjust * 2, GTABitmapWidth);
        auto texture_height = std::max(bitmap_height, GTABitmapHeight);

        pixels.resize(texture_width * texture_height, 0);

        int start_x = bitmap_left + FTLeftAdjust;
        int start_y = -bitmap_top + FTTopAdjust;

        if (start_x < 0 || (start_x + bitmap_width) > texture_width)
            start_x = 0;

        if (start_y < 0 || (start_y + bitmap_height) > texture_height)
            start_y = 0;

        for (int row_index = 0; row_index < bitmap_height; ++row_index)
        {
            std::memcpy(pixels.data() + (row_index + start_y) * texture_width + start_x,
                bitmap_buffer + row_index * bitmap_stride,
                bitmap_width);
        }

        result.gta_width = static_cast<int>(ceil(Fix26_6ToFloat(m_ftFace->glyph->advance.x) / 2)) + FTLeftAdjust;
        result.texture = PixelsToTexture(m_d3dDevice, pixels, texture_width, texture_height);

        return result;
    }
    else
    {
        return GetSpecialChar();
    }
}

SCharTexture CCharRenderer::MakeSpecialChar()
{
    SCharTexture result;

    std::vector<unsigned char> pixels;

    pixels.resize(GTABitmapWidth * GTABitmapHeight, 255);

    result.gta_width = GTABitmapWidth / 2;
    result.texture = PixelsToTexture(m_d3dDevice, pixels, GTABitmapWidth, GTABitmapHeight);

    return result;
}

SCharTexture CCharRenderer::GetSpecialChar()
{
    if (m_specialChar.texture == nullptr)
    {
        m_specialChar = MakeSpecialChar();
    }

    return m_specialChar;
}

float CCharRenderer::Fix26_6ToFloat(long fix_point)
{
    return fix_point / 64.0f;
}

float CCharRenderer::Fix16_16ToFloat(long fix_point)
{
    return fix_point / 65536.0f;
}

IDirect3DTexture9* CCharRenderer::PixelsToTexture(IDirect3DDevice9* dev, const std::vector<unsigned char>& pixels, int width, int height)
{
    IDirect3DTexture9* result = nullptr;

    dev->CreateTexture(width, height, 1, 0, D3DFMT_A8, D3DPOOL_MANAGED, &result, nullptr);

    D3DLOCKED_RECT locked_rect;

    result->LockRect(0, &locked_rect, nullptr, 0);

    for (int row_index = 0; row_index < height; ++row_index)
    {
        std::memcpy(reinterpret_cast<unsigned char*>(locked_rect.pBits) + row_index * locked_rect.Pitch, pixels.data() + row_index * width, width);
    }

    result->UnlockRect(0);

    return result;
}
