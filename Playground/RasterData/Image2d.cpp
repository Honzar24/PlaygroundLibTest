#include "./Image2d.h"

#include <algorithm>

#include "../Compression/3rdParty/lodepng.h"
#include "../Compression/PNGLoader.h"

#include "../Utils/Logger.h"

#include "../FileUtils/FileMacros.h"
#include "../FileUtils/RawFile.h"


#ifdef HAVE_OPENCV
#	include <opencv2/highgui.hpp>
#	ifdef _DEBUG
#		pragma comment(lib, "opencv_world420d.lib")
#	else
#		pragma comment(lib, "opencv_world420.lib")
#	endif
#endif


#ifdef _MSC_VER
#	ifndef my_fopen 
#		define my_fopen(a, b, c) fopen_s(a, b, c)	
#	endif	
#else
#	ifndef my_fopen 
#		define my_fopen(a, b, c) (*a = fopen(b, c))
#	endif	
#endif


//=================================================================================================
// static factories
//=================================================================================================

/// <summary>
/// Load data from RAW file
/// User must specify image dimension and format,
/// loaded file is loaded directly as it is
/// </summary>
/// <param name="w"></param>
/// <param name="h"></param>
/// <param name="pf"></param>
/// <param name="fileName"></param>
/// <returns></returns>
template <typename T>
Image2d<T> Image2d<T>::CreateFromRawFile(int w, int h, ColorSpace::PixelFormat pf, const char * fileName)
{
	Image2d<T> img(w, h, pf);

	RawFile f = RawFile(fileName, "rb");
	if (f.GetSize() != img.data.size() * sizeof(T))
	{
		return img;
	}
	f.Read(&img.data[0], sizeof(T), img.data.size());
	return img;
}

/// <summary>
/// Create image filled with single value
/// </summary>
/// <param name="w"></param>
/// <param name="h"></param>
/// <param name="value"></param>
/// <param name="pf"></param>
/// <returns></returns>
template <typename T>
Image2d<T> Image2d<T>::CreateWithSingleValue(int w, int h, const T * value, ColorSpace::PixelFormat pf)
{		
	size_t chanCount = ColorSpace::GetChannelsCount(pf);

	if (chanCount == 1)
	{
		Image2d<T> img;
		img.dim = { w, h };
		img.channelsCount = chanCount;
		img.pf = pf;
		img.data = std::vector<T>(w * h * img.channelsCount, *value);

		return img;
	}
	else
	{
		Image2d<T> img(w, h, pf);

		for (size_t i = 0; i < img.GetPixelsCount(); i++)
		{
			T * px = img.GetPixelStart(i);
			for (size_t c = 0; c < img.GetChannelsCount(); c++)
			{
				px[c] = value[c];
			}
		}

		return img;
	}		
}

//=================================================================================================
// ctors & dtor
//=================================================================================================

/// <summary>
/// Create empty image with size 0
/// </summary>
template <typename T>
Image2d<T>::Image2d() : 
	dim({ 0, 0 }),
	pf(ColorSpace::PixelFormat::NONE),
	channelsCount(ColorSpace::GetChannelsCount(ColorSpace::PixelFormat::NONE))
{
}

/// <summary>
/// Create image form file with fileName
/// Only supported compression of file are PNG and JPG
/// (Files are loaded as they are. If they contains color profile,
/// it is ignored)
/// </summary>
/// <param name="fileName"></param>
template <typename T>
Image2d<T>::Image2d(const char * fileName) :
	dim({ 0, 0 }),
	pf(ColorSpace::PixelFormat::NONE),
	channelsCount(ColorSpace::GetChannelsCount(ColorSpace::PixelFormat::NONE))
{
	RawFile f = RawFile(fileName, "rb");
	if (f.GetRawFilePtr() == nullptr)
	{
		MY_LOG_ERROR("File %s not found", fileName);
		return;
	}

	uint8_t header[8];
	f.Read(header, sizeof(uint8_t), 8);
	f.Seek(0, SEEK_SET);

	if ((header[0] == 137) && (header[1] == 'P')) // N G \r \n \032 \n
	{
		//PNG
		PNGLoader png;
		png.SetKeepPalette(false); //we do not want palette - just a simple load
		auto dPng = png.DecompressFromFile(&f);

		this->dim.w = dPng.w;
		this->dim.h = dPng.h;
		
		switch (dPng.channelsCount)
		{
		case 1:
			this->pf = ColorSpace::PixelFormat::GRAY;
			break;
		case 3:
			this->pf = ColorSpace::PixelFormat::RGB;
			break;
		case 4:
			this->pf = ColorSpace::PixelFormat::RGBA;
			break;
		default:
			this->pf = ColorSpace::PixelFormat::NONE;
			break;
		}

		this->channelsCount = dPng.channelsCount;
		if constexpr (std::is_same<T, uint8_t>::value)
		{
			this->data = std::move(dPng.data);
		}
		else 
		{
			this->data.resize(dPng.data.size());

			for (size_t i = 0; i < this->data.size(); i++)
			{
				this->data[i] = static_cast<T>(dPng.data[i] / 255.0);
			}
		}
	}
	else if ((header[0] == 0xFF) && (header[1] == 0xD8))
	{
		//JPG	
		//not supported
	}
	
}


