#include "stdafx.h"
#include "SVGLinearGradient.h"
#include "SVGDocument.h"
#include "SVGRadialGradient.h"
#include "SVGHelper.h"
#include <vector>
#include <algorithm>

using namespace std;
using namespace Gdiplus;

void SVGLinearGradient::Parse(rapidxml::xml_node<>* node, SVGDocument* doc)
{
    if (auto attr = node->first_attribute("id")) id = attr->value();

    if (auto attr = node->first_attribute("x1")) x1 = ParseUnit(attr->value());
    if (auto attr = node->first_attribute("y1")) y1 = ParseUnit(attr->value());
    if (auto attr = node->first_attribute("x2")) x2 = ParseUnit(attr->value());
    if (auto attr = node->first_attribute("y2")) y2 = ParseUnit(attr->value());

    if (auto attr = node->first_attribute("gradientUnits"))
        userSpace = string(attr->value()) == "userSpaceOnUse";

    if (auto attr = node->first_attribute("xlink:href")) {
        string href = attr->value();
        if (!href.empty() && href[0] == '#' && doc) {
            string pid = href.substr(1);
            if (auto* lin = doc->GetLinearGradient(pid)) stops = lin->GetStops();
            else if (auto* rad = doc->GetRadialGradient(pid)) stops = rad->GetStops();
        }
    }

    if (auto attr = node->first_attribute("gradientTransform")) {
        ParseTransformString(attr->value(), transform);
    }

    if (node->first_node("stop")) stops.clear();
    for (auto* stop = node->first_node("stop"); stop; stop = stop->next_sibling("stop")) {
        SVGGradientStop gs; gs.offset = 0.0f;
        if (auto a = stop->first_attribute("offset")) gs.offset = ParseUnit(a->value());

        string cStr;
        if (auto c = stop->first_attribute("stop-color")) cStr = c->value();

        gs.color = ParseColor(cStr);
        if (auto op = stop->first_attribute("stop-opacity")) {
            const char* p = op->value();
            gs.color = Color((BYTE)(ParseNumber(p) * 255), gs.color.GetR(), gs.color.GetG(), gs.color.GetB());
        }
        stops.push_back(gs);
    }
}

LinearGradientBrush* SVGLinearGradient::CreateBrush(const RectF& bounds) const
{
    PointF p1(x1, y1);
    PointF p2(x2, y2);
    if (abs(p1.X - p2.X) < 0.0001f && abs(p1.Y - p2.Y) < 0.0001f) p2.X += 0.001f;

    auto* brush = new LinearGradientBrush(p1, p2, Color::Black, Color::White);

    Matrix m;
    // 1. Gradient Transform
    if (!transform.IsIdentity()) {
        m.Multiply(const_cast<Matrix*>(&transform), MatrixOrderAppend);
    }
    // 2. Map to Bounds
    if (!userSpace) {
        m.Translate(bounds.X, bounds.Y, MatrixOrderAppend);
        m.Scale(bounds.Width, bounds.Height, MatrixOrderAppend);
    }
    brush->MultiplyTransform(&m);

    if (stops.size() >= 2) {
        // Chuẩn hóa offset
        vector<SVGGradientStop> safeStops = stops;
        float maxOff = 0;
        for (auto& s : safeStops) { if (s.offset < maxOff) s.offset = maxOff; else maxOff = s.offset; }

        vector<Color> cols; vector<REAL> pos;
        for (const auto& s : safeStops) { cols.push_back(s.color); pos.push_back(s.offset); }

        if (pos.front() > 0.001f) {
            Color c = cols.front(); pos.insert(pos.begin(), 0.0f); cols.insert(cols.begin(), c);
        }
        else pos.front() = 0.0f;

        if (pos.back() < 0.999f) {
            Color c = cols.back(); pos.push_back(1.0f); cols.push_back(c);
        }
        else pos.back() = 1.0f;

        brush->SetInterpolationColors(cols.data(), pos.data(), (INT)cols.size());
    }
    else if (!stops.empty()) {
        Color c = stops[0].color;
        brush->SetLinearColors(c, c);
    }

    brush->SetWrapMode(WrapModeTileFlipXY);
    return brush;
}