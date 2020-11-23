#include "plugin.h"
#include "font.h"
#include "table.h"
#include "game.h"
#include "batch_matching.h"

static const char * __stdcall GetTextFileName(int)
{
    return "CHINESE.GXT";
}

namespace Plugin
{
    void RegisterPatchSteps()
    {
        byte_pattern::set_log_base(0x570000);

        //变量和函数的地址
        batch_matching::get_instance().register_step("A1 ? ? ? ? 80 7C 24 08 00", 1, [](const std::vector<memory_pointer> &addresses)
        {
            Game::Addresses.pGraphics_SetRenderState = addresses[0].p();
        });
        batch_matching::get_instance().register_step("8B 34 ED ? ? ? ? 0F 2E C1", 1, [](const std::vector<memory_pointer> &addresses)
        {
            Game::Addresses.pFont_Details = *addresses[0].p<Font::CFontDetails *>(3);
        });
        batch_matching::get_instance().register_step("81 3D ? ? ? ? AD 7F 33 31", 2, [](const std::vector<memory_pointer> &addresses)
        {
            Game::Addresses.pFont_RenderState = *addresses[0].p<Font::CFontRenderState*>(2);
        });
        batch_matching::get_instance().register_step("F3 0F 11 05 ? ? ? ? 66 0F 6E 84 24 AC 00 00", 1, [](const std::vector<memory_pointer> &addresses)
        {
            Game::Addresses.pFont_ResolutionX = *addresses[0].p<float *>(4);
        });
        batch_matching::get_instance().register_step("A1 ? ? ? ? 83 F8 FF 75 1E", 1, [](const std::vector<memory_pointer> &addresses)
        {
            Game::Addresses.pFont_GetRenderIndex = addresses[0].p();
        });
        batch_matching::get_instance().register_step("83 EC 30 83 3D ? ? ? ? FF", 1, [](const std::vector<memory_pointer> &addresses)
        {
            Game::Addresses.pFont_PrintChar = addresses[0].p();
        });
        batch_matching::get_instance().register_step("51 55 56 E8", 1, [](const std::vector<memory_pointer> &addresses)
        {
            Game::Addresses.pFont_GetCharacterSizeNormal = addresses[0].p();
        });
        batch_matching::get_instance().register_step("8A 0D ? ? ? ? 0F B6 D1", 1, [](const std::vector<memory_pointer> &addresses)
        {
            Game::Addresses.pFont_GetCharacterSizeDrawing = addresses[0].p();
        });
        batch_matching::get_instance().register_step("83 EC 10 8B 44 24 14 F3 0F 7E 00", 1, [](const std::vector<memory_pointer> &addresses)
        {
            Game::Addresses.pFont_Render2DPrimitive = addresses[0].p();
        });
        batch_matching::get_instance().register_step("8B 54 24 08 53 56 8B 74 24 0C 80 3E 22", 1, [](const std::vector<memory_pointer> &addresses)
        {
            Game::Addresses.pHash_HashStringFromSeediCase = addresses[0].p();
        });
        batch_matching::get_instance().register_step("53 55 56 57 8B F9 85 FF 74 3F", 1, [](const std::vector<memory_pointer> &addresses)
        {
            Game::Addresses.pDictionary_GetElementByKey = addresses[0].p();
        });

        //获取字符串宽度
        batch_matching::get_instance().register_step("0F B7 06 83 F8 20", 1, [](const std::vector<memory_pointer> &addresses)
        {
            injector::MakeCALL(addresses[0].i(), Font::GetStringWidthHook);
        });

        //跳过单词
        batch_matching::get_instance().register_step("57 8B 7C 24 08 85 FF 75 04 33 C0 5F C3 56", 1, [](const std::vector<memory_pointer> &addresses)
        {
            injector::MakeJMP(addresses[0].i(), Font::SkipAWord);
        });

        //获取字符宽度
        batch_matching::get_instance().register_step("83 C0 E0 50 E8 ? ? ? ? D9 5C 24", 2, [](const std::vector<memory_pointer> &addresses)
        {
            injector::MakeCALL(addresses[0].i(4), Font::GetCharacterSizeNormalDispatch);
            injector::MakeCALL(addresses[1].i(4), Font::GetCharacterSizeNormalDispatch);
        });

        batch_matching::get_instance().register_step("6A 01 57 E8 ? ? ? ? D9 5C 24 30", 1, [](const std::vector<memory_pointer> &addresses)
        {
            injector::MakeCALL(addresses[0].i(3), Font::GetCharacterSizeDrawingDispatch);
        });
        batch_matching::get_instance().register_step("EB 12 6A 01 57 E8 ? ? ? ? D9 5C 24 24", 1, [](const std::vector<memory_pointer> &addresses)
        {
            injector::MakeCALL(addresses[0].i(5), Font::GetCharacterSizeDrawingDispatch);
        });

        //这些地方只读了一个字节..但是输入应该不是宽字符
        //batch_matching::get_instance().registerStep("66 0F BE 06 0F B7 C0 50 E8", 1, [](const std::vector<memory_pointer> &addresses) {injector::MakeCALL(addresses[0].i(8), Font::GetCharacterSizeNormalDispatch); });
        //batch_matching::get_instance().registerStep("66 0F BE 04 1E 0F B7 C0 50 E8", 2, [](const std::vector<memory_pointer> &addresses) {injector::MakeCALL(addresses[0].i(9), Font::GetCharacterSizeNormalDispatch); injector::MakeCALL(addresses[1].i(9), Font::GetCharacterSizeNormalDispatch); });

        //绘制字符
        batch_matching::get_instance().register_step("E8 ? ? ? ? 6A 01 57 E8 ? ? ? ? D9 5C 24 30", 1, [](const std::vector<memory_pointer> &addresses)
        {
            injector::MakeCALL(addresses[0].i(), Font::PrintCharDispatch);
        });
        batch_matching::get_instance().register_step("F3 0F 11 0C 24 E8 ? ? ? ? 8B 35", 1, [](const std::vector<memory_pointer> &addresses)
        {
            injector::MakeCALL(addresses[0].i(5), Font::PrintCharDispatch);
        });


        //加载font_chs，即中文字库
        batch_matching::get_instance().register_step("8B CE 50 E8 ? ? ? ? 80 3D ? ? ? ? 6A", 2, [](const std::vector<memory_pointer> &addresses)
        {
            injector::MakeCALL(addresses[0].i(3), Font::LoadTextureCB);
            injector::MakeCALL(addresses[1].i(3), Font::LoadTextureCB);
        });

        //改变GXT的文件名
        batch_matching::get_instance().register_step("80 7C 24 04 00 56 8B F1 74 07", 1, [](const std::vector<memory_pointer> &addresses)
        {
            injector::MakeJMP(addresses[0].i(), GetTextFileName);
        });

        //Esc菜单抬头的鼠标热区，用两个单词的字符串测试

        //存档名字读取2字节

    }

    bool Init(HMODULE module)
    {
        wchar_t PluginPath[512];
        GetModuleFileNameW(module, PluginPath, 512);

        RegisterPatchSteps();
        batch_matching::get_instance().perform_search();
        if (batch_matching::get_instance().is_all_succeed())
        {
            CCharacterTable::GlobalTable.LoadTable(std::filesystem::path{ PluginPath }.parent_path() / "GTA4.CHS/table.dat");
            batch_matching::get_instance().run_callbacks();
            return true;
        }
        else
        {
            return false;
        }
    }
}
