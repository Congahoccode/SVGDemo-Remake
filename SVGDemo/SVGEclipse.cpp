#include "stdafx.h"
#include "SVGEclipse.h" 
#include "SVGHelper.h"

void SVGEclipse::Parse(rapidxml::xml_node<>* node)
{
    SVGElement::Parse(node);

    if (auto attr = node->first_attribute("cx")) cx = ParseUnit(attr->value());
    if (auto attr = node->first_attribute("cy")) cy = ParseUnit(attr->value());

    if (auto attr = node->first_attribute("rx")) rx = ParseUnit(attr->value());
    if (auto attr = node->first_attribute("ry")) ry = ParseUnit(attr->value());

    if (rx > 0 && ry == 0) ry = rx;
    if (ry > 0 && rx == 0) rx = ry;
}

void SVGEclipse::Draw(Gdiplus::Graphics& g)
{
    if (rx <= 0 || ry <= 0) return;
    g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);

    Gdiplus::GraphicsState state = g.Save();
    ApplyClip(g);

    g.MultiplyTransform(&transform);

    Gdiplus::RectF bounds(cx - rx, cy - ry, 2 * rx, 2 * ry);

    if (auto* brush = CreateFillBrush(bounds)) {
        g.FillEllipse(brush, bounds);
        delete brush;
    }

    if (auto* pen = CreateStrokePen()) {
        g.DrawEllipse(pen, bounds);
        delete pen;
    }

    g.Restore(state);
}

Gdiplus::RectF SVGEclipse::GetBoundingBox()
{
    if (rx <= 0 || ry <= 0) return Gdiplus::RectF(0, 0, 0, 0);
    return Gdiplus::RectF(cx - rx, cy - ry, 2 * rx, 2 * ry);
}

Gdiplus::GraphicsPath* SVGEclipse::GetGraphicsPath()
{
    if (rx <= 0 || ry <= 0) return nullptr;

    auto* path = new Gdiplus::GraphicsPath();
    path->AddEllipse(cx - rx, cy - ry, 2 * rx, 2 * ry);

    if (!transform.IsIdentity()) {
        path->Transform(&transform);
    }
    return path;
}