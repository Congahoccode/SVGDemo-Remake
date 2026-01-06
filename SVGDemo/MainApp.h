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
    SVGParser* parser = nullptr;
    SVGRenderer renderer;
    bool needsAutoFit = false;

public:
    MainApp();
    ~MainApp();

    bool LoadSVG(const std::string& filePath);
    void Render(Gdiplus::Graphics& g);
    void Clear();
    SVGRenderer& GetRenderer() { return renderer; }
    const std::vector<SVGElement*>& GetElements() const {
        if (parser) return parser->GetElements();
        static std::vector<SVGElement*> empty;
        return empty;
    }
};