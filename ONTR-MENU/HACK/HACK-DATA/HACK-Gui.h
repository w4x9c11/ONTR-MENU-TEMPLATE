#pragma once
#include "../../GUI/ImGui-UI/imgui.h"

struct ESP_Settings
{
    static inline bool Settings_Player2DBOX = false;
    static inline bool Settings_Player3DBOX = false;
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