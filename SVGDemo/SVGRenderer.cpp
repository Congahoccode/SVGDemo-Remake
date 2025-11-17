#include "stdafx.h"
#include <windows.h>      // Must be included before GDI+ headers
#include <gdiplus.h>      // GDI+ main header
#include <vector>
#include "SVGRenderer.h"  // Ensure this is after system headers

using namespace Gdiplus;
using namespace std;

void SVGRenderer::Render(Graphics& g, const vector<SVGElement*>& elements)
{
    // Clear previous drawing, fill background
    g.Clear(backgroundColor);

    /*
    // Save current graphics state
    GraphicsState state = g.Save();

    // Apply renderer-wide transforms (scale, offset)
    Matrix transform;
    transform.Scale(scale, scale);
    transform.Translate(offset.X, offset.Y, MatrixOrderAppend);
    g.MultiplyTransform(&transform);
    */

    g.TranslateTransform(offset.X, offset.Y);
    g.ScaleTransform(zoomFactor, zoomFactor);


    // --- Draw each SVG element ---
    for (auto e : elements)
    {
        if (e)
        {
            // Each element applies its own transform in Draw
            e->Draw(g);
        }
    }

    // Restore original graphics state
    //g.Restore(state);
}

void SVGRenderer::Zoom(float factor)
{
    zoomFactor *= factor;
    // Giới hạn zoom để tránh mất hình
    if (zoomFactor < 0.1f) zoomFactor = 0.1f;
    if (zoomFactor > 10.0f) zoomFactor = 10.0f;
}

void SVGRenderer::Pan(float dx, float dy)
{
    offset.X += dx;
    offset.Y += dy;
}