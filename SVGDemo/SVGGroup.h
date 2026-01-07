#pragma once
#include "stdafx.h"
#include "SVGElement.h"
#include <vector>
#include <string>
#include <gdiplus.h>

using namespace Gdiplus;
using namespace std;
using namespace rapidxml;

class SVGParser;

class SVGGroup : public SVGElement
{
private:
    vector<SVGElement*> children;
public:
    SVGGroup() {};
    ~SVGGroup();
    void Parse(xml_node<>* node) override;
    void Draw(Graphics& g) override;
    RectF GetBoundingBox() override;
    GraphicsPath* GetGraphicsPath();
};