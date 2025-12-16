#pragma once

#include <vector>
#include <string>

#include "SVGParser.h"
#include "SVGRenderer.h"
#include "SVGElement.h"

namespace Gdiplus
{
    class Graphics;
}

class MainApp
{
private:
    SVGParser parser;
    SVGRenderer renderer;
    bool needsAutoFit = false;

public:
    MainApp() = default;
    ~MainApp();

    bool LoadSVG(const std::string& filePath);
    void Render(Gdiplus::Graphics& g);
    void Clear();

    SVGRenderer& GetRenderer() { return renderer; }
    const std::vector<SVGElement*>& GetElements() const { return parser.GetElements(); }
};