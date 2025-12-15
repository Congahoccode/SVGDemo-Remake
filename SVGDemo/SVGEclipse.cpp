#include "stdafx.h"
#include "SVGEclipse.h"
#include <string>

using namespace std;
using namespace Gdiplus;
using namespace rapidxml;

void SVGEllipse::Parse(xml_node<>* node) 
{
    SVGElement::Parse(node);
    for (auto* a = node->first_attribute(); a; a = a->next_attribute()) 
    {
        string n = a->name();
        string v = a->value();
        if (n == "cx") cx = stof(v);
        else if (n == "cy") cy = stof(v);
        else if (n == "rx") rx = stof(v);
        else if (n == "ry") ry = stof(v);
    }
}

void SVGEllipse::Draw(Gdiplus::Graphics& g)
{
    auto state = g.Save();
    g.MultiplyTransform(&transform);

    RectF bounds(cx - rx, cy - ry, 2 * rx, 2 * ry);

    if (Brush* brush = CreateFillBrush(bounds))
    {
        g.FillEllipse(brush, bounds);
        delete brush;
    }

    if (Pen* pen = CreateStrokePen())
    {
        g.DrawEllipse(pen, bounds);
        delete pen;
    }

    g.Restore(state);
}

