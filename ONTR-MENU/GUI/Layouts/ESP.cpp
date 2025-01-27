#include "../GUI.h"

struct ESP_Settings
{
    static inline bool Settings_Player2DBOX = false;
    static inline bool Settings_3DBOX = false;
    static inline bool Settings_2DSkeleton = false;
    static inline bool Settings_3DSkeleton = false;
    static inline bool Settings_PlayerHealthBar = false;
    static inline bool Settings_PlayerDistance = false;
    static inline bool Settings_PlayerName = false;
};

struct ESP_Data
{
    static inline bool Player2DBOX_Color_BOOL = false;
    static inline ImColor Player2DBOX_Color = ImColor(79, 0, 255, 255);
};

struct AIM_Settings
{
    static inline bool Settings_PlayerAIM = false;
    static inline bool Settings_PlayerHeadAIM = false;
    static inline bool Settings_PlayerArmAIM = false;
    static inline bool Settings_PlayerLegsAIM = false;
    static inline bool Settings_PlayerFootAIM = false;
    static inline bool Settings_PlayerChestAIM = false;
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

        // 二维视角 控制开关
        CompLayout::TextAlign_Widgets(
            ESP_Settings::Settings_Player2DBOX ? "2D Box - ON" : "2D Box - OFF",
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
                Checkbox_Widgets("#ESP_Settings_2DBOX", &ESP_Settings::Settings_Player2DBOX);
            },
            70.f);

        CompLayout::TextAlign_Widgets(
            "三维视角 - 方框",
            []()
            {
                // 三维视角 控制开关
                Checkbox_Widgets("#ESP_Settings_3DBOX", &ESP_Settings::Settings_3DBOX);
            },
            40.f);

        CompLayout::TextAlign_Widgets(
            "二维人物骨骼 - 线性",
            []()
            {
                // 人物骨骼 控制开关
                Checkbox_Widgets("#ESP_Settings_2DSkeleton", &ESP_Settings::Settings_2DSkeleton);
            },
            40.f);

        CompLayout::TextAlign_Widgets(
            "三维人物骨骼 - 线性",
            []()
            {
                // 人物骨骼 实现功能
                Checkbox_Widgets("#ESP_Settings_3DSkeleton", &ESP_Settings::Settings_3DSkeleton);
            },
            40.f);

        CompLayout::TextAlign_Widgets(
            ESP_Settings::Settings_PlayerHealthBar ? "Player Health - ON" : "Player Health - OFF",
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

    // Center Panel
    ImGui::BeginChild("Role Settings", ImVec2(ImGui::GetWindowSize().x * 0.4f, ImGui::GetContentRegionAvail().y), false);
    {
        ImVec2 CenterPos = ImVec2((ImGui::GetWindowWidth() - 604) * 0.5f, (ImGui::GetWindowHeight() - 1070) * 0.5f);

        ImGui::SetCursorPos(CenterPos);
        RenderTexture("figure", ImageTexture::figure, ImageTexture::figure_size, ImVec2(604, 1070));
        // 功能判断::矩形边框
        if (ESP_Settings::Settings_Player2DBOX)
        {
            ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ESP_Data::Player2DBOX_Color, 0.0f, 0, 2.0f);
        }

        if (AIM_Settings::Settings_PlayerHeadAIM)
        {
            ImGui::SetCursorPos(ImVec2(CenterPos.x + 290.f, CenterPos.y + 10.f));
            RenderTexture("Red_AIM", IconTexture::Red_AIM, IconTexture::Red_AIM_Size, ImVec2(30, 30));
        }

        if (AIM_Settings::Settings_PlayerArmAIM)
        {
            ImGui::SetCursorPos(ImVec2(CenterPos.x + 95.f, CenterPos.y + 350.f));
            RenderTexture("Green_AIM", IconTexture::Green_AIM, IconTexture::Green_AIM_Size, ImVec2(30, 30));
        }

        if (AIM_Settings::Settings_PlayerChestAIM)
        {
            ImGui::SetCursorPos(ImVec2(CenterPos.x + 290.f, CenterPos.y + 250.f));
            RenderTexture("Yellow_AIM", IconTexture::Yellow_AIM, IconTexture::Yellow_AIM_Size, ImVec2(30, 30));
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Right Panel
    BeginChild_Layout("ESP Config", ImVec2(ImGui::GetWindowSize().x * 0.3f, ImGui::GetContentRegionAvail().y));
    {
    }
    ImGui::EndChild();
}
