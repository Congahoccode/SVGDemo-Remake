#pragma once
#include <gdiplus.h>
#include <vector>
#include "SVGElement.h"

class SVGRenderer
{
private:
    Gdiplus::Color backgroundColor;  // Màu nền vùng vẽ
    float scale;                     // Tỷ lệ phóng to / thu nhỏ
    Gdiplus::PointF offset;          // Vị trí tịnh tiến khung nhìn (pan)

public:
    // Constructor mặc định
    SVGRenderer()
        : backgroundColor(Gdiplus::Color(255, 255, 255)), // trắng
        scale(1.0f),
        offset(0, 0)
    {
    }

    // --- Thiết lập thuộc tính ---
    void SetBackgroundColor(const Gdiplus::Color& color) { backgroundColor = color; }
    void SetScale(float s) { scale = s; }
    void SetOffset(const Gdiplus::PointF& p) { offset = p; }

    // --- Phương thức chính ---
    void Render(Gdiplus::Graphics& g, const std::vector<SVGElement*>& elements);
};
