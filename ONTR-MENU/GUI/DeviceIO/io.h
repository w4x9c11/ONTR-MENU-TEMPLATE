#pragma once
#include "ImGui/imgui.h"

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

        io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\ConSolAB.ttf", 15.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());

        // io.Fonts->Build();
    }

}