/// <summary>
/// Create empty image with size w x h and given pixel format
/// </summary>
/// <param name="w"></param>
/// <param name="h"></param>
/// <param name="pf"></param>
template <typename T>
Image2d<T>::Image2d(int w, int h, ColorSpace::PixelFormat pf) :
	dim({ w, h }),	
	data(std::vector<T>(w * h * ColorSpace::GetChannelsCount(pf), 0)),
	pf(pf),
	channelsCount(ColorSpace::GetChannelsCount(pf))
{		
}

/// <summary>
/// Create image with size w x h, filled with data and given pixel format
/// </summary>
/// <param name="w"></param>
/// <param name="h"></param>
/// <param name="data"></param>
/// <param name="pf"></param>
template <typename T>
Image2d<T>::Image2d(int w, int h, const std::vector<T> & data, ColorSpace::PixelFormat pf) :
	dim({ w, h }),
	data(data),
	pf(pf),
	channelsCount(ColorSpace::GetChannelsCount(pf))
{
}

/// <summary>
/// Create image with size w x h, filled with data and given pixel format
/// </summary>
/// <param name="w"></param>
/// <param name="h"></param>
/// <param name="data"></param>
/// <param name="pf"></param>
template <typename T>
Image2d<T>::Image2d(int w, int h, const T * rawData, ColorSpace::PixelFormat pf) :
	dim({ w, h }),
	data(rawData, rawData + w * h * ColorSpace::GetChannelsCount(pf)),
	pf(pf),
	channelsCount(ColorSpace::GetChannelsCount(pf))
{
}

template <typename T>
Image2d<T>::Image2d(int w, int h, const T ** rawData, ColorSpace::PixelFormat pf) :
	dim({ w, h }),	
	pf(pf),
	channelsCount(ColorSpace::GetChannelsCount(pf))
{
	for (int i = 0; i < h; i++)
	{
		data.insert(data.end(), rawData[i], rawData[i] + w * channelsCount);
	}
}

/// <summary>
/// Create image with size w x h, filled with data and given pixel format
/// data are moved
/// </summary>
/// <param name="w"></param>
/// <param name="h"></param>
/// <param name="data"></param>
/// <param name="pf"></param>
template <typename T>
Image2d<T>::Image2d(int w, int h, std::vector<T> && data, ColorSpace::PixelFormat pf) :
	dim({ w, h }),	
	data(std::move(data)),
	pf(pf),
	channelsCount(ColorSpace::GetChannelsCount(pf))
{	
}

template <typename T>
Image2d<T>::Image2d(const Image2d<T> & other) : 
	dim(other.dim),	
	data(other.data),
	pf(other.pf),
	channelsCount(other.channelsCount)
{
}

/// <summary>
/// move ctor
/// </summary>
/// <param name="other"></param>
template <typename T>
Image2d<T>::Image2d(Image2d<T> && other) noexcept :
	dim(other.dim),
	data(std::move(other.data)),
	pf(other.pf),
	channelsCount(other.channelsCount)
{
	other.Release();
}


#ifdef HAVE_OPENCV

/// <summary>
/// Copy data from cv::Mat to Image
/// Data are treated as GRAY/RGB/RGBA
/// Image owns the data
/// 
/// Source: https://stackoverflow.com/questions/26681713/convert-mat-to-array-vector-in-opencv
/// </summary>
/// <param name="cvMat"></param>
template <typename T>
Image2d<T>::Image2d(const cv::Mat & cvMat) :
	dim({ cvMat.cols, cvMat.rows }),	
	channelsCount(cvMat.channels())
{	
	if (channelsCount == 1) pf = ColorSpace::PixelFormat::GRAY;
	else if (channelsCount == 3) pf = ColorSpace::PixelFormat::RGB;
	else if (channelsCount == 4) pf = ColorSpace::PixelFormat::RGBA;
	else pf = ColorSpace::PixelFormat::NONE;
	
	if (cvMat.isContinuous())
	{		
		data.assign((T*)cvMat.data, (T*)cvMat.data + cvMat.total() * channelsCount);		
	}
	else 
	{
		for (int i = 0; i < cvMat.rows; i++)
		{
			data.insert(data.end(), cvMat.ptr<T>(i), cvMat.ptr<T>(i) + cvMat.cols);
		}
	}
}

#endif

/// <summary>
/// dtor
/// </summary>
template <typename T>
Image2d<T>::~Image2d()
{	
}

/// <summary>
/// Manually release data
/// dimensions of image are set to 0
/// </summary>
template <typename T>
void Image2d<T>::Release() noexcept
{
	this->data.clear();
	this->channelsCount = 0;
	this->dim.w = 0;
	this->dim.h = 0;
	this->pf = ColorSpace::PixelFormat::NONE;
}

//=================================================================================================
// Operators section
//=================================================================================================


template <typename T>
Image2d<T> & Image2d<T>::operator=(const Image2d<T> & other)
{
	this->dim = other.dim;	
	this->data = other.data;
	this->pf = other.pf;
	this->channelsCount = other.channelsCount;
	return *this;
}

template <typename T>
Image2d<T> & Image2d<T>::operator=(Image2d<T> && other) noexcept
{
	this->dim = other.dim;	
	this->data = std::move(other.data);	
	this->pf = other.pf;
	this->channelsCount = other.channelsCount;

	other.Release();

	return *this;
}

