#include "./ImageUtils.h"

#include <stdlib.h>
#include <algorithm>
#include <stack>
#include <queue>
#include <thread>
#include <string.h>

#include "../Utils/Logger.h"

#include "./Image2d.h"



//=================================================================================================
// Drawing
//==============================================================================================

/// <summary>
/// Create random RGB color
/// Use rand(). It must be inited before
/// </summary>
/// <returns></returns>
std::array<uint8_t, 3> ImageUtils::CreateRandomColor()
{	
	return { uint8_t(rand() & 255), uint8_t(rand() & 255), uint8_t(rand() & 255) };
}

/// <summary>
/// Codes for Cohen-Sutherland
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns></returns>
int ImageUtils::ComputeOutCode(int x, int y, int w, int h)
{
	int code = INSIDE;

	if (x < 0) code |= LEFT;
	else if (x >= w) code |= RIGHT;

	if (y < 0) code |= BOTTOM;
	else if (y >= h) code |= TOP;

	return code;
}


/// <summary>
/// Draw simple line (no anti-aliasing) to image with simple Bresenham algorithm
/// Line if from: [x0, y0] -> [x1, y1] 
/// If line is outside image bounds, it is clamped with Cohen-Sutherland
/// </summary>
/// <param name="input"></param>
/// <param name="value"></param>
/// <param name="x0"></param>
/// <param name="y0"></param>
/// <param name="x1"></param>
/// <param name="y1"></param>
template <typename T>
void ImageUtils::DrawLine(Image2d<T> & input, const T * value,
	int x0, int y0, int x1, int y1)
{
	// compute outcodes for P0, P1, and whatever point lies outside the clip rectangle
	int outcode0 = ImageUtils::ComputeOutCode(x0, y0, input.GetWidth(), input.GetHeight());
	int outcode1 = ImageUtils::ComputeOutCode(x1, y1, input.GetWidth(), input.GetHeight());
	bool accept = false;

	double xmin = 0;
	double xmax = input.GetWidth() - 1;

	double ymin = 0;
	double ymax = input.GetHeight() - 1;

	while (true)
	{
		if (!(outcode0 | outcode1))
		{
			// Bitwise OR is 0. Trivially accept and get out of loop
			accept = true;
			break;
		}
		else if (outcode0 & outcode1)
		{
			// Bitwise AND is not 0. (implies both end points are in the same region outside the window). Reject and get out of loop
			break;
		}
		else
		{
			// failed both tests, so calculate the line segment to clip
			// from an outside point to an intersection with clip edge
			double x = 0;
			double y = 0;

			// At least one endpoint is outside the clip rectangle; pick it.
			int outcodeOut = outcode0 ? outcode0 : outcode1;

			// Now find the intersection point;
			// use formulas y = y0 + slope * (x - x0), x = x0 + (1 / slope) * (y - y0)
			if (outcodeOut & TOP) {           // point is above the clip rectangle
				x = x0 + (x1 - x0) * (ymax - y0) / (y1 - y0);
				y = ymax;
			}
			else if (outcodeOut & BOTTOM) { // point is below the clip rectangle
				x = x0 + (x1 - x0) * (ymin - y0) / (y1 - y0);
				y = ymin;
			}
			else if (outcodeOut & RIGHT) {  // point is to the right of clip rectangle
				y = y0 + (y1 - y0) * (xmax - x0) / (x1 - x0);
				x = xmax;
			}
			else if (outcodeOut & LEFT) {   // point is to the left of clip rectangle
				y = y0 + (y1 - y0) * (xmin - x0) / (x1 - x0);
				x = xmin;
			}

			// Now we move outside point to intersection point to clip
			// and get ready for next pass.
			if (outcodeOut == outcode0)
			{
				x0 = static_cast<int>(x);
				y0 = static_cast<int>(y);
				outcode0 = ImageUtils::ComputeOutCode(x0, y0, input.GetWidth(), input.GetHeight());
			}
			else
			{
				x1 = static_cast<int>(x);
				y1 = static_cast<int>(y);
				outcode1 = ImageUtils::ComputeOutCode(x1, y1, input.GetWidth(), input.GetHeight());
			}
		}
	}

	if (accept == false)
	{
		return;
	}

	ImageUtils::ProcessLinePixels(x0, y0, x1, y1,
		[&](int x, int y) {

		auto * val = input.GetPixelStart(x, y);
		for (size_t c = 0; c < input.GetChannelsCount(); c++)
		{
			val[c] = value[c];
		}

	});
}

/// <summary>
/// Iterate pixels on simple line (no anti-aliasing) with simple Bresenham algorithm
/// Line if from: [x0, y0] -> [x1, y1] 
/// 
/// For each line pixel, pixelCallback(x, y) is called
/// </summary>
/// <param name="x0"></param>
/// <param name="y0"></param>
/// <param name="x1"></param>
/// <param name="y1"></param>
/// <param name="pixelCallback"></param>
void ImageUtils::ProcessLinePixels(int x0, int y0, int x1, int y1,
	std::function<void(int x, int y)> pixelCallback)
{
			   
	int dx = std::abs(x1 - x0);
	int dy = std::abs(y1 - y0);
	int sx, sy, e2;


	(x0 < x1) ? sx = 1 : sx = -1;
	(y0 < y1) ? sy = 1 : sy = -1;
	int err = dx - dy;

	while (1)
	{		
		pixelCallback(x0, y0);
		
		if ((x0 == x1) && (y0 == y1))
		{
			break;
		}
		e2 = 2 * err;
		if (e2 > -dy)
		{
			err = err - dy;
			x0 = x0 + sx;
		}
		if (e2 < dx)
		{
			err = err + dx;
			y0 = y0 + sy;
		}
	}
}




template void ImageUtils::DrawLine(Image2d<float> & input, const float * value, int x0, int y0, int x1, int y1);
template void ImageUtils::DrawLine(Image2d<uint8_t> & input, const uint8_t * value, int x0, int y0, int x1, int y1);

