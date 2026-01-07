#include "stdafx.h"
#include "SVGEclipse.h" 
#include "SVGHelper.h"

using namespace Gdiplus;
using namespace rapidxml;

void SVGEclipse::Parse(xml_node<>* node)
{
    SVGElement::Parse(node);
    if (auto attr = node->first_attribute("cx")) cx = ParseUnit(attr->value());
    if (auto attr = node->first_attribute("cy")) cy = ParseUnit(attr->value());
    if (auto attr = node->first_attribute("rx")) rx = ParseUnit(attr->value());
    if (auto attr = node->first_attribute("ry")) ry = ParseUnit(attr->value());
    if (rx > 0 && ry == 0) ry = rx;
    if (ry > 0 && rx == 0) rx = ry;
}

void SVGEclipse::Draw(Graphics& g)
{
    if (rx <= 0 || ry <= 0) return;
    g.SetSmoothingMode(SmoothingModeHighQuality);
    g.SetPixelOffsetMode(PixelOffsetModeHighQuality);
    GraphicsState state = g.Save();
    ApplyClip(g);
    g.MultiplyTransform(&transform);
    RectF bounds(cx - rx, cy - ry, 2 * rx, 2 * ry);
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

RectF SVGEclipse::GetBoundingBox()
{
    if (rx <= 0 || ry <= 0) return Gdiplus::RectF(0, 0, 0, 0);
    return Gdiplus::RectF(cx - rx, cy - ry, 2 * rx, 2 * ry);
}

GraphicsPath* SVGEclipse::GetGraphicsPath()
{
    if (rx <= 0 || ry <= 0) return nullptr;

    auto* path = new Gdiplus::GraphicsPath();
    path->AddEllipse(cx - rx, cy - ry, 2 * rx, 2 * ry);

    if (!transform.IsIdentity()) path->Transform(&transform);
    return path;
}