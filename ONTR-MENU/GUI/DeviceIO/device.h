#pragma once

#include "../Elements/PagesData.h"
#include "../ImGui-UI/imgui_impl_dx11.h"
#include "../ImGui-UI/imgui_impl_win32.h"

extern ID3D11Device*            g_pd3dDevice;
extern ID3D11DeviceContext*     g_pd3dDeviceContext;
extern IDXGISwapChain*          g_pSwapChain;
extern bool                     g_SwapChainOccluded;
extern UINT                     g_ResizeWidth , g_ResizeHeight;
extern ID3D11RenderTargetView*  g_mainRenderTargetView;
extern DXGI_SWAP_CHAIN_DESC sd;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void RenderFrame();
void SwitchFullScreen();
