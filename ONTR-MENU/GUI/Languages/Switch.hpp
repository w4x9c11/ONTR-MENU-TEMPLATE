#pragma once
#include "../ImGui/imgui.h"
#include "en_US.h"
#include "zh_CN.h"

namespace Language
{

// 默认使用英文
static bool English = false; // English

// 获取文本
inline const char *GetText(const char *CN_Text, const char *EN_Text)
{
    return English ? CN_Text : EN_Text;
}

// 切换语言
inline void SwitchLanguage()
{
    English = !English;
}

} // namespace Language