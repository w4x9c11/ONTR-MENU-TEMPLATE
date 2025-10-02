/* 固定页面 */
#include "../GUI.h"

// 主窗口
void Panel_Layout()
{
    ImGui::SetNextWindowSize(WindowSize::CalcPanelSize());
    ImGui::Begin("Panel", NULL, ImGuiWindowFlags_NoTitleBar);

    NavBar_Layout();
    SwitchPage();

    ImGui::End();
}

// 顶部栏
void NavBar_Layout()
{
    const float NavBarHeight = 40.0f;                       // 顶部栏高度
    const float IconSize = 25.0f;                           // 图标默认大小
    const float CursorY = (NavBarHeight - IconSize) * 0.5f; // 顶部栏居中

    ImGui::SetCursorPosX(WindowSize::CalcNavBarPosX());
    ImGui::BeginChild("NavBar", WindowSize::CalcNavBarSize(NavBarHeight), true);
    const float NavBarWidth = ImGui::GetWindowWidth();

    // Logo
    RenderTexture("#LOGO", IconTexture::logo, IconTexture::logo_size, ImVec2(40, 40));
    ImGui::SameLine();

    ImGui::SetCursorPosY(CursorY);

    // 按钮
    ImGui::SetCursorPosX(NavBarWidth * 0.05f);
    if (IconButton("#HOME", "Home", IconTexture::home, IconTexture::home_size, ImVec2(IconSize, IconSize)))
    {
        SwitchPage_Data::CurrentPage = 0;
    }
    ImGui::SameLine();

    ImGui::SetCursorPosX(NavBarWidth * 0.1f);
    if (IconButton("#ESP", "ESP", IconTexture::esp, IconTexture::esp_size, ImVec2(IconSize, IconSize)))
    {
        SwitchPage_Data::CurrentPage = 1;
    }
    ImGui::SameLine();

    ImGui::SetCursorPosX(NavBarWidth * 0.15f);
    if (IconButton("#AIM", "AIM", IconTexture::aim, IconTexture::aim_size, ImVec2(IconSize, IconSize)))
    {
        SwitchPage_Data::CurrentPage = 2;
    }
    ImGui::SameLine();

    ImGui::SetCursorPosX(NavBarWidth * 0.2f);
    if (IconButton("#Config", "Config", IconTexture::config, IconTexture::config_size, ImVec2(IconSize, IconSize)))
    {
        SwitchPage_Data::CurrentPage = 3;
    }

    ImGui::SameLine();

    ImGui::SetCursorPosX(NavBarWidth - 50.f);
    IconButton("#Languages", nullptr, IconTexture::languages, IconTexture::languages_size, ImVec2(IconSize, IconSize));

    ImGui::EndChild();
}

// 人物面板
void Figure_Settings()
{
    // Center Panel
    ImGui::BeginChild("figure Settings", ImVec2(ImGui::GetWindowSize().x * 0.4f, ImGui::GetContentRegionAvail().y), false);
    {
        ImVec2 CenterPos = ImVec2((ImGui::GetWindowWidth() - 604) * 0.5f, (ImGui::GetWindowHeight() - 1070) * 0.5f);

        ImGui::SetCursorPos(CenterPos);
        RenderTexture("figure", ImageTexture::figure, ImageTexture::figure_size, ImVec2(604, 1070));
        // 功能判断::矩形边框
        if (ESP_Settings::Player2DBOX_Settings)
        {
            ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ESP_Data::Player2DBOX_Color, 0.0f, 0, 2.0f);
        }

        if (AIM_Settings::PlayerHeadAIM_Settings)
        {
            ImGui::SetCursorPos(ImVec2(CenterPos.x + 290.f, CenterPos.y + 10.f));
            RenderTexture("Red_AIM", IconTexture::Red_AIM, IconTexture::Red_AIM_Size, ImVec2(30, 30));
        }

        if (AIM_Settings::PlayerArmAIM_Settings)
        {
            ImGui::SetCursorPos(ImVec2(CenterPos.x + 95.f, CenterPos.y + 350.f));
            RenderTexture("Green_AIM", IconTexture::Green_AIM, IconTexture::Green_AIM_Size, ImVec2(30, 30));
        }

        if (AIM_Settings::PlayerChestAIM_Settings)
        {
            ImGui::SetCursorPos(ImVec2(CenterPos.x + 290.f, CenterPos.y + 250.f));
            RenderTexture("Yellow_AIM", IconTexture::Yellow_AIM, IconTexture::Yellow_AIM_Size, ImVec2(30, 30));
        }
    }
    ImGui::EndChild();
}