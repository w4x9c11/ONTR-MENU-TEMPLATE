#pragma once
#include "../../lib/VendorModle/Resources/GUI-icon.h"
#include "../../lib/VendorModle/Resources/GUI-image.h"
#include "../../lib/VendorModle/stb-API.hpp"

#include "../ImGui-UI/imgui_internal.h"

namespace CompLayout
{

    // 组件布局::文本&组件对齐 参数1: 文本内容 参数2: 组件绘制函数 参数3: 组件宽度（可选）
    inline void TextAlign_Widgets(const char *text, std::function<void()> componentDrawFunc, float componentWidth)
    {
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::GetWindowWidth() * 0.8f) * 0.5f);
        ImGui::Text("%s", text);
        ImGui::SameLine();
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::GetWindowWidth() * 0.8f) * 0.5f + ImGui::GetWindowWidth() * 0.8f - componentWidth);
        ImGui::SetNextItemWidth(componentWidth);
        componentDrawFunc();
        ImGui::Dummy(ImVec2(0, 10));
    }

    // 定位组件::水平/垂直
    inline void CenterNextWidth(float Width)
    {
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - Width) * 0.5f);
    }

    inline void CenterNextWH(float Width, float Height)
    {
        ImGui::SetCursorPos(ImVec2((ImGui::GetWindowWidth() - Width) * 0.5f, (ImGui::GetWindowHeight() - Height) * 0.5f));
    }

    // 文本定位::水平/垂直
    template <typename... Args>
    inline void TextCenterF(std::format_string<Args...> fmt, Args &&...args)
    {
        const std::string text = std::format(fmt, std::forward<Args>(args)...);
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(text.c_str()).x) * 0.5f);
        ImGui::TextUnformatted(text.c_str());
    }

} // namespace CompLayout

// IconButton: 参数1: 按钮标签文本, 参数2: 图标数据指针, 参数3: 图标数据大小, 参数4: 图标尺寸, 参数5: 按钮文本（可选）
inline bool IconButton(const char *LabelText, const char *Text, const void *Icon_Data, const size_t Icon_DataSize, const ImVec2 &Icon_Size)
{
    static std::unordered_map<const void *, ID3D11ShaderResourceView *> TextureCache;
    if (!TextureCache[Icon_Data])
        LoadTextureFromMemory(Icon_Data, Icon_DataSize, &TextureCache[Icon_Data], nullptr, nullptr);
    const ImVec2 pos = ImGui::GetCursorPos(), size(Icon_Size.x + (Text ? ImGui::CalcTextSize(Text).x + 5.0f : 0), Icon_Size.y);
    ImGui::PushID(LabelText);
    static constexpr ImVec4 colors[] = {{0, 0, 0, 0}};
    ImGui::PushStyleColor(ImGuiCol_Button, colors[0]);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors[0]);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors[0]);
    const bool clicked = ImGui::Button("##btn", size);
    ImGui::PopStyleColor(3);
    ImGui::SetCursorPos(pos);
    ImGui::Image((ImTextureID)(intptr_t)TextureCache[Icon_Data], Icon_Size);
    if (Text)
    {
        ImGui::SameLine();
        ImGui::SetCursorPosY(pos.y + (Icon_Size.y - ImGui::GetTextLineHeight()) * 0.5f);
        ImGui::TextUnformatted(Text);
    }
    ImGui::PopID();
    return clicked;
}

