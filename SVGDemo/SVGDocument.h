#pragma once
#include <map>
#include <string>
#include <gdiplus.h>

class SVGLinearGradient;
class SVGRadialGradient;
class SVGElement;

class SVGDocument
{
private:
    std::map<std::string, SVGLinearGradient*> linearGradients;
    std::map<std::string, SVGRadialGradient*> radialGradients;
    std::map<std::string, SVGElement*> elementMap;

public:
    float viewX = 0, viewY = 0, viewW = 0, viewH = 0;
    bool hasViewBox = false;
    float width = 0, height = 0;

    ~SVGDocument();

    void AddLinearGradient(SVGLinearGradient* g);
    SVGLinearGradient* GetLinearGradient(const std::string& id);

    void AddRadialGradient(SVGRadialGradient* gradient);
    SVGRadialGradient* GetRadialGradient(const std::string& id);

    void RegisterElement(const std::string& id, SVGElement* element);
    SVGElement* GetElementById(const std::string& id);

    void Clear();
};