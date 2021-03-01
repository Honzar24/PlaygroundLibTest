#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

template <typename T>
class Image2d;

#include <stdint.h>
#include <vector>
#include <list>
#include <array>
#include <functional>

//=========================================================================

/// <summary>
/// Image dimension structure
/// Holds width and height
/// </summary>
struct ImageDimension
{
	int w;
	int h;

	/// <summary>
	/// Create resized image with given scale
	/// </summary>
	/// <param name="scale"></param>
	/// <returns></returns>
	ImageDimension CreateResized(double scale) const
	{
		return { static_cast<int>(w * scale), static_cast<int>(h * scale) };
	};

	/// <summary>
	/// Create new dimension and keep aspect ratio
	/// new width is calculated from given new height and AR of current size
	/// </summary>
	/// <param name="newHeight"></param>
	/// <returns></returns>
	ImageDimension CreateResizedWidthAr(int newHeight) const
	{
		double ar = double(w) / h;
		return { static_cast<int>(newHeight * ar), newHeight };
	};

	/// <summary>
	/// Create new dimension and keep aspect ratio
	/// new height is calculated from given new width and AR of current size
	/// </summary>
	/// <param name="newWidth"></param>
	/// <returns></returns>
	ImageDimension CreateResizedHeightAr(int newWidth) const
	{
		double ar = double(h) / w;
		return { newWidth, static_cast<int>(newWidth * ar) };
	};
};


//=========================================================================

class ImageUtils 
{
public:
	enum class BorderMode 
	{ 
		CLAMP = 0, 
		WRAP = 1, 
		ENLARGE = 2,
		ZERO = 3	//border is set to 0
	};

	struct Pixel 
	{
		int x;
		int y;

		Pixel() : x(0), y(0) {}
		Pixel(int x, int y) : x(x), y(y) {}
	};


	static std::array<uint8_t, 3> CreateRandomColor();

	template <typename T>
	static void DrawLine(Image2d<T> & input, const T * value,
		int x0, int y0, int x1, int y1);
	
	static void ProcessLinePixels(int x0, int y0, int x1, int y1,
		std::function<void(int x, int y)> pixelCallback);

	
	
	template <typename T, typename V>
	static T clamp_cast(const V & val);

	
private:
	static const int INSIDE = 0; // 0000
	static const int LEFT = 1;   // 0001
	static const int RIGHT = 2;  // 0010
	static const int BOTTOM = 4; // 0100
	static const int TOP = 8;    // 1000

	static const float INF;// = 1E20;

	static int ComputeOutCode(int x, int y, int w, int h);
	
};


/// <summary>
/// cast input val to output
/// if output is uint8_t, 
/// cast is with a clamp to interval [0,255]
/// </summary>
/// <param name="val"></param>
/// <returns></returns>
template <typename T, typename V>
T ImageUtils::clamp_cast(const V & val)
{
	if constexpr (std::is_same<T, uint8_t>::value)
	{
		return (double(val) > 255.0) ? static_cast<T>(255) :
			(double(val) < 0.0) ? static_cast<T>(0) : static_cast<T>(val);
	}
	else
	{
		return static_cast<T>(val);
	}
}

#endif