// StatusBar_Widgets: 参数1: 状态栏标签文本, 参数2: 纹理标识符, 参数3: 背景数据指针, 参数4: 背景数据大小, 参数5: 状态栏文本, 参数6: 组件尺寸（默认空）, 参数7: 主颜色（默认绿色）, 参数8: 侧边颜色（默认白色）
inline bool StatusBar_Widgets(const char *TextureID, const void *BackGround_Data, size_t BackGround_DataSize, const char *Text, ImVec2 WidgesSize = {}, ImU32 MainColor = IM_COL32(0, 175, 60, 255), ImU32 SideColor = 0xFFFFFFFF, bool Clickable = false)
{
    const ImVec2 cursor = ImGui::GetCursorScreenPos(), start_pos = ImGui::GetCursorPos();
    const float sideWidth = WidgesSize.x * 0.2f, iconSize = WidgesSize.y * 0.6f;

    // 绘制背景
    ImGui::GetWindowDrawList()->AddRectFilled(cursor, {cursor.x + WidgesSize.x, cursor.y + WidgesSize.y}, MainColor, WidgesSize.y * 0.2f);
    ImGui::GetWindowDrawList()->AddRectFilled(cursor, {cursor.x + sideWidth, cursor.y + WidgesSize.y}, SideColor, WidgesSize.y * 0.2f, ImDrawFlags_RoundCornersLeft);

    // 绘制图标
    ImGui::SetCursorPos({start_pos.x + (sideWidth - iconSize) * 0.5f, start_pos.y + (WidgesSize.y - iconSize) * 0.5f});
    RenderTexture(TextureID, BackGround_Data, BackGround_DataSize, {iconSize, iconSize});

    // 绘制文本
    const float textWidth = ImGui::CalcTextSize(Text).x;
    ImGui::SetCursorPos({start_pos.x + sideWidth + (WidgesSize.x - sideWidth - textWidth) * 0.5f, start_pos.y + (WidgesSize.y - ImGui::GetTextLineHeight()) * 0.5f});
    ImGui::TextUnformatted(Text);

    // 设置下一个控件的位置
    ImGui::SetCursorPosY(start_pos.y + WidgesSize.y + 5.0f);

    // 检测点击
    if (Clickable)
    {
        ImGui::PushID(TextureID);
        ImGui::SetCursorScreenPos(cursor);
        if (ImGui::InvisibleButton("##StatusBar_Widgets_Clickable", WidgesSize))
        {
            ImGui::PopID();
            return true; // 点击后返回 true
        }
        ImGui::PopID();
    }

    return false; // 默认返回 false
}

// BeginChild_Layout: 参数1: 子窗口标签文本, 参数2: 子窗口尺寸, 参数3: 是否显示边框（默认true）
inline void BeginChild_Layout(const char *LabelText, const ImVec2 &WindowSize, bool WindowBorder = true)
{
    static constexpr struct
    {
        float line = 3.0f, glow = 15.0f;
        int segs = 8;
    } style;
    ImGui::BeginChild(LabelText, WindowSize, WindowBorder); // 窗口圆角
    auto *dl = ImGui::GetWindowDrawList();
    const ImVec2 pos = ImGui::GetWindowPos();
    const float width = ImGui::GetWindowWidth();

    for (int i = 0; i <= style.segs; ++i)
    {
        const float t = i / float(style.segs);
        dl->AddRectFilled(ImVec2(pos.x, pos.y + style.line + i * (style.glow / style.segs)), ImVec2(pos.x + width, pos.y + style.line + (i + 1) * (style.glow / style.segs)), IM_COL32(0, (int)(255.0f * (1.0f - t)), (int)(100.0f * (1.0f - t)), (int)(60.0f * (1.0f - t * t))), 0, 0);
    }

    dl->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + style.line), IM_COL32(0, 255, 100, 255), 0, 0);
}

// Slider_Widgets: 参数1: 滑块标签文本, 参数2: 滑块宽度, 参数3: 滑块值指针, 参数4: 滑块最小值, 参数5: 滑块最大值, 参数6: 值显示格式（默认"%.1f"）
inline bool Slider_Widgets(const char *label, float width, float *value, float min, float max)
{
    static std::unordered_map<const void *, float> lastValues;
    constexpr float height = 6.0f, knobRadius = height * 1.5f;
    ImGui::PushID(label);
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    ImGuiStyle &style = ImGui::GetStyle();
    const ImVec4 oldFrameBgCol = style.Colors[ImGuiCol_FrameBg];
    style.Colors[ImGuiCol_FrameBg] = style.Colors[ImGuiCol_FrameBgHovered] = style.Colors[ImGuiCol_FrameBgActive] = style.Colors[ImGuiCol_SliderGrab] = style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0, 0, 0, 0);
    style.FramePadding = ImVec2(0, 0);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + knobRadius - height / 2);
    ImGui::SetNextItemWidth(width);
    bool changed = ImGui::SliderFloat("##slider", value, min, max, "");
    if (lastValues.find(value) == lastValues.end())
    {
        lastValues[value] = *value;
    }
    if (changed)
    {
        lastValues[value] = *value;
    }
    else if (!ImGui::IsItemActive())
    {
        *value = lastValues[value];
    }
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    const float y = pos.y + knobRadius, sliderPos = (*value - min) / (max - min) * (width - knobRadius * 2);
    draw_list->AddRectFilled(ImVec2(pos.x + knobRadius, y - height / 2), ImVec2(pos.x + width - knobRadius, y + height / 2), IM_COL32(79, 70, 229, 255), height / 2);
    draw_list->AddRectFilled(ImVec2(pos.x + knobRadius, y - height / 2), ImVec2(pos.x + knobRadius + sliderPos, y + height / 2), IM_COL32(50, 255, 100, 255), height / 2);
    draw_list->AddCircleFilled(ImVec2(pos.x + knobRadius + sliderPos, y), knobRadius, ImGui::IsItemActive() ? IM_COL32(200, 200, 200, 255) : IM_COL32(255, 255, 255, 255));
    // if (format)
    // {
    //     ImGui::SameLine();
    //     ImGui::SetCursorPosX(pos.x + width + 10);
    //     ImGui::Text(format, *value);
    // }
    style.Colors[ImGuiCol_FrameBg] = oldFrameBgCol;
    ImGui::PopID();
    return changed;
}

