#include "stdafx.h"
#include "SVGRadialGradient.h"
#include "SVGDocument.h" 
#include "SVGLinearGradient.h"
#include "SVGHelper.h"
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>

using namespace std;
using namespace Gdiplus;

// Helper: Hex2Int (Static)
static int Hex2Int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

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
            SVGLinearGradient* lin = doc->GetLinearGradient(pid);
            if (lin) this->stops = lin->GetStops();
            else {
                SVGRadialGradient* rad = doc->GetRadialGradient(pid);
                if (rad) this->stops = rad->GetStops();
            }
        }
    }

    // Parse Matrix (Fix Instagram)
    if (auto attr = node->first_attribute("gradientTransform")) {
        string t = attr->value();
        transform.Reset();

        size_t pos = 0;
        while (pos < t.length()) {
            size_t start = t.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ", pos);
            if (start == string::npos) break;
            size_t openParen = t.find('(', start);
            if (openParen == string::npos) break;
            string command = t.substr(start, openParen - start);
            size_t closeParen = t.find(')', openParen);
            if (closeParen == string::npos) break;
            string args = t.substr(openParen + 1, closeParen - openParen - 1);

            vector<float> vals;
            ParseNumberList(args.c_str(), vals);

            if (command == "matrix" && vals.size() >= 6) {
                Matrix m(vals[0], vals[1], vals[2], vals[3], vals[4], vals[5]);
                transform.Multiply(&m, MatrixOrderPrepend);
            }
            else if (command == "translate" && vals.size() >= 1) {
                transform.Translate(vals[0], (vals.size() > 1) ? vals[1] : 0, MatrixOrderPrepend);
            }
            else if (command == "scale" && vals.size() >= 1) {
                transform.Scale(vals[0], (vals.size() > 1) ? vals[1] : vals[0], MatrixOrderPrepend);
            }
            else if (command == "rotate" && vals.size() >= 1) {
                if (vals.size() >= 3) transform.RotateAt(vals[0], PointF(vals[1], vals[2]), MatrixOrderPrepend);
                else transform.Rotate(vals[0], MatrixOrderPrepend);
            }
            pos = closeParen + 1;
        }
    }

    // Parse Stops
    if (node->first_node("stop")) stops.clear();

    for (auto* stop = node->first_node("stop"); stop; stop = stop->next_sibling("stop")) {
        SVGGradientStop gs; gs.offset = 0.0f;
        if (auto a = stop->first_attribute("offset")) gs.offset = ParseUnit(a->value());

        int rr = 0, gg = 0, bb = 0, aa = 255;
        if (auto c = stop->first_attribute("stop-color")) {
            string s = c->value();
            if (!s.empty() && s[0] == '#') {
                string hex = s.substr(1);
                if (hex.length() >= 6) {
                    rr = stoi(hex.substr(0, 2), 0, 16); gg = stoi(hex.substr(2, 2), 0, 16); bb = stoi(hex.substr(4, 2), 0, 16);
                }
                else if (hex.length() >= 3) {
                    rr = Hex2Int(hex[0]) * 17; gg = Hex2Int(hex[1]) * 17; bb = Hex2Int(hex[2]) * 17;
                }
            }
        }

        if (auto op = stop->first_attribute("stop-opacity")) {
            const char* ptr = op->value();
            aa = (int)(255 * ParseNumber(ptr));
        }

        gs.color = Color(aa, rr, gg, bb);
        stops.push_back(gs);
    }
}

PathGradientBrush* SVGRadialGradient::CreateBrush(const RectF& bounds) const
{
    GraphicsPath path;

    // --- LOGIC MỚI: UNIT SPACE MAPPING (Sửa lỗi Instagram) ---
    // Tạo hình tròn chuẩn ở không gian gốc
    path.AddEllipse(cx - r, cy - r, r * 2, r * 2);

    auto* brush = new PathGradientBrush(&path);

    Matrix m;

    // Nếu objectBoundingBox -> Map vào Bounds
    if (!userSpace) {
        m.Translate(bounds.X, bounds.Y);
        m.Scale(bounds.Width, bounds.Height);
    }

    // Áp dụng gradientTransform (Ma trận của SVG)
    if (!transform.IsIdentity()) {
        m.Multiply(const_cast<Matrix*>(&transform), MatrixOrderPrepend);
    }

    // Áp dụng Matrix tổng hợp vào Brush
    brush->MultiplyTransform(&m);

    // Thiết lập tâm tiêu điểm
    brush->SetCenterPoint(PointF(fx, fy));

    // Setup màu
    if (stops.size() >= 2) {
        vector<Color> colors; vector<REAL> positions;
        for (auto it = stops.rbegin(); it != stops.rend(); ++it) {
            colors.push_back(it->color);
            positions.push_back(1.0f - it->offset);
        }
        if (positions.front() > 0.001f) { positions.insert(positions.begin(), 0.0f); colors.insert(colors.begin(), colors.front()); }
        if (positions.back() < 0.999f) { positions.push_back(1.0f); colors.push_back(colors.back()); }

        brush->SetInterpolationColors(colors.data(), positions.data(), (INT)colors.size());
    }
    else if (!stops.empty()) {
        brush->SetCenterColor(stops.back().color);
        int c = 1; Color sc = stops.front().color;
        brush->SetSurroundColors(&sc, &c);
    }

    return brush;
}