//=================================================================================================
// Creators section
// - create new image from existing
//=================================================================================================

/// <summary>
/// Create empty image with same parametrs as the current one
/// </summary>
/// <returns></returns>
template <typename T>
template <typename V>
Image2d<V> Image2d<T>::CreateEmpty() const
{	
	return Image2d<V>(this->GetWidth(),
		this->GetHeight(),
		std::vector<V>(this->data.size(), 0),
		this->pf);
}

/// <summary>
/// Create deep copy of current image
/// </summary>
/// <returns></returns>
template <typename T>
Image2d<T> Image2d<T>::CreateDeepCopy() const
{
	return Image2d<T>(this->GetWidth(),
		this->GetHeight(),
		this->data,
		this->pf);
}

/// <summary>
/// Create new image using only single channel of the current one
/// Output image is PixelFormat::GRAY
/// </summary>
/// <param name="channelIndex"></param>
/// <returns></returns>
template <typename T>
Image2d<T> Image2d<T>::CreateFromChannel(size_t channelIndex) const
{
	size_t len = size_t(this->dim.w) * size_t(this->dim.h);

	std::vector<T> channelData;
	channelData.resize(len);
	
	for (size_t i = 0; i < len; i++)
	{
		const T * tmp = this->GetPixelStart(i);

		channelData[i] = tmp[channelIndex];
	}

	return Image2d<T>(this->GetWidth(),
		this->GetHeight(),
		std::move(channelData),
		ColorSpace::PixelFormat::GRAY);
}

/// <summary>
/// Create new image from the current image sub-image
/// sub-image starts at [x, y] and has given size 
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="size"></param>
/// <returns></returns>
template <typename T>
Image2d<T> Image2d<T>::CreateSubImage(int x, int y, const ImageDimension & size) const
{
	size_t len = size_t(size.w) * size_t(size.h) * this->channelsCount;

	std::vector<T> subData;
	subData.resize(len);

	x = std::max(0, x);
	y = std::max(0, y);
	
	int endX = x + size.w;
	int endY = y + size.h;
		
	endX = std::min(endX, this->GetWidth());
	endY = std::min(endY, this->GetHeight());

	size_t index = 0;
	for (int yy = y; yy < endY; yy++)
	{
		for (int xx = x; xx < endX; xx++)
		{
			const T * tmp = this->GetPixelStart(xx, yy);
			for (size_t c = 0; c < this->channelsCount; c++)
			{
				subData[index] = tmp[c];
				index++;
			}
		}
	}

	return Image2d<T>(size.w,
		size.h,
		std::move(subData),
		this->pf);
}

/// <summary>
/// Create new image from the current image by adding empty
/// border with size w and h around the image
/// </summary>
/// <param name="w"></param>
/// <param name="h"></param>
/// <returns></returns>
template <typename T>
Image2d<T> Image2d<T>::CreateWithBorder(int w, int h) const
{
	Image2d<T> newImage(this->GetWidth() + 2 * w,
		this->GetHeight() + 2 * h,		
		this->pf);
	
	for (int yy = 0; yy < this->GetHeight(); yy++)
	{
		for (int xx = 0; xx < this->GetWidth(); xx++)
		{
			const T* tmp = this->GetPixelStart(xx, yy);

			T* tmp2 = newImage.GetPixelStart(xx + w, yy + h);

			for (size_t c = 0; c < this->channelsCount; c++)
			{				
				tmp2[c] = tmp[c];			
			}
		}
	}

	return newImage;
}


/// <summary>
/// Create new Image2d from current
/// but cast data from T to V
/// 
/// Use to cast image from:
/// float -> uint8_t
/// uint8_t -> float
/// 
/// Data must be in correct range
/// </summary>
/// <returns></returns>
template <typename T>
template <typename V>
Image2d<V> Image2d<T>::CreateAs() const
{	
	std::vector<V> d;
	d.resize(this->data.size());

	size_t dataSize4 = this->data.size() - this->data.size() % 4;

	for (size_t i = 0; i < dataSize4; i += 4)
	{		
		d[i] = static_cast<V>(this->data[i]);
		d[i + 1] = static_cast<V>(this->data[i + 1]);
		d[i + 2] = static_cast<V>(this->data[i + 2]);
		d[i + 3] = static_cast<V>(this->data[i + 3]);
	}

	for (size_t i = dataSize4; i < this->data.size(); i++)
	{
		d[i] = static_cast<V>(this->data[i]);
	}

	return Image2d<V>(this->GetWidth(),
		this->GetHeight(),
		std::move(d),
		this->pf);
}


//=================================================================================================
// Setters
//=================================================================================================

template <typename T>
void Image2d<T>::SetPixelFormat(ColorSpace::PixelFormat pf) noexcept
{
	this->pf = pf;
	this->channelsCount = ColorSpace::GetChannelsCount(pf);
}