// Checkbox_Widgets: 参数1: 复选框标签文本, 参数2: 复选框值指针
inline bool Checkbox_Widgets(const char *label, bool *value)
{
    static std::unordered_map<const bool *, float> animations;
    constexpr float height = 20.0f, width = 40.0f, radius = height * 0.5f;
    constexpr ImU32 No_Color = IM_COL32(79, 70, 229, 255), On_Color = IM_COL32(34, 197, 94, 255), white = IM_COL32(255, 255, 255, 255);
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    ImGui::PushID(label);
    bool changed = ImGui::InvisibleButton("##", ImVec2(width, height)) ? (*value = !*value, true) : false;
    float &anim = animations.try_emplace(value, *value ? 1.0f : 0.0f).first->second;
    anim = ImLerp(anim, *value ? 1.0f : 0.0f, ImGui::GetIO().DeltaTime * 8.f);
    auto draw = ImGui::GetWindowDrawList();
    draw->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), ImGui::GetColorU32(ImLerp(ImGui::ColorConvertU32ToFloat4(No_Color), ImGui::ColorConvertU32ToFloat4(On_Color), anim)), radius);
    if (ImGui::IsItemHovered())
    {
        draw->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), IM_COL32(255, 255, 255, 18), radius);
    }
    const float knob_x = pos.x + radius + (width - height) * anim;
    draw->AddCircleFilled(ImVec2(knob_x, pos.y + radius), radius - 1.5f, IM_COL32(0, 0, 0, 48));
    draw->AddCircleFilled(ImVec2(knob_x, pos.y + radius), radius - 2.0f, white);
    if (label[0] != '#')
    {
        ImGui::SameLine();
        ImGui::SetCursorPosX(pos.x + width + 8);
        ImGui::Text("%s", label);
    }
    ImGui::PopID();
    return changed;
}

// Notify_Widgets: 参数1: 公告栏窗口名称, 参数2: 公告栏通知文本, 参数3: OK图标数据指针, 参数4: OK图标数据大小, 参数5: 进度条图标数据指针, 参数6: 进度条图标数据大小, 参数7: 数量图标数据指针, 参数8: 数量图标数据大小
inline void Notify_Widgets(const char *WindowName, const char *notice, float WindowHeight, const void *const &State = IconTexture::OK, const size_t State_Data = IconTexture::OK_size, const void *const &Bar_Icon = IconTexture::Bar, const size_t Bar_Size = IconTexture::Bar_size, const void *const &Quantity_Icon = IconTexture::quantity, const size_t Quantity_Size = IconTexture::quantity_size)
{
    static constexpr float PADDING = 15.0f;
    const ImVec2 size(ImGui::GetWindowWidth() * 0.8f, ImGui::GetWindowHeight() * WindowHeight);
    ImGui::BeginChild(WindowName, size, true);
    ImGui::SetCursorPos(ImVec2(PADDING, PADDING));
    RenderTexture("OK", State, State_Data, ImVec2(35.f, 25.f));
    ImGui::SetCursorPos(ImVec2((size.x - ImGui::CalcTextSize(notice).x) * 0.5f, PADDING));
    ImGui::TextUnformatted(notice);
    const float barY = size.y * 0.25f;
    ImGui::SetCursorPos(ImVec2(size.x * 0.25f, barY));
    RenderTexture("Bar", Bar_Icon, Bar_Size, ImVec2(size.x * 0.5f, 8.f));
    ImGui::SetCursorPos(ImVec2(size.x * 0.15f, barY + 10.0f));
    ImGui::TextUnformatted(WindowName);
    ImGui::SetCursorPos(ImVec2(size.x - 80.0f, size.y - 35.0f));
    RenderTexture("quantity", Quantity_Icon, Quantity_Size, ImVec2(25.f, 25.f));
    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);
    ImGui::TextUnformatted("Done");
}

