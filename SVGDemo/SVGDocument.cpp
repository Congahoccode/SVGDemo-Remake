#include "stdafx.h"
#include "SVGDocument.h"
#include "SVGLinearGradient.h"
#include "SVGRadialGradient.h"
#include "SVGElement.h"

SVGDocument::~SVGDocument()
{
    Clear();
}

void SVGDocument::AddLinearGradient(SVGLinearGradient* g)
{
    if (!g) return;
    if (g->GetId().empty()) { delete g; return; }
    if (linearGradients.count(g->GetId())) delete linearGradients[g->GetId()];
    linearGradients[g->GetId()] = g;
}

SVGLinearGradient* SVGDocument::GetLinearGradient(const std::string& id)
{
    auto it = linearGradients.find(id);
    return (it != linearGradients.end()) ? it->second : nullptr;
}

void SVGDocument::AddRadialGradient(SVGRadialGradient* gradient)
{
    if (!gradient) return;
    if (gradient->GetId().empty()) { delete gradient; return; }
    if (radialGradients.count(gradient->GetId())) delete radialGradients[gradient->GetId()];
    radialGradients[gradient->GetId()] = gradient;
}

SVGRadialGradient* SVGDocument::GetRadialGradient(const std::string& id)
{
    auto it = radialGradients.find(id);
    return (it != radialGradients.end()) ? it->second : nullptr;
}

void SVGDocument::RegisterElement(const std::string& id, SVGElement* element)
{
    if (id.empty() || !element) return;
    elementMap[id] = element;
}

SVGElement* SVGDocument::GetElementById(const std::string& id)
{
    auto it = elementMap.find(id);
    return (it != elementMap.end()) ? it->second : nullptr;
}

void SVGDocument::Clear()
{
    for (auto& p : linearGradients) delete p.second;
    linearGradients.clear();

    for (auto& p : radialGradients) delete p.second;
    radialGradients.clear();
    elementMap.clear();
    width = 0; height = 0;
    viewX = 0; viewY = 0; viewW = 0; viewH = 0;
    hasViewBox = false;
}