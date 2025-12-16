#include "stdafx.h"
#include "MainApp.h"
#include <iostream>
#include <gdiplus.h>

using namespace Gdiplus;

MainApp::~MainApp()
{
    parser.Clear();
}

void MainApp::Clear()
{
    parser.Clear();
    renderer.ResetTransform();
}

bool MainApp::LoadSVG(const std::string& filePath)
{
    // 1. Dọn dẹp
    Clear();

    // 2. Parse file mới
    bool ok = parser.ParseFile(filePath);
    if (!ok)
    {
        std::wstring wFilePath(filePath.begin(), filePath.end());
        std::cerr << "Không thể đọc File SVG: " << filePath << std::endl;
        return false;
    }

    std::cout << "Đã đọc thành công " << parser.GetElements().size() << " phần tử SVG." << std::endl;
    needsAutoFit = true;

    return true;
}

void MainApp::Render(Graphics& g)
{
    // 1. Nếu có hình
    if (!parser.GetElements().empty())
    {
        //2. AutoFit
        if (needsAutoFit)
        {
            RectF bounds;
            g.GetVisibleClipBounds(&bounds); // Lấy kích thước màn hình hiện tại
            renderer.AutoFit((int)bounds.Width, (int)bounds.Height, parser.GetElements());

            needsAutoFit = false;
        }

        // 3. Vẽ hình
        renderer.Render(g, parser.GetElements());
    }
    // 4. Nếu chưa có hình -> Hiện thông báo
    else
    {
        SolidBrush brush(Color(128, 128, 128));
        FontFamily fontFamily(L"Arial");
        Font font(&fontFamily, 16, FontStyleBold, UnitPoint);
        StringFormat format;
        format.SetAlignment(StringAlignmentCenter);
        format.SetLineAlignment(StringAlignmentCenter);

        RectF bounds;
        g.GetVisibleClipBounds(&bounds);
        g.DrawString(L"Vui lòng kéo thả File SVG vào đây!", -1, &font, bounds, &format, &brush);
    }
}