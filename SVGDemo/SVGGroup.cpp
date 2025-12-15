#include "stdafx.h"
#include "SVGParser.h"
#include "SVGGroup.h"


SVGGroup::~SVGGroup()
{
    for (auto child : children)
        delete child;
    children.clear();
}

void SVGGroup::Parse(xml_node<>* node)
{
    // Parse thuộc tính của group (fill, stroke, transform)
    SVGElement::Parse(node);

    for (auto* child = node->first_node(); child; child = child->next_sibling())
    {
        SVGElement* element = SVGParser::CreateElement(child);
        if (!element) continue;

        // Truyền document
        element->SetDocument(this->document);

        // INHERIT TRƯỚC (fallback)
        element->InheritFrom(*this);

        // Parse thuộc tính riêng (override)
        element->Parse(child);

        children.push_back(element);
    }
}


void SVGGroup::Draw(Graphics& g)
{
    auto state = g.Save();

    // áp dụng transform của group
    g.MultiplyTransform(&transform);

    // vẽ các children
    for (auto* c : children) {
        c->Draw(g);
    }

    g.Restore(state);
}
