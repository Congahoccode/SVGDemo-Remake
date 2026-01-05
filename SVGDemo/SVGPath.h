#pragma once
#include "SVGElement.h"
#include <vector>
#include <sstream>

class SVGPath : public SVGElement
{
private:
    Gdiplus::GraphicsPath path;

public:
    SVGPath() {}
    void Parse(rapidxml::xml_node<>* node) override;
    void Draw(Gdiplus::Graphics& g) override;
    RectF GetBoundingBox() override;

    GraphicsPath* GetGraphicsPath() override { return path.Clone(); }
};