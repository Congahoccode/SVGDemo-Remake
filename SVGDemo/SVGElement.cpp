#include "stdafx.h"
#include "SVGElement.h"

void SVGElement::Parse(xml_node<>* node) {

    // --- fill ---
    if (auto attr = node->first_attribute("fill")) {
        string s = attr->value();

        if (s == "none") {
            fillColor = Color(0, 0, 0, 0);
        }
        else if (s.rfind("rgb(", 0) == 0) {     // parse rgb(r,g,b)
            int r, g, b;
            sscanf_s(s.c_str(), "rgb(%d,%d,%d)", &r, &g, &b);
            fillColor = Color((BYTE)(fillOpacity * 255), r, g, b);
        }
        else if (s[0] == '#') {                // parse #RRGGBB
            int r, g, b;
            sscanf_s(s.c_str() + 1, "%02x%02x%02x", &r, &g, &b);
            fillColor = Color((BYTE)(fillOpacity * 255), r, g, b);
        }
    }

    // --- fill-opacity ---
    if (auto attr = node->first_attribute("fill-opacity")) {
        fillOpacity = atof(attr->value());

        // cập nhật alpha của fillColor
        fillColor = Color((BYTE)(fillOpacity * 255),
            fillColor.GetR(),
            fillColor.GetG(),
            fillColor.GetB());
    }

    // --- stroke ---
    if (auto attr = node->first_attribute("stroke")) {
        string s = attr->value();

        if (s == "none") {
            strokeColor = Color(0, 0, 0, 0);
        }
        else if (s.rfind("rgb(", 0) == 0) {
            int r, g, b;
            sscanf_s(s.c_str(), "rgb(%d,%d,%d)", &r, &g, &b);
            strokeColor = Color((BYTE)(strokeOpacity * 255), r, g, b);
        }
        else if (s[0] == '#') {
            int r, g, b;
            sscanf_s(s.c_str() + 1, "%02x%02x%02x", &r, &g, &b);
            strokeColor = Color((BYTE)(strokeOpacity * 255), r, g, b);
        }
    }

    // --- stroke-opacity ---
    if (auto attr = node->first_attribute("stroke-opacity")) {
        strokeOpacity = atof(attr->value());
        strokeColor = Color((BYTE)(strokeOpacity * 255),
            strokeColor.GetR(),
            strokeColor.GetG(),
            strokeColor.GetB());
    }

    // --- stroke-width ---
    if (auto attr = node->first_attribute("stroke-width")) {
        strokeWidth = atof(attr->value());
    }
}
