#include "../GUI.h"

struct ESP_Settings
{
    static inline bool Settings_Player2DBOX = false;
    static inline bool Settings_PlayerHealthBar = false;
    static inline bool Settings_PlayerDistance = false;
    static inline bool Settings_PlayerName = false;
};

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

        CompLayout::TextAlign_Widgets(
            ESP_Settings::Settings_Player2DBOX ? "2D Box - ON" : "2D Box - OFF",
            []()
            {
                // 二维视角 控制开关
                Checkbox_Widgets("#ESP_Settings_2DBOX", &ESP_Settings::Settings_Player2DBOX);
            },
            40.f);

        // CompLayout::TextAlign_Widgets(
        //     "三维视角 - 方框",
        //     []()
        //     {
        //         // 三维视角 控制开关
        //         Checkbox_Widgets("#ESP_Settings_3DBOX", &ESP_Settings::Settings_3DBOX);
        //     },
        //     40.f);

        // CompLayout::TextAlign_Widgets(
        //     "二维人物骨骼 - 线性",
        //     []()
        //     {
        //         // 人物骨骼 控制开关
        //         Checkbox_Widgets("#ESP_Settings_2DSkeleton", &ESP_Settings::Settings_2DSkeleton);
        //     },
        //     40.f);

        // CompLayout::TextAlign_Widgets(
        //     "三维人物骨骼 - 线性",
        //     []()
        //     {
        //         // 人物骨骼 实现功能
        //         Checkbox_Widgets("#ESP_Settings_3DSkeleton", &ESP_Settings::Settings_3DSkeleton);
        //     },
        //     40.f);

        CompLayout::TextAlign_Widgets(
            ESP_Settings::Settings_PlayerHealthBar ? "Player Health - ON" : "2D Box - OFF",
            []()
            {
                // 人物血量 控制开关
                Checkbox_Widgets("#ESP_Settings_HealthBar", &ESP_Settings::Settings_PlayerHealthBar);
            },
            40.f);

        CompLayout::TextAlign_Widgets(
            ESP_Settings::Settings_PlayerDistance ? "Player Distance - ON" : "Player Distance - OFF",
            []()
            {
                // 人物距离 控制开关
                Checkbox_Widgets("#Settings_PlayerDistance", &ESP_Settings::Settings_PlayerDistance);
            },
            40.f);

        CompLayout::TextAlign_Widgets(
            ESP_Settings::Settings_PlayerName ? "Player Name - ON" : "Player Name - OFF",
            []()
            {
                // 人物名称 控制开关
                Checkbox_Widgets("#Settings_PlayerName", &ESP_Settings::Settings_PlayerName);
            },
            40.f);
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Center Panel
    ImGui::BeginChild("Role Settings", ImVec2(ImGui::GetWindowSize().x * 0.4f, ImGui::GetContentRegionAvail().y), false);
    {
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Right Panel
    BeginChild_Layout("ESP Config", ImVec2(ImGui::GetWindowSize().x * 0.3f, ImGui::GetContentRegionAvail().y));
    {
    }
    ImGui::EndChild();
}