/// <summary>
/// Save file to JPG or PNG
/// In case of JPG, default quality 80 is used
/// </summary>
/// <param name="fileName"></param>
template <typename T>
void Image2d<T>::Save(const char * fileName) const
{
	
	size_t len = strlen(fileName);

	if ((len > 4) &&
		(fileName[len - 4] == '.') && (fileName[len - 3] == 'r') &&
		(fileName[len - 2] == 'a') && (fileName[len - 1] == 'w'))
	{
		FILE * f = nullptr;
		my_fopen(&f, fileName, "wb");
		if (f != nullptr)
		{
			fwrite(this->data.data(), sizeof(T), this->data.size(), f);
			fclose(f);
		}
		return;
	}

	struct RawData
	{
		const uint8_t* data;
		size_t w;
		size_t h;
		size_t bytesPerPixel; //number of bytes per pixel		
	};

	RawData rd;
	rd.w = size_t(this->GetWidth());
	rd.h = size_t(this->GetHeight());
	rd.bytesPerPixel = this->GetChannelsCount();
	
	if constexpr (std::is_same<T, uint8_t>::value)
	{
		//PngSaver can modify data by removing premultiplied alpha
		//but we dont use this here
		rd.data = (uint8_t *)(this->data.data());	
	}
	else 
	{
		//data must be mapped to 0 - 1 interval
		//for floats -> we convert them from 0 - 1 to  0 - 255

		uint8_t * d = new uint8_t[this->data.size()];
		for (size_t i = 0; i < this->data.size(); i++)
		{
			d[i] = ImageUtils::clamp_cast<uint8_t>(this->data[i] * 255.0f);
		}

		rd.data = d;
	}
	
	if ((len > 4) &&
		(fileName[len - 4] == '.') && (fileName[len - 3] == 'j') &&
		(fileName[len - 2] == 'p') && (fileName[len - 1] == 'g'))
	{
		//not supported
	}
	else
	{
		if (this->pf == ColorSpace::PixelFormat::GRAY)
		{
			lodepng::encode(fileName, rd.data,
				static_cast<int>(rd.w), static_cast<int>(rd.h),
				LodePNGColorType::LCT_GREY);
		}
		else if (this->pf == ColorSpace::PixelFormat::RGB)
		{
			lodepng::encode(fileName, rd.data,
				static_cast<int>(rd.w), static_cast<int>(rd.h),
				LodePNGColorType::LCT_RGB);
		}
		else if (this->pf == ColorSpace::PixelFormat::RGBA)
		{
			lodepng::encode(fileName, rd.data,
				static_cast<int>(rd.w), static_cast<int>(rd.h),
				LodePNGColorType::LCT_RGBA);
		}


		
	}

	if constexpr (std::is_same<T, uint8_t>::value == false)
	{
		delete rd.data;
		rd.data = nullptr;
	}
}

//=================================================================================================
// Getters
//=================================================================================================


template <typename T>
ColorSpace::PixelFormat Image2d<T>::GetPixelFormat() const noexcept
{
	return this->pf;
}

template <typename T>
size_t Image2d<T>::GetChannelsCount() const noexcept
{
	return this->channelsCount;
}


template <typename T>
int Image2d<T>::GetWidth() const noexcept
{
	return this->dim.w;
}

template <typename T>
int Image2d<T>::GetHeight() const noexcept
{
	return this->dim.h;
}

template <typename T>
size_t Image2d<T>::GetPixelsCount() const noexcept
{
	return size_t(this->dim.w) * size_t(this->dim.h);
}

template <typename T>
const ImageDimension & Image2d<T>::GetDimension() const noexcept
{
	return this->dim;
}

/// <summary>
/// Convert 1D index to 2D [x, y]
/// </summary>
/// <param name="index"></param>
/// <param name="x"></param>
/// <param name="y"></param>
template <typename T>
void Image2d<T>::GetPositionFromIndex(size_t index, int & x, int & y) const noexcept
{
	x = static_cast<int>(index % this->GetWidth());
	y = static_cast<int>(index / this->GetWidth());
}

/// <summary>
/// Convert 2D index [x, y] to 1D index
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns></returns>
template <typename T>
size_t Image2d<T>::GetIndexFromPosition(int x, int y) const noexcept
{
	return size_t(x) + size_t(y) * size_t(this->dim.w);
}

/// <summary>
/// Get indices of 8-ring neighborhood of center pixel
/// Layout:
/// 0 | 1 | 2
/// 3 | x | 4
/// 5 | 6 | 7
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
template <typename T>
std::array<size_t, 8> Image2d<T>::GetNeighborsIndicesFromPosition(size_t index) const noexcept
{
	int x, y;
	this->GetPositionFromIndex(index, x, y);
	return this->GetNeighborsIndicesFromPosition(x, y);
}

/// <summary>
/// Get indices of 8-ring neighborhood of center pixel
/// Layout:
/// 0 | 1 | 2
/// 3 | x | 4
/// 5 | 6 | 7
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns></returns>
template <typename T>
std::array<size_t, 8> Image2d<T>::GetNeighborsIndicesFromPosition(int x, int y) const noexcept
{
	int x1m = (x < 1) ? 0 : x - 1;
	int x1p = (x + 1 >= this->GetWidth()) ? this->GetWidth() - 1 : x + 1;

	int y1m = (y < 1) ? 0 : y - 1;
	int y1p = (y + 1 >= this->GetHeight()) ? this->GetHeight() - 1 : y + 1;

	std::array<size_t, 8> res;

	res[0] = this->GetIndexFromPosition(x1m, y1m);
	res[1] = this->GetIndexFromPosition(x1m, y);
	res[2] = this->GetIndexFromPosition(x1m, y1p);

	res[3] = this->GetIndexFromPosition(x, y1m);
	res[4] = this->GetIndexFromPosition(x, y1p);

	res[5] = this->GetIndexFromPosition(x1p, y1m);
	res[6] = this->GetIndexFromPosition(x1p, y);
	res[7] = this->GetIndexFromPosition(x1p, y1p);

	return res;
}


