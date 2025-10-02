#include "../DeviceIO/device.h"
#include "../GUI.h"

extern HWND hwnd;

void Home_Layout()
{
    // Left Panel

    // 初始化获取公告时间
    // (ImGui::GetTime() - Notify_Data::UpdataNotify_Time >= 30.f || (Notify_Data::Announce.empty() && !Notify_Data::Loading_Notify)) ? (Notify_Data::UpdataNotify_Time = static_cast<float>(ImGui::GetTime()), HomeModule::Get_Notify()) : void();

    BeginChild_Layout("Notify & Help", {ImGui::GetWindowWidth() * 0.3f, ImGui::GetContentRegionAvail().y});
    {
        ImGui::SetCursorPosY(50);

        CompLayout::CenterNextWidth(ImGui::GetWindowWidth() * 0.8f);
        for (const auto &[title, content, date] : Notify_Data::Announce)
        {
            CompLayout::CenterNextWidth(ImGui::GetWindowWidth() * 0.8f); // 居中
            // 获取公告
            // Notify_Widgets(content.c_str(),
            //     (date + ": " + title).c_str(),
            //     0.2f,
            //     IconTexture::OK, IconTexture::OK_size, IconTexture::Bar, IconTexture::Bar_size, IconTexture::quantity, IconTexture::quantity_size);
            // ImGui::EndChild();

            ImGui::Dummy(ImVec2(0, 30)); // 间距
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    RenderTexture("Home_BackGround", ImageTexture::home_background, ImageTexture::home_background_size, ImVec2(ImGui::GetWindowWidth() * 0.4f, ImGui::GetContentRegionAvail().y));

    ImGui::SameLine();

    // Right Panel
    BeginChild_Layout("Status Check", ImVec2(ImGui::GetWindowSize().x * 0.3f, ImGui::GetContentRegionAvail().y));
    {
        ImGui::SetCursorPosY(50);
        const ImVec2 Widget_Size(ImGui::GetWindowWidth() * 0.8f, 40);

        CompLayout::CenterNextWidth(Widget_Size.x);
        StatusBar_Widgets(
            "State",
            IconTexture::state,
            IconTexture::state_size,
            HomeModule::Get_CPU(),
            Widget_Size);

        ImGui::Dummy(ImVec2(0, 30));

        CompLayout::CenterNextWidth(Widget_Size.x);
        StatusBar_Widgets(
            "State",
            IconTexture::state,
            IconTexture::state_size,
            HomeModule::Get_GPU(),
            Widget_Size);

        ImGui::Dummy(ImVec2(0, 30));

        CompLayout::CenterNextWidth(Widget_Size.x);
        StatusBar_Widgets(
            "State",
            IconTexture::state,
            IconTexture::state_size,
            HomeModule::Get_WindowOS(),
            Widget_Size);

        ImGui::Dummy(ImVec2(0, 30));

        CompLayout::CenterNextWidth(ImGui::GetWindowWidth() * 0.8f);
        Notify_Widgets(
            "Some display settings are only effective for GPUs of the 20 series or higher! (Recommended default)",
            "GPU Settings",
            0.15f,
            IconTexture::OK, IconTexture::OK_size, IconTexture::Bar, IconTexture::Bar_size, IconTexture::quantity, IconTexture::quantity_size);
        ImGui::EndChild();

        ImGui::Dummy(ImVec2(0, 50));

        CompLayout::TextAlign_Widgets(
            (std::string(Display_Data::SwitchVSync ? "Vertical Sync " : "Quick Sync ") +
                std::to_string(static_cast<int>(ImGui::GetIO().Framerate)) + " FPS")
                .c_str(),
            []()
            { Checkbox_Widgets("##SwitchVSync", &Display_Data::SwitchVSync); },
            40.f);
        if (!Display_Data::SwitchVSync)
        {
            CompLayout::TextAlign_Widgets(
                "Frame Rate Limit",
                []()
                {
                    if (Slider_Widgets("##DisplayFPS", 300.f, &Display_Data::DisplayFPS, 60.0f, 360.0f))
                    {
                    }
                },
                300.f);
        }

        CompLayout::TextAlign_Widgets(
            Display_Data::SwitchScreenMode ? "Full-Screen Mode" : "Borderless Mode",
            []()
            {
                if (Checkbox_Widgets("##fullscreen", &Display_Data::SwitchScreenMode))
                {
                    SwitchFullScreen();
                }
            },
            40.f);

        CompLayout::TextAlign_Widgets(
            ("Number of Buffers:" + std::to_string(Display_Data::BufferCount) + "BC").c_str(),
            []()
            {
                static float BufferCount = static_cast<float>(Display_Data::BufferCount);
                if (Slider_Widgets(
                        "##BufferCount", 300.f, &BufferCount, 1.0f, 3.0f))
                {
                    Display_Data::BufferCount = static_cast<int>(BufferCount);
                }
            },
            300.f);

        CompLayout::TextAlign_Widgets(
            ("Refresh Rate Numerator:" + std::to_string(Display_Data::RefreshRate_Numerator) + "Hz").c_str(),
            []()
            {
                static float Numerator = static_cast<float>(Display_Data::RefreshRate_Numerator);
                if (Slider_Widgets(
                        "##RefreshRate_Numerator", 300.f, &Numerator, 30.0f, 360.0f))
                {
                    Display_Data::RefreshRate_Numerator = static_cast<int>(Numerator);
                }
            },
            300.f);

        CompLayout::TextAlign_Widgets(
            ("Refresh Rate Denominator:" + std::to_string(Display_Data::RefreshRate_Denominator)).c_str(),
            []()
            {
                static float Denominator = static_cast<float>(Display_Data::RefreshRate_Denominator);
                if (Slider_Widgets(
                        "##RefreshRate_Denominator", 300.f, &Denominator, 1.0f, 1.0f))
                {
                    Display_Data::RefreshRate_Denominator = static_cast<int>(Denominator);
                }
            },
            300.f);

        CompLayout::TextAlign_Widgets(
            ("MSAA Sample Count:" + std::to_string(Display_Data::SampleCount) + "X").c_str(),
            []()
            {
                static float msaa = static_cast<float>(Display_Data::SampleCount);
                if (Slider_Widgets("##SampleCount", 300.f, &msaa, 1.0f, 8.0f))
                {
                    Display_Data::SampleCount = static_cast<int>(msaa);
                    CleanupRenderTarget();
                    g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
                    CreateRenderTarget();
                }
            },
            300.f);

        CompLayout::TextAlign_Widgets(
            ("MSAA Sample Quality:" + std::to_string(Display_Data::SampleQuality) + "X").c_str(),
            []()
            {
                static float quality = static_cast<float>(Display_Data::SampleQuality);
                if (Slider_Widgets("##SampleQuality", 300.f, &quality, 0.0f, 4.0f))
                {
                    Display_Data::SampleQuality = static_cast<int>(quality);
                    CleanupRenderTarget();
                    g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
                    CreateRenderTarget();
                }
            },
            300.f);

        CompLayout::TextAlign_Widgets(
            "Background Settings",
            []()
            {
                // 透明度滑块
                static float Alpha = 1.0f;
                if (Slider_Widgets("#Alpha", 300.f, &Alpha, 0.4f, 1.0f))
                {
                    SetLayeredWindowAttributes(hwnd, 0, (BYTE)(Alpha * 255), LWA_ALPHA);
                }

                // 颜色选择器
                ImGui::ColorEdit3("#color", Display_Data::Background_Color,
                    ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
            },
            300.f);

        CompLayout::TextAlign_Widgets(
            !NVIDIA_Performance::EnableHighPerformance ? "Balanced Mode" : "Maximum Performance Mode",
            []()
            {
                if (Checkbox_Widgets(
                        "##EnableHighPerformance",
                        &NVIDIA_Performance::EnableHighPerformance))
                {
                    NVIDIA::SDK::SetPerformanceMode();
                }
            },
            40.f);

        CompLayout::CenterNextWidth(400.f);
        ImGui::BeginGroup();
        {
            static const std::vector<std::string> resolutions_str = {"1920x1080", "2560x1440", "3840x2160"};
            static const std::vector<ImVec2> resolutions = {ImVec2(1920, 1080), ImVec2(2560, 1440), ImVec2(3840, 2160)};
            static int Current_Res = 0;

            if (ResolutionSelector_Widgets("##DisplayResolution", resolutions_str, &Current_Res))
            {
                NVIDIA::SDK::SetResolution(static_cast<int>(resolutions[Current_Res].x), static_cast<int>(resolutions[Current_Res].y), 144);
            }
            // 显示当前分辨率
            ImGui::Text(
                "Current Resolution: %dx%d", (int)resolutions[Current_Res].x, (int)resolutions[Current_Res].y);
        }
        ImGui::EndGroup();
    }
    ImGui::EndChild();
}