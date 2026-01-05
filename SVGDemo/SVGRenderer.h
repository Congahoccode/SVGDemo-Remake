#pragma once
#include <gdiplus.h>
#include <vector>
#include "SVGElement.h"

using namespace Gdiplus;
using namespace std;

class SVGRenderer
{
private:
    Color backgroundColor;
    float scale;
    PointF offset;
    float zoomFactor = 1.0f;
    float rotationAngle = 0.0f;

public:
    SVGRenderer() : backgroundColor(Color(255, 255, 255)),scale(1.0f), offset(0, 0){}

    void SetBackgroundColor(const Color& color) { backgroundColor = color; }
    void Render(Graphics& g, const vector<SVGElement*>& elements);

    void Zoom(float factor);
    void Pan(float dx, float dy);
    void Rotate(float degrees);

    void AutoFit(int screenWidth, int screenHeight, const std::vector<SVGElement*>& elements);
    void FitViewBox(int screenWidth, int screenHeight, float vx, float vy, float vw, float vh);

    void ResetTransform() {
        offset = PointF(0, 0);
        zoomFactor = 1.0f;
        rotationAngle = 0.0f;
    }
};