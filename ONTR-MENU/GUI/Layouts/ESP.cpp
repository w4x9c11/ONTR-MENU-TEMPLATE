#include "GUI.h"

void ESP_Layout()
{
    // Left Panel
    BeginChild_Layout("ESP Settings", ImVec2(ImGui::GetWindowSize().x * 0.3f, ImGui::GetContentRegionAvail().y));
    {
        ImGui::SetCursorPosY(50.f);

        CompLayout::CenterNextWidth(ImGui::GetWindowWidth() * 0.8f);
        Notify_Widgets("[+]\n", "ESP", 0.15f, IconTexture::OK, IconTexture::OK_size, IconTexture::Bar, IconTexture::Bar_size, IconTexture::quantity, IconTexture::quantity_size);
        ImGui::EndChild();

        ImGui::Dummy(ImVec2(0, 30));

        // 二维视角 控制开关
        CompLayout::TextAlign_Widgets(
            ESP_Settings::Player2DBOX_Settings ? "2DBox - ON" : "2DBox - OFF",
            []()
            {
                // 二维矩形 颜色控制器
                if (IconButton("#Config", "", IconTexture::quantity, IconTexture::quantity_size, ImVec2(20, 20)))
                {
                    ESP_Data::Player2DBOX_Color_BOOL = !ESP_Data::Player2DBOX_Color_BOOL;
                }
                if (ESP_Data::Player2DBOX_Color_BOOL)
                {
                    ImGui::ColorPicker4("Box Color", (float *)&ESP_Data::Player2DBOX_Color, ImGuiColorEditFlags_AlphaBar);
                }
                ImGui::SameLine();
                Checkbox_Widgets("#ESP 2DBox", &ESP_Settings::Player2DBOX_Settings);
            },
            70.f);

        CompLayout::TextAlign_Widgets(
            ESP_Settings::Player3DBOX_Settings ? "3D Box - ON" : "3D Box - OFF",
            []()
            {
                // 三维视角 控制开关
                Checkbox_Widgets("#ESP 3DBox", &ESP_Settings::Player3DBOX_Settings);
            },
            40.f);

        CompLayout::TextAlign_Widgets(
            ESP_Settings::Player2DSkeleton_Settings ? "2D Skeleton - ON" : "2D Skeleton - OFF",
            []()
            {
                // 人物骨骼 控制开关
                Checkbox_Widgets("#ESP 2DSkeleton", &ESP_Settings::Player2DSkeleton_Settings);
            },
            40.f);

        CompLayout::TextAlign_Widgets(
            ESP_Settings::Player3DSkeleton_Settings ? "3D Skeleton - ON" : "3D Skeleton - OFF",
            []()
            {
                // 人物骨骼 实现功能
                Checkbox_Widgets("#ESP 3DSkeleton", &ESP_Settings::Player3DSkeleton_Settings); },
            40.f);

        CompLayout::TextAlign_Widgets(
            ESP_Settings::PlayerHealthBar_Settings ? "PlayerHealth - ON" : "PlayerHealth - OFF",
            []()
            {
                // 人物血量 控制开关
                Checkbox_Widgets("#ESP HealthBar", &ESP_Settings::PlayerHealthBar_Settings);
            },
            40.f);

        CompLayout::TextAlign_Widgets(
            ESP_Settings::PlayerDistance_Settings ? "PlayerDistance - ON" : "PlayerDistance - OFF",
            []()
            {
                // 人物距离 控制开关
                Checkbox_Widgets("#ESP Distance", &ESP_Settings::PlayerDistance_Settings);
            },
            40.f);

        CompLayout::TextAlign_Widgets(
            ESP_Settings::PlayerName_Settings ? "PlayerName - ON" : "PlayerName - OFF",
            []()
            {
                // 人物名称 控制开关
                Checkbox_Widgets("#ESP Name", &ESP_Settings::PlayerName_Settings);
            },
            40.f);
    }
    ImGui::EndChild();

    ImGui::SameLine();
    Figure_Settings(); // 人物面板

    ImGui::SameLine();
    // Right Panel
    BeginChild_Layout("ESP Config", ImVec2(ImGui::GetWindowSize().x * 0.3f, ImGui::GetContentRegionAvail().y));
    {
    }
    ImGui::EndChild();
}
