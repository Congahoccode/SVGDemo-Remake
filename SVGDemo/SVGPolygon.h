#pragma once
#include "SVGElement.h"
#include <vector>

class SVGPolygon : public SVGElement {
private:
    std::vector<Gdiplus::PointF> points;

public:
    void Parse(rapidxml::xml_node<>* node) override;
    void Draw(Gdiplus::Graphics& g) override;
};
