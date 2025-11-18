#include "stdafx.h"
#include "SVGText.h"
#include <string>

using namespace std;
using namespace Gdiplus;
using namespace rapidxml;

void SVGText::Parse(xml_node<>* node) {
    SVGElement::Parse(node);
    for (auto* a = node->first_attribute(); a; a = a->next_attribute()) 
    {
        string n = a->name();
        string v = a->value();
        if (n == "x") x = stof(v);
        else if (n == "y") y = stof(v);
        else if (n == "font-size") fontSize = stof(v);
        else if (n == "font-family") fontFamily = wstring(v.begin(), v.end());
    }
    // Nội dung text nằm trong node->value()
    if (node->value())
        content = wstring(node->value(), node->value() + strlen(node->value()));
}

void SVGText::Draw(Graphics& g) 
{
    SolidBrush brush(fillColor);
    FontFamily family(fontFamily.c_str());
    Font font(&family, fontSize, FontStyleRegular, UnitPixel);

    // SVG dùng baseline, GDI+ dùng top-left
    REAL ascent = family.GetCellAscent(FontStyleRegular);
    REAL em = family.GetEmHeight(FontStyleRegular);
    REAL ascentPx = fontSize * ascent / em;

    // GDI+ muốn y_top = y_baseline - ascentPx
    REAL yTop = y - ascentPx;   

    g.DrawString(content.c_str(), -1, &font, PointF(x, yTop), &brush);
}

