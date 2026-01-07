#include "stdafx.h"
#include "SVGCircle.h"
#include "SVGHelper.h"

using namespace Gdiplus;
using namespace rapidxml;

void SVGCircle::Parse(xml_node<>* node)
{
    SVGElement::Parse(node);
    if (auto attr = node->first_attribute("cx")) cx = ParseUnit(attr->value());
    if (auto attr = node->first_attribute("cy")) cy = ParseUnit(attr->value());
    if (auto attr = node->first_attribute("r"))  r = ParseUnit(attr->value());
}

void SVGCircle::Draw(Graphics& g)
{
    if (r <= 0) return;
    g.SetSmoothingMode(SmoothingModeHighQuality);
    g.SetPixelOffsetMode(PixelOffsetModeHighQuality);
    GraphicsState state = g.Save();
    ApplyClip(g);
    g.MultiplyTransform(&transform);
    RectF bounds(cx - r, cy - r, 2 * r, 2 * r);
    if (auto* brush = CreateFillBrush(bounds)) 
    {
        g.FillEllipse(brush, bounds);
        delete brush;
    }
    if (auto* pen = CreateStrokePen()) 
    {
        g.DrawEllipse(pen, bounds);
        delete pen;
    }
    g.Restore(state);
}

RectF SVGCircle::GetBoundingBox()
{
    return RectF(cx - r, cy - r, 2 * r, 2 * r);
}