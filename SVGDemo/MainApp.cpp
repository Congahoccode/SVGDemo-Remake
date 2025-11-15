#include "stdafx.h"
#include "MainApp.h"
#include <iostream>
#include <gdiplus.h>
using namespace Gdiplus;

MainApp::~MainApp()
{
    Clear();
}

bool MainApp::LoadSVG(const std::string& filePath)
{
    Clear(); // Xóa dữ liệu cũ nếu có

    bool ok = parser.ParseFile(filePath);
    if (!ok)
    {
        std::cerr << "Không thể đọc file SVG: " << filePath << std::endl;
        return false;
    }

    elements = parser.GetElements();
    std::cout << "Đã đọc thành công " << elements.size() << " phần tử SVG." << std::endl;
    return true;
}

void MainApp::Render(Gdiplus::Graphics& g)
{
    if (elements.empty())
    {
        std::cerr << "Không có phần tử SVG nào để vẽ!" << std::endl;
        return;
    }

    renderer.Render(g, elements);
}

void MainApp::Clear()
{
    for (auto e : elements)
        delete e;
    elements.clear();
}
