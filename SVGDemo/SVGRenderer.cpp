#include "stdafx.h"
#include <windows.h>
#include <gdiplus.h>
#include <vector>
#include <algorithm>
#include "SVGRenderer.h"

using namespace Gdiplus;
using namespace std;

void SVGRenderer::Render(Graphics& g, const vector<SVGElement*>& elements)
{
    // Clear previous drawing, fill background
    g.Clear(backgroundColor);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.TranslateTransform(offset.X, offset.Y);
    g.ScaleTransform(zoomFactor, zoomFactor);
    g.RotateTransform(rotationAngle);


    // Draw each SVG element
    for (auto e : elements)
    {
        if (e)
        {
            e->Draw(g);
        }
    }
}

void SVGRenderer::Zoom(float factor)
{
    zoomFactor *= factor;
    if (zoomFactor < 0.001f) zoomFactor = 0.001f;
    if (zoomFactor > 10000.0f) zoomFactor = 10000.0f;
}

void SVGRenderer::Pan(float dx, float dy)
{
    offset.X += dx;
    offset.Y += dy;
}

void SVGRenderer::Rotate(float degrees)
{
    rotationAngle += degrees;

    // Giữ góc trong khoảng 0–360 cho gọn
    if (rotationAngle >= 360.0f) rotationAngle -= 360.0f;
    if (rotationAngle < 0.0f) rotationAngle += 360.0f;
}

void SVGRenderer::AutoFit(int screenWidth, int screenHeight, const vector<SVGElement*>& elements)
{
    if (elements.empty()) return;

    // 1. Tính tổng khung bao của toàn bộ bản vẽ
    RectF totalRect(0, 0, 0, 0);
    bool first = true;

    for (auto* e : elements)
    {
        RectF r = e->GetBoundingBox();

        // Bỏ qua phần tử không có kích thước thực (như defs rỗng)
        if (r.Width <= 0 || r.Height <= 0) continue;

        if (first) { totalRect = r; first = false; }
        else { totalRect.Union(totalRect, totalRect, r); }
    }

    // Nếu không có gì để vẽ
    if (totalRect.Width <= 0 || totalRect.Height <= 0) return;

    // 2. Tính tỉ lệ Zoom cần thiết
    float padding = 40.0f;
    float availableW = (float)screenWidth - padding;
    float availableH = (float)screenHeight - padding;

    if (availableW < 1) availableW = 1;
    if (availableH < 1) availableH = 1;

    float scaleX = availableW / totalRect.Width;
    float scaleY = availableH / totalRect.Height;

    zoomFactor = (std::min)(scaleX, scaleY);

    float midX = totalRect.X + totalRect.Width / 2.0f;
    float midY = totalRect.Y + totalRect.Height / 2.0f;

    offset.X = (screenWidth / 2.0f) - (midX * zoomFactor);
    offset.Y = (screenHeight / 2.0f) - (midY * zoomFactor);

    rotationAngle = 0.0f;
}