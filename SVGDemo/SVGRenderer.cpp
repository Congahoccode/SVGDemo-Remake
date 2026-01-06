#include "stdafx.h"
#include <windows.h>
#include <gdiplus.h>
#include <vector>
#include <algorithm>
#include "SVGRenderer.h"
#include "SVGDocument.h" 

using namespace Gdiplus;
using namespace std;

void SVGRenderer::Render(Graphics& g, const vector<SVGElement*>& elements)
{
    g.Clear(backgroundColor);
    g.SetSmoothingMode(SmoothingModeHighQuality);
    g.SetPixelOffsetMode(PixelOffsetModeHighQuality);
    g.SetCompositingQuality(CompositingQualityHighQuality);
    g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
    Matrix matrix;

    matrix.Translate(offset.X, offset.Y);
    matrix.Scale(zoomFactor, zoomFactor);
    matrix.Rotate(rotationAngle);

    g.MultiplyTransform(&matrix);

    for (auto e : elements)
    {
        if (e) e->Draw(g);
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
}

void SVGRenderer::AutoFit(int screenWidth, int screenHeight, const std::vector<SVGElement*>& elements)
{
    if (elements.empty()) return;

    RectF totalRect(0, 0, 0, 0);
    bool first = true;

    for (auto e : elements)
    {
        if (!e) continue;
        RectF r = e->GetBoundingBox();
        if (r.Width <= 0 || r.Height <= 0 || r.Width > 1000000) continue;
        if (first) { totalRect = r; first = false; }
        else { totalRect.Union(totalRect, totalRect, r); }
    }
    if (totalRect.Width <= 0 || totalRect.Height <= 0) return;
    ResetTransform();
    float padding = 40.0f;
    float availW = (float)screenWidth - padding;
    float availH = (float)screenHeight - padding;
    if (availW < 1) availW = 1;
    if (availH < 1) availH = 1;
    // Tính tỷ lệ scale
    float scaleX = availW / totalRect.Width;
    float scaleY = availH / totalRect.Height;
    zoomFactor = (std::min)(scaleX, scaleY);
    // Tính offset
    float midX = totalRect.X + totalRect.Width / 2.0f;
    float midY = totalRect.Y + totalRect.Height / 2.0f;

    offset.X = (screenWidth / 2.0f) - (midX * zoomFactor);
    offset.Y = (screenHeight / 2.0f) - (midY * zoomFactor);
}

// Hàm Fit theo ViewBox
void SVGRenderer::FitViewBox(int screenWidth, int screenHeight, float vx, float vy, float vw, float vh)
{
    if (vw <= 0 || vh <= 0) return;

    ResetTransform();

    // 1. Tính toán tỷ lệ scale
    float padding = 20.0f;
    float availW = (float)screenWidth - padding * 2;
    float availH = (float)screenHeight - padding * 2;

    if (availW < 1) availW = 1;
    if (availH < 1) availH = 1;

    float scaleX = availW / vw;
    float scaleY = availH / vh;

    zoomFactor = (std::min)(scaleX, scaleY);

    // 2. Tính toán offset

    float screenCenterX = screenWidth / 2.0f;
    float screenCenterY = screenHeight / 2.0f;

    float vbCenterX = vx + vw / 2.0f;
    float vbCenterY = vy + vh / 2.0f;

    offset.X = screenCenterX - (vbCenterX * zoomFactor);
    offset.Y = screenCenterY - (vbCenterY * zoomFactor);
}