#pragma once
#include "../Elements/Animation.h"
#include "../Elements/PagesData.h"

void Panel_Layout();
void NavBar_Layout();

void Home_Layout();
void ESP_Layout();
void AIM_Layout();
void Config_Layout();

inline void (*Pages[])() = {Home_Layout, ESP_Layout, AIM_Layout, Config_Layout};

inline void SwitchPage()
{
    if (SwitchPage_Data::CurrentPage < 0 || SwitchPage_Data::CurrentPage >= sizeof(Pages) / sizeof(Pages[0]))
        return;

    auto& animation = SlideAnimation::Get();
    if (SwitchPage_Data::PreviousPage != SwitchPage_Data::CurrentPage)
    {
        SwitchPage_Data::PreviousPage = SwitchPage_Data::CurrentPage;
        animation.StartAnimation();
    }

    // 渲染进度条
    animation.RenderProgressBar();

    if (animation.IsAnimating())
    {
        float scale, alpha;
        animation.GetAnimationValues(scale, alpha);
        const ImVec2& size = ImGui::GetWindowSize();

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        ImGui::SetCursorPos(
            ImVec2(size.x * (1.0f - scale) * 0.5f, SlideAnimation::BAR_Y_OFFSET + size.y * (1.0f - scale)));

        Pages[SwitchPage_Data::CurrentPage]();
        ImGui::PopStyleVar();
        animation.Update();
    }
    else
    {
        ImGui::SetCursorPos(ImVec2(0.0f, SlideAnimation::BAR_Y_OFFSET));
        Pages[SwitchPage_Data::CurrentPage]();
    }
}