template <typename T>
std::array<T *, 8> Image2d<T>::GetNeighborsPixelStartsFromPosition(size_t index) noexcept
{
	int x, y;
	this->GetPositionFromIndex(index, x, y);
	return this->GetNeighborsPixelStartsFromPosition(x, y);
}

template <typename T>
std::array<const T *, 8> Image2d<T>::GetNeighborsPixelStartsFromPosition(size_t index) const noexcept
{
	int x, y;
	this->GetPositionFromIndex(index, x, y);
	return this->GetNeighborsPixelStartsFromPosition(x, y);
}

template <typename T>
std::array<T *, 8> Image2d<T>::GetNeighborsPixelStartsFromPosition(int x, int y) noexcept
{
	int x1m = (x < 1) ? 0 : x - 1;
	int x1p = (x + 1 >= this->GetWidth()) ? this->GetWidth() - 1 : x + 1;

	int y1m = (y < 1) ? 0 : y - 1;
	int y1p = (y + 1 >= this->GetHeight()) ? this->GetHeight() - 1 : y + 1;

	std::array<T *, 8> res;

	res[0] = this->GetPixelStart(x1m, y1m);
	res[1] = this->GetPixelStart(x1m, y);
	res[2] = this->GetPixelStart(x1m, y1p);

	res[3] = this->GetPixelStart(x, y1m);
	res[4] = this->GetPixelStart(x, y1p);

	res[5] = this->GetPixelStart(x1p, y1m);
	res[6] = this->GetPixelStart(x1p, y);
	res[7] = this->GetPixelStart(x1p, y1p);

	return res;
}


template <typename T>
std::array<const T *, 8> Image2d<T>::GetNeighborsPixelStartsFromPosition(int x, int y) const noexcept
{
	int x1m = (x < 1) ? 0 : x - 1;
	int x1p = (x + 1 >= this->GetWidth()) ? this->GetWidth() - 1 : x + 1;

	int y1m = (y < 1) ? 0 : y - 1;
	int y1p = (y + 1 >= this->GetHeight()) ? this->GetHeight() - 1 : y + 1;

	std::array<const T *, 8> res;

	res[0] = this->GetPixelStart(x1m, y1m);
	res[1] = this->GetPixelStart(x1m, y);
	res[2] = this->GetPixelStart(x1m, y1p);

	res[3] = this->GetPixelStart(x, y1m);
	res[4] = this->GetPixelStart(x, y1p);

	res[5] = this->GetPixelStart(x1p, y1m);
	res[6] = this->GetPixelStart(x1p, y);
	res[7] = this->GetPixelStart(x1p, y1p);

	return res;
}


/// <summary>
/// Find min / max values for a given channelIndex
/// min / max are outputs
/// </summary>
/// <param name="min"></param>
/// <param name="max"></param>
/// <param name="channelIndex"></param>
template <typename T>
void Image2d<T>::FindMinMax(size_t channelIndex, T & min, T & max) const
{
#ifdef ENABLE_SIMD
	if constexpr (std::is_same<T, float>::value)
	{
		if (this->GetChannelsCount() == 1)
		{
			this->FindMinMaxSimd(min, max);
			return;
		}
	}
#endif

	const T * tmp = this->GetPixelStart(0);
	min = tmp[channelIndex];
	max = tmp[channelIndex];
	
	size_t len = this->GetPixelsCount();
	for (size_t i = 1; i < len; i++)
	{
		const T * tmp = this->GetPixelStart(i);

		max = std::max(tmp[channelIndex], max);
		min = std::min(tmp[channelIndex], min);		
	}
}

/// <summary>
/// Calculate average value of all pixels
/// within channel
/// </summary>
/// <param name="channelIndex"></param>
/// <returns></returns>
template <typename T>
T Image2d<T>::CalcAvgValue(size_t channelIndex) const
{
	float avg = 0.0f;

	size_t len = this->GetPixelsCount();
	for (size_t i = 0; i < len; i++)
	{
		const T * tmp = this->GetPixelStart(i);
		avg += tmp[channelIndex];		
	}

	return static_cast<T>(avg / len);
}

/// <summary>
/// Get pointer to the first element of pixel
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
template <typename T>
const T * Image2d<T>::GetPixelStart(size_t index) const
{
	return &this->data[index * this->GetChannelsCount()];
}

template <typename T>
T * Image2d<T>::GetPixelStart(size_t index)
{
	return &this->data[index * this->GetChannelsCount()];
}


