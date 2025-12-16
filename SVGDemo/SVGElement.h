#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include <gdiplus.h>
#pragma warning(push)
#pragma warning(disable : 26495) 
#include "rapidxml.hpp"
#pragma warning(pop)

using namespace std;
using namespace rapidxml;
using namespace Gdiplus;

enum class FillType {
    None,
    Unset,
    Solid,
    LinearGradient,
    RadialGradient
};

class SVGLinearGradient;
class SVGRadialGradient;
class SVGDocument;

class SVGElement
{
protected:
    Color fillColor;
    float fillOpacity;
    Color strokeColor;
    float strokeOpacity;
    float strokeWidth;
    Matrix transform;
    float strokeMiterLimit;

    std::vector<float> strokeDashArray;

    SVGDocument* document;
    SVGLinearGradient* fillGradient;
    SVGRadialGradient* fillRadialGradient;
    FillType fillType = FillType::Unset;
    LineCap strokeLineCap;
    LineJoin strokeLineJoin;

    Brush* CreateFillBrush(const RectF& bounds);
    Pen* CreateStrokePen();

    virtual RectF GetBounds() { return RectF(0, 0, 0, 0); }

public:
    SVGElement()
        : fillColor(Color(255, 0, 0, 0)),
        fillOpacity(1.0f),
        strokeColor(Color(0, 0, 0, 0)),
        strokeOpacity(1.0f),
        strokeWidth(1.0f),
        strokeMiterLimit(4.0f),
        strokeLineCap(LineCapFlat),
        strokeLineJoin(LineJoinMiter)
    {
        document = nullptr;
        fillGradient = nullptr;
        fillRadialGradient = nullptr;
        fillType = FillType::Unset;
        transform.Reset();
    }

    virtual ~SVGElement() {}

    void SetDocument(SVGDocument* doc) { document = doc; }

    virtual void InheritFrom(const SVGElement& parent); // Kế thừa node cha

    virtual void Parse(xml_node<>* node);

    virtual void Draw(Graphics& graphics) = 0;

    virtual RectF GetBoundingBox() { return RectF(0, 0, 0, 0); }
};