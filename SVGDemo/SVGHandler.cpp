#include "stdafx.h"
#include "SVGHandler.h"

void SVGHandler::OnMouseUp(int button) 
{
	if (button == 0) isPanning = false; // Left mouse button
}

bool SVGHandler::OnMouseWheel(int delta, bool ctrlDown) 
{
	if (ctrlDown) 
	{
		float zoomFactor = (delta > 0) ? 1.1f : 0.9f;
		renderer->Zoom(zoomFactor);
		return true;
	}
	return false;
}

void SVGHandler::OnMouseDown(const Gdiplus::PointF& pos, int button) 
{
	if (button == 0) 
	{								// Left button
		isPanning = true;
		lastMousePos.x = static_cast<LONG>(pos.X);
		lastMousePos.y = static_cast<LONG>(pos.Y);
	}
}

bool SVGHandler::OnMouseMove(const Gdiplus::PointF& pos) 
{
	if (isPanning) 
	{
		float dx = pos.X - lastMousePos.x;
		float dy = pos.Y - lastMousePos.y;
		renderer->Pan(dx, dy);
		lastMousePos.x = static_cast<LONG>(pos.X);
		lastMousePos.y = static_cast<LONG>(pos.Y);
		return true;
	}
	return false;
}
