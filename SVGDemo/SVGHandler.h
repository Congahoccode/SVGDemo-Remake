#pragma once
#include "SVGRenderer.h"

class SVGHandler 
{
private:
	SVGRenderer* renderer;
	bool isPanning;
	POINT lastMousePos;
public:
	SVGHandler(SVGRenderer* rend) : renderer(rend), isPanning(false) {}
	void OnMouseUp(int button);
	bool OnMouseWheel(int delta, bool ctrlDown);
	void OnMouseDown(const Gdiplus::PointF& pos, int button);
	bool OnMouseMove(const Gdiplus::PointF& pos);
};