/// <summary>
/// Get pixel at position [x, y]
/// Position can be outside of image bounds and based on border mode,
/// it is updates to be in image bounds
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="border"></param>
/// <returns></returns>
template <typename T>
const T * Image2d<T>::GetPixelStartWithBorder(int x, int y, ImageUtils::BorderMode border) const
{	
	int h1 = this->dim.h - 1;
	int w1 = this->dim.w - 1;

	if (border == ImageUtils::BorderMode::WRAP)
	{
		y = (y < 0) ? h1 : (y > h1) ? 0 : y;
		x = (x < 0) ? w1 : (x > w1) ? 0 : x;
	}
	else if (border == ImageUtils::BorderMode::ENLARGE)
	{
		//reflect
		//[-2] [-1] [0] [1] [2]
		//put [-1] on [0]
		//put [-2] on [1]

		y = (y < 0) ? -y - 1 : (y > h1) ? (2 * this->dim.h - y - 1) : y;
		x = (x < 0) ? -x - 1 : (x > w1) ? (2 * this->dim.w - x - 1) : x;
	}
	else
	{
		//clamp
		y = (y < 0) ? 0 : (y > h1) ? h1 : y;
		x = (x < 0) ? 0 : (x > w1) ? w1 : x;
	}

	return this->GetPixelStart(x, y);
}

/// <summary>
/// Get pointer to the first byte of pixel
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param> 
/// <returns></returns>
template <typename T>
const T * Image2d<T>::GetPixelStart(int x, int y) const
{
	return this->GetPixelStart(this->GetIndexFromPosition(x, y));
}

template <typename T>
T * Image2d<T>::GetPixelStart(int x, int y)
{
	return this->GetPixelStart(this->GetIndexFromPosition(x, y));
}

template <typename T>
const T * Image2d<T>::operator[](size_t index) const
{
	return this->GetPixelStart(index);
}

template <typename T>
T * Image2d<T>::operator[](size_t index)
{
	return this->GetPixelStart(index);
}

template <typename T>
const std::vector<T> & Image2d<T>::GetData() const noexcept
{
	return this->data;
}

template <typename T>
std::vector<T> & Image2d<T>::GetData() noexcept
{
	return this->data;
}

//=================================================================================================
// Methods
//=================================================================================================


template <typename T>
void Image2d<T>::ForEachPixel(std::function<void(T *, size_t)> callback)
{
	size_t len = this->GetPixelsCount();

	for (size_t i = 0; i < len; i++)
	{
		T * tmp = this->GetPixelStart(i);
		callback(tmp, i);
	}	
}

template <typename T>
void Image2d<T>::ForEachPixel(std::function<void(const T *, size_t)> callback) const
{
	size_t len = this->GetPixelsCount();

	for (size_t i = 0; i < len; i++)
	{
		const T * tmp = this->GetPixelStart(i);
		callback(tmp, i);
	}	
}



/// <summary>
/// Calculate absolute value of image
/// Only supported for float data type, since uint8_t is always positive
/// Use SIMD if enabled
/// </summary>
template <typename T>
void Image2d<T>::Abs()
{
	if constexpr (std::is_same<T, uint8_t>::value)
	{
		//not supported for uint8_t since it is already positive
		return;
	}
	else
	{
		size_t len8 = 0;

#ifdef ENABLE_SIMD
		len8 = this->data.size() - this->data.size() % MM256_ELEMENT_COUNT;

		for (size_t i = 0; i < len8; i += MM256_ELEMENT_COUNT)
		{
			auto v = _mm256_loadu_ps(this->data.data() + i);
			v = _my_mm256_abs_ps(v);
			_mm256_storeu_ps(this->data.data() + i, v);
		}

#endif

		for (size_t i = len8; i < this->data.size(); i++)
		{
			this->data[i] = std::abs(this->data[i]);
		}
	}
}

/// <summary>
/// Combine current image and input img
/// for each pixel, callback is called with user defined combination
/// Result can be written to a in callback (a = this)
/// </summary>
/// <param name="img"></param>
/// <param name="callback"></param>
template <typename T>
void Image2d<T>::Combine(const Image2d<T> & img, std::function<void(T *, const T *, size_t size)> callback)
{
	size_t len = this->GetPixelsCount();
	for (size_t i = 0; i < len; i++)
	{
		T * a = this->GetPixelStart(i);
		const T * b = img.GetPixelStart(i);
		callback(a, b, this->GetChannelsCount());
	}
}



/// <summary>
/// Append image to the right of the current image
/// Appended image must have same number of channels as the current one
/// </summary>
/// <param name="img"></param>
template <typename T>
void Image2d<T>::AppendRight(const Image2d<T> & img)
{
	if (this->channelsCount != img.channelsCount)
	{
		MY_LOG_ERROR("Number of channels of append image is not same");
		return;
	}

	ImageDimension newDim;
	newDim.w = dim.w + img.GetWidth();
	newDim.h = std::max(dim.h, img.GetHeight());
		
	size_t len = size_t(newDim.w) * size_t(newDim.h) * this->channelsCount;

	std::vector<T> channelData;
	channelData.resize(len);

	//size_t newIndex = 0;

	//copy old image to the new array
	for (size_t y = 0; y < size_t(dim.h); y++)
	{
		std::copy(this->data.data() + ((0 + y * dim.w) * this->channelsCount),
			this->data.data() + ((dim.w + y * dim.w) * this->channelsCount),
			channelData.data() + ((0 + y * newDim.w) * this->channelsCount));

		/*
		for (size_t x = 0; x < dim.w; x++)
		{
			size_t oldIndex = (x + y * dim.w) * img.channelsCount;
			size_t newIndex = (x + y * newDim.w) * this->channelsCount;			
			for (size_t c = 0; c < this->channelsCount; c++)
			{
				channelData[newIndex + c] = this->data[oldIndex + c];
			}
		}
		*/
	}

	//copy appended image to the new array
	for (size_t y = 0; y < size_t(img.dim.h); y++)
	{
		std::copy(img.data.data() + ((0 + y * img.dim.w) * this->channelsCount),
			img.data.data() + ((img.dim.w + y * img.dim.w) * this->channelsCount),
			channelData.data() + ((dim.w + y * newDim.w) * this->channelsCount));
		
		/*
		for (size_t x = 0; x < img.dim.w; x++)
		{
			size_t oldIndex = (x + y * img.dim.w) * img.channelsCount;
			size_t newIndex = ((dim.w + x) + (y) * newDim.w) * this->channelsCount;			
			for (size_t c = 0; c < img.channelsCount; c++)
			{
				channelData[newIndex + c] = img.data[oldIndex + c];
			}
		}
		*/
	}
	
	this->dim = newDim;
	this->data = std::move(channelData);
}


