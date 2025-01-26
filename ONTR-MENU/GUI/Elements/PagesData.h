#pragma once
#include "../ImGui-UI/imgui.h"
#include "../../lib/nlohmann/json.hpp"

#include <d3d11.h>
#include <Windows.h>
#include <winhttp.h>

using json = nlohmann::json;


// 数据::页面切换
struct SwitchPage_Data
{
    static inline int CurrentPage = 0;
    static inline int PreviousPage = 0;
};

// 数据::公告通知
struct Notify_Data
{
    static inline float UpdataNotify_Time{};
    static inline std::vector<std::tuple<std::string, std::string, std::string>> Announce;
    static inline bool Loading_Notify{};
};

// 数据::窗口渲染设置
struct Display_Data
{
    static inline bool SwitchVSync = true; // 垂直同步
    static inline float DisplayFPS = 360;  // 帧率限制
    static inline UINT RefreshRate_Numerator = 60;  // 刷新率分子
    static inline UINT RefreshRate_Denominator = 1; // 刷新率分母
    static inline bool SwitchScreenMode = FALSE;    // 窗口模式
    static inline UINT BufferCount = 2;
    static inline int SampleCount = 1; // MSAA采样数量
    static inline int SampleQuality = 0; // MASS采样质量
    static inline BYTE Background_Transparency = 255; // 背景透明度
    static inline float Background_Color[3] = {0.0f, 0.0f, 0.0f};

    // 获取屏幕尺寸
    static inline UINT BufferDesc_WindowWidth = GetSystemMetrics(SM_CXSCREEN);
    static inline UINT BufferDesc_WindowHeight = GetSystemMetrics(SM_CYSCREEN);
};


namespace WindowSize
{
    // 计算主面板大小
    inline static ImVec2 CalcPanelSize()
    {
        ImVec2 display = ImGui::GetIO().DisplaySize;
        return ImVec2(display.x * 0.9f, display.y * 0.85f);
    }

    // 计算导航栏大小
    inline static ImVec2 CalcNavBarSize(float NavBarHeight)
    {
        return ImVec2(ImGui::GetWindowWidth() * 0.9f, NavBarHeight);
    }

    // 计算导航栏X轴位置
    inline static float CalcNavBarPosX()
    {
        float navBarWidth = ImGui::GetWindowWidth() * 0.9f;
        return (ImGui::GetWindowWidth() - navBarWidth) * 0.5f;
    }
}


namespace HomeModule
{
    inline char buf_[128]{};

    inline static const char *Get_CPU()
    {
        DWORD size = sizeof(buf_);
        RegGetValueA(HKEY_LOCAL_MACHINE,
            "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
            "ProcessorNameString", RRF_RT_REG_SZ, 0, buf_, &size);
        return buf_;
    }

    inline static const char *Get_GPU()
    {
        DISPLAY_DEVICEW dd = { sizeof(dd) };
        EnumDisplayDevicesW(nullptr, 0, &dd, 0);
        WideCharToMultiByte(CP_UTF8, 0, dd.DeviceString, -1, buf_, sizeof(buf_),
            nullptr, nullptr);
        return buf_;
    }

    inline static const char *Get_WindowOS()
    {
        DWORD size = sizeof(buf_);
        RegGetValueA(HKEY_LOCAL_MACHINE,
            "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
            "ProductName", RRF_RT_REG_SZ, 0, buf_, &size);
        return buf_;
    }

    inline static void Get_Notify()
    {
        if (Notify_Data::Loading_Notify) return;
        Notify_Data::Loading_Notify = true;

        std::string response;
        auto session_handle = WinHttpOpen(L"Z3RO", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, 0, 0, 0);
        auto connection_handle = session_handle ? WinHttpConnect(session_handle, L"38.165.20.82", INTERNET_DEFAULT_HTTP_PORT, 0) : nullptr;
        auto request_handle = connection_handle ? WinHttpOpenRequest(connection_handle, L"GET", L"/notify.json", 0, 0, 0, 0) : nullptr;

        if (request_handle && WinHttpSendRequest(request_handle, 0, 0, 0, 0, 0, 0) && WinHttpReceiveResponse(request_handle, 0))
        {
            char buffer[4096];
            DWORD bytes_available, bytes_read;
            while (WinHttpQueryDataAvailable(request_handle, &bytes_available) && bytes_available &&
                WinHttpReadData(request_handle, buffer, min(bytes_available, sizeof(buffer)), &bytes_read))
            {
                response += std::string(buffer, bytes_read);
            }

            try
            {
                if (!response.empty())
                {
                    auto json_data = json::parse(response);
                    Notify_Data::Announce.clear();
                    Notify_Data::Announce.reserve(json_data["Announce"].size());

                    for (const auto& item : json_data["Announce"])
                    {
                        Notify_Data::Announce.emplace_back(
                            item["title"].get<std::string>(),
                            item["content"].get<std::string>(),
                            item["date"].get<std::string>()
                        );
                    }
                }
            }
            catch (...) {}
        }
        if (request_handle) WinHttpCloseHandle(request_handle);
        if (connection_handle) WinHttpCloseHandle(connection_handle);
        if (session_handle) WinHttpCloseHandle(session_handle);
        Notify_Data::Loading_Notify = false;
    }
}