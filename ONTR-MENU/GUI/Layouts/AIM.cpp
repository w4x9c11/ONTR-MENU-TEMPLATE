#include "GUI.h"

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
            AIM_Settings::PlayerAIM_Settings ? "PlayerAIM - ON" : "PlayerAIM - OFF",
            []()
            {
                // 人物自瞄 控制开关
                Checkbox_Widgets("#PlayerAIM_Settings", &AIM_Settings::PlayerAIM_Settings);
            },
            40.f);

        CompLayout::TextAlign_Widgets(
            AIM_Settings::PlayerHeadAIM_Settings ? "PlayerHeadAIM - ON" : "PlayerHeadAIM - OFF",
            []()
            {
                // 人物头部自瞄 控制开关
                Checkbox_Widgets("#PlayerHeadAIM_Settings", &AIM_Settings::PlayerHeadAIM_Settings);
            },
            40.f);

        CompLayout::TextAlign_Widgets(
            AIM_Settings::PlayerArmAIM_Settings ? "PlayerArmAIM - ON" : "PlayerArmAIM - OFF",
            []()
            {
                // 人物手臂自瞄 控制开关
                Checkbox_Widgets("#PlayerArmAIM_Settings", &AIM_Settings::PlayerArmAIM_Settings);
            },
            40.f);

        CompLayout::TextAlign_Widgets(
            AIM_Settings::PlayerLegsAIM_Settings ? "PlayerLegsAIM - ON" : "PlayerLegsAIM - OFF",
            []()
            {
                // 人物腿部自瞄 控制开关
                Checkbox_Widgets("#PlayerLegsAIM_Settings", &AIM_Settings::PlayerLegsAIM_Settings);
            },
            40.f);

        CompLayout::TextAlign_Widgets(
            AIM_Settings::PlayerFootAIM_Settings ? "PlayerFootAIM - ON" : "PlayerFootAIM - OFF",
            []()
            {
                // 人物脚部自瞄 控制开关
                Checkbox_Widgets("#PlayerFootAIM_Settings", &AIM_Settings::PlayerFootAIM_Settings);
            },
            40.f);

        CompLayout::TextAlign_Widgets(
            AIM_Settings::PlayerChestAIM_Settings ? "PlayerChestAIM - ON" : "PlayerChestAIM - OFF",
            []()
            {
                // 人物名称 控制开关
                Checkbox_Widgets("#PlayerChestAIM_Settings", &AIM_Settings::PlayerChestAIM_Settings);
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