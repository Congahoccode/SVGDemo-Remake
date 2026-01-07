#include "stdafx.h"
#include "MainApp.h"
#include <iostream>
#include <gdiplus.h>

using namespace Gdiplus;
using namespace std;

MainApp::MainApp()
{
    parser = new SVGParser();
}

MainApp::~MainApp()
{
    if (parser) {
        parser->Clear();
        delete parser;
        parser = nullptr;
    }
}

void MainApp::Clear()
{
    if (parser) {
        parser->Clear();
        delete parser;
        parser = nullptr;
    }
    parser = new SVGParser();

    renderer.ResetTransform();
    needsAutoFit = false;
}

bool MainApp::LoadSVG(const string& filePath)
{
    Clear();
    if (!parser) parser = new SVGParser();
    bool ok = parser->ParseFile(filePath);
    if (!ok)
    {
        wstring wFilePath(filePath.begin(), filePath.end());
        return false;
    }
    needsAutoFit = true;
    return true;
}

void MainApp::Render(Graphics& g)
{
    if (!parser || parser->GetElements().empty())
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
        return;
    }

    if (needsAutoFit)
    {
        RectF bounds;
        g.GetVisibleClipBounds(&bounds);

        const SVGDocument& doc = parser->GetDocument();

        if (doc.hasViewBox && doc.viewW > 0 && doc.viewH > 0)
        {
            renderer.FitViewBox((int)bounds.Width, (int)bounds.Height,
                doc.viewX, doc.viewY, doc.viewW, doc.viewH);
        }
        else
        {
            renderer.AutoFit((int)bounds.Width, (int)bounds.Height, parser->GetElements());
        }

        needsAutoFit = false;
    }

    renderer.Render(g, parser->GetElements());
}