/// <summary>
/// Insert subimage at position given by top left corner [x, y]
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="img"></param>
template <typename T>
void Image2d<T>::SetSubImage(int x, int y, const Image2d<T>& img)
{
	if (this->GetChannelsCount() == img.GetChannelsCount())
	{
		for (int yy = y; yy < y + img.GetHeight(); yy++)
		{		
			T * lineStart = &this->data[x + yy * this->GetWidth()];
			const T* lineStartSub = &img.data[(yy - y) * img.GetWidth()];

			std::copy(lineStartSub,
				lineStartSub + img.GetWidth() * img.GetChannelsCount(),
				lineStart);
			
		}				
	}
	else
	{
		size_t chanCount = std::min(this->GetChannelsCount(), img.GetChannelsCount());

		for (int yy = y; yy < y + img.GetHeight(); yy++)
		{
			for (int xx = x; xx < x + img.GetWidth(); xx++)
			{
				T* val = this->GetPixelStart(xx, yy);

				const T* valSub = img.GetPixelStart(xx - x, yy - y);

				for (size_t c = 0; c < chanCount; c++)
				{
					val[c] = valSub[c];
				}
			}
		}
	}
}

/// <summary>
/// Helper method to convert gaus filtered data
/// to image data based on image type and channels count
/// </summary>
/// <param name="tmp"></param>
/// <param name="index"></param>
/// <returns></returns>
template <typename T>
void Image2d<T>::SetValue(double tmp, size_t channel, size_t index)
{
	T * val = this->GetPixelStart(index);
	val[channel] = ImageUtils::clamp_cast<T>(tmp);
}

template <typename T>
void Image2d<T>::SetValue(double tmp, size_t channel, int x, int y)
{
	T * val = this->GetPixelStart(x, y);
	val[channel] = ImageUtils::clamp_cast<T>(tmp);
}

template <typename T>
void Image2d<T>::SetValue(T value, size_t channel, size_t index)
{
	T * val = this->GetPixelStart(index);
	val[channel] = value;
}

template <typename T>
void Image2d<T>::SetValue(T value, size_t channel, int x, int y)
{
	T * val = this->GetPixelStart(x, y);
	val[channel] = value;
}

template <typename T>
void Image2d<T>::Clear(T clearValue)
{	
	std::fill(data.begin(), data.end(), clearValue);
}

template <typename T>
void Image2d<T>::SwapChannels(size_t c0, size_t c1)
{
	if ((c0 >= this->channelsCount) || (c1 >= this->channelsCount))
	{
		return;
	}

	size_t len = this->GetPixelsCount();
	
	for (size_t i = 0; i < len; i++)
	{
		T * val = this->GetPixelStart(i);

		T tmp = val[c0];
		val[c0] = val[c1];
		val[c1] = tmp;		
	}
}

template <typename T>
void Image2d<T>::AddChannels(size_t count)
{
	size_t len = this->GetPixelsCount();

	std::vector<T> d;
	d.resize(len * (this->channelsCount + count), 0);

	for (size_t i = 0; i < len; i++)
	{		
		const T * val = this->GetPixelStart(i);

		size_t newIndex = i * (this->channelsCount + count);
		for (size_t c = 0; c < this->channelsCount; c++)
		{
			d[newIndex + c] = val[c];
		}
	}

	this->channelsCount += count;
	this->pf = ColorSpace::PixelFormat::NONE;
}


//==================================================================================
// OpenCV only related section
//==================================================================================

#ifdef HAVE_OPENCV

