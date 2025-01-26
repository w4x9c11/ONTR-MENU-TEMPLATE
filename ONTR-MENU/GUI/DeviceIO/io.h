#pragma once
#include "../ImGui-UI/imgui.h"

namespace IO
{
    inline void IoPut()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        (void)io;

        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

        ImGui::StyleColorsClassic();

        // ImGuiStyle& style = ImGui::GetStyle();
        // if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        // {
        //     style.WindowRounding = 0.0f;
        //     style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        // }
    }

    inline void ImGuiFonts()
    {
        ImGuiIO &io = ImGui::GetIO();

        // 中文字体
        io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msyhbd.ttc", 18.0f, nullptr,
            io.Fonts->GetGlyphRangesChineseFull());

        // Emoji配置
        static const ImWchar ranges[] = {
            0x0001, // 起始范围
            0xFFFF, // 结束范围
            0x0000  // 结束标记
        };

        static ImFontConfig cfg;
        cfg.OversampleH = cfg.OversampleV = 1;
        cfg.MergeMode = true;

        // 加载彩色Emoji
        io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\seguiemj.ttf", 18.0f, &cfg, ranges);

        io.Fonts->Build();
    }

} // namespace IO
