#pragma once
#include <gdiplus.h>
#include <vector>
#include "SVGElement.h"

using namespace Gdiplus;
using namespace std;

class SVGRenderer
{
private:
    Color backgroundColor;  // Màu nền vùng vẽ
    float scale;                     // Tỷ lệ phóng to / thu nhỏ
    PointF offset;          // Vị trí tịnh tiến khung nhìn (pan)
    float zoomFactor = 1.0f;   // zoom tổng
public:
    // Constructor mặc định
    SVGRenderer()
        : backgroundColor(Gdiplus::Color(255, 255, 255)), // trắng
        scale(1.0f),
        offset(0, 0)
    {
    }

    // --- Thiết lập thuộc tính ---
    void SetBackgroundColor(const Color& color) { backgroundColor = color; }
    void SetScale(float s) { scale = s; }
    void SetOffset(const PointF& p) { offset = p; }

    // --- Phương thức chính ---
    void Render(Graphics& g, const vector<SVGElement*>& elements);

    // --- Xử lý tương tác ---
    void Zoom(float factor);
    void Pan(float dx, float dy);
};
