#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include <gdiplus.h>
#include <map>
#pragma warning(push)
#pragma warning(disable : 26495) 
#include "rapidxml.hpp"
#pragma warning(pop)

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
    float strokeMiterLimit;
    bool strokeColorSet;
    bool strokeWidthSet;
    Matrix transform;
    std::vector<float> strokeDashArray;
    SVGDocument* document;
    SVGLinearGradient* fillGradient;
    SVGRadialGradient* fillRadialGradient;
    FillType fillType = FillType::Unset;
    LineCap strokeLineCap;
    LineJoin strokeLineJoin;
    std::string id;
    std::string clipPathId;

public:
    std::string href;

protected:
    Gdiplus::FillMode fillRule = Gdiplus::FillModeWinding;
    Brush* CreateFillBrush(const RectF& bounds);
    Pen* CreateStrokePen();
    void ApplyClip(Graphics& g);
    virtual RectF GetBounds() { return RectF(0, 0, 0, 0); }

public:
    SVGElement();
    virtual ~SVGElement() {}
    void SetDocument(SVGDocument* doc) { document = doc; }

    // Getter ID
    std::string GetId() const { return id; }

    virtual void InheritFrom(const SVGElement& parent);
    virtual void Parse(rapidxml::xml_node<>* node);
    virtual void Draw(Gdiplus::Graphics& g) = 0;
    virtual Gdiplus::RectF GetBoundingBox() = 0;
    virtual GraphicsPath* GetGraphicsPath() { return nullptr; }

    void ParseFillValue(const std::string& value);
    void ParseStrokeValue(const std::string& value);
    void ParseTransform(const std::string& value);
};