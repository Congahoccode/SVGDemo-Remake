#pragma once
#include "SVGElement.h"
#include <string>

class SVGText : public SVGElement {
private:
    float x, y;
    std::wstring content;
    std::wstring fontFamily;
    float fontSize;

public:
    SVGText() : x(0), y(0), fontFamily(L"Arial"), fontSize(16.0f) {}
    void Parse(rapidxml::xml_node<>* node) override;
    void Draw(Gdiplus::Graphics& g) override;
};
