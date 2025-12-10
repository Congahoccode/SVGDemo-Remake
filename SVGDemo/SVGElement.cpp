#include "stdafx.h"
#include "SVGElement.h"

void SVGElement::Parse(xml_node<>* node) 
{

    // ===== parse fill =====
    if (auto attr = node->first_attribute("fill")) 
    {
        string s = attr->value();

        if (s == "none") fillColor = Color(0,0,0,0);
        else if (s.rfind("rgb(", 0) == 0) {
            int r,g,b;
            sscanf_s(s.c_str(), "rgb(%d,%d,%d)", &r, &g, &b);
            fillColor = Color((BYTE)(fillOpacity * 255), r, g, b);
        }
        else if (s[0] == '#') 
        {
            int r,g,b;
            sscanf_s(s.c_str()+1, "%02x%02x%02x", &r, &g, &b);
            fillColor = Color((BYTE)(fillOpacity * 255), r, g, b);
        }
    }

    // ===== fill-opacity =====
    if (auto attr = node->first_attribute("fill-opacity")) 
    {
        fillOpacity = atof(attr->value());
        fillColor = Color(
            (BYTE)(fillOpacity * 255),
            fillColor.GetR(), fillColor.GetG(), fillColor.GetB()
        );
    }

    // ===== stroke =====
    if (auto attr = node->first_attribute("stroke")) 
    {
        string s = attr->value();

        if (s == "none") strokeColor = Color(0,0,0,0);
        else if (s.rfind("rgb(", 0) == 0) 
        {
            int r,g,b;
            sscanf_s(s.c_str(), "rgb(%d,%d,%d)", &r, &g, &b);
            strokeColor = Color((BYTE)(strokeOpacity * 255), r, g, b);
        }
        else if (s[0] == '#') 
        {
            int r,g,b;
            sscanf_s(s.c_str()+1, "%02x%02x%02x", &r, &g, &b);
            strokeColor = Color((BYTE)(strokeOpacity * 255), r, g, b);
        }
    }

    // ===== stroke-opacity =====
    if (auto attr = node->first_attribute("stroke-opacity")) 
    {
        strokeOpacity = atof(attr->value());
        strokeColor = Color(
            (BYTE)(strokeOpacity * 255),
            strokeColor.GetR(), strokeColor.GetG(), strokeColor.GetB()
        );
    }

    // ===== stroke-width =====
    if (auto attr = node->first_attribute("stroke-width")) 
    {
        strokeWidth = atof(attr->value());
    }

	// ===== transform =====
	// Note: This is a simplified parser for transform attribute
	if (auto attr = node->first_attribute("transform"))
	{
		string s = attr->value();
		if (s.rfind("translate(", 0) == 0)
		{
			float tx, ty = 0;
			sscanf_s(s.c_str(), "translate(%f,%f)", &tx, &ty);
			transform.Translate(tx, ty);
		}
		else if (s.rfind("scale(", 0) == 0)
		{
			float sx, sy;
			sscanf_s(s.c_str(), "scale(%f,%f)", &sx, &sy);
			transform.Scale(sx, sy);
		}
		else if (s.rfind("rotate(", 0) == 0)
		{
			float angle;
			sscanf_s(s.c_str(), "rotate(%f)", &angle);
			transform.Rotate(angle);
		}
	}
}

void SVGElement::InheritFrom(const SVGElement& parent) {
    fillColor = parent.fillColor;
	fillOpacity = parent.fillOpacity;
    strokeColor = parent.strokeColor;
	strokeOpacity = parent.strokeOpacity;
    strokeWidth = parent.strokeWidth;
}
