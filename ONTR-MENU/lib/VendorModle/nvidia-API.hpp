#pragma once
// 系统头文件
#include <format>
#include <string>
#include <unordered_set>
#include <vector>
#include <windows.h>

// NVIDIA SDK
#include "NVIDIA-SDK/nvapi.h"
#include "NVIDIA-SDK/nvapi_lite_common.h"

struct NVIDIA_Performance
{
    static inline bool EnableHighPerformance = false;
    // NVIDIA性能状态ID
    static inline constexpr DWORD PREFERRED_PSTATE_ID = 0x1057EB71;
    // 性能状态值
    static inline constexpr DWORD PREFERRED_PSTATE_DEFAULT = 0x0;    // 默认平衡
    static inline constexpr DWORD PREFERRED_PSTATE_PREFER_MAX = 0x1; // 最大性能
    static inline constexpr DWORD PREFERRED_PSTATE_MIN = 0x2;        // 最小性能
    static inline constexpr DWORD PREFERRED_PSTATE_OPTIMAL = 0x3;    // 最佳性能
};

namespace NVIDIA::SDK
{

    // 初始化 NVIDIA API
    inline bool InitializeNVAPI()
    {
        static bool initialized = false;
        if (initialized)
            return true;
        NvAPI_Status status = NvAPI_Initialize();
        if (status != NVAPI_OK)
            return false;
        NvDisplayHandle displayHandle = nullptr;
        if (NvAPI_EnumNvidiaDisplayHandle(0, &displayHandle) != NVAPI_OK)
        {
            NvAPI_Unload();
            return false;
        }
        NvU32 pathCount = 0;
        if (NvAPI_DISP_GetDisplayConfig(&pathCount, nullptr) != NVAPI_OK ||
            pathCount == 0)
        {
            NvAPI_Unload();
            return false;
        }
        initialized = true;
        return true;
    }

    // NVIDIA::SDK 获取分辨率列表
    inline std::vector<std::string> GetResolutionList()
    {
        static std::vector<std::string> cache;
        if (!cache.empty())
            return cache;
        if (!InitializeNVAPI())
            return {};
        DEVMODE DisplayMode{};
        DisplayMode.dmSize = sizeof(DisplayMode);
        std::unordered_set<std::string> unique;
        for (int i = 0; EnumDisplaySettings(NULL, i, &DisplayMode) != 0; i++)
        {
            unique.emplace(std::format("{}x{}@{}Hz", DisplayMode.dmPelsWidth,
                DisplayMode.dmPelsHeight,
                DisplayMode.dmDisplayFrequency));
        }
        cache = {unique.begin(), unique.end()};
        return cache;
    }
    // NVIDIA::SDK 设置分辨率
    inline bool SetResolution(int DisplayWidth, int DisplayHeight, int hz)
    {
        if (!InitializeNVAPI())
            return false;
        DEVMODE DisplayMode{};
        DisplayMode.dmSize = sizeof(DisplayMode);
        DisplayMode.dmPelsWidth = static_cast<DWORD>(DisplayWidth);
        DisplayMode.dmPelsHeight = static_cast<DWORD>(DisplayHeight);
        DisplayMode.dmDisplayFrequency = static_cast<DWORD>(hz);
        DisplayMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
        return ChangeDisplaySettings(&DisplayMode, CDS_FULLSCREEN) ==
               DISP_CHANGE_SUCCESSFUL;
    }

    // NVIDIA::SDK 显卡性能设置
    inline bool SetPerformanceMode()
    {
        struct Result
        {
            bool success = false;
            NvDRSSessionHandle session = nullptr;
            ~Result()
            {
                if (session)
                    NvAPI_DRS_DestroySession(session);
            }
        } result;

        if (!InitializeNVAPI() ||
            NvAPI_DRS_CreateSession(&result.session) != NVAPI_OK ||
            NvAPI_DRS_LoadSettings(result.session) != NVAPI_OK)
        {
            return false;
        }

        NvDRSProfileHandle profile = nullptr;
        if (NvAPI_DRS_GetCurrentGlobalProfile(result.session, &profile) != NVAPI_OK)
        {
            return false;
        }

        NVDRS_SETTING setting = {
            .version = NVDRS_SETTING_VER,
            .settingId = NVIDIA_Performance::PREFERRED_PSTATE_ID,
            .settingType = NVDRS_DWORD_TYPE,
            .u32CurrentValue = NVIDIA_Performance::EnableHighPerformance
                                   ? NVIDIA_Performance::PREFERRED_PSTATE_PREFER_MAX
                                   : NVIDIA_Performance::PREFERRED_PSTATE_DEFAULT};

        result.success =
            NvAPI_DRS_SetSetting(result.session, profile, &setting) == NVAPI_OK &&
            NvAPI_DRS_SaveSettings(result.session) == NVAPI_OK;

        return result.success;
    }

}