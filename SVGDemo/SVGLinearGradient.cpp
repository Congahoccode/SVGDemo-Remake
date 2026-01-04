#include "stdafx.h"
#include "SVGLinearGradient.h"
#include "SVGDocument.h"
#include "SVGRadialGradient.h"
#include "SVGHelper.h"
#include "rapidxml.hpp"
#include <string>
#include <vector>
#include <algorithm>

static Color ParseLinearGradientGColor(const std::string& s, float opacity = 1.0f)
{
    int r = 0, g = 0, b = 0;
    int a = (int)(opacity * 255);

    if (s.empty())
        return Color(0, 0, 0, 0);

    // #RRGGBB
    if (s[0] == '#' && s.length() == 7)
    {
        r = stoi(s.substr(1, 2), nullptr, 16);
        g = stoi(s.substr(3, 2), nullptr, 16);
        b = stoi(s.substr(5, 2), nullptr, 16);
    }
    // #RGB
    else if (s[0] == '#' && s.length() == 4)
    {
        r = stoi(s.substr(1, 1), nullptr, 16) * 17;
        g = stoi(s.substr(2, 1), nullptr, 16) * 17;
        b = stoi(s.substr(3, 1), nullptr, 16) * 17;
    }
    // rgb(r,g,b)
    else if (s.rfind("rgb(", 0) == 0)
    {
        int rr, gg, bb;
        sscanf_s(s.c_str(), "rgb(%d,%d,%d)", &rr, &gg, &bb);
        r = rr; g = gg; b = bb;
    }

    return Color(a, r, g, b);
}

static std::string GetStyleValue(const std::string& style, const std::string& key)
{
    size_t pos = style.find(key);
    if (pos == std::string::npos) return "";

    pos = style.find(':', pos);
    if (pos == std::string::npos) return "";

    size_t end = style.find(';', pos);
    return style.substr(pos + 1, end - pos - 1);
}

static float ParseFloat(const char* s)
{
    return (float)atof(s);
}

void SVGLinearGradient::Parse(rapidxml::xml_node<>* node, SVGDocument* doc)
{
    if (auto attr = node->first_attribute("id")) id = attr->value();

    if (auto attr = node->first_attribute("x1")) x1 = ParseUnit(attr->value());
    if (auto attr = node->first_attribute("y1")) y1 = ParseUnit(attr->value());
    if (auto attr = node->first_attribute("x2")) x2 = ParseUnit(attr->value());
    if (auto attr = node->first_attribute("y2")) y2 = ParseUnit(attr->value());

    if (auto attr = node->first_attribute("gradientUnits"))
        userSpace = string(attr->value()) == "userSpaceOnUse";

    // Parse <stop>
    for (auto* stop = node->first_node("stop"); stop; stop = stop->next_sibling("stop"))
    {
        SVGGradientStop gs;
        gs.offset = 0.0f;
        float opacity = 1.0f;
        string colorStr;

        if (auto a = stop->first_attribute("offset"))
        {
            string v = a->value();
            if (v.back() == '%')
                gs.offset = atof(v.c_str()) / 100.0f; 
            else
                gs.offset = atof(v.c_str());
        }

        if (auto c = stop->first_attribute("stop-color"))
            colorStr = c->value();

        if (auto o = stop->first_attribute("stop-opacity"))
            opacity = (float)atof(o->value());

        if (auto s = stop->first_attribute("style"))
        {
            string style = s->value();

            string sc = GetStyleValue(style, "stop-color");
            if (!sc.empty()) colorStr = sc;

            string so = GetStyleValue(style, "stop-opacity");
            if (!so.empty()) opacity = (float)atof(so.c_str());
        }

        gs.color = ParseLinearGradientGColor(colorStr, opacity);
        stops.push_back(gs);
    }
}

LinearGradientBrush* SVGLinearGradient::CreateBrush(const RectF& bounds) const
{
    PointF p1(x1, y1);
    PointF p2(x2, y2);

    // Fix lỗi GDI+ InvalidParameter nếu 2 điểm trùng nhau
    if (abs(p1.X - p2.X) < 0.0001f && abs(p1.Y - p2.Y) < 0.0001f) p2.X += 0.001f;

    auto* brush = new LinearGradientBrush(p1, p2, Color::Black, Color::White);

    Matrix m;

    // 1. Map toạ độ 0..1 vào Bounds (nếu cần)
    if (!userSpace) {
        m.Translate(bounds.X, bounds.Y);
        m.Scale(bounds.Width, bounds.Height);
    }

    // 2. Áp dụng Gradient Transform (Xoay, Nghiêng...)
    if (!transform.IsIdentity()) {
        m.Multiply(const_cast<Matrix*>(&transform), MatrixOrderPrepend);
    }

    brush->MultiplyTransform(&m);

    if (stops.size() >= 2) {
        vector<Color> colors; vector<REAL> positions;
        for (auto& s : stops) { colors.push_back(s.color); positions.push_back(s.offset); }

        if (positions.front() > 0.001f) { positions.insert(positions.begin(), 0.0f); colors.insert(colors.begin(), colors.front()); }
        if (positions.back() < 0.999f) { positions.push_back(1.0f); colors.push_back(colors.back()); }

        brush->SetInterpolationColors(colors.data(), positions.data(), (INT)colors.size());
    }
    return brush;
}