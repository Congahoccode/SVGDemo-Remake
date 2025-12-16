#pragma once
#include <map>
#include <string>

class SVGLinearGradient;
class SVGRadialGradient;

class SVGDocument
{
private:
    std::map<std::string, SVGLinearGradient*> linearGradients;
    std::map<std::string, SVGRadialGradient*> radialGradients;

public:
    ~SVGDocument();

    void AddLinearGradient(SVGLinearGradient* g);
    SVGLinearGradient* GetLinearGradient(const std::string& id);

    void AddRadialGradient(SVGRadialGradient* gradient);
    SVGRadialGradient* GetRadialGradient(const std::string& id);
    void Clear();
};
