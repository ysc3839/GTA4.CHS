#include "plugin.h"
#include "font.h"
#include "game.h"
#include "batch_matching.h"
#include <json/value.h>
#include <json/reader.h>

CPlugin PluginObject;

bool CPlugin::PatchGame()
{
    batch_matching matcher;

    //Hooks

    //获取字符串宽度
    matcher.register_step("0F B7 06 83 F8 20", 1, [](const std::vector<memory_pointer>& addresses)
        {
            injector::MakeCALL(addresses[0].i(), CFont::GetStringWidthHook);
        });

    //跳过单词
    matcher.register_step("57 8B 7C 24 08 85 FF 75 04 33 C0 5F C3 56", 1, [](const std::vector<memory_pointer>& addresses)
        {
            injector::MakeJMP(addresses[0].i(), CFont::SkipAWord);
        });

    //获取字符宽度
    matcher.register_step("83 C0 E0 50 E8 ? ? ? ? D9 5C 24", 2, [](const std::vector<memory_pointer>& addresses)
        {
            injector::MakeCALL(addresses[0].i(4), CFont::GetCharacterSizeNormalDispatch);
            injector::MakeCALL(addresses[1].i(4), CFont::GetCharacterSizeNormalDispatch);
        });

    matcher.register_step("6A 01 57 E8 ? ? ? ? D9 5C 24 30", 1, [](const std::vector<memory_pointer>& addresses)
        {
            injector::MakeCALL(addresses[0].i(3), CFont::GetCharacterSizeDrawingDispatch);
        });
    matcher.register_step("EB 12 6A 01 57 E8 ? ? ? ? D9 5C 24 24", 1, [](const std::vector<memory_pointer>& addresses)
        {
            injector::MakeCALL(addresses[0].i(5), CFont::GetCharacterSizeDrawingDispatch);
        });

    //这些地方只读了一个字节..但是输入应该不是宽字符
    //matcher.registerStep("66 0F BE 06 0F B7 C0 50 E8", 1, [](const std::vector<memory_pointer> &addresses) {injector::MakeCALL(addresses[0].i(8), CFont::GetCharacterSizeNormalDispatch); });
    //matcher.registerStep("66 0F BE 04 1E 0F B7 C0 50 E8", 2, [](const std::vector<memory_pointer> &addresses) {injector::MakeCALL(addresses[0].i(9), CFont::GetCharacterSizeNormalDispatch); injector::MakeCALL(addresses[1].i(9), CFont::GetCharacterSizeNormalDispatch); });

    //绘制字符
    matcher.register_step("E8 ? ? ? ? 6A 01 57 E8 ? ? ? ? D9 5C 24 30", 1, [](const std::vector<memory_pointer>& addresses)
        {
            injector::MakeCALL(addresses[0].i(), CFont::PrintCharDispatch);
        });
    matcher.register_step("F3 0F 11 0C 24 E8 ? ? ? ? 8B 35", 1, [](const std::vector<memory_pointer>& addresses)
        {
            injector::MakeCALL(addresses[0].i(5), CFont::PrintCharDispatch);
        });

    //加载font_chs，即中文字库
    matcher.register_step("8B CE 50 E8 ? ? ? ? 80 3D ? ? ? ? 6A", 2, [](const std::vector<memory_pointer>& addresses)
        {
            injector::MakeCALL(addresses[0].i(3), CFont::LoadTextureHook);
            injector::MakeCALL(addresses[1].i(3), CFont::LoadTextureHook);
        });

    //改变GXT的文件名
    matcher.register_step("80 7C 24 04 00 56 8B F1 74 07", 1, [](const std::vector<memory_pointer>& addresses)
        {
            injector::MakeJMP(addresses[0].i(), CFont::GetTextFileNameHook);
        });

    //Esc菜单抬头热区

    //存档名字读双字节

    //-----------------------------------------------------------------------------------------------------------------------------------
    matcher.perform_search();

    if (matcher.is_all_succeed())
    {
        matcher.run_callbacks();
        return true;
    }
    else
    {
        return false;
    }
}

