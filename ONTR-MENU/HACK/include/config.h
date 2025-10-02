#pragma once
#include "../../GUI/ImGui/imgui.h"

struct ESP_Settings
{
    static inline bool Player2DBOX_Settings = false;
    static inline bool Player3DBOX_Settings = false;
    static inline bool Player2DSkeleton_Settings = false;
    static inline bool Player3DSkeleton_Settings = false;
    static inline bool PlayerHealthBar_Settings = false;
    static inline bool PlayerDistance_Settings = false;
    static inline bool PlayerName_Settings = false;
};

struct ESP_Data
{
    static inline bool Player2DBOX_Color_BOOL = false;
    static inline ImColor Player2DBOX_Color = ImColor(79, 0, 255, 255);
};

struct AIM_Settings
{
    static inline bool PlayerAIM_Settings = false;
    static inline bool PlayerHeadAIM_Settings = false;
    static inline bool PlayerArmAIM_Settings = false;
    static inline bool PlayerLegsAIM_Settings = false;
    static inline bool PlayerFootAIM_Settings = false;
    static inline bool PlayerChestAIM_Settings = false;
};