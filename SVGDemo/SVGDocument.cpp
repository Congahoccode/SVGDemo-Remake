#include "stdafx.h"
#include "SVGDocument.h"
#include "SVGLinearGradient.h"
#include "SVGRadialGradient.h"

SVGDocument::~SVGDocument()
{
    Clear();
}

void SVGDocument::AddLinearGradient(SVGLinearGradient* g)
{
    if (!g || g->GetId().empty()) return;
    linearGradients[g->GetId()] = g;
}

SVGLinearGradient* SVGDocument::GetLinearGradient(const std::string& id)
{
    auto it = linearGradients.find(id);
    return (it != linearGradients.end()) ? it->second : nullptr;
}

void SVGDocument::AddRadialGradient(SVGRadialGradient* gradient)
{
    if (gradient) {
        // Lưu gradient vào map với key là ID của nó
        radialGradients[gradient->GetId()] = gradient;
    }
}

SVGRadialGradient* SVGDocument::GetRadialGradient(const std::string& id)
{
    // Tìm kiếm trong map
    auto it = radialGradients.find(id);
    if (it != radialGradients.end()) {
        return it->second; // Trả về con trỏ nếu tìm thấy
    }
    return nullptr; // Trả về null nếu không tìm thấy
}


void SVGDocument::Clear()
{
    for (auto& p : linearGradients)
        delete p.second;
    linearGradients.clear();

    for (auto& pair : radialGradients) {
        delete pair.second;
    }
    radialGradients.clear();
}