// SelectBox_Widgets: 参数1:标签ID, 参数2:选项列表, 参数3:当前选中项, 参数4:尺寸, 参数5:是否多选模式
inline bool SelectBox_Widgets(const char *l, const std::vector<std::string> &i, int *c, const ImVec2 &s = ImVec2(200, 40))
{
    static constexpr struct
    {
        const ImU32 b = IM_COL32(217, 217, 217, 204), h = IM_COL32(242, 242, 242, 255), a = IM_COL32(0, 122, 255, 255), t = IM_COL32(51, 51, 51, 255), w = IM_COL32(255, 255, 255, 255);
        const float r = 6.f;
    } S;
    const ImVec2 p = ImGui::GetCursorScreenPos();
    auto *d = ImGui::GetWindowDrawList();
    ImGui::PushID(l);
    d->AddRectFilled(p, ImVec2(p.x + s.x, p.y + s.y), S.w, S.r);
    d->AddRect(p, ImVec2(p.x + s.x, p.y + s.y), S.b, S.r);
    const float w = s.x / i.size();
    for (int j = 0; j < i.size(); j++)
    {
        const ImVec2 b(p.x + j * w, p.y), M(b.x + w, b.y + s.y);
        const bool e = (*c == j);
        ImGui::SetCursorScreenPos(b);
        ImGui::InvisibleButton(i[j].c_str(), ImVec2(w, s.y));
        const bool h = ImGui::IsItemHovered();
        if (e || h)
            d->AddRectFilled(b, M, e ? S.a : S.h, S.r);
        if (ImGui::IsItemClicked())
        {
            *c = j;
            ImGui::PopID();
            return 1;
        }
        const ImVec2 t = ImGui::CalcTextSize(i[j].c_str());
        d->AddText(ImVec2(b.x + (w - t.x) * .5f, b.y + (s.y - t.y) * .5f), e ? S.w : S.t, i[j].c_str());
    }
    ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + s.y));
    ImGui::PopID();
    return 0;
}

// ResolutionSelector_Widgets: 参数1:标签ID, 参数2:分辨率列表, 参数3:当前选中项，参数4:尺寸
inline bool ResolutionSelector_Widgets(const char *l, const std::vector<std::string> &i, int *c, const ImVec2 &z = ImVec2(400, 100))
{
    static constexpr struct
    {
        const ImU32 g = IM_COL32(242, 242, 242, 255), b = IM_COL32(217, 217, 217, 204), h = IM_COL32(229, 229, 229, 255), a = IM_COL32(34, 197, 94, 255), w = IM_COL32(255, 255, 255, 255), t = IM_COL32(51, 51, 51, 255);
        const float r = 12.f;
    } s;
    const ImVec2 p = ImGui::GetCursorScreenPos();
    const float w = z.x / i.size();
    auto *d = ImGui::GetWindowDrawList();
    ImGui::PushID(l);
    d->AddRectFilled(p, ImVec2(p.x + z.x, p.y + z.y), s.g, s.r);
    d->AddRect(p, ImVec2(p.x + z.x, p.y + z.y), s.b, s.r);
    for (int j = 0; j < i.size(); j++)
    {
        const ImVec2 b(p.x + j * w, p.y);
        const bool e = (*c == j);
        const ImVec2 m(b.x + w, b.y + z.y);
        const bool h = ImGui::IsMouseHoveringRect(b, m);
        d->AddRectFilled(b, m, e ? s.a : (h ? s.h : s.w), s.r);
        const float q = w * .25f;
        ImGui::SetCursorScreenPos(ImVec2(b.x + (w - q) * .5f, b.y + z.y * .2f));
        RenderTexture("Display_Icon", IconTexture::Display_Icon, IconTexture::Display_Icon_Size, ImVec2(q, q));
        const ImVec2 &t = ImGui::CalcTextSize(i[j].c_str());
        d->AddText(ImVec2(b.x + (w - t.x) * .5f, b.y + z.y * .7f), e ? s.w : s.t, i[j].c_str());
        if (h && ImGui::IsMouseClicked(0))
        {
            *c = j;
            ImGui::PopID();
            return 1;
        }
    }
    ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + z.y));
    ImGui::PopID();
    return 0;
}