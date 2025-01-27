#include "../GUI.h"

void Config_Layout()
{
    CompLayout::CenterNextWidth(ImGui::GetWindowSize().x * 0.5f);
    BeginChild_Layout("Config Settings", ImVec2(ImGui::GetWindowSize().x * 0.5f, ImGui::GetContentRegionAvail().y));
    {
        ImGui::SetCursorPosY(50);

        if (StatusBar_Widgets("Telegram_Icon", IconTexture::Telegram, IconTexture::Telegram_Size, "Join Telegram", ImVec2(ImGui::GetWindowWidth() * 0.3f, 40), IM_COL32(79, 0, 255, 255), 0xFFFFFFFF, true))
        {
            ShellExecuteA(0, 0, "https://telegram.org", 0, 0, SW_SHOW);
        }

        ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::GetContentRegionAvail().x * 0.65f);

        if (StatusBar_Widgets("Official_Website_Icon", IconTexture::Official_Website, IconTexture::Official_Website_Size, "Official Website", ImVec2(ImGui::GetWindowWidth() * 0.3f, 40), IM_COL32(79, 0, 255, 255), 0xFFFFFFFF, true))
        {
            ShellExecuteA(0, 0, "https://YRBOT.org", 0, 0, SW_SHOW);
        }

        ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::GetContentRegionAvail().x * 0.3f);

        if (StatusBar_Widgets("CustomerService_Icon", IconTexture::CustomerService, IconTexture::CustomerService_Size, "Customer Service", ImVec2(ImGui::GetWindowWidth() * 0.3f, 40), IM_COL32(79, 0, 255, 255), 0xFFFFFFFF, true))
        {
            ShellExecuteA(0, 0, "https://YRBOT.org", 0, 0, SW_SHOW);
        }
    }
    ImGui::EndChild();
}