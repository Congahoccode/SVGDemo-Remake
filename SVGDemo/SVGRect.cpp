#include "stdafx.h"
#include "SVGRect.h"
#include <string>

void SVGRect::Parse(rapidxml::xml_node<>* node) 
{
    SVGElement::Parse(node);
    for (auto* a = node->first_attribute(); a; a = a->next_attribute()) 
    {
        std::string n = a->name();
        std::string v = a->value();
        if (n == "x") x = std::stof(v);
        if (n == "y") y = std::stof(v);
        if (n == "width") width = std::stof(v);
        if (n == "height") height = std::stof(v);
    }
}

void SVGRect::Draw(Gdiplus::Graphics& g)
{
    auto state = g.Save();
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