/// <summary>
/// Get OpenCV Mat representation.
/// Note: 
/// It only holds pointer to the data, so Image2d instance must exist
/// while working with OpenCV Mat
/// </summary>
/// <returns></returns>
template <typename T>
cv::Mat Image2d<T>::CreateOpenCVLightCopy()
{
	int cvFormat = 0;

	if constexpr (std::is_same<T, uint8_t>::value)
	{
		if (this->pf == ColorSpace::PixelFormat::GRAY) cvFormat = CV_8UC1;
		else if (this->pf == ColorSpace::PixelFormat::RGB) cvFormat = CV_8UC3;
		else if (this->pf == ColorSpace::PixelFormat::RGBA) cvFormat = CV_8UC4;
		else
		{
			MY_LOG_ERROR("Incorrect pixel format for uint8_t image");
		}
	}
	else
	{
		//RGB and RGBA image can be float with values in range 0 - 1

		if (this->pf == ColorSpace::PixelFormat::GRAY) cvFormat = CV_32FC1;
		else if (this->pf == ColorSpace::PixelFormat::RGB) cvFormat = CV_32FC3;
		else if (this->pf == ColorSpace::PixelFormat::RGBA) cvFormat = CV_32FC4;
		else if (this->pf == ColorSpace::PixelFormat::XYZ) cvFormat = CV_32FC3;
		else if (this->pf == ColorSpace::PixelFormat::CIE_LUV) cvFormat = CV_32FC3;
		else if (this->pf == ColorSpace::PixelFormat::HSV) cvFormat = CV_32FC3;
		else
		{
			MY_LOG_ERROR("Incorrect pixel format for float image");
		}
	}

	return cv::Mat(static_cast<int>(this->GetHeight()),
		static_cast<int>(this->GetWidth()),
		cvFormat,
		this->data.data());
}

template <typename T>
cv::Mat Image2d<T>::CreateOpenCVDeepCopy()
{
	int cvFormat = 0;

	if constexpr (std::is_same<T, uint8_t>::value)
	{
		if (this->pf == ColorSpace::PixelFormat::GRAY) cvFormat = CV_8UC1;
		else if (this->pf == ColorSpace::PixelFormat::RGB) cvFormat = CV_8UC3;
		else if (this->pf == ColorSpace::PixelFormat::RGBA) cvFormat = CV_8UC4;
		else
		{
			MY_LOG_ERROR("Incorrect pixel format for uint8_t image");
		}
	}
	else
	{
		//RGB and RGBA image can be float with values in range 0 - 1

		if (this->pf == ColorSpace::PixelFormat::GRAY) cvFormat = CV_32FC1;
		else if (this->pf == ColorSpace::PixelFormat::RGB) cvFormat = CV_32FC3;
		else if (this->pf == ColorSpace::PixelFormat::RGBA) cvFormat = CV_32FC4;
		else if (this->pf == ColorSpace::PixelFormat::XYZ) cvFormat = CV_32FC3;
		else if (this->pf == ColorSpace::PixelFormat::CIE_LUV) cvFormat = CV_32FC3;
		else if (this->pf == ColorSpace::PixelFormat::HSV) cvFormat = CV_32FC3;
		else
		{
			MY_LOG_ERROR("Incorrect pixel format for float image");
		}
	}

	cv::Mat m(static_cast<int>(this->GetHeight()),
		static_cast<int>(this->GetWidth()),
		cvFormat);
	memcpy(m.data, this->data.data(), this->data.size() * sizeof(T));
	
	return m;
}

template <typename T>
Image2d<T> & Image2d<T>::operator=(const cv::Mat & cvMat)
{
	this->dim.w = cvMat.cols;
	this->dim.h = cvMat.rows;
	this->channelsCount = cvMat.channels();
	
	data.clear();

	if (cvMat.isContinuous())
	{
		data.assign((T*)cvMat.data, (T*)cvMat.data + cvMat.total() * channelsCount);
	}
	else
	{
		for (int i = 0; i < cvMat.rows; i++)
		{
			data.insert(data.end(), cvMat.ptr<T>(i), cvMat.ptr<T>(i) + cvMat.cols);
		}
	}

	return *this;
}

template <typename T>
void Image2d<T>::RunOpenCV(std::function<void(Image2d<T> * img, cv::Mat &)> cvCallback)
{
	cv::Mat cvMat = this->CreateOpenCVLightCopy();
	cvCallback(this, cvMat);
}

template <typename T>
void Image2d<T>::RunWithResultOpenCV(std::function<cv::Mat(Image2d<T> * img, cv::Mat &)> cvCallback)
{
	cv::Mat cvMat = this->CreateOpenCVLightCopy();
	auto res = cvCallback(this, cvMat);
	*this = res;
}

template <typename T>
void Image2d<T>::ShowImageOpenCV(bool waitForKey)
{

	cv::Mat cvMat = this->CreateOpenCVLightCopy();

	const char* cv_window = "OpenCV image";

	cv::namedWindow(cv_window, cv::WINDOW_NORMAL);
	cv::resizeWindow(cv_window, 
		std::min(this->GetWidth(), 1680), 
		std::min(this->GetHeight(), 1050));
	cv::imshow(cv_window, cvMat);
	if (waitForKey)
	{
		cv::waitKey();
	}
}
#endif


//=================================================================================================

template Image2d<float> Image2d<float>::CreateAs() const;
template Image2d<float> Image2d<uint8_t>::CreateAs() const;
template Image2d<uint8_t> Image2d<float>::CreateAs() const;
template Image2d<uint8_t> Image2d<uint8_t>::CreateAs() const;


template Image2d<float> Image2d<uint8_t>::CreateEmpty() const;
template Image2d<uint8_t> Image2d<uint8_t>::CreateEmpty() const;
template Image2d<float> Image2d<float>::CreateEmpty() const;
template Image2d<uint8_t> Image2d<float>::CreateEmpty() const;

template class Image2d<uint8_t>;
template class Image2d<float>;
