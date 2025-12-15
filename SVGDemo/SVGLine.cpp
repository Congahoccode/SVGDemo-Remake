#include "stdafx.h"
#include "SVGLine.h"
#include <string>

using namespace std;
using namespace Gdiplus;
using namespace rapidxml;

void SVGLine::Parse(xml_node<>* node) 
{
    SVGElement::Parse(node);
    for (auto* a = node->first_attribute(); a; a = a->next_attribute()) 
    {
        string n = a->name();
        string v = a->value();
        if (n == "x1") x1 = stof(v);
        else if (n == "y1") y1 = stof(v);
        else if (n == "x2") x2 = stof(v);
        else if (n == "y2") y2 = stof(v);
    }
}

void SVGLine::Draw(Gdiplus::Graphics& g)
{
    auto state = g.Save();
    g.MultiplyTransform(&transform);

    if (Pen* pen = CreateStrokePen())
    {
        g.DrawLine(pen, x1, y1, x2, y2);
        delete pen;
    }

    g.Restore(state);
}

