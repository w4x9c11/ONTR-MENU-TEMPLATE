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
    if (IconButton("#HOME", IconTexture::home, IconTexture::home_size, ImVec2(IconSize, IconSize), "Home"))
    {
        SwitchPage_Data::CurrentPage = 0;
    }
    ImGui::SameLine();
    
    ImGui::SetCursorPosX(NavBarWidth * 0.1f);
    if (IconButton("#ESP", IconTexture::esp, IconTexture::esp_size, ImVec2(IconSize, IconSize), "ESP"))
    {
        SwitchPage_Data::CurrentPage = 1;
    }
    ImGui::SameLine();

    ImGui::SetCursorPosX(NavBarWidth * 0.15f);
    if (IconButton("#AIM", IconTexture::aim, IconTexture::aim_size, ImVec2(IconSize, IconSize), "AIM"))
    {
        SwitchPage_Data::CurrentPage = 2;
    }
    ImGui::SameLine();

    ImGui::SetCursorPosX(NavBarWidth * 0.2f);
    if (IconButton("#Config", IconTexture::config, IconTexture::config_size, ImVec2(IconSize, IconSize), "Config"))
    {
        SwitchPage_Data::CurrentPage = 3;
    }

    ImGui::SameLine();

    ImGui::SetCursorPosX(NavBarWidth - 50.f);
    IconButton("#Languages", IconTexture::languages, IconTexture::languages_size, ImVec2(IconSize, IconSize), nullptr);

    ImGui::EndChild();
}
