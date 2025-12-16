#include "stdafx.h"
#define _USE_MATH_DEFINES 
#include "SVGPath.h"
#include "SVGHelper.h" // <--- Include
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace Gdiplus;
using namespace std;

// Wrapper đơn giản gọi về Helper
static float GetNextNumber(const char*& ptr)
{
    return ParseNumber(ptr);
}

// Giữ nguyên hàm GetNextFlag cũ của bạn, nó đã ổn
static bool GetNextFlag(const char*& ptr)
{
    while (*ptr && (*ptr == ' ' || *ptr == ',' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r')) ptr++;
    if (*ptr == '0') { ptr++; return false; }
    if (*ptr == '1') { ptr++; return true; }
    return false;
}

// ... (Giữ nguyên phần AddArcToBezier - không cần thay đổi) ...
// Để tiết kiệm không gian, tôi chỉ viết lại phần AddArcToBezier nếu bạn bị mất, 
// nhưng code cũ của bạn đoạn này ok.
static void AddArcToBezier(GraphicsPath& path, float x0, float y0, float rx, float ry,
    float angle, bool largeArc, bool sweep, float x, float y)
{
    // ... Copy lại hàm cũ của bạn ...
    if (rx == 0 || ry == 0) { path.AddLine(PointF(x0, y0), PointF(x, y)); return; }
    if (x0 == x && y0 == y) return;
    rx = abs(rx); ry = abs(ry);
    float dx2 = (x0 - x) / 2.0f; float dy2 = (y0 - y) / 2.0f;
    float rad = angle * (float)M_PI / 180.0f;
    float cosA = cos(rad); float sinA = sin(rad);
    float x1 = cosA * dx2 + sinA * dy2; float y1 = -sinA * dx2 + cosA * dy2;
    float Prx = rx * rx; float Pry = ry * ry; float Px1 = x1 * x1; float Py1 = y1 * y1;
    float d = Px1 / Prx + Py1 / Pry;
    if (d > 1) { rx *= sqrt(d); ry *= sqrt(d); Prx = rx * rx; Pry = ry * ry; }
    float sign = (largeArc == sweep) ? -1.0f : 1.0f;
    float num = Prx * Pry - Prx * Py1 - Pry * Px1;
    float coef = sign * sqrt(max(0.0f, num / (Prx * Py1 + Pry * Px1)));
    float cx1 = coef * ((rx * y1) / ry); float cy1 = coef * -((ry * x1) / rx);
    float sx2 = (x0 + x) / 2.0f; float sy2 = (y0 + y) / 2.0f;
    float cx = sx2 + (cosA * cx1 - sinA * cy1); float cy = sy2 + (sinA * cx1 + cosA * cy1);
    float ux = (x1 - cx1) / rx; float uy = (y1 - cy1) / ry;
    float vx = (-x1 - cx1) / rx; float vy = (-y1 - cy1) / ry;
    auto angleBetween = [](float ux, float uy, float vx, float vy) {
        float sign = (ux * vy - uy * vx < 0) ? -1.0f : 1.0f;
        float dot = ux * vx + uy * vy; return sign * acos(max(-1.0f, min(1.0f, dot)));
        };
    float startAngle = angleBetween(1, 0, ux, uy); float dAngle = angleBetween(ux, uy, vx, vy);
    if (sweep && dAngle < 0) dAngle += 2 * (float)M_PI; else if (!sweep && dAngle > 0) dAngle -= 2 * (float)M_PI;
    int segments = (int)ceil(abs(dAngle) / (M_PI / 2.0));
    float delta = dAngle / segments;
    float t = 8.0f / 3.0f * sin(delta / 4.0f) * sin(delta / 4.0f) / sin(delta / 2.0f);
    float cosTheta1 = cos(startAngle); float sinTheta1 = sin(startAngle);
    for (int i = 0; i < segments; ++i) {
        float theta2 = startAngle + delta;
        float cosTheta2 = cos(theta2); float sinTheta2 = sin(theta2);
        float epx1 = cosA * rx * cosTheta2 - sinA * ry * sinTheta2 + cx;
        float epy1 = sinA * rx * cosTheta2 + cosA * ry * sinTheta2 + cy;
        float dx1 = t * (-cosA * rx * sinTheta1 - sinA * ry * cosTheta1);
        float dy1 = t * (-sinA * rx * sinTheta1 + cosA * ry * cosTheta1);
        float dxe = t * (cosA * rx * sinTheta2 + sinA * ry * cosTheta2);
        float dye = t * (sinA * rx * sinTheta2 - cosA * ry * cosTheta2);
        path.AddBezier(x0, y0, x0 + dx1, y0 + dy1, epx1 - dxe, epy1 - dye, epx1, epy1);
        startAngle = theta2; x0 = epx1; y0 = epy1; cosTheta1 = cosTheta2; sinTheta1 = sinTheta2;
    }
}

// Phần Parse giữ nguyên logic nhưng đổi sang dùng GetNextNumber (tức là ParseNumber)
void SVGPath::Parse(rapidxml::xml_node<>* node)
{
    SVGElement::Parse(node); // Gọi parse cha để lấy fill/stroke/transform

    if (auto attrRule = node->first_attribute("fill-rule")) {
        string r = attrRule->value();
        if (r == "evenodd") path.SetFillMode(FillModeAlternate);
        else if (r == "nonzero") path.SetFillMode(FillModeWinding);
    }

    rapidxml::xml_attribute<>* attr = node->first_attribute("d");
    if (!attr) return;

    const char* ptr = attr->value();
    PointF current(0, 0), startFig(0, 0), lastControl(0, 0);
    char command = ' ';

    while (*ptr)
    {
        // Skip delimiters
        while (*ptr && strchr(" \t\n\r,", *ptr)) ptr++;
        if (!*ptr) break;

        if (isalpha(*ptr)) command = *ptr++;

        switch (command)
        {
            // Các case M, L, C, Z... của bạn giữ nguyên
            // Chỉ cần đảm bảo tất cả đều gọi GetNextNumber(ptr)
        case 'M': 
        { 
            float x = GetNextNumber(ptr);
            float y = GetNextNumber(ptr);
            path.StartFigure(); current = startFig = lastControl = PointF(x, y);
            command = 'L'; break;
        }
        case 'm': {
            float dx = GetNextNumber(ptr);
            float dy = GetNextNumber(ptr);

            if (path.GetPointCount() == 0) {
                current = startFig = PointF(dx, dy);
            }
            else {
                path.StartFigure();
                current = startFig = PointF(current.X + dx, current.Y + dy);
            }

            lastControl = current;
            command = 'l'; // Sau lệnh 'm' (moveto relative), các lệnh tiếp theo được coi là 'l' (lineto)
            break;
        }

        case 'L': {
            float x = GetNextNumber(ptr);
            float y = GetNextNumber(ptr);
            path.AddLine(current, PointF(x, y));
            current = lastControl = PointF(x, y);
            break;
        }

        case 'l': {
            float dx = GetNextNumber(ptr);
            float dy = GetNextNumber(ptr);
            PointF p(current.X + dx, current.Y + dy);
            path.AddLine(current, p);
            current = lastControl = p;
            break;
        }

        case 'H': {
            float x = GetNextNumber(ptr);
            path.AddLine(current, PointF(x, current.Y));
            current.X = x;
            lastControl = current;
            break;
        }

        case 'h': {
            float dx = GetNextNumber(ptr);
            PointF p(current.X + dx, current.Y);
            path.AddLine(current, p);
            current = lastControl = p;
            break;
        }

        case 'V': {
            float y = GetNextNumber(ptr);
            path.AddLine(current, PointF(current.X, y));
            current.Y = y;
            lastControl = current;
            break;
        }

        case 'v': {
            float dy = GetNextNumber(ptr);
            PointF p(current.X, current.Y + dy);
            path.AddLine(current, p);
            current = lastControl = p;
            break;
        }

        case 'C': {
            float x1 = GetNextNumber(ptr);
            float y1 = GetNextNumber(ptr);
            float x2 = GetNextNumber(ptr);
            float y2 = GetNextNumber(ptr);
            float x = GetNextNumber(ptr);
            float y = GetNextNumber(ptr);

            path.AddBezier(current, PointF(x1, y1), PointF(x2, y2), PointF(x, y));
            lastControl = PointF(x2, y2);
            current = PointF(x, y);
            break;
        }

        case 'c': {
            float dx1 = GetNextNumber(ptr);
            float dy1 = GetNextNumber(ptr);
            float dx2 = GetNextNumber(ptr);
            float dy2 = GetNextNumber(ptr);
            float dx = GetNextNumber(ptr);
            float dy = GetNextNumber(ptr);

            PointF p1(current.X + dx1, current.Y + dy1);
            PointF p2(current.X + dx2, current.Y + dy2);
            PointF p(current.X + dx, current.Y + dy);

            path.AddBezier(current, p1, p2, p);
            lastControl = p2;
            current = p;
            break;
        }

        case 'S': {
            // Phản chiếu điểm điều khiển (reflection)
            float x1 = 2 * current.X - lastControl.X;
            float y1 = 2 * current.Y - lastControl.Y;
            float x2 = GetNextNumber(ptr);
            float y2 = GetNextNumber(ptr);
            float x = GetNextNumber(ptr);
            float y = GetNextNumber(ptr);

            path.AddBezier(current, PointF(x1, y1), PointF(x2, y2), PointF(x, y));
            lastControl = PointF(x2, y2);
            current = PointF(x, y);
            break;
        }

        case 's': {
            float x1 = 2 * current.X - lastControl.X;
            float y1 = 2 * current.Y - lastControl.Y;
            float dx2 = GetNextNumber(ptr);
            float dy2 = GetNextNumber(ptr);
            float dx = GetNextNumber(ptr);
            float dy = GetNextNumber(ptr);

            PointF p2(current.X + dx2, current.Y + dy2);
            PointF p(current.X + dx, current.Y + dy);

            path.AddBezier(current, PointF(x1, y1), p2, p);
            lastControl = p2;
            current = p;
            break;
        }

        case 'Q': {
            // Chuyển đổi Quadratic Bezier sang Cubic Bezier
            float x1 = GetNextNumber(ptr);
            float y1 = GetNextNumber(ptr);
            float x = GetNextNumber(ptr);
            float y = GetNextNumber(ptr);

            PointF c1(current.X + 2.0f / 3.0f * (x1 - current.X), current.Y + 2.0f / 3.0f * (y1 - current.Y));
            PointF c2(x + 2.0f / 3.0f * (x1 - x), y + 2.0f / 3.0f * (y1 - y));

            path.AddBezier(current, c1, c2, PointF(x, y));
            lastControl = PointF(x1, y1);
            current = PointF(x, y);
            break;
        }

        case 'q': {
            float dx1 = GetNextNumber(ptr);
            float dy1 = GetNextNumber(ptr);
            float dx = GetNextNumber(ptr);
            float dy = GetNextNumber(ptr);

            PointF p1(current.X + dx1, current.Y + dy1);
            PointF p(current.X + dx, current.Y + dy);

            // Tính toán control points để chuyển từ Quadratic sang Cubic
            PointF c1(current.X + 2.0f / 3.0f * (p1.X - current.X), current.Y + 2.0f / 3.0f * (p1.Y - current.Y));
            PointF c2(p.X + 2.0f / 3.0f * (p1.X - p.X), p.Y + 2.0f / 3.0f * (p1.Y - p.Y));

            path.AddBezier(current, c1, c2, p);
            lastControl = p1;
            current = p;
            break;
        }

        case 'T': {
            float x = GetNextNumber(ptr);
            float y = GetNextNumber(ptr);

            float x1 = 2 * current.X - lastControl.X;
            float y1 = 2 * current.Y - lastControl.Y;

            PointF c1(current.X + 2.0f / 3.0f * (x1 - current.X), current.Y + 2.0f / 3.0f * (y1 - current.Y));
            PointF c2(x + 2.0f / 3.0f * (x1 - x), y + 2.0f / 3.0f * (y1 - y));

            path.AddBezier(current, c1, c2, PointF(x, y));
            lastControl = PointF(x1, y1);
            current = PointF(x, y);
            break;
        }

        case 't': {
            float dx = GetNextNumber(ptr);
            float dy = GetNextNumber(ptr);
            PointF p(current.X + dx, current.Y + dy);

            float x1 = 2 * current.X - lastControl.X;
            float y1 = 2 * current.Y - lastControl.Y;

            PointF c1(current.X + 2.0f / 3.0f * (x1 - current.X), current.Y + 2.0f / 3.0f * (y1 - current.Y));
            PointF c2(p.X + 2.0f / 3.0f * (x1 - p.X), p.Y + 2.0f / 3.0f * (y1 - p.Y));

            path.AddBezier(current, c1, c2, p);
            lastControl = PointF(x1, y1);
            current = p;
            break;
        }

        case 'A': {
            float rx = GetNextNumber(ptr);
            float ry = GetNextNumber(ptr);
            float angle = GetNextNumber(ptr);
            bool large = GetNextFlag(ptr);
            bool sweep = GetNextFlag(ptr);
            float x = GetNextNumber(ptr);
            float y = GetNextNumber(ptr);

            AddArcToBezier(path, current.X, current.Y, rx, ry, angle, large, sweep, x, y);
            current = lastControl = PointF(x, y);
            break;
        }

        case 'a': {
            float rx = GetNextNumber(ptr);
            float ry = GetNextNumber(ptr);
            float angle = GetNextNumber(ptr);
            bool large = GetNextFlag(ptr);
            bool sweep = GetNextFlag(ptr);
            float dx = GetNextNumber(ptr);
            float dy = GetNextNumber(ptr);
            float x = current.X + dx;
            float y = current.Y + dy;

            AddArcToBezier(path, current.X, current.Y, rx, ry, angle, large, sweep, x, y);
            current = lastControl = PointF(x, y);
            break;
        }

        case 'Z':
        case 'z': {
            path.CloseFigure();
            current = lastControl = startFig;
            break;
        }
        default: ptr++; break;
        }
    }
}

void SVGPath::Draw(Graphics& g)
{
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    GraphicsState state = g.Save();
    g.MultiplyTransform(&transform);
    RectF bounds; path.GetBounds(&bounds);
    if (auto* brush = CreateFillBrush(bounds)) 
    { 
        g.FillPath(brush, &path);
        delete brush;
    }
    if (auto* pen = CreateStrokePen()) 
    {
        g.DrawPath(pen, &path); 
        delete pen; 
    }
    g.Restore(state);
}

RectF SVGPath::GetBoundingBox()
{
    RectF bounds; path.GetBounds(&bounds); return bounds;
}