#pragma once
#include "SVGElement.h" 
#include "SVGLinearGradient.h" // Để lấy struct SVGGradientStop
#include <vector>
#include <string>
#include "rapidxml.hpp"
#include <gdiplus.h>

using namespace Gdiplus;

// Class này cần public các hàm GetStops để LinearGradient có thể kế thừa
class SVGRadialGradient
{
private:
    std::string id;
    float cx = 0.5f, cy = 0.5f, r = 0.5f;
    float fx = 0.5f, fy = 0.5f;
    std::vector<SVGGradientStop> stops;
    bool userSpace = false;

    // Matrix biến đổi (Quan trọng cho Instagram)
    Matrix transform;

public:
    SVGRadialGradient() { transform.Reset(); }

    std::string GetId() const { return id; }
    void Parse(rapidxml::xml_node<>* node, SVGDocument* doc);

    PathGradientBrush* CreateBrush(const RectF& bounds) const;

    // --- QUAN TRỌNG: Hàm này phải có để LinearGradient gọi ---
    const std::vector<SVGGradientStop>& GetStops() const { return stops; }
};