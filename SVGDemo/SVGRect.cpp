#include "stdafx.h"
#include "SVGRect.h"
#include "SVGHelper.h"
#include <string>

using namespace Gdiplus;
using namespace rapidxml;

void SVGRect::Parse(xml_node<>* node)
{
    SVGElement::Parse(node);
    if (auto a = node->first_attribute("x")) x = ParseUnit(a->value());
    if (auto a = node->first_attribute("y")) y = ParseUnit(a->value());
    if (auto a = node->first_attribute("width")) width = ParseUnit(a->value());
    if (auto a = node->first_attribute("height")) height = ParseUnit(a->value());
}

void SVGRect::Draw(Graphics& g)
{
    auto state = g.Save();
    ApplyClip(g);

    g.MultiplyTransform(&transform);

    RectF rect(x, y, width, height);

    if (Brush* brush = CreateFillBrush(rect)) 
    {
        g.FillRectangle(brush, rect);
        delete brush;
    }

    if (Pen* pen = CreateStrokePen()) 
    {
        g.DrawRectangle(pen, rect);
        delete pen;
    }

    g.Restore(state);
}

RectF SVGRect::GetBoundingBox()
{
    return RectF(x, y, width, height);
}

GraphicsPath* SVGRect::GetGraphicsPath() 
{
    GraphicsPath* p = new GraphicsPath();
    p->AddRectangle(RectF(x, y, width, height));
    p->Transform(&transform);
    return p;
}