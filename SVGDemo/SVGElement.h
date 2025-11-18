#pragma once
#include <string>
#include <windows.h>
#include <gdiplus.h>
#include "rapidxml.hpp"

using namespace std;
using namespace rapidxml;
using namespace Gdiplus;

class SVGElement 
{
protected:
    Color fillColor;
    float fillOpacity;
    Color strokeColor;
    float strokeOpacity;
    float strokeWidth;

public:
    SVGElement()
        : fillColor(Color(0, 0, 0, 0)),
        fillOpacity(1.0f),
        strokeColor(Color(0, 0, 0, 0)),
        strokeOpacity(1.0f),
        strokeWidth(1.0f) 
    {
    }

    virtual ~SVGElement() {}

    virtual void Parse(xml_node<>* node);
    virtual void Draw(Graphics& graphics) = 0;
};