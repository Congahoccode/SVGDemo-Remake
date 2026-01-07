#pragma once
#include "rapidxml.hpp"
#include <string>
#include <vector>
#include <gdiplus.h>

class SVGDocument;
struct SVGGradientStop
{
    float offset = 0.0;     // [0..1]
    Color color;      // ARGB
};

class SVGLinearGradient
{
private:
    std::string id;
    float x1 = 0, y1 = 0;
    float x2 = 1, y2 = 0;
    bool userSpace = false;
    std::vector<SVGGradientStop> stops;
    Matrix transform;

public:
    SVGLinearGradient() { transform.Reset(); }

    void SetId(const std::string& _id) { id = _id; }
    const std::string& GetId() const { return id; }

    void Parse(rapidxml::xml_node<>* node, SVGDocument* doc);

    LinearGradientBrush* CreateBrush(const RectF& bounds) const;
    const std::vector<SVGGradientStop>& GetStops() const { return stops; }
};