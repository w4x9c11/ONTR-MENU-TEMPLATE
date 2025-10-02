#pragma once
#include "../../GUI/DeviceIO/device.h"
#include "../../GUI/ImGui/imgui.h"
#include "../stb/stb_image.h"
#include "../stb/stb_image_resize.h"
#include <mutex>
#include <unordered_map>

namespace
{
    inline static std::mutex textureCacheMutex;                                             // 用于保护纹理缓存访问的互斥锁
    inline static std::unordered_map<std::string, ID3D11ShaderResourceView *> textureCache; // 纹理缓存映射表, 键为纹理标识符, 值为对应的着色器资源视图
    inline static constexpr size_t MAX_TEXTURE_CACHE_SIZE = 32;                             // 限制最大缓存数量
} // namespace

inline bool LoadTextureFromMemory(const void *data, size_t data_size, ID3D11ShaderResourceView **out_srv, int *out_width, int *out_height)
{
    // 输入验证
    if (!data || data_size == 0 || !out_srv)
    {
        OutputDebugStringA("Invalid input parameters\n");
        return false;
    }

    // 加载图片数据
    int image_width = 0, image_height = 0;
    unsigned char *image_data = stbi_load_from_memory(
        static_cast<const unsigned char *>(data), static_cast<int>(data_size),
        &image_width, &image_height, nullptr, 4);

    if (!image_data)
    {
        const char *error = stbi_failure_reason();
        OutputDebugStringA(error);
        return false;
    }

    // 创建纹理描述
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = image_width;
    desc.Height = image_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    // 创建纹理数据
    D3D11_SUBRESOURCE_DATA subResource = {};
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;

    // 创建纹理
    ID3D11Texture2D *pTexture = nullptr;
    HRESULT hr = g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

    // 释放图片数据
    stbi_image_free(image_data);

    if (FAILED(hr))
    {
        printf("Failed to create texture: %ld\n", hr); // 修改格式说明符
        return false;
    }

    // 创建着色器资源视图
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    hr = g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
    pTexture->Release();

    if (FAILED(hr))
    {
        printf("Failed to create shader resource view: %ld\n",
            hr); // 修改格式说明符
        return false;
    }

    if (out_width)
        *out_width = image_width;
    if (out_height)
        *out_height = image_height;

    return true;
}

inline void RenderTexture(const std::string &key, const void *data, size_t data_size, const ImVec2 &img_size)
{
    ID3D11ShaderResourceView *texture = nullptr;

    {
        std::lock_guard<std::mutex> lock(textureCacheMutex);

        // 检查缓存
        auto it = textureCache.find(key);
        if (it != textureCache.end())
        {
            texture = it->second;
        }
        else if (data && data_size > 0)
        {
            int width, height;
            if (LoadTextureFromMemory(data, data_size, &texture, &width, &height))
            {
                if (textureCache.size() >= MAX_TEXTURE_CACHE_SIZE)
                {
                    auto oldest = textureCache.begin();
                    if (oldest->second)
                        oldest->second->Release();
                    textureCache.erase(oldest);
                }
                textureCache[key] = texture;
            }
            else
            {
                fprintf(stderr, "Failed to load texture from memory for key: %s\n",
                    key.c_str());
            }
        }
    }

    if (texture)
    {
        if (img_size.x <= 0 || img_size.y <= 0)
        {
            fprintf(stderr, "Invalid image size: (%f, %f)\n", img_size.x, img_size.y);
            return;
        }

        ImGui::Image((ImTextureID)texture, img_size, ImVec2(0, 0), ImVec2(1, 1));
    }
    else
    {
        ImGui::Text("Loading...");
    }
}

inline void ReleaseUnusedTextures_GUI()
{
    std::lock_guard<std::mutex> lock(textureCacheMutex);
    for (auto &[key, texture] : textureCache)
    {
        if (texture)
        {
            texture->Release();
        }
    }
    textureCache.clear();
}
