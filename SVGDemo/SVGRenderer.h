#pragma once
#include <gdiplus.h>
#include <vector>
#include "SVGElement.h"

using namespace Gdiplus;
using namespace std;

class SVGRenderer
{
private:
    Color backgroundColor;     // Màu nền
    PointF offset;             // Pan (Dịch chuyển)
    float zoomFactor = 1.0f;   // Zoom (Phóng to)
    float rotationAngle = 0.0f;// Xoay

public:
    SVGRenderer()
        : backgroundColor(Gdiplus::Color(255, 255, 255)),
        offset(0, 0)
    {
    }

    void SetBackgroundColor(const Color& color) { backgroundColor = color; }
    void SetOffset(const PointF& p) { offset = p; }

    void Render(Graphics& g, const vector<SVGElement*>& elements);

    void Zoom(float factor);
    void Pan(float dx, float dy);
    void Rotate(float degrees);
    void AutoFit(int screenWidth, int screenHeight, const std::vector<SVGElement*>& elements);

    void ResetTransform()
    {
        offset = PointF(0, 0);
        zoomFactor = 1.0f;
        rotationAngle = 0.0f;
    }
};