std::string CPlugin::ReadFontFileName(const std::filesystem::path& filename)
{
    Json::Value value;
    std::ifstream ifs(filename);

    if (!ifs)
        return std::string();

    ifs >> value;

    if (value.isObject())
    {
        auto ref = value["Font"];

        if (ref.isString())
            return ref.asString();
        else
            return std::string();
    }
    else
    {
        return std::string();
    }
}

bool CPlugin::Init(HMODULE module)
{
    wchar_t PluginPath[512];
    GetModuleFileNameW(module, PluginPath, 512);

    auto plugin_folder = std::filesystem::path(PluginPath).parent_path();

    //读取字体
    std::string font_name = ReadFontFileName(plugin_folder / "GTA4.CHS" / "font.json");

    if (font_name.empty())
        return false;

    if (!FontObject.renderer.LoadFont(plugin_folder / "GTA4.CHS" / font_name))
        return false;

    //最后才Patch
    byte_pattern::set_log_base(0x570000);
    byte_pattern::start_log("gta4.chs");

    bool result = GetMemoryAddresses() && PatchGame();

    byte_pattern::shutdown_log();

    return result;
}

bool CPlugin::GetMemoryAddresses()
{
    batch_matching matcher;

    //变量和函数的地址
    matcher.register_step("8B 3D ? ? ? ? 89 54 24 10", 1, [](const std::vector<memory_pointer>& addresses)
        {
            GameMeta.ppDirect3DDevice9 = *addresses[0].p<IDirect3DDevice9**>(2);
        });
    matcher.register_step("A1 ? ? ? ? 80 7C 24 08 00", 1, [](const std::vector<memory_pointer>& addresses)
        {
            GameMeta.pGraphics_SetRenderState = addresses[0].p();
        });
    matcher.register_step("8B 34 ED ? ? ? ? 0F 2E C1", 1, [](const std::vector<memory_pointer>& addresses)
        {
            GameMeta.pFont_Details = *addresses[0].p<CFontDetails*>(3);
        });
    matcher.register_step("81 3D ? ? ? ? AD 7F 33 31", 2, [](const std::vector<memory_pointer>& addresses)
        {
            GameMeta.pFont_RenderState = *addresses[0].p<CFontRenderState*>(2);
        });
    matcher.register_step("F3 0F 11 05 ? ? ? ? 66 0F 6E 84 24 AC 00 00", 1, [](const std::vector<memory_pointer>& addresses)
        {
            GameMeta.pFont_ResolutionX = *addresses[0].p<float*>(4);
        });
    matcher.register_step("A1 ? ? ? ? 83 F8 FF 75 1E", 1, [](const std::vector<memory_pointer>& addresses)
        {
            GameMeta.pFont_GetRenderIndex = addresses[0].p();
        });
    matcher.register_step("83 EC 30 83 3D ? ? ? ? FF", 1, [](const std::vector<memory_pointer>& addresses)
        {
            GameMeta.pFont_PrintChar = addresses[0].p();
        });
    matcher.register_step("51 55 56 E8", 1, [](const std::vector<memory_pointer>& addresses)
        {
            GameMeta.pFont_GetCharacterSizeNormal = addresses[0].p();
        });
    matcher.register_step("8A 0D ? ? ? ? 0F B6 D1", 1, [](const std::vector<memory_pointer>& addresses)
        {
            GameMeta.pFont_GetCharacterSizeDrawing = addresses[0].p();
        });
    matcher.register_step("83 EC 10 8B 44 24 14 F3 0F 7E 00", 1, [](const std::vector<memory_pointer>& addresses)
        {
            GameMeta.pFont_Render2DPrimitive = addresses[0].p();
        });
    matcher.register_step("8B 54 24 08 53 56 8B 74 24 0C 80 3E 22", 1, [](const std::vector<memory_pointer>& addresses)
        {
            GameMeta.pHash_HashStringFromSeediCase = addresses[0].p();
        });
    matcher.register_step("53 55 56 57 8B F9 85 FF 74 3F", 1, [](const std::vector<memory_pointer>& addresses)
        {
            GameMeta.pDictionary_GetElementByKey = addresses[0].p();
        });

    //-----------------------------------------------------------------------------------------------------------------------------------
    matcher.perform_search();

    if (matcher.is_all_succeed())
    {
        matcher.run_callbacks();
        return true;
    }
    else
    {
        return false;
    }
}
