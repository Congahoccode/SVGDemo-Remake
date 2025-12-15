#include "stdafx.h"
#include "SVGCircle.h"
#include <string>
#include <gdiplus.h>
#include "rapidxml.hpp" 

using namespace std;
using namespace Gdiplus;
using namespace rapidxml;

void SVGCircle::Parse(xml_node<>* node) 
{
    SVGElement::Parse(node);
    for (auto* a = node->first_attribute(); a; a = a->next_attribute()) 
    {
        string n = a->name();
        string v = a->value();
        if (n == "cx") cx = stof(v);
        else if (n == "cy") cy = stof(v);
        else if (n == "r") r = stof(v);
    }
}

void SVGCircle::Draw(Graphics& g)
{
    auto state = g.Save();
    g.MultiplyTransform(&transform);

    RectF bounds(cx - r, cy - r, 2 * r, 2 * r);

    // ===== FILL =====
    if (Brush* brush = CreateFillBrush(bounds))
    {
        g.FillEllipse(brush, bounds);
        delete brush;
    }

    // ===== STROKE =====
    if (Pen* pen = CreateStrokePen())
    {
        g.DrawEllipse(pen, bounds);
        delete pen;
    }

    g.Restore(state);
}
