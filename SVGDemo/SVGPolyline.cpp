#include "stdafx.h"
#include "SVGPolyline.h"
#include <sstream>
#include <string>

using namespace std;
using namespace Gdiplus;
using namespace rapidxml;

void SVGPolyline::Parse(xml_node<>* node) 
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

void SVGPolyline::Draw(Graphics& g) 
{
    if (points.size() < 2) return;
    Pen pen(strokeColor, strokeWidth);
    SolidBrush brush(fillColor);
    g.DrawLines(&pen, points.data(), points.size());
    g.FillPolygon(&brush, points.data(), points.size());
}
