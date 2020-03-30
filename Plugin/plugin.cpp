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
        batch_matching::get_instance().registerStep("80 7C 24 08 00 A1", 1, [&](const std::vector<memory_pointer> &addresses) {Game::Addresses.pGraphics_SetRenderState = addresses[0].p(); });
        batch_matching::get_instance().registerStep("85 C9 53 55 56 57 74 40", 1, [&](const std::vector<memory_pointer> &addresses) {Game::Addresses.pDictionary_GetElementByKey = addresses[0].p(); });
        batch_matching::get_instance().registerStep("25 C7 28 98 22 11 22", 1, [&](const std::vector<memory_pointer> &addresses) {Game::Addresses.pHash_HashStringFromSeediCase = addresses[0].p(); });
        batch_matching::get_instance().registerStep("8D 89 ? ? ? ? 83 C4 48 8D 94 24 A8 00 00 00", 1, [&](const std::vector<memory_pointer> &addresses) {Game::Addresses.pFont_Details = *addresses[0].p<Font::CFontDetails *>(2); });
        batch_matching::get_instance().registerStep("83 C6 2C 83 C7 01 81 FE ? ? ? ?", 1, [&](const std::vector<memory_pointer> &addresses) {Game::Addresses.pFont_RenderState = *addresses[0].p<Font::CFontRenderState*>(8); });
        batch_matching::get_instance().registerStep("D8 35 ? ? ? ? 5E D8 05", 1, [&](const std::vector<memory_pointer> &addresses) {Game::Addresses.pFont_ResolutionX = *addresses[0].p<float *>(2); });
        batch_matching::get_instance().registerStep("A1 ? ? ? ? 83 F8 FF 75 1E", 1, [&](const std::vector<memory_pointer> &addresses) {Game::Addresses.pFont_GetRenderIndex = addresses[0].p(); });
        batch_matching::get_instance().registerStep("83 EC 34 83 3D ? ? ? ? FF", 1, [&](const std::vector<memory_pointer> &addresses) {Game::Addresses.pFont_PrintChar = addresses[0].p(); });
        batch_matching::get_instance().registerStep("83 EC 08 56 57 E8", 1, [&](const std::vector<memory_pointer> &addresses) {Game::Addresses.pFont_GetCharacterSizeNormal = addresses[0].p(); });
        batch_matching::get_instance().registerStep("51 A0 ? ? ? ? 0F B6 15", 1, [&](const std::vector<memory_pointer> &addresses) {Game::Addresses.pFont_GetCharacterSizeDrawing = addresses[0].p(); });
        batch_matching::get_instance().registerStep("83 EC 10 8B 44 24 14 F3 0F 7E 00", 1, [&](const std::vector<memory_pointer> &addresses) {Game::Addresses.pFont_Render2DPrimitive = addresses[0].p(); });
        batch_matching::get_instance().registerStep("0F B7 03 66 3D 20 00", 1, [](const std::vector<memory_pointer> &addresses) {injector::MakeCALL(addresses[0].i(), Font::GetStringWidthHook); });
        batch_matching::get_instance().registerStep("8B 44 24 04 85 C0 75 01 C3 0F B7 08", 1, [](const std::vector<memory_pointer> &addresses) {injector::MakeJMP(addresses[0].i(), Font::SkipAWord); });
        batch_matching::get_instance().registerStep("83 C0 E0 50 E8 ? ? ? ? D8 40", 1, [](const std::vector<memory_pointer> &addresses) {injector::MakeCALL(addresses[0].i(), Font::GetCharacterSizeNormalDispatch); });
        batch_matching::get_instance().registerStep("83 C0 E0 50 E8 ? ? ? ? D8 44 24 18", 1, [](const std::vector<memory_pointer> &addresses) {injector::MakeCALL(addresses[0].i(), Font::GetCharacterSizeNormalDispatch); });
        batch_matching::get_instance().registerStep("66 0F BE 07 50 E8 ? ? ? ?", 1, [](const std::vector<memory_pointer> &addresses) {injector::MakeCALL(addresses[0].i(), Font::GetCharacterSizeNormalDispatch); });
        batch_matching::get_instance().registerStep("6A 01 57 E8 ? ? ? ? D8 06", 1, [](const std::vector<memory_pointer> &addresses) {injector::MakeCALL(addresses[0].i(), Font::GetCharacterSizeDrawingDispatch); });
        batch_matching::get_instance().registerStep("E8 ? ? ? ? D8 44 24 18 83 C4 08 D9 5C 24 10 66 85 F6", 1, [](const std::vector<memory_pointer> &addresses) {injector::MakeCALL(addresses[0].i(), Font::GetCharacterSizeDrawingDispatch); });
        batch_matching::get_instance().registerStep("E8 ? ? ? ? 6A 01 57 E8 ? ? ? ? D8 06", 1, [](const std::vector<memory_pointer> &addresses) {injector::MakeCALL(addresses[0].i(), Font::PrintCharDispatch); });
        batch_matching::get_instance().registerStep("E8 ? ? ? ? 8B 1D ? ? ? ? 83 C4 10 85 DB", 1, [](const std::vector<memory_pointer> &addresses) {injector::MakeCALL(addresses[0].i(), Font::PrintCharDispatch); });
        batch_matching::get_instance().registerStep("8B CE E8 ? ? ? ? 80 3D ? ? ? ? 6A", 2, [](const std::vector<memory_pointer> &addresses) {injector::MakeCALL(addresses[0].i(), Font::LoadTextureCB); injector::MakeCALL(addresses[0].i(), Font::LoadTextureCB); });
        batch_matching::get_instance().registerStep("80 7C 24 04 00 56 8B F1 74 07", 1, [](const std::vector<memory_pointer> &addresses) {injector::MakeJMP(addresses[0].i(), GetTextFileName); });

        //Esc菜单鼠标指向只变色一个词

        //存档名字只取一个字节

        //不同语言的字符设置
    }

    bool Init(HMODULE module)
    {
        wchar_t PluginPath[512];
        GetModuleFileNameW(module, PluginPath, 512);

        RegisterPatchSteps();
        batch_matching::get_instance().peformSearch();
        if (batch_matching::get_instance().isAllSucceed())
        {
            globalTable.LoadTable(std::experimental::filesystem::v1::path{ PluginPath }.parent_path() / "GTAIV.CHS/table.dat");
            batch_matching::get_instance().runCallbacks();
            return true;
        }
        else
        {
            return false;
        }
    }
}
