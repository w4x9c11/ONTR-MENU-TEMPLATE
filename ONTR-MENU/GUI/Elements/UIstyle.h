#pragma once
#include "../ImGui-UI/imgui.h"

namespace Style
{
    struct Colors
    {
        ImVec4 GreenColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    };
    inline Colors g_Colors;

    class ThemeManager
    {
        public:
        // 基础样式初始化
        static void InitBase()
        {
            auto& style = ImGui::GetStyle();

            style.ItemSpacing = ImVec2(5.0f, 0.0f); // 项之间的间距


            // 窗口样式
            style.WindowPadding = ImVec2(0.f, 0.f);
            // style.WindowRounding = 10.0f;
            style.WindowBorderSize = 0.0f;

            // 子窗口样式
            style.ChildRounding = 10.0f;
            style.ChildBorderSize = 2.0f;

            // 框架样式
            style.FramePadding = ImVec2(10.0f, 10.0f);
            // style.FrameRounding = 10.0f;
            style.FrameBorderSize = .0f;

            // 其他圆角
            // style.PopupRounding = 10.0f;
            style.ScrollbarRounding = 10.0f;
            style.GrabRounding = 10.0f;
            style.TabRounding = 10.0f;
        }

        // 亮色主题
        static void ApplyLight()
        {
            InitBase();
            auto& colors = ImGui::GetStyle().Colors;
            colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
            colors[ImGuiCol_WindowBg] = ImVec4(1.f, 1.f, 1.f, 1.00f);
            colors[ImGuiCol_ChildBg] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
            colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
            colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
            colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
            colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
            colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        }

        // 暗色主题
        static void ApplyDark()
        {
            InitBase();
            auto& colors = ImGui::GetStyle().Colors;

            colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
            colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
            colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
            colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
            colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
            colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
            colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
            colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
        }
    };
} // namespace Style