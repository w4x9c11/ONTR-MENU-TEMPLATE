#pragma once
#include "../ImGui-UI/imgui.h"
#include "../ImGui-UI/imgui_internal.h"
#include <algorithm>

class SlideAnimation {
private:
  bool isAnimating = false;
  float progress = 0.0f;
  float lastTime = 0.0f;

  static constexpr float DURATION = 1.2f;
  static constexpr float SCALE_MIN = 0.1f;
  static constexpr float BAR_HEIGHT = 3.0f;

public:
  static constexpr float BAR_Y_OFFSET = 40.0f;

  static SlideAnimation &Get() {
    static SlideAnimation instance;
    return instance;
  }

  void StartAnimation() {
    if (!isAnimating) {
      isAnimating = true;
      progress = 0.0f;
      lastTime = static_cast<float>(ImGui::GetTime());
    }
  }

  void RenderProgressBar() const {
    if (!isAnimating)
      return;

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetWindowPos();
    draw_list->AddRectFilled(ImVec2(pos.x, pos.y + BAR_Y_OFFSET),
                             ImVec2(pos.x + ImGui::GetWindowSize().x * progress,
                                    pos.y + BAR_Y_OFFSET + BAR_HEIGHT),
                             IM_COL32(0, 255, 102, 255));
  }

  void Update() {
    if (!isAnimating)
      return;

    float currentTime = static_cast<float>(ImGui::GetTime());
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    progress += deltaTime / DURATION;
    if (progress >= 1.0f) {
      progress = 1.0f;
      isAnimating = false;
    }
  }

  bool IsAnimating() const { return isAnimating; }

  void GetAnimationValues(float &scale, float &alpha) const {
    if (!isAnimating) {
      scale = alpha = 1.0f;
      return;
    }
    float easeProgress = 1.0f - powf(1.0f - progress, 2.0f);
    scale = SCALE_MIN + (1.0f - SCALE_MIN) * easeProgress;
    alpha = easeProgress;
  }

private:
  SlideAnimation() = default;
  SlideAnimation(const SlideAnimation &) = delete;
  void operator=(const SlideAnimation &) = delete;
};
