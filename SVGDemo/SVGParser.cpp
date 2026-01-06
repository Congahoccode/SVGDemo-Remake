#include "stdafx.h"
#include "SVGParser.h"
#include "SVGLinearGradient.h"
#include "SVGRadialGradient.h"
#include "SVGHelper.h"
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

bool SVGParser::ParseFile(const std::string& filePath)
{
    // Dọn dẹp dữ liệu cũ (bao gồm cả doc.clear())
    Clear();

    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    // Đọc toàn bộ file vào buffer
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size == 0) return false;

    // Resize buffer để chứa dữ liệu mới
    buffer.resize(size + 1);
    file.read(&buffer[0], size);
    buffer[size] = '\0';

    try 
    {
        // Parse XML (Sử dụng buffer vừa đọc)
        doc.parse<rapidxml::parse_default>(&buffer[0]);
    }
    catch (...)
    {
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node("svg");
    if (!root) return false;

    // --- 1. Parse Header Info ---
    if (auto attr = root->first_attribute("width")) document.width = ParseUnit(attr->value());
    if (auto attr = root->first_attribute("height")) document.height = ParseUnit(attr->value());

    if (auto attr = root->first_attribute("viewBox")) {
        std::string vb = attr->value();
        std::vector<float> vals;
        ParseNumberList(vb.c_str(), vals);

        if (vals.size() >= 4) {
            document.viewX = vals[0];
            document.viewY = vals[1];
            document.viewW = vals[2];
            document.viewH = vals[3];
            document.hasViewBox = true;
        }
    }

    // --- 2. Parse Definitions (Gradient) ---
    // Tìm trong <defs>
    for (auto* node = root->first_node("defs"); node; node = node->next_sibling("defs")) 
    {
        for (auto* def = node->first_node(); def; def = def->next_sibling()) 
        {
            std::string name = def->name();
            if (name == "linearGradient") 
            {
                auto* grad = new SVGLinearGradient();
                grad->Parse(def, &document);
                document.AddLinearGradient(grad);
            }
            else if (name == "radialGradient") 
            {
                auto* grad = new SVGRadialGradient();
                grad->Parse(def, &document);
                document.AddRadialGradient(grad);
            }
        }
    }
    // Tìm Gradient nằm ngoài (trực tiếp trong svg)
    for (auto* node = root->first_node(); node; node = node->next_sibling()) 
    {
        std::string name = node->name();
        if (name == "linearGradient") 
        {
            auto* grad = new SVGLinearGradient();
            grad->Parse(node, &document);
            document.AddLinearGradient(grad);
        }
        else if (name == "radialGradient") 
        {
            auto* grad = new SVGRadialGradient();
            grad->Parse(node, &document);
            document.AddRadialGradient(grad);
        }
    }

    // --- 3. Parse Elements ---
    for (auto* node = root->first_node(); node; node = node->next_sibling())
    {
        string name = node->name();
        // Bỏ qua các thẻ metadata/defs
        if (name == "defs" || name == "linearGradient" || name == "radialGradient" ||
            name == "style" || name == "metadata" || name == "title" || name == "desc")
            continue;

        SVGElement* element = CreateElement(node);
        if (!element) continue;

        element->SetDocument(&document);
        element->Parse(node);
        elements.push_back(element);
    }

    return true;
}

SVGElement* SVGParser::CreateElement(rapidxml::xml_node<>* node)
{
    std::string name = node->name();
    if (name == "rect") return new SVGRect();
    if (name == "circle") return new SVGCircle();
    if (name == "ellipse") return new SVGEclipse();
    if (name == "line") return new SVGLine();
    if (name == "polygon") return new SVGPolygon();
    if (name == "polyline") return new SVGPolyline();
    if (name == "text") return new SVGText();
    if (name == "g") return new SVGGroup();
    if (name == "path") return new SVGPath();
    return nullptr;
}

void SVGParser::Clear()
{
    for (auto* e : elements) delete e;
    elements.clear();
    document.Clear();
    document.hasViewBox = false;
    document.width = 0; document.height = 0;
    doc.clear();
}