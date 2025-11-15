#pragma once

#include <vector>
#include <string>

#include "SVGParser.h"
#include "SVGRenderer.h"
#include "SVGElement.h"


// ✅ Khai báo trước (forward declaration) để tránh lỗi Gdiplus chưa định nghĩa
namespace Gdiplus {
    class Graphics;
}

class MainApp
{
private:
    SVGParser parser;                    // Bộ phân tích SVG
    SVGRenderer renderer;                // Bộ kết xuất GDI+
    std::vector<SVGElement*> elements;   // Danh sách các phần tử SVG

public:
    MainApp() = default;
    ~MainApp();

    // Đọc file SVG, trả về true nếu thành công
    bool LoadSVG(const std::string& filePath);

    // Vẽ toàn bộ phần tử SVG
    void Render(Gdiplus::Graphics& g);

    // Giải phóng bộ nhớ
    void Clear();
};
