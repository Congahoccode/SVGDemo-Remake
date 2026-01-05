#include "stdafx.h"
#include "SVGRadialGradient.h"
#include "SVGDocument.h" 
#include "SVGLinearGradient.h"
#include "SVGHelper.h"
#include <algorithm>
#include <vector>

using namespace std;
using namespace Gdiplus;

void SVGRadialGradient::Parse(rapidxml::xml_node<>* node, SVGDocument* doc)
{
    if (auto attr = node->first_attribute("id")) id = attr->value();
    if (auto attr = node->first_attribute("cx")) cx = ParseUnit(attr->value()); else cx = 0.5f;
    if (auto attr = node->first_attribute("cy")) cy = ParseUnit(attr->value()); else cy = 0.5f;
    if (auto attr = node->first_attribute("r"))  r = ParseUnit(attr->value());  else r = 0.5f;
    if (auto attr = node->first_attribute("fx")) fx = ParseUnit(attr->value()); else fx = cx;
    if (auto attr = node->first_attribute("fy")) fy = ParseUnit(attr->value()); else fy = cy;

    if (auto attr = node->first_attribute("gradientUnits"))
        userSpace = string(attr->value()) == "userSpaceOnUse";

    // Kế thừa
    if (auto attr = node->first_attribute("xlink:href")) {
        string href = attr->value();
        if (!href.empty() && href[0] == '#' && doc) {
            string pid = href.substr(1);
            if (auto* rad = doc->GetRadialGradient(pid)) stops = rad->GetStops();
            else if (auto* lin = doc->GetLinearGradient(pid)) stops = lin->GetStops();
        }
    }

    // Parse Matrix
    if (auto attr = node->first_attribute("gradientTransform")) {
        ParseTransformString(attr->value(), transform);
    }

    // Parse Stops
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

PathGradientBrush* SVGRadialGradient::CreateBrush(const RectF& bounds) const
{
    GraphicsPath path;
    float realR = r;
    if (realR <= 0.0001f) realR = 0.5f;

    // Tạo hình tròn đơn vị tại tâm cx, cy
    path.AddEllipse(cx - realR, cy - realR, realR * 2, realR * 2);

    auto* brush = new PathGradientBrush(&path);
    Matrix m;

    // 1. Áp dụng GradientTransform trước
    if (!transform.IsIdentity()) {
        m.Multiply(const_cast<Matrix*>(&transform), MatrixOrderAppend);
    }

    // 2. Map vào Bounds
    if (!userSpace) {
        m.Translate(bounds.X, bounds.Y, MatrixOrderAppend);
        m.Scale(bounds.Width, bounds.Height, MatrixOrderAppend);
    }

    brush->MultiplyTransform(&m);
    brush->SetCenterPoint(PointF(fx, fy));

    // Setup Colors
    if (stops.size() >= 2) {
        vector<SVGGradientStop> safeStops = stops;
        float maxOff = 0;
        for (auto& s : safeStops) { if (s.offset < maxOff) s.offset = maxOff; else maxOff = s.offset; }

        vector<Color> cols; vector<REAL> pos;
        for (auto it = safeStops.rbegin(); it != safeStops.rend(); ++it) {
            cols.push_back(it->color);
            pos.push_back(1.0f - it->offset);
        }

        if (pos.empty() || pos.front() > 0.001f) {
            Color c = cols.empty() ? Color::Black : cols.front();
            pos.insert(pos.begin(), 0.0f); cols.insert(cols.begin(), c);
        }
        else pos.front() = 0.0f;

        if (pos.back() < 0.999f) {
            Color c = cols.back();
            pos.push_back(1.0f); cols.push_back(c);
        }
        else pos.back() = 1.0f;

        brush->SetInterpolationColors(cols.data(), pos.data(), (INT)cols.size());
    }
    else if (!stops.empty()) {
        brush->SetCenterColor(stops.back().color);
        int c = 1; Color sc = stops.front().color;
        brush->SetSurroundColors(&sc, &c);
    }

    brush->SetWrapMode(WrapModeTileFlipXY);

    return brush;
}