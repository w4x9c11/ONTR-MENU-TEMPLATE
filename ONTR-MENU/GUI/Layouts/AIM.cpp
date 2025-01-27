#include "../GUI.h"

void AIM_Layout()
{
    BeginChild_Layout("AIM Settings", ImVec2(ImGui::GetWindowSize().x * 0.3f, ImGui::GetContentRegionAvail().y));
    {
        ImGui::SetCursorPosY(50.f);

        CompLayout::CenterNextWidth(ImGui::GetWindowWidth() * 0.8f);
        Notify_Widgets("[+]\n", "AIM", 0.15f, IconTexture::OK, IconTexture::OK_size, IconTexture::Bar, IconTexture::Bar_size, IconTexture::quantity, IconTexture::quantity_size);
        ImGui::EndChild();

        ImGui::Dummy(ImVec2(0, 30));

        CompLayout::TextAlign_Widgets(
            AIM_Settings::Settings_PlayerAIM ? "Player AIM - ON" : "Player AIM - OFF",
            []()
            {
                // 人物自瞄 控制开关
                Checkbox_Widgets("#Settings_PlayerAIM", &AIM_Settings::Settings_PlayerAIM);
            },
            40.f);

        CompLayout::TextAlign_Widgets(
            AIM_Settings::Settings_PlayerHeadAIM ? "Player Head AIM - ON" : "Player Head AIM - OFF",
            []()
            {
                // 人物头部自瞄 控制开关
                Checkbox_Widgets("#Settings_PlayerHeadAIM", &AIM_Settings::Settings_PlayerHeadAIM);
            },
            40.f);

        CompLayout::TextAlign_Widgets(
            AIM_Settings::Settings_PlayerArmAIM ? "Player Arm AIM - ON" : "Player Arm AIM - OFF",
            []()
            {
                // 人物手臂自瞄 控制开关
                Checkbox_Widgets("#Settings_PlayerArmAIM", &AIM_Settings::Settings_PlayerArmAIM);
            },
            40.f);

        CompLayout::TextAlign_Widgets(
            AIM_Settings::Settings_PlayerLegsAIM ? "Player Legs AIM - ON" : "Player Legs AIM - OFF",
            []()
            {
                // 人物腿部自瞄 控制开关
                Checkbox_Widgets("#Settings_PlayerLegsAIM", &AIM_Settings::Settings_PlayerLegsAIM);
            },
            40.f);

        CompLayout::TextAlign_Widgets(
            AIM_Settings::Settings_PlayerFootAIM ? "Player Foot AIM - ON" : "Player Foot AIM - OFF",
            []()
            {
                // 人物脚部自瞄 控制开关
                Checkbox_Widgets("#Settings_PlayerFootAIM", &AIM_Settings::Settings_PlayerFootAIM);
            },
            40.f);

        CompLayout::TextAlign_Widgets(
            AIM_Settings::Settings_PlayerChestAIM ? "Player Chest AIM - ON" : "Player Chest AIM - OFF",
            []()
            {
                // 人物名称 控制开关
                Checkbox_Widgets("#Settings_PlayerChestAIM", &AIM_Settings::Settings_PlayerChestAIM);
            },
            40.f);
    }
    ImGui::EndChild();

    ImGui::SameLine();
    Figure_Settings(); // 人物面板

    ImGui::SameLine();
    BeginChild_Layout("AIM Config", ImVec2(ImGui::GetWindowSize().x * 0.3f, ImGui::GetContentRegionAvail().y));
    {
    }
    ImGui::EndChild();
}