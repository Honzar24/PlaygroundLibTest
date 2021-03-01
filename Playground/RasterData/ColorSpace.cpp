#include "./ColorSpace.h"



/// <summary>
/// Get number of channels based on PixelFormat
/// </summary>
/// <param name="pf"></param>
/// <returns></returns>
size_t ColorSpace::GetChannelsCount(PixelFormat pf)
{
	switch (pf)
	{
	case PixelFormat::GRAY: return 1;
	case PixelFormat::RGB: return 3;
	case PixelFormat::RG: return 2;
	case PixelFormat::RGBA: return 4;
	case PixelFormat::XYZ: return 3;	
	case PixelFormat::CIE_LUV: return 3;
	case PixelFormat::HSV: return 3;
	default: return 0;
	}
}

