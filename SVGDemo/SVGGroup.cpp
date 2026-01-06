#include "stdafx.h"
#include "SVGParser.h"
#include "SVGGroup.h"

SVGGroup::~SVGGroup() 
{
    for (auto child : children) delete child;
    children.clear();
}

void SVGGroup::Parse(xml_node<>* node) 
{
    SVGElement::Parse(node);
    for (auto* child = node->first_node(); child; child = child->next_sibling()) 
    {
        SVGElement* element = SVGParser::CreateElement(child);
        if (!element) continue;
        element->SetDocument(this->document);
        element->InheritFrom(*this);
        element->Parse(child);
        children.push_back(element);
    }
}

void SVGGroup::Draw(Graphics& g) 
{
    auto state = g.Save();
    ApplyClip(g);
    g.MultiplyTransform(&transform);
    for (auto* c : children) c->Draw(g);
    g.Restore(state);
}

RectF SVGGroup::GetBoundingBox() 
{
    RectF totalRect(0, 0, 0, 0);
    bool first = true;
    for (auto* c : children) 
    {
        RectF r = c->GetBoundingBox();
        if (r.Width <= 0 || r.Height <= 0) continue;
        if (first) { totalRect = r; first = false; }
        else { totalRect.Union(totalRect, totalRect, r); }
    }
    return totalRect;
}

GraphicsPath* SVGGroup::GetGraphicsPath() 
{
    GraphicsPath* groupPath = new GraphicsPath();
    for (auto* c : children) 
    {
        GraphicsPath* childPath = c->GetGraphicsPath();
        if (childPath) {
            groupPath->AddPath(childPath, FALSE);
            delete childPath;
        }
        if (!c->href.empty()) 
        {
            SVGElement* target = document->GetElementById(c->href);
            if (target) {
                GraphicsPath* targetPath = target->GetGraphicsPath();
                if (targetPath) 
                {
                    groupPath->AddPath(targetPath, FALSE);
                    delete targetPath;
                }
            }
        }
    }
    if (!transform.IsIdentity()) groupPath->Transform(&transform);
    return groupPath;
}