#include "device.h"
#include "../Elements/PagesData.h"
#include <dwmapi.h>
#include <dxgi1_5.h>
#include <iostream>

// Global variables definition
ID3D11Device *g_pd3dDevice = nullptr;
ID3D11DeviceContext *g_pd3dDeviceContext = nullptr;
IDXGISwapChain *g_pSwapChain = nullptr;
bool g_SwapChainOccluded = false;
UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
ID3D11RenderTargetView *g_mainRenderTargetView = nullptr;
DXGI_SWAP_CHAIN_DESC sd;
HWND hwnd;

bool CreateDeviceD3D(HWND hWnd)
{
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = Display_Data::BufferCount;
    sd.BufferDesc.Width = Display_Data::BufferDesc_WindowWidth;
    sd.BufferDesc.Height = Display_Data::BufferDesc_WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = Display_Data::RefreshRate_Numerator;
    sd.BufferDesc.RefreshRate.Denominator = Display_Data::RefreshRate_Denominator;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = Display_Data::SampleCount;
    sd.SampleDesc.Quality = Display_Data::SampleQuality;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;

    const D3D_FEATURE_LEVEL featureLevelArray[2] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0,
    };

    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain,
        &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);

    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr,
            createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain,
            &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);

    if (FAILED(res))
        return false;

    CreateRenderTarget();

    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain)
    {
        g_pSwapChain->Release();
        g_pSwapChain = nullptr;
    }
    if (g_pd3dDeviceContext)
    {
        g_pd3dDeviceContext->Release();
        g_pd3dDeviceContext = nullptr;
    }
    if (g_pd3dDevice)
    {
        g_pd3dDevice->Release();
        g_pd3dDevice = nullptr;
    }
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice)
    {
        g_pd3dDevice->Release();
        g_pd3dDevice = nullptr;
    }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;

    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView)
    {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = nullptr;
    }
    if (g_mainRenderTargetView)
    {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = nullptr;
    }
}

void RenderFrame()
{
    const float clear_color_with_alpha[4] = {Display_Data::Background_Color[0], Display_Data::Background_Color[1], Display_Data::Background_Color[2], Display_Data::Background_Transparency / 255.0f};

    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Present
    HRESULT hr = g_pSwapChain->Present(Display_Data::SwitchVSync ? 1 : 0, 0);
    g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
}

void SwitchFullScreen()
{
}