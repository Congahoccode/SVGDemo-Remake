#include "stdafx.h"
#include "SVGPolygon.h"
#include <sstream>
#include <string>

using namespace std;
using namespace Gdiplus;
using namespace rapidxml;

void SVGPolygon::Parse(xml_node<>* node) 
{
    SVGElement::Parse(node);
    for (auto* a = node->first_attribute(); a; a = a->next_attribute()) 
    {
        string n = a->name();
        if (n == "points") 
        {
            stringstream ss(a->value());
            float x, y;
            char comma;
            while (ss >> x) 
            {
                if (ss.peek() == ',') ss >> comma;
                if (ss >> y) points.push_back(PointF(x, y));
            }
        }
    }
}

void SVGPolygon::Draw(Graphics& g) 
{
    if (points.empty()) return;
    SolidBrush brush(fillColor);
    Pen pen(strokeColor, strokeWidth);
    g.FillPolygon(&brush, points.data(), points.size());
    g.DrawPolygon(&pen, points.data(), points.size());
}
