#include "stdafx.h"
#include "SVGElement.h"

void SVGElement::Parse(xml_node<>* node) {
	if (auto attr = node->first_attribute("fill")) {
		string fillStr = attr->value();
		if (fillStr == "none") fillColor = Color(0, 0, 0, 0);
		else {
			int r = 0, g = 0, b = 0;
			if (sscanf_s(fillStr.c_str(), "#%02x%02x%02x", &r, &g, &b) == 3) fillColor = Color(255, r, g, b);
		}
	}
	if (auto attr = node->first_attribute("fill-opacity")) {
		fillOpacity = atof(attr->value());
	}
	if (auto attr = node->first_attribute("stroke-width")) {
		strokeWidth = stof(attr->value());
	}
	if (auto attr = node->first_attribute("stroke")) {
		string strokeStr = attr->value();
		if (strokeStr == "none") strokeColor = Color(0, 0, 0, 0);
		else {
			int r = 0, g = 0, b = 0;
			if (sscanf_s(strokeStr.c_str(), "#%02x%02x%02x", &r, &g, &b) == 3) strokeColor = Color(255, r, g, b);
		}
	}
	if (auto attr = node->first_attribute("stroke-opacity")) {
		strokeOpacity = atof(attr->value());
	}
}