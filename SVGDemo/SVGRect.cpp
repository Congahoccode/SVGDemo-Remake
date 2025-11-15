#include "stdafx.h"
#include "SVGRect.h"
#include <string>

void SVGRect::Parse(rapidxml::xml_node<>* node) {
    SVGElement::Parse(node);
    for (auto* a = node->first_attribute(); a; a = a->next_attribute()) {
        std::string n = a->name();
        std::string v = a->value();
        if (n == "x") x = std::stof(v);
        if (n == "y") y = std::stof(v);
        if (n == "width") width = std::stof(v);
        if (n == "height") height = std::stof(v);
    }
}

void SVGRect::Draw(Gdiplus::Graphics& g) {
    Gdiplus::SolidBrush brush(fillColor);
    Gdiplus::Pen pen(strokeColor, strokeWidth);
    g.FillRectangle(&brush, x, y, width, height);
    g.DrawRectangle(&pen, x, y, width, height);
}
